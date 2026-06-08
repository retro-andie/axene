/*
** RS_xclamation.h for Xclamation in RemoteServer/
** Remote server for Xclamation. This header contain exceptionnaly some code !
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
** Started on  Wed Jan 14 12:16:30 1998 Emmanuel Paris
** Last update Sat Jan 17 17:32:45 1998 Emmanuel Paris
*/

#ifndef _RS_xclamation_h_
#define _RS_xclamation_h_

#include "MainInterface.h"
#include "WInterface.h"

extern c_MainInterface  *MainInterface1;
extern void MenuFileOpen();

#define SOFTWARE_VERSION_PROP   "_XCLAMATION_VERSION"
#define SOFTWARE_LOCK_PROP      "_XCLAMATION_LOCK"
#define SOFTWARE_COMMAND_PROP   "_XCLAMATION_COMMAND"
#define SOFTWARE_RESPONSE_PROP  "_XCLAMATION_RESPONSE"
static Atom XA_SOFTWARE_VERSION  = 0;
static Atom XA_SOFTWARE_LOCK     = 0;
static Atom XA_SOFTWARE_COMMAND  = 0;
static Atom XA_SOFTWARE_RESPONSE = 0;

static void init_eventlook();
static void before_cmd();
static void after_cmd();

static boolean hs_open();
static boolean hs_raise();
static boolean hs_exit();

static cmd_parse_t hs_parse[] = 
{
 { "OPEN", 1, hs_open },
 { "RAISE", 0, hs_raise },
 { "EXIT", 0, hs_exit },
 { NULL, 0, NULL}
};

static void init_eventlook(param)
void **param;
{
 *param = MainInterface1;
}

static void before_cmd(param, cmd_func, argc, argv)
void **param;
boolean (*cmd_func)();
int argc;
char **argv;
{
}

static void after_cmd(param, cmd_func, argc, argv)
void **param;
boolean (*cmd_func)();
int argc;
char **argv;
{
}

static boolean hs_open(This, MInterface, argc, argv)
c_RemoteServer *This;
c_MainInterface *MInterface;
int argc;
char **argv;
{
 Xc_TRACE(("OPEN command: %s", argv[0]));
  
 MenuFileOpen(WIDGET(MInterface), argv[0]);
 
 return TRUE;
}

static boolean hs_raise(This, MInterface, argc, argv)
c_RemoteServer *This;
c_MainInterface *MInterface;
int argc;
char **argv;
{
 Window window;
  
 Xc_TRACE(("RAISE command"));
  
 window =  XtWindow(XtParent(MInterface->w_This));
 XRaiseWindow(This->display, window);
 return TRUE;
}

static boolean hs_exit(This, MInterface, argc, argv)
c_RemoteServer *This;
c_MainInterface *MInterface;
int argc;
char **argv;
{
 XEvent event;
  
 Xc_TRACE(("EXIT command"));
 
 event.type = ClientMessage;
 event.xclient.send_event = True;
 event.xclient.display = This->display;
 event.xclient.window = XtWindow(WIDGET(MInterface));
 event.xclient.format = 32;
 event.xclient.data.l[0] = 0xdeaddddd;
 XSendEvent(This->display, XtWindow(WIDGET(MInterface)),
	    True, 0, &event);
  
 return TRUE;
}

#endif /* _RS_xclamation_h_ */
