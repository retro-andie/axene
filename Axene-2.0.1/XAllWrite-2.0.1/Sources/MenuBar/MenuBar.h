/*
** MenuBar.h for Xclamation, XQuad, XAllWrite and XMayday in MenuBar/
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
** Started on  Mon Jul 17 19:44:23 1995 Robin Castanier
** Last update Thu Sep 17 15:24:01 1998 Emmanuel Paris
*/

#ifndef _MenuBar_h
#define _MenuBar_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>

/* define the method for Class MenuBar */
typedef struct
{
 F_STD;
} sf_MenuBar;

/* define the MenuBar Class */
typedef struct
{
 sf_MenuBar	*f;
 Widget	w_This;
} c_MenuBar;

extern sf_MenuBar fc_MenuBar;

#endif




