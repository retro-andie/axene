/*
** Focus_Region_Utils.c for XQuad in Feuille/
** Focus Region optimized
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
** Started on  Mon Feb 20 11:56:48 1995 Emmanuel Paris
** Last update Tue Oct  3 12:41:31 1995 Stéphane Boisson
*/

#include "Feuille.h"

void focus_cell_region_optimized(This, region, col1, row1, col2, row2)
c_Feuille	*This;
region_t	*region;
int		col1;
int		row1;
int		col2;
int		row2;
{
  static region_t aff={0, 0, 0, 0}, eff={0, 0, 0, 0}; 
  int	xA, xB, yA, yB;
  
  xA = region->x_min;
  xB = region->x_max;
#define xC col2
  yA = region->y_min;
  yB = region->y_max;
#define yC row2 

# define EFFACE(xmin, xmax, ymin, ymax) \
	    eff.x_min = (xmin); \
	    eff.x_max = (xmax); \
	    eff.y_min = (ymin); \
	    eff.y_max = (ymax); \
	    focus_out_region(This, &eff, Xq_CELL);
# define AFFICHE(xmin, xmax, ymin, ymax) \
	    aff.x_min = (xmin); \
	    aff.x_max = (xmax); \
	    aff.y_min = (ymin); \
	    aff.y_max = (ymax); \
	    focus_in_region(This, &aff, Xq_CELL);	  
    	  
  if (xA == col1)
  {
/* ----------------------------------------------------------------- */
    if (xC > xB)
    {
      if (yA == row1)
      {
	if (yC > yB)
	{
	  AFFICHE( xA, xC, yB+1, yC);
	  AFFICHE( xB+1, xC, yA, yB);
	}
	else
	{
	  if (yC < yA)
	  {
	    EFFACE( xA, xB, yA+1,yB);
	    AFFICHE( xA, xC, yC, yA-1);
	    AFFICHE( xB+1, xC, yA, yA);
	  }
	  else
	  {
	    EFFACE( xA, xB, yC+1, yB);
	    AFFICHE( xB+1, xC, yA, yC);
	  }
	}
      }
      else
      {
	if (yC > yB)
	{
	  EFFACE( xA, xB, yA, yB-1);
	  AFFICHE( xA, xC, yB+1, yC);
	  AFFICHE( xB+1, xC, yB, yB);
	}
	else
	{
	  if (yC < yA)
	  {
	    AFFICHE( xA, xC, yC, yA-1);
	    AFFICHE( xB+1, xC, yA, yB);
	  }
	  else
	  {
	    EFFACE( xA, xB, yA, yC-1);
	    AFFICHE( xB+1, xC, yC, yB);
	  }
	}
      }
    }
    else
    {
/* ----------------------------------------------------------------- */
      if (xC < xA)
      {
	if (yA == row1)
	{
	  if (yC > yB)
	  {
	    EFFACE( xA+1, xB, yA, yB);
	    AFFICHE( xC, xA-1, yA, yC);
	    AFFICHE( xA, xA, yB+1, yC);
	  }
	  else
	  {
	    if (yC < yA)
	    {
	      EFFACE( xA, xB, yA+1, yB);
	      EFFACE( xA+1, xB, yA, yA);
	      AFFICHE( xC, xA, yC, yA-1);
	      AFFICHE( xC, xA-1, yA, yA);
	    }
	    else
	    {
	      EFFACE( xA, xB, yC+1, yB);
	      EFFACE( xA+1, xB, yA, yC);
	      AFFICHE( xC, xA-1, yA, yC);
	    }
	  }
	}
	else
	{
	  if (yC > yB)
	  {
	    EFFACE( xA, xB, yA, yB-1);
	    EFFACE( xA+1, xB, yB, yB);
	    AFFICHE( xC, xA-1, yB, yC);
	    AFFICHE( xA, xA, yB+1, yC);
	  }
	  else
	  {
	    if (yC < yA)
	    {
	      EFFACE( xA+1, xB, yA, yB);
	      AFFICHE( xC, xA-1, yC, yB);
	      AFFICHE( xA, xA, yC, yA-1);
	    }
	    else
	    {
	      EFFACE( xA+1, xB, yA, yB);
	      EFFACE( xA, xA, yA, yC-1);
	      AFFICHE( xC, xA-1, yC, yB);
	    }
	  }
	}
      }
      else
      {
/* ----------------------------------------------------------------- */
	if (yA == row1)
	{
	  if (yC > yB)
	  {
	    EFFACE( xC+1, xB, yA, yB);
	    AFFICHE( xA, xC, yB+1, yC);
	  }
	  else
	  {
	    if (yC < yA)
	    {
	      EFFACE( xA, xB, yA+1, yB);
	      EFFACE( xC+1, xB, yA, yA);
	      AFFICHE( xA, xC, yC, yA-1);
	    }
	    else
	    {
	      EFFACE( xA, xB, yC+1, yB);
	      EFFACE( xC+1, xB, yA, yB);
	    }
	  }
	}
	else
	{
	  if (yC > yB)
	  {
	    EFFACE( xA, xB, yA, yB-1);
	    EFFACE( xC+1, xB, yB, yB);
	    AFFICHE( xA, xC, yB+1, yC);
	  }
	  else
	  {
	    if (yC < yA)
	    {
	      EFFACE( xC+1, xB, yA, yB);
	      AFFICHE( xA, xC, yC, yA-1);
	    }
	    else
	    {
	      EFFACE( xA, xB, yA, yC-1);
	      EFFACE( xC+1, xB, yC, yB);
	    }
	  }
	}
      }
    }
  }
  else
  {
/* ----------------------------------------------------------------- */
    if (xC > xB)
    {
      if (yA == row1)
      {
	if (yC > yB)
	{
	  EFFACE( xA, xB-1, yA, yB);
	  AFFICHE( xB+1, xC, yA, yC);
	  AFFICHE( xB, xB, yB+1, yC);
	}
	else
	{
	  if (yC < yA)
	  {
	    EFFACE( xA, xB, yA+1, yB);
	    EFFACE( xA, xB-1, yA, yA);
	    AFFICHE( xB, xC, yC, yA-1);
	    AFFICHE( xB+1, xC, yA, yA);
	  }
	  else
	  {
	    EFFACE( xA, xB-1, yA, yB);
	    EFFACE( xB, xB, yC+1, yB);
	    AFFICHE( xB+1, xC, yA, yC);
	  }
	}
      }
      else
      {
	if (yC > yB)
	{
	  EFFACE( xA, xB, yA, yB-1);
	  EFFACE( xA, xB-1, yB, yB);
	  AFFICHE( xB, xC, yB+1, yC);
	  AFFICHE( xB+1, xC, yB, yB);
	}
	else
	{
	  if (yC < yA)
	  { 
	    EFFACE( xA, xB-1, yA, yB);
	    AFFICHE( xB+1, xC, yC, yB);
	    AFFICHE( xB, xB, yC, yA-1);
	  }
	  else
	  {
	    EFFACE( xA, xB-1, yA, yB);
	    EFFACE( xB, xB, yA, yC-1);
	    AFFICHE( xB+1, xC, yC, yB);
	  }
	}
      }
    }
    else
    {
      /* ----------------------------------------------------------------- */
      if (xC < xA)
      {
	if (yA == row1)
	{
	  if (yC > yB)
	  {
	    AFFICHE( xC, xB, yB+1, yC);
	    AFFICHE( xC, xA-1, yA, yB);
	  }
	  else
	  {
	    if (yC < yA)
	    {
	      EFFACE( xA, xB, yA+1, yB);
	      AFFICHE( xC, xB, yC, yA-1);
	      AFFICHE( xC, xA-1, yA, yA);
	    }
	    else
	    {
	      EFFACE( xA, xB, yC+1, yB);
	      AFFICHE( xC, xA-1, yA, yC);
	    }
	  }
	}
	else
	{
	  if (yC > yB)
	  {
	    EFFACE( xA, xB, yA, yB-1);
	    AFFICHE( xC, xB, yB+1, yC);
	    AFFICHE( xC, xA-1, yB, yB);
	  }
	  else
	  {
	    if (yC < yA)
	    {
	      AFFICHE( xC, xB, yC, yA-1);
	      AFFICHE( xC, xA-1, yA, yB);
	    }
	    else
	    {
	      EFFACE( xA, xB, yA, yC-1);
	      AFFICHE( xC, xA-1, yC, yB);
	    }
	  }
	}
      }
      else
      {
/* ----------------------------------------------------------------- */
	if (yA == row1)
	{
	  if (yC > yB)
	  {
	    EFFACE( xA, xC-1, yA, yB);
	    AFFICHE( xC, xB, yB+1, yC);
	  }
	  else
	  {
	    if (yC < yA)
	    {
	      EFFACE( xA, xB, yA+1, yB);
	      EFFACE( xA, xC-1, yA, yA);
	      AFFICHE( xC, xB, yC, yA-1);
	    }
	    else
	    {
	      EFFACE( xA, xC-1, yA, yB);
	      EFFACE( xC, xB, yC+1, yB);
	    }
	  }
	}
	else
	{
	  if (yC > yB)
	  {
	    EFFACE( xA, xB, yA, yB-1);
	    EFFACE( xA, xC-1, yB, yB);
	    AFFICHE( xC, xB, yB+1, yC);
	  }
	  else
	  {
	    if (yC < yA)
	    {
	      EFFACE( xA, xC-1, yA, yB);
	      AFFICHE( xC, xB, yC, yA-1);
	    }
	    else
	    {
	      EFFACE( xA, xB, yA, yC-1);
	      EFFACE( xA, xC-1, yC, yB);
	    }
	  }
	}
      }
    }
  }
#undef xC
#undef yC
#undef EFFACE
#undef AFFICHE
}

void focus_column_region_optimized(This, region, col1, col2)
c_Feuille	*This;
region_t	*region;
int		col1;
int		col2;
{
  static region_t aff={0, 0, 0, ROW_MAX}, eff={0, 0, 0, ROW_MAX};
  int	A, B;
  
  A = region->x_min;
  B = region->x_max;
#define C col2
  if (A == col1)
  {
    if ( C > B)
    {
      aff.x_min = B+1;
      aff.x_max = C;
      focus_in_region(This, &aff, Xq_CELL);
      focus_in_region(This, &aff, Xq_COLUMN);
    }
    else
    {
      if (C < A)
      {
	eff.x_min = A+1;
	eff.x_max = B;
	focus_out_region(This, &eff, Xq_CELL);
	focus_out_region(This, &eff, Xq_COLUMN);
	aff.x_min = C;
	aff.x_max = A-1;
	focus_in_region(This, &aff, Xq_CELL);
	focus_in_region(This, &aff, Xq_COLUMN);
      }
      else
      {
	eff.x_min = C+1;
	eff.x_max = B;
	focus_out_region(This, &eff, Xq_CELL);
	focus_out_region(This, &eff, Xq_COLUMN);
      }
    }
  }
  else
  {
    if (C > B)
    {
      eff.x_min = A;
      eff.x_max = B-1;
      focus_out_region(This, &eff, Xq_CELL);
      focus_out_region(This, &eff, Xq_COLUMN);
      aff.x_min = B+1;
      aff.x_max = C;
      focus_in_region(This, &aff, Xq_CELL);
      focus_in_region(This, &aff, Xq_COLUMN);
    }
    else
    {
      if (C < A)
      {
	aff.x_min = C;
	aff.x_max = A-1;
	focus_in_region(This, &aff, Xq_CELL);
	focus_in_region(This, &aff, Xq_COLUMN);
      }
      else
      {
	eff.x_min = A;
	eff.x_max = C-1;
	focus_out_region(This, &eff, Xq_CELL);
	focus_out_region(This, &eff, Xq_COLUMN);
      }
    }
  }
#undef C
}
  
void focus_row_region_optimized(This, region, row1, row2)
c_Feuille	*This;
region_t	*region;
int		row1;
int		row2;
{
  static region_t aff={0,COLUMN_MAX,0,0}, eff={0,COLUMN_MAX,0,0};
  int	A, B;
  
  A = region->y_min;
  B = region->y_max;
#define C row2
  
  if (A == row1)
  {
    if (C > B)
    {
      aff.y_min = B+1;
      aff.y_max = C;
      focus_in_region(This, &aff, Xq_CELL);
      focus_in_region(This, &aff, Xq_ROW);
    }
    else
    {
      if (C < A)
      {
	eff.y_min = A+1;
	eff.y_max = B;
	focus_out_region(This, &eff, Xq_CELL);
	focus_out_region(This, &eff, Xq_ROW);
	aff.y_min = C;
	aff.y_max = A-1;
	focus_in_region(This, &aff, Xq_CELL);
	focus_in_region(This, &aff, Xq_ROW);
      }
      else
      {
	eff.y_min = C+1;
	eff.y_max = B;
	focus_out_region(This, &eff, Xq_CELL);
	focus_out_region(This, &eff, Xq_ROW);
      }
    }
  }
  else
  {
    if (C > B)
    {
      eff.y_min = A;
      eff.y_max = B-1;
      focus_out_region(This, &eff, Xq_CELL);
      focus_out_region(This, &eff, Xq_ROW);
      aff.y_min = B+1;
      aff.y_max = C;
      focus_in_region(This, &aff, Xq_CELL);
      focus_in_region(This, &aff, Xq_ROW);
    }
    else
    {
      if (C < A)
      {
	aff.y_min = C;
	aff.y_max = A-1;
	focus_in_region(This, &aff, Xq_CELL);
	focus_in_region(This, &aff, Xq_ROW);
      }
      else
      {
	eff.y_min = A;
	eff.y_max = C-1;
	focus_out_region(This, &eff, Xq_CELL);
	focus_out_region(This, &eff, Xq_ROW);
      }
    }
  }
#undef C
}


