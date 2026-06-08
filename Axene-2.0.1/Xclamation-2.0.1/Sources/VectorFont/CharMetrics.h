/*
** CharMetrics.h for Xclamation, XQuad and XAllWrite in VectorFont/
** Definitions for the CharMetrics class
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
** Started on  Fri Jul  8 17:33:41 1994 Stéphane Boisson
** Last update Thu Apr 23 11:58:45 1998 Emmanuel Paris
*/

#ifndef __CharMetrics_h__
#define __CharMetrics_h__

/*--- Forward declarations ---*/
struct sfs_CharMetrics;
typedef struct sc_CharMetrics c_CharMetrics;

#include "RasterFont.h"
#include "xcalibur.h"
#include "vectors.h"
#include "Matrix.h"
#include "Hook.h"

#define XcCM_B_NONE	  0
#define XcCM_B_OUTLINE	  1
#define XcCM_B_SMALL_CAPS 2
#define XcCM_B_BIG_CAPS   4

/*--- Track Kerning Information ---*/
typedef struct 
{
  long degree;
  coord_t min_point_size;
  coord_t min_kern;
  coord_t max_point_size;
  coord_t max_kern;
} track_kern_t;

/*--- Ligature ---*/
typedef struct 
{
  char_t succ;
  char_t ligature;
} ligature_t;
  
/*--- Kern pair ---*/
typedef struct 
{
  char_t succ;
  vector_t offset;
} kern_pair_t;

/*--- Composite char part ---*/
typedef struct 
{
  char_t code;
  vector_t offset;
} composite_part_t;

/*--- Simple metrics ---*/
typedef struct 
{
  char *encoding_name;
  vector_t width[2];
  bbox_t char_bbox;
} simple_char_metrics_t;

/*--- Complex metrics ---*/
typedef struct 
{
  char *encoding_name;
  vector_t vv;
  vector_t width[2];
  bbox_t char_bbox;
  int composite_parts_count;
  int kern_pairs_count;
  int ligature_count;
} complex_char_metrics_t;

/*--- Get the Composite Part Array in complex metrics structure ---*/
#define XcCM_COMPOSITE_PARTS(complex_ptr) \
((composite_part_t *)(((complex_char_metrics_t *)(complex_ptr)) + 1))

/*--- Get the Kern pair Array in complex metrics structure ---*/
#define XcCM_KERN_PAIRS(complex_ptr) \
((kern_pair_t *)(XcCM_COMPOSITE_PARTS(complex_ptr) + \
		 (complex_ptr)->composite_parts_count))

/*--- Get the Ligature Array in complex metrics structure ---*/
#define XcCM_LIGATURES(complex_ptr) \
((ligature_t *)(XcCM_KERN_PAIRS(complex_ptr) + \
		 (complex_ptr)->kern_pairs_count))

/*--- Get the real size of the complex metric structure ---*/
#define XcCM_SIZEOF(complex_ptr) \
(sizeof(complex_char_metrics_t) + \
 ((complex_ptr)->composite_parts_count * sizeof(composite_part_t)) + \
 ((complex_ptr)->kern_pairs_count * sizeof(kern_pair_t)) + \
 ((complex_ptr)->ligature_count * sizeof(ligature_t)))


/*--- Define the CharMetrics class ---*/
struct sc_CharMetrics 
{
  struct sc_CharMetrics *previous;
  struct sc_CharMetrics *next;

  struct sfs_CharMetrics *f;
  boolean pure;
  unsigned int counter;

  /*___Global Font Metrics___*/
  coord_t point_size;
  angle_t angle;
  scale_t scale;
  scale_t width;
  bbox_t font_bbox;
  vector_t vvector;
  boolean is_fixed_v;
  coord_t cap_height;
  coord_t x_height;
  coord_t ascender;
  coord_t descender;
  matrix_t matrix;
  matrix_t final_matrix;
  matrix_t final_matrix2;

  /*___Control Information___*/
  SHORT metrics_sets; /* indicate writting directions */
  int	  attributes; /* XcCM_B_XXX attributes */
  
  /*___Writing Direction Metrics___*/
  coord_t underline_position[2];
  coord_t underline_thickness[2];
  angle_t italic_angle[2];
  vector_t char_width[2];
  boolean is_fixed_pitch[2];

  /*___Track Kerning___*/
  int track_kern_count;
  track_kern_t *track_kern;

  /*___Encoded chars___*/
  simple_char_metrics_t *encoded[Xc_CHAR_TOTAL];

  /*___Normal and Composites chars___*/
  complex_char_metrics_t *glyph[Xc_CHAR_TOTAL];

  /*___Rasterized info___*/
  c_RasterFont *raster_font;
  coord_t font_width;

  /*___Packed informations___*/
  /*___Hook___*/
  c_Hook *hook;

  ItemHandle *pack_handle;
  unsigned long pack_len;
};


/*--- Define the methods for the CharMetrics Class ---*/
typedef struct sfs_CharMetrics 
{
  F_STD;
				 
  c_CharMetrics *(*transform) ___PROTO((c_CharMetrics *this,
					scale_t scale,
					scale_t width, 
					angle_t angle));
  void (*purgeImpure) ___PROTO((c_CharMetrics *this,
				scale_t scale,
				scale_t width, 
				angle_t angle));

  /*___Creation methods___*/
  void (*addEncodedChar) ___PROTO((c_CharMetrics *obj,
				   char_t code,
				   char *encoding_name,
				   vector_t *width,
				   bbox_t *char_bbox));
  void (*addChar) ___PROTO((c_CharMetrics *obj,
			    char_t code,
			    char *encoding_name,
			    vector_t *width,
			    bbox_t *char_bbox,
			    vector_t *vv));
  void (*addTrackKern) ___PROTO((c_CharMetrics *obj,
				 track_kern_t *data));
  void (*addLigature) ___PROTO((c_CharMetrics *obj,
				char_t code,
				ligature_t *data));
  void (*addKernPair) ___PROTO((c_CharMetrics *obj,
				char_t code,
				kern_pair_t *data));
  void (*addCompositePart) ___PROTO((c_CharMetrics *obj,
				     char_t code,
				     composite_part_t *data));

  /*___Structure changes___*/
  c_CharMetrics *(*pack) ___PROTO((c_CharMetrics *this));

  /*___Misc___*/
  coord_t (*textWidth) ___PROTO((c_CharMetrics *this,
				 char_t *text, long len));
  coord_t (*textHeight) ___PROTO((c_CharMetrics *this,
				  char_t *text, long len));
  boolean (*compare) ___PROTO((c_CharMetrics *metrics1,
			       c_CharMetrics *metrics2));

  long (*calibrate) ___PROTO((c_CharMetrics *this, coord_t width,
			      char **textp, char *end));
  
  c_CharMetrics *(*makefont) ___PROTO((c_CharMetrics *this, matrix_t *matrix,
				       coord_t size, scale_t width));
  void (*unmakefont) ___PROTO((c_CharMetrics *this, matrix_t *matrix,
			       coord_t size, scale_t width));

  /*___Lock function___*/
  boolean (*lock) ___PROTO((c_CharMetrics *this, void *data));
  void (*unlock) ___PROTO((c_CharMetrics *this, void *data));
  boolean (*isLocked) ___PROTO((c_CharMetrics *this, void *data));

  c_CharMetrics *(*findTransform) ___PROTO((c_CharMetrics *this,
					    scale_t scale,
					    scale_t width, 
					    angle_t angle));

  void (*fast_copy) ___PROTO((c_CharMetrics *this, c_CharMetrics *dest));
  void (*fast_free) ___PROTO((c_CharMetrics *this));
  
  coord_t (*textWidthSpec) ___PROTO((c_CharMetrics *this, char_t *text,
				     long len, coord_t char_inc,
				     coord_t word_inc));
} sf_CharMetrics;

extern sf_CharMetrics fc_CharMetrics;

#endif /* !__CharMetrics_h__ */
