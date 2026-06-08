#
# Makefile for Xclamation in TextFrame/
# Makefile for TextFrame class
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
# Started on  Fri Sep 23 12:33:43 1994 Stéphane Boisson
# Last update Sat Feb 14 18:12:52 1998 Emmanuel Paris
#

LIB=	../lib/TextFrame.a
OBJ=	TextFrame.o calibrate.o import.o print.o configure.o redraw.o 
SHELL=	/bin/sh

all:	showenv $(LIB)

$(LIB):	$(OBJ)
	@echo "LIB $(LIB) Archiving: $(OBJ)"
	@$(AR) $(ARFLAGS) $@ $(OBJ) > /dev/null
	@echo "LIBRARY $(LIB) built."

.c.o:
	@../Utils/echonl.sh "LIB $(LIB) Compile: "$@" "
	@$(CC) $(CPPFLAGS) $(CFLAGS) -I. -I../Headers -c $<
	@echo "done."

clean:
	rm -f $(LIB) $(OBJ) *~

showenv:
	@echo -------------- LIB Compilation options -------------------------------------
	@echo "setenv CFLAGS '$(CFLAGS)'"
	@echo "setenv CPPFLAGS '$(CPPFLAGS)'"
	@echo "setenv ARFLAGS '$(ARFLAGS)'"
	@echo ----------------------------------------------------------------------------

TextFrame.o: TextFrame.c TextFrameP.h TextFrame.h ../Headers/DocFile.h
calibrate.o: calibrate.c TextFrameP.h TextFrame.h
configure.o: configure.c TextFrameP.h TextFrame.h ../Headers/ScanLine.h 
import.o: import.c TextFrame.h ../Headers/DocFile.h
print.o : print.c TextFrameP.h TextFrame.h ../Headers/PostScript.h
redraw.o : redraw.c ../Headers/RasterFontP.h ../Headers/RasterFont.h