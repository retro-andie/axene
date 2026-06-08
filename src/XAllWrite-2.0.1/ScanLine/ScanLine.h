/*
** ScanLine.h for XAllWrite in ScanLine/
** ScanLine object: calcul line width for calibration
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
** Started on  Wed Apr 23 15:38:26 1997 Emmanuel Paris
** Last update Tue Jun 23 14:35:19 1998 Emmanuel Paris
*/

#ifndef _ScanLine_h_
#define _ScanLine_h_

/*--- Forward declarations ---*/

typedef struct sc_ScanLine c_ScanLine;

#include "xcalibur.h"
#include "Page.h"
#include "FrameManager.h"
#include "TextRuler.h"

#define SCANLINE_VALID		0
#define SCANLINE_HORIZONTAL	1
#define SCANLINE_NOT_VALID	2

enum
{
 SSS_NOT_INITIATED,
 SSS_INITIATED,
 SSS_FINISHED
};

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

#define SL_MAX_SEGMENTS 20

typedef struct _scan_stat_t
{
 l_Cadre	*lcadre;
 sl_Arrete	*arrete_buf;
 int		nb_tranche;
 int		cur_tranche;
 sl_Tranche	*tranche;
 coord_t	delta_y;
 coord_t	y_min;
 int		status;
 struct _scan_stat_t *next;
} scan_state_t;


typedef struct
{
 F_STD;
 void (*initScanLine) ___PROTO((c_ScanLine *This, coord_t y));
 coord_t (*startScanLine) ___PROTO((c_ScanLine *This));
 coord_t (*newLine) ___PROTO((c_ScanLine *This, c_TextRuler *ruler,
			      boolean np, coord_t height));
 sl_Segment *(*getNextScanLine) ___PROTO((c_ScanLine *This, 
					  c_TextRuler *ruler, boolean np,
					  coord_t height, coord_t *y,
					  int *nb_seg));
} sf_ScanLine;

/* define the ScanLine Class */
struct sc_ScanLine
{
 sf_ScanLine	*f;

 sl_Segment	*record_segments;
 sl_Segment	segments[SL_MAX_SEGMENTS];
 c_Page		*page;
 coord_t	y;

 ItemHandle	*state_handle;
 scan_state_t	*state;
};

extern sf_ScanLine fc_ScanLine;

#endif /* _ScanLine_h_ */
