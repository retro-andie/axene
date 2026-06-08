/*
** ColRow_Size.h for XQuad in Box_colsize/
** define the colrow_size_t structure
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
** Started on  Fri Jun  9 17:39:47 1995 Emmanuel Paris
** Last update Fri Jun  9 17:50:29 1995 Emmanuel Paris
*/

#ifndef _ColRow_Size_h_
#define _ColRow_Size_h_

typedef struct 
{
  coord_t	col_size;
  boolean	change_col_size;
  coord_t	row_size;
  boolean	change_row_size;
} colrow_size_t;

#endif

