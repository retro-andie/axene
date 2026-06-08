/*
** SpecialFileSelector.h for XAllWrite in FileSelector/
** Definition for special file selectors
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
** Started on  Fri Dec 16 14:36:06 1994 Stéphane Boisson
** Last update Mon Aug 10 14:18:11 1998 Emmanuel Paris
*/

#ifndef __XC_SPECIAL_FILE_SELECTOR_H__
#define __XC_SPECIAL_FILE_SELECTOR_H__

#include "FileSelector.h"
#include "Resource.h"

extern c_FileSelector *ImportImageSelector;
extern c_FileSelector *ImportDocumentSelector;
extern c_FileSelector *ImportVectorSelector;

extern c_FileSelector *OpenDocumentSelector;
extern c_FileSelector *CloseDocumentSelector;
extern c_FileSelector *PrintDocumentSelector;
extern c_Resource *GlobResources;

/*--- Creation methods ---*/
extern boolean CreateOpenDocumentSelector ___PROTO((Widget parent));
extern boolean CreateCloseDocumentSelector ___PROTO((Widget parent));
extern boolean CreatePrintDocumentSelector ___PROTO((Widget parent));

extern boolean CreateImportImageSelector ___PROTO((Widget parent));
extern boolean CreateImportDocumentSelector ___NPROTO((Widget parent, 
						c_Import *import,
						boolean doc_selected));
extern boolean CreateImportVectorSelector ___PROTO((Widget parent));

/*--- Destruction methods ---*/
extern void DestroyImportDocumentSelector ___PROTO((void));

/*--- Manage Methods ---*/
extern void ManageImportDocumentSelector ___PROTO((boolean doc_available));

#endif /* !__XC_SPECIAL_FILE_SELECTOR_H__ */
