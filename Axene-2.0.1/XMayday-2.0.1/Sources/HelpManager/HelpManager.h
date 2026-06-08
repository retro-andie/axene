/*
** HelpManager.h for XMayday in HelpManager/
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
** Started on  Mon Jul 17 17:46:15 1995 Emmanuel Paris
** Last update Tue Jun 18 19:19:55 1996 Emmanuel Paris
*/

#ifndef _HelpManager_h
#define _HelpManager_h

#include "xcalibur.h"
#include "HelpWidget.h"
#include "PulldownMenu.h"
#include "HelpManagerStd.h"
#include <X11/Intrinsic.h>

#define XcR_NONE -1
#define HM_URL_LIST_LENGTH 20

typedef struct sc_HelpManager c_HelpManager;

typedef struct _anchor_list_t
{
  char	*href;
  char	*anchor;
  char	*title;
  int	vx;
  int	vy;
  struct _anchor_list_t *Prev;
  struct _anchor_list_t *Next;
} anchor_list_t;

/* defint the method for Class HelpManager */
typedef struct
{
  F_STD;
  boolean (*read_cmd_line) ___PROTO((c_HelpManager *This, int argc,
				     char **argv));
  void (*open_index) ___PROTO((c_HelpManager *This, char *index));
  void (*open_help) ___PROTO((c_HelpManager *This, char *help));
  void (*select_function) ___PROTO((c_HelpManager *This, int function));
} sf_HelpManager;

/* define the HelpManager Class */
struct sc_HelpManager
{
 sf_HelpManager	*f;
 anchor_list_t	*anchor_list;
 anchor_list_t	*anchor_current;
 c_HelpWidget	*index_widget;
 c_HelpWidget	*help_widget;
 c_PulldownMenu	*menu_widget;
 int		menu_button;
 Widget		url_widget;
 boolean	index_exist;
 boolean	help_exist;
 char		*url_list[HM_URL_LIST_LENGTH];
};

extern sf_HelpManager fc_HelpManager;


#endif






