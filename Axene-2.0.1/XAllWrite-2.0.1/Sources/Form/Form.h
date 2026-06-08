/*
** Form.h for Xclamation, XQuad, XAllWrite, XMayday and XInstall in Form/
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
** Started on  Mon Jul 17 15:24:40 1995 Robin Castanier
** Last update Mon Jul 17 15:24:40 1995 Robin Castanier
*/

#ifndef _Form_h
#define _Form_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include <Xm/Form.h>

/* define the method for Class Form */
typedef struct
{
  F_STD;
  void (*attach_top)();
  void (*attach_bottom)();
  void (*attach_left)();
  void (*attach_right)();
  void (*align_top)();
  void (*align_bottom)();
  void (*align_right)();
  void (*align_left)();
} sf_Form;

/* define the Form Class */
typedef struct
{
  sf_Form	*f;
  Widget	w_This;
} c_Form;

extern sf_Form fc_Form;

#endif




