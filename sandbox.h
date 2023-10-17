#ifndef SBX_SANDBOX
#define SBX_SANDBOX

#include "input.h"

/*
 * runs process in sandbox
 * @params
 * input - filled input with sandbox config
 * @return 1 on failure, 0 on success
 */
int sbx_run_sandbox(sbx_input* input);
#endif