/*
** UnitLength.h for Xclamation, XQuad, XAllWrite and XMayday in WTextField/
** Header for Length Unit Convertion in WTextField
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
** Started on  Sun Aug 18 15:08:04 1996 Emmanuel Paris
** Last update Tue Aug 20 18:35:05 1996 Emmanuel Paris
*/

#ifndef _UnitLength_h_
#define _UnitLength_h_

#include "UnitConvertion.h"

enum {
  XcUC_LENGTH_POINT,
  XcUC_LENGTH_MILLIMETER,
  XcUC_LENGTH_CENTIMETER,
  XcUC_LENGTH_DECIMETER,
  XcUC_LENGTH_METER,
  XcUC_LENGTH_INCH,
  XcUC_LENGTH_INCH2,
  XcUC_LENGTH_PICA,
  XcUC_LENGTH_PICA2,
  XcUC_LENGTH_CICERO,
  XcUC_LENGTH_CICERO2
};

#ifdef XcUC_UNIT_DEF

extern real str_to_points();
extern char *points_to_str();
extern real str_to_millimeters();
extern char *millimeters_to_str();
extern real str_to_centimeters();
extern char *centimeters_to_str();
extern real str_to_decimeters();
extern char *decimeters_to_str();
extern real str_to_meters();
extern char *meters_to_str();
extern real str_to_inches();
extern char *inches_to_str();
extern real str_to_picas();
extern char *picas_to_str();
extern real str_to_ciceros();
extern char *ciceros_to_str();

static unit_conv_t unit_length[] = 
{ { "%g pt", str_to_points, points_to_str },
  { "%g mm", str_to_millimeters, millimeters_to_str },
  { "%g cm", str_to_centimeters, centimeters_to_str },
  { "%g dm", str_to_decimeters, decimeters_to_str },
  { "%g m" , str_to_meters, meters_to_str },
  { "%g in", str_to_inches, inches_to_str },
  { "%g\"" , str_to_inches, inches_to_str },
  { "%dp%g", str_to_picas, picas_to_str },
  { "%dp"  , str_to_picas, picas_to_str },
  { "%dc%g", str_to_ciceros, ciceros_to_str },
  { "%dc"  , str_to_ciceros, ciceros_to_str },
  { NULL, NULL, NULL }
};

#else	/* ! XcUC_UNIT_DEF */

extern unit_conv_t unit_length[];

#endif

#endif /* _UnitLength_h_ */



























