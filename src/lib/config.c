#include <pkgr.h>
#include <libconfig.h>

/**
 * Load a configuration file.  May be changed to a Lua configuration in the future.
 *
 * @param file the configuration file to be loaded
 *
 * @see pkgr_repos
 *
 * @date 2012-08-04
 * @since 2012-08-01
 */
void pkgr_load_config(const char *file) {
    config_t config;
    config_setting_t *settings;
    config_init(&config);
    if(!config_read_file(&config, file)) {
        pkgr_repos = malloc(32);
        *pkgr_repos = "http://pkgr.org/packages/" TARGET "/";
    } else {
        settings = config_root_setting(&config);
        config_setting_t *reposettings = config_setting_get_member(settings, "repos");
        if(config_setting_is_array(reposettings)) {
            const char *repostr;
            int i, reposize = 0, repolength = 0;
            for(i = 0; (repostr = config_setting_get_string_elem(reposettings, i)) != NULL; i++) {
                if(strlen(repostr) + repolength > reposize) {
                    reposize += 1024;
                    pkgr_repos = realloc(pkgr_repos, reposize);
                }
                pkgr_repos[i] = repostr;
                repolength += strlen(repostr);
            }
        } else {
            pkgr_repos = malloc(32);
            *pkgr_repos = "http://pkgr.org/packages/" TARGET "/";
        }
    }
    config_destroy(&config);
}