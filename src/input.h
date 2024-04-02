#ifndef SBX_INPUT
#define SBX_INPUT
#include <stddef.h>

/*
 * represents input process
 * @params
 * exec - path to binary to run in container
 * name - name of a container (affects hostname of the process)
 * rootfs - image directory
 * container_dir - path to directory where container overlayfs should be created
 * stack - stack size, default 1MiB
 * seccomp - enable bpf seccomp
 * privileged - run in privileged mode
 */
typedef struct {
    char* exec;             // [REQUIRED]
    char* name;             // [REQUIRED]
    char* rootfs;           // [REQUIRED]
    char* container_dir;    // [REQUIRED]
    char* cli_par;          // [IGNORED ]
    size_t stack;           // [OPTIONAL]
    int seccomp;            // [OPTIONAL]
    int privileged;         // [OPTIONAL]
} sbx_input;

#endif