/*
** Dispatch_To_Object.c for XAllWrite in FrameManager/
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
** Started on  Mon Jul 17 20:24:46 1995 Emmanuel Paris
** Last update Thu Jun 25 18:37:49 1998 Emmanuel Paris
*/

#include "Dispatch_To_Object.h"
#include "Image.h"
#include "VectorGraph.h"
#include "Cursor.h"

boolean SetFunctionObject();

void init_Dispatch_To_Object();
status_t eventlook_Dispatch_To_Object();
void abort_Dispatch_To_Object();
void redraw_Dispatch_To_Object();

boolean SetFunctionObject(cfrmg, num_function)
c_FrMg	*cfrmg;
int	num_function;
{
 void	*object = NULL;
 boolean ret_code = FALSE;
 DATA(Dispatch_To_Object);

 SET_DATA(Dispatch_To_Object);
 d(cursor) = cfrmg->cursor;
 if (cfrmg->SelectedCadre)
 {
  d(cadre) = cfrmg->SelectedCadre->cadre;
  object = d(object) = cfrmg->SelectedCadre->cadre->object;
 }

 d(function) = NULL;
 switch(num_function)
 {
 case MOVE_IMAGE:
  d(function) = &fc_Move_Image;
  d(function_type) = cfrmg->function_type;
  break;
 case ROTATE_IMAGE:
 case CENTER_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Recenter_Image((c_Image *)object,
				      DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case INIT_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Reinit_Zoom_Image((c_Image *)object,
					 DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case RESIZE_IMAGE:
 case SCALE_IMAGE:
 case MAPPING1_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).
   Mapping1_Mode((c_Image *)object,
		 DATA_ITSELF(Dispatch_To_Object));     
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case MAPPING2_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).
   Mapping2_Mode((c_Image *)object,
		 DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case NORMAL_ASPECT_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Zoom_Normal_Mode((c_Image *)object,
					DATA_ITSELF(Dispatch_To_Object)); 
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case BEST_ASPECT_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Zoom_Best_Aspect_Mode((c_Image *)object,
					     DATA_ITSELF(Dispatch_To_Object)); 
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case MAX_ASPECT_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Zoom_Max_Aspect_Mode((c_Image *)object,
					    DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case AUTO_RESIZE_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Zoom_Auto_Resize_Mode((c_Image *)object,
					     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case FLIP_HORIZONTAL_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Flip_Horizontal_Image((c_Image *)object,
					     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case FLIP_VERTICAL_IMAGE:
  SET_WAIT_MODE_STATIC;
  F((c_Image *)object).Flip_Vertical_Image((c_Image *)object,
					   DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

 case RESIZE_VECTOR:
 case SCALE_VECTOR:
 case ROTATE_VECTOR:
  Xc_TRACE(("VECTOR function %d not done....",cfrmg->function_type));
  break;

  /*--- Moving vector ---*/
  case MOVE_VECTOR:
  d(function) = &fc_Move_Vector;
  d(function_type) = cfrmg->function_type;
  break;
  
  /*--- Centering vector ---*/
 case CENTER_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).center((c_VectorGraph *)object,
				    DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

  /*--- Normal mode for vector ---*/
 case INIT_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).setMode((c_VectorGraph *)object,
				     XcVG_NORMAL,
				     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

  /*--- Normal aspect for vector ---*/
 case NORMAL_ASPECT_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).setMode((c_VectorGraph *)object,
				     XcVG_NONE,
				     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

  /*--- Best aspect for vector ---*/
 case BEST_ASPECT_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).setMode((c_VectorGraph *)object,
				     XcVG_BEST_ASPECT,
				     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

  /*--- Auto resize for vector ---*/
 case AUTO_RESIZE_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).setMode((c_VectorGraph *)object,
				     XcVG_AUTOSIZE,
				     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

  /*--- Max aspect for vector ---*/
 case MAX_ASPECT_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).setMode((c_VectorGraph *)object,
				     XcVG_MAX_ASPECT,
				     DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

  /*--- Horizontal flipping for vector ---*/
 case FLIP_HORIZONTAL_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).horizontalFlip((c_VectorGraph *)object,
					    DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

  /*--- Vertical flipping for vector ---*/
 case FLIP_VERTICAL_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).verticalFlip((c_VectorGraph *)object,
					  DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

  /*--- Hide / Display Text vector ---*/
 case TOGGLE_TEXT_VECTOR:
  SET_WAIT_MODE_STATIC;
  F((c_VectorGraph *)object).textToggle((c_VectorGraph *)object,
					DATA_ITSELF(Dispatch_To_Object));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 default:
  d(function) = &fc_Invalid_OFunc;
  d(function_type) = cfrmg->function_type;
  break;
 }

 if (d(function) == NULL)
 {
  FREE_DATA;
 }

 return ret_code;
}

void init_Dispatch_To_Object(cfrmg)
c_FrMg *cfrmg;
{
 DATA(Dispatch_To_Object);
 
 if (cfrmg->function_data == NULL)
 {
  SetFunctionObject(cfrmg, cfrmg->function_type);
 }
 else
 {
  GET_DATA(Dispatch_To_Object);
 }
 
 INIT(DATA_ITSELF(Dispatch_To_Object));
}

status_t eventlook_Dispatch_To_Object(cfrmg,event)
c_FrMg *cfrmg;
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

void abort_Dispatch_To_Object(cfrmg)
c_FrMg *cfrmg;
{ 
 DATA(Dispatch_To_Object);
  
 GET_DATA(Dispatch_To_Object);
 ABORT(DATA_ITSELF(Dispatch_To_Object));
 FREE_DATA;
}

void redraw_Dispatch_To_Object(cfrmg)
c_FrMg *cfrmg;
{ 
 DATA(Dispatch_To_Object);
  
 GET_DATA(Dispatch_To_Object);
 REDRAW(DATA_ITSELF(Dispatch_To_Object));
}

/* ----------------------------------------------------------------- ** 
** Invalid Object Function                                           ** 
** ----------------------------------------------------------------- */

void init_Invalid_OFunc();

status_t eventlook_Invalid_OFunc();
void abort_Invalid_OFunc();
void redraw_Invalid_OFunc();

void init_Invalid_OFunc(cobject)
d_object_function	*cobject;
{
 F(_cursor).push_and_set_cursor(_cursor, C_LOCK_CADRE);
}

status_t eventlook_Invalid_OFunc(cobject, event)
d_object_function	*cobject;
XEvent			event;
{
 if (event.type == ButtonPress && event.xbutton.button != 1)
  return FUNCTION_REINIT;
 
 return FUNCTION_NOT_FINISHED;
}

void abort_Invalid_OFunc(cobject)
d_object_function	*cobject;
{
 F(_cursor).pop_cursor(_cursor);
}

void redraw_Invalid_OFunc(cobject)
d_object_function	*cobject;
{
}
