/*
** Resize_Column.h for XQuad in Feuille/
** Resize Column header
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
** Started on  Mon Feb 20 11:57:53 1995 Emmanuel Paris
** Last update Mon Mar 13 18:37:25 1995 Emmanuel Paris
*/

#ifndef _Resize_Column_h_
#define _Resize_Column_h_

extern void resize_column_init();
extern void resize_column_abort();
extern void resize_column_done();
extern void resize_column_to_xy();
extern void resize_column_trace();

#endif
