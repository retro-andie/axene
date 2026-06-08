/*
** ExportTextFileSelector.h for Xclamation in FileSelector/
** File selector for text ExportText
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
** Started on  Mon Jan  1 14:34:10 1996 Emmanuel Paris
** Last update Wed Aug 11 00:21:42 1999 Emmanuel Paris
*/

#ifndef _ExportText_FileSelector_h
#define _ExportText_FileSelector_h

#include "Grid.h"
#include "Frame.h"
#include "ItemMenu.h"
#include "Label.h"
#include "WTextField.h"
#include "ToggleButton.h"
#include "Form.h"
#include "encodings.h"
#include <Xm/SeparatoG.h>

enum 
{
 IDS_EXPORT_XCL,
 IDS_EXPORT_ASCII,
 IDS_EXPORT_HTML,
 NB_IDS_ITEM
};

typedef struct
{
 char		*name;
 int		num;
 boolean	created;
 boolean	managed;
} ids_format_t;

typedef struct 
{
 ids_format_t	*format;
 ids_format_t	*selected_item;
 c_Grid	*grid;
 c_ItemMenu	*menu;
 c_Frame	*frame;
 Widget		wSep;
  
 boolean	dim_calc;
 Dimension	width, height;
  
 c_Form	*formXCL;
  
 c_Grid	*gridHTML;
 c_ToggleButton *tbHTMLRulers;
 c_Label	*lHTMLFrameBreak;
 c_WTextField	*tfHTMLFrameBreak;
 c_Label	*lHTMLCarrierReturn;
 c_WTextField	*tfHTMLCarrierReturn;
 Widget	sepHTML1;
 c_Label	*lHTMLStyles;
 c_ItemMenu	*imHTMLStyles;
 c_Label	*lHTMLStyleBegin;
 c_WTextField	*tfHTMLStyleBegin;
 c_Label	*lHTMLStyleEnd;
 c_WTextField	*tfHTMLStyleEnd;
} ids_data_t;

typedef struct 
{
 encoding_t	encoding;
} expt_ASCII_t;

typedef struct 
{
 boolean	rulers;		/* interpret ruler */
 boolean	ordered_list;	/* interpret ordered list */
 boolean	unordered_list;	/* interpret unordered list */
 int		nb_style;
 char		**style_name;
 char		**style_begin;
 char		**style_end;
 char		*frame_break;
 char		*carriage_return;
 int		selected_style;
} expt_HTML_t;

typedef struct 
{
 expt_ASCII_t	expt_ASCII;
 expt_HTML_t	expt_HTML;
} export_text_t;

extern export_text_t	export_text;

#endif



