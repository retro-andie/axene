/*
** ScrolledText.h for XAllWrite in ScrolledText/
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Thu Nov 28 18:23:59 1996 Robin Castanier
** Last update Thu Nov 28 18:24:59 1996 Robin Castanier
*/

#ifndef _ScrolledText_h
#define _ScrolledText_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include <Xm/Text.h>

/* define the method for Class ScrolledText */
typedef struct
{
  F_STD;
} sf_ScrolledText;

/* define the ScrolledText Class */
typedef struct
{
  sf_ScrolledText	*f;
  Widget		w_This;
} c_ScrolledText;

extern sf_ScrolledText fc_ScrolledText;

#endif




