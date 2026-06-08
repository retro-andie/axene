/*
** Xinfo.h for XInstall in Global/
** Definitions of misc. x_info structures
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
** Started on  Mon Jun 13 00:08:10 1994 Stéphane Boisson
** Last update Tue Jun 13 15:21:58 1995 Stéphane Boisson
*/

#ifndef __XINFOS_H__
#define __XINFOS_H__

#include <X11/Xlib.h>

/* definition for displaying */
#define _display This->X_info.display
#define _ecran   This->X_info.ecran
#define _window  This->X_info.window
#define _gc_contour	This->X_info.gc_contour
#define _gc_inside 	This->X_info.gc_inside
#define _contour_color	This->X_info.contour_color
#define _inside_color	This->X_info.inside_color

struct _d_object_std;

typedef struct {
  Display	*display;
  Drawable	window;
  GC		gc_contour;
  GC            gc_inside;
  int		ecran;
  unsigned long	contour_color;
  unsigned long	inside_color;
} x_info;  

#endif /* !__XINFOS_H__ */

