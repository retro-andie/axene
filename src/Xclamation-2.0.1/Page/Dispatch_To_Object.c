/*
** Dispatch_To_Object.c for Xclamation in Page/
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
** Started on  Mon Jul 17 20:24:46 1995 Emmanuel Paris
** Last update Fri Oct  9 20:35:27 1998 Emmanuel Paris
*/

#include "Dispatch_To_Object.h"
#include "Image.h"
#include "VectorGraph.h"
#include "ScanLine.h"

void init_Dispatch_To_Object();
status_t eventlook_Dispatch_To_Object();
void abort_Dispatch_To_Object();
void redraw_Dispatch_To_Object();

void init_Dispatch_To_Object(cpage)
c_Page *cpage;
{
 void	*object;
 int	tmp_function;
 DATA(Dispatch_To_Object);
  
 tmp_function = (int)cpage->function_data;
 SET_DATA(Dispatch_To_Object);
 d(cadre) = cpage->SelectedCadre->cadre;
 d(cursor) = cpage->cursor;
 d(old_function) = tmp_function;
 object = d(object) = cpage->SelectedCadre->cadre->object;
  
 d(function) = NULL;
 switch(cpage->function_type)
 {
 case MOVE_IMAGE:
  d(function) = &fc_Move_Image;
  d(function_type) = cpage->function_type;
  break;
 case ROTATE_IMAGE:
 case CENTER_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Recenter_Image((c_Image *)object,
				      DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;
 case INIT_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Reinit_Zoom_Image((c_Image *)object,
					 DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;
 case RESIZE_IMAGE:
 case SCALE_IMAGE:
 case MAPPING1_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).
   Mapping1_Mode((c_Image *)object,
		 DATA_ITSELF(Dispatch_To_Object));     
  UNSET_WAIT_MODE;
  break;
 case MAPPING2_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).
   Mapping2_Mode((c_Image *)object,
		 DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;
 case NORMAL_ASPECT_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Zoom_Normal_Mode((c_Image *)object,
					DATA_ITSELF(Dispatch_To_Object)); 
  UNSET_WAIT_MODE;
  break;
 case BEST_ASPECT_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Zoom_Best_Aspect_Mode((c_Image *)object,
					     DATA_ITSELF(Dispatch_To_Object)); 
  UNSET_WAIT_MODE;
  break;
 case MAX_ASPECT_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Zoom_Max_Aspect_Mode((c_Image *)object,
					    DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;
 case AUTO_RESIZE_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Zoom_Auto_Resize_Mode((c_Image *)object,
					     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;
 case FLIP_HORIZONTAL_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Flip_Horizontal_Image((c_Image *)object,
					     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;
 case FLIP_VERTICAL_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Flip_Vertical_Image((c_Image *)object,
					   DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;

 case RESIZE_VECTOR:
 case SCALE_VECTOR:
 case ROTATE_VECTOR:
  break;

  /*--- Centering vector ---*/
 case CENTER_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).center((c_VectorGraph *)object,
				    DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;

  /*--- Normal mode for vector ---*/
 case INIT_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).setMode((c_VectorGraph *)object,
				     XcVG_NORMAL,
				     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;

  /*--- Normal aspect for vector ---*/
 case NORMAL_ASPECT_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).setMode((c_VectorGraph *)object,
				     XcVG_NONE,
				     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;

  /*--- Best aspect for vector ---*/
 case BEST_ASPECT_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).setMode((c_VectorGraph *)object,
				     XcVG_BEST_ASPECT,
				     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;

  /*--- Auto resize for vector ---*/
 case AUTO_RESIZE_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).setMode((c_VectorGraph *)object,
				     XcVG_AUTOSIZE,
				     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;

  /*--- Max aspect for vector ---*/
 case MAX_ASPECT_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).setMode((c_VectorGraph *)object,
				     XcVG_MAX_ASPECT,
				     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;

  /*--- Horizontal flipping for vector ---*/
 case FLIP_HORIZONTAL_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).horizontalFlip((c_VectorGraph *)object,
					    DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;

  /*--- Vertical flipping for vector ---*/
 case FLIP_VERTICAL_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).verticalFlip((c_VectorGraph *)object,
					  DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;

  /*--- Hide / Display Text vector ---*/
 case TOGGLE_TEXT_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).textToggle((c_VectorGraph *)object,
					DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  break;

 case MOVE_VECTOR:
  d(function) = &fc_Move_Vector;
  d(function_type) = cpage->function_type;
  break;
  
#if 0
  {
   l_Cadre		*lcadre;
   c_ScanLine	*scanline;
   sl_Segment	*segment;
   int		nb_segment;
   int		i;
   coord_t	y;
   coord_t	px,py;
   coord_t	cx,cy;
   double	cs,ss;
   coord_t	dx,dy;
   XSegment	x_segment[4];
	
   lcadre= cpage->SelectedCadre;
   F(lcadre->cadre).set_contour_action(lcadre->cadre,GFX_SET);
   scanline=(c_ScanLine *)NEW(c_ScanLine)(lcadre->cadre,0*COORD_SCALEVALUE,
					  SCANLINE_ORDONNEE);
   Xc_TRACE(("scanline: %p", scanline));
   if (!scanline)
    return;
   cs=cos(SCALE_TO_RADIANS(lcadre->cadre->sAngle));
   ss=sin(SCALE_TO_RADIANS(lcadre->cadre->sAngle));
   cx=lcadre->cadre->sOrig_X;
   cy=lcadre->cadre->sOrig_Y;
   px=lcadre->cadre->saOrig_X;
   py=lcadre->cadre->saOrig_Y;
	
   y=0*COORD_SCALEVALUE;
   while((nb_segment=F(scanline).
	  get_next_scanline(scanline,&segment,15*(COORD_SCALEVALUE>>1)))!=-1)
   {
    Xc_TRACE(("nb_seg %d",nb_segment));
    for(i=0;i<nb_segment;i++)
    {
     if (segment[i].x_max>segment[i].x_min)
     {
      dx= -segment[i].x_min;
      dy= -y;
      x_segment[0].x1=x_segment[3].x2=
       (cx+px-(coord_t)(dx*cs+dy*ss))/cpage->sScale;
      x_segment[0].y1=x_segment[3].y2=
       (cy+py-(coord_t)(dy*cs-dx*ss))/cpage->sScale;
		    
      dx= -segment[i].x_max;
      dy= -y;
      x_segment[0].x2=x_segment[1].x1=
       (cx+px-(coord_t)(dx*cs+dy*ss))/cpage->sScale;
      x_segment[0].y2=x_segment[1].y1=
       (cy+py-(coord_t)(dy*cs-dx*ss))/cpage->sScale;
		    
      dx= -segment[i].x_max;
      dy= -(y+15*(COORD_SCALEVALUE>>1));
      x_segment[1].x2=x_segment[2].x1=
       (cx+px-(coord_t)(dx*cs+dy*ss))/cpage->sScale;
      x_segment[1].y2=x_segment[2].y1=
       (cy+py-(coord_t)(dy*cs-dx*ss))/cpage->sScale;
		    
      dx= -segment[i].x_min;
      dy= -(y+15*(COORD_SCALEVALUE>>1));
      x_segment[2].x2=x_segment[3].x1=
       (cx+px-(coord_t)(dx*cs+dy*ss))/cpage->sScale;
      x_segment[2].y2=x_segment[3].y1=
       (cy+py-(coord_t)(dy*cs-dx*ss))/cpage->sScale;
		    
      XDrawSegments(lcadre->cadre->X_info.display,
		    lcadre->cadre->X_info.window,
		    lcadre->cadre->X_info.gc_contour,
		    x_segment,4);
		    
     }
    }
    y+=15*(COORD_SCALEVALUE>>1);
    Xc_free(segment);
   }
   DELETE(c_ScanLine)(scanline);
   F(lcadre->cadre).set_contour_action(lcadre->cadre,GFX_REVERSE);
  }
  Xc_TRACE(("VECTOR function %d not done....",cpage->function_type));
  break;
#endif

 }
  
 if (d(function))
 {
  INIT(DATA_ITSELF(Dispatch_To_Object));
 }
 else
 {
  tmp_function = d(old_function);
  FREE_DATA;
  F(cpage).set_function(cpage,tmp_function);
 }
}

status_t eventlook_Dispatch_To_Object(cpage,event)
c_Page *cpage;
XEvent	event;
{ 
 status_t	retour_func;
 DATA(Dispatch_To_Object);
  
 GET_DATA(Dispatch_To_Object);
 retour_func = EVENTLOOK(DATA_ITSELF(Dispatch_To_Object),event);
 if (retour_func == FUNCTION_FINISHED)
 {
  FREE_DATA;
 }
 return retour_func;
}

void abort_Dispatch_To_Object(cpage)
c_Page *cpage;
{ 
 DATA(Dispatch_To_Object);
  
 GET_DATA(Dispatch_To_Object);
 ABORT(DATA_ITSELF(Dispatch_To_Object));
 FREE_DATA;
}

void redraw_Dispatch_To_Object(cpage)
c_Page *cpage;
{ 
 DATA(Dispatch_To_Object);
  
 GET_DATA(Dispatch_To_Object);
 REDRAW(DATA_ITSELF(Dispatch_To_Object));
}
