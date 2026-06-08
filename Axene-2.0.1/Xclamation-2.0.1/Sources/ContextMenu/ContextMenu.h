/*
** ContextMenu.h for Xclamation in ContextMenu/
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
** Started on  Mon Jul 17 14:18:58 1995 Emmanuel Paris
** Last update Sat Nov 14 17:26:42 1998 Emmanuel Paris
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
 CM_PAGER = 0,
 CM_CADRE_EMPTY,
 CM_PAGE_EMPTY,
 CM_CADRE_TEXT,
 CM_KEYBOARD
} num_CM_t;

typedef struct 
{
 int	*ret_button;
 int	button;
} but_CM_s;

enum 
{
 CM_Pgr_INSERT_BEFORE = 1,
 CM_Pgr_SEPARATOR_1,
 CM_Pgr_INSERT_AFTER,
 CM_Pgr_SEPARATOR_2,
 CM_Pgr_MODIFY,
 CM_Pgr_SEPARATOR_3,
 CM_Pgr_DELETE
};

enum 
{
 CM_Cde_IMPORT_TEXT = 1,
 CM_Cde_SEPARATOR_1,
 CM_Cde_IMPORT_IMAGE,
 CM_Cde_SEPARATOR_2,
 CM_Cde_IMPORT_VECTOR,
 CM_Cde_SEPARATOR_3,
 CM_Cde_EDIT_TEXTE
};

enum 
{
 CM_Pge_CREATE_SQUARE = 1,
 CM_Pge_SEPARATOR_1,
 CM_Pge_CREATE_ELIPSE,
 CM_Pge_SEPARATOR_2,
 CM_Pge_CREATE_POLYGON
};

enum 
{
 CM_Cdt_EDIT_TEXTE = 1
};

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

#ifdef _CONTEXT_MENU_PRIVATE_

#define CM_Pgr_callback_num	4

static but_CM_s CM_Pgr_callback[CM_Pgr_callback_num] =
{ 
 { NULL, CM_Pgr_INSERT_BEFORE },
 { NULL, CM_Pgr_INSERT_AFTER },
 { NULL, CM_Pgr_MODIFY },
 { NULL, CM_Pgr_DELETE }
};

#define CM_Cde_callback_num	4

static but_CM_s CM_Cde_callback[CM_Cde_callback_num] = 
{ 
 { NULL, CM_Cde_IMPORT_TEXT },
 { NULL, CM_Cde_IMPORT_IMAGE },
 { NULL, CM_Cde_IMPORT_VECTOR },
 { NULL, CM_Cde_EDIT_TEXTE }
};

#define CM_Pge_callback_num	3

static but_CM_s CM_Pge_callback[CM_Pge_callback_num] =
{
 { NULL, CM_Pge_CREATE_SQUARE },
 { NULL, CM_Pge_CREATE_ELIPSE },
 { NULL, CM_Pge_CREATE_POLYGON }
};

#define CM_Cdt_callback_num	1

static but_CM_s CM_Cdt_callback[CM_Cdt_callback_num] = 
{ 
 { NULL, CM_Cdt_EDIT_TEXTE }
};

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

#endif

#endif





