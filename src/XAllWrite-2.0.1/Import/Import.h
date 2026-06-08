/*
** Import.h for XAllWrite in Import/
** Header for Import Object
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
** Started on  Wed Feb 22 16:59:27 1995 Emmanuel Paris
** Last update Mon Aug 10 14:23:10 1998 Emmanuel Paris
*/

#ifndef _Import_h_
#define _Import_h_

#include "xcalibur.h"
#include "ImportStd.h"

typedef struct sc_Import c_Import;

typedef struct
{
  F_STD;
  void	(*import) ___PROTO((c_Import *This, c_Document *current_document));
} sf_Import;

struct sc_Import
{
  sf_Import     *f;
  
  int		count_import_format;
  c_Import_Std	**import_format;
  
  int		format_selected;
  c_Document	*current_document;
};

extern sf_Import fc_Import;

#endif /* _Import_h_ */


