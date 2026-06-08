/*
** drawstring.c for XQuad in RasterFont/
** RasterFont method to draw a string on a X11 drawable
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
** Started on  Tue Mar 21 17:56:50 1995 Stéphane Boisson
** Last update Thu Nov 20 18:05:40 1997 Emmanuel Paris
*/

#define NTRACE
#define NHISTORY

#include "RasterFontP.h"
#include "char_translate.h"

static void rasterDrawShadow();
static void rasterDrawUnderline();
static void rasterDrawStrikeout();

typedef struct _word_pos_t
{
 vector_t	pos1;
 vector_t	pos2;
 struct _word_pos_t *next;
} word_pos_t;

/* ----------------------------------------------------------------- ** 
** draw_string - Draw a string with increments                       ** 
** ----------------------------------------------------------------- */
void rasterDrawString(this, display, drawable, mask_gc, set_gc,
		      pos, char_inc, word_inc, text, text_len, tstyle)
c_RasterFont *this;
Display *display;
Drawable drawable;
GC mask_gc;
GC set_gc;
vector_t *pos;
coord_t char_inc;
coord_t word_inc;
char_t *text;
unsigned int text_len;
c_TextStyle *tstyle;
{
 complex_char_metrics_t **metrics;
 register coord_t x, y, cx, cy, wx, wy;
 register char_t chr;
 boolean	underline = FALSE, strikeout = FALSE, prevspc = TRUE;
 word_pos_t	*word_top = NULL, *word_root = NULL;
 int		word_count = 1;
 coord_t	sx = COORD_ZERO, sy = COORD_ZERO;
  
 /*Xc_HISTORY(("rasterDrawString"));*/
  
 x = pos->dx;
 y = pos->dy;
  
 if (tstyle)
 {
  if (tstyle->attributes & (XcTS_B_SUBSCRIPT | XcTS_B_SUPSCRIPT))
  {
   double sinus, cosinus;
   coord_t shift;
   
   cosinus = cos(SCALE_TO_RADIANS(this->metrics->angle));
   sinus = sin(SCALE_TO_RADIANS(this->metrics->angle));
   
   if (tstyle->attributes & XcTS_B_SUBSCRIPT)
    shift = ROUNDFLOAT(tstyle->subscript.dy * this->metrics->ascender);
   else
    shift = ROUNDFLOAT(tstyle->supscript.dy * this->metrics->ascender);
   sx = (coord_t)ROUNDFLOAT(shift * sinus);
   sy = (coord_t)ROUNDFLOAT(shift * cosinus);
   x -= sx; y -= sy;
  }
  
  if (tstyle->attributes & XcTS_B_SHADOW)
  {
   rasterDrawShadow(this, display, drawable, mask_gc, set_gc,
		    pos, char_inc, word_inc, text, text_len, tstyle);
  }
  
  if (((tstyle->attributes & XcTS_B_UNDERLINE) != 0 &&
       tstyle->underline.overspace) ||
      ((tstyle->attributes & XcTS_B_STRIKEOUT) != 0 &&
       tstyle->strikeout.overspace))
  {
   word_top = word_root = (word_pos_t *)Xc_malloc("wp", sizeof(word_pos_t));
   word_top->pos1.dx = x;
   word_top->pos1.dy = y;
   if ((tstyle->attributes & XcTS_B_UNDERLINE) != 0 && 
       tstyle->underline.overspace)
    underline = TRUE;
   if ((tstyle->attributes & XcTS_B_STRIKEOUT) != 0 && 
       tstyle->strikeout.overspace)
    strikeout = TRUE;
  }
 }
 
 if(char_inc || word_inc)
 {
  double sinus, cosinus;

  cosinus = cos(SCALE_TO_RADIANS(this->metrics->angle));
  sinus = sin(SCALE_TO_RADIANS(this->metrics->angle));
  cx = (coord_t)ROUNDFLOAT(char_inc * cosinus);
  cy = (coord_t)ROUNDFLOAT(char_inc * sinus);
  wx = (coord_t)ROUNDFLOAT(word_inc * cosinus);
  wy = (coord_t)ROUNDFLOAT(word_inc * sinus);
 }
 else
  cx = cy = wx = wy = COORD_ZERO;

 metrics = this->metrics->glyph;
 while(text_len--)
 {
  complex_char_metrics_t *m;

#if 0
  Xc_ASSERT(*text >= Xc_FIRST_CHAR);
#else
  if(*text < Xc_FIRST_CHAR) continue;
#endif

  chr = *text++ - Xc_FIRST_CHAR;
  if((m = metrics[chr]) != NULL)
  {
   int i;

#if 0
   XDrawRectangle(display, window, gc,
		  SCALE_TO_PIXELS(x + m->char_bbox.llx),
		  SCALE_TO_PIXELS(y - m->char_bbox.lly),
		  SCALE_TO_PIXELS(m->char_bbox.urx - m->char_bbox.llx),
		  SCALE_TO_PIXELS(m->char_bbox.lly - m->char_bbox.ury));
#endif

   if((i = m->composite_parts_count) != 0)
   {
    composite_part_t *p;

    p = XcCM_COMPOSITE_PARTS(m);
    for(; i--; p++)
    {
     raster_glyph_t *g;

     g = &(this->glyphs[p->code - Xc_FIRST_CHAR]);
     if(g->image.width == 0) continue;

     F(GlobImgMng).drawMasked(GlobImgMng, this->image, drawable, 
			      mask_gc, set_gc, g->image.offset, 0,
			      g->image.width, g->image.height,
			      g->image.off_x +
			      SCALE_TO_PIXELS(x + p->offset.dx),
			      g->image.off_y +
			      SCALE_TO_PIXELS(y + p->offset.dy));
    }
   }
   else
   {
    Xc_WARNING(("`%s' do not have any composite part",
		m->encoding_name));
   }

   x += m->width[0].dx + cx;
   y -= m->width[0].dy + cy;
   if (Xc_IS_PUNCTUATION(chr + Xc_FIRST_CHAR))
   {
    if (underline || strikeout)
    {
     if (prevspc)
     {
      word_top->pos2.dx = x - m->width[0].dx - cx;
      word_top->pos2.dy = y + m->width[0].dy + cy;
      prevspc = FALSE;
     }
     if (text_len && !Xc_IS_PUNCTUATION(*text))
     {
      word_top->next = (word_pos_t *)Xc_malloc
       ("wp", sizeof(word_pos_t));	
      word_count++;
      word_top = word_top->next;
      word_top->pos1.dx = x + wx;
      word_top->pos1.dy = y + wy;
      prevspc = TRUE;
     }
    }
   }
   
   if(chr == (32 - Xc_FIRST_CHAR))
   {
    x += wx;
    y -= wy;
   }

  }
 }
 
 if (tstyle)
 {
  if (tstyle->attributes & XcTS_B_UNDERLINE)
  {
   vector_t spc;
    
   spc.dx = metrics[32 - Xc_FIRST_CHAR]->width[0].dx + wx;
   spc.dy = metrics[32 - Xc_FIRST_CHAR]->width[0].dy + wy;
   if (underline)
   {
    word_top->next = NULL;
    if (prevspc)
    {
     word_top->pos2.dx = x;
     word_top->pos2.dy = y;
    }      
    rasterDrawUnderline(this, display, drawable, set_gc,
			word_root, word_count, &spc, tstyle);
    if (!strikeout)
    {
     while(word_root)
     {
      word_top = word_root;
      word_root = word_root->next;
      Xc_free(word_top);
     }
    }
   }
   else
   {
    word_pos_t word_pos;
      
    word_pos.pos1.dx = pos->dx - sx; word_pos.pos1.dy = pos->dy - sy;
    word_pos.pos2.dx = x; word_pos.pos2.dy = y;
    word_pos.next = NULL;
    rasterDrawUnderline(this, display, drawable, set_gc,
			&word_pos, 1, &spc, tstyle);      
   }
  }

  if (tstyle->attributes & XcTS_B_STRIKEOUT)
  {
   vector_t spc;
    
   spc.dx = metrics[32 - Xc_FIRST_CHAR]->width[0].dx + wx;
   spc.dy = metrics[32 - Xc_FIRST_CHAR]->width[0].dy + wy;
   if (strikeout)
   {
    word_top->next = NULL;
    if (prevspc)
    {
     word_top->pos2.dx = x;
     word_top->pos2.dy = y;
    }      
    rasterDrawStrikeout(this, display, drawable, set_gc,
			word_root, word_count, &spc, tstyle);
    while(word_root)
    {
     word_top = word_root;
     word_root = word_root->next;
     Xc_free(word_top);
    }
   }
   else
   {
    word_pos_t word_pos;
      
    word_pos.pos1.dx = pos->dx - sx; word_pos.pos1.dy = pos->dy - sy;
    word_pos.pos2.dx = x; word_pos.pos2.dy = y;
    word_pos.next = NULL;
    rasterDrawStrikeout(this, display, drawable, set_gc,
			&word_pos, 1, &spc, tstyle);      
   }
  }
  
  if (tstyle->attributes & (XcTS_B_SUBSCRIPT | XcTS_B_SUPSCRIPT))
  {
   x += sx; y += sy;
  }
 }
 pos->dx = x;
 pos->dy = y;
 /*Xc_TRACE(("drawn"));*/
}


/* ----------------------------------------------------------------- ** 
** rasterDrawStringBis - Draw a string  for XRectangle cache         ** 
** ----------------------------------------------------------------- */
void rasterDrawStringBis(this, display, drawable, mask_gc, set_gc,
			 pos, char_inc, word_inc, text, text_len, tstyle)
c_RasterFont *this;
Display *display;
Drawable drawable;
GC mask_gc;
GC set_gc;
vector_t *pos;
coord_t char_inc;
coord_t word_inc;
char_t *text;
unsigned int text_len;
c_TextStyle *tstyle;
{
 complex_char_metrics_t **metrics;
 register coord_t x, y, cx, cy, wx, wy;
 register char_t chr;
 boolean	underline = FALSE, strikeout = FALSE, prevspc = TRUE;
 word_pos_t	*word_top = NULL, *word_root = NULL;
 int		word_count = 1;
 coord_t	sx = COORD_ZERO, sy = COORD_ZERO;

 Xc_HISTORY(("rasterDrawString"));
  
 x = pos->dx;
 y = pos->dy;
 
 if (tstyle)
 {
  if (tstyle->attributes & (XcTS_B_SUBSCRIPT | XcTS_B_SUPSCRIPT))
  {
   double sinus, cosinus;
   coord_t shift;
    
   cosinus = cos(SCALE_TO_RADIANS(this->metrics->angle));
   sinus = sin(SCALE_TO_RADIANS(this->metrics->angle));
    
   if (tstyle->attributes & XcTS_B_SUBSCRIPT)
    shift = ROUNDFLOAT(tstyle->subscript.dy * this->metrics->ascender);
   else
    shift = ROUNDFLOAT(tstyle->supscript.dy * this->metrics->ascender);
   sx = (coord_t)ROUNDFLOAT(shift * sinus);
   sy = (coord_t)ROUNDFLOAT(shift * cosinus);
   x -= sx; y -= sy;
  }

  if (tstyle->attributes & XcTS_B_SHADOW)
  {
   rasterDrawShadow(this, display, drawable, mask_gc, set_gc,
		    pos, char_inc, word_inc, text, text_len, tstyle);
  }
  
  if (((tstyle->attributes & XcTS_B_UNDERLINE) != 0 &&
       tstyle->underline.overspace) ||
      ((tstyle->attributes & XcTS_B_STRIKEOUT) != 0 &&
       tstyle->strikeout.overspace))
  {
   word_top = word_root = (word_pos_t *)Xc_malloc("wp", sizeof(word_pos_t));
   word_top->pos1.dx = x;
   word_top->pos1.dy = y;
   if ((tstyle->attributes & XcTS_B_UNDERLINE) != 0 && 
       tstyle->underline.overspace)
    underline = TRUE;
   if ((tstyle->attributes & XcTS_B_STRIKEOUT) != 0 && 
       tstyle->strikeout.overspace)
    strikeout = TRUE;
  }
 }
 if(char_inc || word_inc)
 {
  double sinus, cosinus;

  cosinus = cos(SCALE_TO_RADIANS(this->metrics->angle));
  sinus = sin(SCALE_TO_RADIANS(this->metrics->angle));
  cx = (coord_t)ROUNDFLOAT(char_inc * cosinus);
  cy = (coord_t)ROUNDFLOAT(char_inc * sinus);
  wx = (coord_t)ROUNDFLOAT(word_inc * cosinus);
  wy = (coord_t)ROUNDFLOAT(word_inc * sinus);
 }
 else
  cx = cy = wx = wy = COORD_ZERO;

 metrics = this->metrics->glyph;
 while(text_len--)
 {
  complex_char_metrics_t *m;

#if 0
  Xc_ASSERT(*text >= Xc_FIRST_CHAR);
#else
  if(*text < Xc_FIRST_CHAR) continue;
#endif

  chr = *text++ - Xc_FIRST_CHAR;
  if((m = metrics[chr]) != NULL)
  {
   int i;

   if((i = m->composite_parts_count) != 0)
   {
    composite_part_t *p;

    p = XcCM_COMPOSITE_PARTS(m);
    for(; i--; p++)
    {
     register short dx, dy;
     raster_glyph_t *g;

     g = &(this->glyphs[p->code - Xc_FIRST_CHAR]);
     dx = SCALE_TO_PIXELS(x + p->offset.dx) - g->vector.dx;
     dy = SCALE_TO_PIXELS(y + p->offset.dy) - g->vector.dy;

     /*--- Translate rectangles ---*/
     if(dx != 0)
     {
      register int i;
      register XRectangle *ptr;

      ptr = g->vector.rectangles;
      if(dy != 0)
      {
       for(i = g->vector.count; i--; ptr++)
       {
	ptr->x += dx;
	ptr->y += dy;
       }
       g->vector.dy += dy;
      }
      else
       for(i = g->vector.count; i--; ptr++) ptr->x += dx;
		      
      g->vector.dx += dx;
     }
     else if(dy != 0)
     {
      register int i;
      register XRectangle *ptr;

      ptr = g->vector.rectangles;
      for(i = g->vector.count; i--; ptr++) ptr->y += dy;
      g->vector.dy += dy;
     }

     /*--- draw rectangles ---*/
     XFillRectangles(display, drawable, set_gc,
		     g->vector.rectangles, g->vector.count);
		  
    }
   }
   else
   {
    Xc_WARNING(("`%s' do not have any composite part",
		m->encoding_name));
   }

   x += m->width[0].dx + cx;
   y -= m->width[0].dy + cy;
   if(Xc_IS_PUNCTUATION(chr + Xc_FIRST_CHAR))
   {
    if (underline || strikeout)
    {
     if (prevspc)
     {
      word_top->pos2.dx = x - m->width[0].dx - cx;
      word_top->pos2.dy = y + m->width[0].dy + cy;
      prevspc = FALSE;
     }
     if (text_len && !Xc_IS_PUNCTUATION(*text))
     {
      word_top->next = (word_pos_t *)Xc_malloc
       ("wp", sizeof(word_pos_t));	
      word_count++;
      word_top = word_top->next;
      word_top->pos1.dx = x + wx;
      word_top->pos1.dy = y + wy;
      prevspc = TRUE;
     }
    }
   }
   if(chr == (32 - Xc_FIRST_CHAR))
   {
    x += wx;
    y -= wy;
   }
  }
 }

 if (tstyle)
{
 if (tstyle->attributes & XcTS_B_UNDERLINE)
 {
  vector_t spc;
    
  spc.dx = metrics[32 - Xc_FIRST_CHAR]->width[0].dx + wx;
  spc.dy = metrics[32 - Xc_FIRST_CHAR]->width[0].dy + wy;
  if (underline)
  {
   word_top->next = NULL;
   if (prevspc)
   {
    word_top->pos2.dx = x;
    word_top->pos2.dy = y;
   }
   rasterDrawUnderline(this, display, drawable, set_gc,
		       word_root, word_count, &spc, tstyle);
   if (!strikeout)
   {
    while(word_root)
    {
     word_top = word_root;
     word_root = word_root->next;
     Xc_free(word_top);
    }
   }
  }
  else
  {
   word_pos_t word_pos;
      
   word_pos.pos1.dx = pos->dx - sx; word_pos.pos1.dy = pos->dy - sy;
   word_pos.pos2.dx = x; word_pos.pos2.dy = y;
   word_pos.next = NULL;
   rasterDrawUnderline(this, display, drawable, set_gc,
		       &word_pos, 1, &spc, tstyle);      
  }
 }
  
 if (tstyle->attributes & XcTS_B_STRIKEOUT)
 {
  vector_t spc;
    
  spc.dx = metrics[32 - Xc_FIRST_CHAR]->width[0].dx + wx;
  spc.dy = metrics[32 - Xc_FIRST_CHAR]->width[0].dy + wy;
  if (strikeout)
  {
   word_top->next = NULL;
   if (prevspc)
   {
    word_top->pos2.dx = x;
    word_top->pos2.dy = y;
   }      
   rasterDrawStrikeout(this, display, drawable, set_gc,
		       word_root, word_count, &spc, tstyle);
   while(word_root)
   {
    word_top = word_root;
    word_root = word_root->next;
    Xc_free(word_top);
   }
  }
  else
  {
   word_pos_t word_pos;
      
   word_pos.pos1.dx = pos->dx - sx; word_pos.pos1.dy = pos->dy - sy;
   word_pos.pos2.dx = x; word_pos.pos2.dy = y;
   word_pos.next = NULL;
   rasterDrawStrikeout(this, display, drawable, set_gc,
		       &word_pos, 1, &spc, tstyle);      
  }
 }

 if (tstyle->attributes & (XcTS_B_SUBSCRIPT | XcTS_B_SUPSCRIPT))
 {
  x += sx; y += sy;
 }
}
 pos->dx = x;
 pos->dy = y;
 Xc_TRACE(("drawn"));
}

/* ----------------------------------------------------------------- ** 
** rasterDrawStringTer - Draw a string  for XPoints cache            ** 
** ----------------------------------------------------------------- */
void rasterDrawStringTer(this, display, drawable, mask_gc, set_gc,
			 pos, char_inc, word_inc, text, text_len, tstyle)
c_RasterFont *this;
Display *display;
Drawable drawable;
GC mask_gc;
GC set_gc;
vector_t *pos;
coord_t char_inc;
coord_t word_inc;
char_t *text;
unsigned int text_len;
c_TextStyle *tstyle;
{
 complex_char_metrics_t **metrics;
 coord_t x, y, cx, cy, wx, wy;
 register short rx, ry;
 register short ox, oy;
 char_t chr;
 register int j;
 XPoint *spool;
 boolean	underline = FALSE, strikeout = FALSE, prevspc = TRUE;
 word_pos_t	*word_top = NULL, *word_root = NULL;
 int		word_count = 1;
 coord_t	sx = COORD_ZERO, sy = COORD_ZERO;
  
 Xc_HISTORY(("dotDrawString"));
  
 x = pos->dx;
 y = pos->dy;
 
 if (tstyle)
 { 
  if (tstyle->attributes & (XcTS_B_SUBSCRIPT | XcTS_B_SUPSCRIPT))
  {
   double sinus, cosinus;
   coord_t shift;
    
   cosinus = cos(SCALE_TO_RADIANS(this->metrics->angle));
   sinus = sin(SCALE_TO_RADIANS(this->metrics->angle));
    
   if (tstyle->attributes & XcTS_B_SUBSCRIPT)
    shift = ROUNDFLOAT(tstyle->subscript.dy * this->metrics->ascender);
   else
    shift = ROUNDFLOAT(tstyle->supscript.dy * this->metrics->ascender);
   sx = (coord_t)ROUNDFLOAT(shift * sinus);
   sy = (coord_t)ROUNDFLOAT(shift * cosinus);
   x -= sx; y -= sy;
  }

  if (tstyle->attributes & XcTS_B_SHADOW)
  {
   rasterDrawShadow(this, display, drawable, mask_gc, set_gc,
		    pos, char_inc, word_inc, text, text_len, tstyle);
  }
  
  if (((tstyle->attributes & XcTS_B_UNDERLINE) != 0 &&
       tstyle->underline.overspace) ||
      ((tstyle->attributes & XcTS_B_STRIKEOUT) != 0 &&
       tstyle->strikeout.overspace))
  {
   word_top = word_root = (word_pos_t *)Xc_malloc("wp", sizeof(word_pos_t));
   word_top->pos1.dx = x;
   word_top->pos1.dy = y;
   if ((tstyle->attributes & XcTS_B_UNDERLINE) != 0 && 
       tstyle->underline.overspace)
    underline = TRUE;
   if ((tstyle->attributes & XcTS_B_STRIKEOUT) != 0 && 
       tstyle->strikeout.overspace)
    strikeout = TRUE;
  }
 }
 if(char_inc || word_inc)
 {
  double sinus, cosinus;

  cosinus = cos(SCALE_TO_RADIANS(this->metrics->angle));
  sinus = sin(SCALE_TO_RADIANS(this->metrics->angle));
  cx = (coord_t)ROUNDFLOAT(char_inc * cosinus);
  cy = (coord_t)ROUNDFLOAT(char_inc * sinus);
  wx = (coord_t)ROUNDFLOAT(word_inc * cosinus);
  wy = (coord_t)ROUNDFLOAT(word_inc * sinus);
 }
 else
  cx = cy = wx = wy = COORD_ZERO;

 metrics = this->metrics->glyph;
 rx = ry = ox = oy = 0; j = 0;
  
 while(text_len--)
 {
  complex_char_metrics_t *m;
    
#if 0
  Xc_ASSERT(*text >= Xc_FIRST_CHAR);
#else
  if(*text < Xc_FIRST_CHAR) continue;
#endif
    
  chr = *text++ - Xc_FIRST_CHAR;
  if((m = metrics[chr]) != NULL)
  {
   int i, count;
      
   if((i = m->composite_parts_count) != 0)
   {
    composite_part_t *p;
	
    p = XcCM_COMPOSITE_PARTS(m);
    for(; i--; p++)
    {
     register short dx, dy;
     raster_glyph_t *g;
	    
     g = &(this->glyphs[p->code - Xc_FIRST_CHAR]);
	    
     if ((count = g->dots.count) != 0)
     {
      dx = SCALE_TO_PIXELS(x + p->offset.dx);
      dy = SCALE_TO_PIXELS(y + p->offset.dy);
      spool = g->dots.points;
      points[j].x =  (dx - ox) - rx + spool->x;
      points[j++].y = (dy - oy) - ry + spool->y;
      spool++; count--;
      if (j == MAX_POINT)
      {
       XDrawPoints(display, drawable, set_gc,
		   points, MAX_POINT, CoordModePrevious);
       j = 0;
      }
		
      if (j + count < MAX_POINT)
      {
       memcpy(points + j, spool, sizeof(XPoint) * count);
       j += count;
      }
      else
      {
       j = MAX_POINT - j;
       memcpy(points + j, spool,
	      sizeof(XPoint) * j);
       XDrawPoints(display, drawable, set_gc,
		   points, MAX_POINT, CoordModePrevious);
       memcpy(points, spool + j, sizeof(XPoint) * (count - j));
       j = 0;
      }
      rx = g->dots.dx;
      ry = g->dots.dy;
      ox = dx;
      oy = dy;
     }
    }
   }
   else
   {
    Xc_WARNING(("`%s' do not have any composite part",
		m->encoding_name));
   }
      
   x += m->width[0].dx + cx;
   y -= m->width[0].dy + cy;
   if(Xc_IS_PUNCTUATION(chr + Xc_FIRST_CHAR))
   {
    if (underline || strikeout)
    {
     if (prevspc)
     {
      word_top->pos2.dx = x - m->width[0].dx - cx;
      word_top->pos2.dy = y + m->width[0].dy + cy;
      prevspc = FALSE;
     }
     if (text_len && !Xc_IS_PUNCTUATION(*text))
     {
      word_top->next = (word_pos_t *)Xc_malloc
       ("wp", sizeof(word_pos_t));	
      word_count++;
      word_top = word_top->next;
      word_top->pos1.dx = x + wx;
      word_top->pos1.dy = y + wy;
      prevspc = TRUE;
     }
    }
   }
   if(chr == (32 - Xc_FIRST_CHAR))
   {
    x += wx;
    y -= wy;
   }
  }
 }
  
 if (j)
  XDrawPoints(display, drawable, set_gc,
	      points, j, CoordModePrevious);
  
 if (tstyle)
 {
  if (tstyle->attributes & XcTS_B_UNDERLINE)
  { 
   vector_t spc;
    
   spc.dx = metrics[32 - Xc_FIRST_CHAR]->width[0].dx + wx;
   spc.dy = metrics[32 - Xc_FIRST_CHAR]->width[0].dy + wy;
   if (underline)
   {
    word_top->next = NULL;
    if (prevspc)
    {
     word_top->pos2.dx = x;
     word_top->pos2.dy = y;
    }
    rasterDrawUnderline(this, display, drawable, set_gc,
			word_root, word_count, &spc, tstyle);
    if (!strikeout)
    {
     while(word_root)
     {
      word_top = word_root;
      word_root = word_root->next;
      Xc_free(word_top);
     }
    }
   }
   else
   {
    word_pos_t word_pos;
      
    word_pos.pos1.dx = pos->dx - sx; word_pos.pos1.dy = pos->dy - sy;
    word_pos.pos2.dx = x; word_pos.pos2.dy = y;
    word_pos.next = NULL;
    rasterDrawUnderline(this, display, drawable, set_gc,
			&word_pos, 1, &spc, tstyle);      
   }
  }
  
  if (tstyle->attributes & XcTS_B_STRIKEOUT)
  {
   vector_t spc;
    
   spc.dx = metrics[32 - Xc_FIRST_CHAR]->width[0].dx + wx;
   spc.dy = metrics[32 - Xc_FIRST_CHAR]->width[0].dy + wy;
   if (strikeout)
   {
    word_top->next = NULL;
    if (prevspc)
    {
     word_top->pos2.dx = x;
     word_top->pos2.dy = y;
    }      
    rasterDrawStrikeout(this, display, drawable, set_gc,
			word_root, word_count, &spc, tstyle);
    while(word_root)
    {
     word_top = word_root;
     word_root = word_root->next;
     Xc_free(word_top);
    }
   }
   else
   {
    word_pos_t word_pos;
      
    word_pos.pos1.dx = pos->dx - sx; word_pos.pos1.dy = pos->dy - sy;
    word_pos.pos2.dx = x; word_pos.pos2.dy = y;
    word_pos.next = NULL;
    rasterDrawStrikeout(this, display, drawable, set_gc,
			&word_pos, 1, &spc, tstyle);      
   }
  }

  if (tstyle->attributes & (XcTS_B_SUBSCRIPT | XcTS_B_SUPSCRIPT))
  {
   x += sx; y += sy;
  }
 }
 pos->dx = x;
 pos->dy = y;
 Xc_TRACE(("drawn"));
}

/* ----------------------------------------------------------------- ** 
** rasterDrawShadow - draw shadow		                     ** 
** ----------------------------------------------------------------- */
static void rasterDrawShadow(this, display, drawable, mask_gc, set_gc,
			     pos, char_inc, word_inc, text, text_len, tstyle)
c_RasterFont *this;
Display *display;
Drawable drawable;
GC mask_gc;
GC set_gc;
vector_t *pos;
coord_t char_inc;
coord_t word_inc;
char_t *text;
unsigned int text_len;
c_TextStyle *tstyle;
{
 double sinus, cosinus;
 coord_t incx, incy;
 vector_t pos2;
 GC new_gc;
 XGCValues xgcv;
 unsigned long vmask = GCForeground;
 c_Color	*underline, *strikeout;
  
 cosinus = cos(SCALE_TO_RADIANS(this->metrics->angle));
 sinus = sin(SCALE_TO_RADIANS(this->metrics->angle));
  
 pos2.dx = pos->dx; pos2.dy = pos->dy;
  
 incx = this->metrics->ascender * tstyle->shadow.dx;
 incy = this->metrics->ascender * tstyle->shadow.dy;
 pos->dx += (coord_t)ROUNDFLOAT(incx * cosinus + incy * sinus);
 pos->dy += (coord_t)ROUNDFLOAT(incy * cosinus - incx * sinus);
  
 xgcv.foreground = tstyle->shadow.color->cell.pixel;
 new_gc = XCreateGC(display, drawable, vmask, &xgcv);
 XCopyGC(display, set_gc, ~vmask, new_gc);
  
 tstyle->attributes &= ~XcTS_B_SHADOW;

 underline = tstyle->underline.color;
 strikeout = tstyle->strikeout.color;
 tstyle->underline.color = tstyle->shadow.color;
 tstyle->strikeout.color = tstyle->shadow.color;

 F(this).drawString(this, display, drawable, mask_gc, new_gc,
		    pos, char_inc, word_inc, text, text_len, tstyle);
  
 tstyle->underline.color = underline;
 tstyle->strikeout.color = strikeout;

 tstyle->attributes |= XcTS_B_SHADOW;

 XFreeGC(display, new_gc);   
 pos->dx = pos2.dx; pos->dy = pos2.dy;  
}

/* ----------------------------------------------------------------- ** 
** rasterDrawUnderline - draw underline                             ** 
** ----------------------------------------------------------------- */
static void rasterDrawUnderline(this, display, drawable, set_gc,
				word_pos, word_count, spc, tstyle)
c_RasterFont *this;
Display *display;
Drawable drawable;
GC set_gc;
word_pos_t *word_pos;
int word_count;
vector_t *spc;
c_TextStyle *tstyle;
{
 GC	new_gc;
 XSegment *seg;
 int	nb_seg;
 coord_t x1, x2, y1, y2, thick;
 coord_t rx = COORD_ZERO, ry = COORD_ZERO;
 int	line_width;
 XGCValues xgcv;
 unsigned long vmask = 0;
  
  
 xgcv.function = GXcopy;
 vmask |= GCFunction;
 if (tstyle->color != tstyle->underline.color)
 {
  xgcv.foreground = tstyle->underline.color->cell.pixel;
  vmask |= GCForeground;
 }
  
 thick = this->metrics->ascender * tstyle->underline.thickness;
 line_width = SCALE_TO_PIXELS(thick);
 if (line_width > 1)
 {
  xgcv.line_width = line_width;
  vmask |= GCLineWidth;
 }
  
 new_gc = XCreateGC(display, drawable, vmask, &xgcv);
 XCopyGC(display, set_gc, ~vmask, new_gc);
  
 seg = (XSegment *)Xc_malloc("seg", sizeof(XSegment) * word_count * 3);
 nb_seg = 0;
  
 while(word_count--)
 {
  x1 = word_pos->pos1.dx;
  y1 = word_pos->pos1.dy;
  x2 = word_pos->pos2.dx;
  y2 = word_pos->pos2.dy;
    
  word_pos = word_pos->next;
    
  if (x1 == x2 && y1 == y2) continue;
    
  if (tstyle->underline.overstroke != 0.0)
  {
   x1 -= (spc->dx * tstyle->underline.overstroke);
   y1 += (spc->dy * tstyle->underline.overstroke);
   x2 += (spc->dx * tstyle->underline.overstroke);
   y2 -= (spc->dy * tstyle->underline.overstroke);      
  }
    
  if (tstyle->underline.dy != 0.0)
  {
   double sinus, cosinus;
   coord_t rx, ry, hh;
      
   cosinus = cos(SCALE_TO_RADIANS(this->metrics->angle));
   sinus = sin(SCALE_TO_RADIANS(this->metrics->angle));
      
   hh = this->metrics->ascender * tstyle->underline.dy;
   rx = (coord_t)ROUNDFLOAT(hh * sinus);
   ry = (coord_t)ROUNDFLOAT(hh * cosinus);
      
   x1 += rx; x2 += rx;
   y1 += ry; y2 += ry;
  }
    
  if (tstyle->underline.type != XcTS_SIMPLE)
  {
   double sinus, cosinus;
      
   cosinus = cos(SCALE_TO_RADIANS(this->metrics->angle));
   sinus = sin(SCALE_TO_RADIANS(this->metrics->angle));
      
   rx = SCALE_TO_PIXELS((coord_t)ROUNDFLOAT(thick * 2 * sinus));
   ry = SCALE_TO_PIXELS((coord_t)ROUNDFLOAT(thick * 2 * cosinus));      
  }
    
  switch(tstyle->underline.type)
  {
  case XcTS_TRIPLE:
   seg[nb_seg].x1 = SCALE_TO_PIXELS(x1) - rx; 
   seg[nb_seg].x2 = SCALE_TO_PIXELS(x2) - rx;
   seg[nb_seg].y1 = SCALE_TO_PIXELS(y1) - ry;
   seg[nb_seg].y2 = SCALE_TO_PIXELS(y2) - ry;
   nb_seg ++;
  case XcTS_DOUBLE:
   seg[nb_seg].x1 = SCALE_TO_PIXELS(x1) + rx; 
   seg[nb_seg].x2 = SCALE_TO_PIXELS(x2) + rx;
   seg[nb_seg].y1 = SCALE_TO_PIXELS(y1) + ry;
   seg[nb_seg].y2 = SCALE_TO_PIXELS(y2) + ry;
   nb_seg ++;
  case XcTS_SIMPLE:  
   seg[nb_seg].x1 = SCALE_TO_PIXELS(x1); 
   seg[nb_seg].x2 = SCALE_TO_PIXELS(x2);
   seg[nb_seg].y1 = SCALE_TO_PIXELS(y1);
   seg[nb_seg].y2 = SCALE_TO_PIXELS(y2);
   nb_seg ++;
   break;
  }
    
 }
 XDrawSegments(display, drawable, new_gc, seg, nb_seg);
  
 XFreeGC(display, new_gc); 
 Xc_free(seg);
}

/* ----------------------------------------------------------------- ** 
** rasterDrawUnderline - draw strikeout                             ** 
** ----------------------------------------------------------------- */
static void rasterDrawStrikeout(this, display, drawable, set_gc,
				word_pos, word_count, spc, tstyle)
c_RasterFont *this;
Display *display;
Drawable drawable;
GC set_gc;
word_pos_t *word_pos;
int word_count;
vector_t *spc;
c_TextStyle *tstyle;
{
 GC	new_gc;
 XSegment *seg;
 int	nb_seg;
 coord_t x1, x2, y1, y2, thick;
 coord_t rx = COORD_ZERO, ry = COORD_ZERO;
 int	line_width;
 XGCValues xgcv;
 unsigned long vmask = 0;
  
  
 xgcv.function = GXcopy;
 vmask |= GCFunction;
 if (tstyle->color != tstyle->strikeout.color)
 {
  xgcv.foreground = tstyle->strikeout.color->cell.pixel;
  vmask |= GCForeground;
 }
  
 thick = this->metrics->ascender * tstyle->strikeout.thickness;
 line_width = SCALE_TO_PIXELS(thick);
 if (line_width > 1)
 {
  xgcv.line_width = line_width;
  vmask |= GCLineWidth;
 }
  
 new_gc = XCreateGC(display, drawable, vmask, &xgcv);
 XCopyGC(display, set_gc, ~vmask, new_gc);
  
 seg = (XSegment *)Xc_malloc("seg", sizeof(XSegment) * word_count * 3);
 nb_seg = 0;
  
 while(word_count--)
 {
  x1 = word_pos->pos1.dx;
  y1 = word_pos->pos1.dy;
  x2 = word_pos->pos2.dx;
  y2 = word_pos->pos2.dy;
    
  word_pos = word_pos->next;
    
  if (x1 == x2 && y1 == y2) continue;
    
  if (tstyle->strikeout.overstroke != 0.0)
  {
   x1 -= (spc->dx * tstyle->strikeout.overstroke);
   y1 += (spc->dy * tstyle->strikeout.overstroke);
   x2 += (spc->dx * tstyle->strikeout.overstroke);
   y2 -= (spc->dy * tstyle->strikeout.overstroke);      
  }
    
  if (tstyle->strikeout.dy != 0.0)
  {
   double sinus, cosinus;
   coord_t rx, ry, hh;
      
   cosinus = cos(SCALE_TO_RADIANS(this->metrics->angle));
   sinus = sin(SCALE_TO_RADIANS(this->metrics->angle));
      
   hh = this->metrics->ascender * tstyle->strikeout.dy;
   rx = (coord_t)ROUNDFLOAT(hh * sinus);
   ry = (coord_t)ROUNDFLOAT(hh * cosinus);
      
   x1 += rx; x2 += rx;
   y1 += ry; y2 += ry;
  }
    
  if (tstyle->strikeout.type != XcTS_SIMPLE)
  {
   double sinus, cosinus;
      
   cosinus = cos(SCALE_TO_RADIANS(this->metrics->angle));
   sinus = sin(SCALE_TO_RADIANS(this->metrics->angle));
      
   rx = SCALE_TO_PIXELS((coord_t)ROUNDFLOAT(thick * 2 * sinus));
   ry = SCALE_TO_PIXELS((coord_t)ROUNDFLOAT(thick * 2 * cosinus));      
  }
    
  switch(tstyle->strikeout.type)
  {
  case XcTS_TRIPLE:
   seg[nb_seg].x1 = SCALE_TO_PIXELS(x1) - rx; 
   seg[nb_seg].x2 = SCALE_TO_PIXELS(x2) - rx;
   seg[nb_seg].y1 = SCALE_TO_PIXELS(y1) - ry;
   seg[nb_seg].y2 = SCALE_TO_PIXELS(y2) - ry;
   nb_seg ++;
  case XcTS_DOUBLE:
   seg[nb_seg].x1 = SCALE_TO_PIXELS(x1) + rx; 
   seg[nb_seg].x2 = SCALE_TO_PIXELS(x2) + rx;
   seg[nb_seg].y1 = SCALE_TO_PIXELS(y1) + ry;
   seg[nb_seg].y2 = SCALE_TO_PIXELS(y2) + ry;
   nb_seg ++;
  case XcTS_SIMPLE:  
   seg[nb_seg].x1 = SCALE_TO_PIXELS(x1); 
   seg[nb_seg].x2 = SCALE_TO_PIXELS(x2);
   seg[nb_seg].y1 = SCALE_TO_PIXELS(y1);
   seg[nb_seg].y2 = SCALE_TO_PIXELS(y2);
   nb_seg ++;
   break;
  }
    
 }
 XDrawSegments(display, drawable, new_gc, seg, nb_seg);
  
 XFreeGC(display, new_gc); 
 Xc_free(seg);
}

/* ----------------------------------------------------------------- ** 
** rasterSetGC - Set GC mask and GC paint function                   ** 
** ----------------------------------------------------------------- */
boolean rasterSetGC(display, drawable, gc_mask, gc_paint)
Display *display;
Drawable drawable;
GC *gc_mask;
GC gc_paint;
{
 Xc_HISTORY(("rasterSetGC(mask=%lx, paint=%lx)",
	     (long)*gc_mask, (long)gc_paint));

 if(*gc_mask == (GC)0)
 {
  XGCValues gc_values;

  /*--- Set mask ---*/
  gc_values.function = GXand;
  gc_values.background = -1;
  gc_values.foreground = 0;
  *gc_mask = XCreateGC(display, drawable,
		       GCFunction|GCForeground|GCBackground, &gc_values);
  XCopyGC(display, gc_paint, GCClipXOrigin|GCClipYOrigin|GCClipMask,
	  *gc_mask);

  /*--- Set paint ---*/
  gc_values.function = GXor;
  gc_values.background = 0;
  XChangeGC(display, gc_paint, GCFunction|GCBackground, &gc_values);
 }

 Xc_TRACE(("done"));
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** rasterSetGCBis - Set mask GC and paint gc for vector cache        ** 
** ----------------------------------------------------------------- */
boolean rasterSetGCBis(display, drawable, gc_mask, gc_paint)
Display *display;
Drawable drawable;
GC *gc_mask;
GC gc_paint;
{
 Xc_HISTORY(("rasterSetGC(mask=%lx, paint=%lx)",
	     (long)*gc_mask, (long)gc_paint));

 if(*gc_mask != (GC)0)
 {
  XFreeGC(display, *gc_mask);
  *gc_mask = (GC)0;
 }
  
 /*--- Set paint ---*/
 XSetFunction(display, gc_paint, GXcopy);

 Xc_TRACE(("done"));
 return TRUE;
}








