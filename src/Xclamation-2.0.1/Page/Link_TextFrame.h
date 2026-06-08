/*
** Link_TextFrame.h for Xclamation in Page/
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
** Started on  Mon Jul 17 20:20:07 1995 Emmanuel Paris
** Last update Mon Jul 17 20:20:07 1995 Emmanuel Paris
*/

#ifndef _Link_TextFrames_h
#define _Link_TextFrames_h

#include "Page.h"

extern void init_Link_TextFrame();
extern status_t eventlook_Link_TextFrame();
extern void abort_Link_TextFrame();
extern void redraw_Link_TextFrame();

sf_page_function fc_Link_TextFrame={
  init_Link_TextFrame,
  eventlook_Link_TextFrame,
  abort_Link_TextFrame,
  redraw_Link_TextFrame
};

typedef struct 
{
  int		function;
  int		xm,ym;
} 
d_Link_TextFrame;

#define LT_SELECT_SOURCE	0
#define LT_SELECT_TARGET	1

#endif

