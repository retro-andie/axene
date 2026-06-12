/*
** RadioBox.h for Xclamation, XQuad, XAllWrite and XMayday in RadioBox/
** Definition for class RadioBox
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
** Started on  Wed Oct 19 11:21:40 1994 Robin Castanier
** Last update Mon Jun  5 16:46:07 1995 Emmanuel Paris
*/

#ifndef _RadioBox_h
#define _RadioBox_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleBG.h>
#include <stdarg.h>

#define B_IS_NONE	0
#define B_IS_SET	1
#define B_IS_UNSET	2
#define COND_END	32

#define RADIO_NORMAL	0
#define RADIO_TEST	42

/* ----------------------------------------------------------------- ** 
** define methods                                                    ** 
** ----------------------------------------------------------------- */
typedef struct
{
  F_STD;
  void (*AddCallback)();
  void (*AddCallbackByName)();
  void (*AddSetCondition)();
  void (*AddUnSetCondition)();
  void (*AddSetfCondition)();
  void (*AddUnSetfCondition)();
} sf_RadioBox;

/* ----------------------------------------------------------------- ** 
** define the attributes                                             ** 
** ----------------------------------------------------------------- */
typedef struct listZcondition
{
  int			scondition; /*seted condition*/
  int			ucondition; /*unseted condition*/
  struct listZcondition	*Next;
} listsetcondition;

typedef struct listZbutton
{
  listsetcondition	*setcondition;
  listsetcondition	*unsetcondition;
  listsetcondition	*setfcondition;
  listsetcondition	*unsetfcondition;
  Widget		w_Buttons;
  char			*Name;
  struct listZbutton	*Next;
} listbutton;

typedef struct
{
  sf_RadioBox	*f;
  char		*Title;
  Widget	w_This;
  int		number;
  listbutton	*Button;
  int		state;
  int		state_c;
  int		state_cf;
  int		state_u;
  int		state_uf;
} c_RadioBox;

extern sf_RadioBox fc_RadioBox;

#endif




