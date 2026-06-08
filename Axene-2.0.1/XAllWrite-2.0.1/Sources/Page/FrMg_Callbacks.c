/*
** FrMg_Callbacks.c for XAllWrite in Page/
** Page's Callbacks for FrameManager
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Thu Apr 16 14:10:13 1998 Emmanuel Paris
** Last update Tue Aug  4 15:07:52 1998 Emmanuel Paris
*/

#include "Page.h"

void frmg_select_callback(This)
c_Page *This;
{
 Xc_TRACE(("frmg_select_callback"));
 if (!This->X_info_calc)	/* this case is used when loading a document */
 {
  *This->select_type = FALSE;
  return;
 }
 
 if (*This->select_type)
 {
  F(This->Redraw).undisplay_selection(This->Redraw, NULL);
  *This->select_type = FALSE;
 }
 else
 {	/* deselect frame from SelectedPage */
  c_Page *page = *(This->BasePage);
  
  while(page)
  {
   if (page != This && page->frmg->SelectedCadre)
    F(page->frmg).deselect_all_cadre(page->frmg, TRUE);
   page = page->NextPage;
  }
 }

 F(This->Calibration).managePageSelection(This->Calibration, This);
}

void frmg_deselect_callback(This)
c_Page *This;
{
 Xc_TRACE(("frmg_deselect_callback"));

 if (!*This->select_type)
 {
  F(This->Redraw).display_selection(This->Redraw, NULL);
  *This->select_type = TRUE;
 }
}

void frmg_redraw_polygone_callback(This, polygone, clear)
c_Page *This;
Polygone *polygone;
boolean clear;
{
 if (!polygone->memory)
  return;

 Xc_TRACE(("frmg_redraw_polygone_callback"));
 Xc_TRACE(("select_type: %s", *This->select_type ? "TEXT" : "CADRE"));
 
 /*clip = CalculPolygoneMask(polygone, This->X_info);*/
 if (clear)
  DrawPolygone(polygone, This->X_info);
 
 if (!This->frmg->redraw_cache_actif)
 {
  F(This->frmg).start_draw_cache(This->frmg);
  F(This->Redraw).redraw_polygone(This->Redraw, This, polygone, 
				  *This->select_type);
  F(This->frmg).flush_draw_cache(This->frmg);
 }
 else
 {
  F(This->frmg).flush_draw_cache(This->frmg);
  F(This->frmg).start_draw_cache(This->frmg);

  F(This->Redraw).redraw_polygone(This->Redraw, This, polygone, 
				  *This->select_type);
 }
}

void frmg_modify_cadre_callback(This, recalibrate)
c_Page *This;
boolean recalibrate;
{
 Xc_TRACE(("frmg_modify_cadre_callback"));
 
 if (!This->X_info_calc) /* this case is used when loading a document */
 {
  *This->select_type = FALSE;
  return;
 }
 
 F(This).CalculClipping(This);
 
 if (recalibrate && This->text_line_start)
 {
  F(This->Calibration).Init(This->Calibration, This);
  while(This)
  {
   if (This->mapped) F(This).RedrawAll(This);
   This = This->NextPage;
  }
 }
}

void frmg_attract_point_callback(This, sx, sy, x, y)
c_Page *This;
coord_t *sx;
coord_t *sy;
int *x;
int *y;
{

}


void display_frame_border(This)
c_Page *This;
{
 F(This->frmg).set_function(This->frmg, DISPLAY_FRAME_BORDER);
 F(This).RedrawAll(This);
}
