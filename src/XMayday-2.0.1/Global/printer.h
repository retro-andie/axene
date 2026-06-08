/*
** printer.h for XMayday in Global/
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
** Started on  Mon Jul 17 17:34:52 1995 Emmanuel Paris
** Last update Wed Sep  4 16:21:57 1996 Emmanuel Paris
*/

#ifndef _printer_h_
#define _printer_h_

#include "PageFormat.h"

/*--- Printer list ---*/
typedef struct _printer_t
{
  struct _printer_t	*next;
  char			*name;
  char			*command;
} printer_t;

typedef struct
{
  printer_t	*printer;
  boolean	bDraft;
  boolean	bColor;
  boolean	bBanners;
  boolean	bFootnotes;
  
  int		exporttype;
  int		fonttype;
  page_format_t *page_format;
  
} print_param_t;

extern printer_t	 *GlobPrinterBase;
extern print_param_t	 GlobPrintParam;

#endif /* _printer_h_ */

