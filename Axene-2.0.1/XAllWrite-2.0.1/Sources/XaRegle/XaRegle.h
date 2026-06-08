/*
** XaRegle.h for XAllWrite in XaRegle/
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
** Started on  Tue Jul 18 12:13:40 1995 Emmanuel Paris
** Last update Mon Jun  1 17:48:16 1998 Emmanuel Paris
*/

#ifndef _XaRegle_h
#define _XaRegle_h

typedef struct sc_XaRegle c_XaRegle;

#include "xcalibur.h"
#include "Cursor.h"
#include <X11/Intrinsic.h>
#include "Page.h"
#include "TextRuler.h"

extern void Init_XaRegle();
extern void Reset_XaRegle();

#define VOID_INTERV	0
#define MARGIN_INTERV	1
#define NORMAL_INTERV	2
#define COLUMN_INTERV	3

/*--- define some struct ---*/
typedef struct
{
 int		min;
 int		max;
 int		type;
} Xa_interv_t;

/*--- define the method for Class XaRegle ---*/
typedef struct
{
  F_STD;
  void	(*Resize) ___PROTO((c_XaRegle *This, int Size));
  void	(*Draw) ___PROTO((c_XaRegle *This, boolean recalcul));
  boolean (*EventLook) ___PROTO((c_XaRegle *This, XEvent *Event));
  void	(*Configure) ___PROTO((c_XaRegle *This, c_Page *Page, 
			       c_TextRuler *Ruler));
  void	(*Init_XaRegle_By_Page) ___PROTO((c_XaRegle *This, c_Page *Page));
  void	(*Init_XaRegle_By_Ruler) ___PROTO((c_XaRegle *This, 
					   c_TextRuler *Ruler));
} sf_XaRegle;

/*--- define the XaRegle Class ---*/
struct sc_XaRegle
{
 sf_XaRegle	*f;
 Display	*display;
 Window		window;
 GC		gc;
 GC		gc_repere;
 GC		gc_Shadow;
 GC		gc_Light;
 c_Cursor	*cursor;
 c_Page		*Page;
 c_TextRuler	*Ruler;

 int		Orient;
 int		Size;	/* taille en pixel en largeur si horizontale ... */
 int		Unit;	/* voir ../Global/units.h */
 coord_t	Scale;  /* Size*Scale = largeur en coord_t */
 float		step_in_unit;		/* step in unit (cm, inch ...) */
 float		step_in_pixel;		/* for graduation (float for prec.) */
 float		digit_step_in_unit;	/* step in unit (cm, inch ...) */

 int		nb_intervs;
 Xa_interv_t	*intervs;

 int		Start;		/* Position of Left/Top page border */
 int		End;		/* Position of Right/Bottom page border */
 int		MarginStart;	/* Position of Left/Top page margin */
 int		MarginEnd;	/* Position of Left/Top page margin */

 int		LeftMargin;	/* Position of Left Ruler margin */
 int		RightMargin;	/* Position of Right Ruler margin */
 int		IndentMargin;	/* Position of Indent Ruler margin (Alinea) */

 tab_just_t	currentTab;
 int		tabPos[XcTR_MAX_TABULATOR];
 tab_just_t	tabType[XcTR_MAX_TABULATOR];
 int		nbTab;

 Pixmap		under_bot_left;
 Pixmap		under_bot_right;
 Pixmap		under_top_left;
 Pixmap		under_tabs[XcTR_MAX_TABULATOR];

 int		bot_left_pos;
 int		bot_right_pos;
 int		top_left_pos;

 /* function data */
 int		function;
 boolean	source;
 boolean	prems;
 int		xm, ym;
 boolean	shift;
 int		tabPtr;
};

extern sf_XaRegle fc_XaRegle;

#define NB_TAB		4
#define TAB_START	12

#define XaRegle_HORIZONTAL 	0
#define XaRegle_VERTICAL	1

/*--- sizes ---*/
#define XaRegle_SIZE		30
#define XaRegle_RSIZE		XaRegle_SIZE

#define XaRegle_UPBORDER	5
#define XaRegle_DOWNBORDER	8

#define XaRegle_MARK		2
#define XaRegle_BIGMARK		5

/*--- color ---*/
/*#define XaRegle_COLOR		"gray60"*/
#define XaRegle_COLOR		"gray60"
#define XaRegle_SHADOW		"gray30"
#define XaRegle_LIGHT		"gray80"
#define XaRegle_SEPARATOR	"gray30"

/*--- regle function ---*/
#define XaR_NO_FUNCTION			0
#define XaR_MOVE_PAGE_TOP_MARGIN	1
#define XaR_MOVE_PAGE_BOTTOM_MARGIN	2
#define XaR_MOVE_PAGE_LEFT_MARGIN	3
#define XaR_MOVE_PAGE_RIGHT_MARGIN	4
#define XaR_CHANGE_TAB_TYPE		5
#define XaR_MOVE_RULER_LEFT_MARGIN	6
#define XaR_MOVE_RULER_LEFT2_MARGIN	7
#define XaR_MOVE_RULER_RIGHT_MARGIN	8
#define XaR_MOVE_RULER_INDENT_MARGIN	9
#define XaR_MOVE_RULER_TAB		10
#define XaR_DROP_RULER_TAB		11

/*--- type of redraw ---*/
#define XaR_PAGE_MARGIN			0
#define XaR_LEFT_MARGIN			1
#define XaR_LEFT2_MARGIN		2
#define XaR_RIGHT_MARGIN		3
#define XaR_INDENT_MARGIN		4
#define XaR_TAB_MARK			5

#ifdef _PRIVATE_XaRegle_

/*___ global for memory & speed improvement? ___*/
/*--- all colors ---*/
long	XaRegle_Color;
long	XaRegle_Shadow;
long	XaRegle_Light;
long	XaRegle_Separator;

typedef struct
{
  int	width;
  int	height;
  int	offset_x;
  int	offset_y;
  char	* data[7];
} XaRegle_Font_t;

#define NB_CHAR 16

XaRegle_Font_t	XaRegle_Font[NB_CHAR]=
{
#include "font_0.h"
#include "font_1.h"
#include "font_2.h" 
#include "font_3.h" 
#include "font_4.h" 
#include "font_5.h" 
#include "font_6.h" 
#include "font_7.h" 
#include "font_8.h" 
#include "font_9.h" 
#include "font_..h"
#include "font_-.h"
#include "font_tabl.h"
#include "font_tabr.h"
#include "font_tabc.h"
#include "font_tabcp.h"
};

typedef struct
{
  int		nb_point;
  int		incx,incy;
  XPoint	*xpoint;
} XaRegle_Char_t;

XaRegle_Char_t	XaRegle_CharH[NB_CHAR];
XaRegle_Char_t	XaRegle_CharV[NB_CHAR];

#include "../Pixmaps/bot_left.zpm"
#include "../Pixmaps/bot_right.zpm"
#include "../Pixmaps/top_left.zpm"

Pixmap		Xa_Pixmap_Bottom_Left;
Pixmap		Xa_Pixmap_Bottom_Right;
Pixmap		Xa_Pixmap_Top_Left;

int		Xa_Width_Bottom_Left;
int		Xa_Width_Bottom_Right;
int		Xa_Width_Top_Left;

int		Xa_Height_Bottom_Left;
int		Xa_Height_Bottom_Right;
int		Xa_Height_Top_Left;

int		Xa_Hot_Bottom_Left;
int		Xa_Hot_Bottom_Right;
int		Xa_Hot_Top_Left;

GC		Xa_GC_Bottom_Left;
GC		Xa_GC_Bottom_Right;
GC		Xa_GC_Top_Left;

#endif

#endif
