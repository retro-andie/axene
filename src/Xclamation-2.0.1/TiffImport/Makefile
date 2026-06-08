#
# Makefile for Xclamation in TiffImport/
# Tag Image File Format Library
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
# Started on  Sometime during     1994 One of the authors
# Last update Sat Mar  7 15:05:52 1998 One of the authors
#

# 
# Makefile for XCalibur in Main/
# 
# Copyright (C) 1994, XCalibur-PAO. All Rights reserved.
# Login <xcalibur@axene.org>
# 
# Started on  Tue Oct  4 11:11:32 1994 Nucleus
# Last update Sat Mar  7 14:05:52 1998 Xibios 2
#

SHELL=	/bin/sh

LIB=	../lib/TiffImport.a

OBJS=	tif_fax3.o \
	tif_fax4.o \
	tif_aux.o \
	tif_ccittrle.o \
	tif_close.o \
	tif_compress.o \
	tif_dir.o \
	tif_dirinfo.o \
	tif_dirread.o \
	tif_dirwrite.o \
	tif_dumpmode.o \
	tif_error.o \
	tif_getimage.o \
	tif_jpeg.o \
	tif_flush.o \
	tif_lzw.o \
	tif_next.o \
	tif_open.o \
	tif_packbits.o \
	tif_print.o \
	tif_read.o \
	tif_strip.o \
	tif_swab.o \
	tif_thunder.o \
	tif_tile.o \
	tif_unix.o \
	tif_version.o \
	tif_warning.o \
	tif_write.o 

all:	showenv $(LIB)

$(LIB): $(OBJS)
	@echo "LIB $(LIB) Archiving: $(OBJS)"
	@$(AR) $(ARFLAGS) $@ $(OBJS) > /dev/null
	@echo "LIBRARY $(LIB) built."

.c.o:
	@../Utils/echonl.sh "LIB $(LIB) Compile: "$@" "
	@$(CC) $(CPPFLAGS) $(CFLAGS) -Dlint -I. -I../Headers -c $<
	@echo "done."

tif_fax3.o: g3states.h

g3states.h: mkg3states.c t4.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -Dlint -o mkg3states mkg3states.c
	./mkg3states -c > g3states.h

clean:
	rm -f $(OBJS) $(LIB) *~ core a.out mkg3states g3states.h

showenv:
	@echo -------------- LIB Compilation options -------------------------------------
	@echo "setenv CFLAGS '$(CFLAGS)'"
	@echo "setenv CPPFLAGS '$(CPPFLAGS)'"
	@echo "setenv ARFLAGS '$(ARFLAGS)'"
	@echo ----------------------------------------------------------------------------
