#
# Makefile for XQuad in PARI_src/
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
# Started on  Sat Jun 11 20:39:54 1994 Stéphane Boisson
# Last update Mon Feb 16 17:35:33 1998 Emmanuel Paris
#

LIB=	../lib/Pari.a
SHELL=	/bin/sh

OBJ= mp.o anal.o alglin1.o alglin2.o base1.o base2.o base3.o arith1.o \
	arith2.o bibli1.o bibli2.o elliptic.o es.o gen1.o gen2.o gen3.o \
	init.o polarit1.o polarit2.o trans1.o trans2.o trans3.o version.o \
	gp_rl.o dummy.o

all:	showenv $(LIB)

.c.o:
	@../Utils/echonl.sh "LIB $(LIB) Compile: "$@" "
	@$(CC) $(CPPFLAGS) $(CFLAGS) -I. -I../Headers -c $<
	@echo "done."


$(LIB): $(OBJ)
	@echo "LIB $(LIB) Archiving: $(OBJ)"
	@$(AR) $(ARFLAGS) $@ $(OBJ) > /dev/null
	@echo "LIBRARY $(LIB) built."

clean:
	rm -f $(LIB) $(OBJ) *.o *.a gato *~

showenv:
	@echo -------------- LIB Compilation options -------------------------------------
	@echo "setenv CFLAGS '$(CFLAGS)'"
	@echo "setenv CPPFLAGS '$(CPPFLAGS)'"
	@echo "setenv ARFLAGS '$(ARFLAGS)'"
	@echo ----------------------------------------------------------------------------
