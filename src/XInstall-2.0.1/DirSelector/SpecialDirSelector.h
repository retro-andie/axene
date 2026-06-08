/*
** SpecialDirSelector.h for XInstall in DirSelector/
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
** Last update Tue Jan 13 13:42:07 1998 Robin Castanier
*/

#ifndef __XC_SPECIAL_DIR_SELECTOR_H__
#define __XC_SPECIAL_DIR_SELECTOR_H__

#include "DirSelector.h"
#include "Resource.h"

extern c_DirSelector *BrowseDirectorySrc;
extern c_DirSelector *BrowseDirectoryDst;
extern c_Resource *GlobResources;

/*--- Creation methods ---*/
extern boolean CreateBrowseDirectorySrc ___PROTO((Widget parent));
extern boolean CreateBrowseDirectoryDst ___PROTO((Widget parent));

/*--- Destruction methods ---*/

/*--- Manage Methods ---*/

#endif /* !__XC_SPECIAL_DIR_SELECTOR_H__ */
