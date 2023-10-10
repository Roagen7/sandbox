#ifndef SBX_INPUT
#define SBX_INPUT
#include <stdint.h>

/*
 * represents input process
 * @params
 * process - process name
 * namespaces - enum, which namespaces to unshare
 * root_dir - directory to chroot to
 */
typedef struct {
    char* path; // required
    char* root_dir; // optional
    uint8_t namespaces;
} sbx_input;

/*
 * list of available namespaces to unshare
 */
enum sbx_NAMESPACE {
    STH = 1<<8,
};
#endif