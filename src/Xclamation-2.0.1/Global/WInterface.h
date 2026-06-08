/*
** WInterface.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Global/
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
** Started on  Mon Jul 17 17:39:14 1995 Antoine Buat
** Last update Thu Jan 16 22:24:22 1997 Antoine Buat
*/


#ifndef _WInterface_h
#define _WInterface_h

/* short cut for widget access */
#define WIDGET(a) (a->w_This)
#define MANAGER(a) (a->manage_widget)

#define CALLBACK XtAddCallback
#define REMOVE_CALLBACK XtRemoveCallback
#define FUNC_CANCEL   XmNcancelCallback
#define FUNC_OK       XmNokCallback
#define FUNC_ARM      XmNarmCallback
#define FUNC_DISARM   XmNdisarmCallback
#define FUNC_ACTIVATE XmNactivateCallback
#define FUNC_VALUE    XmNvalueChangedCallback

#endif






