/*
** Select_Region_For_EditBar.c for XQuad in Feuille/
** Select Region For EditBar
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
** Started on  Thu Apr  6 14:49:27 1995 Emmanuel Paris
** Last update Sat Oct 26 19:42:32 1996 Emmanuel Paris
*/

#include "Select_Region_For_EditBar.h"
#include "Select_Cells_Region_For_EditBar.h"
#include "Select_Column_Region_For_EditBar.h"
#include "Select_Row_Region_For_EditBar.h"

static void init_Select_Region_For_EditBar();
status_t eventlook_Select_Region_For_EditBar();
static void abort_Select_Region_For_EditBar();
static void redraw_Select_Region_For_EditBar();
static int get_function_and_change_cursor();

sf_feuille_function fc_Select_Region_For_EditBar={
  init_Select_Region_For_EditBar,
  eventlook_Select_Region_For_EditBar,
  abort_Select_Region_For_EditBar,
  redraw_Select_Region_For_EditBar
};

static void init_Select_Region_For_EditBar(cfeuille)
c_Feuille	*cfeuille;
{
  Window	root;
  Window	fille;
  int		xrac,yrac;
  int		xm,ym;
   int		type;
  unsigned int	mod_button;
  DATA(Select_Region_For_EditBar);

  SET_DATA(Select_Region_For_EditBar);
  
  d(source)=d(outwindow)=FALSE;
  d(region) = d(region_column) = d(region_row) = NULL;
  d(AutoScroll) = (c_AutoScroll *)NULL;
  XQueryPointer(cfeuille->X_info.display, cfeuille->X_info.window, 
		&root, &fille, &xrac, &yrac, &xm, &ym, &mod_button);
  d(cntrl)=((mod_button & ControlMask)!=0);
  type = Xq_CELL;
  if (xm<0) 
  {
    xm += cfeuille->origin_width;
    type = Xq_ROW;
  }
  if (ym<0)
  {
    ym += cfeuille->origin_height;
    if (type == Xq_ROW)
      type = Xq_ORIGIN;
    else
      type = Xq_COLUMN;
  }
  get_function_and_change_cursor(cfeuille, type, xm, ym, TRUE);
}

status_t eventlook_Select_Region_For_EditBar(cfeuille,event)
c_Feuille	*cfeuille;
XEvent	event;
{
  Display	*display;
  Window	window;
  Window	root;
  Window	fille;
  GC		gc;
  status_t	retour_func;
  char		tamp_cla[20];
  int		tailtamp=20;
  KeySym	symtouche;
  int		type;
  int		nb_car;
  int		xm,ym;
  int		xrac,yrac;
  unsigned int	mod_button;
  boolean	touch;
  XComposeStatus	seqtouche;
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);
  display=cfeuille->X_info.display;
  type = event.xany.serial;
  switch(type)
  {
  case Xq_CELL:
    window = cfeuille->X_info.window;
    gc = cfeuille->X_info.gc_contour;
    break;
  case Xq_ORIGIN:
    window=cfeuille->X_info_O.window;
    gc = cfeuille->X_info_O.gc_contour;
    break;
  case Xq_COLUMN:
    window=cfeuille->X_info_C.window;
    gc = cfeuille->X_info_C.gc_contour;
    break;
  case Xq_ROW:
    window=cfeuille->X_info_R.window;
    gc = cfeuille->X_info_R.gc_contour;
    break;
  }
  
  if (d(AutoScroll))
    F(d(AutoScroll)).eventlook(d(AutoScroll), &event);
  
  retour_func=FUNCTION_NOT_FINISHED;
  switch(event.type)
    {
    case KeyPress:
      nb_car=XLookupString((XKeyEvent *)&event,tamp_cla,tailtamp,
			   &symtouche,&seqtouche);
      if (symtouche==XK_Control_R || symtouche==XK_Control_L)
	{
	  d(cntrl)=TRUE;
	  if (!d(source))
	    get_function_and_change_cursor(cfeuille, type,
					   event.xkey.x,event.xkey.y,TRUE);
	}
      break;
    case KeyRelease:
      nb_car=XLookupString((XKeyEvent *)&event,tamp_cla,tailtamp,
			   &symtouche,&seqtouche);
      if (symtouche==XK_Control_R || symtouche==XK_Control_L)
	{
	  d(cntrl)=FALSE;
	  if (!d(source))
	    get_function_and_change_cursor(cfeuille, type,
					   event.xkey.x,event.xkey.y,TRUE);
	}
      break;
    case EnterNotify:
      XQueryPointer(cfeuille->X_info.display,
		    cfeuille->X_info.window,&root,&fille,
		    &xrac,&yrac,&xm,&ym,&mod_button);
      touch=((mod_button & ControlMask)!=0);
      if (touch != d(cntrl))
	{
	  d(cntrl) = touch;
	  if (!d(source))
	    get_function_and_change_cursor(cfeuille, type, xm, ym,TRUE);
	}
      break;
    case MotionNotify:
      xm=event.xmotion.x;
      ym=event.xmotion.y;
      if (d(source) && !d(outwindow))
	{
	  switch(d(function))
	  {
	  case SRF_SELECT_CELL:
	    select_cells_region_for_editbar_to_xy(cfeuille, xm, ym);
	    break;
	  case SRF_SELECT_COLUMN:
	    select_column_region_for_editbar_to_xy(cfeuille, xm, ym); 
	    break;
	  case SRF_SELECT_ROW:
	    select_row_region_for_editbar_to_xy(cfeuille, xm, ym); 
	    break;
	  }	
	}
      else
	get_function_and_change_cursor(cfeuille, type, xm, ym, TRUE);
      break;
    case ButtonRelease:
      xm=event.xbutton.x;
      ym=event.xbutton.y;
      if (d(source))
	{
	  switch(d(function))
	  {
	  case SRF_SELECT_CELL:
	    select_cells_region_for_editbar_done(cfeuille);
	    break;
	  case SRF_SELECT_COLUMN:
	    select_column_region_for_editbar_done(cfeuille); 
	    break;
	  case SRF_SELECT_ROW:
	    select_row_region_for_editbar_done(cfeuille); 
	    break;
	  case SRF_SELECT_ORIGIN:
/*	    select_deselect_origin(cfeuille); */
	    break;
	  }
	}
      break;
    case ButtonPress:
      xm=event.xbutton.x;
      ym=event.xbutton.y;
      switch(event.xbutton.button)
	{
	case 1:
	  if (!d(source))
	    d(function) = 
	      get_function_and_change_cursor(cfeuille, type, xm, ym, FALSE);
	  switch(d(function))
	  {
	  case SRF_SELECT_CELL:
	    select_cells_region_for_editbar_init(cfeuille,xm,ym);
	    break;
	  case SRF_SELECT_COLUMN:
	    select_column_region_for_editbar_init(cfeuille,xm,ym); 
	    break;
	  case SRF_SELECT_ROW:
	    select_row_region_for_editbar_init(cfeuille,xm,ym); 
	    break;
	  case SRF_SELECT_ORIGIN:
/*	    select_deselect_origin(cfeuille); */
	    break;
	  }
	  break;
	case 2:
	  if(!d(source))
	  {
	    case SRF_SELECT_CELL:
	      break;
	    }
	  break;
	case 3:
	  retour_func=FUNCTION_ABORTED;
	  break;
	}
      if (!d(source))
	get_function_and_change_cursor(cfeuille, type, xm, ym, TRUE);
      break;
    }
  return retour_func;
}

static void abort_Select_Region_For_EditBar(cfeuille)
     c_Feuille	*cfeuille;
{
  Display	*display;
  Window	window;
  GC		gc;
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);
  display=cfeuille->X_info.display;
  window=cfeuille->X_info.window;
  gc=cfeuille->X_info.gc_contour;
  
  if (d(region))
    select_cells_region_for_editbar_abort(cfeuille);
  if (d(region_column))
    select_column_region_for_editbar_abort(cfeuille); 
  if (d(region_row))
    select_row_region_for_editbar_abort(cfeuille); 
  FREE_DATA;
}

static void redraw_Select_Region_For_EditBar(cfeuille)
     c_Feuille	*cfeuille;
{
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);
  
  Xc_TRACE(("redraw function"));
  if (d(region))
    select_cells_region_for_editbar_trace(cfeuille, TRUE);
  if (d(region_column))
    select_column_region_for_editbar_trace(cfeuille, TRUE); 
  if (d(region_row))
    select_row_region_for_editbar_trace(cfeuille, TRUE); 
}

static int get_function_and_change_cursor(cfeuille, type, xm, ym, change)
c_Feuille	*cfeuille;
int	type;
int	xm;
int	ym;
boolean	change;
{
  int		function;
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);
  
  switch(type)
    {
    case Xq_ORIGIN:
      if (change)
	F(CURSOR).set_cursor(CURSOR,C_RESIZE_7);
      function = SRF_SELECT_ORIGIN;
      break;
    case Xq_COLUMN:
      if (change)
	F(CURSOR).set_cursor(CURSOR,C_RESIZE_1);
      function = SRF_SELECT_COLUMN;
      break;
    case Xq_ROW:
      if (change)
	F(CURSOR).set_cursor(CURSOR,C_RESIZE_5);
      function = SRF_SELECT_ROW;
      break;
    case Xq_CELL:
    default:
      if (change)
	F(CURSOR).set_cursor(CURSOR,C_SELECT_CELLS);
      function = SRF_SELECT_CELL;
      break;
    }
  return function;
}






































































