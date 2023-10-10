#include <stdio.h>
#include <stdint.h>

#include "cli.h"
#include "sandbox.h"

int main(int argc, char** argv) {
    sbx_input* input = sbx_cli_create_process(argc, argv);
    sbx_run_sandbox(input);
    return 0;
}
