/*
** RegisterHelp.c for Xclamation, XQuad, XAllWrite and XMayday in RegisterHelp/
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
** Started on  Mon Jul 17 15:24:07 1995 Emmanuel Paris
** Last update Thu Jan  6 16:51:13 2000 Emmanuel Paris
*/

#define GLOBREGHELP

#include "RegisterHelp.h"
#include "Resource.h"
#include "Cursor.h"
#include "file.h"
#include "AlertBox.h"
#ifdef Xc_XMAYDAY
#include "Interface.h"

extern c_Interface *GlobInterface;
extern void DisplayIndex();
#else
#include "MainInterface.h"

extern c_MainInterface *MainInterface1;
#endif

extern c_Resource *GlobResources;

static void *cons_RegisterHelp();
static void dest_RegisterHelp();
static void *copy_RegisterHelp();
static void register_help();
static void call_help();
static void call_help_for_widget();
static void tracking_help();
static boolean read_config();
static boolean write_config();

static void xt_convert_string_to_xpointer();
static void xt_help_callback();
#ifndef Xc_XMAYDAY
static boolean xmayday_already_active();
static boolean netscape_already_active();

extern void xmayday_remote_init_atoms();
extern void xmayday_remote_reset();
extern Window xmayday_remote_find_window();
extern boolean xmayday_remote_obtain_lock();
extern boolean xmayday_remote_free_lock();
extern boolean xmayday_remote_command();

extern void mozilla_remote_init_atoms();
extern void mozilla_remote_reset();
extern Window mozilla_remote_find_window();
extern boolean mozilla_remote_obtain_lock();
extern boolean mozilla_remote_free_lock();
extern boolean mozilla_remote_command();
#endif

sf_RegisterHelp fc_RegisterHelp =
{
 cons_RegisterHelp,
 dest_RegisterHelp,
 copy_RegisterHelp,
 register_help,
 call_help,
 call_help_for_widget,
 tracking_help,
 read_config,
 write_config
};

static void *cons_RegisterHelp(display)
Display		 *display;
{
 c_RegisterHelp	*This;
 XtConvertArgRec	converter_arg;
 char			*env_dir;
  
 if ((This = (c_RegisterHelp *)Xc_malloc("RegisterHelp",
					 sizeof(c_RegisterHelp))) == NULL)
  Xc_FATAL(("Can't build this object: memory allocation error"));
  
 This->f = &fc_RegisterHelp;
 This->display = display;
  
 This->reg_data = NULL;
 if ((env_dir = getenv(Xc_ENVIRONNEMENT)) == NULL)
  env_dir = getenv(Xc_ENVIRONNEMENT_GLOBAL);
 This->base_location = Xc_malloc("bl", strlen(env_dir) + 
				 strlen(Xc_SOFTWARE_NAME) + 7);
 sprintf(This->base_location, "%s/doc/%s/", env_dir, Xc_SOFTWARE_NAME);
  
#ifdef Xc_XMAYDAY
 This->command_list = Xc_malloc("cmd init", sizeof(help_cmd_t));
 This->command_list->command = NULL;
 This->command_list->Next = NULL;
 This->command_list->protocol = XcRH_LOCAL_PROTOCOL;
#else
 This->command_list = NULL;
#endif
 This->help_exist = FALSE;
  
 converter_arg.address_mode = XtAddress;
 converter_arg.address_id = (void *)This;
 converter_arg.size = sizeof(void *);
  
 XtAddConverter(XtRString, XtRPointer, xt_convert_string_to_xpointer,
		&converter_arg, 1);
  
#ifndef Xc_XMAYDAY
 This->netscape_open = This->xmayday_open = FALSE;
 xmayday_remote_init_atoms(display);
 mozilla_remote_init_atoms(display);
#endif
  
 Xc_TRACE(("Object RegisterHelp build"));
 return This;
}

static void dest_RegisterHelp(This)
c_RegisterHelp *This;
{
 reg_help_t	*reg1, *reg2;
 help_cmd_t	*cmd_list, *cmd_list2;
  
#ifndef Xc_XMAYDAY
 if (This->xmayday_open)
 {
  if (xmayday_already_active(This))
  {
   Xc_TRACE(("A XMayday process detected. Try to close it"));
   XSelectInput (This->display, This->xmayday_window, 
		 (PropertyChangeMask|StructureNotifyMask));
   if (xmayday_remote_obtain_lock(This->display, This->xmayday_window))
   {
    boolean status;
    char	*xmayday_cmd;
	
    xmayday_cmd = Xc_malloc("xhlp cmd", strlen(This->base_location) + 16);
	
    sprintf(xmayday_cmd, "EXIT=%sindex.html", This->base_location);
	
    status = xmayday_remote_command(This->display, This->xmayday_window,
				    xmayday_cmd, FALSE);
    Xc_free(xmayday_cmd);
    xmayday_remote_free_lock(This->display, This->xmayday_window);
   }
  }
 }
  
 if (This->netscape_open)
 {
  XFlush(This->display); 
  if (netscape_already_active(This))
  {
   XFlush(This->display); 
   Xc_TRACE(("A netcape process detected. Try to close it"));
   XSelectInput (This->display, This->netscape_window, 
		 (PropertyChangeMask|StructureNotifyMask));
      
   if (mozilla_remote_obtain_lock(This->display, This->netscape_window))
   {
    boolean status;
    char	*netscape_cmd;
	
    netscape_cmd = Xc_malloc("moz cmd", 10);
    sprintf(netscape_cmd, "exit()");
    status = mozilla_remote_command(This->display, This->netscape_window,
				    netscape_cmd, TRUE, FALSE);
    Xc_free(netscape_cmd);
   }
  }
 }
  
 xmayday_remote_reset();
 mozilla_remote_reset();
#endif
  
 reg1 = This->reg_data;
 while(reg1)
 {
  reg2 = reg1->Next;
  if (reg1->help_filename)
   Xc_free(reg1->help_filename);
  Xc_free(reg1);
  reg1 = reg2;
 }
  
 cmd_list = This->command_list;
 while(cmd_list)
 {
  cmd_list2 = cmd_list->Next;
  if (cmd_list->command) 
   Xc_free(cmd_list->command);
  Xc_free(cmd_list);
  cmd_list = cmd_list2;
 }
  
 Xc_free(This->base_location);
 Xc_free(This);
 Xc_TRACE(("Object RegisterHelp destroyed"));
}

static void *copy_RegisterHelp(This)
c_RegisterHelp *This;
{
 Xc_ERROR(("Object RegisterHelp not copied, not usefull"));
 return NULL;
}

static void register_help(This, w)
c_RegisterHelp	*This;
Widget	w;
{
 XtAddCallback(w, XmNhelpCallback, (void *)xt_help_callback, (caddr_t)This);  
}

static void call_help(This, filename)
c_RegisterHelp	*This;
char *filename;
{
 char	*full_filename, *index_filename, *diese_pos;
 help_cmd_t	*cmd_list;
 boolean	finish = FALSE;
 char		command_line[800];
 int		_err;
#ifndef Xc_XMAYDAY
 int		height;
#endif
 
 full_filename = Xc_malloc("fulfn", strlen(filename) +
			   strlen(This->base_location) + 1);
 strcpy(full_filename, This->base_location);
 strcat(full_filename, filename);
 Xc_TRACE(("HERE must call help with filename: %s", full_filename));
 
 diese_pos = strrchr(full_filename, '#');
 if (diese_pos)
  *diese_pos = '\0';
 
 if (!IsFileExist(full_filename))
 {
  if (!This->help_exist)
  {
   XmString xms;
   char tmp_string[300];

   SET_WAIT_MODE_STATIC;
   sprintf(tmp_string, F(GlobResources).getString(GlobResources,
						  XcR_helpNotFound),
	   This->base_location);

   xms = XmStringCreateLtoR(tmp_string, XmSTRING_DEFAULT_CHARSET);
   Xc_ErrorAlert2(xms);
   XmStringFree(xms);
   UNSET_WAIT_MODE_STATIC;
   Xc_free(full_filename);
   return;
  }
  else
  {
   char	*error_msg;
      
   error_msg = (char *)Xc_malloc("errmsg",
				 strlen(F(GlobResources).getString
					(GlobResources,
					 XcR_loadingError)) +
				 strlen(full_filename) + 1);
      
   sprintf(error_msg, F(GlobResources).getString
	   (GlobResources, XcR_loadingError), full_filename);
   SET_WAIT_MODE_STATIC;
   Xc_ErrorAlert(error_msg);
   UNSET_WAIT_MODE_STATIC;
   Xc_free(error_msg);
   Xc_free(full_filename);
   return;
  }
 }
 if (diese_pos)
  *diese_pos = '#';
 
 cmd_list = This->command_list;
 while(cmd_list)
 {
  switch(cmd_list->protocol)
  {
  case XcRH_NO_PROTOCOL:
      
   if (cmd_list->command)
   {
    sprintf(command_line, "%s %s &", cmd_list->command, full_filename);
    _err = system(command_line);
    if (_err != -1 && _err != 127)
     finish = TRUE;
   }
   break;
#ifndef Xc_XMAYDAY
  case XcRH_XMAYDAY_PROTOCOL:
   if (xmayday_already_active(This))
   {
    Xc_TRACE(("A XMayday process detected. Try to use it"));
    XSelectInput (This->display, This->xmayday_window, 
		  (PropertyChangeMask|StructureNotifyMask));
    if (xmayday_remote_obtain_lock(This->display, This->xmayday_window))
    {
     boolean status;
     char	*xmayday_cmd;
	  
     xmayday_cmd = Xc_malloc("xhlp cmd", strlen(This->base_location) + 
			     strlen(full_filename) + 28);
     sprintf(xmayday_cmd, "INDEX=%sindex.html;URL=%s;RAISE",
	     This->base_location, full_filename);
	  
     status = xmayday_remote_command(This->display, This->xmayday_window,
				     xmayday_cmd, TRUE);
     Xc_free(xmayday_cmd);
     xmayday_remote_free_lock(This->display, This->xmayday_window);
	  
     if (status)
     {
      finish = TRUE;
      This->xmayday_open = TRUE;
      break;
     }
    }
    break;
   }
   if (cmd_list->command)
   {
    index_filename = Xc_malloc("idfn", strlen(This->base_location) + 11);
    strcpy(index_filename, This->base_location);
    strcat(index_filename, "index.html");
	
    height = DisplayHeight(This->display, DefaultScreen(This->display));
	
    if (MainInterface1->display_name && *MainInterface1->display_name)
     sprintf(command_line, "%s -display %s -geom x%d-20+50 -index %s %s 2> /dev/null &",
	     cmd_list->command,  MainInterface1->display_name, 
	     height - 300, index_filename,
	     full_filename); /* XMayday Toplevel minHeight = 200 */
    else
     sprintf(command_line, "%s -geom x%d-20+50 -index %s %s 2> /dev/null &",
	     cmd_list->command,  height - 100, index_filename,
	     full_filename);
    
    _err = system(command_line);
    if (_err != -1 && _err != 127)
    {
     This->xmayday_open = TRUE;
     finish = TRUE;
    }
#ifndef NTRACE
    else
    {
     Xc_TRACE(("XMayday call failed with error code %d", _err));
    }
#endif
    Xc_free(index_filename);
   }
   
   break;
  case XcRH_NETSCAPE_PROTOCOL:
   if (netscape_already_active(This))
   {
    Xc_TRACE(("A netcape process detected. Try to use it"));
    XSelectInput (This->display, This->netscape_window, 
		  (PropertyChangeMask|StructureNotifyMask));
	
    if (mozilla_remote_obtain_lock(This->display, This->netscape_window))
    {
     boolean status;
     char	*netscape_cmd;
	  
     netscape_cmd = Xc_malloc("moz cmd", strlen(full_filename) + 10);
     sprintf(netscape_cmd, "openURL(%s)", full_filename);
     status = mozilla_remote_command(This->display, This->netscape_window,
				     netscape_cmd, TRUE, TRUE);
     Xc_free(netscape_cmd);
     mozilla_remote_free_lock(This->display, This->netscape_window);
	  
     if (status)
     {
      finish = TRUE;
      break;
     }
    }
   }
   if (cmd_list->command)
   {
    height = DisplayHeight(This->display, DefaultScreen(This->display));
	
    if (MainInterface1->display_name && *MainInterface1->display_name)
     sprintf(command_line, "%s -display %s -geometry x%d-20+50 %s &",
	     cmd_list->command, MainInterface1->display_name,
	     height - 100, full_filename);
    else
     sprintf(command_line, "%s -geometry x%d-20+50 %s &",
	     cmd_list->command, height - 100, full_filename);
    _err = system(command_line);
    if (_err != -1 && _err != 127)
    {
     This->netscape_open = TRUE;
     finish = TRUE;
    }
   }
  case XcRH_LOCAL_PROTOCOL:
   break;
  }
#else  
  case XcRH_XMAYDAY_PROTOCOL:
  case XcRH_NETSCAPE_PROTOCOL:
   break;
  case XcRH_LOCAL_PROTOCOL:
   {
    c_Interface *Interface = NULL, *RInterface = GlobInterface;
       
    index_filename = Xc_malloc("idxfn", strlen(This->base_location) + 11);
    strcpy(index_filename, This->base_location);
    strcat(index_filename, "index.html");
       
    Interface = GlobInterface;
    while(Interface)
    {
     if ((Interface->HelpManager->index_exist &&
	  Xstrcmp(Interface->HelpWIndex->url_filename, index_filename)) ||
	 (!Interface->HelpManager->index_exist && 
	  !Interface->HelpManager->help_exist))
      break;
     Interface = Interface->Next;
    }
    if (Interface)
     RInterface = Interface;
    else
     RInterface = (c_Interface *)COPY(c_Interface)(RInterface);

    F(RInterface->HelpManager).open_index(RInterface->HelpManager,
					  index_filename);
    if (RInterface->ButtonMaskIndex == FALSE)
     DisplayIndex(WIDGET(RInterface->HelpWIndex), RInterface);

    F(RInterface->HelpManager).open_help(RInterface->HelpManager,
					 full_filename);

    Xc_free(index_filename);
    finish = TRUE;
    break;
   }
  }
#endif
     
  if (finish) break;
  cmd_list = cmd_list->Next;
 }
 if (!cmd_list)
 {
    
  Xc_WARNING(("Can not display Help message for %s. No valid browser found.",
	      full_filename));
  SET_WAIT_MODE_STATIC;
  Xc_ErrorAlert("No browser available \nto display on-line help.");
  UNSET_WAIT_MODE_STATIC;
 }
  
 Xc_free(full_filename);
}

static void call_help_for_widget(This, w)
c_RegisterHelp	*This;
Widget	w;
{
 reg_help_t	*reg;
 char		*filename;
 Arg		argsw[1];

 while(w && !XtIsTopLevelShell(w))
 {
  XtSetArg(argsw[0], XmNuserData, (XPointer *)&filename);
  XtGetValues(w, argsw, 1);
  
  reg = This->reg_data;
  while(reg && reg->help_filename != filename)
   reg = reg->Next;
  
  if (reg)
  {
   call_help(This, filename);
   return;
  }
  w = XtParent(w);
 }
 Xc_TRACE(("NO HELP registered at all"));
}

static void tracking_help(This, w)
c_RegisterHelp	*This;
Widget	w;
{
 Widget w2;
 XEvent event;
 reg_help_t	 *reg;
 char		*filename;
 Arg		argsw[1];
  
 w2 = (Widget)XmTrackingEvent(w, GlobCursor->cursor_x[C_HELP_ASK],
			      True, &event);
  
 if (!w2)
  return;
  
 while(w2 != w)
 {
  XtSetArg(argsw[0], XmNuserData, (XPointer *)&filename);
  XtGetValues(w2, argsw, 1);
    
  reg = This->reg_data;
  while(reg && reg->help_filename != filename)
   reg = reg->Next;
    
  if (reg)
  {
   call_help(This, filename);
   return;
  }
  w2 = XtParent(w2);
 }
 Xc_TRACE(("NO HELP registered at all"));
}

/* -------------------------------------------------------------------- **
** read and write config functions					**
** -------------------------------------------------------------------- */

static boolean cb_base_location();
static boolean cb_help_command();
static boolean cb_command();
static boolean cb_xmayday_proto();
static boolean cb_netscape_proto();

static boolean read_config(doc, keyword, param, This)
c_DocFile *doc;
char *keyword;
long param;
c_RegisterHelp *This;
{
 if( !F(doc).addCallbacks
    (doc,
     XcDF_HELP_LOCATION_KEYWORD, cb_base_location, This,
     XcDF_HELP_COMMAND_KEYWORD, cb_help_command, This,
     NULL)) 
  return FALSE;
  
 if(!F(doc).expectKeyword(doc, NULL, TRUE))
  return FALSE;
  
 {
  help_cmd_t	*cmd_list;
    
  cmd_list = This->command_list;
  while(cmd_list)
  {
   Xc_TRACE(("command: %s proto: %d", (cmd_list->command ? cmd_list->command :
				       ""), cmd_list->protocol));
   cmd_list = cmd_list->Next;
  }
 }
  
 Xc_TRACE(("base location = %s", This->base_location));
  
 if (IsDirExist(This->base_location))
 {
  char	*index;
    
  index = Xc_malloc("index", strlen(This->base_location) + 12);
  sprintf(index, "%sindex.html", This->base_location);
    
  if (IsFileExist(index))
   This->help_exist = TRUE;
    
  Xc_free(index);
 }
  
 return TRUE;
}


static boolean write_config(This, doc)
c_RegisterHelp *This;
c_DocFile *doc;
{
 /*  if(!F(doc).writeKeyword(doc, XcDF_MEMORY_CACHE_SIZE_KEYWORD,
     This->max_size_in_memory))
     return FALSE; */
  
  
 /*  if (This->disk_cache_available && 
     (!F(doc).writeKeyword(doc, XcDF_CACHE_DIR_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).write(doc, 
     This->cache_directory, strlen(This->cache_directory)) ||
     !F(doc).endSequence(doc)))
     return FALSE; */
  
 return TRUE;
}

static boolean cb_base_location(doc, keyword, param, This)
c_DocFile *doc;
char *keyword;
long param;
c_RegisterHelp *This;
{
 unsigned char *name;
 char *home_dir, *new_name;
 char envv[100];
 int i;
 
 if (!F(doc).readString(doc, &name)) 
  return FALSE;
 if (name != NULL)
 {
  Xc_free(This->base_location);
  if ( *name == '~')
  {
   home_dir = getenv("HOME");
   if (home_dir == NULL) 
    home_dir = "/";
   new_name = Xc_malloc("base loc", strlen(name) + strlen(home_dir) + 1);
   sprintf(new_name, "%s%s", home_dir, name+1);
   Xc_free(name);
   name = (unsigned char *)new_name;
  }
   
  if (*name == '$')
  {
   i = 1;
   while(name[i] && name[i] != '/')
   {
    envv[i-1] = name[i];
    i++;
   }
   envv[i-1] = '\0';
   home_dir = getenv(envv);
   if (home_dir == NULL)
    home_dir = "/";
   new_name = Xc_malloc("base loc", strlen(name+i) + strlen(home_dir) + 1);
   sprintf(new_name, "%s%s", home_dir, name+i);
   Xc_free(name);
   name = (unsigned char *)new_name;
  }
   
  if (name[strlen(name)-1] != '/')
  {
   name = Xc_realloc("base loc", name, strlen(name) + 2);
   strcat(name, "/");
  }
   
  This->base_location = (char *)name;
 }
 return TRUE;
}

static boolean cb_help_command(doc, keyword, param, This)
c_DocFile *doc;
char *keyword;
long param;
c_RegisterHelp *This;
{
 help_cmd_t	*cmd_list;
  
 cmd_list = This->command_list;
 if (!cmd_list)
 {
  This->command_list = cmd_list =
   (help_cmd_t *)Xc_malloc("hc", sizeof(help_cmd_t));
 }
 else
 {
  while(cmd_list->Next)
   cmd_list = cmd_list->Next;
  cmd_list->Next = (help_cmd_t *)Xc_malloc("hc", sizeof(help_cmd_t));
  cmd_list = cmd_list->Next;
 }
 cmd_list->Next = NULL;
 cmd_list->command = NULL;
 cmd_list->protocol = XcRH_NO_PROTOCOL;
 
 if( !F(doc).addCallbacks
    (doc, 
     XcDF_COMMAND_KEYWORD, cb_command, cmd_list,
     XcDF_HELP_XMAYDAY_PROTOCOL_KEYWORD, cb_xmayday_proto, cmd_list,
     XcDF_HELP_NETSCAPE_PROTOCOL_KEYWORD, cb_netscape_proto, cmd_list,
     NULL)) 
  return FALSE;
 
 if(!F(doc).expectKeyword(doc, NULL, TRUE))
  return FALSE;
 
 return TRUE;
}

static boolean cb_command(doc, keyword, param, cmd_list)
c_DocFile *doc;
char *keyword;
long param;
help_cmd_t *cmd_list;
{
 unsigned char	*name;
  
 if(!F(doc).readString(doc, &name))
  return FALSE;

 cmd_list->command = SearchFile((char *)name);
 Xc_free(name);
 return TRUE;
}

static boolean cb_xmayday_proto(doc, keyword, param, cmd_list)
c_DocFile *doc;
char *keyword;
long param;
help_cmd_t *cmd_list;
{
 cmd_list->protocol = XcRH_XMAYDAY_PROTOCOL;
 return TRUE;
}

static boolean cb_netscape_proto(doc, keyword, param, cmd_list)
c_DocFile *doc;
char *keyword;
long param;
help_cmd_t *cmd_list;
{
 cmd_list->protocol = XcRH_NETSCAPE_PROTOCOL;
 return TRUE;
}

/* -------------------------------------------------------------------- **
** callbacks and internal private functions				**
** -------------------------------------------------------------------- */

static void xt_convert_string_to_xpointer(args, nargs, source, target)
XrmValue *args;
int *nargs;
XrmValue *source;
XrmValue *target;
{ 
 c_RegisterHelp *This;
 reg_help_t	 *reg;
  
 if (*nargs == 1)
  This = (c_RegisterHelp *)args->addr;
 else
  return;
  
 if (source->addr && source->size > sizeof(int))
 {
  reg = (reg_help_t *)Xc_malloc("reghlp", sizeof(reg_help_t));
  reg->help_filename = Xc_strdup("regdup", source->addr);
  reg->Next = This->reg_data;
  This->reg_data = reg;
    
  target->addr = (caddr_t)&reg->help_filename;
  target->size = sizeof(char *);
 }
 else
 {
  XtStringConversionWarning("Null", "String");
 }
}

static void xt_help_callback(w, This, cbs)
Widget w;
c_RegisterHelp *This;
XmAnyCallbackStruct *cbs;
{
 Widget	wshell;
  
 Xc_TRACE(("help callback"));
  
 if (cbs->event->type == KeyPress && 
     (cbs->event->xkey.state & ControlMask) != 0)
 {
  Xc_TRACE(("action help callback"));
  wshell = w;
  while(wshell && !XtIsShell(wshell))
   wshell = XtParent(wshell);
    
  if (wshell)
   F(This).tracking_help(This, wshell);
 }
 else
 {
  F(This).call_help_for_widget(This, w);
 }
}

/* -------------------------------------------------------------------- **
** functions to test if browser already open				**
** -------------------------------------------------------------------- */
#ifndef Xc_XMAYDAY
static boolean xmayday_already_active(This)
c_RegisterHelp *This;
{
 This->xmayday_window = xmayday_remote_find_window(This->display);
 return (This->xmayday_window != (Window)0);
}

static boolean netscape_already_active(This)
c_RegisterHelp *This;
{
 This->netscape_window = mozilla_remote_find_window(This->display);
 return (This->netscape_window != (Window)0);
}
#endif
