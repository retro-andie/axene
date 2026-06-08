/*
** Dispatch_To_FrMg.c for XAllWrite in Page/
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
** Last update Mon Aug  3 20:28:18 1998 Emmanuel Paris
*/

#include "Dispatch_To_FrMg.h"

static void init_Dispatch_To_FrMg();
static status_t eventlook_Dispatch_To_FrMg();
static void abort_Dispatch_To_FrMg();
static void redraw_Dispatch_To_FrMg();

sf_page_function fc_Dispatch_To_FrMg = 
{
 init_Dispatch_To_FrMg,
 eventlook_Dispatch_To_FrMg,
 abort_Dispatch_To_FrMg,
 redraw_Dispatch_To_FrMg
};

static void init_Dispatch_To_FrMg(cpage)
c_Page	*cpage;
{
 DATA(Dispatch_To_FrMg);

 SET_DATA(Dispatch_To_FrMg);
  
 d(function_type) = cpage->frmg->function_type;
 Xc_TRACE(("init function %d", d(function_type)));
 INIT(cpage->frmg);
}

static status_t eventlook_Dispatch_To_FrMg(cpage, event)
c_Page	*cpage;
XEvent	event;
{ 
 status_t	retour_func;
  
 if (event.type != KeyPress && event.type != KeyRelease && 
     event.xany.window != cpage->X_info.window)
  return FUNCTION_NOT_FINISHED;

 retour_func = EVENTLOOK(cpage->frmg, event);

 if (retour_func == FUNCTION_FINISHED)
  Xc_TRACE(("function FINISHED !!!!!!!!!!!!!!!!!!!!!"));
 return retour_func;
}

static void abort_Dispatch_To_FrMg(cpage)
c_Page	*cpage;
{
 DATA(Dispatch_To_FrMg);
  
 GET_DATA(Dispatch_To_FrMg);
 
 if (cpage->frmg->function_data)
  ABORT(cpage->frmg);
 FREE_DATA;
}

static void redraw_Dispatch_To_FrMg(cpage)
c_Page	*cpage;
{
 REDRAW(cpage->frmg);
}
