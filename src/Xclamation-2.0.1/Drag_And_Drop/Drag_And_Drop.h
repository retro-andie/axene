/*
** Drag_And_Drop.h for Xclamation, XQuad and XAllWrite in Drag_And_Drop/
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
** Started on  Mon Jul 17 14:42:41 1995 Emmanuel Paris
** Last update Mon Apr 20 15:00:27 1998 Emmanuel Paris
*/


#ifndef _Drag_And_Drop_h
#define _Drag_And_Drop_h

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "types.h"

#define DD_DROP		1
#define DD_MOTION	2
#define DD_ABORT	3

#define DD_DROP_ON_WRONG_WINDOW	0
#define DD_DROP_ABORTED_BY_USER 1
#define DD_DROP_UNKNOW_REASON	2
#define DD_DROP_FOR_SOURCE	3
#define DD_DROP_FOR_TARGET	4
#define DD_DROP_FOR_MOVE	16
#define DD_DROP_FOR_COPY	48
#define DD_XCLAMATION_INDENTITY	0xfada00

#define DD_STRING	0
#define DD_C_STRING	'S'
#define DD_S_STRING	"S"
#define DD_INT		1
#define DD_C_INT	'I'
#define DD_S_INT	"I"
#define DD_LONG		2
#define DD_C_LONG	'L'
#define DD_S_LONG	"L"

#define DD_C_SEPARATOR	'|'
#define DD_S_DROP	"DROP"
#define DD_S_MOTION	"MOTION"
#define DD_S_SEPARATOR	"|"

/* define the methods for Class Drag_And_Drop */
typedef struct
{
  F_STD;
  boolean (*eventlook_drag)();
  void (*eventlook_drop)();
  void (*select_input)();
  void (*change_argument)();
  void (*create_context)();
  void (*test_context)();
  void (*enable_drag)();
  void (*disable_drag)();
  void (*activate_drag)();
  void (*disactivate_drag)();
  void (*set_callback)();
} sf_DragAndDrop;

/* define the DragAndDrop class */
typedef struct
{
  sf_DragAndDrop	*f;
  Display		*display;
  Window		root_window;
  long			root_event_mask;
  Window		current_window;
  long			current_event_mask;
  boolean		active;
  boolean		enable;
  void			(*callback_drop)();
  void			*callback_drop_data;
  void			(*callback_motion)();
  void			*callback_motion_data;
  void			(*callback_abort)();
  void			*callback_abort_data;
  void			*data;
} c_DragAndDrop;

extern sf_DragAndDrop fc_DragAndDrop;


#endif
