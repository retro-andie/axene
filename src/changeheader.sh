#!/bin/sh
#
# changeheader.sh for changeheader.sh in Axene-2.0.1/
# Change all source's header for including GPL.
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
# Started on  Tue Jun  1 12:11:22 1999 Emmanuel Paris
# Last update Mon Jun 12 20:09:04 2000 Emmanuel Paris
#

prglist="Xclamation* XQuad* XAllWrite* XMayday* XInstall* AxeneOffice*"
prgabrv="C Q A M I O"
prgcopy="1994 1995 1997 1996 1997 1998"
commonfile="./CommonObjects"

prgdir="Sources"
docdir="Documentation"

tmpfile="___tmp"
srcfile="___src"
dstfile="___dst"

trap "test -f $tmpfile && rm -f $tmpfile; \
	test -f $srcfile && rm -f $srcfile; \
	test -f $dstfile && rm -f $dstfile" 0

###############################################################################
# split_line string nbc char: split <string> in line of max <nbc> char and
#	place char <char> as line separator
 
split_line()
{
 local string="$1"
 local nbc=${2:-80}
 local char=${3:-"|"}

 local strlen=`expr length "$string"`
 if [ $strlen -gt $nbc ]; then
  str="$string"
  
  local svidx="1"
  idx=`expr index "$str" " "`
  while [ `expr $svidx + $idx` -lt $nbc -a $idx -ne 0 ]; do

   svidx=`expr $svidx + $idx`
   idx=`expr $idx + 1`
   str=`expr substr "$str" $idx $strlen`
   idx=`expr index "$str" " "`
  done

  if [ $svidx -eq 1 ]; then
   tmpstr=`expr substr "$string" $nbc $strlen`
   tmpstr=`split_line "$tmpstr" $nbc "$char"`
   nbc=`expr $nbc - 1`
   str=`expr substr "$string" 1 $nbc`"$char""$tmpstr"
   echo $str
   return
  fi
  
  tmpstr=`expr substr "$string" $svidx $strlen`
  tmpstr=`split_line "$tmpstr" $nbc "$char"`
  svidx=`expr $svidx - 1`
  str=`expr substr "$string" 1 $svidx`"$char""$tmpstr"
  echo $str  
  return
 else
  echo "$string"
  return
 fi
}

###############################################################################
# print_usage: print this shell script usage
#

print_usage()
{
 prgname=`basename $0 .sh`

 cat << EOF
Usage: $prgname [Argument] ...
Change files header to a pretty standard format. 

 $prgname			- change headers in all Axene projects 
				  directory trees.

 $prgname dir_tree		- change headers in a single Axene projects 
				  directory tree.

 $prgname file1 file2 ...	- change headers for <file1> <file2> ...

EOF
}

###############################################################################
# print_copyright date: print copyright replacing copyright start date with
#			<date>

print_copyright()
{
 now=`date +%Y`
 date=${1:-$now}
 
 if [ "$sdate" != "$now" ]; then
  date=$date"-"$now
 fi

 cat << EOF
Copyright (C) $date Axene.
Authors: Stéphane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
Email: xcalibur@axene.org

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
EOF

}

###############################################################################
# cahnge_header files... : change header for files <files...>
#

change_header()
{
 local files="$@"
 local nb_fls=$#

 printf "+---------+--------------------------------+----------+-----------------------+\n"
 printf "|From/ To | %-30s | %-8s | %-21s |\n" "File Name" "Type" "Comment"
 printf "+---------+--------------------------------+----------+-----------------------+\n"
 countfile=0
 for f in $files; do
  countfile=`expr $countfile + 1`

  if [ `expr index "$f" "/"` = "0" ]; then
   shortf=`echo $PWD/$f | sed "s:.*/\(.*/.*\)$:\1:g"`
  else
   shortf=`echo $f | sed "s:.*/\(.*/.*\)$:\1:g"`
  fi
 
  if [ ${#shortf} -gt 30 ]; then
   shortsfx=`basename $shortf | sed "s:^.*\(\..*\)$:\1:"`
   if [ "v$shortfx" = "v$shortf" ]; then
    tshortf=`echo $shortf | cut -b-30`
   else
    tsize=`expr 26 - ${#shortsfx}`
    tshortf=`echo $shortf | cut -b-$tsize`"[..]"$shortsfx
   fi
  else
   tshortf=$shortf
  fi

  printf "\r| %03d/%03d | %-30s | " $countfile $nb_fls $tshortf

  if [ ! -f $f ]; then
   filetype="z"
  elif `echo $f | egrep -q "\.c$"`; then
   filetype="c"
  elif `echo $f | egrep -q "\.h$"`; then
   filetype="h"
  elif `echo $f | egrep -q "\.sh$"`; then
   filetype="s"
  elif `echo $f | egrep -q "\.html?$"`; then
   filetype="d"
  elif `echo $f | egrep -q "Makefile.*"`; then
   filetype="m"
  elif `head -1 $f | egrep -q "# *! */.*sh"`; then
   filetype="s"
  else
   filetype="u"
  fi

  case $filetype in
   "z") printf "%-8s | " "Missing";;
   "c") printf "%-8s | " "C source";;
   "h") printf "%-8s | " "C header";;
   "s") printf "%-8s | " "Shell";;
   "m") printf "%-8s | " "Makefile";;
   "d") printf "%-8s | " "HTML";;
   "u") printf "%-8s | " "Unknow";;
  esac

# compute header's project and copyright fields
 
  dftcopyright=${dftcopyright:-`date +%Y`}
  abrev=${abrev:-X}

  copyright=`egrep "[Ss]tarted on  .*[12][90][1-9][1-9]" $f | head -1 | \
	sed "s:^.* \([12][90][1-9][1-9]\) .*$:\1:"`
  test $dftcopyright -lt ${copyright:-3000} && copyright="$dftcopyright"

  project=""
  if [ -f $commonfile ]; then
   sf=`echo $shortf | sed "s:Sources/\(.*\):\1:"`
   is_common=`egrep "^[A-Z.]*${abrev}[A-Z.]* $sf[ #]*$" $commonfile`
   if [ ${#is_common} -ne 0 ]; then
    common=`echo $is_common | cut -f1 -d" "`
    count="0"
    for k in $prgabrv; do
     if `echo $common | grep -q $k`; then
      set -- $prglist
      shift $count
      project="$project, "`echo $1 | sed "s:^\([^-]*\)-.*:\1:"`
      set -- $prgcopy
      shift $count
      test $1 -lt $copyright && copyright="$1"
     fi
     count=`expr $count + 1`
    done
    project=`echo $project | sed "s/, \(..*\), \(.[^ ,]*\)/\1 and \2/"`
   fi
  fi

  if [ ${#project} -eq 0 ]; then
    project=`echo $f | sed "s:^\([AX][A-Za-z]*\)-.*$:\1:"`
    project=${project:-Axene}
  fi


  location=`basename $shortf`" for $project in "`dirname $shortf`"/"
  location=`split_line "$location" 76 "<>"`

  test -f $tmpfile && rm -f $tmpfile
  test -f $srcfile  && rm -f $srcfile
  test -f $dstfile  && rm -f $dstfile
  
  if [ $filetype = "z" ]; then
   printf "%-21s |\n" "File doesn't exists."
   continue
  fi

  if [ $filetype = "u" ]; then
   printf "%-21s |\n" "Skip file."
   continue
  fi

  if [ $filetype = "c" -o $filetype = "h" ]; then
   separator="/* ** */"

   set -- `cat $f | awk '
     BEGIN	{ first=0 }	
     /^ *\/\*/	{ if (first == 0) { print FNR; first = 1 } }
     /\*\/ *$/	{ if (first == 1) { first = 2 } }
     / *\*\//	{ if (first == 2) { print FNR; exit } }
     /^$/	{ if (first == 2) { print FNR-1; exit } }'`

   firstl=${1:-0}
   lastl=${2:-0}
   deltal=`expr $lastl - $firstl`

   if [ $firstl -gt 2 -o $lastl -lt 3 -o $deltal -le 2 ]; then
    printf "%-21s |\n" "Truncated header."
    continue
   fi

    #   echo Header of $f from line $firstl to $lastl > /dev/console

   head -$lastl $f | tail +$firstl > $srcfile
    #   cat $srcfile > /dev/console

    #   printf "\n--------------------------------------------------------\n" \
    #    > /dev/console

   format=`awk '
	BEGIN		  { mfor = 0; new = 0; old = 0 }
	/.* for .* in .*/ { new++; next }
        /.* for.*/	  { if (mfor == 0) mfor = FNR }
	/[^a-z]*in .*/	  { if (mfor == FNR - 1) { mfor = -1; new++ } }
	/[Ss]tarted on /  { new++ }
	/[Ll]ast update / { new++ }
	/[Cc]opyright .* (Xcalibur|Axene)/ { old++ }
 	END	{ if (new != 0) print "1"; else 
		    { if (old != 0) print "2"; else print "0" } }
	' $srcfile`
  
   case $format in
    "0")
 	printf "%-21s |\n" "Unrecognized format."
	continue;;
    "1")
     echo $location | tr "<>" "\n\t" > $tmpfile
     awk -v location=`dirname $shortf | sed 's/^\(.*\)s$/\1/'` '
      BEGIN		{ step = 0; mfor = 0 }
      $0 ~ location"s?/" { if (step == 0 || mfor = FNR - 1) {step = 1; next } }
      /.* for .*/	{ if (step == 0 && ($0 ~ location)) 
			{step = 1; mfor = FNR; next } }
      /*.*[A-Za-z].*/	{ if (step == 1) 
			    print gensub("^[ *]*(.*)$", "\\1", "1"); }
      /^Copyright ([cC])/	{ step = 2 }
      /^[* ]+$/		{ if (step == 0) next; else step = 2 }
      ' $srcfile >> $tmpfile
     
     printf "%-21s |" "New format.";;
    "2")
     echo $location | tr "<>" "\n\t" > $tmpfile
     awk -v location=`basename $shortf` '
      BEGIN		{ step = 0 }
      $0 ~ location	{ if (step == 0) { 
		     print gensub("^.* - *(.*)$", "\\1", "1");
		     step = 1; next; } }
      /*.*[A-Za-z].*/	{ if (step == 1) 
			    print gensub("^[ *]*(.*)$", "\\1", "1"); }
      /^[* ]+$/		{ if (step == 0) next; else step = 2 }
      ' $srcfile >> $tmpfile

     printf "%-21s |" "Old format.";;
   esac

   echo "" >> $tmpfile
  fi

  if [ $filetype = "m" -o $filetype = "s" ]; then
   separator="# # #"

   set -- `cat $f | awk '
     BEGIN	{ first=0 }	
     /^ *#/	{ if (first == 0) { print FNR; first = 1 } }
     /^$/	{ if (first == 1) { print FNR-1; exit } }
     /^[^ #].*$/	{ if (first == 1) { print FNR-1; exit } }'`

   firstl=${1:-0}
   lastl=${2:-0}
   deltal=`expr $lastl - $firstl`

   if [ $firstl -gt 2 -o $lastl -lt 3 -o $deltal -le 2 ]; then
    printf "%-21s |\n" "Truncated header."
    continue
   fi

    #   echo Header of $f from line $firstl to $lastl > /dev/console

   head -$lastl $f | tail +$firstl > $srcfile
    #   cat $srcfile > /dev/console

    #   printf "\n--------------------------------------------------------\n" \
    #    > /dev/console

   format=`awk '
	BEGIN		  { mfor = 0; new = 0; old = 0 }
	/.* for [XA].* in .*/ { new++; next }
        /.* for [XA].*/	  { if (mfor == 0) mfor = FNR }
	/[^a-z]*in .*/	  { if (mfor == FNR - 1) { mfor = -1; new++ } }
	/^# *Copyright /  { new++ }
	/^# *[^ ].* - *[^ ].*/ { old++ }
 	END	{ if (new != 0) print "1"; else 
		    { if (old != 0) print "2"; else print "0" } }
	' $srcfile`
  
   case $format in
    "0")
 	printf "%-21s |\n" "Unrecognized format."
	continue;;
    "1")
     echo $location | tr "<>" "\n\t" > $tmpfile
     awk -v location=`dirname $shortf | sed 's/^\(.*\)s$/\1/'` ' 
      BEGIN		{ mfor=0; step = 0; neednl = 1 }
      /.* for .* in .*/ { mfor = FNR; next }
      /.* for [XA].*/	  { mfor = FNR; next }
      $0 ~ locations"s?/" { if (mfor == FNR - 1) next }
      /^# *Copyright/	{ if (neednl == 1) printf "\n"; exit }
      /^# *! *\/.*/	{ next }
      /^# *$/		{ if (step == 1) { print ""; neednl = 0 }; next }
      / *#.*/		{ step = 1; neednl = 1; 
			    print gensub("^ *# ?(.*)$", "\\1", "1"); }
      ' $srcfile >> $tmpfile
     
     printf "%-21s |" "New format.";;
   "2")
     echo $location | tr "<>" "\n\t" > $tmpfile
     awk -v location=`basename $shortf .sh` '
      BEGIN		{ step = 0 }
      $0 ~ location	{ if (step == 0) { 
		     print gensub("^.* - *(.*)$", "\\1", "1");
		     step = 1; next; } }
      / *#.*[A-Za-z].*/	{ if (step == 1) 
			    print gensub("^ *# ?(.*)$", "\\1", "1"); }
      /^[* ]+$/		{ if (step == 0) next; else step = 2 }
      ' $srcfile >> $tmpfile

     printf "%-21s |" "Old format."

     echo "" >> $tmpfile;;
   esac

  fi

  if [ "$filetype" = "d" ]; then
    headsize=`grep -in "<html>" $f | cut -d: -f1`
    if [ -z "$headsize" ]; then
      headsize=0
      printf "%-21s |" "Not HTML."
    else
     case "$headsize" in
     1)
      printf "%-21s |" "Truncated."
     ;;
     2)
      printf "%-21s |" "Good size."
     ;;
     *)
      printf "%-21s |" "Wrong size."
     ;;  
    esac
   fi

   firstl=1
   lastl=`expr $headsize - 1`
   head -$lastl $f | tail +$firstl > $srcfile
   echo "<!-- Documentation $project (c)$dftcopyright-`date +%Y` AXENE contact axene@axene.org>" > $tmpfile
  fi

  test -f $tmpfile || continue

  if [ "$filetype" = "d" ]; then
   cp $tmpfile $dstfile
  else
   print_copyright $copyright >> $tmpfile
   echo "" >> $tmpfile

   lastm=`stat $f | grep "Modify:" | sed "s/.*: \(.*\)(.*/\1/"`
   awk -v syear="$copyright" -v lastm="$lastm" '
    BEGIN	{ start = ""; end = "" }
    /[Ss]tarted on /	{ start = gensub("^[ *]*.* on *(.*)$", "\\1", "1"); }
    /[Ll]ast update /	{ end = gensub("^[ *]*.* update *(.*)$", "\\1", "1"); }
    END	{ if (start != "") printf "Started on  %s\n", start; else 
 	printf "Started on  %-19s %4d One of the authors\n", 
 	"Sometime during", syear;
 	if (end != "") printf "Last update %s\n", end; else
 	printf "Last update %s One of the authors\n", lastm; }
    ' $srcfile >> $tmpfile

   if [ $filetype = "s" ]; then
    head -1 $srcfile | sed 's/ \(.*sh\)/\1/' > $dstfile
   else
    touch $dstfile
   fi

   awk -v beg=`echo "$separator" | cut -f1 -d" "` \ 
       -v mid=`echo "$separator" | cut -f2 -d" "` \
       -v end=`echo "$separator" | cut -f3 -d" "`  '
    BEGIN	{ name=""; print beg }
    /^$/		{ print mid; next }
    /^[SL].* [12][09][0-9][0-9] *[Nn]ucleus/	{ name="Stéphane Boisson" }
    /^[SL].* [12][09][0-9][0-9] *[Gg]udul/	{ name="Antoine Buat" }
    /^[SL].* [12][09][0-9][0-9] *[Dd]ewe[Yy]/	{ name="Robin Castanier" }
    /^[SL].* [12][09][0-9][0-9] *[Xx]ibios ?2?/	{ name="Emmanuel Paris" }
    /^[SL].* [12][09][0-9][0-9] *[Ii][Ww][Yy]/   { name="Marc Blet" }
    /./		{ if (name != "") {  printf "%s %s %s\n", mid, 
		   gensub("^(.* [12][09][0-9][0-9]) *.*",
		   "\\1", "1"), name ; name="" }
		  else printf "%s %s\n", mid, $0 }
    END		{ print end }
   ' $tmpfile >> $dstfile
  fi
    #  cat $dstfile > /dev/console 

#  differ=""
  differ=`diff -q $srcfile $dstfile`
  if [ ${#differ} = "0" ]; then
   printf "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
   printf "%-21s |" "No change made."
  else
   fmode=`stat $f | grep "Mode:" | sed "s;.*Mode: (\(....\)/.*;\1;1"`

   mv -f $f $tmpfile
   cat $dstfile > $f
   tail +`expr $lastl + 1` $tmpfile >> $f
   chmod $fmode $f

   printf "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
   printf "%-21s |\n" "Ok changed."
  fi
  
 done

 printf "\r+---------+--------------------------------+----------+-----------------------+\n"
}

###############################################################################
# MAIN FUNCTION

if [ $# -eq 2 ]; then
 if [ $1 = "-ts" -a -f $2 ]; then
  file $2 | egrep -q '[Ss]hell *[Ss]cript' && echo $2
  exit
 fi
fi

dirlist="$prglist"

if [ $# -eq 1 ]; then
 testd=`echo $1 | sed "s:\(.*\)/$:\1:"`
 if [ -d $testd ] && [ -d $testd/$prgdir -o -d $testd/$docdir ]; then
  dirlist=$testd 
 fi
fi

if [ $# -gt 0 ]; then
 if [ -f $1 ]; then

  prj=`echo $1 | sed "s:^\([AX][A-Za-z]*\)-.*$:\1:"`
  if [ -n "$prj" ]; then
   count=0
   args="$@"
   set -- $prglist
   while `echo $1 | grep -q "$prj"`; do 
    shift
    count=`expr $count + 1`
   done
    
   set -- $prgabrv; shift $count; abrev=$1
   set -- $prgcopy; shift $count; dftcopyright=$1
   set -- $args
  fi
  change_header $@
  exit
 elif [ ! -d $1 ]; then
  print_usage
  exit
 fi
fi


for i in $dirlist; do
 echo -n "Processing: $i "

 count=0
 set -- $prglist
 while [ $1 != $i ]; do 
  shift
  count=`expr $count + 1`
 done

 set -- $prgabrv; shift $count; abrev=$1
 set -- $prgcopy; shift $count; dftcopyright=$1

 if [ -d $i/$prgdir ]; then
  echo -n "."

  cfiles=`find $i/$prgdir -type f -name "*.[ch]" -print | grep -v "/Headers" | \
     grep -v T1imager | grep -v JpegImport | grep -v PngImport | \
     grep -v TiffImport | grep -v RH_vroot.h | grep -v RH_netscape.c | \
     grep -v PARI_src | grep -v Ufc | grep -v fnmatch`
 
  echo -n "."
  mfiles=`find $i/$prgdir -type f -name "Makefile*" -print`
  echo -n "."
  sfiles=`find $i/$prgdir -type f -name "*[^.]?"  -name "*[^.][^z]??" \
     -exec ./$0 -ts {} \;`
 else
  cfiles=""
  mfiles=""
  sfiles=""
 fi

 if [ -d $i/$docdir ]; then
  echo -n "+"
 
  dfiles=`find  $i/$docdir -type f -name "*.htm*"`
 else
  dfiles=""
 fi

 nb_src=`echo $cfiles | tr " " "\n" | egrep -c "\.c$"`
 nb_hdr=`echo $cfiles | tr " " "\n" | egrep -c "\.h$"`
 nb_mkf=`echo $mfiles | tr " " "\n" | egrep -c "."`
 nb_scr=`echo $sfiles | tr " " "\n" | egrep -c "."`
 nb_htm=`echo $dfiles | tr " " "\n" | egrep -c "."`
 nb_fls=`expr $nb_src + $nb_hdr + $nb_mkf + $nb_scr + $nb_htm`

 if [ $nb_fls -eq 0 ]; then
  echo "Skip. No suitable files detected."
  continue
 fi

 echo "ok"

 split_line "Nb files: $nb_fls ($nb_mkf Makefiles, $nb_scr shell scripts, $nb_src .c, $nb_hdr .h and $nb_htm .html)." \
    80 "%" | tr "%" "\n"

# files="$mfiles $sfiles $cfiles $dfiles"
 files="$dfiles"

 change_header $files
done







