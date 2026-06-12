#!/bin/sh
# 
# AxeneOffice
# AxeneOffice post-exec script
# 
# Copyright (c)1996-2000. All Rights reserved Axene.
# EMail: <xcalibur@axene.org>
# 
# Started on  Fri Nov 17 17:34:10 1998 Antoine Buat
# Last update Sat Jun 10 18:08:53 2000 Emmanuel Paris

prefix=$1
lang=$2

[ -z "$prefix" ] && prefix=`pwd`
[ -z "$lang" ] && lang="English.us"

#--- remove archive files
[ -f ${prefix}/.Axene_desc ] && rm -f ${prefix}/.Axene_desc
[ -f ${prefix}/.Icon ] && rm -f ${prefix}/.Icon

#--- Link ressources files ---
if [ ! -d ${prefix}/app-defaults ]; then
 mkdir ${prefix}/app-defaults || exit 1
 chmod 755 ${prefix}/app-defaults || exit 1
fi
[ -f ${prefix}/app-defaults/AxeneOffice ] && \
    rm -f ${prefix}/app-defaults/AxeneOffice
ln -s ${prefix}/lib/locale/AxeneOffice.$lang ${prefix}/app-defaults/AxeneOffice

#--- Patch Exec Script with ZHOME=${prefix} ---
chmod 755 ${prefix}/AxeneOffice
ed - ${prefix}/AxeneOffice > /dev/null 2>&1 << EOF
/^ZHOME=/s:^.*$:ZHOME=${prefix}:
w
q
EOF

#--- remove post exec script
rm -f ${prefix}/.post_exec

