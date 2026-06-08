/*
** Set_TextRuler.c for XAllWrite in Page/
** Set Text Ruler.
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
** Started on  Mon Sep  8 20:00:02 1997 Emmanuel Paris
** Last update Sat Jul 31 16:15:28 1999 Emmanuel Paris
*/

#include "Page.h"

#define INDENT_GAP	SCALE_FROM_CENTIMETERS(1.25)
#define INTERLINE_GAP	SCALE_FROM_CENTIMETERS(0.05)
#define INTERPARAG_GAP	SCALE_FROM_CENTIMETERS(0.05)
#define MARGIN_MINSPACE	SCALE_FROM_CENTIMETERS(0.5)

void set_ruler_left ___PROTO((c_Page *This));
void set_ruler_center ___PROTO((c_Page *This));
void set_ruler_right ___PROTO((c_Page *This));
void set_ruler_justify ___PROTO((c_Page *This));
static c_TextRuler *set_ruler_justification_callback();

void set_ruler_inc_indent ___PROTO((c_Page *This));
void set_ruler_dec_indent ___PROTO((c_Page *This));
static c_TextRuler *set_ruler_indent_callback();

void set_ruler_inc_interline ___PROTO((c_Page *This));
void set_ruler_dec_interline ___PROTO((c_Page *This));
static c_TextRuler *set_ruler_interline_callback();

void set_ruler_inc_interparag ___PROTO((c_Page *This));
void set_ruler_dec_interparag ___PROTO((c_Page *This));
static c_TextRuler *set_ruler_interparag_callback();

void set_page_margin ___PROTO((c_Page *This, coord_t size, int margin));

void set_ruler_margin ___PROTO((c_Page *This, coord_t size, int margin));
static c_TextRuler *set_ruler_left_margin_callback();
static c_TextRuler *set_ruler_left2_margin_callback();
static c_TextRuler *set_ruler_right_margin_callback();
static c_TextRuler *set_ruler_indent_margin_callback();

void set_ruler_tab ___PROTO((c_Page *This, int index, coord_t pos,
				 tab_just_t tabtype, char filler, int func));
static c_TextRuler *set_ruler_tab_callback();

enum
{
 TR_JLEFT, TR_JCENTER, TR_JRIGHT, TR_JUSTIFY
};

void set_ruler_left(This)
c_Page *This;
{
 c_Text	*Text = This->BaseStd->text_base;
 
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(Text).orTextRuler(Text, set_ruler_justification_callback, 
		      This, (void *)TR_JLEFT);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

void set_ruler_center(This)
c_Page *This;
{
 c_Text	*Text = This->BaseStd->text_base;
 
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(Text).orTextRuler(Text, set_ruler_justification_callback, 
		     This, (void *)TR_JCENTER);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

void set_ruler_right(This)
c_Page *This;
{
 c_Text	*Text = This->BaseStd->text_base;
 
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(Text).orTextRuler(Text, set_ruler_justification_callback,
		      This, (void *)TR_JRIGHT);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

void set_ruler_justify(This)
c_Page *This;
{
 c_Text	*Text = This->BaseStd->text_base;
 
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(Text).orTextRuler(Text, set_ruler_justification_callback,
		     This, (void *)TR_JUSTIFY);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

static c_TextRuler *set_ruler_justification_callback(page, truler, justif)
c_Page	    *page;
c_TextRuler *truler;
int justif;
{
 c_TextRuler   *new_truler;
 
 switch(justif)
 {
 case TR_JLEFT:
  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_JUSTIFICATION, XcTR_LEFT,
			     XcTR_INHERIT, truler, 
			     (unsigned int)XcTR_H_JUSTIFICATION,
			     XcTR_END);
  break;
 case TR_JCENTER:
  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_JUSTIFICATION, XcTR_CENTER,
			     XcTR_INHERIT, truler, 
			     (unsigned int)XcTR_H_JUSTIFICATION,
			     XcTR_END);
  break;
 case TR_JRIGHT:
  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_JUSTIFICATION, XcTR_RIGHT,
			     XcTR_INHERIT, truler, 
			     (unsigned int)XcTR_H_JUSTIFICATION,
			     XcTR_END);
  break;
 case TR_JUSTIFY:
 default:
  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_JUSTIFICATION, XcTR_JUSTIFIED,
			     XcTR_INHERIT, truler, 
			     (unsigned int)XcTR_H_JUSTIFICATION,
			     XcTR_END);
  break;
 }

 Xc_TRACE(("new ruler name: %s", new_truler->name));

 return new_truler;
}

void set_ruler_inc_indent(This)
c_Page *This;
{
 c_Text	*Text = This->BaseStd->text_base;
 
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(Text).orTextRuler(Text, set_ruler_indent_callback,
		     This, (void *)TRUE);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

void set_ruler_dec_indent(This)
c_Page *This;
{
 c_Text	*Text = This->BaseStd->text_base;
 
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(Text).orTextRuler(Text, set_ruler_indent_callback,
		     This, (void *)FALSE);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

static c_TextRuler *set_ruler_indent_callback(page, truler, inc)
c_Page	    *page;
c_TextRuler *truler;
boolean inc;
{
 c_TextRuler   *new_truler;
 coord_t       minx, delta, maxx;
 
 delta = truler->paragraph_indent;
 if (delta >= COORD_ZERO)
 {
  minx = truler->left_margin;
  
  if (minx < COORD_ZERO) minx = COORD_ZERO;
  minx = ((minx / INDENT_GAP) + (inc ? 1 : -1) ) * INDENT_GAP;
  if (minx < COORD_ZERO) minx = COORD_ZERO;
 }
 else
 {
  minx = truler->left_margin + delta;
  
  if (minx < COORD_ZERO) minx = COORD_ZERO;
  minx = ((minx / INDENT_GAP) + (inc ? 1 : -1) ) * INDENT_GAP;
  if (minx < COORD_ZERO) minx = COORD_ZERO;
  
  minx -= delta;
 }

 maxx = page->sWidth - page->right_margin - page->left_margin - 
  truler->right_margin - MARGIN_MINSPACE;

 if (minx > maxx)
  minx = maxx;
 
 if (minx + delta > maxx)
  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_LEFT_MARGIN, minx,
			     XcTR_INDENT, COORD_ZERO, XcTR_INHERIT, truler,
			     (unsigned int)(XcTR_H_LEFT_MARGIN|XcTR_H_INDENT),
			     XcTR_END);
 else
  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_LEFT_MARGIN, minx,
			     XcTR_INHERIT, truler, 
			     (unsigned int)XcTR_H_LEFT_MARGIN,
			     XcTR_END);
 return new_truler;
}

void set_ruler_inc_interline(This)
c_Page *This;
{
 c_Text	*Text = This->BaseStd->text_base;
 
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(Text).orTextRuler(Text, set_ruler_interline_callback,
		     This, (void *)TRUE);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

void set_ruler_dec_interline(This)
c_Page *This;
{
 c_Text	*Text = This->BaseStd->text_base;
 
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(Text).orTextRuler(Text, set_ruler_interline_callback,
		     This, (void *)FALSE);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

static c_TextRuler *set_ruler_interline_callback(page, truler, inc)
c_Page	    *page;
c_TextRuler *truler;
boolean inc;
{
 c_TextRuler   *new_truler;
 coord_t interline;

 if (truler->inter_mode)
 {			/* absolute interline */
  interline = truler->abs_inter_line + ( inc ? INTERLINE_GAP : -INTERLINE_GAP);
  if (interline < COORD_ZERO) interline = COORD_ZERO;

  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_ABS_INTERLINE, interline,
			     XcTR_INHERIT, truler, 
			     (unsigned int) XcTR_H_ABS_INTERLINE,
			     XcTR_END);  
 }
 else
 {			/* relative interline */
  interline = truler->rel_inter_line + ( inc ? INTERLINE_GAP : -INTERLINE_GAP);
  if (interline < COORD_ZERO) interline = COORD_ZERO;

  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_REL_INTERLINE, interline,
			     XcTR_INHERIT, truler, 
			     (unsigned int) XcTR_H_REL_INTERLINE,
			     XcTR_END);  
 }

 return new_truler;
}
void set_ruler_inc_interparag(This)
c_Page *This;
{
  c_Text	*Text = This->BaseStd->text_base;
 
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(Text).orTextRuler(Text, set_ruler_interparag_callback,
		     This, (void *)TRUE);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

void set_ruler_dec_interparag(This)
c_Page *This;
{
 c_Text	*Text = This->BaseStd->text_base;
 
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(Text).orTextRuler(Text, set_ruler_interparag_callback,
		     This, (void *)FALSE);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

static c_TextRuler *set_ruler_interparag_callback(page, truler, inc)
c_Page	    *page;
c_TextRuler *truler;
boolean inc;
{
 c_TextRuler   *new_truler;
 coord_t interparag;

 interparag = truler->inter_paragraph + 
  ( inc ? INTERPARAG_GAP : -INTERPARAG_GAP);
 if (interparag < COORD_ZERO) interparag = COORD_ZERO;
 
 new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			    XcTR_HIDE_ON, XcTR_INTER_PARAGRAPH, interparag,
			    XcTR_INHERIT, truler, 
			    (unsigned int) XcTR_H_INTER_PARAGRAPH,
			    XcTR_END);  

 return new_truler;
}


void set_page_margin(This, size, margin)
c_Page *This;
coord_t size;
int margin;
{
 switch(margin)
 {
 case XP_MARGIN_TOP:
  This->top_margin = size;
  break;
 case XP_MARGIN_BOTTOM:
  This->bottom_margin = size;
  break;
 case XP_MARGIN_LEFT:
  This->left_margin = size;
  break;
 case XP_MARGIN_RIGHT:
  This->right_margin = size;
  break;
 }
 
 F(This->Redraw).undisplay_selection(This->Redraw, NULL);
 F(This->Calibration).Init(This->Calibration, This);
 while(This)
 {
  if (This->mapped) F(This).RedrawAll(This);
  This = This->NextPage;
 }
}

void set_ruler_margin(This, size, margin)
c_Page *This;
coord_t size;
int margin;
{
 c_Text	*Text = This->BaseStd->text_base;
 
 F(This->Redraw).display_selection(This->Redraw, NULL);

 switch(margin)
 {
 case XP_MARGIN_LEFT:
  F(Text).orTextRuler(Text, set_ruler_left_margin_callback, 
		      This, (void *)size);
  break;
 case XP_MARGIN_LEFT2:
  F(Text).orTextRuler(Text, set_ruler_left2_margin_callback, 
		      This, (void *)size);
  break;
 case XP_MARGIN_RIGHT:
  F(Text).orTextRuler(Text, set_ruler_right_margin_callback, 
		      This, (void *)size);
  break;
 case XP_MARGIN_INDENT:
  F(Text).orTextRuler(Text, set_ruler_indent_margin_callback, 
		      This, (void *)size);
  break;
 }

 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

static c_TextRuler *set_ruler_left_margin_callback(page, truler, size)
c_Page	    *page;
c_TextRuler *truler;
coord_t size;
{
 c_TextRuler   *new_truler;
 
 new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			    XcTR_HIDE_ON, XcTR_LEFT_MARGIN, size,
			    XcTR_INHERIT, truler, 
			    (unsigned int)XcTR_H_LEFT_MARGIN,
			    XcTR_END);
 return new_truler;
}

static c_TextRuler *set_ruler_left2_margin_callback(page, truler, size)
c_Page	    *page;
c_TextRuler *truler;
coord_t size;
{
 coord_t xi;
 c_TextRuler   *new_truler;
 
 xi = truler->paragraph_indent - (size - truler->left_margin);
 
 new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			    XcTR_HIDE_ON, XcTR_LEFT_MARGIN, size,
			    XcTR_INDENT, xi, XcTR_INHERIT, truler, 
			    (unsigned int)(XcTR_H_LEFT_MARGIN | XcTR_H_INDENT),
			    XcTR_END);
 return new_truler;
}

static c_TextRuler *set_ruler_right_margin_callback(page, truler, size)
c_Page	    *page;
c_TextRuler *truler;
coord_t size;
{
 c_TextRuler   *new_truler;

 new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			    XcTR_HIDE_ON, XcTR_RIGHT_MARGIN, size,
			    XcTR_INHERIT, truler, 
			    (unsigned int)XcTR_H_RIGHT_MARGIN,
			    XcTR_END);
 return new_truler;
}

static c_TextRuler *set_ruler_indent_margin_callback(page, truler, size)
c_Page	    *page;
c_TextRuler *truler;
coord_t size;
{
 c_TextRuler   *new_truler;
 
 new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			    XcTR_HIDE_ON, XcTR_INDENT, size,
			    XcTR_INHERIT, truler, (unsigned int)XcTR_H_INDENT,
			    XcTR_END);
 return new_truler;
}

typedef struct 
{
 int index;
 coord_t pos;
 tab_just_t ttype;
 char filler;
 int func;
} ruler_tab_t;

void set_ruler_tab(This, index, pos, tabtype, filler, func)
c_Page *This;
int index;
coord_t pos;
tab_just_t tabtype;
char filler;
int func;
{
 c_Text	*Text = This->BaseStd->text_base;
 ruler_tab_t rtab;

 F(This->Redraw).display_selection(This->Redraw, NULL);

 rtab.index = index;
 rtab.pos = pos;
 rtab.ttype = tabtype; 
 rtab.filler = filler;
 rtab.func = func;
 F(Text).orTextRuler(Text, set_ruler_tab_callback, 
		     This, (void *)&rtab);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

static c_TextRuler *set_ruler_tab_callback(page, truler, rtab)
c_Page	    *page;
c_TextRuler *truler;
ruler_tab_t *rtab;
{
 c_TextRuler   *new_truler;
 
 switch(rtab->func)
 {
 case XP_ADD_TAB:
  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_ADD_TAB, rtab->pos, 
			     rtab->ttype, (int)' ', XcTR_INHERIT, truler, 
			     (unsigned int)XcTR_H_TAB, XcTR_END);
  break;
 case XP_DEL_TAB:
  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_DEL_TAB, rtab->index,
			     XcTR_INHERIT, truler, (unsigned int)XcTR_H_TAB,
			     XcTR_END);
  break;
 case XP_MOVE_TAB:
  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_MOVE_TAB, rtab->index, 
			     rtab->pos, XcTR_INHERIT, truler, 
			     (unsigned int)XcTR_H_TAB,
			     XcTR_END);
  break;
 case XP_CHANGE_TAB_TYPE:
  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_CHANGE_TAB_TYPE, rtab->index, 
			     rtab->ttype, XcTR_INHERIT, truler, 
			     (unsigned int)XcTR_H_TAB,
			     XcTR_END);
  break;
 case XP_CHANGE_TAB_FILLER:
 default:
  new_truler = F(truler).get(truler, page->BaseStd,  XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_CHANGE_TAB_FILLER,
			     rtab->index, (int)rtab->filler, XcTR_INHERIT, 
			     truler, (unsigned int)XcTR_H_TAB, XcTR_END);
  break;
 }
 return new_truler;
}
