/* ======================================================================
 * Title: Simple POC of process containerization
 * Author: Dominik Lau
 * ======================================================================
 * Description: 
 *  containerizes given process using namespaces and pivot_root
 *  works for pre-made rootfs (so all dependencies are present)
 * ======================================================================
 * Usage:
 *  see below
 * =======================================================================
 */

#include <stdio.h>
#include <stdint.h>

#include "cli.h"
#include "sandbox.h"

void print_help(){
    printf("Usage:"
   "--exec      <path_to_process>       [REQUIRED]                          \n" 
   "    executes process provided in path                                   \n"
   "    the process needs to be present in rootfs under that path           \n"
   "--name      <container_name>        [REQUIRED]                          \n" 
   "    set container name                                                  \n"
   "--rootfs    <path_to_rootfs>        [REQUIRED]                          \n"
   "    path to premade rootfs (with all dependencies present for --exec)   \n"
   "    fetching such image:                                                \n"
   "    docker export $(docker create ubuntu) --output=\"image.tar\"        \n"
   "--cli_par   <params_enumeration>    [OPTIONAL]                          \n"
   "    specify process argv                                                \n"
   "--stack     <size>                  [OPTIONAL]                          \n"
   "    specify process stack size, default 1 MiB)                          \n");
}

int main(int argc, char** argv) {
    sbx_input* input = sbx_cli_create_process(argc, argv);
    if(input == NULL){
        fprintf(stderr, "Invalid arguments...\n");
        print_help();
        return 1;
    }
    return sbx_run_sandbox(input);
}
