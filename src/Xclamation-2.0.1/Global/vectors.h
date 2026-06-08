/*
** vectors.h for Xclamation, XQuad, XAllWrite and XMayday in Global/
** Definitions of misc. vector structures
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
** Started on  Mon Jun 13 00:08:10 1994 Stéphane Boisson
** Last update Thu Jan 25 17:02:53 1996 Stéphane Boisson
*/

#ifndef __VECTORS_H__
#define __VECTORS_H__

#include "units.h"

/*--- Bounding box ---*/
typedef struct {
  coord_t llx;
  coord_t lly;
  coord_t urx;
  coord_t ury;
} bbox_t;


/*--- Init bounding box computing ---*/
#define Xc_BBOX_INIT(bbox, px, py) \
 ((bbox)->llx = (bbox)->urx = (px), (bbox)->lly = (bbox)->ury = (py))

/*--- Update bounding box computing ---*/
#define Xc_BBOX_UPDATE(bbox, px, py) \
do { \
  if((bbox)->llx > (px)) (bbox)->llx = (px); \
  if((bbox)->lly > (py)) (bbox)->lly = (py); \
  if((bbox)->urx < (px)) (bbox)->urx = (px); \
  if((bbox)->ury < (py)) (bbox)->ury = (py); \
} while(0);

/*--- Copy a bounding box ---*/
#define Xc_BBOX_COPY(target, source) \
((target)->llx = (source)->llx, (target)->lly = (source)->lly, \
 (target)->urx = (source)->urx, (target)->ury = (source)->ury)

/*--- Compute union of two bounding bboxes ---*/
#define Xc_BBOX_UNION(target, source) \
do { \
  if((target)->llx > (source)->llx) (target)->llx = (source)->llx; \
  if((target)->lly > (source)->lly) (target)->lly = (source)->lly; \
  if((target)->urx < (source)->urx) (target)->urx = (source)->urx; \
  if((target)->ury < (source)->ury) (target)->ury = (source)->ury; \
} while(0);


/*--- Test intersection between two bounding boxes ---*/
#define Xc_BBOX_INTERSECTION(a, b) \
  ((MIN((a)->urx, (b)->urx) >= MAX((a)->llx, (b)->llx)) && \
   (MIN((a)->ury, (b)->ury) >= MAX((a)->lly, (b)->lly)))

/*--- test if point in bounding box ---*/
#define Xc_BBOX_TEST(bbox, xc, yc) \
(((bbox)->llx <= (xc)) && ((bbox)->urx >= (xc)) && \
 ((bbox)->lly <= (yc)) && ((bbox)->ury >= (yc)))


/*--- Dump trace of bounding box ---*/
#define Xc_DUMP_BBOX(str, bboxp) \
  Xc_TRACE(("%s: X: %g %g  Y: %g %g", (str), \
	    SCALE_TO_POINTS((bboxp)->llx), SCALE_TO_POINTS((bboxp)->urx), \
	    SCALE_TO_POINTS((bboxp)->lly), SCALE_TO_POINTS((bboxp)->ury)))

/*--- vector ---*/
typedef struct {
  coord_t dx;
  coord_t dy;
} vector_t;

#endif /* !__VECTORS_H__ */



