/*
** Classor.h for XQuad in Classor/
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
** Last update Mon Jul 17 20:31:27 1995 Emmanuel Paris
*/


#ifndef _Classor_h
#define _Classor_h

typedef struct sc_Classor c_Classor;

#include "xcalibur.h"
#define DATA_MACROS
#include "Tableau.h"

/*--- define the method for Class Classor ---*/
extern void Init_Classor();

typedef struct
{
  F_STD;
  void	(*Resize)();
  void	(*Draw)();
  void	(*EventLook)();
} sf_Classor;

/*--- define the Classor Class ---*/
struct sc_Classor
{
 sf_Classor	*f;
 Display	*display;
 Window		window;
 GC		gc;
 XFontStruct	*font_struct;
 /*--- The document for reselect sheet ---*/
 c_Tableau	*Tableau;
 /*--- Position of the first Sheet viewable on the Classor ---*/
 int		Sheet_Begin_pos;
 int		Sheet_Previous_pos;
 int		Sheet_Number_pos;
 int		Sheet_Next_pos;
 int		Sheet_End_pos;
 int		Classor_pos;
 /*--- Classor part ---*/
 int		nb_sheet;
 listsheet	*list_sheet;
 listsheet	*selected_sheet;
 int		nb_sheet_visible;
 int		first_sheet_pos;
 listsheet	*first_sheet_visible;
 int		last_sheet_pos;
 listsheet	*last_sheet_visible;
 listsheet	*sheet_number_display;
 int		selected_sheet_pos;
/*--- Classor size ---*/
 int		Size;
};

extern sf_Classor fc_Classor;

/*--- Classor size ---*/
#define CLASSOR_SIZE		13

/*--- Some Color definition for Classor ---*/
#define CLASSOR_SHAD		"gray30"
#define CLASSOR_NORM		"gray60"
#define CLASSOR_HIGH		"gray80"

#define P_HIGH	0
#define P_SHAD	1
#define P_NORM	2

#define ICON_SEPARATOR		2
#define SHEET_NUMBER_SIZE	sheet_number_size

/* repeat time config */
#define LOOPTIME 10
#define WTIME 50000
#define WTIME2 500
#define MIN_WTIME2 100

#ifdef _PRIVATE_CLASSOR_

static sheet_number_size;

/* classors colors */
static long Classor_High_Color;
static long Classor_Shad_Color;
static long Classor_Norm_Color;

/* include pixmap */
#include "sheet.xpm"
#include "Page_Begin.xpm"
#include "Page_Previous.xpm"
#include "Page_Next.xpm"
#include "Page_End.xpm"

static Pixmap	Sheet_Begin;
static int	Sheet_Begin_width;
static int	Sheet_Begin_height;
static Pixmap	Sheet_Previous;
static int	Sheet_Previous_width;
static int	Sheet_Previous_height;
static Pixmap	Sheet_Next;
static int	Sheet_Next_width;
static int	Sheet_Next_height;
static Pixmap	Sheet_End;
static int	Sheet_End_width;
static int	Sheet_End_height;

enum {
  CLASSOR_SHEET_NORMAL = 0,
  NB_ICON_CLASSOR,
};

typedef struct 
{
  Pixmap   Icon;
  int	   width;
  int	   height;
} classor_icons;

static classor_icons Classor_Icons[NB_ICON_CLASSOR];
  
#define NB_PIXMAP_CLASSOR  5

typedef struct 
{
  char 		**Image;
  Pixmap	*Pixmap;
  int		*width;
  int		*height;
} tab_pixmap;

static tab_pixmap Tab_Pixmap_Classor[NB_PIXMAP_CLASSOR]=
{
  { sheet_xpm,
      &(Classor_Icons[CLASSOR_SHEET_NORMAL].Icon),
      &(Classor_Icons[CLASSOR_SHEET_NORMAL].width),
      &(Classor_Icons[CLASSOR_SHEET_NORMAL].height) },
  { Page_Begin_xpm, &Sheet_Begin, &Sheet_Begin_width, &Sheet_Begin_height },
  { Page_Previous_xpm, &Sheet_Previous, &Sheet_Previous_width,
      &Sheet_Previous_height },
  { Page_Next_xpm, &Sheet_Next, &Sheet_Next_width, &Sheet_Next_height },
  { Page_End_xpm, &Sheet_End, &Sheet_End_width, &Sheet_End_height }
};

#endif

#endif


