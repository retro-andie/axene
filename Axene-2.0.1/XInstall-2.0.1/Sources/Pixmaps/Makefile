#
# Makefile for XInstall in Pixmaps/
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
# Started on  Tue Oct  4 11:11:32 1994 Stéphane Boisson
# Last update Tue Feb 17 15:04:21 1998 Emmanuel Paris
#

SHELL=/bin/sh

.c.o:	
	@../Utils/echonl.sh "Pixmaps - Compile: "$@" "
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo "done."

all:	encode_gif compress_xpm 
	touch ./uptodate

compress_xpm:	xpmtozpm
	@../Utils/echonl.sh "converting xpm to zpm: "; \
	for xpm in *.xpm; do if [ ! -r `basename $$xpm .xpm`.zpm ]; then \
	 ../Utils/echonl.sh "."; ./xpmtozpm $$xpm; fi; done; echo "done"

encode_gif:	giftoh
	@../Utils/echonl.sh "converting gif to ascii form: "; \
	for gif in *.gif; do if [ ! -r `basename $$gif .gif`.h ]; then \
	 ../Utils/echonl.sh "."; ./giftoh $$gif; fi; done; echo "done"

xpmtozpm:	../Utils/xpmtozpm.o
	@../Utils/echonl.sh "Link: "$@" "
	@$(CC) $? -o $@
	@echo "done."

xpmrsize:	../Utils/xpmrsize.o
	@../Utils/echonl.sh "Link: "$@" "
	@$(CC) $? -o $@
	@echo "done."

zpmrsize:	../Utils/zpmrsize.o
	@../Utils/echonl.sh "Link: "$@" "
	@$(CC) $? -o $@
	@echo "done."

giftoh: ../Utils/giftoh.o
	@../Utils/echonl.sh "Link: "$@" "
	@$(CC) $? -o $@
	@echo "done."

test:	xpmrsize zpmrsize
	@../Utils/echonl.sh "calculating xpm and zpm file size: "
	@xsize=0; zsize=0; for xpm in *.xpm; do \
	zpm=`basename $$xpm .xpm`.zpm; if [ -r $$zpm ]; then \
	size=`./xpmrsize $$xpm`; xsize=`expr $$xsize + $$size`; \
	size=`./zpmrsize $$zpm`; zsize=`expr $$zsize + $$size`; \
	../Utils/echonl.sh ".";	fi; done; echo "done"; \
	../Utils/echonl.sh "xpm size: "$$xsize" zpm size: "$$zsize; \
	../Utils/echonl.sh " win: "`expr $$xsize - $$zsize`; \
	echo " rate: "`expr 100 - $$zsize \* 100 / $$xsize 2>/dev/null || \
	echo "0"`"%"

clean:	
	@echo "CLEANING PIXMAPS"
	@rm -f *.zpm xpmtozpm xpmrsize zpmrsize giftoh ../Utils/xpmtozpm.o \
		../Utils/xpmrsize.o ../Utils/zpmrsize.o uptodate

