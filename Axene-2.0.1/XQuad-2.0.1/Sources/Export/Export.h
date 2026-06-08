/*
** Export.h for XQuad in Export/
** Header for Export Object
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
** Started on  Sun Dec 15 17:20:11 1996 Robin Castanier
** Last update Sun Dec 15 17:20:25 1996 Robin Castanier
*/

#ifndef _Export_h_
#define _Export_h_

#include "xcalibur.h"
#include "ExportStd.h"

typedef struct sc_Export c_Export;

typedef struct
{
  F_STD;
  void	(*export) ___PROTO((c_Export *This, c_Tableau *current_sheet));
} sf_Export;

struct sc_Export
{
  sf_Export     *f;
  
  int		count_export_format;
  c_Export_Std	**export_format;
  
  int		format_selected;
  c_Tableau	*current_sheet;
};

extern sf_Export fc_Export;

#endif /* _Export_h_ */


