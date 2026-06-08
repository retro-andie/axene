#
# Makefile for XQuad in VectorGraph/
# Makefile for VectorGraph class
#
# Copyright (C) 1995-2000 Axene.
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
# Started on  Mon Jan 15 13:36:51 1996 Stéphane Boisson
# Last update Wed Jan 28 15:45:18 1998 Robin Castanier
#

LIB=	../lib/VectorGraph.a
OBJ=	VectorGraph.o commands.o dispatch.o docfile.o path.o print.o \
	redraw.o load.o Move_Vector.o
SHELL=	/bin/sh

DEPEND=	VectorGraph.h VectorGraphP.h 

$(LIB):	$(OBJ)
	@echo "LIB $(LIB) Archiving: $(OBJ)"
	@$(AR) $(ARFLAGS) $@ $(OBJ) > /dev/null
	@echo "LIBRARY $(LIB) built."

.c.o:
	@../Utils/echonl.sh "LIB $(LIB) Compile: "$@" "
	@$(CC) $(CPPFLAGS) $(CFLAGS) -I. -I../Headers -c $< -o $@
	@echo "done."

showenv:
	@echo -------------- LIB Compilation options -------------------------------------
	@echo "setenv CFLAGS '$(CFLAGS)'"
	@echo "setenv CPPFLAGS '$(CPPFLAGS)'"
	@echo "setenv ARFLAGS '$(ARFLAGS)'"
	@echo ----------------------------------------------------------------------------

clean:
	rm -f $(LIB) $(OBJ) gato *~

VectorGraph.c:	VectorGraph.h VectorGraphP.h ../Headers/CadreStd.h
commands.c:	VectorGraph.h VectorGraphP.h
dispatch.c:	VectorGraph.h VectorGraphP.h
docfile.c:	VectorGraph.h VectorGraphP.h ../Headers/DocFile.h
path.c:		VectorGraph.h VectorGraphP.h
print.c:	VectorGraph.h VectorGraphP.h ../Headers/PostScript.h
redraw.c:	VectorGraph.h VectorGraphP.h
Move_Vector.c:	VectorGraph.h VectorGraphP.h
