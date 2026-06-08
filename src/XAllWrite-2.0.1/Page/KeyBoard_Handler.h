/*
** KeyBoard_Handler.h for XAllWrite in Page/
** Handler for KeyBoard Event.
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
** Started on  Wed Oct 15 20:01:18 1997 Emmanuel Paris
** Last update Mon Dec 29 15:07:59 1997 Emmanuel Paris
*/

#ifndef _KeyBoard_Handler_h_
#define _KeyBoard_Handler_h_

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "Page.h"

extern boolean KB_KeyPress_Handler ___PROTO((c_Page *This, XEvent *event,
					  boolean *ctrl, boolean *shift));

extern boolean KB_KeyRelease_Handler ___PROTO((c_Page *This, XEvent *event,
					    boolean *ctrl, boolean *shift));
extern boolean KB_ButtonPress_Handler ___PROTO((c_Page *This, XEvent *event));
extern void KB_OtherEvent_Handler ___PROTO((c_Page *This, XEvent *event));

#endif /* _KeyBoard_Handler_h_ */
