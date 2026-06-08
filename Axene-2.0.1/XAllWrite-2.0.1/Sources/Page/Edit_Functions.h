/*
** Edit_Functions.h for XAllWrite in Page/
** Miscellaneous functions for the Edit Menu
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
** Started on  Sat Apr 19 15:55:13 1997 Emmanuel Paris
** Last update Sat Jan 16 21:49:57 1999 One of the authors
*/

#ifndef _Edit_Functions_h_
#define _Edit_Functions_h_

extern void clipboard_cut_Page ___PROTO((c_Page *This));
extern void clipboard_copy_Page ___PROTO((c_Page *This));
extern void clipboard_paste_Page ___PROTO((c_Page *This));
extern void delete_selection_Page ___PROTO((c_Page *This));
extern void select_all_Page ___PROTO((c_Page *This));

extern void select_text_by_blocks ___PROTO((c_Page *This, int type));

extern void insert_page_break_Page ___PROTO((c_Page *This));
extern void insert_euro_Page ___PROTO((c_Page *This));
extern void insert_spec_char_Page ___PROTO((c_Page *This));

#endif /* _Edit_Functions_h_ */
