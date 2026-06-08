/*
** Scale.h for Xclamation, XQuad and XAllWrite in Scale/
** Object and method definition for Object Scale
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
** Started on  Wed Dec  7 16:19:25 1994 Antoine Buat
** Last update Tue Dec 27 12:29:03 1994 Antoine Buat
*/

#ifndef _Scale_h
#define _Scale_h

/*--- Forward declarations ---*/
typedef struct sc_Scale c_Scale;

#include "xcalibur.h"
#include "ManageWidget.h"
#include "WInterface.h"
#include <X11/Intrinsic.h>
#include <Xm/Scale.h>

/*--- define the method for Class Scale ---*/
typedef struct
{
  F_STD;
  void (*set) ___PROTO((c_Scale *this, int *value));
  void (*change) ___PROTO((c_Scale *this, int val));
  void (*set_drag_callback) ___PROTO((c_Scale *this, void (*func)(),
				      void *data));
  void (*set_valuechange_callback) ___PROTO((c_Scale *this, void (*func)(),
					     void *data));
} sf_Scale;

/*--- define the Scale Class ---*/
struct sc_Scale
{
  int		(*cf)();
  int		(*icf)();
  int		*value;
  sf_Scale	*f;
  Widget   	w_This;
};

extern sf_Scale fc_Scale;

#endif




