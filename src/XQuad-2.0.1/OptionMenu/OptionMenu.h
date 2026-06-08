/*
** OptionMenu.h for Xclamation, XQuad and XAllWrite in OptionMenu/
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
** Started on  Mon Jul 17 19:46:27 1995 Robin Castanier
** Last update Thu Mar  6 02:02:52 1997 Antoine Buat
*/

#ifndef _OptionMenu_h
#define _OptionMenu_h

#ifndef MENUSEPARATOR
#define MENUSEPARATOR	1
#define SUBMENUON	2
#define SUBMENUOFF	3
#define NOMANAGE	4	
#endif
#define LABELTYPE	5	
#define ICONMODEON	6	
#define ICONMODEOFF	7	
#define OICONMODEON	8
#define HELPLINE	9

#define ICONMODE_INACTIF	0
#define ICONMODE_INTERNE	1
#define ICONMODE_EXTERNE	2

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
#include <Xm/CascadeB.h>
#include "WInterface.h"
#include "Icon.h"

/* define the method for Class OptionMenu */
typedef struct
{
  F_STD;
  void (*ButtonFlag)();
  void (*AddCallback)();
  void (*AddCallback2)();
  void (*waitForUnmap)();
  void (*setSelection)();
  void (*setGlobalCallback)();
} sf_OptionMenu;

typedef struct
{
  Widget  w_content;
  void (*destructor)();
  void *objet;
} strPopupContent;

/* define the OptionMenu Class */
typedef struct
{
  sf_OptionMenu	*f;
  Widget		w_This;
  Widget		w_Pulldown;
  char			*Title;
  strPopupContent	*Buttons;
  int			numberw;
  boolean		mapped_flag;
  void			*glob_arg;
  void			(*glob_func)();
} c_OptionMenu;

extern sf_OptionMenu fc_OptionMenu;

#endif
