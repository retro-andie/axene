/*
** XFIGDocFile.h for Xclamation and XAllWrite in XFIGDocFile/
** Declaration of the XFIGDocFile class
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
** Started on  Mon Apr 28 16:42:57 1997 Antoine Buat
** Last update Wed May 21 19:14:29 1997 Antoine Buat
*/

#ifndef __XC_XFIG_DOCUMENT_FILE_H__
#define __XC_XFIG_DOCUMENT_FILE_H__

/*--- Forward declarations ---*/
typedef struct sc_XFIGDocFile c_XFIGDocFile;

/*--- include ---*/
#include "xcalibur.h"
#include "VectorGraph.h"
#include "VectorPolygon.h"

typedef struct _arrow
{
 int	type;
 int	style;
 float	thickness;
 float 	width;
 float	height;
} arrow;

typedef struct s_spoint
{
 int			x;
 int			y;
 struct s_spoint	*next;
} t_spoint;

typedef struct s_scontrol 
{
 float			lx;
 float			ly;
 float			rx;
 float			ry;
 struct s_scontrol	*next;
} t_scontrol;

/*--- define all possible object ---*/
typedef struct
{
 int		num;
 int		r;
 int		g;
 int		b;
} fig_color;

typedef struct
{
 int		type;
 int		depth;
 int		sub_type;
 int		line_style;
 int		thickness;
 int		pen_color;
 int		fill_color;
 int		pen_style;
 int		area_fill;
 float		style_val;
 int		cap_style;
 int		direction;
 int		forward_arrow;
 int		backward_arrow;
 float		center_x, center_y;
 int		x1, y1;
 int		x2, y2;
 int		x3, y3;
 arrow		fore_arrow;
 arrow		back_arrow;
} fig_arc;

typedef struct
{
 int		type;
 int		depth;
 int		sub_type;
 int		line_style;
 int		thickness;
 int		pen_color;
 int		fill_color;
 int		pen_style;
 int		area_fill;
 float		style_val;
 int		direction;
 float		angle;
 int		center_x, center_y;
 int		radius_x, radius_y;
 int		start_x, start_y;
 int		end_x, end_y;
} fig_ellipse;

typedef struct
{
 int		type;
 int		depth;
 int		sub_type;
 int		line_style;
 int		thickness;
 int		pen_color;
 int		fill_color;
 int		pen_style;
 int		area_fill;
 float		style_val;
 int		join_style;
 int		cap_style;
 int		radius;
 int		forward_arrow;
 int		backward_arrow;
 int		npoints;
 arrow		fore_arrow;
 arrow		back_arrow;
 char		*pic_name;
 boolean	flipped;
 t_spoint	*points;
} fig_polyline;

typedef struct
{
 int		type;
 int		depth;
 int		sub_type;
 int		line_style;
 int		thickness;
 int		pen_color;
 int		fill_color;
 int		pen_style;
 int		area_fill;
 float		style_val;
 int		cap_style;
 int		forward_arrow;
 int		backward_arrow;
 int		npoints;
 arrow		fore_arrow;
 arrow		back_arrow;
 t_spoint	*points;
 t_scontrol	*controls;
} fig_spline;

typedef struct
{
 int		type;
 int		depth;
 int		sub_type;
 int		color;
 int		pen_style;
 int		font;
 float		font_size;
 float		angle;
 int		font_flags;
 float		height;
 float		length;
 int		x, y;
 char		*string;
 int		str_len;
} fig_text;

typedef union
{
 int		type;
 fig_arc	arc;
 fig_ellipse	ellipse;
 fig_polyline	polyline;
 fig_spline	spline;
 fig_text	text;
} fig_obj;

typedef struct _obj_list
{
 fig_obj		*object;
 struct _obj_list	*Next;
 struct _obj_list	*Prev;
} obj_list;

/*--- Methods ---*/
typedef struct
{
 F_STD;
 boolean (*readMain) ___PROTO((c_XFIGDocFile *this));
} sf_XFIGDocFile;

/*--- class declaration ---*/
struct sc_XFIGDocFile
{
 sf_XFIGDocFile	*f;

 /*___File info___*/
 char		*filename;
 FILE		*fd;
 boolean	close_flag;

 /*___Memory struct___*/
 c_VectorGraph	*vector;
 
 /*___Color___*/
 c_Color	**color_base;

 /*___Internals___*/
 int		version;
 int		factor;		/* number of unit per inch */
 int		factor80;	/* 80 unit per inch */
 int		neg_y;

 obj_list	*list;
 obj_list	*last_obj;	/* pointer sur la position a inserer */
 obj_list	**pins_obj;	/* pointer sur la position a inserer */
 /*___ only for 3.0 & > ___*/
 int		orientation;	/* landscape:0 portrait:1 */
 int		justification;	/* center:0 left:1 */
 int		units;		/* metric:0 inches:1 */

 fig_color	*user_col;
 int		num_col;
};

extern sf_XFIGDocFile fc_XFIGDocFile;

#endif /* !__XC_XFIG_DOCUMENT_FILE_H__ */
