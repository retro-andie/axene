/*
** Zoom_Page.h for Xclamation in Page/
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
** Started on  Mon Jul 17 19:49:03 1995 Emmanuel Paris
** Last update Mon Jul 17 19:49:03 1995 Emmanuel Paris
*/

#ifndef _Zoom_Page_h
#define _Zoom_Page_h

#include "Page.h"

extern void init_Zoom_Plus_Page();
extern status_t eventlook_Zoom_Plus_Page();
extern void abort_Zoom_Plus_Page();
extern void redraw_Zoom_Plus_Page();

sf_page_function fc_Zoom_Plus_Page={
  init_Zoom_Plus_Page,
  eventlook_Zoom_Plus_Page,
  abort_Zoom_Plus_Page,
  redraw_Zoom_Plus_Page,
};  

/*
typedef struct
{
}
d_Zoom_Plus_Page;
*/

extern void init_Zoom_Minus_Page();
extern status_t eventlook_Zoom_Minus_Page();
extern void abort_Zoom_Minus_Page();
extern void redraw_Zoom_Minus_Page();

sf_page_function fc_Zoom_Minus_Page={
  init_Zoom_Minus_Page,
  eventlook_Zoom_Minus_Page,
  abort_Zoom_Minus_Page,
  redraw_Zoom_Minus_Page,
};  

/*
typedef struct
{
}
d_Zoom_Minus_Page;
*/

extern void init_Zoom_Page_In_Zone();
extern status_t eventlook_Zoom_Page_In_Zone();
extern void abort_Zoom_Page_In_Zone();
extern void redraw_Zoom_Page_In_Zone();

sf_page_function fc_Zoom_Page_In_Zone={
  init_Zoom_Page_In_Zone,
  eventlook_Zoom_Page_In_Zone,
  abort_Zoom_Page_In_Zone,
  redraw_Zoom_Page_In_Zone,
};  

typedef struct
{
  boolean	source;
  boolean	prems;
  int		x1,y1;
  int		x2,y2;
  scale_t	factor;
  Time		time;
}
d_Zoom_Page_In_Zone;

extern void zoom_page_fit_in_width();
extern void zoom_page_fit_in_screen();
extern void zoom_page_at_real_size();
extern void zoom_page_at_percent();

#endif


