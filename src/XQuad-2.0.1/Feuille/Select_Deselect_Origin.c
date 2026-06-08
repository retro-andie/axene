/*
** Select_Deselect_Origin.c for XQuad in Feuille/
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
** Started on  Thu Jan  5 10:51:31 1995 Emmanuel Paris
** Last update Fri Mar 24 10:14:45 1995 Emmanuel Paris
*/

#include "Select_Move_Resize_Cells.h"

void select_deselect_origin(cfeuille)
c_Feuille *cfeuille;
{
  if (!cfeuille->select_type)
  {
    F(cfeuille->frmg).deselect_all_cadre(cfeuille->frmg, TRUE);
    focus_out_active_cellule(cfeuille, cfeuille->cell_x, cfeuille->cell_y);
  }
  F(cfeuille).DeselectRegion(cfeuille,Xq_ORIGIN,TRUE);
  
  if (cfeuille->origin_selected)
  {
    Xc_TRACE(("deselect all"));
    F(cfeuille).SelectRegion(cfeuille, 
			     cfeuille->column_orig, cfeuille->row_orig,
			     cfeuille->column_orig, cfeuille->row_orig,
			     Xq_CELL, TRUE);
    F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar);
    F(cfeuille).SetActiveCellule(cfeuille, 
				 cfeuille->column_orig, 
				 cfeuille->row_orig, FALSE);
  }
  else
  {
    Xc_TRACE(("select all"));
   
    F(cfeuille).SelectRegion(cfeuille,0,0,0,0,Xq_ORIGIN,TRUE);
    F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar);
    F(cfeuille).SetActiveCellule(cfeuille, cfeuille->column_orig,
				 cfeuille->row_orig, FALSE);
  }
}

