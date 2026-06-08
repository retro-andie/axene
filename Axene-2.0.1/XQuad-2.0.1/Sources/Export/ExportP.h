/*
** ExportP.h for XQuad in Export/
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
** Started on  Sun Dec 15 17:21:59 1996 Robin Castanier
** Last update Sun Dec 15 20:16:44 1996 Robin Castanier
*/

#ifndef _ExportP_h_
#define _ExportP_h_

typedef struct {
  sf_Export_Std		*export_methods;
  void			*export_init_parameter;
} export_init_t;

extern sf_Export_Std	fc_Export_Text;
extern sf_Export_Std	fc_Export_FText;

static export_init_t	export_init[] = {
  { &fc_Export_Text, (void *)0 },	/* for text unix separator tab */
  { &fc_Export_FText, (void *)0 },	/* for text unix separator space */
  { &fc_Export_Text, (void *)1 },	/* for text unix separator ; */
  { &fc_Export_FText, (void *)1 },	/* for text dos separator space */
  { &fc_Export_Text, (void *)2 },	/* for text dos separator tab */
  { &fc_Export_Text, (void *)3 },	/* for text dos separator ; */
  { &fc_Export_FText, (void *)2 },	/* for text mac separator space */
  { &fc_Export_Text, (void *)4 },	/* for text mac separator tab */
  { &fc_Export_Text, (void *)5 },	/* for text mac separator ; */
  { NULL, NULL }    /* set end of export format list */
};

#endif /* _ExportP_h_ */
