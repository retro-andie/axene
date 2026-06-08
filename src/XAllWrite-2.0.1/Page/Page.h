/*
** Page.h for XAllWrite in Page/
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
** Started on  Mon Jul 17 19:57:42 1995 Emmanuel Paris
** Last update Thu Aug 13 16:59:46 1998 Emmanuel Paris
*/

#ifndef _Page_h
#define _Page_h

typedef struct sc_Page c_Page;

#include "xcalibur.h"
#include "Colormap.h"
#include "Cursor.h"
#include "PageStd.h"
#include "ImageManager.h"
#include "AutoScroll.h"
#include "DocFile.h"
#include "Xinfo.h"
#include "PageFormat.h"
#include "PageStd.h"
#include "BaseStd.h"
#include "ScanLine.h"
#include "Calibration.h"
#include "Redraw.h"
#include "Cadre.h"
#include "FrameManager.h"
#include "Drag_Cadre.h"
#include "PostScript.h"

/*--- Constants for save & load ---*/
#define XcDF_PAGE_KEYWORD		"PAGE"
#define XcDF_MARGINS_KEYWORD		"MARGINS"

/* domain definitions */
#define NAME_MAX_LENGTH 	21

#define XPi_SCRATCH		0
#define XPi_LOAD		1
#define XPi_MODIFY		2

/*--- set page margin constant ---*/
#define XP_MARGIN_TOP		0	/* use only for page margins */
#define XP_MARGIN_BOTTOM	1	/* use only for page margins */
#define XP_MARGIN_LEFT		2	/* use for page and ruler margins */
#define XP_MARGIN_RIGHT		3	/* use for page and ruler margins */
#define XP_MARGIN_LEFT2		4	/* use only for ruler margins */
#define XP_MARGIN_INDENT	5	/* use only for ruler margins */

/*--- set tab constant ---*/
#define XP_ADD_TAB		0
#define XP_DEL_TAB		1
#define XP_MOVE_TAB		2
#define XP_CHANGE_TAB_TYPE	3
#define XP_CHANGE_TAB_FILLER	4

/* extern definition */
extern c_Colormap *GlobColormap;
extern sf_page_function fc_Select_Move_Edit_Text;
extern sf_page_function fc_Dispatch_To_FrMg;

/* define the method for Class Page */

typedef struct
{
 F_STD;
 void		(*Init) ___PROTO((c_Page *this, Display *display,
				       Window window, scale_t sFactor));
 void 		(*Resize) ___PROTO((c_Page *This, int posx, int posy,
					 int new_width, int new_height));
 boolean	(*SetFunction)();
 status_t	(*Eventlook)();
 void		(*MapWindow) ___PROTO((c_Page *This, boolean map));
 void		(*MoveWindow) ___PROTO((c_Page *This, int x, int y));
 void		(*UpdateVisiblePart) ___PROTO((c_Page *This, int vxo, int vyo,
					       int vxd, int vyd));
 boolean	(*IsActive) ___PROTO((c_Page *This, Window window));
 void		(*ChangeScaling) ___PROTO((c_Page *This, scale_t scale));
 void		(*RedrawAll) ___PROTO((c_Page *This));
 void		(*RedrawRectangle) ___PROTO((c_Page *This, int x, int y,
					     int width, int height));
 void		(*CalculClipping) ___PROTO((c_Page *This));
 void		(*SetFontStyle) ___PROTO((c_Page *This, c_TextStyle *style));
 void		(*SetFontFamily) ___PROTO((c_Page *This, family_tree *family));
 void		(*SetFontColor) ___PROTO((c_Page *This, c_Color *color));
 void		(*SetFontBgColor) ___PROTO((c_Page *This, c_Color *color));
 void		(*SetFontSize) ___PROTO((c_Page *This, coord_t size));
 
 void		(*SetPageMargin) ___PROTO((c_Page *This, coord_t size, 
					   int margin));
 void		(*SetRulerMargin) ___PROTO((c_Page *This, coord_t size, 
					    int margin));
 void		(*SetRulerTab) ___PROTO((c_Page *This, int index, coord_t pos,
					 tab_just_t tabtype, char filler, 
					 int func));

 /*___Loading, saving and printing___*/
 boolean	(*ps_print)();
 boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			   long *page_number, BaseStd_t *base));
 boolean (*write) ___PROTO((c_Page *this, c_DocFile *doc));
} sf_Page;

/* define the Page Class */
struct sc_Page
{
 sf_Page       	*f;

 struct sc_Page		**BasePage;
 struct sc_Page		*PrevPage;
 struct sc_Page		*NextPage;

 boolean	        X_info_calc;    /* True if X_info usable */
 x_info			X_info;		/* x misc info as display etc... */
 Window			shadowWin;	/* shadow window */
 c_Cursor		*cursor;
 BaseStd_t		*BaseStd;

 int			PosX;	/* window position relative to it parent */
 int			PosY;	/* parent */
 int			Width;	/* window width */
 int			Height;	/* window height */
 boolean		mapped;	/* if window is mapped */
  
 unsigned int		page_number;/* Page number */
 page_format_t		*page_format;/* A3 A4 etc... */
 page_type_t		page_type;/* Page Left,Right,Double */
 page_orient_t		page_orient;/* portrait, landscape */

 coord_t		sScale;	/* page scaling */	
 scale_t		sFactor;/* zoom factor in scale_t */	

 coord_t		sWidth;	/* page width in coord_t */
 coord_t		sHeight;/* page height in coord_t */

 sf_page_function	*function;
 int			function_type;
 void			*function_data;

 coord_t		left_margin;
 coord_t		right_margin;
 coord_t		top_margin;
 coord_t		bottom_margin;
 
 c_ScanLine		*ScanLine;
 c_Calibration		*Calibration;
 text_line_t		*text_line_start;
 text_line_t		*text_line_end;
 c_Redraw		*Redraw;
 c_FrMg			*frmg;

 coord_t		vx;		/* visible part of the page window */
 coord_t		vy;		/* v[x, y, width and height] */
 coord_t		vwidth;
 coord_t		vheight;

 boolean		in_use;		/* if TRUE, prevent undesirable */
					/* function changement. */
 boolean		*select_type;	/* TRUE: text block; FALSE: frame */
};

extern sf_Page fc_Page;

#endif

