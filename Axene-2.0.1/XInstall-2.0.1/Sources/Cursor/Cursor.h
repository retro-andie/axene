/*
** Cursor.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Cursor/
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
** Started on  Mon Jul 17 14:20:36 1995 Emmanuel Paris
** Last update Thu Nov 16 12:23:51 1995 Emmanuel Paris
*/

#ifndef _cursor_h_
#define	_cursor_h_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include "xcalibur.h"
#include "Xpm.h"

#define NO_CURSOR		0
#define	STANDARD_CURSOR		1
#define DEFINED_CURSOR_XBM	2
#define DEFINED_CURSOR_XPM	3

#define WAIT_STATIC	FALSE
#define WAIT_ANIMATE	TRUE

typedef	struct
{
  int	type;
  void	*data;
  int	type2;
  void	*data2;
} Cursor_Set;

#include "Cursor_Set.h"

typedef struct _p_Cursor
{
  int			cursor;
  struct _p_Cursor	*NextCursor;
} p_Cursor;

typedef	struct
{
  F_STD;
  void (*set_cursor)();
  int  (*get_cursor)();
  void (*push_cursor)();
  void (*push_and_set_cursor)();
  void (*pop_cursor)();
  void (*flush_cursor)();
  void (*unset_cursor)();
  void (*reset_cursor)();
} sf_Cursor;

typedef	struct
{
  sf_Cursor	*f;
  
  Display	*display;
  Window	window;
  int		current_cursor;
  boolean	cursor_set;
  p_Cursor	*cursor_pile;
} c_Cursor;

extern sf_Cursor fc_Cursor;

/* --------------------------------------------------------------------- */
/* ----                       Global Cursor Object                   --- */
/* --------------------------------------------------------------------- */


#define SET_WAIT_MODE_ANIM F(GlobCursor).set_wait_mode(GlobCursor, \
						       WAIT_ANIMATE)
#define SET_WAIT_MODE_STATIC F(GlobCursor).set_wait_mode(GlobCursor, \
							 WAIT_STATIC)
#define UNSET_WAIT_MODE_ANIM F(GlobCursor).unset_wait_mode(GlobCursor, \
							   WAIT_ANIMATE)
#define UNSET_WAIT_MODE_STATIC F(GlobCursor).unset_wait_mode(GlobCursor, \
							     WAIT_STATIC)
#define UNSET_WAIT_MODE UNSET_WAIT_MODE_STATIC

#define TEST_WAIT_MODE GlobCursor->wait_mode

typedef struct _l_GCursor
{
  c_Cursor		*cursor;
  boolean		cursor_reset;
  struct _l_GCursor	*NextCursor;
} l_GCursor;

typedef struct	
{
  int	width;
  int	height;
  int	x_hot;
  int	y_hot;
  char	*csource;
  char	*cmask;
} defined_cursor;

typedef struct 
{
  F_STD;
  void (*add_cursor)();
  void (*del_cursor)();
  void (*change_cursor_set)();
  void (*enable_wait_mode)();
  void (*set_wait_mode)();
  void (*unset_wait_mode)();
} sf_GCursor;	

typedef struct
{
  sf_GCursor	*f;
  Display	*display;
  Cursor_Set	cursor_defs[NB_CURSOR];
  Cursor	cursor_x[NB_CURSOR];
  l_GCursor	*ListCursor;
  c_Cursor	*root_cursor;
  boolean	wait_mode;
  boolean	wait_once;
  boolean	wait_style;
  int		wait_cursor_set;
  int		wait_stack_static;
  int		wait_stack_animate;
} c_GCursor;

extern sf_GCursor fc_GCursor;
extern c_GCursor *GlobCursor;

#endif




