/*
** ProgressBar.h for XInstall in ProgressBar/
** Objet ProgressBar
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Thu Mar 20 22:02:53 1997 Antoine Buat
** Last update Wed Oct 22 18:12:08 1997 Emmanuel Paris
*/

#ifndef _ProgressBar_h
#define _ProgressBar_h

typedef struct sc_ProgressBar c_ProgressBar;

/*--- include ---*/
#include "xcalibur.h"
#include "ManageWidget.h"
#include "WInterface.h"
#include <X11/Intrinsic.h>
#include <Xm/DrawingA.h>

/*--- define methods for Class ProgressBar ---*/
typedef struct
{
  F_STD;
  void (*change_part) ___PROTO((c_ProgressBar *This, int size));
  void (*change_total) ___PROTO((c_ProgressBar *This, int size));
  void (*redraw) ___PROTO((Widget w, c_ProgressBar *This));
} sf_ProgressBar;

/*--- define filed for Class ProgressBar ---*/
struct sc_ProgressBar
{
 sf_ProgressBar		*f;
 unsigned long		total;
 unsigned long		part;
 unsigned long		last_part; 

 char			**buf_off;
 Pixmap			pix_off;
 Pixmap			pix_off_m;
 int			off_y;
 int			off_x;
 int			off_w;
 int			off_h;
 char			**buf_on;
 Pixmap			pix_on;
 Pixmap			pix_on_m;
 int			on_y;
 int			on_x;
 int			on_w;
 int			on_h;
 int			pwidth;
 int			pheight;
 int			pix_num;
 int			all_y;

 int			width;
 int			height;
 int			center_w;
 int			center_h;
 Widget			w_This;
 GC			gc_on;
 GC			gc_on_mask;
 GC			gc_off;
 GC			gc_off_mask;
 Display		*display;
 Window			window;
};

extern sf_ProgressBar fc_ProgressBar;

#endif




