#
# Makefile for XAllWrite in WTextField/
#
# Copyright (C) 1996-2000 Axene.
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
# Started on  Sun Aug 18 15:10:58 1996 Emmanuel Paris
# Last update Tue Aug 20 18:48:22 1996 Emmanuel Paris
#

#--- program name ---
PROG_NAME = main

#--- sources ---
SRC = UnitConvertion.c \
 UnitLength.c \
 UnitRatio.c \
 ../Global/misc.c

OBJ = UnitConvertion.o \
 UnitLength.o \
 UnitRatio.o \
 ../Global/misc.o

XcLIBS=	../lib/Managers.a

all:	$(PROG_NAME)

$(PROG_NAME):	$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(XcLIBS) -lm

link:
	$(CC) $(LDFLAGS) -o $(PROG_NAME) $(OBJ) $(XcLIBS) -lm

../lib/Managers.a:
	cd ../Managers; $(MAKE)

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -I../Headers -c $< -o $@

UnitConvertion.o:	UnitConvertion.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D_UNIT_CONVERTION_MAIN_ -I../Headers \
		-c $< -o $@
zclean: 
	rm -f $(OBJ) $(PROG_NAME)

clean:
	rm -f *.o *~ *.bak $(PROG_NAME)

clean2:
	rm -f *~ *.bak

check:
	nm $(OBJ) > /dev/null

depend:	
	cp Makefile Makefile-depend
	gcc -MM -I../Headers $(SRC) >> Makefile-depend

# ------------------------------------------------------------------- 






