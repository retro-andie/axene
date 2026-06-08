/*
** UnitRatio.c for Xclamation, XQuad, XAllWrite and XMayday in WTextField/
** Routines for Ratio Unit Convertion in WTextField
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
** Last update Thu Dec 30 18:14:06 1999 Emmanuel Paris
*/

#include "UnitConvertion.h"

/* ------------- convertion for PERCENTAGE ------------------ */

real str_to_percentage(pulist)
parse_unit_t	*pulist;
{
 if (pulist->type == XcPU_REAL)
  return (real)pulist->pu_real.value / 100.0;
 else
  return (real)pulist->pu_integer.value / 100.0;
}

char *percentage_to_str(unit_def, value)
char *unit_def;
real value;
{
 char	str[100];
  
 sprintf(str, unit_def, SCALE_TO_PERCENT(value));
 return Xc_strdup("%2str", str);
}

/* ------------- convertion for FRACTION ------------------ */

real str_to_fraction(pulist)
parse_unit_t	*pulist;
{
 real	num, denom;
  
 if (pulist->type == XcPU_REAL)
  num = (real)pulist->pu_real.value;
 else
  num = (real)pulist->pu_integer.value;
 pulist = pulist->pu_any.next->pu_any.next;
 denom = (real)pulist->pu_integer.value;
  
 if (denom == 0.0)
 {
  if (num > 0.0)
   return 1e+6;
  else
   return -1e+6;
 }
 return num / denom;
}

char *fraction_to_str(unit_def, value)
char *unit_def;
real value;
{
 char	str[100];
 real	num, denom;
 real sign = 1.0;
  
 if (value == 0.0)
  return Xc_strdup("/2str", "0/1");
  
 if (value < 0.0)
  sign = -1.0;
 value = ABS(value);
 denom = pow( 10.0, floor( log10( value ) ));
 if (denom < 1.0)
  denom = 1.0;
 num = value * denom * sign;
  
 sprintf(str, unit_def, num, (int)(ceil(denom)));
 return Xc_strdup("/2str", str);
}









