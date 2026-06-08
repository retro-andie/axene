/*
** PageFormat.h for Xclamation, XQuad, XAllWrite and XMayday in Global/
** Descibe Page Format
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
** Started on  Wed Dec 28 11:03:02 1994 Emmanuel Paris
** Last update Mon Jan 16 18:01:50 1995 Stéphane Boisson
*/

#ifndef _PageFormat_h_
#define _PageFormat_h_

/*--- Page type ---*/
typedef enum
{
  Xc_PAGE_LEFT = 0,
  Xc_PAGE_RIGHT,
  Xc_PAGE_DOUBLE
} page_type_t;

#define XcDF_LEFT_KEYWORD "LEFT"
#define XcDF_RIGHT_KEYWORD "RIGHT"
#define XcDF_DOUBLE_KEYWORD "DOUBLE"


/*--- Page orientation ---*/
typedef enum
{
  Xc_PORTRAIT = 0,
  Xc_LANDSCAPE
} page_orient_t;

#define XcDF_PORTRAIT_KEYWORD "PORTRAIT"
#define XcDF_LANDSCAPE_KEYWORD "LANDSCAPE"


/*--- Page format ---*/
typedef struct {
  char		*name;
  char		*keyword;
  coord_t	width;
  coord_t	height;
  long		param;
} page_format_t;

#ifdef _PRIVATE_PAGEFORMAT_

page_format_t Xc_Table_Page_Format[] =
{
  {"Other", "CUSTOM",
     SCALE_FROM_MILLIMETERS(200.0), SCALE_FROM_MILLIMETERS(200.0), -1},
  {"A4", "A",
     SCALE_FROM_MILLIMETERS(210.0), SCALE_FROM_MILLIMETERS(297.0), 4}, 
  {"A3", "A",
     SCALE_FROM_MILLIMETERS(297.0), SCALE_FROM_MILLIMETERS(420.0), 3}, 
  {"A5", "A",
     SCALE_FROM_MILLIMETERS(148.5), SCALE_FROM_MILLIMETERS(210.0), 5}, 
  {"B5", "B",
     SCALE_FROM_MILLIMETERS(176.0), SCALE_FROM_MILLIMETERS(250.0), 5}, 
  {"Letter US", "US_LETTER",
     SCALE_FROM_MILLIMETERS(215.9), SCALE_FROM_MILLIMETERS(279.4), -1}, 
  {"Legal US", "US_LEGAL",
     SCALE_FROM_MILLIMETERS(215.9), SCALE_FROM_MILLIMETERS(355.6), -1}, 
  {"Tabloid", "TABLOID",
     SCALE_FROM_MILLIMETERS(279.4), SCALE_FROM_MILLIMETERS(431.8), -1} 
};

int Xc_Count_Page_Format = sizeof(Xc_Table_Page_Format)/sizeof(page_format_t);
#else

extern page_format_t Xc_Table_Page_Format[];
extern int Xc_Count_Page_Format;

#endif /* _PRIVATE_PAGEFORMAT_ */

#define Xc_CUSTOM_FORMAT 0
#define Xc_DEFAULT_FORMAT 1

#endif /* _PageFormat_h_ */
