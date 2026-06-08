/*
** Select_Column_Region_For_EditBar.c for XQuad in Feuille/
** Select And Deselect Column region
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
** Last update Thu Apr 20 15:56:20 1995 Emmanuel Paris
*/

#include "Select_Region_For_EditBar.h"
#include "Select_Cells_Region_For_EditBar.h"
#include "Select_Row_Region_For_EditBar.h"

void select_column_region_for_editbar_init();
void select_column_region_for_editbar_abort();
void select_column_region_for_editbar_done();
void select_column_region_for_editbar_to_xy();
void select_column_region_for_editbar_trace();

void select_column_region_for_editbar_init(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	  xm;
int	  ym;
{
  int		column;
  region_t	*region_column;
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);
    
  if (!d(cntrl))
  {
    if (d(region))
      select_cells_region_for_editbar_abort(cfeuille);
    if (d(region_column))
      select_column_region_for_editbar_abort(cfeuille);
    if (d(region_row))
      select_row_region_for_editbar_abort(cfeuille);
     
    d(region_column) = (region_t *)Xc_malloc("SRFregion", sizeof(region_t));
    d(region_column)->NextRegion = NULL;
  }
  else
  {
    if (d(region_column))
    {
      region_column = (region_t *)Xc_malloc("SRFregion", sizeof(region_t));
      region_column->NextRegion = d(region_column);
      d(region_column) = region_column;
    }
    else
    {
      d(region_column) = (region_t *)Xc_malloc("SRFregion", sizeof(region_t));
      d(region_column)->NextRegion = NULL;
    }
  }
  F(cfeuille).GetColumnByX(cfeuille, xm, &column);
  
  d(column) = column;
  d(prems) = TRUE;
  d(source) = TRUE;
  
  select_column_region_for_editbar_to_xy(cfeuille, xm, ym);  
  
  F(GlobEditBar).NewSelect(GlobEditBar, d(region_column), Xq_COLUMN, d(cntrl));

  d(AutoScroll) = (c_AutoScroll *)NEW(c_AutoScroll)
    (cfeuille->X_info.display, 0, 0,
     cfeuille->sheet_width, 0, cfeuille->ScrollBarH, NULL,
     eventlook_Select_Region_For_EditBar, cfeuille);
}

void select_column_region_for_editbar_abort(cfeuille)
c_Feuille *cfeuille;
{ 
  region_t	*region_column, *region_column_tmp;
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);
  
  if (d(AutoScroll))
    {
      DELETE(c_AutoScroll)(d(AutoScroll));
      d(AutoScroll) = (c_AutoScroll *)NULL;
    }  
  
  select_column_region_for_editbar_trace(cfeuille, TRUE);
  region_column = d(region_column);
  while(region_column)
    {
      region_column_tmp = region_column->NextRegion;
      Xc_free(region_column);
      region_column = region_column_tmp;
    }
  d(region_column) = NULL;
  d(source) = FALSE;
}

void select_column_region_for_editbar_done(cfeuille)
c_Feuille *cfeuille;
{  
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);

  if (d(AutoScroll))
    {
      DELETE(c_AutoScroll)(d(AutoScroll));
      d(AutoScroll) = (c_AutoScroll *)NULL;
    }
  d(source)=FALSE;
}

void select_column_region_for_editbar_to_xy(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	xm;
int	ym;
{ 
  int	column;
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);

  if (xm < 0)
    xm = 0;
  if (xm >= cfeuille->sheet_width)
    xm = cfeuille->sheet_width - 1;
  if (ym < 0 || ym >= cfeuille->origin_height)
    ym = 1;
  
  if (F(cfeuille).GetColumnByX(cfeuille, xm, &column) )
  {
    if (column!=d(xc) || d(prems))
    {
      d(xc) = column;

      if (d(prems))
      {
	d(region_column)->x_min = d(region_column)->x_max = column;
	d(region_column)->y_min = 0; d(region_column)->y_max = ROW_MAX;
	
	select_column_region_for_editbar_trace(cfeuille,FALSE);
	d(prems) = FALSE;
	return;
      }
      select_column_region_for_editbar_trace(cfeuille,FALSE);
      d(region_column)->x_min = MIN(d(column),column);
      d(region_column)->x_max = MAX(d(column),column);
      select_column_region_for_editbar_trace(cfeuille,FALSE);
      F(GlobEditBar).SelectMove(GlobEditBar, d(region_column), Xq_COLUMN);
    }
  }
}

void select_column_region_for_editbar_trace(cfeuille, all_r)
     c_Feuille *cfeuille;
     boolean	all_r;
{  
 Display	*display;
 Window		window;
 GC		gc;
 region_t	*region_column;
 int		x;
 DATA(Select_Region_For_EditBar);
 
 GET_DATA(Select_Region_For_EditBar);
 display=cfeuille->X_info.display;
 window=cfeuille->X_info.window;
 gc=cfeuille->X_info.gc_contour;
 
 region_column = d(region_column);
 while(region_column)
   {
     if (region_column->x_min >= cfeuille->column_orig &&
	 region_column->x_min < cfeuille->column_orig+cfeuille->nb_column)
       {
	 x = get_column_pos(cfeuille, region_column->x_min); 
	 XDrawLine(display, window, gc, x, 0, x, cfeuille->sheet_height);
       }
     if (region_column->x_max+1 >= cfeuille->column_orig &&
	 region_column->x_max+1 < cfeuille->column_orig+cfeuille->nb_column)
       {
	 x = get_column_pos(cfeuille, region_column->x_max+1);
	 XDrawLine(display, window, gc, x, 0, x, cfeuille->sheet_height);
       }
   if (!all_r)
       break;
     region_column = region_column->NextRegion;
   }
}

