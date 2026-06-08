/*
** Dispatch_To_FrMg.c for XQuad in Feuille/
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
** Last update Fri Jun  2 17:45:00 1995 Emmanuel Paris
*/

#include "Dispatch_To_FrMg.h"

static void init_Dispatch_To_FrMg();
static status_t eventlook_Dispatch_To_FrMg();
static void abort_Dispatch_To_FrMg();
static void redraw_Dispatch_To_FrMg();

sf_feuille_function fc_Dispatch_To_FrMg={
  init_Dispatch_To_FrMg,
  eventlook_Dispatch_To_FrMg,
  abort_Dispatch_To_FrMg,
  redraw_Dispatch_To_FrMg
};

static void init_Dispatch_To_FrMg(cfeuille)
     c_Feuille	*cfeuille;
{
  DATA(Dispatch_To_FrMg);

  SET_DATA(Dispatch_To_FrMg);
  
  d(function_type) = cfeuille->frmg->function_type;
  Xc_TRACE(("init function %d", d(function_type)));
  INIT(cfeuille->frmg);
}

static status_t eventlook_Dispatch_To_FrMg(cfeuille, event)
c_Feuille	*cfeuille;
XEvent	event;
{ 
  status_t	retour_func;
  
  if (event.xany.serial == Xq_CELL)
  {
    retour_func = EVENTLOOK(cfeuille->frmg, event);
  }
  else
    retour_func = FUNCTION_NOT_FINISHED;
  if (retour_func == FUNCTION_FINISHED)
    Xc_TRACE(("function FINISHED !!!!!!!!!!!!!!!!!!!!!"));
  return retour_func;
}

static void abort_Dispatch_To_FrMg(cfeuille)
c_Feuille	*cfeuille;
{
  DATA(Dispatch_To_FrMg);
  
  GET_DATA(Dispatch_To_FrMg);
  
  if (cfeuille->frmg->function_data)
    ABORT(cfeuille->frmg);
  FREE_DATA;
}

static void redraw_Dispatch_To_FrMg(cfeuille)
c_Feuille	*cfeuille;
{
  REDRAW(cfeuille->frmg);
}
