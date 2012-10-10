#include <pkgr.h>
#include <archive.h>
#include <archive_entry.h>

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
int pkgr_unpack(Package *package, bool cwd) {
    char filename[1024];
    // .kpkg files can be any compression format, but must be the tar archiving format
    snprintf(filename, sizeof(filename), "%s.kpkg", package->name);
    if(!cwd) {
        chdir("/var/cache/pkgr/");
    }
    return extract(filename);
}