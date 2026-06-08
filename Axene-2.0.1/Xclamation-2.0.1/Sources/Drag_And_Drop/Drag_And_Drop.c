/*
** Drag_And_Drop.c for Xclamation, XQuad and XAllWrite in Drag_And_Drop/
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
** Started on  Mon Jul 17 14:41:57 1995 Emmanuel Paris
** Last update Mon May 18 14:27:11 1998 Emmanuel Paris
*/

#include <varargs.h>
#include "Drag_And_Drop.h"

void *cons_DragAndDrop();
void dest_DragAndDrop();
void *copy_DragAndDrop();

boolean eventlook_drag();
void eventlook_drop();
void select_input_drag();
void change_argument();
void create_context_drag();
void test_context_drag();
void enable_drag();
void disable_drag();
void activate_drag();
void disactivate_drag();
void set_callback_drag();

void XGetPointedWindow();
boolean	Is_Motion();
boolean	Is_Drop();
void read_arguments();

sf_DragAndDrop fc_DragAndDrop =
{
 cons_DragAndDrop,
 dest_DragAndDrop,
 copy_DragAndDrop,
 eventlook_drag,
 eventlook_drop,
 select_input_drag,
 change_argument,
 create_context_drag,
 test_context_drag,
 enable_drag,
 disable_drag,
 activate_drag,
 disactivate_drag,
 set_callback_drag,
};

void *cons_DragAndDrop(display)
Display	*display;
{
 c_DragAndDrop	*This;
  
 This = (c_DragAndDrop *)Xc_malloc("DragAndDrop", sizeof(c_DragAndDrop));
 This->f = &fc_DragAndDrop;
 This->display = display;
 This->active = FALSE;
 This->enable = FALSE;
 This->callback_motion = NULL;
 This->callback_drop = NULL;
 return This;
}

void dest_DragAndDrop(This)
c_DragAndDrop	*This;
{
 Xc_free(This);
}

void *copy_DragAndDrop(This)
c_DragAndDrop	*This;
{
 Xc_TRACE(("Not usefull"));
 return NULL;
}

boolean eventlook_drag(This, event)
c_DragAndDrop	*This;
XEvent		*event;
{
 Window		window;
 Window		fille;
 int		x;
 int		y;
 char		*name;
 int		i;
 void		*data_target;
  
 XEvent	peek_event;
 XWindowAttributes	attrib; 

 switch(event->type)
 {
 case MotionNotify:
  if (!This->current_window)
  {
   if ((i = XEventsQueued(This->display, QueuedAlready))>0)
   {
    XPeekEvent(This->display, &peek_event);
    while(peek_event.type == MotionNotify && i)
    {
     XNextEvent(This->display, event);
     XPeekEvent(This->display, &peek_event);
     i--;
    }
   }
   XGetPointedWindow(This->display, &window);
   if (window == This->root_window)
   {
    return FALSE;
   }
   XFetchName(This->display, window, &name);
   if (Is_Motion(name) || Is_Drop(name))
   {
    Xc_TRACE(("Entering window %x", window)); 
    This->current_window = window;
    XGetWindowAttributes(This->display, window, &attrib);
    This->current_event_mask = attrib.your_event_mask; 
    XSelectInput(This->display, window,  ButtonReleaseMask |
		 ButtonMotionMask | LeaveWindowMask );
    read_arguments(This, window);
    XFree(name);
    return FALSE;
   }
   else
   {
    XTranslateCoordinates(This->display,
			  window,
			  This->root_window,
			  event->xmotion.x,
			  event->xmotion.y,
			  &x, &y, &fille);
    event->xmotion.x = x;
    event->xmotion.y = y;
    event->xmotion.window = 0;
   }
   XFree(name);
  }
  if (This->callback_motion)
   (This->callback_motion)(This->callback_motion_data, *event,
			   This->data);
  return FALSE;
  break;
 case EnterNotify:
  if (event->xcrossing.window != This->root_window)
   return FALSE;
  break;
 case LeaveNotify:
  if (event->xcrossing.window == This->current_window)
  {
   Xc_TRACE(("Leaving window %x", This->current_window));
   XSelectInput(This->display,
		This->current_window,
		This->current_event_mask);
   This->current_window = 0;
   if (This->data)
   {
    Xc_free(This->data);
    This->data = NULL;
   }
   return FALSE;
  }
  break;
 case ButtonRelease:
  if (event->xbutton.button == 1 && 
      event->xbutton.window == This->current_window)
  {
   /*	Xc_TRACE(("Drop in Window %x",This->current_window)); */
   XSelectInput(This->display,
		This->current_window,
		This->current_event_mask);
   if (This->callback_drop)
   {
    data_target = This->data;
    read_arguments(This, This->root_window);
    (This->callback_drop)(This->callback_drop_data,
			  This->root_window,
			  This->data,
			  This->current_window,
			  data_target);
    Xc_free(data_target);
   }
  }
  else
  {
   if (This->data)
   {
    Xc_free(This->data);
    This->data = NULL;
   }
   if (event->xbutton.window == This->current_window)
   {
    XSelectInput(This->display,
		 This->current_window,
		 This->current_event_mask);
    read_arguments(This, This->root_window);
    if (This->callback_abort)
     (This->callback_abort)(This->callback_abort_data,
			    DD_DROP_ABORTED_BY_USER,
			    This->data);
   }
   else
   {
    read_arguments(This, This->root_window);
    if (This->callback_abort)
     (This->callback_abort)(This->callback_abort_data,
			    DD_DROP_ON_WRONG_WINDOW,
			    This->data);
   }
  } 
  if (This->data)
  {
   Xc_free(This->data);
   This->data = NULL;
  }
  This->current_window = 0;
  return FALSE;
  break;
 }
 return TRUE;
}

void eventlook_drop(This, event)
c_DragAndDrop	*This;
XEvent	*event;
{
 read_arguments(This, event->xclient.window);
 Xc_TRACE(("function adr: %ld  param adr: %ld",
	   ((long *)This->data)[0],
	   ((long *)This->data)[1]));
 ((status_t (*)())((long *)This->data)[0])
  ((void *)(((long *)This->data)[1]),  event);
 Xc_free(This->data);
}


void select_input_drag(This, window, mode, va_alist)
c_DragAndDrop	*This;
Window		window;
int		mode;
va_dcl
{
 va_list	list_arg;
 char		window_name[256];
 char		icon_name[256];
 int		i_value;
 void		*v_value;
 int		nb_mode;
 int		pnt;

 nb_mode = pnt = 0;
 window_name[0] = '\0';
 if (mode&DD_DROP)
 {
  if (nb_mode++)
   strcat(window_name, DD_S_SEPARATOR);
  strcat(window_name, DD_S_DROP);
 }
 if (mode&DD_MOTION)
 {
  if (nb_mode++)
   strcat(window_name, DD_S_SEPARATOR);
  strcat(window_name, DD_S_MOTION);
 }
 icon_name[0] = 'N';
 icon_name[1] = (char)sizeof(long)+'0';
 icon_name[2] = '|';
 icon_name[3] = '\0';
 va_start(list_arg);
 if ((i_value = va_arg(list_arg, int)))
 {
  while(1)
  {
   v_value = va_arg(list_arg, void *);
   switch(i_value)
   {
   case DD_STRING:
    strcat(icon_name, DD_S_STRING);
    pnt = strlen(icon_name);
    strcat(icon_name, (char *)v_value);
    break;
   case DD_INT:
    strcat(icon_name, DD_S_INT);
    pnt = strlen(icon_name);
    sprintf(icon_name+pnt, "%d", (int)((long)v_value));
    break;
   case DD_LONG:
    strcat(icon_name, DD_S_LONG);
    pnt = strlen(icon_name);
    sprintf(icon_name+pnt, "%ld", (long)v_value);
    break;
   }
   pnt = strlen(icon_name);
   if (!(i_value = va_arg(list_arg, int)))
    break;
   strcat(icon_name, DD_S_SEPARATOR);
  }
 }
 XStoreName(This->display, window, window_name);
 XSetIconName(This->display, window, icon_name);
 va_end(list_arg);
}

void change_argument(This, window, num, type, value)
c_DragAndDrop	*This;
Window		window;
int		num;
int		type;
void		*value;
{
 char	*name;
 char	*pnt_name;
 char	new[256];
 int	nb_sep;
 int	pnt_new;

 XGetIconName(This->display, window, &name);
 pnt_name = name;
 nb_sep =  -1;
 while(nb_sep<num)
 {
  while(*pnt_name != DD_C_SEPARATOR)
   pnt_name++;
  nb_sep++;
  pnt_name++;
 }
 memcpy(new, name, pnt_name-name);
 new[pnt_name-name] = '\0';
 switch(type)
 {
 case DD_STRING:
  strcat(new, DD_S_STRING);
  strcat(new, (char *)value);
  break;
 case DD_INT:
  strcat(new, DD_S_INT);
  pnt_new = strlen(new);
  sprintf(new+pnt_new, "%d",  (int)((long)value));
  break;
 case DD_LONG:
  strcat(new, DD_S_LONG);
  pnt_new = strlen(new);
  sprintf(new+pnt_new,  "%ld",  (long)value);
  break;
 }
 while(*pnt_name != DD_C_SEPARATOR && *pnt_name != 0)
  pnt_name++;
 if (*pnt_name)
  strcat(new, pnt_name);
 /*  Xc_TRACE((new)); */
 XSetIconName(This->display, window, new);
 XFree(name);
}

void create_context_drag(This, window)
c_DragAndDrop	*This;
Window		window;
{
 XWindowAttributes	attrib;

 This->root_window = window;
 XGetWindowAttributes(This->display, window, &attrib);
 This->root_event_mask = attrib.your_event_mask;
 This->enable = FALSE;
 /*  Xc_TRACE(("get event mask: %x", This->root_event_mask));*/
}

void test_context_drag(This, window)
c_DragAndDrop	*This;
Window		window;
{
 XWindowAttributes	attrib;

 if (This->root_window != window)
 {
  This->root_window = window;
  XGetWindowAttributes(This->display, window, &attrib);
  This->root_event_mask = attrib.your_event_mask;
  This->enable = FALSE;
  /*      Xc_TRACE(("get event mask: %x", This->root_event_mask));*/
 }
}

void enable_drag(This)
c_DragAndDrop	*This;
{
 if (!This->enable)
 {
  XSelectInput(This->display, This->root_window, This->root_event_mask
	       | OwnerGrabButtonMask); 
  This->enable = TRUE;
  /*      Xc_TRACE(("drag enable"));*/
 }
}

void disable_drag(This)
c_DragAndDrop	*This;
{
 if (This->enable)
 {
  XSelectInput(This->display, This->root_window, This->root_event_mask);
  This->enable = FALSE;
  /*      Xc_TRACE(("drag disable"));*/
 }
}

void activate_drag(This)
c_DragAndDrop	*This;
{
 /*  Xc_TRACE(("activate"));*/

 if (!This->active)
 {
  This->active = TRUE;
  This->data = NULL;
  This->current_window = 0;
 }
}

void disactivate_drag(This)
c_DragAndDrop	*This;
{
 /*  Xc_TRACE(("disactivate"));*/
  
 if (This->active)
 {
  if (This->current_window)
   XSelectInput(This->display, 
		This->current_window, 
		This->current_event_mask);
  This->active = FALSE;
  This->callback_motion = NULL;
  This->callback_drop = NULL;
  This->callback_abort = NULL;
  if (This->data) 
  {
   Xc_free(This->data);
   This->data = NULL;
  }
 }
}

void set_callback_drag(This, type, function, param)
c_DragAndDrop	*This;
int		type;
void		(*function)();
void		*param;
{
 switch(type)
 {
 case DD_DROP:
  This->callback_drop = function;
  This->callback_drop_data = param;  
  break;    
 case DD_MOTION:
  This->callback_motion = function;
  This->callback_motion_data = param;  
  break;
 case DD_ABORT:
  This->callback_abort = function;
  This->callback_abort_data = param;  
  break;
 }
}

void XGetPointedWindow(display, window)
Display	*display;
Window	*window;
{
 Window	Wfille;
 Window	Wnone;
 int		Inone;
  
 Wfille = DefaultRootWindow(display);
 while(Wfille)
 {
  *window = Wfille;
  XQueryPointer(display, *window, &Wnone, &Wfille,
		&Inone, &Inone, &Inone, &Inone, &Inone);
 }
}

boolean	Is_Motion(name)
char	*name;
{
 char	*pos;

 if (!name)
  return FALSE;
 pos = Xstrstr(name, DD_S_MOTION);
 return (pos?TRUE:FALSE);
}

boolean	Is_Drop(name)
char	*name;
{
 char	*pos;

 if (!name)
  return FALSE;
 pos = Xstrstr(name, DD_S_DROP);
 return (pos ? TRUE : FALSE);
}

void read_arguments(This, window)
c_DragAndDrop	*This;
Window		window;
{
 int	i;
 int	size;
 int	nb_sep;
 int	long_size;
 char	*pnt_chaine;
 void	*data;
 void  *pnt_data;
 char	*icon_name;

 XGetIconName(This->display, window, &icon_name);
 if (!icon_name)
 {
  This->data = NULL;
  return;
 }
 Xc_TRACE(("icon name: %s", icon_name));
 size = nb_sep = 0;
 pnt_chaine = icon_name;
 if (*pnt_chaine != 'N')
 {
  XFree(icon_name);
  This->data = NULL;
  return;
 }
 else
 {
  long_size = pnt_chaine[1] - '0';
  pnt_chaine += 3;
 }
 do
 {
  switch(*pnt_chaine)
  {
   /*	case DD_C_STRING:
	size+=sizeof(char *);
	break; */
  case DD_C_INT:
   size += sizeof(int);
   break;
  case DD_C_LONG:
   size += sizeof(long);
   break;
  default:
   This->data = NULL;
   return;
  }
  while(*pnt_chaine && *pnt_chaine != DD_C_SEPARATOR)
   pnt_chaine++;
  if (*pnt_chaine)
  {
   *pnt_chaine = '\0';
   pnt_chaine++;
   nb_sep++;
  }
  else
   break;
 }
 while(1);
 pnt_data = data = (void *)Xc_malloc("drag data", size);
 pnt_chaine = icon_name + 3;
 for(i = 0; i <= nb_sep; i++)
 {
  switch(*pnt_chaine++)
  {
  case DD_C_STRING:
   *(char **)pnt_data = 
    (char *)Xc_malloc("chaine", strlen(pnt_chaine) + 1);
   strcpy(*(char **)pnt_data, pnt_chaine);
   pnt_data =  (char **)pnt_data + 1;
   break;
  case DD_C_INT:
   *(int *)pnt_data = atoi(pnt_chaine);
   pnt_data =  (int *)pnt_data + 1;
   break;
  case DD_C_LONG:
   if (long_size == sizeof(long)) 
    *(long *)pnt_data = atol(pnt_chaine);
   else
    *(long *)pnt_data = 0L;
   pnt_data =  (long *)pnt_data + 1;
   break;
  }
  while(*pnt_chaine)
   pnt_chaine++;
  pnt_chaine++;
 }
 This->data = data;
 XFree(icon_name);
}









