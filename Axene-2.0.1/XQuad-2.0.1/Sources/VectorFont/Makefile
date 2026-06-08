#
# Makefile for Xclamation, XQuad and XAllWrite in VectorFont/
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
# Last update Wed Feb  4 01:26:48 1998 Emmanuel Paris
#

LIB=	../lib/VectorFont.a
OBJ=	VectorFont.o dialog.o AFM_parse.o AFM_hash.o CharMetrics.o
SHELL=	/bin/sh

XcLIB=	../lib/VectorFont.a \
	../lib/FontEncoding.a \
	../lib/Managers.a

all:	showenv $(LIB)

$(LIB): $(OBJ)
	@echo "LIB $(LIB) Archiving: $(OBJ)"
	@$(AR) $(ARFLAGS) $@ $(OBJ) > /dev/null
	@echo "LIBRARY $(LIB) built."

.c.o:
	@../Utils/echonl.sh "LIB $(LIB) Compile: "$@" "
	@$(CC) $(CPPFLAGS) $(CFLAGS) -I. -I../Headers -c $<
	@echo "done."

gato:	minimain.o VectorFont.a
	$(CC) -o $@ minimain.o $(XcLIB) -lm

gperf:	AFM_hash.c

AFM_hash.c: AFM.gperf gperfpatch.sh
	gperf -G -p -g -o -t -N is_AFM_keyword -k$$,2,4,6,1 $< > tmp.c
	sh -c './gperfpatch.sh tmp.c' > AFM_hash.c

minimain.o: CharMetrics.h VectorFont.h

CharMetrics.o: CharMetrics.c CharMetrics.h

AFM_hash.o: AFM_hash.c AFM.h

AFM_parse.o: AFM_parse.c AFM.h VectorFont.h CharMetrics.h

VectorFont.o: VectorFont.c VectorFont.h CharMetrics.h AFM.h

dialog.o: VectorFont.h

showenv:
	@echo -------------- LIB Compilation options -------------------------------------
	@echo "setenv CFLAGS '$(CFLAGS)'"
	@echo "setenv CPPFLAGS '$(CPPFLAGS)'"
	@echo "setenv ARFLAGS '$(ARFLAGS)'"
	@echo ----------------------------------------------------------------------------

clean:
	rm -f $(LIB) *~ *.o gato

veryclean:
	$(MAKE) clean
	rm -f AFM_hash.c

AFM_hash.o : AFM_hash.c AFM.h ../Headers/xcalibur.h \
  ../Headers/ErrorManager.h ../Headers/MemoryManager.h ../Headers/ObjStd.h \
  ../Headers/global_defs.h ../Headers/error_defs.h ../Headers/misc.h \
  ../Headers/units.h VectorFont.h ../Headers/vectors.h CharMetrics.h 
AFM_parse.o : AFM_parse.c AFM.h ../Headers/xcalibur.h \
  ../Headers/ErrorManager.h ../Headers/MemoryManager.h ../Headers/ObjStd.h \
  ../Headers/global_defs.h ../Headers/error_defs.h ../Headers/misc.h \
  ../Headers/units.h VectorFont.h ../Headers/vectors.h CharMetrics.h \
  ../Headers/FontEncoding.h 
CharMetrics.o : CharMetrics.c CharMetrics.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h \
  ../Headers/global_defs.h ../Headers/error_defs.h ../Headers/misc.h ../Headers/units.h \
  ../Headers/vectors.h 
VectorFont.o : VectorFont.c VectorFont.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h \
  ../Headers/global_defs.h ../Headers/error_defs.h ../Headers/misc.h ../Headers/units.h \
  ../Headers/vectors.h CharMetrics.h AFM.h 
minimain.o : minimain.c VectorFont.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h \
  ../Headers/global_defs.h ../Headers/error_defs.h ../Headers/misc.h ../Headers/units.h \
  ../Headers/vectors.h CharMetrics.h 
