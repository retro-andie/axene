/*
** Actions.h for Xclamation in MainInterface/
** header file for Actions.c
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
** Started on  Tue Sep  5 11:14:56 1995 Emmanuel Paris
** Last update Tue Dec 30 21:41:57 1997 Emmanuel Paris
*/

#ifndef _Actions_h_
#define _Actions_h_

#include "xcalibur.h"
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>

#define NB_ACTIONS 5
extern XtActionsRec actions[NB_ACTIONS];
extern void allow_ba_action ___PROTO((Widget w));

#endif
