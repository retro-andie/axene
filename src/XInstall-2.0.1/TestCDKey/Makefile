#
# Makefile for XInstall in TestCDKey/
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
# Last update Mon Jan 12 17:05:32 1998 Emmanuel Paris
#

OBJS=	main.o TestCDKey.o TestIDFile.o ../Global/misc.o ../Global/filename.o \
	../Global/fileutils.o ../Global/deadline.o 

LIBS= ../lib/Managers.a

all:	main

main:	$(OBJS) change_code
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) -lm
	./change_code $@

# ---------------------------------------------------------------------
change_code: ../Utils/change_code.o
	$(CC) $(LDFLAGS) ../Utils/change_code.o -o $@

# comment these folowing 2 lines for final version.
TestIDFile.o:	TestIDFile.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DNDEADLINE -D___nocdrom -I. -I../Headers -c $< -o $@

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -DNDEADLINE -I. -I../Headers -c $< -o $@

../lib/Managers.a:
	cd ../Managers; $(MAKE)

clean:
	rm -f $(OBJS) main *~

