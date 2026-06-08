#
# Makefile for XQuad in FileSelector/
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
# Started on  Fri Dec  9 20:36:16 1994 Stéphane Boisson
# Last update Fri Dec 16 12:15:14 1994 Stéphane Boisson
#

OBJ=	FileSelector.o \
	../ItemList/ItemList.o \
	../ItemMenu/ItemMenu.o \
	../WTextField/WTextField.o \
	../CustomDialog/CustomDialog.o \
	../Global/misc.o \
	../ManageWidget/ManageWidget.o

XcLIBS=	../lib/Managers.a

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -I../Headers -c $< -o $@

gato:   $(OBJ) minimain.o
	$(CC) $(LDFLAGS) -o $@ minimain.o $(OBJ) $(XcLIBS) $(MOTIF_LIBS)

clean:
	rm -f *.o *~ *.a gato
