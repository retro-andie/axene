#!/bin/sh
#
# recopyall.sh for recopyall.sh in Axene-2.0.1/
# Copy all projects trees from <src> to <dst>
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
# Started on  Wed Jan 14 08:46:09 1998 Robin Castanier
# Last update Thu Jun  3 17:10:25 1999 Emmanuel Paris
#

prgdir="Sources"
prglist="Xclamation* XQuad* XAllWrite* XMayday* XInstall* AxeneOffice*"

ECHONL1="echo "
ECHONL2=

test_echo=`echo -n test`
if [ $test_echo = "-n test" ]; then
 test_echo=`echo "test\c"`
 if [ $test_echo = "test\c" ]; then
  test_echo=`echo -e "test\c"`
  if [ $test_echo = "test" ]; then
   ECHONL1="echo -e "
   ECHONL2="\c"
  fi
 else
  ECHONL1="echo "
  ECHONL2="\c"
 fi
else
  ECHONL1="echo -n "
fi

if [ $# != 2 ]; then
 echo "Usage: recopyall.sh <source> <target>"
 exit 1
fi

src=$1
dst=$2

echo "Copy from $1 to $2"

while :; do
 $ECHONL1"Do you agree [y/n] ? "$ECHONL2
 read reply
 if [ "v$reply" = "vy" ]; then
  break;
 elif [ "v$reply" = "vn" ]; then
  echo "Abort "`basename $0`"."
  exit 1
 fi
done 

for i in $prglist; do
 echo Removing $i
 rm -fr $2/$i/$prgdir
 echo Copy $i
 mkdir -p  $2/$i/$prgdir
 cp -a $1/$i/$prgdir $2/$i/
done

cp $1/CommonObjects $1/*.sh $2/

echo done.
