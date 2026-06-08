/*
** AutoScroll.h for XQuad in AutoScroll/
** Object Header for Auto-Scrolling
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
** Started on  Wed Feb 22 16:59:27 1995 Emmanuel Paris
** Last update Thu Nov 28 19:32:19 1996 Emmanuel Paris
*/

#ifndef _AutoScroll_h_
#define _AutoScroll_h_

/*--- forward declaration ---*/
typedef struct sc_AutoScroll c_AutoScroll;

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "ScrollBarX.h"

typedef struct
{
  F_STD;
  void (*eventlook) ___PROTO((c_AutoScroll *This, XEvent *event));
} sf_AutoScroll;

struct sc_AutoScroll
{
  sf_AutoScroll     *f;

  Display *display;
  
  boolean recurs;

  int	x_min;
  int	y_min;
  int	x_max;
  int	y_max;
  
  int	x;
  int	y;
  
  c_ScrollBarX	*ScrollBarH;
  c_ScrollBarX	*ScrollBarV;
  
  void		(*Callback)();	/* callback function */
  void		*Callback_data; /* callback parameter */
};

extern sf_AutoScroll fc_AutoScroll;

#endif /* _AutoScroll_h_ */
