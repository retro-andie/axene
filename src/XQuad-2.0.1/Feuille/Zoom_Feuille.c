/*
** Zoom_Feuille.c for XQuad in Feuille/
** Methods for the Feuile class
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
** Started on  Fri Sep 30 15:33:47 1994 Emmanuel Paris
** Last update Wed May 31 13:33:40 1995 Emmanuel Paris
*/

#include "Zoom_Feuille.h"

void zoom_feuille_selection(This)
     c_Feuille	*This;
{
  change_feuille_scaling(This,
		      SCALE_FROM_PERCENT(100),
		      This->cell_x, This->cell_y);
}

void zoom_feuille_at_percent(This, percent)
     c_Feuille	*This;
     int	percent;
{
  if (This->sFactor!=SCALE_FROM_PERCENT(percent))
  {
    change_feuille_scaling(This,
			SCALE_FROM_PERCENT(percent),
			This->column_orig, This->row_orig);
  }
}

void zoom_feuille_plus(This)
c_Feuille *This;
{
  real factor;

  factor = SCALE_TO_PERCENT(This->sFactor) + ZOOM_STEP;
  if (factor>ZOOM_PLUS_LIMIT)
      factor = ZOOM_PLUS_LIMIT;
  if (This->sFactor!=SCALE_FROM_PERCENT(factor))
    {
      change_feuille_scaling(This,
			     SCALE_FROM_PERCENT(factor),
			     This->column_orig, This->row_orig);
    }
}

void zoom_feuille_minus(This)
c_Feuille *This;
{
  real factor;

  factor = SCALE_TO_PERCENT(This->sFactor) - ZOOM_STEP;
  if (factor<ZOOM_MINUS_LIMIT)
    factor = ZOOM_MINUS_LIMIT;
  if (This->sFactor!=SCALE_FROM_PERCENT(factor))
    {
      change_feuille_scaling(This,
			     SCALE_FROM_PERCENT(factor),
			     This->column_orig, This->row_orig);
    }
}
