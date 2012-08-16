dist_doc_DATA = README

nobase_include_HEADERS = include/{pkgr,pkgr/{checksum,config,download,install,uninstall,unpack,build-config}}.h

lib_LTLIBRARIES = libpkgr.la
libpkgr_la_SOURCES = src/lib/{checksum,config,download,install,pkgr,uninstall,unpack}.c
libpkgr_la_LDFLAGS = -version-info 1:0:0

if COMMAND
    bin_PROGRAMS = pkgr
    pkgr_SOURCES = src/cmd/main.c
    pkgr_LDADD = libpkgr.la
endif