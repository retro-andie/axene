/*
** RemoteServer.h for Xclamation in RemoteServer/
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
** Started on  Mon Jul 17 17:46:15 1995 Emmanuel Paris
** Last update Wed Jan 14 12:19:00 1998 Emmanuel Paris
*/

#ifndef _RemoteServer_h
#define _RemoteServer_h

#include "xcalibur.h"
#include <X11/Xlib.h>

typedef struct sc_RemoteServer c_RemoteServer;

typedef struct 
{
  char	*cmd_name;
  int	cmd_arg;
  boolean (*cmd_func)();
} cmd_parse_t;

/* defint the method for Class RemoteServer */
typedef struct
{
  F_STD;
  boolean (*eventlook) ___PROTO((c_RemoteServer *This, XEvent *event));
} sf_RemoteServer;

/* define the RemoteServer Class */
struct sc_RemoteServer
{
  sf_RemoteServer	*f;
  
  Display	*display;
  Window	window;
};

extern sf_RemoteServer fc_RemoteServer;

#endif




