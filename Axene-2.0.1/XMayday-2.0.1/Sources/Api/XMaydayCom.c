/*
** XMaydayCom.c for XMayday in Api/
** XMaydayCom.c
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Sometime during     1996 One of the authors
** Last update Sun Jul 13 15:59:22 1997 One of the authors
*/

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#ifdef LINKED_WITH_XMU
#include <X11/Xmu/WinUtil.h> /* for XmuClientWindow() */
#else
extern Window XmuClientWindow();
/* if you do not want to link with the Xmu library */
#endif

#define STATIC_DEF

/* vroot.h is a header file which lets a client get along with `virtual root'
   window managers like swm, tvtwm, olvwm, etc. 
   If you don't care about supporting virtual root window managers, you can
   comment this line out.
 */
#define VROOT_STATIC

#include "XMayday.h"
#include "Vroot.h"

#define XMAYDAY_VERSION_PROP   "_XMAYDAY_VERSION"
#define XMAYDAY_LOCK_PROP      "_XMAYDAY_LOCK"
#define XMAYDAY_COMMAND_PROP   "_XMAYDAY_COMMAND"
#define XMAYDAY_RESPONSE_PROP  "_XMAYDAY_RESPONSE"
static Atom XA_XMAYDAY_VERSION  = (Atom)0;
static Atom XA_XMAYDAY_LOCK     = (Atom)0;
static Atom XA_XMAYDAY_COMMAND  = (Atom)0;
static Atom XA_XMAYDAY_RESPONSE = (Atom)0;

void xmayday_remote_init_atoms(dpy)
Display *dpy;
{
 if (! XA_XMAYDAY_VERSION)
  XA_XMAYDAY_VERSION = XInternAtom(dpy, XMAYDAY_VERSION_PROP, False);
 if (! XA_XMAYDAY_LOCK)
  XA_XMAYDAY_LOCK = XInternAtom(dpy, XMAYDAY_LOCK_PROP, False);
 if (! XA_XMAYDAY_COMMAND)
  XA_XMAYDAY_COMMAND = XInternAtom(dpy, XMAYDAY_COMMAND_PROP, False);
 if (! XA_XMAYDAY_RESPONSE)
  XA_XMAYDAY_RESPONSE = XInternAtom(dpy, XMAYDAY_RESPONSE_PROP, False);
}

Window xmayday_remote_find_window(dpy)
Display *dpy;
{
 int i;
 Window root = RootWindowOfScreen(DefaultScreenOfDisplay(dpy));
 Window root2, parent, *kids;
 unsigned int nkids;
 Window result = (Window)0;
 double df, tenative_version = 0.0;
  
 if (! XQueryTree(dpy, root, &root2, &parent, &kids, &nkids))
  return (Window)0;
  
 if (root != root2)
  return (Window)0;
  
 if (parent)
  return (Window)0;
  
 if (! (kids && nkids))
  return (Window)0;
  
 for (i = 0; i < nkids; i++)
 {
  Atom type;
  int format;
  unsigned long nitems, bytesafter;
  unsigned char *version = NULL;
  Window w; 
  int status;
    
  w = XmuClientWindow(dpy, kids[i]);
  if (w == (Window)0) continue;
  status = XGetWindowProperty(dpy, w, XA_XMAYDAY_VERSION,
			      0, (65536 / sizeof(long)),
			      False, XA_STRING,
			      &type, &format, &nitems, &bytesafter,
			      &version);    
  if (! version)
   continue;
    
  if (status == Success && type != None)
  {
   df = atof(version);
      
   if (df > tenative_version)
   {
    result = w;
    tenative_version = df;
   }
  }
  XFree(version);
 }
 return result;
}

static char *lock_data = NULL;

boolean xmayday_remote_obtain_lock(dpy, window)
Display *dpy;
Window window;
{
 int result;
 Atom actual_type;
 int actual_format;
 unsigned long nitems, bytes_after;
 unsigned char *data = NULL;
 boolean locked = FALSE;

 if (! lock_data)
 {
  lock_data = (char *)malloc( 255);
  sprintf(lock_data, "pid%d@", getpid());
  if (gethostname(lock_data + strlen(lock_data), 100))
   strcat(lock_data, "unknow");
 }
  
  
 /*  XGrabServer (dpy); */  /* ################################# DANGER! */
  
 result = XGetWindowProperty(dpy, window, XA_XMAYDAY_LOCK,
			     0, (65536 / sizeof(long)),
			     False,/* don't delete */
			     XA_STRING,
			     &actual_type, &actual_format,
			     &nitems, &bytes_after,
			     &data);
 if (result != Success || actual_type == None)
 {
  /* It's not now locked - lock it. */
  XChangeProperty(dpy, window, XA_XMAYDAY_LOCK, XA_STRING, 8,
		  PropModeReplace, (unsigned char *) lock_data,
		  strlen(lock_data));
  locked = TRUE;
 }
  
 /*  XUngrabServer (dpy); */ /* ############################## danger over */
 XSync(dpy, False);
  
 if (data)
  XFree(data);
  
 return locked;
}


void xmayday_remote_free_lock(dpy, window)
Display *dpy;
Window window;
{
 int result;
 Atom actual_type;
 int actual_format;
 unsigned long nitems, bytes_after;
 unsigned char *data = NULL;
  
  
 result = XGetWindowProperty(dpy, window, XA_XMAYDAY_LOCK,
			     0, (65536 / sizeof(long)),
			     True,/* atomic delete after */
			     XA_STRING,
			     &actual_type, &actual_format,
			     &nitems, &bytes_after,
			     &data);
 if (result != Success)
 {
  Xc_WARNING(("unable to read and delete XMAYDAY_LOCK_PROP property"));
  return;
 }
 else if (!data || !*data)
 {
  Xc_WARNING(("invalid data on XMAYDAY_LOCK_PROP of window 0x%x.",
	      (unsigned int) window));
  return;
 }
 else if (strcmp(data, lock_data))
 {
  Xc_WARNING(("XMAYDAY_LOCK_PROP was stolen!  Expected %s, saw %s!",
	      lock_data, data));
  return;
 }
  
 if (data)
  XFree(data);
}

boolean xmayday_remote_command(dpy, window, command, wait_ack)
Display *dpy;
Window window;
char *command;
boolean wait_ack;
{
 boolean result = FALSE;
 boolean done;
 int result_code;
 char *new_command = NULL;
  
 /* The -noraise option is implemented by passing a "noraise" argument
    to each command to which it should apply.
    */
 XChangeProperty(dpy, window, XA_XMAYDAY_COMMAND, XA_STRING, 8,
		 PropModeReplace, (unsigned char *) command,
		 strlen(command));
  
 done = !wait_ack;
 while (!done)
 {
  XEvent event;
    
  XNextEvent(dpy, &event);
    
  switch(event.type)
  {
  case DestroyNotify:
   if (event.xdestroywindow.window == window)
   {
    Xc_WARNING(("window 0x%x unexpectedly destroyed.",
		(unsigned int) window));
    result = FALSE;
    done = TRUE;
   }
   break;
  case PropertyNotify:
   if (event.xproperty.state == PropertyNewValue &&
       event.xproperty.window == window &&
       event.xproperty.atom == XA_XMAYDAY_RESPONSE)
   {
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;
	
    result_code = XGetWindowProperty(dpy, window, XA_XMAYDAY_RESPONSE,
				     0, (65536 / sizeof(long)),
				     True,/* atomic delete after */
				     XA_STRING,
				     &actual_type, &actual_format,
				     &nitems, &bytes_after,
				     &data);
	
    if (result_code != Success)
    {
     Xc_WARNING(("failed reading XMAYDAY_RESPONSE_PROP from window 0x%0x.", (unsigned int) window));
     result = FALSE;
     done = TRUE;
    }
    else
    {
     if (!data)
     {
      Xc_WARNING(("invalid data on XMAYDAY_RESPONSE_PROP property of window 0x%0x.", (unsigned int) window));
      result = FALSE;
      done = TRUE;
     }
     else
     {
      if (strncmp(data, "OK", 2) == 0)/* positive preliminary reply */
      {
       result = TRUE;
       done = TRUE;
      }
      else
      {
       if (strncmp(data, "FAILED", 6) == 0)
       {
	result = FALSE;
	done = TRUE;
       }
      }
     }
    }
    if (data)
     XFree(data);
   }
   break;
  }
 }
  
 if (new_command)
  free(new_command);

 return result;
}

void xmayday_remote_reset()
{
 if (lock_data)
  free(lock_data);
}

