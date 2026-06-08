/*
** HelpWidget.h for XMayday in HelpWidget/
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Fri May 10 16:46:22 1996 Emmanuel Paris
** Last update Thu Sep  5 18:17:22 1996 Emmanuel Paris
*/

#ifndef _HelpWidget_h
#define _HelpWidget_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include "Cursor.h"
#include "Image.h"
#include "HTML.h"

typedef struct sc_HelpWidget c_HelpWidget;

typedef struct _hw_image_t
{
  c_Image	*Image;
  ImageInfo	*ImageInfo;
  struct _hw_image_t *Next;
} hw_image_t;

/* define the method for Class HelpWidget */
typedef struct
{
  F_STD;
  boolean (*set_url) ___PROTO((c_HelpWidget *This, char *filename,
			       char *anchor));
  void    (*add_url) ___PROTO((c_HelpWidget *This, char *anchor));
  boolean (*convert_anchor) ___PROTO((c_HelpWidget *This, char *href,
				      char **filename, char **local_anchor));
  void	  (*reload) ___PROTO((c_HelpWidget *This));
  void	  (*refresh) ___PROTO((c_HelpWidget *This));
  void	  (*get_scroll_pos) ___PROTO((c_HelpWidget *This, int *vx, int *vy));
  void	  (*set_scroll_pos) ___PROTO((c_HelpWidget *This, int vx, int vy));
} sf_HelpWidget;

/* define the HelpWidget Class */
struct sc_HelpWidget
{
  sf_HelpWidget	*f;
  Widget	w_This;
  hw_image_t	*image_list;
  int		num_colors;
  int		*reds;
  int		*blues;
  int		*greens;
  char		*url_filename;
  char		*url_anchor;
  char		*link_base;
  boolean	link_set;
  c_Cursor	*anchor_cursor;
};

extern sf_HelpWidget fc_HelpWidget;

#endif




