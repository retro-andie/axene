/*
** Select_Row_Region_For_EditBar.c for XQuad in Feuille/
** Select And Deselect Row region
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
** Last update Thu Apr 20 15:41:18 1995 Emmanuel Paris
*/

#include "Select_Region_For_EditBar.h"
#include "Select_Cells_Region_For_EditBar.h"
#include "Select_Column_Region_For_EditBar.h"

void select_row_region_for_editbar_init();
void select_row_region_for_editbar_abort();
void select_row_region_for_editbar_done();
void select_row_region_for_editbar_to_xy();
void select_row_region_for_editbar_trace();

void select_row_region_for_editbar_init(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	  xm;
int	  ym;
{
  int		row;
  region_t	*region_row;
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
    
    d(region_row) = (region_t *)Xc_malloc("SRFregion", sizeof(region_t));
    d(region_row)->NextRegion = NULL;
  }
  else
  {
    if (d(region_row))
    {
      region_row = (region_t *)Xc_malloc("SRFregion", sizeof(region_t));
      region_row->NextRegion = d(region_row);
      d(region_row) = region_row;
    }
    else
    {
      d(region_row) = (region_t *)Xc_malloc("SRFregion", sizeof(region_t));
      d(region_row)->NextRegion = NULL;
    }
  }
  F(cfeuille).GetRowByY(cfeuille, ym, &row);
    
  d(row) = row;
  d(prems) = TRUE;
  d(source) = TRUE;
  
  select_row_region_for_editbar_to_xy(cfeuille, xm, ym);  
  
  F(GlobEditBar).NewSelect(GlobEditBar, d(region_row), Xq_ROW, d(cntrl));

    d(AutoScroll) = (c_AutoScroll *)NEW(c_AutoScroll)
    (cfeuille->X_info.display, 0, 0, 
     0, cfeuille->sheet_height, NULL, cfeuille->ScrollBarV, 
     eventlook_Select_Region_For_EditBar, cfeuille);
}

void select_row_region_for_editbar_abort(cfeuille)
c_Feuille *cfeuille;
{ 
  region_t	*region_row, *region_row_tmp;
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);
  
  if (d(AutoScroll))
    {
      DELETE(c_AutoScroll)(d(AutoScroll));
      d(AutoScroll) = (c_AutoScroll *)NULL;
    }  

  select_row_region_for_editbar_trace(cfeuille, TRUE);
  region_row = d(region_row);
  while(region_row)
  {
    region_row_tmp = region_row->NextRegion;
    Xc_free(region_row);
    region_row = region_row_tmp;
  }
  d(region_row) = NULL;
  d(source) = FALSE;
}

void select_row_region_for_editbar_done(cfeuille)
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

void select_row_region_for_editbar_to_xy(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	xm;
int	ym;
{ 
  int	row;
   DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);

   if (xm < 0 || xm >= cfeuille->origin_width)
    xm = 1;
  if (ym < 0)
    ym = 0;
  if (ym >= cfeuille->sheet_height)
    ym = cfeuille->sheet_height - 1;
  
  if (F(cfeuille).GetRowByY(cfeuille, ym, &row) )
  {    
    if (row!=d(yc) || d(prems))
    {
      d(yc) = row;
      
      if (d(prems))
      {
	d(region_row)->x_min = 0; d(region_row)->x_max = COLUMN_MAX;
	d(region_row)->y_min = d(region_row)->y_max = row;
	
	select_row_region_for_editbar_trace(cfeuille,FALSE);
	d(prems) = FALSE;
	return;
      }
      select_row_region_for_editbar_trace(cfeuille,FALSE);
      d(region_row)->y_min = MIN(d(row),row);
      d(region_row)->y_max = MAX(d(row),row);
      select_row_region_for_editbar_trace(cfeuille,FALSE);
      F(GlobEditBar).SelectMove(GlobEditBar, d(region_row), Xq_ROW);
    }
  }
}

void select_row_region_for_editbar_trace(cfeuille, all_r)
     c_Feuille *cfeuille;
     boolean	all_r;
{  
 Display	*display;
 Window		window;
 GC		gc;
 region_t	*region_row;
 int		y;
 DATA(Select_Region_For_EditBar);
 
 GET_DATA(Select_Region_For_EditBar);
 display=cfeuille->X_info.display;
 window=cfeuille->X_info.window;
 gc=cfeuille->X_info.gc_contour;
 
 region_row = d(region_row);
 while(region_row)
   {
     if (region_row->y_min >= cfeuille->row_orig &&
	 region_row->y_min < cfeuille->row_orig+cfeuille->nb_row)
       {
	 y = get_row_pos(cfeuille, region_row->y_min);
	 XDrawLine(display, window, gc, 0, y, cfeuille->Width, y);
       }
     if (region_row->y_max+1 >= cfeuille->row_orig &&
	 region_row->y_max+1 < cfeuille->row_orig+cfeuille->nb_row)
       {
	 y = get_row_pos(cfeuille, region_row->y_max+1);
	 XDrawLine(display, window, gc, 0, y, cfeuille->Width, y);
       }
     if (!all_r)
       break;
     region_row = region_row->NextRegion;
   }
}
