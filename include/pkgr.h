#ifndef _PKGR_H
#define _PKGR_H

#ifndef _cplusplus
    #include <stdbool.h>

    #undef true
    #define true (!0)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <checksum.h>
#include <download.h>
#include <unpack.h>

#ifndef TARGET
    #ifdef __APPLE__
        #include "TargetConditionals.h"
        #if TARGET_OS_IPHONE
            #define TARGET "ios/arm"
        #else
            #define TARGET "mac/x86_64"
        #endif
    #elif __linux
        #if (__WORDSIZE == 64)
            #define TARGET "linux/x86_64"
        #else
            #define TARGET "linux/x86"
        #endif
    #elif __unix
        #if (__WORDSIZE == 64)
            #define TARGET "bsd/x86_64"
        #else
            #define TARGET "bsd/x86"
        #endif
    #elif __posix /* not sure if this is needed */
        #if (__WORDSIZE == 64)
            #define TARGET "posix/x86_64"
        #else
            #define TARGET "posix/x86"
        #endif
    #endif
#endif

#ifndef BOLD
    #define BOLD "\033[1;"
#endif

#ifndef NORMAL
    #define NORMAL "\033[0m"
#endif

#ifndef RED
    #define RED "31m"
#endif

#ifndef YELLOW
    #define YELLOW "33m"
#endif

#ifndef pkgr_error
/**
 * Print out error text in bold red.
 * 
 * @param text the text to print out or the format string to use
 * @param ...  if using a format string, the arguments corresponding to the
 *             format codes
 *
 * @date 2012-07-30
 * @since 2012-07-30
 */
#define pkgr_error(text, ...) fprintf(stderr, bold red "Error: " text "\n" normal, ##__VA_ARGS__)
#endif

#ifndef pkgr_warning
/**
 * Print out warning text in bold yellow.
 * 
 * @param text the text to print out or the format string to use
 * @param ...  if using a format string, the arguments corresponding to the
 *             format codes
 *
 * @date 2012-07-30
 * @since 2012-07-30
 */
#define pkgr_warning(text, ...) fprintf(stderr, bold yellow "Warning: " text "\n" normal, ##__VA_ARGS__)
#endif

/**
 * Container for a package's required info.
 *
 * @see unpack()
 * @see download()
 *
 * @date 2012-07-31
 * @since 2012-07-31
 */
typedef struct pack_data {
    char *name;
    struct pack_data *deps;
    bool pkgbuild_loaded;
} Package;

extern const char **pkgr_repos;
extern char *pkgr_prefix;
extern lua_State *state;

bool pkgr_load_pkgbuild(Package *package);
void pkgr_cleanup();
Package *new_Package();
void pkgr_init();

#endif