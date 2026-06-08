/*
** Moteur.h for XQuad in Moteur/
** Object and methods for Moteur
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
** Started on  Fri Dec 16 15:12:00 1994 Antoine Buat
** Last update Tue Feb 18 14:01:05 1997 Emmanuel Paris
*/

#ifndef _Moteur_h
#define _Moteur_h

/*cellvalue_t is defined in global_defs.h */

typedef struct _c_Moteur c_Moteur;
typedef struct _cell cell;
typedef struct moteur_overrun_s moteur_overrun_t;
typedef struct _cell_style_list cell_style_list;
typedef struct _cell_depend refresh_list;


#include "pari_util.h"
#include "xcalibur.h"
#include "CellStyle.h"
#define _genpari_h
#include "Parser.h"
#include "High_Graph.h"
#include "BaseStd.h"
#include "PostScript.h"
#include "Resource.h"

#define XcDF_AREALIST_KEYWORD "AREALIST"
#define XcDF_AREA_KEYWORD "AREA"
#define XcDF_WIDTHLIST_KEYWORD "WIDTHLIST"
#define XcDF_HEIGHTLIST_KEYWORD "HEIGHTLIST"
#define XcDF_CELLLIST_KEYWORD "CELLLIST"
#define XcDF_CELL_KEYWORD "CELL"
#define XcDF_X_KEYWORD "X"
#define XcDF_Y_KEYWORD "Y"
#define XcDF_VALUE_KEYWORD "VALUE"
#define XcDF_TEXT_KEYWORD "TEXT"
#define XcDF_FORMULA_KEYWORD "FORMULA"
#define XcDF_VT_PERCENT_KEYWORD "PERCENT"
#define XcDF_VT_UNIT_KEYWORD "UNIT"
#define XcDF_VT_FRACTION_KEYWORD "FRACTION"
#define XcDF_VT_BOOLEAN_KEYWORD	"BOOLEAN"
#define XcDF_VT_DAYINWEEK_KEYWORD "DAY_IN_WEEK"
#define XcDF_VT_MONTH_KEYWORD "MONTH"
#define XcDF_VT_DATE_KEYWORD "DATE"
#define XcDF_VT_TIME_KEYWORD "TIME"
#define XcDF_VT_DATE_TIME_KEYWORD "DATE_TIME"
#define XcDF_VT_EXPONANTIAL_KEYWORD "EXPONANTIAL"

#define DEFAULT_ROW_SIZE (SCALE_FROM_MILLIMETERS(6.0)\
			   /COORD_DEFAULT*COORD_DEFAULT)
#define DEFAULT_COL_SIZE (SCALE_FROM_MILLIMETERS(20.0)\
			   /COORD_DEFAULT*COORD_DEFAULT)

/*--- copy mode for paste ---*/
#define Xc_MODE_NOTHING	-1
#define Xc_MODE_CUT	0
#define Xc_MODE_COPY	1

#define BOINK		NULL

#define Xc_LINE		1
#define Xc_ROW		Xc_LINE
#define Xc_COLUMN	2

/*--- different type of cells ---*/
#define Xq_VOID		0
#define Xq_TEXT		1
#define Xq_FORMULA	2
#define Xq_FORMERR	3


#define Xq_PERCENT	4
#define Xq_VALUE	5
#define Xq_UNIT		6
#define Xq_FRACTION	7
#define Xq_BOOLEAN	8
#define Xq_DAY_IN_WEEK	9
#define Xq_MONTH	10
#define Xq_TIME		11
#define Xq_DATE		12
#define Xq_DATE_TIME	13
#define Xq_EXPONANTIAL	14
#define Xq_NB_TYPE	15

#define Xq_IS_VALUE(type) (type >= Xq_PERCENT)
#define Xq_IS_BOOLEAN(type) (type == Xq_BOOLEAN)

#define OVERRUN_VERTICALY	FALSE	/* if Xq_TEXT and ruler= not ANGLE 0 */
#define OVERRUN_HORIZONTALY	TRUE	/* if Xq_TEXT and ruler = ANGLE 0 */
#define OVERRUN_NEGATIVE	FALSE	/* if hz, overrun on left else up */
#define OVERRUN_POSITIVE	TRUE	/* if hz, overrun on right else down */

struct _c_High_Graph;

/*--- Overrun structure ---*/
struct moteur_overrun_s {
  /*___Next one___*/
  moteur_overrun_t *next;

  /*___Overruning cell___*/
  int	row;
  int	column;
  cell	*ptr;

  /*___Region used in overrun___*/
  region_t region;
  int	code;
};

/*--- Print structure ---*/
struct moteur_print_s {
  c_PostScript *post;
  cell_style_list *areas;
  rowcol_info_t *col;
  rowcol_info_t *row;
  CellBorderStyle_t **varray;
  CellBorderStyle_t **harray;
  long w;
  long h;
};

/*--- Cell Style list structure ---*/
struct _cell_style_list
{
  region_t			region;
  CellStyle_t			*cstyle;
  int				id;
  struct _cell_style_list	*Next;
};

/*--- Cell Size list structure ---*/
typedef struct _cell_size_list
{
  int				Amin;
  int				Amax;
  coord_t			size;
  struct _cell_size_list	*Next;
} cell_size_list;

typedef struct _cell_depend
{
  int			line;
  int			column;
  cell			*cell_p;
  struct _cell_depend	*Next;
} cell_depend;

/**/
typedef struct _cell_depend_sort cell_depend_sort;
typedef struct _cell_depend_list cell_depend_list;

struct _cell_depend_sort
{
  /* content */
  int				line;
  int				column;
  cell				*cell_p;
  /* son */
  cell_depend_list	*Next;
};

struct _cell_depend_list
{
  cell_depend_sort	depend;
  boolean		expanded;
  /* brothers */
  struct _cell_depend_list	*Next;
};
/**/

typedef struct _HG_list
{
  c_HighGraph			*Graph;
  struct _HG_list		*Next;
} HG_list;

typedef struct _zone_depend
{
  region_t			region;
  int				line;
  int				column;
  cell				*cell_p;
  struct _zone_depend		*Next;
} zone_depend;

/*--- cell structure ---*/
struct _cell     
{
  int		type;		/* Xq_[TEXT,FORMULA,VOID,VALUE,FORMERR]*/
  char		*text;
  char		*formula;
  char		ret_type;	/* return type to understand value */
  char		status;		/* store the status error */
  cellvalue2_t	value2;
  cell_depend	*depend;	/* involved in wich formula */
  HG_list	*graph_list;	/* graph dependance */
  
  coord_t	overrun_pos;	/* overrun positive (right or down) */
  coord_t	overrun_neg;	/* overrun negative (left or up) */
  boolean	overrun_direction;	/* see OVERRUN defines above */
  CellStyle_t	*overrun_cstyle; /* overrun calculated with this cstyle */
  coord_t	overrun_size;	/* size of cell width or height */
};
/* - Xq_TEXT contain text only
   - Xq_FORMULA contain formula and return text or value
   - Xq_VOID is a null cellule stored for dependances
*/ 

typedef struct _c_line
{
  int			number;
  cell			cell;
  struct _c_line	*Prev;
  struct _c_line	*Next;
} c_line;

typedef struct _c_col
{
  int			number;
  c_line		*line;
  struct _c_col		*Prev;
  struct _c_col		*Next;
  c_line		*last_pointed;
  c_line		**last_added;
} c_col;

/*--- define methods for Class Moteur ---*/
typedef struct
{
  F_STD;
  /*___value get & delete___*/
  cell *(*add_cell)();
  void 	(*del_cell)();
  boolean (*get_val)();
  cell *(*get_cell)();

  /*___size settings___*/
  coord_t (*get_size)();
  void (*set_size)();
  void (*set_size_zone) ___PROTO((c_Moteur *This, int type,
				  region_t *region, coord_t value));
  /*___string setting___*/  
  int (*get_string)();    /* return Xq_VOID, Xq_VALUE, Xq_TEXT .... */
  void (*set_string)();

  /*___dependance on cells___*/
  void (*add_depend)();
  void (*sub_depend)();
  void (*add_graph_dep)();
  void (*sub_graph_dep)();
  void (*add_depend_zone)();
  void (*sub_depend_zone)();
  boolean (*check_c_depend)();
  boolean (*check_c_depend_cell)();

  /*___style settings___*/
  void (*set_cell_style2)();
  CellStyle_t *(*get_cell_style)();
  void (*or_cell_style)();
  int	(*move_style)();
  int	(*copy_style)();

  /*___zone moving, extending, copying ...___*/
  int	(*move_zone)();
  int	(*copy_zone)();
  void (*cut_zone)();
  region_t *(*paste_zone)();
  void (*delete_zone)();
  void (*copy_zone_r_or_d)();
  void (*extend_zone)();

  /*___insert and delete column or row___*/
  void (*insert_zone_col)();
  void (*insert_zone_row)();
  void (*delete_zone_col)();
  void (*delete_zone_row)();

  coord_t (*calcul_page_origin) ___PROTO((c_Moteur *This, int type, int pos, 
				coord_t page_size));

  /*___Overrun methods___*/
  boolean (*test_overrun) ___PROTO((c_Moteur *This, cell **pcell_s,
				    int x, int y, boolean direct,
				    boolean sign, scale_t factor));
  void (*computeOverrun) ___PROTO((c_Moteur *This, cell *cell_s,
				   int x, int y, scale_t factor));
  void (*getOverrunRegion) ___PROTO((c_Moteur *this, cell *cellp,
				     int x, int y, region_t *regionp));
  moteur_overrun_t *(*getOverrunCells) ___PROTO((c_Moteur *this,
						 region_t *region));
  void (*freeOverrunCells) ___PROTO((c_Moteur *this, moteur_overrun_t *ptr));

  /*___compute ideal size___*/
  coord_t (*get_ideal_column_width) ___NPROTO((c_Moteur *This, int column,
					       scale_t factor));
  coord_t (*get_ideal_row_height) ___NPROTO((c_Moteur *This, int row,
					     scale_t factor));

  /*___Look for Filled region___*/
  boolean (*get_filled_region) ___PROTO((c_Moteur *This, region_t *region));

  /*___Read & write methods___*/
  boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			    long param, c_Moteur *this));
  boolean (*write) ___PROTO((c_Moteur *this, c_DocFile *doc));

  /*___Print___*/
  boolean (*ps_print) ___PROTO((c_Moteur *this, c_PostScript *post,
				region_t *region, setup_print_t *setup));
  void (*getBoundingRegion) ___PROTO((c_Moteur *this, region_t *regionp));
  void (*getBoundingBox) ___PROTO((c_Moteur *this,
				   region_t *region, bbox_t *bboxp));

  void (*getCellByXY) ___PROTO((c_Moteur *this, coord_t x, coord_t y,
				int *columnp, int *rowp, bbox_t *bboxp));

  cell_style_list *(*getCellStyleList) ___PROTO((c_Moteur *this, region_t *r));


/* deffered for recompilation */
  void (*insert_size_zone) ___PROTO((c_Moteur *This, int type,
				     int z_start, int z_end));
  void (*delete_size_zone) ___PROTO((c_Moteur *This, int type,
				     int z_start, int z_end));
  boolean (*test_cell_overrun) ___PROTO((c_Moteur *This, cell *cell,
					 CellStyle_t *cstyle, int x, int y,
					 boolean direct, boolean sign,
					 scale_t factor));
  void (*fill_cell_info) ___NPROTO((c_Moteur *This, cell_info_t **cell_info,
				   region_t *region, int xo, int yo));

  void (*simple_set_string) ___NPROTO((c_Moteur	*This, int x, int y,
				       cell **cell_s, char *str,
				       c_Feuille *sel_feuille));
} sf_Moteur;

/*-- define the Moteur Class ---*/
struct _c_Moteur
{
  sf_Moteur		*f;
  c_col			*tableCL;
  c_Parser		*parser; /*each document have there one parser
				   because it contain local variables
				   and functions */
  cell_style_list	*style_list;
  BaseStd_t		*BaseStd;
  int			id_func; /* used by frag style */

  /*___Selection___*/
  region_t		sel_region;
  int			mode;

  /*___Force Set Mode___*/
  int			force;

  /*___Size Zone___*/
  cell_size_list	*col_size;
  cell_size_list	*line_size;

  /*___Store Zone dependance for formula that involve region___*/
  zone_depend		*zone_dep;
};

extern sf_Moteur fc_Moteur;
extern c_Resource *GlobResources;

#endif
