/*
** PushButton.h for XQuad in PushButton/
** encapsulation of Motif's ToggleButton Widget
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
** Started on  Wed Jan  3 22:16:04 1996 Emmanuel Paris
** Last update Mon Apr  1 16:05:33 1996 Robin Castanier
*/

#ifndef _PushButton_h
#define _PushButton_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include "WInterface.h"

#include <X11/Intrinsic.h>
#include <Xm/PushB.h>

/* defint the method for Class PushButton */
typedef struct
{
  F_STD;
  void (*Add_Callback)();
  void (*Set)();
} sf_PushButton;

/* define the PushButton Class */
typedef struct
{
  sf_PushButton	*f;
  Widget			w_This;
} c_PushButton;

extern sf_PushButton fc_PushButton;

#endif

