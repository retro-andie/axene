/*
** Contour_Grille_Repere.h for Xclamation in Page/
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
** Started on  Mon Jul 17 20:27:57 1995 Emmanuel Paris
** Last update Mon Oct  5 13:21:33 1998 Robin Castanier
*/


#ifndef _Contour_Grille_Repere_h_
#define _Contour_Grille_Repere_h_

#include "xcalibur.h"

extern void calcul_desk_area_and_origin();
extern void calcul_desk_area_component();	
extern void destroy_desk_area_component();
extern void draw_desk_area_rectangle();
extern void draw_desk_area_polygone();

extern void attract_point();

extern void display_grid();
extern void display_typo_mark();
extern void display_align_mark();
extern void display_regle();
extern void display_frame_border();
extern void magnetize_all();
extern void redraw_all();

extern void create_page_contour();
extern void create_page_grid();
extern void create_page_typo_mark();
extern void create_page_align_mark();

extern void *add_page_align_mark();
extern void move_page_align_mark();
extern void remove_page_align_mark();
extern void recoordinate_page_align_mark();
extern void *get_page_align_mark();

/*--- Constants for save & load ---*/
#define XcDF_GRID_KEYWORD "GRID"
#define XcDF_MARGINS_KEYWORD "MARGINS"
#define XcDF_VLINE_KEYWORD "VLINE"
#define XcDF_HLINE_KEYWORD "HLINE"
#define XcDF_GRIDLINE_KEYWORD "GRIDLINE"

/* magnetic definition */
#define MAX_ATTRACT	100000*COORD_SCALEVALUE;

/* define vertical line */
typedef struct s_v_line 
{
  coord_t	sX;
  coord_t	sY_min;
  coord_t	sY_max;
  int		X;
  int		Y_min;
  int		Y_max;
  struct s_v_line	*NextVline;
} v_line;

/* define horizontal line */
typedef struct s_h_line 
{
  coord_t	sY;
  coord_t	sX_min;
  coord_t	sX_max;
  int		Y;
  int		X_min;
  int		X_max;
  struct s_h_line	*NextHline;
} h_line;

/* define page contour's elements */
typedef struct
{
  coord_t	sOrig_X;
  coord_t	sOrig_Y;
  coord_t	sWidth;
  coord_t	sHeight;
  int 		nb_page_vertical;
  int		nb_page_horizontal; 	 
  boolean	visible;
  boolean	magnetic;
  coord_t	magnetic_field;
  unsigned long color;
  unsigned long	shadow_color;
  int		shadow_thickness;
  h_line	*ListHline;
  v_line	*ListVline;
} c_Page_Contour;

/* define page grid's elements */
typedef struct
{
  coord_t	sOrig_X;
  coord_t	sOrig_Y;
  coord_t	sEcart_X;
  coord_t	sEcart_Y;
  int		nb_vline;
  int		nb_hline;
  boolean	visible_X;
  boolean	visible_Y;
  boolean	magnetic_X;
  boolean	magnetic_Y;
  coord_t	magnetic_field_X;
  coord_t	magnetic_field_Y;
  unsigned long color_X;
  unsigned long color_Y;
  h_line	*ListHline;
  v_line	*ListVline;
} c_Page_Grid;
 
/* define page typographics marks */
typedef struct
{
  coord_t	left_margin;
  coord_t	right_margin;
  coord_t	top_margin;
  coord_t	bottom_margin;
  int		column;
  int		row;
  coord_t	gouttiere_x;
  coord_t	gouttiere_y;
  boolean	visible;
  boolean	magnetic;
  coord_t	magnetic_field;
  unsigned long color;
  h_line	*ListHline;
  v_line	*ListVline;
} c_Page_Typo_Mark;

/* define page alignment mark */

#define ALIGN_MARK_HORIZONTAL		0
#define ALIGN_MARK_VERTICAL		1
#define ALIGN_MARK_ABS_HORIZONTAL	2
#define ALIGN_MARK_ABS_VERTICAL		3

typedef struct
{
  int		nb_marks_vertical;
  int		nb_marks_horizontal;
  boolean	visible_X;
  boolean	visible_Y;
  boolean	magnetic_X;
  boolean	magnetic_Y;
  coord_t	magnetic_field_X;
  coord_t	magnetic_field_Y;
  unsigned long color_X;
  unsigned long color_Y;
  h_line	*ListHline;
  v_line	*ListVline;
} c_Page_Align_Mark;

#endif






