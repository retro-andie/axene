/*
** RowColumn.h for Xclamation, XQuad, XAllWrite, XMayday and AxeneOffice in 
** 	RowColumn/
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
** Started on  Tue Jul 18 12:17:13 1995 Robin Castanier
** Last update Tue Jan 20 16:31:37 1998 Robin Castanier
*/

#ifndef _RowColumn_h
#define _RowColumn_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>

/* define the method for Class RowColumn */
typedef struct
{
 F_STD;
} sf_RowColumn;

/* define the RowColumn Class */
typedef struct
{
 sf_RowColumn	*f;
 Widget	w_This;
} c_RowColumn;

extern sf_RowColumn fc_RowColumn;

#endif




