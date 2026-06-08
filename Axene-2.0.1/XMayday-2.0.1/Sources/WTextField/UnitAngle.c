/*
** UnitAngle.c for Xclamation, XQuad, XAllWrite and XMayday in WTextField/
** Routines for Angle Unit Convertion in WTextField
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
** Last update Thu Mar 27 15:43:24 1997 Emmanuel Paris
*/

#include "UnitConvertion.h"

/* ------------- convertion for DEGREE ------------------ */

real str_to_degree(pulist)
parse_unit_t	*pulist;
{
 if (pulist->type == XcPU_REAL)
  return (real)SCALE_FROM_DEGREES(pulist->pu_real.value);
 else
  return (real)SCALE_FROM_DEGREES(pulist->pu_integer.value);
}

char *degree_to_str(unit_def, value)
char *unit_def;
real value;
{
 char	str[100];
 
 sprintf(str, unit_def, SCALE_TO_DEGREES(value));
 return Xc_strdup("d2str", str);
}

/* ------------- convertion for RADIAN ------------------ */

real str_to_radian(pulist)
parse_unit_t	*pulist;
{
 if (pulist->type == XcPU_REAL)
  return (real)SCALE_FROM_RADIANS(pulist->pu_real.value);
 else
  return (real)SCALE_FROM_RADIANS(pulist->pu_integer.value);
}

char *radian_to_str(unit_def, value)
char *unit_def;
real value;
{
 char	str[100];
 
 sprintf(str, unit_def, SCALE_TO_RADIANS(value));
 return Xc_strdup("rad2str", str);
}

/* ------------- convertion for GRAD ------------------ */

real str_to_grad(pulist)
parse_unit_t	*pulist;
{
 if (pulist->type == XcPU_REAL)
  return (real)SCALE_FROM_DEGREES(pulist->pu_real.value) * 180.0 / 200.0;
 else
  return (real)SCALE_FROM_DEGREES(pulist->pu_integer.value) * 180.0 / 200.0;
}

char *grad_to_str(unit_def, value)
char *unit_def;
real value;
{
 char	str[100];
 
 sprintf(str, unit_def, SCALE_TO_DEGREES(value) * 200.0 / 180.0);
 return Xc_strdup("g2str", str);
}










