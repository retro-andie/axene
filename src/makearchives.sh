#!/bin/sh
set -xv
#
# makearchives.sh for makearchives.sh in Axene-2.0.1/
#
# Copyright (C) 2000-2000 Axene.
# Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
# Started on  Sat Apr  8 17:42:59 2000 Emmanuel Paris
# Last update Mon Jun  5 19:52:22 2000 Emmanuel Paris
#

PWD=`pwd`
THIS="$0"
NAME=`basename $THIS`
TOP=`dirname $PWD/$THIS`
TMPDIR=`echo $TOP/Archives-*`/Temp
DSTDIR=`echo $TOP/Archives-*`/Archives
OWN_UID=`id -u`
OWN_GID=`id -g`

trap "[ -d $TMPDIR ] && rm -fr $TMPDIR; exit 1" HUP INT QUIT TERM
trap "[ -d $TMPDIR ] && rm -fr $TMPDIR" EXIT

######### #### #  # #  #  ### ##### ###  ##  #  #  ### ######################
######### ###  #  # ## # #      #    #  #  # ## # ##   ######################
######### #    #  # # ## #      #    #  #  # # ##   ## ######################
######### #     ##  #  #  ###   #   ###  ##  #  # ###  ######################

######### ###  ### #  #   #   ###  ### ####  ### ############################
######### # ##  #  ## #  # #  #  #  #  ###  ##   ############################
######### ## #  #  # ## ##### ###   #  #      ## ############################
######### ###  ### #  # #   # # ## ### #### ###  ############################

CreateBinArchive()
{
 _arch=$1
 _arch_binfile=0
 _arch_adfile=0	
 _arch_rcfile=0
 _arch_prcfile=0
 _arch_supfile=""
 _arch_supbinfile=""
  
 case $_arch in
  cm)
   _arch_name="common"
   _arch_desc="Axene Common Files"
   _arch_subdir="Common-*"
   _arch_supfile="*"
   _arch_xcnum="255 0"
   ;;
  ao)
   _arch_name="AxeneOffice"
   _arch_desc="Axene Launch Bar"
   _arch_subdir="AxeneOffice-2.0.1"
   _arch_binfile=1
   _arch_adfile=1
   _arch_xcnum="8 0 o9"
   ;;
  xc)
   _arch_name="Xclamation"
   _arch_desc="Axene Xclamation"
   _arch_subdir="Xclamation-2.0.1"
   _arch_binfile=1
   _arch_adfile=1
   _arch_rcfile=1
   _arch_prcfile=1
   _arch_xcnum="0 1 o1 m255"
   ;;
  xq)
   _arch_name="XQuad"
   _arch_desc="Axene XQuad"
   _arch_subdir="XQuad-2.0.1"
   _arch_binfile=1
   _arch_adfile=1
   _arch_rcfile=1
   _arch_prcfile=1
   _arch_xcnum="2 2 o3 m255"
   ;;
  xa)
   _arch_name="XAllWrite"
   _arch_desc="Axene XAllWrite"
   _arch_subdir="XAllWrite-2.0.1"
   _arch_binfile=1
   _arch_adfile=1
   _arch_rcfile=1
   _arch_prcfile=1
   _arch_supbinfile="AxeneLauncher"
   _arch_xcnum="4 3 o5 m255"
   ;;
  xm)
   _arch_name="XMayday"
   _arch_desc="Axene XMayday"
   _arch_subdir="XMayday-2.0.1"
   _arch_binfile=1
   _arch_adfile=1
   _arch_rcfile=1
   _arch_supfile="Api/*=lib/api/"
   _arch_xcnum="6 0 o7"
   ;;
 esac

 echo -n "Creation of archive for ${_arch_name} : [" 
 [ -d $TMPDIR ] && rm -fr $TMPDIR
 mkdir -p $TMPDIR

 _xfiles="" 
 _subdir=`echo $_arch_subdir`
 if [ ! -d $_subdir ]; then
  echo "] failed."
  echo "$_subdir doesn't exist. Skip to next archive."
  return
 fi
 echo -n "S"

 touch $TMPDIR/.Axene_desc
 [ -f $TOP/Archives-*/Icons/${_arch_name}.bin.xpm ] && \
  cp -p $TOP/Archives-*/Icons/${_arch_name}.bin.xpm $TMPDIR/.Icon
 echo -n "I"
 if [ -f  $TOP/Archives-*/PostInstallScripts/$_arch_name ]; then
  cp -p $TOP/Archives-*/PostInstallScripts/$_arch_name $TMPDIR/.post_exec
  _xfiles=${_xfiles}"$TMPDIR/.post_exec "
  echo -n "P"
 fi
 if [ -f  $TOP/Archives-*/LaunchScripts/$_arch_name ]; then
  cp -p $TOP/Archives-*/LaunchScripts/$_arch_name $TMPDIR/
  _xfiles=${_xfiles}"$TMPDIR/$_arch_name "
  echo -n "L"
 fi
 cp -p $TOP/Archives-*/InfoFiles/* $TMPDIR/
 echo -n "i"
  
 if [ $_arch_adfile -eq 1 ]; then
  _adfiles=`ls -1 $_subdir/AD/${_arch_name}.* 2>/dev/null`
  if [ -z "$_adfiles" ]; then
   echo "] failed."
   echo "No AD files found in $_subdir/AD, run 'make ad' before\
    Skip to next archive."
   return
  fi
  mkdir $TMPDIR/app-defaults
  mkdir -p $TMPDIR/lib/locale
  cp -p $_subdir/AD/${_arch_name}.* $TMPDIR/lib/locale/
  cp -p $_subdir/AD/${_arch_name}.English.us \
	$TMPDIR/app-defaults/$_arch_name
  echo -n "a"
 fi

 if [ $_arch_rcfile -eq 1 ]; then
  _rcfilename=`echo $_arch_name | tr "[:upper:]" "[:lower:]"`rc
  _rcfiles=`ls -1 $_subdir/RC/${_rcfilename}.* 2>/dev/null`
  if [ -z "$_rcfiles" ]; then
   echo "] failed."
   echo "No RC files found in $_subdir/RC, run 'make rc' before\
    Skip to next archive."
   return
  fi
  cp -p $_subdir/RC/${_rcfilename}.* $TMPDIR/lib/locale/
  cp -p $_subdir/RC/${_rcfilename}.English.us $TMPDIR/lib/${_rcfilename}
  echo -n "r"
 fi

 if [ $_arch_prcfile -eq 1 ]; then
  if [ ! -f $_subdir/PostScript/xcalibur.prc ]; then
   echo "] failed."
   echo "The prc file $_subdir/PostScript/xcalibur.prc is missing. \
    Skip to next archive."
   return
  fi
  cp $_subdir/PostScript/xcalibur.prc $TMPDIR/lib/
  echo -n "p"
 fi

 if [ -d $_subdir/Documentation ]; then
  for _lang in English French; do
   if [ -d $_subdir/Documentation/$_lang ]; then
    mkdir -p $TMPDIR/lib/doc/${_arch_name}/$_lang
    cp -Rp $_subdir/Documentation/$_lang/* \
      $TMPDIR/lib/doc/${_arch_name}/$_lang/
   fi
  done
  echo -n "d"
 fi

 if [ -n "$_arch_supfile" ]; then
  set -f	# disable path name expansion
  for _i in $_arch_supfile; do
   _dstdir=$TMPDIR
   if `echo $_i | grep -q "="`; then
    _src=`echo $_i | cut -d"=" -f1`
    _dst=`echo $_i | cut -d"=" -f2`
    _dstdir=${_dstdir}/$_dst
   else
    _src=$_i
   fi
   
   set +f	# enable path name expansion
   _files=`ls -1 $_subdir/$_src 2>/dev/null`
   if [ -z "$_files" ]; then
    echo "] failed."
    echo "There is no files in $_files. Skip to next archive."
    return
   fi
   
   [ ! -d $_dstdir ] && mkdir -p $_dstdir
   cp -frp $_subdir/$_src $_dstdir
   _xfiles=${_xfiles}`find $_subdir/$_src -perm -111 -type f`" "
   set -f	# disable path name expansion
  done

  set +f	# enable path name expansion
  echo -n "+"
 fi

  # Set right permission
  find $TMPDIR -type f -exec chmod 644 {} \;
  find $TMPDIR -type d -exec chmod 755 {} \;
  _xfiles=`echo $_xfiles | sed "s: *$::g"`
  [ -n "$_xfiles" ] && chmod 755 $_xfiles
  echo -n "R"

  # Set UID/GID
  chown -R $OWN_UID $TMPDIR
  chgrp -R $OWN_GID $TMPDIR
  echo -n "O"

 _binfiles="none"
 if [ $_arch_binfile -eq 1 ]; then
  _binfiles=`ls -1 $_subdir/${_arch_name}-v* \
    $TOP/Archives-*/CrossBinaries/${_arch_name}-v* 2>/dev/null`
  if [ -z "$_binfiles" ]; then
   echo "] failed."
   echo "No binary file in $_subdir/ or in"
   echo "$TOP/Archives-*/CrossBinaries/."
   echo "Run 'make' before. Skip to next archive."
   return
  fi
 fi

 for _binf in $_binfiles; do

  if [ $_arch_binfile -eq 1 ]; then
   [ -d $TMPDIR/bin ] && rm -fr $TMPDIR/bin
   mkdir $TMPDIR/bin
   cp -p  $_binf $TMPDIR/bin
   chmod 755 $TMPDIR/bin/$_arch_name-v*
   chown -R $OWN_UID $TMPDIR/bin/$_arch_name-v*
   chgrp -R $OWN_GID $TMPDIR/bin/$_arch_name-v*
   echo -n "B"
  fi 

  if [ -n "$_arch_supbinfile" ]; then
   _supbinfile=`dirname $_binf`/${_arch_supbinfile}`echo $_binf | \
    sed "s:.*\(-v...\)[^.]*\(.*$\):\1*\2:"`
   _supbinf=`ls -1 $_supbinfile 2> /dev/null`
   if [ -z $_supbinf ]; then
    echo "] failed."
    echo "Can not found $_supbinfile. Skip to next cross binary file."
    echo -n "            ["
    continue
   fi
   cp -p $_supbinf $TMPDIR/bin
   chmod 755 $TMPDIR/bin/${_arch_supbinfile}-v*
   chown -R $OWN_UID $TMPDIR/bin/${_arch_supbinfile}-v*
   chgrp -R $OWN_GID $TMPDIR/bin/${_arch_supbinfile}-v*
   echo -n "b"
  fi

  _version=""
  if [ $_arch_binfile -eq 1 ]; then
   _version=`echo $TMPDIR/bin/$_arch_name-v* | \
     sed "s:.*-v\([0-9][^.]*\)\..*:\1:"`
   if `echo $_version | grep -Eq "^[0-9][0-9][0-9]"`; then
    _version=" "`echo $_version | sed -e "s:\([0-9]\):\1.:g" \
     -e "s:\([0-9]\)\.\([^0-9]\):\1 \2:" -e "s:\.$::"`
   else
    _version=""
   fi
  fi
 
  _size=`du -s -b $TMPDIR/ 2>/dev/null`
  if [ $? -ne 0 ]; then
   _size=`du -s -k $TMPDIR/ 2>/dev/null | cut -f1`
   _size=`expr $_size \* 1024`
  else
   _size=`echo $_size | cut -d" " -f1`
  fi

  echo "${_arch_desc}${_version}"'\n'"[s]: [k]" >> $TMPDIR/.Axene_desc
  echo "${_size}" >> $TMPDIR/.Axene_desc
  echo "${_arch_xcnum}" >> $TMPDIR/.Axene_desc
  [ -f $TMPDIR/.post_exec ] && echo ".post_exec" >> $TMPDIR/.Axene_desc
  echo -n "A"
 
  _name=${_arch_name}.tar.gz
  if [ $_arch_binfile -eq 1 ]; then
   _name=`basename $TMPDIR/bin/$_arch_name-v*`.tar.gz
  fi
  rm -f $_name $DSTDIR/$_name

  cd $TMPDIR; tar cvpf - .[_0-9A-z]* * | gzip -c - > $DSTDIR/$_name
  archname=${archname}${_name}" "
  echo -n "T"
  cd "$TOP"

 done

 echo "] done."
 rm -fr $TMPDIR
}

######### ###   ##   ### #  # #   # #### #  # ##### #########################
######### #  # #  # #    #  # ## ## ###  ## #   #   #########################
######### #  # #  # #    #  # # # # #    # ##   #   #########################
######### ###   ##   ###  ##  #   # #### #  #   #   #########################

CreateDocArchive()
{
 _arch=$1
 case $_arch in
  xcen)
   _arch_name="Xclamation"
   _arch_desc="Axene Xclamation Doc"
   _arch_subdir="Xclamation-2.0.1/Documentation"
   _arch_lang="English"
   _arch_version="1.4"
   _arch_xcnum="1 0"
   ;;
  xcfr)
   _arch_name="Xclamation"
   _arch_desc="Axene Xclamation Doc"
   _arch_subdir="Xclamation-2.0.1/Documentation"
   _arch_lang="French"
   _arch_version="1.4"
   _arch_xcnum="1 0 l3"
   ;;
  xqen)
   _arch_name="XQuad"
   _arch_desc="Axene XQuad Doc"
   _arch_subdir="XQuad-2.0.1/Documentation"
   _arch_lang="English"
   _arch_version="1.4"
   _arch_xcnum="3 0"
   ;;
  xqfr)
   _arch_name="XQuad"
   _arch_desc="Axene XQuad Doc"
   _arch_subdir="XQuad-2.0.1/Documentation"
   _arch_lang="French"
   _arch_version="1.4"
   _arch_xcnum="3 0 l3"
   ;;
  xaen)
   _arch_name="XAllWrite"
   _arch_desc="Axene XAllWrite Doc"
   _arch_subdir="XAllWrite-2.0.1/Documentation"
   _arch_lang="English"
   _arch_version="1.0"
   _arch_xcnum="5 0"
   ;;
  xafr)
   _arch_name="XAllWrite"
   _arch_desc="Axene XAllWrite Doc"
   _arch_subdir="XAllWrite-2.0.1/Documentation"
   _arch_lang="French"
   _arch_version="1.0"
   _arch_xcnum="5 0 l3"
   ;;
  xmen)
   _arch_name="XMayday"
   _arch_desc="Axene XMayday Doc"
   _arch_subdir="XMayday-2.0.1/Documentation"
   _arch_lang="English"
   _arch_version="1.2"
   _arch_xcnum="7 0"
   ;;
  xmfr)
   _arch_name="XMayday"
   _arch_desc="Axene XMayday Doc"
   _arch_subdir="XMayday-2.0.1/Documentation"
   _arch_lang="French"
   _arch_version="1.2"
   _arch_xcnum="7 0 l3"
   ;;
 esac

 echo -n "Creation of archive for ${_arch_name}'s documentation in ${_arch_lang} : [" 
 [ -d $TMPDIR ] && rm -fr $TMPDIR
 mkdir -p $TMPDIR

 _subdir=`echo $_arch_subdir`/${_arch_lang}
 if [ ! -d $_subdir ]; then
  echo "] failed."
  echo "$_subdir doesn't exist. Skip to next archive."
  return
 fi
 echo -n "S"
 touch $TMPDIR/.Axene_desc

 [ -f $TOP/Archives-*/Icons/${_arch_name}.doc.xpm ] && \
  cp -p $TOP/Archives-*/Icons/${_arch_name}.doc.xpm $TMPDIR/.Icon
 echo -n "I"

 cp -p $TOP/Archives-*/InfoFiles/* $TMPDIR/
 echo -n "i"

 # copy recursively doc subdirectory to tmpdir
 mkdir -p $TMPDIR/lib/doc/${_arch_name}/${_arch_lang}
 cp -vpR $_subdir/* $TMPDIR/lib/doc/${_arch_name}/${_arch_lang} 
 echo -n "C"

 # generate .Axene_desc
 _size=`du -s -b $TMPDIR/ 2>/dev/null`
 if [ $? -ne 0 ]; then
  _size=`du -s -k $TMPDIR/ 2>/dev/null | cut -f1`
  _size=`expr $size \* 1024`
 else
  _size=`echo $_size | cut -d" " -f1`
 fi

 echo "${_arch_desc} ${_arch_version}"'\n'"[s]: [k]" >> $TMPDIR/.Axene_desc
 echo "${_size}" >> $TMPDIR/.Axene_desc
 echo "${_arch_xcnum}" >> $TMPDIR/.Axene_desc
 echo -n "A"

 # Set right permission
 find $TMPDIR -type f -exec chmod 644 {} \;
 find $TMPDIR -type d -exec chmod 755 {} \;
 echo -n "R"

 # generate final tar
 _lang=`echo $_arch_lang | tr '[:upper:]' '[:lower:]'`
 _version=`echo $_arch_version | tr -d "."`x
 _name=${_arch_name}-v${_version}.doc.${_lang}.tar.gz
 rm -f $_name $DSTDIR/$_name

 cd $TMPDIR; tar cvpf - .[_0-9A-z]* * | gzip -c - > $DSTDIR/$_name
 archname=${archname}${_name}" "
 echo -n "T"
 cd "$TOP"
 echo "] done."
 rm -fr $TMPDIR
}

#########  ###  ##  #  # ###   ### ####  ### ###############################
######### ##   #  # #  # #  # #    ###  ##   ###############################
#########   ## #  # #  # ###  #    #      ## ###############################
######### ###   ##   ##  # ##  ### #### ###  ###############################

CreateSrcArchive()
{
 _arch=$1
 case $_arch in
  ao)
   _arch_name="AxeneOffice"
   _arch_desc="Axene Launch Bar Src"
   _arch_subdir="AxeneOffice-2.0.1"
   _arch_xcnum="18 0 o9"
   ;;
  xc)
   _arch_name="Xclamation"
   _arch_desc="Axene Xclamation Src"
   _arch_subdir="Xclamation-2.0.1"
   _arch_xcnum="10 0 o1 m255"
   ;;
  xq)
   _arch_name="XQuad"
   _arch_desc="Axene XQuad Src"
   _arch_subdir="XQuad-2.0.1"
   _arch_xcnum="12 0 o3 m255"
   ;;
  xa)
   _arch_name="XAllWrite"
   _arch_desc="Axene XAllWrite Src"
   _arch_subdir="XAllWrite-2.0.1"
   _arch_xcnum="14 0 o5 m255"
   ;;
  xm)
   _arch_name="XMayday"
   _arch_desc="Axene XMayday Src"
   _arch_subdir="XMayday-2.0.1"
   _arch_xcnum="16 0 o7"
   ;;
  xi)
   _arch_name="XInstall"
   _arch_desc="Axene XInstall Src"
   _arch_subdir="XInstall-2.0.1"
   _arch_xcnum="20 0"
   ;;
 esac

 echo -n "Creation of archive for ${_arch_name}'s sources : [" 
 [ -d $TMPDIR ] && rm -fr $TMPDIR
 mkdir -p $TMPDIR

 _xfiles="" 
 _subdir=`echo $_arch_subdir`
 if [ ! -d $_subdir ]; then
  echo "] failed."
  echo "$_subdir doesn't exist. Skip to next archive."
  return
 fi
 echo -n "S"

 touch $TMPDIR/.Axene_desc
 [ -f $TOP/Archives-*/Icons/${_arch_name}.src.xpm ] && \
  cp -p $TOP/Archives-*/Icons/${_arch_name}.src.xpm $TMPDIR/.Icon
 echo -n "I"

 cp -p $TOP/Archives-*/InfoFiles/* $TMPDIR/
 echo -n "i"

 mkdir -p $TMPDIR/src
 cp -p $TOP/Archives-*/Configure/* $TMPDIR/src/
 _xfiles="$TMPDIR/src/configure $TMPDIR/src/config.guess"
 echo -n "c"

 # get usefull file list
 _arch_adfiles="AD/"`echo ${_arch_name} | sed "s/\-.*$//"`
 _arch_rcfiles="RC/"`echo ${_arch_name} | sed "s/\-.*$//" | tr A-Z a-z`"rc"
 _files=`(cd $_subdir; find . ! -type d -print) | \
    grep -Ev "\.[ao]$" | grep -Ev "[~#]$" | grep -Ev ".zpm$" | \
    grep -v "core" | grep -v "uptodate" | grep -Ev "^\./Headers/" | \
    grep -v "${_arch_adfiles}" | grep -v "${_arch_rcfiles}" | \
    grep -Ev 'Main/Makefile$' | grep -v "Main/builtdate.h" | \
    grep -v "Global/${_arch_name}.ad.h" | grep -Ev "^\./main" | \
    grep -Ev "^\./launcher" | grep -Ev "^\./get_realname" | \
    grep -Ev "^\./${_arch_name}-v[0-9][0-9]*" | \
    grep -Ev "^\./AxeneLauncher-v.*" | grep -Ev "/\." | \
    grep -v "g3states.h" | grep -Ev "Tongues/Tongue\..*"`
 echo -n "F"

 # get directory list 
 dirs=`(cd $_subdir; find . -type d)`
 echo -n "D"

 # get executable script list
 _xfiles=`(cd $_subdir; find . -type f ! -name "*.[choa]" -exec file {} \; )| grep -wi "script" | cut -d":" -f1 | grep -Ev "[~#]$"`" "$_xfiles 
 echo -n "X"

 # create all directory in tmpdir
 _version=`echo ${_arch_subdir} | sed "s:.*-\([0-9][0-9.]*\).*:\1:"`
 mkdir -p $TMPDIR/src/${_arch_name}-${_version}
 for _dir in $_dirs; do
  mkdir -p $TMPDIR/src/${_arch_name}-${_version}/$_dir
 done
 echo -n "M"

 # copy recursively src subdirectory to tmpdir
 for _f in $_files; do
  cp -vp ${_subdir}/${_f} $TMPDIR/src/${_arch_name}-${_version}/${_f} 
 done
 echo -n "C"

 # copy launch and postinstall script in Utils/
 if [ -f  $TOP/Archives-*/PostInstallScripts/$_arch_name ]; then
  cp -p $TOP/Archives-*/PostInstallScripts/$_arch_name \
    $TMPDIR/src/${_arch_name}-${_version}/Utils/postinstall.sh
   _xfiles=$_xfiles" \
    $TMPDIR/src/${_arch_name}-${_version}/Utils/postinstall.sh"
  echo -n "P"
 fi
 if [ -f  $TOP/Archives-*/LaunchScripts/$_arch_name ]; then
  cp -p $TOP/Archives-*/LaunchScripts/$_arch_name \
   $TMPDIR/src/${_arch_name}-${_version}/Utils/launch.sh
  _xfiles=$_xfiles" $TMPDIR/src/${_arch_name}-${_version}/Utils/launch.sh"
  echo -n "L"
 fi

 # fill .Axene_desc
 _size=`du -s -b $TMPDIR/ 2>/dev/null`
 if [ $? -ne 0 ]; then
  _size=`du -s -k $TMPDIR/ 2>/dev/null | cut -f1`
  _size=`expr $size \* 1024`
 else
  _size=`echo $_size | cut -d" " -f1`
 fi

 echo "${_arch_desc} ${_version}"'\n'"[s]: [k]" >> $TMPDIR/.Axene_desc
 echo "${_size}" >> $TMPDIR/.Axene_desc
 echo "${_arch_xcnum}" >> $TMPDIR/.Axene_desc
 echo -n "A"

 # Set right permission
 find $TMPDIR -type f -exec chmod 644 {} \;
 find $TMPDIR -type d -exec chmod 755 {} \;
 _xfiles=`echo $_xfiles | sed "s: *$::g"`
 [ -n "$_xfiles" ] && (cd $TMPDIR/src/${_arch_name}-${_version}; \
   chmod 755 $_xfiles )
 echo -n "R"

 # Set UID/GID
 echo -n "O"

 # generate final tar
 _version=`echo $_version | tr -d "."`
 _name=${_arch_name}-v${_version}.src.tar.gz
 rm -f $_name $DSTDIR/$_name

 cd $TMPDIR; tar cvpf - .[_0-9A-z]* * | gzip -c - > $DSTDIR/$_name
 cd "$TOP"
 archname=${archname}${_name}" "
 echo -n "T"

 echo "] done."
 rm -fr $TMPDIR
}

#########  #### ###  #  # ###################################################
######### ###   #  # ###  ###################################################
#########   ##  #  # # #  ###################################################
######### ###   ###  #  # ###################################################

CreateSdkArchive()
{
 _top=`basename $PWD`
 _arch_name="AxeneSDK"
 _arch_arch_subdir="Archives-*/"
 _arch_common_subdir="Common-*/"
 _arch_src_subdirs="AxeneOffice-*/ Xclamation-*/ XQuad-*/ XAllWrite-*/ \
	XMayday-*/ XInstall-*/"
 _version=`echo ${_arch_arch_subdir} | sed "s:.*-\([0-9][0-9.]*\).*:\1:"`
 _name=${_arch_name}-v${_version}.tar

 echo -n "Creation of archive for ${_arch_name} : [" 
 [ -d $TMPDIR ] && rm -fr $TMPDIR
 mkdir -p $TMPDIR

 # archive misc scripts
 _files=`(cd $TOP/..; find $_top/ -maxdepth 1 -type f ) | grep -Ev "[~#]$"`
 (cd $TOP/..; tar cpf $DSTDIR/$_name $_files ) > /dev/null 2>&1
 echo -n "S"

 # add common files
 cd $TOP/..; tar rvpf $DSTDIR/$_name $_top/$_arch_common_subdir
 cd "$TOP"
 echo -n "C"
 
 # add archives files
 _files=`(cd $TOP/..; find $_top/$_arch_arch_subdir ! -type d -print) | \
    grep -Ev "[~#]$" | grep -Ev "tgz$" | grep -Ev "tar.gz$" | grep -vw Temp | \
    grep -Ev "CrossBinaries/[^R][^E]"`
 echo -n "A"

 cd $TOP/..; tar rvpf $DSTDIR/$_name $_files 
 echo -n "m"

 # add axene products
 for _arch_subdir in $_arch_src_subdirs; do

  # add sources
  _bin_name=`echo ${_arch_subdir} | sed "s/\-.*$//"`
  _arch_adfiles="AD/"`echo ${_arch_subdir} | sed "s/\-.*$//"`
  _arch_rcfiles="RC/"`echo ${_arch_subdir} | sed "s/\-.*$//" | tr A-Z a-z`"rc"
  _files=`(cd $TOP/..; find $_top/${_arch_subdir} ! -type d -print) | \
    grep -Ev "\.[ao]$" | grep -Ev "[~#]$" | grep -Ev ".zpm$" | \
    grep -v "core" | grep -v "uptodate" | grep -Ev "/Headers/.*" | \
    grep -v "${_arch_adfiles}" | grep -v "${_arch_rcfiles}" | \
    grep -Ev 'Main/Makefile$' | grep -v "Main/builtdate.h" | \
    grep -v "Global/${_bin_name}.ad.h" | grep -Ev "/main$" | \
    grep -Ev "/launcher$" | grep -Ev "/get_realname$" | \
    grep -Ev "/${_bin_name}-v[0-9][0-9]*" | \
    grep -Ev "/AxeneLauncher-v.*" | grep -Ev "/\." | \
    grep -v "g3states.h" | grep -Ev "Tongues/Tongue\..*"`
  echo -n `echo $_arch_subdir | cut -b1`

  cd $TOP/..; tar rvpf $DSTDIR/$_name $_files
  cd "$TOP"
  echo -n `echo $_arch_subdir | cut -b2`

  # add documentation
  cd $TOP/..; tar rvpf $DSTDIR/$_name $_top/${_arch_subdir}Documentation
  cd "$TOP"
  echo -n `echo $_arch_subdir | cut -b3`

  # add examples
  if [ -d $_arch_subdir/Example ]; then
   cd $TOP/..; tar rvpf $DSTDIR/$_name $_top/${_arch_subdir}Example
   cd "$TOP"
   echo -n `echo $_arch_subdir | cut -b4`
 fi

 done

 # gzip archive
 [ -f $DSTDIR/${_name}.gz ] && rm -f $DSTDIR/${_name}.gz
 gzip $DSTDIR/$_name
 echo -n "G"
 
 archname=${archname}${_name}.gz" "
 cd "$TOP"
 echo "] done."
 #rm -fr $TMPDIR
}

######### #   #   #   ### #  # ##############################################
######### ## ##  # #   #  ## # ##############################################
######### # # # #####  #  # ## ##############################################
######### #   # #   # ### #  # ##############################################

if [ $# -eq 0 ]; then

 cat << EOF
 Choose one or several archives in the following list:

   ---------------------------------------------------- 
   Binaries
   ----------------------------------------------------
   1 - Common Files          2 - AxeneOffice
   3 - Xclamation            4 - XQuad                 
   5 - XAllWrite             6 - XMayday
   ----------------------------------------------------
   Documentations
   ----------------------------------------------------
   10 - Xclamation English  11 - Xclamation French
   12 - XQuad English       13 - XQuad French
   14 - XAllWrite English   15 - XAllWrite French
   16 - XMayday English     17 - XMayday French
   ----------------------------------------------------
   Sources
   ----------------------------------------------------
   30 - AxeneOffice         31 - XInstall
   32 - Xclamation          33 - XQuad
   34 - XAllWrite           35 - XMayday
   
   40 - Axene SDK

EOF

 echo -n " Choice: "
 read choice
 echo ""
else
 if [ $1 = "ticker" ]; then
  exit
 fi

 choice=$@
fi

archlist=""
set - $choice
while [ $# -gt 0 ]; do
 i=$1
 shift
 val=`echo $i | sed "s:[^0-9-]::g"`
 if `echo $val | grep -q -- "-"`; then
  start=`echo $val | cut -d"-" -f1`
  end=`echo $val | cut -d"-" -f2`
  val=""
  while [ $start -le $end ]; do
   val=${val}${start}" "
   start=`expr $start + 1`
  done
 fi
 archlist=${archlist}$val" "
done

binarch=""
docarch=""
srcarch=""
sdkarch=""
 
for arch in $archlist; do

 case $arch in

  1) binarch=${binarch}"cm ";;
  2) binarch=${binarch}"ao ";;
  3) binarch=${binarch}"xc ";;
  4) binarch=${binarch}"xq ";;
  5) binarch=${binarch}"xa ";;
  6) binarch=${binarch}"xm ";;

  10) docarch=${docarch}"xcen ";;
  11) docarch=${docarch}"xcfr ";;
  12) docarch=${docarch}"xqen ";;
  13) docarch=${docarch}"xqfr ";;
  14) docarch=${docarch}"xaen ";;
  15) docarch=${docarch}"xafr ";;
  16) docarch=${docarch}"xmen ";;
  17) docarch=${docarch}"xmfr ";;

  30) srcarch=${srcarch}"ao ";;
  31) srcarch=${srcarch}"xi ";;
  32) srcarch=${srcarch}"xc ";;
  33) srcarch=${srcarch}"xq ";;
  34) srcarch=${srcarch}"xa ";;
  35) srcarch=${srcarch}"xm ";;

  40) sdkarch=${sdkarch}"sd ";;
   
 esac

done

[ ! -d $DSTDIR ] && mkdir -p $DSTDIR
archname=""

if [ -n "$binarch" ]; then  
 for arch in $binarch; do
  CreateBinArchive $arch
 done
fi

if [ -n "$docarch" ]; then
 for arch in $docarch; do
  CreateDocArchive $arch
 done
fi

if [ -n "$srcarch" ]; then
 for arch in $srcarch; do
  CreateSrcArchive $arch
 done
fi

if [ -n "$sdkarch" ]; then
 for arch in $sdkarch; do
  CreateSdkArchive $arch
 done
fi 

if [ -n "$archname" ]; then
 echo ""
 echo "The following archives were generated in"
 echo "$DSTDIR:"
 for arch in $archname; do
  size=`wc -c $DSTDIR/$arch | awk '{ SZ=$1/1024; SF="%d KB"; \
    if (SZ > 1024) { SZ=SZ/1024; SF="%.2f MB" } \
    printf(SF" (%7s)\n", SZ, $1 ) }'`
  sum=`sum $DSTDIR/$arch | cut -d" " -f1`
  printf -- "- %-43s %18s sum=%s\n" "$arch" "$size" "$sum"
 done
fi

