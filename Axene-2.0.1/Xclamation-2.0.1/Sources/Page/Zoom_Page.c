/*
** Zoom_Page.c for Xclamation in Page/
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
** Started on  Mon Jul 17 19:50:05 1995 Emmanuel Paris
** Last update Wed Nov  4 18:46:13 1998 Emmanuel Paris
*/

#include "Zoom_Page.h"

void init_Zoom_Plus_Page();
status_t eventlook_Zoom_Plus_Page();
void abort_Zoom_Plus_Page();
void redraw_Zoom_Plus_Page();
void init_Zoom_Minus_Page();
status_t eventlook_Zoom_Minus_Page();
void abort_Zoom_Minus_Page();
void redraw_Zoom_Plus_Page();

void zoom_page_fit_in_width();
void zoom_page_fit_in_screen();
void zoom_page_at_real_size();
void zoom_page_at_percent();

/* ----------------------------------------------------------------- ** 
** Zoom plus                                                         ** 
** ----------------------------------------------------------------- */
void init_Zoom_Plus_Page(cpage)
c_Page	*cpage;
{
 cpage->function_data = (void *)1;
 F(CURSOR).push_and_set_cursor(CURSOR, C_MAG_PLUS);
}

status_t eventlook_Zoom_Plus_Page(cpage, event)
c_Page	*cpage;
XEvent	event;
{
 status_t	retour_func;
 float		factor;
  
 retour_func = FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case ButtonPress:
  switch(event.xbutton.button)
  {
  case 1:
   factor = SCALE_TO_PERCENT(cpage->sFactor);
   if (factor < ZOOM_PLUS_LIMIT)
   {
    factor += ZOOM_STEP;
    if (factor < ZOOM_PLUS_LIMIT)
    {
     F(cpage).change_page_scaling(cpage, SCALE_FROM_PERCENT(factor),
				  event.xbutton.x, event.xbutton.y);
    }
    else
    {
     F(cpage).change_page_scaling(cpage,
				  SCALE_FROM_PERCENT(ZOOM_PLUS_LIMIT),
				  event.xbutton.x, event.xbutton.y);
    }
   }
   else
   {
    cpage->function_data = NULL;
    F(CURSOR).pop_cursor(CURSOR);
    retour_func = FUNCTION_ABORTED;
   }
   break;
  case 3:
   cpage->function_data = NULL;
   F(CURSOR).pop_cursor(CURSOR);
   retour_func = FUNCTION_ABORTED;
   break;
  }
 }
 return retour_func;
}

void abort_Zoom_Plus_Page(cpage)
c_Page	*cpage;
{
 cpage->function_data = NULL;
 F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Zoom_Plus_Page(cpage)
c_Page	*cpage;
{
}

/* ----------------------------------------------------------------- ** 
** Zoom minus                                                        ** 
** ----------------------------------------------------------------- */
void init_Zoom_Minus_Page(cpage)
c_Page	*cpage;
{
 cpage->function_data = (void *)1;
 F(CURSOR).push_and_set_cursor(CURSOR, C_MAG_MINUS);
}

status_t eventlook_Zoom_Minus_Page(cpage, event)
c_Page	*cpage;
XEvent	event;
{
 status_t	retour_func;
 float		factor;
  
 retour_func = FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case ButtonPress:
  switch(event.xbutton.button)
  {
  case 1:
   factor = SCALE_TO_PERCENT(cpage->sFactor);
   if (factor>ZOOM_MINUS_LIMIT)
   {
    factor -= ZOOM_STEP;
    if (factor > ZOOM_MINUS_LIMIT)
    {
     F(cpage).change_page_scaling(cpage, SCALE_FROM_PERCENT(factor),
				  event.xbutton.x, event.xbutton.y);
    }
    else
    {
     F(cpage).change_page_scaling(cpage,
				  SCALE_FROM_PERCENT(ZOOM_MINUS_LIMIT),
				  event.xbutton.x, event.xbutton.y);
    }
   }
   else
   {
    cpage->function_data = NULL;
    F(CURSOR).pop_cursor(CURSOR);
    retour_func = FUNCTION_ABORTED;
   }
   break;
  case 3:
   cpage->function_data = NULL;
   F(CURSOR).pop_cursor(CURSOR);
   retour_func = FUNCTION_ABORTED;
   break;
  }
 }
 return retour_func;
}

void abort_Zoom_Minus_Page(cpage)
c_Page	*cpage;
{
 cpage->function_data = NULL;
 F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Zoom_Minus_Page(cpage)
c_Page	*cpage;
{
}

/* ----------------------------------------------------------------- ** 
** Zoom in Zone                                                      ** 
** ----------------------------------------------------------------- */
void init_Zoom_Page_In_Zone(cpage)
c_Page	*cpage;
{
 DATA(Zoom_Page_In_Zone);
  
 SET_DATA(Zoom_Page_In_Zone);
  
 F(CURSOR).push_and_set_cursor(CURSOR, C_ZOOM_IN_ZONE);
 d(source) = d(prems) = FALSE;
 F(cpage).init_recoordinate_point(cpage);
}

status_t eventlook_Zoom_Page_In_Zone(cpage, event)
c_Page	*cpage;
XEvent	event;
{
 int		ox, oy;
 int		width, height;
 int		thickness;
 int		depth;
 scale_t	new_factor;
 Display	*display;
 Window		mere;
 Window		window;
 GC		gc;
 status_t	retour_func;
 Status		code_ret;
 DATA(Zoom_Page_In_Zone);
  
 GET_DATA(Zoom_Page_In_Zone);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, 0, 0, &d(x1), &d(y1), FALSE);
 F(cpage).recoordinate_point(cpage, 0, 0, &d(x2), &d(y2), TRUE);
  
 code_ret = XGetGeometry(display, window, &mere, &ox, &oy,
		       &width, &height, &thickness, &depth);
  
 retour_func = FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case MotionNotify:
  if (d(source))
  {
   if (d(prems))
    XDrawRectangle(display, window, gc,
		   MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		   ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));
   d(x2) = event.xmotion.x;
	  
   d(y2) = d(y1) + (d(x2) - d(x1))*height / width;
   XDrawRectangle(display, window, gc,
		  MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		  ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));
   d(prems) = TRUE;
  }
  break;
 case ButtonRelease:
  if (d(source) && event.xbutton.time > d(time) + 600)
  {
   if (d(prems))
    XDrawRectangle(display, window, gc,
		   MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		   ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));
	
   new_factor = (scale_t)(((double)cpage->sFactor * (double)width) /
			  (double)ABS(d(x2) - d(x1)));
   if (SCALE_TO_PERCENT(new_factor) > ZOOM_PLUS_LIMIT)
    new_factor = SCALE_FROM_PERCENT(ZOOM_PLUS_LIMIT);
   d(prems) = FALSE;
   F(cpage).change_page_scaling(cpage, new_factor,
				d(x1) + ABS(d(x2) - d(x1)) / 2,
				d(y1) + ABS(d(y2) - d(y1)) / 2);
   F(CURSOR).pop_cursor(CURSOR);
   FREE_DATA;
	
   retour_func = FUNCTION_FINISHED; 
  }
  break;
 case ButtonPress:
  if (event.xbutton.button == 1)
  {
   if (d(source))
   {
    if (d(prems))
     XDrawRectangle(display, window, gc,
		    MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		    ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));
	  
    new_factor = (scale_t)(((double)cpage->sFactor *
			  (double)width) /
			 (double)ABS(d(x2) - d(x1)));
    if (SCALE_TO_PERCENT(new_factor)>ZOOM_PLUS_LIMIT)
     new_factor = SCALE_FROM_PERCENT(ZOOM_PLUS_LIMIT);
    d(prems) = FALSE;
    F(cpage).change_page_scaling
     (cpage, new_factor,
      d(x1) + ABS(d(x2) - d(x1)) / 2,
      d(y1) + ABS(d(y2) - d(y1)) / 2);
    F(CURSOR).pop_cursor(CURSOR);
    FREE_DATA;
	  
    retour_func = FUNCTION_FINISHED;
   }
   else
   {
    d(time) = event.xbutton.time;
    d(x1) = d(x2) = event.xbutton.x;
    d(y1) = d(y2) = event.xbutton.y;
    d(source) = TRUE;
   } 
  }
  else
  {
   if (d(source))
    retour_func = FUNCTION_ABORTED;
   else
    retour_func = FUNCTION_REINIT;
  }
  break;
 }
 return retour_func;
}

void abort_Zoom_Page_In_Zone(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window		window;
 GC		gc;
 DATA(Zoom_Page_In_Zone);
  
 GET_DATA(Zoom_Page_In_Zone);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, 0, 0, &d(x1), &d(y1), FALSE);
 F(cpage).recoordinate_point(cpage, 0, 0, &d(x2), &d(y2), TRUE);
  
 if (d(prems))
  XDrawRectangle(display, window, gc,
		 MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		 ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));
 FREE_DATA;
 F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Zoom_Page_In_Zone(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window		window;
 GC		gc;
 DATA(Zoom_Page_In_Zone);
  
 GET_DATA(Zoom_Page_In_Zone);
  
 F(cpage).recoordinate_point(cpage, 0, 0, &d(x1), &d(y1), FALSE);
 F(cpage).recoordinate_point(cpage, 0, 0, &d(x2), &d(y2), TRUE);
  
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
 if (d(prems))
  XDrawRectangle(display, window, gc,
		 MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		 ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));
}

/* ----------------------------------------------------------------- ** 
** Miscellaneous Zoom                                                ** 
** ----------------------------------------------------------------- */
void zoom_page_fit_in_width(cpage)
c_Page	*cpage;
{
  
 Window		mere;
 int		ox, oy;
 int		width, height;
 int		thickness;
 int		depth;
 Status		code_ret;
 coord_t	sWidth;
 coord_t	sScale;
 int		zoom_center_x;
 int		zoom_center_y;
 scale_t	new_factor;

 code_ret = XGetGeometry(cpage->X_info.display,
		       cpage->X_info.window, &mere, &ox, &oy,
		       &width, &height, &thickness, &depth); 
  
 sWidth = (cpage->Page_Contour.sWidth * 
	   cpage->Page_Contour.nb_page_horizontal) + 
	    2 * SCALE_FROM_MILLIMETERS(5);
  
 Xc_TRACE(("ScrHactif: %d ScrVactif: %d", 
	   cpage->ScrollBarH_actif, cpage->ScrollBarV_actif));

 if (cpage->ScrollBarH_actif)
  zoom_center_x = (cpage->Page_Contour.sOrig_X + 
		 (cpage->Page_Contour.sWidth *
		  cpage->Page_Contour.nb_page_horizontal) / 2 - 
		 cpage->sOrig_X) / cpage->sScale;
 else
  zoom_center_x = width/2;
 zoom_center_y = height/2;
 sScale = sWidth / cpage->RegleH->Size;
 Xc_TRACE(("A: %d  B: %d  C: %d", 
	   cpage->Width * sScale > cpage->sDeskArea_Width,
	   cpage->Width < cpage->RegleH->Size, !cpage->ScrollBarV_actif));
 if (cpage->Width*sScale > cpage->DeskArea_Width && 
     cpage->Width < cpage->RegleH->Size &&
     !cpage->ScrollBarV_actif)
 {
  sScale = sWidth/(cpage->RegleH->Size - SCROLLB_SIZE);
  Xc_TRACE(("add scrollbar to calcul"));
 }
 new_factor = (scale_t)(((double)COORD_DEFAULT*
			(double)SCALE_FROM_PERCENT(100)) /
			(double)sScale);
 Xc_TRACE(("change page scaling factor: %f", new_factor));
 F(cpage).change_page_scaling(cpage, new_factor,
			      zoom_center_x, zoom_center_y);
}

void zoom_page_fit_in_screen(cpage)
c_Page	*cpage;
{
  
 Window		mere;
 int		ox, oy;
 int		width, height;
 int		thickness;
 int		depth;
 Status		code_ret;
 coord_t	sWidth;
 coord_t	sHeight;
 coord_t	sScale_x;
 coord_t	sScale_y;
 coord_t	sScale;
 int		zoom_center_x;
 int		zoom_center_y;
 scale_t	new_factor;
  
 code_ret = XGetGeometry(cpage->X_info.display,
		       cpage->X_info.window, &mere, &ox, &oy,
		       &width, &height, &thickness, &depth); 
 sWidth = (cpage->Page_Contour.sWidth * 
	   cpage->Page_Contour.nb_page_horizontal) + 
	    2 * SCALE_FROM_MILLIMETERS(5);
 sHeight = (cpage->Page_Contour.sHeight * 
	    cpage->Page_Contour.nb_page_vertical) + 
	     2 * SCALE_FROM_MILLIMETERS(5);

 zoom_center_x = (cpage->Page_Contour.sOrig_X + 
		(cpage->Page_Contour.sWidth *
		 cpage->Page_Contour.nb_page_horizontal) / 2 - 
		cpage->sOrig_X) / cpage->sScale;
 zoom_center_y = (cpage->Page_Contour.sOrig_Y + 
		(cpage->Page_Contour.sHeight *
		 cpage->Page_Contour.nb_page_vertical) / 2 - 
		cpage->sOrig_Y) / cpage->sScale;
 sScale_x = sWidth / cpage->RegleH->Size;
 sScale_y = sHeight / cpage->RegleV->Size;
 sScale = MAX(sScale_x, sScale_y);
 new_factor = (scale_t)(((double)cpage->sScale * (double)cpage->sFactor) /
			(double)sScale);
 Xc_TRACE(("change page scaling factor: %f", new_factor));
 F(cpage).change_page_scaling(cpage, new_factor,
			      zoom_center_x, zoom_center_y);
}

void zoom_page_at_real_size(cpage)
c_Page	*cpage;
{
 int		zoom_center_x;
 int		zoom_center_y;
 scale_t	new_factor;
 coord_t	sWidth;
 int		Width;
 int		WidthMM;
  
 Width = DisplayWidth(cpage->X_info.display, cpage->X_info.ecran);
 WidthMM = DisplayWidthMM(cpage->X_info.display, cpage->X_info.ecran);
 if (WidthMM > 1000)
  WidthMM = 255;
 sWidth = SCALE_FROM_MILLIMETERS(WidthMM);
 new_factor = SCALE_FROM_COEF( (double)(Width * COORD_DEFAULT)
			      / (double)sWidth);
  
 Xc_TRACE(("Width: %d sWidth(mm): %d", Width, sWidth));
  
 zoom_center_x = (cpage->Page_Contour.sOrig_X + 
		(cpage->Page_Contour.sWidth *
		 cpage->Page_Contour.nb_page_horizontal) / 2 - 
		cpage->sOrig_X) / cpage->sScale;
 zoom_center_y = (cpage->Page_Contour.sOrig_Y + 
		(cpage->Page_Contour.sHeight *
		 cpage->Page_Contour.nb_page_vertical) / 2 - 
		cpage->sOrig_Y) / cpage->sScale;
  
 Xc_TRACE(("change page scaling factor: %f", new_factor));
 F(cpage).change_page_scaling(cpage, new_factor, 
			      zoom_center_x, zoom_center_y);
}

void zoom_page_at_percent(cpage, factor)
c_Page	*cpage;
scale_t	factor;
{
 Window	mere;
 int	ox, oy;
 int	width, height;
 int	thickness;
 int	depth;
 Status	code_ret;

 code_ret = XGetGeometry(cpage->X_info.display,
		       cpage->X_info.window, &mere, &ox, &oy,
		       &width, &height, &thickness, &depth);
 if (cpage->sFactor != factor)
 {
  F(cpage).change_page_scaling(cpage, factor, width / 2, height / 2);
 }
}




