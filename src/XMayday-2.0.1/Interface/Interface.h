/*
** Interface.h for XMayday in Interface/
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
** Started on  Mon Jul 17 15:05:00 1995 Emmanuel Paris
** Last update Fri Feb  4 10:58:50 2000 Emmanuel Paris
*/

#ifndef _interface_h_
#define _interface_h_

#include "xcalibur.h"

#include "WInterface.h"
#include "Resource.h"
#include "MainInterface.h"
#include "ManageWidget.h"
#include "MenuBar.h"
#include "PulldownMenu.h"
#include "RowColumn.h"
#include "PanedWindow.h"
#include "Frame.h"
#include "Form.h"
#include "Icon.h"
#include "Label.h"
#include "HelpLine.h"
#include "HelpWidget.h"
#include "HelpManager.h"
#include "SpecialFileSelector.h"

typedef struct sc_Interface c_Interface;

enum 
{
 MENU_FILE, MENU_DISPLAY, MENU_NAVIGATE, MENU_HELP, NB_MENU
}/* Menu Bar entries */;

#define NOTHING	1
enum 
{
 MF_NEW_XMAYDAY = 1, MF_OPEN_INDEX, MF_OPEN_FILE, MF_SEPARATOR1, MF_PRINT,
 MF_SEPARATOR2, MF_CLOSE, MF_QUIT
};				/* Menu File entries */

enum 
{
 MD_REDISPLAY = 1, MD_RELOAD, MD_COPY_INDEX, MD_SEPARATOR1, MD_UNMASK_INDEX,
 MD_MASK_INDEX, MD_UNMASK_BOTTOMBAR, MD_MASK_BOTTOMBAR
};				/* Menu Display entries */

enum 
{
 MN_GOTO_PREV = 1, MN_GOTO_NEXT , MN_GOTO_INDEX, MN_SEPARATOR1, MN_URL_LIST 
};				/* Menu Navigate entries */

enum 
{
 MH_OVERVIEW, MH_CONTENT, MH_KEYBOARD, MH_SEPARATOR1, 
 MH_ONITEM, MH_SEPARATOR2, MH_USING_HELP, 
 MH_SEPARATOR3, MH_IMAGE_CACHE, MH_ABOUT
};				/* Menu Help entries */

enum 
{
 I_NAVIGATE_PREV, I_NAVIGATE_NEXT, I_NAVIGATE_INDEX, I_RADIAN, I_OPEN, 
 I_MOVEI, I_PRINT, NB_ICON
};				/* Icon Bar items */

typedef struct
{
 F_STD;
 c_Interface *(*get_by_widget) ___PROTO((c_Interface *base, Widget widget));
 c_Interface *(*get_by_window) ___PROTO((c_Interface *base, Window window));
} sf_Interface;

struct sc_Interface
{
 sf_Interface	*f;
 c_MainInterface *MainInterface;
 c_ManageWidget	*Manager;
  
 c_MenuBar	*MenuBar;
 c_PulldownMenu	*Menu[NB_MENU];
  
 Widget		fPaned;
 c_PanedWindow	*Paned;
 c_Form		*foBottom;
 Widget		fIconBar;
 c_Form         *foIconBar;
 Widget		fuIconBar;
 c_Form 	*IconBar;
 c_RowColumn	*IconBar2;
 c_Icon		*Icon[NB_ICON];
  
 c_Form		*BottomBar;
 c_Icon		*IconBottom;
 c_Label	*lHelpLine;
  
 c_HelpLine	*HelpLine;
 c_HelpWidget	*HelpWIndex;
 c_HelpWidget	*HelpWHelp;
 c_HelpManager	*HelpManager;
  
 c_FileSelector	*OpenFileSelector;
 c_FileSelector	*OpenIndexSelector;
 c_FileSelector	*PrintDocumentSelector;
  
 boolean	PreviousAlreadyGrised;
 boolean	NextAlreadyGrised;
 boolean	RefreshAlreadyGrised;
 boolean	IndexAlreadyGrised;
 boolean	ICreationAlreadyGrised;
 boolean	CloseAlreadyGrised;
 boolean	IndexExist;
  
 boolean	ButtonMaskIndex;
 boolean	ButtonMaskBottomBar;
  
 char		AppTitle[64];
  
 struct sc_Interface	*Prev;
 struct sc_Interface	*Next;
};

extern sf_Interface fc_Interface;

#endif /* _interface_h_ */











