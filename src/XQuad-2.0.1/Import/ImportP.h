/*
** ImportP.h for XQuad in Import/
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
** Last update Fri Nov 29 15:12:07 1996 Emmanuel Paris
*/

#ifndef _ImportP_h_
#define _ImportP_h_

typedef struct {
  sf_Import_Std		*import_methods;
  void			*import_init_parameter;
} import_init_t;

extern sf_Import_Std	fc_Import_Text;
extern sf_Import_Std	fc_Import_Excel;
extern sf_Import_Std	fc_SYLKDocFile;

static import_init_t	import_init[] = {
  { &fc_Import_Text, (void *)0 },	/* for text unix separator space */
  { &fc_Import_Text, (void *)1 },	/* for text unix separator tab */
  { &fc_Import_Text, (void *)2 },	/* for text unix separator ; */
  { &fc_Import_Text, (void *)3 },	/* for text dos separator space */
  { &fc_Import_Text, (void *)4 },	/* for text dos separator tab */
  { &fc_Import_Text, (void *)5 },	/* for text dos separator ; */
  { &fc_Import_Text, (void *)6 },	/* for text mac separator space */
  { &fc_Import_Text, (void *)7 },	/* for text mac separator tab */
  { &fc_Import_Text, (void *)8 },	/* for text mac separator ; */
/*  { &fc_SYLKDocFile, (void *)0 }, */	/* for SYLK */
  { &fc_Import_Excel, (void *)2},	/* for Excel 2 */
  { &fc_Import_Excel, (void *)3},	/* for Excel 3 */
  { &fc_Import_Excel, (void *)4},	/* for Excel 4 */
  { NULL, NULL }    /* set end of import format list */
};

#endif /* _ImportP_h_ */
