/*
** calibrate.c for Xclamation in TextFrame/
** Calibration functions
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
** Started on  Wed Oct 12 10:17:20 1994 Stéphane Boisson
** Last update Wed Jun  4 22:16:13 1997 Antoine Buat
*/

#define NTRACE

#include "TextFrameP.h"

#define XcTF_CALIBRATION_INIT 128

#ifndef NTRACE
static void show ___PROTO((textframe_char_t *start, long len));
#endif
static boolean grow_buffer ___PROTO((textframe_char_line_t *line,
				     unsigned long extra));

/* ----------------------------------------------------------------- ** 
** CalibrationStart - Start calibration                              ** 
** ----------------------------------------------------------------- */
boolean CalibrationStart(line, line_init_size)
textframe_char_line_t *line;
unsigned long line_init_size;
{
 Xc_TRACE(("CalibrationStart"));

 /*--- Init textframe_char buffer ---*/
 line->buffer = NULL;
 line->pos = line->len = 0;
 if(grow_buffer(line, XcTF_CALIBRATION_INIT) == FALSE) return FALSE;

 /*--- Init textframe_line buffer ---*/
 line_init_size *= sizeof(textframe_hline_t) + sizeof(textframe_line_t);
 line->line_buffer = Xc_malloc("TMP Frame metrics", line_init_size);
 if(line->line_buffer == NULL) return FALSE;
  
 line->line_buffer_end = (char *)line->line_buffer + line_init_size - 1;
 line->line_buffer_end -= sizeof(textframe_hline_t);

 line->line_ptr = line->line_buffer;

 /*--- Init values ---*/
 line->line_count = 0;
 line->descender = line->style->metrics->descender;
 if(line->ruler->inter_mode == TRUE)
 {
  line->interline = line->ruler->inter_line;
  line->y_pos = line->style->point_size;
 }
 else
 {
  line->interline = line->style->point_size;
  line->y_pos = 0;
 }
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** CalibrationEnd - End calibration                                  ** 
** ----------------------------------------------------------------- */
void CalibrationEnd(line, ptr, count)
textframe_char_line_t *line;
textframe_line_t **ptr;
unsigned int *count;
{
 unsigned long size;

 Xc_TRACE(("CalibrationEnd"));

 Xc_free(line->buffer);
 if((size = (char *)line->line_ptr - (char *)line->line_buffer) == 0)
 {
  Xc_TRACE(("Empty frame metrics!"));
  *ptr = NULL;
  *count = 0;
  Xc_free(line->line_buffer);
  line->line_buffer = NULL;
 }
 else
 {
  textframe_line_t *new, *old;

  old = line->line_buffer;
  new = Xc_realloc("Frame metrics", old, size);
  *ptr = (new != NULL)? new : old;
  *count = line->line_count;
 }
}

/* ----------------------------------------------------------------- ** 
** CalibrationShift - Add chars to the current line                  ** 
** ----------------------------------------------------------------- */
boolean CalibrationShift(line, seq)
textframe_char_line_t *line;
text_seq_t *seq;
{
 complex_char_metrics_t **glyph;
 textframe_char_t *ptr;
 coord_t interline;
 coord_t r_desc;
 text_pos_t pos, len;
 char_t *start;

 Xc_TRACE(("CalibrationShift"));

 len = seq->len;
 if((line->pos + len) > line->len)
  if(grow_buffer(line, (len + line->pos) - line->len) == FALSE) return FALSE;

 ptr = line->buffer + line->pos;
 line->pos += len;

 glyph = line->style->metrics->glyph - Xc_FIRST_CHAR;

 if(line->ruler->inter_mode == TRUE)
  interline = line->ruler->inter_line;
 else
 {
  interline = line->style->point_size;
  if(interline > line->interline) 
   line->interline = interline;
 }

 r_desc = line->style->metrics->descender;
 if(r_desc < line->descender)
  line->descender = r_desc;

 start = seq->start;
 for(pos = seq->start_pos; len--; pos++)
 {
  register char_t code;
      
  code = *start++;
  if(code <  Xc_FIRST_CHAR)
  {
#ifndef NTRACE
   fprintf(stderr, "<%x>", code);
#endif
   continue;
  }
  else
  {
   register complex_char_metrics_t *m;

#ifndef NTRACE
   putc((code == 32)? '_' : code, stderr);
#endif

   if((m = glyph[code]) == NULL)
   {
    line->pos--;
    continue;
   }
   ptr->pos = pos;
   ptr->code = code;
   ptr->width = m->width[0].dx;
   ptr->height = interline; 
   ptr->width_delta = 0;
/*ptr->descender = line->descender;*/
   ptr->descender = r_desc;
  }
  ptr++;
 }
#ifndef NTRACE
 putc('\n', stderr);
#endif
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** CalibrationReduce - Reduction of the buffer                       ** 
** ----------------------------------------------------------------- */
boolean CalibrationReduce(line, count, segment, end_pos)
textframe_char_line_t *line;
int count;
sl_Segment *segment;
text_pos_t end_pos;
{
 textframe_hline_t *hline_ptr;
 double min_ratio, max_ratio;
 textframe_char_t *ptr;
 int n;

 Xc_TRACE(("CalibrationReduce(count=%d, end_pos=%ld)", count, end_pos));

 if((n = line->pos) == 0)
 {
  line->reduction_flag = FALSE;
  return FALSE;
 }
 Xc_TRACE(("line->pos = %d", n));

 ptr = line->buffer;
 min_ratio = SCALE_TO_COEF(line->ruler->min_ratio);
 max_ratio = SCALE_TO_COEF(line->ruler->max_ratio);
 hline_ptr = (textframe_hline_t *)(line->line_ptr + 1);
 line->line_ptr->column_count = 0;

 /*--- Loop for every segment in line ---*/
 for(; (count > 0) && (n > 0); count--, segment++)
 {
  int space_count, start_n, mark_n, count2;
  coord_t mark_len, len, min_len, max_len;
  textframe_char_t *mark_ptr, *start_ptr;
  boolean word_flag;

  Xc_TRACE(("Remaining segments/chars: %d/%d", count, n));
  Xc_TRACE(("min %g -> max %g", (double)SCALE_TO_POINTS(segment->x_min),
	    (double)SCALE_TO_POINTS(segment->x_max)));
  max_len = ((segment->x_max - segment->x_min)
	     - (line->ruler->left_margin + line->ruler->right_margin));
  if(line->paragraph_flag) max_len -= line->ruler->paragraph_indent;
  if(max_len <= 0) continue;

  min_len = ROUNDFLOAT((float)max_len * min_ratio);
  if(line->ruler->justification == XcTR_JUSTIFIED)
   max_len = ROUNDFLOAT((float)max_len * max_ratio);
  space_count = 0;

  /*--- Init marker values ---*/
  mark_n = start_n = n;
  mark_ptr = start_ptr = ptr;
  mark_len = len = COORD_ZERO;
  word_flag = FALSE;
  count2 = 0;

  /*--- Start calibration loop ---*/
  do {
#ifndef NTRACE
      putc((ptr->code == 32)? '_' : ptr->code, stderr);
#endif
      if(ptr->code == 32)
      {
       if(word_flag)
       {
	space_count += count2;
	count2 = 0;
	word_flag = FALSE;
	mark_n = n;
	mark_len = len;
	mark_ptr = ptr;
       }
       count2++;
      }
      else if(!word_flag)
       word_flag = TRUE;
     } while(((len += (ptr++)->width) <= max_len) && (--n > 0));

#ifndef NTRACE
  putc('\n', stderr);
#endif

  /*--- Enough chars to fill the line ---*/
  if(n != 0)
  {
   Xc_TRACE(("-> Completion"));
   n--;

   /*--- Adjust mark_len ---*/
   mark_len += mark_ptr[-1].width_delta;
   if(mark_len > max_len)
   {
    Xc_TRACE(("mark_len > max_len"));
    n = start_n;
    ptr = start_ptr;
    continue;
   }

   if(mark_len < min_len)
   {
    if(mark_len == 0)
    {
     if((ptr > start_ptr) && (ptr[-1].code == 32))
     {
      Xc_TRACE(("Big space"));
		      
      mark_n = n;
      mark_ptr = ptr - 1;
      mark_len = len - ptr[-1].width + ptr[-1].width_delta;
      Xc_ASSERT(mark_len <= max_len);
      space_count = mark_ptr - start_ptr;
     }
    }
    else
    {
     Xc_TRACE(("hyphenization needed"));
		  
     if(mark_len != COORD_ZERO)
      mark_len -= mark_ptr[-1].width_delta;
		  
     /*=== appel Cesure ici ===*/
    }
 
    if(mark_len == COORD_ZERO)
    {
     if(start_ptr->code == 32)
     {
      mark_n = start_n;
      mark_ptr = start_ptr;
      mark_len = COORD_ZERO;
      space_count = 0;
      len = start_ptr->width - start_ptr->width_delta;
      while((mark_ptr[1].code == 32) &&
	    ((mark_len + len) < max_len))
      {
       mark_len += start_ptr->width;
       mark_ptr++;
       mark_n--;
       space_count++;
      } 
      mark_len -= start_ptr->width_delta;
     }
     else
     {
      Xc_TRACE(("Word doesn't fit on line"));
      n = start_n;
      ptr = start_ptr;
      continue;
     }
    }
   }
	  
   n = mark_n - 1;
   ptr = mark_ptr + 1;
  }
  else if(end_pos != 0)
  {
   Xc_TRACE(("-> Force completion"));
   space_count += count2;
   mark_ptr = ptr;
   mark_len = len + ptr[-1].width_delta;
  }
  else
  {
   Xc_TRACE(("-> No completion"));
   Xc_TRACE(("end CalibrationReduce"));
   line->reduction_flag = FALSE;
   return FALSE;
  }

  /*--- Record line ---*/
  Xc_TRACE(("Record line"));
#ifndef NTRACE
  show(start_ptr, mark_ptr - start_ptr);
#endif

  Xc_ASSERT((char *)hline_ptr < line->line_buffer_end);

  hline_ptr->start_pos = start_ptr->pos;
  if(end_pos == 0)
   hline_ptr->end_pos = mark_ptr->pos;
  else
   hline_ptr->end_pos = (n == 0)? ABS(end_pos) : mark_ptr->pos;
  hline_ptr->x_min = segment->x_min + line->ruler->left_margin;
  hline_ptr->x_max = segment->x_max - line->ruler->right_margin;
  hline_ptr->gap = hline_ptr->x_max - hline_ptr->x_min - mark_len;

  /*--- Adjust paragraph indent ---*/
  if(line->paragraph_flag)
  {
   hline_ptr->x_min += line->ruler->paragraph_indent;
   hline_ptr->gap -= line->ruler->paragraph_indent;
   line->paragraph_flag = FALSE;
  }

  /*--- Adjust last justified paragraph line ---*/
  if((line->ruler->justification == XcTR_JUSTIFIED) && (n == 0)
     && (hline_ptr->gap > 0) && (end_pos < 0)) hline_ptr->gap = 0;

  hline_ptr->space_count = space_count;
  hline_ptr->char_count = start_n - n - ((n == 0)? 1:2);

  Xc_TRACE(("x_min = %g", SCALE_TO_POINTS(hline_ptr->x_min)));
  Xc_TRACE(("x_max = %g", SCALE_TO_POINTS(hline_ptr->x_max)));
  Xc_TRACE(("gap = %g", SCALE_TO_POINTS(hline_ptr->gap)));
  Xc_TRACE(("char_count = %d", hline_ptr->char_count));
  Xc_TRACE(("space_count = %d", hline_ptr->space_count));

  /*--- Go to next one ---*/
  hline_ptr++;
  line->line_ptr->column_count++;
 }

 line->y_pos += line->interline;
 if(line->line_ptr->column_count > 0)
 {
  line->line_ptr->y_pos = line->y_pos;
  if(line->ruler->inter_mode == FALSE)
  {
   line->line_ptr->y_pos += line->descender; 
   line->y_pos += line->ruler->inter_line;
  }
  line->line_ptr = (textframe_line_t *) hline_ptr;
  line->line_count++;
  line->reduction_flag = TRUE;
 }
 else
 {
  Xc_TRACE(("No reduction performed"));
  line->reduction_flag = FALSE;
 }

 /*--- Buffer reduction ---*/
 if(n != 0)
 {
  memmove(line->buffer,
	  line->buffer + line->pos - n,
	  n * sizeof(textframe_char_t));
 }
 line->pos = n;

 Xc_TRACE(("end CalibrationReduce"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** CalibrationReactualize - Recompute line height                    ** 
** ----------------------------------------------------------------- */
void CalibrationReactualize(line, start)
textframe_char_line_t *line;
int	start;
{
 textframe_char_t *ptr;
 int n;

 Xc_TRACE(("CalibrationReactualize"));

 if(line->ruler->inter_mode == TRUE)
  line->interline = line->ruler->inter_line;
 else
  line->interline = line->style->point_size;
 line->descender = line->style->metrics->descender;

 if(line->ruler->inter_mode == FALSE)
  for(ptr = line->buffer, n = line->pos; n-- > 0; ptr++)
  {
   if(ptr->height > line->interline) line->interline = ptr->height;
   if(ptr->descender < line->descender) line->descender = ptr->descender;
  }
 else
  for(ptr = line->buffer, n = line->pos; n-- > 0; ptr++)
   if(ptr->descender < line->descender) line->descender = ptr->descender;
}


/* ----------------------------------------------------------------- ** 
** grow_buffer - Grow calibration buffer                             ** 
** ----------------------------------------------------------------- */
static boolean grow_buffer(line, extra)
textframe_char_line_t *line;
unsigned long extra;
{
 textframe_char_t *tmp;

 Xc_TRACE(("grow_buffer(%ld)", extra));

 extra += line->len;
 tmp = Xc_realloc("Calibration Buffer",
		  line->buffer,
		  extra * sizeof(textframe_char_t));
 if(tmp != NULL)
 {
  line->buffer = tmp;
  line->len = extra;
  return TRUE;
 }
 return FALSE;
}

#ifndef NTRACE
/* ----------------------------------------------------------------- ** 
** show - debug function                                             ** 
** ----------------------------------------------------------------- */
static void show(start, len)
textframe_char_t *start;
long len;
{
 while(len-- > 0)
 {
  char_t code;

  code = (start++)->code;
  putc(((code == 32)? '_' : code), stderr);
 }
 putc('\n', stderr);
}
#endif
