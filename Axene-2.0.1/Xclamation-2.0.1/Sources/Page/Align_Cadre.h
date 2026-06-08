/*
** Align_Cadre.h for Xclamation in Page/
** Align cadre horizontaly/verticaly
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
** Started on  Fri Jan 20 17:03:09 1995 Emmanuel Paris
** Last update Thu Feb  9 15:44:08 1995 Emmanuel Paris
*/

#ifndef _Align_Cadre_h
#define _Align_Cadre_h

typedef enum {
  ALIGN_LEFT = 0,
  ALIGN_TOP = 0,
  ALIGN_RIGHT = 1,
  ALIGN_BOTTOM = 1,
  ALIGN_CENTER,
  ALIGN_BLOCK,
  ALIGN_END_NUM
} align_type_t;

typedef struct 
{
  boolean	horizontal;
  boolean	vertical;
  boolean	distribution_h;
  boolean	distribution_v;
  align_type_t	type_h;
  align_type_t	type_v;
  coord_t	offset_h;
  coord_t	offset_v;
} align_param_t;

typedef struct _align_coord_t
{
  coord_t	sOrig_X;
  coord_t	sOrig_Y;
} align_coord_t;

extern void align_cadre();

#endif



