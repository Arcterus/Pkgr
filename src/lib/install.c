#include <pkgr.h>

bool pkgr_install(Package *package) {
    char filename[2073];
    snprintf(filename, sizeof(filename), "/var/lib/pkgr/%s/%s-build.lua", package->name);
    pkgr_load_pkgbuild(package);
    lua_getglobal(state, "pre_install");
    lua_pcall(state, 0, 0, 0);
    if(package->type == PKGR_SOURCE) {
        char dir[1040];
        snprintf(dir, sizeof(dir), "/var/cache/pkgr/%s", package->name);
        chdir(dir);
        lua_pushstring(state, "/var/tmp/pkgr/fake-root");
        lua_setglobal(state, "FAKEROOT");
        lua_getglobal(state, "build");
        if(lua_pcall(state, 0, 0, 0) != 0) {
            pkgr_error("Could not run " NORMAL "build" BOLD RED "in %s", filename);
            return false;
        }
        pkgr_pack(package, PKGR_DATA);
    }
    chdir("/");
    snprintf(filename, sizeof(filename), "/var/cache/pkgr/%s" PKGR_DATA, package->name);
    if(!pkgr_unpack(package, true)) {
        pkgr_error("Could not install %s", package->name);
        return false;
    }
    lua_getglobal(state, "post_install");
    lua_pcall(state, 0, 0, 0);
    return true;
}
