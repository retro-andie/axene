/*
** FeuilleStd.h for XQuad in Global/
** standard definition for Feuille functions
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
** Started on  Sometime during     1995 One of the authors
** Last update Mon Jan 25 23:12:23 1999 One of the authors
*/


#ifndef _FeuilleStd_h
#define _FeuilleStd_h

#include "xcalibur.h"

enum {
  FEUILLE_FUNCTION_BEGIN = 0,
  FRMG_FUNCTION,
  SELECT_MOVE_RESIZE_CELL,
  UNSET_ALL_BORDER,
  SET_BORDER_LEFT,
  SET_BORDER_DOWN,
  SET_BORDER_RIGHT,
  SET_BORDER_UP,
  SET_ALL_BORDER,
  INVERSE_COLOR,
  SET_BACKGROUND,
  SELECT_REGION_FOR_EDITBAR,
  SET_UNSET_GRID,
  ZOOM_50_SHEET,
  ZOOM_75_SHEET,
  ZOOM_100_SHEET,
  ZOOM_200_SHEET,
  ZOOM_400_SHEET,
  ZOOM_SELECTION,
  ZOOM_PLUS_SHEET,
  ZOOM_MINUS_SHEET,
  REDRAW_ALL,
  CUT_REGION,
  COPY_REGION,
  PASTE_REGION,
  ALIGN_DEFAULT,
  ALIGN_H_LEFT,
  ALIGN_H_RIGHT,
  ALIGN_H_CENTER,
  ALIGN_V_TOP,
  ALIGN_V_BOTTOM,
  ALIGN_V_CENTER,
  DISPLAY_COLUMN_ORIGIN,
  DISPLAY_ROW_ORIGIN,
  DISPLAY_PAGE_MARK,
  DISPLAY_FORMULA,
  DISPLAY_ZERO_CELL,
  UNSET_FORMULAS,
  UNSET_ALL_REGION,
  UNSET_FORMATNBR,
  UNSET_TEXTSTYLE,
  UNSET_INSIDE_PATTERN,
  UNSET_ALL_CELLSTYLE,
  SELECT_ALL_REGION,
  INSERT_COLUMN,
  INSERT_ROW,
  DELETE_COLUMN,
  DELETE_ROW,
  GRAPH_HISTO_V,
  GRAPH_HISTO_H,
  GRAPH_CURVE,
  GRAPH_SURFACE,
  GRAPH_SECTOR,
  GRAPH_RADAR,
  JUSTIFY_H,
  JUSTIFY_V,
  MONOLINE,
  MULTILINES,
  ANGLE_0,
  ANGLE_90,
  ANGLE_270,
  COPY_DOWN,
  COPY_RIGHT,
  GRAPH_LEGEND,
  GRAPH_AXES,
  GRAPH_TITLE,
  GRAPH_CONFIG,
  GRAPH_ABSCISSE,
  GRAPH_ORDONNEE,
  SET_BOLD,
  SET_ITALIC,
  SET_STRIKEOUT,
  SET_UNDERLINE,
  SET_SUBSCRIPT,
  SET_SUPSCRIPT,
  SET_SHADOW,
  SET_OUTLINE,
  SET_SMALL_CAPS,
  SET_BIG_CAPS,
  INC_FONT_SIZE,
  DEC_FONT_SIZE,
  PERCENT_FORMAT,
  CURRENCY_FORMAT,
  THOUSANDS_FORMAT,
  SCIENTIFIC_FORMAT,
  ENGINEER_FORMAT,
  FORMAT_ADD_DIGIT,
  FORMAT_SUB_DIGIT,
  GOTO_LAST_CELL,
  GOTO_ACTIVE_CELL,
  SELECT_TO_LAST_CELL,
  DELETE_SELECTION,
  FEUILLE_FUNCTION_END
};

#ifndef FUNCTION_FINISHED
#define FUNCTION_FINISHED	0
#define	FUNCTION_NOT_FINISHED	1
#define FUNCTION_ABORTED	2
#define FUNCTION_REINIT		3
#define FUNCTION_MODIFIED	4
#endif

#ifndef KEY_KEEP
#define KEY_KEEP		0
#define KEY_REJECTED		1
#define KEY_PUT_BACK		2
#endif

/* short cut for standard function definition */
#define F_FEUILLE_STD		\
void (*init)();			\
status_t (*eventlook)();	\
void (*abort)();		\
void (*redraw)()     
     
typedef struct _sf_feuille_function {
  F_FEUILLE_STD;
} sf_feuille_function;

#ifndef FUNCTION_STD
#define FUNCTION_STD

#define INIT(obj)	(obj)->function->init(obj);
#define EVENTLOOK(obj, event) (obj)->function->eventlook(obj, event)
#define ABORT(obj)	(obj)->function->abort(obj)
#define REDRAW(obj) (obj)->function->redraw(obj)

#define CURSOR (cfeuille->cursor)
#endif

#define CURSOR_O (cfeuille->cursor_O)
#define CURSOR_C (cfeuille->cursor_C)
#define CURSOR_R (cfeuille->cursor_R)

#ifdef _PRIVATE_FEUILLE_

#ifndef DATA_MACROS
#define DATA_MACROS
#ifdef ___PSEUDO_COMPOSE_MACRO
#define	DATA(a)	d_##a *data	 
#define DATA_ITSELF(a) (d_##a *)cfeuille->function_data
#define	SET_DATA(a) data=(d_##a *)Xc_malloc(#a ,sizeof(d_##a)); \
		    cfeuille->function_data=(void *)data
#define GET_DATA(a) data=(d_##a *)cfeuille->function_data
#define GET_DATAF(a) c_FrMg *cfrmg; \
		     cfrmg = cfeuille->frmg; \
		     data=(d_##a *)cfeuille->function_data
#define	d(a)   (data->a)
#else
#ifdef ___GNUC_COMPOSE_MACRO
#define	DATA(a)	d_##a## *data	 
#define DATA_ITSELF(a) (d_##a## *)cfeuille->function_data
#define	SET_DATA(a) data=(d_##a## *)Xc_malloc("##a##",sizeof(d_##a##)); \
		    cfeuille->function_data=(void *)data
#define GET_DATA(a) data=(d_##a## *)cfeuille->function_data
#define GET_DATAF(a) c_FrMg *cfrmg; \
		     cfrmg = cfeuille->frmg; \
		     data=(d_##a## *)cfeuille->function_data
#define	d(a)   (data->##a##)
#else
#define	DATA(a)	d_/**/a/**/ *data	 
#define	DATA_ITSELF(a)	(d_/**/a/**/ *)cfeuille->function_data	 
#define	SET_DATA(a) data=(d_/**/a/**/ *)Xc_malloc("/**/a/**/",sizeof(d_/**/a/**/)); \
		    cfeuille->function_data=(void *)data
#define GET_DATA(a) data=(d_/**/a/**/ *)cfeuille->function_data
#define GET_DATAF(a) c_FrMg *cfrmg; \
		     cfrmg = cfeuille->frmg; \
		     data=(d_/**/a/**/ *)cfeuille->function_data
#define	d(a)   (data->/**/a/**/)
#endif
#endif /* PSEUDO */
#define FREE_DATA Xc_free(data); \
                  cfeuille->function_data=NULL
#endif

#endif /*_PRIVATE_FEUILLE_*/

#endif /*_FeuilleStd_h*/




