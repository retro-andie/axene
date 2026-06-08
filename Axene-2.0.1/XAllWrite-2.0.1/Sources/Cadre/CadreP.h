/*
** CadreP.h for Xclamation, XQuad and XAllWrite in Cadre/
** Private header for Cadre class
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
** Started on  Tue Dec 27 12:51:15 1994 Stéphane Boisson
** Last update Fri Aug  7 14:33:17 1998 Emmanuel Paris
*/

#ifndef __XC_CADRE_P_H__
#define __XC_CADRE_P_H__

#include "Cadre.h"
#include "DocFile.h"
#include "BaseStd.h"

boolean readCadre ___PROTO((c_DocFile *doc, char *keyword,
			    scale_t factor, BaseStd_t *base));
boolean printCadre ___PROTO((c_Cadre *this, c_PostScript *post));

#endif /* !__XC_CADRE_P_H__ */
