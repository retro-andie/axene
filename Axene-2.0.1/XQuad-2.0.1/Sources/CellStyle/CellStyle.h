/*
** CellStyle.h for XQuad in CellStyle/
** define CellStyle Object
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
** Started on  Tue Mar 28 13:43:16 1995 Emmanuel Paris
** Last update Mon Dec  2 17:56:12 1996 Robin Castanier
*/

#ifndef _CellStyle_h_
#define _CellStyle_h_

/*--- forward declaration ---*/
typedef struct sc_CellStyle c_CellStyle;
typedef struct moteur_print_s moteur_print_t;

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "BaseStd.h"
#include "CellStyleInit.h"
#include "Display_CellStyle.h"
#include "FormatNbr.h"
#include "ruler.h"

#define XcDF_CELLSTYLE_KEYWORD "CELLSTYLE"
#define XcDF_CELLSTYLELIST_KEYWORD "CELLSTYLELIST"
#define XcDF_LEFT_KEYWORD "LEFT"
#define XcDF_RIGHT_KEYWORD "RIGHT"
#define XcDF_TOP_KEYWORD "TOP"
#define XcDF_BOTTOM_KEYWORD "BOTTOM"
#define XcDF_PATTERN_KEYWORD "PATTERN"
#define XcDF_DASH_KEYWORD "DASH"
#define XcDF_RULER_KEYWORD "RULER"

#define PATTERN_HORIZONTAL     	FALSE
#define PATTERN_VERTICAL     	TRUE
#define PATTERN_NONE		-1
#define CELL_STYLE_NONE		((CellStyle_t *)0)
#define CELL_STYLE_DEFAULT	((CellStyle_t *)1)
#define DISPLAY_CELL_STYLE (*fc_CellStyle.DisplayCellStyle)
#define SET_CELL_STYLE (*fc_CellStyle.set)

#define	XcH_COLOR_CHANGED	0
#define	XcH_TEXT_STYLE_CHANGED	1
#define	XcH_FORMAT_NBR_CHANGED	2

/*--- Set codes ---*/
typedef enum {
  XqCS_END,
  XqCS_FOREGROUND, XqCS_BACKGROUND,
  XqCS_PATTERN, XqCS_ALIGNEMENT, XqCS_TEXTSTYLE,
  XqCS_LEFT_FOREGROUND, XqCS_LEFT_BACKGROUND,
  XqCS_LEFT_PATTERN, XqCS_LEFT_DASH,
  XqCS_RIGHT_FOREGROUND, XqCS_RIGHT_BACKGROUND,
  XqCS_RIGHT_PATTERN, XqCS_RIGHT_DASH,
  XqCS_TOP_FOREGROUND, XqCS_TOP_BACKGROUND,
  XqCS_TOP_PATTERN, XqCS_TOP_DASH,
  XqCS_BOTTOM_FOREGROUND, XqCS_BOTTOM_BACKGROUND,
  XqCS_BOTTOM_PATTERN, XqCS_BOTTOM_DASH,
  XqCS_FORMAT
} cellstyle_set_code_t;

/*-- cell style hook structure: call HookCellStyleCallback(feuille,cstyle) --*/
typedef struct {
  void			(*hook_callback)();
  void			*hook_callback_data;
} cstyle_hook_t;  


typedef struct {
  c_Color		*border_foreground;
  c_Color		*border_background;
  
  int			border_thickness; /* = line_thick*patrn_thick */
  
  int			border_line_thickness;
  int			border_line_cap_style;
  int			border_line_join_style;
  int			border_stipple_pattern;
  
  int			border_dash_pattern;
} CellBorderStyle_t;

/* struct for changing cellstyle inside params */
typedef struct {
  c_Color	*foreground;
  c_Color	*background;
  int	pattern;
} cell_inside_t;

/* struct for changing cellstyle borders params */
typedef struct {
  CellBorderStyle_t	cellb_outline;
  CellBorderStyle_t	cellb_left;
  CellBorderStyle_t	cellb_right;
  CellBorderStyle_t	cellb_up;
  CellBorderStyle_t	cellb_down;
  boolean		outline;
  boolean		left;
  boolean		right;
  boolean		up;
  boolean		down;
  int			set_border;
} cell_border_t;


/*--- Infos about rows or columns ---*/
typedef struct {
  coord_t pos;
  coord_t size;
  CellBorderStyle_t *border;
} rowcol_info_t;


#define XqCS_INC_REFERENCE(cs) ((cs)->count++)
#define XqCS_DEC_REFERENCE(cs) ((cs)->count--)
 
typedef struct _CellStyle_t
{ 
  long			count;
  struct _CellStyle_t	*PrevCellStyle;
  struct _CellStyle_t	*NextCellStyle;
  long id;

  /*___Inside area infos___*/
  c_Color		*inside_foreground;
  c_Color		*inside_background;
  int			inside_stipple_pattern;
  ruler_t		ruler_type;
  c_TextStyle		*text_style;
  c_FormatNbr		*format;
  
  /*___Borders infos___*/
  CellBorderStyle_t	left;
  CellBorderStyle_t	right;
  CellBorderStyle_t	up;
  CellBorderStyle_t	down;

  cstyle_hook_t		*hook_info;
  c_RasterFont		*rfont;
  c_CharMetrics		*metrics;
  scale_t		factor;
} CellStyle_t;

extern CellStyle_t  default_cstyle;

/*--- define the method for Class CellStyle ---*/
typedef struct
{
  F_STD;
  void	(*init) ___PROTO((c_CellStyle *This, Display *display, Window window,
			  void *moteur, void (*hook_callback)(), 
			  void *hook_callback_data));
  CellStyle_t *(*CreateNewCellStyle) ___PROTO((c_CellStyle *This,
					       CellStyle_t *cell_style));
  void (*DeleteCellStyle) ___PROTO((CellStyle_t *cell_style));
  CellStyle_t *(*AddCellStyleToBase) ___PROTO((c_CellStyle *This,
					       CellStyle_t *cell_style));
  void (*RemoveCellStyleFromBase) ___PROTO((c_CellStyle *This,
					    CellStyle_t *cell_style));
  void (*DisplayCellStyle) ___NPROTO((c_CellStyle *This, void *cell_info, 
				      int x, int y, int width, int height,
				      scale_t factor,int xg,
				      int yg, int part));
  /*___Control method___*/
  boolean (*compare) ___PROTO((CellStyle_t *a, CellStyle_t *b));
  void (*set) ___PROTO((CellStyle_t *this, ...));
  CellStyle_t *(*get) ___PROTO((c_CellStyle *this, ...));

  /*___Loading & saving___*/
  boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			    long param, BaseStd_t *base));
  boolean (*readList) ___PROTO((c_DocFile *doc, char *keyword,
				long param, BaseStd_t *base));
  boolean (*write) ___PROTO((CellStyle_t *this, c_DocFile *doc));
  boolean (*writeList) ___PROTO((c_CellStyle *this, c_DocFile *doc));

  /*___Printing___*/
  boolean (*printBackground) ___PROTO((CellStyle_t *this, c_PostScript *post,
				       bbox_t *bbox));
  void (*printAddBorders) ___PROTO((CellStyle_t *this, moteur_print_t *info,
				    region_t *region));
  boolean (*printFlushBorders) ___PROTO((moteur_print_t *info,
					 c_PostScript *post,
					 region_t *region));
  boolean (*printText) ___NPROTO((CellStyle_t *this, c_PostScript *post,
				 bbox_t *bbox, char *text,
				 int value_type, boolean clip_flag));
  void (*printClearBorders) ___PROTO((moteur_print_t *info, region_t *region));
  void (*printSetCellMargins) ___PROTO((moteur_print_t *info, bbox_t *bbox,
					int x, int y));
  boolean (*CalculTextOverrun) ___NPROTO((c_CellStyle *This,
					  char *txt, CellStyle_t *cstyle,
					  coord_t *over_pos, coord_t *over_neg,
					  scale_t factor));
  
  void (*printRemoveBorder) ___PROTO((moteur_print_t *info, region_t *region));
  
  coord_t (*CalculIdealCellWidth) ___NPROTO((c_CellStyle *This,
					     cell *cellule, 
					     CellStyle_t *cstyle, 
					     coord_t height, scale_t factor));
  coord_t (*CalculIdealCellHeight) ___NPROTO((c_CellStyle *This,
					      cell *cellule, 
					      CellStyle_t *cstyle,
					      coord_t width, scale_t factor));

} sf_CellStyle;


/*--- define the CellStyle Class ---*/
struct sc_CellStyle
{
  sf_CellStyle       	*f;
  
  Display		*display;
  Window		window;
  BaseStd_t		*BaseStd;

  void			*moteur;  
  cstyle_hook_t		cstyle_hook;
  
  GC			gc_inside;
  GC			gc_left;
  GC			gc_right;
  GC			gc_up;
  GC			gc_down;
  GC			gc_paint;
  GC			gc_mask;
  unsigned long		text_color;
  
  CellStyle_t		cell_style_current;

  int		nb_style;
  CellStyle_t	*cell_style;

  CellStyle_t		*default_cstyle;
  boolean		display_formula;
  boolean		display_zero_cell;
  boolean		init;
};

extern sf_CellStyle fc_CellStyle;

#endif


