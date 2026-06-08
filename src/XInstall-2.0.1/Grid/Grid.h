/*
** Grid.h for XInstall in Grid/
** Definitions for the Grid class
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
** Started on  Tue Dec  6 11:53:53 1994 Stéphane Boisson
** Last update Sun Jan 16 15:53:14 2000 Emmanuel Paris
*/

#ifndef __XC_GRID_H__
#define __XC_GRID_H__

typedef struct sc_Grid c_Grid;

#include <X11/Intrinsic.h>
#include "xcalibur.h"
#include "ManageWidget.h"
#include "WInterface.h"

#define XcG_MAX_COLUMNS 12
#define XcG_MAX_ROWS 12

#define XcG_BEST_SIZE (0)
#define XcG_MAX_SIZE (-1)

#define XcG_VERTICAL_ORIENTATION TRUE
#define XcG_HORIZONTAL_ORIENTATION FALSE
#define XcG_WIDGET_NULL	((Widget)1)

/*--- Internal structures ---*/
typedef struct 
{
 Widget w;
 int size;
 int border;
} grid_item_t;


/*--- Definition of the Grid methods ---*/
typedef struct
{
 F_STD;
 void (*addColumn) ___NPROTO((c_Grid *this, int size, ...));
 void (*callResize) ___PROTO((c_Grid *this));
} sf_Grid;


/*--- Definition of the Grid class ---*/
struct sc_Grid
{
 sf_Grid *f;

 /*___Widgets stuff___*/
 Widget w_This;

 int column_count;
 int column_size_count;
 boolean vertical_flag;
 int column_size;
 int row_sizes[XcG_MAX_COLUMNS][2];
 int row_count[XcG_MAX_COLUMNS][2];
 grid_item_t children[XcG_MAX_COLUMNS][XcG_MAX_ROWS];

 /*___Grid dimensions___*/
 int width;
 int height;
 boolean force_resize;
};


extern sf_Grid fc_Grid;

#endif /* !__XC_GRID_H__ */
