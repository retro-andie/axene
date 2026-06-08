#
# Makefile for XInstall in Gzip/
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
# Started on  Tue Oct  4 11:11:32 1994 Stéphane Boisson
# Last update Wed Feb  4 02:48:48 1998 Emmanuel Paris
#

OBJ=  mainzip.o util.o unzip.o unlzw.o unlzh.o unpack.o inflate.o
LIB= ../lib/gzip.a
SHELL=	/bin/sh

all:	showenv $(LIB)

$(LIB): $(OBJ)
	@echo "LIB $(LIB) Archiving: $(OBJ)"
	@$(AR) $(ARFLAGS) $@ $(OBJ) > /dev/null
	@echo "LIBRARY $(LIB) built."

.c.o:
	@../Utils/echonl.sh "LIB $(LIB) Compile: "$@" "
	@$(CC) $(CPPFLAGS) $(CFLAGS) -I../Headers -c $<
	@echo "done."

main:	main.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ main.o $(LIB)

clean:
	rm -f $(OBJ) $(LIB) *.o *~ *.bak main core

showenv:
	@echo -------------- LIB Compilation options -------------------------------------
	@echo "setenv CFLAGS '$(CFLAGS)'"
	@echo "setenv CPPFLAGS '$(CPPFLAGS)'"
	@echo "setenv ARFLAGS '$(ARFLAGS)'"
	@echo ----------------------------------------------------------------------------

# ---------------------------------------------------------------------
# Make depend rule

depend:
	cp Makefile Makefile-depend
	gcc -MM -I../Headers $(SRC) >> Makefile-depend

# ---------------------------------------------------------------------
# Installationd rule




