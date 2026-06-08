/*
** WMFDocFile.h for Xclamation in WMFDocFile/
** Declaration of the WMFDocFile class
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
** Last update Thu May 15 22:30:07 1997 Antoine Buat
*/

#ifndef __XC_WMF_DOCUMENT_FILE_H__
#define __XC_WMF_DOCUMENT_FILE_H__

/*--- Forward declarations ---*/
typedef struct sc_WMFDocFile c_WMFDocFile;

/*--- include ---*/
#include "xcalibur.h"
#include "VectorGraph.h"
#include "VectorPolygon.h"


/*--- define ---*/
#define M_OBJ_VOID	0
#define M_OBJ_PEN	1
#define M_OBJ_BRUSH	2
#define M_OBJ_BITMAP	3
#define M_OBJ_FONT	4
#define M_OBJ_REGION	5
#define M_OBJ_OTHER	6

/* define basic pen style */
#define PS_SOLID            0
#define PS_DASH             1       /* -------  */
#define PS_DOT              2       /* .......  */
#define PS_DASHDOT          3       /* _._._._  */
#define PS_DASHDOTDOT       4       /* _.._.._  */
#define PS_NULL             5
#define PS_INSIDEFRAME      6
#define PS_USERSTYLE        7
#define PS_ALTERNATE        8

/* define basic Brush Styles */
#define BS_SOLID            0
#define BS_NULL             1
#define BS_HOLLOW           BS_NULL
#define BS_HATCHED          2
#define BS_PATTERN          3
#define BS_INDEXED          4
#define BS_DIBPATTERN       5
#define BS_DIBPATTERNPT     6
#define BS_PATTERN8X8       7
#define BS_DIBPATTERN8X8    8

/*--- local struct ---*/
typedef struct s_meta_clip_zone
{
 int				Left;
 int				Top;
 int				Right;
 int				Bottom;
 struct s_meta_clip_zone	*Next;
} MClipZone;

typedef struct s_meta_obj_pen
{
  int	type;
  int	style;
  long	color;
  int	width;
} MObjPen;

typedef struct s_meta_obj_brush
{
  int	type;
  int	style;
  long	color;
  int	hatch;
} MObjBrush;

typedef struct s_meta_obj_font
{
  int	type;
  int	size;
  c_VectorFont *font;
} MObjFont;


typedef union u_meta_object
{
  int		type;
  MObjPen	metapen;
  MObjBrush	metabrush;
  MObjFont	metafont;
} MObject;

typedef struct s_meta_dc_stack
{
 MClipZone		*clip;
 struct s_meta_dc_stack	*Next;
} MDCStack;

/*--- Methods ---*/
typedef struct
{
 F_STD;
 boolean (*readMain) ___PROTO((c_WMFDocFile *this));
} sf_WMFDocFile;

/*--- class declaration ---*/
struct sc_WMFDocFile
{
 sf_WMFDocFile	*f;

 /*___File info___*/
 char		*filename;
 FILE		*fd;
 boolean	close_flag;

 /*___Memory struct___*/
 c_VectorGraph	*vector;
 
 /*___Color___*/
 c_Color	**color_base;

 /*___Internals___*/
 int		factor;		/* number of unit per inch (normally 1440) */
 MObject	*obj_list;
 int		num_obj;
 int		pen_style;
 int	        brush_style;

 MDCStack	*dc_stack;

 int		last_x;	/* special hard line clip */
 int		last_y; /* special hard line clip */
};

extern sf_WMFDocFile fc_WMFDocFile;

#endif /* !__XC_WMF_DOCUMENT_FILE_H__ */
