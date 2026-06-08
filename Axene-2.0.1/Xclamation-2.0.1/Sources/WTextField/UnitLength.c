/*
** UnitLength.c for Xclamation, XQuad, XAllWrite and XMayday in WTextField/
** Routines for Length Unit Convertion in WTextField
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
** Last update Tue Aug 20 18:33:25 1996 Emmanuel Paris
*/

#include "UnitConvertion.h"

/* ------------- convertion for POINTS ------------------ */

real str_to_points(pulist)
parse_unit_t	*pulist;
{
  if (pulist->type == XcPU_REAL)
    return (real)pulist->pu_real.value * (real)COORD_SCALEVALUE;
  else
    return (real)pulist->pu_integer.value * (real)COORD_SCALEVALUE;
}

char *points_to_str(unit_def, value)
char *unit_def;
real value;
{
  char	str[100];
  
  sprintf(str, unit_def, SCALE_TO_POINTS(value));
  return Xc_strdup("pt2str", str);
}

/* ------------- convertion for MILLIMETERS ------------------ */

real str_to_millimeters(pulist)
parse_unit_t	*pulist;
{
  if (pulist->type == XcPU_REAL)
    return (real)pulist->pu_real.value /
      (25.4 / (real)(COORD_SCALEVALUE * 72));
  else
    return (real)pulist->pu_integer.value /
      (25.4 / (real)(COORD_SCALEVALUE * 72));
}

char *millimeters_to_str(unit_def, value)
char *unit_def;
real value;
{
  char	str[100];
  
  sprintf(str, unit_def, SCALE_TO_MILLIMETERS(value));
  return Xc_strdup("mm2str", str);
}

/* ------------- convertion for CENTIMETERS ------------------ */

real str_to_centimeters(pulist)
parse_unit_t	*pulist;
{
  if (pulist->type == XcPU_REAL)
    return (real)pulist->pu_real.value /
      (2.54 / (real)(COORD_SCALEVALUE * 72));
  else
    return (real)pulist->pu_integer.value /
      (2.54 / (real)(COORD_SCALEVALUE * 72));
}

char *centimeters_to_str(unit_def, value)
char *unit_def;
real value;
{
  char	str[100];
  
  sprintf(str, unit_def, SCALE_TO_CENTIMETERS(value));
  return Xc_strdup("cm2str", str);
}

/* ------------- convertion for DECIMETERS ------------------ */

real str_to_decimeters(pulist)
parse_unit_t	*pulist;
{
  if (pulist->type == XcPU_REAL)
    return (real)pulist->pu_real.value /
      (0.254 / (real)(COORD_SCALEVALUE * 72));
  else
    return (real)pulist->pu_integer.value /
      (0.254 / (real)(COORD_SCALEVALUE * 72));
}

char *decimeters_to_str(unit_def, value)
char *unit_def;
real value;
{
  char	str[100];
  
  sprintf(str, unit_def, SCALE_TO_CENTIMETERS(value) / 10.0);
  return Xc_strdup("dm2str", str);
}

/* ------------- convertion for CENTIMETERS ------------------ */

real str_to_meters(pulist)
parse_unit_t	*pulist;
{
  if (pulist->type == XcPU_REAL)
    return (real)pulist->pu_real.value /
      (0.0254 / (real)(COORD_SCALEVALUE * 72));
  else
    return (real)pulist->pu_integer.value /
      (0.0254 / (real)(COORD_SCALEVALUE * 72));
}

char *meters_to_str(unit_def, value)
char *unit_def;
real value;
{
  char	str[100];
  
  sprintf(str, unit_def, SCALE_TO_CENTIMETERS(value) / 100.0);
  return Xc_strdup("m2str", str);
}

/* ------------- convertion for INCHES ------------------ */

real str_to_inches(pulist)
parse_unit_t	*pulist;
{
  if (pulist->type == XcPU_REAL)
    return (real)pulist->pu_real.value * (real)(COORD_SCALEVALUE * 72);
  else
    return (real)pulist->pu_integer.value * (real)(COORD_SCALEVALUE * 72);
}

char *inches_to_str(unit_def, value)
char *unit_def;
real value;
{
  char	str[100];
  
  sprintf(str, unit_def, SCALE_TO_INCHES(value));
  return Xc_strdup("inch2str", str);
}

/* ------------- convertion for PICAS ------------------ */

real str_to_picas(pulist)
parse_unit_t	*pulist;
{
  real result;
  
  result = 4.212 * pulist->pu_integer.value;
  if (pulist->pu_any.next && pulist->pu_any.next->pu_any.next)
  {
    pulist = pulist->pu_any.next->pu_any.next;
    if (pulist->type == XcPU_REAL)
      result += 0.351 * pulist->pu_real.value;
    else
      result += 0.351 * pulist->pu_integer.value;
  }
  return (real)result / (25.4 / (real)(COORD_SCALEVALUE * 72));
}

char *picas_to_str(unit_def, value)
char *unit_def;
real value;
{
  char	str[100];
  int	pica;
  real	pica_point;
  
  pica_point = SCALE_TO_MILLIMETERS(value) / 0.351;
  if (pica_point > 0)
    pica = (int)floor(pica_point / 12.0);
  else
    pica = (int)ceil(pica_point / 12.0);
  pica_point = pica_point - (pica * 12);
  
  sprintf(str, unit_def, pica, pica_point);
  return Xc_strdup("pica2str", str);
}

/* ------------- convertion for CICEROS ------------------ */

real str_to_ciceros(pulist)
parse_unit_t	*pulist;
{
  real result;
  
  result = 4.5108 * pulist->pu_integer.value;
  
  if (pulist->pu_any.next && pulist->pu_any.next->pu_any.next)
  {
    pulist = pulist->pu_any.next->pu_any.next;
    if (pulist->type == XcPU_REAL)
      result += 0.3759 * pulist->pu_real.value;
    else
      result += 0.3759 * pulist->pu_integer.value;
  }
  
  return (real)result / (25.4 / (real)(COORD_SCALEVALUE * 72));
}

char *ciceros_to_str(unit_def, value)
char *unit_def;
real value;
{
  char	str[100];
  int	cicero;
  real	didot;
  
  didot = SCALE_TO_MILLIMETERS(value) / 0.3759;
  if (didot > 0)
    cicero = (int)floor(didot / 12.0);
  else
    cicero = (int)ceil(didot / 12.0);
  
  didot = didot - (cicero * 12);
  
  sprintf(str, unit_def, cicero, didot);
  return Xc_strdup("pica2str", str);
}










