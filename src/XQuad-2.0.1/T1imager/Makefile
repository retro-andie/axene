#
# Makefile for Xclamation, XQuad and XAllWrite in T1imager/
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
# Started on  Wed Oct 19 17:04:11 1994 Stéphane Boisson
# Last update Wed Feb  4 01:35:09 1998 Emmanuel Paris
#

LIB=	../lib/T1imager.a
SHELL=	/bin/sh

SRC=	t1_arith.c t1_lines.c t1_regions.c t1_t1malloc.c t1_token.c \
	t1_curves.c t1_scanfont.c t1_t1snap.c t1_type1.c t1_fontfcn.c \
	t1_objects.c t1_spaces.c t1_t1stub.c t1_util.c t1_hints.c \
	t1_paths.c t1_t1io.c

OBJ=	t1_arith.o t1_lines.o t1_regions.o t1_t1malloc.o t1_token.o \
	t1_curves.o t1_scanfont.o t1_t1snap.o t1_type1.o t1_fontfcn.o \
	t1_objects.o t1_spaces.o t1_t1stub.o t1_util.o t1_hints.o \
	t1_paths.o t1_t1io.o

all:	showenv $(LIB)

.c.o:
	@../Utils/echonl.sh "LIB $(LIB) Compile: "$@" "
	@$(CC) $(CPPFLAGS) $(CFLAGS) -I. -I../Headers -c $<
	@echo "done."

$(LIB):	$(OBJ)
	@echo "LIB $(LIB) Archiving: $(OBJ)"
	@$(AR) $(ARFLAGS) $@ $(OBJ) > /dev/null
	@echo "LIBRARY $(LIB) built."

clean:
	rm -f *.a *.o *~ $(LIB)

showenv:
	@echo -------------- LIB Compilation options -------------------------------------
	@echo "setenv CFLAGS '$(CFLAGS)'"
	@echo "setenv CPPFLAGS '$(CPPFLAGS)'"
	@echo "setenv ARFLAGS '$(ARFLAGS)'"
	@echo ----------------------------------------------------------------------------

depend: 
	cp Makefile Makefile-depend
	$(CC) -MM -I../Headers $(SRC) >> Makefile-depend



t1_arith.o: t1_arith.c t1_port.h t1_objects.h t1_spaces.h t1_arith.h
t1_lines.o: t1_lines.c t1_port.h t1_objects.h t1_spaces.h t1_regions.h \
 t1_lines.h
t1_regions.o: t1_regions.c t1_port.h t1_objects.h t1_spaces.h \
 t1_regions.h t1_paths.h t1_curves.h t1_lines.h t1_pictures.h \
 t1_fonts.h t1_hints.h t1_strokes.h
t1_t1malloc.o: t1_t1malloc.c t1_port.h t1_objects.h
t1_token.o: t1_token.c t1_port.h t1_t1stdio.h t1_util.h t1_digit.h \
 t1_token.h t1_tokst.h t1_hdigit.h
t1_curves.o: t1_curves.c t1_port.h t1_objects.h t1_spaces.h t1_paths.h \
 t1_regions.h t1_curves.h t1_lines.h t1_arith.h
t1_scanfont.o: t1_scanfont.c t1_port.h t1_t1stdio.h t1_util.h \
 t1_token.h t1_fontfcn.h t1_blues.h
t1_t1snap.o: t1_t1snap.c t1_port.h t1_objects.h t1_spaces.h t1_paths.h
t1_type1.o: t1_type1.c t1_port.h t1_objects.h t1_spaces.h t1_paths.h \
 t1_fonts.h t1_pictures.h t1_util.h t1_blues.h
t1_fontfcn.o: t1_fontfcn.c t1_t1imager.h t1_fontmisc.h \
 ../Headers/MemoryManager.h ../Headers/xcalibur.h \
 ../Headers/ErrorManager.h ../Headers/ObjStd.h \
 ../Headers/global_defs.h ../Headers/error_defs.h ../Headers/misc.h \
 ../Headers/units.h t1_port.h t1_util.h t1_fontfcn.h
t1_objects.o: t1_objects.c t1_port.h t1_objects.h t1_spaces.h \
 t1_paths.h t1_regions.h t1_fonts.h t1_pictures.h t1_strokes.h \
 t1_cluts.h
t1_spaces.o: t1_spaces.c t1_port.h t1_objects.h t1_spaces.h t1_paths.h \
 t1_pictures.h t1_fonts.h t1_arith.h t1_trig.h
t1_t1stub.o: t1_t1stub.c t1_port.h t1_objects.h
t1_util.o: t1_util.c t1_fontmisc.h ../Headers/MemoryManager.h \
 ../Headers/xcalibur.h ../Headers/ErrorManager.h ../Headers/ObjStd.h \
 ../Headers/global_defs.h ../Headers/error_defs.h ../Headers/misc.h \
 ../Headers/units.h t1_port.h t1_util.h
t1_hints.o: t1_hints.c t1_port.h t1_objects.h t1_spaces.h t1_paths.h \
 t1_regions.h t1_hints.h
t1_paths.o: t1_paths.c t1_port.h t1_objects.h t1_spaces.h t1_paths.h \
 t1_regions.h t1_fonts.h t1_pictures.h t1_strokes.h t1_trig.h
t1_t1io.o: t1_t1io.c t1_port.h t1_t1stdio.h t1_t1hdigit.h
