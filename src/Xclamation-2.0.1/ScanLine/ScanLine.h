/*
** ScanLine.h for Xclamation in ScanLine/
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
** Started on  Tue Jul 18 12:19:33 1995 Emmanuel Paris
** Last update Tue Jun  9 17:10:30 1998 Emmanuel Paris
*/

#ifndef _ScanLine_h_
#define _ScanLine_h_

#include "Cadre.h"

#define SCANLINE_VALID		0
#define SCANLINE_HORIZONTAL	1
#define SCANLINE_NOT_VALID	2

#define SCANLINE_MINIMA		FALSE
#define SCANLINE_ORDONNEE	TRUE

typedef struct 
{
 real	pente;
 real	x_value;
} sl_Arrete;

typedef struct 
{
 coord_t	y;
 l_Point	*point;
 l_Point	*point1;
 l_Point	*point2;
 sl_Arrete	*arrete1;
 sl_Arrete	*arrete2;
} sl_Sommet;

typedef struct 
{
 coord_t	delta_y;
 int		nb_arrete;
 l_Point	**point;
 sl_Arrete	**arrete;
} sl_Tranche;

typedef struct 
{
 coord_t x_min;
 coord_t x_max;
} sl_Segment;

/* defint the method for Class ScanLine */
typedef struct
{
 F_STD;
 int (*get_next_scanline)();
 /* int get_next_scanline(c_ScanLine *scanline,sl_Segment **segment,
    coord_t delta_y); */
 boolean (*skip_scanline)();
 /* error skip_scanline(c_ScanLine *scanline,coord_t delta_y); */
 void (*intersect_segment)();
 /* void intersect_segment(Segment **segmentA,int *nb_segmentA,
    Segment  *segmentB,int  nb_segmentB); */
} sf_ScanLine;


/* define the ScanLine Class */
typedef struct cs_ScanLine
{
 sf_ScanLine	*f;
 sl_Arrete	*arrete_buf;
 int		nb_tranche;
 int		cur_tranche;
 sl_Tranche	*tranche;
 coord_t	delta_y;
} c_ScanLine;

extern sf_ScanLine fc_ScanLine;

#endif /* _ScanLine_h_ */




