#include "cli.h"

sbx_input* sbx_cli_create_process(int argc, char** argv){
    static sbx_input input;
    /*
     * fill cli.h params
     */
    input.path = argv[1];
    return &input;
}