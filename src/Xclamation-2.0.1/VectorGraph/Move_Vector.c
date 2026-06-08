/*
** Move_Vector.c for Xclamation and XAllWrite in VectorGraph/
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
** Started on  Sun Jun  8 17:38:14 1997 Emmanuel Paris
** Last update Tue Apr 28 18:29:23 1998 Emmanuel Paris
*/

#include "Move_Vector.h"

#undef _display
#define _display _cadre->X_info.display
#undef _window
#define _window _cadre->X_info.window

static void Move_Vector_done();

void init_Move_Vector(cobject)
d_object_function	*cobject;
{
 DATA(Move_Vector);
  
 SET_DATA(Move_Vector);

 F(_cursor).push_and_set_cursor(_cursor, C_HAND_OPEN);
 d(source) = FALSE;

 d(sxo) = _vector->offset.dx;
 d(syo) = _vector->offset.dy;
 d(mode) = _vector->mode;
 _vector->realtime_draw = TRUE;
 
 d(polygone) = Fo(get_clip_polygone, cobject);
 d(clip) = CalculPolygoneMask(d(polygone), _cadre->X_info);
}

status_t eventlook_Move_Vector(cobject, event)
d_object_function	*cobject;
XEvent			event;
{ 
 int			i;
 status_t		retour_func;
 int			xm, ym;
 angle_t		angle;
 double			ss, cs;
 coord_t	        dx, dy;
 XEvent			peek_event;
 XGCValues		values;
 DATA(Move_Vector);

 GET_DATA(Move_Vector);

 retour_func = FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case MotionNotify:
  if (d(source))
  {
   if ((i = XEventsQueued(_display, QueuedAlready)) > 0)
   {
    XPeekEvent(_display, &peek_event);
    while(peek_event.type == MotionNotify && i)
    {
     XNextEvent(_display, &event);
     XPeekEvent(_display, &peek_event);
     i--;
    }
   }
   xm = event.xmotion.x;
   ym = event.xmotion.y;
   
   DrawPolygone(d(polygone), _cadre->X_info);
   _cadre->clip = d(clip);

   d(x2) = xm;
   d(y2) = ym;

   angle = _cadre->sAngle;
   cs = cos(SCALE_TO_RADIANS(angle));
   ss = sin(SCALE_TO_RADIANS(angle));
   
   dx = (d(x2)- d(x1)) * _cadre->sScale;
   dy = (d(y2)- d(y1)) * _cadre->sScale;
   
   _vector->offset.dx = d(sxo) + (coord_t)(dx * cs - dy * ss);
   _vector->offset.dy = d(syo) + (coord_t)(dy * cs + dx * ss);
   _vector->update_flag = TRUE;
   DISPLAY_INIT(_vector)(_vector, _cadre);
   
   values.clip_x_origin = d(polygone)->Orig_X;
   values.clip_y_origin = d(polygone)->Orig_Y;
   values.clip_mask = d(clip);
   /* pour nuc */
   values.foreground = GlobColormap->Cblack;
   values.background = GlobColormap->Cwhite;
   XChangeGC(_cadre->X_info.display, _cadre->X_info.gc_inside, GCClipMask|
	     GCClipXOrigin | GCClipYOrigin | GCForeground | GCBackground,
	     &values);
   
   DISPLAY_REDRAW(_vector)(_vector, _cadre, d(polygone));

   values.clip_mask = (Pixmap)NULL;
   values.foreground = _cadre->X_info.inside_color;
   values.line_width = 0;
   values.line_style = LineSolid;
   values.cap_style = CapButt;
   values.join_style = JoinMiter;
   values.function = GXcopy;
   XChangeGC(_cadre->X_info.display, _cadre->X_info.gc_inside,
	     GCClipMask | GCForeground | GCLineWidth | GCLineStyle |
	     GCCapStyle | GCJoinStyle| GCFunction, &values);

   XFlush(_display);	  
  }
  break;
 case ButtonRelease:
  if (d(source) && event.xbutton.time>d(time) + 600)
  {
   Move_Vector_done(cobject);
   retour_func = FUNCTION_FINISHED;
  }
  break;
 case ButtonPress:
  if (event.xbutton.button == 1)
  {
   if (d(source))
   {
    Move_Vector_done(cobject);
    retour_func = FUNCTION_FINISHED;
   }
   else
   {
    F(_cursor).set_cursor(_cursor, C_HAND_CLOSE);
    d(time) = event.xbutton.time;
    d(x1) = d(x2) = event.xbutton.x;
    d(y1) = d(y2) = event.xbutton.y;
    _vector->mode = XcVG_NORMAL;
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

void abort_Move_Vector(cobject)
d_object_function	*cobject;
{ 
 DATA(Move_Vector);
  
 GET_DATA(Move_Vector);
  
 XFreePixmap(_cadre->X_info.display, d(clip));
 FreePolygone(d(polygone), DEL_POLYGONE);

 _vector->realtime_draw = FALSE;
 if (d(source))
 {
  _vector->mode = d(mode);
  _vector->offset.dx = d(sxo);
  _vector->offset.dy = d(syo);
  _vector->update_flag = TRUE;
  DISPLAY_INIT(_vector)(_vector, _cadre);

  Fo(redraw_cadre, cobject);  
  XFlush(_display);
 }
 
 FREE_DATA;
  
 F(_cursor).pop_cursor(_cursor);
}

void redraw_Move_Vector(cobject)
d_object_function	*cobject;
{ 
 DATA(Move_Vector);
  
 GET_DATA(Move_Vector);
}

static void Move_Vector_done(cobject)
d_object_function	*cobject;
{ 
 DATA(Move_Vector);
  
 GET_DATA(Move_Vector);
 
 _vector->realtime_draw = FALSE;
 XFreePixmap(_cadre->X_info.display, d(clip));
 FreePolygone(d(polygone), DEL_POLYGONE);
 
 Fo(redraw_cadre, cobject);  
 F(_cursor).pop_cursor(_cursor);
  
 FREE_DATA;
}
