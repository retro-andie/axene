/*
** Import_Text.h for XAllWrite in Import/
** Header for Import_Text Object
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
** Last update Mon Aug 10 15:58:26 1998 Emmanuel Paris
*/

#ifndef _Import_Text_h_
#define _Import_Text_h_

/* forward declaration */
typedef struct sc_Import_Text c_Import_Text;

#include "ImportStd.h"
#include "Import_Text_Interface.h"
#include "Resource.h"
#include "encodings.h"

struct sc_Import_Text
{
 sf_Import_Std     *f;
  
 IMPORT_FIELD;	
 import_text_box_t	*box;	/* import text attribs box */

 boolean	new_doc;
 boolean	suppress_newline;
 encoding_t	encoding;
};

extern sf_Import_Std fc_Import_Text;
extern c_Resource *GlobResources;

#endif /* _Import_Text_h_ */



