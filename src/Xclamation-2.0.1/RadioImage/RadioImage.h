/*
** RadioImage.h for Xclamation in RadioImage/
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
** Started on  Tue Jul 18 12:03:49 1995 Robin Castanier
** Last update Sun Jun 15 12:07:33 1997 Robin Castanier
*/


#ifndef _RadioImage_h
#define _RadioImage_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include "WInterface.h"
#include "Grid.h"
#include "Icon.h"
#include "ToggleButton.h"
#include <Xm/RowColumn.h>
#include <stdarg.h>
#include <X11/Intrinsic.h>

/*--- define the method for Class RadioImage ---*/
typedef struct
{
 F_STD;
} sf_RadioImage;

/* define the RadioImage Class */

typedef struct
{
 void	*RadioImage;
 int	iIconToManage;
} ToCallback;

typedef struct
{
 sf_RadioImage	*f;
 Widget		w_This;
 c_Grid		*Grid;
 Widget		w_Row;
 c_ToggleButton **Toggles;
 c_Icon		**Icons;
 int		iNbItems;
 int		iIconManaged;
 int		*p_change;
 ToCallback	*pstrToCallback;
} c_RadioImage;

extern sf_RadioImage fc_RadioImage;

#endif









