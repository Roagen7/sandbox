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
#include <sys/wait.h>


const char* get_last_path_part(const char* path){

}

void handle_ns(sbx_input* input){

    // if(unshare(CLONE_NEWNS) == -1){
    //     perror("unshare");
    // }

    // if(unshare(CLONE_NEWNS | CLONE_FS | CLONE_VM | CLONE_NEWPID) == -1){
    //     perror("unshare");
    //     exit(1);
    // }

    // if(umount("/") == -1){
    //     perror("umount");
    //     exit(1);
    // }

    // if(mount("none", "/", "tmpfs", 0, NULL) == -1){
    //     perror("mount");
    //     exit(1);
    // }
    // if(unshare(CLONE_FS | CLONE_VM) == -1){
    //     perror("unshare");
    //     exit(1);
    // }
    // if(unshare(CLONE_NEWPID) == -1){
    //     perror("unshare");
    //     exit(1);
    // };
}


char* argv[] = {NULL};


int child_function(void* arg){
    if(execvp(argv[0], argv) == -1){
        assert(0);
    }
}

void sbx_run_sandbox(sbx_input* input){
    if(!input) return;

    char child_stack[4096];
    argv[0] = input->path;

    if(unshare(CLONE_NEWUSER | CLONE_NEWPID) == -1){
        perror("unshare");
        exit(1);
    }

    if(clone(child_function, child_stack + sizeof(child_stack), CLONE_VM | SIGCHLD, NULL) == -1){
        perror("clone");
        exit(1);
    }

    if(wait(NULL) == -1){
        perror("wait");
        exit(1);
    }


}

// void sbx_run_sandbox(sbx_input* input){
//     if(!input) return;

//     argv[0] = input->path;

//     char child_stack[4096];

//     // handle_ns(input);
//     printf("hello!\n");
//     unshare(CLONE_NEWPID | CLONE_NEWNS);
//     umount("/");
//     int pid = fork();
//     if(pid != 0){
//         int status;
//         waitpid(-1, &status, 0);
//         return ;
//     }
//     //mount("none", "/proc", NULL, MS_PRIVATE | MS_REC, NULL);
  
//     printf("hello from new pid %d", getpid());
//     child_process(NULL);

//     // if(clone(child_process, child_stack + sizeof(child_stack), CLONE_VM | SIGCHLD, NULL) == -1){
//     //     perror("clone");
//     //     exit(1);
//     // }

//     // if(-1 == wait(NULL)){
//     //     perror("wait");
//     //     exit(1);
//     // }
//     // pid_t pid = fork();
//     // if(pid) {
//     //     wait(NULL);
//     //     return ;
//     // }; 
    

//     // reparent
//     // enter ns's for child
//     // enter cgroup for child
//     // chroot child
//     // exec process
//     // else return
// }