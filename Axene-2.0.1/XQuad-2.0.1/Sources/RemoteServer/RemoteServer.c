/*
** RemoteServer.c for XQuad in RemoteServer/
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
** Started on  Mon Jul 17 17:45:41 1995 Emmanuel Paris
** Last update Sat Jan 17 17:31:25 1998 Emmanuel Paris
*/


#include "RemoteServer.h"
#include <X11/Xatom.h>
#include "version.h"

#ifdef Xc_XCLAMATION
#include "RS_xclamation.h"
#endif

#ifdef Xc_XQUAD
#include "RS_xquad.h"
#endif

#ifdef Xc_XALLWRITE
#include "RS_xallwrite.h"
#endif

#ifdef Xc_XMAYDAY
#include "RS_xmayday.h"
#endif

static void *cons_RemoteServer();
static void dest_RemoteServer();
static void *copy_RemoteServer();
static boolean eventlook();


sf_RemoteServer fc_RemoteServer =
{
 cons_RemoteServer,
 dest_RemoteServer,
 copy_RemoteServer,
 eventlook
};


static void *cons_RemoteServer(display, window)
Display *display;
Window window;
{
 c_RemoteServer	*This;
 char version[100];
  
 if ((This = (c_RemoteServer *)Xc_malloc
      ("RemoteServer", sizeof(c_RemoteServer))) == NULL)
  Xc_FATAL(("Can't build this object: memory allocation error"));

 This->f = &fc_RemoteServer;
 This->display = display;
 This->window = window;
  
 if (! XA_SOFTWARE_VERSION)
  XA_SOFTWARE_VERSION = XInternAtom(display, SOFTWARE_VERSION_PROP, False);
 if (! XA_SOFTWARE_LOCK)
  XA_SOFTWARE_LOCK = XInternAtom(display, SOFTWARE_LOCK_PROP, False);
 if (! XA_SOFTWARE_COMMAND)
  XA_SOFTWARE_COMMAND = XInternAtom(display, SOFTWARE_COMMAND_PROP, False);
 if (! XA_SOFTWARE_RESPONSE)
  XA_SOFTWARE_RESPONSE = XInternAtom(display, SOFTWARE_RESPONSE_PROP, False);
  
 /* delete atom lock */  
 sprintf(version, "%1d%1d.%d", MAJOR, MINOR, PL);
 XChangeProperty (display, window, XA_SOFTWARE_VERSION, XA_STRING, 8,
		  PropModeReplace, (unsigned char *) version,
		  strlen(version) + 1);
  
 XSelectInput(display, window, (PropertyChangeMask|StructureNotifyMask));
  
 Xc_TRACE(("Object RemoteServer build"));
 return This;
}

static void dest_RemoteServer(This)
c_RemoteServer *This;
{
 XDeleteProperty(This->display, This->window, XA_SOFTWARE_VERSION);
 XDeleteProperty(This->display, This->window, XA_SOFTWARE_LOCK);
 XDeleteProperty(This->display, This->window, XA_SOFTWARE_COMMAND);
 XDeleteProperty(This->display, This->window, XA_SOFTWARE_RESPONSE);

 Xc_TRACE(("Object RemoteServer destroyed"));
 Xc_free(This);
}

static void *copy_RemoteServer(This)
c_RemoteServer *This;
{
 Xc_WARNING(("Copy object RemoteServer useless"));
 return NULL;
}


static boolean eventlook(This, event)
c_RemoteServer *This;
XEvent	*event;
{
 Atom actual_type;
 int actual_format;
 unsigned long nitems, bytes_after;
 unsigned char *data = NULL;
 int  i, j, k, l, len, nb_cmd, argc, result;
 boolean success = FALSE, valid_command, valid_argument;
 char *command, *cmd, **argv;
 void *param;
 
 init_eventlook(&param);
 if (event->type == PropertyNotify &&
     event->xproperty.window == This->window &&
     event->xproperty.state == PropertyNewValue &&
     event->xproperty.atom == XA_SOFTWARE_COMMAND)
 {
  result = XGetWindowProperty(This->display, This->window, 
			      XA_SOFTWARE_COMMAND,
			      0, (65536 / sizeof (long)),
			      True,/* atomic delete after */
			      XA_STRING, &actual_type, &actual_format,
			      &nitems, &bytes_after, &data);
  if (result == Success && data != NULL)
  {
     
   Xc_TRACE(("Command: %s", data));
     
   command = Xc_strdup("comm", data);
   cmd = Xc_malloc("cmd", strlen(data));
     
   i = 0; nb_cmd = 1;
     
   /* separate and count commands */
   while(command[i])
   {
    if (command[i] == ';')
    {
     command[i] = '\0';
     nb_cmd++;
    }
    i++;
   }
   j = 0;
     
   /* get commands */
   for(i = 0; i < nb_cmd; i++)
   {
    len = strlen(command + j);
    for(k = 0; k < len; k++)
    {
     if (command[k + j] == '=')
      break;
     cmd[k] = command[k + j];
    }
    cmd[k] = '\0'; 
       
       
    if (k != len) 
    {				/* argument detected */
     argc = 1;
     l = ++k;
	 
     /* separate and count arguments */
     while(command[l + j])
     {
      if (command[l + j] == ',')
      {
       command[l + j] = '\0';
       argc++;
      }
      l++;
     }
	 
     /* get arguments */
     argv = (char **)Xc_malloc("*argv", sizeof(char *) * argc);
     for(l = 0; l < argc; l++)
     {
      argv[l] = Xc_strdup("argv", command + k + j);
      k += strlen(argv[l]) + 1;
     }
    }
    else
    {				/* no argument detected */
     argc = 0;
     argv = NULL;
    }
       
       
    valid_command = valid_argument = FALSE;
    k = 0;
    while(hs_parse[k].cmd_name != NULL)
    {
     if (Xstrcmp(cmd, hs_parse[k].cmd_name))
     {
      valid_command = TRUE;
      if (hs_parse[k].cmd_arg < 0 || hs_parse[k].cmd_arg == argc)
      {
       valid_argument = TRUE;
       break;
      }
     }
     k++;
    }
       
    if (valid_command && valid_argument)
    {
     before_cmd(&param, hs_parse[k].cmd_func, argc, argv);
     
     success |= hs_parse[k].cmd_func(This, param, argc, argv);

     after_cmd(&param, hs_parse[k].cmd_func, argc, argv);
    }
    else
    {
     if (valid_command)
      Xc_WARNING(("Invalid number of argument for command: %s", cmd));
     else
      Xc_WARNING(("Invalid command: %s", cmd?cmd:"(NULL)"));
    }
       
    for(k = 0; k < argc; k++)
     Xc_free(argv[k]);
    if (argv) Xc_free(argv);
       
    j += len + 1;
   }
     
   Xc_free(cmd);
   Xc_free(command);
  }
     
   
  if (data)
   XFree (data);
  if (success)
   XChangeProperty (This->display, This->window,
		    XA_SOFTWARE_RESPONSE, XA_STRING, 8,
		    PropModeReplace, (unsigned char *) "OK", 3);
  else
   XChangeProperty (This->display, This->window,
		    XA_SOFTWARE_RESPONSE, XA_STRING, 8,
		    PropModeReplace, (unsigned char *) "FAILED", 7);
     
  return TRUE;
 }
 return FALSE;
}












