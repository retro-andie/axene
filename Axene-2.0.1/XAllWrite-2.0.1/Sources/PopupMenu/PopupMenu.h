/*
** PopupMenu.h for Xclamation, XAllWrite and AxeneOffice in PopupMenu/
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
** Started on  Tue Jul 18 11:47:30 1995 Robin Castanier
** Last update Fri Oct 23 14:25:08 1998 Emmanuel Paris
*/

#ifndef _PopupMenu_h
#define _PopupMenu_h

#ifndef MENUSEPARATOR
#define MENUSEPARATOR	1
#define SUBMENUON	2
#define SUBMENUOFF	3
#define NOMANAGE	4	
#define TOGGLEMODEON	8	
#define TOGGLEMODEOFF	9	
#endif
#define LABELTYPE	5	
#define ICONMODEON	6	
#define ICONMODEOFF	7	
#define HELPLINE        10

#include "xcalibur.h"
#include "ManageWidget.h"
#include <Xm/RowColumn.h>
#include <Xm/CascadeBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/PushBG.h>
#include <Xm/LabelG.h>
#include <Xm/SeparatoG.h>
#include <varargs.h>
#include <X11/Intrinsic.h>
#include "WInterface.h"
#include "Icon.h"

typedef struct sc_PopupMenu c_PopupMenu;

/* define the method for Class PopupMenu */
typedef struct
{
 F_STD;
 void (*ButtonFlag) ___NPROTO((c_PopupMenu *This, int button,
			       Boolean flag));
 void (*ToggleManage) ___PROTO((c_PopupMenu *This, int button_umng,
				int button_mng));
 void (*ToggleButton) ___NPROTO((c_PopupMenu *This, int button,
				 Boolean state));
 void (*AddCallback)();
 void (*AddCallback2)();
 void (*mapAndWaitForMap)();
 void (*waitForUnmap)();
} sf_PopupMenu;

#ifndef _STRPOPUPCONTENT	/* to avoid conflict w/ OptionMenu.h */
#define _STRPOPUPCONTENT
typedef struct
{
 Widget  w_content;
 void (*destructor)();
 void *objet;
} strPopupContent;
#endif

/* define the PopupMenu Class */
struct sc_PopupMenu
{
 sf_PopupMenu		*f;
 Widget			w_This;
 char			*Title;
 strPopupContent	*Buttons;
 int			numberw;
 boolean		mapped_flag;
 Widget			first_button;
};

extern sf_PopupMenu fc_PopupMenu;

#endif
