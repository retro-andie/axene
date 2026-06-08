/*
** Dispatch_To_FrMg.h for XAllWrite in Page/
** define Dispatch to FrameManager function
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
** Started on  Tue Mar 28 13:43:16 1995 Emmanuel Paris
** Last update Mon Aug  3 20:31:16 1998 Emmanuel Paris
*/

#ifndef _Dispatch_To_FrMgs_h
#define _Dispatch_To_FrMgs_h

#define DATA_MACROS
#include "FrameManagerStd.h"
#undef	DATA_MACROS
#include "Page.h"

typedef struct 
{
  int function_type;
} 
d_Dispatch_To_FrMg;

#endif



