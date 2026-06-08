/*
** RegisterHelp.h for Xclamation, XQuad, XAllWrite and XMayday in RegisterHelp/
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
** Started on  Fri May 10 16:46:22 1996 Emmanuel Paris
** Last update Fri Jan 16 22:18:21 1998 Emmanuel Paris
*/

#ifndef _RegisterHelp_h
#define _RegisterHelp_h

#include "xcalibur.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/XmStrDefs.h>
#include "DocFile.h"

typedef struct sc_RegisterHelp c_RegisterHelp;

extern c_RegisterHelp *GlobRegHelp;

#define Xc_REGISTER_HELP(widget) \
F(GlobRegHelp).register_help(GlobRegHelp, widget);

#define XcDF_HELP_CONFIG_KEYWORD "HELP_CONFIG"
#define XcDF_HELP_LOCATION_KEYWORD "BASE_LOCATION"
#define XcDF_HELP_COMMAND_KEYWORD "HELP_COMMAND"
#ifndef XcDF_COMMAND_KEYWORD
#define XcDF_COMMAND_KEYWORD "COMMAND"
#endif
#define XcDF_HELP_XMAYDAY_PROTOCOL_KEYWORD "XMAYDAY_PROTOCOL"
#define XcDF_HELP_NETSCAPE_PROTOCOL_KEYWORD "NETSCAPE_PROTOCOL"

typedef enum 
{
 XcRH_NO_PROTOCOL,
 XcRH_XMAYDAY_PROTOCOL,
 XcRH_NETSCAPE_PROTOCOL,
 XcRH_LOCAL_PROTOCOL
} help_protocol_t;

typedef struct _help_cmd_t
{
 char			*command;
 help_protocol_t	protocol;
 struct _help_cmd_t	*Next;
} help_cmd_t;

typedef struct _reg_help_t
{
 char	*help_filename;
 struct _reg_help_t *Next;
} reg_help_t;

/* define the method for Class RegisterHelp */
typedef struct
{
 F_STD;
 void (*register_help) ___PROTO((c_RegisterHelp *This, Widget w));
 void (*call_help) ___PROTO((c_RegisterHelp *This, char *filename));
 void (*call_help_for_widget) ___PROTO((c_RegisterHelp *This, Widget w));
 void (*tracking_help) ___PROTO((c_RegisterHelp *This, Widget w));
 boolean (*read_config) ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_RegisterHelp *This));
 boolean (*write_config) ___PROTO((c_RegisterHelp *This, c_DocFile *doc));

} sf_RegisterHelp;

/* define the RegisterHelp Class */
struct sc_RegisterHelp
{
 sf_RegisterHelp	*f;
 Display		*display;
 reg_help_t		*reg_data;
 char			*base_location;
 help_cmd_t		*command_list;

 boolean		help_exist;
 boolean		netscape_open;
 boolean		xmayday_open;
  
 Window		xmayday_window;
 Window		netscape_window;
  
};

extern sf_RegisterHelp fc_RegisterHelp;

#endif










