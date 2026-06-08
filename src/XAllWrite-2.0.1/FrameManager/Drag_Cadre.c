/*
** Drag_Cadre.c for XAllWrite in FrameManager/
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
** Started on  Mon Jul 17 20:24:03 1995 Emmanuel Paris
** Last update Tue Jan 18 17:54:42 2000 Emmanuel Paris
*/

#include "Drag_Cadre.h"

#if defined(___rs6000) || \
 defined(__DECC)        /* __DECC : DEC C Compiler on Alpha platform */
#undef d
#else
#undef d(a)
#endif

#define d(menber) (drag->menber)

d_Drag_Cadre *drag_cadre_init();
void drag_cadre_abort();
void drag_cadre_done();
void drag_cadre_motion();
void drag_cadre_trace();
void drag_cadre_move_done();
void drag_cadre_copy_done();

d_Drag_Cadre *drag_cadre_init(display, window, lcadre, cursor, function, 
			      xo, yo)
Display		*display;
Window		window;
l_Cadre		*lcadre;
c_Cursor	*cursor;
boolean		function;
int		xo;
int		yo;
{
 d_Drag_Cadre	*drag;
 XGCValues	val_gc;
 
 drag = (d_Drag_Cadre *)Xc_malloc("drag data", sizeof(d_Drag_Cadre));
 drag->display = display;
 
 if (GlobColormap->Cblack)
  val_gc.function = GXxor;
 else
  val_gc.function = GXinvert;
 val_gc.plane_mask = -1;
 val_gc.background = GlobColormap->Cwhite;
 val_gc.foreground = GlobColormap->Cblack;
 val_gc.line_width = 0;
 val_gc.line_style = LineSolid;
 val_gc.cap_style = CapNotLast;
 val_gc.join_style = JoinMiter;
 drag->gc = XCreateGC(display,
		      DefaultRootWindow(display),
		      GCFunction | GCPlaneMask | GCForeground | 
		      GCBackground | GCLineWidth | GCLineStyle | GCCapStyle | 
		      GCJoinStyle, &val_gc);
 drag->root_window = window;
 drag->window = 0;
 drag->lcadre = lcadre;
 drag->cursor = cursor;
 drag->accepted = FALSE;
 drag->function = function;
 drag->sScale = COORD_DEFAULT;
 drag->x_orig = xo;
 drag->y_orig = yo;
 return drag;
}

void drag_cadre_abort(drag, reason, data)
d_Drag_Cadre	*drag;
int		reason;
d_Drag_data	*data;
{
 XEvent	event, event2;
 
 if (d(accepted))
 {
  drag_cadre_trace(drag);
 }
 XFreeGC(d(display), d(gc));
 if (reason == DD_DROP_ON_WRONG_WINDOW ||
     reason == DD_DROP_ABORTED_BY_USER)
 {
  event.type = ClientMessage;
  event.xclient.send_event = True;
  event.xclient.display = d(display);
  event.xclient.window = d(root_window);
  event.xclient.format = 32;
  DROP_FLAGS(event) = (int32_t)reason | DD_XCLAMATION_INDENTITY;
  DROP_ID(event) = (int32_t)data->pid;
  SET_DROP_DATA(event, drag->lcadre);
  XSendEvent(d(display), d(root_window), True, 0, &event);
 
  while(1)
  {
   XPeekEvent(d(display), &event2);
   if (event2.type == ClientMessage && 
       ((DROP_FLAGS(event2) & 0xffff00) == DD_XCLAMATION_INDENTITY))
    break;
   XNextEvent(d(display), &event2);
  }
 }
 F(GlobDragAndDrop).disable_drag(GlobDragAndDrop);
 F(GlobDragAndDrop).disactivate_drag(GlobDragAndDrop);
 Xc_free(drag);
}

void drag_cadre_done(drag, window_source, data_source, 
		     window_target, data_target)
d_Drag_Cadre	*drag;
Window		window_source;
d_Drag_data	*data_source;
Window		window_target;
d_Drag_data	*data_target;
{
 XEvent		event, event2;
 int		message;
 long		coord;
 int		px, py;
 
 if (d(accepted))
 {
  drag_cadre_trace(drag);
 }
 message = (d(function) == DROP_FOR_MOVE?DD_DROP_FOR_MOVE:DD_DROP_FOR_COPY) |
  DD_XCLAMATION_INDENTITY;
 XFreeGC(d(display), d(gc));
 
 px = ((coord_t)d(x_orig) * d(lcadre)->cadre->sScale) / d(sScale) - d(x_dest);
 py = ((coord_t)d(y_orig) * d(lcadre)->cadre->sScale) / d(sScale) - d(y_dest); 
 coord = ((px & 0xffff) << 16) | (py & 0xffff);
 
 event.type = ClientMessage;
 event.xclient.send_event = True;
 event.xclient.display = d(display);
 
 event.xclient.window = window_source;
 event.xclient.format = 32;
 DROP_FLAGS(event) = (int32_t)DD_DROP_FOR_SOURCE | message;
 DROP_ID(event) = (int32_t)data_target->pid;
 SET_DROP_DATA(event, drag->lcadre); 
 XSendEvent(d(display), window_source, True, 0, &event);
 
 event.xclient.window = window_target;
 DROP_VALUE(event) = (int32_t)coord;
 DROP_FLAGS(event) = (int32_t)DD_DROP_FOR_TARGET | message;
 DROP_ID(event) = (int32_t)data_source->pid;
 XSendEvent(d(display), window_target, True, 0, &event);

 while(1)
 {
  XPeekEvent(d(display), &event2);
  if (event2.type == ClientMessage && 
      ((DROP_FLAGS(event2) & 0xffff00) == DD_XCLAMATION_INDENTITY))
   break;
  XNextEvent(d(display), &event2);
 }
 
 F(GlobDragAndDrop).disable_drag(GlobDragAndDrop);
 F(GlobDragAndDrop).disactivate_drag(GlobDragAndDrop);
 Xc_free(drag);
}

void drag_cadre_motion(drag, event, data)
d_Drag_Cadre	*drag;
XEvent		event;
d_Drag_data	*data;
{
 int		xm;
 int		ym;
 
 xm = event.xmotion.x;
 ym = event.xmotion.y;
 if (event.xmotion.window)
 {
  if (data && (data->sFactor & (1 << 20)))
   F(d(cursor)).set_cursor(d(cursor), data->sFactor & 0xffff);
  else
  {
   if (!d(accepted))
   {
    if (d(function) == DROP_FOR_MOVE)
     F(d(cursor)).set_cursor(d(cursor), C_MOVE);
    else
     F(d(cursor)).set_cursor(d(cursor), C_COPY);
   }
   if (drag->window != event.xmotion.window)
   {
    if (drag->window)
     drag_cadre_trace(drag);
    drag->window = event.xmotion.window;
   }
   else
   {
    drag_cadre_trace(drag);
   }
   d(x_dest) = xm;
   d(y_dest) = ym;
   if (data)
    d(sScale) = (coord_t)
     (((double)COORD_DEFAULT * 
       (double)SCALE_FROM_PERCENT(100)) / 
      (double)(SCALE_FROM_PERCENT((double)data->sFactor / 100.0)));
   else
    d(sScale) = COORD_DEFAULT;
   drag_cadre_trace(drag);
   d(accepted) = TRUE;
  }
 }
 else
 {
  if (d(x_orig)>xm)
   F(d(cursor)).set_cursor(d(cursor), C_DRAG_L);
  else
   F(d(cursor)).set_cursor(d(cursor), C_DRAG_R);
  if (d(accepted))
  {
   drag_cadre_trace(drag);
   d(window) = 0;
   d(accepted) = FALSE;
  }
 }  
}

void drag_cadre_trace(drag)
d_Drag_Cadre	*drag;
{
 l_Cadre	*lcadre;
 c_Cadre	*cadre;
 Display	*display;
 Window	window;
 GC		gc;
 int			count_seg;
 int		px;
 int		py;
 l_Point	*listp;
 l_Point	*point_s;
 l_Point	*point_d;
 l_Point	*sv_pnt;
 coord_t	scale;
 
 display = drag->display;
 window = drag->window;
 gc = drag->gc;
 scale = drag->sScale;

 lcadre = drag->lcadre;
 while(lcadre)
 {
  cadre = lcadre->cadre;
  px = (cadre->sOrig_X - d(x_orig) * cadre->sScale) / scale + d(x_dest);
  py = (cadre->sOrig_Y - d(y_orig) * cadre->sScale) / scale + d(y_dest);
  listp = cadre->ListPoint;
  count_seg = 0;
  point_d = sv_pnt = listp;
  point_s = listp->NextPoint;
  while(point_s)
  {
   if (point_s->cont)
   {
    if (count_seg != MAX_SEGMENT)
    {
     segments[count_seg].x1 = point_s->sX / scale + px;
     segments[count_seg].y1 = point_s->sY / scale + py;
     segments[count_seg].x2 = point_d->sX / scale + px;
     segments[count_seg++].y2 = point_d->sY / scale + py;
    }
    else
    {
     XDrawSegments(display, window, gc, segments, MAX_SEGMENT);
     count_seg = 0;
    }
    point_d = point_s;
   }
   else
   {
    if (sv_pnt->NextPoint != point_d)
    {
     if (point_d != sv_pnt)
      if (count_seg != MAX_SEGMENT)
      {
       segments[count_seg].x1 = point_d->sX / scale + px;
       segments[count_seg].y1 = point_d->sY / scale + py;
       segments[count_seg].x2 = sv_pnt->sX / scale + px;
       segments[count_seg++].y2 = sv_pnt->sY / scale + py;
      }
      else
      {
       XDrawSegments(display, window, gc, segments, MAX_SEGMENT);
       count_seg = 0;
      }
     else
      XDrawPoint(display, window, gc, 
		 point_d->sX / scale + px, 
		 point_d->sY / scale + py);
    }
    sv_pnt = point_d = point_s;
   }
   point_s = point_s->NextPoint;
  }
  if (sv_pnt->NextPoint != point_d)
  {
   if (point_d != sv_pnt)
    if (count_seg != MAX_SEGMENT)
    {
     segments[count_seg].x1 = point_d->sX / scale + px;
     segments[count_seg].y1 = point_d->sY / scale + py;
     segments[count_seg].x2 = sv_pnt->sX / scale + px;
     segments[count_seg++].y2 = sv_pnt->sY / scale + py;
    }
    else
    {
     XDrawSegments(display, window, gc, segments, MAX_SEGMENT);
     count_seg = 0;
    }
   else
    XDrawPoint(display, window, gc, 
	       point_d->sX / scale + px, 
	       point_d->sY / scale + py);
  }
  if (count_seg)
  {
   XDrawSegments(display, window, gc, segments, count_seg);
  }
  lcadre = lcadre->NextCadre;
 }
}

void drag_cadre_move_done(cfrmg, listcadre, xo, yo)
c_FrMg	*cfrmg;
l_Cadre	*listcadre;
int	xo, yo;
{
 int		priority;
 char		*name;
 char		*name2;
 l_Cadre	*svglcadre;
 l_Cadre	*lcadre;
 l_Cadre	*lcadre2;
 boolean	recalibrate = FALSE;
 
 F(cfrmg).deselect_all_cadre(cfrmg, TRUE);
 priority = F(cfrmg).get_highest_priority(cfrmg);
 
 lcadre = svglcadre = (l_Cadre *)sort_asc_list_cadre(listcadre);
 while(lcadre)
 {
  c_Color *color;
  c_Cadre *frame;

  frame = lcadre->cadre;
  
  /*--- Change colors base ---*/
  color = frame->border_color;
  if(((color = F(color).virtualCopy(color, cfrmg->BaseStd)) == NULL) ||
     !F(frame).setOutlineColor(frame, color)) continue;

  color = frame->inside_color;
  if(((color = F(color).virtualCopy(color, cfrmg->BaseStd)) == NULL) ||
     !F(frame).setBackgroundColor(frame, color)) continue;
  
  frame->Forme->Orig_X = frame->sOrig_X / cfrmg->sScale - xo;
  frame->Forme->Orig_Y = frame->sOrig_Y / cfrmg->sScale - yo;
  frame->sOrig_X = frame->Forme->Orig_X * cfrmg->sScale;
  frame->sOrig_Y = frame->Forme->Orig_Y * cfrmg->sScale;
  frame->sScale = cfrmg->sScale;
  frame->sFactor = cfrmg->sFactor;
  F(frame).calculscaling(frame);
  F(frame).calculorigine(frame);
  F(frame).calculforme(frame, CONTOUR);
  
  /*--- Change object base ---*/
  if(frame->object_type != CADRE_EMPTY)
  {
   void *ptr;

   ptr = frame->object;
   F(cfrmg->cursor).push_and_set_cursor(cfrmg->cursor, C_WAIT);

   if((CHANGE_BASE(ptr)(ptr, cfrmg->BaseStd) != XC_NO_ERROR) ||
      (DISPLAY_INIT(ptr)(ptr, frame) != XC_NO_ERROR))
    F(frame).detachobject(frame);

   F(cfrmg->cursor).pop_cursor(cfrmg->cursor);
  } 
  
  name = Xc_strdup("name", lcadre->name);
  
  if(F(cfrmg).add_cadre(cfrmg, name, priority++, frame) ==
     XC_NAME_ALREADY_EXIST)
  {
   name2 = F(cfrmg).copy_cadre_name(cfrmg, name);
   Xc_free(name);
   name = name2;
   F(cfrmg).add_cadre(cfrmg, name, priority++, frame);
  }
  lcadre2 = F(cfrmg).get_cadre_by_name(cfrmg, name);
  Xc_free(name);
  F(lcadre2->cadre).reinit_X_info(lcadre2->cadre, cfrmg->X_info.display, 
				  cfrmg->X_info.window);

  F(cfrmg).draw_cadre(cfrmg, lcadre2);
  F(cfrmg).select_cadre(cfrmg, lcadre2, FALSE);
  
  if ((lcadre->cadre->textflow_mode & XcTFM_HOLE_MASK) > XcTFM_OVER)
   recalibrate = TRUE;
  
  lcadre = lcadre->NextCadre;
 } 

 F(cfrmg).calcul_select_handle(cfrmg);
 cfrmg->modify_cadre_callback(cfrmg->callback_data, recalibrate);
 if (!recalibrate) F(cfrmg).display_selected_lcadre(cfrmg);
 
 free_list_cadre(svglcadre, NO_DEL_CADRE);
 free_list_cadre(listcadre, NO_DEL_CADRE);
}

void drag_cadre_copy_done(cfrmg, listcadre, xo, yo)
c_FrMg	*cfrmg;
l_Cadre	*listcadre;
int	xo, yo;
{
 int		priority;
 char		*name;
 char		*name2;
 l_Cadre	*svglcadre;
 l_Cadre	*lcadre;
 l_Cadre	*lcadre2;
 c_Cadre	*cadre;
 boolean	recalibrate = FALSE;

 F(cfrmg).deselect_all_cadre(cfrmg, TRUE);
 priority = F(cfrmg).get_highest_priority(cfrmg);
 
 lcadre = svglcadre = (l_Cadre *)sort_asc_list_cadre(listcadre);
 while(lcadre)
 {
  cadre = (c_Cadre *)COPY(c_Cadre)(lcadre->cadre, cfrmg->BaseStd);
  cadre->Forme->Orig_X = cadre->sOrig_X / cfrmg->sScale - xo;
  cadre->Forme->Orig_Y = cadre->sOrig_Y / cfrmg->sScale - yo;
  cadre->sOrig_X = cadre->Forme->Orig_X * cfrmg->sScale;
  cadre->sOrig_Y = cadre->Forme->Orig_Y * cfrmg->sScale;
  cadre->sScale = cfrmg->sScale;
  cadre->sFactor = cfrmg->sFactor;
  F(cadre).calculscaling(cadre);
  F(cadre).calculorigine(cadre);
  F(cadre).calculforme(cadre, CONTOUR);
  if (cadre->object_type != CADRE_EMPTY)
  {
   F(cfrmg->cursor).push_and_set_cursor(cfrmg->cursor, C_WAIT);
   if (DISPLAY_INIT(cadre->object)(cadre->object, cadre) != XC_NO_ERROR)
    F(cadre).detachobject(cadre);
   F(cfrmg->cursor).pop_cursor(cfrmg->cursor);
  } 
  name = (char *) Xc_malloc("name", strlen(lcadre->name) + 1);
  strcpy(name, lcadre->name);
  
  if (F(cfrmg).add_cadre(cfrmg, name, priority++, cadre) ==
      XC_NAME_ALREADY_EXIST)
  {
   name2 = F(cfrmg).copy_cadre_name(cfrmg, name);
   Xc_free(name);
   name = name2;
   F(cfrmg).add_cadre(cfrmg, name, priority++, cadre);
  }
  lcadre2 = F(cfrmg).get_cadre_by_name(cfrmg, name);
  Xc_free(name);
  F(lcadre2->cadre).reinit_X_info(lcadre2->cadre, cfrmg->X_info.display, 
				  cfrmg->X_info.window);

  F(cfrmg).draw_cadre(cfrmg, lcadre2);
  F(cfrmg).select_cadre(cfrmg, lcadre2, FALSE);
  
  if ((lcadre2->cadre->textflow_mode & XcTFM_HOLE_MASK) > XcTFM_OVER)
   recalibrate = TRUE;

  lcadre = lcadre->NextCadre;
 } 
 F(cfrmg).calcul_select_handle(cfrmg);
 cfrmg->modify_cadre_callback(cfrmg->callback_data, recalibrate);
 if (!recalibrate) F(cfrmg).display_selected_lcadre(cfrmg);

 free_list_cadre(svglcadre, NO_DEL_CADRE);
}




