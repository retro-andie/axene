/*
** SpecialFileSelector.h for XMayday in FileSelector/
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
** Last update Wed Feb 12 16:03:36 1997 Emmanuel Paris
*/

#ifndef __XC_SPECIAL_FILE_SELECTOR_H__
#define __XC_SPECIAL_FILE_SELECTOR_H__

#include "FileSelector.h"
#include "Resource.h"

extern c_Resource *GlobResources;

/*--- Creation methods ---*/
extern c_FileSelector *CreateOpenIndexSelector ___PROTO((Widget parent));
extern c_FileSelector *CreateOpenFileSelector ___PROTO((Widget parent));
extern c_FileSelector *CreatePrintDocumentSelector ___PROTO((Widget parent,
							     char *pattern));
/*--- Copy methods ---*/
extern c_FileSelector *CopyOpenIndexSelector ___PROTO((Widget parent,
						       c_FileSelector *fs));
extern c_FileSelector *CopyOpenFileSelector ___PROTO((Widget parent,
						       c_FileSelector *fs));
extern c_FileSelector *CopyPrintDocumentSelector ___PROTO((Widget parent,
							 c_FileSelector *fs));

/*--- Destruction methods ---*/

/*--- Manage Methods ---*/

#endif /* !__XC_SPECIAL_FILE_SELECTOR_H__ */
