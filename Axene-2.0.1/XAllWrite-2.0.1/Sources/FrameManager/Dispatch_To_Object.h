/*
** Dispatch_To_Object.h for XAllWrite in FrameManager/
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
** Started on  Mon Jul 17 20:24:26 1995 Emmanuel Paris
** Last update Wed Jun 24 18:12:30 1998 Emmanuel Paris
*/


#ifndef _Dispatch_To_Object_h
#define _Dispatch_To_Object_h

#include "FrameManagerStd.h"
#include "ObjectStd.h"
#include "BaseStd.h"
#include "FrameManager.h"

extern sf_object_function fc_Move_Image;
extern sf_object_function fc_Move_Vector;

extern boolean SetFunctionObject ___PROTO((c_FrMg *cfrmg, int num_function));

extern void init_Dispatch_To_Object();
extern status_t eventlook_Dispatch_To_Object();
extern void abort_Dispatch_To_Object();
extern void redraw_Dispatch_To_Object();

sf_frmg_function fc_Dispatch_To_Object =
{
 init_Dispatch_To_Object,
 eventlook_Dispatch_To_Object,
 abort_Dispatch_To_Object,
 redraw_Dispatch_To_Object
};  

typedef struct _d_object_function d_Dispatch_To_Object;


extern void init_Invalid_OFunc();
extern status_t eventlook_Invalid_OFunc();
extern void abort_Invalid_OFunc();
extern void redraw_Invalid_OFunc();

sf_object_function fc_Invalid_OFunc =
{
 init_Invalid_OFunc,
 eventlook_Invalid_OFunc,
 abort_Invalid_OFunc,
 redraw_Invalid_OFunc
};

#endif
