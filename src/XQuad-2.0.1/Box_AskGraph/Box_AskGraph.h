/*
** Box_AskGraph.h for XQuad in Box_AskGraph/
** Declaration for Object Box_AskGraph
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
** Started on  Fri May 16 13:50:15 1997 Antoine Buat
** Last update Fri May 16 13:59:16 1997 Antoine Buat
*/

#ifndef _Box_AskGraph_h
#define _Box_AskGraph_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "ManageWidget.h"
#include "Frame.h"
#include "Label.h"
#include "Grid.h"
#include "RadioImage.h"
#include "High_Graph.h"
#include <X11/Intrinsic.h>
#include <Xm/DrawingA.h>
#include <Xm/RowColumn.h>

typedef struct _c_Box_AskGraph c_Box_AskGraph;

#define GRAPH_DRAWING_SIZE 350

/* define the method for Class Box_AskGraph */
typedef struct
{
 F_STD;
 void	(*cancel) ___PROTO((c_Box_AskGraph *This));
} sf_Box_AskGraph;

/* define the Box_AskGraph Class */
struct _c_Box_AskGraph
{
 sf_Box_AskGraph	*f;
 Widget			w_This;
 c_CustomDialog		*Dialog;
 c_Grid			*Grid;
 c_Label		*Ltitre;
 c_Frame		*Frame1;
 c_Frame		*Frame2;
 c_Frame		*Frame3;
 c_Frame		*Frame4;

 Widget			drawing;

 c_Grid			*GridLC;
 c_Label		*l_LC;
 c_RadioImage		*ri_LC;

 c_Grid			*GridFCL;
 c_Label		*l_FCL;
 c_RadioImage		*ri_FCL;

 c_Grid			*GridFLL;
 c_Label		*l_FLL;
 c_RadioImage		*ri_FLL;

 c_HighGraph		*highg;

 int			col_row;
 int			abscisse;
 int			ordonnee;

 int			old_col_row;
 int			old_abscisse;
 int			old_ordonnee;
};

extern sf_Box_AskGraph fc_Box_AskGraph;
extern c_Resource *GlobResources;

#endif
