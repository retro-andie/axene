/* -*- Mode:C; tab-width: 8 -*-
 * remote.c --- remote control of Netscape Navigator for Unix.
 * version 1.1.2, for Netscape Navigator 1.1 and newer.
 *
 * Copyright © 1995 Netscape Communications Corporation, all rights reserved.
 * Created: Jamie Zawinski <jwz@netscape.com>, 24-Dec-94.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * Documentation for the protocol which this code implements may be found at:
 *
 *    http://home.netscape.com/newsref/std/x-remote.html
 *
 * Bugs and commentary to x_cbug@netscape.com.
 */

#include "xcalibur.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
/* #include <X11/Xmu/WinUtil.h> */	/* for XmuClientWindow() */
/* Xibios: I put this in comment. If you don't want to link with libXmu.a,
   compile and link with ClientWin.c */
extern Window XmuClientWindow();

/* vroot.h is a header file which lets a client get along with `virtual root'
   window managers like swm, tvtwm, olvwm, etc.  If you don't have this header
   file, you can find it at "http://home.netscape.com/newsref/std/vroot.h".
   If you don't care about supporting virtual root window managers, you can
   comment this line out.
 */
#define VROOT_STATIC
#include "RH_vroot.h"

#define MOZILLA_VERSION_PROP   "_MOZILLA_VERSION"
#define MOZILLA_LOCK_PROP      "_MOZILLA_LOCK"
#define MOZILLA_COMMAND_PROP   "_MOZILLA_COMMAND"
#define MOZILLA_RESPONSE_PROP  "_MOZILLA_RESPONSE"
static Atom XA_MOZILLA_VERSION  = (Atom)0;
static Atom XA_MOZILLA_LOCK     = (Atom)0;
static Atom XA_MOZILLA_COMMAND  = (Atom)0;
static Atom XA_MOZILLA_RESPONSE = (Atom)0;

void mozilla_remote_init_atoms(dpy)
Display *dpy;
{
  if (! XA_MOZILLA_VERSION)
    XA_MOZILLA_VERSION = XInternAtom (dpy, MOZILLA_VERSION_PROP, False);
  if (! XA_MOZILLA_LOCK)
    XA_MOZILLA_LOCK = XInternAtom (dpy, MOZILLA_LOCK_PROP, False);
  if (! XA_MOZILLA_COMMAND)
    XA_MOZILLA_COMMAND = XInternAtom (dpy, MOZILLA_COMMAND_PROP, False);
  if (! XA_MOZILLA_RESPONSE)
    XA_MOZILLA_RESPONSE = XInternAtom (dpy, MOZILLA_RESPONSE_PROP, False);
}

Window mozilla_remote_find_window(dpy)
Display *dpy;
{
  int i;
  Window root = RootWindowOfScreen (DefaultScreenOfDisplay (dpy));
  Window root2, parent, *kids;
  unsigned int nkids;
  Window result = (Window)0;
  double df, tenative_version = 0.0;
  
  if (! XQueryTree (dpy, root, &root2, &parent, &kids, &nkids))
    return (Window)0;
  
  if (root != root2)
    return (Window)0;
  
  if (parent)
    return (Window)0;
  
  if (! (kids && nkids))
    return (Window)0;
  
  for (i = 0; i < (int)nkids; i++)
  {
    Atom type;
    int format;
    unsigned long nitems, bytesafter;
    unsigned char *version = NULL;
    Window w;
    int status;
    
    w = XmuClientWindow (dpy, kids[i]);
    if (w == (Window)0) continue;
    status = XGetWindowProperty (dpy, w, XA_MOZILLA_VERSION,
				 0, (65536 / sizeof (long)),
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
    XFree (version);
  }
  return result;
}

static char *lock_data = NULL;

boolean mozilla_remote_obtain_lock(dpy, window)
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
    lock_data = (char *)Xc_malloc("lock data", 255);
    sprintf (lock_data, "pid%d@", (int)getpid());
    if (gethostname (lock_data + strlen (lock_data), 100))
      strcat(lock_data, "unknow");
  }
  
  
  XGrabServer (dpy);   /* ################################# DANGER! */
  
  result = XGetWindowProperty (dpy, window, XA_MOZILLA_LOCK,
			       0, (65536 / sizeof (long)),
			       False, /* don't delete */
			       XA_STRING,
			       &actual_type, &actual_format,
			       &nitems, &bytes_after,
			       &data);
  if (result != Success || actual_type == None)
  {
    /* It's not now locked - lock it. */
    XChangeProperty (dpy, window, XA_MOZILLA_LOCK, XA_STRING, 8,
		     PropModeReplace, (unsigned char *) lock_data,
		     strlen (lock_data));
    locked = True;
  }
  
  XUngrabServer (dpy); /* ################################# danger over */
  XSync (dpy, False);
  
  if (data)
    XFree (data);
  
  return locked;
}


void mozilla_remote_free_lock(dpy, window)
Display *dpy;
Window window;
{
  int result;
  Atom actual_type;
  int actual_format;
  unsigned long nitems, bytes_after;
  unsigned char *data = NULL;

  
  result = XGetWindowProperty (dpy, window, XA_MOZILLA_LOCK,
			       0, (65536 / sizeof (long)),
			       True, /* atomic delete after */
			       XA_STRING,
			       &actual_type, &actual_format,
			       &nitems, &bytes_after,
			       &data);
  if (result != Success)
    {
      Xc_WARNING(("unable to read and delete MOZILLA_LOCK_PROP property"));
      return;
    }
  else if (!data || !*data)
    {
      Xc_WARNING(("invalid data on MOZILLA_LOCK_PROP of window 0x%x.",
		  (unsigned int) window));
      return;
    }
  else if (strcmp (data, lock_data))
    {
      Xc_WARNING(("MOZILLA_LOCK_PROP was stolen!  Expected %s, saw %s!",
	       lock_data, data));
      return;
    }

  if (data)
    XFree (data);
}

boolean mozilla_remote_command(dpy, window, command, raise_p, wait_ack)
Display *dpy;
Window window;
char *command;
boolean raise_p;
boolean wait_ack;
{
  boolean result = FALSE;
  boolean done;
  char *new_command = NULL;
  int result_code;
  
  /* The -noraise option is implemented by passing a "noraise" argument
     to each command to which it should apply.
   */
  if (! raise_p)
    {
      char *close;
      new_command = (char *)Xc_malloc("new cmd", strlen (command) + 20);
      strcpy (new_command, command);
      close = strrchr (new_command, ')');
      if (close)
	strcpy (close, ", noraise)");
      else
	strcat (new_command, "(noraise)");
      command = new_command;
    }

  XChangeProperty (dpy, window, XA_MOZILLA_COMMAND, XA_STRING, 8,
		   PropModeReplace, (unsigned char *) command,
		   strlen (command));
  done = !wait_ack;
  while (!done)
  {
    XEvent event;
    
    XNextEvent (dpy, &event);
    
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
	  event.xproperty.atom == XA_MOZILLA_RESPONSE)
      {
	Atom actual_type;
	int actual_format;
	unsigned long nitems, bytes_after;
	unsigned char *data = NULL;
      
	result_code = XGetWindowProperty (dpy, window, XA_MOZILLA_RESPONSE,
					  0, (65536 / sizeof (long)),
					  True, /* atomic delete after */
					  XA_STRING,
					  &actual_type, &actual_format,
					  &nitems, &bytes_after,
					  &data);
	
	if (result_code != Success)
	{
	  Xc_WARNING(("failed reading MOZILLA_RESPONSE_PROP from window 0x%0x.", (unsigned int) window));
	  result = FALSE;
	  done = TRUE;
	}
	else if (!data)
	{
	  Xc_WARNING(("invalid data on MOZILLA_RESPONSE_PROP property of window 0x%0x.", (unsigned int) window));
	  result = FALSE;
	  done = TRUE;
	}
	else if (*data == '1')	/* positive preliminary reply */
	{
	  Xc_TRACE((data + 4));
	  /* keep going */
	  done = FALSE;
	}
	else if (!strncmp (data, "200", 3))	/* positive completion */
	{
	  result = TRUE;
	  done = TRUE;
	}
	else if (*data == '2')		/* positive completion */
	{
	  Xc_TRACE((data + 4));
	  result = TRUE;
	  done = TRUE;
	}
	else if (*data == '3')	/* positive intermediate reply */
	{
	  Xc_WARNING(("internal error: server wants more information? %s",
		      data));
	  result = FALSE;
	  done = TRUE;
	}
	else if (*data == '4' ||	/* transient negative completion */
		 *data == '5')	/* permanent negative completion */
	{
	  Xc_TRACE((data + 4));
	  result = (*data - '0');
	  done = TRUE;
	}
	else
	{
	  Xc_WARNING(("unrecognised MOZILLA_RESPONSE_PROP from window 0x%x: %s\n", (unsigned int) window, data));
	  result = FALSE;
	  done = TRUE;
	}
	  
	if (data)
	  XFree (data);
      }
      break;
    }
  }
  
  if (new_command)
    Xc_free (new_command);

  return result;
}

void mozilla_remote_reset()
{
  if (lock_data)
    Xc_free(lock_data);
}




