/*
** Cursor_Drag_And_Drop.h for Xclamation in Cursor/
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
** Started on  Mon Jul 17 14:20:58 1995 Emmanuel Paris
** Last update Wed Jun  4 17:31:55 1997 Emmanuel Paris
*/

#ifndef _Cursor_Drag_And_Drop_
#define _Cursor_Drag_And_Drop_

#ifndef NB_CURSOR
#define NB_CURSOR 0
#endif

enum {
  C_DROP_TRASH =  NB_CURSOR,
  C_DRAG_R,
  C_DRAG_L,
  LAST_CURSOR_DRAG_AND_DROP
};

#undef NB_CURSOR
#define	NB_CURSOR LAST_CURSOR_DRAG_AND_DROP

#ifdef _PRIVATE_CURSOR_

#include "../Pixmaps/drag.trash.zpm"
#include "../Pixmaps/drag.right.zpm"
#include "../Pixmaps/drag.left.zpm"

#endif

#define CURSOR_SET_DRAG_AND_DROP \
{ DEFINED_CURSOR_XPM, (void *)drag_trash_xpm,				      \
    STANDARD_CURSOR, (void *)XC_pirate},	       /* C_DROP_TRASH */     \
{ DEFINED_CURSOR_XPM,(void *)drag_right_xpm,				      \
    STANDARD_CURSOR, (void *)XC_iron_cross},	       /* C_DRAG_R */	      \
{ DEFINED_CURSOR_XPM,(void *)drag_left_xpm,				      \
    STANDARD_CURSOR, (void *)XC_iron_cross},	       /* C_DRAG_L */	      

#endif
