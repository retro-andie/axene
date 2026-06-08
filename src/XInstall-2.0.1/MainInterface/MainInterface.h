/*
** MainInterface.h for XInstall in MainInterface/
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
** Started on  Mon Jul 17 19:23:23 1995 Antoine Buat
** Last update Wed Feb  4 15:55:16 1998 Emmanuel Paris
*/

#ifndef _MainInterface_h
#define _MainInterface_h

#include "xcalibur.h"
#if defined(Xc_XMAYDAY) || defined(Xc_XINSTALL)
#define NO_TIME_INDICATOR
#endif

#include "ManageWidget.h" 

#include <X11/Intrinsic.h>
#include <Xm/Form.h>

#ifndef NO_TIME_INDICATOR
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>
#endif

/* define the method for Class MainInterface */
typedef struct
{
  F_STD;
#ifndef NO_TIME_INDICATOR
  void (*create_and_map_StartUp)();
  void (*unmap_StartUp)();
#endif
  void (*create_TopLevel)();
  void (*map_TopLevel)();
  void (*unmap_TopLevel)();
  void (*attach_top)();
  void (*attach_bottom)();
  void (*attach_left)();
  void (*attach_right)();
  void (*set_main_icon)();
  void (*reopen_display)();
} sf_MainInterface;

/* define the MainInterface Class */
typedef struct
{
 sf_MainInterface	*f;
 Widget			w_This;
 Widget			w_Main;
 Widget			w_Hidden;
#ifndef NO_TIME_INDICATOR
 Widget			w_Startup;
 Widget                 w_Frame;
 Widget                 w_RowStartup;
 Widget                 w_LabelStartup1;
 Widget                 w_LabelStartup2;
 Widget                 w_LabelStartup3;
 Widget			w_DAreaStartup;
#endif
 char			*display_name;
} c_MainInterface;

extern sf_MainInterface fc_MainInterface;

#endif


