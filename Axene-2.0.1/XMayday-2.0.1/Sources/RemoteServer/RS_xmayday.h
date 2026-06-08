/*
** RS_xmayday.h for XMayday in RemoteServer/
** Remote server for XMayday. This header contain exceptionnaly some code !
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
** Started on  Wed Jan 14 12:16:30 1998 Emmanuel Paris
** Last update Thu Jan 15 11:59:43 1998 Emmanuel Paris
*/

#ifndef _RS_xmayday_h_
#define _RS_xmayday_h_

#include "MainInterface.h"
#include "Interface.h"
#include "HelpManager.h"

extern c_Interface     *GlobInterface;
extern void DisplayIndex();

#define SOFTWARE_VERSION_PROP   "_XMAYDAY_VERSION"
#define SOFTWARE_LOCK_PROP      "_XMAYDAY_LOCK"
#define SOFTWARE_COMMAND_PROP   "_XMAYDAY_COMMAND"
#define SOFTWARE_RESPONSE_PROP  "_XMAYDAY_RESPONSE"
static Atom XA_SOFTWARE_VERSION  = 0;
static Atom XA_SOFTWARE_LOCK     = 0;
static Atom XA_SOFTWARE_COMMAND  = 0;
static Atom XA_SOFTWARE_RESPONSE = 0;

static void init_eventlook();
static void before_cmd();
static void after_cmd();

static boolean hs_index();
static boolean hs_url();
static boolean hs_raise();
static boolean hs_exit();

static cmd_parse_t hs_parse[] = 
{
 { "INDEX", 1, hs_index },
 { "URL", 1, hs_url },
 { "RAISE", 0, hs_raise },
 { "EXIT", 1, hs_exit },
 { "EXIT", 0, hs_exit },
 { NULL, 0, NULL}
};

static void init_eventlook(param)
void **param;
{
 *param = (void *)GlobInterface;
}

static void before_cmd(param, cmd_func, argc, argv)
void **param;
boolean (*cmd_func)();
int argc;
char **argv;
{
 if (cmd_func == hs_index)
 {
  c_Interface *Interface;
  c_Interface *RInterface = (c_Interface *)(*param);
  

  if (!RInterface->HelpManager->index_exist ||
      !Xstrcmp(RInterface->HelpWIndex->url_filename, argv[0]))
  {				/* search a window with same index url */
   Interface = GlobInterface;
   while(Interface)
   {
    if (Interface->HelpManager->index_exist &&
	Xstrcmp(Interface->HelpWIndex->url_filename, argv[0]))
     break;
    Interface = Interface->Next;
   }
   
   if (Interface)
    RInterface = Interface;
   else
    RInterface = (c_Interface *)COPY(c_Interface)(RInterface);
  }
  *param = (void *)RInterface;
 }
}

static void after_cmd(param, cmd_func, argc, argv)
void **param;
boolean (*cmd_func)();
int argc;
char **argv;
{
 if (cmd_func == hs_exit)
 {
  *param = (void *)GlobInterface;
 }
}

static boolean hs_index(This, Interface, argc, argv)
c_RemoteServer *This;
c_Interface *Interface;
int argc;
char **argv;
{
 Xc_TRACE(("INDEX command: %s", argv[0]));
  
 if (!Interface->HelpManager->index_exist || 
     (Interface->HelpManager->index_exist &&
      !Xstrcmp(Interface->HelpManager->index_widget->url_filename, argv[0])))
 {
  F(Interface->HelpManager).open_index(Interface->HelpManager, argv[0]); 
  if (Interface->ButtonMaskIndex == FALSE)
   DisplayIndex(WIDGET(Interface->HelpWIndex), Interface);
 }
 return TRUE;
}

static boolean hs_url(This, Interface, argc, argv)
c_RemoteServer *This;
c_Interface *Interface;
int argc;
char **argv;
{
 Xc_TRACE(("URL command: %s", argv[0]));
  
 F(Interface->HelpManager).open_help(Interface->HelpManager, argv[0]);
 return TRUE;
}

static boolean hs_raise(This, Interface, argc, argv)
c_RemoteServer *This;
c_Interface *Interface;
int argc;
char **argv;
{
 Window window;
  
 Xc_TRACE(("RAISE command"));
  
 window =  XtWindow(XtParent(Interface->MainInterface->w_This));
 XRaiseWindow(This->display, window);
 return TRUE;
}

static boolean hs_exit(This, Interface, argc, argv)
c_RemoteServer *This;
c_Interface *Interface;
int argc;
char **argv;
{
 XEvent event;
  
 Xc_TRACE(("EXIT command"));
  
 if (argc == 1)
 {
  while(Interface)
  {
   if (Interface->HelpManager->index_exist &&
       Xstrcmp(Interface->HelpWIndex->url_filename, argv[0]))
    break;
   Interface = Interface->Next;
  }
  if (!Interface) return TRUE;
 }
  
 event.type = ClientMessage;
 event.xclient.send_event = True;
 event.xclient.display = This->display;
 event.xclient.window = XtWindow(WIDGET(Interface->MainInterface));
 event.xclient.format = 32;
 event.xclient.data.l[0] = 0xdeaddddd;
 XSendEvent(This->display, XtWindow(WIDGET(Interface->MainInterface)),
	    True, 0, &event);
  
 return TRUE;
}

#endif /* _RS_xmayday_h_ */
