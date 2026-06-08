/*
** UnitAngle.h for Xclamation, XQuad, XAllWrite and XMayday in WTextField/
** Header for Angle Unit Convertion in WTextField
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
** Last update Thu Mar 27 15:38:23 1997 Emmanuel Paris
*/

#ifndef _UnitAngle_h_
#define _UnitAngle_h_

#include "UnitConvertion.h"

#define XcUC_ANGLE_DEGREE	0
#define XcUC_ANGLE_RADIAN	1
#define XcUC_ANGLE_GRAD		2

#ifdef XcUC_UNIT_DEF

extern real str_to_degree();
extern char *degree_to_str();
extern real str_to_radian();
extern char *radian_to_str();
extern real str_to_grad();
extern char *grad_to_str();

static unit_conv_t unit_angle[] = 
{ { "%g°"  , str_to_degree, degree_to_str },
  { "%grad", str_to_radian, radian_to_str },
  { "%gg", str_to_grad, grad_to_str },
  { NULL, NULL, NULL }
};

#else	/* ! XcUC_UNIT_DEF */

extern unit_conv_t unit_angle[];

#endif

#endif /* _UnitAngle_h_ */
