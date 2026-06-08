/*
** CellStyleP.h for XQuad in CellStyle/
** Private header for Cell Style class
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
** Started on  Thu Apr  6 12:50:35 1995 Stéphane Boisson
** Last update Sat Oct 26 18:54:08 1996 Emmanuel Paris
*/

#ifndef __XC_CELL_STYLE_P_H__
#define __XC_CELL_STYLE_P_H__

#include "CellStyle.h"

/*--- Read & write methods (docfile.c) ---*/
boolean readCellStyle ___PROTO((c_DocFile *doc, char *keyword,
				long param, BaseStd_t *base));
boolean writeCellStyle ___PROTO((CellStyle_t *this, c_DocFile *doc));
boolean readCellStyleList ___PROTO((c_DocFile *doc, char *keyword,
				    long param, BaseStd_t *base));
boolean writeCellStyleList ___PROTO((c_CellStyle *this, c_DocFile *doc));

/*--- Print method (print.c) ---*/
boolean printCSBackground ___PROTO((CellStyle_t *this, c_PostScript *post,
				    bbox_t *bbox));
void printCSAddBorders ___PROTO((CellStyle_t *this, moteur_print_t *info,
				 region_t *region));
boolean printCSFlushBorders ___PROTO((moteur_print_t *info,
				      c_PostScript *post, region_t *region));
boolean printCSText ___NPROTO((CellStyle_t *this, c_PostScript *post,
			       bbox_t *bbox, char *text, int value_type,
			       boolean clip_flag));
void printCSClearBorders ___PROTO((moteur_print_t *info, region_t *region));
void printCSSetCellMargins ___PROTO((moteur_print_t *info, bbox_t *bbox,
				     int x, int y));
void printCSRemoveBorder ___PROTO((moteur_print_t *info, region_t *region));


#endif /* !__XC_CELL_STYLE_P_H__ */
