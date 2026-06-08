/*
** PrintParam.h for XQuad in PostScript/
** Printing parameters
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
** Started on  Fri Jun  2 15:18:09 1995 Stéphane Boisson
** Last update Wed Oct 18 12:00:24 1995 Stéphane Boisson
*/

#ifndef __XQ_PRINT_PARAM_H__
#define __XQ_PRINT_PARAM_H__

typedef struct print_param_s print_param_t;
typedef struct setup_print_s setup_print_t;

#include "PostScript.h"
#include "TextStyle.h"
#include "PageFormat.h"


struct print_param_s
{
  printer_t	*printer;
  int		NbCopies;
  int		StartPage;
  int		EndPage;
  int		NbPage;
  boolean	AllPages;

  /*___Misc___*/
  boolean	selection_flag;
  boolean	Draft; /* currently not used but needed */
  boolean	Color; /* currently not used but needed */
};

/*--- Docfile keywords ---*/
#define XcDF_PRINTSETUP_KEYWORD "PRINTSETUP"
#define XcDF_DIRECTION_KEYWORD "DIRECTION"
#define XcDF_GRID_KEYWORD "GRID"
#define XcDF_AXIS_KEYWORD "AXIS"
#define XcDF_MARGINS_KEYWORD "MARGINS"
#define XcDF_VCENTER_KEYWORD "VCENTER"
#define XcDF_HCENTER_KEYWORD "HCENTER"
#define XcDF_SCALE_KEYWORD "SCALE"
#define XcDF_SCALEFIT_KEYWORD "FIT"

struct setup_print_s
{
  boolean	direction_f;		/* FALSE: L2R then T2B */
  boolean	grid_f;
  boolean	rowcol_f;

  /*___Margins___*/
  coord_t	left_margin;
  coord_t	right_margin;
  coord_t	top_margin;
  coord_t	bottom_margin;

  /*___Format___*/
  page_format_t	*page_format;
  coord_t	width;
  coord_t	height;

  /*___Position___*/
  boolean	vcenter_f;		/* Vertical centering		*/
  boolean	hcenter_f;		/* Horizontal centering		*/
  page_orient_t	orientation;		/* TRUE/FALSE:Landscape/Portrait*/

  /*___Scaling___*/
  int		scale_mode;		/* 0:rien 1:scale 2:tenir sur	*/
  scale_t	scale;
  unsigned int	horizontal_pages;
  unsigned int	vertical_pages;

  /*___Private___*/
  c_TextStyle *title_style;
  coord_t	title_width;
  coord_t	title_height;
};

#endif /* !__XQ_PRINT_PARAM_H__ */
