#include <pkgr.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>

/**
 * Array of repositories available for use.
 *
 * @see download()
 *
 * @date 2012-08-01
 * @since 2012-08-01
 */
const char **pkgr_repos;

/**
 * Prefix to the location in which files will be installed.
 *
 * @see install()
 * @see uninstall()
 *
 * @date 2012-08-01
 * @since 2012-08-01
 */
char *pkgr_prefix;

lua_State *state;

lua_State *create_lua_state() {
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);
    return lua;
}

bool pkgr_load_pkgbuild(Package *package) {
    if(package->pkgbuild_loaded) {
        return true;
    } else {
        char filename[1048];
        snprintf(filename, sizeof(filename), "/var/lib/pkgr/%s-build.lua", package->name);
        if(luaL_loadfile(state, filename) || lua_pcall(state, 0, 0, 0)) {
            pkgr_error("%s does not exist", filename);
            return false;
        } else {
            package->pkgbuild_loaded = true;
            return true;
        }
    }
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
    free(pkgr_repos);
    lua_close(state);
}

Package *new_Package() {
    Package *package = malloc(sizeof(Package));
    package->name = malloc(1024);
    return package;
}

void init() {
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
            pkgr_prefix = "/usr/local/";
        } else {
            if((pkgr_prefix = getenv("HOME")) == NULL) {
                pkgr_prefix = getpwuid(getuid())->pw_dir;
            }
        }
    }
    pkgr_load_config("/etc/pkgr.conf");  // I may just use Lua for this.  Less libraries that way...
    state = create_lua_state();
}