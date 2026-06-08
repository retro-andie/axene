/*
** PulldownMenu.h for Xclamation, XQuad, XAllWrite and XMayday in PulldownMenu/
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
** Started on  Tue Jul 18 11:52:18 1995 Robin Castanier
** Last update Thu Oct 22 19:46:02 1998 Emmanuel Paris
*/

#ifndef _PulldownMenu_h
#define _PulldownMenu_h

#ifndef MENUSEPARATOR
#define MENUSEPARATOR	1
#define SUBMENUON	2
#define SUBMENUOFF	3
#define NOMANAGE	4
#define TOGGLEMODEON	8	
#define TOGGLEMODEOFF	9	
#endif
#define RADIOBOXON	5
#define RADIOBOXOFF	6

#define DYNAMIC		7

#include "xcalibur.h"
#include "ManageWidget.h"
#include <Xm/RowColumn.h>
#include <Xm/CascadeBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/SeparatoG.h>
#include <varargs.h>
#include <X11/Intrinsic.h>
#include <Xm/CascadeBG.h>

typedef struct sc_PulldownMenu c_PulldownMenu;

typedef enum
{
 PMt_NONE,
 PMt_BEGIN,
 PMt_END,
 PMt_CENTER,
 PMt_GUESS
} pm_truncate_t;

typedef struct 
{
 int	numberw;
 int	min;
 int	max;
 int	max_accelerator;
 char	*accelerator;
 char	*acceleratorText;
 boolean autocount;
 boolean radio;
 boolean toggle;
 int	button_push;
 pm_truncate_t truncate_type;
 int	truncate_count;
 char	**text;
 void	(*callback)();
 void	*callback_data;
} pm_dynamic_t;

/* define the method for Class PulldownMenu */
typedef struct
{
 F_STD;
 void (*ButtonFlag) ___NPROTO((c_PulldownMenu *This, int button,
			       Boolean flag));
 void (*ToggleManage) ___PROTO((c_PulldownMenu *This, int button_umng,
				int button_mng));
 void (*ToggleButton) ___NPROTO((c_PulldownMenu *This, int button,
				 Boolean state));
 void (*AddCallback)();
 void (*AddCallback2)();
 void (*SetDynamicFlag) ___NPROTO((c_PulldownMenu *This, int button, 
				   boolean autocount, boolean radio,
				   int max_accelerator, char *accelerator,
				   char *acceleratorText,
				   pm_truncate_t truncate_type,
				   int truncate_count));
  
 void (*SetDynamicList) ___PROTO((c_PulldownMenu *This, int button, 
				  char **url_list));
 void (*ToggleDynamicButton) ___NPROTO((c_PulldownMenu *This, int button,
					Boolean state));
 void (*AddDynamicCallback) ___PROTO((c_PulldownMenu *This, int button,
				      void (*callback)(),
				      void *callback_data));
} sf_PulldownMenu;

/* define the PulldownMenu Class */

struct sc_PulldownMenu
{
 sf_PulldownMenu	*f;
 Widget			w_This;
 char			*Title;
 int			numberw;
 Widget			*w_Buttons;
 Widget			w_Cascade;
  
 int			nb_pm_dynamic;
 pm_dynamic_t		*pm_dynamic;
};

extern sf_PulldownMenu fc_PulldownMenu;

#endif






