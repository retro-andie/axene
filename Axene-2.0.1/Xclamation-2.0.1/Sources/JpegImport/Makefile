#
# Makefile for Xclamation in JpegImport/
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
# Started on  Wed Oct 12 12:36:18 1994 Stéphane Boisson
# Last update Wed Feb  4 01:03:35 1998 Emmanuel Paris
#

#Only on a pure BSD system
#CFLAGS=	-DBSD

LIB=	../lib/JpegImport.a
SHELL=	/bin/sh

OBJ=	jdcolor.o jddeflts.o jdhuff.o jdmaster.o jdmcu.o \
	jdpipe.o jdsample.o jerror.o jmemmgr.o jmemsys.o jpeg.o jrdjfif.o \
	jrevdct.o jutils.o

all:	showenv $(LIB)

.c.o:	
	@../Utils/echonl.sh "LIB $(LIB) Compile: "$@" "
	@$(CC) $(CPPFLAGS) $(CFLAGS) -I. -I../Headers -c $< -o $@
	@echo "done."

$(LIB):	$(OBJ)
	@echo "LIB $(LIB) Archiving: $(OBJ)"
	@$(AR) $(ARFLAGS) $@ $(OBJ) > /dev/null
	@echo "LIBRARY $(LIB) built."

clean:
	rm -f *.o *~ $(LIB)

showenv:
	@echo -------------- LIB Compilation options -------------------------------------
	@echo "setenv CFLAGS '$(CFLAGS)'"
	@echo "setenv CPPFLAGS '$(CPPFLAGS)'"
	@echo "setenv ARFLAGS '$(ARFLAGS)'"
	@echo ----------------------------------------------------------------------------

jdcolor.o : jdcolor.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h 
jddeflts.o : jddeflts.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h 
jdhuff.o : jdhuff.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h 
jdmaster.o : jdmaster.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h 
jdmcu.o : jdmcu.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h 
jdpipe.o : jdpipe.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h 
jdsample.o : jdsample.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h 
jerror.o : jerror.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h 
jmemmgr.o : jmemmgr.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h jmemsys.h 
jmemsys.o : jmemsys.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h jmemsys.h 
jpeg.o : jpeg.c jpeg.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h ../Headers/Image.h \
  jinclude.h jconfig.h jpegdata.h 
jrdjfif.o : jrdjfif.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h 
jrevdct.o : jrevdct.c jinclude.h jconfig.h ../Headers/xcalibur.h ../Headers/ErrorManager.h \
  ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h 
jutils.o : jutils.c jinclude.h jconfig.h ../Headers/xcalibur.h \
  ../Headers/ErrorManager.h ../Headers/MemoryManager.h ../Headers/ObjStd.h ../\
  ../Headers/global_defs.h ../Headers/error_defs.h \
  ../Headers/misc.h ../Headers/units.h jpegdata.h 
