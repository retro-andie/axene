/*
** Misc_Functions.c for XQuad in Feuille/
** miscellaneous functions
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
** Started on  Tue May 30 15:06:35 1995 Emmanuel Paris
** Last update Mon Jan 25 23:20:46 1999 Emmanuel Paris
*/

#include "Feuille.h"

void redraw_all_feuille(This)
c_Feuille *This;
{
 calcul_column_width(This);
 calcul_row_height(This);
 calcul_cell_info(This);

 redraw_rectangle(This, Xq_ORIGIN, 0, 0, 
		  This->origin_width, This->origin_height);
 redraw_rectangle(This, Xq_COLUMN, 0, 0, 
		  This->sheet_width, This->origin_height);
 redraw_rectangle(This, Xq_ROW, 0, 0, This->origin_width, This->sheet_height);
 redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, This->sheet_height);
}

void set_unset_grid(This)
c_Feuille *This;
{
 This->grid ^= TRUE;
 redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, This->sheet_height);
}

void cut_region(This)
c_Feuille *This;
{
 if (This->nb_cell_region == 1)
 {
  F(This->moteur).cut_zone(This->moteur, This->cell_region, Xc_MODE_CUT);
 }
}

void copy_region(This)
c_Feuille *This;
{
 if (This->nb_cell_region == 1)
 {
  F(This->moteur).cut_zone(This->moteur, This->cell_region, Xc_MODE_COPY);
 }
}

void paste_region(This)
c_Feuille *This;
{
 region_t	*region;
  
 region = F(This->moteur).paste_zone(This->moteur, This->cell_region,
				     This->cell_x, This->cell_y);
 if (region)
 {
  calcul_cell_info(This);
  if (region->NextRegion)
  {
   F(This).RefreshRegion(This, region, Xq_CELLS);
   while (region)
   {
    region_t	*tregion;
    
    tregion = region->NextRegion;
    Xc_free(region);
    region = tregion;
   }
  }
  else
  {
   F(This).RefreshRegion(This, region, Xq_CELL);
   Xc_free(region);
  }
  F(GlobEditBar).refresh_cell(GlobEditBar, This);
 }
}

void display_frame_border(This)
c_Feuille *This;
{
 F(This->frmg).set_function(This->frmg, DISPLAY_FRAME_BORDER);
 redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, This->sheet_height);
}

void display_column_origin(This)
c_Feuille *This;
{
 calcul_column_width(This);
 calcul_row_height(This);
 calcul_cell_info(This);
  
 if (This->Column_Origin_actif)
 {
  This->Column_Origin_actif = FALSE;
  This->origin_height = 0;
  This->sheet_height = This->Height;
  if (This->Row_Origin_actif)
  {
   XUnmapWindow(_display, _O_window);
   XUnmapWindow(_display, _C_window);
   XMoveResizeWindow(_display, _R_window, 0, 0, This->origin_width, 
		     This->sheet_height);
   XMoveResizeWindow(_display, _window, This->origin_width, 0, 
		     This->sheet_width, This->sheet_height);
   calcul_column_width(This);
   calcul_row_height(This);
   calcul_cell_info(This);
   redraw_rectangle(This, Xq_ROW, 0, 0, This->origin_width, 
		    This->sheet_height);
   redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, 
		    This->sheet_height);
  }
  else
  {
   XUnmapWindow(_display, _C_window);
   XMoveResizeWindow(_display, _window, 0, 0, 
		     This->sheet_width, This->sheet_height);
   calcul_column_width(This);
   calcul_row_height(This);
   calcul_cell_info(This);
   redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, 
		    This->sheet_height);
  }
 }
 else
 {
  This->Column_Origin_actif = TRUE;
  This->origin_height = DEFAULT_ROW_SIZE / This->sScale;
  This->sheet_height = This->Height - This->origin_height;
  if (This->Row_Origin_actif)
  {
   XMapWindow(_display, _O_window);
   XMapWindow(_display, _C_window);
   XMoveResizeWindow(_display, _O_window, 0, 0, 
		     This->origin_width, This->origin_height);
   XMoveResizeWindow(_display, _C_window, This->origin_width, 0, 
		     This->sheet_width, This->origin_height);
   XMoveResizeWindow(_display, _R_window, 0, This->origin_height, 
		     This->origin_width, This->sheet_height);
   XMoveResizeWindow(_display, _window, This->origin_width, 
		     This->origin_height, This->sheet_width, 
		     This->sheet_height);
   calcul_column_width(This);
   calcul_row_height(This);
   calcul_cell_info(This);
   redraw_rectangle(This, Xq_ROW, 0, 0, This->origin_width, 
		    This->sheet_height);
   redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, 
		    This->sheet_height);
  }
  else
  {
   XMapWindow(_display, _C_window);
   XMoveResizeWindow(_display, _C_window, 0, 0, 
		     This->sheet_width, This->origin_height);
   XMoveResizeWindow(_display, _window, 0, This->origin_height, 
		     This->sheet_width, This->sheet_height);
   calcul_column_width(This);
   calcul_row_height(This);
   calcul_cell_info(This);
   redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, 
		    This->sheet_height);
  }
 }
}

void display_row_origin(This)
c_Feuille *This;
{
 calcul_column_width(This);
 calcul_row_height(This);
 calcul_cell_info(This);
  
 if (This->Row_Origin_actif)
 {
  This->Row_Origin_actif = FALSE;
  This->origin_width = 0;
  This->sheet_width = This->Width;
  if (This->Column_Origin_actif)
  {
   XUnmapWindow(_display, _O_window);
   XUnmapWindow(_display, _R_window);
   XMoveResizeWindow(_display, _C_window, 0, 0, This->sheet_width, 
		     This->origin_height);
   XMoveResizeWindow(_display, _window, 0, This->origin_height, 
		     This->sheet_width, This->sheet_height);
   calcul_column_width(This);
   calcul_row_height(This);
   calcul_cell_info(This);
   redraw_rectangle(This, Xq_COLUMN, 0, 0, 
		    This->sheet_width, This->origin_height);
   redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, 
		    This->sheet_height);
  }
  else
  {
   XUnmapWindow(_display, _R_window);
   XMoveResizeWindow(_display, _window, 0, 0, 
		     This->sheet_width, This->sheet_height);
   calcul_column_width(This);
   calcul_row_height(This);
   calcul_cell_info(This);
   redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, 
		    This->sheet_height);
  }
 }
 else
 {
  This->Row_Origin_actif = TRUE;
  This->origin_width = DEFAULT_COL_SIZE / This->sScale;
  This->sheet_width = This->Width - This->origin_width;
  if (This->Column_Origin_actif)
  {  
   XMapWindow(_display, _O_window);
   XMapWindow(_display, _R_window);
   XMoveResizeWindow(_display, _O_window, 0, 0, 
		     This->origin_width, This->origin_height);
   XMoveResizeWindow(_display, _C_window, This->origin_width, 0, 
		     This->sheet_width, This->origin_height);
   XMoveResizeWindow(_display, _R_window, 0, This->origin_height, 
		     This->origin_width, This->sheet_height);
   XMoveResizeWindow(_display, _window, This->origin_width, 
		     This->origin_height, This->sheet_width, 
		     This->sheet_height);
   calcul_column_width(This);
   calcul_row_height(This);
   calcul_cell_info(This);
   redraw_rectangle(This, Xq_COLUMN, 0, 0, 
		    This->sheet_width, This->origin_height);
   redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, 
		    This->sheet_height);
  }
  else
  {
   XMapWindow(_display, _R_window);
   XMoveResizeWindow(_display, _R_window, 0, 0, 
		     This->origin_width, This->sheet_height);
   XMoveResizeWindow(_display, _window, This->origin_width, 0, 
		     This->sheet_width, This->sheet_height);
   calcul_column_width(This);
   calcul_row_height(This);
   calcul_cell_info(This);
   redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, 
		    This->sheet_height);
  }
 }
}

void display_page_mark(This)
c_Feuille *This;
{
 This->page_mark ^= TRUE;
 redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, This->sheet_height);
}

void display_formula(This)
c_Feuille *This;
{
 This->BaseStd->cell_style_base->display_formula ^= TRUE;
 redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, This->sheet_height);
}

void display_zero_cell(This)
c_Feuille *This;
{
 This->BaseStd->cell_style_base->display_zero_cell ^= TRUE;
 redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, This->sheet_height);
}

void select_all_region(This)
c_Feuille *This;
{
 if (!F(This).IsSelected(This,0,0,Xq_ORIGIN))
 {
  F(This).DeselectRegion(This,Xq_ORIGIN,TRUE);
  F(This).SetActiveCellule(This, This->column_orig,
			   This->row_orig, FALSE);
  F(This).SelectRegion(This,0,0,0,0,Xq_ORIGIN,TRUE);
 }
}

void delete_selection(This)
c_Feuille *This;
{
 if (This->select_type)
 {
  if (This->nb_cell_region == 1)
  {
   if (This->nb_col_region)
     F(This).SetFunction(This, DELETE_COLUMN);
   else if (This->nb_row_region)
    F(This).SetFunction(This, DELETE_ROW);
   else
    F(This).SetFunction(This, UNSET_ALL_REGION);
  }
  else
   F(This).SetFunction(This, UNSET_ALL_REGION);
 }
 else
 {
  F(This).SetFunction(This, DELETE_OBJECT);
 }
}

void sheet_insert_column(This)
c_Feuille *This;
{
 region_t	region;
  
 if (This->nb_cell_region == 1 && This->nb_row_region == 0)
 {
  F(This->moteur).insert_zone_col(This->moteur, This->cell_region);
  calcul_column_width(This);
  calcul_row_height(This);
  calcul_cell_info(This);
  if (This->nb_col_region == 1)
  {
   region.x_min = This->cell_region->x_min;
   region.x_max = COLUMN_MAX;
   region.y_min = 0;
   region.y_max = ROW_MAX;
  }
  else
  {
   region.x_min = This->cell_region->x_min;
   region.x_max = COLUMN_MAX;
   region.y_min = This->cell_region->y_min;
   region.y_max = This->cell_region->y_max;
  }
  F(This).RefreshRegion(This, &region, Xq_CELL);
  F(This).RefreshRegion(This, &region, Xq_COLUMN);
  F(GlobEditBar).refresh_cell(GlobEditBar, This);
 }
}

void sheet_insert_row(This)
c_Feuille *This;
{
 region_t	region;
  
 if (This->nb_cell_region == 1 && This->nb_col_region == 0)
 {
  F(This->moteur).insert_zone_row(This->moteur, This->cell_region);
  calcul_column_width(This);
  calcul_row_height(This);
  calcul_cell_info(This);
  if (This->nb_row_region == 1)
  {
   region.x_min = 0;
   region.x_max = COLUMN_MAX;
   region.y_min = This->cell_region->y_min;
   region.y_max = ROW_MAX;
  }
  else
  {
   region.x_min = This->cell_region->x_min;
   region.x_max = This->cell_region->x_max;
   region.y_min = This->cell_region->y_min;
   region.y_max = ROW_MAX;
  }
  F(This).RefreshRegion(This, &region, Xq_CELL);
  F(This).RefreshRegion(This, &region, Xq_ROW);
  F(GlobEditBar).refresh_cell(GlobEditBar, This);
 }
}

void sheet_delete_column(This)
c_Feuille *This;
{
 region_t	region;

 if (This->nb_cell_region == 1 && This->nb_row_region == 0)
 {
  F(This->moteur).delete_zone_col(This->moteur, This->cell_region);
  calcul_column_width(This);
  calcul_row_height(This);
  calcul_cell_info(This);
  if (This->nb_col_region == 1)
  {
   region.x_min = This->cell_region->x_min;
   region.x_max = COLUMN_MAX;
   region.y_min = 0;
   region.y_max = ROW_MAX;
  }
  else
  {
   region.x_min = This->cell_region->x_min;
   region.x_max = COLUMN_MAX;
   region.y_min = This->cell_region->y_min;
   region.y_max = This->cell_region->y_max;
  }
  F(This).RefreshRegion(This, &region, Xq_CELL);
  F(This).RefreshRegion(This, &region, Xq_COLUMN);
  F(GlobEditBar).refresh_cell(GlobEditBar, This);
 }
}

void sheet_delete_row(This)
c_Feuille *This;
{
 region_t	region;

 if (This->nb_cell_region == 1 && This->nb_col_region == 0)
 {
  F(This->moteur).delete_zone_row(This->moteur, This->cell_region);
  calcul_column_width(This);
  calcul_row_height(This);
  calcul_cell_info(This);
  if (This->nb_row_region == 1)
  {
   region.x_min = 0;
   region.x_max = COLUMN_MAX;
   region.y_min = This->cell_region->y_min;
   region.y_max = ROW_MAX;
  }
  else
  {
   region.x_min = This->cell_region->x_min;
   region.x_max = This->cell_region->x_max;
   region.y_min = This->cell_region->y_min;
   region.y_max = ROW_MAX;
  }
  F(This).RefreshRegion(This, &region, Xq_CELL);
  F(This).RefreshRegion(This, &region, Xq_ROW);
  F(GlobEditBar).refresh_cell(GlobEditBar, This);
 }
}

void set_column_and_row_size(This, colrow)
c_Feuille *This;
colrow_size_t *colrow;
{
 region_t	*region;
 int	x_min, y_min;
  
 if (!This->cell_region)
  return;
  
 x_min = COLUMN_MAX;
 y_min = ROW_MAX;
  
 region = This->cell_region;
 while(region)
 {
  if (colrow->change_col_size)
  {
   F(This->moteur).set_size_zone(This->moteur, Xc_COLUMN, region, 
				 colrow->col_size);
  }
  if (colrow->change_row_size)
  {
   F(This->moteur).set_size_zone(This->moteur, Xc_ROW, region, 
				 colrow->row_size);
  }
  region = region->NextRegion;
 }
 redraw_all_feuille(This);
}

void copy_region_r_or_d(This, type)
c_Feuille *This;
int type;
{
 region_t	*region;
 region_t	region_s;
 
 region = This->cell_region;
 while(region)
 {
  if ( (region->x_min > 0 || region->x_max < COLUMN_MAX) &&
      (region->y_min > 0 || region->y_max < ROW_MAX) )
  {
   F(This->moteur).copy_zone_r_or_d(This->moteur, type, region);
   memcpy(&region_s, region, sizeof(region_t));
   if (type == Xc_COLUMN)
    region_s.x_min++;
   else
    region_s.y_min++;
   F(This).RefreshRegion(This, &region_s, Xq_CELL);
  }
  region = region->NextRegion;
 }
 F(GlobEditBar).refresh_cell(GlobEditBar, This);
}

void resize_column_idealy(This, col)
c_Feuille *This;
int	col;
{
 int		min_col;
 region_t	*rregion;
 region_t	region;
 coord_t	new_width;
 scale_t	factor;
 boolean	resize_region = FALSE;
  
 if (This->col_region)
 {
  rregion = This->col_region;
  while(rregion && (col < rregion->x_min || col > rregion->x_max))
   rregion = rregion->NextRegion;
    
  if (!rregion)
   min_col = col;
  else
  {
   rregion = This->col_region;
   min_col = COLUMN_MAX + 1;
   while(rregion)
   {
    if (rregion->x_min < min_col)
     min_col = rregion->x_min;
    rregion = rregion->NextRegion;
   }
   resize_region = TRUE;
  }
 }
 else
  min_col = col;
  
 factor = This->sFactor * COORD_SCALEVALUE / COORD_DEFAULT;
 new_width = 
  F(This->moteur).get_ideal_column_width(This->moteur, col, factor);
  
 if (new_width < 0)
 {
  if (resize_region)
   new_width = F(This->moteur).get_size(This->moteur, Xc_COLUMN, col);
  else
   return;
 }
 else
  new_width /= factor;
  
 if (resize_region)
 {
  rregion = This->col_region;
  while(rregion)
  {
   F(This->moteur).set_size_zone(This->moteur, Xc_COLUMN,
				 rregion, new_width);
   rregion = rregion->NextRegion;
  }
 }
 else
  F(This->moteur).set_size(This->moteur, Xc_COLUMN, col, new_width);
  
 calcul_column_width(This);
 allocate_cell_info(This);
 calcul_frame_info(This);
 region.x_min = min_col;
 region.x_max = COLUMN_MAX;
 region.y_min = 0;
 region.y_max = ROW_MAX;
 if (!This->select_type)
  F(This->frmg).undisplay_selected_lcadre(This->frmg);
 if (This->page_mark && min_col > 0)
  region.x_min--;
 F(This).RefreshRegion(This, &region, Xq_CELL);
 F(This).RefreshRegion(This, &region, Xq_COLUMN);
 if (!This->select_type)
  F(This->frmg).display_selected_lcadre(This->frmg);
}

void resize_row_idealy(This, row)
c_Feuille *This;
int	row;
{
 int		min_row;
 region_t	*rregion;
 region_t	region;
 coord_t	new_height;
 scale_t	factor;
 boolean	resize_region = FALSE;
  
 if (This->row_region)
 {
  rregion = This->row_region;
  while(rregion && (row < rregion->y_min || row > rregion->y_max))
   rregion = rregion->NextRegion;
    
  if (!rregion)
   min_row = row;
  else
  {
   rregion = This->row_region;
   min_row = ROW_MAX + 1;
   while(rregion)
   {
    if (rregion->y_min < min_row)
     min_row = rregion->y_min;
    rregion = rregion->NextRegion;
   }
   resize_region = TRUE;
  }
 }
 else
  min_row = row;
  
 factor = This->sFactor * COORD_SCALEVALUE / COORD_DEFAULT;
 new_height =
  F(This->moteur).get_ideal_row_height(This->moteur, row, factor);
  
 if (new_height < 0)
 {
  if (resize_region)
   new_height = F(This->moteur).get_size(This->moteur, Xc_ROW, row);
  else
   return;
 }
 else
  new_height /= factor;
  
 if (resize_region)
 {
  rregion = This->row_region;
  while(rregion)
  {
   F(This->moteur).set_size_zone(This->moteur, Xc_ROW,
				 rregion, new_height);
   rregion = rregion->NextRegion;
  }
 }
 else
  F(This->moteur).set_size(This->moteur, Xc_ROW, row, new_height);
  
 calcul_row_height(This);
 allocate_cell_info(This);
 calcul_frame_info(This);
 region.x_min = 0;
 region.x_max = COLUMN_MAX;
 region.y_min = min_row;
 region.y_max = ROW_MAX;
 if (!This->select_type)
  F(This->frmg).undisplay_selected_lcadre(This->frmg);
 if (This->page_mark && min_row > 0)
  region.y_min--;
 F(This).RefreshRegion(This, &region, Xq_CELL);
 F(This).RefreshRegion(This, &region, Xq_ROW);
 if (!This->select_type)
  F(This->frmg).display_selected_lcadre(This->frmg);
}

void goto_last_cell(This)
c_Feuille *This;
{
 region_t	region;
  
 region.x_min = region.y_min = 0;
 region.x_max = COLUMN_MAX;
 region.y_max = ROW_MAX;

 if (!F(This->moteur).get_filled_region(This->moteur, &region))
  return;
  
 F(This).SetActiveCellule(This, region.x_max, region.y_max, TRUE);
}

void goto_active_cell(This)
c_Feuille *This;
{
 F(This).SetActiveCellule(This, This->cell_x, This->cell_y, TRUE);
}

void select_to_last_cell(This)
c_Feuille *This;
{
 region_t	region;
  
 region.x_min = region.y_min = 0;
 region.x_max = COLUMN_MAX;
 region.y_max = ROW_MAX;
  
 if (!F(This->moteur).get_filled_region(This->moteur, &region))
  return;
  
 F(This).DeselectRegion(This, Xq_ORIGIN, This->select_type);
 F(This).SetActiveCellule(This, region.x_min, region.y_min, FALSE);
 F(This).SelectRegion(This, region.x_min, region.y_min, region.x_max,
		      region.y_max, Xq_CELL, TRUE);
}
