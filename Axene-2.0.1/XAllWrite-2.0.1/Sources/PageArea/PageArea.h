/*
** PageArea.h for XAllWrite in PageArea/
** Declaration of the PageArea class
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Sat Mar  8 14:47:20 1997 Emmanuel Paris
** Last update Wed Nov 11 18:08:57 1998 Emmanuel Paris
*/


#ifndef _PageArea_h
#define _PageArea_h

typedef struct sc_PageArea c_PageArea;

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "PageAreaStd.h"
#include "Page.h"
#include "Document.h"
#include "DocFile.h"
#include "PostScript.h"
#include "ScrollBarX.h"
#include "Pager.h"
#include "Calibration.h"
#include "Redraw.h"
#include "Cursor.h"
#include "XaRegle.h"

typedef enum 
{
 Xa_VIEW_SINGLE_PAGE,
 Xa_VIEW_NORMAL_PAGE,
 Xa_VIEW_TWO_PAGES,
 Xa_VIEW_MULTIPLE_PAGES
} view_page_t;


/*--- forward declarations ---*/
struct sfs_PageArea;

/*--- define PageArea Callbacks (call to Document) ---*/
typedef struct 
{
 void	*cbData;
 void	(*mapScrollBar) ___NPROTO((void *cbdata, boolean scrollbH,
				   boolean scrollbV));
} pagearea_cb_t;


/*--- define the PageArea Class ---*/
struct sc_PageArea
{
 struct sfs_PageArea *f;   

 Display	*display;  
 Window	        window;	   
 Window	        doc_window;	   
 Window		tracing_window;
 c_Cursor	*cursor;
 
 int	PosX;
 int	PosY;
 int	Width;				/* window size width */
 int	Height;				/* window size height */

 pagearea_cb_t	callbacks;
 c_ScrollBarX	*ScrollBarV;
 c_ScrollBarX	*ScrollBarH;
 
 c_Pager	*Pager;
 
 c_XaRegle	*RegleV;
 c_XaRegle	*RegleH;

 c_Page		*PageList;
 c_Page		*SelectedPage;
 c_Page		*ActivePage;	/* mouse pointer within this page */
 int		NbrPage;
 boolean	select_type;   /* page's global selection type */ 	

 BaseStd_t	*BaseStd;
 c_Calibration	*Calibration;
 c_Redraw	*Redraw;
 
 coord_t	sScale;		/* pages scaling */	
 scale_t	sFactor;	/* zoom factor in scale_t */	
 view_page_t	view_page;
 
 int		AreaOrig_X;
 int		AreaOrig_Y;
 int		AreaWidth;
 int		AreaHeight;
 boolean	NeedScrollBarV;
 boolean	NeedScrollBarH;
 
 boolean	recto_verso;
 boolean	double_page;

 sf_pagearea_function	*function;
 int			function_type;
 void			*function_data;

 c_Page		*GuestPage;
 int		GuestX, GuestY;
 c_Page		*FirstPageViewable;
 c_Page		*LastPageViewable;

 int		ForcedWidth;   /* calcul page position with this width */
 int		NbGutter;      /* number of gutter */	

};

/*--- define the method for Class PageArea ---*/
typedef struct sfs_PageArea
{
 F_STD;
 void (*Init) ___PROTO((c_PageArea *This, Display *display, Window window,
			c_ScrollBarX *scrollbV, c_ScrollBarX *scrollbH,
			int PosX, int PosY, int Width, int Height,
			c_Pager *Pager, c_XaRegle *RegleV, c_XaRegle *RegleH));
 void (*Resize) ___PROTO((c_PageArea *This, int PosX, int PosY,
			  int Width, int Height));
 status_t (*EventLook) ___PROTO((c_PageArea *This, XEvent *Event));
 boolean (*SetFunction) ___PROTO((c_PageArea *This, int function));
 void (*SelectPage) ___PROTO((c_PageArea *This, c_Page *page_list));
 boolean (*IsInSubWindow) ___PROTO((c_PageArea *This, Window window));
 void (*UnsetCursor) ___PROTO((c_PageArea *This));
 void (*ResetCursor) ___PROTO((c_PageArea *This));
 void (*AddPage) ___PROTO((c_PageArea *This));
 void (*DelPage) ___PROTO((c_PageArea *This, c_Page *from_page));
 void (*Configure) ___PROTO((c_PageArea *This));
 void (*MakeAreaVisible) ___PROTO((c_PageArea *This, c_Page *page, 
				   int x, int y, int width, int height));
 c_AutoScroll *(*CreateAutoScroll) ___PROTO((c_PageArea *This, c_Page *page,
					     void (*function)(), void *data));
 void (*DestroyAutoScroll) ___PROTO((c_PageArea *This, 
				     c_AutoScroll *autoscroll));
 c_Page *(*GetPageByXY) ___PROTO((c_PageArea *This, int x, int y));
 void (*RecalibrateAll) ___PROTO((c_PageArea *This));
 void (*Update) ___PROTO((c_PageArea *This));
 boolean (*GotoPage) ___PROTO((c_PageArea *This, c_Page **Lpage));
 
 boolean (*readPageArea) ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_PageArea *This));
 boolean (*writePageArea) ___PROTO((c_PageArea *This, c_DocFile *doc));
 boolean (*ps_print) ___PROTO((c_PageArea *this, c_PostScript *post));
} sf_PageArea;

extern sf_PageArea fc_PageArea;

#define PA_COLOR "gray50"
#define PA_SHADOW "gray30"
/* Page mode size in millimeters */
#define PA_GUTTER SCALE_FROM_MILLIMETERS(5)/COORD_DEFAULT 
#define PA_HMARGIN SCALE_FROM_MILLIMETERS(15)/COORD_DEFAULT
#define PA_VMARGIN SCALE_FROM_MILLIMETERS(5)/COORD_DEFAULT

#endif












