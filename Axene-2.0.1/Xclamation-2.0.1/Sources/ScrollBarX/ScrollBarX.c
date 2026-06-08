/*
** ScrollBarX.c for Xclamation in ScrollBarX/
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
** Started on  Tue Jul 18 12:22:12 1995 Antoine Buat
** Last update Mon Dec 29 19:31:31 1997 Emmanuel Paris
*/

#include "ScrollBarX.h"
#include "Colormap.h"

extern c_Colormap *GlobColormap;
extern void XcHandleEvent ___PROTO((XEvent *event));

#define ARROWU_POSY	This->Bsize
#define ARROWD_POSY	(This->Size-SCROLLB_SIZE)

#define ARROWL_POSX	This->Bsize
#define ARROWR_POSX	(This->Size-SCROLLB_SIZE)

#define CENTRAL_POS	1
#define CENTRAL_SIZE	(This->Bsize-2)

/* repeat time config */
#define LOOPTIME 10
#define WTIME 30000
#define WTIME2 5000

static void *cons_ScrollBarX();
static void dest_ScrollBarX();
static void *copy_ScrollBarX();
static int move_ScrollBarX();
static int amove_ScrollBarX();
static void request_move_ScrollBarX();
static void resize_ScrollBarX();
static void draw_ScrollBarX();
static boolean eventlook_ScrollBarX();
static void change_Rsize();
static void set_callback();
static void unset_callback();

static void draw_left_arrow();
static void draw_right_arrow();
static void draw_up_arrow();
static void draw_down_arrow();
static void draw_h_central_part();
static void draw_h_central_button();
static void draw_v_central_part();
static void draw_v_central_button();
static int event_on_button();

sf_ScrollBarX fc_ScrollBarX =
{
 cons_ScrollBarX,
 dest_ScrollBarX,
 copy_ScrollBarX,
 move_ScrollBarX,
 amove_ScrollBarX,
 request_move_ScrollBarX,
 resize_ScrollBarX,
 draw_ScrollBarX,
 eventlook_ScrollBarX,
 change_Rsize,
 set_callback,
 unset_callback
};

static void *cons_ScrollBarX(display, window, PosX, PosY, 
			     Size, VSize, Orient, Hole)
Display	*display;
Window	window;
int	PosX;
int	PosY;
int	Size;
int	VSize;
int	Orient;
int	Hole;
{
 c_ScrollBarX 	*This;
 color_cells_t	realcolor;
 XGCValues 	xgcv;
 long		normalc;
  
 if ((This = (c_ScrollBarX *)Xc_malloc("ScrollBarX",
				       sizeof(c_ScrollBarX))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_ScrollBarX;
 This->display = display;
 This->Orient = Orient;
 This->Vsize = VSize;
 This->Bsize = Size - (Hole + 2) * SCROLLB_SIZE;
 This->Rsize = 0;
 This->Csize = CENTRAL_SIZE;
 This->CPos = CENTRAL_POS;
 This->Pos = 0;
 This->PosX = PosX - 2*SCROLLBARX_BORDER;
 This->PosY = PosY - 2*SCROLLBARX_BORDER;
 This->Size = Size;
 This->Hole = Hole;
 unset_callback(This);
  
 /*******************/
 /* Creation of GCs */
 /*******************/
 xgcv.line_width = 0;

 F(GlobColormap).allocate_named_color(GlobColormap, SCROLLBARX_NORM,
				      &realcolor);
 xgcv.foreground = normalc = realcolor.pixel;
 This->gc_normal = XCreateGC(display, window, 
			     GCLineWidth | GCForeground, &xgcv);
  
 F(GlobColormap).allocate_named_color(GlobColormap, SCROLLBARX_SHAD,
				      &realcolor);
 xgcv.foreground = realcolor.pixel;
 This->gc_shadow = XCreateGC(display, window,
			     GCLineWidth | GCForeground, &xgcv);
  
 F(GlobColormap).allocate_named_color(GlobColormap, SCROLLBARX_HIGH,
				      &realcolor);
 xgcv.foreground = realcolor.pixel;
 This->gc_highlight = XCreateGC(display, window,
				GCLineWidth | GCForeground, &xgcv);
  
 F(GlobColormap).allocate_named_color(GlobColormap, SCROLLBARX_SLIDER,
				      &realcolor);
 xgcv.foreground = realcolor.pixel;
 This->gc_slider = XCreateGC(display, window,
			     GCLineWidth | GCForeground, &xgcv);
  
 /*******************/
 /* Window creation */
 /*******************/
 if (Orient == SBHORIZONTAL) 
 {
  This->window = 
   XCreateSimpleWindow(display, window, 
		       PosX,  PosY,
		       Size, SCROLLB_SIZE,
		       SCROLLBARX_BORDER,
		       BlackPixel(display,DefaultScreen(display)),
		       normalc);
 }
 else
 {
  This->window = 
   XCreateSimpleWindow(display, window, 
		       PosX,  PosY,
		       SCROLLB_SIZE, Size,
		       SCROLLBARX_BORDER,
		       BlackPixel(display,DefaultScreen(display)),
		       normalc);
 }
 XSelectInput(display, This->window, ButtonPressMask | ExposureMask | 
	      ButtonMotionMask | ButtonReleaseMask);
 XMapWindow(display, This->window);
 Xc_TRACE(("Object ScrollBarX build"));
 return This;
}

static void dest_ScrollBarX(This, display)
c_ScrollBarX 	*This;
Display		*display;
{
 XFreeGC(This->display, This->gc_shadow);
 XFreeGC(This->display, This->gc_highlight);
 XFreeGC(This->display, This->gc_normal);
 XFreeGC(This->display, This->gc_slider);
 Xc_free(This);
 Xc_TRACE(("Object ScrollBarX destroyed"));
}

static void *copy_ScrollBarX(This)
c_ScrollBarX *This;
{
 c_ScrollBarX *ObjTmp;

 if ((ObjTmp =
      (c_ScrollBarX *)Xc_malloc("CScrollBarX", sizeof(c_ScrollBarX))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_ScrollBarX));
 Xc_TRACE(("Object ScrollBarX copied"));
 return ObjTmp;
}

static void request_move_ScrollBarX(This1, direction1, This2, direction2,x,y,d)
c_ScrollBarX	*This1;
boolean		direction1;
c_ScrollBarX	*This2;
boolean		direction2;
int		x;
int		y;
int		d;
{
 int	step1;
 int	step2;
  
 if (d==0) 
  d = SCRX_ARROW_STEP;
 step1 = F(This1).Move(This1, (direction1?d:-d));
 if (This2)
  step2 = F(This2).Move(This2,(direction2?d:-d));
 if (!This2 || !step2)
 {
  if (step1 && This1->Callback)
  {
   if (This1->Orient == SBVERTICAL)
    This1->Callback(This1->Callback_data, x, y+step1);
   else
    This1->Callback(This1->Callback_data, x+step1, y);
  }
 }
 else
 {
  if (!step1 && This2->Callback)
  {
   if (This2->Orient == SBVERTICAL)
    This2->Callback(This2->Callback_data, x, y+step2);
   else
    This2->Callback(This2->Callback_data, x+step2, y);
  }
  else
  {
   if (This1->Orient == SBVERTICAL)
    This1->Callback(This2->Callback_data, x+step2, y+step1);
   else
    This1->Callback(This2->Callback_data, x+step1, y+step1);
  }
 }
}

static int amove_ScrollBarX(This, Pos)
c_ScrollBarX	*This;
int		Pos;
{
 This->Pos = 0;
 return F(This).Move(This, Pos);
}

static int move_ScrollBarX(This, Step)
c_ScrollBarX	*This;
int		Step;
{
 int		OldCPos;

 if (Step == 0) return 0;
  
 if (This->Pos + Step < 0)
  Step = 0-This->Pos;

 if (This->Pos+This->Vsize+Step>This->Rsize)
  Step = This->Rsize-This->Vsize-This->Pos;
 
 This->Pos += Step;
 OldCPos = This->CPos;
 This->CPos = This->Bsize * This->Pos / This->Rsize;
 if (This->CPos != OldCPos)
 {
  if (This->Orient==SBVERTICAL)
  {
   if (OldCPos < This->CPos)
    XClearArea(This->display, This->window, 
	       1, OldCPos, SCROLLB_SIZE-2, This->CPos-OldCPos+1,
	       False);
   else
    XClearArea(This->display, This->window, 
	       1, This->CPos+This->Csize,
	       SCROLLB_SIZE-2, OldCPos-This->CPos,
	       False);
       
   draw_v_central_button(This);
  }
  else
  {
   if (OldCPos < This->CPos)
    XClearArea(This->display, This->window, 
	       OldCPos, 1, This->CPos-OldCPos+1, SCROLLB_SIZE-2,
	       False);
   else
    XClearArea(This->display, This->window, 
	       This->CPos + This->Csize, 1, OldCPos-This->CPos,
	       SCROLLB_SIZE-2, False);

   draw_h_central_button(This);
  }
 }
 return Step;
}

static void resize_ScrollBarX(This, PosX, PosY, Vsize, Bsize, Hole,
			      orig_x, orig_y, actif)
c_ScrollBarX	*This;
int		PosX;
int		PosY;
int		Vsize;
int		Bsize;
int 		Hole;
int		orig_x;
int		orig_y;
boolean		actif;
{
 int	x,y;
  
 This->PosX = PosX;
 This->PosY = PosY;
 This->Size = Bsize;
 This->Bsize = Bsize - (Hole + 2) * SCROLLB_SIZE;
 This->Vsize = Vsize;
 This->Hole = Hole;
 This->Csize = This->Bsize * Vsize / This->Rsize;
 This->CPos = This->Bsize * This->Pos / This->Rsize;
 x = orig_x;
 y = orig_y;
 if (This->Orient == SBHORIZONTAL)
 {
  XMoveResizeWindow(This->display, This->window,
		    PosX, PosY, Bsize, SCROLLB_SIZE);
  if (actif)
  {
   x += F(This).Move(This, 0);
   if (x!=orig_x)
    if (This->Callback)
     This->Callback(This->Callback_data, x, y);
  }
 }
 else
 {
  XMoveResizeWindow(This->display, This->window,
		    PosX, PosY, SCROLLB_SIZE, Bsize);
  if (actif)
  {
   y += F(This).Move(This, 0);
   if (y!=orig_y)
    if (This->Callback)
     This->Callback(This->Callback_data, x, y);
  }
 }
}


static void draw_ScrollBarX(This, x, y, width, height)
c_ScrollBarX	*This;
int		x;
int		y;
int		width;
int		height;
{
 if (This->Orient==SBVERTICAL)
 {
  draw_up_arrow(This, FALSE);
  draw_down_arrow(This, FALSE);
  draw_v_central_part(This);
  draw_v_central_button(This);
 }
 else
 {
  draw_left_arrow(This, FALSE);
  draw_right_arrow(This, FALSE);
  draw_h_central_part(This);
  draw_h_central_button(This);
 }
}

#if 0
static void MoveCursor_ScrollBarX(This, PosX, PosY, Size)
c_ScrollBarX	*This;
int		PosX;
int		PosY;
int		Size;
{
}
#endif

/*---Event---*/

static boolean eventlook_ScrollBarX(This, display, Event, window, x, y)
c_ScrollBarX	*This;
Display		*display;
XEvent		*Event;
Window		window;
int		x;
int		y;
{
 int		sortie;
 int		Tmove;
 int		Tpos;
 int		posini;

 if (This->Orient == SBVERTICAL && 
     (Event->type == ButtonPress || Event->type == ButtonRelease) &&
     (Event->xbutton.button == 4 || Event->xbutton.button == 5))
 {
  if (Event->type == ButtonRelease) return TRUE;

  if (Event->xany.window == window)
  {
   XEvent eventl;
   
   memcpy(&eventl, Event, sizeof(XEvent));
   eventl.type = LeaveNotify;

   XcHandleEvent(&eventl);
  }
  else if (Event->xany.window != This->window) return TRUE;
  
  if (Event->xbutton.button == 4)
   y += F(This).Move(This, -SCRX_ARROW_STEP);
  else /* Event->xbutton.button == 5 */
   y += F(This).Move(This, SCRX_ARROW_STEP);

  if (This->Callback)
   This->Callback(This->Callback_data, x, y);
  
  if (Event->xany.window == window)
  {
   XEvent eventm;
   
   memcpy(&eventm, Event, sizeof(XEvent));
   eventm.type = MotionNotify;
   
   XSendEvent(display, window, True, 0, &eventm);
  }

  return TRUE;
 }

 if (Event->xany.window == This->window)
 {
  if (Event->type == Expose)
  {
   F(This).Draw(This, Event->xexpose.x, Event->xexpose.y,
		Event->xexpose.width, Event->xexpose.height);
  }
  if (Event->type == ButtonPress)
  {
   if (window)
   {
    if (This->Orient == SBVERTICAL)
    {
     /******************************/
     /* scroll bouton vers le haut */
     /******************************/
     if (Event->xbutton.y > This->Bsize &&
	 Event->xbutton.y < This->Bsize + SCROLLB_SIZE)
     {
      draw_up_arrow(This, TRUE);
      y += event_on_button(This, -SCRX_ARROW_STEP);
      draw_up_arrow(This, FALSE);
     }
     /*****************************/
     /* scroll bouton vers le bas */
     /*****************************/
     if (Event->xbutton.y >= This->Size-SCROLLB_SIZE)
     {
      draw_down_arrow(This, TRUE);
      y += event_on_button(This, SCRX_ARROW_STEP);
      draw_down_arrow(This, FALSE);
     }
     /*******************/
     /*  bouton central */
     /*******************/
     if ((Event->xbutton.y>This->CPos+CENTRAL_POS) &&
	 (Event->xbutton.y<This->CPos+This->Csize+CENTRAL_POS))
     {
      Tmove = 0;
      sortie = 0;
      posini = Event->xbutton.y;
      while (!sortie)
      {
       XNextEvent(display, Event);
       switch (Event->type)
       {
       case ButtonRelease:
	sortie = 1;
	break;
       case MotionNotify:
	if (Event->xmotion.y < 0)
	 Tpos = 0; 
	else
	 if (Event->xmotion.y > This->Bsize)
	  Tpos = This->Bsize;
	 else
	  Tpos = Event->xmotion.y;
	Tmove = (Tpos - posini) * 
	 (This->Rsize / This->Bsize);
	y += F(This).Move(This, Tmove);
	posini = Tpos;
       }
      }
     }
     /*******************************/
     /* au dessus du bouton central */
     /*******************************/
     if (Event->xbutton.y<This->CPos+CENTRAL_POS) 
      y+=F(This).Move(This, -SCRX_SLIDE_STEP);
     /********************************/
     /* au dessous du bouton central */
     /********************************/
     if ((Event->xbutton.y > This->CPos+This->Csize+CENTRAL_POS)
	 && (Event->xbutton.y < This->Bsize))
      y += F(This).Move(This, SCRX_SLIDE_STEP);
    }
    else
    {
     /********************************/
     /* scroll bouton vers la gauche */
     /********************************/
     if ((Event->xbutton.x > This->Bsize) &&
	 (Event->xbutton.x < This->Bsize+SCROLLB_SIZE))
     {
      draw_left_arrow(This, TRUE);
      x += event_on_button(This, -SCRX_ARROW_STEP);
      draw_left_arrow(This, FALSE);
     }
     /********************************/
     /* scroll bouton vers la droite */
     /********************************/
     if (Event->xbutton.x >= This->Size-SCROLLB_SIZE)
     {
      draw_right_arrow(This, TRUE);
      x += event_on_button(This, SCRX_ARROW_STEP);
      draw_right_arrow(This, FALSE);
     }
     /*******************/
     /*  bouton central */
     /*******************/
     if ((Event->xbutton.x>This->CPos+CENTRAL_POS) &&
	 (Event->xbutton.x<This->CPos+This->Csize+CENTRAL_POS))
     {
      Tmove = 0;
      sortie = 0;
      posini = Event->xbutton.x;
      while (!sortie)
      {
       XNextEvent(display, Event);
       switch (Event->type)
       {
       case ButtonRelease:
	sortie = 1;
	break;
       case MotionNotify:
	if (Event->xmotion.x < 0)
	 Tpos = 0; 
	else
	 if (Event->xmotion.x > This->Bsize)
	  Tpos = This->Bsize;
	 else
	  Tpos = Event->xmotion.x;
	Tmove = (Tpos - posini) * 
	 (This->Rsize / This->Bsize);
	x += F(This).Move(This, Tmove);
	posini = Tpos;
       }
      }
     }
     /********************************/
     /* a droite du bouton central */
     /********************************/
     if (Event->xbutton.x<This->CPos+CENTRAL_POS)
      x+=F(This).Move(This, -SCRX_SLIDE_STEP);
     /********************************/
     /* a gauche du bouton central */
     /********************************/
     if ((Event->xbutton.x > This->CPos+This->Csize+CENTRAL_POS)
	 && (Event->xbutton.x < This->Bsize))
      x+=F(This).Move(This, SCRX_SLIDE_STEP);
    }
    if (This->Callback)
     This->Callback(This->Callback_data, x, y);
   }
  }
 }  
 return FALSE;
}

static void change_Rsize(This, display, NsizeX, NsizeY)
c_ScrollBarX	*This;
Display		*display;
int		NsizeX;
int		NsizeY;
{
 int old_Rsize = This->Rsize;

 if (This->Orient == SBVERTICAL)
 {
  This->Rsize = NsizeY;
  XClearArea(display, This->window,
	     1, CENTRAL_POS, SCROLLB_SIZE-2, CENTRAL_SIZE, False);
 }
 else
 {
  This->Rsize = NsizeX;
  XClearArea(display, This->window, 
	     CENTRAL_POS, 1, CENTRAL_SIZE, SCROLLB_SIZE-2, False);
 }
  
 if (old_Rsize)
  This->Pos = This->Pos * This->Rsize / old_Rsize;

 This->CPos = This->Bsize * This->Pos / This->Rsize;
 This->Csize = This->Bsize * This->Vsize / This->Rsize;
 XRaiseWindow(display, This->window);
 
 if (This->Orient == SBVERTICAL)
  draw_v_central_button(This);
 else
  draw_h_central_button(This);
}

static void set_callback(This, function, data)
c_ScrollBarX	*This;
void		(*function)();
void		*data;
{
 This->Callback=function;
 This->Callback_data=data;
}

static void unset_callback(This)
c_ScrollBarX	*This;
{
 This->Callback=(void (*)())NULL;
 This->Callback_data=NULL;
}

/*		PRIVATE METHODES		*/

/* ********************************************************************** */
/* ** Draw Functions: button, central part ans central button	       ** */
/* ********************************************************************** */

static void draw_left_arrow(This, select)
c_ScrollBarX	*This;
boolean		select;
{
 GC	gc_1;
 GC	gc_2;
 XSegment	xseg[4];
  
 if (select)
 {
  gc_1 = This->gc_shadow;
  gc_2 = This->gc_highlight;
 }
 else
 {
  gc_1 = This->gc_highlight;
  gc_2 = This->gc_shadow;
 }
  
 xseg[0].x1 = ARROWL_POSX;
 xseg[0].x2 = SCROLLB_SIZE-2+ARROWL_POSX;
 xseg[0].y1 = xseg[0].y2 = 0;
  
 xseg[1].x1 = xseg[1].x2 = ARROWL_POSX;
 xseg[1].y1 = 1;
 xseg[1].y2 = SCROLLB_SIZE-2;
  
 xseg[2].x1 = DECALAGE+ARROWL_POSX;
 xseg[2].y1 = (SCROLLB_SIZE-1)/2;
 xseg[2].x2 = SCROLLB_SIZE-1-DECALAGE+ARROWL_POSX;
 xseg[2].y2 = DECALAGE; 
  
 XDrawSegments(This->display, This->window, gc_1, xseg, 3);
  
 xseg[0].x1 = xseg[0].x2 = SCROLLB_SIZE-1+ARROWL_POSX;
 xseg[0].y1 = 0;
 xseg[0].y2 = SCROLLB_SIZE-1;
  
 xseg[1].x1 = ARROWL_POSX;
 xseg[1].x2 = SCROLLB_SIZE-2+ARROWL_POSX;
 xseg[1].y1 = xseg[1].y2 = SCROLLB_SIZE-1;

 xseg[2].x1 = DECALAGE+ARROWL_POSX;
 xseg[2].y1 = (SCROLLB_SIZE-1)/2;
 xseg[2].x2 = SCROLLB_SIZE-1-DECALAGE+ARROWL_POSX;
 xseg[2].y2 = SCROLLB_SIZE-1-DECALAGE; 
  
 xseg[3].x1 = xseg[3].x2 = SCROLLB_SIZE-1-DECALAGE+ARROWL_POSX;
 xseg[3].y1 = DECALAGE;
 xseg[3].y2 = SCROLLB_SIZE-1-DECALAGE; 
  
 XDrawSegments(This->display, This->window, gc_2, xseg, 4);
}

static void draw_right_arrow(This, select)
c_ScrollBarX	*This;
boolean		select;
{
 GC	gc_1;
 GC	gc_2;
  
 if (select)
 {
  gc_1 = This->gc_shadow;
  gc_2 = This->gc_highlight;
 }
 else
 {
  gc_1 = This->gc_highlight;
  gc_2 = This->gc_shadow;
 }

 XDrawLine(This->display, This->window, gc_1,
	   ARROWR_POSX, 0,
	   ARROWR_POSX+SCROLLB_SIZE-2, 0);
 XDrawLine(This->display, This->window, gc_1,
	   ARROWR_POSX, 1,
	   ARROWR_POSX, SCROLLB_SIZE-2);
 XDrawLine(This->display, This->window, gc_2, 
	   ARROWR_POSX+SCROLLB_SIZE-1, 0,
	   ARROWR_POSX+SCROLLB_SIZE-1,
	   SCROLLB_SIZE-1);
 XDrawLine(This->display, This->window, gc_2,
	   ARROWR_POSX, SCROLLB_SIZE-1,
	   ARROWR_POSX+SCROLLB_SIZE-2,
	   SCROLLB_SIZE-1); 
 XDrawLine(This->display, This->window, gc_2,
	   ARROWR_POSX+DECALAGE, 
	   SCROLLB_SIZE-1-DECALAGE,
	   ARROWR_POSX+SCROLLB_SIZE-1-DECALAGE,
	   (SCROLLB_SIZE-1) /2);
 XDrawLine(This->display, This->window, gc_1,
	   ARROWR_POSX+DECALAGE, DECALAGE,
	   ARROWR_POSX+DECALAGE,
	   SCROLLB_SIZE-1-DECALAGE);
 XDrawLine(This->display, This->window, gc_1,
	   ARROWR_POSX+DECALAGE, DECALAGE,
	   ARROWR_POSX+SCROLLB_SIZE-1-DECALAGE,
	   (SCROLLB_SIZE-1) /2);  
}

static void draw_up_arrow(This, select)
c_ScrollBarX	*This;
boolean		select;
{
 GC	gc_1;
 GC	gc_2;
  
 if (select)
 {
  gc_1 = This->gc_shadow;
  gc_2 = This->gc_highlight;
 }
 else
 {
  gc_1 = This->gc_highlight;
  gc_2 = This->gc_shadow;
 }

 XDrawLine(This->display, This->window, gc_1,
	   0, ARROWU_POSY,
	   SCROLLB_SIZE-2, ARROWU_POSY);
 XDrawLine(This->display, This->window, gc_1,
	   0, 1+ARROWU_POSY,
	   0, SCROLLB_SIZE-2+ARROWU_POSY);
 XDrawLine(This->display, This->window, gc_2, 
	   SCROLLB_SIZE-1, ARROWU_POSY,
	   SCROLLB_SIZE-1, SCROLLB_SIZE-1+ARROWU_POSY);
 XDrawLine(This->display, This->window, gc_2,
	   0, SCROLLB_SIZE-1+ARROWU_POSY,
	   SCROLLB_SIZE-2, SCROLLB_SIZE-1+ARROWU_POSY); 
 XDrawLine(This->display, This->window, gc_2,
	   DECALAGE, SCROLLB_SIZE-1-DECALAGE+ARROWU_POSY,
	   SCROLLB_SIZE-1-2, SCROLLB_SIZE-1-DECALAGE+ARROWU_POSY);
 XDrawLine(This->display, This->window, gc_2,
	   SCROLLB_SIZE-1-DECALAGE, SCROLLB_SIZE-1-DECALAGE+ARROWU_POSY,
	   (SCROLLB_SIZE-1)/2, DECALAGE+ARROWU_POSY);
 XDrawLine(This->display, This->window, gc_1,
	   DECALAGE, SCROLLB_SIZE-1-DECALAGE+ARROWU_POSY,
	   (SCROLLB_SIZE-1)/2, DECALAGE+ARROWU_POSY);
}

static void draw_down_arrow(This, select)
c_ScrollBarX	*This;
boolean		select;
{
 GC	gc_1;
 GC	gc_2;
  
 if (select)
 {
  gc_1 = This->gc_shadow;
  gc_2 = This->gc_highlight;
 }
 else
 {
  gc_1 = This->gc_highlight;
  gc_2 = This->gc_shadow;
 }

 XDrawLine(This->display, This->window, gc_1,
	   0, ARROWD_POSY,
	   SCROLLB_SIZE-2, ARROWD_POSY);
 XDrawLine(This->display, This->window, gc_1,
	   0, ARROWD_POSY+1,
	   0, ARROWD_POSY+SCROLLB_SIZE-2);
 XDrawLine(This->display, This->window, gc_2, 
	   SCROLLB_SIZE-1, ARROWD_POSY,
	   SCROLLB_SIZE-1, 
	   ARROWD_POSY+SCROLLB_SIZE-1);
 XDrawLine(This->display, This->window, gc_2,
	   0, ARROWD_POSY+SCROLLB_SIZE-1,
	   SCROLLB_SIZE-2, 
	   ARROWD_POSY+SCROLLB_SIZE-1); 
 XDrawLine(This->display, This->window, gc_1,
	   DECALAGE, ARROWD_POSY+DECALAGE,
	   SCROLLB_SIZE-1-DECALAGE, 
	   ARROWD_POSY+DECALAGE);
 XDrawLine(This->display, This->window, gc_1,
	   DECALAGE, ARROWD_POSY+DECALAGE,
	   (SCROLLB_SIZE-1)/2, 
	   ARROWD_POSY+SCROLLB_SIZE-1-DECALAGE);
 XDrawLine(This->display, This->window, gc_2,
	   (SCROLLB_SIZE-1)/2, 
	   ARROWD_POSY+SCROLLB_SIZE-1-DECALAGE,
	   SCROLLB_SIZE-1-DECALAGE, 
	   ARROWD_POSY+DECALAGE);
}

static void draw_h_central_part(This)
c_ScrollBarX	*This;
{
 XPoint       xpoint[3];
  
 xpoint[0].x = xpoint[1].x = 0;
 xpoint[2].x = This->Bsize - 1;
 xpoint[0].y = SCROLLB_SIZE-2;
 xpoint[1].y = xpoint[2].y = 0;
  
 XDrawLines(This->display, This->window, This->gc_highlight,
	    xpoint, 3, CoordModeOrigin);
  
 xpoint[0].x = xpoint[1].x = This->Bsize -1;
 xpoint[2].x = 0;
 xpoint[0].y = 0;
 xpoint[1].y = xpoint[2].y = SCROLLB_SIZE-1;
  
 XDrawLines(This->display, This->window, This->gc_shadow,
	    xpoint, 3, CoordModeOrigin);
}

static void draw_h_central_button(This)
c_ScrollBarX	*This;
{
 XPoint       xpoint[3];
  
 if (This->CPos+This->Csize >= CENTRAL_POS + CENTRAL_SIZE)
 {
  This->Csize = CENTRAL_POS + CENTRAL_SIZE - This->CPos;
  /*      This->CPos = CENTRAL_POS + CENTRAL_SIZE - This->Csize - 1; */
 }
 xpoint[0].x = xpoint[1].x = This->CPos + 1;
 xpoint[2].x = This->CPos+This->Csize - 1;
 xpoint[0].y = SCROLLB_SIZE - 1 - DECALAGE;
 xpoint[1].y = xpoint[2].y = DECALAGE;

 XDrawLines(This->display, This->window, This->gc_shadow,
	    xpoint, 3, CoordModeOrigin);
  
 xpoint[0].x = xpoint[1].x = This->CPos + This->Csize - 1;
 xpoint[2].x = This->CPos + 2;
 xpoint[0].y = DECALAGE + 1;
 xpoint[1].y = xpoint[2].y = SCROLLB_SIZE - 1 - DECALAGE;
  
 XDrawLines(This->display, This->window, This->gc_highlight,
	    xpoint, 3, CoordModeOrigin);
    
 if (This->Csize > 3)
  XFillRectangle(This->display, This->window, This->gc_slider,
		 This->CPos + 2, DECALAGE + 1,
		 This->Csize - 3,
		 SCROLLB_SIZE - 2 * (DECALAGE + 1));
}

static void draw_v_central_part(This)
c_ScrollBarX	*This;
{
 XPoint       xpoint[3];
  
 xpoint[0].x = xpoint[1].x = 0;
 xpoint[2].x = SCROLLB_SIZE - 2;
 xpoint[0].y = This->Bsize - 1;
 xpoint[1].y = xpoint[2].y = 0;
  
 XDrawLines(This->display, This->window, This->gc_highlight,
	    xpoint, 3, CoordModeOrigin);
  
 xpoint[0].x = xpoint[1].x = SCROLLB_SIZE-1;
 xpoint[2].x = 0;
 xpoint[0].y = 0;
 xpoint[1].y = xpoint[2].y = This->Bsize -1;
  
 XDrawLines(This->display, This->window, This->gc_shadow,
	    xpoint, 3, CoordModeOrigin);
}

static void draw_v_central_button(This)
c_ScrollBarX	*This;
{
 XPoint       xpoint[3];
  
 if (This->CPos+This->Csize >= CENTRAL_POS + CENTRAL_SIZE)
 {
  This->Csize = CENTRAL_POS + CENTRAL_SIZE - This->CPos;
  /*    This->CPos = CENTRAL_POS + CENTRAL_SIZE - This->Csize - 1; */
 }
 xpoint[0].x = xpoint[1].x = DECALAGE;
 xpoint[2].x = SCROLLB_SIZE-1-DECALAGE;
 xpoint[0].y = This->CPos+This->Csize-1;
 xpoint[1].y = xpoint[2].y = This->CPos+1;

 XDrawLines(This->display, This->window, This->gc_shadow,
	    xpoint, 3, CoordModeOrigin);
  
 xpoint[0].x = xpoint[1].x = SCROLLB_SIZE-1-DECALAGE;
 xpoint[2].x = DECALAGE+1;
 xpoint[0].y = This->CPos+2;
 xpoint[1].y = xpoint[2].y = This->CPos+This->Csize-1;
  
 XDrawLines(This->display, This->window, This->gc_highlight,
	    xpoint, 3, CoordModeOrigin);
  
 if (This->Csize > 3)
  XFillRectangle(This->display, This->window, This->gc_slider,
		 DECALAGE+1,  This->CPos + 2,
		 SCROLLB_SIZE-2*(DECALAGE+1),
		 This->Csize-3);
}


static int event_on_button(This, incr)
c_ScrollBarX	*This;
int		incr;
{
 XEvent	none, none2;
 Display	*display;
 boolean	sortie;
 int		sortie2;
 int		pos;
  
 display = This->display;
 sortie = FALSE;
 pos = sortie2 = 0;
 pos += F(This).Move(This, incr);
 XFlush(display);
  
 while (sortie2 < LOOPTIME)
 {
  Zusleep(WTIME);
  sortie2++;
  if (XEventsQueued(display, QueuedAfterReading))
  {
   if (XCheckTypedEvent(display, ButtonRelease, &none))
   {
    sortie = TRUE;
    break;
   }
   else
    XNextEvent(display, &none);
  }
 }
 while (!sortie)
 {
  pos += F(This).Move(This, incr >> 1);
  XFlush(display);
    
  Zusleep(WTIME2);
  if (XEventsQueued(display, QueuedAfterReading))
  {
   if (XCheckTypedEvent(display, ButtonRelease, &none))
    sortie = TRUE;
   else
    XNextEvent(display, &none);
  }
 }
 if ((sortie2 = XEventsQueued(display, QueuedAfterReading)) != 0)
 {
  while(sortie2)
  {
   XNextEvent(display, &none2);
   if (none.xany.serial == none2.xany.serial)
    break;
   sortie2--;
  }
 } 
 return pos;
}





