/*
** WinManager.h for Xclamation, XQuad and XAllWrite in WinManager/
** Declaration of the WinManager Class
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
** Started on  Tue Oct 11 13:47:24 1994 Antoine Buat
** Last update Thu Jun  3 20:05:41 1999 Emmanuel Paris
*/

#ifndef _WinManager_h
#define _WinManager_h

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include "xcalibur.h"
#include "Cursor.h"

#ifdef _WINMANAGER_PRIVATE_
#include "Resource.h"
extern c_Resource *GlobResources;
#endif

/*--- define the method for Class WinManager ---*/
typedef struct
{
 F_STD;
 void (*ManageWin)();
 void (*DrawDecorP)();
 void (*addManaged)();
 void (*delManaged)();
 void (*delSelected)();
 void (*CloseWindow)();
 void (*TitleWindow)();
 void (*IconWindow)();
 void (*MaxWindow)();
 void (*UnMaxWindow)();
 void (*ResizeLeftWindow)();
 void (*ResizeRightWindow)();
 void (*ResizeCornerLeftWindow)();
 void (*ResizeCornerRightWindow)();
 void (*ResizeBottomWindow)();
 void (*DiselectWindow)();
 void (*MoveIconWindow)();
 void (*RaiseIcon)();
 void (*SetWindowName)();
 void (*DrawIconDecor)();
 void (*DrawIconNameDecor)();
 void (*ArrangeIcon)();
 void (*ArrangeWindow)();
 void (*TileWindow)();
 int (*eventlook)();
#ifdef Xc_XALLWRITE
 void (*UpdateWindow)();
#endif
} sf_WinManager;

/*--- define the window inside WindowManager ---*/
typedef struct 
{
 Window CloseWin;
 Window TitleWin;
 Window	IconWin;
 Window MaxWin;
 Window UnMaxWin;
 Window ResizeLeftWin;
 Window ResizeRightWin;
 Window ResizeCornerLeftWin;
 Window ResizeCornerRightWin;
 Window ResizeBottomWin;
 c_Cursor	*curscl;	/* Cursor corner bottom left */
 c_Cursor	*curscr;	/* Cursor corner bottom right */
 c_Cursor	*cursb;		/* Cursor bottom */
 c_Cursor	*cursl;		/* Cursor left */
 c_Cursor	*cursr;		/* Cursor right */
 c_Cursor	*cursc;		/* Cursor close */
 c_Cursor	*cursi;		/* Cursor icon */
 c_Cursor	*cursm;		/* Cursor max */
 c_Cursor	*cursu;		/* Cursor unmax */
 c_Cursor	*curst;		/* Cursor title */
} insidemanager;

/*--- define the listmanaged type ---*/
typedef struct listZ 
{
 insidemanager	*Use;
 char		*WinName;	/* name of the managed window */
 Window		Win;		/* this is the global Full window */
 Window		MainWin;	/* this is the managed window */
 Window		Icon;		/* this is the icone window */
 Window		Icon_Name;	/* this is the icone name window */
 int		Iconplace;
 int		Type;		/* D_NORMAL D_ICONED D_MAXIMIZED */
 c_Cursor	*cursor;
 int		Normal_x;
 int		Normal_y;
 int		Normal_w;
 int		Normal_h;
 struct listZ	*NextManaged;
} listmanaged;

#define NBR_ICON 16

/* define the WinManager Class */
typedef struct
{
 sf_WinManager	*f;
 listmanaged	*ListManaged;
 listmanaged	*SelectedWindow;
 int		nbrmanaged;
 int		totalcreated;
 Window		RestrictionWindow;
 int		Color[3];
 int		SColor[3];
 GC		graph_c;
 Cursor		cursor;
 void		(*SelectCallBack)();/* parameters are old & new window */

 /* temporary for dbl_click */
 Time		last_iclick;
 Window		last_iwin;

} c_WinManager;

extern sf_WinManager fc_WinManager;

/*--- define the event received ---*/
#define NO_EVENT	0
#define INTERN_EVENT	1
#define MOD_STRUCT	2
#define CLOSE_DOC	3
#define MAX_DOC		4

/*--- define some size for window manager ---*/
#define BUTTON_SIZEX 	18
#define BUTTON_SIZEY 	18
#define CORNER_SIZEX 	16
#define CORNER_SIZEY 	16
#define RESIZE_WIDTH 	5
#define BUTTON_UMAXW	13  /* must be equal to SCROLLB_SIZE */
#define BUTTON_UMAXH	17  /* must be equal to REGLE_SIZE */
#define	ICONW		50
#define ICON_SPACE	3
#define ICON_SPACEH	3
#define	ICONH		65
#define ICONNAMEH	16
#define ICONNAMEMH	(-3)
#define WINMINX		100
#define WINMINY		100

/*--- define the event mask ---*/
#define EVENT_MASK2	ButtonPressMask | ButtonMotionMask | ButtonReleaseMask
#define EVENT_MASK	EVENT_MASK2 | ExposureMask

/*--- define some position en size for redrawing ---*/ 
#define wmADDEDX 	2*RESIZE_WIDTH+1
#define wmADDEDY 	(BUTTON_SIZEY+RESIZE_WIDTH+1)

#define wmcloseX 	1
#define wmcloseY 	1
#define wmcloseW 	BUTTON_SIZEX
#define wmcloseH 	BUTTON_SIZEY

#define wmtitleX 	(BUTTON_SIZEX+1)
#define wmtitleY 	1
#define wmtitleW 	(width+2*RESIZE_WIDTH-BUTTON_SIZEX*3)
#define wmtitleH 	BUTTON_SIZEY

#define wmiconX 	(width+2*RESIZE_WIDTH-2*BUTTON_SIZEX+1)
#define wmiconY 	1
#define wmiconW 	BUTTON_SIZEX
#define wmiconH 	BUTTON_SIZEY

#define wmmaxX 		(width+2*RESIZE_WIDTH-BUTTON_SIZEX+1)
#define wmmaxY 		1
#define wmmaxW 		BUTTON_SIZEX
#define wmmaxH 		BUTTON_SIZEY

#define wmresizelX 	1
#define wmresizelY 	(BUTTON_SIZEY+1)
#define wmresizelW 	RESIZE_WIDTH
#define wmresizelH 	(height+RESIZE_WIDTH-CORNER_SIZEY)

#define wmresizerX 	(width+RESIZE_WIDTH+1)
#define wmresizerY 	(BUTTON_SIZEY+1)
#define wmresizerW 	RESIZE_WIDTH
#define wmresizerH 	(height+RESIZE_WIDTH-CORNER_SIZEY)

#define wmresizecornerlX 1
#define wmresizecornerlY (height+BUTTON_SIZEY+RESIZE_WIDTH-CORNER_SIZEY+1)
#define wmresizecornerlW CORNER_SIZEX
#define wmresizecornerlH CORNER_SIZEY

#define wmresizecornerrX (width+2*RESIZE_WIDTH-CORNER_SIZEX+1)
#define wmresizecornerrY (height+BUTTON_SIZEY+RESIZE_WIDTH-CORNER_SIZEY+1)
#define wmresizecornerrW CORNER_SIZEX
#define wmresizecornerrH CORNER_SIZEY

#define wmresizebottomX CORNER_SIZEX
#define wmresizebottomY (height+BUTTON_SIZEY+1)
#define wmresizebottomW (width+2*RESIZE_WIDTH-2*CORNER_SIZEX+1)
#define wmresizebottomH RESIZE_WIDTH 

#define wmmainW 	width
#define wmmainH 	height


/*--- define some color ---*/
#define SHAD		"#618530C238E3"
#define HIGH		"#FFFF8A28A699"
#define NORM		"#C30B618575D6"

#define SSHAD		"gray40"
/*"#30C251446185"*/
#define SHIGH		"gray90"
/*"#8A28E38DFFFF"*/
#define SNORM		"gray70"
/*"#6185A289C30B"*/

/*--- define some macro for color using ---*/
#define SHADOW_COLOR	Color[0]
#define HIGHLIGHT_COLOR Color[1]
#define NORMAL_COLOR    Color[2]
#define SSHADOW_COLOR	This->Color[0]
#define SHIGHLIGHT_COLOR This->Color[1]
#define SNORMAL_COLOR	This->Color[2]
#define LINE_WIDTH      0

/*--- macro to draw ghost window: only window border ---*/
#define DRAW_GHOST \
XDrawRectangle(display, This->RestrictionWindow, This->graph_c,\
	       realx+1, realy+1,\
	       width-3, height-3)

/*--- define Mask to redraw WinManager ---*/
#define MGWMCLOSE 		(1<<0)
#define MGWMTITLE 		(1<<1)
#define MGWMICON 		(1<<2)
#define MGWMMAX 		(1<<3)
#define MGWMUNMAX 		(1<<4)
#define MGWMRESIZELEFT 		(1<<5)
#define MGWMRESIZERIGHT 	(1<<6)
#define MGWMRESIZEBOTTOM 	(1<<7)
#define MGWMRESIZECORNERLEFT 	(1<<8)
#define MGWMRESIZECORNERRIGHT	(1<<9)
#define MGWMALL		        MGWMCLOSE | MGWMTITLE | MGWMICON | MGWMMAX |\
MGWMUNMAX | MGWMRESIZELEFT | MGWMRESIZERIGHT | MGWMRESIZEBOTTOM |\
MGWMRESIZECORNERLEFT | MGWMRESIZECORNERRIGHT

/*--- define the value for icon setting ---*/
#define D_ICONED	0
#define D_NORMAL	1
#define D_MAXIMIZED	2

#endif
