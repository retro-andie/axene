/*
** Pager.h for XAllWrite in Pager/
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Mon Jul 17 20:31:27 1995 Emmanuel Paris
** Last update Mon Aug 18 17:48:12 1997 Emmanuel Paris
*/


#ifndef _Pager_h
#define _Pager_h

typedef struct sc_Pager c_Pager;

#include "xcalibur.h"
#include "Document.h"
#include "PageFormat.h"

/*--- define the method for Class Pager ---*/
extern void Init_Pager();

typedef struct
{
  F_STD;
  void	(*Resize)();
  void	(*Draw)();
  void	(*EventLook)();
  void	(*Configure)();
} sf_Pager;

/*--- define the Pager Class ---*/
struct sc_Pager
{
 sf_Pager	*f;
 Display	*display;
 Window		window;
 GC		gc;
 XFontStruct	*font_struct;
 /*--- The document for reselect page ---*/
 c_Document	*document;
 c_PageArea	*pagearea;
 /*--- Position of the first Page viewable on the pager ---*/
 int		Page_Begin_pos;
 int		Page_Previous_pos;
 int		Page_Number_pos;
 int		Page_Next_pos;
 int		Page_End_pos;
 int		Pager_pos;
 /*--- Pager part ---*/
 int		nb_page;
 c_Page		*list_page;
 c_Page		*selected_page;
 int		nb_page_visible;
 int		first_page_pos;
 c_Page		*first_page_visible;
 int		last_page_pos;
 c_Page		*last_page_visible;
 c_Page		*page_number_display;
 int		selected_page_pos;
/*--- pager size ---*/
 int		Size;
};

extern sf_Pager fc_Pager;

/*--- Pager size ---*/
#define PAGER_SIZE		13

/*--- Some Color definition for Pager ---*/
#define PAGER_SHAD		"gray30"
#define PAGER_NORM		"gray60"
#define PAGER_HIGH		"gray80"

#define P_HIGH	0
#define P_SHAD	1
#define P_NORM	2

#define ICON_SEPARATOR		2
#define PAGE_NUMBER_SIZE	page_number_size

/* repeat time config */
#define LOOPTIME 10
#define WTIME 50000
#define WTIME2 500
#define MIN_WTIME2 100

#ifdef _PRIVATE_PAGER_

#define IS_DOUBLE_PAGE(list_page) \
list_page->page_type == Xc_PAGE_DOUBLE && \
list_page->NextPage && \
list_page->NextPage->page_type == (Xc_PAGE_DOUBLE + Xc_PAGE_RIGHT)

static int page_number_size;

/* pagers colors */
static long Pager_High_Color;
static long Pager_Shad_Color;
static long Pager_Norm_Color;

/* include pixmap */
#include "Portrait_Left.xpm"
#include "Portrait_Right.xpm"
#include "Portrait_Double.xpm"
#include "Landscape_Left.xpm"
#include "Landscape_Right.xpm"
#include "Landscape_Double.xpm"
#include "Page_Begin.xpm"
#include "Page_Previous.xpm"
#include "Page_Next.xpm"
#include "Page_End.xpm"

static Pixmap	Page_Begin;
static int	Page_Begin_width;
static int	Page_Begin_height;
static Pixmap	Page_Previous;
static int	Page_Previous_width;
static int	Page_Previous_height;
static Pixmap	Page_Next;
static int	Page_Next_width;
static int	Page_Next_height;
static Pixmap	Page_End;
static int	Page_End_width;
static int	Page_End_height;

enum {
  PAGER_PORTRAIT_LEFT = 0,
  PAGER_PORTRAIT_RIGHT,
  PAGER_PORTRAIT_DOUBLE,
  PAGER_LANDSCAPE_LEFT,
  PAGER_LANDSCAPE_RIGHT,
  PAGER_LANDSCAPE_DOUBLE,
  NB_ICON_PAGER,
};

typedef struct 
{
  Pixmap   Icon;
  int	   width;
  int	   height;
} pager_icons;

static pager_icons Pager_Icons[NB_ICON_PAGER];
  
#define NB_PIXMAP_PAGER  10

typedef struct 
{
  char 		**Image;
  Pixmap	*Pixmap;
  int		*width;
  int		*height;
} tab_pixmap;

static tab_pixmap Tab_Pixmap_Pager[NB_PIXMAP_PAGER]=
{
  { Portrait_Left_xpm,
      &(Pager_Icons[PAGER_PORTRAIT_LEFT].Icon),
      &(Pager_Icons[PAGER_PORTRAIT_LEFT].width),
      &(Pager_Icons[PAGER_PORTRAIT_LEFT].height) },
  { Portrait_Right_xpm,
      &(Pager_Icons[PAGER_PORTRAIT_RIGHT].Icon),
      &(Pager_Icons[PAGER_PORTRAIT_RIGHT].width),
      &(Pager_Icons[PAGER_PORTRAIT_RIGHT].height) },
  { Portrait_Double_xpm,
      &(Pager_Icons[PAGER_PORTRAIT_DOUBLE].Icon),
      &(Pager_Icons[PAGER_PORTRAIT_DOUBLE].width),
      &(Pager_Icons[PAGER_PORTRAIT_DOUBLE].height) },
  { Landscape_Left_xpm,
      &(Pager_Icons[PAGER_LANDSCAPE_LEFT].Icon),
      &(Pager_Icons[PAGER_LANDSCAPE_LEFT].width),
      &(Pager_Icons[PAGER_LANDSCAPE_LEFT].height) },
  { Landscape_Right_xpm,
      &(Pager_Icons[PAGER_LANDSCAPE_RIGHT].Icon),
      &(Pager_Icons[PAGER_LANDSCAPE_RIGHT].width),
      &(Pager_Icons[PAGER_LANDSCAPE_RIGHT].height) },
  { Landscape_Double_xpm,
      &(Pager_Icons[PAGER_LANDSCAPE_DOUBLE].Icon),
      &(Pager_Icons[PAGER_LANDSCAPE_DOUBLE].width),
      &(Pager_Icons[PAGER_LANDSCAPE_DOUBLE].height) }, 
  { Page_Begin_xpm, &Page_Begin, &Page_Begin_width, &Page_Begin_height },
  { Page_Previous_xpm, &Page_Previous, &Page_Previous_width,
      &Page_Previous_height },
  { Page_Next_xpm, &Page_Next, &Page_Next_width, &Page_Next_height },
  { Page_End_xpm, &Page_End, &Page_End_width, &Page_End_height }
};

#endif

#endif






