/*
** check.h for XMayday in Main/
** functions that check the existence of various necessary environment
** variables and files
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Mon Jan 29 18:55:24 1996 Emmanuel Paris
** Last update Mon Jan 29 18:55:24 1996 Emmanuel Paris
*/

#ifndef _check_h_
#define _check_h_

#include "xcalibur.h"

#ifndef Xc_XTOP
#define Xc_XTOP "/usr/X11R6/lib/X11"
#endif

#define Xc_MXTOP "X11"

#define XcE_XKEYSYMDB "XKEYSYMDB"
#define XcF_XKEYSYMDB "XKeysymDB"

#ifdef Xc_NEED_LOCALE
#define XcE_LOCALEDIR "XLOCALEDIR"
#define XcF_LOCALEDIR "locale"
#define XcF_LOCALE_DIR	 "locale.dir"
#define XcF_LOCALE_ALIAS "locale.alias"
#endif

#ifdef Xc_NEED_NLS
#define XcE_NLSPATH "XNLSPATH"
#define XcF_NLSPATH "nls"
#define XcF_NLS_DIR "nls.dir"
#endif

extern void check_configuration();

#endif /* _check_h_ */


