/*
** TrashCan.h for Xclamation, XQuad and XAllWrite in TrashCan/
** Trash Manager header
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
** Started on  Thu Nov 24 16:39:47 1994 Emmanuel Paris
** Last update Mon Apr 20 15:23:50 1998 Emmanuel Paris
*/

#ifndef _TrashCan_h_
#define _TrashCan_h_

#include <X11/Xlib.h>
#include "xcalibur.h"

typedef struct
{
  F_STD;
  void (*eventlook)();
} sf_Trash;

typedef struct
{
  sf_Trash     *f;
  
  Display       *display;
  Window        window;
} c_Trash;

extern sf_Trash fc_Trash;

#endif
