/*
** Feuille.h for XQuad in Feuille/
** Methods for the Feuile class
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
** Started on  Wed Jul 19 15:33:47 1995 Emmanuel Paris
** Last update Mon Apr 14 17:26:04 1997 Emmanuel Paris
*/

#ifndef _Feuille_h
#define _Feuille_h

/*--- forward declaration ---*/
typedef struct sc_Feuille c_Feuille;

#include "xcalibur.h"
#include "Colormap.h"
#include "Cursor.h"
#include "FontManager.h"
#define _PRIVATE_FEUILLE_
#include "FeuilleStd.h"
#include "Moteur.h"
#ifdef	___mips
#undef 	 d
#include "EditBar.h"
#define d(a)   (data->/**/a/**/)
#else
#include "EditBar.h"
#endif
#include "ScrollBarX.h"
#include "AutoScroll.h"
#include "DocFile.h"
#include "BaseStd.h"
#include "Draw_Feuille.h"
#include "Set_CellStyle.h"
#include "Zoom_Feuille.h"
#include "Misc_Functions.h"
#include "Resource.h"
#include "ColRow_Size.h"
#include "Cadre.h"
#include "FrameManager.h"
#include "Drag_Cadre.h"
#include "PostScript.h"
#include "PrintParam.h"

/* define for load & save */
#define XcDF_SHEET_KEYWORD		"SHEET"
#define XcDF_SHEET_ZOOM_KEYWORD		"ZOOM"
#define XcDF_SHEET_ORIGIN_KEYWORD	"ORIGIN"
#define XcDF_SELECTED_AREAS_KEYWORD	"SELECTED_AREAS"
#define XcDF_ACTIVE_CELL_KEYWORD	"ACTIVE_CELL"

/* domain definitions */
#define NAME_MAX_LENGTH 	21
#define INTERS_CADRE		TRUE
#define ALL_CADRE		FALSE
#define DEL_CADRE		TRUE
#define NO_DEL_CADRE		FALSE
#define ZOOM_PLUS_LIMIT		400
#define ZOOM_MINUS_LIMIT	 50
#define ZOOM_STEP		 25

/* extern definition */
extern c_Colormap	*GlobColormap;
extern c_EditBar	*GlobEditBar;
extern c_Resource	*GlobResources;
extern c_VectorFont	*GlobFontBase;
extern c_FontManager	*GlobFntMng;

extern sf_feuille_function fc_Select_Move_Resize_Cell;
extern sf_feuille_function fc_Select_Region_For_EditBar;
extern sf_feuille_function fc_Dispatch_To_FrMg;
extern sf_feuille_function fc_Create_Graph;

extern void set_region_selected ___PROTO((c_Feuille *This, region_t *region,
					  int type));
extern void unset_region_selected ___PROTO((c_Feuille *This, region_t *region,
					  int type));
extern void add_region_in_list ___PROTO((c_Feuille *This, 
					 region_t *region,
					 int type));
extern void remove_region_from_list ___PROTO((c_Feuille *This, 
					      region_t *region,
					      int type));
extern void focus_in_region ___PROTO((c_Feuille *This, region_t *region,
				      int type));
extern void focus_out_region ___PROTO((c_Feuille *This, region_t *region,
				       int type));
extern int  get_column_pos ___PROTO((c_Feuille *This, int column));
extern int  get_row_pos ___PROTO((c_Feuille *This, int row));
extern int  get_column_width ___PROTO((c_Feuille *This, int column));
extern int  get_row_height ___PROTO((c_Feuille *This, int row));
extern void redraw_rectangle ___PROTO((c_Feuille *This, int type, 
				       int x1, int y1, int x2, int y2));
extern void calcul_column_width ___PROTO((c_Feuille *This));
extern void calcul_row_height ___PROTO((c_Feuille *This));
extern void allocate_cell_info ___PROTO((c_Feuille *This));
extern void calcul_frame_info ___PROTO((c_Feuille *This));
extern void calcul_cell_info ___PROTO((c_Feuille *This));

extern void change_feuille_origin ___PROTO((c_Feuille *This, int x, int	y));
extern void change_feuille_scaling ___PROTO((c_Feuille *This,
					     scale_t new_factor,
					     int x, int y));

/* definition for displaying cells */
#define _O_window	This->X_info_O.window
#define _O_gc_contour	This->X_info_O.gc_contour
#define _O_gc_inside	This->X_info_O.gc_inside
#define _C_window	This->X_info_C.window
#define _C_gc_contour	This->X_info_C.gc_contour
#define _C_gc_inside	This->X_info_C.gc_inside
#define _R_window	This->X_info_R.window
#define _R_gc_contour	This->X_info_R.gc_contour
#define _R_gc_inside	This->X_info_R.gc_inside

#define _gc_select	This->gc_select
#define	_gc_grid1	This->gc_grid1
#define	_gc_grid2	This->gc_grid2
#define	_gc_page_mark	This->gc_page_mark
#define _gc_font_paint	This->gc_font_paint
#define _gc_font_mask	This->gc_font_mask

/* color definition for relief */

#define	COL_RELIEF_LIGHT	0xD000,0xD000,0xD000	
#define	COL_RELIEF_DARK 	0x4000,0x4000,0x4000
#define	COL_BACK_LIGHT  	0xA000,0xA000,0xA000
#define	COL_BACK_DARK		0x7000,0x7000,0x7000
#define COL_GRID_RELIEF		0xA000,0xA000,0xA000

#define Xq_CELL		0
#define Xq_COLUMN	1
#define Xq_ROW		2
#define Xq_ORIGIN	3
#define Xq_CELLS	4

#define NONE_REGION	((region_t *)0)
#define ALL_REGION	((region_t *)1)

#define XcCTM_CLIPMASK			1
#define XcCTM_REDRAW			2
#define XcCT_NO_CLIPMASK_NO_REDRAW	0
#define XcCT_CLIPMASK			1
#define XcCT_REDRAW			2
#define XcCT_CLIPMASK_REDRAW		3
#define XcCT_CLIPMASK_FULL		4

#ifndef offsetof
#define offsetof(type, member) ((long) &((type *) 0)->member)
#endif

#define IN_REGION(reg, col, row) \
 ((reg->x_min <= col) && (reg->x_max >= col) \
  && (reg->y_min <= row) && (reg->y_max >= row))

/* cell info structure */
typedef struct _cell_info_t
{
  int		sequence;
  int		selected;
  cell		*cellule;
  CellStyle_t	*cstyle;
  short		clip_type;
  Pixmap	clip_mask;
  int		clip_origin_x;
  int		clip_origin_y;
  boolean	modified;
  
  int		left_border;
  boolean	left_to_show;
  int		up_border;
  boolean	up_to_show;
  int		right_border;
  boolean	right_to_show;
  int		down_border;
  boolean	down_to_show;
  
  cell		*cell_over_left;
  coord_t	offset_over_left;
  int		col_over_left;
  cell		*cell_over_up;
  coord_t	offset_over_up;
  int		row_over_up;
  
  cell		*cell_over_right;
  coord_t	offset_over_right;
  int		col_over_right;
  cell		*cell_over_down;
  coord_t	offset_over_down;
  int		row_over_down;
} cell_info_t;

/* define the method for Class Feuille */
typedef struct
{
  F_STD;
  void		(*init_Feuille)();
  void 		(*Resize)();
  status_t	(*EventLook)();
  boolean	(*SetFunction) ___PROTO((c_Feuille *This, int num_function));
  boolean	(*IsSelected) ___PROTO((c_Feuille *This, int column, int row,
				   int type));
  boolean	(*IsVisible) ___PROTO((c_Feuille *This, int column, int row,
				       int type));
  void		(*SelectRegion) ___NPROTO((c_Feuille *This, int x_min,
					  int y_min, int x_max, int y_max,
					  int type, boolean draw));
  void		(*DeselectRegion) ___NPROTO((c_Feuille *This, int type, 
					    boolean draw));
  boolean 	(*GetCellByXY) ___PROTO((c_Feuille *This, int x, int y,
					 int *column, int *row));
  boolean 	(*GetColumnByX) ___PROTO((c_Feuille *This, int x,
					  int *column));
  boolean 	(*GetRowByY) ___PROTO((c_Feuille *This, int y, int *row));
  boolean 	(*GetColumnBorderByX) ___PROTO((c_Feuille *This, 
						 int x, int *column));
  boolean 	(*GetRowBorderByY) ___PROTO((c_Feuille *This, int y,
					      int *row));
  void		(*GetCellGeometry) ___PROTO((c_Feuille *This, int column,
					     int row, int type, int *x,
					     int *y,int *width,int *height)); 
  cell 		*(*GetActiveCellule) ___PROTO((c_Feuille *This, int *column,
					       int *row));
  void		(*SetActiveCellule) ___NPROTO((c_Feuille *This, int column,
					      int row, boolean draw));
  void		(*SetActiveCelluleRelative) ___NPROTO((c_Feuille *This,
						      int column,
						      int row,
						      boolean draw));
  void 	        (*SetActiveCelluleAdvanceSelection)___PROTO((c_Feuille *This));
  
  void		(*RefreshCellule) ___PROTO((c_Feuille *This, int column,
					    int row));
  void 		(*RefreshRegion) ___PROTO((c_Feuille *This, 
					   region_t *region, int type));
  void		(*CalculPageWindow) ___PROTO((c_Feuille *This, 
					      boolean redraw));
  void		(*getBoundingRegion) ___PROTO((c_Feuille *this, 
					       region_t *region));
  void		(*getPagesCount) ___PROTO((c_Feuille *this, region_t *region,
					   unsigned int *wp, 
					   unsigned int *hp));
  /*___Compute scaling to fit width*height page___*/
  scale_t (*computeScaling) ___PROTO((c_Feuille *this, region_t *region,
				      unsigned int width,
				      unsigned int height));
  void		(*SetCellStyleInside) ___PROTO((c_Feuille *This, 
						cell_inside_t *inside));
  void		(*SetCellStyleFillColor) ___PROTO((c_Feuille *This,
						   c_Color *color));
  void		(*SetCellStyleBorders) ___PROTO((c_Feuille *This, 
						cell_border_t *borders));
  void		(*SetCellStyleFormatNbr) ___PROTO((c_Feuille *This, 
						   c_FormatNbr *formatnbr));
  void		(*SetCellStyleTextStyle) ___PROTO((c_Feuille *This,
						   c_TextStyle *textstyle));
  void		(*SetColumnAndRowSize) ___PROTO((c_Feuille *This,
						 colrow_size_t	*colrow));
  void		(*SetCellStyleFontFamily) ___PROTO((c_Feuille *This,
						    family_tree *family));
  void		(*SetCellStyleFontColor) ___PROTO((c_Feuille *This,
						    c_Color *color));
  void		(*SetCellStyleFontSize) ___PROTO((c_Feuille *This,
						  coord_t size));
  void		(*ResizeColumnIdealy) ___PROTO((c_Feuille *This, int column));
  void		(*ResizeRowIdealy) ___PROTO((c_Feuille *This, int row));
  void		(*HookCellStyleCallback) ___PROTO((c_Feuille *This, 
						   CellStyle_t *cstyle, 
						   int reason));
  /*___Print in Poscript format___*/
  boolean	(*ps_print) ___PROTO((c_Feuille *this, c_PostScript *post));

  /*___Loading & saving___*/
  boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			    long param, c_Feuille *this));
  boolean (*write) ___PROTO((c_Feuille *this, c_DocFile *doc));

} sf_Feuille;

/* define the Feuille Class */
struct sc_Feuille
{
  sf_Feuille       	*f;
  boolean	        X_info_calc; /* True if X_info usable */
  x_info		X_info;      /* x misc info as display etc... */
  x_info		X_info_O;    /* X info on Origin Window */
  x_info		X_info_C;    /* X_info on Column Window */	
  x_info		X_info_R;    /* X_info on Row Window */
  
  GC			gc_select;
  GC			gc_grid1;
  GC			gc_grid2;
  GC			gc_font_paint;
  GC			gc_font_mask;
  
  c_TextStyle		*orig_style;
  c_Color		*orig_color;
  c_RasterFont		*orig_font;
  c_CharMetrics		*orig_metrics;
  
  c_Cursor		*cursor;
  c_Cursor		*cursor_O;
  c_Cursor		*cursor_C;
  c_Cursor		*cursor_R;
  BaseStd_t		*BaseStd;
  c_ScrollBarX		*ScrollBarV;
  c_ScrollBarX		*ScrollBarH;
  void			*Pager;
  c_Moteur		*moteur;
  c_FrMg		*frmg;
  
  int			PosX;
  int			PosY;
  int			Width;
  int			Height;	
  
  long			col_relief_light; /* valeur de pixel pour */
  long			col_relief_dark;  /* le relief des colonnes */
  long			col_back_light;   /* et lignes */
  long			col_back_dark;
  
  coord_t		sScale;	     /* feuille scaling */	
  scale_t		sFactor;     /* zoom factor in scale_t */	
  
  int			sequence;	/* sequence time */
  int			column_orig;
  int			row_orig;
  short			nb_column;
  short			*column_width;
  short			nb_row;
  short			*row_height;
  int			origin_width;
  int			origin_height;
  int			sheet_width;
  int			sheet_height;
  
  short			nb_column_ci;	/* ci = cell_info */
  short			nb_row_ci;			
  cell_info_t		**cell_info;	/*size: nb_column*nb_row */
  int			*column_info;
  int			*row_info;
  int			origin_info;
  
  boolean		grid;
  boolean		Column_Origin_actif;
  boolean		Row_Origin_actif;
  boolean		select_type; /* TRUE: cell region; FALSE: frame */
  
  int			cell_x;
  int			cell_y;
  
  int			nb_cell_region;
  region_t		*cell_region;
  int			nb_col_region;
  region_t		*col_region;
  int			nb_row_region;
  region_t		*row_region;
  region_t		*last_advance;
  
  sf_feuille_function	*function;
  int			function_type;
  void			*function_data;

  setup_print_t		*print_setup;
  boolean		origin_selected;
  boolean		page_mark;	/* TRUE: display page separation */
  coord_t		page_window_width;  /* for displaying page marks */
  coord_t		page_window_height; /* idem */
  GC			gc_page_mark;
  boolean		*column_page_mark;
  boolean		*row_page_mark;
  boolean		*modified;
};

extern sf_Feuille fc_Feuille;

#endif




