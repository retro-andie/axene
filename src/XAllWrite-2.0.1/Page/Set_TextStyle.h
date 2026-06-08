/*
** Set_TextStyle.h for XAllWrite in Page/
** Set Text Style Header
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
** Started on  Fri Apr 11 17:13:13 1997 Emmanuel Paris
** Last update Tue Jul 21 18:15:18 1998 Emmanuel Paris
*/

#ifndef _Set_TextStyle_h_
#define _Set_TextStyle_h_

extern void set_font_bold ___PROTO((c_Page *This));
extern void set_font_italic ___PROTO((c_Page *This));
extern void set_font_underline ___PROTO((c_Page *This));
extern void set_font_strikeout ___PROTO((c_Page *This));
extern void set_font_shadow ___PROTO((c_Page *This));
extern void set_font_subscript ___PROTO((c_Page *This));
extern void set_font_supscript ___PROTO((c_Page *This));
extern void set_font_outline ___PROTO((c_Page *This));
extern void set_font_small_caps ___PROTO((c_Page *This));
extern void set_font_big_caps ___PROTO((c_Page *This));
extern void set_font_inc_size ___PROTO((c_Page *This));
extern void set_font_dec_size ___PROTO((c_Page *This));
extern void set_font_family ___PROTO((c_Page *This, family_tree *family));
extern void set_font_color ___PROTO((c_Page *This, c_Color *color));
extern void set_font_bgcolor ___PROTO((c_Page *This, c_Color *color));
extern void set_font_size ___PROTO((c_Page *This, coord_t size));
extern void set_font_style ___PROTO((c_Page *This, c_TextStyle *style));

#endif


