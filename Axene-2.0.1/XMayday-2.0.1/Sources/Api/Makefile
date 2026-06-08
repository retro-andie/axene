#
# Makefile for XMayday in Api/
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
# Started on  Sat May 11 17:00:10 1996 Emmanuel Paris
# Last update Thu Feb 27 16:14:33 1997 Emmanuel Paris
#

# --------------------------------------------------------------------- 
# 

#CC=gcc
#CC=cc
#CPPFLAGS=
#CFLAGS=
LDFLAGS=-L/usr/X11R6/lib

AR=ar
ARFLAGS=rvs


# --------------------------------------------------------------------- 
# Program name variable
PROG_NAME	= test

# --------------------------------------------------------------------- 
# compil' objects variable
OBJS=	TestApi.o FileTools.o

XmOBJS=	XMaydayCom.o ClientWin.o

# --------------------------------------------------------------------- 
# Program libraries variable
LIBS=	-lX11 -lm
XmLIBS=	XMaydayCom.a

# ---------------------------------------------------------------------
# Compilation rules

all:	$(XmLIBS) $(PROG_NAME)

$(PROG_NAME):	$(OBJS) $(XcLIBS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(XmLIBS) $(LIBS)
	@echo Please enter \`./$(PROG_NAME)\' to test the API.

objs:	$(OBJS) $(XcLIBS)

link:
	$(CC) $(LDFLAGS) -o $(PROG_NAME) $(OBJS) $(XcLIBS) $(LIBS)

# ---------------------------------------------------------------------
# Profile rules

profile:
	make "CFLAGS=$(CFLAGS) -pg" "LDFLAGS=$(LDFLAGS) -static -pg" \
		"LIBS=$(LIBS) -lgmon"

# ---------------------------------------------------------------------
# Special compilation command

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(XmLIBS):	$(XmOBJS)
	$(AR) $(ARFLAGS) $@ $(XmOBJS)
	@echo $@ archive made...	

# ---------------------------------------------------------------------
# Clean rules

clean:
	rm -f *~ *.bak $(PROG_NAME) $(OBJS) $(XmOBJS) $(XmLIBS)
