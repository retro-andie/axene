/*
** ModifList.h for Xclamation, XQuad and XAllWrite in ModifList/
** Object and method for ModifList
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
** Started on  Thu Nov 24 15:51:53 1994 Antoine Buat
** Last update Fri Oct 16 15:33:22 1998 Emmanuel Paris
*/

#ifndef _ModifList_h
#define _ModifList_h

/*--- Forward declarations ---*/
typedef struct sc_ModifList c_ModifList;

#include "xcalibur.h"

#define TOSET	0
#define TOSUB	1
#define TOADD	2
#define NEWSET	3

typedef struct _obj_save
{
 void	*object;		/*to store object adress*/
 char	*contain;		/*to store [sizeof(field) field] ...*/ 
 int	type;
 struct _obj_save *Next;
} obj_save;


/*--- define the method for Class ModifList ---*/
typedef struct
{
 F_STD;
 void	(*set)();
 int	(*get)();
 void	(*add)();
 void	(*sub)();
 void	(*change_all)();
 void	(*change_all2)();
 boolean (*something_to_change)();
} sf_ModifList;

/*--- define the ModifList Class ---*/
struct sc_ModifList
{
 sf_ModifList	*f;
 void		*Box;
 obj_save	*o_save;
 int		*off_inout;
 int		globsize;
 int		num;
 void		(*func)();
 obj_save	*last_use;
};

extern sf_ModifList fc_ModifList;

#endif
