/*
** BrowseDirectoryDst.c for XInstall in DirSelector/
** File selector for OpenFile
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
** Started on  Fri Dec 16 15:12:21 1994 Stéphane Boisson
** Last update Tue Jan 13 15:10:49 1998 Robin Castanier
*/

#include "DirSelector.h"
#include "Resource.h"
extern c_Resource *GlobResources;

c_DirSelector *BrowseDirectoryDst = NULL;

boolean CreateBrowseDirectoryDst(w)
Widget w;
{
 Arg		argsw[4];
 XmString       xmsBaratin;

 BrowseDirectoryDst = NEW(c_DirSelector)(w, "BrowseDirectoryDst",
					  600, 360, 
					  XcDS_FREE_SPACE | XcDS_CREATE_DIR);
 if(BrowseDirectoryDst == NULL)
  return FALSE;
 BrowseDirectoryDst->force_extension = FALSE;
 xmsBaratin = F(GlobResources).getXmString(GlobResources,
					   XcR_browseDirectoryDst);
 XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
 XtSetValues(BrowseDirectoryDst->w_Title, argsw, 1);
 XmStringFree(xmsBaratin);
 return TRUE;
}
