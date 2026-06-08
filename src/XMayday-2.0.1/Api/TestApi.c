/*
** TestApi.c for XMayday in Api/
** TestApi.c
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

#include "XMayday.h"
#include "XMaydayCom.h"
#include <X11/Xlib.h>

#define INDEX_FILENAME "index.html"
#define PAGE1_FILENAME "page1.html"
#define PAGE2_FILENAME "page2.html"
		  
/*--- File prototypes ---*/
extern char *GetWorkingDirectory ___PROTO((void));
extern boolean IsFileExist ___PROTO((char *filename));
extern boolean IsFileExecutable ___PROTO((char *filename));
extern char *SearchFile ___PROTO((char *filename));

int main(argc, argv)
int argc;
char **argv;
{
 Display *display;
 Window	window;
 char	*command;
 char	command_line[800];
 char	*base_location = NULL;
 char	*index_filename;
 char	*page_filename;
 int	height;
 int	_err;

 display = XOpenDisplay(NULL);
 if (display == NULL)
  Xc_FATAL(("Can not open display."));
 
 Xc_TRACE(("Initialize XMayday Atoms."));
 xmayday_remote_init_atoms(display);

 Xc_TRACE(("Test if an XMayday Window is already open."));
 window = xmayday_remote_find_window(display);
 if (window != (Window)0)
 {
  Xc_TRACE(("XMayday Window detected (id: 0x%x).", window));
 
  XSelectInput(display, window, (PropertyChangeMask|StructureNotifyMask));
  Xc_TRACE(("Try to otain a lock"));
  if (xmayday_remote_obtain_lock(display, window))
  {
   boolean status;
   char	*xmayday_cmd;
   
   base_location = GetWorkingDirectory();

   xmayday_cmd = malloc(strlen(base_location) + strlen(INDEX_FILENAME) +
			strlen(PAGE2_FILENAME) + 27);
   sprintf(xmayday_cmd, "INDEX=%s/%s;URL=%s;RAISE",
	   base_location, INDEX_FILENAME, PAGE2_FILENAME);
   
   Xc_TRACE(("Send remote command to XMayday Window:\n%s", xmayday_cmd));
   status = xmayday_remote_command(display, window, xmayday_cmd, TRUE);
   free(xmayday_cmd);
   xmayday_remote_free_lock(display, window);
   
   if (status)
    Xc_TRACE(("Remote command succeed."));
   else
    Xc_TRACE(("Remote command failed."));
    
   free(base_location);
  }
  else
   Xc_FATAL(("Can not obtain a lock."));
 }
 else
 {
  Xc_TRACE(("XMayday Window not detected."));
  
  Xc_TRACE(("Try to find XMayday executable file."));
  
  command = (char *)SearchFile("XMayday");
  if (command == NULL)
   Xc_FATAL(("XMayday executable file not found."));
  
  Xc_TRACE(("XMayday found: %s", command));

  base_location = GetWorkingDirectory();
  index_filename = malloc(strlen(base_location) + strlen(INDEX_FILENAME) + 1);
  sprintf(index_filename, "%s/%s", base_location, INDEX_FILENAME);
  
  page_filename = PAGE1_FILENAME;
  
  height = DisplayHeight(display, DefaultScreen(display));
  sprintf(command_line, "%s -geom x%d-20+50 -index %s %s&",
	  command, height - 100, index_filename, page_filename);
  
  Xc_TRACE(("Launch XMayday with command line: \n%s", command_line));
  _err = system(command_line);
  if (!(_err != -1 && _err != 127))
  {
   Xc_FATAL(("An error occured while launching XMayday"));
  }

  Xc_TRACE(("Ok XMayday is launched.\nPlease rerun this programme\
 to test On-line communication."));
  free(index_filename);
  free(base_location);
  free(command);
 }

 xmayday_remote_reset();
 return 0;
}


