/*
** Set_TextRuler.h for XAllWrite in Page/
** Set Text Ruler Header
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
** Started on  Mon Sep  8 20:04:38 1997 Emmanuel Paris
** Last update Thu Aug 13 16:59:09 1998 Emmanuel Paris
*/


#ifndef _Set_TextRuler_h_
#define _Set_TextRuler_h_

extern void set_ruler_left ___PROTO((c_Page *This));
extern void set_ruler_center ___PROTO((c_Page *This));
extern void set_ruler_right ___PROTO((c_Page *This));
extern void set_ruler_justify ___PROTO((c_Page *This));
extern void set_ruler_inc_indent ___PROTO((c_Page *This));
extern void set_ruler_dec_indent ___PROTO((c_Page *This));
extern void set_ruler_inc_interline ___PROTO((c_Page *This));
extern void set_ruler_dec_interline ___PROTO((c_Page *This));
extern void set_ruler_inc_interparag ___PROTO((c_Page *This));
extern void set_ruler_dec_interparag ___PROTO((c_Page *This));
extern void set_page_margin ___PROTO((c_Page *This, coord_t size, int margin));
extern void set_ruler_margin ___PROTO((c_Page *This, coord_t size, 
				       int margin));
extern void set_ruler_tab ___PROTO((c_Page *This, int index, coord_t pos,
				    tab_just_t tabtype, char filler, 
				    int func));


#endif
