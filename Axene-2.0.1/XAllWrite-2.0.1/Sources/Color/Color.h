/*
** Color.h for Xclamation, XQuad and XAllWrite in Color/
** Definition of the Color class
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
** Started on  Sun Jun 12 02:15:48 1994 Stéphane Boisson
** Last update Wed Jan 21 00:19:27 1998 Emmanuel Paris
*/

#ifndef __XC_COLOR_H__
#define __XC_COLOR_H__

typedef struct sc_Color c_Color;

#include "xcalibur.h"
#include "Hook.h"
#include "Colormap.h"
#include "DocFile.h"
#include "PostScript.h"
#include "BaseStd.h"

#define XcDF_COLOR_KEYWORD "COLOR"
#define XcDF_COLORLIST_KEYWORD "COLORLIST"
#define XcDF_TRANSPARENCY_KEYWORD "TRANSPARENCY"
#define XcDF_RGB_KEYWORD "RGB"
#define XcDF_CMYK_KEYWORD "CMYK"
#define XcDF_YCBCR_KEYWORD "YCBCR"
#define XcDF_HSL_KEYWORD "HSL"
#define XcDF_GRAY_KEYWORD "GRAY"
#ifndef XcDF_LOCK_KEYWORD
#define XcDF_LOCK_KEYWORD "LOCK"
#endif

typedef enum
{
 XcC_RGB, XcC_CMYK, XcC_YCbCr, XcC_HSL, XcC_GRAY,
 XcC_TRANSPARENCY, XcC_NAME, XcC_END, XcC_LOCK,
 XcC_HIDE_ON, XcC_HIDE_OFF
} color_set_code_t;

#define XcC_MAX_COLOR_BITS 13
#define XcC_MAX_COLOR_VALUE ((1 << XcC_MAX_COLOR_BITS)- 1)
#define XcC_SCALE_COLOR(value) \
	(unsigned int)ROUNDFLOAT((float)(value) * (float)XcC_MAX_COLOR_VALUE)
#define XcC_DESCALE_COLOR(type, value) \
        ((type)(value) / (type)XcC_MAX_COLOR_VALUE)
#define XcC_PERCENT_COLOR(value) \
        (float)((((float)(value) * 100.0)) / (float)XcC_MAX_COLOR_VALUE)
#define XcC_COLOR_PERCENT(value) \
	(int)ROUNDFLOAT(((float)(value) * (float)XcC_MAX_COLOR_VALUE) / 100.0)
#define XcC_BYTEVALUE_COLOR(value) ((value) >> (XcC_MAX_COLOR_BITS - 8))
#define XcC_SHORTVALUE_COLOR(value) ((value) << (16 - XcC_MAX_COLOR_BITS))

typedef struct
{
 int r;
 int g;
 int b;
} rgb_t;

typedef struct
{
 int y;
 int cb;
 int cr;
} ycbcr_t;

typedef struct
{
 int c;
 int m;
 int y;
 int k;
} cmyk_t;

typedef struct
{
 int h;
 int s;
 int l;
} hsl_t;


/*--- Define the methods for the Color Class ---*/
typedef struct
{
 F_STD;

 /*___Class control___*/
 void (*set) ___PROTO((c_Color *class, ...));
 boolean (*compare) ___PROTO((c_Color *a, c_Color *b, boolean with_name));
 void (*merge) ___PROTO((c_Color *source, c_Color **target, boolean fusion));
 void (*deleteBase) ___PROTO((c_Color *color_base));
 void (*copyBase) ___PROTO((c_Color *color_base, c_Color **new_color_base));
 c_Color *(*getColor) ___NPROTO((c_Color **base, ...));
 c_Color *(*virtualCopy) ___PROTO((c_Color *color, BaseStd_t *base));

 /*___Loading & saving___*/
 boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			   long param, c_Color **base));
 boolean (*write) ___PROTO((c_Color *class, c_DocFile *doc));
 boolean (*readList) ___PROTO((c_DocFile *doc, char *keyword,
			       long param, c_Color **base));
 boolean (*writeList) ___PROTO((c_Color *class, c_DocFile *doc));
  
 /*___Information requests___*/
 rgb_t *(*getRGB) ___PROTO((c_Color *class));
 cmyk_t *(*getCMYK) ___PROTO((c_Color *class));
 ycbcr_t *(*getYCbCr) ___PROTO((c_Color *class));
 hsl_t *(*getHSL) ___PROTO((c_Color *class));

 void (*setGC) ___PROTO((c_Color *this, Display *display, GC gc));
 boolean (*ps_print) ___PROTO((c_Color *this,
			       c_PostScript *post, boolean stroke_flag));
 boolean (*ps_print2) ___PROTO((c_Color *this, c_PostScript *post,
				int type, boolean stroke_flag));
} sf_Color;


/*--- Define the Color Class ---*/
struct sc_Color
{
 c_Color *previous;
 c_Color *next;
 c_Color **base;

 sf_Color *f;

 char name[XcMAX_NAME_LENGTH];	/* color name                  */
 long id;			/* color ID                    */
 color_set_code_t mode;
 unsigned char transparency;	/* 0: opaque -> 255: invisible */
 rgb_t rgb;			/* RGB colorspace              */
 ycbcr_t ycbcr;			/* YCbCr colorspace            */
 cmyk_t cmyk;			/* CMYK colorspace             */
 hsl_t hsl;			/* HSL colorspace              */

 /*___Colormap entry___*/
 color_cells_t cell;

 /*___Hook___*/
 c_Hook *hook;

 /*___Flags___*/
 boolean lock_flag;
 boolean hide_flag;
};

extern sf_Color fc_Color;

#endif /* !__XC_COLOR_H__ */




