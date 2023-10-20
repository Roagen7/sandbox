#include "sandbox.h"

#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>
#include <unistd.h>
#include <sys/prctl.h>
#undef __USE_GNU
#undef _GNU_SOURCE

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>
#include <linux/capability.h>

#include "utils.h"
#include "setup.h"

void setup_mounts();
void pivot_root(char* new_root);
void setup_envs(sbx_input* input);
char* setup_overlayfs(sbx_input* input);
int boot_container(void* arg);
void setup_dns(char* new_root);
void drop_caps();


int sbx_run_sandbox(sbx_input* input){
    if(!input) return 1;

    char* child_stack = (char*) malloc(input->stack);    
    if(!child_stack){
        perror("malloc");
        return 1;
    }

    // unsharing PID namespace works for the children of the process so we need to fork
    pid_t child_pid = clone(boot_container, child_stack + input->stack, SIGCHLD | CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS, input);
    if(child_pid == -1){
        perror("clone");
        free(child_stack);
        return 1;
    }

    int status;
    if(waitpid(child_pid, &status, 0) == -1){
        perror("waitpid");
    }
    printf("container exit %d", status);
    free(child_stack);

    // TODO: cleanup overlayfs
    return 0;
}

/*
 * sets up container environment 
 * executes process provided in input->exec
 */
int boot_container(void* arg){
    sbx_input* input = (sbx_input*) arg;
    // for debug, check if it truly is in new PID namespace
    // should be PID=1
    printf("[INFO] pid = %int\n", getpid());

    /* remount / as private, on some systems / is shared */
    if(mount("/", "/", "none", MS_PRIVATE | MS_REC, NULL) == -1){
        perror("/ private");
        exit(1);
    };
    
    char* new_root = setup_overlayfs(input);
    setup_dns(new_root);
    drop_caps();
    pivot_root(new_root);
    setup_mounts();
    setup_envs(input);
	
    char* argv[256] = {NULL};
    argv[0] = input->exec;
    if(execvp(argv[0], argv) == -1){
        perror("exec");
        exit(1);
    }    
}

/*
 * mount /etc/resolv.conf, /etc/hostname and /etc/hosts
 * inside container rootfs
 */
void setup_dns(char* new_root){
    char* resolv_conf, *hostname, *hosts;
    asprintf(&resolv_conf, "%s/etc/resolv.conf", new_root);
    asprintf(&hostname, "%s/etc/hostname", new_root);
    asprintf(&hosts, "%s/etc/hosts", new_root);


    if(mount("/etc/resolv.conf", resolv_conf, "none", MS_BIND, "") == -1){
        perror("mount resolv.conf");
        exit(1);
    }
    if(mount("/etc/hostname", hostname, "none", MS_BIND, "") == -1){
        perror("mount resolv.conf");
        exit(1);
    }
    if(mount("/etc/hosts", hosts, "none", MS_BIND, "") == -1){
        perror("mount resolv.conf");
        exit(1);
    }

    free(resolv_conf);
    free(hostname);
    free(hosts);
}

/*
 * drop all the unneeded capabilities to prevent breakout
 * IN THE FUTURE: it should be taken from config.json
 */
#define MAX_CAP 40
void drop_caps(){

    int cap_keep[MAX_CAP] = { 0 };

    // FUTURE: these should be set from config.json
    cap_keep[CAP_AUDIT_WRITE] = 1;
    cap_keep[CAP_KILL] = 1;
    cap_keep[CAP_NET_BIND_SERVICE] = 1;

    // always keep these
    cap_keep[CAP_CHOWN] = 1;
    cap_keep[CAP_DAC_OVERRIDE] = 1;
    cap_keep[CAP_FOWNER] = 1;
    cap_keep[CAP_FSETID] = 1;
    cap_keep[CAP_SETGID] = 1;
    cap_keep[CAP_SETUID] = 1;
    cap_keep[CAP_SETPCAP] = 1;
    cap_keep[CAP_SYS_CHROOT] = 1;
    cap_keep[CAP_MKNOD] = 1;
    cap_keep[CAP_SETFCAP] = 1;


    for(int i = 0; i <= MAX_CAP; i++){
        if(cap_keep[i]) continue;

        if(prctl(PR_CAPBSET_DROP, i) == -1){
            perror("prctl CAP_SYS_ADMIN");
            fprintf(stderr, "[capnum]: %d", i);
            exit(1);
        }
    }
}
#undef MAX_CAP

/*
 * wrapper to system call pivot_root
 * essentially:
 *  mount --rbind overlay_upper overlay_upper
 *  mkdir oldroot
 *  pivot_root . oldroot
 */
void pivot_root(char* new_root){
    // we need to mount rbind it to self due to some API restraints
    if(mount(new_root, new_root, NULL, MS_BIND | MS_REC, NULL) == -1){
        perror("mount --rbind new_root new_root");
        exit(1);
    }

    chdir(new_root);
    if(mkdir("oldroot", 0777) == -1 && errno != EEXIST){
        perror("mkdir oldroot");
        exit(1);
    }
    
    if(syscall(SYS_pivot_root, ".", "oldroot") == -1){
        perror("pivot_root . oldroot");
        exit(1);
    }
}

/*
 * sets up all linux pseudofs
 * finishes up jailing (unmounts oldroot)
 * IN THE FUTURE: it should be taken from config.json (aside from unmounting oldroot)
 */
void setup_mounts(){
    // setup procfs because it gets unmounted
    // otherwise unmounting oldroot doesn't work >:(
    if(mount("proc", "/proc", "proc", 0, NULL) == -1){
        perror("mount -t proc proc /proc");
        exit(1);
    }
    if(mount("devtmpfs", "/dev", "devtmpfs", 0, NULL) == -1){
        perror("mount -t devtmpfs devtmpfs /dev");
        exit(1);
    }
    if(mount("sysfs", "/sys", "sysfs", 0, NULL) == -1){
        perror("mount -t sysfs sysfs /sys");
        exit(1);
    }
    
    // final step to jail the process - remove the access to root
    if(umount2("oldroot", MNT_DETACH) == -1){
        perror("umount -l oldroot");
        exit(1);
    }
}

/*
 * environment variables clean-up
 * hostname set to container name
 * IN THE FUTURE: it should be taken from config.json
 */
void setup_envs(sbx_input* input){
    clearenv();
    setenv("PATH", "/bin:/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin", 1);
    setenv("TERM", "xterm", 1);
    setenv("HOME", "/root", 1);
    setenv("HOSTNAME", input->name, 1);
    setenv("PWD", "/", 1);
    sethostname(input->name,strlen(input->name));
}

/*
 * creates overlayfs, where:
 *  lower -> image folder
 *  upper -> container_dir/upper
 *  work  -> container_dir/work 
 * 
 * which is mounted into container_dir
 */
char* setup_overlayfs(sbx_input* input){
    char* overlay;
    char* upper;
    char* work;
    char* merged;

    asprintf(&upper, "%s/upper", input->container_dir);
    asprintf(&work, "%s/work", input->container_dir);
    asprintf(&merged, "%s/merged", input->container_dir);

    asprintf(&overlay, "lowerdir=%s,upperdir=%s,workdir=%s", 
        input->rootfs, 
        upper,
        work);
    mkdir(upper, 0777);
    mkdir(work, 0777);
    mkdir(merged, 0777);    
    
    if(mount("", merged, "overlay", MS_RELATIME, overlay) == -1){
        perror("mount overlayfs");
        exit(1);
    }

    free(overlay);
    free(upper);
    free(work);
    return merged;
}
