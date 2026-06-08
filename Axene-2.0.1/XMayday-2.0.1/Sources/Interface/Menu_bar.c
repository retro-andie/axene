/*
** Menu_bar.c for XMayday in Interface/
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
** Last update Sun Aug  8 18:25:32 1999 Emmanuel Paris
*/

#include "Interface.h"
#include "InterfaceP.h"

/* ------------------------------------------------------------------------ **
** Menu Bar Creation --                                                     ** 
** ------------------------------------------------------------------------ */
void create_menu_bar(This)
c_Interface *This;
{
 Arg      args[1];
  
 This->MenuBar = (c_MenuBar *)NEW(c_MenuBar)(WIDGET(This->MainInterface), 
					     This->Manager, "Menu");
 /*--- Menu File ---*/
  
 This->Menu[MENU_FILE] = (c_PulldownMenu *)
  NEW(c_PulldownMenu)(WIDGET(This->MenuBar), This->Manager, 
		      "pmFile",
		      "mNewXMayday", MF_NEW_XMAYDAY,
		      "mOpenIndex", MF_OPEN_INDEX,
		      "mOpenFile", MF_OPEN_FILE,
		      MENUSEPARATOR, MF_SEPARATOR1,
		      "mPrint", MF_PRINT,
		      MENUSEPARATOR, MF_SEPARATOR2,
		      "mClose", MF_CLOSE,
		      "mQuit", MF_QUIT,
		      NULL);
  
 F(This->Menu[MENU_FILE]).AddCallback
  (This->Menu[MENU_FILE], MF_NEW_XMAYDAY, MenuFileNewXMayday, This);
 F(This->Menu[MENU_FILE]).AddCallback
  (This->Menu[MENU_FILE], MF_OPEN_INDEX, MenuFileOpenIndex, This);
 F(This->Menu[MENU_FILE]).AddCallback
  (This->Menu[MENU_FILE], MF_OPEN_FILE, MenuFileOpen, This);
 F(This->Menu[MENU_FILE]).AddCallback
  (This->Menu[MENU_FILE], MF_PRINT, MenuFilePrint, This);
 F(This->Menu[MENU_FILE]).AddCallback
  (This->Menu[MENU_FILE], MF_CLOSE, MenuFileClose, This);
 F(This->Menu[MENU_FILE]).AddCallback
  (This->Menu[MENU_FILE], MF_QUIT, MenuFileQuit, This);
  
 /*--- Menu Display ---*/
  
 This->Menu[MENU_DISPLAY] = (c_PulldownMenu *)
  NEW(c_PulldownMenu)(WIDGET(This->MenuBar), This->Manager,
		      "pmDisplay",
		      "mRefresh", MD_REDISPLAY,
		      "mReload", MD_RELOAD,
		      "mTransfert", MD_COPY_INDEX,
		      MENUSEPARATOR, MD_SEPARATOR1,
		      NOMANAGE, NOTHING,
		      "mMaskIndex", MD_MASK_INDEX,
		      "mShowIndex", MD_UNMASK_INDEX,
		      NOMANAGE, NOTHING,
		      "mShowInfobar", MD_UNMASK_BOTTOMBAR,
		      "mMaskInfobar", MD_MASK_BOTTOMBAR,
		      NULL);
  
 F(This->Menu[MENU_DISPLAY]).AddCallback
  (This->Menu[MENU_DISPLAY], MD_REDISPLAY, RefreshAll, This);
 F(This->Menu[MENU_DISPLAY]).AddCallback
  (This->Menu[MENU_DISPLAY], MD_RELOAD, ReloadAll, This);
 F(This->Menu[MENU_DISPLAY]).AddCallback(This->Menu[MENU_DISPLAY],
					 MD_COPY_INDEX, SelectFunction,
					 (void *)CREATE_INDEX);
 F(This->Menu[MENU_DISPLAY]).AddCallback
  (This->Menu[MENU_DISPLAY], MD_MASK_INDEX,  DisplayIndex, This);
 F(This->Menu[MENU_DISPLAY]).AddCallback
  (This->Menu[MENU_DISPLAY], MD_UNMASK_INDEX, DisplayIndex, This);
 F(This->Menu[MENU_DISPLAY]).AddCallback
  (This->Menu[MENU_DISPLAY], MD_MASK_BOTTOMBAR, DisplayBottomBar, This);
 F(This->Menu[MENU_DISPLAY]).AddCallback
  (This->Menu[MENU_DISPLAY], MD_UNMASK_BOTTOMBAR, DisplayBottomBar, This);
  
  
 /*--- Menu Navigate ---*/
  
 This->Menu[MENU_NAVIGATE] = (c_PulldownMenu *)
  NEW(c_PulldownMenu)(WIDGET(This->MenuBar), This->Manager,
		      "pmGo",
		      "mBack", MN_GOTO_PREV,
		      "mForward", MN_GOTO_NEXT,
		      "mHome", MN_GOTO_INDEX,
		      MENUSEPARATOR, MN_SEPARATOR1,
		      RADIOBOXON, 0,
		      DYNAMIC, MN_URL_LIST,
		      0, HM_URL_LIST_LENGTH,
		      NULL);

 F(This->Menu[MENU_NAVIGATE]).AddCallback(This->Menu[MENU_NAVIGATE],
					  MN_GOTO_PREV, SelectFunction, 
					  (void *)VIEW_PREVIOUS_ANCHOR);
 F(This->Menu[MENU_NAVIGATE]).AddCallback(This->Menu[MENU_NAVIGATE],
					  MN_GOTO_NEXT, SelectFunction, 
					  (void *)VIEW_NEXT_ANCHOR);
 F(This->Menu[MENU_NAVIGATE]).AddCallback(This->Menu[MENU_NAVIGATE],
					  MN_GOTO_INDEX, SelectFunction,
					  (void *)VIEW_HOME_PAGE);
 /*--- Menu Help ---*/
 This->Menu[MENU_HELP] = (c_PulldownMenu *)
  NEW(c_PulldownMenu)(WIDGET(This->MenuBar), This->Manager,
		      "pmHelp",
		      "mOverview", MH_OVERVIEW,
		      "mContent", MH_CONTENT,
		      "mKeyboard", MH_KEYBOARD,
		      MENUSEPARATOR, MH_SEPARATOR1,
		      "mOnItem", MH_ONITEM,
		      MENUSEPARATOR, MH_SEPARATOR2,
		      "mUsingHelp", MH_USING_HELP,
		      MENUSEPARATOR, MH_SEPARATOR3,
		      "mCache", MH_IMAGE_CACHE,
		      "mAbout", MH_ABOUT,
		      NULL); 
  
 F(This->Menu[MENU_HELP]).AddCallback
  (This->Menu[MENU_HELP], MH_OVERVIEW, MenuHelpCallHelp, NULL);
 F(This->Menu[MENU_HELP]).AddCallback
  (This->Menu[MENU_HELP], MH_CONTENT, MenuHelpCallHelp, NULL);
 F(This->Menu[MENU_HELP]).AddCallback
  (This->Menu[MENU_HELP], MH_KEYBOARD, MenuHelpCallHelp, NULL);
 F(This->Menu[MENU_HELP]).AddCallback
  (This->Menu[MENU_HELP], MH_ONITEM, MenuHelpTrackHelp, This);
 F(This->Menu[MENU_HELP]).AddCallback
  (This->Menu[MENU_HELP], MH_USING_HELP, MenuHelpCallHelp, NULL);
 F(This->Menu[MENU_HELP]).AddCallback
  (This->Menu[MENU_HELP], MH_IMAGE_CACHE, MenuHelpCache, This);
 F(This->Menu[MENU_HELP]).AddCallback
  (This->Menu[MENU_HELP], MH_ABOUT, MenuHelpAbout, This);
  
 XtSetArg(args[0], XmNmenuHelpWidget, This->Menu[MENU_HELP]->w_Cascade);
 XtSetValues(WIDGET(This->MenuBar), args, 1);
}

/* ------------------------------------------------------------------------ **
** Menu Bar Destruction --                                                  ** 
** ------------------------------------------------------------------------ */
void destroy_menu_bar(This)
c_Interface *This;
{
 int i;
  
 for(i = 0; i < NB_MENU; i++)
 {
  DELETE(c_PulldownMenu)(This->Menu[i]);
 }
  
 DELETE(c_MenuBar)(This->MenuBar);
}






