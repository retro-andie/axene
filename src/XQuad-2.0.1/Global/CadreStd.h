/*
** CadreStd.h for Xclamation, XQuad and XAllWrite in Global/
** Interface between Cadre Class and Objects for display
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
** Started on  Fri Jun 10 15:59:52 1994 Emmanuel Paris
** Last update Mon Jan 15 13:12:39 1996 Stéphane Boisson
*/

#ifndef __CADRE_STD_H__
#define __CADRE_STD_H__

#include "xcalibur.h"
#include "Cadre.h"
#include "PostScript.h"
#include "DocFile.h"
#include "BaseStd.h"

#define CADRE_EMPTY 		0
#define CADRE_IMAGE_BITMAP	1
#define CADRE_TEXT		2
#define CADRE_VECTOR		3


typedef boolean (*XcFrameStdReadProc) ___PROTO((c_DocFile *doc, char *keyword,
						long param, c_Cadre *frame));

#define F_CADRE_STD \
error (*change_base) ___PROTO((void *this, BaseStd_t *new_base)); \
error (*display_init) ___PROTO((void *this, c_Cadre *cadre)); \
void (*display_redraw) ___PROTO((void *this, c_Cadre *cadre, Polygone *p)); \
void (*display_configure) ___PROTO((void *, c_Cadre *, Polygone *)); \
boolean (*ps_print) ___PROTO((void *this, c_PostScript *post)); \
XcFrameStdReadProc read; \
boolean (*write) ___PROTO((void *this, c_DocFile *doc))

struct cadre_callback_sf {
  F_STD;
  F_CADRE_STD;
};

struct cadre_callback_s {
  struct cadre_callback_sf *f;
};

#define CHANGE_BASE(obj) \
  ((struct cadre_callback_s *)(obj))->f->change_base
#define DISPLAY_INIT(obj) \
  ((struct cadre_callback_s *)(obj))->f->display_init
#define DISPLAY_REDRAW(obj) \
  ((struct cadre_callback_s *)(obj))->f->display_redraw
#define DISPLAY_CONFIGURE(obj) \
  ((struct cadre_callback_s *)(obj))->f->display_configure
#define DISPLAY_DELETE(obj) \
  ((struct cadre_callback_s *)(obj))->f->dest
#define DISPLAY_COPY(obj) \
  ((struct cadre_callback_s *)(obj))->f->copy
#define PS_PRINT(obj) \
  ((struct cadre_callback_s *)(obj))->f->ps_print
#define READ(obj) \
  ((struct cadre_callback_s *)(obj))->f->read
#define WRITE(obj) \
  ((struct cadre_callback_s *)(obj))->f->write

#endif /* !__CADRE_STD_H__ */





