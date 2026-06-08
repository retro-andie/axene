/*
** ManageWidget.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in ManageWidget/
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
** Started on  Mon Jul 17 19:39:15 1995 Antoine Buat
** Last update Tue Jan 20 18:31:51 1998 Robin Castanier
*/

#ifndef _ManageWidget_h
#define _ManageWidget_h

#include "xcalibur.h"
#include <X11/Intrinsic.h>

/* define the method for Class ManageWidget */
typedef struct
{
 F_STD;
 void (*Manage_all)();
 void (*Add_child)();
} sf_ManageWidget;

/* define the listwidget type */
typedef struct listr
{
 Widget	w_current;
 Widget	w_parent;
 struct listr	*NextWidget;
} listwidget;

/* define the ManageWidget Class */
typedef struct
{
 sf_ManageWidget	*f;
 int			number; 
 listwidget		*w_childlist;
} c_ManageWidget;

extern sf_ManageWidget fc_ManageWidget;

#endif


