#!/bin/sh
#
# magiccopy.sh for magiccopy.sh in Axene-2.0.1/
# Re-copy Commom files across projects trees
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
# Last update Thu Jan  6 16:30:11 2000 eparis
#

top=$HOME/Axene-2.0.1
srcfile=$top/CommonObjects
nb_soft=6
version="-2.0.1"
prgdir="Sources"
soft_dir="$top/Xclamation$version $top/XQuad$version $top/XAllWrite$version \
$top/XMayday$version $top/XInstall$version $top/AxeneOffice$version"
force="no"
force2="no"
toplen=`expr \( length $top \) + 2`
soft_name="Xclamation XQuad XAllWrite XMayday XInstall AxeneOffice"
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
# magic copy - 
# --------------------------------------------------------
magic_copy()
{
 copy_file=

 cutpos=`expr $nb_soft + 2`
 objfile=`echo "$1" | cut -c$cutpos-`
 comfile=`echo "$1" | cut -c-$nb_soft`

 echo "-----------------------------------------------------------------------"
 echo "Copy filename: "$objfile
 while :; do
  $ECHONL1"Copy Source [$comfile]"$ECHONL2

  defsrc=
  i=1
  cwd=`pwd`
  for soft in $soft_name; do
   tmp=`echo $cwd | grep $soft`
   if [ -n "$tmp" ]; then
    defnum=`expr $i \* 3`
    defnum=`expr $defnum - 1`
    defsrc=`echo $soft_abrv | cut -b $defnum`
    tmp=`echo $comfile | grep $defsrc`
    if [ -n "$tmp" ]; then
     $ECHONL1" (Default $defsrc)"$ECHONL2
    fi
    break
   fi
   i=`expr $i + 1`
  done 

  $ECHONL1"? "$ECHONL2
  if [ "${force2}" = "yes" -a -n "$defsrc" ]; then
   reply=$defsrc
   echo $defsrc
  else
   read reply
  fi

  if [ -z "$reply" -a -n "$defsrc" ]; then
   reply=$defsrc
   break;
  elif [ `expr length "x$reply"` -eq 2 -a "$reply" != "." ]; then
   tmp=`echo $comfile | grep $reply`
   if [ -n "$tmp" ]; then
    break;
   fi
  fi
 done
 src=$reply

 dstfile=`echo $comfile | tr $src "."`
 defdst=`echo $dstfile | tr -d "."`

 while :; do
  $ECHONL1"Copy Destination [$dstfile] (Default $defdst)? "$ECHONL2

  if [ "${force2}" = "yes" -a -n "$defdst" ]; then
   reply=$defdst
   echo $defdst
  else
   read reply
  fi

  if [ -z "$reply" ]; then
   reply=$defdst
   break;
  else
   endreply=
   while [ -n "$reply" ]; do
    c=`echo $reply | cut -c1`
    tmp=`echo $dstfile | grep $c`
    if [ "$c" != "." ]; then
     if [ -n "$tmp" ]; then
      endreply=$endreply$c
     else
      endreply=
      break;
     fi
    fi
    reply=`echo $reply | cut -c2-`
   done
   if [ -n "$endreply" ]; then
    reply=$endreply
    break;
   fi
  fi
 done
 dst=$reply

 i=1
 while [ $i -le $nb_soft ]; do
  c=`echo "$1" | cut -c$i`
  if [ "$c" = "$src" ]; then
   j=1
   for soft in $soft_dir; do
    if [ $i -eq $j ]; then
     srcfile=$soft
    fi
    j=`expr $j + 1`
   done
  fi
  i=`expr $i + 1`
 done

 i=1
 while [ $i -le $nb_soft ]; do
  c=`echo "$1" | cut -c$i`
  if [ "$c" != "." ]; then
   tmp=`echo $dst | grep "$c"`
   if [ -n "$tmp" ]; then
    j=1
    for soft in $soft_dir; do
     if [ $i -eq $j ]; then
      dstfile=$soft
      while :; do
       $ECHONL1"Copy from `basename $srcfile` to `basename $dstfile`"$ECHONL2
       $ECHONL1" [y/n] (Default y)? "$ECHONL2
       
       if [ "${force}" = "yes" ]; then
        reply="y"
        echo "y"
        break;
       else
        read reply
        if [ -z "$reply" ]; then
         reply="y"
         break;
        elif [ "$reply" = "y" -o "$reply" = "n" ]; then
         break;
        fi
       fi
      done
      if [ "$reply" = "y" ]; then
       cp $srcfile/$prgdir/$objfile $dstfile/$prgdir/$objfile
      fi
     fi
     j=`expr $j + 1`
    done
   fi
  fi
  i=`expr $i + 1`
 done
}

test -r $srcfile || Fatal "The \`Common Objects' file is missing!"

if [ "$1" = "-f" ]; then
 force="yes"
 shift
fi

if [ "$1" = "-ff" ]; then
 force="yes"
 force2="yes"
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
    magic_copy "`echo $gret | cut -d' ' -f-2`"
    gret=`echo $gret | cut -d' ' -f3-`
   done
  fi
  shift
  nb_arg=`expr $nb_arg - 1`
 done
else
 echo "Usage: $progname [-f/-ff] string"
 echo "   -f          force copy."
 echo "   -ff         force source, destination and copy (be sure !)."
fi

