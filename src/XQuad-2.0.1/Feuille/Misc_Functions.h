/*
** Misc_Functions.h for XQuad in Feuille/
** miscellaneous functions
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
** Started on  Tue May 30 15:03:28 1995 Emmanuel Paris
** Last update Mon Jan 25 23:06:25 1999 Emmanuel Paris
*/

#ifndef _Misc_Functions_h_
#define _Misc_Functions_h_

#include "ColRow_Size.h"

extern void redraw_all_feuille ___PROTO((c_Feuille *This));
extern void set_unset_grid ___PROTO((c_Feuille *This));
extern void cut_region ___PROTO((c_Feuille *This));
extern void copy_region ___PROTO((c_Feuille *This));
extern void paste_region ___PROTO((c_Feuille *This));
extern void display_frame_border ___PROTO((c_Feuille *This));
extern void display_column_origin ___PROTO((c_Feuille *This));
extern void display_row_origin ___PROTO((c_Feuille *This));
extern void display_page_mark ___PROTO((c_Feuille *This));
extern void display_formula ___PROTO((c_Feuille *This));
extern void display_zero_cell ___PROTO((c_Feuille *This));
extern void select_all_region ___PROTO((c_Feuille *This));
extern void delete_selection ___PROTO((c_Feuille *This));
extern void sheet_insert_column ___PROTO((c_Feuille *This));
extern void sheet_insert_row ___PROTO((c_Feuille *This));
extern void sheet_delete_column ___PROTO((c_Feuille *This));
extern void sheet_delete_row ___PROTO((c_Feuille *This));
extern void set_column_and_row_size ___PROTO((c_Feuille *This,
					      colrow_size_t *colrow));
extern void copy_region_r_or_d ___PROTO((c_Feuille *This, int type));
extern void resize_column_idealy ___PROTO((c_Feuille *This, int col));
extern void resize_row_idealy ___PROTO((c_Feuille *This, int row));

extern void goto_last_cell ___PROTO((c_Feuille *This));
extern void goto_active_cell ___PROTO((c_Feuille *This));
extern void select_to_last_cell ___PROTO((c_Feuille *This));

#endif

