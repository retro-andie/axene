/*
** Color.c for Xclamation, XQuad and XAllWrite in Color/
** Methods for the Color class
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
** Started on  Sun Jun 12 02:16:38 1994 Stéphane Boisson
** Last update Mon Nov  2 16:38:19 1998 Emmanuel Paris
*/

#define NHISTORY
#define NTRACE

#include "Color.h"
#include <string.h>
#include <math.h>
#include <varargs.h>


/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();
static void set_color ()  /* ___PROTO((c_Color *c, ...)) */ ;
static boolean compare ___PROTO((c_Color *a, c_Color *b, boolean with_name));
static void merge ___PROTO((c_Color *source, c_Color **target,boolean fusion));
static c_Color *getColor ___NPROTO((c_Color **base, ...));

static rgb_t *get_RGB_Color ___PROTO((c_Color *c));
static cmyk_t *get_CMYK_Color ___PROTO((c_Color *c));
static ycbcr_t *get_YCbCr_Color ___PROTO((c_Color *c));
static hsl_t *get_HSL_Color ___PROTO((c_Color *c));

static void deleteColorBase ___PROTO((c_Color *color_base));
static void copyColorBase ___PROTO((c_Color *color_base, c_Color **base));

static boolean readColor ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_Color **base));
static boolean readColorList ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_Color **base));
static boolean writeColor ___PROTO((c_Color *this, c_DocFile *doc));
static boolean writeColorList ___PROTO((c_Color *this, c_DocFile *doc));
static void setGC ___PROTO((c_Color *this, Display *display, GC gc));
static c_Color *virtualCopy ___PROTO((c_Color *color, BaseStd_t *base));

/*--- Loading callbacks ---*/
static void vset ___PROTO((c_Color *c, va_list ap));
static boolean cb_trans ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Color *color));
static boolean cb_rgb ___PROTO((c_DocFile *doc, char *keyword,
				long param, c_Color *color));
static boolean cb_gray ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Color *color));
static boolean cb_cmyk ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Color *color));
static boolean cb_ycbcr ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Color *color));
static boolean cb_hsl ___PROTO((c_DocFile *doc, char *keyword,
				long param, c_Color *color));
static boolean cb_name ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Color *color));
static boolean cb_lock ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Color *color));
static boolean ps_print ___PROTO((c_Color *this,
				  c_PostScript *post, boolean stroke_flag));
static boolean ps_print2 ___PROTO((c_Color *this, c_PostScript *post,
				   int type, boolean stroke_flag));

static void convert_RGBtoCYMK ___PROTO((c_Color *c));
static void convert_RGBtoYCbCr ___PROTO((c_Color *c));
static void convert_RGBtoHSL ___PROTO((c_Color *c));
static void convert_CYMKtoRGB ___PROTO((c_Color *c));
static void convert_YCbCrtoRGB ___PROTO((c_Color *c));
static void convert_HSLtoRGB ___PROTO((c_Color *c));

sf_Color fc_Color =
{
 constructor, destructor, copy,
 (void (*) ___PROTO((c_Color *this, ...)))set_color,
 compare, merge, deleteColorBase, copyColorBase, getColor, virtualCopy,
 readColor, writeColor, readColorList, writeColorList,
 get_RGB_Color, get_CMYK_Color, get_YCbCr_Color, get_HSL_Color,
 setGC, ps_print, ps_print2
};

static ItemHandle *i_handle = NULL;
static long i_references = 0;
extern c_Colormap *GlobColormap;


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(color_base, name)
c_Color **color_base;
char *name;
{
 c_Color *c, *ct;
 
 Xc_HISTORY(("constructor"));
 
 if(i_handle == NULL)
  i_handle = Xc_createItem("Color base", 16, sizeof(c_Color));
 if(i_handle == NULL) return NULL;
 
 if((c = Xc_new(i_handle)) == NULL) return NULL;
 memset(c, 0, sizeof(c_Color));
 if((c->hook = NEW(c_Hook)(c, 16)) == NULL)
 {
  Xc_delete(i_handle, c);
  return NULL;
 }

 c->f = &fc_Color;
 i_references++;
  
 /*--- Insert in linked list ---*/
 if(*color_base == NULL)
 {
  c->next = c->previous = NULL;
  *color_base = c;
 }
 else
 {
  ct = *color_base;
  while(ct->next) ct = ct->next;
      
  ct->next = c;
  c->next = NULL;
  c->previous = ct;
 }
 c->base = color_base;
 c->lock_flag = c->hide_flag = FALSE;
  
 F(c).set(c, XcC_GRAY, 0, XcC_TRANSPARENCY, 0, XcC_END);
 if(name) F(c).set(c, XcC_NAME, name, XcC_END);

 Xc_TRACE(("constructor end"));
 return c;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_Color *this;
{
 Xc_HISTORY(("destructor(`%s')", this->name));
 Xc_ASSERT(i_references > 0);
  
 DELETE(c_Hook)(this->hook);

 /*--- Remove from the linked list ---*/
 if(this->next != NULL) this->next->previous = this->previous;
 Xc_TRACE(("$%lx (base=$%lx prev=$%lx next=$%lx)",
	   (long)this, (long)this->base,
	   (long)this->previous, (long)this->next));
  
 if(this->previous != NULL) this->previous->next = this->next;
 if(*(this->base) == this)
 {
  Xc_ASSERT(this->previous == NULL);
  *(this->base) = this->next;
 }
  
 Xc_delete(i_handle, this);
 if(--i_references == 0)
 {
  Xc_destroyItem(i_handle);
  i_handle = NULL;
 }
}


/* ----------------------------------------------------------------- ** 
** Copier                                                            ** 
** ----------------------------------------------------------------- */
static void *copy(this, base)
c_Color *this;
c_Color **base;
{
 c_Color *p, *ct;
  
 Xc_HISTORY(("copy(`%s')", this->name));
  
 if((p = Xc_new(i_handle)) == NULL)
  return NULL;
  
 memcpy(p, this, sizeof(c_Color));
 p->lock_flag = FALSE;
 if((p->hook = NEW(c_Hook)(p, 16)) == NULL)
 {
  Xc_delete(i_handle, p);
  return NULL;
 }

 i_references++;
  
 /*--- Insert copy in linked list ---*/
 if(base == NULL)
  base = this->base;
  
 if (*base == NULL)
 {
  p->next = p->previous = NULL;
  *base = p;
 }
 else
 {
  ct = *base;
  while(ct->next) ct = ct->next;
    
  ct->next = p;
  p->next = NULL;
  p->previous = ct;
 }
 p->base = base;
  
 Xc_TRACE(("end copy"));
 return p;
}


/* ----------------------------------------------------------------- ** 
** virtualCopy - Copy if object doesn't exist already in base        ** 
** ----------------------------------------------------------------- */
static c_Color *virtualCopy(this, base)
c_Color *this;
BaseStd_t *base;
{
 c_Color *color;

 Xc_HISTORY(("virtualCopy(`%s')", this->name));
 
 if(this->base == &(base->color_base)) return this;
 for(color = base->color_base; color != NULL; color = color->next)
  if(F(this).compare(color, this, CMP_WITH_NAME))
  {
   Xc_TRACE(("found in new base"));
   break;
  }
 
 return((color == NULL)? COPY(c_Color)(this, &base->color_base) : color);
}


/* ----------------------------------------------------------------- ** 
** set_color - Set color parameters                                  ** 
** ----------------------------------------------------------------- */
static void set_color(c, va_alist)
c_Color *c;
va_dcl
{
 va_list ap;
  
 Xc_HISTORY(("set(`%s')", c->name));

 va_start(ap);
 vset(c, ap);
 va_end(ap);
 F(c->hook).callback(c->hook, XcH_CONTENT_CHANGED, c);
}


/* ----------------------------------------------------------------- ** 
** getColor - Get or create color                                    ** 
** ----------------------------------------------------------------- */
static c_Color *getColor(base, va_alist)
c_Color **base;
va_dcl
{
 c_Color color, *ptr;
 va_list ap;

 Xc_HISTORY(("getColor"));

 memset(&color, 0, sizeof(c_Color));
 va_start(ap);
 vset(&color, ap);
 va_end(ap);

 for(ptr = *base; ptr != NULL; ptr = ptr->next)
  if(F(ptr).compare(ptr, &color, CMP_WITHOUT_NAME)) return ptr;

 color.f = &fc_Color;
 return copy(&color, base);
}


/* ----------------------------------------------------------------- ** 
** vset - Set color parameters                                       ** 
** ----------------------------------------------------------------- */
static void vset(c, ap)
c_Color *c;
va_list ap;
{
 boolean out_flag;
  
 Xc_TRACE(("vset"));

 out_flag = FALSE;
 do 
 {
  switch(va_arg(ap, color_set_code_t))
  {
  case XcC_END:
   Xc_TRACE(("XcC_END"));
   out_flag = TRUE;
   break;
   
  case XcC_NAME:
   c->hide_flag = FALSE;
   strncpy(c->name, va_arg(ap, char *), XcMAX_NAME_LENGTH - 1);
   Xc_TRACE(("XcC_NAME `%s'", c->name));
   continue;
   
  case XcC_TRANSPARENCY:
   c->transparency = (unsigned char)va_arg(ap, int);
   Xc_TRACE(("XcC_TRANSPARENCY %d", (int)c->transparency));
   continue;
   
  case XcC_RGB:
   c->mode = XcC_RGB;
   c->rgb.r = va_arg(ap, unsigned int);
   c->rgb.g = va_arg(ap, unsigned int);
   c->rgb.b = va_arg(ap, unsigned int);
   if(!(*(c->name)))
   {
    c->hide_flag = TRUE;
    sprintf(c->name, "R%d G%d B%d",
	    (int)XcC_PERCENT_COLOR(c->rgb.r),
	    (int)XcC_PERCENT_COLOR(c->rgb.g),
	    (int)XcC_PERCENT_COLOR(c->rgb.b));
   }
   convert_RGBtoCYMK(c);
   convert_RGBtoYCbCr(c);
   convert_RGBtoHSL(c);
   Xc_TRACE(("RGB colorspace R%d G%d B%d", c->rgb.r, c->rgb.g, c->rgb.b));
   continue;
   
  case XcC_CMYK:
   c->mode = XcC_CMYK;
   c->cmyk.c = va_arg(ap, unsigned int);
   c->cmyk.m = va_arg(ap, unsigned int);
   c->cmyk.y = va_arg(ap, unsigned int);
   c->cmyk.k = va_arg(ap, unsigned int);
   if(!(*(c->name)))
   {
    c->hide_flag = TRUE;
    sprintf(c->name, "C%d M%d Y%d K%d",
	    (int)XcC_PERCENT_COLOR(c->cmyk.c),
	    (int)XcC_PERCENT_COLOR(c->cmyk.m),
	    (int)XcC_PERCENT_COLOR(c->cmyk.y),
	    (int)XcC_PERCENT_COLOR(c->cmyk.k));
   }
   convert_CYMKtoRGB(c);
   convert_RGBtoYCbCr(c);
   convert_RGBtoHSL(c);
   Xc_TRACE(("CMYK colorspace C%d M%d Y%d K%d",c->cmyk.c, c->cmyk.m,
	     c->cmyk.y, c->cmyk.k));
   continue;
   
  case XcC_YCbCr:
   c->mode = XcC_YCbCr;
   c->ycbcr.y = va_arg(ap, unsigned int);
   c->ycbcr.cb = va_arg(ap, unsigned int);
   c->ycbcr.cr = va_arg(ap, unsigned int);
   if(!(*(c->name)))
   {
    c->hide_flag = TRUE;
    sprintf(c->name, "Y%d Cb%d Cr%d",
	    (int)XcC_PERCENT_COLOR(c->ycbcr.y),
	    (int)XcC_PERCENT_COLOR(c->ycbcr.cb),
	    (int)XcC_PERCENT_COLOR(c->ycbcr.cr));
   }
   convert_YCbCrtoRGB(c);
   convert_RGBtoCYMK(c);
   convert_RGBtoHSL(c);
   Xc_TRACE(("YCbCr colorspace Y%d Cb%d Cr%d", c->ycbcr.y, c->ycbcr.cb,
	     c->ycbcr.cr));
   continue;
   
  case XcC_HSL:
   c->mode = XcC_HSL;
   c->hsl.h = va_arg(ap, unsigned int);
   c->hsl.s = va_arg(ap, unsigned int);
   c->hsl.l = va_arg(ap, unsigned int);
   if(!(*(c->name)))
   {
    c->hide_flag = TRUE;
    sprintf(c->name, "H%d S%d L%d",
	    (int)XcC_PERCENT_COLOR(c->hsl.h),
	    (int)XcC_PERCENT_COLOR(c->hsl.s),
	    (int)XcC_PERCENT_COLOR(c->hsl.l));
   }
   convert_HSLtoRGB(c);
   convert_RGBtoCYMK(c);
   convert_RGBtoYCbCr(c);
   Xc_TRACE(("HSL colorspace H%d S%d L%d", c->hsl.h, c->hsl.s, c->hsl.l));
   continue;
   
  case XcC_GRAY:
   c->mode = XcC_GRAY;
   c->ycbcr.y = va_arg(ap, unsigned int);
   c->ycbcr.cb = c->ycbcr.cr = XcC_MAX_COLOR_VALUE >> 1;
   if(!(*(c->name)))
   {
    switch(c->ycbcr.y)
    {
    case 0:
     strcpy(c->name, "Black");
     break;
    case XcC_MAX_COLOR_VALUE:
     strcpy(c->name, "White");
     break;
    default:
     sprintf(c->name, "Gray %d%%",
	     (int)XcC_PERCENT_COLOR(c->ycbcr.y));
     break;
    }
    c->hide_flag = TRUE;
   }
   convert_YCbCrtoRGB(c);
   convert_RGBtoCYMK(c);
   convert_RGBtoHSL(c);
   Xc_TRACE(("Grayscale G%d", c->ycbcr.y));
   continue;
   
  case XcC_LOCK:
   c->lock_flag = TRUE;
   Xc_TRACE(("Lock flag set"));
   continue;
  case XcC_HIDE_ON:
   c->hide_flag = TRUE;
   break;
  case XcC_HIDE_OFF:
   c->hide_flag = FALSE;
   break;
  default:
   Xc_BREAK(("Unknown color-set-code."));
   break;
  }
 } while(!out_flag);
 
 F(GlobColormap).get_closest_color(GlobColormap, &c->cell,
				   XcC_SHORTVALUE_COLOR(c->rgb.r),
				   XcC_SHORTVALUE_COLOR(c->rgb.g),
				   XcC_SHORTVALUE_COLOR(c->rgb.b));
}


/* ----------------------------------------------------------------- ** 
** compare - Compare two Color objects                               ** 
** ----------------------------------------------------------------- */
static boolean compare(a, b, with_name)
c_Color *a;
c_Color *b;
boolean with_name;
{
 Xc_TRACE(("compare(`%s', `%s')", a->name, b->name));

 if (a == b) return TRUE;
 if ((a == NULL) || (b == NULL)) return FALSE;
  
 if (with_name && 
     !(a->hide_flag || b->hide_flag || Xstrcmp(a->name, b->name)))
  return FALSE;
  
 if((a->transparency == b->transparency) &&
    (a->mode == b->mode))
  switch(a->mode)
  {
  case XcC_RGB:
   return(((a->rgb.r == b->rgb.r) && (a->rgb.g == b->rgb.g) &&
	   (a->rgb.b == b->rgb.b))? TRUE : FALSE);
  case XcC_CMYK:
   return(((a->cmyk.c == b->cmyk.c) && (a->cmyk.m == b->cmyk.m) &&
	   (a->cmyk.y == b->cmyk.y) && (a->cmyk.k == b->cmyk.k))?
	  TRUE : FALSE);
  case XcC_GRAY:
  case XcC_YCbCr:
   return(((a->ycbcr.y == b->ycbcr.y) && (a->ycbcr.cb == b->ycbcr.cb) &&
	   (a->ycbcr.cr == b->ycbcr.cr))? TRUE : FALSE);
  case XcC_HSL:
   return(((a->hsl.h == b->hsl.h) && (a->hsl.s == b->hsl.s) &&
	   (a->hsl.l == b->hsl.l))? TRUE : FALSE);
  default:
   Xc_BREAK(("Arrg"));
  }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** merge - Merge two color list                                      ** 
** ----------------------------------------------------------------- */
static void merge(source, target, fusion_flag)
c_Color *source;
c_Color **target;
boolean fusion_flag;
{
 c_Color *ptr, *base;

 Xc_HISTORY(("merge"));

 source = *source->base;
 base = *target;

 Xc_TRACE(("src_base=$%lx dst_base=$%lx", source->base, target));
 Xc_ASSERT(target != source->base);

 for(ptr = base; ptr != NULL; ptr = ptr->next) ptr->id = -1;

 while(source != NULL)
 {
  Xc_TRACE(("Source `%s' #%ld", source->name, source->id));
  if(fusion_flag != FALSE)
  {
   for(ptr = base; ptr != NULL; ptr = ptr->next)
   {
    Xc_TRACE(("  Target `%s' #%ld", ptr->name, ptr->id));
    if(F(source).compare(source, ptr, CMP_WITH_NAME)) break;
   }

   if(ptr != NULL)
   {
    Xc_TRACE(("  Found %ld (ref %ld)", ptr->id, source->id));

    ptr->id = source->id;

    ptr = source;
    source = source->next;

    DELETE(c_Color)(ptr);
    continue;
   }
  }

  Xc_TRACE(("  Add %lx to %lx", (long)source, (long)base));

  ptr = source;
  source = source->next;

  /*--- Remove from source base ---*/
  Xc_ASSERT(ptr->previous == NULL);
  if(ptr->next != NULL) ptr->next->previous = NULL;
  *ptr->base = ptr->next;

  /*--- Add to target base ---*/
  ptr->next = base;
  ptr->base = target;
  if(base != NULL) base->previous = ptr;
  *target = base = ptr;
 }
}


/* ----------------------------------------------------------------- ** 
** readColorList - Load some colors                                  ** 
** ----------------------------------------------------------------- */
static boolean readColorList(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
c_Color **base;
{
 Xc_TRACE(("readColorList"));

 if(!F(doc).addCallbacks(doc, XcDF_COLOR_KEYWORD, readColor, base, NULL))
  return FALSE;
 return F(doc).expectKeyword(doc, XcDF_COLOR_KEYWORD, TRUE);
}


/* ----------------------------------------------------------------- ** 
** readColor - Set a color from a saved form                         ** 
** ----------------------------------------------------------------- */
static boolean readColor(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
c_Color **base;
{
 c_Color *color;

 Xc_TRACE(("readColor(%ld)", param));

 if((color = NEW(c_Color)(base, NULL)) == NULL) return FALSE;
 Xc_ASSERT(color->hide_flag == TRUE);
 
 do {
     color->id = param;
     if(!F(doc).addCallbacks(doc,
			     XcDF_TRANSPARENCY_KEYWORD, cb_trans, color,
			     XcDF_RGB_KEYWORD, cb_rgb, color,
			     XcDF_CMYK_KEYWORD, cb_cmyk, color,
			     XcDF_YCBCR_KEYWORD, cb_ycbcr, color,
			     XcDF_HSL_KEYWORD, cb_hsl, color,
			     XcDF_GRAY_KEYWORD, cb_gray, color,
			     XcDF_LOCK_KEYWORD, cb_lock, color,
			     XcDF_NAME_KEYWORD, cb_name, color,
			     NULL)) break;
     if(!F(doc).expectKeyword(doc, NULL, TRUE)) break;
     return TRUE;
    } while(0);
 DELETE(c_Color)(color);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_name - Name keyword loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_name(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Color *this;
{
 unsigned char *name;

 Xc_TRACE(("cb_name"));
 if(!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  F(this).set(this, XcC_NAME, name, XcC_END);
  Xc_free(name);
 }
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_lock - lock loading callback                                   ** 
** ----------------------------------------------------------------- */
static boolean cb_lock(doc, keyword, param, color)
c_DocFile *doc;
char *keyword;
long param;
c_Color *color;
{
 F(color).set(color, XcC_LOCK, XcC_END);
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_trans - Transparency loading callback                          ** 
** ----------------------------------------------------------------- */
static boolean cb_trans(doc, keyword, param, color)
c_DocFile *doc;
char *keyword;
long param;
c_Color *color;
{
 Xc_TRACE(("cb_trans(%ld)", param));

 param = MIN(255, MAX(0, param));
 F(color).set(color, XcC_TRANSPARENCY, (int)param, XcC_END);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_rgb - RGB loading callback                                     ** 
** ----------------------------------------------------------------- */
static boolean cb_rgb(doc, keyword, param, color)
c_DocFile *doc;
char *keyword;
long param;
c_Color *color;
{
 real r, g, b;

 Xc_TRACE(("cb_rgb"));
 if(F(doc).readReal(doc, &r) && F(doc).readReal(doc, &g) &&
    F(doc).readReal(doc, &b))
 {
  F(color).set(color, XcC_RGB,
	       XcC_SCALE_COLOR(r), XcC_SCALE_COLOR(g), XcC_SCALE_COLOR(b),
	       XcC_END);
  return TRUE;
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_cmyk - CMYK loading callback                                   ** 
** ----------------------------------------------------------------- */
static boolean cb_cmyk(doc, keyword, param, color)
c_DocFile *doc;
char *keyword;
long param;
c_Color *color;
{
 real c, m, y, k;

 Xc_TRACE(("cb_cmyk"));
 if(F(doc).readReal(doc, &c) && F(doc).readReal(doc, &m) &&
    F(doc).readReal(doc, &y) && F(doc).readReal(doc, &k))
 {
  F(color).set(color, XcC_CMYK, XcC_SCALE_COLOR(c), XcC_SCALE_COLOR(m),
	       XcC_SCALE_COLOR(y), XcC_SCALE_COLOR(k), XcC_END);
  return TRUE;
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_ycbcr - YCbCr loading callback                                 ** 
** ----------------------------------------------------------------- */
static boolean cb_ycbcr(doc, keyword, param, color)
c_DocFile *doc;
char *keyword;
long param;
c_Color *color;
{
 real y, cb, cr;

 if(F(doc).readReal(doc, &y) && F(doc).readReal(doc, &cb) &&
    F(doc).readReal(doc, &cr))
 {
  F(color).set(color, XcC_YCbCr, XcC_SCALE_COLOR(y), XcC_SCALE_COLOR(cb),
	       XcC_SCALE_COLOR(cr), XcC_END);
  return TRUE;
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_gray - Grayscale loading callback                              ** 
** ----------------------------------------------------------------- */
static boolean cb_gray(doc, keyword, param, color)
c_DocFile *doc;
char *keyword;
long param;
c_Color *color;
{
 real g;

 Xc_TRACE(("cb_gray"));
 if(F(doc).readReal(doc, &g))
 {
  F(color).set(color, XcC_GRAY, XcC_SCALE_COLOR(g), XcC_END);
  return TRUE;
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_hsl - HSL loading callback                                     ** 
** ----------------------------------------------------------------- */
static boolean cb_hsl(doc, keyword, param, color)
c_DocFile *doc;
char *keyword;
long param;
c_Color *color;
{
 real h, s, l;

 if(F(doc).readReal(doc, &h) && F(doc).readReal(doc, &s) &&
    F(doc).readReal(doc, &l))
 {
  F(color).set(color, XcC_HSL, XcC_SCALE_COLOR(h), XcC_SCALE_COLOR(s),
	       XcC_SCALE_COLOR(l), XcC_END);
  return TRUE;
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** writeColor - Write the color description                          ** 
** ----------------------------------------------------------------- */
static boolean writeColor(this, doc)
c_Color *this;
c_DocFile *doc;
{
 Xc_HISTORY(("write(`%s')", this->name));

#ifdef Xc_XCLAMATION
 if(this->hide_flag) return TRUE;
#endif

 if(F(doc).writeKeyword(doc, XcDF_COLOR_KEYWORD, this->id) &&
    F(doc).startSequence(doc))
 {
  if((this->transparency != 0) &&
     !F(doc).writeKeyword(doc, XcDF_TRANSPARENCY_KEYWORD,
			  this->transparency)) return FALSE;

  if((this->mode == XcC_RGB) &&
     (!F(doc).writeKeyword(doc, XcDF_RGB_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->rgb.r)) ||
      !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->rgb.g)) ||
      !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->rgb.b)) ||
      !F(doc).endSequence(doc))) return FALSE;
  else if((this->mode == XcC_CMYK) &&
	  (!F(doc).writeKeyword(doc, XcDF_CMYK_KEYWORD, -1L) ||
	   !F(doc).startSequence(doc) ||
	   !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->cmyk.c)) ||
	   !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->cmyk.m)) ||
	   !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->cmyk.y)) ||
	   !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->cmyk.k)) ||
	   !F(doc).endSequence(doc))) return FALSE;
  else if((this->mode == XcC_YCbCr) &&
	  (!F(doc).writeKeyword(doc, XcDF_YCBCR_KEYWORD, -1L) ||
	   !F(doc).startSequence(doc) ||
	   !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->ycbcr.y))||
	   !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real,this->ycbcr.cb))||
	   !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real,this->ycbcr.cr))||
	   !F(doc).endSequence(doc))) return FALSE;
  else if((this->mode == XcC_HSL) &&
	  (!F(doc).writeKeyword(doc, XcDF_HSL_KEYWORD, -1L) ||
	   !F(doc).startSequence(doc) ||
	   !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->hsl.h)) ||
	   !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->hsl.s)) ||
	   !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->hsl.l)) ||
	   !F(doc).endSequence(doc))) return FALSE;
  else if((this->mode == XcC_GRAY) &&
	  (!F(doc).writeKeyword(doc, XcDF_GRAY_KEYWORD, -1L) ||
	   !F(doc).startSequence(doc) ||
	   !F(doc).writeReal(doc, XcC_DESCALE_COLOR(real, this->ycbcr.y))||
	   !F(doc).endSequence(doc))) return FALSE;
  else if((this->lock_flag == TRUE) && 
	  (!F(doc).writeKeyword(doc, XcDF_LOCK_KEYWORD, -1L))) 
   return FALSE;
      
  if((this->hide_flag == FALSE) &&
     (!F(doc).writeKeyword(doc, XcDF_NAME_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !F(doc).write(doc, this->name, strlen(this->name)) ||
      !F(doc).endSequence(doc))) return FALSE;
      
  return F(doc).endSequence(doc);
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** writeColorList - Write all the colors in the list                 ** 
** ----------------------------------------------------------------- */
static boolean writeColorList(this, doc)
c_Color *this;
c_DocFile *doc;
{
 c_Color *ptr;
 long id;

 Xc_HISTORY(("writeList"));
 if(!F(doc).writeKeyword(doc, XcDF_COLORLIST_KEYWORD, -1L) ||
    !F(doc).startSequence(doc)) return FALSE;
 for(ptr = *this->base, id = 0; ptr != NULL; ptr = ptr->next, id++)
 {
  ptr->id = id;
  if(!F(ptr).write(ptr, doc)) return FALSE;
 }
 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** get_RGB_Color - Get RGB component of color                        ** 
** ----------------------------------------------------------------- */
static rgb_t *get_RGB_Color(c)
c_Color *c;
{
 Xc_HISTORY(("get_RGB_Color: R%d G%d B%d", c->rgb.r, c->rgb.g, c->rgb.b));
 return &(c->rgb);
}


/* ----------------------------------------------------------------- ** 
** get_CMYK_Color - Get CMYK component of color                      ** 
** ----------------------------------------------------------------- */
static cmyk_t *get_CMYK_Color(c)
c_Color *c;
{
 Xc_HISTORY(("get_CMYK_Color: C%d M%d Y%d K%d",c->cmyk.c, c->cmyk.m,
	     c->cmyk.y, c->cmyk.k));
 return &(c->cmyk);
}


/* ----------------------------------------------------------------- ** 
** get_YCbCr_Color - Get YCbCr component of color                    ** 
** ----------------------------------------------------------------- */
static ycbcr_t *get_YCbCr_Color(c)
c_Color *c;
{
 Xc_HISTORY(("get_YCbCr_Color: Y%d Cb%d Cr%d", c->ycbcr.y, c->ycbcr.cb,
	     c->ycbcr.cr));
 return &(c->ycbcr);
}


/* ----------------------------------------------------------------- ** 
** get_HSL_Color - Get HSL component of color                        ** 
** ----------------------------------------------------------------- */
static hsl_t *get_HSL_Color(c)
c_Color *c;
{
 Xc_HISTORY(("get_HSL_Color: H%d S%d L%d", c->hsl.h, c->hsl.s, c->hsl.l));
 return &(c->hsl);
}


/* ----------------------------------------------------------------- ** 
** convert_RGBtoCYMK - Convert RGB to CYMK colorspace                ** 
** ----------------------------------------------------------------- */
static void convert_RGBtoCYMK(c)
c_Color *c;
{
 c->cmyk.k = MIN(XcC_MAX_COLOR_VALUE - c->rgb.r,
		 MIN(XcC_MAX_COLOR_VALUE - c->rgb.g,
		     XcC_MAX_COLOR_VALUE - c->rgb.b));
 c->cmyk.c = XcC_MAX_COLOR_VALUE - c->rgb.r - c->cmyk.k;
 c->cmyk.m = XcC_MAX_COLOR_VALUE - c->rgb.g - c->cmyk.k;
 c->cmyk.y = XcC_MAX_COLOR_VALUE - c->rgb.b - c->cmyk.k;
}


/* ----------------------------------------------------------------- ** 
** convert_RGBtoYCbCr - Convert RGB to YCbCr colorspace              ** 
** ----------------------------------------------------------------- */
static void convert_RGBtoYCbCr(c)
c_Color *c;
{
 c->ycbcr.y = (int)((0.2990 * (float)c->rgb.r)
		    + (0.5870 * (float)c->rgb.g)
		    + ( 0.1140 * (float)c->rgb.b) + .5);
  
 c->ycbcr.cb = ((int)((-0.1687 * (float)c->rgb.r)
		      - (0.3313 * (float)c->rgb.g)
		      + (0.5000 * (float)c->rgb.b) + .5)
		+ (XcC_MAX_COLOR_VALUE >> 1));
  
 c->ycbcr.cr = ((int)((0.5000 * (float)c->rgb.r) 
		      - (0.4187 * (float)c->rgb.g)
		      - (0.0813 * (float)c->rgb.b) + .5)
		+ (XcC_MAX_COLOR_VALUE >> 1));
}


/* ----------------------------------------------------------------- ** 
** convert_RGBtoHSL - Convert RGB to HSL colorspace                  ** 
** ----------------------------------------------------------------- */
static void convert_RGBtoHSL(c)
c_Color *c;
{
 float	  r = XcC_DESCALE_COLOR(float, c->rgb.r);
 float	  g = XcC_DESCALE_COLOR(float, c->rgb.g);
 float	  b = XcC_DESCALE_COLOR(float, c->rgb.b);
 float   max = MAX(r, MAX(g, b));
 float   min = MIN(r, MIN(g, b));
 float   h, delta;
  
 c->hsl.l = XcC_SCALE_COLOR(max);
 if (max != 0.0)
  c->hsl.s = XcC_SCALE_COLOR((max - min) / max);
 else
  c->hsl.s = XcC_SCALE_COLOR(0.0);
  
 if (c->hsl.s == 0)
 {
  c->hsl.h = XcC_SCALE_COLOR(0.0);
 }
 else
 {
  delta = max - min;
  if (r == max) 
   h = (g - b) / delta;
  else
   if (g == max)
    h = 2 + (b - r) / delta;
   else
    h = 4 + (r - g) / delta;

  if (h > 6.0)
   h = h - 6.0;
  if (h < 0)
   h = h + 6.0;
  c->hsl.h = XcC_SCALE_COLOR(h / 6.0);
 }
}


/* ----------------------------------------------------------------- ** 
** convert_CYMKtoRGB - Convert CYMK to RGB colorspace                ** 
** ----------------------------------------------------------------- */
static void convert_CYMKtoRGB(c)
c_Color *c;
{
 c->rgb.r = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE, c->cmyk.c + c->cmyk.k);
 c->rgb.g = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE, c->cmyk.m + c->cmyk.k);
 c->rgb.b = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE, c->cmyk.y + c->cmyk.k);
}


/* ----------------------------------------------------------------- ** 
** convert_YCbCrtoRGB - Convert YCbCr to RGB colorspace              ** 
** ----------------------------------------------------------------- */
static void convert_YCbCrtoRGB(c)
c_Color *c;
{
 c->rgb.r = ((float)c->ycbcr.y
	     + 1.4020 * (float)(c->ycbcr.cr - (XcC_MAX_COLOR_VALUE>>1)) + .5);
  
 c->rgb.g = ((float)c->ycbcr.y
	     - 0.3441 * (float)(c->ycbcr.cb - (XcC_MAX_COLOR_VALUE>>1))
	     - 0.7141 * (float)(c->ycbcr.cr - (XcC_MAX_COLOR_VALUE>>1)) + .5);
  
 c->rgb.b = ((float)c->ycbcr.y
	     + 1.7720 * (float)(c->ycbcr.cb - (XcC_MAX_COLOR_VALUE>>1)) + .5);
}


/* ----------------------------------------------------------------- ** 
** convert_HSLtoRGB - Convert HSL to RGB colorspace                  ** 
** ----------------------------------------------------------------- */
static void convert_HSLtoRGB(c)
c_Color *c;
{
 int     i;
 float	 h, s, l, r, g, b;
 float   f, p, q, t;
 

 h =  XcC_DESCALE_COLOR(float, c->hsl.h);
 s =  XcC_DESCALE_COLOR(float, c->hsl.s);
 l =  XcC_DESCALE_COLOR(float, c->hsl.l);
 
 if (h >= 1.0) h -= 1.0;

 if (s == 0.0 && h == 0.0)
 {
  c->rgb.r = c->rgb.g = c->rgb.b = XcC_SCALE_COLOR(l);
 } 
 else
 {
  h = h * 6.0;
  
  i = (int)h;
  f = h - i;
  p = l * (1.0 - s);
  q = l * (1.0 - (s * f));
  t = l * (1.0 - (s * (1.0 - f)));
  switch (i)
  {
  case 0: r = l; g = t; b = p; break;
  case 1: r = q; g = l; b = p; break;
  case 2: r = p; g = l; b = t; break;
  case 3: r = p; g = q; b = l; break;
  case 4: r = t; g = p; b = l; break;
  default:
  case 5: r = l; g = p; b = q; break;
  }
  c->rgb.r = XcC_SCALE_COLOR(r);
  c->rgb.g = XcC_SCALE_COLOR(g);
  c->rgb.b = XcC_SCALE_COLOR(b);
 }
}

/* ----------------------------------------------------------------- ** 
** deleteColorBase                                                   ** 
** ----------------------------------------------------------------- */
static void deleteColorBase(color_base)
c_Color	*color_base;
{
 c_Color	*color_List;
  
 if (color_base->previous != NULL)
  Xc_WARNING(("Probably not the top of color base"));
 color_List = color_base;
 while (color_List)
 {
  DELETE(c_Color)(color_List);
  color_List = color_base->next;
  color_base = color_List;
 }
}

/* ----------------------------------------------------------------- ** 
** copyColorBase                                                     ** 
** ----------------------------------------------------------------- */
static void copyColorBase(color_base, new_color_base)
c_Color	*color_base;
c_Color	**new_color_base;
{
 c_Color	*color_List;
  
 if (color_base->previous != NULL)
  Xc_WARNING(("Probably not the top of source color base"));
 color_List = color_base;
 while (color_List)
 {
  COPY(c_Color)(color_List, new_color_base);
  color_List = color_base->next;
  color_base = color_List;
 }
}


/* ----------------------------------------------------------------- ** 
** setGC - Set the X11 GraphicContext from a color class             ** 
** ----------------------------------------------------------------- */
static void setGC(this, display, gc)
c_Color *this;
Display *display;
GC gc;
{
 Xc_HISTORY(("setGC(`%s')", this->name));

#ifndef NTRACE
 if(this->transparency)
  Xc_TRACE(("transparency"));
 else
#else
 if(!this->transparency)
#endif
  XSetForeground(display, gc, this->cell.pixel);
}


/* ----------------------------------------------------------------- ** 
** ps_print - Print a color                                          ** 
** ----------------------------------------------------------------- */
static boolean ps_print(this, post, stroke_flag)
c_Color *this;
c_PostScript *post;
boolean stroke_flag;
{
 Xc_HISTORY(("ps_print(`%s')", this->name));
 
 if(stroke_flag == TRUE)
 {
  if(F(this).compare(this, post->state.stroke_color, CMP_WITH_NAME))
   return TRUE;
  post->state.stroke_color = this;
 }
 else
 {
  if(F(this).compare(this, post->state.fill_color, CMP_WITH_NAME))
   return TRUE;
  post->state.fill_color = this;
 }
 
 if((this->mode == XcC_GRAY) ||
    ((post->ai_mode == FALSE) && (post->print_param->Color == FALSE)))
 {
  Xc_TRACE(("gray"));
  return F(post).putLine(post, stroke_flag? "$f G" : "$f g",
			 XcC_DESCALE_COLOR(double, this->ycbcr.y));
 }
 else if((this->mode == XcC_RGB) && (post->ai_mode == FALSE))
 {
  Xc_TRACE(("RGB colorspace"));
  F(post).addColor(post, this);
  return F(post).putLine(post, stroke_flag? "$f $f $f RG" : "$f $f $f rg",
			 XcC_DESCALE_COLOR(double, this->rgb.r),
			 XcC_DESCALE_COLOR(double, this->rgb.g),
			 XcC_DESCALE_COLOR(double, this->rgb.b));
 }
 else
 {
  Xc_TRACE(("CMYK colorspace"));
  F(post).addColor(post, this);
  return F(post).putLine(post,
			 stroke_flag? "$f $f $f $f K" : "$f $f $f $f k",
			 XcC_DESCALE_COLOR(double, this->cmyk.c),
			 XcC_DESCALE_COLOR(double, this->cmyk.m),
			 XcC_DESCALE_COLOR(double, this->cmyk.y),
			 XcC_DESCALE_COLOR(double, this->cmyk.k));
 }
}

/* ----------------------------------------------------------------- ** 
** ps_print2 - Print a color                                         ** 
** ----------------------------------------------------------------- */
static boolean ps_print2(this, post, type, stroke_flag)
c_Color *this;
c_PostScript *post;
int	type;
boolean stroke_flag;
{
 Xc_HISTORY(("ps_print(`%s')", this->name));

 if(stroke_flag == TRUE)
 {
  if(F(this).compare(this, post->state2[type].stroke_color, CMP_WITH_NAME))
   return TRUE;
  post->state2[type].stroke_color = this;
 }
 else
 {
  if(F(this).compare(this, post->state2[type].fill_color, CMP_WITH_NAME))
   return TRUE;
  post->state2[type].fill_color = this;
 }

 if((this->mode == XcC_GRAY) ||
    ((post->ai_mode == FALSE) && (post->print_param->Color == FALSE)))
 {
  Xc_TRACE(("gray"));
  return F(post).putLine2(post, type, stroke_flag? "$f G" : "$f g",
			  XcC_DESCALE_COLOR(double, this->ycbcr.y));
 }
 else if((this->mode == XcC_RGB) && (post->ai_mode == FALSE))
 {
  Xc_TRACE(("RGB colorspace"));
  F(post).addColor(post, this);
  return F(post).putLine2(post, type, 
			  stroke_flag? "$f $f $f RG" : "$f $f $f rg",
			  XcC_DESCALE_COLOR(double, this->rgb.r),
			  XcC_DESCALE_COLOR(double, this->rgb.g),
			  XcC_DESCALE_COLOR(double, this->rgb.b));
 }
 else
 {
  Xc_TRACE(("CMYK colorspace"));
  F(post).addColor(post, this);
  return F(post).putLine2(post, type,
			  stroke_flag? "$f $f $f $f K" : "$f $f $f $f k",
			  XcC_DESCALE_COLOR(double, this->cmyk.c),
			  XcC_DESCALE_COLOR(double, this->cmyk.m),
			  XcC_DESCALE_COLOR(double, this->cmyk.y),
			  XcC_DESCALE_COLOR(double, this->cmyk.k));
 }
}




