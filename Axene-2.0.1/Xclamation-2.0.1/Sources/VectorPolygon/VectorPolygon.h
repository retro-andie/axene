/*
** VectorPolygon.h for Xclamation in VectorPolygon/
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
** Started on  Tue Jul 18 12:55:41 1995 Stéphane Boisson
** Last update Thu Oct 24 19:20:49 1996 Emmanuel Paris
*/

#ifndef _VectorPolygon_h
#define _VectorPolygon_h

/*--- Forward declarations ---*/
typedef struct sc_VectorPolygon c_VectorPolygon;

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "Cadre.h"
#include "Matrix.h"
#include "CadreStd.h"
#include "Color.h"
#include "Hook.h"

/*--- Macros ---*/
#define XcVP_XPOINTS 30000
#define XcVP_POINT_CHUNK_SIZE 16

#define XcVP_FILL	(1<<0)
#define XcVP_STROKE	(1<<1)
#define XcVP_CLIP	(1<<2)

typedef enum {
  XcVP_END,
  XcVP_STROKE_COLOR, XcVP_FILL_COLOR,
  XcVP_LINE_DASH, XcVP_FLATNESS, XcVP_CAP_STYLE, XcVP_JOIN_STYLE,
  XcVP_MITER_LIMIT, XcVP_LINE_WIDTH
} polygon_set_code_t;


/*--- Internal types ---*/
typedef enum {
  XcVP_CONTROL,
  XcVP_MOVE,      	/* moveto    */
  XcVP_LINE,		/* lineto    */
  XcVP_CURVE,		/* curveto   */
  XcVP_CLOSEPATH,	/* closepath */
  XcVP_RENDER
} poly_mode_t;

typedef enum{XcVP_BUTT_CAP, XcVP_ROUND_CAP, XcVP_SQUARE_CAP} poly_cap_t;
typedef enum{XcVP_MITER_JOIN, XcVP_ROUND_JOIN, XcVP_BELEVEL_JOIN} poly_join_t;

typedef struct poly_chunk_s poly_chunk_t;

struct poly_chunk_s {
  poly_chunk_t	*next;
  coord_t	x[XcVP_POINT_CHUNK_SIZE];
  coord_t	y[XcVP_POINT_CHUNK_SIZE];
  poly_mode_t	type[XcVP_POINT_CHUNK_SIZE];
};


/*--- Method declarations ---*/
typedef struct
{
  F_STD;

  /*___display functions___*/
  error (*change_base) ___PROTO((c_VectorPolygon *this, BaseStd_t *base));
  error (*display_init) ___PROTO((c_VectorPolygon *this,
				  c_Matrix *matrix, scale_t scale));
  void (*display_redraw) ___PROTO((c_VectorPolygon *this,
				   c_Cadre *cadre, Polygone *p));

  /*___Printing___*/
  void (*ps_print) ___PROTO((c_VectorPolygon *this, c_PostScript *post));

  /*___Save & Load___*/
  boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			    long param, c_Cadre *frame));
  boolean (*write) ___PROTO((c_VectorPolygon *this, c_DocFile *doc));
  
  /*___Drawing methods___*/
  boolean (*moveto) ___PROTO((c_VectorPolygon *this, coord_t x, coord_t y));
  boolean (*lineto) ___PROTO((c_VectorPolygon *this, coord_t x, coord_t y));
  boolean (*curveto) ___PROTO((c_VectorPolygon *this, 
			       coord_t x, coord_t y,
			       coord_t x2, coord_t y2,
			       coord_t x3, coord_t y3));
  boolean (*closepath) ___PROTO((c_VectorPolygon *this));
  boolean (*stroke) ___PROTO((c_VectorPolygon *this));
  boolean (*fill) ___PROTO((c_VectorPolygon *this));
  boolean (*clip) ___PROTO((c_VectorPolygon *this));

  boolean (*set) ___NPROTO((c_VectorPolygon *this, ...));
  void (*getCurrentPoint) ___PROTO((c_VectorPolygon *this,
				    coord_t *xp, coord_t *yp));
} sf_VectorPolygon;


/*--- Class definition ---*/
struct sc_VectorPolygon {
  sf_VectorPolygon	*f;

  c_Hook	*hook;

  /*___Data___*/
  bbox_t	bbox;
  poly_chunk_t	*start;
  poly_chunk_t	*end;
  boolean	end_flag;
  boolean	first_flag;
  vector_t	first_point;
  vector_t	last_point;
  unsigned long count;

  int		draw_type;
  c_Color	*stroke_color;
  c_Color	*fill_color;

  coord_t	line_width;

  /*___Dash___*/
  coord_t	phase;
  unsigned long	dash_count;
  coord_t	*dash;

  coord_t	flatness;
  poly_cap_t	cap;
  poly_join_t	join;
  coord_t	miter_limit;

  /*___X11 cache___*/
  XPoint	*cache;
  unsigned long	cache_count;
  unsigned int	cache_width;
};

extern sf_VectorPolygon fc_VectorPolygon;

#endif
