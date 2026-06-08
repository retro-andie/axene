/*
** High_GraphP.h for XQuad in High_Graph/
** Private header for High_Graph class
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
** Started on  Mon Aug 28 18:46:42 1995 Stéphane Boisson
** Last update Tue Aug 29 15:07:34 1995 Stéphane Boisson
*/

#ifndef __XC_HIGH_GRAPH_P_H__
#define __XC_HIGH_GRAPH_P_H__

#include "High_Graph.h"

boolean readHighGraph ___PROTO((c_DocFile *doc, char *keyword,
				long param, c_VectorGraph *vector));
boolean writeHighGraph ___PROTO((c_HighGraph *this, c_DocFile *doc));

#endif /* !__XC_HIGH_GRAPH_P_H__ */
