/*
** print.c for XAllWrite in Calibration/
** Print Text with Calibration.
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
** Started on  Fri Aug 14 14:49:48 1998 Emmanuel Paris
** Last update Sun May 13 14:32:01 2001 Emmanuel Paris
*/

#include "Calibration.h"

/* ----------------------------------------------------------------- ** 
** ps_print_text_lines : print all text lines between start and end  ** 
** ----------------------------------------------------------------- */
boolean ps_print_text_lines(This, post, start, end)
c_Calibration *This; 
c_PostScript *post;
text_line_t *start; 
text_line_t *end;
{
 text_line_t	*tl;
 text_spot_t	*ts;
 c_TextStyle	*style;
 matrix_t	*mat;
 xa_item_t	*item;
 boolean	shadow, printable_item, backgroundable_item;
 coord_t	last_char_inc, last_word_inc, y;
 coord_t	last_x, last_sx, last_y, last_sy;
 coord_t	current_x;
 
 Xc_HISTORY(("print text lines [%d, %d]", start->line_number, 
	     end->line_number));
 
 /*--- Set matrix ---*/
 if(!F(post).pushState(post)) return FALSE;
 F(post->matrix).scale(post->matrix, TRUE, SCALE_ONE, -SCALE_ONE);
 
 /*--- init text rise ---*/
 if(!F(post).putLine2(post, NORMAL_TEXT, "$c Ts", 0)) return FALSE;
 post->state2[NORMAL_TEXT].text_rise = 0;
 if(!F(post).putLine2(post, SHADOW_TEXT, "$c Ts", 0)) return FALSE;
 post->state2[SHADOW_TEXT].text_rise = 0;
 
 last_char_inc = last_word_inc = COORD_ONE << 10;
 last_x = last_sx = last_y = last_sy = current_x = COORD_ZERO;

 F(post).putLine(post, "q $m cm", F(post->matrix).get(post->matrix, 0));

 tl = start;
 while(tl && tl != end->next)
 {
  /*--- Set char interspace ---*/
  if (tl->char_inc != last_char_inc)
  {
   F(post).putLine2(post, SHADOW_TEXT, "$c Tc", (coord_t)tl->char_inc);
   F(post).putLine2(post, NORMAL_TEXT, "$c Tc", (coord_t)tl->char_inc);
   post->state.char_space = tl->char_inc;
   last_char_inc = tl->char_inc;
  }
  
  /*--- Set word interspace ---*/
  if (tl->word_inc != last_word_inc)
  {
   F(post).putLine2(post, SHADOW_TEXT, "$c Tw", (coord_t)tl->word_inc);
   F(post).putLine2(post, NORMAL_TEXT, "$c Tw", (coord_t)tl->word_inc);
   post->state.word_space = tl->word_inc;
   last_word_inc = tl->word_inc;
  }
  
  ts = tl->text_spot;
  
  while(ts)
  {
   item = ts->item;
   style = item->any.style;
   
   switch(item->type)
   {
   case XA_TEXT:
    printable_item = backgroundable_item = TRUE;
    break;
   case XA_TAB:
    if (item->tab.filler != ' ' ||
	(item->any.style->attributes & (XcTS_H_UNDERLINE | XcTS_H_STRIKEOUT)))
     printable_item = TRUE;
    else
     printable_item = FALSE;
    backgroundable_item = TRUE;
    break;
   default:
    printable_item = backgroundable_item = FALSE;
    break;
   }

   if (backgroundable_item && style->bgcolor && !style->bgcolor->transparency)
   {
    mat = post->matrix->base + post->matrix->pos; 
    
    if(!F(post).putLine(post, "q")) return FALSE;
    
    if(!F(post).putLine(post, "$p $v re", 
			-mat->v.dx + ts->x, -mat->v.dy - tl->y, 
			ts->width, -tl->height)) return FALSE;
    
    if(!F(style->bgcolor).ps_print(style->bgcolor, post, FALSE) ||
       !F(post).putLine(post, "f* Q")) return FALSE;
   }

   if (printable_item)
   {
    y = tl->y + ts->y_offset;

    if (((style->attributes & XcTS_B_SHADOW) != 0) && 
	(style->shadow.dx != 0 || style->shadow.dy != 0) &&
	((last_sx != ts->x) || (y != last_sy)))
    {
     F(post).putLine2(post, SHADOW_TEXT, "$c $c Td", ts->x - last_sx, 
		      last_sy - y);
     last_sy = tl->y + ts->y_offset;
     last_sx = ts->x;
    }
   
    if ((last_x != ts->x) || (last_y != y))
    {
     F(post).putLine2(post, NORMAL_TEXT, "$c $c Td", ts->x - current_x, 
		      last_y - y);      
     last_y = tl->y + ts->y_offset;
     last_x = current_x = ts->x;
    }
    
    switch(item->type)
    {
    case XA_TEXT:
     mat = post->matrix->base + post->matrix->pos; 
     F(style).ps_print_text(style, item->text.text, item->text.text_len,
			    post, -mat->v.dx + last_x, -mat->v.dy - last_y,
			    "Tj");
     last_x += ts->width;
     break;
    case XA_TAB:
     {
      coord_t fs, space;
      int nb, i, l;
      char filler[101];
      
      memset(filler, (int)item->tab.filler, 100);
      filler[100] = '\0';
    
      fs = F(item->any.style->metrics).textWidth(item->any.style->metrics,
						 &item->tab.filler, 1);
      nb = (int)(ts->width / fs);
      
      if (nb)
      {
       space = ROUNDFLOAT((ts->width - (fs * nb)) / (double)nb);
       if (space < COORD_ZERO) space = COORD_ZERO;
      }
      else
      {
       nb = 1;
       space = COORD_ZERO;;
      }
      
      /*--- Set char interspace ---*/
      if (last_char_inc != space)
      {
       F(post).putLine2(post, SHADOW_TEXT, "$c Tc", (coord_t)space);
       F(post).putLine2(post, NORMAL_TEXT, "$c Tc", (coord_t)space);
       post->state.char_space = space;
       last_char_inc = space;
      }
      
      /*--- Set word interspace ---*/
      if (last_word_inc != COORD_ZERO)
      {
       F(post).putLine2(post, SHADOW_TEXT, "$c Tw", (coord_t)COORD_ZERO);
       F(post).putLine2(post, NORMAL_TEXT, "$c Tw", (coord_t)COORD_ZERO);
       post->state.word_space = COORD_ZERO;
       last_word_inc = COORD_ZERO;
      }
      mat = post->matrix->base + post->matrix->pos; 
      
      i = nb;
      while(i != 0)
      {
       l = MIN(i, 100);
       
       F(style).ps_print_text(style,  filler, l,
			      post, -mat->v.dx + last_x, -mat->v.dy - last_y,
			      "Tj");       
       i -= l;
       last_x += F(item->any.style->metrics).textWidthSpec
	(item->any.style->metrics, filler, l, space, COORD_ZERO);
      }
     }
     break;
    default:
     break;
    }
   }
  
   ts = ts->next;
  }
  
  last_x = current_x;
  tl = tl->next;
 }
 
 F(post).putLine(post, "BT");

 shadow = FALSE;
 if (post->shadow_text)
 {
  F(post).deferredWrite(post, SHADOW_TEXT);
  if (post->state2[SHADOW_TEXT].horizontal_scale != SCALE_ONE)
  {
   post->state2[SHADOW_TEXT].horizontal_scale = SCALE_ONE;
   F(post).putLine(post, "$f Tz", (double)SCALE_TO_PERCENT(SCALE_ONE));
  }
  F(post).putLine(post, "ET");
  shadow = TRUE;
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
  if (shadow)
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
 Xc_TRACE(("end print text line"));

 F(post->matrix).pop(post->matrix);
 F(post).putLine(post, "Q", NULL);
 return (F(post).popState(post));
}
