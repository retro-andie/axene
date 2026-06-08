#
# Makefile for Xclamation, XQuad, XAllWrite, XMayday and AxeneOffice in 
# 	Managers/
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
# Last update Sat Jun  5 15:38:49 1999 Emmanuel Paris
#


OBJ=	memdebug.o memnormal.o memspool.o memstack.o ErrorManager.o
LIB=	../lib/Managers.a
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


gato:	$(LIB) minimain.o
	$(CC) -o $@ minimain.o $(LIB)

minimain.o: minimain.c MemoryManager.h ErrorManager.h


clean:
	@rm -f $(LIB) core *.o *.a gato *~

showenv:
	@echo -------------- LIB Compilation options -------------------------------------
	@echo "CFLAGS=\""'$(CFLAGS)'"\""
	@echo "CPPFLAGS=\""'$(CPPFLAGS)'"\""
	@echo "ARFLAGS=\""'$(ARFLAGS)'"\""
	@echo "export CFLAGS CPPFLAGS ARFLAGS"
	@echo ----------------------------------------------------------------------------

# ---------------------------------------------------------------------
# Dependencies rules

$(OBJ): MemoryManager.h ErrorManager.h MemoryManagerP.h
