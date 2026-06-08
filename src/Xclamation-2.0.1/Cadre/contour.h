/*
** contour.h for Xclamation, XQuad and XAllWrite in Cadre/
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
** Started on  Mon Jul 17 14:08:22 1995 Emmanuel Paris
** Last update Tue Aug  4 18:25:28 1998 Emmanuel Paris
*/

#ifndef _contour_h_
#define _contour_h_

typedef struct
{
 coord_t	x;
 coord_t	y;
 boolean	init;	  /* is one of initial points */
} ct_pnt;

typedef struct
{
 ct_pnt		*point1;
 ct_pnt		*point2;
 boolean	cadre1;   /* is in cadre 1 */ 
 boolean	is_in;    /* is in the other cadre */ 
} ct_seg;

typedef struct _l_ct_seg
{
 ct_seg			*segment;
 struct _l_ct_seg	*PrevSegment;
 struct _l_ct_seg	*NextSegment;
} l_ct_seg;

typedef struct
{
  ct_pnt	*inters;
  coord_t	distance;
} ct_inters;

extern l_Point	*SubstractCadre();
extern l_Point	*AdditionCadre();
extern boolean is_frame_a_rectangle();

#endif

