/*
** ColorListP.h for Xclamation in ColorList/
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
** Started on  Fri Feb 21 11:48:10 1997 Emmanuel Paris
** Last update Tue Jan 18 16:51:46 2000 Emmanuel Paris
*/

#ifndef CL_EXTERN_DEFINES

#include "../Pixmaps/font.colorlist.zpm"
#include "../Pixmaps/underline.colorlist.zpm"
#include "../Pixmaps/underline.colorlist.mask.xbm"
#include "../Pixmaps/strikeout.colorlist.zpm"
#include "../Pixmaps/strikeout.colorlist.mask.xbm"
#include "../Pixmaps/shadow.colorlist.zpm"
#include "../Pixmaps/shadow.colorlist.mask.xbm"

enum 
{
 RECT_TYPE, MASK_TYPE, NAME_TYPE
};

typedef struct 
{
 int type;			/* can be RECT_TYPE, MASK_TYPE or NAME_TYPE */
 int widget_type;		/* can be XcPush or XcLabel */
 char **icon_data;
 char *mask_data;
 int x, y;			/* rectangle fill origin */
 int width, height;		/* rectangle fill size or bitmap mask size */
 Pixmap mask_pixmap;
} cl_icon_t;

#define NP (Pixmap)(0)

static cl_icon_t cl_icon[NB_XcCL_TYPE] = 
{ 
 { NAME_TYPE, XcLabel, NULL, NULL, 0, 0, 100, 22, NP },
 { RECT_TYPE, XcPush, font_colorlist_xpm, NULL, 12, 9, 9, 9, NP}, 
 { RECT_TYPE, XcLabel, font_colorlist_xpm, NULL, 12, 9, 9, 9, NP}, 
 { RECT_TYPE, XcLabel, font_colorlist_xpm, NULL, 12, 9, 9, 9, NP },
 { MASK_TYPE, XcLabel, underline_colorlist_xpm, 
    (char *)underline_bits, 0, 0, 26, 22, NP },
 { MASK_TYPE, XcLabel, strikeout_colorlist_xpm, 
    (char *)strikeout_bits, 0, 0, 26, 22, NP },
 { MASK_TYPE, XcLabel, shadow_colorlist_xpm, 
    (char *)shadow_bits, 0, 0, 26, 22, NP }
};

#undef NP

#else		/* CL_EXTERN_DEFINES */

enum 
{
 XcCL_NORMAL,
 XcCL_FONT,
 XcCL_FONTL,
 XcCL_FILL,
 XcCL_UNDERLINE,
 XcCL_STRIKEOUT,
 XcCL_SHADOW,
 NB_XcCL_TYPE
};

#endif
