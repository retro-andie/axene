/*
** Regle.h for Xclamation in Regle/
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
** Started on  Tue Jul 18 12:13:40 1995 Emmanuel Paris
** Last update Thu Jun 20 18:24:53 1996 Emmanuel Paris
*/

#ifndef _Regle_h
#define _Regle_h

typedef struct sc_Regle c_Regle;

#include "xcalibur.h"
#include "Cursor.h"
#include <X11/Intrinsic.h>
#include "Page.h"

#define REGLEHEIGHT	10
#define REGLEMARK	2
#define REGLEBIGMARK	4

/* structur for interval management */

typedef struct
{
  coord_t	min;
  coord_t	max;
  coord_t	origine;
} interv_t;

/* defint the method for Class Regle */
extern void Init_Regle();
extern void Reset_Regle();

typedef struct
{
  F_STD;
  void	(*Resize)();
  boolean (*EventLook)();
  void	(*Init_Regle_By_Page)();
  void	(*Draw)();
  void  (*Draw_Repere)();
  void  (*Clear_Repere)();
} sf_Regle;


/* define the Regle Class */
struct sc_Regle
{
 sf_Regle	*f;
 Display	*display;
 Window		window;
 GC		gc;
 GC		gc_repere;
 c_Cursor	*cursor;
 c_Page		*Page;
 int		Orient;
 int		Size;	/* taille en pixel en largeur si horizontale ... */
 int		Unit;	/* voir ../Global/units.h */
 coord_t	Scale;  /* Size*Scale = largeur en coord_t */
 int		Grad;	/* indice du tableau graduate_factor */
 real		Division;
 real		SubDivision;
 real		SubSubDivision;
 real		SubSubSubDivision;
 boolean	intervs_calc;
 int		nb_intervs;
 interv_t	*intervs;
 
 boolean	premsh;
 boolean	premsv;
 boolean	source;
 int		xo;
 int		yo;

 int		repere1;	/* repere 1 (coordonne 1er point cadre) */
 int		repere2;	/* repere 2 (width | height cadre) */
 boolean	rep1_actif;	/* if TRUE then repere2 disable */
 boolean	rep1_drawn;
 boolean	rep2_drawn;
};

extern sf_Regle fc_Regle;

#define REGLE_HORIZONTAL 	0
#define REGLE_VERTICAL		1
#define REGLE_CENTER		2

#define REGLE_REPERE_1		0
#define REGLE_REPERE_2		1

#define REGLE_SIZE		17
#define REGLE_RSIZE		REGLE_SIZE

#define REGLE_COLOR		"gray60"
#define REGLE_SEPARATOR		"gray30"
#define REGLE_MIN_GRAD	60

#ifdef _PRIVATE_REGLE_

long Regle_Color;
long Regle_Separator;

typedef struct 
{
  char	*color;
  int	nb_pixel;
  long	real_color;
} Regle_Slice_t;

#define NB_SLICE  7

Regle_Slice_t	Regle_Slice[NB_SLICE]=
{
  { "gray80", 1, 0},
  { "gray60", 8, 0},
  { "gray55", 1, 0},
  { "gray52", 1, 0},
  { "gray51", 2, 0},
  { "gray50", 3, 0},
  { "black", 1, 0}
};

typedef struct
{
  int	width;
  int	height;
  int	offset_x;
  int	offset_y;
  char	* data[7];
} Regle_Font_t;

#define NB_CHAR 12

Regle_Font_t	Regle_Font[NB_CHAR]=
{
#include "font_0.h"
#include "font_1.h"
#include "font_2.h" 
#include "font_3.h" 
#include "font_4.h" 
#include "font_5.h" 
#include "font_6.h" 
#include "font_7.h" 
#include "font_8.h" 
#include "font_9.h" 
#include "font_..h"
#include "font_-.h"
};

typedef struct
{
  int		nb_point;
  int		incx,incy;
  XPoint	*xpoint;
} Regle_Char_t;

Regle_Char_t	Regle_CharH[NB_CHAR];
Regle_Char_t	Regle_CharV[NB_CHAR];

#define NB_GRADUATE_FACTOR	18
#define GRAD_DIVISION       	12
#define GRAD_SUBDIVISION       	6
#define GRAD_SUBSUBDIVISION	4      
#define GRAD_SUBSUBSUBDIVISION	2

typedef struct	
{
  float	factor;
  int   subdivision;
  int	subsubdivision;
  int	subsubsubdivision;
} graduate_factor_t;

graduate_factor_t graduate_factor[NB_GRADUATE_FACTOR]={
  { 0.5, 0, 5, 2},  
  { 1.0, 0, 2, 5},  
  { 2.0, 2, 2, 5},   
  { 3.0, 3, 2, 5},   
  { 4.0, 4, 2, 5},   
  { 5.0, 5, 2, 5},   
  { 6.0, 2, 3, 2},  
  { 8.0, 2, 4, 2},   
  { 10.0, 2, 5, 2},  
  { 12.0, 2, 3, 2},  
  { 20.0, 2, 2, 5},  
  { 50.0, 5, 2, 5},
  {100.0, 2, 5, 2},
  {200.0, 2, 2, 5},
  {500.0, 5, 2, 5},
 {1000.0, 2, 5, 2},
 {5000.0, 5, 2, 5},
{10000.0, 2, 5, 2}  
};

#define MAX_LENGTH 2000

typedef struct
{
  int	offset;
  int	length;
} graduation_t;

graduation_t graduation[MAX_LENGTH];

#endif

#endif








