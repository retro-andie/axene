#!/bin/sh
#
# makesure.sh for makesure.sh in Axene-2.0.1/
# Launch make with rules <sure> in all project trees.
#
# Copyright (C) 1999-2000 Axene.
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
# Started on  Fri Jun  4 20:56:15 1999 Emmanuel Paris
# Last update Sat Jun 12 20:22:35 1999 Emmanuel Paris
#

prglist="Xclamation* XQuad* XAllWrite* XMayday* XInstall* AxeneOffice*"
prgdir="Sources"
mode="sure"

if [ "v$OSPRGTYPE" != "v" ]; then
 prgdir=$prgdir"."$OSPRGTYPE
fi

make=`echo $0 | cut -c-4`    
if [ "$make" = "make" ]; then
 mode=`echo $0 | cut -c5- | cut -d. -f1`
fi

for i in $prglist; do
 cd $i/$prgdir
 ./Utils/mkmakefile
 make -k $mode
 cd ../..
done

echo "All is now make $mode."




