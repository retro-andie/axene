/*
** DocIcon.h for AxeneOffice in DocIcon/
** Object Header for Loading Document Icons
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
** Last update Tue Jan 13 09:35:45 1998 Robin Castanier
*/

#ifndef _DocIcon_h_
#define _DocIcon_h_

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "Xpm.h"

typedef struct sc_DocIcon c_DocIcon;

typedef enum
{
 Icon_BACKGROUND,
 Icon_BACKGROUND_light,
 NB_ICON_DOCUMENT 
} doc_icon_t;

typedef struct
{
 F_STD;
 Pixmap (*get_pixmap) ___PROTO((c_DocIcon *This, doc_icon_t num));
#ifndef NO_INTRINSIC
 void (*registerPixmapConverters) ___PROTO((c_DocIcon *This));
#endif
} sf_DocIcon;

struct sc_DocIcon
{
 sf_DocIcon     *f;
  
 Display	*display;
 Pixmap	icon_Doc[NB_ICON_DOCUMENT];
};

extern sf_DocIcon fc_DocIcon;

#endif /* _DocIcon_h_ */

#ifdef DOC_ICON_PRIVATE

#include "../Pixmaps/back_app.zpm"
#include "../Pixmaps/back_light.zpm"

static char **IconDocumentXPM[NB_ICON_DOCUMENT] =
{ back_app_xpm, back_light_xpm };

typedef struct 
{
 char		*resource_name;
 doc_icon_t	resource_icon;
} res_icon_t;

static res_icon_t IconResource[] =
{ { "leather_dark", Icon_BACKGROUND},
  { "leather_light", Icon_BACKGROUND_light},
  { NULL, 0}
};

#endif


