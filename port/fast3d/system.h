#ifndef FAST3D_SYSTEM_SHIM_H
#define FAST3D_SYSTEM_SHIM_H

/**
 * The system hooks the fast3d sources call (logging, fatal errors, command-line
 * switches). Implementations in port/src/fast3d_glue.c.
 */

#include <stdint.h>

typedef int32_t s32;
typedef int64_t s64;
typedef uint32_t u32;

#define LOGFLAG_SHOWMSG 0x100

enum logLevel {
    LOG_NOTE = 0,
    LOG_CHAT = 0 | LOGFLAG_SHOWMSG,
    LOG_WARNING = 1 | LOGFLAG_SHOWMSG,
    LOG_ERROR = 2 | LOGFLAG_SHOWMSG,
};

#ifdef __cplusplus
extern "C" {
#endif

s32 sysArgCheck(const char *arg);
const char *sysArgGetString(const char *arg);
void sysFatalError(const char *fmt, ...) __attribute__((noreturn));
void sysLogPrintf(s32 level, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
