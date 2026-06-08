/*
** Interface.h for AxeneOffice in Interface/
** Interface Part of XMayday
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
** Started on  Mon Jul 17 15:05:00 1995 Antoine Buat
** Last update Mon Jan 12 17:07:05 1998 Robin Castanier
*/

#ifndef _interface_h_
#define _interface_h_

#include "xcalibur.h"

#include "WInterface.h"
#include "Resource.h"
#include "MainInterface.h"
#include "ManageWidget.h"
#include "RowColumn.h"
#include "PopupMenu.h"
#include "Icon.h"

#define RIEN 1
#define POPUP_OVERRIDE         1
#define POPUP_TRANSIENT        2
#define POPUP_FLIP_VERTICAL    3
#define POPUP_FLIP_HORIZONTAL  4
#define POPUP_SEP1             5
#define POPUP_SUBMENU1         6
#define POPUP_LAUNCH           7
#define POPUP_XCLAMATION       8
#define POPUP_XQUAD            9
#define POPUP_XALLWRITE        10
#define POPUP_XMAYDAY          11
#define POPUP_SEP2             12
#define POPUP_QUIT             13

#define NB_SOFT                4

typedef struct sc_Interface c_Interface;

typedef struct
{
 F_STD;
 c_Interface *(*get_by_widget) ___PROTO((c_Interface *base, Widget widget));
 c_Interface *(*get_by_window) ___PROTO((c_Interface *base, Window window));
} sf_Interface;

struct sc_Interface
{
 sf_Interface	  *f;
 c_MainInterface  *MainInterface;
 c_ManageWidget   *Manager;
  
 c_RowColumn	*rcMain;
  
 c_Icon		*Icon[NB_SOFT];

 c_PopupMenu	*popup;

 int            nbIcon;
 char		AppTitle[64];
 boolean	override;
 boolean	transient;
 boolean        flip;

 struct sc_Interface	*Prev;
 struct sc_Interface	*Next;
};

extern sf_Interface fc_Interface;

#endif /* _interface_h_ */











