dist_doc_DATA = README

include_HEADERS = include/pkgr.h include/pkgr/build-config.h include/pkgr/checksum.h include/pkgr/config.h include/pkgr/download.h include/pkgr/install.h include/pkgr/uninstall.h include/pkgr/unpack.h

lib_LTLIBRARIES = libpkgr.la $(PKGRLIBS)

EXTRA_LTLIBRARIES = libpkgr_gfx.la

libpkgr_la_SOURCES = src/lib/checksum.c src/lib/config.c src/lib/deps.c src/lib/download.c src/lib/install.c src/lib/pkgr.c src/lib/uninstall.c src/lib/unpack.c
libpkgr_la_LDFLAGS = -version-info 1:0:0

libpkgr_gfx_la_SOURCES = src/gfx/graphics.c
libpkgr_gfx_la_LDFLAGS = -version-info 1:0:0

bin_PROGRAMS = $(PKGR)

EXTRA_PROGRAMS = pkgr

pkgr_SOURCES = src/cmd/main.c
pkgr_LDADD = libpkgr.la $(PKGRLIBS)

