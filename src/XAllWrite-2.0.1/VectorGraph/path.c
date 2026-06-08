/*
** path.c for Xclamation and XAllWrite in VectorGraph/
** path functions
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
** Started on  Thu Jan  4 16:47:37 1996 Stéphane Boisson
** Last update Thu May 22 00:04:31 1997 Antoine Buat
*/

#define NHISTORY
#define NTRACE

#include "VectorGraphP.h"

/*--- Macros ---*/
#define BEZIER_SCALEBITS 14
#define BEZIER_HALF (1L << (BEZIER_SCALEBITS - 1))
#define BEZIER_LIMIT (1L << (BEZIER_SCALEBITS + 2))

#define BEZIER_SCALE(value) (((long)(value)) << BEZIER_SCALEBITS)
#define BEZIER_DESCALE(value) (((long)(value)) >> BEZIER_SCALEBITS)
/*#define BEZIER_DESCALE(value) (((value) + BEZIER_HALF) >> BEZIER_SCALEBITS)*/
#define BEZIER_FLATNESS BEZIER_HALF

#define XcVG_PATH_CACHE_INC 1024
#define XcVG_CACHE_LINE(cache, dx, dy, errorcmd) \
do { \
      register XPoint *_xptr; \
       \
	if(((cache)->pos == (cache)->size) && !allocCache(cache)) errorcmd; \
	 _xptr = (cache)->xdata + (cache)->pos++; \
	  _xptr->x = (dx); _xptr->y = (dy); \
   } while(0)


/*--- Clipping ---*/
#if 0

#define XcVG_XPOINT_MAX 16000
#define XcVG_XPOINT_MIN -16000
#define XcVG_CLIP_XPOINT(xcoord) \
do { \
      if((xcoord) > XcVG_XPOINT_MAX) \
      {(xcoord) = XcVG_XPOINT_MAX; Xc_WARNING(("Clip!"));} \
	   else if((xcoord) < XcVG_XPOINT_MIN)\
	   {(xcoord) = XcVG_XPOINT_MIN;  Xc_WARNING(("Clip!"));} \
} while(0)

#define XcVG_CLIP_BEZIER(xcoord) \
do { \
      if((xcoord) > BEZIER_SCALE(XcVG_XPOINT_MAX)) \
      {(xcoord) = BEZIER_SCALE(XcVG_XPOINT_MAX); Xc_WARNING(("Clip!"));} \
  else if((xcoord) < BEZIER_SCALE(XcVG_XPOINT_MIN)) \
    {(xcoord) = BEZIER_SCALE(XcVG_XPOINT_MIN); Xc_WARNING(("Clip!"));} \
} while(0)

#else

#define XcVG_CLIP_XPOINT(xcoord)
#define XcVG_CLIP_BEZIER(xcoord)

#endif


/*--- Prototypes ---*/
static boolean allocCache ___PROTO((vector_path_cache_t *cache));
static boolean bezier ___PROTO((register vector_path_cache_t *cache,
				register long x0, register long y0,
				register long x1, register long y1,
				register long x2, register long y2,
				register long x3, register long y3));
static boolean arc ___PROTO((register vector_path_cache_t *cache,
			     matrix_t *m,
			     long Cx, long Cy,
			     long Rx, long Ry,
			     long Ang1, long Ang2));

/* ----------------------------------------------------------------- ** 
** vectorPathCacheInit - Init patch cache                            ** 
** ----------------------------------------------------------------- */
boolean vectorPathCacheInit(target, source)
vector_path_cache_t *target;
vector_path_cache_t *source;
{
 Xc_TRACE(("InitCache"));

 if(source == NULL)
 {
  target->xdata = NULL;
  target->size = target->pos = 0;
  target->outline_count = target->outline_size = 0;
  target->outline = NULL;
  target->old.x = target->old.y = 0;
  return TRUE;
 }

 /*--- Copy xpoints ---*/
 target->pos = source->pos;
 if((target->size = source->size) != 0)
 {
  target->xdata = Xc_malloc("xpoints", target->size * sizeof(XPoint));
  if(target->xdata == NULL)
  {
   target->size = target->pos = 0;
   target->outline_size = target->outline_count = 0;
   return FALSE;
  }
  memcpy(target->xdata, source->xdata, source->pos * sizeof(XPoint));
 }

 /*--- Copy outline info ---*/
 target->outline_count = source->outline_count;
 if((target->outline_size = source->outline_size) != 0)
 {
  size_t len;

  len = source->outline_size * sizeof(struct vector_cache_stroke_s);
  if((target->outline = Xc_malloc("outline", len)) == NULL)
  {
   target->size = 0;
   target->outline_size = target->outline_count = 0;
   Xc_free(target->xdata);
   return FALSE;
  }
  memcpy(target->outline, source->outline, len);
 }

 target->end.dx = source->end.dx, target->end.dy = source->end.dy;
 target->last.dx = source->last.dx, target->last.dy = source->last.dy;
 target->old.x = source->old.x, target->old.y = source->old.y;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** vectorPathCacheReset - Reset path cache                           ** 
** ----------------------------------------------------------------- */
void vectorPathCacheReset(cache)
vector_path_cache_t *cache;
{
 cache->pos = 0;
 cache->outline_count = 0;
 cache->old.x = cache->old.y = 0;
}


/* ----------------------------------------------------------------- ** 
** vectorPathCacheFree - Restore cache data and re-init cache        ** 
** ----------------------------------------------------------------- */
void vectorPathCacheFree(cache)
vector_path_cache_t *cache;
{
 Xc_TRACE(("FreeCache(%d/%d)", cache->pos, cache->size));

 if(cache->size) Xc_free(cache->xdata);
 if(cache->outline_size) Xc_free(cache->outline);
 cache->xdata = NULL;
 cache->size = cache->pos = 0;
 cache->outline_count = cache->outline_size = 0;
 cache->old.x = cache->old.y = 0;
 cache->outline = NULL;
}


/* ----------------------------------------------------------------- ** 
** vectorPathFlatten - Flatten path to X-Polygon                     ** 
** ----------------------------------------------------------------- */
boolean vectorPathFlatten(path, matrix, cache)
vector_path_t *path;
c_Matrix *matrix;
vector_path_cache_t *cache;
{
 unsigned char *control;
 unsigned int count;
 matrix_t *m, *mb;
 XPoint current; 
 coord_t *data;
 long len;


 Xc_TRACE(("pathFlatten"));

 if((count = path->control_count) == 0) return TRUE;

 /*--- Setup matrix ---*/
 if(!F(matrix).scale(matrix, TRUE, SCALE_FROM_COEF(1 << BEZIER_SCALEBITS),
		     SCALE_FROM_COEF(1 << BEZIER_SCALEBITS))) return FALSE;
 m = F(matrix).get(matrix, 1);
 mb = F(matrix).get(matrix, 0);
 XcM_DUMP_MATRIX("  scale", m);
 XcM_DUMP_MATRIX("  bezier_scale", mb);

 count = path->control_count;
 control = path->control;
 data = path->data;
 if(cache->outline_count) cache->outline_count--;

 /*--- Path control codes ---*/
 while(count--)
  switch(*control++)
  {
   /*--- moveto ---*/
  case XcVG_MOVE_OP:
   Xc_TRACE(("  move(%g, %g)", SCALE_TO_POINTS(data[0]),
	     SCALE_TO_POINTS(data[1])));
   if(cache->pos > 0)
   {
    /*--- Update outline info ---*/
    Xc_ASSERT(cache->outline_count <= cache->outline_size);
    len = cache->pos - cache->outline[cache->outline_count].offset;
    if(len > 1) cache->outline[cache->outline_count++].count = len;
	    
    /*--- Close path ---*/
    current.x = (short)XcM_X(m, cache->end.dx, cache->end.dy);
    current.y = (short)XcM_Y(m, cache->end.dx, cache->end.dy);
    if((current.x != cache->old.x) || (current.y != cache->old.y))
    {
     XcVG_CACHE_LINE(cache, current.x - cache->old.x,
		     current.y - cache->old.y, goto alloc_error);
     cache->old.x = current.x, cache->old.y = current.y;
    }
	    
    /*--- Join base point ---*/
    if((cache->old.x != cache->xdata->x) ||
       (cache->old.y != cache->xdata->y))
    {
     XcVG_CACHE_LINE(cache, cache->xdata->x - cache->old.x,
		     cache->xdata->y - cache->old.y,
		     goto alloc_error);
     cache->old.x = cache->xdata->x, cache->old.y = cache->xdata->y;
    }
   }
	
   /*--- Outline info ---*/
   if(cache->outline_count >= cache->outline_size)
   {
    struct vector_cache_stroke_s *ptr;
	    
    ptr = Xc_realloc("outline", cache->outline,
		     (cache->outline_size + 1) *
		     sizeof(struct vector_cache_stroke_s));
    if(ptr == NULL) goto alloc_error;
    cache->outline = ptr;
    cache->outline_size++;
   }
   cache->outline[cache->outline_count].offset = cache->pos;
   cache->outline[cache->outline_count].base.x = cache->old.x;
   cache->outline[cache->outline_count].base.y = cache->old.y;

   /*--- Start subpath ---*/
   cache->last.dx = cache->end.dx = *data++;
   cache->last.dy = cache->end.dy = *data++;
   current.x = (short)XcM_X(m, cache->last.dx, cache->last.dy);
   current.y = (short)XcM_Y(m, cache->last.dx, cache->last.dy);

   XcVG_CLIP_XPOINT(current.x);
   XcVG_CLIP_XPOINT(current.y);
   if((current.x != cache->old.x) || (current.y != cache->old.y) ||
      !cache->pos)
   {
    XcVG_CACHE_LINE(cache, current.x - cache->old.x,
		    current.y - cache->old.y, goto alloc_error);
    cache->old.x = current.x, cache->old.y = current.y;
   }
   break;
	
   /*--- lineto ---*/
  case XcVG_LINE_OP:
   Xc_TRACE(("  line(%g, %g)", SCALE_TO_POINTS(data[0]),
	     SCALE_TO_POINTS(data[1])));
   cache->last.dx = *data++;
   cache->last.dy = *data++;
   current.x = (short)XcM_X(m, cache->last.dx, cache->last.dy);
   current.y = (short)XcM_Y(m, cache->last.dx, cache->last.dy);

   XcVG_CLIP_XPOINT(current.x);
   XcVG_CLIP_XPOINT(current.y);
   if((current.x != cache->old.x) || (current.y != cache->old.y))
   {
    XcVG_CACHE_LINE(cache, current.x - cache->old.x,
		    current.y - cache->old.y, goto alloc_error);
    cache->old.x = current.x, cache->old.y = current.y;
   }
   break;
	
   /*--- arcto ---*/
  case XcVG_ARC_OP:
   Xc_TRACE(("  arc(%g, %g, %g, %g, %g, %g)",
	     SCALE_TO_POINTS(data[0]), SCALE_TO_POINTS(data[1]),
	     SCALE_TO_POINTS(data[2]), SCALE_TO_POINTS(data[3]),
	     SCALE_TO_POINTS(data[4]), SCALE_TO_POINTS(data[5])));
   if (!arc(cache, m,
	    data[0], data[1],
	    data[2], data[3],
	    data[4], data[5]))
    goto alloc_error;
   cache->last.dx = (long)(data[0]+data[2]*cos(SCALE_TO_RADIANS(data[5])));
   cache->last.dy = (long)(data[1]+data[3]*sin(SCALE_TO_RADIANS(data[5])));
   data += 6;
   cache->old.x = (short)XcM_X(m, cache->last.dx, cache->last.dy);
   cache->old.y = (short)XcM_Y(m, cache->last.dx, cache->last.dy);
   break;
   /*--- curveto ---*/
  case XcVG_CURVE_OP:
   Xc_TRACE(("  curve(%g, %g, %g, %g, %g, %g)",
	     SCALE_TO_POINTS(data[0]), SCALE_TO_POINTS(data[1]),
	     SCALE_TO_POINTS(data[2]), SCALE_TO_POINTS(data[3]),
	     SCALE_TO_POINTS(data[4]), SCALE_TO_POINTS(data[5])));
   if(!bezier(cache,
	      (long)XcM_X(mb, cache->last.dx, cache->last.dy),
	      (long)XcM_Y(mb, cache->last.dx, cache->last.dy),
	      (long)XcM_X(mb, data[0], data[1]),
	      (long)XcM_Y(mb, data[0], data[1]),
	      (long)XcM_X(mb, data[2], data[3]),
	      (long)XcM_Y(mb, data[2], data[3]),
	      (long)XcM_X(mb, data[4], data[5]),
	      (long)XcM_Y(mb, data[4], data[5]))) goto alloc_error;
   data += 4;
   cache->last.dx = *data++;
   cache->last.dy = *data++;
   cache->old.x = (short)XcM_X(m, cache->last.dx, cache->last.dy);
   cache->old.y = (short)XcM_Y(m, cache->last.dx, cache->last.dy);
   break;
	  
   /*--- curve at start cap ---*/
  case XcVG_CURVE_BEGIN_OP:
   Xc_TRACE(("  vcurve(%g, %g, %g, %g)", SCALE_TO_POINTS(data[0]),
	     SCALE_TO_POINTS(data[1]), SCALE_TO_POINTS(data[2]),
	     SCALE_TO_POINTS(data[3])));
   if(!bezier(cache,
	      (long)XcM_X(mb, cache->last.dx, cache->last.dy),
	      (long)XcM_Y(mb, cache->last.dx, cache->last.dy),
	      (long)XcM_X(mb, data[0], data[1]),
	      (long)XcM_Y(mb, data[0], data[1]),
	      (long)XcM_X(mb, data[2], data[3]),
	      (long)XcM_Y(mb, data[2], data[3]),
	      (long)XcM_X(mb, data[2], data[3]),
	      (long)XcM_Y(mb, data[2], data[3]))) goto alloc_error;
   data += 2;
   cache->last.dx = *data++;
   cache->last.dy = *data++;
   cache->old.x = (short)XcM_X(m, cache->last.dx, cache->last.dy);
   cache->old.y = (short)XcM_Y(m, cache->last.dx, cache->last.dy);
   break;
	
   /*--- curve at end cap ---*/
  case XcVG_CURVE_END_OP:
   Xc_TRACE(("  ycurve(%g, %g, %g, %g)", SCALE_TO_POINTS(data[0]),
	     SCALE_TO_POINTS(data[1]), SCALE_TO_POINTS(data[2]),
	     SCALE_TO_POINTS(data[3])));
   if(!bezier(cache,
	      (long)XcM_X(mb, cache->last.dx, cache->last.dy),
	      (long)XcM_Y(mb, cache->last.dx, cache->last.dy),
	      (long)XcM_X(mb, cache->last.dx, cache->last.dy),
	      (long)XcM_Y(mb, cache->last.dx, cache->last.dy),
	      (long)XcM_X(mb, data[0], data[1]),
	      (long)XcM_Y(mb, data[0], data[1]),
	      (long)XcM_X(mb, data[2], data[3]),
	      (long)XcM_Y(mb, data[2], data[3]))) goto alloc_error;
   data += 2;
   cache->last.dx = *data++;
   cache->last.dy = *data++;
   cache->old.x = (short)XcM_X(m, cache->last.dx, cache->last.dy);
   cache->old.y = (short)XcM_Y(m, cache->last.dx, cache->last.dy);
   break;
	
   /*--- closepath ---*/
  case XcVG_CLOSE_OP:
   Xc_TRACE(("  closepath"));
   cache->last.dx = cache->end.dx, cache->last.dy = cache->end.dy;
   current.x = (short)XcM_X(m, cache->last.dx, cache->last.dy);
   current.y = (short)XcM_Y(m, cache->last.dx, cache->last.dy);
	
   XcVG_CLIP_XPOINT(current.x);
   XcVG_CLIP_XPOINT(current.y);
   if((current.x != cache->old.x) || (current.y != cache->old.y))
   {
    XcVG_CACHE_LINE(cache, current.x - cache->old.x,
		    current.y - cache->old.y, goto alloc_error);
    cache->old.x = current.x, cache->old.y = current.y;
   }
   break;
	
   /*--- Unknown control ---*/
  default:
   Xc_BREAK(("Unknown path control code"));
  alloc_error:
   F(matrix).pop(matrix);
   return FALSE;
  }

 F(matrix).pop(matrix);

 /*--- Update outline info ---*/
 Xc_ASSERT(cache->outline_count <= cache->outline_size);
 len = cache->pos - cache->outline[cache->outline_count].offset;
 if(len > 1) cache->outline[cache->outline_count++].count = len;
  
 Xc_TRACE(("pathFlatten done (%d/%d)", cache->pos, cache->size));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** vectorPathComputeBBox - Compute path bbox                         ** 
** ----------------------------------------------------------------- */
void vectorPathComputeBBox(path)
vector_path_t *path;
{
 unsigned char *control;
 boolean first_flag;
 unsigned int count;
 coord_t *data;
 
 Xc_TRACE(("PathComputeBBox"));

 count = path->control_count;
 control = path->control;
 data = path->data;
 first_flag = TRUE;

 while(count--)
  switch(*control++)
  {
   /*--- moveto ---*/
  case XcVG_MOVE_OP:
   Xc_TRACE(("  move(%g, %g)", SCALE_TO_POINTS(data[0]),
	     SCALE_TO_POINTS(data[1])));
   if(first_flag != FALSE)
   {
    Xc_BBOX_INIT(&path->bbox, data[0], data[1]);
    first_flag = FALSE;
   }
   else
    Xc_BBOX_UPDATE(&path->bbox, data[0], data[1]);
   data += 2;
   break;
	
   /*--- lineto ---*/
  case XcVG_LINE_OP:
   Xc_TRACE(("  line(%g, %g)", SCALE_TO_POINTS(data[0]),
	     SCALE_TO_POINTS(data[1])));
   if(first_flag != FALSE)
   {
    Xc_BBOX_INIT(&path->bbox, data[0], data[1]);
    first_flag = FALSE;
   }
   else
    Xc_BBOX_UPDATE(&path->bbox, data[0], data[1]);
   data += 2;
   break;

   /*--- arcto ---*/
  case XcVG_ARC_OP:
   {
    int	angle1;
    int	angle2;
   if(first_flag != FALSE)
   {
    Xc_BBOX_INIT(&path->bbox, 
		 data[0] + data[2]*cos(SCALE_TO_RADIANS(data[4])),
		 data[1] + data[3]*sin(SCALE_TO_RADIANS(data[4])));
    first_flag = FALSE;
   }
   else
    Xc_BBOX_UPDATE(&path->bbox,
		   data[0] + data[2]*cos(SCALE_TO_RADIANS(data[4])),
		   data[1] + data[3]*sin(SCALE_TO_RADIANS(data[4])));

    angle1 = SCALE_TO_DEGREES(data[4]);
    angle2 = SCALE_TO_DEGREES(data[5]);
    if (angle1 < angle2)
    {
     if (angle1 < 90)
      angle1 = 90;
     else
      if (angle1 < 180)
       angle1 = 180;
      else
       if (angle1 < 270)
	angle1 = 270;
       else
	angle1 = 360;
     while (angle1 < angle2)
     {
      Xc_BBOX_UPDATE(&path->bbox,
		     data[0] + data[2]*cos(M_PI*angle1/180),
		     data[1] + data[3]*sin(M_PI*angle1/180));
      angle1 += 90;
     }
    }
    else
    {
     if (angle1 > 270)
      angle1 = 270;
     else
      if (angle1 > 180)
       angle1 = 180;
      else
       if (angle1 > 90)
	angle1 = 90;
       else
	angle1 = 0;

     while (angle1 > angle2)
     {
      Xc_BBOX_UPDATE(&path->bbox,
		     data[0] + data[2]*cos(M_PI*angle1/180),
		     data[1] + data[3]*sin(M_PI*angle1/180));
      angle1 -= 90;
     }
    }
    Xc_BBOX_UPDATE(&path->bbox,
		   data[0] + data[2]*cos(SCALE_TO_RADIANS(data[5])),
		   data[1] + data[3]*sin(SCALE_TO_RADIANS(data[5])));
   }
   data += 6;
   break;

   /*--- curveto ---*/
  case XcVG_CURVE_OP:
   Xc_TRACE(("  curve(%g, %g, %g, %g, %g, %g)", SCALE_TO_POINTS(data[0]),
	     SCALE_TO_POINTS(data[1]), SCALE_TO_POINTS(data[2]),
	     SCALE_TO_POINTS(data[3]), SCALE_TO_POINTS(data[4]),
	     SCALE_TO_POINTS(data[5])));
   if(first_flag != FALSE)
   {
    Xc_BBOX_INIT(&path->bbox, data[0], data[1]);
    first_flag = FALSE;
   }
   else
    Xc_BBOX_UPDATE(&path->bbox, data[0], data[1]);
   Xc_BBOX_UPDATE(&path->bbox, data[2], data[3]);
   Xc_BBOX_UPDATE(&path->bbox, data[4], data[5]);
   data += 6;
   break;
	
   /*--- curve at start cap ---*/
  case XcVG_CURVE_BEGIN_OP:
   Xc_TRACE(("  vcurve(%g, %g, %g, %g)", SCALE_TO_POINTS(data[0]),
	     SCALE_TO_POINTS(data[1]), SCALE_TO_POINTS(data[2]),
	     SCALE_TO_POINTS(data[3])));
   if(first_flag != FALSE)
   {
    Xc_BBOX_INIT(&path->bbox, data[0], data[1]);
    first_flag = FALSE;
   }
   else
    Xc_BBOX_UPDATE(&path->bbox, data[0], data[1]);
   Xc_BBOX_UPDATE(&path->bbox, data[2], data[3]);
   data += 4;
   break;
	
   /*--- curve at end cap ---*/
  case XcVG_CURVE_END_OP:
   Xc_TRACE(("  ycurve(%g, %g, %g, %g)", SCALE_TO_POINTS(data[0]),
	     SCALE_TO_POINTS(data[1]), SCALE_TO_POINTS(data[2]),
	     SCALE_TO_POINTS(data[3])));
   if(first_flag != FALSE)
   {
    Xc_BBOX_INIT(&path->bbox, data[0], data[1]);
    first_flag = FALSE;
   }
   else
    Xc_BBOX_UPDATE(&path->bbox, data[0], data[1]);
   Xc_BBOX_UPDATE(&path->bbox, data[2], data[3]);
   data += 4;
   break;
	
   /*--- closepath ---*/
  case XcVG_CLOSE_OP:
   Xc_TRACE(("  closepath"));
   if(path->control_count == 1)
    Xc_BBOX_INIT(&path->bbox, COORD_ZERO, COORD_ZERO);
   break;

   /*--- Unknown control ---*/
  default:
   Xc_BREAK(("Unknown path control code"));
  }
 Xc_DUMP_BBOX("bbox", &path->bbox);
}



/* ----------------------------------------------------------------- ** 
** bezier - Bezier subdivision                                       ** 
** ----------------------------------------------------------------- */
static boolean bezier(cache, x0, y0, x1, y1, x2, y2, x3, y3)
register vector_path_cache_t *cache;
register long x0;
register long y0;
register long x1;
register long y1;
register long x2;
register long y2;
register long x3;
register long y3;
{
 register long xm, ym;

#if 1
 double sa, ca, h;
  
 xm = x3 - x0;
 ym = y3 - y0;

 if((h = sqrt((double)xm * (double)xm + (double)ym * (double)ym)) == 0.0)
  return TRUE;

 ca = (double)xm / h;
 sa = (double)ym / h;
 ym = ca * (y1 - y0) - sa * (x1 - x0);
 if((ym <= BEZIER_FLATNESS) && (ym >= -BEZIER_FLATNESS))
 {
  ym = ca * (y2 - y0) - sa * (x2 - x0);
  if((ym <= BEZIER_FLATNESS) && (ym >= -BEZIER_FLATNESS))
  {
   /*--- Output a line ---*/
   XcVG_CLIP_BEZIER(x3);
   XcVG_CLIP_BEZIER(y3);
   XcVG_CLIP_BEZIER(x0);
   XcVG_CLIP_BEZIER(y0);

   if((x3 != x0) || (y3 != y0))
    XcVG_CACHE_LINE(cache,
		    BEZIER_DESCALE(x3) - BEZIER_DESCALE(x0),
		    BEZIER_DESCALE(y3) - BEZIER_DESCALE(y0),
		    return FALSE);
   return TRUE;
  }
 }
      
 xm = ((x0 + x3) + 3 * (x1 + x2)) >> 3;
 ym = ((y0 + y3) + 3 * (y1 + y2)) >> 3;
 return(bezier(cache, x0, y0,
	       (x0 + x1) >> 1, (y0 + y1) >> 1, ((x0 + x2) >> 2) + (x1 >> 1),
	       ((y0 + y2) >> 2) + (y1 >> 1), xm, ym) &&
	bezier(cache, xm, ym, 
	       ((x1 + x3) >> 2) + (x2 >> 1), ((y1 + y3) >> 2) + (y2 >> 1),
	       (x2 + x3) >> 1, (y2 + y3) >> 1, x3, y3));
#else

 xm = ABS(x1 - x0);
 ym = ABS(y1 - y0);
 if(xm > ym) ym = xm;

 xm = ABS(x2 - x1);
 if(xm < ym) xm = ym;
 ym = ABS(y2 - y1);
 if(xm > ym) ym = xm;

 xm = ABS(x3 - x2);
 if(xm < ym) xm = ym;
 ym = ABS(y3 - y2);
 if(xm > ym) ym = xm;
  
 if(ym > BEZIER_LIMIT)
 {
  xm = ((x0 + x3) + 3 * (x1 + x2)) >> 3;
  ym = ((y0 + y3) + 3 * (y1 + y2)) >> 3;
  return(bezier(out, x0, y0, (x0 + x1) >> 1, (y0 + y1) >> 1,
		((x0 + x2) >> 2) + (x1 >> 1), ((y0 + y2) >> 2) + (y1 >> 1),
		xm, ym) &&
	 bezier(out, xm, ym, ((x1 + x3) >> 2) + (x2 >> 1),
		((y1 + y3) >> 2) + (y2 >> 1), (x2 + x3) >> 1,
		(y2 + y3) >> 1, x3, y3));
 }
  
 /*--- Output a line ---*/
 if((x3 != x0) || (y3 != y0))
  XcVG_CACHE_LINE(cache,
		  BEZIER_DESCALE(x3) - BEZIER_DESCALE(x0),
		  BEZIER_DESCALE(y3) - BEZIER_DESCALE(y0),
		  return FALSE);
 return TRUE;
#endif
}


/* ----------------------------------------------------------------- ** 
** vectorPathCacheFill - Fill rendering of cache                     ** 
** ----------------------------------------------------------------- */
void vectorPathCacheFill(cache, x, y, display, win, gc)
vector_path_cache_t *cache;
int x;
int y;
Display *display;
Drawable win;
GC gc;
{
 Xc_TRACE(("FillCache(#%ld; %d,%d)", cache->pos, x, y));

 if(cache->pos)
 {
  cache->xdata->x += x, cache->xdata->y += y;
  XFillPolygon(display, win, gc, cache->xdata, (int)cache->pos,
	       Complex, CoordModePrevious);
  cache->xdata->x -= x, cache->xdata->y -= y;
 }
 Xc_TRACE(("done"));
}


/* ----------------------------------------------------------------- ** 
** vectorPathCacheStroke - Stroke rendering of cache                 ** 
** ----------------------------------------------------------------- */
void vectorPathCacheStroke(cache, x, y, display, win, gc)
vector_path_cache_t *cache;
int x;
int y;
Display *display;
Drawable win;
GC gc;
{
 struct vector_cache_stroke_s *outline;
 int i;

 Xc_TRACE(("StrokeCache(#%ld; %d,%d)", cache->pos, x, y));

#if 1
 outline = cache->outline;
 for(i = cache->outline_count; i--; outline++)
 {
  XPoint *ptr = cache->xdata + outline->offset;

  Xc_TRACE(("  (%d;%d) offset=%ld len=%d",
	    outline->base.x, outline->base.y,
	    outline->offset, outline->count));
  ptr->x += x + outline->base.x, ptr->y += y + outline->base.y;
  XDrawLines(display, win, gc, ptr, outline->count, CoordModePrevious);
  ptr->x -= x + outline->base.x, ptr->y -= y + outline->base.y;
 }
#else
 if(cache->pos)
 {
  cache->xdata->x += x, cache->xdata->y += y;
  XDrawLines(display, win, gc, cache->xdata, (int)cache->pos,
	     CoordModePrevious);
  cache->xdata->x -= x, cache->xdata->y -= y;
 }
#endif
 Xc_TRACE(("done"));
}


/* ----------------------------------------------------------------- ** 
** allocCache - Alloc some memory for cache                          ** 
** ----------------------------------------------------------------- */
static boolean allocCache(cache)
vector_path_cache_t *cache;
{
 XPoint *ptr;

 Xc_TRACE(("allocCache(%d ->%d)", cache->size,

	   cache->size + XcVG_PATH_CACHE_INC));
 cache->size += XcVG_PATH_CACHE_INC;
  
 ptr = Xc_realloc("xpoints", cache->xdata, cache->size * sizeof(XPoint));
 if(ptr == NULL)
 {
  cache->size -= XcVG_PATH_CACHE_INC;
  return FALSE;
 }
 cache->xdata = ptr;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** arc - Arc subdivision                                             ** 
** ----------------------------------------------------------------- */
static boolean arc(cache, m, Cx, Cy, Rx, Ry, Ang1, Ang2)
register vector_path_cache_t *cache;
matrix_t *m;
long Cx;
long Cy;
long Rx;
long Ry;
long Ang1;
long Ang2;
{
 long sx1, sx2;
 long sy1, sy2;
 double rAng1, rAng2;
 double Angstep;
 long tx1;
 long ty1;

 sx1 = (long)XcM_X(m, cache->last.dx, cache->last.dy);
 sy1 = (long)XcM_Y(m, cache->last.dx, cache->last.dy);
 
 /*--- determine un pas de progression fonction du rayon et de l'angle ---*/
 /* todo */
 rAng1 = Ang1;
 rAng2 = Ang2;
 Angstep = (rAng2 - rAng1) / 30;

 if (rAng1 < rAng2)
 {
  while (rAng1 < rAng2)
  {
   tx1 = Cx + Rx*cos((double)SCALE_TO_RADIANS(rAng1));
   ty1 = Cy + Ry*sin((double)SCALE_TO_RADIANS(rAng1));
   sx2 = (long)XcM_X(m, tx1, ty1);
   sy2 = (long)XcM_Y(m, tx1, ty1);
   if((sx1 != sx2) || (sy1 != sy2))
   {
    XcVG_CACHE_LINE(cache,
		    sx2 - sx1, sy2 - sy1, return FALSE);
    sx1 = sx2;
    sy1 = sy2;
   }
   rAng1 += Angstep;
  }
  tx1 = Cx + Rx*cos((double)SCALE_TO_RADIANS(rAng2));
  ty1 = Cy + Ry*sin((double)SCALE_TO_RADIANS(rAng2));
  sx2 = (long)XcM_X(m, tx1, ty1);
  sy2 = (long)XcM_Y(m, tx1, ty1);
  if((sx1 != sx2) || (sy1 != sy2))
  {
   XcVG_CACHE_LINE(cache,
		   sx2 - sx1, sy2 - sy1, return FALSE);
   sx1 = sx2;
   sy1 = sy2;
  }
 }
 else
 {
  while (rAng1 >= rAng2)
  {
   tx1 = Cx + Rx*cos((double)SCALE_TO_RADIANS(rAng1));
   ty1 = Cy + Ry*sin((double)SCALE_TO_RADIANS(rAng1));
   sx2 = (long)XcM_X(m, tx1, ty1);
   sy2 = (long)XcM_Y(m, tx1, ty1);
   if((sx1 != sx2) || (sy1 != sy2))
   {
    XcVG_CACHE_LINE(cache,
		    sx2 - sx1, sy2 - sy1, return FALSE);
    sx1 = sx2;
    sy1 = sy2;
   }
   rAng1 += Angstep;
  }
  tx1 = Cx + Rx*cos((double)SCALE_TO_RADIANS(rAng2));
  ty1 = Cy + Ry*sin((double)SCALE_TO_RADIANS(rAng2));
  sx2 = (long)XcM_X(m, tx1, ty1);
  sy2 = (long)XcM_Y(m, tx1, ty1);
  if((sx1 != sx2) || (sy1 != sy2))
  {
   XcVG_CACHE_LINE(cache,
		   sx2 - sx1, sy2 - sy1, return FALSE);
   sx1 = sx2;
   sy1 = sy2;
  }
 }
 return TRUE;
}
