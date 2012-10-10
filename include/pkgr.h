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

#ifndef OS
    #ifdef __APPLE__
        #define OS "darwin"
    #elif __linux
        #define OS "linux"
    #elif __unix
        #define OS "bsd"
    #else
        #error Unknown operating system
    #endif
#endif

#ifndef ARCH
    #ifdef __arm__ || __thumb__ || _ARM
        #define ARCH "arm"
    #elif __amd64__ || __amd64 || __x86_64__ || __x86_64 || _M_X64 || _M_AMD64
        #define ARCH "x86_64"
    #elif 
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
#define pkgr_error(text, ...) fprintf(stderr, BOLD RED "Error: " text "\n" NORMAL, ##__VA_ARGS__)
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
#define pkgr_warning(text, ...) fprintf(stderr, BOLD YELLOW "Warning: " text "\n" NORMAL, ##__VA_ARGS__)
#endif

#define PKGR_SOURCE 0
#define PKGR_BINARY 1
#define PKGR_DATA ".kdata"
#define PKGR_PACKAGE ".kpkg"

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
    const char *name;
    struct pack_data **deps;
    bool pkgbuild_loaded;
    int type;
} Package;

typedef struct repo_data {
    const char *url;
    bool safe;
} Repo;

extern Repo **pkgr_repos;
extern int pkgr_repo_length;
extern char *pkgr_prefix;
extern lua_State *state;

bool pkgr_load_pkgbuild(Package *package);
void pkgr_cleanup();
Package *new_Package();
Repo *new_Repo();
void pkgr_init();

#include <pkgr/checksum.h>
#include <pkgr/download.h>
#include <pkgr/unpack.h>

#endif
