#include "sandbox.h"

#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>
#include <unistd.h>
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
#include <errno.h>

#include "utils.h"

void create_default_devices(){

}

int pivot_root(sbx_input* input){
    // we need to mount rbind it to self due to some API restraints
    SBX_SAFE_CALL(mount(input->rootfs, input->rootfs, NULL, MS_BIND | MS_REC, NULL), "mount --rbind rootfs rootfs");

    chdir(input->rootfs);
    mkdir("oldroot", 0777);
    
    SBX_SAFE_CALL(syscall(SYS_pivot_root, ".", "oldroot"), "pivot_root");
    return 0;
}

int mount_pseudofs(){
    // setup procfs because it gets unmounted
    // otherwise unmounting oldroot doesn't work >:(
    SBX_SAFE_CALL(mount("proc", "/proc", "proc", 0, NULL), "mount -t proc proc /proc");
    SBX_SAFE_CALL(mount("devtmpfs", "/dev", "devtmpfs", 0, NULL), "mount -t devtmpfs devtmpfs /dev");
}

int set_envs(){
    /*
     * guarantee proper shell work
     */
    setenv("PATH", "/bin:/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin", 1);
    unsetenv("LC_ALL");
    return 0;
}

/*
 * setup overlayfs so that container acts like live cd
 */
// int setup_overlayfs(){
//     char *ovfs_opts;
//     char *upper;
//     char *work;
//     char *merged;

//     asprintf(&upper, "%s/upper", c->rootfs);
//     asprintf(&work, "%s/work", c->rootfs);
//     asprintf(&merged, "%s/merged", c->rootfs);
//     if (mkdir(c->path, 0700) == -1 && errno != EEXIST) return 1;
//     if (mkdir(upper, 0700) == -1 && errno != EEXIST) die("container upper dir");
//     if (mkdir(work, 0700) == -1 && errno != EEXIST) die("container work dir");
//     if (mkdir(merged, 0700) == -1 && errno != EEXIST) die("container merged dir");
//     asprintf(&ovfs_opts, "lowerdir=%s/images/%s,upperdir=%s,workdir=%s",cwd, c->image, upper, work);
//     free(ovfs_opts);
//     free(upper);
//     free(work);
//     free(merged);
// }

int child_function(void* arg){
    sbx_input* input = (sbx_input*) arg;
    // for debug, check if it truly is in new PID namespace
    // should be PID=1
    printf("[INFO] pid = %int\n", getpid());

    /* remount / as private, on some systems / is shared */
    SBX_SAFE_CALL(mount("/", "/", "none", MS_PRIVATE | MS_REC, NULL), "mount / private");
    SBX_SAFE_CALL(pivot_root(input), "pivot_root()");
    SBX_SAFE_CALL(mount_pseudofs(), "mount pseudofs");

    // umount -l oldroot
    SBX_SAFE_CALL(umount2("oldroot", MNT_DETACH), "umount -l oldroot");
    
    char* argv[256] = {NULL};
    argv[0] = input->exec;

    SBX_SAFE_CALL(set_envs(), "envs");
    SBX_SAFE_CALL(sethostname(input->name,strlen(input->name)), "hostname");
    SBX_SAFE_CALL(execvp(argv[0], argv), "execvp");
    
    
}

int sbx_run_sandbox(sbx_input* input){
    if(!input) return 1;

    char* child_stack = (char*) malloc(input->stack);    
    if(!child_stack){
        perror("malloc");
        return 1;
    }
    #define GUARDED child_stack

    SBX_SAFE_CALL_DEALLOC(unshare(CLONE_NEWUSER), "unshare --pid");


    // current user must be mapped to root (otherwise we can't use mount)
    // so map UID 0 (root) to UID 1000 in the NEW user namespace
    int fd = open("/proc/self/uid_map", O_WRONLY);
    if(!fd){
        perror("opening uid_map fail");
        free(child_stack);
        return 1;
    }
    //mount -t overlay overlay -o lowerdir=/lower1:/lower2,upperdir=/upper,workdir=/work /merged

    SBX_SAFE_CALL_DEALLOC(write(fd, "0 1000 1", 8), "map uuid");  
    close(fd);

    // unsharing PID namespace works for the children of the process so we need to fork
    pid_t child_pid = clone(child_function, child_stack + input->stack, SIGCHLD | CLONE_NEWPID | CLONE_NEWUTS  | CLONE_NEWNS, input);
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
    return 0;
    #undef GUARDED
}
