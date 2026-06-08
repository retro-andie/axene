/*
** ImportP.h for XAllWrite in Import/
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
** Last update Mon Aug 10 16:05:52 1998 Emmanuel Paris
*/

#ifndef _ImportP_h_
#define _ImportP_h_

typedef struct 
{
 sf_Import_Std		*import_methods;
 void			*import_init_parameter;
} import_init_t;

extern sf_Import_Std	fc_Import_Text;

static import_init_t	import_init[] = 
{
 { &fc_Import_Text, (void *)0 },	/* for text importation */
 { NULL, NULL }				/* set end of import format list */
};

#endif /* _ImportP_h_ */
