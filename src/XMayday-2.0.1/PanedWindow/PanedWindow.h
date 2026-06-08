/*
** PanedWindow.h for Xclamation, XQuad and XMayday in PanedWindow/
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
** Started on  Tue Jul 18 11:44:46 1995 Antoine Buat
** Last update Tue Jul 18 11:44:46 1995 Robin Castanier
*/


#ifndef _PanedWindow_h
#define _PanedWindow_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include <Xm/PanedW.h>

/* define the method for Class PanedWindow */
typedef struct
{
  F_STD;
} sf_PanedWindow;

/* define the PanedWindow Class */
typedef struct
{
  sf_PanedWindow	*f;
  Widget		w_This;
} c_PanedWindow;

extern sf_PanedWindow fc_PanedWindow;

#endif




