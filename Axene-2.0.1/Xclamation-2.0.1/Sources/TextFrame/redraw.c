/*
** redraw.c for Xclamation in TextFrame/
** Redraw methods
**
** Copyright (C) 1994-2000 Axene.
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
** Started on  Mon Apr  3 17:27:52 1995 Stéphane Boisson
** Last update Fri Dec 31 16:10:21 1999 Emmanuel Paris
*/

#define NTRACE
#define NHISTORY

#include "TextFrameP.h"
#include "FontManager.h"

extern c_FontManager	*GlobFntMng;

/* ----------------------------------------------------------------- ** 
** textframeRedraw - Redraw text in frame                            ** 
** ----------------------------------------------------------------- */
void tf_Redraw(this, frame, poly)
c_TextFrame *this;
c_Cadre *frame;
Polygone *poly;
{
 unsigned int line_count;
 textframe_line_t *line;
 real scale, Cos, Sin;
 GC gc_mask, gc_paint;
 c_RasterFont *font; 
 c_TextRuler *ruler;
 c_TextStyle *style;
 c_CharMetrics *cm;
 Display *display;
 text_seq_t *seq;
 text_pos_t p;
 c_Text *text;

 Xc_HISTORY(("display_redraw frame: %s", this->frame->name));

 this->frame = frame;
 if(this->text == NULL) return;
 if (!frame->X_info_calc) goto end_redraw;/* frame not in a viewable page */

 display = frame->X_info.display;
 if((line_count = this->line_count) == 0)
 {
  Xc_TRACE(("Nothing to draw (line_count == 0)"));
  goto end_redraw;
 }
 text = this->text;
 line = this->lines;
 Xc_ASSERT(this->lines != NULL);

 scale = SCALE_TO_COEF(this->factor);
 Xc_TRACE(("scale %g", scale));
 Cos = cos(SCALE_TO_RADIANS(this->angle));
 Sin = sin(SCALE_TO_RADIANS(this->angle));
  
 p = this->start_pos;
 F(text).getFirstSeq(text, p, &style, &ruler);
 cm = F(style->metrics).transform(style->metrics, this->factor,
				  XcTS_GET_WIDTH(style), this->angle);
 if(cm == NULL) goto end_redraw;
 if(!F(cm).lock(cm, this))
 {
  F(cm).purgeImpure(style->metrics, this->factor,
		    XcTS_GET_WIDTH(style), this->angle);
  goto end_redraw;
 }
    
 this->drawn_flag = TRUE;
 if((font = cm->raster_font) == NULL)
 {
  font = F(GlobFntMng).new_font(GlobFntMng, style->font, cm);
  if((cm->raster_font = font) == NULL)
  {
   F(cm).purgeImpure(style->metrics, this->factor,
		     XcTS_GET_WIDTH(style), this->angle);
   goto end_redraw;
  }
 }

 gc_paint = frame->X_info.gc_inside;
 gc_mask = (GC)0;

 Xc_TRACE(("set gc"));
 F(font).setGC(display, frame->X_info.window, &gc_mask, gc_paint);
 F(style->color).setGC(style->color, display, gc_paint);

 Xc_TRACE(("begin frame-loop"));
 while(line_count--)
 {
  int i;

  if((i = line->column_count) > 0)
  {
   textframe_hline_t *hline;
   real ry_X, ry_Y;

   ry_X = ((real)(frame->sOrig_X + frame->saOrig_X) +
	   (real)(line->y_pos * Sin));
   ry_Y = ((real)(frame->sOrig_Y + frame->saOrig_Y) +
	   (real)(line->y_pos * Cos));

   for(hline = (textframe_hline_t *) (line + 1); i--; hline++)
   {
    coord_t char_inc = COORD_ZERO, word_inc = COORD_ZERO;
    vector_t pos;
    real rx;

    Xc_TRACE(("begin first loop"));
    do
    {
     seq = F(text).getNextSeq(text, hline->end_pos);
     if(seq->ref != NULL)
     {
      if(seq->ref->type == XcT_TEXT_STYLE)
      {
       Xc_TRACE(("new style"));
       F(cm).purgeImpure(style->metrics,
			 this->factor,
			 XcTS_GET_WIDTH(style),
			 this->angle);
       style = seq->ref->data.style;
       F(style->color).setGC(style->color, display, gc_paint);
       cm = F(style->metrics).transform(style->metrics,
					this->factor,
					XcTS_GET_WIDTH(style),
					this->angle);
       while(42)
       {
	if((cm != NULL) && (font = cm->raster_font) == NULL)
	{
	 font = F(GlobFntMng).new_font(GlobFntMng, 
				       style->font, cm);
	 if(((cm->raster_font = font) != NULL) &&
	    F(cm).lock(cm, this)) break;
	}
	else
	 break;

	if(cm != NULL)
	 F(cm).purgeImpure(style->metrics,
			   this->factor,
			   XcTS_GET_WIDTH(style),
			   this->angle);
	XFreeGC(display, gc_mask);
	goto end_redraw;
       }
		    
       F(font).setGC(display, frame->X_info.window,
		     &gc_mask, gc_paint);
      }
      else if(seq->ref->type == XcT_TEXT_RULER)
      {
       Xc_TRACE(("new ruler"));
       ruler = seq->ref->data.ruler;
      }
     }
    } 
    while((seq->len == 0) && (seq->limit_reached == FALSE));
    Xc_TRACE(("end first loop"));

    rx = hline->x_min;

    switch(ruler->justification)
    {
     /*--- Left & right justification ---*/
    case XcTR_JUSTIFIED:
     {
      coord_t w;

      Xc_TRACE(("justified"));
      if((hline->gap == 0) || (hline->char_count == 0))
      {
       word_inc = char_inc = 0;
       rx += hline->gap >> 1;
       break;
      }

      w = hline->gap * SCALE_TO_COEF(ruler->word_distribution);
      if(hline->space_count == 0)
      {
       word_inc = 0;
#if 0
       rx += w >> 1;
       char_inc = (hline->gap - w)/(long)hline->char_count;
#else
       char_inc = hline->gap / (long)hline->char_count;
#endif
      }
      else
      {
       char_inc = (hline->gap - w) / (long)hline->char_count;
       word_inc = w / (long)hline->space_count;
      } 
      char_inc = ROUNDFLOAT(char_inc * scale);
      word_inc = ROUNDFLOAT(word_inc * scale);
      break;
     }

     /*--- Center justification ---*/
    case XcTR_CENTER:
     Xc_TRACE(("center"));
     rx += hline->gap >> 1;
     word_inc = char_inc = 0;
     break;

     /*--- Right justification ---*/
    case XcTR_RIGHT:
     rx += hline->gap;

     /*--- Left justification ---*/
    case XcTR_LEFT:
     word_inc = char_inc = 0;
     break;
    }

    pos.dx= ROUNDFLOAT((ry_X + (rx * Cos)) * scale) + COORD_HALF_ONE;
    pos.dy= ROUNDFLOAT((ry_Y - (rx * Sin)) * scale) + COORD_HALF_ONE;

    Xc_TRACE(("begin second loop"));
    while(42)
    {
     if(seq->len > 0)
     {
#ifndef NTRACE 
      int i;

      for(i = 0; i < seq->len; i++)
       if(seq->start[i] == 32)
	putc('_', stderr);
       else if(seq->start[i] < 32)
	fprintf(stderr, "<%x>", seq->start[i]);
       else
	putc(seq->start[i], stderr);
      putc('\n', stderr);
#endif

      F(font).drawString(font, display, frame->X_info.window,
			 gc_mask, gc_paint,
			 &pos, char_inc, word_inc,
			 seq->start, seq->len, style);
     }

     if(seq->limit_reached == TRUE) break;
     seq = F(text).getNextSeq(text, hline->end_pos);
     if((seq->ref != NULL) && (seq->ref->type == XcT_TEXT_STYLE))
     {
      F(cm).purgeImpure(style->metrics,
			this->factor,
			XcTS_GET_WIDTH(style),
			this->angle);
      style = seq->ref->data.style;
      F(style->color).setGC(style->color, display, gc_paint);
      cm = F(style->metrics).transform(style->metrics,
				       this->factor,
				       XcTS_GET_WIDTH(style),
				       this->angle);
      while(42)
      {
       if((cm != NULL) && (font = cm->raster_font) == NULL)
       {
	font = F(GlobFntMng).new_font(GlobFntMng, 
				      style->font, cm);
	if(((cm->raster_font = font) != NULL) &&
	   F(cm).lock(cm, this)) break;
       }
       else
	break;

       if(cm != NULL)
	F(cm).purgeImpure(style->metrics,
			  this->factor,
			  XcTS_GET_WIDTH(style),
			  this->angle);
       XFreeGC(display, gc_mask);
       goto end_redraw;
      }
		      
      F(font).setGC(display, frame->X_info.window,
		    &gc_mask, gc_paint);
     }
    }
    Xc_TRACE(("end second loop"));
	      
   }
   line = (textframe_line_t *) hline;
  }
  else
   line++;
 }
 F(cm).purgeImpure(style->metrics, this->factor,
		   XcTS_GET_WIDTH(style), this->angle);
 if(gc_mask != (GC)0) XFreeGC(display, gc_mask);
 XSync(display, 0);

 end_redraw:
 /* try to redraw frame on a viewable page end of the same sequence */
 {
  int sequence;
  c_TextFrame *tf, *tf2;
  
  sequence = this->text->sequence + 1;
  this->text->sequence = sequence;
  
  tf = this;
  while(tf->previous) tf = tf->previous;
  while(tf)
  {
   tf2 = tf;
   tf = tf->next;
   
   if (tf2 == this || !tf2->frame->X_info_calc)
    continue;

   if (tf2->sequence == sequence)	/* this test avoid recursion */
   {
    frame = tf2->frame;
    frame->data_to_object->redraw_cadre(frame->data_to_object->page,
					frame);
   }
  }
 }
 Xc_HISTORY(("display_redraw frame: %s done", this->frame->name));
}

