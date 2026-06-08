/*
** ScrollBarX.h for Xclamation in ScrollBarX/
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
** Started on  Tue Jul 18 12:22:45 1995 Antoine Buat
** Last update Mon Dec 29 17:03:18 1997 Emmanuel Paris
*/

#ifndef _ScrollBarX_h
#define _ScrollBarX_h

#include "xcalibur.h"
#include <X11/Intrinsic.h>


/* define the method for Class ScrollBarX */
typedef struct
{
  F_STD;
  int	(*Move)();
  int	(*AMove)();
  void  (*RequestMove)();
  void	(*Resize)();
  void	(*Draw)();
  boolean (*EventLook)();
  void  (*Change_Rsize)();
  void  (*Set_Callback)();
  void  (*Unset_Callback)();
} sf_ScrollBarX;


/* define the ScrollBarX Class */
typedef struct
{
 sf_ScrollBarX	*f;
 Display	*display;
 Window		window;
 
 GC		gc_highlight;
 GC		gc_normal;
 GC		gc_shadow;
 GC		gc_slider;
 
 int		Rsize;		/* Real size */
 int		Vsize;		/* Viewable size */
 int		Bsize;		/* Bar size in pixel */
 int		Csize;		/* Cursor size = Vsize*2/Rsize */
 int		CPos;		/* Cursor position */
 int		Pos;		/* Window position */
 
 int		PosX;		/* pos of the window in her parent */
 int		PosY;
 int		Size;		/* totam window size in pixel */
 
 int		Orient;		/* orientation SBVERTICAL or SBHORIZONTAL */
 int		Hole;		/* space in SCROLLB_SIZE unit between 2 arrow*/

 void		(*Callback)();	/* callback function */
 void		*Callback_data; /* callback parameter */
} c_ScrollBarX;

extern sf_ScrollBarX fc_ScrollBarX;

#define SCROLLB_SIZE		13
#define SCROLLBARX_BORDER       0
#define DECALAGE                2

#define SCRX_ARROW_STEP 100
#define SCRX_SLIDE_STEP 200

#define SCROLL_LEFT	FALSE
#define SCROLL_RIGHT	TRUE
#define SCROLL_UP	FALSE
#define SCROLL_DOWN	TRUE

/* some define for Colors setting */

#define SCROLLBARX_SHAD		"gray30"
#define SCROLLBARX_NORM		"gray60"
#define SCROLLBARX_HIGH		"gray80"
#define SCROLLBARX_SLIDER       "gray50"

#define SBX_HIGH	0
#define SBX_SHAD	1
#define SBX_NORM	2
#define SBX_SLIDER      3

/* some dfine for drawing */
#define 	D_UP	0
#define 	D_DOWN	1
#define 	D_LEFT	2
#define 	D_RIGHT	3

/* some define for orientation */
#define SBHORIZONTAL	0
#define SBVERTICAL	1

#endif
