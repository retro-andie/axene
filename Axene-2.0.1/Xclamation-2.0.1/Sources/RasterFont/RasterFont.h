/*
** RasterFont.h for Xclamation in RasterFont/
** Definition of the RasterFont class
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
** Started on  Tue Mar 21 18:15:45 1995 Stéphane Boisson
** Last update Wed Feb  5 18:13:49 1997 Emmanuel Paris
*/

#ifndef __XC_RASTER_FONT_H__
#define __XC_RASTER_FONT_H__

typedef struct sc_RasterFont c_RasterFont;

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "VectorFont.h"
#include "CharMetrics.h"
#include "ImageManager.h"
#include "TextStyle.h"

/*--- Internal structures ---*/
typedef union {
  /*___Image cache___*/
  struct {
    /*___Glyph infos in the image___*/
    unsigned int width;
    unsigned int height;
    unsigned int offset;
    
    /*___Offset of the glyph image in the glyph___*/
    int off_x;
    int off_y;
  } image;
  
  /*___Poly rectangle cache___*/
  struct {
    short dx;
    short dy;
    int count;
    XRectangle *rectangles;
  } vector;
  
  /*___Poly point cache___*/
  struct {
    short dx;
    short dy;
    int	count;
    XPoint *points;
  } dots;
} raster_glyph_t;

enum {
  RF_PIXMAPS,
  RF_RECTANGLES,
  RF_POINTS
};

/*--- Define the methods for the RasterFont Class ---*/
typedef struct
{
  F_STD;

  /*___awidthshow emulation___*/
  void (*drawString) ___PROTO((c_RasterFont *this, Display *display,
			       Drawable drawable, GC mask_gc, GC set_gc,
			       vector_t *pos,
			       coord_t char_inc, coord_t word_inc,
			       char_t *text, unsigned int text_len,
			       c_TextStyle *tstyle));
  boolean (*setGC) ___PROTO((Display *display, Drawable window,
			     GC *gc_mask, GC gc_paint));
} sf_RasterFont;


/*--- RasterFont class definition ---*/
struct sc_RasterFont
{
  sf_RasterFont *f;
  
  /*___Font info___*/
  c_VectorFont *font;
  c_CharMetrics *metrics;

  /*--- ImageManager handle ---*/
  SpoolHandle *spool;
  image_item_t *image;
  XPoint *dot_spool;

  /*___Glyphs infos___*/
  raster_glyph_t glyphs[Xc_CHAR_TOTAL];
};


extern sf_RasterFont fc_RasterFont;

#endif /* !__XC_RASTER_FONT_H__ */

