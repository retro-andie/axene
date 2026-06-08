/*
** Calibration.c for XAllWrite in Calibration/
** Calibrate the text: calcul text placements.
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
** Started on  Wed Apr 23 15:26:02 1997 Emmanuel Paris
** Last update Sun Jun 13 19:16:43 1999 Emmanuel Paris
*/

#include "Calibration.h"
#include "char_translate.h"

/*--- public functions ---*/
static void *cons_Calibration();
static void dest_Calibration();
static void *copy_Calibration();
static void init_Calibration ___PROTO((c_Calibration *This, c_Page *page));
static void calcul_selection ___PROTO((c_Calibration *This,boolean direction));
static void update_Text ___PROTO((c_Calibration *This));
static void make_cursor_visible ___PROTO((c_Calibration *This));
static void manage_page_selection ___PROTO((c_Calibration *This, c_Page *pg));
static void goto_begin_of_line ___PROTO((c_Calibration *This, boolean select));
static void goto_end_of_line ___PROTO((c_Calibration *This, boolean select));
static void goto_begin_of_page ___PROTO((c_Calibration *This, boolean select));
static void goto_end_of_page ___PROTO((c_Calibration *This, boolean select));
static void goto_upper_line ___PROTO((c_Calibration *This, boolean select));
static void goto_bottom_line ___PROTO((c_Calibration *This, boolean select));
static void goto_page_up ___PROTO((c_Calibration *This, boolean select));
static void goto_page_down ___PROTO((c_Calibration *This, boolean select));
static boolean get_TextLine_by_Y ___PROTO((c_Calibration *This, c_Page *page,
					   coord_t y, coord_t x,
					   text_line_t **rtl));
static void checkSelectionOrder ___PROTO((c_Text *txt,
					  text_line_t *tl_start,
					  text_line_t *tl_end));
extern boolean ps_print_text_lines ___PROTO((c_Calibration *This, 
					     c_PostScript *post, 
					     text_line_t *start, 
					     text_line_t *end));

/*--- private functions ---*/
static void calcul_page_selection ___PROTO((c_Calibration *This));
static void destroyTextLines ___PROTO((c_Calibration *This, text_line_t *tl));
static void calibrateWithRuler ___PROTO((c_Calibration *This, 
					 text_line_t *tl, boolean nl));

static text_line_t *calculTextLine(); /* ___PROTO((c_Calibration *This,
					 c_ScanLine *scanline, coord_t *y,
					 int line_number, boolean *np,
					 xa_item_t **item, 
					 c_TextRuler **ruler)); */
static coord_t calibrateSegment(); /* ___PROTO(c_Calibration	*This, 
				      xa_item_t **item, text_line_t *tl,
				      sl_Segment segment, 
				      c_Textruler **ruler, boolean *np,
				      boolean *nl)); */

static coord_t calibrateForWidth ___PROTO((xa_item_t *item, coord_t width,
					   boolean analyse_p));

static text_spot_t *create_text_spot ___PROTO((c_Calibration *This,
					       text_spot_t *ts, 
					       xa_item_t *item));
static void get_position_on_Textline ___PROTO((c_Calibration *This, 
					       c_Page *page,
					       text_line_t **rtl, coord_t x,
					       xa_item_t **item, int *pos,
					       boolean direction));
static c_Page *get_Page_by_TextLine ___PROTO((c_Calibration *This,
					      text_line_t *tl));
static boolean checkDiffTextLine ___PROTO((c_Calibration *This,
					   text_line_t *tl_start,
					   text_line_t *tl));

#define FONT_HEIGHT(style) \
((style)->metrics->font_bbox.ury - (style)->metrics->font_bbox.lly)

sf_Calibration fc_Calibration =
{
 cons_Calibration,
 dest_Calibration,
 copy_Calibration,
 init_Calibration,
 calcul_selection,
 update_Text,
 make_cursor_visible,
 manage_page_selection,
 goto_begin_of_line,
 goto_end_of_line,
 goto_begin_of_page,
 goto_end_of_page,
 goto_upper_line,
 goto_bottom_line,
 goto_page_down,
 goto_page_up,
 get_TextLine_by_Y,
 checkSelectionOrder,
 ps_print_text_lines
};

static void *cons_Calibration(BaseStd, PageArea)
BaseStd_t       *BaseStd;
c_PageArea	*PageArea;
{
 c_Calibration *This;

 Xc_HISTORY(("Constructor Calibration..."));

 if((This = (c_Calibration *)Xc_malloc("Calibration",
				       sizeof (c_Calibration))) == NULL)
  return NULL;
 memset(This, 0, sizeof(c_Calibration));
 This->f= &fc_Calibration;

 This->BaseStd = BaseStd;
 This->PageArea = PageArea;

 This->tl_handle = Xc_createItem("text_line", 100, sizeof(text_line_t));
 This->ts_handle = Xc_createItem("text_spot", 100, sizeof(text_spot_t));

 This->text_line = NULL;
 This->forceTextLine = This->newPageFlag = FALSE;
 Xc_TRACE(("Constructor Calibration done"));
 return This;
}

static void dest_Calibration(This)
c_Calibration *This;
{ 
 text_line_t	*tl, *tl2;
 text_spot_t	*ts, *ts2;

 Xc_HISTORY(("Destructor Calibration..."));

 tl = This->text_line;
 while(tl)
 {
  tl2 = tl->next;
  ts = tl->text_spot;
  while(ts)
  {
   ts2 = ts->next;
   Xc_delete(This->ts_handle, ts);
   ts = ts2;
  }
  Xc_delete(This->tl_handle, tl);
  tl = tl2;
 }

 Xc_destroyItem(This->tl_handle);
 Xc_destroyItem(This->ts_handle);
 
 Xc_free(This);
 Xc_TRACE(("Calibration Destroyed"));
}

static void *copy_Calibration(Orig)
c_Calibration *Orig;
{
 Xc_TRACE(("Copy Calibration not done."));
 return NULL;
}

static void init_Calibration(This, page)
c_Calibration *This;
c_Page *page;
{
 text_line_t	*otl, *tl;
 xa_item_t	*item;
 c_Text		*text;
 c_ScanLine	*scanline;
 c_TextRuler	*ruler;
 coord_t	y;
 boolean	np;		/* new_paragraph flag */
 int		line_number;
 
 text = This->BaseStd->text_base;
 if (page == NULL || page->page_number == 1)
 {
  item = This->BaseStd->text_base->item_base;
  ruler = item->linefeed.ruler;
  line_number = 0;
  np = TRUE;
  
  destroyTextLines(This, This->text_line);
  This->text_line = NULL;

  page = This->PageArea->PageList;
 }
 else
 {
  tl = page->text_line_start;
  item = tl->text_spot->item;
  line_number = tl->line_number;
  np = tl->new_paragraph;
  ruler = tl->ruler;

  destroyTextLines(This, tl);
 }

 scanline = page->ScanLine;
 This->tab_origin = page->left_margin;
 This->newPageFlag = FALSE;
 otl = NULL;

 y = F(scanline).startScanLine(scanline);

 while(item)
 {
  tl = calculTextLine(This, scanline, &y, line_number, &np, &item, &ruler);

 new_page:  
  if (tl == ((text_line_t *)-1))/* must change page or create it */
  {
   if (!otl)			/* there is no text line on this page ! */
   {
    y = F(scanline).startScanLine(scanline);
    This->forceTextLine = TRUE;
    continue;
   }
   page->text_line_end = otl;
   if (!page->NextPage)
   {
    F(This->PageArea).AddPage(This->PageArea);
    page = page->NextPage;
    F(page).Init(page, This->PageArea->display, This->PageArea->window, 
		 This->PageArea->sFactor);
    F(This->PageArea).Configure(This->PageArea);
   }
   else
    page = page->NextPage;
   
   scanline = page->ScanLine;
   This->tab_origin = page->left_margin;

   otl = NULL;
   y = F(scanline).startScanLine(scanline);

   continue;
  }
  else
  {
   line_number++;
   y = F(scanline).newLine(scanline, ruler, np, tl->height);
  }

  if (!otl)
  {
   otl = tl;
   if (!This->text_line)
    This->text_line = otl;
   else
   {
    tl->prev = page->PrevPage->text_line_end;
    tl->prev->next = tl;
   }
   page->text_line_start = otl;
   while(otl->next)
    otl = otl->next;
  }
  else
  {
   otl->next = tl;
   tl->prev = otl;
   while(tl->next)
    tl = tl->next;
   otl = tl;
  }
  
  if (This->newPageFlag)
  {
   This->newPageFlag = FALSE;
   tl = (text_line_t *)-1;
   goto new_page;
  }
 }
 
 page->text_line_end = otl;

 /* destroy all remaining and unused pages */
 if (page->NextPage)
 {
  F(This->PageArea).DelPage(This->PageArea, page->NextPage);
  F(This->PageArea).Configure(This->PageArea);
 }

 if (This->PageArea->select_type)
  calcul_page_selection(This);
}

static void update_Text(This)
c_Calibration *This;
{
 c_Page		*page;
 c_ScanLine	*scanline;
 c_TextRuler	*ruler;
 text_line_t	*tl_start, *tl_last = NULL;
 text_line_t	*tl, *stl;
 int		line_number, last_ln;
 int		start_ln, end_ln;
 boolean	np;
 xa_item_t	*item;
 boolean	diff;
 coord_t	y;
 
 tl_start = This->tl_select_start;
 if (tl_start->new_paragraph && tl_start->prev)
 {
  tl_start = tl_start->prev;
  line_number = tl_start->line_number;
  while(tl_start->prev && tl_start->prev->line_number == line_number)
   tl_start = tl_start->prev;
 }
 else
 {
  while(!tl_start->new_paragraph && tl_start->prev)
   tl_start = tl_start->prev;
 }
 
 stl = This->tl_select_end;
 while(stl->next && !stl->next->new_paragraph)
  stl = stl->next;
 last_ln = stl->line_number;
 
 upper:
 item = tl_start->text_spot->item;
 if (item->any.prev) 
 {
  if (item != item->any.prev->any.next)
  {
   tl_start = tl_start->prev;
   line_number = tl_start->line_number;
   while(tl_start->prev && tl_start->prev->line_number == line_number)
    tl_start = tl_start->prev;
   goto upper;
  }
 }
 else
  item = This->BaseStd->text_base->item_base;

 if (tl_start->prev) tl_last = tl_start->prev;

 Xc_TRACE(("recalcul from line %d to line %d", 
	   tl_start->line_number, last_ln));

 page = get_Page_by_TextLine(This, tl_start);
 scanline = page->ScanLine;
 start_ln = page->text_line_start->line_number;
 end_ln = page->text_line_end->line_number;
 This->tab_origin = page->left_margin;
 This->newPageFlag = FALSE;

 line_number = tl_start->line_number;
 if (item->type == XA_BEGIN)
  ruler = item->linefeed.ruler;
 else if (item->any.prev->type == XA_LINEFEED)
  ruler = item->any.prev->linefeed.ruler;
 else
  ruler = tl_start->ruler;
 np = tl_start->new_paragraph;
 y = tl_start->y;

 F(scanline).initScanLine(scanline, y);

 F(page->Redraw).start_draw_cache(page->Redraw);

 while(item)
 {
  tl = calculTextLine(This, scanline, &y, line_number, 
		      &np, &item, &ruler);

 new_page:
  if (tl == ((text_line_t *)-1))/* must change page or create it */
  {
   if (!tl_last || tl_start == page->text_line_start)
    /* there is no text line on this page ! */
   {
    if (tl_start)
     y = tl_start->y;
    else
     y = F(scanline).startScanLine(scanline);
    This->forceTextLine = TRUE;
    continue;
   }
   page->text_line_end = tl_last;

   if (tl_last->line_number < end_ln)
   {				/* set in redraw cache the clear of last line of the page */
    text_line_t *tlpt, *tlnt;
    
    tlpt = tl_last;
    while(tlpt->next && tlpt->next->line_number <= end_ln)
     tlpt = tlpt->next;
    
    tlnt = tlpt->next; tlpt->next = NULL;
    F(page->Redraw).redraw_text_line_diff(page->Redraw, page, NULL, tl_start);
    tlpt->next = tlnt;
   }

   F(page->Redraw).flush_redraw_cache(page->Redraw, page);

   if (!page->NextPage)
   {
    F(This->PageArea).AddPage(This->PageArea);
    page = page->NextPage;
    F(page).Init(page, This->PageArea->display, This->PageArea->window, 
		 This->PageArea->sFactor);
    F(This->PageArea).Configure(This->PageArea);
   }
   else
   {
    page = page->NextPage;
    start_ln = page->text_line_start->line_number;
    end_ln = page->text_line_end->line_number;
   }
   scanline = page->ScanLine;
   This->tab_origin = page->left_margin;

   y = F(scanline).startScanLine(scanline);
   page->text_line_start = tl_start;

   F(page->Redraw).start_draw_cache(page->Redraw);
   
   continue;
  }

  diff = checkDiffTextLine(This, tl_start, tl);

  Xc_TRACE(("line: %d has%schanged", line_number, diff ? " " : " not "));
  
  if (tl_start == page->text_line_start)
   page->text_line_start = tl;
  
  if (tl_start)
  {
   if (tl_start == This->text_line)
   {
    This->text_line = tl;
   }
   else
   {
    tl->prev = tl_start->prev;
    tl_start->prev->next = tl;
   }
   tl_start->prev = NULL;
   
   stl = tl_start;
   while(stl && stl->line_number == tl_start->line_number)
    stl = stl->next;
   if (stl)
    stl->prev->next = NULL;
   
   
   if (diff)
   {
    if (tl_start->line_number > end_ln)
    {
     Xc_TRACE(("clear line in next page"));
     F(page->Redraw).redraw_text_line_diff(page->Redraw, page, tl, NULL);
     page->Redraw->redraw_cache = FALSE;
     F(page->Redraw).redraw_text_line_diff(page->Redraw, 
					   page->NextPage, NULL, tl_start);
     page->Redraw->redraw_cache = TRUE;
    }
    else 
     if (tl_start->line_number < start_ln)
     {
      Xc_TRACE(("clear line in prev page"));
      F(page->Redraw).redraw_text_line_diff(page->Redraw, page, tl, NULL);
     }
     else
      F(page->Redraw).redraw_text_line_diff(page->Redraw, page, tl, tl_start);
   }
   destroyTextLines(This, tl_start);
  }
  else
  {
   tl_last->next = tl;
   tl->prev = tl_last;
   stl = NULL;
   if (diff)
    F(page->Redraw).redraw_text_line_diff(page->Redraw, page, tl, NULL);
  }
   
  while(tl && tl->next)
   tl = tl->next;
   
  if (stl)
   stl->prev = tl;

  tl_last = tl;
  tl->next = stl;
  tl_start = stl;

  
  if (!diff && line_number > last_ln)
   break;
  
  line_number++;
  y = F(scanline).newLine(scanline, ruler, np, tl->height);

  if (This->newPageFlag)
  {
   This->newPageFlag = FALSE;
   tl = (text_line_t *)-1;
   goto new_page;
  }
 }
 
 if (!item)
 {
  if (tl_start)
  {
   tl_start->prev->next = NULL;
   stl = tl_start;
   while(stl && stl->line_number <= page->text_line_end->line_number)
   {
    F(page->Redraw).redraw_text_line_diff(page->Redraw, page, NULL, stl);
    stl = stl->next;
   }
   destroyTextLines(This, tl_start);
  }
  page->text_line_end = tl_last;

  if (page->NextPage)
  {
   F(This->PageArea).DelPage(This->PageArea, page->NextPage);
   F(This->PageArea).Configure(This->PageArea);
  }
 }
 else
 {
  text_spot_t *ts;
  
  ts = tl_start->text_spot;
  while(ts && ts->item != item)
  {
   ts->item = item;
   ts = ts->next;
   item = item->any.next;
  }

  if (page->text_line_end->line_number <= tl_last->line_number)
   page->text_line_end = tl_last;
 }

 F(page->Redraw).flush_redraw_cache(page->Redraw, page);

 calcul_page_selection(This);

#if 0
{
 text_spot_t *ts;
 xa_item_t   *item;

 tl_start = This->text_line;
 while(tl_start)
 {
  printf("**************************************************\nline: %d\n",
	 tl_start->line_number);
  ts = tl_start->text_spot;
  while(ts)
  {
   switch(ts->item->type)
   {
   case XA_TEXT:
    printf("\tXA_TEXT (%p): `%s'\n", ts->item, ts->item->text.text);
    break;
   case XA_LINEFEED:
    printf("\tXA_LINEFEED (%p)\n", ts->item);
    break;
   case XA_END:
    printf("\tXA_END (%p)\n", ts->item);
    break;
   case XA_BEGIN:
    printf("\tXA_BEGIN (%p)\n", ts->item);
    break;
   default:
    printf("\tOTHER (%p)\n", ts->item);
    break;
   }
   ts = ts->next;
  }
  tl_start = tl_start->next;
 }
  
 printf("\n\n");
 item = This->BaseStd->text_base->item_base;
 while(item)
 {
  switch(item->type)
  {
  case XA_TEXT:
   printf("\tXA_TEXT (%p): `%s'\n", item, item->text.text);
   break;
  case XA_LINEFEED:
   printf("\tXA_LINEFEED (%p)\n", item);
   break;
  case XA_END:
   printf("\tXA_END (%p)\n", item);
   break;
  case XA_BEGIN:
   printf("\tXA_BEGIN (%p)\n", item);
   break;
  default:
   printf("\tOTHER (%p)\n", item);
   break;
  }
  item = item->any.next;
 }
}
#endif

#if 0
{
 tl_start = This->text_line;
  
 while(tl_start)
 {
  if (!tl_start->prev) printf("]<-");

  if (tl_start == This->tl_select_start) printf("{");
  printf("[%p/%d]", tl_start, tl_start->line_number);
  if (tl_start == This->tl_select_end) printf("}");
   
  if (!tl_start->next) printf("->[\n");
  else
  {
   if (tl_start->next->prev != tl_start)
    printf("*!*->"); else printf("<->");
  }
  tl_start = tl_start->next;
 }
}
#endif

}

static void goto_begin_of_line(This, select)
c_Calibration *This;
boolean	      select;
{
 c_Text		*text;
 text_line_t	*tl_start;
 
 text = This->BaseStd->text_base;
 if (text->start_end)
  tl_start = This->tl_select_start;
 else
  tl_start = This->tl_select_end;
 
 F(text).goTo(text, XA_GLOBAL, XA_LEFT, 0, 
	      tl_start->text_spot->item, 0, select);

 F(This).calculSelection(This, FALSE);
}

static void goto_end_of_line(This, select)
c_Calibration *This;
boolean	      select;
{
 c_Text		*text;
 text_line_t	*tl_start;
 text_spot_t	*ts;
 
 text = This->BaseStd->text_base;
 if (text->start_end)
  tl_start = This->tl_select_start;
 else
  tl_start = This->tl_select_end;
 
 ts = tl_start->text_spot;
 while(ts->next)
 {
  if (!select && ts->next->item->type == XA_LINEFEED) break;
  ts = ts->next;
 }
 
 if (ts->item->type == XA_TEXT)
  F(text).goTo(text, XA_GLOBAL, XA_RIGHT, 0, ts->item, 
	       ts->item->text.text_len, select);
 else
  F(text).goTo(text, XA_GLOBAL, XA_RIGHT, 0, ts->item, 1, select);
 
 F(This).calculSelection(This, TRUE);
}

static void goto_begin_of_page(This, select)
c_Calibration *This;
boolean	      select;
{
 c_Text		*text;
 c_Page		*page;
 
 text = This->BaseStd->text_base;
 
 page = get_Page_by_TextLine(This, This->tl_select_start);
 if (page && page->text_line_start)
 {
  F(text).goTo(text, XA_GLOBAL, XA_LEFT, 0, 
	       page->text_line_start->text_spot->item, 0, select);
  
  F(This).calculSelection(This, FALSE);
 }
}

static void goto_end_of_page(This, select)
c_Calibration *This;
boolean	      select;
{
 c_Text		*text;
 c_Page		*page;
 text_line_t	*tl_start;
 text_spot_t	*ts;
 
 text = This->BaseStd->text_base;
 
 page = get_Page_by_TextLine(This, This->tl_select_start);
 if (page && page->text_line_start)
 {
  tl_start = page->text_line_end;
  
  ts = tl_start->text_spot;
  while(ts->next)
  {
   if (!select && ts->next->item->type == XA_LINEFEED) break;
   ts = ts->next;
  }
  
  if (ts->item->type == XA_TEXT)
   F(text).goTo(text, XA_GLOBAL, XA_RIGHT, 0, ts->item, 
		ts->item->text.text_len, select);
  else
   F(text).goTo(text, XA_GLOBAL, XA_RIGHT, 0, ts->item, 1, select);
  
  F(This).calculSelection(This, FALSE);
 }
}

static void goto_upper_line(This, select)
c_Calibration *This;
boolean select;
{
 c_Text *text;
 c_Page *page;
 xa_item_t	*item;
 int		pos, ln;
 coord_t	scr_pos;
 text_line_t	*tl_start;

 text = This->BaseStd->text_base; 
 if (text->start_end)
 {
  item = text->select_start;
  pos = text->select_start_pos;
  tl_start = This->tl_select_start;
 }
 else
 {
  item = text->select_end;
  pos = text->select_end_pos;
  tl_start = This->tl_select_end;
 }

 if (!tl_start->prev)
  return;			/* Already on the first line */

 page = get_Page_by_TextLine(This, tl_start);

 if (item == This->ubl_select && pos == This->ubl_select_pos)
  scr_pos = This->ubl_scr_pos;
 else
 {
  scr_pos = F(page->Redraw).get_x_by_item(tl_start, NULL, item, pos);
  This->ubl_scr_pos = scr_pos;
 }

 ln = tl_start->line_number;
 while(tl_start && tl_start->line_number == ln)
  tl_start = tl_start->prev;
 if (!tl_start) return;
 ln --;
 while(tl_start->prev && tl_start->prev->line_number == ln)
  tl_start = tl_start->prev;
 if (!tl_start) return;
 
 get_position_on_Textline(This, page, &tl_start, scr_pos, &item, &pos, TRUE);
 
 F(text).goTo(text, XA_LINE, XA_LEFT, 0, item, pos, select);

 if (text->start_end)
  checkSelectionOrder(text, tl_start, This->tl_select_end);
 else
  checkSelectionOrder(text, This->tl_select_start, tl_start);

 F(This).calculSelection(This, FALSE);
 
 if (text->start_end)
 {
  This->ubl_select = text->select_start;
  This->ubl_select_pos = text->select_start_pos;
 }
 else
 {
  This->ubl_select = text->select_end;
  This->ubl_select_pos = text->select_end_pos;
 }
}

static void goto_bottom_line(This, select)
c_Calibration *This;
boolean select;
{
 c_Text *text;
 c_Page *page;
 xa_item_t	*item;
 int		pos, ln;
 coord_t	scr_pos;
 text_line_t	*tl_start;

 text = This->BaseStd->text_base; 
 if (text->start_end)
 {
  item = text->select_start;
  pos = text->select_start_pos;
  tl_start = This->tl_select_start;
 }
 else
 {
  item = text->select_end;
  pos = text->select_end_pos;
  tl_start = This->tl_select_end;
 }

 if (!tl_start->next)
  return;			/* Already on the last line */

 page = get_Page_by_TextLine(This, tl_start);

 if (item == This->ubl_select && pos == This->ubl_select_pos)
  scr_pos = This->ubl_scr_pos;
 else
 {
  scr_pos = F(page->Redraw).get_x_by_item(tl_start, NULL, item, pos);
  This->ubl_scr_pos = scr_pos;
 }
 
 ln = tl_start->line_number;
 while(tl_start && tl_start->line_number == ln)
  tl_start = tl_start->next;
 if (!tl_start) return;
 
 get_position_on_Textline(This, page, &tl_start, scr_pos, &item, &pos, 
			  !select);
  
 F(text).goTo(text, XA_LINE, XA_RIGHT, 0, item, pos, select);

 if (text->start_end)
  checkSelectionOrder(text, tl_start, This->tl_select_end);
 else
  checkSelectionOrder(text, This->tl_select_start, tl_start);
 
 F(This).calculSelection(This, TRUE);

 if (text->start_end)
 {
  This->ubl_select = text->select_start;
  This->ubl_select_pos = text->select_start_pos;
 }
 else
 {
  This->ubl_select = text->select_end;
  This->ubl_select_pos = text->select_end_pos;
 }
}

static void goto_page_up(This, select)
c_Calibration *This;
boolean select;
{
 c_Text *text;
 c_Page *page;
 xa_item_t	*item;
 int		pos;
 coord_t	y, pageh, scr_pos;
 text_line_t	*tl_start;
 
 text = This->BaseStd->text_base; 
 if (text->start_end)
 {
  item = text->select_start;
  pos = text->select_start_pos;
  tl_start = This->tl_select_start;
 }
 else
 {
  item = text->select_end;
  pos = text->select_end_pos;
  tl_start = This->tl_select_end;
 }

 page = get_Page_by_TextLine(This, tl_start);

 if (item == This->ubl_select && pos == This->ubl_select_pos)
  scr_pos = This->ubl_scr_pos;
 else
 {
  scr_pos = F(page->Redraw).get_x_by_item(tl_start, NULL, item, pos);
  This->ubl_scr_pos = scr_pos;
 }
 
 y = 0;
 pageh = (page->sHeight >> 2);
 while(tl_start->next && y < pageh)
 {
  if (tl_start->next->y < tl_start->y)
  {				/* tl_start and tl_start->prev on two different pages */
   y += page->sHeight - tl_start->y;
   page = page->NextPage;
   y += tl_start->next->y;
  }
  else				/* tl_start and tl_start->prev on the same page */
   y +=  tl_start->next->y - tl_start->y;

  tl_start = tl_start->next;
 }

 F(page->Redraw).get_item_by_x(tl_start, scr_pos, &item, &pos);
 
 F(text).goTo(text, XA_GLOBAL, XA_LEFT, 0, item, pos, select);

 if (text->start_end)
  checkSelectionOrder(text, tl_start, This->tl_select_end);
 else
  checkSelectionOrder(text, This->tl_select_start, tl_start);
 
 F(This).calculSelection(This, TRUE);

 if (text->start_end)
 {
  This->ubl_select = text->select_start;
  This->ubl_select_pos = text->select_start_pos;
 }
 else
 {
  This->ubl_select = text->select_end;
  This->ubl_select_pos = text->select_end_pos;
 }
}

static void goto_page_down(This, select)
c_Calibration *This;
boolean select;
{
 c_Text *text;
 c_Page *page;
 xa_item_t	*item;
 int		pos;
 coord_t	y, pageh, scr_pos;
 text_line_t	*tl_start;
 
 text = This->BaseStd->text_base; 
 if (text->start_end)
 {
  item = text->select_start;
  pos = text->select_start_pos;
  tl_start = This->tl_select_start;
 }
 else
 {
  item = text->select_end;
  pos = text->select_end_pos;
  tl_start = This->tl_select_end;
 }

 page = get_Page_by_TextLine(This, tl_start);

 if (item == This->ubl_select && pos == This->ubl_select_pos)
  scr_pos = This->ubl_scr_pos;
 else
 {
  scr_pos = F(page->Redraw).get_x_by_item(tl_start, NULL, item, pos);
  This->ubl_scr_pos = scr_pos;
 }
 
 y = 0;
 pageh = (page->sHeight >> 2);
 while(tl_start->prev && y < pageh)
 {
  if (tl_start->prev->y > tl_start->y)
  {				/* tl_start and tl_start->prev on two different pages */
   y += tl_start->y;
   page = page->PrevPage;
   y += page->sHeight - tl_start->prev->y;
  }
  else				/* tl_start and tl_start->prev on the same page */
   y +=  tl_start->y - tl_start->prev->y;

  tl_start = tl_start->prev;
 }

 F(page->Redraw).get_item_by_x(tl_start, scr_pos, &item, &pos);
 
 F(text).goTo(text, XA_GLOBAL, XA_RIGHT, 0, item, pos, select);

 if (text->start_end)
  checkSelectionOrder(text, tl_start, This->tl_select_end);
 else
  checkSelectionOrder(text, This->tl_select_start, tl_start);
 
 F(This).calculSelection(This, TRUE);

 if (text->start_end)
 {
  This->ubl_select = text->select_start;
  This->ubl_select_pos = text->select_start_pos;
 }
 else
 {
  This->ubl_select = text->select_end;
  This->ubl_select_pos = text->select_end_pos;
 }
}

static text_line_t *calculTextLine(This, scanline, y, line_number,
				   np, item, ruler)
c_Calibration *This;
c_ScanLine *scanline;
coord_t	*y;
int line_number;
boolean *np;
xa_item_t	**item;
c_TextRuler	**ruler;
{
 c_Text		*text;
 text_line_t	*tl, *stl;
 text_spot_t	*ts;
 boolean	snp;		/* new_paragraph flag */
 boolean	nl;		/* new_line flag */
 int		recurse_iteration;
 int		i, nb_seg;
 sl_Segment	*segs, forceseg[1];
 coord_t	sy, h, height, sheight;
 coord_t	descend, max_descend = COORD_ZERO;
 xa_item_t	*sitem, *svitem;
 c_TextRuler	*sruler;

 height = FONT_HEIGHT((*item)->any.style);
 text = This->BaseStd->text_base;

 next_line:
 
 recurse_iteration = 0;
 sy = *y;

 do
 {
  snp = (((*item)->type == XA_PAGEBREAK) ? FALSE : *np);
  
  segs = F(scanline).getNextScanLine(scanline, *ruler, snp, height, y,
				     &nb_seg);
  if (nb_seg == -1)
  {
   if (This->forceTextLine)
   {
    segs = F(scanline).getNextScanLine(scanline, *ruler, snp, COORD_ZERO, y,
				       &nb_seg);    
    if (nb_seg == -1)
    {
     segs = forceseg; *y = sy;
     segs[0].x_min = COORD_ZERO; segs[0].x_max = COORD_HALF_ONE;
     nb_seg = 1;
    }
   }
   else
    return (text_line_t *)-1;
  }

  sitem = *item; sheight = height; sruler = *ruler; snp = *np;
  stl = tl = NULL;
  nl = FALSE;
  F(sruler).initGetTab(sruler);

  for(i = 0; i < nb_seg; i++)
  {
   if (tl)
   {
    tl->next = (text_line_t *)Xc_new(This->tl_handle);
    tl->next->prev = tl;
    tl = tl->next;
   }
   else
   {
    stl = tl = (text_line_t *)Xc_new(This->tl_handle);
    tl->prev = NULL;
   }
   
   tl->new_paragraph = ((i == 0) ? snp : FALSE);
   tl->text_spot = NULL;
   tl->line_number = line_number;
   tl->ruler = *ruler;
   tl->y = *y;
   tl->x_min = segs[i].x_min;
   tl->x_max = segs[i].x_max;
   tl->char_inc = tl->word_inc = COORD_ZERO;
   tl->next = NULL;
   
   svitem = *item;
   h = calibrateSegment(This, item, tl, segs[i], ruler, np, &nl);

   if (!tl->text_spot)
   {
    if (tl->prev)
    {
     tl = tl->prev;
     Xc_delete(This->tl_handle, tl->next);
     tl->next = NULL;
    }
    else
    {
     Xc_delete(This->tl_handle, tl);
     stl = tl = NULL;
    }
    if (svitem->any.prev)
     *item = svitem->any.prev->any.next;
    else
     *item = text->item_base;
    
    continue;
   }

   if (h > height)
    height = h;
   
   /* if new_line or new_paragraph, skips remaining segments */
   if (nl || *np) 
    break;
  }
  /* if line height is similar to scanline ask or too many recursion, break */
  if (!tl || height == sheight ||
      (recurse_iteration > 2 && height < sheight))
   break;
  
  if (sitem->any.prev)
   *item = sitem->any.prev->any.next;
  else
   *item = text->item_base;

  sheight = height; *ruler = sruler; *np = snp; *y = sy;
  destroyTextLines(This, tl);
  recurse_iteration ++;
  
 } while(1);
 
 if (!stl)
 {
  *y = F(scanline).newLine(scanline, *ruler, *np, height);
  goto next_line;
 }

 tl = stl;
 while(tl)
 {
  ts = tl->text_spot;
  while(ts)
  {
   descend = -ts->item->any.style->metrics->descender;
   if (descend > max_descend)
    max_descend = descend;
   ts = ts->next;
  }
  tl = tl->next;
 }
 
 tl = stl;
 while(tl)
 {
  tl->height = height;
  calibrateWithRuler(This, tl, ((tl->next != NULL) ? FALSE : nl));
  
  ts = tl->text_spot;
  while(ts)
  {
   ts->y_offset = tl->height - max_descend;
   
   if (ts->item == text->select_start)
   {
    This->tl_select_start = tl;
    Xc_TRACE(("select start found in line: %d (%p)", tl->line_number, tl));
   }
   
   if (ts->item == text->select_end)
   {
    This->tl_select_end = tl;
    Xc_TRACE(("select end found in line: %d (%p)", tl->line_number, tl));
   }
   
   ts = ts->next;
  }
  tl = tl->next;
 } 
 
 This->forceTextLine = FALSE;
 return stl;
}

static void calibrateWithRuler(This, tl, nl)
c_Calibration *This;
text_line_t   *tl;
boolean	      nl;
{
 c_TextRuler	*ruler;
 text_spot_t	*ts, *last_ts = NULL;
 coord_t	x, tl_width;
 textruler_just_t justification;
  
 ruler = tl->ruler;
 
 ts = tl->text_spot;
 tl_width = COORD_ZERO;
 while(ts)
 {
  tl_width += ts->width;
  last_ts = ts;
  ts = ts->next;
 }
 
 justification = ruler->justification;
 if (justification == XcTR_JUSTIFIED && nl)
  justification = XcTR_LEFT;
 
 switch(justification)
 {
 case XcTR_LEFT:
  x = tl->x_min;
  ts = tl->text_spot;
  while(ts)
  {
   ts->x = x;
   x += ts->width;
   ts = ts->next;
  } 
  break;
 case XcTR_RIGHT:
  x = tl->x_max - tl_width;
  ts = tl->text_spot;
  while(ts)
  {
   ts->x = x;
   x += ts->width;
   ts = ts->next;
  } 
  break;
 case XcTR_CENTER:
  x = (tl->x_min + tl->x_max - tl_width) >> 1;
  ts = tl->text_spot;
  while(ts)
  {
   ts->x = x;
   x += ts->width;
   ts = ts->next;
  } 
  break;
 case XcTR_JUSTIFIED:
  {
   int nb_car, nb_space, i;
   xa_item_t	*item;
   c_TextStyle	*style;
   coord_t	disp_width, space_width;
   
   nb_car = nb_space = 0;
   
   ts = tl->text_spot;
   while(ts)
   {
    item = ts->item;
    switch(item->type)
    {
    case XA_TEXT:
     nb_car += item->text.text_len;
     for(i = 0; i < item->text.text_len; i++)
      if (item->text.text[i] == ' ') nb_space++;
    case XA_LINEFEED:
    case XA_END:
    case XA_BEGIN:
    case XA_PAGEBREAK:
     break;
    default:
     nb_car++;
    }
    ts = ts->next;
   }
   
   disp_width = (tl->x_max - tl->x_min) - tl_width;
   
   if (nb_space > 1)
   {
    nb_space--; nb_car--;
    space_width = disp_width * SCALE_TO_COEF(ruler->word_distribution);
    tl->word_inc = space_width / nb_space;
    tl->char_inc = (disp_width - space_width) / nb_car;
   }
   else if (nb_car > 1)
    tl->char_inc = disp_width / (nb_car - 1);
   
   ts = tl->text_spot;
   x = tl->x_min;
   while(ts)
   {
    item = ts->item;
    style = item->any.style;
    
    ts->x = x;
    switch(item->type)
    {
    case XA_TEXT:
     ts->width = F(style->metrics).textWidthSpec
      (style->metrics, item->text.text, item->text.text_len, 
       tl->char_inc, tl->word_inc);
     break;
    case XA_LINEFEED:
    case XA_END:
    case XA_TAB:
    case XA_BULLET:
     ts->width += tl->char_inc;
     break;
    default:
     ts->width = 0;
     break;
    }
    
    x += ts->width; 
    ts = ts->next;
   }
  }
  break;
 }

 if (last_ts->item->type == XA_PAGEBREAK)
  last_ts->width = tl->x_max - last_ts->x;

}

static void destroyTextLines(This, tl)
c_Calibration	*This;
text_line_t	*tl;
{
 text_line_t *tl2;
 text_spot_t *ts, *ts2;

 while(tl)
 {
  tl2 = tl->next;
  ts = tl->text_spot;
  while(ts)
  {
   ts2 = ts->next;
   Xc_delete(This->ts_handle, ts);
   ts = ts2;
  }
  Xc_delete(This->tl_handle, tl);
  tl = tl2;
 }
}

static coord_t calibrateSegment(This, ritem, tl, segment, rruler, rnp, rnl)
c_Calibration	*This;
xa_item_t	**ritem;
text_line_t	*tl;
sl_Segment	segment;
c_TextRuler	**rruler;
boolean		*rnp;
boolean		*rnl;
{
 xa_item_t	*item = *ritem;
 c_TextRuler	*ruler = *rruler;
 boolean	nl = FALSE, np = FALSE, line_end = FALSE;
 c_Text		*text;
 text_spot_t	*ts, *tsc, *tsc2;
 c_TextStyle	*style;
 tab_just_t	ttype;
 char		tfiller;
 coord_t	swidth, twidth,	width, height, opos, pos;
 int		i;

 swidth = segment.x_max - segment.x_min;
 text = This->BaseStd->text_base;
 ts = NULL;
 width = COORD_ZERO;

 ruler = *rruler;
 do
 {  
  style = item->any.style;
  twidth = COORD_ZERO;
  
  switch(item->type)
  {
  case XA_TEXT:
   F(text).pack(text, &item);
   twidth = F(style->metrics).textWidth(style->metrics, item->text.text,
					item->text.text_len);
   break;
  case XA_BEGIN:
   twidth = COORD_ZERO;
   ruler = item->linefeed.ruler;
   break;
  case XA_LINEFEED:
   np = TRUE;
   ruler = item->linefeed.ruler;
  case XA_END:
   twidth = F(style->metrics).textWidth(style->metrics, "\266", 1);
   nl = TRUE;
   break;
  case XA_TAB:
   opos = pos = segment.x_min + width - This->tab_origin;
   F(ruler).getNextTab(ruler, &pos, &ttype, &tfiller);
   
   if (!tfiller) tfiller = ' ';
   item->tab.filler = tfiller;
   /*if (pos + This->tab_origin>= segment.x_max) { nl = TRUE; break; }*/
   
   switch(ttype)
   {
   case XcTAB_LEFT:
    twidth = pos - opos;
    break;
   case XcTAB_RIGHT:
    twidth = calibrateForWidth(item, pos - opos, FALSE);
    break;
   case XcTAB_CENTER:
    twidth = calibrateForWidth(item, (pos - opos) * 2, FALSE) / 2;
    break;
   case XcTAB_CPERIOD:
    twidth = calibrateForWidth(item, pos - opos, TRUE);
    break;
   }
   if (twidth < 0) twidth = 0;
   break;
  case XA_BULLET:
   if (width == COORD_ZERO)
   {
    twidth = SCALE_FROM_MILLIMETERS(10) * item->bullet.bullet_level;
   }
   else
   {
    nl = TRUE;
    line_end = TRUE;
   }
   break;
  case XA_PAGEBREAK:
   twidth = 0;
   nl = TRUE;
   np = *rnp;
   This->newPageFlag = TRUE;
   break;
  default:
   break;
  }
  
  if (line_end) break;
  
  if (width + twidth < swidth || nl)
  {
   if (ts == NULL)
    tl->text_spot = ts = create_text_spot(This, NULL, item);
   else
    ts = create_text_spot(This, ts, item);
   ts->width = twidth;
   
   width += twidth;
  }
  else
  {	
   if (item->type == XA_TEXT)	/* here must cut TEXT block */
   {
    char	*ptr;
    coord_t	remain_width = swidth - width, owidth;
    xa_item_t	*sitem = item;
    int		cut_pos;

    ptr = item->text.text;
    i = 0;
    owidth = COORD_ZERO;
    while( (owidth += F(style->metrics).textWidth(style->metrics, ptr + i, 1))
	  < remain_width)
    {
     twidth = owidth;
     i++;
    }
    
    if (This->forceTextLine && !i)/* force 1 character */
     i = 1;
    else
     i--;
    cut_pos = i;
    
    while(i < 0 || !Xc_IS_BREAKABLE(ptr[i]))
    {
     i--;
     if (i < 0)
     {
      item = item->any.prev;
      if (item == (*ritem)->any.prev || item->type != XA_TEXT) 
      {
       line_end = TRUE;
       if (cut_pos <= 0)
       {
	item = sitem;
	break;
       }
       
       F(text).cut_line(text, sitem, cut_pos);
       
       if (ts == NULL)
	tl->text_spot = ts = create_text_spot(This, NULL, sitem);
       else
	ts = create_text_spot(This, ts, sitem);
       ts->width = twidth;
       
       item = sitem->any.next;
       break;
      }
      
      ptr = item->text.text;
      i = item->text.text_len - 1;
     }
    } 
    if (line_end) 
     break;

    F(text).cut_line(text, item, i + 1);
    
    tsc = tl->text_spot;
    while(tsc && tsc->item != item)
     tsc = tsc->next;

    if (tsc)
    {
     ts = tsc;
     tsc = tsc->next;
     while(tsc)
     {
      tsc2 = tsc->next;
      Xc_delete(This->ts_handle, tsc);
      tsc = tsc2;
     }
     ts->next = NULL;
     line_end = TRUE;
    }
    else
    {
     if (ts == NULL)
      tl->text_spot = ts = create_text_spot(This, NULL, item);
     else
      ts = create_text_spot(This, ts, item);
    }
  
    ts->width = F(style->metrics).textWidth(style->metrics, item->text.text,
					    item->text.text_len);
    line_end = TRUE;
   }
   else				/* here create a text_spot that fill the remaining space */
   {
    if (ts == NULL)
     tl->text_spot = ts = create_text_spot(This, NULL, item);
    else
     ts = create_text_spot(This, ts, item);
    ts->width = swidth - width;
    
    line_end = TRUE;
   }
  }
  
  item = item->any.next;
  if (line_end || nl || !item) break;
 }
 while(width < swidth);

 if (!tl->text_spot || 
     (tl->text_spot->item->type == XA_BEGIN && !tl->text_spot->next))
 {
  height = FONT_HEIGHT((*ritem)->any.style);
  tl->text_spot = NULL;
  return height;
 }
 
 *rruler = ruler;
 *ritem = item;
 *rnp = np;
 *rnl = nl;
 
 height = COORD_ZERO;
 ts = tl->text_spot;
 while(ts)
 {
  if (FONT_HEIGHT(ts->item->any.style) > height)
   height = FONT_HEIGHT(ts->item->any.style);
  ts = ts->next;
 }
 
 return height;
}

static text_spot_t *create_text_spot(This, ts, item)
c_Calibration	*This;
text_spot_t	*ts;
xa_item_t	*item;
{
 if (ts)
 {
  ts->next = (text_spot_t *)Xc_new(This->ts_handle);
  ts = ts->next;
 }
 else
  ts = (text_spot_t *)Xc_new(This->ts_handle);
 
 ts->x = ts->width = ts->y_offset = COORD_ZERO;
 ts->item = item;
 ts->next = NULL;
 return ts;
}

/* ----------------------------------------------------------------- ** 
** calibrateForWidth: use for Tabulation space calcul                ** 
** ----------------------------------------------------------------- */
static coord_t calibrateForWidth(item, width, analyse_p)
xa_item_t *item;
coord_t width;
boolean analyse_p;
{
 c_TextStyle	*style;
 coord_t twidth = COORD_ZERO;
 boolean digit_found = FALSE;
 int i;

 item = item->any.next;
 style = item->any.style;
 
 while(item->type == XA_TEXT && twidth < width)
 {
  if (analyse_p)
  {
   for(i = 0; i < item->text.text_len; i++)
   {
    if (Xc_IS_DIGIT(item->text.text[i]))
    {
     if (!digit_found) digit_found = TRUE;
    }
    else
    {
     if (digit_found || item->text.text[i] == ',' ) break;
    }
   }
   
   if (i != item->text.text_len)
   {
    twidth += F(style->metrics).textWidth(style->metrics, item->text.text, i) +
     F(style->metrics).textWidth(style->metrics, item->text.text + i, 1) / 2;
    break;
   }
   
  }

  twidth += F(style->metrics).textWidth(style->metrics, item->text.text,
					item->text.text_len);
  item = item->any.next;
  style = item->any.style;
 }

 return width - twidth;
}

static void calcul_selection(This, direction)
c_Calibration *This;
boolean	direction;
{
 c_Text	     *text;
 text_line_t *tl;
 text_spot_t *ts;
 xa_item_t   *select_start;
 xa_item_t   *select_end;

 text = This->BaseStd->text_base;
 select_start = text->select_start;
 select_end = text->select_end;
 
 if (text->start_end)
  tl = This->tl_select_start;
 else
  tl = This->tl_select_end;

 if (!tl) { tl = This->text_line; direction = TRUE; }
 
 do
 {
  ts = tl->text_spot;
  while(ts && ts->item != select_start)
   ts = ts->next;
  
  if (ts)
  {
   This->tl_select_start = tl;
   break;
  }
  
  if (direction)		/* direction == TRUE: seek forward */
   tl = tl->next;
  else				/* direction == FALSE: seek backward */
   tl = tl->prev;
  
  if (!tl)
  {
   calcul_selection(This, !direction);
   break;
  }
 }
 while(1);
 
 tl = This->tl_select_start;
 if (select_start == select_end)
 {
  This->tl_select_end = tl;
 }
 else
 {
  while(tl)
  {
   ts = tl->text_spot;
   while(ts && ts->item != select_end)
    ts = ts->next;
   if (ts)
   {
    This->tl_select_end = tl;
    break;
   }
   tl = tl->next;
  }
 
  if (!tl)
  {
   This->tl_select_end = This->tl_select_start;
   Xc_WARNING(("End of selection item not found !!"));
  }
 }
 calcul_page_selection(This);
}

static void manage_page_selection(This, page)
c_Calibration *This;
c_Page *page;
{
 c_TextRuler *ruler = This->BaseStd->text_base->ruler;

 if (page)
 {
  if (This->PageArea->SelectedPage == page)
  {
   F(This->PageArea->RegleH).Init_XaRegle_By_Ruler(This->PageArea->RegleH, 
						   ruler); 
  }
  else
  {
   This->PageArea->SelectedPage = page;
   F(This->PageArea->Pager).Configure(This->PageArea->Pager);
   F(This->PageArea->RegleV).Configure(This->PageArea->RegleV, page, NULL);
   F(This->PageArea->RegleH).Configure(This->PageArea->RegleH, page, ruler); 
  }
 }
 else
  calcul_page_selection(This);
}

static void calcul_page_selection(This)
c_Calibration *This;
{
 c_Page	*pagel;
 int	line_num;
 c_Text	     *text;
 
 text = This->BaseStd->text_base;
 pagel = This->PageArea->SelectedPage;

 if (This->tl_select_start != This->tl_select_end &&
     F(pagel->Redraw).is_last_pos_in_textline(This->tl_select_start,
					      text->select_start,
					      text->select_start_pos))
 {
  text->select_start = text->select_start->any.next;
  text->select_start_pos = 0;
  This->tl_select_start = This->tl_select_start->next;
 }
 
 if (This->BaseStd->text_base->start_end)
 {
  line_num = This->tl_select_start->line_number;
 }
 else
 {
  line_num = This->tl_select_end->line_number;
 }

 if (line_num < pagel->text_line_start->line_number)
 {
  do
  {
   pagel = pagel->PrevPage;
  } 
  while(line_num < pagel->text_line_start->line_number);
  manage_page_selection(This, pagel);
 }
 else if (line_num > pagel->text_line_end->line_number)
 {
  do
  {
   pagel = pagel->NextPage;
  } 
  while(line_num > pagel->text_line_end->line_number);
  manage_page_selection(This, pagel);
 }
 else
 {
  manage_page_selection(This, pagel);
 }
}

static c_Page *get_Page_by_TextLine(This, tl)
c_Calibration *This;
text_line_t *tl;
{
 c_Page *page;
 int	ln;

 page = This->PageArea->SelectedPage;
 ln = tl->line_number;
 
 if (ln >= page->text_line_start->line_number &&
     ln <= page->text_line_end->line_number)
  return page;
 
 if (ln < page->text_line_start->line_number)
 {				/* search in backward's page */
  do
  {
   page = page->PrevPage;
  }
  while(ln < page->text_line_start->line_number);
 }
 else
 {				/* search in foreward's page */
  do
  {
   page = page->NextPage;
  }
  while(ln > page->text_line_end->line_number);
 }
 
 return page;
}

/* ----------------------------------------------------------------- ** 
** get_TextLine_by_Y : return the textline pointed by y coord.       ** 
** if the pos (x, y) is not within text interleave then return FALSE ** 
** ----------------------------------------------------------------- */
static boolean get_TextLine_by_Y(This, page, y, x, rtl)
c_Calibration *This;
c_Page	*page;
coord_t y;
coord_t x;
text_line_t **rtl;
{
 coord_t y1, y2, yt, tolerance;
 text_line_t *tl, *otl;
 int ln;
 boolean found = FALSE;

 tl = otl = page->text_line_start;
 ln = tl->line_number;
 y2 = tl->y - (tl->height >> 1);
 yt = tl->y + tl->height;
 
 if (y < y2)			/* in case y is above the first textline */
 {
  if (rtl) *rtl = tl;
  return FALSE;
 }

 while(ln < page->text_line_end->line_number)
 {
  while(tl->line_number == ln)
   tl = tl->next;

  y1 = y2;
  y2 = yt + ((tl->y - yt) >> 1);
  
  if (y >= y1 && y <= y2)
  {
   if (rtl) *rtl = otl;
   found = TRUE;
   break;
  }
  yt = tl->y + tl->height; 
  ln = tl->line_number;
  otl = tl;
 }
 
 if (!found)
 {
  y1 = y2;
  y2 = yt + (tl->height >> 1);
  if (y >= y1 && y <= y2)
   found = TRUE;
  else
  {
   ln = tl->line_number;
   while(tl->next && tl->next->line_number == ln)
    tl = tl->next;
   if (rtl) *rtl = tl;
   return FALSE;		/* in case y is under the last textline */
  }
 }
 
 if (found)
 {
  tolerance = (otl->height >> 1);
  tl = otl;
  while(otl && otl->line_number == ln)
  {
   if (F(page->Redraw).is_within_textline(otl, x, tolerance))
   {
    if (rtl) *rtl = otl;
    return TRUE;
   }
   otl = otl->next;
  }
  /* if not in text line, find nearest text line */
  if (rtl)
  {
   while(tl && tl->line_number == ln)
   {
    *rtl = tl;
    if (x < tl->x_min) break;
    tl = tl->next;
   }
  }
 }
 else 
  if (rtl) *rtl = NULL;

 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** get_position_on_Textline : found suitable position on text_line   ** 
**                            by x coordinate.                       ** 
** ----------------------------------------------------------------- */
static void get_position_on_Textline(This, page, rtl, x, item, pos, direction)
c_Calibration *This;
c_Page *page;
text_line_t **rtl;
coord_t x;
xa_item_t **item;
int *pos;
boolean direction;
{
 boolean	result;
 text_line_t	*tl, *stl;
 int		ln, xm;
 
 if (!direction && (*item)->type == XA_LINEFEED && *pos == 1)
 {
  tl = *rtl; ln = tl->line_number;
  while(tl->line_number == ln && tl->next)
   tl = tl->next;
  *rtl = tl;
  *item = tl->text_spot->item;
  *pos = 0;
 }

 stl = tl = *rtl;
 ln = tl->line_number;
 while(!(result = F(page->Redraw).get_item_by_x(tl, x, item, pos)))
 {
  if (tl->next && tl->next->line_number == ln)
   tl = tl->next;
  else break;
 }

 if (!result)
 {
  tl = stl;
  if (x <= tl->text_spot->x)
  {
   *item = tl->text_spot->item;
   if (!direction)
   {
    text_spot_t *ts = stl->text_spot;
    
    while(ts->next) ts = ts->next;
    if (ts->item->type == XA_TEXT)
     *pos = ts->item->text.text_len;
    else
     *pos = 1; 
   }
   else
    *pos = 0;
  }
  else
  {
   while(tl->next && tl->next->line_number == ln)
   {
    xm = tl->x_max;
    stl = tl;
    tl = tl->next;
    if (x <= tl->text_spot->x)
    {
     if (x <= xm + (tl->x_min - xm) / 2)
     {
      tl = stl;
      break;
     }
     else
     {
      *item = tl->text_spot->item;
      *pos = 0;
      result = TRUE;
      break;
     }
    }
   }
   if (!result)
   {
    text_spot_t *ts;
    
    ts = tl->text_spot;
    while(ts->next && ts->next->item->type != XA_LINEFEED)
     ts = ts->next;
     
    *item = ts->item;
    if ((*item)->type == XA_TEXT)
     *pos = (*item)->text.text_len;
    else if (direction && (*item)->type == XA_LINEFEED)
     *pos = 0;
    else
     *pos = 1;
   }
  }
 }
 else if ((*item)->type == XA_PAGEBREAK) *pos = 0;

 *rtl = tl;
}

/* ----------------------------------------------------------------- ** 
** checkSelectionOrder : correct mistake in selection position.      ** 
** ----------------------------------------------------------------- */
static void checkSelectionOrder(text, tl_start, tl_end)
c_Text *text;
text_line_t *tl_start;
text_line_t *tl_end;
{
 xa_item_t	*item, *start, *end;

 if (tl_start->line_number < tl_end->line_number)
  return;

 if (tl_start->line_number > tl_end->line_number)
 {
  SWAP(xa_item_t *, text->select_start, text->select_end);
  SWAP(int, text->select_start_pos, text->select_end_pos);
  text->start_end ^= TRUE;
 }
 else
 {
  start = text->select_start;
  end = text->select_end;
  if (start == end)
  {
   if (text->select_start_pos > text->select_end_pos)
   {
    SWAP(xa_item_t *, text->select_start, text->select_end);
    SWAP(int, text->select_start_pos, text->select_end_pos);
    text->start_end ^= TRUE;
   }
   return;
  }
  
  while(tl_start->prev && tl_start->prev->line_number == tl_start->line_number)
   tl_start = tl_start->prev;
  
  item = tl_start->text_spot->item;
  while(item != start)
  {
   if (item == end)
   {
    SWAP(xa_item_t *, text->select_start, text->select_end);
    SWAP(int, text->select_start_pos, text->select_end_pos);
    text->start_end ^= TRUE;
    return;
   }
   item = item->any.next;
  }
 }
}

/* ----------------------------------------------------------------- ** 
** checkDiffTextLine : compare the original and new textline.        ** 
** ----------------------------------------------------------------- */
static boolean checkDiffTextLine(This, tl_start, tl)
c_Calibration *This;
text_line_t *tl_start;
text_line_t *tl;
{
 c_Text	*text =  This->BaseStd->text_base;
 text_spot_t *ts, *ts_start;

 while(tl && tl_start)
 {
  if (!memcmp((void *)&(tl->line_number), (void *)&(tl_start->line_number),
	      offsetof(text_line_t, new_paragraph) + sizeof(boolean)
	      - offsetof(text_line_t, line_number)))
  {
   ts = tl->text_spot;
   ts_start = tl_start->text_spot;
   while(ts && ts_start)
   {
    if (memcmp((void *)&(ts->x), (void *)&(ts_start->x), 
	       sizeof(text_spot_t) - offsetof(text_spot_t, x)) ||
	F(text).checkDiffTextItem(text, ts->item, ts_start->item, NULL))
     return TRUE;

    ts = ts->next;
    ts_start = ts_start->next;
   }
   if (ts || ts_start) 
    return  TRUE;
  }
  else
   return TRUE;

  tl = tl->next;
  tl_start = tl_start->next;
 }
 if (tl) return TRUE;		/* tl_start == NULL */
 return FALSE;
}


static void make_cursor_visible(This)
c_Calibration *This;
{
 c_Page *page;
 c_Text	*text =  This->BaseStd->text_base;
 xa_item_t	*item;
 int		pos;
 text_line_t	*tl_start;
 coord_t	scale;
 int	x, y, width, height;	
 
 page = This->PageArea->SelectedPage;
 scale = This->PageArea->sScale;

 if (text->start_end)
 {
  item = text->select_start;
  pos = text->select_start_pos;
  tl_start = This->tl_select_start;
 }
 else
 {
  item = text->select_end;
  pos = text->select_end_pos;
  tl_start = This->tl_select_end;
 }

 x = F(page->Redraw).get_x_by_item(tl_start, NULL, item, pos) / scale;
 y = tl_start->y / scale;
 width = F(text->style->metrics).textWidth
  (text->style->metrics, " ", 1) / scale;
 height = MAX(tl_start->height, FONT_HEIGHT(item->any.style)) / scale;

 F(This->PageArea).MakeAreaVisible(This->PageArea, page, x, y, width, height);

#if 0
 XSetForeground(page->X_info.display, page->X_info.gc_inside, 0xFF00);
 XDrawRectangle(page->X_info.display, page->X_info.window,
		page->X_info.gc_inside, x, y, width, height);
 XSetForeground(page->X_info.display, page->X_info.gc_inside, 0x0000);
#endif
}


