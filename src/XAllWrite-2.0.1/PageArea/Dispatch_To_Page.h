/*
** Dispatch_To_Page.h for XAllWrite in PageArea/
** Dispatch function to a page function
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Mon Jun 16 15:55:09 1997 Emmanuel Paris
** Last update Mon Jun 16 16:17:36 1997 Emmanuel Paris
*/

#ifndef _Dispatch_To_Page_h
#define _Dispatch_To_Page_h

#include "PageStd.h"
#include "PageArea.h"

extern void init_Dispatch_To_Page();
extern status_t eventlook_Dispatch_To_Page();
extern void abort_Dispatch_To_Page();
extern void redraw_Dispatch_To_Page();

#ifdef _DTP_PRIVATE_
sf_pagearea_function fc_Dispatch_To_Page =
{
 init_Dispatch_To_Page,
 eventlook_Dispatch_To_Page,
 abort_Dispatch_To_Page,
 redraw_Dispatch_To_Page
};
#else
extern sf_pagearea_function fc_Dispatch_To_Page;
#endif

#endif
