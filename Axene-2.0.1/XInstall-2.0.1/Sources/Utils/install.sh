#!/bin/sh
#
# install.sh for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
# 	AxeneOffice in Utils/
# software installation script
#
# Copyright (C) 1994-2000 Axene.
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
# Started on  Mon Jun  5 14:57:09 2000 Emmanuel Paris
# Last update Sat Jun 10 20:06:36 2000 Emmanuel Paris
#


PWD=`pwd`
THISDIR=`dirname $PWD/$0`
TOP="$THISDIR"/..

#--- Init global functions
progname=`basename $0`
SkipLine() { echo " "; }
Fatal() { echo "${progname} Error: ${1}" 1>&2 ; exit 1; }

SHORT_NAME=$1
LONG_NAME=$2
PREFIX=$3
short_name=`echo $SHORT_NAME | tr "[A-Z]" "[a-z]"`
long_name=`echo $LONG_NAME | sed "s:-[^.]*::"`

###########################################################################
#                              FUNCTIONS                                  #
###########################################################################
Create_directory()
{
 DIRNAME="$1"

 if [ ! -d "$DIRNAME" ]; then
  echo "Creating directory $DIRNAME"
  mkdir -p "$DIRNAME" || Fatal "Can not create directory $DIRNAME. Aborting."
  chmod 0755 "$DIRNAME"
 fi 
}

Copy_file()
{
 FILE="$1"
 
 _srcs=`echo "$FILE" | sed -e "s:=.*$::"`
 for _src in $_srcs; do
  _dst=`echo "$FILE" | sed -e "s:^.*=::" -e "s:~.*$::"`
  _right=`echo "$FILE" | sed "s:^.*~::"`

  [ -z "$_src" ] && return
  if [ -z "$_dst" ]; then
   _dst="${PREFIX}/"`echo $_src | tr -d "$TOP"`
  else
   if [ `echo $_dst | sed "s:^.*\(.\)$:\1:"` = "/" ]; then
    _dst=${_dst}`basename $_src`
   fi
  fi

  if [ -z "$_right" ]; then
   _type=`file $_src`
   _exec=`echo $_type | grep "executable"`
   _script=`echo $_type | grep "script"`
   if [ -z "$_exec" -a -z "$_script" ]; then
    _right="644"
   else
    _right="755"
   fi
  fi

  _bsrc=`basename $_src`
  _ddst=`dirname $_dst`
  _bdst=`basename $_dst`
  if [ "$_bsrc" = "$_bdst" ]; then
   echo "Copy $_bsrc in $_ddst, set permission $_right".
  else
   echo "Copy $_bsrc to $_dst, set permission $_right".   
  fi
  [ ! -f $_src ] && Fatal "$_src is missing. Build everything before install."
  cp $_src $_dst || Fatal "Can not copy $_bsrc to $_ddst. Aborting."  
  chmod $_right $_dst
 done
}

###########################################################################
###########################################################################
##                             MAIN                                      ##
###########################################################################
###########################################################################

###########################################################################
#                          DIRECTORY CREATION                             #
###########################################################################

dirlist=""

case $short_name in
 xclamation | xquad | xallwrite | xmayday | axeneoffice)
  dirlist=$PREFIX" "$PREFIX"/bin "$PREFIX"/lib "$PREFIX"/lib/locale \
    "$PREFIX"/app-defaults"
 ;;
 xinstall)
  dirlist="$PREFIX"
 ;;
 *)
  Fatal "Unknown software $SHORT_NAME. Aborting."
 ;;
esac

for dir in "$PREFIX" "$PREFIX"/bin "$PREFIX"/lib "$PREFIX"/lib/locale \
    "$PREFIX"/app-defaults; do
 Create_directory "$dir"
done

###########################################################################
#                              FILE COPY                                  #
###########################################################################

file_list=""

case $short_name in
 xclamation | xquad)
  filelist=$TOP/$LONG_NAME=$PREFIX/bin/${long_name}~755" \
    "$TOP/AD/${SHORT_NAME}.*=$PREFIX/lib/locale/~644" \
    "$TOP/RC/${short_name}rc.*=$PREFIX/lib/locale/~644" \
    "$TOP/PostScript/xcalibur.prc=$PREFIX/lib/xcalibur.prc~644" \
    "$TOP/Utils/launch.sh=$PREFIX/$SHORT_NAME~755
 ;;
 xallwrite)
  filelist=$TOP/$LONG_NAME=$PREFIX/bin/${long_name}~755" \
    "$TOP/AxeneLauncher-v\*=$PREFIX/bin/~755" \
    "$TOP/AD/${SHORT_NAME}.*=$PREFIX/lib/locale/~644" \
    "$TOP/RC/${short_name}rc.*=$PREFIX/lib/locale/~644" \
    "$TOP/PostScript/xcalibur.prc=$PREFIX/lib/xcalibur.prc~644" \
    "$TOP/Utils/launch.sh=$PREFIX/$SHORT_NAME~755
 ;;
 xmayday)
  filelist=$TOP/$LONG_NAME=$PREFIX/bin/${long_name}~755" \
    "$TOP/AD/${SHORT_NAME}.*=$PREFIX/lib/locale/~644" \
    "$TOP/RC/${short_name}rc.*=$PREFIX/lib/locale/~644" \
    "$TOP/Utils/launch.sh=$PREFIX/$SHORT_NAME~755
 ;;
 axeneoffice)
  filelist=$TOP/$LONG_NAME=$PREFIX/bin/${long_name}~755" \
    "$TOP/AD/${SHORT_NAME}.*=$PREFIX/lib/locale/~644" \
    "$TOP/Utils/launch.sh=$PREFIX/$SHORT_NAME~755
 ;;
 xinstall)
  filelist=$TOP/$LONG_NAME=$PREFIX/bin/${long_name}~755
 ;;
 *)
  Fatal "Unknown software $SHORT_NAME. Aborting."
 ;;
esac

for file in $filelist; do
 Copy_file "$file"
done


