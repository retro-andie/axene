/*
** CharMetrics.c for Xclamation, XQuad and XAllWrite in VectorFont/
** Methods for the CharMetrics class
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
** Started on  Fri Jul  8 18:59:02 1994 Stéphane Boisson
** Last update Mon Oct 12 13:39:51 1998 Emmanuel Paris
*/

#define NHISTORY
/*#define NTRACE*/

#include "CharMetrics.h"
#include "FontManager.h"
#include "char_translate.h"
#include <math.h>

extern c_FontManager *GlobFntMng;

/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copier();

static c_CharMetrics *transform ___PROTO((c_CharMetrics *this, scale_t scale,
					  scale_t width, angle_t angle));
static c_CharMetrics *findTransform ___PROTO((c_CharMetrics *this,
					      scale_t scale,
					      scale_t width, angle_t angle));
static c_CharMetrics *makefont ___PROTO((c_CharMetrics *this, matrix_t *matrix,
					 coord_t size, scale_t width));
static void unmakefont ___PROTO((c_CharMetrics *this, matrix_t *matrix,
				 coord_t size, scale_t width));
static void purge_impure ___PROTO((c_CharMetrics *this, scale_t scale,
				   scale_t width, angle_t angle));
static void add_EncodedChar ___PROTO((c_CharMetrics *obj, char_t code,
				      char *encoding_name, vector_t *width,
				      bbox_t *char_bbox));
static void add_Char ___PROTO((c_CharMetrics *obj, char_t code,
			       char *encoding_name, vector_t *width,
			       bbox_t *char_bbox, vector_t *vv));
static void add_TrackKern ___PROTO((c_CharMetrics *obj, track_kern_t *data));
static void add_Ligature ___PROTO((c_CharMetrics *obj, char_t code,
				   ligature_t *data));
static void add_KernPair ___PROTO((c_CharMetrics *obj, char_t code,
				   kern_pair_t *data));
static void add_CompositePart ___PROTO((c_CharMetrics *obj, char_t code,
					composite_part_t *data));
static c_CharMetrics *packMetrics ___PROTO((c_CharMetrics *this));
static coord_t textWidth ___PROTO((c_CharMetrics *this,
				   char_t *text, long len));
static coord_t textWidthSpec ___PROTO((c_CharMetrics *this, char_t *text,
				       long len, coord_t char_inc,
				       coord_t word_inc));
static coord_t textHeight ___PROTO((c_CharMetrics *this,
				    char_t *text, long len));
static boolean compare ___PROTO((c_CharMetrics *metrics1,
				 c_CharMetrics *metrics2));

static long calibrate ___PROTO((c_CharMetrics *this, coord_t width,
				char **textp, char *end));

static boolean lockMetrics ___PROTO((c_CharMetrics *this, void *data));
static void unlockMetrics ___PROTO((c_CharMetrics *this, void *data));
static boolean isLockedMetrics ___PROTO((c_CharMetrics *this, void *data));

static void fast_copy ___PROTO((c_CharMetrics *this, c_CharMetrics *dest));
static void fast_free ___PROTO((c_CharMetrics *this));

sf_CharMetrics fc_CharMetrics =
{
 constructor, destructor, copier,
 transform, purge_impure,
 add_EncodedChar,
 add_Char,
 add_TrackKern,
 add_Ligature,
 add_KernPair,
 add_CompositePart,
 packMetrics,
 textWidth,
 textHeight,
 compare,
 calibrate,
 makefont, unmakefont,
 lockMetrics, unlockMetrics, isLockedMetrics,
 findTransform,
 fast_copy, fast_free,
 textWidthSpec
};


/* ----------------------------------------------------------------- ** 
** constructor - CharMetrics class constructor                       ** 
** ----------------------------------------------------------------- */
static void *constructor()
{
 c_CharMetrics *this;

 Xc_HISTORY(("constructor"));

 this = Xc_malloc("Temp CharMetrics", sizeof(c_CharMetrics));
 if(this == NULL) return NULL;

 memset(this, 0, sizeof(c_CharMetrics));
 this->f = &fc_CharMetrics;

 this->previous = this->next = NULL; 
 this->point_size = SCALE_FROM_POINTS(1);
 this->width = SCALE_FROM_COEF(1.0);
 this->scale = SCALE_FROM_COEF(1.0);
 this->angle = SCALE_FROM_DEGREES(0);
 this->font_bbox.llx = this->font_bbox.urx = COORD_ZERO;
 this->font_bbox.lly = this->font_bbox.ury = COORD_ZERO;
 this->vvector.dx = this->vvector.dy = COORD_ZERO;
 this->cap_height = this->x_height = COORD_ZERO;
 this->ascender = this->descender = COORD_ZERO;

 this->attributes = XcCM_B_NONE;
 this->pure = TRUE;
 this->pack_len = 0;
 this->pack_handle = NULL;
 this->counter = 1;
 this->raster_font = NULL;
 XcM_INIT_MATRIX(&this->matrix, 1.0, 0.0, 0.0, 1.0, COORD_ZERO, COORD_ZERO);
 XcM_INIT_MATRIX(&this->final_matrix, 1.0, 0.0, 0.0, 1.0,
		 COORD_ZERO, COORD_ZERO);
 XcM_INIT_MATRIX(&this->final_matrix2, 1.0, 0.0, 0.0, 1.0,
		 COORD_ZERO, COORD_ZERO);

 return this;
}


/* ----------------------------------------------------------------- ** 
** destructor - CharMetrics class destructor                         ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_CharMetrics *this;
{
 long i;

 Xc_HISTORY(("destructor"));

 if(this->raster_font != NULL) 
  F(GlobFntMng).delete_font(GlobFntMng, this->raster_font); 
  
 /*--- Destroy all copies when destroying the original ---*/
 if(this->previous == NULL)
  while(this->next != NULL)
   destructor(this->next);
 else if(this->pure != FALSE)
  while((this->next != NULL) && (this->next->pure == FALSE))
   destructor(this->next);
    
  
 /*--- Remove from the linked list ---*/
 if(this->next != NULL)
  this->next->previous = this->previous;
 if(this->previous != NULL)
  this->previous->next = this->next;

 if ((i = F(this).isLocked(this, NULL)) != 0)
 {
  int j;
  void **ptr;
    
  i = F(this->hook).getClientData(this->hook, &ptr);
  for(j = 0; j < i; j++)
   XcH_UNREGISTER(this, NULL, ptr[j]);
 }
  
 if(this->hook != NULL) DELETE(c_Hook)(this->hook);

 if(this->pack_len == 0)
 {
  /*--- Free inlined mallocs ---*/
  if(this->track_kern != NULL)
   Xc_free(this->track_kern);
      
  for(i = Xc_CHAR_TOTAL; i--;)
  {
   if(this->encoded[i] != NULL)
    Xc_free(this->encoded[i]);
   if(this->glyph[i] != NULL)
    Xc_free(this->glyph[i]);
  }
  Xc_free(this);
 }
 else
 {
  if((this->next == NULL) && (this->previous == NULL))
   Xc_destroyItem(this->pack_handle);
  else
   Xc_delete(this->pack_handle, this);
 }
}


/* ----------------------------------------------------------------- ** 
** packMetrics - Pack inline mallocs                                 ** 
** ----------------------------------------------------------------- */
static c_CharMetrics *packMetrics(this)
c_CharMetrics *this;
{
 c_CharMetrics *new;
 ItemHandle *handle;
 unsigned long len;
 coord_t width;
 char *ptr;
 long i;

 Xc_HISTORY(("packMetrics"));
 Xc_ASSERT(this->pack_len == 0);
 /*  Xc_ASSERT((this->next == NULL) && (this->previous == NULL)); */

 /*--- Fix metrics from strange AFM files ---*/
 if(this->ascender == this->descender)
 {
  this->descender = this->font_bbox.lly;
  this->ascender = this->font_bbox.ury;
 }
 if(this->cap_height == COORD_ZERO) this->cap_height = this->ascender;
 if(this->x_height == COORD_ZERO) this->x_height = this->ascender;

 /*--- Pack metrics ---*/
 len = (sizeof(c_CharMetrics)
	+ (this->track_kern_count * sizeof(track_kern_t)));

 for(i = Xc_CHAR_TOTAL; i--;)
 {
  if (i == 160 - Xc_FIRST_CHAR && this->encoded[i] == NULL && 
      this->encoded[0] != NULL)
  {
   len += sizeof(simple_char_metrics_t);
   if (this->glyph[i] == NULL && this->glyph[0] != NULL)
    len += XcCM_SIZEOF(this->glyph[0]);
  }
  if(this->encoded[i] != NULL) len += sizeof(simple_char_metrics_t);
  if(this->glyph[i] != NULL) len += XcCM_SIZEOF(this->glyph[i]);
 }

 if((handle = Xc_createItem("CharMetrics packed base", 3, len)) == NULL)
  return NULL;

 if((new = Xc_new(handle)) == NULL)
 {
  Xc_destroyItem(handle);
  return NULL;
 }

 memcpy(new, this, sizeof(c_CharMetrics));
 new->pack_len = len;
 new->pack_handle = handle;

 /*--- Copy track kerning data ---*/
 ptr = ((char *)new) + sizeof(c_CharMetrics);
 len = this->track_kern_count * sizeof(track_kern_t);
 if(len != 0)
 {
  memcpy(ptr, this->track_kern, len);
  new->track_kern = (track_kern_t *)ptr;
  ptr += len;
 }

 /*--- Copy char data ---*/
 for(i = Xc_CHAR_TOTAL, width = COORD_ZERO; i--;)
 {      
  /*--- Case for nobreakspace. If not defined copy space ---*/
  if (i == 160 - Xc_FIRST_CHAR && new->encoded[i] == NULL && 
      this->encoded[0] != NULL)
  {
   new->encoded[i] = (simple_char_metrics_t *)ptr;
   memcpy(ptr, this->encoded[0], sizeof(simple_char_metrics_t));
   ptr += sizeof(simple_char_metrics_t);
   
   if (new->glyph[i] == NULL && this->glyph[0] != NULL)
   {
    len = XcCM_SIZEOF(this->glyph[0]);
    new->glyph[i] = (complex_char_metrics_t *)ptr;
    memcpy(ptr, this->glyph[0], len);
    width += new->glyph[i]->char_bbox.urx - new->glyph[i]->char_bbox.llx;
    ptr += len;
   }
   continue;
  }
  
  if(new->encoded[i] != NULL)
  {
   new->encoded[i] = (simple_char_metrics_t *)ptr;
   memcpy(ptr, this->encoded[i], sizeof(simple_char_metrics_t));
   ptr += sizeof(simple_char_metrics_t);
  }

  if(new->glyph[i] != NULL)
  {
   len = XcCM_SIZEOF(this->glyph[i]);
   new->glyph[i] = (complex_char_metrics_t *)ptr;
   memcpy(ptr, this->glyph[i], len);
   width += new->glyph[i]->char_bbox.urx - new->glyph[i]->char_bbox.llx;
   ptr += len;
  }
 }

 Xc_ASSERT(ptr == (((char *)new) + new->pack_len));
 new->font_width = width;

 if((new->hook = NEW(c_Hook)(new, 8)) == NULL)
 {
  DELETE(c_CharMetrics)(new);
  return NULL;
 }
  
 DELETE(c_CharMetrics)(this);

 Xc_TRACE(("done"));
 return new;
}


/* ----------------------------------------------------------------- ** 
** copier - CharMetrics method copy method                           ** 
**          (with scaling)                                           ** 
** ----------------------------------------------------------------- */
static void *copier(this, matrix, newsize, width, angle, attributes, purity)
c_CharMetrics *this;
matrix_t *matrix;
coord_t newsize;
scale_t width;
angle_t angle;
int	attributes;
boolean purity;
{
 complex_char_metrics_t **complex;
 simple_char_metrics_t **simple;
 c_CharMetrics *p, *ptr;
 c_Matrix *mat;
 matrix_t matrix2;
 coord_t font_width, tmp;
 boolean force_unpack = FALSE, small_caps = FALSE;
 double coef;
 long i;

 Xc_HISTORY(("copier(%g, %g%% width, %g degres, %sPURE)",
	     SCALE_TO_POINTS(newsize),
	     SCALE_TO_PERCENT(width),
	     SCALE_TO_DEGREES(angle),
	     purity? "":"IM"));
#ifndef NTRACE
  Xc_TRACE(("src = $%lx", (long)this));
  if(matrix != NULL) XcM_DUMP_MATRIX("matrix", matrix);
#endif

 Xc_ASSERT(this->pure == TRUE);
  
  /*--- Create matrix ---*/
  if((mat = NEW(c_Matrix)()) == NULL) return NULL;
    
 force_unpack = (this->attributes != attributes) &&
  ((attributes & (XcCM_B_SMALL_CAPS | XcCM_B_BIG_CAPS)) != 0);
  
 if(this->pack_len == 0 || force_unpack)
 {
  p = Xc_malloc("Temp CharMetrics copy", sizeof(c_CharMetrics));
  if(p == NULL)
  {
   DELETE(c_Matrix)(mat);
   return NULL;
  }
  memcpy(p, this, sizeof(c_CharMetrics));
  p->pack_len = 0;
 }
 else
 {
  if((p = Xc_new(this->pack_handle)) == NULL)
  {
   DELETE(c_Matrix)(mat);
   return NULL;
  }
  memcpy(p, this, this->pack_len);
 }
  
 /*--- Hook ---*/
 if((p->hook = NEW(c_Hook)(p, 8)) == NULL)
 {
  DELETE(c_Matrix)(mat);
  if(p->pack_len == 0)
   Xc_free(p);
  else
   Xc_delete(p->pack_handle, p);
  return NULL;
 }
 
 /*--- Build matrix ---*/
 if(matrix == NULL)
 {
  F(mat).rotate(mat, FALSE, angle);
  
  matrix = F(mat).get(mat, 0);
  XcM_DUMP_MATRIX("computed matrix", matrix);
 }
 
 XcM_COPY_MATRIX(&p->matrix, matrix);
 p->matrix.v.dx = p->matrix.v.dy = COORD_ZERO; 
 
 /*--- Insert copy in linked list ---*/
 ptr = this;
 if(purity == TRUE)
  while((ptr->next != NULL) && (ptr->next->pure == FALSE)) ptr = ptr->next;
  
 p->previous = ptr;
 p->next = ptr->next;
 ptr->next = p;
 if(p->next != NULL) p->next->previous = p;
  

 p->scale = SCALE_FROM_COEF(1.0);
 p->pure = purity;
 p->counter = 1;
 p->raster_font = NULL;
 p->point_size = newsize;
 p->angle = angle;
 p->width = width;

 if(p->pack_len == 0)
 {
  register size_t len;
    
  /*--- Copy Track Kerning data ---*/
  if((this->track_kern != NULL) && (this->track_kern_count != 0))
  {
   len = this->track_kern_count * sizeof(track_kern_t);
   if((p->track_kern = Xc_malloc("Track Kerning", len)) == NULL)
   {
    DELETE(c_CharMetrics)(p);
    DELETE(c_Matrix)(mat);
    return NULL;
   }
   memcpy(p->track_kern, this->track_kern, len);
  }
    
  /*--- Copy Encoded Char data ---*/
  for(i = Xc_CHAR_TOTAL - 1; i >= 0; i--)
  {      
   if(this->encoded[i] != NULL)
   {
    len = sizeof(simple_char_metrics_t);
    if((p->encoded[i] = Xc_malloc("EncodedChar", len)) == NULL)
    {
     DELETE(c_CharMetrics)(p);
     DELETE(c_Matrix)(mat);
     return NULL;
    }
    memcpy(p->encoded[i], this->encoded[i], len);
   }
      
   if(this->glyph[i] != NULL)
   {
    len = XcCM_SIZEOF(this->glyph[i]);
    p->glyph[i] = Xc_malloc("Char", len);
    if(p->glyph[i] == NULL)
    {
     DELETE(c_CharMetrics)(p);
     DELETE(c_Matrix)(mat);
     return NULL;
    }
    memcpy(p->glyph[i], this->glyph[i], len);
   }
      
   if (force_unpack)
   {
    int conv;
	
    conv = ((unsigned char)Xc_TO_UPPER(i + Xc_FIRST_CHAR)) - Xc_FIRST_CHAR;
    if (conv != i)
    {
     if (this->encoded[i] && this->encoded[conv])
     {
      memcpy(&(p->encoded[i]->width), &(this->encoded[conv]->width),
	     sizeof(simple_char_metrics_t) - sizeof(char *));
     }
     if (this->glyph[i] && this->glyph[conv])
     {
      memcpy(&(p->glyph[i]->vv), &(this->glyph[conv]->vv),
	     offsetof(complex_char_metrics_t, composite_parts_count) - 
	     offsetof(complex_char_metrics_t, vv));
     }
    }
   }
  }
 }
 else
 {
  register size_t delta;
  register char *ptr;

  /*--- Relocation ---*/
  delta = ((char *)p) - ((char *)this);
  Xc_TRACE(("delta %lx -> %lx : %ld", this, p, delta));
    
  if(p->track_kern != NULL)
   p->track_kern = (track_kern_t *)(((char *)p->track_kern) + delta);
    
  simple = &(p->encoded[0]);
  complex = &(p->glyph[0]);
  for(i = Xc_CHAR_TOTAL; i--; simple++, complex++)
  {
   if((ptr = (char *)(*simple)) != NULL)
   {
    ptr += delta;
    *simple = (simple_char_metrics_t *)ptr;
   }
   if((ptr = (char *)(*complex)) != NULL)
   {
    ptr += delta;
    *complex = (complex_char_metrics_t *)ptr;
   }
  }
 }

 /*--- Compute final matrix ---*/

 coef = SCALE_TO_POINTS(p->point_size);
 
 F(mat).init(mat, FALSE, &p->matrix);
 F(mat).scale(mat, FALSE, SCALE_FROM_COEF(SCALE_TO_COEF(p->width) * coef),
	      SCALE_FROM_COEF(coef));
 
 matrix = F(mat).get(mat, 0);
 XcM_COPY_MATRIX(&p->final_matrix, matrix);
 p->final_matrix.v.dx = p->final_matrix.v.dy = COORD_ZERO; 
 XcM_DUMP_MATRIX("final", matrix);
 
 /*--- Compute update matrix ---*/
 F(mat).init(mat, FALSE, &this->final_matrix);
 XcM_DUMP_MATRIX("original", F(mat).get(mat, 0));
 F(mat).invert(mat, FALSE);
 XcM_DUMP_MATRIX("original(-1)", F(mat).get(mat, 0));
 F(mat).invertConcat(mat, FALSE, &p->final_matrix);
 
 matrix = F(mat).get(mat, 0);
 XcM_DUMP_MATRIX("update", matrix);

 if ((attributes & XcCM_B_SMALL_CAPS) && 
     !(this->attributes & XcCM_B_SMALL_CAPS))
 {
  coef = SCALE_TO_COEF(0.75); /* = sqr(0.866) */   
  XcM_COPY_MATRIX(&matrix2, matrix);
  matrix2.M[0][0] *= coef; matrix2.M[0][1] *= coef;
  matrix2.M[1][0] *= coef; matrix2.M[1][1] *= coef;
  XcM_DUMP_MATRIX("update", &matrix2);

  coef = SCALE_TO_COEF(0.75);
  XcM_COPY_MATRIX(&p->final_matrix2, &p->final_matrix);
  p->final_matrix2.M[0][0] *= coef; p->final_matrix2.M[0][1] *= coef;
  p->final_matrix2.M[1][0] *= coef; p->final_matrix2.M[1][1] *= coef;

  p->final_matrix2.v.dx = p->final_matrix2.v.dy = COORD_ZERO; 
 }
 else
 {
  if (attributes & XcCM_B_SMALL_CAPS)
  {
   coef = SCALE_TO_COEF(0.75); /* = sqr(0.866) */   
   XcM_COPY_MATRIX(&matrix2, matrix);
   matrix2.M[0][0] *= coef; matrix2.M[0][1] *= coef;
   matrix2.M[1][0] *= coef; matrix2.M[1][1] *= coef;
   XcM_DUMP_MATRIX("update", &matrix2);

   coef = SCALE_TO_COEF(0.75);
   XcM_COPY_MATRIX(&p->final_matrix2, &p->final_matrix);
   p->final_matrix2.M[0][0] *= coef; p->final_matrix2.M[0][1] *= coef;
   p->final_matrix2.M[1][0] *= coef; p->final_matrix2.M[1][1] *= coef;

   p->final_matrix2.v.dx = p->final_matrix2.v.dy = COORD_ZERO; 
  }
 }
#define XcCM_SCALE_XCOORD(var) \
 var = SGN(var) * (coord_t)(sqrt(pow((matrix)->M[0][0] * (var), 2.0) + \
				 pow((matrix)->M[1][0] * (var), 2.0)));
#define XcCM_SCALE_YCOORD(var) \
 var = SGN(var) * (coord_t)(sqrt(pow((matrix)->M[0][1] * (var), 2.0) + \
				 pow((matrix)->M[1][1] * (var), 2.0)));
#define XcCM_SCALE_BBOX(bbox) F(mat).convertBBox(mat, (bbox), (bbox))
#define XcCM_SCALE_VECTOR(vector) \
  ((tmp = vector.dx), \
   (vector.dx = (coord_t)XcM_DX(matrix, tmp, vector.dy)), \
   (vector.dy = (coord_t)XcM_DY(matrix, tmp, vector.dy)))

 Xc_TRACE(("rescale"));
 XcCM_SCALE_BBOX(&p->font_bbox);
 XcCM_SCALE_VECTOR(p->vvector);
 XcCM_SCALE_YCOORD(p->cap_height);
 XcCM_SCALE_YCOORD(p->x_height);
 XcCM_SCALE_YCOORD(p->ascender);
 XcCM_SCALE_YCOORD(p->descender);

 XcCM_SCALE_YCOORD(p->underline_position[0]);
 XcCM_SCALE_XCOORD(p->underline_position[1]);
 XcCM_SCALE_YCOORD(p->underline_thickness[0]);
 XcCM_SCALE_XCOORD(p->underline_thickness[1]);
 XcCM_SCALE_VECTOR(p->char_width[0]);
 XcCM_SCALE_VECTOR(p->char_width[1]);
 
 for(i = p->track_kern_count - 1; i >= 0; i--)
 {
  XcCM_SCALE_YCOORD(p->track_kern[i].min_point_size);
  XcCM_SCALE_XCOORD(p->track_kern[i].min_kern);
  XcCM_SCALE_YCOORD(p->track_kern[i].max_point_size);
  XcCM_SCALE_XCOORD(p->track_kern[i].max_kern);
 }
 
 simple = &p->encoded[Xc_CHAR_TOTAL-1];
 complex = &p->glyph[Xc_CHAR_TOTAL-1];
 for(i = Xc_CHAR_TOTAL, font_width = COORD_ZERO; i--; complex--, simple--)
 {
  if ((attributes & XcCM_B_SMALL_CAPS) &&
      !(this->attributes & XcCM_B_SMALL_CAPS))
  {
   int conv;
   
   conv = ((unsigned char)Xc_TO_UPPER(i + Xc_FIRST_CHAR)) - Xc_FIRST_CHAR;
   if ((i == conv && small_caps) || (i != conv && !small_caps))
   {
    XcM_SWAP_MATRIX(matrix, &matrix2);
    small_caps ^= TRUE;
   }
  }
  
  if(*simple != NULL)
  {
   XcCM_SCALE_BBOX(&(*simple)->char_bbox);
   XcCM_SCALE_VECTOR((*simple)->width[0]);
   XcCM_SCALE_VECTOR((*simple)->width[1]);
  }
  
  if(*complex != NULL)
  {
   int j;
   
   XcCM_SCALE_BBOX(&(*complex)->char_bbox);
   font_width += (*complex)->char_bbox.urx - (*complex)->char_bbox.llx;
   font_width += COORD_ONE;
   XcCM_SCALE_VECTOR((*complex)->vv);
   XcCM_SCALE_VECTOR((*complex)->width[0]);
   XcCM_SCALE_VECTOR((*complex)->width[1]);
   if((j = (*complex)->composite_parts_count) != 0)
   {
    composite_part_t *ptr;
    
    ptr = XcCM_COMPOSITE_PARTS(*complex);
    for( ; j--; ptr++) XcCM_SCALE_VECTOR(ptr->offset);
   }
   if((j = (*complex)->kern_pairs_count) != 0)
   {
    kern_pair_t *ptr;
    
    ptr = XcCM_KERN_PAIRS(*complex);
    for( ; j--; ptr++) XcCM_SCALE_VECTOR(ptr->offset);
   }
  }
 }
 p->font_width = font_width;
 p->attributes = attributes;
 
#undef XcCM_SCALE_XCOORD
#undef XcCM_SCALE_YCOORD
#undef XcCM_SCALE_BBOX
#undef XcCM_SCALE_VECTOR
 
 DELETE(c_Matrix)(mat);

 Xc_TRACE(("copied($%lx)", (long)p));
 /*  if (force_unpack)   should be desirable, but don't work... 
     return F(p).pack(p); */
 return p;
}


/* ----------------------------------------------------------------- ** 
** transform - Get a transformed *PURE* CharMetrics object           ** 
** ----------------------------------------------------------------- */
static c_CharMetrics *transform(this, scale, width, angle)
c_CharMetrics *this;
scale_t scale;
scale_t width;
angle_t angle;
{
 c_CharMetrics *ptr;
 coord_t newsize;

 Xc_HISTORY(("transform(%g%%, expand %g%%, %g degrees)",
	     SCALE_TO_PERCENT(scale), SCALE_TO_PERCENT(width),
	     SCALE_TO_DEGREES(angle)));
 Xc_ASSERT(this->pure == TRUE);

 for(ptr = this->next; (ptr != NULL) && (ptr->pure == FALSE); ptr = ptr->next)
  if((ptr->angle == angle) && (ptr->scale == scale) && (ptr->width == width))
  {
   Xc_TRACE(("(%lx).count = %d", ptr, ptr->counter));
   ptr->counter++;
   return ptr;
  }
  
 newsize = ROUNDFLOAT(SCALE_TO_COEF(scale) * this->point_size);
 ptr = COPY(c_CharMetrics)(this, NULL, newsize, width, angle,
			   this->attributes, FALSE);
 ptr->scale = scale;
 return ptr;
}


/* ----------------------------------------------------------------- ** 
** findTransfrom - Find a transformed *PURE* CharMetrics object      ** 
** ----------------------------------------------------------------- */
static c_CharMetrics *findTransform(this, scale, width, angle)
c_CharMetrics *this;
scale_t scale;
scale_t width;
angle_t angle;
{
 c_CharMetrics *ptr;

 Xc_HISTORY(("findTransform(%g%%, expand %g%%, %g degrees)",
	     SCALE_TO_PERCENT(scale), SCALE_TO_PERCENT(width),
	     SCALE_TO_DEGREES(angle)));
 Xc_ASSERT(this->pure == TRUE);

 for(ptr = this->next; (ptr != NULL) && (ptr->pure == FALSE); ptr = ptr->next)
  if((ptr->angle == angle) && (ptr->scale == scale) && (ptr->width == width))
  {
   XcM_DUMP_MATRIX("matrix", &ptr->final_matrix);
   return ptr;
  }
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** purge_impure - Purge all copies of an *PURE* CharMetrics object   ** 
** ----------------------------------------------------------------- */
static void purge_impure(this, scale, width, angle)
c_CharMetrics *this;
scale_t scale;
scale_t width;
angle_t angle;
{
 Xc_HISTORY(("purgeImpure(%g%%, expand %g%%, %g degrees)",
	     SCALE_TO_PERCENT(scale), SCALE_TO_PERCENT(width),
	     SCALE_TO_DEGREES(angle)));
 Xc_ASSERT(this->pure == TRUE);

 this = this->next;
 while((this != NULL) && (this->pure == FALSE))
 {
  if((this->scale == scale) &&
     (this->angle == angle) && (this->width == width))
  {
   Xc_TRACE(("(%lx).count = %d", this, this->counter));
   if(this->counter-- == 0) DELETE(c_CharMetrics)(this);
   return;
  }
  this = this->next;
 }
}


/* ----------------------------------------------------------------- ** 
** add_EncodedChar - CharMetrics method to add a new encoded char    ** 
** ----------------------------------------------------------------- */
static void add_EncodedChar(obj, code, encoding_name, width, char_bbox)
c_CharMetrics *obj;
char_t code;
char *encoding_name;
vector_t *width;
bbox_t *char_bbox;
{
 simple_char_metrics_t *p;

 /*Xc_TRACE(("add_EncodedChar `%s'", encoding_name));*/
 if(Xc_FIRST_CHAR > code) return;
 Xc_ASSERT(obj->pack_len == 0);

 p = Xc_malloc("SimpleCharMetrics", sizeof(simple_char_metrics_t));
 if(p == NULL) return;
 p->encoding_name = encoding_name;
 
 /*--- bounding box ---*/
 p->char_bbox.llx = char_bbox->llx;
 p->char_bbox.lly = char_bbox->lly;
 p->char_bbox.urx = char_bbox->urx;
 p->char_bbox.ury = char_bbox->ury;

 /*--- width vectors ---*/
 p->width[0].dx = width[0].dx;
 p->width[0].dy = width[0].dy;
 p->width[1].dx = width[1].dx;
 p->width[1].dy = width[1].dy;

 obj->encoded[code - Xc_FIRST_CHAR] = p;
}


/* ----------------------------------------------------------------- ** 
** add_Char - CharMetrics method to add a new complex char metrics   ** 
** ----------------------------------------------------------------- */
static void add_Char(obj, code, encoding_name, width, char_bbox, vv)
c_CharMetrics *obj;
char_t code;
char *encoding_name;
vector_t *width;
bbox_t *char_bbox;
vector_t *vv;
{
 complex_char_metrics_t *p;

 /*Xc_TRACE(("add_Char `%s'", encoding_name));*/
 if(Xc_FIRST_CHAR > code) return;
 Xc_ASSERT(obj->pack_len == 0);

 p = Xc_malloc("ComplexCharMetrics", sizeof(complex_char_metrics_t));
 if(p == NULL) return;
 p->encoding_name = encoding_name;
 /*--- bounding box ---*/
 p->char_bbox.llx = char_bbox->llx;
 p->char_bbox.lly = char_bbox->lly;
 p->char_bbox.urx = char_bbox->urx;
 p->char_bbox.ury = char_bbox->ury;

 /*--- width vectors ---*/
 p->width[0].dx = width[0].dx;
 p->width[0].dy = width[0].dy;
 p->width[1].dx = width[1].dx;
 p->width[1].dy = width[1].dy;

 /*--- origin vector ---*/
 p->vv.dx = vv->dx;
 p->vv.dy = vv->dy;

 p->composite_parts_count = p->kern_pairs_count = p->ligature_count = 0;
 obj->glyph[code - Xc_FIRST_CHAR] = p;
}


/* ----------------------------------------------------------------- ** 
** add_TrackKern - CharMetrics method to add a new track kerning     ** 
** ----------------------------------------------------------------- */
static void add_TrackKern(obj, data)
c_CharMetrics *obj;
track_kern_t *data;
{
 track_kern_t *p;

 /*Xc_TRACE(("add_TrackKern"));*/
 Xc_ASSERT(obj->pack_len == 0);

 p = Xc_realloc("TrackKerning",
		obj->track_kern,
		(obj->track_kern_count + 1) * sizeof(track_kern_t));
 if(p == NULL) return;
 memcpy(p + obj->track_kern_count, data, sizeof(track_kern_t));
 obj->track_kern_count++;
 obj->track_kern = p;
}


/* ----------------------------------------------------------------- ** 
** add_Ligature - Add a ligature to a complex char metrics item      ** 
** ----------------------------------------------------------------- */
static void add_Ligature(obj, code, data)
c_CharMetrics *obj;
char_t code;
ligature_t *data;
{
 size_t len;
 complex_char_metrics_t *p;

 Xc_TRACE(("add_Ligature"));
 if(Xc_FIRST_CHAR > code) return;
 Xc_ASSERT(obj->pack_len == 0);

 if((p = obj->glyph[code - Xc_FIRST_CHAR]) == NULL)
 {
  Xc_ERROR(("Glyph not allocated"));
  return;
 }
 len = XcCM_SIZEOF(p) + sizeof(ligature_t);
 if((p = Xc_realloc("ComplexCharMetrics", p, len)) == NULL)
  return;
 memcpy(XcCM_LIGATURES(p) + p->ligature_count, data, sizeof(ligature_t));
 p->ligature_count++;
 obj->glyph[code - Xc_FIRST_CHAR] = p;

 Xc_ASSERT(XcCM_SIZEOF(p) == len);
}


/* ----------------------------------------------------------------- ** 
** add_KernPair - Add a Kerning pair to a complex metrics item       ** 
** ----------------------------------------------------------------- */
static void add_KernPair(obj, code, data)
c_CharMetrics *obj;
char_t code;
kern_pair_t *data;
{
 size_t len;
 complex_char_metrics_t *p;

 /*Xc_TRACE(("add_KernPair"));*/
 if(Xc_FIRST_CHAR > code) return;
 Xc_ASSERT(obj->pack_len == 0);

 if((p = obj->glyph[code - Xc_FIRST_CHAR]) == NULL)
 {
  Xc_ERROR(("Glyph not allocated"));
  return;
 }
 len = XcCM_SIZEOF(p) + sizeof(kern_pair_t);
 if((p = Xc_realloc("ComplexCharMetrics", p, len)) == NULL)
  return;
 memmove(((kern_pair_t *)XcCM_LIGATURES(p)) + 1,
	 XcCM_LIGATURES(p),
	 p->ligature_count * sizeof(ligature_t));
 memcpy(XcCM_KERN_PAIRS(p) + p->kern_pairs_count, data, sizeof(kern_pair_t));
 p->kern_pairs_count++;
 obj->glyph[code - Xc_FIRST_CHAR] = p;

 Xc_ASSERT(XcCM_SIZEOF(p) == len);
}


/* ----------------------------------------------------------------- ** 
** add_CompositePart - Add a composite part to a complex metrics item** 
** ----------------------------------------------------------------- */
static void add_CompositePart(obj, code, data)
c_CharMetrics *obj;
char_t code;
composite_part_t *data;
{
 size_t len;
 complex_char_metrics_t *p;

 /*Xc_TRACE(("add_CompositePart(%d)", code));*/
 if(Xc_FIRST_CHAR > code) return;
 Xc_ASSERT(obj->pack_len == 0);

 if((p = obj->glyph[code - Xc_FIRST_CHAR]) == NULL)
 {
  Xc_ERROR(("Glyph not allocated"));
  return;
 }
 len = XcCM_SIZEOF(p) + sizeof(composite_part_t);
 if((p = Xc_realloc("ComplexCharMetrics", p, len)) == NULL)
  return;
 memmove(((composite_part_t *)XcCM_KERN_PAIRS(p)) + 1,
	 XcCM_KERN_PAIRS(p),
	 p->ligature_count * sizeof(ligature_t) +
	 p->kern_pairs_count * sizeof(kern_pair_t));
 memcpy(XcCM_COMPOSITE_PARTS(p) + p->composite_parts_count,
	data, sizeof(composite_part_t));
 p->composite_parts_count++;
 obj->glyph[code - Xc_FIRST_CHAR] = p;

 Xc_ASSERT(XcCM_SIZEOF(p) == len);
}


/* ----------------------------------------------------------------- ** 
** textWidth - Compute text width                                    ** 
** ----------------------------------------------------------------- */
static coord_t textWidth(this, text, len)
c_CharMetrics *this;
char_t *text;
long len;
{
 register coord_t width;

 Xc_HISTORY(("textWidth"));
  
 width = COORD_ZERO;
 while(len--)
 {
  register char_t c;
      
  if((c = *text++) >= Xc_FIRST_CHAR)
  {
   register complex_char_metrics_t *m;
	  
   if((m = this->glyph[c - Xc_FIRST_CHAR]) != NULL)
    width += m->width[0].dx;
  }
 }
 return width;
}

/* ----------------------------------------------------------------- ** 
** textWidthSpec - Compute text width special: add word and char inc ** 
** ----------------------------------------------------------------- */
static coord_t textWidthSpec(this, text, len, char_inc, word_inc)
c_CharMetrics *this;
char_t *text;
long len;
coord_t char_inc; 
coord_t word_inc;
{
 register coord_t width;
 
 Xc_HISTORY(("textWidth"));
  
 width = COORD_ZERO;
 while(len--)
 {
  register char_t c;
  
  if((c = *text++) >= Xc_FIRST_CHAR)
  {
   register complex_char_metrics_t *m;
   
   if((m = this->glyph[c - Xc_FIRST_CHAR]) != NULL)
    width += m->width[0].dx + char_inc;

   if (c == ' ') width += word_inc;
  }
 }
 return width;
}

/* ----------------------------------------------------------------- ** 
** textHeight - Compute text height                                  ** 
** ----------------------------------------------------------------- */
static coord_t textHeight(this, text, len)
c_CharMetrics *this;
char_t *text;
long len;
{
 register coord_t height;

 Xc_HISTORY(("textHeight"));

 height = COORD_ZERO;
 while(len--)
 {
  register char_t c;

  if((c = *text++) >= Xc_FIRST_CHAR)
  {
   register complex_char_metrics_t *m;

   if((m = this->glyph[c - Xc_FIRST_CHAR]) != NULL)
    height += m->width[0].dy;
  }
 }
 return height;
}

/* ----------------------------------------------------------------- ** 
** compare - Compare two CharMetrics                                 ** 
** ----------------------------------------------------------------- */
static boolean compare(metrics1, metrics2)
c_CharMetrics *metrics1;
c_CharMetrics *metrics2;
{
 Xc_HISTORY(("compare($%lx, $%lx)", (long)metrics1, (long)metrics2));

 if (memcmp(&metrics1->point_size, &metrics2->point_size,
	    offsetof(c_CharMetrics, track_kern) - 
	    offsetof(c_CharMetrics, point_size)) ||
     (metrics1->pure != metrics2->pure) || 
     (metrics1->font_width != metrics2->font_width))
  return FALSE;
 return TRUE;
}



/* ----------------------------------------------------------------- ** 
** calibrate - Calibrate a text string                               ** 
** ----------------------------------------------------------------- */
static long calibrate(this, width, textp, end)
c_CharMetrics *this;
coord_t width;
char **textp;
char *end;
{
 coord_t word_width;
 char *ptr, *text;
 long pos;

 Xc_HISTORY(("calibrate"));

 text = *textp;

 /*--- Skip spaces at the begining ---*/
 while((text < end) && (*text == 32)) text++;
 if(text == end) return 0;
 *textp = ptr = text;

 word_width = COORD_ZERO;
 pos = 0;

 /*--- Calibration loop ---*/
 while((word_width <= width) && (ptr < end))
 {
  /*--- Include previous word ---*/
  width -= word_width;
  pos += ptr - text;
  text = ptr;

  /*--- Skip spaces at the begining ---*/
  while((ptr < end) && (*ptr == 32)) ptr++;

  /*--- Skip word ---*/
  while((ptr < end) && (*ptr != 32)) ptr++;

  Xc_TRACE(("word len = %ld", ptr - text));
  word_width = F(this).textWidth(this, text, ptr - text);
 }

 if((word_width <= width) || (text == *textp)) pos += ptr - text;

#ifndef NTRACE
 {
  long i;

  fprintf(stderr, "linetext: `");
  for(i = pos, ptr = *textp; i--; ptr++) putc(*ptr, stderr);
  fprintf(stderr, "'\n");
 }
#endif

 return pos;
}

/* ----------------------------------------------------------------- ** 
** makefont- Create new instance transformed with matrix            ** 
** ----------------------------------------------------------------- */
static c_CharMetrics *makefont(this, matrix, size, width)
c_CharMetrics *this;
matrix_t *matrix;
coord_t size;
scale_t width;
{
 c_CharMetrics *ptr;

 Xc_HISTORY(("makefont"));
 XcM_DUMP_MATRIX("matrix", matrix);
 Xc_ASSERT(this->pure == TRUE);

 for(ptr = this->next; (ptr != NULL) && (ptr->pure == FALSE); ptr = ptr->next)
  if((this->point_size == size) && (this->width == width) &&
     XcM_CMP_MATRIX2(&this->matrix, matrix))
  {
   Xc_TRACE(("(%lx).count = %d", ptr, ptr->counter));
   ptr->counter++;
   return ptr;
  }
  
 ptr = COPY(c_CharMetrics)(this, matrix, size, width, ANGLE_ZERO,
			   this->attributes, FALSE);
 ptr->scale = SCALE_ONE;

 return ptr;
}


/* ----------------------------------------------------------------- ** 
** unmakefont - Purge all copies of an *PURE* CharMetrics object     ** 
** ----------------------------------------------------------------- */
static void unmakefont(this, matrix, size, width)
c_CharMetrics *this;
matrix_t *matrix;
coord_t size;
scale_t width;
{
 Xc_HISTORY(("unmakefont($%lx)", (long)this));
 XcM_DUMP_MATRIX("matrix", matrix);
 Xc_ASSERT(this->pure == TRUE);

 this = this->next;

 while((this != NULL) && (this->pure == FALSE))
 {
  if((this->point_size == size) && (this->width == width) &&
     XcM_CMP_MATRIX2(&this->matrix, matrix))
  {
   Xc_TRACE(("(%lx).count = %d", this, this->counter));
   if(--this->counter == 0)
   {
    if(!XcH_USAGE(this))
     DELETE(c_CharMetrics)(this);
    else
     this->counter = 0;
   }

   return;
  }
  this = this->next;
 }
}


/* ----------------------------------------------------------------- ** 
** lock - Lock client for metrics                            ** 
** ----------------------------------------------------------------- */
static boolean lockMetrics(this, data)
c_CharMetrics *this;
void *data;
{
 Xc_HISTORY(("lock($%lx, $%lx)", (long)this, (long)data));

 return XcH_REGISTER_ONCE(this, NULL, data);
}


/* ----------------------------------------------------------------- ** 
** unlock - Unlock client for metrics                                ** 
** ----------------------------------------------------------------- */
static void unlockMetrics(this, data)
c_CharMetrics *this;
void *data;
{
 Xc_HISTORY(("unlock($%lx, $%lx)", (long)this, (long)data));

 XcH_UNREGISTER(this, NULL, data);
 if(!XcH_USAGE(this)) DELETE(c_CharMetrics)(this);
}


/* ----------------------------------------------------------------- ** 
** isLocked - Is a client registred for metrics?                     ** 
** ----------------------------------------------------------------- */
static boolean isLockedMetrics(this, data)
c_CharMetrics *this;
void *data;
{
 Xc_HISTORY(("islocked($%lx, $%lx)?", (long)this, (long)data));
 if (!this->hook) return FALSE;
 Xc_TRACE(("  count=%d", XcH_USAGE(this)));
 return(F(this->hook).getUsage(this->hook, data)? TRUE : FALSE);
}

/* ----------------------------------------------------------------- ** 
** fast function to work with FontManager                            ** 
** ----------------------------------------------------------------- */
static void fast_copy(this, dest)
c_CharMetrics *this;
c_CharMetrics *dest;
{
 int i, size;
 int enc_size, enc_str_size, gly_size, gly_str_size;
 simple_char_metrics_t *enc_ptr = NULL;
 char *gly_ptr = NULL, *enc_str_ptr = NULL, *gly_str_ptr = NULL;
  
 memcpy(dest, this, sizeof(c_CharMetrics));
  
 if (this->track_kern_count)
 {
  dest->track_kern = (track_kern_t *)Xc_malloc
   ("tk", this->track_kern_count * sizeof(track_kern_t));
  memcpy(dest->track_kern, this->track_kern, 
	 this->track_kern_count * sizeof(track_kern_t));
 }
 else
  dest->track_kern = NULL;
  
 enc_size = enc_str_size = gly_size = gly_str_size = 0;
 for(i = 0; i < Xc_CHAR_TOTAL; i++)
 {
  if (this->encoded[i])
  {
   enc_size += sizeof(simple_char_metrics_t);
   enc_str_size += strlen(this->encoded[i]->encoding_name) + 1;
  }
  if (this->glyph[i])
  {
   gly_size += XcCM_SIZEOF(this->glyph[i]);
   gly_str_size += strlen(this->glyph[i]->encoding_name) + 1;
  }
 }
  
 if (enc_size)
  enc_ptr = (simple_char_metrics_t *)Xc_malloc("ec", enc_size);
 if (gly_size)
  gly_ptr = (char *)Xc_malloc("gl", gly_size);
 if (enc_str_size)
  enc_str_ptr = (char *)Xc_malloc("ecs", enc_str_size);
 if (gly_str_size)
  gly_str_ptr = (char *)Xc_malloc("gls", gly_str_size);
    
 for(i = 0; i < Xc_CHAR_TOTAL; i++)
 {
  if (this->encoded[i])
  {
   dest->encoded[i] = enc_ptr++;
   memcpy(dest->encoded[i], this->encoded[i], 
	  sizeof(simple_char_metrics_t));
   dest->encoded[i]->encoding_name = enc_str_ptr;
   strcpy(enc_str_ptr, this->encoded[i]->encoding_name);
   enc_str_ptr += strlen(this->encoded[i]->encoding_name) + 1;
  }
  else
   dest->encoded[i] = NULL;
    
  if (this->glyph[i])
  {
   size = XcCM_SIZEOF(this->glyph[i]);
   dest->glyph[i] = (complex_char_metrics_t *)gly_ptr;
   gly_ptr += size;
   memcpy(dest->glyph[i], this->glyph[i], size);
      
   dest->glyph[i]->encoding_name = gly_str_ptr;
   strcpy(gly_str_ptr, this->glyph[i]->encoding_name);
   gly_str_ptr += strlen(this->glyph[i]->encoding_name) + 1;
  }
  else
   dest->glyph[i] = NULL;
 }
 dest->raster_font = NULL;
 dest->pack_handle = NULL;
 dest->pack_len = 0;
}

static void fast_free(this)
c_CharMetrics *this;
{
 int i;
  
 if (this->track_kern)
  Xc_free(this->track_kern);
 for(i = 0; i < Xc_CHAR_TOTAL; i++)
 {
  if (this->encoded[i])
  {
   Xc_free(this->encoded[i]->encoding_name);
   Xc_free(this->encoded[i]);
   break;
  }
 }
 for(i = 0; i < Xc_CHAR_TOTAL; i++)
 {
  if (this->glyph[i])
  {
   Xc_free(this->glyph[i]->encoding_name);
   Xc_free(this->glyph[i]);
   break;
  }
 }
}
