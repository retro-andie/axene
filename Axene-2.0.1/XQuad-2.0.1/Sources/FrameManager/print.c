/*
** print.c for XQuad in FrameManager/
** Print methods for FrameManger class
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
** Started on  Mon Jul 24 14:24:45 1995 Stéphane Boisson
** Last update Fri Aug  7 14:32:55 1998 Emmanuel Paris
*/

#include "FrameManagerP.h"

static l_Cadre	*list_cadre_by_area ___PROTO((c_FrMg *This, bbox_t area));


/* ----------------------------------------------------------------- ** 
** printFrameList - Print several Frames                             ** 
** ----------------------------------------------------------------- */
boolean printFrameList(this, post, bbox, setup)
c_FrMg *this;
c_PostScript *post;
bbox_t *bbox;
setup_print_t *setup;
{
 unsigned int page_count;
 coord_t width = COORD_ZERO, height = COORD_ZERO;
 l_Cadre *ptr;

 Xc_HISTORY(("print"));

 /*--- Test if normal printing ---*/
 if((post->ai_mode == FALSE) && (post->print_param->selection_flag == FALSE))
 {
  l_Cadre *list;
  
  /*--- Print all frames in bbox ---*/
  if((ptr = list_cadre_by_area(this, *bbox)) == NULL) return TRUE;
  list = sort_asc_list_cadre(ptr);
  free_list_cadre(ptr, NO_DEL_CADRE);
  if(list == NULL) return FALSE;
      
  /*--- Clip region ---*/
  if(!F(post).putLine(post, "q $r re W n", bbox)) return FALSE;
  if(!F(post).pushState(post)) return FALSE;
      
  /*--- Print all frames ---*/
  F(post->matrix).translate(post->matrix, TRUE,
			    -this->sOrig_X, -this->sOrig_Y);
      
  for(ptr = list ; ptr != NULL; ptr = ptr->NextCadre)
   if(!F(ptr->cadre).ps_print(ptr->cadre, post)) return FALSE;
      
  F(post->matrix).pop(post->matrix);
  free_list_cadre(list, NO_DEL_CADRE);
      
  return(F(post).popState(post) && F(post).putLine(post, "Q"));
 }


 /*--- Print selection frame by frame ---*/
 Xc_ASSERT(this->SelectedCadre != NULL);

 if(post->ai_mode == FALSE)
 {
  width = setup->width - setup->left_margin - setup->right_margin;
  height = setup->height - setup->top_margin - setup->bottom_margin;
  Xc_TRACE(("Printing area: w=%g h=%g", SCALE_TO_POINTS(width),
	    SCALE_TO_POINTS(height)));
  if((width <= 0) || (height <= 0))
  {
   Xc_ERROR(("No printable region"));
   return FALSE;
  }
  if(setup->orientation == Xc_LANDSCAPE) SWAP(coord_t, width, height);
 }

 page_count = 1;

 /*--- Frame loop ---*/
 for(ptr = this->SelectedCadre ; ptr != NULL; ptr = ptr->NextCadre)
 {
  double hscale, scale;
  c_Cadre *frame;
  coord_t x, y;
      
  frame = ptr->cadre;
  Xc_TRACE(("frame w=%g h=%g", SCALE_TO_POINTS(frame->sWidth),
	    SCALE_TO_POINTS(frame->sHeight)));

  if(post->ai_mode == FALSE)
  {
   /*--- Compute scaling ---*/
   hscale = ((double)width / frame->sWidth);
   Xc_TRACE(("hscale %g%%", hscale * 100.0));
   scale = ((double)height / frame->sHeight);
   Xc_TRACE(("vscale %g%%", scale * 100.0));
   if(scale > hscale) scale = hscale;
   Xc_TRACE(("scale %g%%", scale * 100.0));
	  
   /*--- Setup matrix ---*/
   if(setup->orientation == Xc_LANDSCAPE)
   {
    Xc_TRACE(("Landscape"));
	      
    x = setup->top_margin;
    y = setup->left_margin;
   }
   else
   {
    Xc_TRACE(("Portrait"));
	      
    x = setup->left_margin;
    y = setup->height - setup->top_margin;
   }
	  
   F(post->matrix).translate(post->matrix, TRUE,
			     (coord_t)((double)x / scale),
			     (coord_t)((double)y / scale));
   if(setup->orientation == Xc_LANDSCAPE)
    F(post->matrix).rotate(post->matrix, FALSE,
			   SCALE_FROM_DEGREES(90.0));
   F(post->matrix).scale(post->matrix, FALSE, SCALE_ONE, -SCALE_ONE);
  }
  else
  {
   scale = 1.0;
   F(post->matrix).scale(post->matrix, TRUE, SCALE_ONE, -SCALE_ONE);
  }

  /*--- Begin printing ---*/
  if(!F(post).beginPage(post, page_count)) return FALSE;

  /*--- Scaling ---*/
  if(((scale != 1.0) &&
      !F(post).putLine(post, "$f 0 0 $f 0 0 cm", scale, scale)))
   return FALSE;

#if 0
  /*--- Print a frame around the printable region ---*/
  if(!F(post).putLine(post, "$p $v re 0 G S", COORD_ZERO, COORD_ZERO,
		      (coord_t)((double)width / scale),
		      (coord_t)((double)height / scale))) return TRUE;
#endif
      
  /*--- Centering ---*/
  if(post->ai_mode == FALSE)
  {
   x = ((setup->hcenter_f == FALSE)? COORD_ZERO :
	((width - (coord_t)(scale * frame->sWidth)) >> 1));
   y = ((setup->vcenter_f == FALSE)? COORD_ZERO :
	((height - (coord_t)(scale * frame->sHeight)) >> 1));
   F(post->matrix).translate
    (post->matrix, FALSE,
     (coord_t)((double)x / scale) - frame->sOrig_X,
     (coord_t)((double)y / scale) - frame->sOrig_Y);
  }
      
  if(!F(frame).ps_print(frame, post)) return FALSE;

  if(!F(post).endPage(post)) return FALSE;
  F(post->matrix).pop(post->matrix);
  if(post->ai_mode != FALSE) return TRUE;
  page_count++;
 }

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** list_cadre_by_area - return a l_Cadre                             ** 
** ----------------------------------------------------------------- */
static l_Cadre *list_cadre_by_area(This, area)
c_FrMg	*This;
bbox_t	area;
{
 coord_t  x_min;
 coord_t  x_max;
 coord_t  y_min;
 coord_t  y_max;
 coord_t  c_x_min;
 coord_t  c_x_max;
 coord_t  c_y_min;
 coord_t  c_y_max;
 l_Cadre  *ret_lcadre;
 l_Cadre  *temp_lcadre;
 l_Cadre  *lcadre;
 l_Point  *lpoint;
 boolean  in_page;

  
 x_min = area.llx;
 y_min = area.lly;
 x_max = area.urx;
 y_max = area.ury;

 ret_lcadre = NULL;
 lcadre = This->ListCadre;
 while(lcadre)
 {
  c_x_min = lcadre->cadre->sOrig_X + This->sOrig_X;
  c_y_min = lcadre->cadre->sOrig_Y + This->sOrig_Y;
  c_x_max = c_x_min + lcadre->cadre->sWidth;
  c_y_max = c_y_min + lcadre->cadre->sHeight;
  if (!((c_x_min<x_min && c_x_max<x_min) || 
	(c_x_min>x_max && c_x_max>x_max) ||
	(c_y_min<y_min && c_y_max<y_min) ||
	(c_y_min>y_max && c_y_max>y_max)))
  {
   lpoint = lcadre->cadre->ListPoint;
   in_page = FALSE;
   while(lpoint && !in_page)
   {
    if (lpoint->sX+c_x_min>=x_min &&
	lpoint->sX+c_x_min<=x_max &&
	lpoint->sY+c_y_min>=y_min &&
	lpoint->sY+c_y_min<=y_max)
     in_page = TRUE;
    lpoint = lpoint->NextPoint;
   }
   if (!in_page &&
       (( c_x_min<=x_min && c_x_max>=x_max) || 
	( c_y_min<=y_min && c_y_max>=y_max)))
    in_page = TRUE;
   if (in_page)
   {
    temp_lcadre = (l_Cadre *)Xc_malloc("ls/pg", sizeof(l_Cadre));
    if(temp_lcadre == NULL) return NULL;
    memcpy(temp_lcadre, lcadre, sizeof(l_Cadre));
    temp_lcadre->NextCadre = ret_lcadre;
    ret_lcadre = temp_lcadre;
   }
  }
  lcadre = lcadre->NextCadre;
 }
 return ret_lcadre;
}
