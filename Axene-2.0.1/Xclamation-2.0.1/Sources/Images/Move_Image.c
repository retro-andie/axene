/*
** Move_Image.c for Xclamation and XAllWrite in Images/
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
** Started on  Mon Jul 17 18:14:57 1995 Emmanuel Paris
** Last update Tue Apr 28 18:29:02 1998 Emmanuel Paris
*/

#include "Move_Image.h"

#undef _display
#define _display _cadre->X_info.display
#undef _window
#define _window _cadre->X_info.window

static void Move_Image_done();

void init_Move_Image(cobject)
d_object_function	*cobject;
{
 XGCValues	values;
 DATA(Move_Image);
  
 SET_DATA(Move_Image);

 F(_cursor).push_and_set_cursor(_cursor, C_HAND_OPEN);
 d(source) = FALSE;
 d(xo) = _image->Dx_orig;
 d(yo) = _image->Dy_orig;
 d(sxo) = _image->x_orig;
 d(syo) = _image->y_orig;
 d(polygone) = Fo(get_clip_polygone, cobject);
 d(clip) = CalculPolygoneMask(d(polygone), _cadre->X_info);
 values.clip_x_origin = d(polygone)->Orig_X;
 values.clip_y_origin = d(polygone)->Orig_Y;
 values.clip_mask = d(clip);
 if (_image->type == BITMAP)
 {
  values.foreground = BlackPixel(_cadre->X_info.display,
				 _cadre->X_info.ecran);
  values.background = WhitePixel(_cadre->X_info.display,
				 _cadre->X_info.ecran);
  XChangeGC(_cadre->X_info.display, _cadre->X_info.gc_inside, GCClipMask|
	    GCClipXOrigin|GCClipYOrigin|GCForeground|GCBackground,
	    &values);
  values.function = GXcopy;
  values.foreground = _cadre->X_info.inside_color;
  d(gc) = XCreateGC(_cadre->X_info.display, _cadre->X_info.window,
		    GCClipMask|GCClipXOrigin|GCClipYOrigin|
		    GCForeground|GCFunction,
		    &values);
 }
 else
 {
  XChangeGC(_cadre->X_info.display, _cadre->X_info.gc_inside, GCClipMask|
	    GCClipXOrigin|GCClipYOrigin, &values);
  d(gc) = _cadre->X_info.gc_inside;
 }
}

status_t eventlook_Move_Image(cobject, event)
d_object_function	*cobject;
XEvent			event;
{ 
 char			tamp_cla[20];
 int			tailtamp = 20;
 KeySym		symtouche;
 int			nb_car, i;
 XComposeStatus	seqtouche;
 status_t		retour_func;
 int			xm, ym;
 int			px, py;
 int			Width, Height, X, Y;
 XEvent		peek_event;
 XRectangle		rect[2];
 DATA(Move_Image);

 GET_DATA(Move_Image);

 retour_func=FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case KeyPress:
  nb_car = XLookupString((XKeyEvent *)&event, tamp_cla, tailtamp,
			 &symtouche, &seqtouche);
  if (symtouche == XK_Shift_R || symtouche == XK_Shift_L)
  {
   d(shift) = TRUE;
  }
  break;
 case KeyRelease:
  nb_car = XLookupString((XKeyEvent *)&event, tamp_cla, tailtamp,
			 &symtouche, &seqtouche);
  if (symtouche == XK_Shift_R || symtouche == XK_Shift_L)
  {
   d(shift) = FALSE;
  }
  break;
 case MotionNotify:
  if (d(source))
  {
   if ((i = XEventsQueued(_display, QueuedAlready))>0)
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
   px = _cadre->Forme->Orig_X + _cadre->Forme->x_min + d(xo)-d(x1)-1;
   py = _cadre->Forme->Orig_Y + _cadre->Forme->y_min + d(yo)-d(y1)-1;
   if (d(x2) <= xm)
    X = d(x2);
   else
    X = xm + _image->Dwidth + 1;
   if (d(y2) <= ym)
    Y = d(y2);
   else
    Y = ym + _image->Dheight + 1;
   Width = ABS(d(x2)-xm);
   Height = ABS(d(y2)-ym);
   if (_image->Dwidth <= Width || _image->Dheight <= Height)
   {
    XFillRectangle(_cadre->X_info.display, _cadre->X_info.window,
		   d(gc), d(x2) + px, d(y2) + py,
		   _image->Dwidth + 1, _image->Dheight + 1);
   }
   else
   {
    rect[0].x = d(x2) + px;
    rect[0].y = Y + py;
    rect[0].width = _image->Dwidth + 1;
    rect[0].height = Height;
    rect[1].x = X + px;
    if (Y == d(y2))
     rect[1].y = d(y2) + Height + py;
    else
     rect[1].y = d(y2) + py;
    rect[1].width = Width;
    rect[1].height = _image->Dheight - Height + 1;
    XFillRectangles(_cadre->X_info.display, _cadre->X_info.window,
		    d(gc), rect, 2);
   }
	  
   d(x2) = xm;
   d(y2) = ym;
   d(sx2) = d(x2)*_cadre->sScale;
   d(sy2) = d(y2)*_cadre->sScale;
   _image->Dx_orig = d(x2) + d(xo)-d(x1);
   _image->Dy_orig = d(y2) + d(yo)-d(y1);
   DISPLAY_REDRAW(_image)(_image, _cadre, d(polygone));
   XFlush(_display);
   /*      F(cpage).attract_point(cpage, &d(sx2), &d(sy2), &d(x2), &d(y2)); */
	  
  }
  break;
 case ButtonRelease:
  if (d(source) && event.xbutton.time>d(time) + 600)
  {
   Move_Image_done(cobject);
   retour_func = FUNCTION_FINISHED;
  }
  break;
 case ButtonPress:
  if (event.xbutton.button == 1)
  {
   if (d(source))
   {
    Move_Image_done(cobject);
    retour_func = FUNCTION_FINISHED;
   }
   else
   {
    F(_cursor).set_cursor(_cursor, C_HAND_CLOSE);
    d(time) = event.xbutton.time;
    d(x1) = d(x2) = event.xbutton.x;
    d(y1) = d(y2) = event.xbutton.y;
    /*	    d(sx1) = d(sx2) = d(x1)*cpage->sScale;
	    d(sy1) = d(sy2) = d(y1)*cpage->sScale;
	    F(cpage).attract_point(cpage, &d(sx1), &d(sy1), &d(x1), &d(y1)); */
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

void abort_Move_Image(cobject)
d_object_function	*cobject;
{ 
 DATA(Move_Image);
  
 GET_DATA(Move_Image);
  
 if (d(source))
 {
  _image->Dx_orig = d(xo);
  _image->Dy_orig = d(yo);
  _image->x_orig = d(sxo);
  _image->y_orig = d(syo);
  Fo(redraw_cadre, cobject);  
  XFlush(_display);
 }
 if (_image->type == BITMAP)
  XFreeGC(_cadre->X_info.display, d(gc));
 XSetClipMask(_cadre->X_info.display, _cadre->X_info.gc_inside, None);
 XFreePixmap(_cadre->X_info.display, d(clip));
 FreePolygone(d(polygone), DEL_POLYGONE);
 FREE_DATA;
  
 F(_cursor).pop_cursor(_cursor);
}

void redraw_Move_Image(cobject)
d_object_function	*cobject;
{ 
 DATA(Move_Image);
  
 GET_DATA(Move_Image);
}

static void Move_Image_done(cobject)
d_object_function	*cobject;
{ 
 coord_t	orig_x, orig_y;
 double	cs, ss;
 angle_t	angle;
 DATA(Move_Image);
  
 GET_DATA(Move_Image);

 angle = _image->Dangle;
 if (angle)
 {
  cs = cos( SCALE_TO_RADIANS(-angle));
  ss = sin( SCALE_TO_RADIANS(-angle));
      
  orig_x = (_image->Dx_orig + _image->Dx_offset) * _cadre->sScale 
   - _cadre->saOrig_X;
  orig_y =(_image->Dy_orig + _image->Dy_offset) * _cadre->sScale
   - _cadre->saOrig_Y;
      
  _image->x_orig = (coord_t)( cs * orig_x + ss * orig_y);
  _image->y_orig = (coord_t)( cs * orig_y - ss * orig_x);
 }
 else
 {
  _image->x_orig = _image->Dx_orig*_cadre->sScale;
  _image->y_orig = _image->Dy_orig*_cadre->sScale;
 }
  
 _image->zoom_type = ZOOM_NORMAL_MODE;
 Fo(redraw_cadre, cobject);  
  
 if (_image->type == BITMAP)
  XFreeGC(_cadre->X_info.display, d(gc));
 XSetClipMask(_cadre->X_info.display, _cadre->X_info.gc_inside, None);
 XFreePixmap(_cadre->X_info.display, d(clip));
 FreePolygone(d(polygone), DEL_POLYGONE);
 F(_cursor).pop_cursor(_cursor);
  
 FREE_DATA;
}
