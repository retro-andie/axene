/*
** Cursor_Set.h for Xclamation in Cursor/
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
** Started on  Mon Jul 17 14:20:58 1995 Emmanuel Paris
** Last update Mon Jul 17 14:20:58 1995 Emmanuel Paris
*/

#ifndef _Cursor_Set_
#define _Cursor_Set_

#define NB_CURSOR 0

#include "Cursor_Drag_And_Drop.h"
#include "Cursor_Wait.h"
#include "Cursor_General.h"
#include "Cursor_Resize.h"
#include "Cursor_Regle.h"
#include "Cursor_PAO.h"

#ifdef _PRIVATE_CURSOR_

Cursor_Set default_cursor_set[NB_CURSOR]={
  CURSOR_SET_DRAG_AND_DROP
  CURSOR_SET_WAIT
  CURSOR_SET_GENERAL
  CURSOR_SET_RESIZE
  CURSOR_SET_REGLE    
  CURSOR_SET_PAO
};

#endif

#endif
