#ifndef _DOWNLOAD_H
#define _DOWNLOAD_H

#include <pkgr.h>

int pkgr_download_file(const char *url, const char *outfile, bool progbar);
bool pkgr_download(Package *package);

#endif