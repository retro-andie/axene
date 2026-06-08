/*
** Cadre.h for Xclamation, XQuad and XAllWrite in Cadre/
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
** Started on  Mon Jul 17 14:09:12 1995 Emmanuel Paris
** Last update Tue Jul  7 17:48:24 1998 Emmanuel Paris
*/

#ifndef _Cadre_h
#define _Cadre_h

typedef struct sc_Cadre c_Cadre;

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "Xinfo.h"
#include "polygone.h"
#include "Colormap.h"
#include "CadreStd.h"
#include "geometry.h"
#include "contour.h"
#include "PostScript.h"
#include "DocFile.h"
#include "BaseStd.h"

/* define for load & save */
#define XcDF_FRAME_KEYWORD		"FRAME"
#define XcDF_SHAPE_KEYWORD		"SHAPE"
#define XcDF_POINT_KEYWORD		"POINT"
#define XcDF_LINE_KEYWORD		"LINE"
#define XcDF_CIRCLE_KEYWORD		"CIRCLE"
#define XcDF_SPLINE_KEYWORD		"SPLINE"
#define XcDF_FRAME_SELECTED_KEYWORD	"SELECTED"
#define XcDF_TEXTFLOW_KEYWORD		"TEXTFLOW_MODE"

/* definition for line's calculation */
typedef struct 
{
 int x_min;
 int x_max;
} absc;

typedef struct 
{ 
 int x_orig;
 int y_orig;
 int x_dest;
 int y_dest;
 int nb_absc;			/* nb_absc = y_dest-y_orig */
 absc *abscisse;		/* abscisse(x) pour chaque ordonnee(y) */
} Ligne;

#define UPDATE_GC	TRUE
#define NO_UPDATE_GC    FALSE

extern void FreeLigne();
extern Ligne *CalculLigne();

#define NO_CONTOUR TRUE    /* sans contour */
#define CONTOUR    FALSE   /* avec contour */

typedef enum 
{
 XcTFM_UNDER = 0,		/* text flow under the frame */
 XcTFM_OVER,			/* text flow over the frame */
 XcTFM_AROUND,			/* text flow around the frame */
 XcTFM_LEFT,			/* text flow on the left of the frame */
 XcTFM_RIGHT,			/* text flow on the right of the frame */
 XcTFM_SKIP,			/* text skip frame horizontal band */
 XcTFM_HOLE_MASK = 127,		/* bit field mask */
 XcTFM_IN_HOLE = 128		/* text flow in frame holes (bit field) */
} textflow_t;

/* definition of handle geometry */
#define	HANDLE_WIDTH	5
#define HANDLE_HEIGHT	5
#define HANDLE_X	(HANDLE_WIDTH >> 1)
#define HANDLE_Y	(HANDLE_HEIGHT >> 1)
#define	HANDLE_X_ACT	(HANDLE_X+2)
#define	HANDLE_Y_ACT	(HANDLE_Y+2)
#define TRACE_HANDLE(d,w,g,x,y) \
XFillRectangle(d,w,g,x-HANDLE_X,y-HANDLE_Y,HANDLE_WIDTH,HANDLE_HEIGHT)
#define	RHANDLE_WIDTH	7
#define RHANDLE_HEIGHT	7
#define RHANDLE_DWIDTH  7.0
#define RHANDLE_DHEIGHT 7.0
#define RHANDLE_X	(RHANDLE_WIDTH >> 1)
#define RHANDLE_Y	(RHANDLE_HEIGHT >> 1)
#define	RHANDLE_X_ACT	(RHANDLE_X+2)
#define	RHANDLE_Y_ACT	(RHANDLE_Y+2)
#define TRACE_RHANDLE(d,w,g,x,y) \
XFillRectangle(d,w,g,x-RHANDLE_X,y-RHANDLE_Y,RHANDLE_WIDTH,RHANDLE_HEIGHT)

/* definition for set gfx action */
#define GFX_SET		0
#define GFX_REVERSE	1
#define GFX_CLEAR	2

/* some extern definitions */

extern c_Colormap *GlobColormap;
extern void free_list_point();
extern l_Point *duplicate_list_point();
extern void trace_list_point();
extern void rotate_list_point();
extern void scale_list_point();
extern void get_limit_list_point();

/* defint the method for Class Cadre */
typedef struct
{
 F_STD;
 void (*addpoint)();
 void (*addspoint)();
 void (*insertpoint)();
 void (*insertspoint)();
 error (*deletepoint)();
 l_Point *(*getpoint)();
 l_Point *(*getpreviouspoint)();
 l_Point *(*getnextpoint)();
 void (*viewpoint)();
 void (*fusion_cadre)();
 void (*attachobject)();
 void (*detachobject)();
 void (*calculorigine)();
 void (*calculscaling)();
 void (*calculforme)();
 boolean (*isincadre)();
 void (*set_X_info)();
 void (*reinit_X_info)();
 void (*setlinewidth)();
 boolean (*setBackgroundColor) ___PROTO((c_Cadre *this, c_Color *color));
 boolean (*setOutlineColor) ___PROTO((c_Cadre *this, c_Color *color));
 void (*set_contour_action)();
 void (*drawcontour)();
 void (*drawcadre)();
 void (*draw_sommet)();

 void (*draw_handle)();
 /*___Hooks___*/
 void (*colorHook) ___PROTO((c_Cadre *this, int reason,
			     c_Color *old, c_Color *new));
 void (*configure_request) ___PROTO((c_Cadre *This, boolean init));

 /*___Printing ___*/
 boolean (*ps_print) ___PROTO((c_Cadre *this, c_PostScript *post));
 boolean (*ps_print_path) ___PROTO((c_Cadre *this, c_PostScript *post));

 /*___Loading & saving___*/
 boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			   scale_t factor, BaseStd_t *base));
 boolean (*write) ___PROTO((c_Cadre *this, c_DocFile *doc,
			    coord_t dx, coord_t dy));
  
} sf_Cadre;


/* define the Cadre Class */
struct sc_Cadre
{
 sf_Cadre	*f;

 boolean       X_info_calc;	/* True if X_info usable */
 x_info	X_info;			/* x misc info as display etc... */
 BaseStd_t	*BaseStd;	/* base standard */

 char		name[XcMAX_NAME_LENGTH];	/* cadre's name */

 c_Color	*border_color;	/* couleur du contour */
 c_Color	*inside_color;  /* couleur de fond */
 coord_t	thickness;	/* epaisseur du contour */

 coord_t	sOrig_X;	/* Origine en coord_t par rapport */
 coord_t	sOrig_Y;	/* a la fenetre courante */
 coord_t	sWidth;		/* largeur du cadre */ 
 coord_t	sHeight;	/* hauteur du cadre */
 coord_t	saOrig_X;	/* origine reelle: sOrig_X+saOrig_X	*/
 coord_t	saOrig_Y;	/* idem en Y */
 coord_t	saWidth;	/* largeur du cadre sans rotation */
 coord_t	saHeight;	/* hauteur du cadre sans rotation */
 coord_t	sScale;		/* valeur pour la convertion pixel<->coord_t */
 scale_t	sFactor;	/* echelle en scale_t */
 angle_t	sAngle;		/* angle du cadre */
 l_Point	*ListPoint;	/* pointeur sur la liste des points */
 boolean	Forme_calc;	/* True if Forme calculated */
 Polygone      *Forme;		/* pointeur sur le polygone */

 int		object_type;	/* type de l'objet */
 void		*object;	/* pointeur sur l'objet */

 boolean	dirty_flag;	/* True if cadre need a DISPLAY CONFIGURE */
 boolean	in_work;	/* le cadre est en cours de modification */
 boolean	verrou;		/* le cadre est verrouille'=imodifiable */
 boolean	selected;	/* true if cadre selected */
 struct _d_object_std *data_to_object;/* voir dans Global/ObjectStd.h */
  
 coord_t	sConfig_X;	/* delta x for display configure */
 coord_t	sConfig_Y;	/* delta y for display configure */

 Pixmap		clip;		/* Clip mask Pixmap for display redraw */
 textflow_t	textflow_mode;	/* how text flow around this frame */
};

extern sf_Cadre fc_Cadre;

#endif

