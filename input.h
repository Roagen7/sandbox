#ifndef SBX_INPUT
#define SBX_INPUT
#include <stddef.h>

/*
 * represents input process
 * @params
 * process - process name
 * namespaces - enum, which namespaces to unshare
 * root_dir - directory to chroot to
 */
typedef struct {
    char* exec;     // [REQUIRED]
    char* name;     // [REQUIRED]
    char* rootfs;   // [REQUIRED]
    char* cli_par;  // [OPTIONAL]
    size_t stack;   // [OPTIONAL]
} sbx_input;

/*
 * list of available namespaces to unshare
 */
enum sbx_NAMESPACE {
    STH = 1<<8,
};
#endif