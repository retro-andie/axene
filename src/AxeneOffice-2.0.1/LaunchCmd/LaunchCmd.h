/*
** LaunchCmd.h for AxeneOffice in LaunchCmd/
** Launch an Axene software or raise it
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
** Started on  Thu Jan 15 12:25:03 1998 Emmanuel Paris
** Last update Fri Jan 16 12:17:47 1998 Emmanuel Paris
*/

#ifndef _LaunchCmd_h_
#define _LaunchCmd_h_

#include "xcalibur.h"
#include "Interface.h"
#include <X11/Intrinsic.h>

typedef struct sc_LaunchCmd c_LaunchCmd;

typedef struct 
{
 int index;
 char *soft_name;
 char *file_name;
 boolean exist;
} launch_info_t;

/* define the method for Class LaunchCmd */
typedef struct
{
  F_STD;
  void (*launch_command) ___PROTO((c_LaunchCmd *This, launch_info_t *launch));
} sf_LaunchCmd;

/* define the LaunchCmd Class */
struct sc_LaunchCmd
{
  sf_LaunchCmd		*f;
  Display		*display;
  char			*display_name;
};

extern sf_LaunchCmd fc_LaunchCmd;

#endif /* _LaunchCmd_h_ */
