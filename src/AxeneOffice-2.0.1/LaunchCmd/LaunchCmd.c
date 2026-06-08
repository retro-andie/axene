/*
** LaunchCmd.c for AxeneOffice in LaunchCmd/
** Launch an Office soft or raise it.
**
** Copyright (C) 1998-2000 Axene.
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
** Started on  Thu Jan 15 12:20:09 1998 Emmanuel Paris
** Last update Sat Jan 17 18:16:17 1998 Emmanuel Paris
*/

#include "LaunchCmd.h"
#define VROOT_STATIC
#include "RH_vroot.h"

extern c_Interface *GlobInterface;

extern void xclamation_remote_init_atoms();
extern void xclamation_remote_reset();
extern Window xclamation_remote_find_window();
extern boolean xclamation_remote_obtain_lock();
extern boolean xclamation_remote_free_lock();
extern boolean xclamation_remote_command();

extern void xquad_remote_init_atoms();
extern void xquad_remote_reset();
extern Window xquad_remote_find_window();
extern boolean xquad_remote_obtain_lock();
extern boolean xquad_remote_free_lock();
extern boolean xquad_remote_command();

extern void xallwrite_remote_init_atoms();
extern void xallwrite_remote_reset();
extern Window xallwrite_remote_find_window();
extern boolean xallwrite_remote_obtain_lock();
extern boolean xallwrite_remote_free_lock();
extern boolean xallwrite_remote_command();

extern void xmayday_remote_init_atoms();
extern void xmayday_remote_reset();
extern Window xmayday_remote_find_window();
extern boolean xmayday_remote_obtain_lock();
extern boolean xmayday_remote_free_lock();
extern boolean xmayday_remote_command();

typedef struct
{
 void (*remote_init_atoms)();
 void (*remote_reset)();
 Window (*remote_find_window)();
 boolean (*remote_obtain_lock)();
 boolean (*remote_free_lock)();
 boolean (*remote_command)();
} func_remote_t;

static func_remote_t func_remote[NB_SOFT] = 
{
 { 
  xclamation_remote_init_atoms, xclamation_remote_reset, 
  xclamation_remote_find_window, xclamation_remote_obtain_lock,
  xclamation_remote_free_lock, xclamation_remote_command
 },
 { 
  xquad_remote_init_atoms, xquad_remote_reset, 
  xquad_remote_find_window, xquad_remote_obtain_lock,
  xquad_remote_free_lock, xquad_remote_command
 },
 { 
  xallwrite_remote_init_atoms, xallwrite_remote_reset, 
  xallwrite_remote_find_window, xallwrite_remote_obtain_lock,
  xallwrite_remote_free_lock, xallwrite_remote_command
 },
 { 
  xmayday_remote_init_atoms, xmayday_remote_reset, 
  xmayday_remote_find_window, xmayday_remote_obtain_lock,
  xmayday_remote_free_lock, xmayday_remote_command
 },
};

static void *cons_LaunchCmd();
static void dest_LaunchCmd();
static void *copy_LaunchCmd();
static void launch_command ___PROTO((c_LaunchCmd *This, 
				     launch_info_t *launch));

sf_LaunchCmd fc_LaunchCmd =
{
 cons_LaunchCmd,
 dest_LaunchCmd,
 copy_LaunchCmd,
 launch_command
};

static void *cons_LaunchCmd(display, display_name)
Display		*display;
char		*display_name;
{
 c_LaunchCmd	*This;
 int i;

 if ((This = (c_LaunchCmd *)Xc_malloc("LaunchCmd",
					 sizeof(c_LaunchCmd))) == NULL)
  Xc_FATAL(("Can't build this object: memory allocation error"));
  
 This->f = &fc_LaunchCmd;
 This->display = display;
 This->display_name = Xc_strdup("dn", display_name);

 for(i = 0; i < NB_SOFT; i++)
  func_remote[i].remote_init_atoms(display);

 Xc_TRACE(("Object LaunchCmd build"));
 return This;
}

static void dest_LaunchCmd(This)
c_LaunchCmd *This;
{
 int i;
 
 for(i = 0; i < NB_SOFT; i++)
  func_remote[i].remote_reset();

 Xc_free(This->display_name);
 Xc_free(This);
 Xc_TRACE(("Object LaunchCmd destroyed"));
}

static void *copy_LaunchCmd(This)
c_LaunchCmd *This;
{
 Xc_ERROR(("Object LaunchCmd not copied, not usefull"));
 return NULL;
}

static void launch_command(This, launch)
c_LaunchCmd *This;
launch_info_t *launch;
{
 Display *display = This->display;
 Window window;
 char *command_line;
 int softn = launch->index;
 int _err;

 Xc_TRACE(("Launch %s\n", launch->soft_name));
 
 window = func_remote[softn].remote_find_window(display);
 if (window != (Window)0)
 {
  Xc_TRACE(("%s Window detected (id: 0x%x).", launch->soft_name, window));
 
  XSelectInput(display, window, (PropertyChangeMask|StructureNotifyMask));
  Xc_TRACE(("Try to otain a lock"));
  if (func_remote[softn].remote_obtain_lock(display, window))
  {
   boolean status;
   
   Xc_TRACE(("Send remote command RAISE to %s Window.", launch->soft_name));
   status = func_remote[softn].remote_command(display, window, "RAISE", TRUE);

   func_remote[softn].remote_free_lock(display, window);
   
   if (status)
   {
    Xc_TRACE(("Remote command succeed."));
    return;
   }
#ifndef NTRACE
   else
    Xc_TRACE(("Remote command failed."));
#endif
  }
#ifndef NTRACE
  else
   Xc_FATAL(("Can not obtain a lock."));
#endif
 }
 Xc_TRACE(("%s Window not detected.", launch->soft_name));
 
 if (!launch->exist)
 {
  Xc_TRACE(("%s doesn't exist !", launch->soft_name));
  return;
 }

 command_line = (char *)Xc_malloc("cl", strlen(launch->file_name) + 
				  strlen(This->display_name) + 13);
 sprintf(command_line, "%s -display %s &", launch->file_name, 
	 This->display_name);
 
 Xc_TRACE(("Launch %s with command line: \n%s", launch->soft_name, 
	   command_line));
 _err = system(command_line);
#ifndef NTRACE
 if (!(_err != -1 && _err != 127))
  Xc_TRACE(("An error occured while launching %s", launch->soft_name));
 else
  Xc_TRACE(("Ok %s is launched.", launch->soft_name));
#endif
 Xc_free(command_line);
}
