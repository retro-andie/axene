/*
** UnitRatio.h for Xclamation, XQuad, XAllWrite and XMayday in WTextField/
** Header for Ratio Unit Convertion in WTextField
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
** Last update Thu Mar 27 15:11:43 1997 Emmanuel Paris
*/

#ifndef _UnitRatio_h_
#define _UnitRatio_h_

#include "UnitConvertion.h"

#define XcUC_RATIO_PERCENTAGE	0
#define XcUC_RATIO_FRACTION	1

#ifdef XcUC_UNIT_DEF

extern real str_to_percentage();
extern char *percentage_to_str();
extern real str_to_fraction();
extern char *fraction_to_str();

static unit_conv_t unit_ratio[] = 
{ { "%g%%"  , str_to_percentage, percentage_to_str },
  { "%g/%d", str_to_fraction, fraction_to_str },
  { NULL, NULL, NULL }
};

#else	/* ! XcUC_UNIT_DEF */

extern unit_conv_t unit_ratio[];

#endif

#endif /* _UnitRatio_h_ */
