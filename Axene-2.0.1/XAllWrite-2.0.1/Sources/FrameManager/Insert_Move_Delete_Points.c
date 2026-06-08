/*
** Insert_Move_Delete_Points.c for XAllWrite in FrameManager/
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
** Started on  Mon Jul 17 20:21:40 1995 Emmanuel Paris
** Last update Tue Apr 21 17:38:21 1998 Emmanuel Paris
*/


#include "Insert_Move_Delete_Points.h"

void init_Insert_Point();
status_t eventlook_Insert_Point();
void abort_Insert_Point();
void redraw_Insert_Point();

void move_point_mode();
void resize_cadre_mode();

void init_Delete_Point();
status_t eventlook_Delete_Point();
void abort_Delete_Point();
void redraw_Delete_Point();

/* 
*  Insert Point Function   *
*                          */

void init_Insert_Point(cfrmg)
c_FrMg	*cfrmg;
{
 DATA(Insert_Point);
  
 SET_DATA(Insert_Point);
 F(CURSOR).push_and_set_cursor(CURSOR, C_CROSS_ADD);  
 d(source) = d(prems) = FALSE;
 d(lp_insert) = (l_Point **)NULL;
 d(lp_connect) = NULL;
 d(function) = IDP_INSERT_POINT;
 if (!cfrmg->select_type)
 {
  F(cfrmg).undisplay_selected_lcadre(cfrmg);
  cfrmg->select_type = TRUE;
  F(cfrmg).display_selected_lcadre(cfrmg);
 } 
 F(cfrmg).init_recoordinate_point(cfrmg);  
}

status_t eventlook_Insert_Point(cfrmg, event)
c_FrMg	*cfrmg;
XEvent     event;
{
 l_Point	**retpnt;
 l_Point	*point_n;
 l_Point	*point_p;
 l_Point	pointD, M;
 l_Point	*H;
 l_Cadre	*lcadre;
 l_Cadre	*svglcadre;
 int		xm, ym;
 int		px, py;
 status_t	retour_func;
 Display	*display;
 Window		window;
 GC		gc;
 DATA(Insert_Point);
  
 GET_DATA(Insert_Point);
 display = cfrmg->X_info.display;
 window=cfrmg->X_info.window;
 gc=cfrmg->X_info.gc_contour;
  
 F(cfrmg).recoordinate_point(cfrmg, &d(sxo), &d(syo), &d(xo), &d(yo), FALSE);
 F(cfrmg).recoordinate_point(cfrmg, &d(sxc), &d(syc), &d(xc), &d(yc), TRUE);
  
 retour_func=FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case MotionNotify:
  xm=event.xmotion.x;
  ym=event.xmotion.y;
  if (!d(source))
  {
   lcadre=F(cfrmg).get_cadre_and_point_by_xy(cfrmg, 
					     cfrmg->SelectedCadre, 
					     &retpnt, xm, ym);
   if (lcadre)
   {
    point_p=F(lcadre->cadre).getpreviouspoint(lcadre->cadre, *retpnt);
    point_n=F(lcadre->cadre).getnextpoint(lcadre->cadre, *retpnt);
    if ((point_n == point_p) || (!(*retpnt)->cont && !point_n->cont))
    {
     F(CURSOR).set_cursor(CURSOR, C_SQUARE_ADD);
     d(function)=IDP_CONNECT_POINT;
     d(lcadre)=lcadre;
     d(lp_first)=d(lp_connect)=retpnt[0];
     if (!retpnt[0]->cont)
     {
      if (!point_n->cont)
       d(lp_loop)=retpnt[0];
      else
       d(lp_loop)=point_n;
     }
     else
      d(lp_loop)=point_p;
    }
    Xc_free(retpnt);
   }
   else
   {
    lcadre=F(cfrmg).get_cadre_and_point_by_line(cfrmg, 
						cfrmg->SelectedCadre, 
						&retpnt, xm, ym);
    if (lcadre)
    {
     F(CURSOR).set_cursor(CURSOR, C_CIRCLE_ADD);
     d(function)=IDP_INSERT_POINT;
     if (d(lp_insert))
      Xc_free(d(lp_insert));
     d(lp_insert)=retpnt;
     d(lcadre)=lcadre;
    }
    else
    {
     F(CURSOR).set_cursor(CURSOR, C_CROSS_ADD);
     d(function)=IDP_INSERT_POINT;
     if (d(lp_insert))
      Xc_free(d(lp_insert));
     d(lp_insert)=(l_Point **)NULL;
    }
    d(xc)=xm; d(yc)=ym;
    d(sxc)=xm*cfrmg->sScale;
    d(syc)=ym*cfrmg->sScale;
    F(cfrmg).attract_point(cfrmg, &d(sxc), &d(syc), &d(xc), &d(yc));
   }
  }
  else
  {
   switch(d(function))
   {
   case IDP_INSERT_POINT:
    d(xc)=xm; d(yc)=ym;
    d(sxc)=xm*cfrmg->sScale;
    d(syc)=ym*cfrmg->sScale;
    F(cfrmg).attract_point(cfrmg, &d(sxc), &d(syc), &d(xc), &d(yc));
    break;
   case IDP_CONNECT_POINT:
    if (d(prems))
     XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
    d(xc)=xm; d(yc)=ym;
    d(sxc)=xm*cfrmg->sScale;
    d(syc)=ym*cfrmg->sScale;
    F(cfrmg).attract_point(cfrmg, &d(sxc), &d(syc), &d(xc), &d(yc));
    XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
    px=d(lcadre)->cadre->Forme->Orig_X;
    py=d(lcadre)->cadre->Forme->Orig_Y;
    if (d(lp_loop)->X+px<xm+HANDLE_X_ACT && 
	d(lp_loop)->X+px>xm-HANDLE_X_ACT &&
	d(lp_loop)->Y+py<ym+HANDLE_Y_ACT && 
	d(lp_loop)->Y+py>ym-HANDLE_Y_ACT )
    {
     F(CURSOR).set_cursor(CURSOR, C_CONNECT_POINT);
     d(connected)=TRUE;
    }
    else
    {
     F(CURSOR).set_cursor(CURSOR, C_CROSS_ADD);
     d(connected)=FALSE;
    }
    d(prems)=TRUE;
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
   switch(d(function))
   {
   case IDP_INSERT_POINT:
    if (d(lp_insert))
    {
     pointD.X=d(lp_insert)[0]->X-
      (d(lp_insert)[1]->Y-d(lp_insert)[0]->Y);
     pointD.Y=d(lp_insert)[0]->Y+
      (d(lp_insert)[1]->X-d(lp_insert)[0]->X);
     M.X=xm-d(lcadre)->cadre->Forme->Orig_X;
     M.Y=ym-d(lcadre)->cadre->Forme->Orig_Y;
     H=(l_Point *)projection_on_line
      (&pointD, 
       ((l_Point **)d(lp_insert))[0], 
       ((l_Point **)d(lp_insert))[1], &M);
     F(cfrmg).undisplay_selected_lcadre(cfrmg);
     F(d(lcadre)->cadre).insertpoint
      (d(lcadre)->cadre,  H->X, H->Y, TRUE, d(lp_insert)[0]);
     if (d(lp_insert))
      Xc_free(d(lp_insert));
     F(cfrmg).redisplay_cadre(cfrmg, d(lcadre));
     F(cfrmg).display_selected_lcadre(cfrmg);
     Xc_free(H);
     FREE_DATA;
     retour_func=FUNCTION_FINISHED;
    }
    else
    {
     d(source)=TRUE;
     lcadre =
      (l_Cadre *)sort_desc_list_cadre(cfrmg->SelectedCadre);
     svglcadre=lcadre;
     F(cfrmg).undisplay_selected_lcadre(cfrmg);
     d(lpoint)=duplicate_list_point(lcadre->cadre->ListPoint);
     F(lcadre->cadre).addspoint(lcadre->cadre, 
				d(sxc)-lcadre->cadre->sOrig_X, 
				d(syc)-lcadre->cadre->sOrig_Y, 
				FALSE);
     F(CURSOR).set_cursor(CURSOR, C_SQUARE_ADD);
     d(function)=IDP_CONNECT_POINT;
     point_p=lcadre->cadre->ListPoint;
     while(point_p->NextPoint)
      point_p=point_p->NextPoint;
     d(lp_first)=d(lp_loop)=d(lp_connect)=point_p;
     d(lcadre)=F(cfrmg).get_cadre_by_name(cfrmg, lcadre->name);
     free_list_cadre(svglcadre, NO_DEL_CADRE);
     d(lcadre)->cadre->in_work=TRUE;
     d(xo)=d(xc); d(yo)=d(yc);
     d(sxo)=d(sxc); d(syo)=d(syc);
     d(xc)=xm;
     d(yc)=ym;
     d(sxc)=xm*cfrmg->sScale;
     d(syc)=ym*cfrmg->sScale;
     F(cfrmg).attract_point(cfrmg, &d(sxc), &d(syc), &d(xc), &d(yc));
     d(connected)=d(prems)=FALSE;
    }
    break;
   case IDP_CONNECT_POINT:
    if (d(source))
    {
     if (d(connected))
     {
      if (d(prems))
      {
       XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
      }	      
      point_n=d(lp_connect)->NextPoint;
      d(lp_connect)->NextPoint=(l_Point *)NULL;
      trace_list_point(d(lp_first), display, window, gc, 
		       d(lcadre)->cadre->Forme->Orig_X, 
		       d(lcadre)->cadre->Forme->Orig_Y, FALSE);
      d(lp_connect)->NextPoint=point_n;
      F(cfrmg).redisplay_cadre(cfrmg, d(lcadre));
      F(cfrmg).display_selected_lcadre(cfrmg);
      d(lcadre)->cadre->in_work=FALSE;
      if (d(lp_insert))
       Xc_free(d(lp_insert));
      F(CURSOR).pop_cursor(CURSOR);
      free_list_point(d(lpoint));
      FREE_DATA;
		      
      retour_func=FUNCTION_FINISHED;
     }
     else
     {
      F(d(lcadre)->cadre).
       insertspoint(d(lcadre)->cadre, 
		    d(sxc)-d(lcadre)->cadre->sOrig_X, 
		    d(syc)-d(lcadre)->cadre->sOrig_Y, 
		    TRUE, d(lp_connect));
      d(lp_connect)=F(d(lcadre)->cadre).
       getnextpoint(d(lcadre)->cadre, d(lp_connect));
      d(xo)=d(xc); d(yo)=d(yc);
      d(sxo)=d(sxc); d(syo)=d(syc);
      d(xc)=xm;
      d(yc)=ym;
      d(sxc)=xm*cfrmg->sScale;
      d(syc)=ym*cfrmg->sScale;
      F(cfrmg).attract_point(cfrmg, &d(sxc), &d(syc), 
			     &d(xc), &d(yc));
      d(prems)=FALSE;
     }
    }
    else
    {
     F(cfrmg).undisplay_selected_lcadre(cfrmg);
     d(lpoint)=duplicate_list_point(d(lcadre)->cadre->ListPoint);
     d(lcadre)->cadre->in_work=TRUE;
     d(xc)=d(xo)=d(lp_connect)->X+d(lcadre)->cadre->Forme->Orig_X;
     d(yc)=d(yo)=d(lp_connect)->Y+d(lcadre)->cadre->Forme->Orig_Y;
     d(sxc)=d(sxo)=d(lp_connect)->sX+d(lcadre)->cadre->sOrig_X;
     d(syc)=d(syo)=d(lp_connect)->sY+d(lcadre)->cadre->sOrig_Y;
     d(source)=TRUE;
     d(connected)=d(prems)=FALSE;
    }
    break;
   }
   break;
  case 2:
   if (d(source) && d(function) == IDP_CONNECT_POINT)
   {
    if (d(prems))
    {
     XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
    }	      
    point_n=d(lp_connect)->NextPoint;
    d(lp_connect)->NextPoint=(l_Point *)NULL;
    trace_list_point(d(lp_first), display, window, gc, 
		     d(lcadre)->cadre->Forme->Orig_X, 
		     d(lcadre)->cadre->Forme->Orig_Y, FALSE);
    d(lp_connect)->NextPoint=point_n;
    F(cfrmg).redisplay_cadre(cfrmg, d(lcadre));
    F(cfrmg).display_selected_lcadre(cfrmg);
    d(lcadre)->cadre->in_work=FALSE;
    if (d(lp_insert))
     Xc_free(d(lp_insert));
    F(CURSOR).pop_cursor(CURSOR);
    free_list_point(d(lpoint));
    FREE_DATA;
	    
    retour_func=FUNCTION_FINISHED;
   }
   break;
  case 3:
   retour_func=FUNCTION_REINIT;
   break;
  }
  break;
 }
 return retour_func;
}

void abort_Insert_Point(cfrmg)
c_FrMg	*cfrmg;
{
 l_Point	*point_n;
 Display	*display;
 Window	window;
 GC		gc;
 DATA(Insert_Point);
  
 GET_DATA(Insert_Point);
 display=cfrmg->X_info.display;
 window=cfrmg->X_info.window;
 gc=cfrmg->X_info.gc_contour;
  
 if (d(lp_insert))
  Xc_free(d(lp_insert));
 if (d(source))
 {
  if (d(prems))
  {
   XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
  }	      
  point_n=d(lp_connect)->NextPoint;
  d(lp_connect)->NextPoint=(l_Point *)NULL;
  trace_list_point(d(lp_first), display, window, gc, 
		   d(lcadre)->cadre->Forme->Orig_X, 
		   d(lcadre)->cadre->Forme->Orig_Y, FALSE);
  d(lp_connect)->NextPoint=point_n;
  free_list_point(d(lcadre)->cadre->ListPoint);
  d(lcadre)->cadre->ListPoint=d(lpoint);
  d(lcadre)->cadre->in_work=FALSE;
  F(cfrmg).display_selected_lcadre(cfrmg);
 }
 FREE_DATA;
 F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Insert_Point(cfrmg)
c_FrMg	*cfrmg;
{
 l_Point	*point_n;
 Display	*display;
 Window	window;
 GC		gc;
 DATA(Insert_Point);
  
 GET_DATA(Insert_Point);
 display=cfrmg->X_info.display;
 window=cfrmg->X_info.window;
 gc=cfrmg->X_info.gc_contour;
  
 if (d(source))
 {
  point_n=d(lcadre)->cadre->ListPoint;
  d(lcadre)->cadre->ListPoint=d(lpoint);
  F(cfrmg).draw_cadre_contour(cfrmg, d(lcadre));
  d(lcadre)->cadre->ListPoint=point_n;
  if (d(prems))
  {
   XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
  }	      
  point_n=d(lp_connect)->NextPoint;
  d(lp_connect)->NextPoint=(l_Point *)NULL;
  trace_list_point(d(lp_first), display, window, gc, 
		   d(lcadre)->cadre->Forme->Orig_X, 
		   d(lcadre)->cadre->Forme->Orig_Y, FALSE);
  d(lp_connect)->NextPoint=point_n;
 }
}

/* 
*  Change Mode Function   *
*                         */

void move_point_mode(cfrmg)
c_FrMg	*cfrmg;
{
  
 if (!cfrmg->select_type)
 {
  F(cfrmg).undisplay_selected_lcadre(cfrmg);
  cfrmg->select_type=TRUE;
  F(cfrmg).display_selected_lcadre(cfrmg);
 }
}

void resize_cadre_mode(cfrmg)
c_FrMg	*cfrmg;
{
  
 if (cfrmg->select_type)
 {
  F(cfrmg).undisplay_selected_lcadre(cfrmg);
  cfrmg->select_type=FALSE;
  F(cfrmg).display_selected_lcadre(cfrmg);
 }
}

/* 
*  Delete Point Function   *
*                          */

void init_Delete_Point(cfrmg)
c_FrMg	*cfrmg;
{
 F(CURSOR).push_and_set_cursor(CURSOR, C_SELECT);
 if (!cfrmg->select_type)
 {
  F(cfrmg).undisplay_selected_lcadre(cfrmg);
  cfrmg->select_type=TRUE;
  F(cfrmg).display_selected_lcadre(cfrmg);
 }
}
status_t eventlook_Delete_Point(cfrmg, event)
c_FrMg	*cfrmg;
XEvent     event;
{
 l_Cadre	*lcadre;
 l_Point	**retpnt;
 int		xo, xm, yo, ym;
 status_t	retour_func;
 Display	*display;
 Window	window;
 GC		gc;
  
 display=cfrmg->X_info.display;
 window=cfrmg->X_info.window;
 gc=cfrmg->X_info.gc_contour;
 retour_func=FUNCTION_NOT_FINISHED;
 switch(event.type)
 {  
 case MotionNotify:
  xm=event.xmotion.x;
  ym=event.xmotion.y;
  lcadre=F(cfrmg).get_cadre_and_point_by_xy(cfrmg, 
					    cfrmg->SelectedCadre, 
					    &retpnt, xm, ym);
  if (lcadre)
  {
   F(CURSOR).set_cursor(CURSOR, C_CIRCLE_DEL);
   Xc_free(retpnt);
  }
  else
   F(CURSOR).set_cursor(CURSOR, C_CROSS_DEL);
  break;
 case ButtonPress:
  switch(event.xbutton.button)
  {
  case 1:
   xo=event.xbutton.x;
   yo=event.xbutton.y;
   lcadre=F(cfrmg).get_cadre_and_point_by_xy(cfrmg, cfrmg->SelectedCadre, 
					     &retpnt, xo, yo);
   if (lcadre)
   {
    xm=retpnt[0]->X+lcadre->cadre->Forme->Orig_X;
    ym=retpnt[0]->Y+lcadre->cadre->Forme->Orig_Y;
    F(cfrmg).undisplay_selected_lcadre(cfrmg);
    if (F(lcadre->cadre).deletepoint(lcadre->cadre, retpnt[0]) 
	!= XC_CADRE_MUST_BE_DELETED)
     F(cfrmg).redisplay_cadre(cfrmg, lcadre);
    else
    {
     XDrawPoint(display, window, gc, lcadre->cadre->Forme->Orig_X, 
		lcadre->cadre->Forme->Orig_Y);
     F(cfrmg).delete_cadre(cfrmg, lcadre, DEL_CADRE);
    }
    F(cfrmg).display_selected_lcadre(cfrmg);
    retour_func = FUNCTION_FINISHED;
    Xc_free(retpnt);
   }
   break;
  case 3:
   retour_func = FUNCTION_REINIT;
   break;
  }
  break;
 } 
 return retour_func;
}

void abort_Delete_Point(cfrmg)
c_FrMg	*cfrmg;
{
 F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Delete_Point(cfrmg)
c_FrMg	*cfrmg;
{
}

