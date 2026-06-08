/*
** DocumentStd.h for Xclamation, XQuad and XAllWrite in Global/
** Interface between Document Class and Objects for display
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
** Last update Mon Dec 29 11:34:34 1997 Robin Castanier
*/

#ifndef __DOCUMENT_STD_H__
#define __DOCUMENT_STD_H__

#include "xcalibur.h"
#include <X11/Xlib.h>
#include "WinManager.h"

#define DOCUMENT_PAO 		0
#define DOCUMENT_TABLEUR	1
#define DOCUMENT_GRAPHEUR	2
#define DOCUMENT_WORDP		3
#define NO_DOCUMENT		4

#define F_DOCUMENT_STD \
void (*resize_document)(); \
void (*eventlook_document)(); \
void (*set_function_document)(); \
boolean (*read_document)(); \
boolean (*write_document)(); \
boolean (*ps_print_document)()

/*--- Prototypes
void (*resize_document)((void *this, XEvent *Event, boolean Maximized));
void (*eventlook_document)((void *this,XEvent *Event,
	listemanaged *listegwm, boolean DocSelected));
void (*set_function_document)((void *this, int function));
boolean *(read_document)((c_DocFile *doc, char *keyword, long param));
boolean (*write_document)((c_Document *this, c_DocFile *doc));
void (*ps_print_document)((void *this, c_PostScript *post));
---*/
     
struct document_callback_sf {
  F_STD;
  F_DOCUMENT_STD;
};

struct document_callback_s {
  struct document_callback_sf *f;
  Display	*display;
  Window	window;
  boolean	modified;
};

#define DELETE_DOCUMENT(obj) \
  ((struct document_callback_s *)(obj))->f->dest
#define RESIZE_DOCUMENT(obj) \
  ((struct document_callback_s *)(obj))->f->resize_document
#define EVENTLOOK_DOCUMENT(obj) \
  ((struct document_callback_s *)(obj))->f->eventlook_document
#define SET_FUNCTION_DOCUMENT(obj) \
  ((struct document_callback_s *)(obj))->f->set_function_document
#define READ_DOCUMENT(obj) \
  ((struct document_callback_s *)(obj))->f->read_document
#define WRITE_DOCUMENT(obj) \
  ((struct document_callback_s *)(obj))->f->write_document
#define PS_PRINT_DOCUMENT(obj) \
  ((struct document_callback_s *)(obj))->f->ps_print_document
#define DOCUMENT_DISPLAY(obj) \
  (((struct document_callback_s *)(obj))->display)
#define DOCUMENT_WINDOW(obj) \
  (((struct document_callback_s *)(obj))->window)
#define DOCUMENT_MODIFIED(obj) \
  (((struct document_callback_s *)(obj))->modified)

/* common extern declaration */

extern	c_WinManager	 *Gwm;

#endif /* !__DOCUMENT_STD_H__ */





