/*
** Label.h for AxeneOffice in Label/
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
** Started on  Mon Jul 17 19:01:54 1995 Robin Castanier
** Last update Fri Feb  6 03:00:51 1998 Emmanuel Paris
*/

#ifndef _Label_h
#define _Label_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include <Xm/Label.h>

/* defint the method for Class Label */
typedef struct
{
 F_STD;
} sf_Label;

/* define the Label Class */
typedef struct
{
 sf_Label	*f;
 Widget		w_This;
} c_Label;

extern sf_Label fc_Label;

#endif




