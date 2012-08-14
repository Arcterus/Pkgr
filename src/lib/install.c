#include <pkgr.h>

bool pkgr_install(Package *package) {
    char filename[1048];
    snprintf(filename, sizeof(filename), "/var/lib/pkgr/%s-build.lua", package->name);
    pkgr_load_pkgbuild(package);
    lua_getglobal(state, "install");
    if(lua_pcall(state, 0, 0, 0) != 0) {
        pkgr_error("Could not run " NORMAL "build" BOLD RED "in %s", filename);
        return false;
    } else {
        return true;
    }
}