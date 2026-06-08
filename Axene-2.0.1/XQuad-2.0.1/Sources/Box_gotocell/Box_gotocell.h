/*
** Box_gotocell.h for XQuad in Box_gotocell/
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
** Started on  Mon Aug 28 13:17:21 1995 Robin Castanier
** Last update Tue Aug 29 13:32:09 1995 Emmanuel Paris
*/

#ifndef _Box_gotocell_h
#define _Box_gotocell_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "ManageWidget.h"
#include "WTextField.h"
#include "Label.h"
#include <X11/Intrinsic.h>

/* define the method for Class Box_gotocell */
typedef struct
{
  F_STD;
} sf_Box_gotocell;

/* define the Box_gotocell Class */
typedef struct
{
  sf_Box_gotocell	*f;
  Widget		w_This;
  c_CustomDialog	*Dialog;
  c_Label		*LgotoAt;
  c_WTextField		*WTgotoAt;
  
  int			*column;
  int			*row;
  int			col_orig;
  int			row_orig;
} c_Box_gotocell;

extern sf_Box_gotocell fc_Box_gotocell;

#endif




