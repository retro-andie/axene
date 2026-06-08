/*
** Keyboard.h for Xclamation and XAllWrite in Keyboard/
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
** Started on  Sat Aug  5 18:26:36 1995 Emmanuel Paris
** Last update Mon Sep 14 18:12:19 1998 Emmanuel Paris
*/

#ifndef _Keyboard_h_
#define	_Keyboard_h_

typedef struct sc_Keyboard c_Keyboard;

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "xcalibur.h"
#include "Keyboard_Box.h"
#include "Keyboard_Specs.h"

#define KEYB_BUFFER_SIZE	50

typedef enum
{
 KeyCode_Normal=0,
 KeyCode_Mode,
 KeyCode_Specs
} keyboard_type_t;

typedef struct
{
 KeySym	keysym;
 KeyCode	keycode;
 int		keyb_offset;
 int		keyb_bit;
} keyboard_mode_t;

typedef	struct
{
 F_STD;
 void (*EventLook) ___PROTO((c_Keyboard *This, XEvent event));
 void (*QueuedMode) ___NPROTO((c_Keyboard *This, boolean queued));
 void (*CreateBoxInsert) ___PROTO((c_Keyboard *This, Widget w_Parent,
				   c_TextStyle *style, void (*callback)(),
				   void *cb_data));
 char (*GetOneCharBoxInsert) ___PROTO((c_Keyboard *This, Widget w_Parent,
				       c_TextStyle *style));
} sf_Keyboard;

struct sc_Keyboard
{
 sf_Keyboard	*f;
  
 Display	*display;
 Window	window;
 char		*buf_keyb;
 char		buf_keybs[KEYB_BUFFER_SIZE];
 int		buf_length;
 KeySym	keysym;
  
 boolean	Shift;
 boolean	Lock;
 boolean	Control;
 boolean	Mod1;
 boolean	Mod2;
 boolean	Mod3;
 boolean	Mod4;
 boolean	Mod5;
  
 boolean	insertBox;
 keyb_box_t	keyb_box;
  
 keyboard_mode_t	*keyb_mode[8];
 int			max_keypermod;
  
 keyboard_type_t	*keyb_tab;
 int			first_keycode;

 boolean	compose;	/* true if composing specs keychar */
 boolean	start_comp;	/* true when compose = TRUE & KeyRelease */
 int		comp_specs;	
 
 boolean	event_enable;

 boolean	queued_buf;	/* enable queued buf mode */
 char		*qbuf_keyb;
 char		qbuf_keybs[KEYB_BUFFER_SIZE];
 int		qbuf_blng;	/* queued keyboard buffer length */
 KeyCode	*qbuf_keyc;
 KeyCode	qbuf_keycs[KEYB_BUFFER_SIZE];
 int		qbuf_clng;	/* queued keycode buffer length */

};

extern sf_Keyboard fc_Keyboard;

#endif






