/*
** Select_Move_Edit_Text.c for XAllWrite in Page/
** Select, move and edit text.
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Mon Jun 16 13:57:40 1997 Emmanuel Paris
** Last update Sat Nov 14 16:32:11 1998 Emmanuel Paris
*/

#include "Select_Move_Edit_Text.h"
#include "KeyBoard_Handler.h"
#include "Select_Text_By_Mouse.h"
#include "Edit_Functions.h"
#include "Resize_Cadre.h"
#include "Move_Selected_Cadre.h"
#include "Copy_Selected_Cadre.h"
#include "Move_Point_On_Cadre.h"
#include "Move_Line_On_Cadre.h"
#include "Context_Menu_Page.h"
#include "Drag_And_Drop.h"
#include "Keyboard.h"
#include "MainInterface.h"
#include "Box_attrib.h"

extern c_Keyboard	*GlobKeyboard;
extern c_DragAndDrop	*GlobDragAndDrop;
extern c_MainInterface	*MainInterface1;

void init_Select_Move_Edit_Text();
status_t eventlook_Select_Move_Edit_Text();
void abort_Select_Move_Edit_Text();
void redraw_Select_Move_Edit_Text();
static int get_function_and_change_cursor();

sf_page_function fc_Select_Move_Edit_Text = 
{
 init_Select_Move_Edit_Text,
 eventlook_Select_Move_Edit_Text,
 abort_Select_Move_Edit_Text,
 redraw_Select_Move_Edit_Text
};

void init_Select_Move_Edit_Text(cpage)
c_Page	*cpage;
{
 Window	root;
 Window	fille;
 int		xrac,yrac;
 int		xm,ym;
 unsigned int	mod_button;
 DATA(Select_Move_Edit_Text);

 Xc_HISTORY(("Init func: %d", cpage->function_type));
 SET_DATA(Select_Move_Edit_Text);
 d(source) = d(outwindow) = d(pressed) = FALSE;
 d(nb_click) = 0;
 d(AutoScroll) = (c_AutoScroll *)NULL;
 F(GlobDragAndDrop).create_context(GlobDragAndDrop, cpage->X_info.window);
 XQueryPointer(cpage->X_info.display, cpage->X_info.window, &root, &fille,
	       &xrac, &yrac, &xm, &ym, &mod_button);
 d(shift) = ((mod_button & ShiftMask) != 0);
 d(cntrl) = ((mod_button & ControlMask) != 0);
 get_function_and_change_cursor(cpage, xm, ym, TRUE);
}

status_t eventlook_Select_Move_Edit_Text(cpage, event)
c_Page	*cpage;
XEvent	event;
{
 status_t	retour_func;
 boolean	touch;
 int		xm, ym;
 l_Cadre	*lcadre;
 c_FrMg		*frmg = cpage->frmg;
 DATA(Select_Move_Edit_Text);
  
 GET_DATA(Select_Move_Edit_Text);

 retour_func = FUNCTION_NOT_FINISHED;

 if (event.type != KeyPress && event.type != KeyRelease && 
     event.xany.window != cpage->X_info.window)
  return retour_func;
 
 F(GlobDragAndDrop).test_context(GlobDragAndDrop, cpage->X_info.window);

 if (d(AutoScroll))
  F(d(AutoScroll)).eventlook(d(AutoScroll), &event);

 switch(event.type)
 {
 case KeyPress:
  if (!d(source))
  {
   if (*cpage->select_type)
    touch = KB_KeyPress_Handler(cpage, &event, &d(cntrl), &d(shift));
   else
   {
    KB_OtherEvent_Handler(cpage, &event);
    if (GlobKeyboard->Control && !d(cntrl))
    {
     d(cntrl) = GlobKeyboard->Control;
     F(frmg).undisplay_selected_lcadre(frmg);
     frmg->select_group = FALSE;
     F(frmg).display_selected_lcadre(frmg);
    }
    d(shift) = GlobKeyboard->Shift;
    retour_func = F(frmg).eventlook(frmg, &event);

    touch = TRUE;
   }   
   
   if (touch)
    get_function_and_change_cursor(cpage, event.xkey.x, event.xkey.y, TRUE);
  }
  else
  {
   if (d(function) == SMS_MOVE_CADRE)
   {
    KB_OtherEvent_Handler(cpage, &event);
    d(shift) = GlobKeyboard->Shift;
    if (d(shift))
    {
     xm = d(xo); ym = d(yo);
     move_selected_cadre_abort(cpage);
     d(function) = SMS_COPY_CADRE;
     copy_selected_cadre_init(cpage, xm, ym); 
     xm = event.xkey.x; ym = event.xkey.y;
     copy_selected_cadre_to_xy(cpage, xm, ym); 
    }
   }
  }
  break;
 case KeyRelease:
  if (*cpage->select_type)
   touch = KB_KeyRelease_Handler(cpage, &event, &d(cntrl), &d(shift));
  else
  {
   KB_OtherEvent_Handler(cpage, &event);
   if (!GlobKeyboard->Control && d(cntrl) && !d(source))
   {
    d(cntrl) = GlobKeyboard->Control;
    F(frmg).undisplay_selected_lcadre(frmg);
    frmg->select_group = TRUE;
    F(frmg).display_selected_lcadre(frmg);
   }
   d(shift) = GlobKeyboard->Shift;
   if (!d(shift) && d(source) && d(function) == SMS_COPY_CADRE)
   {
    xm = d(xo); ym = d(yo);
    copy_selected_cadre_abort(cpage);
    d(function) = SMS_MOVE_CADRE;
    move_selected_cadre_init(cpage, xm, ym); 
    xm = event.xkey.x; ym = event.xkey.y;
    move_selected_cadre_to_xy(cpage, xm, ym); 
   }

   retour_func = F(frmg).eventlook(frmg, &event);
   touch = TRUE;
  }

  if (touch && !d(source))
   get_function_and_change_cursor(cpage, event.xkey.x, event.xkey.y, TRUE);
  break;
 case EnterNotify:
  d(outwindow) = FALSE;
  if (d(source) && d(pressed) &&
      (d(function) == SMS_MOVE_CADRE || d(function) == SMS_COPY_CADRE) )
  {
   Xc_TRACE(("entering page window"));
   xm = event.xcrossing.x;
   ym = event.xcrossing.y;
   move_selected_cadre_to_xy(cpage, xm, ym);
   drag_cadre_abort(d(drag), DD_DROP_UNKNOW_REASON);
   if (d(function) == SMS_MOVE_CADRE)
    F(CURSOR).set_cursor(CURSOR, C_MOVE);
   else
    F(CURSOR).set_cursor(CURSOR, C_COPY);
  }
  KB_OtherEvent_Handler(cpage, &event);
  d(cntrl) = GlobKeyboard->Control;
  d(shift) = GlobKeyboard->Shift;
  break;
 case LeaveNotify:
  d(outwindow) = TRUE;
  if (d(source) && d(pressed) && 
      (d(function) == SMS_MOVE_CADRE || d(function) == SMS_COPY_CADRE) )
  {
   Xc_TRACE(("leaving page window"));
   move_selected_cadre_trace(cpage);
   d(prems) = FALSE;
   if (d(function) == SMS_MOVE_CADRE)
    d(drag) = drag_cadre_init(cpage->X_info.display,
			      cpage->X_info.window,
			      frmg->SelectedCadre,
			      cpage->cursor,
			      DROP_FOR_MOVE,
			      d(xo), d(yo));
   else
    d(drag) = drag_cadre_init(cpage->X_info.display,
			      cpage->X_info.window,
			      frmg->SelectedCadre,
			      cpage->cursor,
			      DROP_FOR_COPY,
			      d(xo), d(yo));
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
  xm = event.xmotion.x;
  ym = event.xmotion.y;

  if (d(source) && !d(outwindow))
  {   
   switch(d(function))
   {
   case SMS_SELECT_TEXT:
   case SMS_SELECT_TEXT_LINE:
    if (event.xbutton.time > d(time)+60)
     d(pressed) = TRUE;
    select_text_by_mouse_to_xy(cpage, xm, ym);
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
   default:
    break;
   }
  }
  else
   get_function_and_change_cursor(cpage, xm, ym, TRUE);
  break;
 case ButtonRelease:
  xm = event.xbutton.x;
  ym = event.xbutton.y;
  if (d(source))
  {   
   switch(d(function))
   {
   case SMS_SELECT_TEXT:
   case SMS_SELECT_TEXT_LINE:
    select_text_by_mouse_done(cpage);
    d(pressed) = FALSE;
    break;
   case SMS_MOVE_CADRE:
    if (event.xbutton.time > d(time)+600)
    {
     d(pressed) = FALSE;
     move_selected_cadre_done(cpage); 
     retour_func = FUNCTION_MODIFIED;
    }
    else
     d(pressed) = FALSE;
    break;
   case SMS_COPY_CADRE:
    if (event.xbutton.time > d(time) + 600)
    {
     d(pressed) = FALSE;
     copy_selected_cadre_done(cpage); 
     retour_func = FUNCTION_MODIFIED;
    }
    break;
   case SMS_RESIZE_CADRE:
    if (event.xbutton.time > d(time) + 600)
    {
     Resize_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
    }
    break;
   case SMS_MOVE_POINT:
    if (event.xbutton.time > d(time) + 600)
    {
     move_point_on_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
    }
    break;
   case SMS_MOVE_LINE:
    if (event.xbutton.time > d(time) + 600)
    {
     move_line_on_cadre_done(cpage);
     retour_func = FUNCTION_MODIFIED;
    }
    break;
   default:
    break;
   }
  }
  break;
 case ButtonPress:

  if (*cpage->select_type)
   if (KB_ButtonPress_Handler(cpage, &event)) break;

  xm = event.xbutton.x;
  ym = event.xbutton.y;
  switch(event.xbutton.button)
  {
  case 1:
   if (!d(source))
    d(function)=
     get_function_and_change_cursor(cpage, xm, ym, FALSE);

   switch(d(function))
   {
   case SMS_SELECT_TEXT:
   case SMS_SELECT_TEXT_LINE:
    if (!d(cntrl))
    {
     if (!*cpage->select_type)
     {
      c_Page *page = cpage->Calibration->PageArea->SelectedPage;
      F(page->frmg).deselect_all_cadre(page->frmg, TRUE);

      F(cpage->Redraw).display_selection(cpage->Redraw, NULL);
      *cpage->select_type = TRUE;
     }

     if (event.xbutton.time < d(time) + DBL_CLK_DELAY &&
	 xm > d(xo) - 5 && xm < d(xo) + 5 &&
	 ym > d(yo) - 5 && ym < d(yo) + 5)
     {
      int btype;
      
      d(nb_click)++;
      if (d(function) == SMS_SELECT_TEXT_LINE && d(nb_click) == 2)
       d(nb_click) = 3;

      switch(d(nb_click))
      {
      case 2:
       Xc_TRACE(("double click."));
       btype = XA_WORD;
       break;
      case 3:
       Xc_TRACE(("triple click."));
       btype = XA_LINE;
       break;
      case 4:
       Xc_TRACE(("quadruple click."));
       btype = XA_PARAGRAPH;
       break;
      case 5:
       Xc_TRACE(("quintuple click."));
       btype = XA_PAGE;
       break;
      case 6:
      default:
       Xc_TRACE(("sextuple click."));
       btype = XA_ALL;
       break;
      }
      select_text_by_blocks(cpage, btype);
      d(time) = event.xbutton.time;
      break;
     }
     
     select_text_by_mouse_init(cpage, xm, ym);
     d(time) = event.xbutton.time;
     d(nb_click) = 1;
    }
    break;
   case SMS_SELECT_ONE_CADRE: 
    lcadre=(l_Cadre *)
     F(frmg).get_cadre_by_xy(frmg, xm, ym);
    F(frmg).deselect_all_cadre(frmg,TRUE);
    F(frmg).select_cadre(frmg,lcadre,TRUE);
    frmg->verrou=lcadre->cadre->verrou;
    break;
   case SMS_SELECT_CADRE:
    lcadre=(l_Cadre *)
     F(frmg).get_cadre_by_xy(frmg, xm, ym);
    F(frmg).select_cadre(frmg,lcadre,TRUE);
    if (lcadre->cadre->verrou)
     frmg->verrou=TRUE;
    break;
   case SMS_DESELECT_CADRE:
    lcadre=(l_Cadre *)
     F(frmg).get_cadre_by_xy(frmg, xm, ym);
    F(frmg).deselect_cadre(frmg,lcadre,TRUE);
    break;
   case SMS_DESELECT_ALL_CADRE:
    /*	    F(frmg).deselect_all_cadre(frmg,TRUE);
	    frmg->verrou=FALSE; */
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
	frmg->BaseStd->color_base,
	frmg->SelectedCadre);
      switch(F(boxattrib->PBox->Dialog).
	     waitForUnmap(boxattrib->PBox->Dialog))
      {
      case XcCD_CANCEL:
      case XcCD_DESTROY:
       retour_func = FUNCTION_MODIFIED;
       break;
      }
      frmg->modify_cadre_callback(frmg->callback_data, FALSE);
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
     d(time) = event.xbutton.time;
     d(pressed) = TRUE;
     move_selected_cadre_init(cpage, xm, ym);
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
     copy_selected_cadre_init(cpage, xm, ym); 
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
     Resize_cadre_init(cpage, xm, ym);
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
     move_point_on_cadre_init(cpage, xm, ym);
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
     move_line_on_cadre_init(cpage, xm, ym);
    }
    break;
   default:
    break;
   }
   break;
  case 2:
   if(!d(source) && !*cpage->select_type && !frmg->verrou)
   {
    F(frmg).undisplay_selected_lcadre(frmg);
    frmg->select_type ^= TRUE;
    if (frmg->select_type)
     cpage->Calibration->PageArea->function_type = 
      frmg->function_type = MOVE_POINT_MODE;
    else
     cpage->Calibration->PageArea->function_type = 
      frmg->function_type = RESIZE_CADRE_MODE;
    F(frmg).display_selected_lcadre(frmg);
   }
   break;
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
   get_function_and_change_cursor(cpage, xm, ym, TRUE);
  break;
 default:
  KB_OtherEvent_Handler(cpage, &event);
 }
 return retour_func;
}

void abort_Select_Move_Edit_Text(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Edit_Text);
  
 Xc_HISTORY(("Abort func: %d", cpage->function_type));
 GET_DATA(Select_Move_Edit_Text);
 
 if (d(source))
 {
  switch(d(function))
  {
  case SMS_SELECT_TEXT:
  case SMS_SELECT_TEXT_LINE:
   select_text_by_mouse_abort(cpage);
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
  default:
   break;
  }
 }
 FREE_DATA;
 F(GlobDragAndDrop).disable_drag(GlobDragAndDrop);
 F(GlobDragAndDrop).disactivate_drag(GlobDragAndDrop);
}

void redraw_Select_Move_Edit_Text(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Edit_Text);
  
 GET_DATA(Select_Move_Edit_Text);
 if (d(source))
 {
  switch(d(function))
  {
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
   F(cpage->frmg).draw_cadre_contour(cpage->frmg, d(lcadre));
   move_point_on_cadre_trace(cpage);
   break;
  case SMS_MOVE_LINE:
   F(cpage->frmg).draw_cadre_contour(cpage->frmg, d(lcadre));
   move_line_on_cadre_trace(cpage);
   break;
  }
 }
}

static int get_function_and_change_cursor(cpage, xm, ym, change)
c_Page	*cpage;
int	xm;
int	ym;
boolean	change;
{
 l_Point	**retpnt;
 l_Point	*rethandle;
 l_Cadre	*lcadre;
 coord_t	x, y;
 text_line_t	*tl = NULL;
 int		angle;
 int		function = 0;
 c_FrMg		*frmg = cpage->frmg;
 DATA(Select_Move_Edit_Text);
  
 GET_DATA(Select_Move_Edit_Text);
 
 x = xm * cpage->sScale;
 y = ym * cpage->sScale;

 do
 {
  if (!*cpage->select_type)
  {
   rethandle = F(frmg).get_handle_by_xy(frmg, 
					xm, ym, &angle, NULL);
   if (rethandle)
   {
    if (frmg->verrou)
    {
     if (change)
      F(CURSOR).set_cursor(CURSOR, C_LOCK_CADRE);
     function = SMS_DESELECT_ALL_CADRE;/* changed to NOP */
    }
    else
    {
     if (change)
     {
      switch(angle)
      {
      case 1:
       F(CURSOR).set_cursor(CURSOR, C_RESIZE_1);
       break;
      case 2:
       F(CURSOR).set_cursor(CURSOR, C_RESIZE_2);
       break;
      case 3:
       F(CURSOR).set_cursor(CURSOR, C_RESIZE_3);
       break;
      case 4:
       F(CURSOR).set_cursor(CURSOR, C_RESIZE_4);
       break;
      case 5:
       F(CURSOR).set_cursor(CURSOR, C_RESIZE_5);
       break;
      case 6:
       F(CURSOR).set_cursor(CURSOR, C_RESIZE_6);
       break;
      case 7:
       F(CURSOR).set_cursor(CURSOR, C_RESIZE_7);
       break;
      case 8:
       F(CURSOR).set_cursor(CURSOR, C_RESIZE_8);
       break;
      case 9:
      case 10:
       F(CURSOR).set_cursor(CURSOR, C_ARROW_MOVE);
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
    break;
   }
  }

  if (!*cpage->select_type && frmg->select_type)
  {
   lcadre = F(frmg).get_cadre_and_point_by_xy(frmg, frmg->SelectedCadre,
					      &retpnt, xm, ym);
   if (lcadre)
   {
    if (frmg->verrou)
    {
     if (change)
      F(CURSOR).set_cursor(CURSOR, C_LOCK_CADRE);
     function = SMS_DESELECT_ALL_CADRE;
    }
    else
    {
     if (change)
      F(CURSOR).set_cursor(CURSOR, C_GET_POINT);
     function = SMS_MOVE_POINT;
    }
    Xc_free(retpnt);
    break;
   }
  }
  
  if (!*cpage->select_type && frmg->select_type)
  {
   lcadre = F(frmg).get_cadre_and_point_by_line(frmg,
						frmg->SelectedCadre,
						&retpnt, xm, ym);
   if (lcadre)
   {
    if (frmg->verrou)
    {
     if (change)
      F(CURSOR).set_cursor(CURSOR, C_LOCK_CADRE);
     function = SMS_DESELECT_ALL_CADRE;
    }
    else
    {
     if (change)
      F(CURSOR).set_cursor(CURSOR, C_GET_LINE);
     function = SMS_MOVE_LINE;
    }
    Xc_free(retpnt);
    break;
   }
  }

  lcadre = 
   (l_Cadre *)F(frmg).get_cadre_by_xy(frmg, xm, ym);
  
  if (lcadre)
  {

   if ((lcadre->cadre->textflow_mode & XcTFM_HOLE_MASK) == XcTFM_OVER &&
       !d(shift))
   {
    if (F(cpage->Calibration).getTextLineByY
	(cpage->Calibration, cpage, y, x, &tl))
    {
     if (y >= tl->y && y < tl->y + tl->height)
     {
      function = SMS_SELECT_TEXT;
      if (change)
       F(CURSOR).set_cursor(CURSOR, C_EDIT_TEXT);
      break;
     }
    }
   }

   if (lcadre->cadre->selected)
   {
    if (frmg->verrou)
    {
     if (change)
      F(CURSOR).set_cursor(CURSOR, C_LOCK_CADRE);
     function = SMS_DESELECT_ALL_CADRE;/* changed to NOP */
    }
    else
    {
     if (d(cntrl) && frmg->SelectedCadre && 
	 frmg->SelectedCadre->NextCadre)
     {
      if (change)
       F(CURSOR).set_cursor(CURSOR, C_DESELECT_CADRE);
      function = SMS_DESELECT_CADRE;
     }
     else
     {
      if (d(shift))
      {
       if (change)
	F(CURSOR).set_cursor(CURSOR, C_ARROW_MOVE);
       function = SMS_COPY_CADRE;
      }
      else
      {
       if (change)
	F(CURSOR).set_cursor(CURSOR, C_ARROW_MOVE);
       function = SMS_MOVE_CADRE;
      }
     }
    }
   }
   else
   {
    if (change)
     F(CURSOR).set_cursor(CURSOR, C_SELECT_CADRE);
    if (d(cntrl))
     function = SMS_SELECT_CADRE;
    else
     function = SMS_SELECT_ONE_CADRE;
   }
  }
  else
  {
   if (!F(cpage->Calibration).getTextLineByY
       (cpage->Calibration, cpage, y, x, &tl))
   {
    if (tl)
     function = SMS_SELECT_TEXT_LINE;
    else
     function = SMS_NO_FUNCTION;
    if (change)
     F(CURSOR).set_cursor(CURSOR, C_NORMAL);
   }
   else
   {
    function = SMS_SELECT_TEXT;
    if (change)
     F(CURSOR).set_cursor(CURSOR, C_EDIT_TEXT);
   }
  }
 }
 while(0);
 
 if (function == SMS_COPY_CADRE || function == SMS_MOVE_CADRE ||
     function == SMS_SELECT_ONE_CADRE || function == SMS_SELECT_CADRE)
  F(GlobDragAndDrop).enable_drag(GlobDragAndDrop);
 else
  F(GlobDragAndDrop).disable_drag(GlobDragAndDrop);
 
 return function;
}


