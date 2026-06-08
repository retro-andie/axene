#!/bin/sh
#
# gperfpatch.sh for Xclamation, XQuad and XAllWrite in FontEncoding/
# Patch for generated C FontEncoding sources by gperf
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
# Started on  Tue Jun 21 16:31:59 1994 Stéphane Boisson
# Last update Fri Oct 25 16:57:57 1996 Emmanuel Paris
#

cat > script.sed <<EOF
s/static struct/struct/g
s/wordlist/hashlist_$2/g
s/hash_table\[str/hash_table[(int)str/g
s/struct encoding_name_s/encoding_name_t/g
EOF
sed -f script.sed $1 | grep -v "encoding_name_t {" > $2.c 
rm -f script.sed $1


