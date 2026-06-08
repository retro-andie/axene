#
# Makefile for XInstall in GenCDKey/
# Makefile for VectorGraph class
#
# Copyright (C) 1997-2000 Axene.
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
# Started on  Wed Jun 25 14:24:36 1997 Emmanuel Paris
# Last update Mon Jun 30 16:56:55 1997 Emmanuel Paris
#

OBJ=	gen_cdkey.o

all:	gen_cdkey run

run:	
	gen_cdkey.tcl

gen_cdkey:	$(OBJ)	
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

stat:	stat.o
	$(CC) $(LDFLAGS) -o $@ stat.o

genid:	genid.o
	$(CC) $(LDFLAGS) -o $@ genid.o

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -I. -I../Headers -c $< -o $@

clean:
	rm -f gen_cdkey stat genid .id\  *.o *~

