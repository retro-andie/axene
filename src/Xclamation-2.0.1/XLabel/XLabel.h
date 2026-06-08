/*
** XLabel.h for Xclamation, XQuad, XAllWrite and XInstall in XLabel/
** Label with direct X display. 
**
** Copyright (C) 1994-2000 Axene.
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
** Started on  Mon Jul 17 19:01:54 1995 Robin Castanier
** Last update Tue Oct 21 03:52:07 1997 Emmanuel Paris
*/

#ifndef _XLabel_h
#define _XLabel_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include <Xm/Label.h>

/*--- foreward declaration ---*/
typedef struct _c_XLabel  c_XLabel;

enum {
 XlALIGNMENT_BEGINNING, XlALIGNMENT_CENTER, XlALIGNMENT_END
} ;

/* defint the method for Class XLabel */
typedef struct 
{
  F_STD;
  void (*set_string) ___PROTO((c_XLabel *This, char *string));
} sf_XLabel;

/* define the XLabel Class */
struct _c_XLabel
{
 sf_XLabel	*f;
 Widget		w_This;
 Display	*display;
 Window		window;
 GC		gc;
 XFontStruct	*font_struct;
 int		fontres;
 int		width;
 int		height;
 int		alignment;
 int		font_height;
 boolean	mapped;
 char		*string;
 char		*oldstring;
};

extern sf_XLabel fc_XLabel;

#endif




