/*
** VectorGraphP.h for XQuad in VectorGraph/
** Private header for VectorGraph class
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
** Started on  Tue Mar  7 18:41:31 1995 Stéphane Boisson
** Last update Thu Nov 16 11:44:54 1995 Emmanuel Paris
*/


#ifndef __XC_VECTOR_GRAPH_P_H__
#define __XC_VECTOR_GRAPH_P_H__

#include "VectorGraph.h"
#include "ObjectStd.h"

/*--- dispatch.c ---*/
void vectorSetMode ___PROTO((c_VectorGraph *this,
			     vector_mode_t mode, d_object_function *data));
void vectorVerticalFlip ___PROTO((c_VectorGraph *this,
				  d_object_function *data));
void vectorHorizontalFlip ___PROTO((c_VectorGraph *this,
				    d_object_function *data));
void vectorCenter ___PROTO((c_VectorGraph *this, d_object_function *data));

/*--- docfile.c ---*/
boolean readVectorGraph ___NPROTO((c_DocFile *doc, char *keyword,
				  long param, c_Cadre *frame));
boolean writeVectorGraph ___NPROTO((c_VectorGraph *this, c_DocFile *doc));

#endif /* !__XC_VECTOR_GRAPH_P_H__ */
