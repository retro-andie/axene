/*
** ExportHTML.c for XQuad in FileSelector/
** File selector for exporting
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
** Started on  Tue Apr 11 18:00:33 1995 Stéphane Boisson
** Last update Wed Dec 18 15:36:29 1996 Robin Castanier
*/

#include "SpecialFileSelector.h"

c_FileSelector *ExportHTMLSelector = NULL;

boolean CreateExportHTMLSelector(w)
Widget w;
{
  Arg		 argsw[4];
  XmString       xmsBaratin;

  ExportHTMLSelector = NEW(c_FileSelector)(w, "ExportHTML", 400, 500);

  if(ExportHTMLSelector == NULL) return FALSE;

  xmsBaratin = F(GlobResources).getXmString(GlobResources,
					    XcR_exportDocument);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(ExportHTMLSelector->w_Title, argsw, 1);
  XmStringFree(xmsBaratin);

  F(ExportHTMLSelector).addPattern(ExportHTMLSelector, TRUE, "*.html");
  return TRUE;
}
