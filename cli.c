#include "cli.h"

#include <string.h>

char* get_param_value(const char* name, int argc, char** argv){

    for(int i = 0; i < argc; i++){
        if(!strcmp(argv[i],name) && i + 1 < argc){
            return argv[i+1];
        }
    }

    return NULL;
}

// false by default
int get_param_bool(const char* name, int argc, char** argv){
    for(int i = 0; i < argc; i++){
        if(!strcmp(argv[i],name)){
            return 1;
        }
    }
    return 0;
}

sbx_input* sbx_cli_create_process(int argc, char** argv){
    static sbx_input input;
    input.exec = get_param_value("--exec", argc, argv);
    if(input.exec == NULL){
        return NULL;
    }
    input.rootfs = get_param_value("--rootfs", argc, argv);
    if(input.rootfs == NULL){
        return NULL;
    }
    input.name = get_param_value("--name", argc, argv);
    if(input.name == NULL){
        return NULL;
    }

    input.container_dir = get_param_value("--container_dir", argc, argv);
    if(input.container_dir == NULL){
        return NULL;
    }

    const char* stack = get_param_value("--stack", argc, argv);
    if(stack){
        input.stack = atoi(stack);
        if(input.stack == 0) return NULL;        
    } else {
        input.stack = 1024 * 1024; // defaults to 1MiB
    }

    input.privileged = get_param_bool("--privileged", argc, argv);
    input.seccomp = get_param_bool("--seccomp", argc, argv);

    return &input;
}