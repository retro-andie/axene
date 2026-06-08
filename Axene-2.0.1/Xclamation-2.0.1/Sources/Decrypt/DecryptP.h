/*
** DecryptP.h for Xclamation, XQuad and XAllWrite in Decrypt/
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
** Started on  Wed Feb 22 16:59:27 1995 Emmanuel Paris
** Last update Thu Oct  8 13:28:11 1998 Antoine Buat
*/

#ifndef _DecryptP_h_
#define _DecryptP_h_

#include "typeseed.h"

#ifdef Xc_XCLAMATION
#define Xc_CRYPT_SEED2 36843
#endif

#ifdef Xc_XQUAD
#define Xc_CRYPT_SEED2 18736
#endif

#ifdef Xc_XALLWRITE
#define Xc_CRYPT_SEED2 73641
#endif

#define Xc_CRYPT_SEED3 67731

#define TABLE_SIZE 94

#endif /* _DecryptP_h_ */










