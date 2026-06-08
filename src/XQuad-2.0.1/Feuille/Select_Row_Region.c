/*
** Select_Row_Region.c for XQuad in Feuille/
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
** Last update Mon Sep 29 19:01:50 1997 Emmanuel Paris
*/

#include "Select_Move_Resize_Cells.h"
#include "Focus_Region_Utils.h"

void select_row_region_init();
void select_row_region_abort();
void select_row_region_done();
void select_row_region_to_xy();
void select_row_region_trace();

void select_row_region_init(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	  xm;
int	  ym;
{
 boolean	select_type;
 int		row, row2;
 region_t	*region, *region2;
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
  
 d(old_column)	= cfeuille->cell_x;
 d(old_row)	= cfeuille->cell_y;
 d(prems)	= TRUE;
 d(source)	= TRUE;
  
 select_type = cfeuille->select_type;
 if (!select_type)
  F(cfeuille->frmg).deselect_all_cadre(cfeuille->frmg, TRUE);
 if (!d(cntrl))
  F(cfeuille).DeselectRegion(cfeuille,Xq_ORIGIN,TRUE);
 F(cfeuille).GetRowByY(cfeuille, ym, &row);
  
 if (d(shift) && select_type)
 {
  if (row >= d(old_row))
   row2 = d(old_row);
  else
  {
   row2 = row;
   row = d(old_row);
  }
  region = cfeuille->cell_region;
  while(region && region->NextRegion)
   region = region->NextRegion;
    
  if (region)
  {
   if (region->x_min == 0 && region->x_max == COLUMN_MAX)
   {
    if (region->y_min == 0 && region->y_max == ROW_MAX)
    {
     if (d(cntrl))
     {
      d(source)=FALSE;
      return;
     }
    }
    else
    {
     region2 = cfeuille->row_region;
     while(region2 && (region2->y_min != region->y_min || 
		       region2->y_max != region->y_max))
      region2 = region2->NextRegion;
     if (region2)
     {
      remove_region_from_list(cfeuille, region, Xq_CELL);
      remove_region_from_list(cfeuille, region2, Xq_ROW);
      focus_out_region(cfeuille, region, Xq_CELL);
      focus_out_region(cfeuille, region2, Xq_ROW);
      Xc_free(region);
      Xc_free(region2);
     }
     else
     {
      remove_region_from_list(cfeuille, region, Xq_CELL);
      focus_out_region(cfeuille, region, Xq_CELL);
      Xc_free(region);
     }
    }
   }
   else
   {
    if (region->y_min == 0 && region->y_max == ROW_MAX)
    {
     region2 = cfeuille->col_region;
     while(region2 && (region2->x_min != region->x_min || 
		       region2->x_max != region->x_max))
      region2 = region2->NextRegion;
     if (region2)
     {
      remove_region_from_list(cfeuille, region, Xq_CELL);
      remove_region_from_list(cfeuille, region2, Xq_COLUMN);
      focus_out_region(cfeuille, region, Xq_CELL);
      focus_out_region(cfeuille, region2, Xq_COLUMN);
      Xc_free(region);
      Xc_free(region2);
     }
     else
     {
      remove_region_from_list(cfeuille, region, Xq_CELL);
      focus_out_region(cfeuille, region, Xq_CELL);
      Xc_free(region);
     }
    }
    else
    {
     remove_region_from_list(cfeuille, region, Xq_CELL);
     focus_out_region(cfeuille, region, Xq_CELL);
     Xc_free(region);
    }
   }
  }
  F(cfeuille).SelectRegion(cfeuille, 0, row2, 0, row, Xq_ROW, TRUE);
  d(source) = FALSE;
  return;
 }
  
 d(row) = row;
  
 F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar);
 F(cfeuille).SetActiveCellule(cfeuille, cfeuille->column_orig, row, FALSE);
 select_row_region_to_xy(cfeuille, xm, ym);  
  
 d(AutoScroll) = (c_AutoScroll *)NEW(c_AutoScroll)
  (cfeuille->X_info.display, 0, 0, 
   0, cfeuille->sheet_height, NULL, cfeuille->ScrollBarV, 
   eventlook_Select_Move_Resize_Cell, cfeuille);
}

void select_row_region_abort(cfeuille)
c_Feuille *cfeuille;
{ 
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
  
 if (d(AutoScroll))
 {
  DELETE(c_AutoScroll)(d(AutoScroll));
  d(AutoScroll) = (c_AutoScroll *)NULL;
 }
 remove_region_from_list(cfeuille, &d(region), Xq_CELL);
 remove_region_from_list(cfeuille, &d(region_row), Xq_ROW);
  
 focus_out_region(cfeuille, &d(region_row), Xq_ROW);
 focus_out_region(cfeuille, &d(region), Xq_CELL);
 F(cfeuille).SetActiveCellule(cfeuille, d(old_column), d(old_row), FALSE);
 if (cfeuille->nb_cell_region==0)
  F(cfeuille).SelectRegion(cfeuille, d(old_column), d(old_row),
			   d(old_column), d(old_row), Xq_CELL, TRUE); 
}

void select_row_region_done(cfeuille)
c_Feuille *cfeuille;
{  
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
  
 if (d(AutoScroll))
 {
  DELETE(c_AutoScroll)(d(AutoScroll));
  d(AutoScroll) = (c_AutoScroll *)NULL;
 }
 remove_region_from_list(cfeuille, &d(region), Xq_CELL);
 remove_region_from_list(cfeuille, &d(region_row), Xq_ROW);

 F(cfeuille).SelectRegion(cfeuille, 0, d(region_row).y_min,
			  0,d(region_row).y_max, Xq_ROW, FALSE);
 F(GlobEditBar).query_aff(GlobEditBar, 0, d(row));
 d(source)=FALSE;
}

void select_row_region_to_xy(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	xm;
int	ym;
{ 
 int	row;
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
  
 if (xm < 0 || xm >= cfeuille->origin_width)
  xm = 1;
 if (ym < 0)
  ym = 0;
 if (ym >= cfeuille->sheet_height)
  ym = cfeuille->sheet_height - 1;
  
 if (F(cfeuille).GetRowByY(cfeuille, ym, &row) )
 {
  if ((!(row==d(region).y_min) && !(row==d(region).y_max)) 
      || (row==d(row)))
  {
   if (d(prems))
   {
    d(region).x_min = 0;
    d(region).x_max = COLUMN_MAX;
    d(region).y_min = d(region).y_max = row;
    d(region_row).x_min = d(region_row).x_max = 0;
    d(region_row).y_min = d(region_row).y_max = row;
    d(region).NextRegion = NULL;
    d(region_row).NextRegion = NULL;

    add_region_in_list(cfeuille, &d(region), Xq_CELL);
    add_region_in_list(cfeuille, &d(region_row), Xq_ROW);
	      
    focus_in_region(cfeuille, &d(region), Xq_CELL);
    focus_in_region(cfeuille, &d(region_row), Xq_ROW);
    d(prems) = FALSE;
    return;
   }
	  
   focus_row_region_optimized(cfeuille, &d(region), d(row), row);
	  
   d(region).x_min = 0;
   d(region).y_min = MIN(d(row),row);
   d(region).x_max = COLUMN_MAX;
   d(region).y_max = MAX(d(row),row);
   d(region_row).x_min = d(region_row).x_max = 0;
   d(region_row).y_min = MIN(d(row),row);
   d(region_row).y_max = MAX(d(row),row);
   F(GlobEditBar).drag_aff(GlobEditBar, 0,
			   d(region).y_max-d(region).y_min+1);
  }
 }
}

void select_row_region_trace(cfeuille)
c_Feuille *cfeuille;
{  
}
