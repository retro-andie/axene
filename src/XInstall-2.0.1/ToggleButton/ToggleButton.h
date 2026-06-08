/*
** ToggleButton.h for Xclamation, XQuad, XAllWrite, XMayday and XInstall in 
** 	ToggleButton/
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
** Started on  Tue Jul 18 12:44:31 1995 Robin Castanier
** Last update Wed Mar 12 16:38:33 1997 Robin Castanier
*/

#ifndef _ToggleButton_h
#define _ToggleButton_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include "WInterface.h"

#include <X11/Intrinsic.h>
#include <Xm/ToggleBG.h>

/* defint the method for Class ToggleButton */
typedef struct
{
  F_STD;
  void (*Add_Callback)();
  void (*Set)();
} sf_ToggleButton;

/* define the ToggleButton Class */
typedef struct
{
 sf_ToggleButton	*f;
 Widget			w_This;
} c_ToggleButton;

extern sf_ToggleButton fc_ToggleButton;

#endif

