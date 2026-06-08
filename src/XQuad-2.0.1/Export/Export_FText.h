/*
** Export_FText.h for XQuad in Export/
** Header for Export_FText Object
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
** Started on  Sun Dec 15 17:30:39 1996 Robin Castanier
** Last update Wed Mar 12 13:16:45 1997 Robin Castanier
*/

#ifndef _Export_FText_h_
#define _Export_FText_h_

/* forward declaration */
typedef struct sc_Export_FText c_Export_FText;

#include "ExportStd.h"
#include "Export_FText_Interface.h"
#include "encodings.h"

struct sc_Export_FText
{
 sf_Export_Std     *f;
  
 EXPORT_FIELD;	
 Export_FText_box_t	*box;	/* export text attribs box */

 boolean	formula;
 boolean	selection;
 boolean	grid;
 boolean	headers;
 encoding_t	encoding;
};

extern sf_Export_Std fc_Export_FText;
extern c_Resource *GlobResources;

#endif /* _Export_FText_h_ */



