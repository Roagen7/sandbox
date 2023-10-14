#include "sandbox.h"

#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>
#undef __USE_GNU
#undef _GNU_SOURCE

#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int child_function(void* arg){
    sbx_input* input = (sbx_input*) arg;

    // for debug, check if it truly is in new PID namespace
    // should be PID=1
    printf("%d\n", getpid());

    if(unshare(CLONE_NEWNS) == -1){
        perror("unshare(CLONE_NEWNS)");
        return 1;
    }

    // initialize procfs
    if(mount("proc", "/proc", "proc", 0, NULL) == -1){
        perror("mount -t proc proc /proc");
        return 1;
    }

    // we need to mount rbind it to self due to some API restraints
    // mount --rbind rootfs rootfs
    if(mount(input->rootfs, input->rootfs, NULL, MS_BIND | MS_REC, NULL) == -1){
        perror("mount --rbind rootfs rootfs");
        return 1;
    }
    chdir(input->rootfs);
    mkdir("oldroot", 0777);
    if(pivot_root(".", "oldroot") == -1){
        perror("pivot_root");
        return 1;
    };

    // remount procfs because it gets unmounted >:(
    // otherwise unmounting oldroot doesn't work
    if(mount("proc", "/proc", "proc", 0, NULL) == -1){
        perror("mount -t proc proc /proc");
        return 1;
    }

    // umount -l oldroot
    if(umount2("oldroot", MNT_DETACH) == -1){
        perror("umount -l oldroot");
        return 1;
    }

    char* argv[256] = {NULL};
    argv[0] = input->exec;

    if(execvp(argv[0], argv) == -1){
        perror("execvp");
        return 1;
    }
}

void sbx_run_sandbox(sbx_input* input){
    if(!input) return;

    char child_stack[1024 * 1024];

    if (unshare(CLONE_NEWUSER) == -1) {
        perror("unshare(CLONE_NEWPID) failed");
        return;
    }

    // current user must be mapped to root (otherwise we can't use mount)
    // so map UID 0 (root) to UID 1000 in the NEW user namespace
    int fd = open("/proc/self/uid_map", O_WRONLY);
    if(!fd){
        perror("opening uid_map fail");
        return;
    }
    if(write(fd, "0 1000 1", 8) == -1){
        perror("write to uid_map fail");
        close(fd);
        return;
    }    
    close(fd);

    // unsharing PID namespace works for the children of the process so we need to fork
    pid_t child_pid = clone(child_function, child_stack + sizeof(child_stack), SIGCHLD | CLONE_NEWPID, input);
    if(child_pid == -1){
        perror("clone");
    }

    int status;
    if(waitpid(child_pid, &status, 0) == -1){
        perror("waitpid");
    }
    printf("container exit %d", status);
}
