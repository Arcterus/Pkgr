/**
 * A package management program named "pkgr".
 * 
 * @file pkgr_download.c
 *
 * @author Arcterus
 *
 * @version 0.1
 * @date 2012-08-05
 * @since 2012-07-29
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <curl/curl.h>
#include <archive.h>
#include <archive_entry.h>
#include <libconfig.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <openssl/evp.h>

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

#define bold "\033[1;"
#define normal "\033[0m"
#define red "31m"
#define yellow "33m"

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
#define error(text, ...) fprintf(stderr, bold red "Error: " text "\n" normal, ##__VA_ARGS__)

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
#define warning(text, ...) fprintf(stderr, bold yellow "Warning: " text "\n" normal, ##__VA_ARGS__)

#ifndef __cplusplus
/**
 * Boolean type to be used in the event the code is being compiled with a
 * normal C compiler.
 *
 * @date 2012-08-01
 * @since 2012-08-01
 */
typedef enum {
    false = 0,  /**< false value is zero */
    true  /**< true value is one */
} bool;
#endif

/**
 * Container for the data used by the downloader's progress bar.
 *
 * @see printProgressBar()
 *
 * @date 2012-07-29
 * @since 2012-07-29
 */
typedef struct prog_data {
    CURL *curl;
    struct ttysize termsize;
    int columns;
} ProgressData;

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

/**
 * Array of repositories available for use.
 *
 * @see download()
 *
 * @date 2012-08-01
 * @since 2012-08-01
 */
const char **repos;

/**
 * Prefix to the location in which files will be installed.
 *
 * @see install()
 * @see uninstall()
 *
 * @date 2012-08-01
 * @since 2012-08-01
 */
char *prefix;

static lua_State *state;

/**
 * Progress bar that can be used for downloads.
 *
 * @param ptr     pointer to the data used
 * @param dltotal the total amount of data to be downloaded
 * @param dlnow   the amount of data that has been downloaded
 * @param ultotal the total amount if data to be uploaded
 * @param ulnow   the amount of data that has been uploaded
 *
 * @return the value of the transfer (whether it was successful or not)
 *
 * @see download_file()
 * @see ProgressData
 *
 * @date 2012-07-31
 * @since 2012-07-29
 */
static int printProgressBar(void *ptr, double dltotal, double dlnow, double ultotal, double ulnow) {
    ProgressData *progress = (ProgressData *) ptr;
    CURL *curl = progress->curl;
    ioctl(0, TIOCGWINSZ, &progress->termsize);
    //if(progress->termsize.ts_cols == progress->columns) {
        double percent = dlnow / dltotal;
        int i, progbarSize = progress->termsize.ts_cols - 9, progbarPercent = (int) (percent * progbarSize);
        printf("\r[");
        for(i = 0; i < progbarPercent; i++) {
            putchar('#');
        }
        for(i = 0; i < progbarSize - progbarPercent; i++) {
            putchar(' ');
        }
        printf("] %.1f%%", percent * 100);
        fflush(stdout);
    /*} else {
        progress->columns = progress->termsize.ts_cols;
        printf("\r\b");
    }*/
    return 0;
}

static int copy_data(struct archive *ar, struct archive *aw);

/**
 * Extract a compressed tarball to the current directory.
 *
 * @param filename the tarball
 *
 * @return the result of the extraction (success or failure)
 *
 * @see copy_data()
 * @see unpack()
 *
 * @date 2012-08-01
 * @since 2012-07-31
 */
static int extract(const char *filename) {
    int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS;
    struct archive *a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_compression_all(a);
    struct archive *ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);
    int r;
    if((r = archive_read_open_file(a, filename, 10240))) {
        error("Oh no!");
        return 1;
    }
    struct archive_entry *entry;
    while(1) {
        if((r = archive_read_next_header(a, &entry)) == ARCHIVE_EOF) {
            break;
        } else if(r != ARCHIVE_OK)
            error("%s", archive_error_string(a));
        }
        if(r < ARCHIVE_WARN) {
            return 1;
        } else if((r = archive_write_header(ext, entry)) != ARCHIVE_OK) {
            error("%s", archive_error_string(ext));
        } else if(archive_entry_size(entry) > 0) {
            copy_data(a, ext);
            if(r != ARCHIVE_OK) {
                error("%s", archive_error_string(ext));
            }
            if(r < ARCHIVE_WARN) {
                error("%s", archive_error_string(a));
                return 1;
            }
        }
        if((r = archive_write_finish_entry(ext)) != ARCHIVE_OK) {
            error("%s", archive_error_string(ext));
        if(r < ARCHIVE_WARN) {
            error("%s", archive_error_string(a));
            return 1;
        }
    }
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
    return 0;
}

/**
 * Unpacks a .kpkg file.
 *
 * @param package the package whose kpkg is to be unpacked
 * @param cwd     whether or not the kpkg is in the current directory
 *
 * @return the result of the extraction (success or failure)
 *
 * @see copy_data()
 * @see extract()
 *
 * @date 2012-07-31
 * @since 2012-07-31
 */
int unpack(Package *package, bool cwd) {
    char filename[1024];
    // .kpkg files can be any compression format, but must be the tar archiving format
    snprintf(filename, sizeof(filename), "%s.kpkg", package->name);
    if(!cwd) {
        chdir("/var/cache/pkgr/");
    }
    return extract(filename);
}

/**
 * Copy data from one archive to another.
 *
 * @param ar the readable archive
 * @param aw the writeable archive
 *
 * @return the result of the copy (success or failure)
 *
 * @see extract()
 * @see unpack()
 *
 * @date 2012-08-01
 * @since 2012-07-31
 */
static int copy_data(struct archive *ar, struct archive *aw) {
    int r;
    const void *buff;
    size_t size;
    off_t offset;

    while(1) {
        if((archive_read_data_block(ar, &buff, &size, &offset)) == ARCHIVE_EOF) {
            return ARCHIVE_OK;
        } else if(r != ARCHIVE_OK) {
            error("%s", archive_error_string(aw));
            return r;
        }
        if((archive_write_data_block(aw, buff, size, offset)) != ARCHIVE_OK) {
            error("%s", archive_error_string(aw));
            return r;
        }
    }
}

/**
 * Download a file with an optional progress bar.
 *
 * @param url     URL of the file to be downloaded
 * @param outfile the location where the downloaded file will go
 * @param progbar whether or not a progress bar is to be displayed
 *
 * @return the result of the download (success or failure)
 *
 * @date 2012-07-30
 * @since 2012-07-29
 */
static int download_file(const char *url, const char *outfile, bool progbar) {
    ProgressData prog;
    CURLcode res = 0;
    CURL *curl = curl_easy_init();
    if(curl) {
        FILE *output = fopen(outfile, "w");
        if(output == NULL) {
            error("Could not create file (%s)", outfile);
            curl_easy_cleanup(curl);
            return 1;
        }
        prog.curl = curl;
        ioctl(0, TIOCGWINSZ, &prog.termsize);
        prog.columns = prog.termsize.ts_cols;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, output);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        if(progbar) {
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, printProgressBar);
            curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        }
        res = curl_easy_perform(curl);
        fclose(output);
        puts("");
        if(res) {
            fputs(curl_easy_strerror(res), stderr);
        }
        curl_easy_cleanup(curl);
    }
    return (int) res;
}

lua_State *create_lua_state() {
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);
    return lua;
}

bool load_pkgbuild(Package *package) {
    if(package->pkgbuild_loaded) {
        return true;
    } else {
        char filename[1048];
        snprintf(filename, sizeof(filename), "/var/lib/pkgr/%s-build.lua", package->name);
        if(luaL_loadfile(state, filename) || lua_pcall(state, 0, 0, 0)) {
            error("%s does not exist", filename);
            return false;
        } else {
            package->pkgbuild_loaded = true;
            return true;
        }
    }
}

char *generate_checksum(char *filename, char *hexdigest) {
    OpenSSL_add_all_digests();
    const EVP_MD *md = EVP_get_digestbyname("rmd160");
    if(!md) {
        error("Ripemd-160 is not supported by this version of OpenSSL");
        exit(1);
    }
    EVP_MD_CTX *ctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(ctx, md, NULL);
    FILE *dfile = fopen(filename, "rb");
    while(!feof(dfile)) {
        char data[1024];
        size_t size = fread(data, sizeof(char), sizeof(data), dfile);
        EVP_DigestUpdate(ctx, data, size);
    }
    unsigned int md_len;
    unsigned char checksum[EVP_MAX_MD_SIZE];
    EVP_DigestFinal_ex(ctx, checksum, &md_len);
    EVP_MD_CTX_destroy(ctx);
    unsigned int i;
    for(i = 0; i < md_len; i++) {
        char buffer[3];
        snprintf(buffer, sizeof(buffer), "%02x", checksum[i]);
        strcat(hexdigest, buffer);
    }
    return hexdigest;
}

bool test_checksum(Package *package) {
    load_pkgbuild(package);
    char filename[1048];
    snprintf(filename, sizeof(filename), "/var/lib/pkgr/%s-build.lua", package->name);
    lua_getglobal(state, "checksum");  // ripemd-160 checksum
    if(!lua_isstring(state, -1)) {
        error(normal "checksum" bold red " should be a string in %s", filename);
        return false;
    }
    char checksum[69];
    snprintf(filename, sizeof(filename), "/var/cache/pkgr/%s.kpkg", package->name);
    if(!strcmp(generate_checksum(filename, checksum), (char *) lua_tostring(state, -1))) {
        return true;
    } else {
        return false;
    }
}

bool download(Package *package) {
    char filename[1048];
    snprintf(filename, sizeof(filename), "/var/lib/pkgr/%s-build.lua", package->name);
    load_pkgbuild(package);
    lua_getglobal(state, "url");
    if(!lua_isstring(state, -2)) {
        error(normal "url" bold red " should be a string in %s", filename);
        return false;
    }
    snprintf(filename, sizeof(filename), "/var/cache/pkgr/%s.kpkg", package->name);
    if(download_file((char *) lua_tostring(state, -2), filename, true)) {
        return false;
    } else {
        return true;
    }
}

bool install(Package *package) {
    char filename[1048];
    snprintf(filename, sizeof(filename), "/var/lib/pkgr/%s-build.lua", package->name);
    load_pkgbuild(package);
    lua_getglobal(state, "install");
    if(lua_pcall(state, 0, 0, 0) != 0) {
        error("Could not run " normal "build" bold red "in %s", filename);
        return false;
    } else {
        return true;
    }
}

bool uninstall(Package *package) {
    
}

/**
 * Load a configuration file.
 *
 * @param file the configuration file to be loaded
 *
 * @see repos
 *
 * @date 2012-08-04
 * @since 2012-08-01
 */
static void load_config(const char *file) {
    config_t config;
    config_setting_t *settings;
    config_init(&config);
    if(!config_read_file(&config, file)) {
        repos = malloc(32);
        *repos = "http://pkgr.org/packages/" TARGET "/";
    } else {
        settings = config_root_setting(&config);
        config_setting_t *reposettings = config_setting_get_member(settings, "repos");
        if(config_setting_is_array(reposettings)) {
            const char *repostr;
            int i, reposize = 0, repolength = 0;
            for(i = 0; (repostr = config_setting_get_string_elem(reposettings, i)) != NULL; i++) {
                if(strlen(repostr) + repolength > reposize) {
                    reposize += 1024;
                    repos = realloc(repos, reposize);
                }
                repos[i] = repostr;
                repolength += strlen(repostr);
            }
        } else {
            repos = malloc(32);
            *repos = "http://pkgr.org/packages/" TARGET "/";
        }
    }
    config_destroy(&config);
}

/**
 * Frees memory allocated throughout the program.
 *
 * @see repos
 *
 * @date 2012-08-01
 * @since 2012-08-01
 */
void cleanup() {
    free(repos);
    lua_close(state);
}

Package *new_Package() {
    Package *package = malloc(sizeof(Package));
    package->name = malloc(1024);
    return package;
}

/**
 * The entry point for the program.
 *
 * @param argc the amount of command-line arguments passed to the program
 * @param argv the command-line arguments passed to the program
 *
 * @return the result of the program's execution
 *
 * @date 2012-08-05
 * @since 2012-07-29
 */
int main(const int argc, char *argv[]) {
    if(argc < 3) {
        error("Too few arguments");
        return 1;
    }
    gid_t gids[100];
    int gcount;
    if((gcount = getgroups(sizeof(gids) / sizeof(gids[0]), gids)) == -1) {
        error("Could not determine your group id");
        return 1;
    } else {
        int i, admin;
        for(i = 0; i < gcount; i++) {
            if(gids[i] ==
               #ifdef __APPLE__
                   80  // "admin" group
               #else
                   0  // "wheel" group, since Unix is the assumed platform
               #endif
            ) {
                admin = true;
                break;
            }
        }
        if(admin) {
            prefix = "/usr/local/";
        } else {
            if((prefix = getenv("HOME")) == NULL) {
                prefix = getpwuid(getuid())->pw_dir;
            }
        }
    }
    load_config("/etc/pkgr.conf");  // I may just use Lua for this.  Less libraries that way...
    state = create_lua_state();
    if(!strcmp(argv[1], "download")) {
        download_file(argv[2], argv[3], true);
    } else if(!strcmp(argv[1], "unpack")) {
        Package *pack = new_Package();
        if(strstr(argv[2], ".kpkg") == NULL) 
            pack->name = argv[2];
            unpack(&pack, true);
        } else {
            strncpy(pack->name, argv[2], strlen(argv[2]) - 5);
            unpack(&pack, false);
        }
    } else if(!strcmp(argv[1], "checksum")) {
        char checksum[69];
        puts(generate_checksum(argv[2], checksum));
    } else {
        error("Unknown command (%s)", argv[1]);
        cleanup();
        return 1;
    }
    cleanup();
    return 0;
}
