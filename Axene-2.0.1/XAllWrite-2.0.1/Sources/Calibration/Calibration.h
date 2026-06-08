/*
** Calibration.h for XAllWrite in Calibration/
** Calibrate the text: calcul text placements.
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Wed Apr 23 14:53:46 1997 Emmanuel Paris
** Last update Mon Jul 20 14:30:57 1998 Emmanuel Paris
*/

#ifndef _Calibration_h_
#define _Calibration_h_

/*--- Forward declarations ---*/

typedef struct sc_Calibration c_Calibration;
typedef struct _text_line_t text_line_t;
typedef struct _text_spot_t text_spot_t;

#include "xcalibur.h"
#include "PageArea.h"
#include "Text.h"

/*--- text line structure ---*/
struct _text_line_t
{
 struct _text_line_t	*prev;
 struct _text_line_t	*next;
 text_spot_t	*text_spot;
 int		line_number;
 c_TextRuler	*ruler;
 coord_t	y;
 coord_t	height;
 coord_t	x_min;
 coord_t	x_max;
 coord_t	char_inc;
 coord_t	word_inc;
 boolean	new_paragraph;	/* TRUE if this line end  with a LINEFEED */
};

/*--- text spot structure ---*/
struct _text_spot_t
{
 struct _text_spot_t	*next;
 xa_item_t	*item;
 coord_t	x;
 coord_t	width;
 coord_t	y_offset;	/* base line offset for text item */
};

typedef struct
{
 F_STD;
 void (*Init) ___PROTO((c_Calibration *This, c_Page *page));
 void (*calculSelection) ___PROTO((c_Calibration *This, boolean direction));
 void (*updateText) ___PROTO((c_Calibration *This));
 void (*makeCursorVisible) ___PROTO((c_Calibration *This));
 void (*managePageSelection) ___PROTO((c_Calibration *This, c_Page *page));
 void (*goToBeginOfLine) ___PROTO((c_Calibration *This, boolean select));
 void (*goToEndOfLine) ___PROTO((c_Calibration *This, boolean select));
 void (*goToBeginOfPage) ___PROTO((c_Calibration *This, boolean select));
 void (*goToEndOfPage) ___PROTO((c_Calibration *This, boolean select));
 void (*goToUpperLine) ___PROTO((c_Calibration *This, boolean select));
 void (*goToBottomLine) ___PROTO((c_Calibration *This, boolean select));
 void (*goToPageUp) ___PROTO((c_Calibration *This, boolean select));
 void (*goToPageDown) ___PROTO((c_Calibration *This, boolean select));
 boolean (*getTextLineByY) ___PROTO((c_Calibration *This, c_Page *page, 
				     coord_t y, coord_t x, text_line_t **rtl));
 void (*checkSelectionOrder) ___PROTO((c_Text *txt, text_line_t *tl_start,
				       text_line_t *tl_end));
 boolean (*ps_print_text_lines) ___PROTO((c_Calibration *This, 
					  c_PostScript *post, 
					  text_line_t *start, 
					  text_line_t *end));
} sf_Calibration;

/* define the Calibration Class */
struct sc_Calibration
{
 sf_Calibration        *f;

 c_PageArea	*PageArea;
 BaseStd_t	*BaseStd;

 ItemHandle     *tl_handle;		/* text line handle */
 ItemHandle     *ts_handle;		/* text spot handle */
 text_line_t	*text_line;
 
 text_line_t	*tl_select_start;
 text_line_t	*tl_select_end;

 /* for upper and bottom line */
 xa_item_t	*ubl_select;
 int		ubl_select_pos;
 coord_t	ubl_scr_pos;

 boolean	forceTextLine;
 boolean	newPageFlag;
 coord_t	tab_origin;	/* generally equal to page left margin */
};

extern sf_Calibration fc_Calibration;

#endif /* _Calibration_h_ */


