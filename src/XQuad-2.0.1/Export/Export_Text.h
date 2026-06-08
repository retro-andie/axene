/*
** Export_Text.h for XQuad in Export/
** Header for Export_Text Object
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
** Last update Wed Dec 18 17:27:24 1996 Antoine Buat
*/

#ifndef _Export_Text_h_
#define _Export_Text_h_

/* forward declaration */
typedef struct sc_Export_Text c_Export_Text;

#include "ExportStd.h"
#include "Export_Text_Interface.h"
#include "encodings.h"

#define SEP_SIZE	30

struct sc_Export_Text
{
  sf_Export_Std     *f;
  
  EXPORT_FIELD;	
  export_text_box_t	*box;  /* export text attribs box */

  boolean	formula;
  boolean	selection;
  char		sep_cmd[SEP_SIZE];
  encoding_t	encoding;
};

extern sf_Export_Std fc_Export_Text;
extern c_Resource *GlobResources;

#endif /* _Export_Text_h_ */
