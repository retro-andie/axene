#!/bin/sh
#
# viewNew.sh for viewNew.sh in Axene-2.0.1/
# Print in stdin all source file modified in the last <n> days.
#
# Copyright (C) 1998-2000 Axene.
# Authors: Stéphane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
# Email: xcalibur@axene.org
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# Started on  Fri Mar 27 19:10:15 1998 Emmanuel Paris
# Last update Sun Jan 30 17:31:37 2000 Emmanuel Paris
#

if [ $# -ne 1 ]; then
 echo "Usage: $0 number(day)"
fi

prglist="Xclamation* XQuad* XAllWrite* XMayday* XInstall* AxeneOffice*"
prgdir="Sources"
if [ "v$OSPRGTYPE" != "v" ]; then
 prgdir=$prgdir"."$OSPRGTYPE
fi

newfiles=""

for i in $prglist; do
 echo "Collection new files in $i/$prgdir." 1>&2
 adfiles="AD/"`echo $i | sed "s/\-.*$//"`
 rcfiles="RC/"`echo $i | sed "s/\-.*$//" | tr A-Z a-z`"rc"
 newfiles="$newfiles "`find $i/$prgdir -mtime -$1 -type f -print | \
    grep -v ".[ao]$" | grep -v "[~#]$" | grep -v ".zpm$" | \
    grep -v "core" | grep -v "uptodate" | grep -v "Headers/" | \
    grep -v "$adfiles" | grep -v "$rcfiles" | egrep -v 'Main/Makefile$' |\
    grep -v "Main/builtdate.h" | grep -v "Global/$i.ad.h" | \
    grep -v "g3states.h" | grep -v "Tongues/Tongue\..*"`
done

for i in $newfiles; do
 if [ -x $i ]; then
  ftype=`file $i | grep "executable"`
  if [ "v$ftype" = "v" ]; then
   echo $i
  fi
 else
  echo $i
 fi
done

