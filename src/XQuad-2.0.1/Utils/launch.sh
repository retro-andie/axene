#!/bin/sh
# 
# XQuad
# XQuad script
# 
# Copyright (c)1996-2000. All Rights reserved Axene.
# EMail: <xcalibur@axene.org>
# 
# Started on  Fri Nov 17 17:34:10 1995 Stéphane Boisson
# Last update Sat Apr  8 16:55:20 2000 Emmanuel Paris
#

#--- Init global variables
SkipLine() { echo " "; }
Fatal() { echo "${progname}: error: ${1}" 1>&2 ; exit 1; }

this="$0"
progname=`basename $this`
bin_list=""
name_base=XQuad

# ---------------------------------------------------------------------- # 
# AskOneFromList - Let the user choose an item among a list              # 
#                  <text> <default-index> <var > <choice-1>...<choice-n> #
# ---------------------------------------------------------------------- #
AskOneFromList()
{
  _ask_text=$1
  _ask_default=$2
  _ask_var=$3
  shift 3
  _arg_all=$@
  _arg_num=$#

  while :; do
    if [ "$force_flag" = "yes" ]; then
      return $_ask_default
    else
      echo $_ask_text
      _ask_count=0
      while [ $_ask_count -lt $_arg_num ]; do
        if [ $_ask_count -eq $_ask_default ]; then
          _ask_reply="(DEFAULT)"
        else
          _ask_reply=
        fi
        _ask_count=`expr $_ask_count + 1`
        eval "echo \"  \$_ask_count. \$1 $_ask_reply\""
	shift 1
      done
      read _ask_reply
      if [ -z "$_ask_reply" ]; then
	_arg_cnt=0
        for _ask_phrase in $_arg_all; do
         if [ $_arg_cnt -eq $_ask_default ]; then
          eval "$_ask_var=\"\$_ask_phrase\""
          break
        fi
        _arg_cnt=`expr $_arg_cnt + 1`
        done        
	return $_ask_default
      fi
      if [ $_ask_reply -gt 0 -a $_ask_reply -le $_arg_num ]; then
        SkipLine
	_arg_cnt=1
        for _ask_phrase in $_arg_all; do
         if [ $_arg_cnt -eq $_ask_reply ]; then
          eval "$_ask_var=\"\$_ask_phrase\""
          break
         fi
         _arg_cnt=`expr $_arg_cnt + 1`
        done
        return `expr $_ask_reply - 1`
      fi
      echo "ERROR: You must enter a number between 1 and $_arg_num"
      exit 1
    fi
  done
} 

# ---------------------------------------------------------------------- # 
# ---------------------------------------------------------------------- #
TestInHostList()
{
 _Host=`hostname`
 _HostsDB=$ZHOME/lib/locale/axene.hosts

 _count_exec=0
 for _file in $ZHOME/bin/${name_base}*; do
  if [ -x $_file ]; then
   _count_exec=`expr $_count_exec + 1`
  fi
 done

 if [ "$_count_exec" -eq "0" ]; then
   Fatal "No executable found in $ZHOME/bin/"
 fi

 if [ "$_count_exec" -eq "1" ]; then
  bin_list=$ZHOME/bin/${name_base}*
 else
  _No_Host=0
  _Bin_ok=0
  test -r $_HostsDB || _No_Host=1
  if [ "$_No_Host" -eq "0" ]; then
   exec 5<$_HostsDB
   while read _v_host <&5 ; do
     if [ "$_v_host" = "$_Host" ]; then
      read bin_list <&5
      bin_list=$ZHOME/bin/${name_base}.${bin_list}
      _Bin_ok=1
      while read _v_host <&5 ; do
      echo flush > /dev/null
      done
     else
      read _v_burk <&5
      read _v_burk <&5
     fi
   done
   exec 5<&-
  else
   echo "No $_HostsDB defined."
   if [ -w $ZHOME/lib/locale/ ]; then
    touch $_HostsDB
   else
    echo "Not authorized to write into directory $ZHOME/lib/locale/"
   fi
  fi
 	
  if [ "$_Bin_ok" -eq "0" ]; then
   AskOneFromList "Please select the program to execute on $_Host" 0 bin_list $ZHOME/bin/${name_base}*
   if [ ! -w $_HostsDB ]; then
    echo "Could not add this entry to $_HostsDB"
   else
    echo "host \`$_Host' is now associated with program :"
    echo "\`$bin_list'."
    echo "(if error, edit or delete $_HostsDB)" 
    echo $_Host >> $_HostsDB
    echo $bin_list | cut -d"." -f2- >> $_HostsDB
    echo ----- >> $_HostsDB
   fi
  fi
 fi
}

PWD=`pwd`
ZHOME=`dirname $PWD/$this`
AXENE_LIB=$ZHOME/lib
XQUAD_LIB=$ZHOME/lib
XAPPLRESDIR=$ZHOME/app-defaults
export AXENE_LIB XQUAD_LIB XAPPLRESDIR

TestInHostList
exec $bin_list $@

