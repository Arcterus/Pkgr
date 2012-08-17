dist_doc_DATA = README

nobase_include_HEADERS = $(wildcard include/*.h) $(wildcard include/pkgr/*.h)

lib_LTLIBRARIES = libpkgr.la
libpkgr_la_SOURCES = $(wildcard src/lib/*.c)
libpkgr_la_LDFLAGS = -version-info 1:0:0

if COMMAND
    bin_PROGRAMS = pkgr
    pkgr_SOURCES = $(wildcard src/cmd/*.c)
    pkgr_LDADD = libpkgr.la
endif
