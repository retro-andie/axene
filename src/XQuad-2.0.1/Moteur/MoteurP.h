/*
** MoteurP.h for XQuad in Moteur/
** Private header for Moteur class
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
** Started on  Thu Apr 20 12:14:04 1995 Stéphane Boisson
** Last update Sun Dec  8 18:34:03 1996 Emmanuel Paris
*/

#ifndef __XQ_MOTEUR_P_H__
#define __XQ_MOTEUR_P_H__

#include "Moteur.h"
#include "vectors.h"


/*--- Overrun methods (overrun.c) ---*/
boolean moteurTestOverrun  ___PROTO((c_Moteur *This, cell **pcell_s,
				     int x, int y, boolean direct,
				     boolean sign, scale_t factor));
boolean moteurTestCellOverrun  ___PROTO((c_Moteur *This, cell *cell,
					 CellStyle_t *cstyle,
					 int x, int y, boolean direct,
					 boolean sign, scale_t factor));
void moteurFillCellInfo ___NPROTO((c_Moteur *This, cell_info_t **cell_info,
				  region_t  *region, int xo, int yo));

void moteurComputeOverrun ___PROTO((c_Moteur *This, cell *cell_s,
				    int x, int y, scale_t factor));
void moteurFreeOverrunCells ___PROTO((c_Moteur *this, moteur_overrun_t *ptr));
moteur_overrun_t *moteurGetOverrunCells ___PROTO((c_Moteur *this,
						 region_t *region));
void moteurGetOverrunRegion ___PROTO((c_Moteur *this, cell *cellp,
				      int x, int y, region_t *regionp));

/*--- Read & write methods (docfile.c) ---*/
boolean readMoteur ___PROTO((c_DocFile *doc, char *keyword,
			     long param, c_Moteur *this));
boolean writeMoteur ___PROTO((c_Moteur *this, c_DocFile *doc));

/*--- Print methods (print.c) ---*/
boolean printMoteur ___PROTO((c_Moteur *this, c_PostScript *post,
			      region_t *region, setup_print_t *setup));
void getMoteurBoundingRegion ___PROTO((c_Moteur *this, region_t *regionp));
void getMoteurBoundingBox ___PROTO((c_Moteur *this,
				    region_t *region, bbox_t *bboxp));
void getMoteurCellByXY ___PROTO((c_Moteur *this, coord_t x, coord_t y,
				 int *columnp, int *rowp, bbox_t *bboxp));
cell_style_list *getMoteurCellStyleList ___PROTO((c_Moteur *this, 
						  region_t *region));

#endif /* !__XQ_MOTEUR_P_H__ */
