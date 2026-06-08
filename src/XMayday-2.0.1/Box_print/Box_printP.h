/*
** Box_printP.h for XMayday in Box_print/
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Sun Sep  1 07:37:25 1996 Robin Castanier
** Last update Wed Sep  4 14:58:17 1996 Emmanuel Paris
*/


#ifndef _Box_print_P_h
#define _Box_print_P_h

#include "Resource.h"

/*--- Export type ---*/

typedef struct {
  int		resource;
  char		*name;
  int		param;
} export_type_t;

static export_type_t Xc_Table_Export_Type[] =
{
  {XcR_plainText,    NULL, 0},
  {XcR_formatedText, NULL, 1},
  {XcR_postscript,   NULL, 2}
};

static int Xc_Count_Export_Type = sizeof(Xc_Table_Export_Type)/sizeof(export_type_t);

/* Font Family */

typedef struct {
  char		*name;
  int		param;
} font_type_t;

static font_type_t Xc_Table_Font_Type[] =
{
  {"Times", 0},
  {"Helvetica", 1},
  {"New Century Schlbk", 2},
  {"Lucida Bright", 3}
};

static int Xc_Count_Font_Type = sizeof(Xc_Table_Font_Type)/sizeof(font_type_t);

#endif







