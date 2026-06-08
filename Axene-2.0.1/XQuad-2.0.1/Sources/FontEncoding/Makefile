#
# Makefile for Xclamation, XQuad and XAllWrite in FontEncoding/
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
# Started on  Sat Jun 11 20:47:14 1994 Stéphane Boisson
# Last update Mon Mar 16 19:17:38 1998 Emmanuel Paris
#

LIB=	../lib/FontEncoding.a
SHELL=	/bin/sh
SRC=	Latin1.c AdobeStd.c Dingbats.c ASCII.c Symbol.c
OBJ=	Latin1.o AdobeStd.o Dingbats.o ASCII.o Symbol.o

all:	showenv $(LIB)

$(LIB): $(OBJ)
	@echo "LIB $(LIB) Archiving: $(OBJ)"
	@$(AR) $(ARFLAGS) $@ $(OBJ) > /dev/null
	@echo "LIBRARY $(LIB) built."

.c.o:
	@../Utils/echonl.sh "LIB $(LIB) Compile: "$@" "
	@$(CC) $(CPPFLAGS) $(CFLAGS) -I. -I../Headers -c $<
	@echo "done."

Latin1.c: Latin1.gperf gperfpatch.sh
	gperf -G -p -g -o -t -N is_$*_char -k$$,2,1 $< > tmp.c
	sh -c './gperfpatch.sh tmp.c $*'

AdobeStd.c: AdobeStd.gperf gperfpatch.sh
	gperf -G -p -g -o -t -N is_$*_char -k$$,2,1,5 $< > tmp.c
	sh -c './gperfpatch.sh tmp.c $*'

Dingbats.c: Dingbats.gperf gperfpatch.sh
	gperf -G -p -g -o -t -N is_$*_char -k$$,2,3,4 $< > tmp.c
	sh -c './gperfpatch.sh tmp.c $*'

ASCII.c: ASCII.gperf gperfpatch.sh
	gperf -G -p -g -o -t -N is_$*_char -k$$,3 $< > tmp.c
	sh -c './gperfpatch.sh tmp.c $*'

Symbol.c: Symbol.gperf gperfpatch.sh
	gperf -G -p -g -o -t -N is_$*_char -k$$,1,2,4,5 $< > tmp.c
	sh -c './gperfpatch.sh tmp.c $*'

$(OBJ): $(SRC) FontEncoding.h

clean:
	rm -f $(LIB) *~ *.o *.a

showenv:
	@echo -------------- LIB Compilation options -------------------------------------
	@echo "setenv CFLAGS '$(CFLAGS)'"
	@echo "setenv CPPFLAGS '$(CPPFLAGS)'"
	@echo "setenv ARFLAGS '$(ARFLAGS)'"
	@echo ----------------------------------------------------------------------------
