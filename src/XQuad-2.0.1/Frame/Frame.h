/*
** Frame.h for Xclamation, XQuad, XAllWrite, XMayday and XInstall in Frame/
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
** Started on  Mon Jul 17 17:29:32 1995 Robin Castanier
** Last update Mon Mar 10 00:30:32 1997 Antoine Buat
*/

#ifndef _Frame_h
#define _Frame_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include "WInterface.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include "ToggleButton.h"

/* defint the method for Class Frame */
typedef struct
{
  F_STD;
  void (*Add_Toggle)();
  void (*Toggle_Add_Callback)();
  void (*Toggle_Set)();
} sf_Frame;

/* define the Frame Class */
typedef struct
{
 sf_Frame	*f;
 Widget		w_This;
 Widget         w_Label;
 c_ToggleButton	*TBtitle;
} c_Frame;

extern sf_Frame fc_Frame;

#endif




































