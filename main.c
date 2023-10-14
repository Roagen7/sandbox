/* ============================================================
 * Title: Simple POC of process containerization
 * Author: Dominik Lau
 * ============================================================
 * Description: 
 *  containerizes given process using namespaces and pivot_root
 *  works for pre-made rootfs (so all dependencies are present)
 * ============================================================
 * Usage:
 *  --exec <path_to_process>       [REQUIRED]
 *      executes process provided in path
 *      the process needs to be present in rootfs under that path
 *  --rootfs <path_to_rootfs>      [REQUIRED]
 *      path to premade rootfs (with all dependencies present
 *      for --exec), fetching such image:
 *      docker export $(docker create ubuntu) --output="image.tar"
 * ============================================================
 */

#include <stdio.h>
#include <stdint.h>

#include "cli.h"
#include "sandbox.h"

int main(int argc, char** argv) {
    sbx_input* input = sbx_cli_create_process(argc, argv);
    sbx_run_sandbox(input);
    return 0;
}
