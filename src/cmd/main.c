/**
 * A package management program named "pkgr".
 * 
 * @file main.c
 *
 * @author Arcterus
 *
 * @version 0.1
 * @date 2012-08-05
 * @since 2012-07-29
 */

#include <pkgr.h>

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
        pkgr_error("Too few arguments");
        return 1;
    }
    pkgr_init();
    if(!strcmp(argv[1], "download")) {
        pkgr_download_file(argv[2], argv[3], true);
    } else if(!strcmp(argv[1], "unpack")) {
        Package *pack = new_Package();
        if(strstr(argv[2], ".kpkg") == NULL) {
            pack->name = argv[2];
            pkgr_unpack(pack, true);
        } else {
            strncpy(pack->name, argv[2], strlen(argv[2]) - 5);
            pkgr_unpack(pack, false);
        }
    } else if(!strcmp(argv[1], "checksum")) {
        char checksum[CHECKSUM_LENGTH];
        puts(pkgr_generate_checksum(argv[2], checksum));
    } else {
        pkgr_error("Unknown command (%s)", argv[1]);
        pkgr_cleanup();
        return 1;
    }
    pkgr_cleanup();
    return 0;
}
