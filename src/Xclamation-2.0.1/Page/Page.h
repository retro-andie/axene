/*
** Page.h for Xclamation in Page/
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
** Started on  Mon Jul 17 19:57:42 1995 Emmanuel Paris
** Last update Mon Oct  5 13:21:38 1998 Robin Castanier
*/

#ifndef _Page_h
#define _Page_h

typedef struct sc_Page c_Page;

#include "xcalibur.h"
#include "Colormap.h"
#include "Cursor.h"
#include "Cadre.h"
#include "ClipBuffer.h"
#include "Drag_And_Drop.h"
#include "PageStd.h"
#include "Regle.h"
#include "ScrollBarX.h"
#include "Pager.h"
#include "Unmap_Map_ScrollBar.h"
#include "Contour_Grille_Repere.h"
#include "Set_New_Priority.h"
#include "Delete_Selected_Cadre.h"
#ifndef CADRE_DEF
#include "Cut_Copy_Paste.h"
#endif
#include "Lock_Unlock_Cadre.h"
#include "Fusion_Cadre.h"
#include "Substract_Cadre.h"
#include "Addition_Cadre.h"
#include "Outline_Cadre.h"
#include "Align_Cadre.h"
#include "Move_Selected_Cadre.h"
#include "Copy_Selected_Cadre.h"
#include "ObjectStd.h"
#include "PostScript.h"
#include "ImageManager.h"
#include "Drag_Cadre_Macros.h"
#include "DocFile.h"
#include "BaseStd.h"
#include "PageFormat.h"

/*--- Constants for save & load ---*/
#define XcDF_PAGE_KEYWORD		"PAGE"
#define XcDF_PAGE_ZOOM_KEYWORD		"ZOOM"
#define XcDF_PAGE_SELECTED_KEYWORD	"SELECTED"
#define XcDF_PAGE_ORIGIN_KEYWORD	"ORIGIN"

/* domain definitions */
#define NAME_MAX_LENGTH 	21
#define INTERS_CADRE		TRUE
#define ALL_CADRE		FALSE
#define DEL_CADRE		TRUE
#define NO_DEL_CADRE		FALSE
#define ZOOM_PLUS_LIMIT		400
#define ZOOM_MINUS_LIMIT	 10
#define ZOOM_STEP		 25

#define XPi_SCRATCH		0
#define XPi_LOAD		1
#define XPi_MODIFY		2

/* extern definition */
extern c_Colormap *GlobColormap;
extern c_ClipBuffer *GlobClipBuffer;
extern c_DragAndDrop *GlobDragAndDrop;
extern char *defaultFrameName;
extern int LinkFunction;
extern sf_page_function	fc_Create_Cadre_Rectangle;
extern sf_page_function	fc_Create_Cadre_Elipse;
extern sf_page_function fc_Create_Cadre_Polygonal;
extern sf_page_function fc_Select_Move_Resize_Cadre;
extern sf_page_function fc_Rotate_Cadre;
extern sf_page_function fc_Scale_Cadre;
extern sf_page_function fc_Zoom_Plus_Page;
extern sf_page_function fc_Zoom_Minus_Page;
extern sf_page_function fc_Zoom_Page_In_Zone;
extern sf_page_function fc_Insert_Point;
extern sf_page_function fc_Delete_Point;
extern sf_page_function fc_Dispatch_To_Object;
extern sf_page_function fc_Link_TextFrame;
extern void drag_cadre_move_done();
extern void drag_cadre_copy_done();
extern void move_point_mode();
extern void resize_cadre_mode();
extern void zoom_page_fit_in_width();
extern void zoom_page_fit_in_screen();
extern void zoom_page_at_real_size();
extern void zoom_page_at_percent();

/* define a cadre for list */
typedef struct _l_Cadre
{
  char              *name;
  int		    priority;
  c_Cadre           *cadre;
  struct _l_Cadre   *NextCadre;
} l_Cadre;

/* export definition of Page.c */
extern void free_list_cadre();
extern void get_center_list_cadre();
extern void get_limit_list_cadre();
extern void add_cadre_in_list();
extern void delete_cadre_in_list();
extern l_Cadre *list_cadre_inters();
extern l_Cadre *duplicate_list_cadre();
extern void list_cadre_low_high_priority();
extern l_Cadre *sort_asc_list_cadre();
extern l_Cadre *sort_desc_list_cadre();
extern l_Cadre *get_lcadre_by_cadre();

/* define the method for Class Page */

typedef struct
{
  F_STD;
  void		(*init_Page) ___PROTO((c_Page *this,
				       unsigned int number,
				       page_format_t *format,
				       page_type_t type,
				       page_orient_t orientation,
				       coord_t left_margin,
				       coord_t right_margin,
				       coord_t upper_margin,
				       coord_t lower_margin,
				       int nb_column, int nb_row,
				       coord_t column_spacing,
				       coord_t row_spacing, 
				       coord_t grid_origx,
				       coord_t grid_origy,
				       coord_t grid_deltax,
				       coord_t grid_deltay,
				       int mode));

  void 		(*resize_Page)();
  boolean	(*set_function)();
  status_t	(*eventlook)();
  void		(*select_Page)();
  void		(*deselect_Page)();
  void		(*set_X_info)();
  error		(*add_cadre)();
  void		(*delete_cadre)();
  void		(*list_cadre)();
  void		(*destroy_cadre)();
  void		(*redisplay_cadre)();
  void		(*set_priority_cadre)();
  int		(*get_priority_cadre)();
  void		(*lower_priority)();
  void		(*higher_priority)();
  void		(*lowest_priority)();
  void		(*highest_priority)();
  void 		(*delete_selected_cadre)();
  void 		(*delete_selected_object)();
  void		(*cut_selected_cadre)();
  void 		(*copy_selected_cadre)();
  void		(*paste_selected_cadre)();
  void 		(*set_lower_priority)();
  void		(*set_higher_priority)();
  void		(*set_lowest_priority)();
  void		(*set_highest_priority)();
  int		(*get_highest_priority)();
  int		(*get_lowest_priority)();
  void		(*zoom_page_fit_in_width)();
  void		(*zoom_page_fit_in_screen)();
  void		(*zoom_page_at_real_size)();
  void		(*zoom_page_at_percent)();
  void 		(*move_point_mode)();
  void 		(*resize_cadre_mode)();
  void		*(*add_page_align_mark)();
  void		(*move_page_align_mark)();
  void		(*remove_page_align_mark)();
  void		*(*get_page_align_mark)();
  void		(*lock_cadre)();
  void		(*unlock_cadre)();
  void		(*fusion_cadre)();
  void		(*substract_cadre)();
  void		(*addition_cadre)();
  void		(*outline_cadre)();
  void		(*align_cadre)();
  char		*(*copy_cadre_name)();
  l_Cadre	*(*get_new_cadre)();
  l_Cadre	*(*get_copy_cadre)();
  l_Cadre	*(*get_cadre_by_xy)();
  l_Cadre	*(*get_cadre_by_name)();
  l_Cadre	*(*get_cadre_and_point_by_xy)();
  l_Cadre	*(*get_cadre_and_point_by_line)();
  l_Point	*(*get_handle_by_xy)();
  l_Cadre	*(*list_cadre_by_page) ___PROTO((c_Page *This,
						 int num_page,
						 coord_t *orig_x,
						 coord_t *orig_y));
  void		(*free_list_cadre)();
  l_Cadre	*(*duplicate_list_cadre)();
  void		(*get_center_list_cadre)();
  void		(*calcul_select_handle)();
  void		(*change_page_origin)();
  void		(*change_page_scaling)();
  void		(*init_recoordinate_point)();
  void 		(*recoordinate_point)();
  void 		(*attract_point)();
  void		(*move_cadre_to_xy)();
  void		(*redraw_rectangle)();
  void 		(*draw_cadre_contour)();
  void		(*draw_cadre)();
  void		(*select_cadre)();
  void 		(*deselect_cadre)();
  void 		(*select_all_cadre)();
  void 		(*deselect_all_cadre)();
  void		(*display_selected_lcadre)();
  void		(*undisplay_selected_lcadre)();
  boolean	(*ps_print)();
  /*___Loading & saving___*/
  boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			    long *page_number, BaseStd_t *base));
  boolean (*write) ___PROTO((c_Page *this, c_DocFile *doc,
			     boolean selected));
} sf_Page;

/* define the Page Class */
struct sc_Page
{
  sf_Page       	*f;
  boolean	        X_info_calc; /* True if X_info usable */
  x_info		X_info;      /* x misc info as display etc... */
  c_Cursor		*cursor;
  BaseStd_t		*BaseStd;

  int			PosX;	     /* window position relative to her */
  int			PosY;	     /* parent */
  int			Width;	     /* window width */
  int			Height;	     /* window height */
  boolean		in_window;   /* cursor in window ? */
  
  coord_t		sScale;	     /* page scaling */	
  scale_t		sFactor;     /* zoom factor in scale_t */	
  coord_t		sOrig_X;     /* window origin coordinates */	
  coord_t		sOrig_Y;     /* in coord_t */
  int			Orig_X;      /* window origin coordinates */
  int			Orig_Y;	     /* in pixels */	

  unsigned int		page_number;		/* Page number */
  page_format_t		*page_format;		/* A3 A4 etc... */
  page_type_t		page_type;		/* Page Left,Right,Double */
  page_orient_t		page_orient;		/* portrait, landscape */
  
  coord_t		sDeskArea_Width;	/* domain width & height */
  coord_t		sDeskArea_Height;	/* in coord_t */

  int			DeskArea_Width;		/* domain width & height */
  int			DeskArea_Height; 	/* in pixels */
  coord_t		scroll_delta_sX;	/* to recoordinate sX & sY */
  coord_t		scroll_delta_sY;	/* in some function */
  coord_t		scroll_delta_X;		/* to recoordinate X & Y */
  coord_t		scroll_delta_Y;		/* in some function */
  boolean		magnetic;
  c_Page_Contour	Page_Contour;
  boolean		Page_Contour_actif;
  c_Page_Grid		Page_Grid;
  boolean		Page_Grid_actif;
  c_Page_Typo_Mark	*Page_Typo_Mark;
  boolean		Page_Typo_Mark_actif;
  c_Page_Align_Mark	Page_Align_Mark;
  boolean		Page_Align_Mark_actif;
  c_Regle		*RegleV;		/* Regle horizontale */
  c_Regle		*RegleH;		/* Regle verticale */
  c_Regle		*RegleC;		/* Regle centrale */
  boolean		Regle_actif;	
  c_ScrollBarX		*ScrollBarV;
  boolean		ScrollBarV_actif;
  c_ScrollBarX		*ScrollBarH;
  boolean		ScrollBarH_actif;
  c_Pager		*Pager;
  boolean		Pager_actif;
  boolean		Frame_Border_actif;	/* True: border always disp */
  
  boolean		verrou;
  boolean		select_type; /* TRUE if point mode */
  boolean		select_group;  /* TRUE if group selected */ 
  coord_t		select_sX_min;
  coord_t		select_sY_min;
  coord_t		select_sX_max;
  coord_t		select_sY_max;
  l_Cadre		*SelectedCadre;
  l_Cadre               *ListCadre;
  int                   nb_cadre;
  sf_page_function	*function;
  int			function_type;
  void			*function_data;
  d_object_std		data_to_object;
};

extern sf_Page fc_Page;

#endif

