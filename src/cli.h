#ifndef SBX_CLI
#define SBX_CLI

#include "input.h"

/*
 * create sbx_input for a process
 * based on input from command-line
 * @params
 * cli_argv
 * cli_argc
 * @return static address of created input
 */
sbx_input* sbx_cli_create_process(int argc, char** argv);
#endif