/*
** VectorGraph.c for XQuad in VectorGraph/
** Method for VectorGraph Class
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
** Started on  Fri Jan 13 12:12:00 1995 Antoine Buat
** Last update Sat Jun 14 18:27:52 1997 Antoine Buat
*/

#define NTRACE
#define NHISTORY

#include "VectorGraphP.h"

static void 	*constructor();
static void 	destructor();
static void 	*copy();
static error	changeBase ___NPROTO((c_VectorGraph *this, BaseStd_t *base));
static error	graphInit ___NPROTO((c_VectorGraph *this, c_Cadre *cadre));
static void	graphConfigure ___NPROTO((c_VectorGraph *this, c_Cadre *frame,
				      Polygone *poly));
static void	graphRedraw();
static boolean	graphPrint();
static boolean	addBasicGraph ___PROTO((c_VectorGraph *this,
					void *basic_graph));
static boolean	subBasicGraph ___PROTO((c_VectorGraph *this,
					void *basic_graph));

static error computeParameters ___PROTO((c_VectorGraph *this,
					 c_Cadre *frame));

sf_VectorGraph fc_VectorGraph =
{
 constructor, destructor, copy,
 changeBase, graphInit, graphRedraw, graphConfigure,
 graphPrint,
 readVectorGraph, writeVectorGraph,
 addBasicGraph, subBasicGraph,
 vectorSetMode, vectorVerticalFlip, vectorHorizontalFlip, vectorCenter
};


/* ----------------------------------------------------------------- **
** construction method                                               **
** ----------------------------------------------------------------- */
static void *constructor()
{
 c_VectorGraph 	*this;

 Xc_HISTORY(("constructor"));

 if((this = Xc_malloc("VectorGraph", sizeof(c_VectorGraph))) == NULL)
  return NULL;
 this->f = &fc_VectorGraph;

 this->spool = Xc_createSpool("polygons", 32 * sizeof(vector_item_t), 0);
 if(this->spool == NULL)
 {
  Xc_free(this);
  return NULL;
 }

 this->name[0] = 0;
 this->filename = NULL;
 this->list = NULL;
 this->highg = NULL;
 this->frame = NULL;
 this->last_item = NULL;

 this->angle = ANGLE_ZERO;

 this->bbox.llx = this->bbox.lly = 0;
 this->bbox.urx = BG_AREA_WIDTH;
 this->bbox.ury = BG_AREA_HEIGHT;
 this->offset.dx = this->offset.dy = COORD_ZERO;
 this->width_factor = this->height_factor = SCALE_ONE;
 this->vflip_flag = FALSE;
 this->hflip_flag = FALSE;
 this->scale = SCALE_ONE;
 this->angle = ANGLE_ZERO;
 this->update_flag = TRUE;
 this->mode = XcVG_NORMAL;

 if((this->matrix = NEW(c_Matrix)()) == NULL)
 {
  DELETE(c_VectorGraph)(this);
  return NULL;
 }

 Xc_TRACE(("done"));
 return this;
}


/* ----------------------------------------------------------------- **
** destruction method                                                **
** ----------------------------------------------------------------- */
static void destructor(this)
c_VectorGraph *this;
{
 vector_item_t	*ptr;

 Xc_HISTORY(("destructor"));

 /*--- Deletes attached basic graph ---*/
 if (this->highg)
  DELETE(c_HighGraph)(this->highg);

 for(ptr = this->list; ptr != NULL; ptr = ptr->next)
  BG_DELETE(ptr->basic_graph)(ptr->basic_graph);
 Xc_destroySpool(this->spool);

 if(this->filename != NULL) Xc_free(this->filename);
 if(this->matrix != NULL) DELETE(c_Matrix)(this->matrix);

 Xc_free(this);
 Xc_TRACE(("destroyed"));
}


/* ----------------------------------------------------------------- **
** Copy method                                                       **
** ----------------------------------------------------------------- */
static void *copy(orig, base)
c_VectorGraph *orig;
BaseStd_t *base;
{
 vector_item_t *ptr, **link;
 c_VectorGraph *this;

 Xc_HISTORY(("copy"));

 if((this = Xc_malloc("VectorGraph", sizeof(c_VectorGraph))) == NULL)
  return NULL;
 memcpy(this, orig, sizeof(c_VectorGraph));

 /*--- Copy filename ---*/
 if(orig->filename != NULL)
  if((this->filename = Xc_strdup("filename", orig->filename)) == NULL)
  {
   Xc_free(this);
   return NULL;
  }

 /*--- Copy matrix ---*/
 if((this->matrix = NEW(c_Matrix)()) == NULL)
 {
  Xc_free(this->filename);
  Xc_free(this);
  return NULL;
 }

 /*--- Copy spool ---*/
 this->spool = Xc_createSpool("polygons", 32 * sizeof(vector_item_t), 0);
 if(this->spool == NULL)
 {
  Xc_free(this->filename);
  DELETE(c_Matrix)(this->matrix);
  Xc_free(this);
  return NULL;
 }

 this->list = NULL;
 if (orig->highg)
 {
  this->highg = (c_HighGraph *)COPY(c_HighGraph)(orig->highg, this);
  if (this->highg == NULL)
   ptr = (void *)1;
  else
   ptr = NULL;
 }
 else
 {
  /*--- Copy attached basic graph ---*/
  for(ptr = orig->list, link = &this->list; ptr != NULL; ptr = ptr->next)
  {
   void *basic_graph;
	
   if((basic_graph = BG_COPY(ptr->basic_graph)(ptr->basic_graph, base))
      == NULL) break;
   if((*link = Xc_smalloc(this->spool, sizeof(vector_item_t))) == NULL)
   {
    BG_DELETE(basic_graph)(basic_graph);
    break;
   }
	
   (*link)->basic_graph = basic_graph;
   link = &((*link)->next);
  }
  *link = NULL;
 }


 if(ptr != NULL)
 {
  DELETE(c_VectorGraph)(this);
  return NULL;
 }

 this->update_flag = TRUE;
 Xc_TRACE(("copied"));
 return this;
}


/* ----------------------------------------------------------------- **
** changeBase - BaseStd change                                       **
** ----------------------------------------------------------------- */
static error changeBase(this, base)
c_VectorGraph *this;
BaseStd_t *base;
{
 vector_item_t *ptr;
 error rc = TRUE;

 Xc_TRACE(("changeBase"));

 for(ptr = this->list; ptr != NULL; ptr = ptr->next)
  if((rc = BG_CHANGE_BASE(ptr->basic_graph)(ptr->basic_graph, base))
     != XC_NO_ERROR)
   break;

 return rc;
}


/* ----------------------------------------------------------------- **
** addBasicGraph - Add a basic graph to object                       **
** ----------------------------------------------------------------- */
static boolean addBasicGraph(this, basic_graph)
c_VectorGraph	*this;
void		*basic_graph;
{
 vector_item_t	*ptr;

 Xc_HISTORY(("addBasic_Graph"));

 /*--- Update bounding box ---*/

 if (this->last_item)
  ptr = this->last_item;
 else
 {
  ptr = this->list;
  if (ptr)
  {
   while(ptr->next)
    ptr = ptr->next;
  }
 }

 if (this->list)
 {
  if((ptr->next = Xc_smalloc(this->spool, sizeof(vector_item_t))) == NULL)
   return FALSE;

  ptr = this->last_item = ptr->next;
 }
 else
 {
  if((this->list = ptr = this->last_item =
      Xc_smalloc(this->spool, sizeof(vector_item_t))) == NULL)
   return FALSE;
 }

 ptr->basic_graph = basic_graph;
 ptr->next = NULL;

 this->update_flag = TRUE;

 Xc_TRACE(("bbox(%g/%g, %g/%g)",
	   SCALE_TO_POINTS(this->bbox.llx), SCALE_TO_POINTS(this->bbox.urx),
	   SCALE_TO_POINTS(this->bbox.lly), SCALE_TO_POINTS(this->bbox.ury)));
 return TRUE;
}

/* ----------------------------------------------------------------- **
** subBasicGraph - Remove a basic graph from object                  **
** ----------------------------------------------------------------- */
static boolean subBasicGraph(this, basic_graph)
c_VectorGraph	*this;
void		*basic_graph;
{
 vector_item_t	*ptr, *ptr2;

 Xc_HISTORY(("subBasic_Graph"));

 /*--- Update bounding box ---*/

 this->last_item = NULL;
 ptr = ptr2 = this->list;
 while(ptr && ptr->basic_graph != basic_graph)
 {
  ptr2 = ptr;
  ptr = ptr->next;
 }

 if (!ptr)
  return FALSE;

 if (ptr2 == ptr)
  this->list = ptr->next;
 else
  ptr2->next = ptr->next;

 this->update_flag = TRUE;

 Xc_TRACE(("bbox(%g/%g, %g/%g)",
	   SCALE_TO_POINTS(this->bbox.llx), SCALE_TO_POINTS(this->bbox.urx),
	   SCALE_TO_POINTS(this->bbox.lly), SCALE_TO_POINTS(this->bbox.ury)));
 return TRUE;
}


/* ----------------------------------------------------------------- **
** graphInit - display_init standard frame method                    **
** ----------------------------------------------------------------- */
static error graphInit(this, cadre)
c_VectorGraph	*this;
c_Cadre		*cadre;
{
 Xc_HISTORY(("display_init"));

 this->frame = cadre;
 return computeParameters(this, cadre);
}


/* ----------------------------------------------------------------- **
** graphRedraw - display_redraw - standard frame method              **
** ----------------------------------------------------------------- */
static void graphRedraw(this, frame, poly)
c_VectorGraph	*this;
c_Cadre		*frame;
Polygone	*poly;
{
 vector_item_t	*ptr;
 int		dx, dy;

 Xc_HISTORY(("display_redraw"));
 Xc_DUMP_BBOX("vector bbox", &(this->bbox));

 /*--- Build redraw area bounding box ---*/
 dx = (frame->sOrig_X + frame->saOrig_X) / frame->sScale;
 dy = (frame->sOrig_Y + frame->saOrig_Y) / frame->sScale;

 /*--- call DISPLAY_REDRAW for each basic_graph ---*/
 for(ptr = this->list; ptr != NULL; ptr = ptr->next)
 {
  void		*basic_graph;

  basic_graph = ptr->basic_graph;
  BG_CHANGE_XINFO(basic_graph)(basic_graph, &frame->X_info);
  BG_DISPLAY_REDRAW(basic_graph)(basic_graph, this->matrix,
				 frame->sScale, dx, dy);
 }
}


/* ----------------------------------------------------------------- **
** graphConfigure - display_configure standard frame method          **
** ----------------------------------------------------------------- */
static void graphConfigure(this, frame, poly)
c_VectorGraph	*this;
c_Cadre		*frame;
Polygone	*poly;
{
 Xc_HISTORY(("display_configure"));

 if((frame->sAngle != this->angle) || (this->mode != XcVG_NORMAL))
  computeParameters(this, frame);
}


/* ----------------------------------------------------------------- **
** graphPrint - ps_print frame standard method                       **
** ----------------------------------------------------------------- */
static boolean graphPrint(this, post)
c_VectorGraph	*this;
c_PostScript	*post;
{
 vector_item_t	*ptr;

 Xc_HISTORY(("ps_print"));

 if(post->ai_mode == FALSE)
 {
  F(post->matrix).translate(post->matrix, TRUE,
			    this->offset.dx, this->offset.dy);
  F(post->matrix).scale(post->matrix, FALSE,
			this->width_factor, this->height_factor);

  /*--- Vertical flipping ---*/
  if(this->vflip_flag != FALSE)
  {
   F(post->matrix).translate(post->matrix, FALSE, COORD_ZERO,
			     this->bbox.ury - this->bbox.lly);
   F(post->matrix).scale(post->matrix, FALSE, SCALE_ONE, -SCALE_ONE);
  }

  /*--- Horizontal flipping ---*/
  if(this->hflip_flag != FALSE)
  {
   F(post->matrix).translate(post->matrix, FALSE,
			     this->bbox.urx - this->bbox.llx,
			     COORD_ZERO);
   F(post->matrix).scale(post->matrix, FALSE, -SCALE_ONE, SCALE_ONE);
  }

  F(post->matrix).translate(post->matrix, FALSE,
			    -this->bbox.llx, -this->bbox.lly);

  F(post).putLine(post, "$m cm", F(post->matrix).get(post->matrix, 0));
  F(post->matrix).identity(post->matrix, FALSE);
 }
 else if(!F(post).putLine(post, "q 1 g 0 w $p m $p L $p L $p L $p L s Q",
			  this->bbox.llx, this->bbox.lly,
			  this->bbox.urx, this->bbox.lly,
			  this->bbox.urx, this->bbox.ury,
			  this->bbox.llx, this->bbox.ury,
			  this->bbox.llx, this->bbox.lly)) return FALSE;


 /*--- call PS_PRINT for each basic_graph ---*/
 for(ptr = this->list; ptr != NULL; ptr = ptr->next)
 {
  void *basic_graph;

  basic_graph = ptr->basic_graph;
  if(!BG_PS_PRINT(basic_graph)(basic_graph, post)) return FALSE;
 }
 Xc_TRACE(("done"));

 if(post->ai_mode == FALSE) F(post->matrix).pop(post->matrix);
 return TRUE;
}


/* ----------------------------------------------------------------- **
** computeParameters - Compute parameters from mode                  **
** ----------------------------------------------------------------- */
static error computeParameters(this, frame)
c_VectorGraph *this;
c_Cadre *frame;
{
 Xc_TRACE(("computeParameters"));

 if(this->mode != XcVG_NORMAL)
 {
  double vfactor, hfactor;
  coord_t w, h;

  w = this->bbox.urx - this->bbox.llx;
  h = this->bbox.ury - this->bbox.lly;
  if (h)
   vfactor = (double)frame->saHeight / h;
  else
   vfactor = 1.0;
  if (w)
   hfactor = (double)frame->saWidth / w;
  else
   hfactor = 1.0;

  switch(this->mode)
  {
   /*--- Best aspect from minimum boundaries ---*/
  case XcVG_BEST_ASPECT:
   vfactor = hfactor = MIN(vfactor, hfactor);
   break;
	
   /*--- Best aspect from maximum boundaries ---*/
  case XcVG_MAX_ASPECT:
   vfactor = hfactor = MAX(vfactor, hfactor);
   break;
	
   /*--- Autoresize ---*/
  case XcVG_AUTOSIZE:
   break;
	
   /*--- Default ---*/
  default:
   Xc_BREAK(("Shouldn't be there!"));
  }

  w = ROUNDFLOAT((frame->saWidth - (w * hfactor)) / 2);
  h = ROUNDFLOAT((frame->saHeight - (h * vfactor)) / 2);

  if((this->width_factor != SCALE_FROM_COEF(hfactor)) ||
     (this->height_factor != SCALE_FROM_COEF(vfactor)) ||
     (this->offset.dx != w) || (this->offset.dy != h))
  {
   this->width_factor = SCALE_FROM_COEF(hfactor);
   this->height_factor = SCALE_FROM_COEF(vfactor);
   this->offset.dx = w;
   this->offset.dy = h;
   this->update_flag = TRUE;
  }
 }

 Xc_TRACE(("offset: %g, %g   scale: %g%%, %g%%",
	   SCALE_TO_POINTS(this->offset.dx),
	   SCALE_TO_POINTS(this->offset.dy),
	   SCALE_TO_PERCENT(this->width_factor),
	   SCALE_TO_PERCENT(this->height_factor)));

 /*--- Screen factor ---*/
 if(this->scale != SCALE_FROM_COEF(1.0 / frame->sScale))
 {
  this->scale = SCALE_FROM_COEF(1.0 / frame->sScale);
  this->update_flag = TRUE;
 }

 /*--- Angle ---*/
 if(this->angle != frame->sAngle)
 {
  this->angle = frame->sAngle;
  this->update_flag = TRUE;
 }

 /*--- Matrix update ---*/
 if(this->update_flag != FALSE)
 {
  c_Matrix *matrix;

  Xc_TRACE(("updating"));

  /*--- Setup matrix ---*/
  matrix = this->matrix;
  Xc_ASSERT(matrix != NULL);

  F(matrix).identity(matrix, FALSE);
  F(matrix).scale(matrix, FALSE, this->scale, this->scale);
  F(matrix).rotate(matrix, FALSE, -this->angle);
  F(matrix).translate(matrix, FALSE, this->offset.dx, this->offset.dy);
  F(matrix).scale(matrix, FALSE, this->width_factor, this->height_factor);

  /*--- Vertical flipping ---*/
  if(this->vflip_flag != FALSE)
  {
   F(matrix).translate(matrix, FALSE,
		       COORD_ZERO, this->bbox.ury - this->bbox.lly);
   F(matrix).scale(matrix, FALSE, SCALE_ONE, -SCALE_ONE);
  }

  /*--- Horizontal flipping ---*/
  if(this->hflip_flag != FALSE)
  {
   F(matrix).translate(matrix, FALSE,
		       this->bbox.urx - this->bbox.llx, COORD_ZERO);
   F(matrix).scale(matrix, FALSE, -SCALE_ONE, SCALE_ONE);
  }

  /* F(matrix).translate(matrix, FALSE, -this->bbox.llx, -this->bbox.lly); */

  this->update_flag = FALSE;
 }
 return XC_NO_ERROR;
}


