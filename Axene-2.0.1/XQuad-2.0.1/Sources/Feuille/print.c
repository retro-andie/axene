/*
** print.c for XQuad in Feuille/
** Printing methods for Feuille class
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
** Started on  Fri Jun  2 16:07:28 1995 Stéphane Boisson
** Last update Fri Oct  2 17:05:52 1998 Robin Castanier
*/

/*#define NHISTORY*/
/*#define NTRACE*/

#include "FeuilleP.h"
#include "vectors.h"


#define TITLE_WIDTH(style) \
F((style)->metrics).textWidth((style)->metrics, "88888", 5)
#define TITLE_HEIGHT(style) (style)->point_size


/*--- Local prototypes ---*/
static void getPageSize ___PROTO((c_Feuille *this, region_t *region,
				  coord_t *widthp, coord_t *heightp));


/* ----------------------------------------------------------------- ** 
** printFeuille - Print sheet                                        ** 
** ----------------------------------------------------------------- */
boolean printFeuille(this, post)
c_Feuille *this;
c_PostScript *post;
{
 region_t print_region, *selection;
 unsigned int page_count;
 coord_t width, height;
 setup_print_t *setup;
 c_Moteur *moteur;
  
 Xc_HISTORY(("ps_print"));

 setup = this->print_setup;
 moteur = this->moteur;
 page_count = 1;

 /*--- Get region to print ---*/
 if((post->ai_mode == FALSE) && (post->print_param->selection_flag == FALSE))
 {
  Xc_TRACE(("normal printing"));

  F(this).getBoundingRegion(this, &print_region);
  print_region.x_min = print_region.y_min = 0;
  print_region.NextRegion = NULL;
  selection = &print_region;

  Xc_TRACE(((post->print_param->AllPages?
	     "Print all pages" : "Print from page #%d to #%d"),
	    post->print_param->StartPage, post->print_param->EndPage));
 }
 else if(this->select_type == TRUE)
 {
  Xc_TRACE(("cell selection printing"));
  if ((this->cell_region->x_min == this->cell_region->x_max) &&
      (this->cell_region->y_min == this->cell_region->y_max) &&
      (post->print_param->selection_flag == FALSE))
  {
   F(this).getBoundingRegion(this, &print_region);
   print_region.NextRegion = NULL;
   selection = &print_region;
  }
  else
   selection = this->cell_region;
 }
 else
 {
  Xc_TRACE(("frame selection printing"));
  selection = NULL;
 }

 if(selection != NULL)
  do {
      coord_t home_pos, x, y;
      vector_t offset;
      region_t region;
      bbox_t bbox;
      int home;

      Xc_DUMP_REGION("region to print", selection);

      region.x_min = region.x_max = selection->x_min;
      region.y_min = region.y_max = selection->y_min;
      F(moteur).getBoundingBox(moteur, &region, &bbox);
      offset.dx = bbox.llx, offset.dy = bbox.lly;
      home_pos = (setup->direction_f == FALSE)? bbox.llx : bbox.lly;
      home = (setup->direction_f == FALSE)? region.x_min : region.y_min;

      if(post->ai_mode == FALSE)
      {
       getPageSize(this, selection, &width, &height);
       if((width <= 0) || (height <= 0))
       {
	Xc_ERROR(("No printable region"));
	continue;
       }
	  
       /*--- Orientation ---*/
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
	  
       if(setup->scale != SCALE_ONE)
       {
	x = ((double)x / SCALE_TO_COEF(setup->scale));
	y = ((double)y / SCALE_TO_COEF(setup->scale));
       }
       F(post->matrix).translate(post->matrix, TRUE, x, y);
       if(setup->orientation == Xc_LANDSCAPE)
	F(post->matrix).rotate(post->matrix, FALSE,
			       SCALE_FROM_DEGREES(90.0));
       F(post->matrix).scale(post->matrix, FALSE, SCALE_ONE, -SCALE_ONE);
      }
      else
      {
       F(moteur).getBoundingBox(moteur, selection, &bbox);

       x = y = COORD_ZERO;
       width = bbox.urx - bbox.llx + 1;
       height = bbox.ury - bbox.lly + 1;
       F(post->matrix).scale(post->matrix, TRUE, SCALE_ONE, -SCALE_ONE);
      }

      Xc_TRACE(("page=%gx%g", SCALE_TO_POINTS(width),
		SCALE_TO_POINTS(height)));
      
      while(42)
      {
       Xc_TRACE(("Page #%d offset (%g, %g)", page_count,
		 SCALE_TO_POINTS(offset.dx), SCALE_TO_POINTS(offset.dy)));

       F(moteur).getCellByXY(moteur, offset.dx + width, offset.dy + height,
			     &region.x_max, &region.y_max, &bbox);
       Xc_TRACE(("cell(%d; %d)", region.x_max, region.y_max));
	  
       if((bbox.urx > (offset.dx + width)) &&
	  (region.x_max > region.x_min)) region.x_max--;
       if((bbox.ury > (offset.dy + height)) &&
	  (region.y_max > region.y_min)) region.y_max--;
       if(region.x_max > selection->x_max) region.x_max = selection->x_max;
       if(region.y_max > selection->y_max) region.y_max = selection->y_max;
       Xc_TRACE(("cell2(%d; %d)", region.x_max, region.y_max));

       F(moteur).getBoundingBox(moteur, &region, &bbox);
       Xc_ASSERT(bbox.llx == offset.dx);
       Xc_ASSERT(bbox.lly == offset.dy);

       /*--- Check if page is printable ---*/
       if((post->ai_mode != FALSE) ||
	  (post->print_param->AllPages == TRUE) ||
	  (((int)page_count >= post->print_param->StartPage) &&
	   ((int)page_count <= post->print_param->EndPage)))
       {
	if(!F(post).beginPage(post, page_count)) return FALSE;

	/*--- Scale ---*/
	if(!post->ai_mode && (setup->scale != SCALE_ONE) &&
	   !F(post).putLine(post, "$f 0 0 $f 0 0 cm",
			    (double)SCALE_TO_COEF(setup->scale),
			    (double)SCALE_TO_COEF(setup->scale)))
	 return FALSE;

#if 0
	/*--- Print a frame around the printable region ---*/
	if(!F(post).putLine(post, "$p $v re 0 G S",
			    COORD_ZERO, COORD_ZERO,
			    width + setup->title_width,
			    height + setup->title_height)) return FALSE;
#endif

	/*--- Print region ---*/
	Xc_TRACE(("Page #%d region %d->%d, %d->%d", page_count,
		  region.x_min, region.x_max,
		  region.y_min, region.y_max));
	Xc_DUMP_BBOX("page_bbox", &bbox);
	      

	x = bbox.llx - setup->title_width;
	y = bbox.lly - setup->title_height;
	if(setup->hcenter_f && (bbox.urx < (offset.dx + width)))
	 x -= (width - bbox.urx + bbox.llx - 1) >> 1;
	if(setup->vcenter_f && (bbox.ury < (offset.dy + height)))
	 y -= (height - bbox.ury + bbox.lly - 1) >> 1;

	F(post->matrix).translate(post->matrix, TRUE, -x, -y);
	      
	/*--- Test if clipping needed ---*/
	if((post->ai_mode == FALSE) &&
	   ((bbox.urx > (offset.dx + width)) ||
	    (bbox.ury > (offset.dy + height))))
	{
	 bbox_t clip_bbox;

	 clip_bbox.llx = bbox.llx - setup->title_width;
	 clip_bbox.lly = bbox.lly - setup->title_height;
	 clip_bbox.urx = offset.dx + width;
	 clip_bbox.ury = offset.dy + height;
	 Xc_DUMP_BBOX("clip_bbox", &clip_bbox);
	 if(!F(post).putLine(post, "$r re W n", &clip_bbox))
	  return FALSE;
	}
	      
	/*--- Print cells ---*/
	if(!F(moteur).ps_print(moteur, post, &region, setup))
	 return FALSE;
	      
	/*--- Print frames if needed ---*/
	if((post->ai_mode == FALSE) &&
	   (post->print_param->selection_flag == FALSE) &&
	   !F(this->frmg).ps_print(this->frmg, post, &bbox, setup))
	 return FALSE;

	F(post->matrix).pop(post->matrix);
	if(!F(post).endPage(post)) return FALSE;
       }
	  
       /*--- Increment page number ---*/
       page_count++;
	  
       /*--- Test if region completly printed ---*/
       if((region.x_max == selection->x_max) &&
	  (region.y_max == selection->y_max)) break;
       Xc_ASSERT(post->ai_mode == FALSE);

       /*--- Compute next page ---*/
       if(setup->direction_f == FALSE)
       {
	/*--- Left-To-Right then Top-To-Bottom ---*/
	if(region.x_max == selection->x_max)
	{
	 offset.dx = home_pos;
	 offset.dy = bbox.ury + 1; 
	 region.y_min = region.y_max + 1; 
	 region.x_min = home;
	}
	else
	{
	 region.x_min = region.x_max + 1; 
	 offset.dx = bbox.urx + 1;
	}
       }
       else
       {
	/*--- Top-To-Bottom then Left-To-Right ---*/
	if(region.y_max == selection->y_max)
	{
	 offset.dy = home_pos;
	 offset.dx = bbox.urx + 1; 
	 region.x_min = region.x_max + 1; 
	 region.y_min = home;
	}
	else
	{
	 region.y_min = region.y_max + 1; 
	 offset.dy = bbox.ury + 1;
	}
       }
	  
      }
      F(post->matrix).pop(post->matrix);
     } while((post->ai_mode == FALSE) &&
	     (selection = selection->NextRegion) != NULL);
 else
 {
  /*--- Print only frames ---*/
  return F(this->frmg).ps_print(this->frmg, post, NULL, setup);
 }
    
 Xc_TRACE(("ps_print end"));
 return TRUE;
} 


/* ----------------------------------------------------------------- ** 
** getPageSize - Returns page size                                   ** 
** ----------------------------------------------------------------- */
static void getPageSize(this, region, widthp, heightp)
c_Feuille *this;
region_t *region;
coord_t *widthp;
coord_t *heightp;
{
 coord_t width, height;
 setup_print_t *setup;

 Xc_TRACE(("getPageSize"));

 setup = this->print_setup;
#if 0
 width = setup->width - setup->left_margin - setup->right_margin;
 height = setup->height - setup->top_margin - setup->bottom_margin;

 /*--- Orientation ---*/
 if(setup->orientation == Xc_LANDSCAPE) SWAP(coord_t, width, height);
#endif
 width = setup->width; height = setup->height;

 /*--- Orientation ---*/
 if(setup->orientation == Xc_LANDSCAPE) SWAP(coord_t, width, height);
 width -= setup->left_margin + setup->right_margin;
 height -= setup->top_margin + setup->bottom_margin;

 /*--- Scaling ---*/
 switch(setup->scale_mode)
 {
  /*--- Scale adjust ---*/
 case 2:
  Xc_TRACE(("Adjust scale on %dx%d pages",
	    setup->horizontal_pages, setup->vertical_pages));
      
  setup->scale = F(this).computeScaling(this, region,
					setup->horizontal_pages,
					setup->vertical_pages);
  /*--- Scale ---*/
 case 1:
  Xc_TRACE(("Scaling %g%%", SCALE_TO_PERCENT(setup->scale)));
  Xc_ASSERT(setup->scale > SCALE_ZERO);

  width = (coord_t)((double)width / SCALE_TO_COEF(setup->scale));
  height = (coord_t)((double)height / SCALE_TO_COEF(setup->scale));
  break;

  /*--- No scaling ---*/
 default:
  setup->scale_mode = 0;
  setup->scale = SCALE_ONE;
  break;
 }

 /*--- Title ---*/
 if(setup->rowcol_f != FALSE)
 {
  c_TextStyle *style;

  style = setup->title_style = this->orig_style;
  setup->title_width = TITLE_WIDTH(style) + COORD_ONE + COORD_ONE;
  setup->title_height = TITLE_HEIGHT(style) + COORD_ONE + COORD_ONE;
  width -= setup->title_width;
  height -= setup->title_height;
  Xc_TRACE(("Title %gx%g", SCALE_TO_POINTS(setup->title_width),
	    SCALE_TO_POINTS(setup->title_height)));
 }
 else
  setup->title_width = setup->title_height = COORD_ZERO;

 Xc_TRACE(("Printing area: w=%g h=%g", SCALE_TO_POINTS(width),
	   SCALE_TO_POINTS(height)));
 *widthp = width;
 *heightp = height;
}


/* ----------------------------------------------------------------- ** 
** CalculPageWindow - calcul page window size                        ** 
** ----------------------------------------------------------------- */
void CalculPageWindow(This, redraw)
c_Feuille *This;
boolean	  redraw;
{
 region_t region;

 Xc_HISTORY(("CalculPageWindow"));

 F(This).getBoundingRegion(This, &region);
 region.x_min = region.y_min = 0;
 getPageSize(This, &region, &This->page_window_width,
	     &This->page_window_height);

 if(redraw)
 {
  calcul_column_width(This);
  calcul_row_height(This);
  calcul_cell_info(This);
  redraw_rectangle(This, Xq_ORIGIN, 0, 0, 
		   This->sheet_width, This->sheet_height);
 }
}


/* ----------------------------------------------------------------- ** 
** getBoundingRegion - Get Feuille bounding region                   ** 
** ----------------------------------------------------------------- */
void feuilleGetBoundingRegion(this, region)
c_Feuille *this;
region_t *region;
{
 bbox_t frame_bbox;
  
 Xc_HISTORY(("getBoundingRegion"));
  
 F(this->moteur).getBoundingRegion(this->moteur, region);
  
 if(F(this->frmg).get_bbox_list_cadre(this->frmg, &frame_bbox, NULL))
 {
  int x, y;
      
  /*--- Update print region with frames ---*/
  Xc_DUMP_BBOX("frame_bbox", &frame_bbox);
  F(this->moteur).getCellByXY(this->moteur, frame_bbox.urx, frame_bbox.ury,
			      &x, &y, NULL);
  if(x > region->x_max) region->x_max = x;
  if(y > region->y_max) region->y_max = y;
 }
}


/* ----------------------------------------------------------------- ** 
** getPagesCount - returns horizontal & vertical pages               ** 
** ----------------------------------------------------------------- */
void feuilleGetPagesCount(this, selection, wp, hp)
c_Feuille *this;
region_t *selection;
unsigned int *wp;
unsigned int *hp;
{
 coord_t width, height, home_pos;
 unsigned int page_count;
 setup_print_t *setup;
 vector_t offset;
 region_t region;
 int home, w;
 bbox_t bbox;

 Xc_HISTORY(("moteurGetPagesCount"));

 *wp = *hp = w = 1;

 setup = this->print_setup;
 getPageSize(this, selection, &width, &height);

 region.x_min = region.x_max = selection->x_min;
 region.y_min = region.y_max = selection->y_min;
 F(this->moteur).getBoundingBox(this->moteur, &region, &bbox);
 offset.dx = bbox.llx, offset.dy = bbox.lly;
 home_pos = bbox.llx;
 home = region.x_min;
 page_count = 0;

 while(42)
 {
  page_count++;

  Xc_TRACE(("Page #%d offset (%g, %g)", page_count,
	    SCALE_TO_POINTS(offset.dx), SCALE_TO_POINTS(offset.dy)));

  F(this->moteur).getCellByXY(this->moteur,
			      offset.dx + width, offset.dy + height,
			      &region.x_max, &region.y_max, &bbox);

  if((bbox.urx > (offset.dx + width)) &&
     (region.x_max > region.x_min)) region.x_max--;

  if((bbox.ury > (offset.dy + height)) &&
     (region.y_max > region.y_min)) region.y_max--;
      
  if(region.x_max > selection->x_max) region.x_max = selection->x_max;
  if(region.y_max > selection->y_max) region.y_max = selection->y_max;
      
  F(this->moteur).getBoundingBox(this->moteur, &region, &bbox);
  Xc_ASSERT(bbox.llx == offset.dx);
  Xc_ASSERT(bbox.lly == offset.dy);
      
  Xc_TRACE(("Page #%d region %d->%d, %d->%d", page_count,
	    region.x_min, region.x_max,
	    region.y_min, region.y_max));
	      	  
  /*--- Test if region completly printed ---*/
  if((region.x_max == selection->x_max) &&
     (region.y_max == selection->y_max)) break;
      
      
  /*--- Left-To-Right then Top-To-Bottom ---*/
  if(region.x_max == selection->x_max)
  {
   if(w > (int)*wp) *wp = w;
   w = 1;
   *hp += 1;
   offset.dx = home_pos;
   offset.dy = bbox.ury + 1; 
   region.y_min = region.y_max + 1; 
   region.x_min = home;
  }
  else
  {
   w++;
   region.x_min = region.x_max + 1; 
   offset.dx = bbox.urx + 1;
  }
 }
 if(w > (int)*wp) *wp = w;
 Xc_TRACE(("pages: %d %dx%d", page_count, *wp, *hp));
 Xc_ASSERT(((*wp) * (*hp)) == page_count);
}


/* ----------------------------------------------------------------- ** 
** computeScaling - Compute scaling to fit in w*h pages              ** 
** ----------------------------------------------------------------- */
scale_t feuilleComputeScaling(this, region, w, h)
c_Feuille *this;
region_t *region;
unsigned int w;
unsigned int h;
{
 coord_t width, height, tmp;
 double factor, factor2;
 int old_scale_mode;
 scale_t old_scale;
 bbox_t bbox;

 Xc_HISTORY(("computeScaling(%dx%d)", w, h));

 F(this->moteur).getBoundingBox(this->moteur, region, &bbox);
  
 old_scale_mode = this->print_setup->scale_mode;
 old_scale = this->print_setup->scale;
 this->print_setup->scale_mode = 0;
 getPageSize(this, region, &width, &height);

 /*--- Horizontal scaling ---*/
 tmp = bbox.urx - bbox.llx + 1 + (this->print_setup->title_width * w);
 factor2 = (double)((width + this->print_setup->title_width) * w) / tmp;
 Xc_TRACE(("hscale %g%%", factor2 * 100.0));
 if(factor2 > 1.0) factor2 = 1.0;

 /*--- Vertical scaling ---*/
 tmp = bbox.ury - bbox.lly + 1 + this->print_setup->title_height;
 factor = (double)((height + this->print_setup->title_height) * h) / tmp;
 Xc_TRACE(("vscale %g%%", factor * 100.0));
 if(factor > factor2) factor = factor2;

 /*--- Check scaling ---*/
 Xc_TRACE(("Initial Scale %g%%", factor * 100.0));
 Xc_ASSERT(factor <= 1.0);
 if(factor < 1.0)
 {
  unsigned int wcheck, hcheck;
      
  this->print_setup->scale_mode = 1;
  while(42)
  {
   this->print_setup->scale = SCALE_FROM_COEF(factor);
   F(this).getPagesCount(this, region, &wcheck, &hcheck);
   if(((wcheck <= w) && (hcheck <= h)) || (factor <= 0.0)) break; 
   factor -= 0.01;
  }
  if(factor <= 0.0) factor = 0.01;
 }
 Xc_TRACE(("Final Scale %g%%", factor * 100.0));

 this->print_setup->scale_mode = old_scale_mode;
 this->print_setup->scale = old_scale;
 return SCALE_FROM_COEF(factor);
}
