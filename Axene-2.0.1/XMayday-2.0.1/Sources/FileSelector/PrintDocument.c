/*
** PrintDocument.c for XMayday in FileSelector/
** File selector for printing to a file
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
** Last update Wed Feb 12 16:46:52 1997 Emmanuel Paris
*/

#include "FileSelector.h"
#include "Resource.h"
extern c_Resource *GlobResources;

c_FileSelector *CreatePrintDocumentSelector(w, pattern)
Widget w;
char *pattern;
{
  Arg		 argsw[4];
  XmString       xmsBaratin;
  c_FileSelector *PrintDocumentSelector;
  
  PrintDocumentSelector = NEW(c_FileSelector)(w, "Print", 400, 500);

  if(PrintDocumentSelector == NULL) return NULL;

  xmsBaratin = F(GlobResources).getXmString(GlobResources,
					    XcR_printDocument);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(PrintDocumentSelector->w_Title, argsw, 1);
  XmStringFree(xmsBaratin);

  F(PrintDocumentSelector).addPattern(PrintDocumentSelector, TRUE, pattern);
  return PrintDocumentSelector;
}

c_FileSelector *CopyPrintDocumentSelector(w, fs, pattern)
Widget w;
c_FileSelector *fs;
char *pattern;
{
  c_FileSelector *PrintDocumentSelector;
  
  PrintDocumentSelector = CreatePrintDocumentSelector(w, pattern);
  
  if (PrintDocumentSelector)
    F(PrintDocumentSelector).setPath(PrintDocumentSelector, F(fs).getPath(fs));
  
  return PrintDocumentSelector;
}



