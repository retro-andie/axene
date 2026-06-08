/*
** Display_CellStyle.h for XQuad in CellStyle/
** define Display_CellStyle Object
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
** Last update Thu May 25 17:56:15 1995 Emmanuel Paris
*/

#ifndef _Display_CellStyle_h_
#define _Display_CellStyle_h_

#define BORDER_LEFT		0
#define BORDER_RIGHT		1
#define BORDER_UP		2
#define BORDER_DOWN		3

enum {
  DISPLAY_LEFT_BORDER,
  DISPLAY_RIGHT_BORDER,
  DISPLAY_UP_BORDER,
  DISPLAY_DOWN_BORDER,
  DISPLAY_ALL_BORDERS,
  DISPLAY_INSIDE,
  DISPLAY_TEXT,
  DISPLAY_ALL_INSIDE,
  DISPLAY_ALL
};

extern void DisplayCellStyleBorders();
extern void DisplayCellStyleInside();
extern void DisplayCellStyleText();
extern boolean CalculTextOverrun();
extern coord_t CalculIdealCellWidth();
extern coord_t CalculIdealCellHeight();
extern void CSdrawLine();

#endif

