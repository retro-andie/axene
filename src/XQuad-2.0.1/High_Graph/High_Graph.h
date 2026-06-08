/*
** High_Graph.h for XQuad in High_Graph/
** Declarations for Object High_Graph
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
** Started on  Fri May 16 13:54:12 1997 Antoine Buat
** Last update Fri May 16 14:48:43 1997 Antoine Buat
*/

#ifndef _High_Graph_h
#define _High_Graph_h

typedef struct _c_HighGraph c_HighGraph;

#include "xcalibur.h"
#if defined(___mips) && !defined(_EditBar_h) && !defined(_Moteur_h)
#define _EditBar_h
typedef struct _c_EditBar c_EditBar;
#endif 
#include "Moteur.h"
#include "Tableau.h"
#include "BaseStd.h"
#include "HighGraphStd.h"


/*--- Loading & saving keywords ---*/
#define XcDF_GRAPH_KEYWORD "GRAPH"


typedef struct 
{
 char		*string;
 c_TextStyle	*tstyle;
 int		column;
 int		row;
} HG_string_t;

/* define methods for Class HighGraph */
typedef struct 
{
 F_STD;
 void (*init) ___PROTO((c_HighGraph *This, c_Moteur *moteur));
 void (*draw) ___PROTO((c_HighGraph *This));
 void (*calcul_and_draw) ___PROTO((c_HighGraph *This));
 void (*set_X_info) ___PROTO((c_HighGraph *This, Display *display, 
			      Window window));
 void (*del_X_info) ___PROTO((c_HighGraph *This));
 void (*find_minmax) ___PROTO((c_HighGraph *This, real *min, real *max));
 int (*configure) ___PROTO((c_HighGraph *This));
 void (*set_unset_legend) ___PROTO((c_HighGraph *This));
 void (*set_unset_title) ___PROTO((c_HighGraph *This));
 void (*set_unset_grad_y) ___PROTO((c_HighGraph *This));
 void (*set_unset_abscisse) ___PROTO((c_HighGraph *This));
 void (*set_unset_ordonnee)  ___PROTO((c_HighGraph *This));
 void (*change_graph_class)  ___PROTO((c_HighGraph *This,
				       HG_class_t class));
 void (*test_tstyle_hook) ___PROTO((c_HighGraph *This,
				    c_TextStyle *tstyle));
 void (*test_tstyle_apply) ___PROTO((c_HighGraph *This,
				     region_t *region));
 /*___Loading & saving___*/
 boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			   long param, c_VectorGraph *vector));
 boolean (*write) ___PROTO((c_HighGraph *this, c_DocFile *doc));
} sf_HighGraph;

/* define the HighGraph Class */
struct _c_HighGraph
{
 sf_HighGraph	*f;
 c_Moteur	*moteur;
 BaseStd_t	*BaseStd;
 c_VectorGraph	*vectorg;
  
 void 		*HG_object;
  
 region_t	*orig_region;
 region_t	*region_data;
 region_t	*region_abscisse;
 region_t	*region_ordonnee;
  
 HG_class_t	class;		/* graph class (BARS, CURVES etc) */

 boolean	col_row;	/* TRUE: data in column */
 boolean	abscisse;	/* TRUE: first data serie = mark */
 boolean	ordonnee;	/* TRUE: first data head = legend */
 boolean	title;		/* TRUE: title exist */
 boolean	legend;		/* TRUE: legend exist */
 boolean	axe;		/* TRUE: axe exist & displayed (no sector) */
  
 boolean	d_abscisse;	/* TRUE: abscisse displayed */
 boolean	d_ordonnee;	/* TRUE: ordonnee displayed */
 boolean	d_title;	/* TRUE: title displayed */
 boolean	d_legend;	/* TRUE: legend displayed */
 boolean	graduation_y;	/* TRUE: graduation in y displayed */
  
 int		nb_abscisse;	/* height of series */
 int		nb_ordonnee;	/* nb of series */
  
 real		**tab_value;    /* size = tab_nb_ordonnee * tab_nb_abscisse */
 HG_string_t	tab_data;
 HG_string_t  	*tab_abscisse;	/* size = tab_nb_abscisse */
 HG_string_t	*tab_ordonnee;  /* size = tab_nb_ordonnee */
 HG_string_t	tab_title;
 int		tab_nb_abscisse;
 int		tab_nb_ordonnee;
 boolean	tab_col_row;

 x_info		X_info;
 c_Matrix	*matrix;
 coord_t	scale;
 boolean	multiple_region;/* TRUE: non unique region */
 boolean	region_direction;/* TRUE: HORIZONTAL; FALSE: VERTICAL */
};

extern sf_HighGraph fc_HighGraph;

#endif

