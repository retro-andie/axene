#/*
# * makefile 		- Makefile for Main
# *
# * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
# *
# */

PROG_NAME = toto
CC=	cc
DEFS=	-DHAVE_STRDUP
SRC=	misc.c
OBJ=	misc.o
LIBS=	-lXm -lXt -lX11

$(PROG_NAME):	$(OBJ)
	
.c.o:
	$(CC) $(DEFS) -I../Header $< -c

default: $(OBJ)
	
clean:
	rm -f *.o *~ *.bak $(PROG_NAME)

clean2:
	rm -f *~ *.bak
