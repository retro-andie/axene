/*
** RasterFontP.h for XQuad in RasterFont/
** Private header for RasterFont class
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
** Started on  Tue Mar 21 17:59:39 1995 Stéphane Boisson
** Last update Wed Feb  5 18:13:53 1997 Emmanuel Paris
*/

#ifndef __XC_RASTER_FONT_P_H__
#define __XC_RASTER_FONT_P_H__

#include "RasterFont.h"

#define ALLONE ((unsigned char)0xff)

void rasterDrawString ___PROTO((c_RasterFont *this,
				Display *display,
				Drawable drawable,
				GC gc_mask,
				GC gc_paint,
				vector_t *pos,
				coord_t char_inc,
				coord_t word_inc,
				char_t *text,
				unsigned int text_len,
				c_TextStyle *tstyle));

void rasterDrawStringBis ___PROTO((c_RasterFont *this,
				   Display *display,
				   Drawable drawable,
				   GC gc_mask,
				   GC gc_paint,
				   vector_t *pos,
				   coord_t char_inc,
				   coord_t word_inc,
				   char_t *text,
				   unsigned int text_len,
				   c_TextStyle *tstyle));

void rasterDrawStringTer ___PROTO((c_RasterFont *this,
				   Display *display,
				   Drawable drawable,
				   GC gc_mask,
				   GC gc_paint,
				   vector_t *pos,
				   coord_t char_inc,
				   coord_t word_inc,
				   char_t *text,
				   unsigned int text_len,
				   c_TextStyle *tstyle));

boolean rasterSetGC ___PROTO((Display *display, Drawable window,
			      GC *gc_mask, GC gc_paint));
boolean rasterSetGCBis ___PROTO((Display *display, Drawable window,
				 GC *gc_mask, GC gc_paint));


/*--- Image manager ---*/
extern c_ImageManager *GlobImgMng;


#endif /* !__XC_RASTER_FONT_P_H__ */




