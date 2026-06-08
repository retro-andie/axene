/*
** Cursor_Regle.h for XQuad in Cursor/
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Mon Jul 17 14:20:58 1995 Emmanuel Paris
** Last update Wed Jun  4 18:49:02 1997 Emmanuel Paris
*/

#ifndef _Cursor_Regle_
#define _Cursor_Regle_

#ifndef NB_CURSOR
#define NB_CURSOR 0
#endif

enum {
  C_REGLE_H = NB_CURSOR,
  C_REGLE_V,
  C_REGLE_C,
  LAST_CURSOR_REGLE
};

#undef NB_CURSOR
#define	NB_CURSOR LAST_CURSOR_REGLE

#ifdef _PRIVATE_CURSOR_

#include "../Pixmaps/regle.horizontal.zpm"
#include "../Pixmaps/regle.vertical.zpm"

#endif

#define CURSOR_SET_REGLE \
{ DEFINED_CURSOR_XPM, (void *)regle_horizontal_xpm,			      \
    STANDARD_CURSOR, (void *)XC_sb_v_double_arrow},  /* C_REGLE_H */	      \
{ DEFINED_CURSOR_XPM, (void *)regle_vertical_xpm,			      \
    STANDARD_CURSOR, (void *)XC_sb_h_double_arrow},  /* C_REGLE_V */	      \
{ NO_CURSOR, 0,								      \
    STANDARD_CURSOR, (void *)XC_crosshair},          /* C_REGLE_C */	      

#endif
