#ifndef _CHECKSUM_H
#define _CHECKSUM_H

#include <pkgr.h>

#ifndef CHECKSUM_LENGTH
#define CHECKSUM_LENGTH 69
#endif

char *pkgr_generate_checksum(char *filename, char *hexdigest);
bool pkgr_test_checksum(Package *package);

#endif
