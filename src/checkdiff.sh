#!/bin/sh
#
# checkdiff.sh for checkdiff.sh in Axene-2.0.1/
# Check Differences between Commom objects
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
# Last update Thu Jan  6 16:49:09 2000 Emmanuel Paris
#

top=$HOME/Axene-2.0.1
srcfile=$top/CommonObjects
prgdir="Sources"
nb_soft=6
version="-2.0.1"
soft_dir="$top/Xclamation$version $top/XQuad$version $top/XAllWrite$version \
$top/XMayday$version $top/XInstall$version $top/AxeneOffice$version"
verbose="no"
toplen=`expr \( length $top \) + 2`
soft_name="Xclamation\nXQuad\nXAllWrite\nXMayday\nXInstall\nAxeneOffice"
soft_abrv="XC|XQ|XA|XM|XI|AO|"

#--- Init global functions
progname=`basename $0`
SkipLine() { echo " "; }
Fatal() { echo "${progname} Error: ${1}" 1>&2 ; exit 1; }

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

# --------------------------------------------------------
# do diff2 - 
# --------------------------------------------------------
do_diff2()
{
 if [ $# -le 1 ]; then
  echo .
  return
 fi

 root=$1
 shift
 save=$@
 
 rootname=`echo $root | cut -c$toplen- | cut -d"-" -f1`
 rootnum=`/bin/echo -e $soft_name | grep -n $rootname | cut -d: -f1`
 rootnum=`expr $rootnum \* 3`
 rootnum=`expr $rootnum - 2`
 rootabr=`echo $soft_abrv | cut -c$rootnum- | cut -d\| -f1`

 i=1
 nb_file=$#
 while [ $i -le $nb_file ]; do
  differ=`diff "$root" "$1" 2> /dev/null`
  tokenname=`echo $1 | cut -c$toplen- | cut -d"-" -f1`
  tokennum=`/bin/echo -e $soft_name | grep -n $tokenname | cut -d: -f1`
  tokennum=`expr $tokennum \* 3`
  tokennum=`expr $tokennum - 2`
  tokenabr=`echo $soft_abrv | cut -c$tokennum- | cut -d\| -f1`
  if [ -n "$differ" ]; then
   $ECHONL1"$rootabr<>$tokenabr "$ECHONL2
  else
   $ECHONL1"$rootabr==$tokenabr "$ECHONL2
  fi
  shift
  i=`expr $i + 1`
 done

 do_diff2 $save
}

# --------------------------------------------------------
# do diff - 
# --------------------------------------------------------
do_diff()
{
 if [ $# -le 1 ]; then
  echo ALONE].
  return
 fi

 save=$@
 is_diff="no"

 i=1
 nb_file=$#
 while [ $i -lt $nb_file ]; do
  differ=`diff "$1" "$2" 2> /dev/null`
  $ECHONL1"."$ECHONL2
  if [ -n "$differ" ]; then
   is_diff="yes"
   break
  fi
  shift
  i=`expr $i + 1`
 done

 if [ $is_diff = "no" ]; then
  echo "] OK."
  return
 fi

 echo "] DIFFER:"
 #$ECHONL1"              "$ECHONL2
 do_diff2 $save
}

# --------------------------------------------------------
# check diff - 
# --------------------------------------------------------
check_diff()
{
 diff_file=

 cutpos=`expr $nb_soft + 2`
 objfile=`echo "$1" | cut -c$cutpos-`

 i=1
 while [ $i -le $nb_soft ]; do
  c=`echo "$1" | cut -c$i`
  if [ "$c" != "." ]; then
   j=1
   for soft in $soft_dir; do
    if [ $i -eq $j ]; then
     if [ -z "$diff_file" ]; then
      diff_file=$soft/$prgdir/$objfile
     else
      diff_file="$diff_file $soft/$prgdir/$objfile"
     fi
     break
    fi
    j=`expr $j + 1`
   done
  fi
  i=`expr $i + 1`
 done;

 $ECHONL1"CheckDiff:  $objfile ["$ECHONL2
 do_diff $diff_file
}

test -r $srcfile || Fatal "The \`Common Objects' file is missing!"

if [ "$1" = "-v" ]; then
 verbose="yes"
 shift
fi

nb_arg=$#
if [ $nb_arg -ne 0 ]; then
 while [ $nb_arg -ne 0 ]; do
  filen=$1
  if [ -f $filen ]; then  
   filen=`echo $filen | sed -e "s/^.*\\/\(.*\\/.*\)$/\1/"`
  fi
  gret=`grep $filen $srcfile | grep -v "#"`
  if [ -z "$gret" ]; then
   echo "${progname} Warning: $filen is not a common file"
  else
   while [ -n "$gret" ]; do
    check_diff "`echo $gret | cut -d' ' -f-2`"
    gret=`echo $gret | cut -d' ' -f3-`
   done
  fi
  shift
  nb_arg=`expr $nb_arg - 1`
 done
else
 count=0
 exec 5<$srcfile
 while read conf <&5 ; do
  count=`expr $count + 1`
  if [ -n "$conf" ]; then
  line=`echo $conf | grep "^[ \t]*\#"`
  if [ -z  "$line" ]; then
    check_diff "$conf"
   fi
  fi
 done
fi


