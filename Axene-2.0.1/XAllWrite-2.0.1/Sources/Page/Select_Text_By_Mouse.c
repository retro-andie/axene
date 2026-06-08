/*
** Select_Text_By_Mouse.c for XAllWrite in Page/
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
** Started on  Fri Sep 26 19:20:09 1997 Emmanuel Paris
** Last update Mon Jul 13 15:42:56 1998 Emmanuel Paris
*/

#include "Select_Move_Edit_Text.h"

void select_text_by_mouse_init();
void select_text_by_mouse_abort();
void select_text_by_mouse_done();
void select_text_by_mouse_to_xy();
void select_text_by_mouse_trace();

void select_text_by_mouse_init(cpage, xm, ym)
c_Page	*cpage;
int	xm;
int	ym;
{
 coord_t	x, y;
 text_line_t	*tl;
 c_Text *text = cpage->BaseStd->text_base;
 DATA(Select_Move_Edit_Text);
  
 GET_DATA(Select_Move_Edit_Text);

 d(prems) = FALSE;
 d(source) = TRUE;
 d(select_start) = text->select_start;
 d(select_end) = text->select_end;
 d(select_start_pos) = text->select_start_pos;
 d(select_end_pos) = text->select_end_pos;
 d(start_end) = text->start_end;
 d(xo) = xm; d(yo) = ym;

 x = xm * cpage->sScale;
 y = ym * cpage->sScale;

 F(cpage->Calibration).getTextLineByY(cpage->Calibration, cpage, y, x, &tl);
 d(stl) = tl;
 d(sx) = x;
 
 d(AutoScroll) = F(cpage->Calibration->PageArea).CreateAutoScroll
  (cpage->Calibration->PageArea,cpage, 
   (void (*)())eventlook_Select_Move_Edit_Text, cpage);

 cpage->in_use = TRUE;
 select_text_by_mouse_to_xy(cpage, xm, ym);
}

void select_text_by_mouse_abort(cpage)
c_Page	*cpage;
{
 c_Text *text = cpage->BaseStd->text_base;
 DATA(Select_Move_Edit_Text);
  
 GET_DATA(Select_Move_Edit_Text);

 if (d(AutoScroll))
 {
  F(cpage->Calibration->PageArea).DestroyAutoScroll
   (cpage->Calibration->PageArea, d(AutoScroll));
  d(AutoScroll) = (c_AutoScroll *)NULL;
 }

 cpage->in_use = FALSE;
 if (cpage->mapped == FALSE)
 {
  cpage->mapped = TRUE;
  F(cpage).MapWindow(cpage, FALSE);
 }

 F(cpage->Redraw).start_selection_diff(cpage->Redraw);

 text->select_start = d(select_start);
 text->select_end =  d(select_end);
 text->select_start_pos = d(select_start_pos);
 text->select_end_pos = d(select_end_pos);
 text->start_end = d(start_end);

 F(cpage->Calibration).calculSelection(cpage->Calibration, FALSE);
 F(cpage->Redraw).draw_selection_diff(cpage->Redraw, NULL);

 F(cpage->Calibration).makeCursorVisible(cpage->Calibration);

 d(source) = FALSE;
}

void select_text_by_mouse_done(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Edit_Text);
  
 GET_DATA(Select_Move_Edit_Text);
  
 if (d(AutoScroll))
 {
  F(cpage->Calibration->PageArea).DestroyAutoScroll
   (cpage->Calibration->PageArea, d(AutoScroll));
  d(AutoScroll) = (c_AutoScroll *)NULL;
 }
 cpage->in_use = FALSE;
 d(source) = FALSE;
 F(cpage->Calibration).makeCursorVisible(cpage->Calibration);
}

void select_text_by_mouse_to_xy(cpage, xm, ym)
c_Page	*cpage;
int	xm;
int	ym;
{
 coord_t	x, y;
 text_line_t	*tl;
 xa_item_t	*item;
 int		pos, direction;
 c_Page		*page;
 c_Text		*text = cpage->BaseStd->text_base;
 DATA(Select_Move_Edit_Text);
  
 GET_DATA(Select_Move_Edit_Text);
 
 if (xm < 0 || ym < 0 || xm > cpage->Width || ym > cpage->Height)
 {
  x = cpage->PosX; y = cpage->PosY;
  
  page = F(cpage->Calibration->PageArea).GetPageByXY
   (cpage->Calibration->PageArea, xm + x, ym + y);
  
#ifndef NTRACE
  if (page) Xc_TRACE(("founded page: %d", page->page_number));
  else Xc_TRACE(("no page found"));
#endif

  if (!page) return;
  
  xm = xm + x - page->PosX;
  ym = ym + y - page->PosY;

 }
 else
  page = cpage;

 x = xm * page->sScale;
 y = ym * page->sScale;

 F(page->Calibration).getTextLineByY(page->Calibration, page, y, x, &tl);
 if (!tl) return;

 if (!F(page->Redraw).get_item_by_x(tl, x, &item, &pos) &&
     (d(shift)||d(pressed)) && item->type == XA_LINEFEED)
  pos = 1;

 F(page->Redraw).start_selection_diff(cpage->Redraw);

 if (tl->line_number > d(stl)->line_number) direction = XA_RIGHT;
 else if (tl->line_number < d(stl)->line_number) direction = XA_LEFT;
 else if (x > d(sx)) direction = XA_RIGHT; else direction = XA_LEFT;

 F(text).goTo(text, XA_GLOBAL, direction, 1, item, pos, d(shift)||d(pressed));
 
 if (text->start_end)
  F(page->Calibration).checkSelectionOrder
   (text, tl, page->Calibration->tl_select_end);
 else
  F(cpage->Calibration).checkSelectionOrder
   (text, page->Calibration->tl_select_start, tl);
  
 F(page->Calibration).calculSelection(page->Calibration, FALSE);
 F(page->Redraw).draw_selection_diff(page->Redraw, NULL);
 
 d(stl) = tl;
 d(sx) = x;
 d(prems) = TRUE;
}

void select_text_by_mouse_trace(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Edit_Text);
  
 GET_DATA(Select_Move_Edit_Text);
 
}
