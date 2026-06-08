/*
** minimain.c for XQuad in Managers/
** Tests for managers 
**
** Copyright (C) 1994-2000 Axene.
** Authors: Stéphane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
** Email: xcalibur@axene.org
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Started on  Sun Jun 12 01:52:38 1994 Stéphane Boisson
** Last update Sun Oct  2 15:24:04 1994 Stéphane Boisson
*/

#include "xcalibur.h"
#include "ErrorManager.h"
#include "MemoryManager.h"

void main()
{
  char *test, *a, *b;

  test = Xc_malloc("test", 1024);
  a = Xc_malloc("Little Big One", 1024*1024*1024);
  a = Xc_malloc("schtroumpf a", 13);
  memset(test-1, 0, 1026);
  b = Xc_malloc("gato b", 0);
  *b='A';
  Xc_mcheck();
  Xc_mdump();
  Xc_realloc("big realloc", test, 1024*1024*1024);
/*Xc_realloc("big realloc", test, (unsigned long)-20);*/
  test = Xc_realloc("re-test", test, 2048);
  Xc_mdump();
  Xc_free(test);
  Xc_free(b);
  Xc_free(b);
  Xc_realloc("test null", 0,0);
  Xc_free(0);
  Xc_realloc("test free", a,0);
}

