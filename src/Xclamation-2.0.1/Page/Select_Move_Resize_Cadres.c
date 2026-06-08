/*
** Select_Move_Resize_Cadres.c for Xclamation in Page/
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
** Started on  Mon Jul 17 19:53:09 1995 Emmanuel Paris
** Last update Thu Dec 30 19:30:24 1999 Emmanuel Paris
*/

#include "MainInterface.h"
#include "Select_Move_Resize_Cadres.h"
#include "Select_Cadre_In_Zone.h"
#include "Move_Selected_Cadre.h"
#include "Copy_Selected_Cadre.h"
#include "Move_Point_On_Cadre.h"
#include "Move_Line_On_Cadre.h"
#include "Move_Align_Mark.h"
#include "Resize_Cadre.h"
#include "Context_Menu_Page.h"
#include "Drag_And_Drop.h"
#include "Box_attrib.h"

extern c_DragAndDrop	*GlobDragAndDrop;
extern c_MainInterface	*MainInterface1;

void init_Select_Move_Resize_Cadre();
status_t eventlook_Select_Move_Resize_Cadre();
void abort_Select_Move_Resize_Cadre();
void redraw_Select_Move_Resize_Cadre();
static int get_function_and_change_cursor();

sf_page_function fc_Select_Move_Resize_Cadre = 
{
 init_Select_Move_Resize_Cadre,
 eventlook_Select_Move_Resize_Cadre,
 abort_Select_Move_Resize_Cadre,
 redraw_Select_Move_Resize_Cadre
};

void init_Select_Move_Resize_Cadre(cpage)
c_Page	*cpage;
{
 Window	root;
 Window	fille;
 int		xrac,yrac;
 int		xm,ym;
 boolean	shift;
 unsigned int	mod_button;
 DATA(Select_Move_Resize_Cadre);

 SET_DATA(Select_Move_Resize_Cadre);
  
 F(GlobColormap).use_color(GlobColormap,cpage->X_info.gc_contour,BLACK);
 d(source) = d(outwindow) = d(pressed) = FALSE;
 F(GlobDragAndDrop).create_context(GlobDragAndDrop, cpage->X_info.window);
 XQueryPointer(cpage->X_info.display,cpage->X_info.window,&root,&fille,
	       &xrac,&yrac,&xm,&ym,&mod_button);
 shift = ((mod_button & ShiftMask) != 0);
 if (d(shift) != shift)
 {
  if (!cpage->select_type)
  {
   F(cpage).undisplay_selected_lcadre(cpage);
   cpage->select_group = d(shift);
   F(cpage).display_selected_lcadre(cpage);
  }
  d(shift) = shift;
 }
 Xc_TRACE(("d(shift): %s  mod_button: %d ShiftMask: %d", d(shift)?"TRUE":"FALSE", mod_button, ShiftMask));
 get_function_and_change_cursor(cpage, xm, ym, TRUE);
}

status_t eventlook_Select_Move_Resize_Cadre(cpage, event)
c_Page	*cpage;
XEvent	event;
{
 Display	*display;
 Window	window;
 GC		gc;
 status_t	retour_func;
 l_Cadre	*lcadre;
 char		tamp_cla[20];
 int		tailtamp = 20;
 KeySym		symtouche;
 int		nb_car;
 int		xm,ym;
 XComposeStatus	seqtouche;
 DATA(Select_Move_Resize_Cadre);
  
 GET_DATA(Select_Move_Resize_Cadre);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
    
 F(GlobDragAndDrop).test_context(GlobDragAndDrop,cpage->X_info.window);

 retour_func = FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case KeyPress:
  nb_car = XLookupString((XKeyEvent *)&event, tamp_cla, tailtamp,
		       &symtouche, &seqtouche);
  if (symtouche == XK_Shift_R || symtouche == XK_Shift_L)
  {
   if (!d(shift) && !d(source) && !cpage->select_type)
   {
    F(cpage).undisplay_selected_lcadre(cpage);
    cpage->select_group = FALSE;
    F(cpage).display_selected_lcadre(cpage);
   }
   d(shift) = TRUE;
   if (!d(source))
    get_function_and_change_cursor(cpage, event.xkey.x, event.xkey. y, TRUE);
  }
  break;
 case KeyRelease:
  nb_car = XLookupString((XKeyEvent *)&event, tamp_cla, tailtamp,
		       &symtouche, &seqtouche);
  if (symtouche == XK_Shift_R || symtouche == XK_Shift_L)
  {
   if (d(shift) && !d(source) && !cpage->select_type)
   {
    F(cpage).undisplay_selected_lcadre(cpage);
    cpage->select_group = TRUE;
    F(cpage).display_selected_lcadre(cpage);
   }
   d(shift) = FALSE;
   if (!d(source))
    get_function_and_change_cursor(cpage, event.xkey.x, event.xkey.y, TRUE);
  }
  break;
 case MotionNotify:
  xm = event.xmotion.x;
  ym = event.xmotion.y;
  if (d(source) && !d(outwindow))
  {
   switch(d(function))
   {
   case SMS_SELECT_CADRE_IN_ZONE:
    select_cadre_in_zone_to_xy(cpage, xm, ym);
    break;
   case SMS_MOVE_CADRE:
    move_selected_cadre_to_xy(cpage, xm, ym);
    break;
   case SMS_COPY_CADRE:
    copy_selected_cadre_to_xy(cpage, xm, ym);
    break;
   case SMS_RESIZE_CADRE:
    Resize_cadre_to_xy(cpage, xm, ym);
    break;
   case SMS_MOVE_POINT:
    move_point_on_cadre_to_xy(cpage, xm, ym);
    break;
   case SMS_MOVE_LINE:
    move_line_on_cadre_to_xy(cpage, xm, ym);
    break;
   case SMS_MOVE_ALIGN_MARK:
    move_align_mark_to_xy(cpage, xm, ym);
    break;
   case SMS_SELECT_ONE_CADRE:
    if (event.xbutton.time>d(time)+100)
    {
     lcadre = (l_Cadre *)F(cpage).get_cadre_by_xy(cpage,d(xo),d(yo));
     F(cpage).deselect_all_cadre(cpage,TRUE);
     d(time) = event.xbutton.time;
     if (lcadre && !lcadre->cadre->verrou)
     {
      F(cpage).select_cadre(cpage,lcadre,TRUE);
      cpage->verrou = lcadre->cadre->verrou;
      d(pressed) = TRUE;
      d(function) = SMS_MOVE_CADRE;
      move_selected_cadre_init(cpage,d(xo),d(yo));	    
     }
     else
     {
      d(function) = SMS_SELECT_CADRE_IN_ZONE;
      select_cadre_in_zone_init(cpage,d(xo),d(yo));
     }
    }
    break;
   case SMS_SELECT_CADRE:
    if (event.xbutton.time > d(time)+250)
    {
     lcadre = (l_Cadre *)F(cpage).get_cadre_by_xy(cpage,d(xo),d(yo));
     d(time) = event.xbutton.time;
     if (lcadre && !lcadre->cadre->verrou)
     {
      F(cpage).select_cadre(cpage,lcadre,TRUE);
      cpage->verrou = lcadre->cadre->verrou;
      d(pressed) = TRUE;
      d(function) = SMS_COPY_CADRE;
      copy_selected_cadre_init(cpage,d(xo),d(yo));	    
     }
     else
     {
      d(function) = SMS_SELECT_CADRE_IN_ZONE;
      select_cadre_in_zone_init(cpage,d(xo),d(yo));
     }
    }
    break;
   case SMS_DESELECT_ALL_CADRE:
    if (event.xbutton.time>d(time)+100)
    {
     F(cpage).deselect_all_cadre(cpage,TRUE);
     cpage->verrou = FALSE;
     F(CURSOR).set_cursor(CURSOR,C_SELECT_ZONE);
     d(time) = event.xbutton.time;
     d(function) = SMS_SELECT_CADRE_IN_ZONE;
     select_cadre_in_zone_init(cpage,d(xo),d(yo));
    }
    break;
   }
  }
  else
   get_function_and_change_cursor(cpage,xm,ym,TRUE);
  break;
 case EnterNotify:
  if (d(source) && d(pressed) &&
      (d(function) == SMS_MOVE_CADRE || d(function) == SMS_COPY_CADRE) )
  {
   d(outwindow) = FALSE;
   Xc_TRACE(("entering page window"));
   xm = event.xcrossing.x;
   ym = event.xcrossing.y;
   move_selected_cadre_to_xy(cpage,xm,ym);
   drag_cadre_abort(d(drag),DD_DROP_UNKNOW_REASON);
   if (d(function) == SMS_MOVE_CADRE)
    F(CURSOR).set_cursor(CURSOR, C_MOVE);
   else
    F(CURSOR).set_cursor(CURSOR, C_COPY);
  }
  {
   Window noneW;
   int    noneI;
   boolean	shift;
   unsigned int	mod_button;
      
   XQueryPointer(cpage->X_info.display, cpage->X_info.window,
		 &noneW, &noneW, &noneI, &noneI, &xm,&ym, &mod_button);
   shift = ((mod_button & ShiftMask)!=0);
   if (d(shift) != shift)
   {
    if (!cpage->select_type)
    {
     F(cpage).undisplay_selected_lcadre(cpage);
     cpage->select_group = d(shift);
     F(cpage).display_selected_lcadre(cpage);
    }
    d(shift) = shift;
    if (!d(source))
     get_function_and_change_cursor(cpage, xm, ym, TRUE);
   }
  }
  break;
 case LeaveNotify:
  if (d(source) && d(pressed) &&
      (d(function)==SMS_MOVE_CADRE ||
       d(function)==SMS_COPY_CADRE) )
  {
   d(outwindow) = TRUE;
   Xc_TRACE(("leaving page window"));
   move_selected_cadre_trace(cpage);
   d(prems) = FALSE;
   if (d(function)==SMS_MOVE_CADRE)
    d(drag) = drag_cadre_init(cpage->X_info.display,
			    window,
			    cpage->SelectedCadre,
			    cpage->cursor,
			    DROP_FOR_MOVE,
			    d(xo),d(yo));
   else
    d(drag) = drag_cadre_init(cpage->X_info.display,
			    window,
			    cpage->SelectedCadre,
			    cpage->cursor,
			    DROP_FOR_COPY,
			    d(xo),d(yo));
   F(GlobDragAndDrop).set_callback(GlobDragAndDrop,DD_MOTION,
				   drag_cadre_motion,d(drag));
   F(GlobDragAndDrop).set_callback(GlobDragAndDrop,DD_ABORT,
				   drag_cadre_abort,d(drag));
   F(GlobDragAndDrop).set_callback(GlobDragAndDrop,DD_DROP,
				   drag_cadre_done,d(drag));
   F(GlobDragAndDrop).activate_drag(GlobDragAndDrop);
  }
  break;
 case ButtonRelease:
  xm = event.xbutton.x;
  ym = event.xbutton.y;

  if (event.xbutton.time > d(time)+600)
  {
   if (d(source))
   {
    switch(d(function))
    {
    case SMS_SELECT_CADRE_IN_ZONE:
     select_cadre_in_zone_done(cpage);
     break;
    case SMS_MOVE_CADRE:
     d(pressed) = FALSE;
     move_selected_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
     break;
    case SMS_COPY_CADRE:
     d(pressed) = FALSE;
     copy_selected_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
     break;
    case SMS_RESIZE_CADRE:
     Resize_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
     break;
    case SMS_MOVE_POINT:
     move_point_on_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
     break;
    case SMS_MOVE_LINE:
     move_line_on_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
     break;
    case SMS_MOVE_ALIGN_MARK:
     move_align_mark_done(cpage);
     retour_func = FUNCTION_MODIFIED;
     break;
    case SMS_SELECT_ONE_CADRE:
     lcadre = (l_Cadre *)F(cpage).get_cadre_by_xy(cpage,d(xo),d(yo));
     F(cpage).deselect_all_cadre(cpage,TRUE);
     F(cpage).select_cadre(cpage,lcadre,TRUE);
     cpage->verrou = lcadre->cadre->verrou;
     d(source) = FALSE;
     break;
    case SMS_SELECT_CADRE:
     lcadre = (l_Cadre *)F(cpage).get_cadre_by_xy(cpage,d(xo),d(yo));
     F(cpage).select_cadre(cpage,lcadre,TRUE);
     if (lcadre->cadre->verrou)
      cpage->verrou = TRUE;
     d(source) = FALSE;
     break;
    case SMS_DESELECT_ALL_CADRE:
     F(cpage).deselect_all_cadre(cpage,TRUE);
     cpage->verrou = FALSE;
     d(source) = FALSE;
     break;
    }
    get_function_and_change_cursor(cpage,xm,ym,TRUE);
   }
  }
  else
  {
   if (d(source))
   {
    switch(d(function))
    {
    case SMS_SELECT_CADRE_IN_ZONE:
     select_cadre_in_zone_done(cpage);
     get_function_and_change_cursor(cpage, xm, ym, TRUE);
     break;
    case SMS_MOVE_CADRE:
     d(pressed) = FALSE;
     break;
    case SMS_COPY_CADRE:
     if (ABS(d(xo) - xm) <= 5 && ABS(d(yo) - ym) <= 5)
     {
      lcadre = 
       (l_Cadre *)F(cpage).get_cadre_by_xy(cpage, d(xo), d(yo));
      copy_selected_cadre_abort(cpage);
      F(cpage).deselect_cadre(cpage, lcadre, TRUE);
      get_function_and_change_cursor(cpage, xm, ym, TRUE);
     }
     else
     {
      copy_selected_cadre_done(cpage);
      retour_func = FUNCTION_MODIFIED;
     }
     d(pressed) = FALSE;
     break;
    case SMS_SELECT_ONE_CADRE:
     lcadre = (l_Cadre *)F(cpage).get_cadre_by_xy(cpage,
						d(xo), d(yo));
     F(cpage).deselect_all_cadre(cpage, TRUE);
     F(cpage).select_cadre(cpage, lcadre, TRUE);
     cpage->verrou = lcadre->cadre->verrou;
     d(source) = FALSE;
     get_function_and_change_cursor(cpage, xm, ym, TRUE);
     break;
    case SMS_SELECT_CADRE:
     lcadre = (l_Cadre *)F(cpage).get_cadre_by_xy(cpage,
						d(xo), d(yo));
     F(cpage).select_cadre(cpage, lcadre, TRUE);
     if (lcadre->cadre->verrou)
      cpage->verrou = TRUE;
     d(source) = FALSE;
     get_function_and_change_cursor(cpage, xm, ym, TRUE);
     break;
    case SMS_DESELECT_ALL_CADRE:
     F(cpage).deselect_all_cadre(cpage, TRUE);
     cpage->verrou = FALSE;
     d(source) = FALSE;
     get_function_and_change_cursor(cpage, xm, ym, TRUE);
     break;
    case SMS_MOVE_ALIGN_MARK:
     move_align_mark_done(cpage);
     retour_func = FUNCTION_MODIFIED;
    default:
     get_function_and_change_cursor(cpage, xm, ym, TRUE);
     break;
    }
   }
  }
  break;
 case ButtonPress:
  xm = event.xbutton.x;
  ym = event.xbutton.y;
  switch(event.xbutton.button)
  {
  case 1:
   if (!d(source))
    d(function) = get_function_and_change_cursor(cpage, xm, ym, FALSE);
   switch(d(function))
   {
   case SMS_SELECT_CADRE_IN_ZONE:
    F(CURSOR).set_cursor(CURSOR, C_SELECT_ZONE);
    d(time) = event.xbutton.time;
    select_cadre_in_zone_init(cpage, xm, ym);
    break;
   case SMS_SELECT_ONE_CADRE:
    d(sel_time) = d(time) = event.xbutton.time;
    d(source) = TRUE;
    d(xo) = xm; d(yo) = ym;
    break;
   case SMS_SELECT_CADRE:
    d(sel_time) = d(time) = event.xbutton.time;
    d(source) = TRUE;
    d(xo) = xm; d(yo) = ym;
    break;
   case SMS_DESELECT_ALL_CADRE:
    d(time) = event.xbutton.time;
    d(source) = TRUE;
    d(xo) = xm; d(yo) = ym;
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
      move_selected_cadre_abort(cpage);
      SET_WAIT_MODE_STATIC;
      XtDispatchEvent(&event);
      boxattrib = (c_Box_attrib *)NEW(c_Box_attrib)
       (MainInterface1->w_Main, "BoxAttribs",
	cpage->BaseStd->color_base,
	cpage->SelectedCadre);
      switch(F(boxattrib->PBox->Dialog).waitForUnmap(boxattrib->PBox->Dialog))
      {
      case XcCD_CANCEL:
      case XcCD_DESTROY:
       retour_func = FUNCTION_MODIFIED;
       break;
      }
      DELETE(c_Box_attrib)(boxattrib);
      UNSET_WAIT_MODE;
     }
     else
     {
      move_selected_cadre_done(cpage);
      retour_func = FUNCTION_MODIFIED;
     }
    }
    else
    { 
     if (event.xbutton.time < d(sel_time) + DBL_CLK_DELAY &&
	 xm > d(xo)-5 && xm < d(xo)+5 &&
	 ym > d(yo)-5 && ym < d(yo)+5)
     {
      c_Box_attrib	*boxattrib;

      Xc_TRACE(("double click"));
      SET_WAIT_MODE_STATIC;
      XtDispatchEvent(&event);
      boxattrib = (c_Box_attrib *)NEW(c_Box_attrib)
       (MainInterface1->w_Main, "BoxAttribs",
	cpage->BaseStd->color_base,
	cpage->SelectedCadre);
      switch(F(boxattrib->PBox->Dialog).waitForUnmap(boxattrib->PBox->Dialog))
      {
      case XcCD_CANCEL:
      case XcCD_DESTROY:
       retour_func = FUNCTION_MODIFIED;
       break;
      }
      DELETE(c_Box_attrib)(boxattrib);
      UNSET_WAIT_MODE;
     }
     else
     {
      d(time) = event.xbutton.time;
      d(pressed) = TRUE;
      move_selected_cadre_init(cpage, xm, ym);
     }
    }
    break;
   case SMS_COPY_CADRE:
    if (d(source))
    {
     copy_selected_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
    }
    else
    {
     d(time) = event.xbutton.time;
     d(pressed) = TRUE;
     copy_selected_cadre_init(cpage,xm,ym);
    }
    break;
   case SMS_RESIZE_CADRE:
    if (d(source))
    {
     Resize_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
    }
    else
    {
     d(time) = event.xbutton.time;
     Resize_cadre_init(cpage,xm,ym);
    }
    break;
   case SMS_MOVE_POINT:
    if (d(source))
    {
     move_point_on_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
    }
    else
    {
     d(time) = event.xbutton.time;
     move_point_on_cadre_init(cpage,xm,ym);
    }
    break;
   case SMS_MOVE_LINE:
    if (d(source))
    {
     move_line_on_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
    }
    else
    {
     d(time) = event.xbutton.time;
     move_line_on_cadre_init(cpage,xm,ym);
    }
    break;
   case SMS_MOVE_ALIGN_MARK:
    move_align_mark_init(cpage,xm,ym);
    break;
   }
   break;
  case 2:
   if(!d(source))
   {
    if (!cpage->verrou)
    {
     F(cpage).undisplay_selected_lcadre(cpage);
     cpage->select_type^=TRUE;
     if (cpage->select_type)
      cpage->function_type = MOVE_POINT_MODE;
     else
      cpage->function_type = RESIZE_CADRE_MODE;
     F(cpage).display_selected_lcadre(cpage);
    }
   }
   break;
  case 3:
   if (d(source))
   {
    retour_func = FUNCTION_ABORTED;
   }
   else
   {
    if (context_menu_page_look(cpage, &event))
     return retour_func;
   }
   break;
  }
  if (!d(source))
   get_function_and_change_cursor(cpage,xm,ym,TRUE);
  break;
 }
 return retour_func;
}

void abort_Select_Move_Resize_Cadre(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window	window;
 GC		gc;
 DATA(Select_Move_Resize_Cadre);
  
 GET_DATA(Select_Move_Resize_Cadre);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 if (d(source))
 {
  switch(d(function))
  {
  case SMS_SELECT_CADRE_IN_ZONE:
   select_cadre_in_zone_abort(cpage);
   break;
  case SMS_MOVE_CADRE:
   move_selected_cadre_abort(cpage);
   break;
  case SMS_COPY_CADRE:
   copy_selected_cadre_abort(cpage);
   break;
  case SMS_RESIZE_CADRE:
   Resize_cadre_abort(cpage);
   break;
  case SMS_MOVE_POINT:
   move_point_on_cadre_abort(cpage);
   break;
  case SMS_MOVE_LINE:
   move_line_on_cadre_abort(cpage);
   break;
  case SMS_MOVE_ALIGN_MARK:
   move_align_mark_abort(cpage);
   break;
  }
 }
 FREE_DATA;
 F(GlobDragAndDrop).disable_drag(GlobDragAndDrop);
 F(GlobDragAndDrop).disactivate_drag(GlobDragAndDrop);
}

void redraw_Select_Move_Resize_Cadre(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Resize_Cadre);
  
 GET_DATA(Select_Move_Resize_Cadre);
 if (d(source))
 {
  switch(d(function))
  {
  case SMS_SELECT_CADRE_IN_ZONE:
   select_cadre_in_zone_trace(cpage);
   break;
  case SMS_MOVE_CADRE:
   move_selected_cadre_trace(cpage);
   break;
  case SMS_COPY_CADRE:
   copy_selected_cadre_trace(cpage);
   break;
  case SMS_RESIZE_CADRE:
   Resize_cadre_trace(cpage);
   break;
  case SMS_MOVE_POINT:
   F(cpage).draw_cadre_contour(cpage,d(lcadre));
   move_point_on_cadre_trace(cpage);
   break;
  case SMS_MOVE_LINE:
   F(cpage).draw_cadre_contour(cpage,d(lcadre));
   move_line_on_cadre_trace(cpage);
   break;
  case SMS_MOVE_ALIGN_MARK:
   move_align_mark_trace(cpage);
   break;
  }
 }
}

static int get_function_and_change_cursor(cpage,xm,ym,change)
c_Page	*cpage;
int	xm;
int	ym;
boolean	change;
{
 boolean	found;
 l_Point	**retpnt;
 l_Point	*rethandle;
 l_Cadre	*lcadre;
 void		*vhline;
 int		function = 0;
 int		angle;
 DATA(Select_Move_Resize_Cadre);
  
 GET_DATA(Select_Move_Resize_Cadre);
  
 found = FALSE;
 rethandle = F(cpage).get_handle_by_xy(cpage, xm, ym, &angle, NULL);
 if (rethandle)
 {
  if (cpage->verrou)
  {
   if (change)
    F(CURSOR).set_cursor(CURSOR, C_LOCK_CADRE);
   function = SMS_DESELECT_ALL_CADRE;
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
     if (cpage->select_type)
      F(CURSOR).set_cursor(CURSOR,C_GET_POINT);
     else
      F(CURSOR).set_cursor(CURSOR,C_ARROW_MOVE);
     break;
    case 10:
     if (cpage->select_type)
      F(CURSOR).set_cursor(CURSOR,C_GET_LINE);
     else
      F(CURSOR).set_cursor(CURSOR,C_ARROW_MOVE);
     break;
    }
   }
   switch(angle)
   {
   case 9:
    if (cpage->select_type)
     function = SMS_MOVE_POINT;
    else
     function = SMS_MOVE_CADRE;
    break;
   case 10:
    if (cpage->select_type)
     function = SMS_MOVE_LINE;
    else
     function = SMS_MOVE_CADRE;
    break;
   default:
    function = SMS_RESIZE_CADRE;
    break;
   }
   Xc_free(rethandle);
  }
  found = TRUE;
 }
  
 if (!found && cpage->select_type)
 {
  lcadre = F(cpage).get_cadre_and_point_by_xy(cpage,cpage->SelectedCadre,
					    &retpnt,xm,ym);
  if (lcadre)
  {
   if (cpage->verrou)
   {
    if (change)
     F(CURSOR).set_cursor(CURSOR,C_LOCK_CADRE);
    function = SMS_DESELECT_ALL_CADRE;
   }
   else
   {
    if (change)
     F(CURSOR).set_cursor(CURSOR,C_GET_POINT);
    function = SMS_MOVE_POINT;
   }
   Xc_free(retpnt);
   found = TRUE;
  }
 }
  
 if (!found && cpage->select_type)
 {
  lcadre = F(cpage).get_cadre_and_point_by_line(cpage,
					      cpage->SelectedCadre,
					      &retpnt,xm,ym);
  if (lcadre)
  {
   if (cpage->verrou)
   {
    if (change)
     F(CURSOR).set_cursor(CURSOR,C_LOCK_CADRE);
    function = SMS_DESELECT_ALL_CADRE;
   }
   else
   {
    if (change)
     F(CURSOR).set_cursor(CURSOR,C_GET_LINE);
    function = SMS_MOVE_LINE;
   }
   Xc_free(retpnt);
   found = TRUE;
  }
 }
  
 if (!found)
 {
  lcadre = (l_Cadre *)F(cpage).get_cadre_by_xy(cpage, xm, ym);
  if (!lcadre)
  {
   vhline = F(cpage).get_page_align_mark(cpage,xm,ym,&angle);
   if (vhline)
   {
    if (change)
    {
     if (angle == ALIGN_MARK_HORIZONTAL)
      F(CURSOR).set_cursor(CURSOR,C_REGLE_H);
     else
      F(CURSOR).set_cursor(CURSOR,C_REGLE_V);
    }
    function = SMS_MOVE_ALIGN_MARK;
   }
   else
   {
    if (change)
     F(CURSOR).set_cursor(CURSOR, C_NORMAL);
    if (cpage->SelectedCadre && !d(shift))
    {
     function = SMS_DESELECT_ALL_CADRE;
    }
    else
    {
     function = SMS_SELECT_CADRE_IN_ZONE;
    }
   }
  }
  else
  {
   if (lcadre->cadre->selected)
   {
    if (cpage->verrou)
    {
     if (change)
      F(CURSOR).set_cursor(CURSOR, C_LOCK_CADRE);
     function = SMS_DESELECT_ALL_CADRE;
    }
    else
    {
     if (d(shift))
     {
      if (change)
       F(CURSOR).set_cursor(CURSOR,C_DESELECT_CADRE);
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
   else
   {
    if (change)
     F(CURSOR).set_cursor(CURSOR,C_SELECT_CADRE);
    if (d(shift))
     function = SMS_SELECT_CADRE;
    else
     function = SMS_SELECT_ONE_CADRE;
   }
  }
 }
 if (function == SMS_COPY_CADRE || function == SMS_MOVE_CADRE ||
     function == SMS_SELECT_ONE_CADRE || function == SMS_SELECT_CADRE)
  F(GlobDragAndDrop).enable_drag(GlobDragAndDrop);
 else
  F(GlobDragAndDrop).disable_drag(GlobDragAndDrop);    
 return function;
}


