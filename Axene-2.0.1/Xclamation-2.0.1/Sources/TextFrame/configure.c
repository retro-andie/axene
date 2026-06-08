/*
** configure.c for Xclamation in TextFrame/
** TextFrame method for TextFrame class
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
** Started on  Mon Apr  3 17:37:20 1995 Stéphane Boisson
** Last update Sat Oct 18 18:20:24 1997 Emmanuel Paris
*/

/*#define NTRACE*/
/*#define NHISTORY*/


#include "TextFrameP.h"

/* ----------------------------------------------------------------- ** 
** textframeConfigure - The frame shape changed                      ** 
** ----------------------------------------------------------------- */
void tf_Configure(this, frame, poly)
c_TextFrame *this;
c_Cadre *frame;
Polygone *poly;
{
 Xc_HISTORY(("display_configure frame: %s", this->frame->name));
 
 this->frame = frame;
 if(this->text == NULL) return;
 
 if(frame->sAngle != this->angle)
 {
  /*--- Rotation ---*/

  Xc_TRACE(("Update angle"));

  if(this->drawn_flag != FALSE) tf_releaseResources(this);
  this->angle = frame->sAngle;
 }
 else
 {
  /*--- Frame configuration changed ---*/
  textframe_char_line_t line;
  c_ScanLine *sline;
  sl_Segment *seg;
  text_seq_t *seq = NULL;
  text_pos_t svg_pos;
  c_Text *text;
  int n;
  boolean line_start;

  Xc_TRACE(("Frame configure: %s", frame->name));

  if(this->previous != NULL)
  {
   c_TextFrame *previous;

   Xc_TRACE(("this->frame: $%lx  previous->frame: $%lx",
	     (long)this->frame, (long)this->previous->frame));
	  
   Xc_TRACE(("Not first in chain"));
   previous = this->previous;
   Xc_ASSERT(previous->text == this->text);
   if ((previous->end_pos != this->start_pos) ||
       (previous->end_paragraph_flag != this->start_paragraph_flag))
   {
    Xc_TRACE(("retro-configure"));
    Xc_TRACE(("(this) start=%ld flag=%s", this->start_pos,
	      this->start_paragraph_flag? "True":"False"));
    Xc_TRACE(("(previous) start=%ld flag=%s", previous->end_pos,
	      previous->end_paragraph_flag? "True":"False"));

    tf_Configure(previous, previous->frame, previous->frame->Forme);
    Xc_TRACE(("retro-configure OK"));
    return;
   }
  }
      
  this->sequence = this->text->sequence + 1;

  if(this->lines != NULL) Xc_free(this->lines);
  this->lines = NULL;
  this->line_count = 0;
  text = this->text;

  line.paragraph_flag = this->start_paragraph_flag;

  Xc_TRACE(("### Start: pos %ld par=%s", this->start_pos,
	    line.paragraph_flag? "TRUE" : "FALSE"));
  F(text).getFirstSeq(text, this->start_pos, &line.style, &line.ruler);
  Xc_ASSERT(line.style != NULL);
  Xc_ASSERT(line.ruler != NULL);

  if(!CalibrationStart(&line, frame->sHeight >> (COORD_SCALEBITS - 1)))
   return;

  sline = NEW(c_ScanLine)(frame, line.y_pos, SCANLINE_ORDONNEE);
  if(sline == NULL)
   n = -1;
  else
  {
   Xc_TRACE(("initial get_next_scanline"));
   n = F(sline).get_next_scanline(sline, &seg, line.interline);
  }

  line_start = FALSE;
  while(n >= 0)
  {
   text_pos_t end_pos;

   Xc_TRACE(("pos=%g par=%s", SCALE_TO_POINTS(line.y_pos),
	     line.paragraph_flag? "TRUE":"FALSE"));

   svg_pos = text->seq_pos;			 /* save old position */
   seq = F(text).getNextSeq(text, 0);

   /*--- Reference ---*/
   if(seq->ref != NULL)
    switch(seq->ref->type)
    {
     /*--- Text ruler ---*/
    case XcT_TEXT_RULER:
     Xc_TRACE(("TextRuler(`%s')", seq->ref->data.ruler->name));
     Xc_ASSERT(line.paragraph_flag != FALSE);
     line.ruler = seq->ref->data.ruler;
     break;
		
     /*--- Text style ---*/
    case XcT_TEXT_STYLE:
     Xc_TRACE(("TextStyle(`%s')", seq->ref->data.style->name));
     line.style = seq->ref->data.style;
     if (line_start)
     {
      CalibrationReactualize(&line, 0);
     }
     else
     {
      if(line.ruler->inter_mode == FALSE)
       if(line.style->point_size > line.interline)
       {
	coord_t delta;
	sl_Segment *seg2;
	int n2;

	delta = line.style->point_size - line.interline;
	Xc_TRACE(("grow scan line"));
	n2 = F(sline).get_next_scanline(sline, &seg2, delta);
	line.interline += delta;
	if((n2 > 0) && (n > 0))
	 F(sline).intersect_segment(&seg, &n, seg2, n2);
	else if(n > 0)
	{
	 n = n2;
	 Xc_TRACE(("Seg: %p", seg));
	 Xc_free(seg);
	 seg = NULL;
	}
	if(n < 0) goto frame_end;
       }
     }
    default:
     break;
    }
	  
   /*--- Text ---*/
   if(seq->len > 0)
   {
    if (line_start)
    {
     n = F(sline).get_next_scanline(sline, &seg, line.interline);
     line_start = FALSE;
    }
    if (n < 0)
    {
     text->seq_pos = svg_pos;	/* restore old position */
     break;
    }
    if (CalibrationShift(&line, seq) == FALSE)
     break;
   }
   else
   {
    if (seq->special == XcT_END_OF_TEXT)
    {
     n = F(sline).get_next_scanline(sline, &seg, line.interline);
     line_start = FALSE;
     if (n < 0)
      break;
    }
   }

   /*--- Calibration ---*/
   end_pos = (seq->special == XcT_NOTHING)? 0 : (text->seq_pos - 1);
   if(seq->special == XcT_END_OF_PARAGRAPH) end_pos = -end_pos;

   if(line.pos != 0)
   {
    /*--- Buffer not empty ---*/
    Xc_TRACE(("begin loop (line.y_pos=%g)",
	      SCALE_TO_POINTS(line.y_pos)));
    do 
    {
     if(n == 0)
     {
      Xc_TRACE(("empty scanline"));
      line.y_pos += ((line.ruler->inter_mode != FALSE)? 0 : 
		     line.ruler->inter_line) + line.interline;
     }
     else
     {
      /* in case of reduce may change the line size !!! (including previous)*/
      if(CalibrationReduce(&line, n, seg, end_pos) == FALSE)
       break;
      Xc_free(seg);
      seg = NULL;
     }
		
     if (!line.ruler->inter_mode && line.ruler->inter_line)
      if(!F(sline).skip_scanline(sline, line.ruler->inter_line))
      {
       n = -1;
       if (seg != NULL) { Xc_free(seg); seg = NULL; }
       break;
      }

     if(line.reduction_flag != FALSE)
     {
      if(n != 0) CalibrationReactualize(&line, 0);
      if(line.pos == 0)
      {
       Xc_TRACE(("Empty buffer"));
       n = 0;
       if (seg != NULL) { Xc_free(seg); seg = NULL; }
       break;
      }
     }
     else if(line.line_count == 0)
     {
      Xc_TRACE(("Searching begining y pos"));
      line.y_pos += COORD_HALF_ONE - line.interline; 
      DELETE(c_ScanLine)(sline);
      sline = NEW(c_ScanLine)(frame, line.y_pos,
			      SCANLINE_ORDONNEE);
      n = 0;
      if (seg != NULL) { Xc_free(seg); seg = NULL; }
      if(sline == NULL) goto frame_end;
     }

     if (n != 0 && line.interline == 0 && line.line_count == 0)
     {		/* to avoid an infinite loop */
      n = -1;
      if (seg != NULL) { Xc_free(seg); seg = NULL; }
      break;
     }
     n = F(sline).get_next_scanline(sline, &seg, line.interline);
     CalibrationReactualize(&line, 0);
    } while(n >= 0);
    Xc_TRACE(("end loop (line.y_pos=%g)",
	      SCALE_TO_POINTS(line.y_pos)));
   }
   else if(seq->special != XcT_NOTHING)
   {
    coord_t h;

    /*--- Buffer empty and special flag ---*/
    Xc_TRACE(("seq->special != XcT_NOTHING"));
    if(n > 0)
    {
     Xc_TRACE(("Seg: %p", seg));
     Xc_free(seg);
     seg = NULL;
     line.y_pos += line.interline;
     h = 0;
    }
    else
    {
     h = line.interline;
    }
    h += line.ruler->inter_mode? 0 : line.ruler->inter_line;
    line.y_pos += h;
    n = F(sline).skip_scanline(sline, h)? 0 : -1;
   }

   if((n < 0)
      || (seq->special == XcT_END_OF_PAGE && line.line_count != 0)
      || (seq->special == XcT_END_OF_TEXT))
   {
    Xc_TRACE(("end of text in frame"));
    break;
   }

   if(seq->special == XcT_END_OF_PARAGRAPH)
   {
    Xc_TRACE(("End of paragraph"));
    Xc_ASSERT(n == 0);

    line.paragraph_flag = TRUE;
    if(!F(sline).skip_scanline(sline, line.ruler->inter_paragraph))
     break;
    line.y_pos += line.ruler->inter_paragraph;
    /*TODO: ask for next ruler */
    n = 0;
    if (seg != NULL) { Xc_free(seg); seg = NULL; }
    line_start = TRUE;
   }
   else if(seq->special == XcT_END_OF_LINE)
   {
    Xc_TRACE(("End of line"));
    n = F(sline).get_next_scanline(sline, &seg, line.interline);
   }
  }
      
  /*--- End of frame ---*/
 frame_end:
  Xc_TRACE(("end of frame"));

  if(n > 0) 
  {
   Xc_TRACE(("Seg: %p", seg));
   Xc_free(seg);
   seg = NULL;
  }
  if(sline != NULL) DELETE(c_ScanLine)(sline);

  this->end_pos = line.pos? line.buffer->pos : this->text->seq_pos;
  this->end_paragraph_flag = line.paragraph_flag;

  CalibrationEnd(&line, &this->lines, &this->line_count);

  Xc_TRACE(("### end: pos=%ld par=%s", this->end_pos,
	    this->end_paragraph_flag? "TRUE" : "FALSE"));

  if(this->next != NULL)
  {
   c_TextFrame *next;

   next = this->next;
   if((next->start_pos != this->end_pos) ||
      (next->start_paragraph_flag != this->end_paragraph_flag))
   {
    Xc_TRACE(("configure propagation"));
    next->start_pos = this->end_pos;
    next->start_paragraph_flag = this->end_paragraph_flag;
    if(next->frame != NULL)
     tf_Configure(next, next->frame, next->frame->Forme);
   }
  }
 }
 Xc_HISTORY(("display_configure frame: %s done", this->frame->name));
}
