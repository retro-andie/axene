/*
** print.c for Xclamation in TextFrame/
** TextFrame methods for printing
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
** Started on  Mon Apr  3 17:34:43 1995 Stéphane Boisson
** Last update Fri Dec 31 16:10:48 1999 Emmanuel Paris
*/

#define NTRACE
#define NHISTORY

#include "TextFrameP.h"


/* ----------------------------------------------------------------- ** 
** textframePrint - Print method for text                            ** 
** ----------------------------------------------------------------- */
boolean tf_Print(this, post)
c_TextFrame *this;
c_PostScript *post;
{
 unsigned int line_count;
 textframe_line_t *line;
 c_TextRuler *ruler;
 c_TextStyle *style;
 text_seq_t *seq;
 text_pos_t p;
 c_Text *text;
 scale_t last_scale;
 coord_t last_x, last_y, last_char_inc, last_word_inc;
 boolean scale_flag;
 coord_t	cur_x;
 coord_t	sub_x;
 int		_i;
 int		_nbr_space;
 boolean	shad;

 Xc_HISTORY(("ps_print"));

 if((line_count = this->line_count) == 0)
 {
  F(this).display_configure(this, this->frame, NULL);
  if((line_count = this->line_count) == 0)
  {
   Xc_TRACE(("Nothing to print"));
   return TRUE;
  }
 }
 /*--- Set matrix ---*/
 F(post->matrix).scale(post->matrix, FALSE, SCALE_ONE, -SCALE_ONE);

 text = this->text;
 line = this->lines;

 p = this->start_pos;
 last_x = last_y = 0;
 last_char_inc = last_word_inc = COORD_ONE << 10;
/*last_scale = SCALE_ONE * 16;*/
 last_scale = SCALE_ONE;
 F(text).getFirstSeq(text, p, &style, &ruler);
 scale_flag = TRUE;

 /*--- init text rise ---*/
 if(!F(post).putLine2(post, NORMAL_TEXT, "$c Ts", 
		      0)) return FALSE;
 post->state2[NORMAL_TEXT].text_rise = 0;
 if(!F(post).putLine2(post, SHADOW_TEXT, "$c Ts", 
		      0)) return FALSE;
 post->state2[SHADOW_TEXT].text_rise = 0;

 while(line_count--)
 {
  int i;

  cur_x = 0;
  sub_x = 0;
  if((i = line->column_count) > 0)
  {
   textframe_hline_t *hline;
	  
   for(hline = (textframe_hline_t *) (line + 1); i--; hline++)
   {
    coord_t char_inc = COORD_ZERO, word_inc = COORD_ZERO, px;

    do 
    {
     seq = F(text).getNextSeq(text, hline->end_pos);
     
     if(seq->ref != NULL)
     {
      if(seq->ref->type == XcT_TEXT_STYLE)
       style = seq->ref->data.style;
      else if(seq->ref->type == XcT_TEXT_RULER)
       ruler = seq->ref->data.ruler;
     }
    } 
    while((seq->len == 0) && (seq->limit_reached == FALSE));

    px = hline->x_min;

    Xc_TRACE(("Ruler `%s'", ruler->name));

    switch(ruler->justification)
    {
     /*--- Left & right justification ---*/
    case XcTR_JUSTIFIED:
     {
      coord_t w;

      if((hline->gap == 0) || (hline->char_count == 0))
      {
       word_inc = char_inc = 0;
       px += hline->gap >> 1;
       break;
      }
      w = hline->gap * SCALE_TO_COEF(ruler->word_distribution);
      if(hline->space_count == 0)
      {
       word_inc = 0;
#if 0
       px += w >> 1;
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
      break;
     }

     /*--- Center justification ---*/
    case XcTR_CENTER:
     px += hline->gap >> 1;
     word_inc = char_inc = 0;
     break;

     /*--- Right justification ---*/
    case XcTR_RIGHT:
     px += hline->gap;

     /*--- Left justification ---*/
    case XcTR_LEFT:
     word_inc = char_inc = 0;
     break;
    }

    /*--- Move text current point ---*/
    if((last_x != px) || (last_y != line->y_pos))
    {
     Xc_TRACE(("new pos (%g, %g)",
	       SCALE_TO_POINTS(px),
	       SCALE_TO_POINTS(line->y_pos)));

     F(post).putLine2(post, SHADOW_TEXT, "$c $c Td",
		      px - last_x, last_y - line->y_pos);
     F(post).putLine2(post, NORMAL_TEXT, "$c $c Td",
		      px - last_x, last_y - line->y_pos);
     last_x = px, last_y = line->y_pos;
    }

    while(42)
    {
     if((seq->ref != NULL) && (seq->ref->type == XcT_TEXT_STYLE))
     {
      style = seq->ref->data.style;
     }
		  
     if(seq->len > 0)
     {
      /*--- Set horizontal scale ---*/
      if (style->attributes & XcTS_B_WIDTH)
      {
       last_scale = style->width.scale;
       scale_flag = TRUE;
      }
      else
      {
       last_scale = SCALE_ONE;
       scale_flag = FALSE;
      }

      /*--- Set char interspace ---*/
      if((scale_flag && (char_inc != 0)) ||
	 (char_inc != last_char_inc))
      {
       F(post).putLine2(post, SHADOW_TEXT, "$c Tc", (coord_t)
		       (char_inc /
			SCALE_TO_COEF(last_scale)));
       F(post).putLine2(post, NORMAL_TEXT, "$c Tc", (coord_t)
		       (char_inc /
			SCALE_TO_COEF(last_scale)));
       post->state.char_space = char_inc;
       last_char_inc = char_inc;
      }

      /*--- Set word interspace ---*/
      if((scale_flag && (word_inc != 0)) ||
	 (word_inc != last_word_inc))
      {
       F(post).putLine2(post, SHADOW_TEXT, "$c Tw", (coord_t)
		       (word_inc /
			SCALE_TO_COEF(last_scale)));
       F(post).putLine2(post, NORMAL_TEXT, "$c Tw", (coord_t)
		       (word_inc /
			SCALE_TO_COEF(last_scale)));
       post->state.word_space = word_inc;
       last_word_inc = word_inc;
      }
      scale_flag = FALSE;


      if((style->attributes & XcTS_B_SHADOW) && cur_x &&
	 ((style->shadow.dx != 0) || (style->shadow.dy != 0)))
      {
       /* need position */
       F(post).putLine2(post, SHADOW_TEXT, "$c $c Td",
			cur_x, 0);
       F(post).putLine2(post, NORMAL_TEXT, "$c $c Td",
			cur_x, 0);
       last_x += cur_x;
       cur_x = 0;
      }

      /*--- Write text ---*/
/*      printf("cur_x: %g **** %s\n", SCALE_TO_POINTS(cur_x+last_x), 
	     seq->start);
*/ 
      {
       matrix_t *mat;
       
       mat = post->matrix->base+post->matrix->pos; 
       F(style).ps_print_text(style, seq->start, seq->len,
			      post, 
			      -mat->v.dx + cur_x+last_x,
			      -mat->v.dy -line->y_pos, "Tj");
      }

      cur_x += F(style->metrics).textWidth(style->metrics,
					   seq->start,
					   seq->len);
      _i=0;
      _nbr_space=0;
      while (_i < seq->len)
      {
       if (seq->start[_i] == ' ')
	_nbr_space++;
       _i++;
      }
      cur_x += (seq->len - _nbr_space) * (char_inc / SCALE_TO_COEF(last_scale))
       + _nbr_space * (word_inc / SCALE_TO_COEF(last_scale));

      /*
	F(post).putString(post, seq->start, seq->len);
	F(post).putLine(post, "Tj");
	*/
     }
     if(seq->limit_reached == TRUE) break;
     seq = F(text).getNextSeq(text, hline->end_pos);
    }
   }
   line = (textframe_line_t *) hline;
  }
  else
   line++;
 }
 F(post).putLine(post, "$m cm BT", F(post->matrix).get(post->matrix, 0));
 shad = FALSE;
 if (post->shadow_text)
 {
  F(post).deferredWrite(post, SHADOW_TEXT);
  if (post->state2[SHADOW_TEXT].horizontal_scale != SCALE_ONE)
  {
   post->state2[SHADOW_TEXT].horizontal_scale = SCALE_ONE;
   F(post).putLine(post, "$f Tz", (double)SCALE_TO_PERCENT(SCALE_ONE));
  }
  F(post).putLine(post, "ET");
  shad = TRUE;
  if (post->state2[SHADOW_TEXT].fill_color)
   post->state.fill_color = post->state2[SHADOW_TEXT].fill_color;
  if (post->state2[SHADOW_TEXT].stroke_color)
   post->state.stroke_color = post->state2[SHADOW_TEXT].stroke_color;
 }
 if (post->shadow_graph)
 {
  F(post).deferredWrite(post, SHADOW_GRAPH);
  if (post->state2[SHADOW_GRAPH].fill_color)
   post->state.fill_color = post->state2[SHADOW_GRAPH].fill_color;
  if (post->state2[SHADOW_GRAPH].stroke_color)
   post->state.stroke_color = post->state2[SHADOW_GRAPH].stroke_color;
 }
 if (post->normal_text)
 {
  if (shad)
   F(post).putLine(post, "BT");
  F(post).deferredWrite(post, NORMAL_TEXT);
  if (post->state2[NORMAL_TEXT].horizontal_scale != SCALE_ONE)
  {
   post->state2[NORMAL_TEXT].horizontal_scale = SCALE_ONE;
   F(post).putLine(post, "$f Tz", (double)SCALE_TO_PERCENT(SCALE_ONE));
  }
  F(post).putLine(post, "ET");
  if (post->state2[NORMAL_TEXT].fill_color)
   post->state.fill_color = post->state2[NORMAL_TEXT].fill_color;
  if (post->state2[NORMAL_TEXT].stroke_color)
   post->state.stroke_color = post->state2[NORMAL_TEXT].stroke_color;
 }
 if (post->normal_graph)
 {
  F(post).deferredWrite(post, NORMAL_GRAPH);
  if (post->state2[NORMAL_GRAPH].fill_color)
   post->state.fill_color = post->state2[NORMAL_GRAPH].fill_color;
  if (post->state2[NORMAL_GRAPH].stroke_color)
   post->state.stroke_color = post->state2[NORMAL_GRAPH].stroke_color;
 }

 post->state2[NORMAL_TEXT].font = NULL;
 post->state2[SHADOW_TEXT].font = NULL;
 post->state2[NORMAL_TEXT].point_size = 0;
 post->state2[SHADOW_TEXT].point_size = 0;

 post->state2[NORMAL_TEXT].stroke_color = NULL;
 post->state2[NORMAL_GRAPH].stroke_color = NULL;
 post->state2[SHADOW_TEXT].stroke_color = NULL;
 post->state2[SHADOW_GRAPH].stroke_color = NULL;
 post->state2[NORMAL_TEXT].fill_color = NULL;
 post->state2[NORMAL_GRAPH].fill_color = NULL;
 post->state2[SHADOW_TEXT].fill_color = NULL;
 post->state2[SHADOW_GRAPH].fill_color = NULL;
 Xc_TRACE(("end print"));
 return TRUE;
}
