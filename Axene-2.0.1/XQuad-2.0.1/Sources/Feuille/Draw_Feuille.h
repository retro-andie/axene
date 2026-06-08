/*
** Draw_Feuille.h for XQuad in Feuille/
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
** Started on  Fri Sep 30 15:33:47 1994 Emmanuel Paris
** Last update Mon Feb 20 10:53:15 1995 Emmanuel Paris
*/

#ifndef _Draw_Feuille_h
#define _Draw_Feuille_h

extern void focus_in_cellule();
extern void focus_out_cellule();
extern void focus_in_active_cellule();
extern void focus_out_active_cellule();
extern void focus_in_column();
extern void focus_out_column();
extern void focus_in_row();
extern void focus_out_row();
extern void focus_in_origin();
extern void focus_out_origin();
extern void draw_cellule();
extern void draw_column();
extern void draw_row();
extern void draw_origin();

#endif
