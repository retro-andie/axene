/*
** RasterFont.c for Xclamation in RasterFont/
** Methods for the RasterFont class
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
** Started on  Tue Mar 21 18:12:15 1995 Stéphane Boisson
** Last update Fri Oct  2 15:35:56 1998 Robin Castanier
*/

/*#define NTRACE*/
/*#define NHISTORY*/

#include "RasterFontP.h"
#include "t1_port.h"
#include "t1_objects.h"
#include "t1_spaces.h"
#include "t1_util.h"
#include "t1_fontfcn.h"
#include "t1_regions.h"
#include "t1_fontmisc.h"
#include "char_translate.h"

/*--- Font is cached in a image up to this limit (max = 32767) ---*/
#define RASTER_LIMIT 30000

/*--- Font is cached in dot up to this limit ---*/
#define DOT_LIMIT 22

/*--- Dot_spool is allocated by segment, which size are: ---*/
#define DOT_SPOOL_SEG 10000


extern psfont *FontP;

#define  PAD(bits, pad)  (((bits)+(pad)-1)&-(pad))

/*--- Prototypes ---*/
static void *contructor();
static void destructor();
static void *copy();

/*--- Cache prototypes ---*/
static void fill ___PROTO((unsigned char *image,
			   long bytes_per_line,
			   unsigned int offset,
			   struct region *area));
static void reorganize ___PROTO((SpoolHandle *shandle,
				 struct region *area,
				 raster_glyph_t *glyph));
static void calcul_dot_spool ___PROTO((c_RasterFont *this,
				       unsigned long *spool_ptr,
				       unsigned long *spool_size,
				       struct region *area,
				       raster_glyph_t *glyph));
static void relocate_dot_spool ___PROTO((c_RasterFont *this,
					 XPoint *new_ptr));
static unsigned char *changeImage ___PROTO((unsigned char *data,
					    long bpl0, long h0,
					    long bpl1, long h1));
extern struct XYspace *IDENTITY;
extern struct region *fontfcnB();


/*--- Methods ---*/
sf_RasterFont fc_RasterFont =
{
 contructor, destructor, copy,
 rasterDrawString, rasterSetGC
};

sf_RasterFont fc_RasterFontBis =
{
 contructor, destructor, copy,
 rasterDrawStringBis, rasterSetGCBis
};

sf_RasterFont fc_RasterFontTer =
{
 contructor, destructor, copy,
 rasterDrawStringTer, rasterSetGCBis
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *contructor(display, font, metrics)
Display *display;
c_VectorFont *font;
c_CharMetrics *metrics;
{
 long height, width, offset, bytes_per_line = 0;
 unsigned long dot_spool_ptr, dot_spool_size;
 raster_glyph_t *glyph;
 unsigned char *image = NULL;
 psobj *fontencoding;
 unsigned long *pool;
 int raster_flag;
 c_RasterFont *this;
 struct XYspace *S, *S2;
 int rc, i;
 long size;

 Xc_HISTORY(("constructor(`%s', %g pts, %g%%, %g degres)", font->font_name,
	     SCALE_TO_POINTS(metrics->point_size),
	     SCALE_TO_PERCENT(metrics->width),
	     SCALE_TO_DEGREES(metrics->angle)));

 /*--- Get big image size ---*/
 height = RSCALE_TO_PIXELS(metrics->font_bbox.ury - metrics->font_bbox.lly);
 width = RSCALE_TO_PIXELS(metrics->font_width);
 if(!height || !width) return NULL;

 Xc_TIMING_START(("Timing start"));
  
 /*--- Memory allocation for Type1 Imager ---*/
 size = (200000 + 150 * RSCALE_TO_PIXELS(metrics->point_size) * sizeof(short));
 pool = (unsigned long *) Xc_malloc("Type1Buffer", size);
 if(size<0 || pool == NULL ) return 0;
 addmemory(pool, size);

 /*--- Class init ---*/
 if((this = Xc_malloc("RasterFont", sizeof(c_RasterFont)))==NULL)
 {
  delmemory();
  Xc_free(pool);
  return 0;
 }
 memset(this, 0, sizeof(c_RasterFont));
 this->f = &fc_RasterFont;
 this->font = font;
 this->metrics = metrics;

 if (height < DOT_LIMIT)
 {
  Xc_TRACE(("font cached in dot spool w= %d  h=%d", width, height));
  dot_spool_ptr = 0;
  dot_spool_size = DOT_SPOOL_SEG;
  this->dot_spool = (XPoint *)Xc_malloc("rfpnts",
					sizeof(XPoint) * DOT_SPOOL_SEG);
  if (this->dot_spool == NULL)
  {
   delmemory();
   Xc_free(pool);
   DELETE(c_RasterFont)(this);
   return NULL;
  }
  this->f = &fc_RasterFontTer;
  raster_flag = RF_POINTS;
 }
 else
 {
  if((height < RASTER_LIMIT) && (width < RASTER_LIMIT))
  {
   /*--- Use image cache ---*/
   raster_flag = RF_PIXMAPS;
   bytes_per_line = F(GlobImgMng).getBytesPerLine(GlobImgMng, width, 1);
   Xc_TRACE(("w= %d  h=%d (bpl = %ld)", width, height, bytes_per_line));
      
   /*--- Big image allocation ---*/
   if((image = Xc_malloc("FontImage", height * bytes_per_line)) == NULL)
   {
    delmemory();
    Xc_free(pool);
    DELETE(c_RasterFont)(this);
    return NULL;
   }
   memset(image, 0, height * bytes_per_line);
  }
  else
  {
   Xc_TRACE(("w= %d  h=%d", width, height));
      
   if((this->spool = Xc_createSpool("rectangles", 32000, 0)) == NULL)
   {
    delmemory();
    Xc_free(pool);
    DELETE(c_RasterFont)(this);
    return NULL;
   }
   this->f = &fc_RasterFontBis;
   raster_flag = RF_RECTANGLES;
  }
 }
  
 /*--- Load font ---*/
 if(!fontfcnA(font->filename, &rc))
 {
  Xc_ERROR(("%s: %s", font->filename, t1_strerror[-rc]));
  delmemory();
  Xc_free(pool);
  if(raster_flag == RF_PIXMAPS) Xc_free(image);
  DELETE(c_RasterFont)(this);
  return NULL;
 }
 else
 {
  real_t fontmatrix[6];
  real_t scale;

  /*--- Set matrix ---*/
  scale= (real_t) SCALE_TO_POINTS(metrics->point_size);
  QueryFontLib(NULL, "FontMatrix", (void *)fontmatrix, &rc);
  Xc_TRACE(("FontMatrix=[%g %g %g %g]", fontmatrix[0], fontmatrix[1],
	    fontmatrix[2], fontmatrix[3]));
    
#if 0
  S = S2 = (struct XYspace *)
   Permanent(Rotate(Scale(Transform(IDENTITY,
				    fontmatrix[0], fontmatrix[1],
				    fontmatrix[2], fontmatrix[3]),
			  scale * SCALE_TO_COEF(metrics->width), -scale),
		    -SCALE_TO_RADIANS(metrics->angle)));
#else
  S = S2 = (struct XYspace *)Permanent
   (Transform(Transform(IDENTITY,
			fontmatrix[0], fontmatrix[1],
			fontmatrix[2], fontmatrix[3]),
	      metrics->final_matrix.M[0][0],
	      -metrics->final_matrix.M[0][1],
	      metrics->final_matrix.M[1][0],
	      -metrics->final_matrix.M[1][1]));
#endif

  if (metrics->attributes & XcCM_B_SMALL_CAPS)
  {
#if 0
   scale = scale * SCALE_TO_COEF(0.75);
   S2 = (struct XYspace *)
    Permanent(Rotate(Scale(Transform(IDENTITY,
				     fontmatrix[0], fontmatrix[1],
				     fontmatrix[2], fontmatrix[3]),
			   scale * SCALE_TO_COEF(metrics->width), -scale),
		     -SCALE_TO_RADIANS(metrics->angle)));
#else
   S2 = (struct XYspace *)Permanent
    (Transform(Transform(IDENTITY,
			 fontmatrix[0], fontmatrix[1],
			 fontmatrix[2], fontmatrix[3]),
	       metrics->final_matrix2.M[0][0],
	       -metrics->final_matrix2.M[0][1],
	       metrics->final_matrix2.M[1][0],
	       -metrics->final_matrix2.M[1][1]));
#endif
  }
#ifndef NTRACE
  QuerySpace(S, fontmatrix, fontmatrix + 1,
	     fontmatrix + 2, fontmatrix + 3);
    
  Xc_TRACE(("matrix=[%g %g %g %g]", fontmatrix[0], fontmatrix[1],
	    fontmatrix[2], fontmatrix[3]));
#endif
 }
  
 fontencoding = FontP->fontInfoP[ENCODING].value.data.arrayP;
 glyph = &(this->glyphs[Xc_CHAR_TOTAL - 1]);
  
 /*--- Glyphs generation ---*/
 Xc_TRACE(("glyph generation"));
 for(i = Xc_CHAR_TOTAL, offset = 0; i--; glyph--)
 {
  struct region *area;
  char *codename;
  long h, w, len;
  unsigned int conv;
    
  rc = 0;
    
  conv = i + Xc_FIRST_CHAR;
  if (metrics->attributes & (XcCM_B_SMALL_CAPS|XcCM_B_BIG_CAPS))
   conv = (unsigned int)(unsigned char)Xc_TO_UPPER(conv);
				     
  codename = fontencoding[conv].data.valueP;
  len = fontencoding[conv].len; 
    
  if((len == 7) && !strcmp(codename,".notdef")) continue;
    
  if ((metrics->attributes & XcCM_B_SMALL_CAPS) && 
      ( (int)conv != (i + Xc_FIRST_CHAR)))
   area = fontfcnB(S2, codename, &len, &rc);
  else 
   area = fontfcnB(S, codename, &len, &rc);
    
  if(rc < 0)
  {
   Xc_ERROR(("Bad Type1 font name"));
   break;
  }
  else if(rc > 0)
   continue;
    
  if(area == NULL) continue;
    
  h = area->ymax - area->ymin;
  w = area->xmax - area->xmin;
    
  if((h > 0) && (w > 0))
  {
   switch(raster_flag)
   {
   case RF_PIXMAPS:
    /*--- Setup Image cache ---*/
    glyph->image.offset = offset;
    offset += w;
	
    /*--- Check if image data is big enough ---*/
    if((offset > width) || (h > height))
    {
     long new_bpl;
	  
     if(offset <= width)
      new_bpl = bytes_per_line;
     else
      new_bpl = F(GlobImgMng).getBytesPerLine(GlobImgMng,
					      offset, 1);
     image = changeImage(image, bytes_per_line, height, new_bpl,
			 MAX(h, height));
     if(image == NULL)
     {
      delmemory();
      Xc_free(pool);
      DELETE(c_RasterFont)(this);
      return NULL;
     }
     if(offset > width)
     {
      bytes_per_line = new_bpl;
      width = offset;
     }
     if(h > height) height = h;
    }
	
    fill(image, bytes_per_line, glyph->image.offset, area);
    glyph->image.width = w;
    glyph->image.height = h;
    glyph->image.off_x = area->xmin;
    glyph->image.off_y = area->ymin;
    break;
   case RF_RECTANGLES:
    reorganize(this->spool, area, glyph);
    break;
   case RF_POINTS:
    calcul_dot_spool(this, &dot_spool_ptr,
		     &dot_spool_size, area, glyph);
    break;
   }
   Destroy(area);
  }
 }
  
 delmemory();
 Xc_free(pool);
  
 if(rc < 0)
 {
  if(raster_flag == RF_PIXMAPS) Xc_free(image);
  DELETE(c_RasterFont)(this);
  return NULL;
 }
  
 switch(raster_flag)
 {
 case RF_PIXMAPS:
  /*--- Shrink image buffer ---*/
  if(offset < width)
  {
   long new_bpl;
      
   new_bpl = F(GlobImgMng).getBytesPerLine(GlobImgMng, offset, 1);
   image = changeImage(image, bytes_per_line, height, new_bpl, height);
   if(image == NULL)
   {
    DELETE(c_RasterFont)(this);
    return NULL;
   }
   width = offset;
  }
    
  /*--- Call ImageManager ---*/
  this->image = F(GlobImgMng).allocate(GlobImgMng, image,
				       (int)width, (int)height, 1, FALSE);
  if(this->image == NULL)
  {
   Xc_free(image);
   DELETE(c_RasterFont)(this);
   return NULL;
  }
  break;
 case RF_POINTS:
  {
   XPoint *new_ptr;
      
   new_ptr = (XPoint *)Xc_realloc("dot_spool", this->dot_spool,
				  dot_spool_ptr * sizeof(XPoint));
   relocate_dot_spool(this, new_ptr);
  }
  break;
 }
  
 Xc_TIMING(("font cached"));
 Xc_TRACE(("end constructor"));
 return this;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_RasterFont *this;
{
 Xc_HISTORY(("destructor(`%s', %g pts)",
	     this->font->font_name,
	     SCALE_TO_POINTS(this->metrics->point_size)));
  
 if(this->image != NULL) F(GlobImgMng).free(GlobImgMng, this->image);
 if(this->spool != NULL) Xc_destroySpool(this->spool);
 if(this->dot_spool != NULL) Xc_free(this->dot_spool);
 Xc_free(this);

 Xc_TRACE(("end destructor"));
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_RasterFont *this;
{
 Xc_HISTORY(("copy(`%s'): Not implemented", this->font->font_name));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** fill - Scanline bitmap filling                                    ** 
** ----------------------------------------------------------------- */
static void fill(image, bytes_per_line, offset, area)
unsigned char *image;
long bytes_per_line;
unsigned int offset;
struct region *area;
{
 struct edgelist *edge;
 unsigned int xmin = area->xmin - offset;
 unsigned int ymin = area->ymin;

 for(edge = area->anchor; VALIDEDGE(edge); edge = edge->link->link)
 {
  pel *pRight = edge->link->xvalues;
  pel *pLeft = edge->xvalues;
  register unsigned char *p;
  int y;

  p = image + (edge->ymin - ymin) * bytes_per_line;
  for(y = edge->ymin; y < edge->ymax; p += bytes_per_line, y++)
  {
   register pel x0 = *pLeft++ - xmin;
   register pel x1 = *pRight++ - xmin;

   if(x0 > x1)
    continue;
   else
   {
    register int startmask, endmask;
    register int middle = (x1 >> 3) - (x0 >> 3);
    register unsigned char *ptr = p + (x0 >> 3);

    x0 &= 7; x1 &= 7;
    if(GlobImgMng->bit_order_LSBFirst != FALSE)
    {
     startmask = ALLONE << x0;
     endmask = ~(ALLONE << x1);
    }
    else
    {
     startmask = ALLONE >> x0;
     endmask = ~(ALLONE >> x1);
    }

    if(!middle)
     *ptr |= startmask & endmask;
    else
    {
     *ptr++ |= startmask;

     /*--- Fast span ---*/
     while(middle >= 17)
     {
      *ptr++ = ALLONE; *ptr++ = ALLONE; *ptr++ = ALLONE;
      *ptr++ = ALLONE; *ptr++ = ALLONE; *ptr++ = ALLONE;
      *ptr++ = ALLONE; *ptr++ = ALLONE; *ptr++ = ALLONE;
      *ptr++ = ALLONE; *ptr++ = ALLONE; *ptr++ = ALLONE;
      *ptr++ = ALLONE; *ptr++ = ALLONE; *ptr++ = ALLONE;
      *ptr++ = ALLONE; middle -= 16;
     }

     switch(middle)
     {
     case 16: *ptr++ = ALLONE;
     case 15: *ptr++ = ALLONE;
     case 14: *ptr++ = ALLONE;
     case 13: *ptr++ = ALLONE;
     case 12: *ptr++ = ALLONE;
     case 11: *ptr++ = ALLONE;
     case 10: *ptr++ = ALLONE;
     case  9: *ptr++ = ALLONE;
     case  8: *ptr++ = ALLONE;
     case  7: *ptr++ = ALLONE;
     case  6: *ptr++ = ALLONE;
     case  5: *ptr++ = ALLONE;
     case  4: *ptr++ = ALLONE;
     case  3: *ptr++ = ALLONE;
     case  2: *ptr++ = ALLONE;
     }

     *ptr |= endmask;
    }
   }	  
  }
 }
}


/* ----------------------------------------------------------------- ** 
** reorganize - Make XRectangle cache                                ** 
** ----------------------------------------------------------------- */
static void reorganize(shandle, area, glyph)
SpoolHandle *shandle;
struct region *area;
raster_glyph_t *glyph;
{
 register struct edgelist *edge;
 register XRectangle *rect;
 register int count;

 /*--- Get glyph cache ---*/
 for(count = 0, edge = area->anchor; VALIDEDGE(edge); edge = edge->link->link)
  count += edge->ymax - edge->ymin;

 /*--- Alloc cache ---*/
 if((rect = Xc_smalloc(shandle, count * sizeof(XRectangle))) == NULL) return;
 glyph->vector.rectangles = rect--;
  
 /*--- Fill cache ---*/
 for(edge = area->anchor; VALIDEDGE(edge); edge = edge->link->link)
 {
  register pel *pRight = edge->link->xvalues;
  register pel *pLeft = edge->xvalues;
  register pel old_x;
  register short y;
  register unsigned short old_width = 0;

  for(y = edge->ymin, old_x = 0x7fff; y < edge->ymax; y++)
  {
   register unsigned short width;
   register pel x;

   x = *pLeft++;
   width = (*pRight++) - x;
   if((x == old_x) && (old_width == width))
   {
    rect->height++;
    count--;
   }
   else
   {
    rect++;

    rect->width = (old_width = width) + 1;
    rect->height = 1;
    rect->x = old_x = x;
    rect->y = y;
   }
  }
 }
 glyph->vector.count = count;
}

/* ----------------------------------------------------------------- ** 
** calcul_dot_spool - Make XPoints cache                              ** 
** ----------------------------------------------------------------- */
static void calcul_dot_spool(this, spool_ptr, spool_size, area, glyph)
c_RasterFont *this;
unsigned long *spool_ptr;
unsigned long *spool_size;
struct region *area;
raster_glyph_t *glyph;
{
 pel *pRight;
 pel *pLeft;
 short y, old_y;
 XPoint *dot_spool;
 register XPoint *spool;
 register int count = 0;
 register pel x, mx, old_x;
 register unsigned long remain, old_size;
 register struct edgelist *edge;
  
 /*--- Fill cache ---*/
 old_x = old_y = 0;
 spool = this->dot_spool + *spool_ptr;
 remain = *spool_size - *spool_ptr;
 glyph->dots.points = spool;
  
 if (GlobImgMng->draw_points_bug)
 {
  for(edge = area->anchor; VALIDEDGE(edge); edge = edge->link->link)
  {
   pRight = edge->link->xvalues;
   pLeft = edge->xvalues;
      
   for(y = edge->ymin; y < edge->ymax; y++)
   {
    spool->y = y - old_y;
	
    mx = *pRight++;
    for(x = *pLeft++; x < mx; x++)
    {
     if (x == old_x && !spool->y) continue;
     spool->x = x - old_x;
     if (x < old_x)
      spool->y--;
	  
     old_x = x;
     old_y = y;
	  
     spool++;
     if (!(--remain))
     {
      remain = DOT_SPOOL_SEG;
      old_size = *spool_size;
      (*spool_size) += remain;
      dot_spool =
       (XPoint *)Xc_realloc("dot_spool", this->dot_spool, 
			    *spool_size * sizeof(XPoint));
      spool = dot_spool + old_size;
      relocate_dot_spool(this, dot_spool);
     }
     spool->y = 0;
     count++;
    }
   }
  }
 }
 else
 {
  for(edge = area->anchor; VALIDEDGE(edge); edge = edge->link->link)
  {
   pRight = edge->link->xvalues;
   pLeft = edge->xvalues;
      
   for(y = edge->ymin; y < edge->ymax; y++)
   {
    spool->y = y - old_y;
	
    mx = *pRight++;
    for(x = *pLeft++; x < mx; x++)
    {
     if (x == old_x && !spool->y) continue;
     spool->x = x - old_x;
     old_x = x;
     old_y = y;
	  
     spool++;
     if (!(--remain))
     {
      remain = DOT_SPOOL_SEG;
      old_size = *spool_size;
      (*spool_size) += remain;
      dot_spool =
       (XPoint *)Xc_realloc("dot_spool", this->dot_spool, 
			    *spool_size * sizeof(XPoint));
      spool = dot_spool + old_size;
      relocate_dot_spool(this, dot_spool);
     }
     spool->y = 0;
     count++;
    }
   }
  }
 }
 *spool_ptr = *spool_size - remain;
  
 glyph->dots.dx = old_x;
 glyph->dots.dy = old_y;
 glyph->dots.count = count;
}

/* ----------------------------------------------------------------- ** 
** relocate dot spool                                                ** 
** ----------------------------------------------------------------- */
static void relocate_dot_spool(this, new_ptr)
c_RasterFont *this;
XPoint *new_ptr;
{
 long	diff;
 int	i;
  
 diff = new_ptr - this->dot_spool;
 for(i=0; i < Xc_CHAR_TOTAL; i++) 
 {
  if (this->glyphs[i].dots.points)
   this->glyphs[i].dots.points += diff;
 }
 this->dot_spool = new_ptr;
}

/* ----------------------------------------------------------------- ** 
** changeImage - Change image size                                   ** 
** ----------------------------------------------------------------- */
static unsigned char *changeImage(data, bpl0, h0, bpl1, h1)
unsigned char *data;
long bpl0;
long h0;
long bpl1;
long h1;
{
 unsigned char *new_data;
 size_t new_size;

 Xc_TRACE(("changeImage(%ld, %ld -> %ld, %ld)", bpl0, h0, bpl1, h1));

 new_size = bpl1 * h1;

 if((bpl1 > bpl0) || (h1 > h0))
 {
  Xc_TRACE(("growing"));
  Xc_ASSERT((bpl1 >= bpl0) && (h1 >= h0));

  /*--- Realloc new bigger area ---*/
  if((new_data = Xc_realloc("Image buffer", data, new_size)) == NULL)
  {
   Xc_free(data);
   return NULL;
  }

  /*--- Re-arrange when SourceWidth < TargetWidth ---*/
  if(bpl1 > bpl0)
  {
   unsigned char *s, *d;
   long i;

   s = new_data + ((h0 - 1) * bpl0);
   d = new_data + ((h0 - 1) * bpl1);
   for(i = h0 - 1; i--; s -= bpl0, d -= bpl1)
   {
    memmove(d, s, bpl0);
    memset(d + bpl0, 0, bpl1 - bpl0);
   }
   memset(new_data + bpl0, 0, bpl1 - bpl0);
  }
      
  /*--- Tidy up when SourceHeight < TargetHeight ---*/
  if(h1 > h0) memset(new_data + (bpl1 * h0), 0, new_size - (bpl1 * h0));

  return new_data;
 }
 else if((bpl1 < bpl0) || (h1 < h0))
 {
  Xc_TRACE(("shrinking"));
  Xc_ASSERT((bpl1 <= bpl0) && (h1 <= h0));

  /*--- Re-arrange when SourceWidth > TargetWidth ---*/
  if(bpl0 > bpl1)
  {
   unsigned char *s, *d;
   long i;

   s = data + bpl0;
   d = data + bpl1;
   for(i = h1 - 1; i--; s += bpl0, d += bpl1) memmove(d, s, bpl1);
  }

  /*--- Realloc smaller area ---*/
  if((new_data = Xc_realloc("Image buffer", data, new_size)) == NULL)
  {
   Xc_free(data);
   return NULL;
  }

  return new_data;
 }
 return data;
}
