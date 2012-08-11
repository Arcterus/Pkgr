#!/bin/sh

#  pkgrbuild.sh
#  Pkgr
#
#  Created by Alex Lyon on 8/1/12.
#

clang -o pkgr_download pkgr_download.c  -I/usr/local/include -L/usr/local/lib -lcurl -larchive -lconfig -llua -lcrypto
chown root pkgr_download
chmod ug+s pkgr_download
chmod o-rx pkgr_download