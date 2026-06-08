/*
** InterfaceP.h for XMayday in Interface/
** Interface Private Part of XMayday
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
** Last update Fri Feb  4 10:59:07 2000 Emmanuel Paris
*/

#ifndef _interfaceP_h_
#define _interfaceP_h_

extern void MenuFileNewXMayday();
extern void MenuFileOpenIndex();
extern void MenuFileOpen();
extern void MenuFilePrint();
extern void MenuFileClose();
extern void MenuFileQuit();

extern void ReloadAll();
extern void RefreshAll();
extern void DisplayIndex();
extern void DisplayBottomBar();
extern void SendRefreshRequest();

extern void MenuHelpCache();
extern void MenuHelpAbout();
extern void MenuHelpCallHelp();
extern void MenuHelpTrackHelp();
extern void SelectFunction();


#endif /* _interfaceP_h_ */


