/*
** MainInterface.h for Xclamation, XQuad and XAllWrite in MainInterface/
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
** Started on  Mon Jul 17 19:23:23 1995 Antoine Buat
** Last update Fri Jan 28 10:12:38 2000 Emmanuel Paris
*/

#ifndef _MainInterface_h
#define _MainInterface_h

#include "xcalibur.h"
#include "ManageWidget.h" 
#include "Resource.h" 
#include "XLabel.h" 

#include <X11/Intrinsic.h>
#include <Xm/Form.h>

#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>

/* define the method for Class MainInterface */
typedef struct
{
  F_STD;
  void (*create_and_map_StartUp)();
  void (*unmap_StartUp)();
  void (*create_TopLevel)();
  void (*map_TopLevel)();
  void (*unmap_TopLevel)();
  void (*attach_top)();
  void (*attach_bottom)();
  void (*attach_left)();
  void (*attach_right)();
  void (*set_main_icon)();
  void (*reopen_display)();
  void (*set_title)();
  void (*set_loadingMessage)();
} sf_MainInterface;

/* define the MainInterface Class */
typedef struct
{
 sf_MainInterface	*f;
 Widget			w_This;
 Widget			w_Main;
 Widget			w_Hidden;
 Widget			w_Startup;
 Widget                 w_Frame;
 Widget                 w_RowStartup;
 Widget                 w_LabelStartup1;
 Widget                 w_LabelStartup2;
 Widget                 w_LabelStartup3;
 c_XLabel               *LabelMessage;
 Widget			w_DAreaStartup;
 char			*display_name;
} c_MainInterface;

extern sf_MainInterface fc_MainInterface;

#endif














