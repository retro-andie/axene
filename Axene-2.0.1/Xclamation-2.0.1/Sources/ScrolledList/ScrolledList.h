/*
** ScrolledList.h for Xclamation in ScrolledList/
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
** Started on  Tue Jul 18 12:24:15 1995 Marc Blet
** Last update Tue Jul 18 12:24:15 1995 Marc Blet
*/

#ifndef _ScrolledList_h
#define _ScrolledList_h

#include "xcalibur.h"
#include "WInterface.h"
#include "ManageWidget.h"

#include <Xm/List.h>
#include <Xm/BulletinB.h>



/* define the method for Class ScrolledList */
typedef struct
{
  F_STD;
  void  (*Initialize_Data)();
} sf_ScrolledList;

/* define the Box_document Class */
typedef struct
{
 sf_ScrolledList 	*f;
 Widget          	w_This;
 Widget			w_ScrolledList;
 int			Item_Count;

} c_ScrolledList;

extern sf_ScrolledList fc_ScrolledList;

#endif










