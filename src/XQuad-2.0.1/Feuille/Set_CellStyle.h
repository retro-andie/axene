/*
** Set_CellStyle.h for XQuad in Feuille/
** Set cellule style
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
** Started on  Mon Apr  3 15:32:46 1995 Emmanuel Paris
** Last update Mon Apr 14 17:11:00 1997 Emmanuel Paris
*/

#ifndef _SET_CELLSTYLE_H_
#define _SET_CELLSTYLE_H_

extern void unset_all_border ___PROTO((c_Feuille *This));
extern void set_border_left ___PROTO((c_Feuille *This));
extern void set_border_right ___PROTO((c_Feuille *This));
extern void set_border_up ___PROTO((c_Feuille *This));
extern void set_border_down ___PROTO((c_Feuille *This));
extern void set_all_border ___PROTO((c_Feuille *This));

extern void set_ruler_default ___PROTO((c_Feuille *This));
extern void set_ruler_align_h_left ___PROTO((c_Feuille *This));
extern void set_ruler_align_h_right ___PROTO((c_Feuille *This));
extern void set_ruler_align_h_center ___PROTO((c_Feuille *This));
extern void set_ruler_justify_h ___PROTO((c_Feuille *This));
extern void set_ruler_align_v_top ___PROTO((c_Feuille *This));
extern void set_ruler_align_v_bottom ___PROTO((c_Feuille *This));
extern void set_ruler_align_v_center ___PROTO((c_Feuille *This));
extern void set_ruler_justify_v ___PROTO((c_Feuille *This));
extern void set_ruler_multiline ___PROTO((c_Feuille *This));
extern void set_ruler_monoline ___PROTO((c_Feuille *This));
extern void set_ruler_angle_0 ___PROTO((c_Feuille *This));
extern void set_ruler_angle_90 ___PROTO((c_Feuille *This));
extern void set_ruler_angle_270 ___PROTO((c_Feuille *This));

extern void set_font_bold ___PROTO((c_Feuille *This));
extern void set_font_italic ___PROTO((c_Feuille *This));
extern void set_font_underline ___PROTO((c_Feuille *This));
extern void set_font_strikeout ___PROTO((c_Feuille *This));
extern void set_font_subscript ___PROTO((c_Feuille *This));
extern void set_font_supscript ___PROTO((c_Feuille *This));
extern void set_font_shadow ___PROTO((c_Feuille *This));
extern void set_font_outline ___PROTO((c_Feuille *This));
extern void set_font_small_caps ___PROTO((c_Feuille *This));
extern void set_font_big_caps ___PROTO((c_Feuille *This));

extern void set_font_inc_size ___PROTO((c_Feuille *This));
extern void set_font_dec_size ___PROTO((c_Feuille *This));

extern void set_format_percent ___PROTO((c_Feuille *This));
extern void set_format_currency ___PROTO((c_Feuille *This));
extern void set_format_thousands ___PROTO((c_Feuille *This));
extern void set_format_scientific ___PROTO((c_Feuille *This));
extern void set_format_engineer ___PROTO((c_Feuille *This));
extern void set_format_add_digit ___PROTO((c_Feuille *This));
extern void set_format_sub_digit ___PROTO((c_Feuille *This));

extern void set_cell_backcolor ___PROTO((c_Feuille *This, c_Color *color));
extern void set_cell_background ___PROTO((c_Feuille *This));
extern void set_cell_inverse ___PROTO((c_Feuille *This));

extern void set_cell_inside ___PROTO((c_Feuille *This,
				      cell_inside_t *inside));
extern void set_cell_borders ___PROTO((c_Feuille *This, 
				      cell_border_t *borders));
extern void set_cell_formatnbr ___PROTO((c_Feuille *This, 
					 c_FormatNbr *formatnbr));
extern void set_cell_textstyle ___PROTO((c_Feuille *This,
					 c_TextStyle *textstyle));
extern void set_cell_fontfamily ___PROTO((c_Feuille *This,
					  family_tree *family));
extern void set_cell_fontcolor ___PROTO((c_Feuille *This, c_Color *color));
extern void set_cell_fontsize ___PROTO((c_Feuille *This, coord_t size));

extern void unset_cell_inside ___PROTO((c_Feuille *This));
extern void unset_cellstyle_all ___PROTO((c_Feuille *This));

extern void unset_cell_formatnbr ___PROTO((c_Feuille *This));
extern void unset_cell_textstyle ___PROTO((c_Feuille *This));
extern void unset_cell_formulas ___PROTO((c_Feuille *This));
extern void unset_cell_all ___PROTO((c_Feuille *This));
#endif
