/*
** Create_Cadres.h for XAllWrite in FrameManager/
** define Dispatch to FrameManager function
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
** Started on  Tue Mar 28 13:43:16 1995 Emmanuel Paris
** Last update Tue Apr 21 17:39:46 1998 Emmanuel Paris
*/

#ifndef _Create_Cadres_h
#define _Create_Cadres_h

#include "FrameManager.h"
#include <X11/keysym.h>

/*
*   CREATE CADRE RECTANGLE FUNCTION    *
*                               	*/

extern void init_Create_Cadre_Rectangle();
extern status_t eventlook_Create_Cadre_Rectangle();
extern void abort_Create_Cadre_Rectangle();
extern void redraw_Create_Cadre_Rectangle();

sf_frmg_function fc_Create_Cadre_Rectangle =
{
 init_Create_Cadre_Rectangle,
 eventlook_Create_Cadre_Rectangle,
 abort_Create_Cadre_Rectangle,
 redraw_Create_Cadre_Rectangle
};

typedef struct 
{
 boolean	source;
 boolean	prems;
 boolean	shift;
 int		xm,ym;
 int		x1,y1,x2,y2;
 coord_t	sx1,sy1,sx2,sy2;
 Time		time;
} 
d_Create_Cadre_Rectangle;

/*
*   CREATE CADRE ELIPSE FUNCTION    *
*				     */

extern void init_Create_Cadre_Elipse();
extern status_t eventlook_Create_Cadre_Elipse();
extern void abort_Create_Cadre_Elipse();
extern void redraw_Create_Cadre_Elipse();

sf_frmg_function fc_Create_Cadre_Elipse =
{
 init_Create_Cadre_Elipse,
 eventlook_Create_Cadre_Elipse,
 abort_Create_Cadre_Elipse,
 redraw_Create_Cadre_Elipse
};

typedef struct 
{
 boolean	source;
 boolean	prems;
 boolean	shift;
 int		xm,ym;
 int		x1,y1,x2,y2;
 coord_t	sx1,sy1,sx2,sy2;
 Time		time;
} 
d_Create_Cadre_Elipse;

/*
*   CREATE CADRE POLYGONAL FUNCTION    *
*                                      */

extern void init_Create_Cadre_Polygonal();
extern status_t eventlook_Create_Cadre_Polygonal();
extern void abort_Create_Cadre_Polygonal();
extern void redraw_Create_Cadre_Polygonal();

sf_frmg_function fc_Create_Cadre_Polygonal =
{
 init_Create_Cadre_Polygonal,
 eventlook_Create_Cadre_Polygonal,
 abort_Create_Cadre_Polygonal,
 redraw_Create_Cadre_Polygonal
};

typedef struct 
{
 boolean	source;
 boolean	prems;
 boolean	cont;
 boolean	connected;
 boolean	ended;
 int		xo,yo;
 coord_t	sxo,syo;
 int		xc,yc;
 coord_t	sxc,syc;
 l_Point	*lp_loop;
 l_Cadre	*lcadre;
 c_Cadre	*cadre;
} 
d_Create_Cadre_Polygonal;

#endif
