/*
** TimeIndicator.h for XQuad in TimeIndicator/
** Object Header for Loading Document Icons
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
** Last update Wed Feb 28 18:03:05 1996 Emmanuel Paris
*/

#ifndef _TimeIndicator_h_
#define _TimeIndicator_h_

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "Xpm.h"
#include <Xm/Xm.h>

#define LED_SEPARATOR	5
#define NB_LED_IMAGE	5


typedef struct sc_TimeIndicator c_TimeIndicator;

typedef struct 
{
  F_STD;
  void (*get_indicator_geometry) ___PROTO((c_TimeIndicator *This, int *width,
					   int *height));
  void (*display_indicator) ___PROTO((c_TimeIndicator *This, Window window,
				      int orig_x, int orig_y));
  void (*expose_indicator) ___PROTO((Widget *w, c_TimeIndicator *This));
  void (*advance_indicator) ___PROTO((c_TimeIndicator *This));
} sf_TimeIndicator;

struct sc_TimeIndicator
{
  sf_TimeIndicator     *f;
  
  Display	*display;
  Window	window;
  GC		gc;
  GC		gc_mask;
  Pixmap	pixmap_led[NB_LED_IMAGE];
  Pixmap	pixmap_led_mask[NB_LED_IMAGE];
  int		led_width;
  int		led_height;

  int		orig_x;
  int		orig_y;
  
  int		nb_led;
  int		current_led;
};

extern sf_TimeIndicator fc_TimeIndicator;

#endif /* _TimeIndicator_h_ */
