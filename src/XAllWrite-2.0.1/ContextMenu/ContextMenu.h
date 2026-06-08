/*
** ContextMenu.h for XAllWrite in ContextMenu/
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
** Started on  Mon Jul 17 14:18:58 1995 Emmanuel Paris
** Last update Fri Dec 18 02:12:31 1998 Robin Castanier
*/

#ifndef _ContextMenu_h
#define _ContextMenu_h

#include "xcalibur.h"
#include "WInterface.h"
#include "PopupMenu.h"
#include "MainInterface.h"
#include <X11/Intrinsic.h>

extern c_MainInterface	*MainInterface1;

/* defint the method for Class ContextMenu */
typedef struct
{
 F_STD;
 void (*waitForUnmap)();
} sf_ContextMenu;

/* define the ContextMenu Class */
typedef struct
{
 sf_ContextMenu	*f;
 
 c_PopupMenu	*popup;
} c_ContextMenu;

extern sf_ContextMenu fc_ContextMenu;

#define CM_PAUSE 35000

typedef enum 
{
 CM_KEYBOARD = 0,
 CM_XAREGLE,
 CM_TEXT,
 CM_CADRE_OBJECT,
 CM_CREATE_CADRE,
} num_CM_t;

typedef struct
{
 int	*ret_button;
 int	button;
} but_CM_s;

enum
{
 CM_Kbd_BUFFER_0 = 1,
 CM_Kbd_BUFFER_1,
 CM_Kbd_BUFFER_2,
 CM_Kbd_BUFFER_3,
 CM_Kbd_BUFFER_4,
 CM_Kbd_BUFFER_5,
 CM_Kbd_BUFFER_6,
 CM_Kbd_BUFFER_7
};

enum
{
 CM_Xar_TAB_ALIGNMENT = 1,
 CM_Xar_TAB_LEFT,
 CM_Xar_TAB_RIGHT,
 CM_Xar_TAB_CENTER,
 CM_Xar_TAB_CPERIOD,
 CM_Xar_SUBMENU1,
 CM_Xar_FILLER,
 CM_Xar_FILLER_NONE,
 CM_Xar_FILLER_POINT,
 CM_Xar_FILLER_DOT,
 CM_Xar_FILLER_HYPHEN,
 CM_Xar_SEP1,
 CM_Xar_FILLER_USERDEF,  
 CM_Xar_SUBMENU2,
 CM_Xar_SEP2,
 CM_Xar_TAB_DELETE,
};

enum
{
 CM_Tex_CUT = 1,
 CM_Tex_COPY,
 CM_Tex_PASTE
};
 
enum
{
 CM_Cde_IMPORT_IMAGE = 1,
 CM_Cde_SEPARATOR_1,
 CM_Cde_IMPORT_VECTOR
};

enum 
{
 CM_Pge_CREATE_SQUARE = 1,
 CM_Pge_SEPARATOR_1,
 CM_Pge_CREATE_ELIPSE,
 CM_Pge_SEPARATOR_2,
 CM_Pge_CREATE_POLYGON
};

#ifdef _CONTEXT_MENU_PRIVATE_

#define CM_Kbd_callback_num	8

static but_CM_s CM_Kbd_callback[CM_Kbd_callback_num] =
{
 { NULL, CM_Kbd_BUFFER_0 },
 { NULL, CM_Kbd_BUFFER_1 },
 { NULL, CM_Kbd_BUFFER_2 },
 { NULL, CM_Kbd_BUFFER_3 },
 { NULL, CM_Kbd_BUFFER_4 },
 { NULL, CM_Kbd_BUFFER_5 },
 { NULL, CM_Kbd_BUFFER_6 },
 { NULL, CM_Kbd_BUFFER_7 },
};

#define CM_Xar_callback_num	16

static but_CM_s CM_Xar_callback[CM_Xar_callback_num] =
{ 
 { NULL, 0 },
 { NULL, CM_Xar_TAB_LEFT },
 { NULL, CM_Xar_TAB_RIGHT },
 { NULL, CM_Xar_TAB_CENTER },
 { NULL, CM_Xar_TAB_CPERIOD },
 { NULL, 0 },
 { NULL, 0 },
 { NULL, CM_Xar_FILLER_NONE },
 { NULL, CM_Xar_FILLER_POINT },
 { NULL, CM_Xar_FILLER_DOT },
 { NULL, CM_Xar_FILLER_HYPHEN },
 { NULL, 0 },
 { NULL, CM_Xar_FILLER_USERDEF },
 { NULL, 0 },
 { NULL, CM_Xar_TAB_DELETE }
};

#define CM_Tex_callback_num	3

static but_CM_s CM_Tex_callback[CM_Tex_callback_num] = 
{ 
 { NULL, CM_Tex_CUT },
 { NULL, CM_Tex_COPY },
 { NULL, CM_Tex_PASTE },
};

#define CM_Cde_callback_num	2

static but_CM_s CM_Cde_callback[CM_Cde_callback_num] = 
{ 
 { NULL, CM_Cde_IMPORT_IMAGE },
 { NULL, CM_Cde_IMPORT_VECTOR },
};

#define CM_Pge_callback_num	3

static but_CM_s CM_Pge_callback[CM_Pge_callback_num] =
{
 { NULL, CM_Pge_CREATE_SQUARE },
 { NULL, CM_Pge_CREATE_ELIPSE },
 { NULL, CM_Pge_CREATE_POLYGON }
};

#endif

#endif





