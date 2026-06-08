/*
** VectorGraph.c for Xclamation and XAllWrite in VectorGraph/
** Method for VectorGraph Class
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
** Started on  Fri Jan 13 12:12:00 1995 Antoine Buat
** Last update Sat Jun 14 18:27:41 1997 Antoine Buat
*/

#define NTRACE
#define NHISTORY

#include "VectorGraphP.h"

#define SPOOL_SIZE ((unsigned long)1024 * sizeof(vector_item_t))

/*--- Prototypes ---*/
static void	*constructor();
static void	destructor();
static void	*copy();
static error	changeBase ___NPROTO((c_VectorGraph *this, BaseStd_t *base));
static error	graphInit ___NPROTO((c_VectorGraph *this, c_Cadre *cadre));
static void	graphConfigure ___NPROTO((c_VectorGraph *this, c_Cadre *frame,
				      Polygone *poly));


/*--- Local prototypes ---*/
static error computeParameters ___PROTO((c_VectorGraph *this,
					 c_Cadre *frame));
static boolean itemCopyStep ___PROTO((c_VectorGraph *this, BaseStd_t *base,
				      vector_item_t *shead, 
				      vector_item_t *stail,
				      vector_item_t **thead,
				      vector_item_t **ttail));

sf_VectorGraph fc_VectorGraph =
{
 constructor, destructor, copy,
 changeBase, graphInit, vectorRedraw, graphConfigure,
 vectorPrint,
 vectorRead, vectorWrite,

 vectorSetMode, vectorVerticalFlip, vectorHorizontalFlip, vectorCenter,
 vectorTextToggle,

 vectorSave, vectorRestore,
  
 vectorTranslate, vectorRotate, vectorScale, vectorConcat, 
  
 vectorSetStrokeColor, vectorSetFillColor,
 vectorSetLineWidth, vectorSetLineJoin, vectorSetLineCap,
 vectorSetMiterLimit, vectorSetFlatness, vectorSetLineDash,
  
 vectorRender, vectorClip,
 vectorNewPath, vectorAbortPath, vectorClosePath,
 vectorMoveTo, vectorLineTo, vectorArcTo, 
 vectorCurveTo, vectorCurveBegin, vectorCurveEnd,
 vectorGetCurrentPoint, 

 vectorSetTextMatrix,
 vectorSetTextPoint,
 vectorTextNewline,
 vectorSetTextRise,
 vectorSetTextMode,
 vectorSetTextFont,
 vectorSetTextLeading,
 vectorSetTextWordSpacing,
 vectorSetTextCharSpacing,
 vectorSetTextHScale,
 vectorSetTextKerning,
 vectorTextRender,

 vectorEnd,
 vectorLoad
};


/* ----------------------------------------------------------------- ** 
** construction method                                               ** 
** ----------------------------------------------------------------- */
static void *constructor(base)
BaseStd_t *base;
{
 c_VectorGraph 	*this;

 Xc_HISTORY(("constructor(%lx)", (long)base));

 if((this = Xc_malloc("VectorGraph", sizeof(c_VectorGraph))) == NULL)
  return NULL;
 this->f = &fc_VectorGraph;

 if((this->spool = Xc_createSpool("vector items", SPOOL_SIZE, 0)) == NULL)
 {
  Xc_free(this);
  return NULL;
 }

 /*--- Link ---*/
 this->next = base->vector_base;
 base->vector_base = this;

 this->name[0] = 0;
 this->basestd = base;

 this->filename = NULL;
 this->base_head = this->base_tail = NULL;
 this->ptr = &this->base_head;

 this->angle = ANGLE_ZERO;
 this->frame = NULL;

 this->bbox.llx = this->bbox.lly = COORD_ZERO;
 this->bbox.urx = this->bbox.ury = COORD_ZERO;
 this->offset.dx = this->offset.dy = COORD_ZERO;
 this->width_factor = this->height_factor = SCALE_ONE;
 this->vflip_flag = FALSE;
 this->hflip_flag = FALSE;
 this->scale = SCALE_ONE;
 this->angle = ANGLE_ZERO;
 this->update_flag = TRUE;
 this->drawtext_flag = TRUE;
 this->realtime_draw = FALSE;
 this->mode = XcVG_NONE;

 this->path.control_count = this->path.data_count = 0;
 this->max_path_control = this->max_path_data = 0;
 this->path.control = NULL;
 this->path.data = NULL;
 this->building_path = FALSE;

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
 vector_item_t *item, *last_item;
 c_VectorGraph **ptr;

 Xc_HISTORY(("destructor"));

 /*--- Clean vector items ---*/
 for(item = this->base_head, last_item = NULL; item != NULL; )
 {
  if(item->type == XcVG_CONTEXT_TYPE)
  {
   Xc_ASSERT(item->data.children.tail != NULL);
   if(last_item != item->data.children.tail)
   {
    /*--- Push ---*/
    /*	      Xc_ASSERT(item->data.children.head != NULL); */
    last_item = item;
    item = item->data.children.head;
    continue;
   }
  }
  else if((item->type == XcVG_TEXT_FONT_TYPE) && item->data.font.data)
   XcH_UNREGISTER(item->data.font.data, NULL, this);
  else if(((item->type == XcVG_STROKE_COLOR_TYPE) ||
	   (item->type == XcVG_FILL_COLOR_TYPE)) && item->data.color)
   XcH_UNREGISTER(item->data.color, vectorHookColor, this);
  last_item = item;
  item = item->next;
 }
  
 Xc_destroySpool(this->spool);

 if(this->filename != NULL) Xc_free(this->filename);
 if(this->matrix != NULL) DELETE(c_Matrix)(this->matrix);
 if(this->path.control != NULL) Xc_free(this->path.control);
 if(this->path.data != NULL) Xc_free(this->path.data);

 /*--- Unlink ---*/
 for(ptr = &this->basestd->vector_base; *ptr != this; ptr = &(*ptr)->next)
  Xc_ASSERT(*ptr != NULL);
 *ptr = this->next;

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
 c_VectorGraph *this;

 Xc_HISTORY(("copy"));  

 if((this = Xc_malloc("VectorGraph", sizeof(c_VectorGraph))) == NULL)
  return NULL;

 /*--- Rough copy ---*/
 memcpy(this, orig, sizeof(c_VectorGraph));

 /*--- Copy filename ---*/
 if(orig->filename != NULL)
  if((this->filename = Xc_strdup("filename", orig->filename)) == NULL)
  {
   Xc_free(this);
   return NULL;
  }

 /*--- Init ---*/
 this->base_head = this->base_tail = NULL;
 this->basestd = base;
 this->frame = NULL;
 this->matrix = NULL;
 this->spool = NULL;
 this->max_path_control = this->max_path_data = 0;
 this->path.control = NULL;
 this->path.data = NULL;

 /*--- Link ---*/
 this->next = base->vector_base;
 base->vector_base = this;

 do 
 {
  /*--- Copy matrix ---*/
  if((this->matrix = COPY(c_Matrix)(orig->matrix)) == NULL) break;

  /*--- Copy spool ---*/
  if((this->spool = Xc_createSpool("vector items", SPOOL_SIZE, 0)) == NULL)
   break;
    
  /*--- Copy vector items ---*/
  if(!itemCopyStep(this, base, orig->base_head, orig->base_tail,
		   &this->base_head, &this->base_tail)) break;
  this->ptr = &this->base_tail->next;
    
  this->update_flag = TRUE;
  Xc_TRACE(("copied"));
  return this;
 } while(0);

 /*--- Not copied ---*/
 DELETE(c_VectorGraph)(this);
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** changeBase - BaseStd change                                       ** 
** ----------------------------------------------------------------- */
static error changeBase(this, base)
c_VectorGraph *this;
BaseStd_t *base;
{
 vector_item_t *item, *last_item;
 c_Color *color;
 c_VectorGraph **ptr;

 Xc_HISTORY(("changeBase"));

 for(item = this->base_head, last_item = NULL; item != NULL; )
 {
  if(item->type == XcVG_CONTEXT_TYPE)
  {
   Xc_ASSERT(item->data.children.tail != NULL);
   if(last_item != item->data.children.tail)
   {
    /*--- Push ---*/
    Xc_ASSERT(item->data.children.head != NULL);
    last_item = item;
    item = item->data.children.head;
    continue;
   }
  }
  else if((item->type == XcVG_STROKE_COLOR_TYPE) ||
	  (item->type == XcVG_FILL_COLOR_TYPE))
  {
   color = F(item->data.color).virtualCopy(item->data.color, base);
   if(color == NULL) return XC_MEMORY_ALLOCATION;
	  
   if(!XcH_REGISTER(color, vectorHookColor, this))
    return XC_MEMORY_ALLOCATION;

   XcH_UNREGISTER(item->data.color, vectorHookColor, this);
   item->data.color = color;
  }
  last_item = item;
  item = item->next;
 }

 /*--- Unlink & relink ---*/
 for(ptr = &this->basestd->vector_base; *ptr != this; ptr = &(*ptr)->next)
  Xc_ASSERT(*ptr != NULL);
 *ptr = this->next;
 this->basestd = base;
 this->next = base->vector_base;
 base->vector_base = this;

 Xc_TRACE(("done"));

 return XC_NO_ERROR;
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
** graphConfigure - display_configure standard frame method          ** 
** ----------------------------------------------------------------- */
static void graphConfigure(this, frame, poly)
c_VectorGraph	*this;
c_Cadre		*frame;
Polygone	*poly;
{
 Xc_HISTORY(("display_configure"));

 if((frame->sAngle != this->angle) || (this->mode != XcVG_NORMAL) ||
    (frame->sConfig_X != 0) || (frame->sConfig_Y != 0))
  computeParameters(this, frame);
}


/* ----------------------------------------------------------------- ** 
** computeParameters - Compute parameters from mode                  ** 
** ----------------------------------------------------------------- */
static error computeParameters(this, frame)
c_VectorGraph *this;
c_Cadre *frame;
{
 Xc_TRACE(("computeParameters(frame: %gx%g, scale=%g%%, angle=%g)", 
	   SCALE_TO_POINTS(frame->saWidth),
	   SCALE_TO_POINTS(frame->saHeight),
	   SCALE_TO_PERCENT(frame->sFactor),
	   SCALE_TO_DEGREES(frame->sAngle)));
 Xc_DUMP_BBOX("bbox", &this->bbox);
  
 if(this->mode != XcVG_NORMAL)
 {
  double vfactor, hfactor;
  coord_t w, h;

  w = this->bbox.urx - this->bbox.llx;
  h = this->bbox.ury - this->bbox.lly;
  vfactor = (double)frame->saHeight / h;
  hfactor = (double)frame->saWidth / w;

  switch(this->mode)
  {
   /*--- Best aspect from minimum boundaries ---*/
  case XcVG_BEST_ASPECT:
   Xc_TRACE(("XcVG_BEST_ASPECT"));
   vfactor = hfactor = MIN(vfactor, hfactor);
   break;
	  
   /*--- Best aspect from maximum boundaries ---*/
  case XcVG_MAX_ASPECT:
   Xc_TRACE(("XcVG_MAX_ASPECT"));
   vfactor = hfactor = MAX(vfactor, hfactor);
   break;
	  
   /*--- Autoresize ---*/
  case XcVG_AUTOSIZE:
   Xc_TRACE(("XcVG_AUTOSIZE"));
   break;

  case XcVG_NONE:
   Xc_TRACE(("XcVG_NONE"));
   this->mode = XcVG_NORMAL;
   vfactor = hfactor = 1.0;
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
 else
 {
  this->offset.dx -= frame->sConfig_X;
  this->offset.dy -= frame->sConfig_Y;
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
  if(this->vflip_flag == FALSE)
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

  F(matrix).translate(matrix, FALSE, -this->bbox.llx, -this->bbox.lly);

  /*--- call DISPLAY_INIT for each polygon ---*/

  this->update_flag = FALSE;
  return XC_NO_ERROR;
 }
 return XC_NO_ERROR;
}


/* ----------------------------------------------------------------- ** 
** hook_color - Color class hook                                     ** 
** ----------------------------------------------------------------- */
void vectorHookColor(this, reason, old, new)
c_VectorGraph *this;
int reason;
c_Color *old;
c_Color *new;
{
 vector_item_t *item, *last_item;

 Xc_TRACE(("hook_color"));

 if(reason == XcH_VALUE_CHANGED)
  for(item = this->base_head, last_item = NULL; item != NULL; )
  {
   if(item->type == XcVG_CONTEXT_TYPE)
   {
    Xc_ASSERT(item->data.children.tail != NULL);
    if(last_item != item->data.children.tail)
    {
     /*--- Push ---*/
     Xc_ASSERT(item->data.children.head != NULL);
     last_item = item;
     item = item->data.children.head;
     continue;
    }
   }
   else if(((item->type == XcVG_STROKE_COLOR_TYPE) ||
	    (item->type == XcVG_FILL_COLOR_TYPE)) &&
	   (item->data.color == old)) item->data.color = new;
   last_item = item;
   item = item->next;
  }
  

 if(this->frame != NULL) F(this->frame).configure_request(this->frame, FALSE);
}


/* ----------------------------------------------------------------- ** 
** itemCopyStep - Copy vector item tree                              ** 
** ----------------------------------------------------------------- */
static boolean itemCopyStep(this, base, shead, stail, thead, ttail)
c_VectorGraph *this;
BaseStd_t *base;
vector_item_t *shead;
vector_item_t *stail;
vector_item_t **thead;
vector_item_t **ttail;
{
 vector_item_t *item, *new;
 size_t size;

 Xc_TRACE(("itemCopyStep($%lx->$%lx)", (long)shead, (long)stail));

 for(item = shead; ; item = item->next)
 {
  size = (offsetof(vector_item_t, data) + 
	  vectorItemExtraSize[(int)item->type - (int)XcVG_CONTEXT_TYPE]);
  if((new = Xc_smalloc(this->spool, size)) == NULL) return FALSE;

  new->type = item->type;
  new->next = NULL;

  switch(item->type)
  {
   /*--- New context ---*/
  case XcVG_CONTEXT_TYPE:
   Xc_ASSERT(item->data.children.head != NULL);
   new->data.children.head = new->data.children.tail = NULL;
   if(!itemCopyStep(this, base, item->data.children.head,
		    item->data.children.tail, &new->data.children.head,
		    &new->data.children.tail)) return FALSE;
   new->data.children.tail->next = new;
   Xc_TRACE(("context($%lx->$%lx)", (long)new->data.children.head,
	     (long)new->data.children.tail));
   break;

   /*--- Path decription ---*/
  case XcVG_PATH_TYPE:
   memcpy(&new->data.path, &item->data.path, sizeof(vector_path_t));
   if(new->data.path.control_count)
   {
    /*--- Copy control codes ---*/
    size = new->data.path.control_count * sizeof(unsigned char);
    new->data.path.control = Xc_smalloc(this->spool, size);
    if(new->data.path.control == NULL) return FALSE;
    memcpy(new->data.path.control, item->data.path.control, size);

    /*--- Copy data ---*/
    size = new->data.path.data_count * sizeof(coord_t);
    if((new->data.path.data = Xc_smalloc(this->spool, size)) == NULL)
     return FALSE;
    memcpy(new->data.path.data, item->data.path.data, size);
   }
   break;

   /*--- Colors ---*/
  case XcVG_STROKE_COLOR_TYPE:
  case XcVG_FILL_COLOR_TYPE:
   new->data.color = F(item->data.color).virtualCopy(item->data.color,
						     base);
   if((new->data.color == NULL) ||
      !XcH_REGISTER(new->data.color, vectorHookColor, this))
    return FALSE;
   break;

   /*--- Line dash style ---*/
  case XcVG_LINE_DASH_TYPE:
   new->data.line_dash.phase = item->data.line_dash.phase;
   new->data.line_dash.length = item->data.line_dash.length;
   if((size = new->data.line_dash.length * sizeof(coord_t)) != 0)
   {
    new->data.line_dash.pattern = Xc_smalloc(this->spool, size);
    if(new->data.line_dash.pattern == NULL) return FALSE;
    memcpy(new->data.line_dash.pattern,
	   item->data.line_dash.pattern, size);
   }
   break;

   /*--- Text rendering ---*/
  case XcVG_TEXT_RENDER_TYPE:
   new->data.text.length = item->data.text.length;
   Xc_BBOX_COPY(&new->data.text.bbox, &item->data.text.bbox);
   size = new->data.text.length * sizeof(char_t);
   if((new->data.text.string = Xc_smalloc(this->spool, size)) == NULL)
    return FALSE;
   memcpy(new->data.text.string, item->data.text.string, size);
   break;

   /*--- TExt font ---*/
  case XcVG_TEXT_FONT_TYPE:
   new->data.font.size = item->data.font.size;
   new->data.font.data = NULL;
   if(!XcH_REGISTER(item->data.font.data, NULL, this)) return FALSE;
   new->data.font.data = item->data.font.data;
   break;

   /*--- Others ---*/
  default:
   memcpy(&new->data, &item->data,
	  vectorItemExtraSize[(int)new->type - (int)XcVG_CONTEXT_TYPE]);
   break;
  }
      
  /*--- Link new item ---*/
  if(*ttail != NULL)
  {
   (*ttail)->next = new;
   *ttail = new;
  }
  else
   *ttail = *thead = new;

  if(item == stail) return TRUE;
 }
}
