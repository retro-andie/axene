#!/bin/sh
#
# echonl.sh for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
# 	AxeneOffice in Utils/
# portable echo with non newline script
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
# Started on  Sometime during     1994 One of the authors
# Last update Thu May 22 19:49:00 1997 One of the authors
#

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



$ECHONL1"$1$ECHONL2"

