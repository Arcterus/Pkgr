#include <pkgr.h>

/**
 * Load a Lua-based configuration file.
 *
 * @param file the configuration file to be loaded
 *
 * @see pkgr_repos
 *
 * @date 2012-08-04
 * @since 2012-08-01
 */
void pkgr_load_config(const char *file) {
    if(luaL_loadfile(state, filename) || lua_pcall(state, 0, 0, 0)) {
        pkgr_repos = calloc(1, sizeof(Repo));
        *pkgr_repos = "http://pkgr.org/packages/" TARGET "/";
    } else {
        lua_getglobal(state, "repos");
        luaL_checktype(state, -1, LUA_TTABLE);
        int size = luaL_getn(state, -1), i;
        pkgr_repos = calloc(size, sizeof(Repo));
        for(i = 1; i <= size; i++) {
            lua_rawgeti(state, -1, i);
            if(!lua_isstring(state, -1)) {
                pkgr_error("All repositories must be strings (%s)", file);
            } else {
                const char *str = lua_tostring(state, -1);
                int index = i - 1;
                pkgr_repos[index] = malloc(strlen(str));
                pkgr_repos[index] = str;
                pkgr_repo_size++;
            }
            lua_pop(state, 1);
        }
    }
}
