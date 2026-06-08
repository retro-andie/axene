/*
** FeuilleP.h for XQuad in Feuille/
** Private header for Feuille class
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
** Started on  Fri Aug 25 11:59:28 1995 Stéphane Boisson
** Last update Tue Feb 27 14:01:11 1996 Stéphane Boisson
*/

#ifndef __XC_FEUILLE_P_H__
#define __XC_FEUILLE_P_H__

#include "Feuille.h"
#include <X11/keysym.h>

boolean readFeuille ___PROTO((c_DocFile *doc, char *keyword,
			      long param, c_Feuille *this));
boolean writeFeuille ___PROTO((c_Feuille *this, c_DocFile *doc));

void feuilleGetBoundingRegion ___PROTO((c_Feuille *this, region_t *region));
void feuilleGetPagesCount ___PROTO((c_Feuille *this, region_t *region,
				   unsigned int *wp, unsigned int *hp));
scale_t feuilleComputeScaling ___PROTO((c_Feuille *this, region_t *region,
				       unsigned int w, unsigned int h));

#endif /* !__XC_FEUILLE_P_H__ */
