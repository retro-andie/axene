/*
** Select_Region_By_KeyArrow.c for XQuad in Feuille/
** Select And Deselect Cells region
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
** Last update Sun Jul 12 01:56:12 1998 Emmanuel Paris
*/

#include "Select_Move_Resize_Cells.h"
#include "Select_Region_By_KeyArrow.h"
#include "Focus_Region_Utils.h"

void select_region_by_keyarrow();
static void scroll_if_necessary();
static void manage_cell_region();
static void manage_col_region();
static void manage_row_region();

void select_region_by_keyarrow(cfeuille, direction)
c_Feuille *cfeuille;	
int direction;
{
 int		col_orig, col, new_col;
 int		row_orig, row, new_row;
 int		region_type;
 region_t	*region, *region2, *region_col = NULL, *region_row = NULL;
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
  
 region = cfeuille->cell_region;
 if (region == NULL) return;
 while(region->NextRegion)
  region = region->NextRegion;
  
 if (region->x_min == 0 && region->x_max == COLUMN_MAX)
 {
  if (region->y_min == 0 && region->y_max == ROW_MAX)
  {
   return;
  }
  else
  {
   region2 = cfeuille->row_region;
   while(region2 && (region2->y_min != region->y_min || 
		     region2->y_max != region->y_max))
    region2 = region2->NextRegion;
   if (region2)
   {
    region_type = Xq_ROW;
    region_row = region2;
   }
   else
   {
    region_type = Xq_CELL;
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
    region_type = Xq_COLUMN;
    region_col = region2;
   }
   else
   {
    region_type = Xq_CELL;
   }
  }
  else
  {
   region_type = Xq_CELL;
  }
 }
  
 col_orig = cfeuille->cell_x;
 row_orig = cfeuille->cell_y;
  
 if (region->x_min == col_orig)
  col = region->x_max;
 else
 {
  if (region->x_max == col_orig)
   col = region->x_min;
  else
   col = col_orig;
 }
  
 if (region->y_min == row_orig)
  row = region->y_max;
 else
 {
  if (region->y_max == row_orig)
   row = region->y_min;
  else
   row = row_orig;
 }
  
 new_col = col;
 new_row = row;
  
 switch(direction)
 {
 case SRKA_UP:
  if (region_type != Xq_COLUMN && row > 0)
  {
   new_row--; 
      
   if (region_type == Xq_ROW)
   {
    scroll_if_necessary(cfeuille,  cfeuille->column_orig, new_row);
    manage_row_region(cfeuille, region, region_row, row_orig, new_row);
   }
   else
   {
    scroll_if_necessary(cfeuille, new_col, new_row);
    manage_cell_region(cfeuille, region, col_orig, row_orig, 
		       new_col, new_row);
   }
  }
  else
   return;
  break;
 case SRKA_DOWN:
  if (region_type != Xq_COLUMN && row < ROW_MAX)
  {
   new_row++;
	
   if (region_type == Xq_ROW)
   {
    scroll_if_necessary(cfeuille,  cfeuille->column_orig, new_row);
    manage_row_region(cfeuille, region, region_row, row_orig, new_row);
   }
   else
   {
    scroll_if_necessary(cfeuille, new_col, new_row);
    manage_cell_region(cfeuille, region, col_orig, row_orig, 
		       new_col, new_row);
   }
  }
  else
   return;
  break;
 case SRKA_LEFT:
  if (region_type != Xq_ROW && col > 0)
  {
   new_col--;
	
   if (region_type == Xq_COLUMN)
   {
    scroll_if_necessary(cfeuille, new_col, cfeuille->row_orig);
    manage_col_region(cfeuille, region, region_col, col_orig, new_col);
   }
   else
   {
    scroll_if_necessary(cfeuille, new_col, new_row);
    manage_cell_region(cfeuille, region, col_orig, row_orig, 
		       new_col, new_row);
   }
  }
  else
   return;
  break;
 case SRKA_RIGHT:
  if (region_type != Xq_ROW && col < COLUMN_MAX)
  {
   new_col++;
	
   if (region_type == Xq_COLUMN)
   {
    scroll_if_necessary(cfeuille, new_col, cfeuille->row_orig);
    manage_col_region(cfeuille, region, region_col, col_orig, new_col);
   }
   else
   {
    scroll_if_necessary(cfeuille, new_col, new_row);
    manage_cell_region(cfeuille, region, col_orig, row_orig, 
		       new_col, new_row);
   }
  }
  else
   return;
  break;
 case SRKA_PAGEUP:
  if (region_type != Xq_COLUMN && row > 0)
  {
   if (new_row < cfeuille->nb_row - 1)
    new_row = 0;
   else
    new_row -= cfeuille->nb_row - 1;
	
   if (region_type == Xq_ROW)
   {
    scroll_if_necessary(cfeuille,  cfeuille->column_orig, new_row);
    manage_row_region(cfeuille, region, region_row, row_orig, new_row);
   }
   else
   {
    scroll_if_necessary(cfeuille, new_col, new_row);
    manage_cell_region(cfeuille, region, col_orig, row_orig, 
		       new_col, new_row);
   }
  }
  else
   return;
  break;
 case SRKA_PAGEDOWN:
  if (region_type != Xq_COLUMN && row < ROW_MAX)
  {
   if (ROW_MAX - new_row < cfeuille->nb_row - 1)
    new_row = ROW_MAX; 
   else
    new_row += cfeuille->nb_row - 1;
	
   if (region_type == Xq_ROW)
   {
    scroll_if_necessary(cfeuille,  cfeuille->column_orig, new_row);
    manage_row_region(cfeuille, region, region_row, row_orig, new_row);
   }
   else
   {
    scroll_if_necessary(cfeuille, new_col, new_row);
    manage_cell_region(cfeuille, region, col_orig, row_orig, 
		       new_col, new_row);
   }
  }
  else
   return;
  break;
 case SRKA_HOME:
  if (cfeuille->last_advance->x_min != 0)
   new_row = cfeuille->cell_y;
  else
   new_row = 0;
  
  scroll_if_necessary(cfeuille, 0, new_row);
  
  if (!d(cntrl))
   F(cfeuille).DeselectRegion(cfeuille, Xq_ORIGIN, TRUE);
  F(cfeuille).SelectRegion(cfeuille, 0, new_row, col_orig, row_orig,
			   Xq_CELL, TRUE);
  break;
 case SRKA_END:
  col = cfeuille->column_orig + cfeuille->nb_column - 1;
  
  if (cfeuille->cell_x < col - 1)
   new_row = cfeuille->cell_y;
  else
   new_row = cfeuille->row_orig + cfeuille->nb_row - 1;
  
  scroll_if_necessary(cfeuille, col, new_row);
  
  if (!d(cntrl))
   F(cfeuille).DeselectRegion(cfeuille, Xq_ORIGIN, TRUE);
  F(cfeuille).SelectRegion(cfeuille, col_orig, row_orig, col, new_row, 
			   Xq_CELL, TRUE);
  break;
 }  
}

static void scroll_if_necessary(This, new_col, new_row)
c_Feuille	*This;
int		new_col;
int		new_row;
{
 int	delta_x, delta_y;
  
 if (new_col >= This->column_orig && 
     new_col < This->column_orig + This->nb_column - 1 &&
     new_row >= This->row_orig && 
     new_row < This->row_orig + This->nb_row - 1)
  return;
  
 delta_x = delta_y = 0;
  
 if (new_col < This->column_orig)
 {
  delta_x = new_col - This->column_orig;/* should be negative */
  if (delta_x != -1 && This->column_orig > This->nb_column-1)
   delta_x = -This->nb_column-1;
 }
 else
 {
  if (new_col >= This->column_orig + This->nb_column - 1)
  {
   delta_x = new_col - (This->column_orig + This->nb_column - 1) + 1;
   if (delta_x != 1 && 
       (This->column_orig < COLUMN_MAX - This->nb_column-1))
    delta_x = This->nb_column-1;
  }
  /* should be positive */
 }
  
 if (new_row < This->row_orig)
 {
  delta_y = new_row - This->row_orig;/* should be negative */
  if (delta_y != -1 && This->row_orig > This->nb_row-1)
   delta_y = -This->nb_row-1;
 }
 else
 {
  if (new_row >= This->row_orig + This->nb_row - 1)
  {
   delta_y = new_row - (This->row_orig + This->nb_row - 1) + 1;
   if (delta_y != 1 && (This->row_orig < ROW_MAX - This->nb_row-1))
    delta_y = This->nb_row-1;
  }
  /* should be positive */
 }
  
 if (delta_x)
  F(This->ScrollBarH).Move(This->ScrollBarH, delta_x);
 if (delta_y)
  F(This->ScrollBarV).Move(This->ScrollBarV, delta_y);
 if (delta_x || delta_y)
 {
  change_feuille_origin(This, delta_x, delta_y);
 }
}

#define SORT_CELL_REGION \
if (col_orig > new_col) \
SWAP( int, col_orig, new_col); \
if (row_orig > new_row) \
SWAP( int, row_orig, new_row)

#define SORT_COL_REGION \
if (col_orig > new_col) \
SWAP( int, col_orig, new_col)

#define SORT_ROW_REGION \
if (row_orig > new_row) \
SWAP( int, row_orig, new_row)


static void manage_cell_region(cfeuille, region, col_orig, row_orig, 
			       new_col, new_row)
c_Feuille	*cfeuille;
region_t	*region;
int		col_orig;
int		row_orig;
int		new_col;
int		new_row;
{
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
  
 if (!d(cntrl))
 {
  if (cfeuille->nb_cell_region > 1)
  {	
   SORT_CELL_REGION;
   F(cfeuille).DeselectRegion(cfeuille, Xq_ORIGIN, TRUE);
   F(cfeuille).SelectRegion(cfeuille, col_orig, row_orig, 
			    new_col, new_row, Xq_CELL, TRUE);
  }
  else
  {
   focus_cell_region_optimized(cfeuille, region,
			       col_orig, row_orig, new_col, new_row);
   remove_region_from_list(cfeuille, region, Xq_CELL);
   Xc_free(region);
   SORT_CELL_REGION;
   F(cfeuille).SelectRegion(cfeuille, col_orig, row_orig, 
			    new_col, new_row, Xq_CELL, FALSE);
  }
 }
 else
 {
  F(cfeuille).SelectRegion(cfeuille, new_col, new_row,
			   new_col, new_row, Xq_CELL, TRUE);
  F(cfeuille).SetActiveCellule(cfeuille, new_col, new_row, FALSE);
  F(GlobEditBar).query_aff(GlobEditBar, new_col, new_row);
 }
}

static void manage_col_region(cfeuille, region, region_col, col_orig, new_col)
c_Feuille	*cfeuille;
region_t	*region;
region_t	*region_col;
int		col_orig;
int		new_col;
{
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
  
 if (!d(cntrl))
 {
  if (cfeuille->nb_cell_region > 1)
  {	
   SORT_COL_REGION;
   F(cfeuille).DeselectRegion(cfeuille, Xq_ORIGIN, TRUE);
   F(cfeuille).SelectRegion(cfeuille, col_orig, 0,
			    new_col, 0, Xq_COLUMN, TRUE);
  }
  else
  {
   focus_column_region_optimized(cfeuille, region, col_orig, new_col);
   remove_region_from_list(cfeuille, region, Xq_CELL);
   remove_region_from_list(cfeuille, region_col, Xq_COLUMN);
   Xc_free(region);
   Xc_free(region_col);
   SORT_COL_REGION;
   F(cfeuille).SelectRegion(cfeuille, col_orig, 0, 
			    new_col, 0, Xq_COLUMN, FALSE);
  }
 }
 else
 {
  F(cfeuille).SelectRegion(cfeuille, new_col, 0,
			   new_col, 0, Xq_COLUMN, TRUE);
  F(cfeuille).SetActiveCellule(cfeuille, new_col, 0, FALSE);
  F(GlobEditBar).query_aff(GlobEditBar, new_col, 0);
 }
}

static void manage_row_region(cfeuille, region, region_row, row_orig, new_row)
c_Feuille	*cfeuille;
region_t	*region;
region_t	*region_row;
int		row_orig;
int		new_row;
{
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
  
 if (!d(cntrl))
 {
  if (cfeuille->nb_cell_region > 1)
  {	
   SORT_ROW_REGION;
   F(cfeuille).DeselectRegion(cfeuille, Xq_ORIGIN, TRUE);
   F(cfeuille).SelectRegion(cfeuille, 0, row_orig, 
			    0, new_row, Xq_ROW, TRUE);
  }
  else
  {
   focus_row_region_optimized(cfeuille, region, row_orig, new_row);
   remove_region_from_list(cfeuille, region, Xq_CELL);
   remove_region_from_list(cfeuille, region_row, Xq_ROW);
   Xc_free(region);
   Xc_free(region_row);
   SORT_ROW_REGION;
   F(cfeuille).SelectRegion(cfeuille, 0, row_orig, 
			    0, new_row, Xq_ROW, FALSE);
  }
 }
 else
 {
  F(cfeuille).SelectRegion(cfeuille, 0, new_row,
			   0, new_row, Xq_ROW, TRUE);
  F(cfeuille).SetActiveCellule(cfeuille, 0, new_row, FALSE);
  F(GlobEditBar).query_aff(GlobEditBar, 0, new_row);
 }
}

