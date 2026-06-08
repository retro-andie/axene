/*
 * FrameManager.h 		- Object and method definition for 
 *					Managing frames
 *
 * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
 */

#ifndef _FrameManager_h
#define _FrameManager_h

typedef struct sc_FrMg c_FrMg;

#include "xcalibur.h"
#include "Colormap.h"
#include "Cursor.h"
#include "Cadre.h"
#include "FrameManagerStd.h"
#include "Set_New_Priority.h"
#include "Delete_Selected_Cadre.h"
#include "Lock_Unlock_Cadre.h"
#include "Move_Selected_Cadre.h"
#include "Copy_Selected_Cadre.h" 
#include "ObjectStd.h"
#include "PostScript.h"
#include "DocFile.h"
#include "BaseStd.h"
#include "Drag_And_Drop.h"
#include "Drag_Cadre_Macros.h"

/*--- Constants for save & load ---*/
#define XcDF_FRAMELIST_KEYWORD "FRAMELIST"

/* domain definitions */
#define NAME_MAX_LENGTH 	21
#define INTERS_CADRE		TRUE
#define ALL_CADRE		FALSE
#define DEL_CADRE		TRUE
#define NO_DEL_CADRE		FALSE

/* extern definition */
extern c_Colormap *GlobColormap;
extern char *defaultFrameName;
extern c_DragAndDrop *GlobDragAndDrop;
extern sf_frmg_function	fc_Create_Cadre_Rectangle;
/* extern sf_page_function fc_Select_Move_Resize_Cadre; */

/* define a cadre for list */
typedef struct _l_Cadre
{
  char              *name;
  int		    priority;
  c_Cadre           *cadre;
  struct _l_Cadre   *NextCadre;
} l_Cadre;

typedef struct _draw_cache
{
  l_Cadre	*lcadre;
  Polygone	*polygone;
  struct _draw_cache	*Next;
} draw_cache_t;

/* export definition of FrameManager.c */
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

/* define the method for Class FrameManager */

typedef struct
{
  F_STD;
  void 		(*resize_frmg)();
  boolean	(*set_function)();
  status_t	(*eventlook)();
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
  void 		(*set_lower_priority)();
  void		(*set_higher_priority)();
  void		(*set_lowest_priority)();
  void		(*set_highest_priority)();
  int		(*get_highest_priority)();
  int		(*get_lowest_priority)();
  void		(*lock_cadre)();
  void		(*unlock_cadre)();
  char		*(*copy_cadre_name)();
  l_Cadre	*(*get_new_cadre)();
  l_Cadre	*(*get_copy_cadre)();
  l_Cadre	*(*get_cadre_by_xy)();
  l_Cadre	*(*get_cadre_by_name)();
  l_Cadre	*(*get_cadre_and_point_by_xy)();
  l_Cadre	*(*get_cadre_and_point_by_line)();
  l_Point	*(*get_handle_by_xy)();
  void		(*free_list_cadre)();
  l_Cadre	*(*duplicate_list_cadre)();
  void		(*get_center_list_cadre)();
  void		(*calcul_select_handle)();
  void		(*change_frmg_origin)();
  void		(*change_frmg_scaling)();
  void		(*init_recoordinate_point)();
  void 		(*recoordinate_point)();
  void 		(*attract_point)();
  void		(*move_cadre_to_xy)();
  void		(*redraw_rectangle)();
  void 		(*draw_cadre_contour)();
  void		(*draw_cadre)();
  void		(*start_draw_cache) ___PROTO((c_FrMg *This));
  void		(*flush_draw_cache) ___PROTO((c_FrMg *This));	
  void		(*select_cadre)();
  void 		(*deselect_cadre)();
  void 		(*select_all_cadre)();
  void 		(*deselect_all_cadre)();
  void		(*display_selected_lcadre)();
  void		(*undisplay_selected_lcadre)();
  boolean	(*get_bbox_list_cadre) ___PROTO((c_FrMg *This, bbox_t *bboxp,
						 l_Cadre *lcadre));
  /*___Printing___*/
  boolean (*ps_print) ___PROTO((c_FrMg *this, c_PostScript *post,
				bbox_t *bbox, setup_print_t *setup));

  /*___Loading & saving___*/
  boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			    long param, void *feuille));
  boolean (*write) ___PROTO((c_FrMg *this, c_DocFile *doc));
  void		(*redraw_rectangle_handle)();
} sf_FrMg;

/* define the FrameManager Class */
struct sc_FrMg
{
  sf_FrMg	     	*f;
  boolean	        X_info_calc; /* True if X_info usable */
  x_info		X_info;      /* x misc info as display etc... */
  BaseStd_t		*BaseStd;
  c_Cursor		*cursor;
  
  int			PosX;	     /* window position relative to her */
  int			PosY;	     /* parent */
  int			Width;	     /* window width */
  int			Height;	     /* width haight */
  
  coord_t		sScale;	     /* page scaling */	
  scale_t		sFactor;     /* zoom factor in scale_t */	
  coord_t		sOrig_X;     /* window origin coordinates */	
  coord_t		sOrig_Y;     /* in coord_t */
  int			Orig_X;      /* window origin coordinates */
  int			Orig_Y;	     /* in pixels */	

  coord_t		scroll_delta_sX;	/* to recoordinate sX & sY */
  coord_t		scroll_delta_sY;	/* in some function */
  coord_t		scroll_delta_X;		/* to recoordinate X & Y */
  coord_t		scroll_delta_Y;		/* in some function */
  
  boolean               magnetic;
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
  sf_frmg_function	*function;
  int			function_type;
  void			*function_data;
  d_object_std		data_to_object;
  
  boolean		redraw_cache_actif;
  draw_cache_t		*redraw_cache;
  
  void			*callback_data;
  void			(*select_cadre_callback)();
  void			(*deselect_cadre_callback)();
  void			(*redraw_polygone_callback)();
  void			(*modify_cadre_callback)();
  void			(*attract_point_callback)();
};

extern sf_FrMg fc_FrMg;

#endif

