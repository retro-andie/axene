/*
** Separator.h for XQuad in Separator/
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
** Started on  Thu Dec 15 18:44:46 1994 Robin Castanier
** Last update Thu Apr 11 13:35:33 1996 Robin Castanier
*/

#ifndef _Separator_h
#define _Separator_h

#include "xcalibur.h"
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/SeparatoG.h>

#include "WInterface.h"
#include "ManageWidget.h"

#define XcSEPARATOR_H	1
#define XcSEPARATOR_V	2

/* define the method for Class Separator */
typedef struct
{
  F_STD;
} sf_Separator;

/* define the Separator Class */
typedef struct
{
  sf_Separator		*f;
  Widget		w_This;
} c_Separator;

extern sf_Separator fc_Separator;

#endif




