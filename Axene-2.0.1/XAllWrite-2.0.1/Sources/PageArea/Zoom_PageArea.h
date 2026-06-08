/*
** Zoom_PageArea.h for XAllWrite in PageArea/
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
** Started on  Mon Jul 17 19:49:03 1995 Emmanuel Paris
** Last update Thu Oct  9 20:20:59 1997 Emmanuel Paris
*/

#ifndef _Zoom_PageArea_h
#define _Zoom_PageArea_h

#include "PageArea.h"
#include "PageAreaP.h"

#define ZOOM_PLUS_LIMIT		400
#define ZOOM_MINUS_LIMIT	10
#define ZOOM_STEP		25

extern void init_Zoom_Plus_PageArea();
extern status_t eventlook_Zoom_Plus_PageArea();
extern void abort_Zoom_Plus_PageArea();
extern void redraw_Zoom_Plus_PageArea();

#ifdef _ZPA_PRIVATE_
sf_pagearea_function fc_Zoom_Plus_PageArea =
{
 init_Zoom_Plus_PageArea,
 eventlook_Zoom_Plus_PageArea,
 abort_Zoom_Plus_PageArea,
 redraw_Zoom_Plus_PageArea,
};  
#else
extern sf_pagearea_function fc_Zoom_Plus_PageArea;
#endif

extern void init_Zoom_Minus_PageArea();
extern status_t eventlook_Zoom_Minus_PageArea();
extern void abort_Zoom_Minus_PageArea();
extern void redraw_Zoom_Minus_PageArea();

#ifdef _ZPA_PRIVATE_
sf_pagearea_function fc_Zoom_Minus_PageArea =
{
 init_Zoom_Minus_PageArea,
 eventlook_Zoom_Minus_PageArea,
 abort_Zoom_Minus_PageArea,
 redraw_Zoom_Minus_PageArea,
};  
#else
extern sf_pagearea_function fc_Zoom_Minus_PageArea;
#endif

extern void init_Zoom_PageArea_In_Zone();
extern status_t eventlook_Zoom_PageArea_In_Zone();
extern void abort_Zoom_PageArea_In_Zone();
extern void redraw_Zoom_PageArea_In_Zone();

#ifdef _ZPA_PRIVATE_
sf_pagearea_function fc_Zoom_PageArea_In_Zone = 
{
 init_Zoom_PageArea_In_Zone,
 eventlook_Zoom_PageArea_In_Zone,
 abort_Zoom_PageArea_In_Zone,
 redraw_Zoom_PageArea_In_Zone,
};  
#else
extern sf_pagearea_function fc_Zoom_PageArea_In_Zone;
#endif

typedef struct
{
 boolean	source;
 boolean	prems;
 int		x1,y1;
 int		x2,y2;
 scale_t	factor;
 Time		time;
 GC		gc;
}
d_Zoom_PageArea_In_Zone;

extern void zoom_pagearea_fit_in_width();
extern void zoom_pagearea_fit_in_screen();
extern void zoom_pagearea_at_real_size();
extern void zoom_pagearea_at_percent();

#endif


