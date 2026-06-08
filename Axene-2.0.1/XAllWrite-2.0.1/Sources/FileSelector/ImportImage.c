/*
** ImportImage.c for XAllWrite in FileSelector/
** File selector for image import
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
** Last update Mon Jan 23 16:32:42 1995 Robin Castanier
*/

#include "FileSelector.h"
#include "Resource.h"
extern c_Resource *GlobResources;

c_FileSelector *ImportImageSelector = NULL;

boolean CreateImportImageSelector(w)
Widget w;
{
  Arg		 argsw[4];
  XmString       xmsBaratin;
  
  ImportImageSelector = NEW(c_FileSelector)(w, "imageImport", 400, 500);
  xmsBaratin = F(GlobResources).getXmString(GlobResources,
					    XcR_loadImage);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(ImportImageSelector->w_Title, argsw, 1);
  XmStringFree(xmsBaratin);

  return (ImportImageSelector == NULL) ? FALSE : TRUE;
}
