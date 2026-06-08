/*
** WinManager.c for Xclamation and XQuad in WinManager/
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
** Started on  Tue Jul 18 13:02:01 1995 Antoine Buat
** Last update Wed Jan 12 16:32:30 2000 Emmanuel Paris
*/

/*#define NTRACE*/
/*#define NHISTORY*/

#define _WINMANAGER_PRIVATE_
#include "WinManager.h"
#include "Colormap.h"

extern c_Colormap *GlobColormap;

void *cons_WinManager();
void dest_WinManager();
void *copy_WinManager();
void WinManagerManageWin();
void GwmDrawDecorP();
void PaddManaged();
void PdelManaged();
void PdelSelected();
void GwmCloseWindow();
void GwmTitleWindow();
void GwmIconWindow();
int QueryIconPlace();
int QueryNearIconPlace();
void GwmMaxWindow();
void GwmUnMaxWindow();
void GwmResizeLeftWindow();
void GwmResizeRightWindow();
void GwmResizeCornerLeftWindow();
void GwmResizeCornerRightWindow();
void GwmResizeBottomWindow();
void GwmDiselectWindow();
void GwmMIconWindow();
void GwmRaiseIcon();
void GwmSetWindowName();
void GwmDrawIconDecor();
void GwmDrawIconNameDecor();
void GwmArrangeIcon();
void GwmArrangeWindow();
void GwmTileWindow();
int Gwmeventlook();

extern void XcHandleEvent ___PROTO((XEvent *event));

sf_WinManager fc_WinManager =
{
 cons_WinManager,
 dest_WinManager,
 copy_WinManager,
 WinManagerManageWin,
 GwmDrawDecorP,
 PaddManaged,
 PdelManaged,
 PdelSelected,
 GwmCloseWindow,
 GwmTitleWindow,
 GwmIconWindow,
 GwmMaxWindow,
 GwmUnMaxWindow,
 GwmResizeLeftWindow,
 GwmResizeRightWindow,
 GwmResizeCornerLeftWindow,
 GwmResizeCornerRightWindow,
 GwmResizeBottomWindow,
 GwmDiselectWindow,
 GwmMIconWindow,
 GwmRaiseIcon,
 GwmSetWindowName,
 GwmDrawIconDecor,
 GwmDrawIconNameDecor,
 GwmArrangeIcon,
 GwmArrangeWindow,
 GwmTileWindow,
 Gwmeventlook
};

void *cons_WinManager(display, Gwmroot)
Display	*display;
Window	Gwmroot;
{
 c_WinManager	*ObjTmp;
 color_cells_t	realcolor;
 XGCValues	gcv;

 if ((ObjTmp = (c_WinManager *)Xc_malloc("WinManager",
					 sizeof (c_WinManager))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_WinManager;
 ObjTmp->RestrictionWindow = Gwmroot;

 gcv.function = GXinvert;
 gcv.line_width = LINE_WIDTH;
 gcv.cap_style = CapNotLast;
 gcv.subwindow_mode = IncludeInferiors;
 gcv.plane_mask = BlackPixelOfScreen(DefaultScreenOfDisplay(display)) ^
  WhitePixelOfScreen(DefaultScreenOfDisplay(display));
 ObjTmp->graph_c = XCreateGC(display, Gwmroot,
			     GCFunction | 
			     GCLineWidth | 
			     GCSubwindowMode | 
			     GCCapStyle |
			     GCPlaneMask,
			     &gcv);

 F(GlobColormap).allocate_named_color(GlobColormap, SHAD, &realcolor);
 ObjTmp->Color[0] = realcolor.pixel;
  
 F(GlobColormap).allocate_named_color(GlobColormap, HIGH, &realcolor);
 ObjTmp->Color[1] = realcolor.pixel;
  
 F(GlobColormap).allocate_named_color(GlobColormap, NORM, &realcolor);
 ObjTmp->Color[2] = realcolor.pixel;
  
 F(GlobColormap).allocate_named_color(GlobColormap, SSHAD, &realcolor);
 ObjTmp->SColor[0] = realcolor.pixel;
  
 F(GlobColormap).allocate_named_color(GlobColormap, SHIGH, &realcolor);
 ObjTmp->SColor[1] = realcolor.pixel;
  
 F(GlobColormap).allocate_named_color(GlobColormap, SNORM, &realcolor);
 ObjTmp->SColor[2] = realcolor.pixel;
  
 ObjTmp->cursor = GlobCursor->cursor_x[C_WIN_MOVE];
  
 ObjTmp->ListManaged = NULL;
 ObjTmp->nbrmanaged = 0;
 ObjTmp->totalcreated = 0;
 ObjTmp->SelectedWindow = NULL;
 Xc_TRACE(("Object WinManager build"));
 return ObjTmp;
}

void dest_WinManager(This, display)
c_WinManager 	*This;
Display		*display;
{
 listmanaged	*List_manag;

 List_manag = This->ListManaged;
 while(List_manag)
 {
  F(This).CloseWindow(This, List_manag, display);
  List_manag = List_manag->NextManaged;
 }
 XFreeGC(display, This->graph_c);
 Xc_free(This);
 Xc_TRACE(("Object WinManager destroyed"));
}

void *copy_WinManager(This)
c_WinManager *This;
{
 c_WinManager *ObjTmp;

 if ((ObjTmp = (c_WinManager *)Xc_malloc("CWinManager",
					 sizeof (c_WinManager))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof (c_WinManager));
 Xc_TRACE(("Object WinManager copied"));
 return ObjTmp;
}

void WinManagerManageWin(This, display, Win1, pixmap)
c_WinManager	*This;
Display		*display;
Window		Win1;
Pixmap		pixmap;
{
 Window		r_win;
 Window		m_win;
 Window		*SubWin;
 Window		IconW;
 Window		IconNameW;
 Window		MasterWin;
 int			x;
 int 			y;
 int			none;
 unsigned int		width;
 unsigned int		height;
 unsigned int		border;
 unsigned int		level;
 XFontStruct		*fnt_struct;
 char			*WinName;
 int			Tlength;
 XEvent		Event, event;
 listmanaged		*listegwm;
 insidemanager		*Use;
 XSetWindowAttributes	attribs;

 XQueryTree(display, Win1, &r_win, &m_win, &SubWin, &x);
 XFree((char *)SubWin);
 XGetGeometry(display, Win1, &r_win,
	      &x, &y, &width, &height, &border, &level);

 if ((Use = (insidemanager *)Xc_malloc("insidemanager",
				       sizeof(insidemanager))) == NULL)
 {
  Xc_FATAL(("Can't create inside manager: memory allocation error"));
 }
 
 MasterWin = XCreateSimpleWindow(display, m_win, 
				 x, y,
				 width + wmADDEDX,
				 height + wmADDEDY,
				 0,
				 SSHADOW_COLOR,
				 SSHADOW_COLOR);
 XSelectInput(display, MasterWin, EVENT_MASK2 | StructureNotifyMask);
 attribs.bit_gravity = StaticGravity;
 XChangeWindowAttributes(display, MasterWin, CWBitGravity, &attribs);
 attribs.event_mask = EVENT_MASK;

 IconW = XCreateSimpleWindow(display, m_win,
			     x, y,
			     ICONW,
			     ICONH,
			     0,
			     SNORMAL_COLOR,
			     SNORMAL_COLOR);
 XSelectInput(display, IconW, EVENT_MASK | EnterWindowMask | LeaveWindowMask);
 XSetWindowBackgroundPixmap(display, IconW, pixmap);

 fnt_struct = F(GlobResources).getXFontStruct(GlobResources, XcR_iconFont);
 XFetchName(display, Win1, &WinName);
#ifndef NTRACE
 if (WinName)
  Xc_TRACE(("%s", WinName));
#endif
 Tlength = XTextWidth(fnt_struct, WinName, strlen(WinName))+4;
 Xc_TRACE(("Textwidth+4: %d", Tlength));
 if (Tlength > ICONW)
 {
  IconNameW = XCreateSimpleWindow(display, m_win,
				  x, y,
				  Tlength,
				  ICONNAMEH,
				  0,
				  This->SColor[0],/* SHADOW */
				  This->SColor[0]);
  XSelectInput(display, IconNameW, 
	       ButtonPressMask | ExposureMask | LeaveWindowMask);
 }
 else
  IconNameW = 0;
 XFree(WinName);
 
 Use->CloseWin = XCreateSimpleWindow(display, MasterWin,
				     wmcloseX, wmcloseY,
				     wmcloseW, wmcloseH,
				     0, 
				     SSHADOW_COLOR,
				     SNORMAL_COLOR); 
 attribs.win_gravity = NorthWestGravity;
 XChangeWindowAttributes(display, Use->CloseWin, 
			 CWWinGravity | CWEventMask, &attribs);
 Use->cursc = (c_Cursor *)NEW(c_Cursor)(display, Use->CloseWin);
 F(Use->cursc).set_cursor(Use->cursc, C_WIN_CLOSE);
 
 Use->TitleWin = XCreateSimpleWindow(display, MasterWin,
				     wmtitleX, wmtitleY,
				     wmtitleW, wmtitleH,
				     0,
				     SSHADOW_COLOR,
				     SNORMAL_COLOR);
 attribs.win_gravity = NorthWestGravity;
 XChangeWindowAttributes(display, Use->TitleWin, 
			 CWWinGravity | CWEventMask, &attribs);
 Use->curst = (c_Cursor *)NEW(c_Cursor)(display, Use->TitleWin);
 F(Use->curst).set_cursor(Use->curst, C_WIN_TITLE);
 
 Use->IconWin = XCreateSimpleWindow(display, MasterWin,
				    wmiconX, wmiconY,
				    wmiconW, wmiconH,
				    0,
				    SSHADOW_COLOR,
				    SNORMAL_COLOR);
 attribs.win_gravity = NorthEastGravity;
 XChangeWindowAttributes(display, Use->IconWin, 
			 CWWinGravity | CWEventMask, &attribs);
 Use->cursi = (c_Cursor *)NEW(c_Cursor)(display, Use->IconWin);
 F(Use->cursi).set_cursor(Use->cursi, C_WIN_ICON);

 Use->MaxWin = XCreateSimpleWindow(display, MasterWin,
				   wmmaxX, wmmaxY,
				   wmmaxW, wmmaxH,
				   0,
				   SSHADOW_COLOR,
				   SNORMAL_COLOR);
 attribs.win_gravity = NorthEastGravity;
 XChangeWindowAttributes(display, Use->MaxWin, 
			 CWWinGravity | CWEventMask, &attribs);
 Use->cursm = (c_Cursor *)NEW(c_Cursor)(display, Use->MaxWin);
 F(Use->cursm).set_cursor(Use->cursm, C_WIN_MAX);

 Use->UnMaxWin = XCreateSimpleWindow(display, MasterWin,
				     width + 2 * RESIZE_WIDTH + 1 
				     - BUTTON_UMAXW, 0,
				     BUTTON_UMAXW, BUTTON_UMAXH,
				     0,
				     SSHADOW_COLOR,
				     SNORMAL_COLOR);
 attribs.win_gravity = NorthEastGravity;
 XChangeWindowAttributes(display, Use->UnMaxWin, 
			 CWWinGravity | CWEventMask, &attribs);
 Use->cursu = (c_Cursor *)NEW(c_Cursor)(display, Use->UnMaxWin);
 F(Use->cursu).set_cursor(Use->cursu, C_WIN_UNMAX);

 Use->ResizeLeftWin = XCreateSimpleWindow(display, MasterWin,
					  wmresizelX, wmresizelY,
					  wmresizelW, wmresizelH,
					  0,
					  SSHADOW_COLOR,
					  SNORMAL_COLOR);
 attribs.win_gravity = NorthWestGravity;
 XChangeWindowAttributes(display, Use->ResizeLeftWin, 
			 CWWinGravity | CWEventMask, &attribs);
 Use->cursl = (c_Cursor *)NEW(c_Cursor)(display, Use->ResizeLeftWin);
 F(Use->cursl).set_cursor(Use->cursl, C_WIN_LEFT);

 Use->ResizeRightWin = XCreateSimpleWindow(display, MasterWin,
					   wmresizerX, wmresizerY,
					   wmresizerW, wmresizerH,
					   0,
					   SSHADOW_COLOR,
					   SNORMAL_COLOR);
 attribs.win_gravity = NorthEastGravity;
 XChangeWindowAttributes(display, Use->ResizeRightWin, 
			 CWWinGravity | CWEventMask, &attribs);
 Use->cursr = (c_Cursor *)NEW(c_Cursor)(display, Use->ResizeRightWin);
 F(Use->cursr).set_cursor(Use->cursr, C_WIN_RIGHT);

 Use->ResizeCornerLeftWin = XCreateSimpleWindow(display, MasterWin,
						wmresizecornerlX,
						wmresizecornerlY,
						wmresizecornerlW,
						wmresizecornerlH,
						0,
						SSHADOW_COLOR,
						SNORMAL_COLOR);
 attribs.win_gravity = SouthWestGravity;
 XChangeWindowAttributes(display, Use->ResizeCornerLeftWin, 
			 CWWinGravity | CWEventMask, &attribs);
 Use->curscl = (c_Cursor *)NEW(c_Cursor)(display, Use->ResizeCornerLeftWin);
 F(Use->curscl).set_cursor(Use->curscl, C_WIN_BOTTOM_LEFT);

 Use->ResizeCornerRightWin = XCreateSimpleWindow(display, MasterWin,
						 wmresizecornerrX,
						 wmresizecornerrY,
						 wmresizecornerrW,
						 wmresizecornerrH,
						 0,
						 SSHADOW_COLOR,
						 SNORMAL_COLOR);
 attribs.win_gravity = SouthEastGravity;
 XChangeWindowAttributes(display, Use->ResizeCornerRightWin, 
			 CWWinGravity | CWEventMask, &attribs);
 Use->curscr = (c_Cursor *)NEW(c_Cursor)(display, Use->ResizeCornerRightWin);
 F(Use->curscr).set_cursor(Use->curscr, C_WIN_BOTTOM_RIGHT);

 Use->ResizeBottomWin = XCreateSimpleWindow(display, MasterWin,
					    wmresizebottomX,
					    wmresizebottomY,
					    wmresizebottomW, 
					    wmresizebottomH,
					    0,
					    SSHADOW_COLOR,
					    SNORMAL_COLOR);
 attribs.win_gravity = SouthWestGravity;
 XChangeWindowAttributes(display, Use->ResizeBottomWin, 
			 CWWinGravity | CWEventMask, &attribs);
 Use->cursb = (c_Cursor *)NEW(c_Cursor)(display, Use->ResizeBottomWin);
 F(Use->cursb).set_cursor(Use->cursb, C_WIN_BOTTOM);
    
 F(This).addManaged(This, display, Win1, MasterWin, IconW, IconNameW, Use);

 listegwm = This->ListManaged;
 XGetGeometry(display, listegwm->Win, (Window *)&none,
	      &(listegwm->Normal_x), &(listegwm->Normal_y),
	      &(listegwm->Normal_w), &(listegwm->Normal_h), &none, &none);
 if (This->nbrmanaged == 1)
 {
  listegwm->Type = D_MAXIMIZED;
  XGetGeometry(display, m_win, (Window *)&none,
	       &none, &none, &width, &height, &none, &none);
  XMoveResizeWindow(display, MasterWin, 0, 0, width, height);
  XReparentWindow(display, Win1, MasterWin, 0, 0);
  XResizeWindow(display, Win1, width, height);
  XMapWindow(display, Win1);
  Xc_TRACE(("wait for resize events"));
    
  event.type = ClientMessage;
  event.xclient.send_event = True;
  event.xclient.display = display;
  event.xclient.window = MasterWin;
  event.xclient.format=32;
  event.xclient.data.l[0] = 0xdeaddead;
  XSendEvent(display, MasterWin ,True, 0, &event);
  while(1)
  {
   XNextEvent(display, &Event);
   if (Event.type == ClientMessage)
   {
    Xc_TRACE(("Client Message: Window id: %d message: %x", 
	      Event.xclient.window, Event.xclient.data.l[0]));
    Xc_TRACE(("Track window id: %d", MasterWin));
   }
   if (Event.type == ClientMessage && 
       Event.xclient.window == MasterWin &&
       (int)Event.xclient.data.l[0] == (int)0xdeaddead)
    break;
   XcHandleEvent(&Event);
  }
  Xc_TRACE(("wait done"));
  XMapRaised(display, listegwm->Use->UnMaxWin);
  XMapWindow(display, MasterWin);
 }
 else
 {
  XReparentWindow(display, Win1, MasterWin, 
		  RESIZE_WIDTH+1, BUTTON_SIZEY+1);
  XMapSubwindows(display, MasterWin);
  XUnmapWindow(display, Use->UnMaxWin);
  XMapRaised(display, MasterWin);
 }
}

/* ----------------------------------------------------------------- ** 
** Draw only some parts of the window manager                        ** 
** ----------------------------------------------------------------- */
void GwmDrawDecorP(This, managed, display, width, height, mask)
c_WinManager	*This;
listmanaged	*managed;
Display		*display;
int		width;
int		height;
int		mask;
{
 GC		gc_shadow;
 GC		gc_highlight;
 GC		gc_texte;
 XGCValues 	xgcv;
 insidemanager	*Use; 
 Window	MasterWin;
 int		*Color;

 Use = managed->Use;
 MasterWin = managed->Win;
 if (managed == This->SelectedWindow)
 {
  Color = This->Color;
 }
 else
 {
  Color = This->SColor;
 }

 xgcv.foreground = SHADOW_COLOR;
 xgcv.line_width = 0;
 gc_shadow = XCreateGC(display, MasterWin,
		       GCLineWidth
		       |GCForeground,
		       &xgcv);

 xgcv.foreground = HIGHLIGHT_COLOR;
 xgcv.line_width = 0;
 gc_highlight = XCreateGC(display, MasterWin,
			  GCLineWidth
			  |GCForeground,
			  &xgcv);

 xgcv.function = GXcopy;
 xgcv.background = NORMAL_COLOR;
 xgcv.foreground = BlackPixel(display, DefaultScreen(display));
 xgcv.font = F(GlobResources).getFont(GlobResources, XcR_iconFont);
 gc_texte = XCreateGC(display, Use->TitleWin,
		      GCFunction | GCForeground | GCBackground | GCFont,
		      &xgcv);

 /* ----------------------------------------------------------------- ** 
 ** close window                                                      ** 
 ** ----------------------------------------------------------------- */
 if (mask & MGWMCLOSE)
 {
  XSetWindowBackground(display, Use->CloseWin, NORMAL_COLOR);
  XClearWindow(display, Use->CloseWin);
  XDrawLine(display, Use->CloseWin, gc_highlight, 
	    0, 0,
	    BUTTON_SIZEX-2, 0);
  XDrawLine(display, Use->CloseWin, gc_highlight, 
	    0, 1,
	    BUTTON_SIZEX-2, 1);
  XDrawLine(display, Use->CloseWin, gc_highlight,
	    0, 2,
	    0, BUTTON_SIZEY-2);
  XDrawLine(display, Use->CloseWin, gc_highlight,
	    1, 2,
	    1, BUTTON_SIZEY-2);
  XDrawLine(display, Use->CloseWin, gc_shadow, 
	    BUTTON_SIZEX-1, 0,
	    BUTTON_SIZEX-1, BUTTON_SIZEY-1);
  XDrawLine(display, Use->CloseWin, gc_shadow, 
	    0, BUTTON_SIZEY-1,
	    BUTTON_SIZEX-2, BUTTON_SIZEY-1);
  XDrawArc(display, Use->CloseWin, gc_shadow,
	   4, 4,
	   BUTTON_SIZEY-8 , BUTTON_SIZEY-8,
	   225*64, 180*64);
  XDrawArc(display, Use->CloseWin, gc_highlight,
	   4, 4,
	   BUTTON_SIZEY-8 , BUTTON_SIZEY-8,
	   45*64, 180*64);
 }

 /* ----------------------------------------------------------------- ** 
 ** title window                                                      ** 
 ** ----------------------------------------------------------------- */
 if (mask & MGWMTITLE)
 {
  XSetWindowBackground(display, Use->TitleWin, NORMAL_COLOR);
  XClearWindow(display, Use->TitleWin);
  XDrawImageString(display, Use->TitleWin, gc_texte,
		   5, wmtitleH-4,
		   managed->WinName, strlen(managed->WinName));
  XDrawLine(display, Use->TitleWin, gc_highlight,
	    0, 0,
	    wmtitleW-2, 0);
  XDrawLine(display, Use->TitleWin, gc_highlight,
	    0, 1,
	    wmtitleW-2, 1);
  XDrawLine(display, Use->TitleWin, gc_shadow,
	    wmtitleW-1, 0,
	    wmtitleW-1, wmtitleH-1);
  XDrawLine(display, Use->TitleWin, gc_shadow,
	    0, wmtitleH-1,
	    wmtitleW-2, wmtitleH-1);
  XDrawLine(display, Use->TitleWin, gc_highlight, 
	    0, 0,
	    0, wmtitleH-1);
 }
  
 /* ----------------------------------------------------------------- ** 
 ** icon window                                                       ** 
 ** ----------------------------------------------------------------- */
 if (mask & MGWMICON)
 {
  XSetWindowBackground(display, Use->IconWin, NORMAL_COLOR);
  XClearWindow(display, Use->IconWin);
  XDrawLine(display, Use->IconWin, gc_highlight,
	    0, 0,
	    BUTTON_SIZEX-2, 0);
  XDrawLine(display, Use->IconWin, gc_highlight,
	    0, 1,
	    BUTTON_SIZEX-2, 1);
  XDrawLine(display, Use->IconWin, gc_shadow, 
	    BUTTON_SIZEX-1, 0,
	    BUTTON_SIZEX-1, BUTTON_SIZEY-1);
  XDrawLine(display, Use->IconWin, gc_shadow, 
	    1, BUTTON_SIZEY-1,
	    BUTTON_SIZEX-2, BUTTON_SIZEY-1);
  XDrawLine(display, Use->IconWin, gc_highlight,
	    0, 2,
	    0, BUTTON_SIZEY-1);
  XDrawLine(display, Use->IconWin, gc_highlight,
	    3, 4,
	    BUTTON_SIZEX - 4, 4);
  XDrawLine(display, Use->IconWin, gc_highlight,
	    3, 4,
	    BUTTON_SIZEX /2 - 1, BUTTON_SIZEY - 4);
  XDrawLine(display, Use->IconWin, gc_shadow,
	    BUTTON_SIZEX - 4, 4,
	    BUTTON_SIZEX / 2, BUTTON_SIZEY - 4);
 }

 /* ----------------------------------------------------------------- ** 
 ** maximize window                                                   ** 
 ** ----------------------------------------------------------------- */
 if (mask & MGWMMAX)
 {
  XSetWindowBackground(display, Use->MaxWin, NORMAL_COLOR);
  XClearWindow(display, Use->MaxWin);
  XDrawLine(display, Use->MaxWin, gc_highlight,
	    0, 0,
	    BUTTON_SIZEX-2, 0);
  XDrawLine(display, Use->MaxWin, gc_highlight,
	    0, 1,
	    BUTTON_SIZEX-3, 1);
  XDrawLine(display, Use->MaxWin, gc_shadow, 
	    BUTTON_SIZEX-2, 1,
	    BUTTON_SIZEX-2, BUTTON_SIZEY-1);
  XDrawLine(display, Use->MaxWin, gc_shadow,
	    BUTTON_SIZEX-1, 0,
	    BUTTON_SIZEX-1, BUTTON_SIZEY-1);
  XDrawLine(display, Use->MaxWin, gc_shadow, 
	    0, BUTTON_SIZEY-1,
	    BUTTON_SIZEX-3, BUTTON_SIZEY-1);
  XDrawLine(display, Use->MaxWin, gc_highlight, 
	    0, 2,
	    0, BUTTON_SIZEX-1);
  XDrawLine(display, Use->MaxWin, gc_shadow,
	    4, BUTTON_SIZEY - 4,
	    BUTTON_SIZEX - 5,  BUTTON_SIZEY - 4);
  XDrawLine(display, Use->MaxWin, gc_shadow,
	    BUTTON_SIZEX /2 , 4,
	    BUTTON_SIZEX - 5, BUTTON_SIZEY- 4);
  XDrawLine(display, Use->MaxWin, gc_highlight,
	    3, BUTTON_SIZEY - 4,
	    BUTTON_SIZEX /2 -1, 4);
 }

 /* ----------------------------------------------------------------- ** 
 ** unmaximize window                                                 ** 
 ** ----------------------------------------------------------------- */
 if (mask & MGWMUNMAX)
 {
  XSetWindowBackground(display, Use->UnMaxWin, NORMAL_COLOR);
  XClearWindow(display, Use->UnMaxWin);
  XDrawLine(display, Use->UnMaxWin, gc_highlight,
	    0, 0,
	    BUTTON_UMAXW-2, 0);
  XDrawLine(display, Use->UnMaxWin, gc_highlight,
	    0, 1,
	    BUTTON_UMAXW-2, 1);
  XDrawLine(display, Use->UnMaxWin, gc_shadow, 
	    BUTTON_UMAXW-1, 0,
	    BUTTON_UMAXW-1, BUTTON_UMAXH-1);
  XDrawLine(display, Use->UnMaxWin, gc_shadow, 
	    1, BUTTON_UMAXH-1,
	    BUTTON_UMAXW-2, BUTTON_UMAXH-1);
  XDrawLine(display, Use->UnMaxWin, gc_highlight,
	    0, 2,
	    0, BUTTON_UMAXH-1);
  XDrawLine(display, Use->UnMaxWin, gc_highlight,
	    3, 4,
	    BUTTON_UMAXW - 4, 4);
  XDrawLine(display, Use->UnMaxWin, gc_highlight,
	    3, 4,
	    BUTTON_UMAXW /2 - 1, BUTTON_UMAXH - 4);
  XDrawLine(display, Use->UnMaxWin, gc_shadow,
	    BUTTON_UMAXW - 4, 4,
	    BUTTON_UMAXW / 2, BUTTON_UMAXH - 4);
 }

 /* ----------------------------------------------------------------- ** 
 ** resize left window                                                ** 
 ** ----------------------------------------------------------------- */
 if (mask & MGWMRESIZELEFT)
 {
  XSetWindowBackground(display, Use->ResizeLeftWin, NORMAL_COLOR);
  XClearWindow(display, Use->ResizeLeftWin);
  XDrawLine(display, Use->ResizeLeftWin, gc_highlight,
	    0, 0,
	    0, wmresizelH-2);
  XDrawLine(display, Use->ResizeLeftWin, gc_highlight,
	    1, 0,
	    1, wmresizelH-2);
  XDrawLine(display, Use->ResizeLeftWin, gc_highlight, 
	    2, 0,
	    wmresizelW-1, 0);
  XDrawLine(display, Use->ResizeLeftWin, gc_shadow,
	    wmresizelW-1, 1,
	    wmresizelW-1, wmresizelH-1);
  XDrawLine(display, Use->ResizeLeftWin, gc_shadow, 
	    0, wmresizelH-1,
	    wmresizelW-2, wmresizelH-1);
 }

 /* ----------------------------------------------------------------- ** 
 ** resize right window                                               ** 
 ** ----------------------------------------------------------------- */
 if (mask & MGWMRESIZERIGHT)
 {
  XSetWindowBackground(display, Use->ResizeRightWin, NORMAL_COLOR);
  XClearWindow(display, Use->ResizeRightWin);
  XDrawLine(display, Use->ResizeRightWin, gc_shadow,
	    wmresizerW-2, 1,
	    wmresizerW-2, wmresizerH-1);
  XDrawLine(display, Use->ResizeRightWin, gc_shadow,
	    wmresizerW-1, 0,
	    wmresizerW-1, wmresizerH-1);
  XDrawLine(display, Use->ResizeRightWin, gc_shadow, 
	    0, wmresizerH-1,
	    wmresizerW-3, wmresizerH-1);
  XDrawLine(display, Use->ResizeRightWin, gc_highlight,
	    0, 0,
	    0, wmresizerH-2);
  XDrawLine(display, Use->ResizeRightWin, gc_highlight, 
	    0, 0,
	    wmresizerW-2, 0);
 }

 /* ----------------------------------------------------------------- ** 
 ** resize bottom window                                              ** 
 ** ----------------------------------------------------------------- */
 if (mask & MGWMRESIZEBOTTOM)
 {
  XSetWindowBackground(display, Use->ResizeBottomWin, NORMAL_COLOR);
  XClearWindow(display, Use->ResizeBottomWin);
  XDrawLine(display, Use->ResizeBottomWin, gc_shadow,
	    0, wmresizebottomH-2, 
	    wmresizebottomW-1, wmresizebottomH-2);
  XDrawLine(display, Use->ResizeBottomWin, gc_shadow,
	    0, wmresizebottomH-1, 
	    wmresizebottomW-1, wmresizebottomH-1);
  XDrawLine(display, Use->ResizeBottomWin, gc_shadow, 
	    wmresizebottomW-1, 0,
	    wmresizebottomW-1, wmresizebottomH-3);
  XDrawLine(display, Use->ResizeBottomWin, gc_highlight,
	    0, 0,
	    0, wmresizebottomH-2);
  XDrawLine(display, Use->ResizeBottomWin, gc_highlight, 
	    0, 0,
	    wmresizebottomW-2, 0);
 }

 /* ----------------------------------------------------------------- ** 
 ** resize corner left window                                         ** 
 ** ----------------------------------------------------------------- */
 if (mask & MGWMRESIZECORNERLEFT)
 {
  XSetWindowBackground(display, Use->ResizeCornerLeftWin, NORMAL_COLOR);
  XClearWindow(display, Use->ResizeCornerLeftWin);
  XDrawLine(display, Use->ResizeCornerLeftWin, gc_shadow,
	    1, CORNER_SIZEY-2, 
	    CORNER_SIZEX-1, CORNER_SIZEY-2);
  XDrawLine(display, Use->ResizeCornerLeftWin, gc_shadow,
	    0, CORNER_SIZEY-1, 
	    CORNER_SIZEX-1, CORNER_SIZEY-1);
  XDrawLine(display, Use->ResizeCornerLeftWin, gc_shadow, 
	    CORNER_SIZEX-2, CORNER_SIZEY-wmresizebottomH-1,
	    CORNER_SIZEX-2, CORNER_SIZEY-3);
  XDrawLine(display, Use->ResizeCornerLeftWin, gc_shadow, 
	    wmresizelW-1, 0,
	    wmresizelW-1, CORNER_SIZEY-wmresizebottomH-1);
  XDrawLine(display, Use->ResizeCornerLeftWin, gc_highlight,
	    0, 1,
	    0, CORNER_SIZEY-2);
  XDrawLine(display, Use->ResizeCornerLeftWin, gc_highlight, 
	    1, 1,
	    1, CORNER_SIZEY-3);
  XDrawLine(display, Use->ResizeCornerLeftWin, gc_highlight, 
	    0, 0,
	    wmresizelW-1, 0);
  XDrawLine(display, Use->ResizeCornerLeftWin, gc_highlight, 
	    wmresizelW-1, CORNER_SIZEY-wmresizebottomH,
	    CORNER_SIZEX-3, CORNER_SIZEY-wmresizebottomH);
  XDrawLine(display, Use->ResizeCornerLeftWin, gc_shadow, 
	    wmresizelW, CORNER_SIZEY-wmresizebottomH-1,
	    CORNER_SIZEX, CORNER_SIZEY-wmresizebottomH-1);
 }

 /* ----------------------------------------------------------------- ** 
 ** resize corner right window                                        ** 
 ** ----------------------------------------------------------------- */
 if (mask & MGWMRESIZECORNERRIGHT)
 {
  XSetWindowBackground(display, Use->ResizeCornerRightWin, NORMAL_COLOR);
  XClearWindow(display, Use->ResizeCornerRightWin);
  XDrawLine(display, Use->ResizeCornerRightWin, gc_shadow,
	    CORNER_SIZEX-2, 0,
	    CORNER_SIZEX-2, CORNER_SIZEY-1);
  XDrawLine(display, Use->ResizeCornerRightWin, gc_shadow,
	    CORNER_SIZEX-1, 0,
	    CORNER_SIZEX-1, CORNER_SIZEY-1);
  XDrawLine(display, Use->ResizeCornerRightWin, gc_shadow,
	    1, CORNER_SIZEY-2,
	    CORNER_SIZEX-3,  CORNER_SIZEY-2);
  XDrawLine(display, Use->ResizeCornerRightWin, gc_shadow,
	    0, CORNER_SIZEY-1,
	    CORNER_SIZEX-3,  CORNER_SIZEY-1);
  XDrawLine(display, Use->ResizeCornerRightWin, gc_highlight,
	    CORNER_SIZEX-wmresizerW, 0,
	    CORNER_SIZEX-2, 0);
  XDrawLine(display, Use->ResizeCornerRightWin, gc_highlight,
	    0, CORNER_SIZEY-wmresizebottomH,
	    0, CORNER_SIZEY-2);
  XDrawLine(display, Use->ResizeCornerRightWin, gc_highlight,
	    CORNER_SIZEX-wmresizerW, 1,
	    CORNER_SIZEX-wmresizerW, CORNER_SIZEY-wmresizebottomH);
  XDrawLine(display, Use->ResizeCornerRightWin, gc_shadow,
	    CORNER_SIZEX-wmresizerW-1, 0,
	    CORNER_SIZEX-wmresizerW-1, CORNER_SIZEY-wmresizebottomH-1);
  XDrawLine(display, Use->ResizeCornerRightWin, gc_highlight,
	    1, CORNER_SIZEY-wmresizebottomH,
	    CORNER_SIZEX-wmresizerW, CORNER_SIZEY-wmresizebottomH);
  XDrawLine(display, Use->ResizeCornerRightWin, gc_shadow,
	    0, CORNER_SIZEY-wmresizebottomH-1,
	    CORNER_SIZEX-wmresizerW-1, CORNER_SIZEY-wmresizebottomH-1);
 }

 XFreeGC(display, gc_shadow);
 XFreeGC(display, gc_highlight);
 XFreeGC(display, gc_texte);
}

void PaddManaged(This, display, MainWin1, Win1, IconW, IconNameW, Use)
c_WinManager	*This;
Display		*display;
Window		MainWin1;
Window		Win1;
Window		IconW;
Window		IconNameW;
insidemanager	*Use;
{
 listmanaged	**ManagedTmp;
 listmanaged	*ManagedNew;
 char		*WinName;
  
 if ((ManagedNew = (listmanaged *)Xc_malloc("listmanaged",
					    sizeof (listmanaged))) == NULL)
 {
  Xc_FATAL(("Can't add new Window: memory allocation error"));
 }
 ManagedNew->Win = Win1;

 XFetchName(display, MainWin1, &WinName);
 ManagedNew->WinName = (char *)Xc_strdup("WinName", WinName);
 XFree(WinName);
 This->totalcreated++;

 ManagedNew->MainWin = MainWin1;
 ManagedNew->Use = Use;
 ManagedNew->Icon = IconW;
 ManagedNew->Icon_Name = IconNameW;
 ManagedNew->Type = D_NORMAL;
 
 ManagedNew->cursor = (c_Cursor *)NEW(c_Cursor)(display, Win1);
 F(ManagedNew->cursor).set_cursor(ManagedNew->cursor, C_LOCK_CADRE);
 F(ManagedNew->cursor).unset_cursor(ManagedNew->cursor);
  
 ManagedNew->NextManaged = NULL;

 ManagedTmp = &D(This).ListManaged;
 D(This).nbrmanaged++;
 while (*ManagedTmp != NULL)
 {
  ManagedTmp = &((*ManagedTmp)->NextManaged);
 }
 *ManagedTmp = ManagedNew;
 F(This).DiselectWindow(This, display, ManagedNew);
}

void PdelManaged(This, pos, display)
c_WinManager	*This;
int		pos;
Display		*display;
{
 listmanaged	**ManagedTmp;
 listmanaged	*ManagedDel;
 int		num;

 ManagedTmp = &D(This).ListManaged;
 num = 0;
 while ((*ManagedTmp != NULL) && (num<pos))
 {
  ManagedTmp = &((*ManagedTmp)->NextManaged);
  num++;
 }
 if (num != pos)
 {
  Xc_WARNING(("Can't delete at this position"));
 }
 else
 {
  ManagedDel = *ManagedTmp;
  *ManagedTmp = ((*ManagedTmp)->NextManaged);
  D(This).nbrmanaged--;
  DELETE(c_Cursor)(ManagedDel->cursor);
  DELETE(c_Cursor)(ManagedDel->Use->curscl);
  DELETE(c_Cursor)(ManagedDel->Use->curscr);
  DELETE(c_Cursor)(ManagedDel->Use->cursb);
  DELETE(c_Cursor)(ManagedDel->Use->cursl);
  DELETE(c_Cursor)(ManagedDel->Use->cursr);
  DELETE(c_Cursor)(ManagedDel->Use->cursc);
  DELETE(c_Cursor)(ManagedDel->Use->cursi);
  DELETE(c_Cursor)(ManagedDel->Use->cursm);
  DELETE(c_Cursor)(ManagedDel->Use->cursu);
  DELETE(c_Cursor)(ManagedDel->Use->curst);
  XDestroyWindow(display, ManagedDel->Win);
  XDestroyWindow(display, ManagedDel->Icon);
  if (ManagedDel->Icon_Name)
   XDestroyWindow(display, ManagedDel->Icon_Name);
  Xc_free(ManagedDel->WinName);
  Xc_free(ManagedDel->Use);
  Xc_free(ManagedDel);
 }
}


void PdelSelected(This, display)
c_WinManager	*This;
Display 	*display;
{
 listmanaged	**ManagedTmp;
 listmanaged	*ManagedDel;

 if (This->SelectedWindow)
 {
  ManagedTmp = &D(This).ListManaged;
  while (*ManagedTmp != NULL && *ManagedTmp != This->SelectedWindow)
  {
   ManagedTmp = &((*ManagedTmp)->NextManaged);
  }
  if (*ManagedTmp == NULL)
  {
   Xc_WARNING(("Can't delete at this position"));
  }
  else
  {
   ManagedDel = *ManagedTmp;
   *ManagedTmp = ((*ManagedTmp)->NextManaged);
   D(This).nbrmanaged--;
   DELETE(c_Cursor)(ManagedDel->cursor);
   DELETE(c_Cursor)(ManagedDel->Use->curscl);
   DELETE(c_Cursor)(ManagedDel->Use->curscr);
   DELETE(c_Cursor)(ManagedDel->Use->cursb);
   DELETE(c_Cursor)(ManagedDel->Use->cursl);
   DELETE(c_Cursor)(ManagedDel->Use->cursr);
   DELETE(c_Cursor)(ManagedDel->Use->cursc);
   DELETE(c_Cursor)(ManagedDel->Use->cursi);
   DELETE(c_Cursor)(ManagedDel->Use->cursm);
   DELETE(c_Cursor)(ManagedDel->Use->cursu);
   DELETE(c_Cursor)(ManagedDel->Use->curst);
   XDestroyWindow(display, ManagedDel->Win);
   XDestroyWindow(display, ManagedDel->Icon);
   if (ManagedDel->Icon_Name)
    XDestroyWindow(display, ManagedDel->Icon_Name);
   Xc_free(ManagedDel->WinName);
   Xc_free(ManagedDel->Use);
   Xc_free(ManagedDel);
  }
  This->SelectedWindow = NULL;
 }
}

void GwmCloseWindow(This, listegwm, display)
c_WinManager	*This;
listmanaged	*listegwm;
Display		*display;
{
 listmanaged	*list_tmp;
 int		num;

 list_tmp = This->ListManaged;
 num = 0;
 while (listegwm != list_tmp)
 {
  list_tmp = list_tmp->NextManaged;
  num++;
 }
 F(This).delManaged(This, num, display);
}

void GwmTitleWindow(This, display, TEvent, listegwm)
c_WinManager	*This;
Display		*display;
XEvent		*TEvent;
listmanaged	*listegwm;
{
 XEvent	Event;
 Window	none;
 int	mx, my, cx, cy, nihil;
 int	realx, realy;
 int	width, height;
 int	Rwidth,	Rheight;
 int	bord, level;
 int	move_percent;
 boolean	Ghost = FALSE;
 boolean	sortie = FALSE;
 boolean	raise = FALSE;
  
 XGrabPointer(display, This->RestrictionWindow,
	      False, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask, 
	      GrabModeAsync,GrabModeAsync, 
	      This->RestrictionWindow, This->cursor, 0); 
 XGetGeometry(display, This->RestrictionWindow, &none, 
	      &realx, &realy, &Rwidth, &Rheight, &bord, &level);
 XGetGeometry(display, listegwm->Win, &none,
	      &realx, &realy, &width, &height, &bord, &level);
  
 move_percent = F(GlobResources).getInt(GlobResources, XcR_opaqueMove);
 if (move_percent < 0) move_percent = 0;
 if (move_percent > 100) move_percent = 100;
  
 if ( (width*height) > (Rwidth * Rheight * move_percent / 100)
     && move_percent != 100)
  Ghost = TRUE;
  
 if (TEvent->xbutton.button == 1)
 {
  if (Ghost)
   raise = TRUE;
  else
   XRaiseWindow(display, listegwm->Win);
 }
 XQueryPointer(display, This->RestrictionWindow, &none, &none, 
	       &nihil, &nihil, &mx, &my, &nihil);
 
 /*  XTranslateCoordinates(display, TEvent->xbutton.window, 
     This->RestrictionWindow,
     TEvent->xbutton.x, TEvent->xbutton.y,
     &mx, &my, &none); */
 if (Ghost)
  DRAW_GHOST;
  
 while (!sortie)
 {
  XNextEvent(display, &Event);
  switch (Event.type)
  {
  case ButtonRelease:
   sortie = TRUE;
   break;
  case MotionNotify:
   if (Event.xmotion.window == This->RestrictionWindow)
   {	  
    cx = Event.xmotion.x;
    cy = Event.xmotion.y;
   }
   else
   {
    XQueryPointer(display, This->RestrictionWindow, &none, &none, 
		  &nihil, &nihil, &cx, &cy, &nihil);
   }
   if (Ghost)
    DRAW_GHOST;
	  
   realx += cx - mx;
   realy += cy - my;
   mx = cx;
   my = cy;
   if (Ghost)
    DRAW_GHOST;
   else
    XMoveWindow(display, listegwm->Win, realx, realy);
	  
   break;
  case Expose:
  case GraphicsExpose:
   XcHandleEvent(&Event);
   break;
  }
 }
 if (Ghost)
  DRAW_GHOST;
 if (raise)
  XRaiseWindow(display, listegwm->Win);
  
 XMoveWindow(display, listegwm->Win, realx, realy);
 XUngrabPointer(display , 0);
}

void GwmIconWindow(This, display, listegwm)
c_WinManager	*This;
Display		*display;
listmanaged	*listegwm;
{
 Window	WinTmp;
 int	  	none;
 int		deskX;
 int		deskY;
 int		placeY;
 int		placeX;
 int		nbrperline;
 int		nameSX;

 XUnmapWindow(display, listegwm->Win);
 /* ----------------------------------------------------------------- ** 
 ** place the icon                                                    ** 
 ** ----------------------------------------------------------------- */
 listegwm->Iconplace = QueryIconPlace(This);
 XQueryTree(display, listegwm->Icon, (Window *)&none, &WinTmp,
	    (Window **)&none, &none);
 XGetGeometry(display, WinTmp, (Window *)&none,
	      &none, &none,
	      &deskX, &deskY, &none, &none); 
 nbrperline = deskX / (ICONW+ICON_SPACE);
 placeY = listegwm->Iconplace / nbrperline;
 placeX = listegwm->Iconplace - (placeY*nbrperline);
 XMoveWindow(display, listegwm->Icon, (ICONW+ICON_SPACE)*placeX,
	     deskY-(ICONH+ICON_SPACEH)*(placeY+1));
 if (listegwm->Icon_Name)
 {
  XGetGeometry(display, listegwm->Icon_Name, (Window *)&none,
	       &none, &none,
	       &nameSX, &none, &none, &none); 
  if ((ICONW+ICON_SPACE)*placeX < (nameSX-(ICONW+ICON_SPACE))/2)
   XMoveWindow(display, listegwm->Icon_Name, 0, 
	       deskY-(ICONH+ICON_SPACEH)*(placeY)-ICONNAMEH+ICONNAMEMH);
  else
   if (((ICONW+ICON_SPACE)*(placeX+1) + (nameSX-(ICONW+ICON_SPACE))/2)
       > deskX)
    XMoveWindow(display, listegwm->Icon_Name, deskX - nameSX, 
		deskY-(ICONH+ICON_SPACEH)*(placeY)-ICONNAMEH+ICONNAMEMH);
      
   else
    XMoveWindow(display, listegwm->Icon_Name,
		(ICONW+ICON_SPACE)*placeX - 
		(nameSX-(ICONW+ICON_SPACE))/2,
		deskY-(ICONH+ICON_SPACEH)*(placeY)-ICONNAMEH+ICONNAMEMH);
 }
 listegwm->Type = D_ICONED;
 XLowerWindow(display, listegwm->Icon);
 XMapWindow(display, listegwm->Icon);
}


int QueryIconPlace(This)
c_WinManager	*This;
{
 listmanaged	*Ptr;
 int		found;
 int		num;
 int		*PlaceTab;

 Ptr = This->ListManaged;
 num = 0;
 if (This->nbrmanaged)
 {
  if ((PlaceTab = (int *)Xc_malloc("IconTab",
				   This->nbrmanaged*sizeof(int))) 
      == NULL)
  {
   Xc_FATAL(("Can't build this object: memory allocation error"));
  }
  memset(PlaceTab, 0, sizeof(int)*This->nbrmanaged);
  while (num < This->nbrmanaged)
  {
   if (Ptr->Type == D_ICONED)
    if (Ptr->Iconplace < This->nbrmanaged)
     PlaceTab[Ptr->Iconplace] = 1;
   Ptr = Ptr->NextManaged;
   num++;
  }
  found = 0;
  num = 0;
  while (num < This->nbrmanaged && !found)
  {
   if (PlaceTab[num] == 0) 
    found = 1;
   else
    num++;
  }
  Xc_free(PlaceTab);
 }
 return num;
}

int QueryNearIconPlace(This, place, nperline)
c_WinManager	*This;
int		place;
int		nperline;
{
 listmanaged	*Ptr;
 int		found;
 int		num = 0;
 int		*PlaceTab;
  
 Ptr = This->ListManaged;
 if (This->nbrmanaged)
 {
  if ((PlaceTab = (int *)Xc_malloc("IconTab",
				   This->nbrmanaged*sizeof(int))) 
      == NULL)
  {
   Xc_FATAL(("Can't build this object: memory allocation error"));
  }
  num = 0;
  found = 0;
  memset(PlaceTab, 0, sizeof(int)*This->nbrmanaged);
  while (num < This->nbrmanaged)
  {
   if (Ptr->Type == D_ICONED)
    if (Ptr->Iconplace == place)
    {
     found = 1;
    }
   if (Ptr->Iconplace < This->nbrmanaged)
    PlaceTab[Ptr->Iconplace] = 1;
   Ptr = Ptr->NextManaged;
   num++;
  }
      
  if (!found)
  {
   Xc_free(PlaceTab);
   return place;
  }
  num = 1;
  while (1)
  {
   if (PlaceTab[MIN(MAX(place-num, 0), This->nbrmanaged)] == 0)
   {
    Xc_free(PlaceTab);
    return place-num;
   }
   if (PlaceTab[MIN(place+num, This->nbrmanaged-1)] == 0)
   {
    Xc_free(PlaceTab);
    return place+num;
   }
   num++;
  }
 }
 return num;
}

void GwmUnMaxWindow(This, display, listegwm)
c_WinManager	*This;
Display		*display;
listmanaged	*listegwm;
{
 XMoveResizeWindow(display, listegwm->MainWin,
		   RESIZE_WIDTH+1, BUTTON_SIZEY + 1,
		   listegwm->Normal_w - wmADDEDX -2,
		   listegwm->Normal_h - wmADDEDY);
 XMoveResizeWindow(display, listegwm->Win,
		   listegwm->Normal_x, listegwm->Normal_y,
		   listegwm->Normal_w, listegwm->Normal_h);
 XRaiseWindow(display, listegwm->Win);
 XUnmapWindow(display, listegwm->Use->UnMaxWin);
 XMapWindow(display, listegwm->Use->CloseWin);
 XMapWindow(display, listegwm->Use->TitleWin);
 XMapWindow(display, listegwm->Use->MaxWin);
 XMapWindow(display, listegwm->Use->IconWin);
 XMapWindow(display, listegwm->Use->ResizeBottomWin);
 XMapWindow(display, listegwm->Use->ResizeRightWin);
 XMapWindow(display, listegwm->Use->ResizeLeftWin);
 XMapWindow(display, listegwm->Use->ResizeCornerRightWin);
 XMapWindow(display, listegwm->Use->ResizeCornerLeftWin);
 listegwm->Type = D_NORMAL;
}

void GwmMaxWindow(This, display, listegwm, window1)
c_WinManager	*This;
Display		*display;
listmanaged	*listegwm;
Window		window1;
{
 int	none;
 int 	height;
 int	width;

 /* ----------------------------------------------------------------- ** 
 ** store old size                                                    ** 
 ** ----------------------------------------------------------------- */
 XGetGeometry(display, listegwm->Win, (Window *)&none,
	      &(listegwm->Normal_x), &(listegwm->Normal_y),
	      &(listegwm->Normal_w), &(listegwm->Normal_h), &none, &none);

 /* ----------------------------------------------------------------- ** 
 ** change window size                                                ** 
 ** ----------------------------------------------------------------- */
 XGetGeometry(display, window1, (Window *)&none,
	      &none, &none, &width, &height, &none, &none);
 XMoveResizeWindow(display, listegwm->MainWin, 0, 0,
		   width, height);
 XMoveResizeWindow(display, listegwm->Win, 0, 0,
		   width, height);
 XRaiseWindow(display, listegwm->Win);
 XUnmapWindow(display, listegwm->Use->CloseWin);
 XUnmapWindow(display, listegwm->Use->TitleWin);
 XUnmapWindow(display, listegwm->Use->MaxWin);
 XUnmapWindow(display, listegwm->Use->IconWin);
 XUnmapWindow(display, listegwm->Use->ResizeBottomWin);
 XUnmapWindow(display, listegwm->Use->ResizeRightWin);
 XUnmapWindow(display, listegwm->Use->ResizeLeftWin);
 XUnmapWindow(display, listegwm->Use->ResizeCornerRightWin);
 XUnmapWindow(display, listegwm->Use->ResizeCornerLeftWin);
 XMoveWindow(display, listegwm->Use->UnMaxWin, width-BUTTON_UMAXW, 0);
 XMapRaised(display, listegwm->Use->UnMaxWin);

 listegwm->Type = D_MAXIMIZED;
}

void GwmResizeLeftWindow(This, display, TEvent, listegwm)
c_WinManager	*This;
Display		*display;
XEvent		*TEvent;
listmanaged	*listegwm;
{
 XEvent	Event;
 Window 	MouseW;
 Window	none;
 int	mx, my, cx, nihil;
 int	realx, realy;
 int	width, height;
 int	Rwidth, Rheight;
 int	bord, level;
 XSetWindowAttributes attributes;
 boolean	sortie = FALSE;
 boolean	raise = FALSE;
 boolean	opaque_resize = FALSE;
 char		*opaque;
  
 attributes.background_pixmap = None;
 XGetGeometry(display, This->RestrictionWindow, &none,
	      &realx, &realy, &Rwidth, &Rheight, &bord, &level);
 XGetGeometry(display, listegwm->Win, &none,
	      &realx, &realy, &width, &height, &bord, &level);
 MouseW = XCreateWindow(display, This->RestrictionWindow, 
			0, 0, 
			realx+width-WINMINX, Rheight,
			0, level, 
			CopyFromParent, CopyFromParent, 
			CWBackPixmap, &attributes);
 XLowerWindow(display, MouseW);
  
 opaque = F(GlobResources).getString(GlobResources, XcR_opaqueResize);
 if (Xstrcmp(opaque, "Yes") || Xstrcmp(opaque, "yes") ||
     Xstrcmp(opaque, "On") || Xstrcmp(opaque, "on") || Xstrcmp(opaque, "1") ||
     Xstrcmp(opaque, "True") || Xstrcmp(opaque, "true"))
  opaque_resize = TRUE;
    
 if (TEvent->xbutton.button == 1)
 {
  if (opaque_resize)
   XRaiseWindow(display, listegwm->Win);
  else
   raise = TRUE;
 }
 XQueryPointer(display, This->RestrictionWindow, &none, &none, 
	       &nihil, &nihil, &mx, &my, &nihil);
 /*  XTranslateCoordinates(display, TEvent->xbutton.window, 
     This->RestrictionWindow,
     TEvent->xbutton.x, TEvent->xbutton.y,
     &mx, &my, &none); */
  
 XMapWindow(display, MouseW);
  
 XGrabPointer(display, This->RestrictionWindow,
	      False, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask, 
	      GrabModeAsync, GrabModeAsync,
	      MouseW, This->cursor, 0);
 if (!opaque_resize)  
  DRAW_GHOST;
 while (!sortie)
 {
  XNextEvent(display, &Event);
  switch (Event.type)
  {
  case ButtonRelease:
   sortie = TRUE;
   break;
  case MotionNotify:
   if (Event.xmotion.window == This->RestrictionWindow)
   {
    cx = Event.xmotion.x;
   }
   else
   {
    XQueryPointer(display, This->RestrictionWindow, &none, &none, 
		  &nihil, &nihil, &cx, &nihil, &nihil);
   }
   if (!opaque_resize)
    DRAW_GHOST;
   realx -= mx - cx;
   width += mx - cx;
   mx = cx;
   if (!opaque_resize)
    DRAW_GHOST;
   else
   {
    XMoveResizeWindow(display, listegwm->Win,
		      realx, realy, width, height);
    height -= wmADDEDY;
    width -= wmADDEDX;
    XResizeWindow(display, listegwm->Use->TitleWin,
		  wmtitleW, wmtitleH);
    XResizeWindow(display, listegwm->Use->ResizeBottomWin,
		  wmresizebottomW, wmresizebottomH);
    XResizeWindow(display, listegwm->MainWin,
		  wmmainW, wmmainH);
    height += wmADDEDY;
    width += wmADDEDX;
   }
   break;
  case Expose:
  case GraphicsExpose:
  case ConfigureNotify:
   XcHandleEvent(&Event);
   break; 
  }
 }
 if (!opaque_resize)
  DRAW_GHOST;

 XUngrabPointer(display, 0);
 XDestroyWindow(display, MouseW); 
  
 if (raise)
  XRaiseWindow(display, listegwm->Win);
  
 XMoveResizeWindow(display, listegwm->Win,
		   realx, realy, width, height);
 height -= wmADDEDY;
 width -= wmADDEDX;
 XResizeWindow(display, listegwm->Use->TitleWin,
	       wmtitleW, wmtitleH);
 XResizeWindow(display, listegwm->Use->ResizeBottomWin,
	       wmresizebottomW, wmresizebottomH);
 XResizeWindow(display, listegwm->MainWin,
	       wmmainW, wmmainH);  
}

void GwmResizeRightWindow(This, display, TEvent, listegwm)
c_WinManager	*This;
Display		*display;
XEvent		*TEvent;
listmanaged	*listegwm;
{
 XEvent	Event;
 Window 	MouseW;
 Window	none;
 int	mx, my, cx, nihil;
 int	realx, realy;
 int	width, height;
 int	Rwidth, Rheight;
 int	bord, level;
 XSetWindowAttributes attributes;
 boolean	sortie = FALSE;
 boolean	opaque_resize = FALSE;
 boolean	raise = FALSE;
 char		*opaque;
  
 attributes.background_pixmap = None;
  
 XGetGeometry(display, This->RestrictionWindow, &none, 
	      &realx, &realy, &Rwidth, &Rheight, &bord, &level);
 XGetGeometry(display, listegwm->Win, &none,
	      &realx, &realy, &width, &height, &bord, &level);
  
 MouseW = XCreateWindow(display, This->RestrictionWindow,
			realx+WINMINX, 0,
			Rwidth-realx, Rheight,
			0, level, 
			CopyFromParent, CopyFromParent, 
			CWBackPixmap, &attributes);
  
 XLowerWindow(display, MouseW);

 opaque = F(GlobResources).getString(GlobResources, XcR_opaqueResize);
 if (Xstrcmp(opaque, "Yes") || Xstrcmp(opaque, "yes") ||
     Xstrcmp(opaque, "On") || Xstrcmp(opaque, "on") || Xstrcmp(opaque, "1") ||
     Xstrcmp(opaque, "True") || Xstrcmp(opaque, "true"))
  opaque_resize = TRUE;
    
 if (TEvent->xbutton.button == 1)
 {
  if (opaque_resize)
   XRaiseWindow(display, listegwm->Win);
  else
   raise = TRUE;
 }
 XQueryPointer(display, This->RestrictionWindow, &none, &none, 
	       &nihil, &nihil, &mx, &my, &nihil);
 /*  XTranslateCoordinates(display, TEvent->xbutton.window, 
     This->RestrictionWindow,
     TEvent->xbutton.x, TEvent->xbutton.y,
     &mx, &my, &none);*/
  
 XMapWindow(display, MouseW); 
  
 XGrabPointer(display, This->RestrictionWindow,
	      False, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
	      GrabModeAsync, GrabModeAsync,
	      MouseW, This->cursor, TEvent->xbutton.time);
 if (!opaque_resize)
  DRAW_GHOST;
 while (!sortie)
 {
  XNextEvent(display, &Event);
  switch (Event.type)
  {
  case ButtonRelease:
   sortie = TRUE;
   break;
  case MotionNotify:
   if (Event.xmotion.window == This->RestrictionWindow)
   {
    cx = Event.xmotion.x;
   }
   else
   {
    XQueryPointer(display, This->RestrictionWindow, &none, &none, 
		  &nihil, &nihil, &cx, &nihil, &nihil);
   }
   if (!opaque_resize)
    DRAW_GHOST;
   width += cx - mx;
   mx = cx;
   if (!opaque_resize)
    DRAW_GHOST;
   else
   {
    XResizeWindow(display, listegwm->Win, width, height);
    height -= wmADDEDY;
    width -= wmADDEDX;
    XResizeWindow(display, listegwm->Use->TitleWin,
		  wmtitleW, wmtitleH);
    XResizeWindow(display, listegwm->Use->ResizeBottomWin,
		  wmresizebottomW, wmresizebottomH);
    XResizeWindow(display, listegwm->MainWin,
		  wmmainW, wmmainH);
    height += wmADDEDY;
    width += wmADDEDX;
   }
   break;
  case Expose:
  case GraphicsExpose:
  case ConfigureNotify:
   XcHandleEvent(&Event);
   break;
  }
 }
 if (!opaque_resize)
  DRAW_GHOST;

 XUngrabPointer(display, Event.xbutton.time);
 XDestroyWindow(display, MouseW);  
  
 if (raise)
  XRaiseWindow(display, listegwm->Win);

 XResizeWindow(display, listegwm->Win, width, height);
 height -= wmADDEDY;
 width -= wmADDEDX;
 XResizeWindow(display, listegwm->Use->TitleWin,
	       wmtitleW, wmtitleH);
 XResizeWindow(display, listegwm->Use->ResizeBottomWin,
	       wmresizebottomW, wmresizebottomH);
 XResizeWindow(display, listegwm->MainWin,
	       wmmainW, wmmainH);  
}


void GwmResizeCornerLeftWindow(This, display, TEvent, listegwm)
c_WinManager	*This;
Display		*display;
XEvent		*TEvent;
listmanaged	*listegwm;
{
 XEvent	Event;
 Window 	MouseW;
 Window	none;
 int	mx, my, cx, cy, nihil;
 int	realx, realy;
 int	width, height;
 int	Rwidth, Rheight;
 int	bord, level;
 XSetWindowAttributes attributes;
 boolean	sortie = FALSE;
 boolean	opaque_resize = FALSE;
 boolean	raise = FALSE;
 char		*opaque;
  
 attributes.background_pixmap = None;
  
 XGetGeometry(display, This->RestrictionWindow, &none,
	      &realx, &realy, &Rwidth, &Rheight, &bord, &level);
 XGetGeometry(display, listegwm->Win, &none,
	      &realx, &realy, &width, &height, &bord, &level);
 MouseW = XCreateWindow(display, This->RestrictionWindow, 
			0, realy+WINMINY,
			realx+width-WINMINX, Rheight-realy,
			0, level, 
			CopyFromParent, CopyFromParent, 
			CWBackPixmap, &attributes);
 XLowerWindow(display, MouseW);
  
 opaque = F(GlobResources).getString(GlobResources, XcR_opaqueResize);
 if (Xstrcmp(opaque, "Yes") || Xstrcmp(opaque, "yes") ||
     Xstrcmp(opaque, "On") || Xstrcmp(opaque, "on") || Xstrcmp(opaque, "1") ||
     Xstrcmp(opaque, "True") || Xstrcmp(opaque, "true"))
  opaque_resize = TRUE;
    
 if (TEvent->xbutton.button == 1)
 {
  if (opaque_resize)
   XRaiseWindow(display, listegwm->Win);
  else
   raise = TRUE;
 }
 XQueryPointer(display, This->RestrictionWindow, &none, &none, 
	       &nihil, &nihil, &mx, &my, &nihil);
 /*  XTranslateCoordinates(display, TEvent->xbutton.window, 
     This->RestrictionWindow,
     TEvent->xbutton.x, TEvent->xbutton.y,
     &mx, &my, &none); */

 XMapWindow(display, MouseW);

 XGrabPointer(display, This->RestrictionWindow,
	      False, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
	      GrabModeAsync, GrabModeAsync,
	      MouseW, This->cursor, 0);
 if (!opaque_resize)
  DRAW_GHOST;
 while (!sortie)
 {
  XNextEvent(display, &Event);
  switch (Event.type)
  {
  case ButtonRelease:
   sortie = TRUE;
   break;
  case MotionNotify:
   if (Event.xmotion.window == This->RestrictionWindow)
   {	  
    cx = Event.xmotion.x;
    cy = Event.xmotion.y;
   }
   else
   {
    XQueryPointer(display, This->RestrictionWindow, &none, &none, 
		  &nihil, &nihil, &cx, &cy, &nihil);
   }
   if (!opaque_resize)
    DRAW_GHOST;
   realx -= mx - cx;
   width += mx - cx;
   height -= my - cy;
   mx = cx;
   my = cy;
   if (!opaque_resize)
    DRAW_GHOST;
   else
   {
    XMoveResizeWindow(display, listegwm->Win,
		      realx, realy, width, height);
    height -= wmADDEDY;
    width -= wmADDEDX;
    XResizeWindow(display, listegwm->Use->TitleWin,
		  wmtitleW, wmtitleH);
    XResizeWindow(display, listegwm->Use->ResizeLeftWin,
		  wmresizelW, wmresizelH);
    XResizeWindow(display, listegwm->Use->ResizeRightWin,
		  wmresizerW, wmresizerH);
    XResizeWindow(display, listegwm->Use->ResizeBottomWin,
		  wmresizebottomW, wmresizebottomH);
    XResizeWindow(display, listegwm->MainWin,
		  wmmainW, wmmainH);
    height += wmADDEDY;
    width += wmADDEDX;
   }
   break;
  case Expose:
  case GraphicsExpose:
  case ConfigureNotify:
   XcHandleEvent(&Event);
   break;
  }
 }
 if (!opaque_resize)
  DRAW_GHOST;

 XUngrabPointer(display, 0);
 XDestroyWindow(display, MouseW); 
  
 if (raise)
  XRaiseWindow(display, listegwm->Win);
  
 XMoveResizeWindow(display, listegwm->Win,
		   realx, realy, width, height);
 height -= wmADDEDY;
 width -= wmADDEDX;
 XResizeWindow(display, listegwm->Use->TitleWin,
	       wmtitleW, wmtitleH);
 XResizeWindow(display, listegwm->Use->ResizeLeftWin,
	       wmresizelW, wmresizelH);
 XResizeWindow(display, listegwm->Use->ResizeRightWin,
	       wmresizerW, wmresizerH);
 XResizeWindow(display, listegwm->Use->ResizeBottomWin,
	       wmresizebottomW, wmresizebottomH);
 XResizeWindow(display, listegwm->MainWin,
	       wmmainW, wmmainH);
}

void GwmResizeCornerRightWindow(This, display, TEvent, listegwm)
c_WinManager	*This;
Display		*display;
XEvent		*TEvent;
listmanaged	*listegwm;
{
 XEvent	Event;
 Window 	MouseW;
 Window	none;
 int	mx, my, cx, cy, nihil;
 int	realx, realy;
 int	width, height;
 int	Rwidth, Rheight;
 int	bord, level;
 XSetWindowAttributes attributes;
 boolean	sortie = FALSE;
 boolean	raise = FALSE;
 boolean	opaque_resize = FALSE;
 char		*opaque;
  
 attributes.background_pixmap = None;

 XGetGeometry(display, This->RestrictionWindow, &none,
	      &realx, &realy, &Rwidth, &Rheight, &bord, &level);
 XGetGeometry(display, listegwm->Win, &none,
	      &realx, &realy, &width, &height, &bord, &level);
 MouseW = XCreateWindow(display, This->RestrictionWindow, 
			realx+WINMINX, realy+WINMINY,
			Rwidth-realx, Rheight-realy,
			0, level, 
			CopyFromParent, CopyFromParent, 
			CWBackPixmap, &attributes);
 XLowerWindow(display, MouseW);
  
 opaque = F(GlobResources).getString(GlobResources, XcR_opaqueResize);
 if (Xstrcmp(opaque, "Yes") || Xstrcmp(opaque, "yes") ||
     Xstrcmp(opaque, "On") || Xstrcmp(opaque, "on") || Xstrcmp(opaque, "1") ||
     Xstrcmp(opaque, "True") || Xstrcmp(opaque, "true"))
  opaque_resize = TRUE;
    
 if (TEvent->xbutton.button == 1)
 {
  if (opaque_resize)
   XRaiseWindow(display, listegwm->Win);
  else
   raise = TRUE;
 }
 XQueryPointer(display, This->RestrictionWindow, &none, &none, 
	       &nihil, &nihil, &mx, &my, &nihil);

 /*  XTranslateCoordinates(display, TEvent->xbutton.window, 
     This->RestrictionWindow,
     TEvent->xbutton.x, TEvent->xbutton.y,
     &mx, &my, &none); */
  
 XMapWindow(display, MouseW);

 XGrabPointer(display, This->RestrictionWindow,
	      False, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
	      GrabModeAsync, GrabModeAsync,
	      MouseW, This->cursor, 0);
 if (!opaque_resize)
  DRAW_GHOST;
 while (!sortie)
 {
  XNextEvent(display, &Event);
  switch (Event.type)
  {
  case ButtonPress:
  case ButtonRelease:
   sortie = TRUE;
   break;
  case MotionNotify:
   if (Event.xmotion.window == This->RestrictionWindow)
   {
    cx = Event.xmotion.x;
    cy = Event.xmotion.y;
   }
   else
   {
    XQueryPointer(display, This->RestrictionWindow, &none, &none, 
		  &nihil, &nihil, &cx, &cy, &nihil);
   }
   if (!opaque_resize)
    DRAW_GHOST;
   width += cx - mx;
   height += cy - my;
   mx = cx;
   my = cy;
   if (!opaque_resize)
    DRAW_GHOST;
   else
   {  
    XResizeWindow(display, listegwm->Win, width, height);
    height -= wmADDEDY;
    width -= wmADDEDX;
    XResizeWindow(display, listegwm->Use->TitleWin,
		  wmtitleW, wmtitleH);
    XResizeWindow(display, listegwm->Use->ResizeLeftWin,
		  wmresizelW, wmresizelH);
    XResizeWindow(display, listegwm->Use->ResizeRightWin,
		  wmresizerW, wmresizerH);
    XResizeWindow(display, listegwm->Use->ResizeBottomWin,
		  wmresizebottomW, wmresizebottomH);
    XResizeWindow(display, listegwm->MainWin,
		  wmmainW, wmmainH);
    height += wmADDEDY;
    width += wmADDEDX;
   }
   break;
  case Expose:
  case GraphicsExpose:
   /*	  if (Event.xany.window == MouseW)
	  {
	  XClearArea(display, This->RestrictionWindow,
	  Event.xexpose.x + realx+WINMINX, Event.xexpose.y, 
	  Event.xexpose.width, Event.xexpose.height, False);
	  break;
	  } */
  case ConfigureNotify:
   XcHandleEvent(&Event);
   break;  
  }
 }
 if (!opaque_resize)
  DRAW_GHOST;

 XUngrabPointer(display, 0);
 XDestroyWindow(display, MouseW); 
  
 if (raise)
  XRaiseWindow(display, listegwm->Win);
  
 XResizeWindow(display, listegwm->Win, width, height);
 height -= wmADDEDY;
 width -= wmADDEDX;
 XResizeWindow(display, listegwm->Use->TitleWin,
	       wmtitleW, wmtitleH);
 XResizeWindow(display, listegwm->Use->ResizeLeftWin,
	       wmresizelW, wmresizelH);
 XResizeWindow(display, listegwm->Use->ResizeRightWin,
	       wmresizerW, wmresizerH);
 XResizeWindow(display, listegwm->Use->ResizeBottomWin,
	       wmresizebottomW, wmresizebottomH);
 XResizeWindow(display, listegwm->MainWin,
	       wmmainW, wmmainH);  
}

void GwmResizeBottomWindow(This, display, TEvent, listegwm)
c_WinManager	*This;
Display		*display;
XEvent		*TEvent;	
listmanaged	*listegwm;
{
 XEvent	Event;
 Window 	MouseW;
 Window	none;
 int	mx, my, cy, nihil;
 int	realx, realy;
 int	width, height;
 int	Rwidth, Rheight;
 int	bord, level;
 XSetWindowAttributes attributes;
 boolean	sortie = FALSE;
 boolean	raise = FALSE;
 boolean	opaque_resize = FALSE;
 char		*opaque;
  
 attributes.background_pixmap = None;

 XGetGeometry(display, This->RestrictionWindow, &none,
	      &realx, &realy, &Rwidth, &Rheight, &bord, &level);
 XGetGeometry(display, listegwm->Win, &none,
	      &realx, &realy, &width, &height, &bord, &level);
 MouseW = XCreateWindow(display, This->RestrictionWindow, 
			0, realy+WINMINY,
			Rwidth, Rheight-realy,
			0, level, 
			CopyFromParent, CopyFromParent, 
			CWBackPixmap, &attributes);
 XLowerWindow(display, MouseW);
  
 opaque = F(GlobResources).getString(GlobResources, XcR_opaqueResize);
 if (Xstrcmp(opaque, "Yes") || Xstrcmp(opaque, "yes") ||
     Xstrcmp(opaque, "On") || Xstrcmp(opaque, "on") || Xstrcmp(opaque, "1") ||
     Xstrcmp(opaque, "True") || Xstrcmp(opaque, "true"))
  opaque_resize = TRUE;
  
 if (TEvent->xbutton.button == 1)
 {
  if (opaque_resize)
   XRaiseWindow(display, listegwm->Win);
  else
   raise = TRUE;
 }
 XQueryPointer(display, This->RestrictionWindow, &none, &none, 
	       &nihil, &nihil, &mx, &my, &nihil);
 /*  XTranslateCoordinates(display, TEvent->xbutton.window, 
     This->RestrictionWindow,
     TEvent->xbutton.x, TEvent->xbutton.y,
     &mx, &my, &none); */

 XMapWindow(display, MouseW);

 XGrabPointer(display, This->RestrictionWindow,
	      False, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
	      GrabModeAsync, GrabModeAsync,
	      MouseW, This->cursor, 0);
 if (!opaque_resize)
  DRAW_GHOST;
 while (!sortie)
 {
  XNextEvent(display, &Event);
  switch (Event.type)
  {
  case ButtonRelease:
   sortie = TRUE;
   break;
  case MotionNotify:
   if (Event.xmotion.window == This->RestrictionWindow)
   {	  
    cy = Event.xmotion.y;
   }
   else
   {
    XQueryPointer(display, This->RestrictionWindow, &none, &none, 
		  &nihil, &nihil, &nihil, &cy, &nihil);
   }
   if (!opaque_resize)
    DRAW_GHOST;
   height += cy - my; 
   my = cy;
   if (!opaque_resize)
    DRAW_GHOST;
   else
   {
    XResizeWindow(display, listegwm->Win, width, height);
    height -= wmADDEDY;
    width -= wmADDEDX;
    XResizeWindow(display, listegwm->Use->ResizeLeftWin,
		  wmresizelW, wmresizelH);
    XResizeWindow(display, listegwm->Use->ResizeRightWin,
		  wmresizerW, wmresizerH);
    XResizeWindow(display, listegwm->MainWin, 
		  wmmainW, wmmainH);
    height += wmADDEDY;
    width += wmADDEDX;
   }
   break;
  case Expose:
  case GraphicsExpose:
  case ConfigureNotify:
   XcHandleEvent(&Event);
   break;
  }
 }
 if (!opaque_resize)
  DRAW_GHOST;
  
 if (raise)
  XRaiseWindow(display, listegwm->Win);
  
 XUngrabPointer(display, 0);
 XDestroyWindow(display, MouseW); 

 XResizeWindow(display, listegwm->Win, width, height);
 height -= wmADDEDY;
 width -= wmADDEDX;
 XResizeWindow(display, listegwm->Use->ResizeLeftWin,
	       wmresizelW, wmresizelH);
 XResizeWindow(display, listegwm->Use->ResizeRightWin,
	       wmresizerW, wmresizerH);
 XResizeWindow(display, listegwm->MainWin, 
	       wmmainW, wmmainH);
}

void GwmDiselectWindow(This, display, WinCmp)
c_WinManager	*This;
Display		*display;
listmanaged	*WinCmp;
{
 Window	r_win;
 int		rx;
 int		ry;
 int		rw;
 int		rh;
 int		bord;
 int		level;
 listmanaged	*tmpmanaged;

 if (This->SelectCallBack)
  This->SelectCallBack(This->SelectedWindow, WinCmp);
 if (This->SelectedWindow && (This->SelectedWindow != WinCmp))
 {
  if (This->SelectedWindow->Type == D_MAXIMIZED)
   F(This).UnMaxWindow(This, display, This->SelectedWindow);
  F(WinCmp->cursor).unset_cursor(WinCmp->cursor);
  F(This->SelectedWindow->cursor).reset_cursor
   (This->SelectedWindow->cursor);
   
  tmpmanaged = This->SelectedWindow;
  This->SelectedWindow = WinCmp;
  XGetGeometry(display, tmpmanaged->MainWin, &r_win,
	       &rx, &ry,
	       &rw, &rh,
	       &bord, &level);
  F(This).DrawDecorP(This, tmpmanaged, 
		     display,
		     rw, rh, MGWMALL);
  XGetGeometry(display, WinCmp->MainWin, &r_win,
	       &rx, &ry,
	       &rw, &rh,
	       &bord, &level);
  F(This).DrawDecorP(This, WinCmp,
		     display,
		     rw, rh, MGWMALL);
 }
 This->SelectedWindow = WinCmp;
}

void GwmMIconWindow(This, display, MWindow, listegwm)
c_WinManager	*This;
Display		*display;
Window		MWindow;
listmanaged	*listegwm;
{
 XEvent	Event;
 Window	r_win;
 Window	c_win;

 int	mx;
 int	my;
 int	rx;
 int	ry;
 int	cx;
 int	cy;
 int	mk;

 int	realx;
 int	realy;
 int	width;
 int	height;
 int	Rwidth;
 int	Rheight;
 int	bord;
 int	level;
 int	sortie;
 int	nbrperline;
 int	placeX;
 int	placeY;
 int	Ghost;
 int	nameSX;
 int	none;

 Ghost = 1;
 sortie = 0;

 XGrabPointer(display, This->RestrictionWindow, 
	      True, ButtonPressMask, 
	      GrabModeAsync,GrabModeAsync, 
	      This->RestrictionWindow, This->cursor, 0); 

 XGetGeometry(display, This->RestrictionWindow, &r_win, 
	      &realx, &realy, 
	      &Rwidth, &Rheight, 
	      &bord, &level);
 XGetGeometry(display, MWindow, &r_win,
	      &realx, &realy,
	      &width, &height,
	      &bord, &level);

 XQueryPointer(display, This->RestrictionWindow, 
	       &c_win, &r_win, 
	       &rx, &ry, 
	       &mx, &my, 
	       &mk);
 if (Ghost)
  DRAW_GHOST;
 while (!sortie)
 {
  XNextEvent(display, &Event);
  switch (Event.type)
  {
  case Expose:
   if (Event.xany.window == MWindow)
    F(This).DrawIconDecor(This, listegwm, display);
   break;
  case ButtonRelease:
   XQueryPointer(display, This->RestrictionWindow,
		 &c_win, &r_win, 
		 &rx, &ry, 
		 &cx, &cy,
		 &mk);
   sortie = 1;
   break;
  case MotionNotify:
   XQueryPointer(display, This->RestrictionWindow,
		 &c_win, &r_win, 
		 &rx, &ry, 
		 &cx, &cy,
		 &mk);
   if (Ghost)
    DRAW_GHOST;
   realx -= mx - cx;
   realy -= my - cy;
   mx = cx;
   my = cy;
   if (Ghost)
    DRAW_GHOST;
   /*
     XMoveWindow(display, MWindow, realx, realy);
     */
  }
 }
 if (Ghost)
  DRAW_GHOST;

 /* ----------------------------------------------------------------- ** 
 ** change cordinate to case store                                    ** 
 ** ----------------------------------------------------------------- */
 nbrperline = Rwidth / (ICONW+ICON_SPACE);
 listegwm->Iconplace = (cx / (ICONW+ICON_SPACE)) + ((Rheight-cy)/(ICONH+ICON_SPACEH))*nbrperline;
 listegwm->Type = D_NORMAL;
 listegwm->Iconplace = QueryNearIconPlace(This,
					  listegwm->Iconplace,
					  nbrperline);
 placeY = listegwm->Iconplace / nbrperline;
 placeX = listegwm->Iconplace - (placeY*nbrperline);
 XMoveWindow(display, MWindow, (ICONW+ICON_SPACE)*placeX, Rheight-(ICONH+ICON_SPACEH)*(placeY+1));
 if (listegwm->Icon_Name)
 {
  XGetGeometry(display, listegwm->Icon_Name, (Window *)&none,
	       &none, &none,
	       &nameSX, &none, &none, &none); 
  if ((ICONW+ICON_SPACE)*placeX < (nameSX-(ICONW+ICON_SPACE))/2)
   XMoveWindow(display, listegwm->Icon_Name, 0, 
	       Rheight-(ICONH+ICON_SPACEH)*(placeY)-ICONNAMEH+ICONNAMEMH);
  else
   if ((ICONW+ICON_SPACE)*(placeX+1) + (nameSX-(ICONW+ICON_SPACE))/2 
       > Rwidth)
    XMoveWindow(display, listegwm->Icon_Name, Rwidth - nameSX, 
		Rheight-(ICONH+ICON_SPACEH)*(placeY)-ICONNAMEH
		+ICONNAMEMH);
   else
    XMoveWindow(display, listegwm->Icon_Name,
		(ICONW+ICON_SPACE)*placeX 
		- (nameSX-(ICONW+ICON_SPACE))/2,
		Rheight-(ICONH+ICON_SPACEH)*(placeY)-ICONNAMEH
		+ICONNAMEMH);
      
 }
 listegwm->Type = D_ICONED;
 XUngrabPointer(display , 0);
}

void GwmRaiseIcon(This, display, listegwm)
c_WinManager	*This;
Display		*display;
listmanaged	*listegwm;
{
 listegwm->Type = D_NORMAL;
 XUnmapWindow(display, listegwm->Icon);
 if (listegwm->Icon_Name)
  XUnmapWindow(display, listegwm->Icon_Name);
 XRaiseWindow(display, listegwm->Win);
 XMapWindow(display, listegwm->Win);
}

void GwmSetWindowName(This, display, window, name)
c_WinManager	*This;
Display		*display;
Window		window;
char		*name;
{
 Window	r_win;
 int		rx, ry,	rw, rh;
 int		bord, level;
 listmanaged	*listegwm;

 XStoreName(display, window, name);
  
 listegwm = This->ListManaged;
 while(listegwm && listegwm->MainWin != window)
  listegwm = listegwm->NextManaged;
  
 if (listegwm)
 {
  if (listegwm->WinName) Xc_free(listegwm->WinName);
  listegwm->WinName = (char *)Xc_strdup("WinName", name);
  XGetGeometry(display, window, &r_win, &rx, &ry, &rw, &rh, &bord, &level);
  F(This).DrawDecorP(This, listegwm, display, rw, rh, MGWMTITLE);
 }
}

void GwmDrawIconDecor(This, managed, display)
c_WinManager	*This;
listmanaged	*managed;
Display		*display;
{
 int		Tlength;
 XFontStruct	*fnt_struct;
 GC		gc_shadow;
 int		*Color;
 XGCValues 	xgcv;

 ((managed == This->SelectedWindow )? 
  (Color = This->Color) : (Color = This->SColor) );
  
 xgcv.foreground = HIGHLIGHT_COLOR;
 xgcv.line_width = 0;
 xgcv.font = F(GlobResources).getFont(GlobResources, XcR_iconFont);
 gc_shadow = XCreateGC(display, managed->Icon,
		       GCLineWidth | GCForeground | GCFont,
		       &xgcv);

 XClearWindow(display, managed->Icon);

 if (!managed->Icon_Name)
 {
  fnt_struct = F(GlobResources).getXFontStruct(GlobResources, 
					       XcR_iconFont);
  Tlength = XTextWidth(fnt_struct, managed->WinName,
		       strlen(managed->WinName));
  XDrawString(display, managed->Icon, gc_shadow, 
	      (ICONW - Tlength)/2, ICONH - 4, 
	      managed->WinName, strlen(managed->WinName));
 }
 else
  XDrawString(display, managed->Icon, gc_shadow, 
	      2, ICONH - 4, 
	      managed->WinName, strlen(managed->WinName));
}

void GwmDrawIconNameDecor(This, managed, display)
c_WinManager	*This;
listmanaged	*managed;
Display		*display;
{
 GC		gc_highlight;
 int		*Color;
 XGCValues 	xgcv;
 int		width;
 XPoint	point[3];
 XRectangle    rect;
  
 ((managed == This->SelectedWindow )? 
  (Color = This->Color) : (Color = This->SColor) );
  
 xgcv.foreground = HIGHLIGHT_COLOR;
 xgcv.line_width = 0;
 xgcv.font = F(GlobResources).getFont(GlobResources, XcR_iconFont);

 if (managed->Icon_Name)
 {
  gc_highlight = XCreateGC(display, managed->Icon_Name,
			   GCLineWidth | GCForeground | GCFont,
			   &xgcv);
      
  XClearWindow(display, managed->Icon_Name);
      
  XDrawString(display, managed->Icon_Name, gc_highlight, 
	      2, ICONNAMEH - 4,
	      managed->WinName, strlen(managed->WinName));
      
  width = XTextWidth(F(GlobResources).getXFontStruct(GlobResources, 
						     XcR_iconFont),
		     managed->WinName, 
		     strlen(managed->WinName)) + 4;
      
  XSetForeground(display, gc_highlight, HIGHLIGHT_COLOR);
  point[0].x = 0;
  point[1].x = point[2].x = width - 1;
  point[0].y = point[1].y = ICONNAMEH - 1;
  point[2].y = 0;
  XDrawLines(display, managed->Icon_Name, gc_highlight, point, 3, 
	     CoordModeOrigin);
  XSetForeground(display, gc_highlight, 
		 BlackPixel(display, DefaultScreen(display)));
  point[0].x = point[1].x = 0;
  point[2].x = width - 1;
  point[0].y = ICONNAMEH - 1;
  point[1].y = point[2].y = 0;
  XDrawLines(display, managed->Icon_Name, gc_highlight, point, 3, 
	     CoordModeOrigin);
 }
 else
 {
  gc_highlight = XCreateGC(display, managed->Icon,
			   GCLineWidth | GCForeground | GCFont,
			   &xgcv);
  rect.x = 2;
  rect.y = ICONH - ICONNAMEH;
  rect.width =
   XTextWidth(F(GlobResources).
	      getXFontStruct(GlobResources, XcR_iconFont),
	      managed->WinName, strlen(managed->WinName)) + 4;
  rect.height = ICONNAMEH;
  XSetClipRectangles(display, gc_highlight, 0, 0, &rect, 1, Unsorted);
  XDrawString(display, managed->Icon, gc_highlight, 
	      2, ICONH - 4, managed->WinName, strlen(managed->WinName));
  XSetClipMask(display, gc_highlight, None);
 }

 XFreeGC(display, gc_highlight);
}

void GwmArrangeIcon(This, display, WinTmp)
c_WinManager	*This;
Display		*display;
Window		WinTmp;
{
 int		none;
 int		deskX;
 int		deskY;
 int		placeY;
 int		placeX;
 int		nbrperline;
 listmanaged	*Ptr;
 int		num;
 int		nameSX;
  
 Ptr = This->ListManaged;
 XGetGeometry(display, WinTmp, (Window *)&none,
	      &none, &none,
	      &deskX, &deskY, &none, &none);
 nbrperline = deskX / (ICONW+ICON_SPACE);
 if (!nbrperline)
  nbrperline = 1;
 num = 0;
 while (num < This->nbrmanaged)
 {
  if (Ptr->Type == D_ICONED)
  {
   placeY = Ptr->Iconplace / nbrperline;
   placeX = Ptr->Iconplace - (placeY*nbrperline);
   XMoveWindow(display, Ptr->Icon,
	       (ICONW + ICON_SPACE) * placeX,
	       deskY - (ICONH + ICON_SPACEH) * (placeY + 1));
   if (Ptr->Icon_Name)
   {
    XGetGeometry(display, Ptr->Icon_Name, (Window *)&none,
		 &none, &none,
		 &nameSX, &none, &none, &none); 
    if ((ICONW+ICON_SPACE)*placeX < (nameSX-(ICONW+ICON_SPACE))/2)
     XMoveWindow(display, Ptr->Icon_Name, 0, 
		 deskY-(ICONH+ICON_SPACEH)*(placeY)-ICONNAMEH
		 +ICONNAMEMH);
    else
     if (((ICONW+ICON_SPACE)*(placeX+1) +
	  (nameSX-(ICONW+ICON_SPACE))/2) > deskX)
      XMoveWindow(display, Ptr->Icon_Name, deskX - nameSX, 
		  deskY-(ICONH+ICON_SPACEH)*(placeY)-ICONNAMEH
		  +ICONNAMEMH);
	      
     else
      XMoveWindow(display, Ptr->Icon_Name,
		  (ICONW+ICON_SPACE)*placeX - 
		  (nameSX-(ICONW+ICON_SPACE))/2,
		  deskY-(ICONH+ICON_SPACEH)*(placeY)-ICONNAMEH
		  +ICONNAMEMH);
   }

  }
  Ptr = Ptr->NextManaged;
  num++;
 }
}

#define CASCADEX	20
#define CASCADEY	20

void GwmArrangeWindow(This, display, WinTmp)
c_WinManager	*This;
Display		*display;
Window		WinTmp;
{
 int		none;
 int		deskX;
 int		deskY;
 int		placeY;
 int		placeX;
 int		nbrwindow;
 listmanaged	*Ptr;
 int		num;
 int		width;
 int		height;
 int		maxwindow;
  
 Ptr = This->ListManaged;
 XGetGeometry(display, WinTmp, (Window *)&none,
	      &none, &none,
	      &deskX, &deskY, &none, &none);

 nbrwindow = 0;
 num = 0;
 while (num < This->nbrmanaged)
 {
  if (Ptr->Type != D_ICONED)
  {
   XUnmapWindow(display, WinTmp);
   if (Ptr->Type == D_MAXIMIZED)
    F(This).UnMaxWindow(This, display, Ptr);
   nbrwindow++;
  }
  Ptr = Ptr->NextManaged;
  num ++;
 }

 if (nbrwindow)
 {
  maxwindow = ((deskY-WINMINY)/CASCADEY)+1;
  if (maxwindow > nbrwindow)
  {
   width = deskX - (nbrwindow-1)*CASCADEX - wmADDEDX;
   height = deskY - (nbrwindow-1)*CASCADEY - wmADDEDY;
  }
  else
  {
   width = deskX - (nbrwindow-1)*CASCADEX - wmADDEDX;
   height = WINMINY - wmADDEDY;
  }
  placeY = 0;
  placeX = 0;
  num = 0;
  Ptr = This->ListManaged;
  while (num < This->nbrmanaged)
  {
   if (Ptr->Type == D_NORMAL)
   {
    XRaiseWindow(display, Ptr->Win);
    XMoveResizeWindow(display, Ptr->Win,
		      placeX, placeY,
		      width + wmADDEDX, height + wmADDEDY);
    XResizeWindow(display, Ptr->Use->TitleWin,
		  wmtitleW, wmtitleH);
    XMoveWindow(display, Ptr->Use->IconWin,
		wmiconX, wmiconY);
    XMoveWindow(display, Ptr->Use->MaxWin,
		wmmaxX, wmmaxY);
    XResizeWindow(display, Ptr->Use->ResizeLeftWin,
		  wmresizelW, wmresizelH);
    XMoveResizeWindow(display, Ptr->Use->ResizeRightWin,
		      wmresizerX, wmresizerY,
		      wmresizerW, wmresizerH);
    XMoveWindow(display, Ptr->Use->ResizeCornerLeftWin,
		wmresizecornerlX, wmresizecornerlY);
    XMoveWindow(display, Ptr->Use->ResizeCornerRightWin, 
		wmresizecornerrX, wmresizecornerrY);
    XMoveResizeWindow(display, Ptr->Use->ResizeBottomWin,
		      wmresizebottomX, wmresizebottomY,
		      wmresizebottomW, wmresizebottomH);
    XResizeWindow(display, Ptr->MainWin,
		  wmmainW, wmmainH);
    placeX += CASCADEX;
    placeY += CASCADEY;
   }
   Ptr = Ptr->NextManaged;
   num++;
  }
  Ptr = This->ListManaged;
  num = 0;
  while (num < This->nbrmanaged)
  {
   if (Ptr->Type == D_NORMAL)
    XMapWindow(display, WinTmp);
   Ptr = Ptr->NextManaged;
   num ++;
  }
 }
}

void GwmTileWindow(This, display, WinTmp)
c_WinManager	*This;
Display		*display;
Window		WinTmp;
{
 int		none;
 int		deskX;
 int		deskY;
 int		placeY;
 int		placeX;
 int		nbrwindow;
 listmanaged	*Ptr;
 int		num;
 int		width;
 int		height;

 Ptr = This->ListManaged;
 XGetGeometry(display, WinTmp, (Window *)&none,
	      &none, &none,
	      &deskX, &deskY, &none, &none);
  
 nbrwindow = 0;
 num = 0;
 while (num < This->nbrmanaged)
 {
  if (Ptr->Type != D_ICONED)
  {
   XUnmapWindow(display, WinTmp);
   if (Ptr->Type == D_MAXIMIZED)
    F(This).UnMaxWindow(This, display, Ptr);
   nbrwindow++;
  }
  Ptr = Ptr->NextManaged;
  num ++;
 }

 if (nbrwindow)
 {
  int nbrperline;
  int numforline;
  int nbrline;
  int numline;
  int lastline;
      
  lastline = 0;
  nbrperline = sqrt((double)nbrwindow);
  nbrline = nbrperline;
  if (nbrperline*nbrline != nbrwindow)
  {
   nbrperline++;
   while ((nbrperline*nbrline) > nbrwindow)
    nbrline--;
   lastline = (nbrwindow - nbrperline*nbrline);
  }
  width = deskX/(nbrperline) - wmADDEDX;
  if (lastline)
   height = deskY/(nbrline+1) - wmADDEDY;
  else
   height = deskY/(nbrline) - wmADDEDY;
      
  placeY = 0;
  placeX = 0;
  num = 0;
  Ptr = This->ListManaged;
  numforline = 0;
  numline = 0;
  while (num < This->nbrmanaged)
  {
   if (Ptr->Type == D_NORMAL)
   {
    XRaiseWindow(display, Ptr->Win);
    XMoveResizeWindow(display, Ptr->Win,
		      placeX, placeY,
		      width+wmADDEDX, height+wmADDEDY);
    XResizeWindow(display, Ptr->Use->TitleWin,
		  wmtitleW, wmtitleH);
    XMoveWindow(display, Ptr->Use->IconWin,
		wmiconX, wmiconY);
    XMoveWindow(display, Ptr->Use->MaxWin,
		wmmaxX, wmmaxY);
    XResizeWindow(display, Ptr->Use->ResizeLeftWin,
		  wmresizelW, wmresizelH);
    XMoveResizeWindow(display, Ptr->Use->ResizeRightWin,
		      wmresizerX, wmresizerY,
		      wmresizerW, wmresizerH);
    XMoveWindow(display, Ptr->Use->ResizeCornerLeftWin,
		wmresizecornerlX, wmresizecornerlY);
    XMoveWindow(display, Ptr->Use->ResizeCornerRightWin, 
		wmresizecornerrX, wmresizecornerrY);
    XMoveResizeWindow(display, Ptr->Use->ResizeBottomWin,
		      wmresizebottomX, wmresizebottomY,
		      wmresizebottomW, wmresizebottomH);
    XResizeWindow(display, Ptr->MainWin,
		  wmmainW, wmmainH);
    placeX += width+wmADDEDX;
    numforline++;
    if (numforline == nbrperline)
    {
     numline++;
     if ((numline == nbrline) && lastline)
     {
      width = deskX / (lastline) - wmADDEDX;
     }
     numforline = 0;
     placeX = 0;
     placeY += height+wmADDEDY;
    }
   }
   Ptr = Ptr->NextManaged;
   num++;
  }
  Ptr = This->ListManaged;
  num = 0;
  while (num < This->nbrmanaged)
  {
   if (Ptr->Type == D_NORMAL)
    XMapWindow(display, WinTmp);
   Ptr = Ptr->NextManaged;
   num ++;
  }
 }
}

int Gwmeventlook(This, display, Event, listegwm)
c_WinManager	*This;
Display		*display;
XEvent		*Event;
listmanaged	*listegwm;
{
 int		mask;

 while (1)
 {
  mask = 0;
  /**********************/
  /* analysing Managers */
  /**********************/
  if (Event->xany.window == listegwm->MainWin)
  {
   if (Event->type == ConfigureNotify)
    return MOD_STRUCT;
   return NO_EVENT;
  }
  /*************************/
  /* fermeture du document */
  /*************************/
  if (Event->xany.window == listegwm->Use->CloseWin)
  {
   if (Event->type == Expose || Event->type == GraphicsExpose)
   {
    mask |= MGWMCLOSE;
    break;
   }
   if (Event->type == ButtonPress)
   {
    F(This).DiselectWindow(This, display, listegwm);
    return CLOSE_DOC;
   }
   return INTERN_EVENT;
  }
  /*********************/
  /* Titre du document */
  /*********************/
  if (Event->xany.window == listegwm->Use->TitleWin)
  {
   if (Event->type == Expose || Event->type == GraphicsExpose)
   {
    mask |= MGWMTITLE;
    break;
   }
   if (Event->type == ButtonPress)
   {
    F(This).DiselectWindow(This, display, listegwm);
    F(This).TitleWindow(This, display, Event, listegwm);
   }
   return INTERN_EVENT;
  }
  /*****************************/
  /* Iconification du document */
  /*****************************/
  if (Event->xany.window == listegwm->Use->IconWin)
  {
   if (Event->type == Expose || Event->type == GraphicsExpose)
   {
    mask |= MGWMICON;
    break;
   }
   if (Event->type == ButtonPress)
   {
    int		Tnum;
    listmanaged	*Tlistegwm;
    int		AnotherW;
	      
    if (This->SelectedWindow == listegwm)
    {
     Tnum = This->nbrmanaged;
     Tlistegwm = This->ListManaged;
     AnotherW = 0;
     while (Tnum--)
     {
      if ((Tlistegwm->Type == D_NORMAL) && 
	  (Tlistegwm != listegwm))
      {
       AnotherW = 1;
       Tnum = 0;
      }
      else
       Tlistegwm = Tlistegwm->NextManaged;
     }
     if (AnotherW)
     {
      F(This).DiselectWindow(This, display, Tlistegwm);
     }
     else
      This->SelectedWindow = NULL;
    }
    F(This).IconWindow(This, display, listegwm);
   }
   return INTERN_EVENT;
  }
  /************************/
  /* Maximize du document */
  /************************/
  if (Event->xany.window == listegwm->Use->MaxWin)
  {
   if (Event->type == Expose || Event->type == GraphicsExpose)
   {
    mask |= MGWMMAX;
    break;
   }
   if (Event->type == ButtonPress)
   {
    F(This).DiselectWindow(This, display, listegwm);
    return MAX_DOC;
   }
   return INTERN_EVENT;
  }
  /*********************/
  /* Unmax du document */
  /*********************/
  if (Event->xany.window == listegwm->Use->UnMaxWin)
  {
   if (Event->type == Expose || Event->type == GraphicsExpose)
   {
    mask |= MGWMUNMAX;
    break;
   }
   if (Event->type == ButtonPress)
   {
    F(This).DiselectWindow(This, display, listegwm);
    F(This).UnMaxWindow(This, display, listegwm);
   }
   return INTERN_EVENT;
  }
  /*****************************/
  /* Resize gauche du document */
  /*****************************/
  if (Event->xany.window == listegwm->Use->ResizeLeftWin)
  {
   if (Event->type == Expose || Event->type == GraphicsExpose)
   {
    mask |= MGWMRESIZELEFT;
    break;
   }
   if (Event->type == ButtonPress)
   {
    F(This).DiselectWindow(This, display, listegwm);
    F(This).ResizeLeftWindow(This, display, Event, listegwm);
   }
   return INTERN_EVENT;
  }
  /****************************/
  /* Resize droit du document */
  /****************************/
  if (Event->xany.window == listegwm->Use->ResizeRightWin)
  {
   if (Event->type == Expose || Event->type == GraphicsExpose)
   {
    mask |= MGWMRESIZERIGHT;
    break;
   }
   if (Event->type == ButtonPress)
   {
    F(This).DiselectWindow(This, display, listegwm);
    F(This).ResizeRightWindow(This, display, Event, listegwm);
   }
   return INTERN_EVENT;
  }
  /*********************************/
  /* Resize bas gauche du document */
  /*********************************/
  if (Event->xany.window == listegwm->Use->ResizeCornerLeftWin)
  {
   if (Event->type == Expose || Event->type == GraphicsExpose)
   {
    mask |= MGWMRESIZECORNERLEFT;
    break;
   }
   if (Event->type == ButtonPress)
   {
    F(This).DiselectWindow(This, display, listegwm);
    F(This).ResizeCornerLeftWindow(This, display, Event, listegwm);
   }
   return INTERN_EVENT;
  }
  /********************************/
  /* Resize bas droit du document */
  /********************************/
  if (Event->xany.window == listegwm->Use->ResizeCornerRightWin)
  {
   if (Event->type == Expose || Event->type == GraphicsExpose)
   {
    mask |= MGWMRESIZECORNERRIGHT;
    break;
   }
   if (Event->type == ButtonPress)
   {
    F(This).DiselectWindow(This, display, listegwm);
    F(This).ResizeCornerRightWindow(This, display, Event, listegwm);
   }
   return INTERN_EVENT;
  }
  /**************************/
  /* Resize bas du document */
  /**************************/
  if (Event->xany.window == listegwm->Use->ResizeBottomWin)
  {
   if (Event->type == Expose || Event->type == GraphicsExpose)
   {
    mask |= MGWMRESIZEBOTTOM;
    break;
   }
   if (Event->type == ButtonPress)
   {
    F(This).DiselectWindow(This, display, listegwm);
    F(This).ResizeBottomWindow(This, display, Event, listegwm);
   }
   return INTERN_EVENT;
  }
  return NO_EVENT;
 }
 /******************************/
 /* Des cas d'expose a traiter */
 /******************************/
 if (mask)
 {
  int		width;
  int		height;
  int		none;
  Window		Wnone;
      
  XGetGeometry(display, listegwm->MainWin, &Wnone,
	       &none, &none, &width, &height,
	       &none, &none);
  F(This).DrawDecorP(This, listegwm, display,
		     width, height,
		     mask);
  return INTERN_EVENT;
 }
 return NO_EVENT;
}
