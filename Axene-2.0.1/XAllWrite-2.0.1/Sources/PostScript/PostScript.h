/*
** PostScript.h for Xclamation, XQuad and XAllWrite in PostScript/
** Object and methods definition for PostScript
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
** Started on  Fri Nov  4 14:27:07 1994 Antoine Buat
** Last update Thu Jun  3 15:25:41 1999 Emmanuel Paris
*/

#ifndef __XC_POSTSCRIPT_H__
#define __XC_POSTSCRIPT_H__

typedef struct sc_PostScript c_PostScript;
typedef struct printer_s printer_t;

#include "xcalibur.h"
#include "vectors.h"
#include "Matrix.h"
#include "VectorFont.h"
#include "Color.h"
#include "PrintParam.h"

/*--- Macros ---*/
#define XcPS_BYTES_PER_LINE 39
#define XcPS_MAX_LINE_DASHES 8

#define SHADOW_TEXT	0
#define SHADOW_GRAPH	1
#define NORMAL_TEXT	2
#define NORMAL_GRAPH	3

#define GRAPH		1

/*--- Color spaces ---*/
typedef enum
{
 XcPS_DEVICE_GRAY, XcPS_DEVICE_RGB, XcPS_DEVICE_CMYK, XcPS_INDEXED
} ps_colorspace_t;


/*--- Resources ---*/
typedef enum
{
 XcPS_FONT_RESOURCE, XcPS_ENCODING_RESOURCE,
 XcPS_FORM_RESOURCE, XcPS_PATTERN_RESOURCE,
 XcPS_PROC_SET_RESOURCE, XcPS_COLOR_SPACE_RESOURCE,
 XcPS_HALFTONE_RESOURCE, XcPS_COLOR_RENDERING_RESOURCE
} ps_resource_t;


/*--- Structures ---*/
typedef struct ps_image_s ps_image_t;
struct ps_image_s
{
 boolean (*cb_func) ___PROTO((unsigned char **pbuffer, ps_image_t *cb_data));
 boolean (*filter) ___PROTO((ps_image_t *handle, int len));
 c_PostScript *this;
 int width;
 int height;
 int depth;
 int byte_depth;
 unsigned long line_len;
 unsigned char *line_buffer;
 unsigned char output_buffer[(XcPS_BYTES_PER_LINE * 2) + 1];
 int output_index;
 unsigned short *red;
 unsigned short *green;
 unsigned short *blue;
 unsigned char gray[256];
};


/*--- Methods ---*/
typedef struct
{
 F_STD;

 boolean (*write) ___PROTO((c_PostScript *this, void *ptr, long len));
 boolean (*putLine) ___NPROTO((c_PostScript *this, char *format, ...));
 boolean (*putString) ___PROTO((c_PostScript *this, char_t *start, long len));

 boolean (*addFont) ___PROTO((c_PostScript *This, c_VectorFont *font));
 boolean (*addColor) ___PROTO((c_PostScript *this, c_Color *color));

 boolean (*beginPage) ___PROTO((c_PostScript *this, unsigned int number));
 boolean (*endPage) ___PROTO((c_PostScript *this));
  
 /*___Image methods___*/
 ps_image_t *(*beginImage) ___PROTO((c_PostScript *this,
				     int width, int height, int depth,
				     unsigned short *red,
				     unsigned short *green,
				     unsigned short *blue));
 boolean (*endImage) ___PROTO((c_PostScript *this, ps_image_t *handle));

 /*___File insertion___*/
 boolean (*includeFile) ___PROTO((c_PostScript *this, char *filename));
 boolean (*includeResource) ___NPROTO((c_PostScript *this,
				       ps_resource_t type, ...));

 /*___State methods___*/
 boolean (*pushState) ___PROTO((c_PostScript *this));
 boolean (*popState) ___PROTO((c_PostScript *this));
 boolean (*putLine2) ___NPROTO((c_PostScript *this, int type, 
				char *format, ...));
 boolean (*deferredWrite) ___PROTO((c_PostScript *this, int type));
 boolean (*putString2) ___NPROTO((c_PostScript *this, int type, 
				  char_t *start, long len));
} sf_PostScript;


/*--- Font list structure ---*/
typedef struct postscript_font_s postscript_font_t;
struct postscript_font_s
{
 postscript_font_t *next;
 c_VectorFont *font;
 boolean page_flag;
};


/*--- Color list structure ---*/
typedef struct postscript_color_s postscript_color_t;
struct postscript_color_s
{
 postscript_color_t *next;
 c_Color *color;
 boolean page_flag;
};


/*--- Printer list ---*/
struct printer_s
{
 printer_t *next;
 char *name;
 char *command;
};


/*--- Graphic state structure ---*/
typedef struct postscript_state_s postscript_state_t;
struct postscript_state_s
{
 /*--- Color state ---*/
 c_Color *stroke_color;
 c_Color *fill_color;

 /*___May be saved___*/
 coord_t line_width;
 int line_cap;
 int line_join;
 int flatness;
 real miter_limit;
 coord_t dash_phase;
 int dash_len;			/* must be less than XcPS_MAX_LINE_DASHES */
 coord_t dash[XcPS_MAX_LINE_DASHES];

 /*___Text state___*/
 c_VectorFont *font;
 coord_t point_size;
 coord_t leading;
 int render_mode;
 coord_t text_rise;
 coord_t word_space;
 coord_t char_space;
 scale_t horizontal_scale;
 boolean text_flag;
 boolean clip_flag;

 /*___Link___*/
 postscript_state_t *next;
};


/*--- Class definition ---*/
struct sc_PostScript
{
 sf_PostScript	*f;
  
 /*___File stuff___*/
 char *filename;
 int fd;
 boolean close_flag;
  
 /*___Document stuff___*/
 unsigned int page_count;
 bbox_t doc_bbox;
 postscript_font_t *font_list;
 postscript_color_t *color_list;
  
 /*___Page stuff___*/
 boolean page_flag;
 unsigned int page_number;
 bbox_t page_bbox;
  
 /*___Printing preferences___*/
 print_param_t *print_param;
  
 /*___User stuff___*/
 c_Matrix *matrix;
  
 ItemHandle *item;

 /*___Error flags___*/
 boolean error_flag;
 postscript_state_t state;
 boolean segment_flag;
 boolean ai_mode;

 /*___Debug stuff___*/
 matrix_t *page_matrix;

 /*___Deffered write pointer___*/
 postscript_state_t state2[4];
 char	*shadow_text;
 long	shadow_text_len;
 char	*shadow_graph;
 long	shadow_graph_len;
 char	*normal_text;
 long	normal_text_len;
 char	*normal_graph;
 long	normal_graph_len;
};

extern sf_PostScript fc_PostScript;

#endif /* ! __XC_POSTSCRIPT_H__ */


