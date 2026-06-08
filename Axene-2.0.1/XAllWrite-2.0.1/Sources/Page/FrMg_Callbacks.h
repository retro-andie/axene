/*
** FrMg_Callbacks.h for XAllWrite in Page/
** Page's Callbacks for FrameManager
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
** Started on  Thu Apr 16 14:10:13 1998 Emmanuel Paris
** Last update Mon Aug  3 20:28:03 1998 Emmanuel Paris
*/

#ifndef _Frmg_Callbacks_h_
#define _Frmg_Callbacks_h_


extern void frmg_select_callback ___PROTO((c_Page *This));
extern void frmg_deselect_callback ___PROTO((c_Page *This));
extern void frmg_redraw_polygone_callback ___PROTO((c_Page *This,
						    Polygone *polygone));
extern void frmg_modify_cadre_callback ___PROTO((c_Page *This));
extern void frmg_attract_point_callback ___PROTO((c_Page *This,
						  coord_t *sx, coord_t *sy,
						  int *x, int *y));

extern void display_frame_border ___PROTO((c_Page *This));

#endif
