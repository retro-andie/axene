/*
** Cursor.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Cursor/
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
** Started on  Mon Jul 17 14:19:50 1995 Emmanuel Paris
** Last update Thu Dec 30 17:40:45 1999 Emmanuel Paris
*/

#define _PRIVATE_CURSOR_
#include "Cursor.h"
#include "SignalHandler.h"
extern c_SignalHandler	*SignalH;

static void *cons_Cursor();
static void dest_Cursor();
static void *copy_Cursor();
static void set_cursor();
static int  get_cursor();
static void push_cursor();
static void push_and_set_cursor();
static void pop_cursor();
static void flush_cursor();
static void unset_cursor();
static void deset_cursor();
static void reset_cursor();

static boolean set_x_cursor();
static void *cons_GCursor();
static void dest_GCursor();
static void *copy_GCursor();
static void add_cursor();
static void del_cursor();
static void change_cursor_set();
static void enable_wait_mode();
static void set_wait_mode();
static void unset_wait_mode();
static void wait_mode_alarm();

sf_Cursor fc_Cursor=
{
 cons_Cursor,
 dest_Cursor,
 copy_Cursor,
 set_cursor,
 get_cursor,
 push_cursor,
 push_and_set_cursor,
 pop_cursor,
 flush_cursor,
 unset_cursor,
 reset_cursor,
};

static void *cons_Cursor(display, window)
Display	*display;
Window	window;
{
 c_Cursor *Objet;
  
 if ((Objet=(c_Cursor *)Xc_malloc("Cursor",sizeof(c_Cursor)))==NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 Objet->f = &fc_Cursor;  
 Objet->display = display;
 Objet->window = window;
 Objet->current_cursor = C_STANDARD;
 Objet->cursor_set = TRUE;
 XDefineCursor(display, window, GlobCursor->cursor_x[Objet->current_cursor]);
  
 Objet->cursor_pile = NULL;
 if (GlobCursor)
  F(GlobCursor).add_cursor(GlobCursor, Objet);
 return Objet;
}

static void dest_Cursor(This)
c_Cursor	*This;
{
 flush_cursor(This);
 unset_cursor(This);
 F(GlobCursor).del_cursor(GlobCursor,This);
  
 Xc_free(This);
}

static void *copy_Cursor(This)
c_Cursor	This;
{
 Xc_TRACE(("copy cursor function not done.normally not usefull."));
 return NULL;
}

static void set_cursor(This, num_cursor)
c_Cursor	*This;
int	num_cursor;
{
 if (This->current_cursor != num_cursor)
 {
  if (This->cursor_set)
   XDefineCursor(This->display,This->window,
		 GlobCursor->cursor_x[num_cursor]);
  This->current_cursor = num_cursor;
 }
}

static int get_cursor(This)
c_Cursor	*This;
{
 return This->current_cursor;
}


static void push_cursor(This)
c_Cursor	*This;
{
 p_Cursor     *top;
  
 top = (p_Cursor *)Xc_malloc("top cursor",sizeof(p_Cursor));
  
 top->cursor = This->current_cursor;
 top->NextCursor = This->cursor_pile;
 This->cursor_pile = top;
}

static void push_and_set_cursor(This,num_cursor)
c_Cursor	*This;
int	num_cursor;
{
 push_cursor(This);
 set_cursor(This,num_cursor);
}

static void pop_cursor(This)
c_Cursor	*This;
{
 p_Cursor	*top;
  
 top = This->cursor_pile;
 if (This->cursor_set)
  XDefineCursor(This->display,This->window,
		GlobCursor->cursor_x[top->cursor]);
 This->current_cursor = top->cursor;
 This->cursor_pile = This->cursor_pile->NextCursor;
 Xc_free(top);
}

static void flush_cursor(This)
c_Cursor	*This;
{
 p_Cursor	*pc_tmp1;
 p_Cursor	*pc_tmp2;
  
 pc_tmp1 = This->cursor_pile;
 while(pc_tmp1)
 {
  pc_tmp2 = pc_tmp1->NextCursor;
  if (!pc_tmp2)
  {
   if (This->cursor_set)
    XDefineCursor(This->display,This->window,
		  GlobCursor->cursor_x[pc_tmp1->cursor]);
  }
  Xc_free(pc_tmp1);
  pc_tmp1 = pc_tmp2;
 }
 This->cursor_pile = NULL;
}

static void unset_cursor(This)
c_Cursor	*This;
{
 l_GCursor	*lcursor;
  
 if (This->cursor_set)
 {
  XUndefineCursor(This->display,This->window); 
  This->cursor_set = FALSE;
  if (TEST_WAIT_MODE)
  {
   lcursor = GlobCursor->ListCursor;
   while(lcursor && lcursor->cursor != This)
    lcursor = lcursor->NextCursor;
      
   if (lcursor)
    lcursor->cursor_reset = TRUE;
  }
 }
}

static void deset_cursor(This)
c_Cursor	*This;
{
 if (This->cursor_set)
 {
  XUndefineCursor(This->display,This->window); 
  This->cursor_set = FALSE;
 }
}

static void reset_cursor(This)
c_Cursor	*This;
{
 if (!This->cursor_set)
 {
  XDefineCursor(This->display,This->window,
		GlobCursor->cursor_x[This->current_cursor]);
  This->cursor_set = TRUE;
 }
}

/*******************************************
*       Global Object GCursor Methods     *
*******************************************/

sf_GCursor fc_GCursor=
{
 cons_GCursor,
 dest_GCursor,
 copy_GCursor,
 add_cursor,
 del_cursor,
 change_cursor_set,
 enable_wait_mode,
 set_wait_mode,
 unset_wait_mode,
};

static void *cons_GCursor(display, window)
Display	*display;
Window	window;
{
 c_GCursor		*Objet;
 int			i;  
 if ((Objet=(c_GCursor *)Xc_malloc("GCursor",sizeof(c_GCursor)))==NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 Objet->f= &fc_GCursor;  
 Objet->display = display;
 memcpy(Objet->cursor_defs, default_cursor_set, sizeof(default_cursor_set));
 for(i=0;i<NB_CURSOR;i++)
 {
  if (!set_x_cursor(Objet,i,Objet->cursor_defs[i].type,
		    Objet->cursor_defs[i].data))
   set_x_cursor(Objet,i,Objet->cursor_defs[i].type2,
		Objet->cursor_defs[i].data2);
 }
 Objet->ListCursor = NULL;
  
 Objet->wait_mode = Objet->wait_once = Objet->wait_style = FALSE;
 Objet->root_cursor = NULL;
 Objet->wait_cursor_set = C_WAIT1;  
 Objet->wait_stack_static = Objet->wait_stack_animate = 0;
 return Objet;
}

static boolean set_x_cursor(Objet,i,type,data)
c_GCursor *Objet;
int	i;
int	type;
void    *data;
{
 int			width;
 int			height;
 int			width_allow;
 int			height_allow;
 int 			color;
 int			bit;
 int			x_hot;
 int			y_hot;
 GC			gc;
 Pixmap		psource;
 Pixmap		pmask;
 XImage		*xsource;
 XImage		*xmask;
 char			**image;
 defined_cursor	*dcursor;
 XColor		*xcolor;
 static XColor		black={0,0x0000,0x0000,0x0000,
				DoRed|DoGreen|DoBlue,0};
 static XColor		white={0,0xffff,0xffff,0xffff,
				DoRed|DoGreen|DoBlue,0};
#ifdef WITH_ZPM
 boolean	zpm = FALSE;
#endif
 
 switch(type)
 {
 case NO_CURSOR:
  return FALSE;
  break;
 case STANDARD_CURSOR:
  Objet->cursor_x[i]=XCreateFontCursor(Objet->display,(long)data);
  return TRUE;
  break;
 case DEFINED_CURSOR_XBM:
  dcursor=(defined_cursor *)data;
  psource=XCreateBitmapFromData(Objet->display,
				DefaultRootWindow(Objet->display),
				dcursor->csource,
				dcursor->width,dcursor->height);
  pmask=XCreateBitmapFromData(Objet->display,
			      DefaultRootWindow(Objet->display),
			      dcursor->cmask,
			      dcursor->width,dcursor->height);
  Objet->cursor_x[i]=XCreatePixmapCursor(Objet->display,psource,pmask,
					 &black,&white,dcursor->x_hot,
					 dcursor->y_hot);
  XFreePixmap(Objet->display,psource);
  XFreePixmap(Objet->display,pmask);
  return TRUE;
  break;
 case  DEFINED_CURSOR_XPM:
  image=(char **)data;
#ifdef WITH_ZPM
  if (**image == 'z')
  {
   image = ZPMtoXPM(image);
   zpm = TRUE;
  }
#endif

  XPMGetAttrib(image, &width, &height, &color,
	       &bit, &x_hot, &y_hot);
  XQueryBestCursor(Objet->display,DefaultRootWindow(Objet->display),
		   width, height, &width_allow, &height_allow);
  if (width<=width_allow && height<=height_allow)
  {
   XPMload(Objet->display,image,&xsource,&xmask,TRUE);
   psource=XCreatePixmap(Objet->display, 
			 DefaultRootWindow(Objet->display),
			 width,height,1);
   gc=XCreateGC(Objet->display,psource,0,0);
   XPutImage(Objet->display,psource,gc,xsource,0,0,0,0,width,height);
   Xc_free(xsource->data);
   XFree((char*)xsource);
   pmask=XCreatePixmap(Objet->display, 
		       DefaultRootWindow(Objet->display),
		       width,height,1);
   XPutImage(Objet->display,pmask,gc,xmask,0,0,0,0,width,height);
   Xc_free(xmask->data);
   XFree((char *)xmask);
   XFreeGC(Objet->display,gc);
   if (color==3)
   {
    xcolor=(XColor *)Xc_malloc("xcolor",(color+1)*sizeof(XColor));
    XPMGetColor(Objet->display,image,color,xcolor);
    Objet->cursor_x[i] = 
     XCreatePixmapCursor(Objet->display, psource, pmask,
			 xcolor+1, xcolor, x_hot, y_hot);
    Xc_free(xcolor);
   }
   else
   {
    Objet->cursor_x[i] = 
     XCreatePixmapCursor(Objet->display, psource, pmask,
			 &black, &white, x_hot, y_hot);
   }
   XFreePixmap(Objet->display, psource);
   XFreePixmap(Objet->display, pmask);
#ifdef WITH_ZPM
   if (zpm)
   {
    Xc_free(*image);
    Xc_free(image);
   }
#endif
   return TRUE;
  }
  else
   return FALSE;
 }
 return FALSE;
}

static void dest_GCursor(This)
c_GCursor	*This;
{
 int	i;
  
 DELETE(c_Cursor)(This->root_cursor);
 for(i=0;i<NB_CURSOR;i++)
 {
  /*  if (This->cursor_defs[i].data!=default_cursor_set[i].data && 
      This->cursor_defs[i].type==DEFINED_CURSOR)
      {
      XFreePixmap(This->display,
      ((defined_cursor *)This->cursor_defs[i].data)->psource);
      XFreePixmap(This->display,
      ((defined_cursor *)This->cursor_defs[i].data)->pmask);
      }
      */
  XFreeCursor(This->display,This->cursor_x[i]);
 }
 if (This->wait_mode && This->wait_style == WAIT_ANIMATE)
  alarm(0);
 Xc_free(This);
}
static void *copy_GCursor(This)
c_GCursor	This;
{
 Xc_TRACE(("copy gcursor function not done.normally not usefull."));
 return NULL;
}

static void add_cursor(This,cursor)
c_GCursor	*This;
c_Cursor	*cursor;
{
 l_GCursor	*tmp;
 l_GCursor	*lgcursor;
  
 lgcursor=(l_GCursor *)Xc_malloc("lgcursor",sizeof(l_GCursor));
 lgcursor->cursor=cursor;
 lgcursor->cursor_reset = FALSE;
 lgcursor->NextCursor = NULL;
 tmp=This->ListCursor;
 if (tmp)
 {
  while(tmp->NextCursor)
  {
   tmp=tmp->NextCursor;
  }
  tmp->NextCursor=lgcursor;
 }
 else
 {
  This->ListCursor=lgcursor;
 }
}

static void del_cursor(This,cursor)
c_GCursor	*This;
c_Cursor	*cursor;
{
 l_GCursor	*tmp;
 l_GCursor	*svg;
  
 svg=tmp=This->ListCursor;
 while(tmp && tmp->cursor!=cursor)
 {
  svg=tmp;
  tmp=tmp->NextCursor;
 }
 if (!tmp)
 {
  Xc_WARNING(("Attempt to free an unknown cursor !!!!"));
  return;
 }
 if (svg==tmp)
 {
  This->ListCursor=tmp->NextCursor;
 }
 else
 {
  svg->NextCursor=tmp->NextCursor;
 }
 Xc_free(tmp);
}

static void change_cursor_set(This,num_cursor,cursor_set)
c_GCursor	*This;
int		num_cursor;
Cursor_Set	*cursor_set;
{
 /*  if (This->cursor_defs[num_cursor]!=cursor_set)
     {
     if (num_cursor==This->current_cursor)
     {
     XUndefineCursor(This->display,This->window);
     XFreeCursor(This->display,This->cursor_x[num_cursor]);
     This->cursor_x[num_cursor]=XCreateFontCursor(This->display,cursor_set);
     XDefineCursor(This->display,This->window,This->cursor_x[num_cursor]);
     }
     else
     {
     XFreeCursor(This->display,This->cursor_x[num_cursor]);
     This->cursor_x[num_cursor]=XCreateFontCursor(This->display,cursor_set);
     } 
     } */
 Xc_TRACE(("function not usable."));
}

static void enable_wait_mode(This, window)
c_GCursor	*This;
Window		window;
{
 if (This->root_cursor != NULL)
 {
  DELETE(c_Cursor)(This->root_cursor);
  This->root_cursor = (c_Cursor *)NEW(c_Cursor)(This->display, window);
 }
 else
 {
  This->root_cursor = (c_Cursor *)NEW(c_Cursor)(This->display, window);
  F(SignalH).Add_Callback(SignalH, SIGALRM, wait_mode_alarm, This);
 }
}

static void set_wait_mode(This, style)
c_GCursor	*This;
boolean		style;     
{
 boolean	wait_already = FALSE;
  
 if (!This->root_cursor) return;
 if (This->wait_mode)
 {
  if (style == WAIT_STATIC)
  {
   This->wait_stack_static ++;
   return;
  }
  else				/* WAIT_ANIMATE */
  {
   if (This->wait_style == WAIT_STATIC)
   {
    This->wait_stack_static ++;
    wait_already = TRUE;
   }
   else				/* WAIT_ANIMATE */
   {
    This->wait_stack_animate ++;
    return;
   }
  }
    
 }
    
 This->wait_mode = TRUE;
 This->wait_style = style;
  
 if (style == WAIT_ANIMATE)
 {
  Xc_TRACE(("set wait mode animate"));
  This->wait_once = (wait_already == FALSE);
    
  alarm(1);
 }
 else
 {
  l_GCursor	*lcursor;
    
  lcursor = This->ListCursor;
  while(lcursor)
  {
   if (lcursor->cursor != This->root_cursor)
   {
    if (lcursor->cursor->cursor_set)
    {
     deset_cursor(lcursor->cursor);
    }
    else
    {
     lcursor->cursor_reset = TRUE;
    }
   }
   lcursor = lcursor->NextCursor;
  }
  F(This->root_cursor).set_cursor(This->root_cursor, C_WAIT);
  XFlush(This->display); 
 }
}

static void unset_wait_mode(This, style)
c_GCursor	*This;
int		style;
{  
 l_GCursor	*lcursor;
  
 if (!This->root_cursor) return;
 if (!This->wait_mode)
 {
  Xc_HISTORY(("Wait Mode already desactived."));
  return;
 }
  
 if (style == WAIT_STATIC)
 {
  if (This->wait_stack_static)
  {
   This->wait_stack_static--;
   return;
  }
  if (This->wait_style == WAIT_ANIMATE)
  {
   Xc_HISTORY(("to many unset Wait Mode Static"));
   return;
  }
 }
 else				/* WAIT_ANIMATE */
 {
  if (This->wait_stack_animate)
  {
   This->wait_stack_animate--;
   return;
  }
  if (This->wait_style == WAIT_STATIC)
  {
   Xc_HISTORY(("Error Wait Mode Anim never set, cannot unset it"));
   return;
  }
  if (This->wait_stack_static)
  {
   This->wait_stack_static--;
   alarm(0);
   if (This->wait_once)
   {
    lcursor = This->ListCursor;
    while(lcursor)
    {
     if (lcursor->cursor != This->root_cursor)
     {
      if (lcursor->cursor->cursor_set)
       deset_cursor(lcursor->cursor);
      else
       lcursor->cursor_reset = TRUE;
     }
     lcursor = lcursor->NextCursor;
    }
    F(This->root_cursor).set_cursor(This->root_cursor, C_WAIT);
    XFlush(This->display); 
   }
   else
   {
    F(This->root_cursor).set_cursor(This->root_cursor, C_WAIT);
    XFlush(This->display);
   }
  }
 }
 This->wait_mode = FALSE;
  
 if (This->wait_style == WAIT_ANIMATE)
 {
  alarm(0);
  if (This->wait_once) return;
 }
  
 F(This->root_cursor).set_cursor(This->root_cursor, C_STANDARD);
 lcursor = This->ListCursor;
 while(lcursor)
 {
  if (lcursor->cursor != This->root_cursor)
  {
   if (lcursor->cursor_reset)
    lcursor->cursor_reset = FALSE;
   else
    F(lcursor->cursor).reset_cursor(lcursor->cursor);
  }
  lcursor = lcursor->NextCursor;
 }
}

static void wait_mode_alarm(This)
c_GCursor	*This;
{
 l_GCursor	*lcursor;
  
 if (This->wait_mode)
 {
  if (This->wait_once)
  {
   lcursor = This->ListCursor;
   while(lcursor)
   {
    if (lcursor->cursor != This->root_cursor)
    {
     if (lcursor->cursor->cursor_set)
      deset_cursor(lcursor->cursor);
     else
      lcursor->cursor_reset = TRUE;
    }
    lcursor = lcursor->NextCursor;
   }
   This->wait_once = FALSE;
  }
  else
  {
   if (++This->wait_cursor_set > C_WAIT10)
    This->wait_cursor_set = C_WAIT1;
  }
    
  alarm(1);
    
  F(This->root_cursor).set_cursor(This->root_cursor, 
				  This->wait_cursor_set);
  XFlush(This->display); 
 }
}





