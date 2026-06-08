/*
** polygone.h for Xclamation, XQuad and XAllWrite in Cadre/
** functions for polygone
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
** Started on  Mon Jul 17 14:08:22 1995 Emmanuel Paris
** Last update Tue Apr 28 18:16:45 1998 Emmanuel Paris
*/

#ifndef _polygone_h_
#define _polygone_h_

#include <X11/Xlib.h>

/* define polygone's elements */

typedef struct 
{
 int x_min;
 int x_max;
} Segment_h;

typedef struct 
{
 int nb_seg;
 Segment_h *segment;
}  Ligne_h;

typedef struct 
{
 int nb_lig;		/* nombre de ligne en pixel */
 int Orig_X;		/* origine du cadre en pixel */
 int Orig_Y;     
 int y_min;		/* ordonnee minimale tres souvent a 0 */
 int y_max;		/* ordonnee maximale tres souvent = a la hauteur */
 int x_min;		/* abscisse minimale tres souvent a 0 */
 int x_max;		/* abscisse maximale tres souvent = a la largeur */
 Ligne_h *ligne;	/* pointeur sur la description des lignes */
 void *segbuf_addr;	/* pointeur sur le buffeur des segments */
 int  memory;		/* taille du buffeur des segments en octet */
} Polygone;

/* define the listpoint type */

typedef struct _l_Point 
{
 int		X;
 int		Y;
 boolean	cont;	/* si TRUE alors sommet contigue avec le precedent */
 coord_t	sX;
 coord_t	sY;
 struct _l_Point	*NextPoint;
} l_Point;

/* definition for FreePolygone */

#define DEL_POLYGONE	TRUE
#define NO_DEL_POLYGONE FALSE

/* definition for polygone's calculation */

#define ANGLE_H   0	/* angle horizontal < ou > */
#define ANGLE_V   1	/* angle vertical   ^ ou v */
#define TAKEN     TRUE
#define NOT_TAKEN FALSE

typedef struct 
{
 Segment_h seg_som;
 l_Point   *sommet;
 int	    angle_h;
 boolean   taken;
} Sommet;

typedef struct 
{
 Sommet  som_src;
 Sommet  som_dst;
} Arrete;

/* extern declaration */

extern boolean is_inters();
extern void DrawPolygone();
extern Pixmap CalculPolygoneMask();
extern void FreePolygone();
extern void CalculXminXmax();
extern Polygone *CreateNullPolygone();
extern Polygone *CreateRectPolygone();
extern Polygone *FormatPolygone();
extern Polygone *DuplicatePolygone();
extern void UnionPolygone();
extern void SubtractPolygone();
extern Polygone *IntersectPolygone();
extern void CalculPolygone();
extern boolean IsInPolygone();
extern boolean IsRectIntersPolygone();
extern Polygone	*IntersectRectPolygone();
extern Polygone	*SubtractRectPolygone();
extern Pixmap CalculRectPolygoneMask();

#endif






