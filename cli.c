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

    return &input;
}