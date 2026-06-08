/*
** Cursor_Resize.h for XMayday in Cursor/
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
** Last update Wed Apr  9 16:12:22 1997 Emmanuel Paris
*/

#ifndef _Cursor_Resize_
#define _Cursor_Resize_

#ifndef NB_CURSOR
#define NB_CURSOR 0
#endif

enum {
  C_RESIZE_1 = NB_CURSOR,
  C_RESIZE_2,
  C_RESIZE_3,
  C_RESIZE_4,
  C_RESIZE_5,
  C_RESIZE_6,
  C_RESIZE_7,
  C_RESIZE_8,
  LAST_CURSOR_RESIZE
};

#undef NB_CURSOR
#define	NB_CURSOR LAST_CURSOR_RESIZE

#ifdef _PRIVATE_CURSOR_

#include "../Pixmaps/resize_1.zpm"
#include "../Pixmaps/resize_2.zpm"
#include "../Pixmaps/resize_3.zpm"
#include "../Pixmaps/resize_4.zpm"
#include "../Pixmaps/resize_5.zpm"
#include "../Pixmaps/resize_6.zpm"
#include "../Pixmaps/resize_7.zpm"
#include "../Pixmaps/resize_8.zpm"

#endif

#define CURSOR_SET_RESIZE \
{ DEFINED_CURSOR_XPM,resize_1_xpm,					      \
    STANDARD_CURSOR, (void *)XC_hand1},		/* C_RESIZE_1 */	      \
{ DEFINED_CURSOR_XPM,resize_2_xpm,					      \
    STANDARD_CURSOR, (void *)XC_hand1},		/* C_RESIZE_2 */	      \
{ DEFINED_CURSOR_XPM,resize_3_xpm,					      \
    STANDARD_CURSOR, (void *)XC_hand1},		/* C_RESIZE_3 */	      \
{ DEFINED_CURSOR_XPM,resize_4_xpm,					      \
    STANDARD_CURSOR, (void *)XC_hand1},		/* C_RESIZE_4 */	      \
{ DEFINED_CURSOR_XPM, resize_5_xpm,					      \
    STANDARD_CURSOR, (void *)XC_hand1},		/* C_RESIZE_5 */	      \
{ DEFINED_CURSOR_XPM, resize_6_xpm,					      \
    STANDARD_CURSOR, (void *)XC_hand1},	       /* C_RESIZE_6 */		      \
{ DEFINED_CURSOR_XPM, resize_7_xpm,					      \
    STANDARD_CURSOR, (void *)XC_hand1},	       /* C_RESIZE_7 */		      \
{ DEFINED_CURSOR_XPM, resize_8_xpm,					      \
    STANDARD_CURSOR, (void *)XC_hand1},	       /* C_RESIZE_8 */		      

#endif
