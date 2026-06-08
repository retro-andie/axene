/*
** Create_Graph.c for XQuad in Feuille/
** define Create Graph function
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
** Last update Fri May 16 15:14:35 1997 Antoine Buat
*/

#define _PRIVATE_FEUILLE_
#include "FeuilleStd.h"
#include "Create_Graph.h"
#include "High_Graph.h"
#include "BG_Sector.h"
#include "VectorGraph.h"

static void init_Create_Graph();
static status_t eventlook_Create_Graph();
static void abort_Create_Graph();
static void redraw_Create_Graph();

sf_feuille_function fc_Create_Graph={
				     init_Create_Graph,
				     eventlook_Create_Graph,
				     abort_Create_Graph,
				     redraw_Create_Graph
				    };

static void init_Create_Graph(cfeuille)
c_Feuille	*cfeuille;
{
 DATA(Create_Graph);

 SET_DATA(Create_Graph);
  
 d(function_type) = cfeuille->frmg->function_type;
 d(step) = CG_CREATE_FRAME;
 F(cfeuille->frmg).set_function(cfeuille->frmg, CREATE_CADRE_RECTANGLE);
 INIT(cfeuille->frmg);
}

static status_t eventlook_Create_Graph(cfeuille, event)
c_Feuille	*cfeuille;
XEvent	event;
{ 
 status_t	retour_func;
 c_Cadre	*cadre;
 c_VectorGraph	*vectorg;
 c_HighGraph	*highgraph;
 HG_class_t	class;
  
 if (event.xany.serial == Xq_CELL)
 {
  retour_func = EVENTLOOK(cfeuille->frmg, event);
 }
 else
  retour_func = FUNCTION_NOT_FINISHED;
 if (retour_func == FUNCTION_FINISHED)
 {
  Xc_TRACE(("function FINISHED !!!!!!!!!!!!!!!!!!!!!"));
  cadre = cfeuille->frmg->SelectedCadre->cadre;
  vectorg = (c_VectorGraph *)NEW(c_VectorGraph)();
  vectorg->mode =  XcVG_AUTOSIZE;
  switch(cfeuille->function_type)
  {
  case GRAPH_HISTO_V:
   class = HG_HISTOGRAMME; 
   break;
  case GRAPH_HISTO_H:
   class = HG_BARS;
   break;
  case GRAPH_CURVE:
   class = HG_CURVE;
   break;
  case GRAPH_SURFACE:
   class = HG_SURFACE;
   break;
  case GRAPH_SECTOR:
   class = HG_SECTOR;
   break;
  case GRAPH_RADAR:
  default:
   class = HG_RADAR;
   break;
  }
  F(cfeuille).SetFunction(cfeuille, SELECT_MOVE_RESIZE_CELL);
  highgraph = (c_HighGraph *)
   NEW(c_HighGraph)(cfeuille->moteur, cfeuille->BaseStd, 
		    cfeuille->cell_region, class, 0, vectorg);
  if (highgraph)
  {
   F(cadre).attachobject(cadre, vectorg, CADRE_VECTOR);
   F(cfeuille->frmg).undisplay_selected_lcadre(cfeuille->frmg);
   F(cfeuille->frmg).draw_cadre(cfeuille->frmg,
				cfeuille->frmg->SelectedCadre); 
   F(cfeuille->frmg).display_selected_lcadre(cfeuille->frmg);
  }
  else
  {
   DELETE(c_VectorGraph)(vectorg);
   F(cfeuille->frmg).delete_selected_cadre(cfeuille->frmg);
  }
 }
 return retour_func;
}

static void abort_Create_Graph(cfeuille)
c_Feuille	*cfeuille;
{
 DATA(Create_Graph);
  
 GET_DATA(Create_Graph);
  
 if (cfeuille->frmg->function_data)
  ABORT(cfeuille->frmg);
 FREE_DATA;
}

static void redraw_Create_Graph(cfeuille)
c_Feuille	*cfeuille;
{
 REDRAW(cfeuille->frmg);
}





