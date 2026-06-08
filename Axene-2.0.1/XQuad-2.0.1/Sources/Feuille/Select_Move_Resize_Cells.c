/*
** Select_Move_Resize_Cells.c for XQuad in Feuille/
** Select_Move_Resize' functions
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
** Started on  Mon Sep 18 16:28:10 1995 Emmanuel Paris
** Last update Sat Jan  8 16:06:55 2000 Emmanuel Paris
*/

#include "Select_Region_By_KeyArrow.h"
#include "Select_Move_Resize_Cells.h"
#include "Select_Deselect_Origin.h"
#include "Select_Cells_Region.h"
#include "Select_Column_Region.h"
#include "Select_Row_Region.h"
#include "Resize_Column.h"
#include "Resize_Row.h"
#include "Spread_Horizontaly_Region.h"
#include "Insert_Horizontaly_Region.h"
#include "Spread_Verticaly_Region.h"
#include "Insert_Verticaly_Region.h"
#include "Move_Copy_Region.h"
#include "Resize_Cadre.h"
#include "Move_Selected_Cadre.h"
#include "Copy_Selected_Cadre.h"
#include "Drag_And_Drop.h"
#include "MainInterface.h"
#include "Box_attrib.h"

extern c_DragAndDrop	*GlobDragAndDrop;
extern c_MainInterface	*MainInterface1;

static void init_Select_Move_Resize_Cell();
status_t eventlook_Select_Move_Resize_Cell();
static void abort_Select_Move_Resize_Cell();
static void redraw_Select_Move_Resize_Cell();
static int get_function_and_change_cursor();

sf_feuille_function fc_Select_Move_Resize_Cell = 
{
 init_Select_Move_Resize_Cell,
 eventlook_Select_Move_Resize_Cell,
 abort_Select_Move_Resize_Cell,
 redraw_Select_Move_Resize_Cell
};

static void init_Select_Move_Resize_Cell(cfeuille)
c_Feuille	*cfeuille;
{
 Window	root;
 Window	fille;
 int		xrac,yrac;
 int		xm,ym;
 int		type;
 unsigned int	mod_button;
 DATA(Select_Move_Resize_Cell);

 SET_DATA(Select_Move_Resize_Cell);
  
 d(source)=d(outwindow)=d(pressed)=FALSE;
 d(AutoScroll) = (c_AutoScroll *)NULL;
 F(GlobDragAndDrop).create_context(GlobDragAndDrop, cfeuille->X_info.window);
 XQueryPointer(cfeuille->X_info.display, cfeuille->X_info.window, 
	       &root, &fille, &xrac, &yrac, &xm, &ym, &mod_button);
 d(cntrl)=((mod_button & ControlMask)!=0);
 d(shift)=((mod_button & ShiftMask)!=0);
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

status_t eventlook_Select_Move_Resize_Cell(cfeuille, event)
c_Feuille	*cfeuille;
XEvent		event;
{
 Display	*display;
 Window	window;
 Window	root;
 Window	fille;
 GC		gc;
 status_t	retour_func;
 char		tamp_cla[20];
 int		tailtamp=20;
 KeySym		symtouche;
 int		type;
 int		nb_car;
 int		xm,ym;
 int		xrac,yrac;
 l_Cadre	*lcadre;
 unsigned int	mod_button;
 boolean	touch, change;
 XComposeStatus	seqtouche;
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
 display=cfeuille->X_info.display;
 type = event.xany.serial;
 switch(type)
 {
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
 case Xq_CELL:
 default:
  window = cfeuille->X_info.window;
  gc = cfeuille->X_info.gc_contour;
  F(GlobDragAndDrop).test_context(GlobDragAndDrop, window);
  break;
 }
  
 if (d(AutoScroll))
  F(d(AutoScroll)).eventlook(d(AutoScroll), &event);
  
 retour_func = FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case KeyPress:
  nb_car = XLookupString((XKeyEvent *)&event,tamp_cla,tailtamp,
			 &symtouche,&seqtouche);
  if (!d(source))
  {
   touch = TRUE;
   switch(symtouche)
   {
   case XK_Control_R:
   case XK_Control_L:
    if (!d(cntrl) && !cfeuille->frmg->select_type)
    {
     F(cfeuille->frmg).undisplay_selected_lcadre(cfeuille->frmg);
     cfeuille->frmg->select_group = FALSE;
     F(cfeuille->frmg).display_selected_lcadre(cfeuille->frmg);
    }
    d(cntrl) = TRUE;
    break;
   case XK_Shift_R:
   case XK_Shift_L:
    d(shift) = TRUE;
    if (d(function) == SMS_COPY_REGION)
     F(CURSOR).set_cursor(CURSOR, C_COPY_REGION);
    break;
   case XK_Return:
    Xc_TRACE(("SetActiveCelluleAdvanceSelection"));
    F(cfeuille).SetActiveCelluleAdvanceSelection(cfeuille);
    break;
   case XK_Home:
    if (!d(shift) || !cfeuille->select_type)
    {
     if (cfeuille->cell_x != 0)
      F(cfeuille).SetActiveCellule(cfeuille, 0, cfeuille->cell_y, TRUE);
     else
      F(cfeuille).SetActiveCellule(cfeuille, 0, 0, TRUE);
    }
    else
     select_region_by_keyarrow(cfeuille, SRKA_HOME);
    break;
   case XK_End:
    if (!d(shift) || !cfeuille->select_type)
    {
     int last_col = cfeuille->column_orig + cfeuille->nb_column - 1;
     if (cfeuille->cell_x != last_col)
      F(cfeuille).SetActiveCellule(cfeuille, last_col, cfeuille->cell_y, TRUE);
     else
      F(cfeuille).SetActiveCellule
       (cfeuille, last_col, cfeuille->row_orig + cfeuille->nb_row - 1, TRUE);
    }
    else
     select_region_by_keyarrow(cfeuille, SRKA_END);
    break;
   case XK_Left:
    if (!d(shift) || !cfeuille->select_type)
     F(cfeuille).SetActiveCelluleRelative(cfeuille, -1, 0, TRUE);
    else
     select_region_by_keyarrow(cfeuille, SRKA_LEFT);
    break;
   case XK_Right:
    if (!d(shift) || !cfeuille->select_type)
     F(cfeuille).SetActiveCelluleRelative(cfeuille, 1, 0, TRUE);
    else
     select_region_by_keyarrow(cfeuille, SRKA_RIGHT);
    break;
   case XK_Up:
    if (!d(shift) || !cfeuille->select_type)
     F(cfeuille).SetActiveCelluleRelative(cfeuille, 0, -1, TRUE);
    else
     select_region_by_keyarrow(cfeuille, SRKA_UP);
    break;
   case XK_Down:
    if (!d(shift) || !cfeuille->select_type)
     F(cfeuille).SetActiveCelluleRelative(cfeuille, 0, 1, TRUE);
    else
     select_region_by_keyarrow(cfeuille, SRKA_DOWN);
    break;
   case XK_Prior:
    if (!d(shift) || !cfeuille->select_type)
     F(cfeuille).SetActiveCelluleRelative(cfeuille, 0, 
					  -(cfeuille->nb_row-1), TRUE);
    else
     select_region_by_keyarrow(cfeuille, SRKA_PAGEUP);
    break;
   case XK_Next:
    if (!d(shift) || !cfeuille->select_type)
     F(cfeuille).SetActiveCelluleRelative(cfeuille, 0, 
					  cfeuille->nb_row-1, TRUE);
    else
     select_region_by_keyarrow(cfeuille, SRKA_PAGEDOWN);
    break;
   default:
    touch = FALSE;
    break;
   }
   if (touch && !d(source))
    get_function_and_change_cursor(cfeuille, type,
				   event.xkey.x,event.xkey.y,TRUE);
  }
  else
  {
   switch(symtouche)
   {
   case XK_Control_R:
   case XK_Control_L:
    d(cntrl) = TRUE;
    break;
   case XK_Shift_R:
   case XK_Shift_L:
    d(shift) = TRUE;
    if (d(function) == SMS_COPY_REGION)
     F(CURSOR).set_cursor(CURSOR, C_COPY_REGION);
    break;
   }
  }
  break;
 case KeyRelease:
  nb_car = XLookupString((XKeyEvent *)&event,tamp_cla,tailtamp,
			 &symtouche,&seqtouche);
  touch = TRUE;
  switch(symtouche)
  {
  case XK_Control_R:
  case XK_Control_L:
   Xc_TRACE(("receive a Control RELEASE"));
   if (d(cntrl) && !d(source) && !cfeuille->frmg->select_type)
   {
    F(cfeuille->frmg).undisplay_selected_lcadre(cfeuille->frmg);
    cfeuille->frmg->select_group = TRUE;
    F(cfeuille->frmg).display_selected_lcadre(cfeuille->frmg);
   }
   d(cntrl) = FALSE;
   break;
  case XK_Shift_R:
  case XK_Shift_L:
   d(shift) = FALSE;
   if (d(function) == SMS_COPY_REGION)
    F(CURSOR).set_cursor(CURSOR, C_MOVE_REGION);
   break;
  default:
   touch = FALSE;
   break;
  }
  if (touch && !d(source))
  {
   get_function_and_change_cursor(cfeuille, type,
				  event.xkey.x,event.xkey.y,TRUE);
  }
  break;
 case EnterNotify:
  if (d(source) && d(pressed) && (type == Xq_CELL) &&
      (d(function) == SMS_MOVE_CADRE || d(function) == SMS_COPY_CADRE) )
  {
   d(outwindow)=FALSE;
   Xc_TRACE(("entering page window"));
   xm=event.xcrossing.x;
   ym=event.xcrossing.y;
   move_selected_cadre_to_xy(cfeuille, xm, ym);
   drag_cadre_abort(d(drag), DD_DROP_UNKNOW_REASON);
   if (d(function) == SMS_MOVE_CADRE)
    F(CURSOR).set_cursor(CURSOR, C_MOVE);
   else
    F(CURSOR).set_cursor(CURSOR, C_COPY);
  }
  else
  {
   XQueryPointer(cfeuille->X_info.display, window,&root,&fille,
		 &xrac,&yrac,&xm,&ym,&mod_button);
   change = FALSE;
   touch = ((mod_button & ControlMask) !=0 );
   if (touch != d(cntrl))
   {
    d(cntrl) = touch;
    change = TRUE;
   }
   touch = ((mod_button & ShiftMask) !=0 );
   if (touch != d(shift))
   {
    d(shift) = touch;
    change = TRUE;
   }
   if ( change && !d(source))
    get_function_and_change_cursor(cfeuille, type, xm, ym,TRUE);
  }
  break;
 case LeaveNotify:
  if (d(source) && d(pressed) && (type==Xq_CELL) &&
      (d(function)==SMS_MOVE_CADRE ||
       d(function)==SMS_COPY_CADRE) )
  {
   d(outwindow)=TRUE;
   Xc_TRACE(("leaving page window"));
   move_selected_cadre_trace(cfeuille);
   d(prems)=FALSE;
   if (d(function)==SMS_MOVE_CADRE)
    d(drag)=drag_cadre_init(cfeuille->X_info.display,
			    window,
			    cfeuille->frmg->SelectedCadre,
			    cfeuille->cursor,
			    DROP_FOR_MOVE,
			    d(xo),d(yo));
   else
    d(drag)=drag_cadre_init(cfeuille->X_info.display,
			    window,
			    cfeuille->frmg->SelectedCadre,
			    cfeuille->cursor,
			    DROP_FOR_COPY,
			    d(xo),d(yo));
   F(GlobDragAndDrop).set_callback(GlobDragAndDrop, DD_MOTION,
				   drag_cadre_motion, d(drag));
   F(GlobDragAndDrop).set_callback(GlobDragAndDrop, DD_ABORT,
				   drag_cadre_abort, d(drag));
   F(GlobDragAndDrop).set_callback(GlobDragAndDrop, DD_DROP,
				   drag_cadre_done, d(drag));
   F(GlobDragAndDrop).activate_drag(GlobDragAndDrop);
  }
  break;
 case MotionNotify:
  xm=event.xmotion.x;
  ym=event.xmotion.y;
  if (d(source) && !d(outwindow))
  {
   switch(d(function))
   {
   case SMS_SELECT_CELL:
    select_cells_region_to_xy(cfeuille, xm, ym);
    break;
   case SMS_SELECT_COLUMN:
    select_column_region_to_xy(cfeuille, xm, ym);
    break;
   case SMS_SELECT_ROW:
    select_row_region_to_xy(cfeuille, xm, ym);
    break;
   case SMS_RESIZE_COLUMN:
    resize_column_to_xy(cfeuille, xm, ym);
    break;
   case SMS_RESIZE_ROW:
    resize_row_to_xy(cfeuille, xm, ym);
    break;
   case SMS_MOVE_CADRE:
    move_selected_cadre_to_xy(cfeuille, xm, ym); 
    break;
   case SMS_COPY_CADRE:
    copy_selected_cadre_to_xy(cfeuille, xm, ym); 
    break;
   case SMS_RESIZE_CADRE:
    Resize_cadre_to_xy(cfeuille, xm, ym);
    break;
   case SMS_SPREAD_H_REGION:
    spread_horizontaly_region_to_xy(cfeuille, xm, ym);
    break;
   case SMS_SPREAD_V_REGION:
    spread_verticaly_region_to_xy(cfeuille, xm, ym);
    break;
   case SMS_INSERT_H_REGION:
    insert_horizontaly_region_to_xy(cfeuille, xm, ym);
    break;
   case SMS_INSERT_V_REGION:
    insert_verticaly_region_to_xy(cfeuille, xm, ym);
    break;
   case SMS_COPY_REGION:
    move_copy_region_to_xy(cfeuille, xm, ym);
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
   case SMS_SELECT_CELL:
    select_cells_region_done(cfeuille);
    break;
   case SMS_SELECT_COLUMN:
    select_column_region_done(cfeuille);
    break;
   case SMS_SELECT_ROW:
    select_row_region_done(cfeuille);
    break;
   case SMS_SELECT_ORIGIN:
    select_deselect_origin(cfeuille);
    break;
   case SMS_RESIZE_COLUMN:
    if (event.xbutton.time>d(time)+600)
    {
     resize_column_done(cfeuille);
     retour_func=FUNCTION_MODIFIED;
    }
    break;
   case SMS_RESIZE_ROW:
    if (event.xbutton.time>d(time)+600)
    {
     resize_row_done(cfeuille);
     retour_func=FUNCTION_MODIFIED;
    }
    break;
   case SMS_MOVE_CADRE:
    if (event.xbutton.time>d(time)+600)
    {
     d(pressed)=FALSE;
     move_selected_cadre_done(cfeuille); 
     retour_func=FUNCTION_MODIFIED;
    }
    else
     d(pressed) = FALSE;
    break;
   case SMS_COPY_CADRE:
    if (event.xbutton.time>d(time)+600)
    {
     d(pressed)=FALSE;
     copy_selected_cadre_done(cfeuille); 
     retour_func=FUNCTION_MODIFIED;
    }
    break;
   case SMS_RESIZE_CADRE:
    if (event.xbutton.time>d(time)+600)
    {
     Resize_cadre_done(cfeuille);
     retour_func=FUNCTION_MODIFIED;
    }
    break;
   case SMS_SPREAD_H_REGION:
    spread_horizontaly_region_done(cfeuille);
    retour_func=FUNCTION_MODIFIED;
    break;
   case SMS_SPREAD_V_REGION:
    spread_verticaly_region_done(cfeuille);
    retour_func=FUNCTION_MODIFIED;
    break;
   case SMS_INSERT_H_REGION:
    insert_horizontaly_region_done(cfeuille);
    retour_func=FUNCTION_MODIFIED;
    break;
   case SMS_INSERT_V_REGION:
    insert_verticaly_region_done(cfeuille);
    retour_func=FUNCTION_MODIFIED;
    break;
   case SMS_COPY_REGION:
    move_copy_region_done(cfeuille);
    retour_func=FUNCTION_MODIFIED;
    break;
   }
   get_function_and_change_cursor(cfeuille, type, xm, ym, TRUE);
  }
  break;
 case ButtonPress:
  xm=event.xbutton.x;
  ym=event.xbutton.y;
  switch(event.xbutton.button)
  {
  case 1:
   if (!d(source))
    d(function)=
     get_function_and_change_cursor(cfeuille, type, xm, ym, FALSE);
   switch(d(function))
   {
   case SMS_SELECT_CELL:
    select_cells_region_init(cfeuille, xm, ym);
    break;
   case SMS_SELECT_COLUMN:
    select_column_region_init(cfeuille, xm, ym);
    break;
   case SMS_SELECT_ROW:
    select_row_region_init(cfeuille, xm, ym);
    break;
   case SMS_SELECT_ORIGIN:
    select_deselect_origin(cfeuille);
    break;
   case SMS_RESIZE_COLUMN:
    if (d(source))
    {
     if (event.xbutton.time < d(time)+DBL_CLK_DELAY &&
	 xm > d(xc)-5 && xm < d(xc)+5 &&
	 ym > d(yc)-5 && ym < d(yc)+5)
     {
      int	column;
		    
      Xc_TRACE(("double click"));
      resize_column_abort(cfeuille);
      F(cfeuille).GetColumnBorderByX(cfeuille, xm, &column);
      resize_column_idealy(cfeuille, column);
     }
     else
     {
      resize_column_done(cfeuille);
      retour_func=FUNCTION_MODIFIED;
     }
    }
    else
    {
     d(time)=event.xbutton.time;
     resize_column_init(cfeuille, xm, ym);
    }
    break;
   case SMS_RESIZE_ROW:
    if (d(source))
    {
     if (event.xbutton.time < d(time)+DBL_CLK_DELAY &&
	 xm > d(xc)-5 && xm < d(xc)+5 &&
	 ym > d(yc)-5 && ym < d(yc)+5)
     {
      int	row;
		    
      Xc_TRACE(("double click"));
      resize_row_abort(cfeuille);
      F(cfeuille).GetRowBorderByY(cfeuille, ym, &row);
      resize_row_idealy(cfeuille, row);
     }
     else
     {
      resize_row_done(cfeuille);
      retour_func=FUNCTION_MODIFIED;
     }
    }
    else
    {
     d(time)=event.xbutton.time;
     resize_row_init(cfeuille, xm, ym);
    }
    break;
   case SMS_SELECT_ONE_CADRE: 
    /*	    if (cfeuille->select_type)
	    F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar); */
    lcadre=(l_Cadre *)
     F(cfeuille->frmg).get_cadre_by_xy(cfeuille->frmg, xm, ym);
    F(cfeuille->frmg).deselect_all_cadre(cfeuille->frmg,TRUE);
    F(cfeuille->frmg).select_cadre(cfeuille->frmg,lcadre,TRUE);
    cfeuille->frmg->verrou=lcadre->cadre->verrou;
    break;
   case SMS_SELECT_CADRE:
    /*	    if (cfeuille->select_type)
	    F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar); */
    lcadre=(l_Cadre *)
     F(cfeuille->frmg).get_cadre_by_xy(cfeuille->frmg, xm, ym);
    F(cfeuille->frmg).select_cadre(cfeuille->frmg,lcadre,TRUE);
    if (lcadre->cadre->verrou)
     cfeuille->frmg->verrou=TRUE;
    break;
   case SMS_DESELECT_CADRE:
    lcadre=(l_Cadre *)
     F(cfeuille->frmg).get_cadre_by_xy(cfeuille->frmg, xm, ym);
    F(cfeuille->frmg).deselect_cadre(cfeuille->frmg,lcadre,TRUE);
    break;
   case SMS_DESELECT_ALL_CADRE:
    /*	    F(cfeuille->frmg).deselect_all_cadre(cfeuille->frmg,TRUE);
	    cfeuille->frmg->verrou=FALSE; */
    break;
   case SMS_MOVE_CADRE:
    if (d(source))
    {
     if (event.xbutton.time < d(time)+DBL_CLK_DELAY &&
	 xm > d(xo)-5 && xm < d(xo)+5 &&
	 ym > d(yo)-5 && ym < d(yo)+5)
     {
      c_Box_attrib	*boxattrib;
      
      Xc_TRACE(("double click"));
      move_selected_cadre_abort(cfeuille);
      SET_WAIT_MODE_STATIC;
      XtDispatchEvent(&event);
      boxattrib = (c_Box_attrib *)NEW(c_Box_attrib)
       (MainInterface1->w_Main, "BoxAttribs",
	cfeuille->frmg->BaseStd->color_base,
	cfeuille->frmg->SelectedCadre);
      switch(F(boxattrib->PBox->Dialog).
	     waitForUnmap(boxattrib->PBox->Dialog))
      {
      case XcCD_CANCEL:
      case XcCD_DESTROY:
       retour_func=FUNCTION_MODIFIED;
       break;
      }
      cfeuille->frmg->
       modify_cadre_callback(cfeuille->frmg->callback_data);
      DELETE(c_Box_attrib)(boxattrib);
      UNSET_WAIT_MODE;
     }
     else
     {
      move_selected_cadre_done(cfeuille);
      retour_func=FUNCTION_MODIFIED;
     }
    }
    else
    { 
     /*	      if (cfeuille->select_type)
	      F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar); */
     d(time)=event.xbutton.time;
     d(pressed)=TRUE;
     move_selected_cadre_init(cfeuille, xm, ym);
    }
    break;
   case SMS_COPY_CADRE:
    if (d(source))
    {
     copy_selected_cadre_done(cfeuille); 
     retour_func=FUNCTION_MODIFIED;
    }
    else
    {
     /*	      if (cfeuille->select_type)
	      F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar); */
     d(time)=event.xbutton.time;
     d(pressed)=TRUE;
     copy_selected_cadre_init(cfeuille, xm, ym); 
    }  
    break;
   case SMS_RESIZE_CADRE:
    if (d(source))
    {
     Resize_cadre_done(cfeuille);
     retour_func=FUNCTION_MODIFIED;
    }
    else
    {
     /*	      if (cfeuille->select_type)
	      F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar); */
     d(time)=event.xbutton.time;
     Resize_cadre_init(cfeuille, xm, ym);
    }
    break;
   case SMS_SPREAD_H_REGION:
    spread_horizontaly_region_init(cfeuille, xm, ym);
    break;
   case SMS_SPREAD_V_REGION:
    spread_verticaly_region_init(cfeuille, xm, ym);
    break;
   case SMS_INSERT_H_REGION:
    insert_horizontaly_region_init(cfeuille, xm, ym);
    break;
   case SMS_INSERT_V_REGION:
    insert_verticaly_region_init(cfeuille, xm, ym);
    break;
   case SMS_COPY_REGION:
    move_copy_region_init(cfeuille, xm, ym);
    break;
   }
   break;
  case 2:
   if (!d(source))
   {
    d(function)=
     get_function_and_change_cursor(cfeuille, type, xm, ym, FALSE);
    switch(d(function))
    {
    case SMS_COPY_REGION:
    case SMS_SELECT_CELL:
    case SMS_SELECT_COLUMN:
    case SMS_SELECT_ROW:
     if (cfeuille->nb_cell_region==1)
     {
      if (d(shift))
       F(CURSOR).set_cursor(CURSOR, C_COPY_REGION);
      else
       F(CURSOR).set_cursor(CURSOR, C_MOVE_REGION);
      d(function) = SMS_COPY_REGION;
      move_copy_region_init(cfeuille, xm, ym);
     }
     break;
    }
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

static void abort_Select_Move_Resize_Cell(cfeuille)
c_Feuille	*cfeuille;
{
 Display	*display;
 Window	window;
 GC		gc;
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
 display=cfeuille->X_info.display;
 window=cfeuille->X_info.window;
 gc=cfeuille->X_info.gc_contour;
  
 if (d(source))
 {
  switch(d(function))
  {
  case SMS_SELECT_CELL:
   select_cells_region_abort(cfeuille);
   break;
  case SMS_SELECT_COLUMN:
   select_column_region_abort(cfeuille);
   break;
  case SMS_SELECT_ROW:
   select_row_region_abort(cfeuille);
   break;
  case SMS_RESIZE_COLUMN:
   resize_column_abort(cfeuille);
   break;
  case SMS_RESIZE_ROW:
   resize_row_abort(cfeuille);
   break;
  case SMS_MOVE_CADRE:
   move_selected_cadre_abort(cfeuille); 
   break;
  case SMS_COPY_CADRE:
   copy_selected_cadre_abort(cfeuille); 
   break;
  case SMS_RESIZE_CADRE:
   Resize_cadre_abort(cfeuille);
   break;
  case SMS_SPREAD_H_REGION:
   spread_horizontaly_region_abort(cfeuille);
   break;
  case SMS_SPREAD_V_REGION:
   spread_verticaly_region_abort(cfeuille);
   break;
  case SMS_INSERT_H_REGION:
   insert_horizontaly_region_abort(cfeuille);
   break;
  case SMS_INSERT_V_REGION:
   insert_verticaly_region_abort(cfeuille);
   break;
  case SMS_COPY_REGION:
   move_copy_region_abort(cfeuille);
   break;
  }
 }
 FREE_DATA;
 F(GlobDragAndDrop).disable_drag(GlobDragAndDrop);
 F(GlobDragAndDrop).disactivate_drag(GlobDragAndDrop);
}

static void redraw_Select_Move_Resize_Cell(cfeuille)
c_Feuille	*cfeuille;
{
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
 if (d(source))
 {
  switch(d(function))
  {
  case SMS_MOVE_CADRE:
   move_selected_cadre_trace(cfeuille);
   break;
  case SMS_COPY_CADRE:
   copy_selected_cadre_trace(cfeuille); 
   break;
  case SMS_RESIZE_CADRE:
   Resize_cadre_trace(cfeuille);
   break;
  case SMS_SPREAD_H_REGION:
   spread_horizontaly_region_trace(cfeuille);
   break;
  case SMS_SPREAD_V_REGION:
   spread_verticaly_region_trace(cfeuille);
   break;
  case SMS_INSERT_H_REGION:
   insert_horizontaly_region_trace(cfeuille);
   break;
  case SMS_INSERT_V_REGION:
   insert_verticaly_region_trace(cfeuille);
   break;
  case SMS_COPY_REGION:
   move_copy_region_trace(cfeuille);
   break;
  }
 }
}

static int get_function_and_change_cursor(cfeuille, type, xm, ym, change)
c_Feuille	*cfeuille;
int	type;
int	xm;
int	ym;
boolean	change;
{
 boolean	found;
 l_Point	*rethandle;
 l_Cadre	*lcadre;
 int		function = 0;
 int		angle;
 int		column, row;
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
  
 switch(type)
 {
 case Xq_CELL:
  found = FALSE;
  rethandle = F(cfeuille->frmg).get_handle_by_xy(cfeuille->frmg, 
						 xm, ym, &angle, NULL);
  if (rethandle)
  {
   if (cfeuille->frmg->verrou)
   {
    if (change)
     F(CURSOR).set_cursor(CURSOR,C_LOCK_CADRE);
    function = SMS_DESELECT_ALL_CADRE;/* changed to NOP */
   }
   else
   {
    if (change)
    {
     switch(angle)
     {
     case 1:
      F(CURSOR).set_cursor(CURSOR,C_RESIZE_1);
      break;
     case 2:
      F(CURSOR).set_cursor(CURSOR,C_RESIZE_2);
      break;
     case 3:
      F(CURSOR).set_cursor(CURSOR,C_RESIZE_3);
      break;
     case 4:
      F(CURSOR).set_cursor(CURSOR,C_RESIZE_4);
      break;
     case 5:
      F(CURSOR).set_cursor(CURSOR,C_RESIZE_5);
      break;
     case 6:
      F(CURSOR).set_cursor(CURSOR,C_RESIZE_6);
      break;
     case 7:
      F(CURSOR).set_cursor(CURSOR,C_RESIZE_7);
      break;
     case 8:
      F(CURSOR).set_cursor(CURSOR,C_RESIZE_8);
      break;
     case 9:
     case 10:
      F(CURSOR).set_cursor(CURSOR,C_ARROW_MOVE);
      break;
     }
    }
    switch(angle)
    {
    case 9:
    case 10:
     function = SMS_MOVE_CADRE;
     function = SMS_MOVE_CADRE;
     break;
    default:
     function = SMS_RESIZE_CADRE;
     break;
    }
   }
   Xc_free(rethandle);
   found=TRUE;
  }
  if (!found)
  {
   lcadre = 
    (l_Cadre *)F(cfeuille->frmg).get_cadre_by_xy(cfeuille->frmg,xm,ym);
   if (!lcadre)
   {
    F(cfeuille).GetCellByXY(cfeuille, xm, ym, &column, &row);
    if (F(cfeuille).IsSelected(cfeuille, column, row, Xq_CELL) &&
	cfeuille->nb_cell_region == 1 &&
	cfeuille->nb_col_region == 0 &&
	cfeuille->nb_row_region == 0)
    {
     if ( !found && 
	 F(cfeuille).GetColumnBorderByX(cfeuille, xm, &column))
     {
      if (column+1==cfeuille->cell_region->x_min)
      {
       if (d(cntrl))
       {
	if (change)
	 F(CURSOR).set_cursor(CURSOR, C_SPREAD_LEFT_REGION);
	function = SMS_SPREAD_H_REGION;
       }
       else
       {
	if (d(shift))
	{
	 if (change)
	  F(CURSOR).set_cursor(CURSOR, C_RESIZE_COL); 
	 function = SMS_INSERT_H_REGION;
	}
	else
	{
	 if (change)
	  F(CURSOR).set_cursor(CURSOR, C_MOVE_REGION);
	 function = SMS_COPY_REGION;
	}
       }
       found =TRUE;
      }
      if (!found && column == cfeuille->cell_region->x_max)
      {
       if (d(cntrl))
       {
	if (change)
	 F(CURSOR).set_cursor(CURSOR,C_SPREAD_RIGHT_REGION);
	function = SMS_SPREAD_H_REGION;
       }
       else
       {
	if (d(shift))
	{
	 if (change)
	  F(CURSOR).set_cursor(CURSOR, C_RESIZE_COL);
	 function = SMS_INSERT_H_REGION;
	}
	else
	{
	 if (change)
	  F(CURSOR).set_cursor(CURSOR, C_MOVE_REGION);
	 function = SMS_COPY_REGION;
	}
       }
       found =TRUE;
      }
     }
     if ( !found &&
	 F(cfeuille).GetRowBorderByY(cfeuille, ym, &row))
     {
      if (row+1==cfeuille->cell_region->y_min)
      {
       if (d(cntrl))
       {
	if (change)
	 F(CURSOR).set_cursor(CURSOR, C_SPREAD_UP_REGION);
	function = SMS_SPREAD_V_REGION;
       }
       else
       {
	if (d(shift))
	{
	 if (change)
	  F(CURSOR).set_cursor(CURSOR, C_RESIZE_ROW);
	 function = SMS_INSERT_V_REGION;
	}
	else
	{
	 if (change)
	  F(CURSOR).set_cursor(CURSOR, C_MOVE_REGION);
	 function = SMS_COPY_REGION;
	}
       }
       found =TRUE;
      }
      if (!found && row==cfeuille->cell_region->y_max)
      {
       if (d(cntrl))
       {
	if (change)
	 F(CURSOR).set_cursor(CURSOR, C_SPREAD_DOWN_REGION);
	function = SMS_SPREAD_V_REGION;
       }
       else
       {
	if (d(shift))
	{
	 if (change)
	  F(CURSOR).set_cursor(CURSOR, C_RESIZE_ROW);
	 function = SMS_INSERT_V_REGION;
	}
	else
	{
	 if (change)
	  F(CURSOR).set_cursor(CURSOR, C_MOVE_REGION);
	 function = SMS_COPY_REGION;
	}
       }
       found =TRUE;
      }
     }
    }
    if (!found)
    {
     if (change)
      F(CURSOR).set_cursor(CURSOR,C_SELECT_CELLS);
     function = SMS_SELECT_CELL;
    }
   }
   else
   {
    if (lcadre->cadre->selected)
    {
     if (cfeuille->frmg->verrou)
     {
      if (change)
       F(CURSOR).set_cursor(CURSOR,C_LOCK_CADRE);
      function = SMS_DESELECT_ALL_CADRE;/* changed to NOP */
     }
     else
     {
      if (d(cntrl) && cfeuille->frmg->SelectedCadre && 
	  cfeuille->frmg->SelectedCadre->NextCadre)
      {
       if (change)
	F(CURSOR).set_cursor(CURSOR,C_DESELECT_CADRE);
       function = SMS_DESELECT_CADRE;
      }
      else
      {
       if (d(shift))
       {
	if (change)
	 F(CURSOR).set_cursor(CURSOR,C_ARROW_MOVE);
	function = SMS_COPY_CADRE;
       }
       else
       {
	if (change)
	 F(CURSOR).set_cursor(CURSOR,C_ARROW_MOVE);
	function = SMS_MOVE_CADRE;
       }
      }
     }
    }
    else
    {
     if (change)
      F(CURSOR).set_cursor(CURSOR,C_SELECT_CADRE);
     if (d(cntrl))
      function = SMS_SELECT_CADRE;
     else
      function = SMS_SELECT_ONE_CADRE;
    }
   }
  }
  break;
 case Xq_ORIGIN:
  if (change)
   F(CURSOR_O).set_cursor(CURSOR_O, C_SELECT_ALL);
  function = SMS_SELECT_ORIGIN;
  break;
 case Xq_COLUMN:
  if (F(cfeuille).GetColumnBorderByX(cfeuille, xm, &column))
  {
   column -= cfeuille->column_orig;
   if (change)
   {
    if (cfeuille->column_width[column] == 0)
    {
     F(CURSOR_C).set_cursor(CURSOR_C, C_RESIZE_COL_RIGHT);
    }
    else
    {
     if (column+1 < cfeuille->nb_column && 
	 cfeuille->column_width[column+1] == 0)
      F(CURSOR_C).set_cursor(CURSOR_C, C_RESIZE_COL_LEFT);
     else
      F(CURSOR_C).set_cursor(CURSOR_C, C_RESIZE_COL);
    }
   }
   function = SMS_RESIZE_COLUMN;
  }
  else
  {
   if (change)
    F(CURSOR_C).set_cursor(CURSOR_C, C_SELECT_COL);
   function = SMS_SELECT_COLUMN;
  }
  break;
 case Xq_ROW:
  if (F(cfeuille).GetRowBorderByY(cfeuille, ym, &row))
  {
   row -= cfeuille->row_orig;
   if (change)
   {
    if (cfeuille->row_height[row] == 0)
    {
     F(CURSOR_R).set_cursor(CURSOR_R, C_RESIZE_ROW_DOWN);
    }
    else
    {
     if (row + 1 < cfeuille->nb_row && 
	 cfeuille->row_height[row + 1] == 0)
      F(CURSOR_R).set_cursor(CURSOR_R, C_RESIZE_ROW_UP);
     else
      F(CURSOR_R).set_cursor(CURSOR_R, C_RESIZE_ROW);
    }
   } 
   function = SMS_RESIZE_ROW;
  }
  else
  {
   if (change)
    F(CURSOR_R).set_cursor(CURSOR_R, C_SELECT_ROW);
   function = SMS_SELECT_ROW;
  }
  break;
 }
 if (function == SMS_COPY_CADRE || function == SMS_MOVE_CADRE ||
     function == SMS_SELECT_ONE_CADRE || function == SMS_SELECT_CADRE)
  F(GlobDragAndDrop).enable_drag(GlobDragAndDrop);
 else
  F(GlobDragAndDrop).disable_drag(GlobDragAndDrop);
 return function;
}




