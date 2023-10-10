#include "sandbox.h"

#include <unistd.h>
#include <assert.h>


const char* get_last_path_part(const char* path){

}

void enter_ns(sbx_input* input){

}

void sbx_run_sandbox(sbx_input* input){
    if(!input) return;


    char* argv[] = { input->path, NULL };
    char* envp[] = {NULL } ;
    if(execve(argv[0], argv, envp) == -1){
        assert(0);
    }
    // reparent
    // enter ns's for child
    // enter cgroup for child
    // chroot child
    // exec process
    // else return
}