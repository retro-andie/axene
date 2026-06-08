/*
** BaseStd.h for XQuad in Global/
** structure de base standard
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
** Started on  Mon Dec 12 16:17:18 1994 Emmanuel Paris
** Last update Thu May 18 13:59:04 1995 Emmanuel Paris
*/

#ifndef _BaseStd_h_
#define _BaseStd_h_

typedef struct BaseStd_s BaseStd_t;

#include "TextStyle.h"
#include "Color.h"
#include "CellStyle.h"
#include "FormatNbr.h"

struct BaseStd_s
{
 c_Color	*color_base;
 c_TextStyle	*text_style_base;
 c_CellStyle	*cell_style_base;
 c_FormatNbr	*format_base;
 c_Color	*BG_color_base;
 c_TextStyle	*BG_text_style_base;
};

#define CMP_WITH_NAME		TRUE
#define CMP_WITHOUT_NAME	FALSE

#endif



