/*
** print.c for XQuad in Moteur/
** Print methods
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
** Started on  Fri Jun  2 14:55:32 1995 Stéphane Boisson
** Last update Fri Jul 24 01:54:50 1998 Emmanuel Paris
*/

#define NTRACE

#include "MoteurP.h"
#include "CharMetrics.h"

#define AREA_OPTIMIZATION

/*--- Local functions ---*/
static boolean print_cell ___PROTO((c_Moteur *this, c_PostScript *post,
				    c_CellStyle *csbase, moteur_print_t *info,
				    int x, int y, cell *cp,
				    boolean clip_flag, boolean overrun_flag));
static cell_style_list *optimizeArea ___PROTO((cell_style_list *src,
					       cell_style_list *array,
					       long len));

/*--- Constants ---*/
#define XqCELL_MARGIN (COORD_ONE >> 1)


/* ----------------------------------------------------------------- ** 
** getMoteurBoundingRegion - Get bounding region                     ** 
** ----------------------------------------------------------------- */
void getMoteurBoundingRegion(this, regionp)
c_Moteur *this;
region_t *regionp;
{
 int minx, miny, maxx, maxy;
 cell_style_list *area;
 c_CellStyle *csbase;
 c_col *column;

 Xc_HISTORY(("getBoundingRegion"));
 Xc_ASSERT(regionp != NULL);

 minx = miny = 100000;
 maxx = maxy = -1;

 /*--- Get bound box from cell content ---*/
 for(column = this->tableCL ; column != NULL; column = column->Next)
 {
  boolean first_flag;
  c_line *line;

  first_flag = TRUE;
 
  for(line = column->line; line != NULL; line = line->Next)
  {
   /*--- Empty cell ---*/
   if(line->cell.type == Xq_VOID) continue;

   /*--- This cell may be overrunning ---*/
   if(line->cell.type == Xq_TEXT)
   {
    cell *cp;
	      
    cp = &line->cell;
    F(this).computeOverrun(this, cp, column->number, line->number,
			   SCALE_ONE);
    if((cp->overrun_pos || cp->overrun_neg))
    {
     region_t over_region;
		  
     first_flag = FALSE;
     F(this).getOverrunRegion(this, cp, column->number,
			      line->number, &over_region);
     if(over_region.x_min < minx) minx = over_region.x_min;
     if(over_region.y_min < miny) miny = over_region.y_min;
     if(over_region.x_max > maxx) maxx = over_region.x_max;
     if(over_region.y_max > maxy) maxy = over_region.y_max;
     continue;
    }
   }

   if(first_flag != FALSE)
   {
    first_flag = FALSE;
    
    /*--- Update column extrema ---*/
    if(maxx < column->number) maxx = column->number;
    if(minx > column->number) minx = column->number;
   }
	  
   if(maxy < line->number) maxy = line->number;
   if(miny > line->number) miny = line->number;
  }

 }
 Xc_TRACE(("intermediate: %d->%d, %d->%d", minx, maxx, miny, maxy));

 /* attention, il faudrait seulement tester les cell styles avec des bords */
 /* ou une couleur de fond differentes, et se baser sur la region obtenue */
 /* precedemment. */ 

 /*--- Update bounding box from areas ---*/
 csbase = this->BaseStd->cell_style_base;
 for(area = this->style_list; area != NULL; area = area->Next)
  if(area->cstyle != csbase->default_cstyle)
  {
   region_t *region;

   region = &area->region;
   if(region->x_min < minx) minx = region->x_min;
   if(region->y_min < miny) miny = region->y_min;
   if((region->x_max > maxx) && (region->x_max != COLUMN_MAX))
    maxx = region->x_max;
   if((region->y_max > maxy) && (region->y_max != ROW_MAX))
    maxy = region->y_max;
  }
  
 if(maxx == -1) minx = miny = maxx = maxy = 0;

 regionp->x_min = minx;
 regionp->x_max = maxx;
 regionp->y_min = miny;
 regionp->y_max = maxy;
 Xc_TRACE(("final: %d->%d, %d->%d", minx, maxx, miny, maxy));
}


/* ----------------------------------------------------------------- ** 
** getMoteurBoundingBox - Get the bound box of an region             ** 
** ----------------------------------------------------------------- */
void getMoteurBoundingBox(this, region, bboxp)
c_Moteur *this;
region_t *region;
bbox_t *bboxp;
{
 cell_size_list *ptr;
 coord_t size;

 Xc_ASSERT(region != NULL);
 Xc_HISTORY(("getBoundingBox(%d->%d, %d->%d)",
	     region->x_min, region->x_max, region->y_min, region->y_max));
 Xc_ASSERT(bboxp != NULL);
  
 /*--- Min X pos ---*/
 size = COORD_ZERO;
 for(ptr = this->col_size; ptr->Amax < region->x_min; ptr = ptr->Next)
  size += ptr->size * (ptr->Amax - ptr->Amin + 1);
    
 bboxp->llx = size + ptr->size * (region->x_min - ptr->Amin);
  
 /*--- Max X pos ---*/
 for( ; ptr->Amax < region->x_max; ptr = ptr->Next)
  size += ptr->size * (ptr->Amax - ptr->Amin + 1);
    
 bboxp->urx = size + ptr->size * (region->x_max - ptr->Amin + 1) - 1;

 /*--- Min Y pos ---*/
 size = COORD_ZERO;
 for(ptr = this->line_size; ptr->Amax < region->y_min; ptr = ptr->Next)
  size += ptr->size * (ptr->Amax - ptr->Amin + 1);

 bboxp->lly = size + ptr->size * (region->y_min - ptr->Amin);
  
 /*--- Max Y pos ---*/
 for( ; ptr->Amax < region->y_max; ptr = ptr->Next)
  size += ptr->size * (ptr->Amax - ptr->Amin + 1);

 bboxp->ury = size + ptr->size * (region->y_max - ptr->Amin + 1) - 1;

 Xc_DUMP_BBOX("bbox", bboxp);
}


/* ----------------------------------------------------------------- ** 
** getMoteurCellByXY - Get the cell at position XY                   ** 
** ----------------------------------------------------------------- */
void getMoteurCellByXY(this, x, y, columnp, rowp, bboxp)
c_Moteur *this;
coord_t x;
coord_t y;
int *columnp;
int *rowp;
bbox_t *bboxp;
{
 coord_t size, old_size;
 cell_size_list *ptr;
 int pos;

 Xc_HISTORY(("getCellByXY(%g, %g)", SCALE_TO_POINTS(x), SCALE_TO_POINTS(y)));

 /*--- Search column ---*/
 if(columnp != NULL)
 {
  old_size = COORD_ZERO;
  ptr = this->col_size;
      
  if(x >= COORD_ZERO)
  {
   size = ptr->size * (ptr->Amax - ptr->Amin + 1);
   while(((old_size + size) <= x) && (ptr->Next != NULL))
   {
    ptr = ptr->Next;
    old_size += size;
    size = ptr->size * (ptr->Amax - ptr->Amin + 1);
   }
      
   *columnp = pos = ptr->Amin + ((x - old_size) / ptr->size);
  }
  else
   *columnp = pos = 0;

  Xc_TRACE(("Column %d", pos));

  if(bboxp != NULL)
  {
   bboxp->llx = bboxp->urx = old_size + (ptr->size * (pos - ptr->Amin));
   bboxp->urx += (((ptr->Amax >= pos) || (ptr->Next == NULL))?
		  ptr->size : ptr->Next->size); 
  }
 }
 else if(bboxp != NULL)
  bboxp->llx = bboxp->urx = COORD_ZERO;

 /*--- Search row ---*/
 if(rowp != NULL)
 {
  old_size = COORD_ZERO;
  ptr = this->line_size;
      
  if(y >= COORD_ZERO)
  {
   size = ptr->size * (ptr->Amax - ptr->Amin + 1);
   while(((old_size + size) <= y) && (ptr->Next != NULL))
   {
    ptr = ptr->Next;
    old_size += size;
    size = ptr->size * (ptr->Amax - ptr->Amin + 1);
   }
	  
   *rowp = pos = ptr->Amin + ((y - old_size) / ptr->size);
  }
  else
   *rowp = pos = 0;

  Xc_TRACE(("Row %d", pos));

  if(bboxp != NULL)
  {
   bboxp->lly = bboxp->ury = old_size + (ptr->size * (pos - ptr->Amin));
   bboxp->ury += (((ptr->Amax >= pos) || (ptr->Next == NULL))?
		  ptr->size : ptr->Next->size); 
  }
 }
 else if(bboxp != NULL)
  bboxp->lly = bboxp->ury = COORD_ZERO;
}


/* ----------------------------------------------------------------- ** 
** optimizeArea - Optimize areas                                     ** 
** ----------------------------------------------------------------- */
static cell_style_list *optimizeArea(dest, ptr, len)
cell_style_list *dest;
cell_style_list *ptr;
long len;
{
 for( ; len-- > 0; ptr++)
  if(ptr->cstyle == dest->cstyle)
  {
   Xc_DUMP_REGION("test region", &ptr->region);
		    
   if((ptr->region.x_min == dest->region.x_min) &&
      (ptr->region.x_max == dest->region.x_max))
   {
    if(ptr->region.y_min == (dest->region.y_max + 1))
    {
     Xc_TRACE(("optimize up"));
     ptr->region.y_min = dest->region.y_min;
     return dest;
    }
    else if(ptr->region.y_max == (dest->region.y_min - 1))
    {
     Xc_TRACE(("optimize down"));
     ptr->region.y_max = dest->region.y_max;
     return dest;
    }
   }
   else if((ptr->region.y_min == dest->region.y_min) &&
	   (ptr->region.y_max == dest->region.y_max))
   {
    if(ptr->region.x_min == (dest->region.x_max + 1))
    {
     Xc_TRACE(("optimize left"));
     ptr->region.x_min = dest->region.x_min;
     return dest;
    }
    else if(ptr->region.x_max ==(dest->region.x_min - 1))
    {
     Xc_TRACE(("optimize right"));
     ptr->region.x_max = dest->region.x_max;
     return dest;
    }
   }
  }
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** getMoteurCellStyleList - Get the CellStyle list for a region      ** 
** ----------------------------------------------------------------- */
cell_style_list *getMoteurCellStyleList(this, region)
c_Moteur *this;
region_t *region;
{
 cell_style_list *base, *src, *dest, tmp;
 int *top_buffer, *left_buffer;
 long len, width, height, i;
  
 Xc_HISTORY(("getCellStyleList(%d->%d, %d->%d)",
	     region->x_min, region->x_max, region->y_min, region->y_max));

 width = region->x_max - region->x_min + 1;
 height = region->y_max - region->y_min + 1;
 len = width * height;

 Xc_TRACE(("len = %ld", len));
 dest = base = Xc_malloc("CellStyleList", len * sizeof(cell_style_list));
 if(base == NULL) return NULL;

 for(src = this->style_list; src != NULL; src = src->Next)
 {
  register int a, b;

  a = MIN(src->region.x_max, region->x_max);
  b = MAX(src->region.x_min, region->x_min);
  if(a >= b)
  {
   dest->region.x_min = b;
   dest->region.x_max = a;

   a = MIN(src->region.y_max, region->y_max);
   b = MAX(src->region.y_min, region->y_min);
   if(a >= b)
   {
    dest->region.y_min = b;
    dest->region.y_max = a;
    dest->cstyle = src->cstyle;
    Xc_DUMP_REGION("region", &dest->region);

#ifdef AREA_OPTIMIZATION
    if(optimizeArea(dest, base, dest - base) == NULL) dest++;
#else
    dest++;
#endif
    if((dest - base) == len) break;
   }
  }
 }

#ifdef AREA_OPTIMIZATION
 /*--- Optimize regions ---*/
 for(src = base; src != dest; )
  if(optimizeArea(src, base, dest - base) != NULL)
  {
   long size;
	
   if((size = (--dest - src)) != 0)
    memmove(src, src + 1, size * sizeof(cell_style_list));
  }
  else
   src++;
#endif

 if(len != (dest - base))
 {
  /*--- Realloc memory ---*/
  len = dest - base;
  Xc_TRACE(("real len = %ld", len));
  Xc_ASSERT(len != 0);
  dest = Xc_realloc("CellStyleList", base, len * sizeof(cell_style_list));
  if(dest != NULL) base = dest;
 }

 /*--- Rearrange areas ---*/
 Xc_TRACE(("re-arrange areas"));
 top_buffer = Xc_malloc("buffer", (width + height) * sizeof(int));
 if(top_buffer == NULL)
 {
  Xc_free(dest);
  return NULL;
 }
 left_buffer = top_buffer + (width - region->y_min);
 top_buffer -= region->x_min;

 for(i = region->x_min; i <= region->x_max; i++)
  top_buffer[i] = region->y_min;

 for(i = region->y_min; i <= region->y_max; i++)
  left_buffer[i] = region->x_min;

 for(src = base, dest = base + len - 1; src != dest; )
 {
  Xc_DUMP_REGION("region", &src->region);
  for(i = src->region.x_min; i <= src->region.x_max; i++)
   if(top_buffer[i] != src->region.y_min) break;

  if(i > src->region.x_max)
  {
   for(i = src->region.y_min; i <= src->region.y_max; i++)
    if(left_buffer[i] != src->region.x_min) break;
	  
   if(i > src->region.y_max)
   {
    Xc_TRACE(("inc"));
    for(i = src->region.x_min; i <= src->region.x_max; i++)
     top_buffer[i] = src->region.y_max + 1;
    for(i = src->region.y_min; i <= src->region.y_max; i++)
     left_buffer[i] = src->region.x_max + 1;
	      
    src++;
    continue;
   }
  }
  Xc_TRACE(("swap %ld", (long)(dest - src + 1)));
  memcpy(&tmp, src, sizeof(cell_style_list));
  memmove(src, src + 1, (dest - src) * sizeof(cell_style_list));
  memcpy(dest, &tmp, sizeof(cell_style_list));
 }
 Xc_free(top_buffer + region->x_min);

 /*--- Link them to each others ---*/
 Xc_TRACE(("link"));
 for(dest = base; --len > 0; dest++)
 {
  Xc_DUMP_REGION("sorted region", &dest->region);
  dest->Next = dest + 1;
 }
 Xc_DUMP_REGION("sorted region", &dest->region);
  
 dest->Next = NULL;

 return base;
}


/* ----------------------------------------------------------------- ** 
** printMoteur - Print cells                                         ** 
** ----------------------------------------------------------------- */
boolean printMoteur(this, post, region, setup)
c_Moteur *this;
c_PostScript *post;
region_t *region;
setup_print_t *setup;
{
 moteur_overrun_t *overrun_list;
 cell_style_list *area;
 moteur_print_t info;
 char buffer[Xq_FORMULA_LENGTH +1];
 c_col *column;
 bbox_t bbox;
 long i;

 Xc_HISTORY(("ps_print(%d->%d, %d->%d)", region->x_min, region->x_max,
	     region->y_min, region->y_max));

 if(!F(post).pushState(post) || !F(post).putLine(post, "q")) return FALSE;
 info.post = post;

 /*--- Allocate widths & heights array ---*/
 i = region->x_max + region->y_max - region->x_min - region->y_min + 2;
 info.col = Xc_malloc("widths & heights", i * sizeof(rowcol_info_t));
 if(info.col != NULL)
 {
  rowcol_info_t *array;
  cell_size_list *ptr;
  coord_t pos;

  info.col -= region->x_min;
  info.row = info.col + (region->x_max + 1 - region->y_min);
      
  /*--- Get X first position ---*/
  pos = COORD_ZERO;
  for(ptr = this->col_size; ptr->Amax < region->x_min; ptr = ptr->Next)
   pos += ptr->size * (ptr->Amax - ptr->Amin + 1);
      
  pos += ptr->size * (region->x_min - ptr->Amin);
  bbox.llx = pos;

  /*--- Fill columns infos ---*/
  i = region->x_min;
  for(array = info.col + i; i <= region->x_max; i++, array++)
  {
   if(i > ptr->Amax) ptr = ptr->Next;
   Xc_ASSERT((i >= ptr->Amin) && (i <= ptr->Amax));

   array->size = ptr->size;
   array->border = NULL;
   array->pos = pos;
   pos += ptr->size;
  }
  bbox.urx = pos - 1;

  /*--- Get Y first position ---*/
  pos = COORD_ZERO;
  for(ptr = this->line_size; ptr->Amax < region->y_min; ptr = ptr->Next)
   pos += ptr->size * (ptr->Amax - ptr->Amin + 1);
      
  pos += ptr->size * (region->y_min - ptr->Amin);
  bbox.lly = pos;

  /*--- Fill rows infos ---*/
  i = region->y_min;
  for(array = info.row + i; i <= region->y_max; i++, array++)
  {
   if(i > ptr->Amax) ptr = ptr->Next;
   Xc_ASSERT((i >= ptr->Amin) && (i <= ptr->Amax));

   array->size = ptr->size;
   array->border = NULL;
   array->pos = pos;
   pos += ptr->size;
  }
  bbox.ury = pos - 1;

  Xc_DUMP_BBOX("bbox", &bbox);
 }      
 else
  return FALSE;
  
 if((info.areas = F(this).getCellStyleList(this, region)) == NULL)
 {
  Xc_free(info.col + region->x_min);
  return FALSE;
 }

 while(42)
 {
  c_CellStyle *csbase;
  boolean rc;

  csbase = this->BaseStd->cell_style_base;
  rc = TRUE;

  /*--- Print grid ---*/
  if(setup->grid_f != FALSE)
  {
   Xc_TRACE(("print grid"));
	  
   if(!F(post).putLine(post, "q 0 G 0 w [.5 1]0 d")) break;

   /*--- Frame ---*/
   if(post->ai_mode?
      !F(post).putLine(post, "$p m $p L $p L $p L $p L s",
		       bbox.llx, bbox.lly, bbox.urx, bbox.lly,
		       bbox.urx, bbox.ury, bbox.llx, bbox.ury,
		       bbox.llx, bbox.lly) :
      !F(post).putLine(post, "$r re", &bbox)) break;
	  
   /*--- Vertical lines ---*/
   for(i = region->x_min + 1; i <= region->x_max; i++)
    if(!F(post).putLine(post, "$p m $p $s", info.col[i].pos, bbox.lly,
			info.col[i].pos, bbox.ury,
			post->ai_mode? "L S":"l")) break;

   /*--- Horizontal lines ---*/
   for(i = region->y_min + 1; i <= region->y_max; i++)
    if(!F(post).putLine(post, "$p m $p $s", bbox.llx, info.row[i].pos,
			bbox.urx, info.row[i].pos,
			post->ai_mode? "L S":"l")) break;
	  
   /*--- Stroke in black & minimum linewidth ---*/
   if(!F(post).putLine(post, "$sQ", post->ai_mode? "": "S ")) break;
   post->state.stroke_color = NULL;
  }

  /*--- Print cells background ---*/
  Xc_TRACE(("print background"));
  for(area = info.areas; area != NULL; area = area->Next)
  {
   bbox_t area_bbox;

   area_bbox.llx = info.col[area->region.x_min].pos;
   area_bbox.lly = info.row[area->region.y_min].pos;
   area_bbox.urx = (info.col[area->region.x_max].pos +
		    info.col[area->region.x_max].size - 1);
   area_bbox.ury = (info.row[area->region.y_max].pos +
		    info.row[area->region.y_max].size - 1);
   rc = F(csbase).printBackground(area->cstyle, post, &area_bbox);
   if(rc == FALSE) break;
  }
  if(rc == FALSE) break;

  /*--- Print rows & columns titles ---*/
  if(setup->rowcol_f != FALSE)
  {
   coord_t height, width, th, tw, x, y, last_x, last_y;
   c_CharMetrics *metrics;

   Xc_TRACE(("print titles"));

   metrics = setup->title_style->metrics;
   height = setup->title_height;
   width = setup->title_width;
   th = metrics->font_bbox.ury - metrics->font_bbox.lly;
	  
   if(!F(post).putLine(post, "0 G .5 w []0 d")) break;

   /*--- Vertical lines ---*/
   for(i = region->x_min + 1; i <= region->x_max; i++)
    if(!F(post).putLine(post, "$p m $p $s", info.col[i].pos, bbox.lly,
			info.col[i].pos, bbox.lly - height,
			post->ai_mode? "L S":"l")) break;

   /*--- Horizontal lines ---*/
   for(i = region->y_min + 1; i <= region->y_max; i++)
    if(!F(post).putLine(post, "$p m $p $s", bbox.llx, info.row[i].pos,
			bbox.llx - width, info.row[i].pos,
			post->ai_mode? "L S":"l")) break;

   /*--- Frames ---*/
   if(post->ai_mode == FALSE)
   {
    if(!F(post).putLine(post, "$p $v re $p $v re S",
			bbox.llx, bbox.lly,
			bbox.urx - bbox.llx + 1, -height,
			bbox.llx, bbox.lly,
			-width, bbox.ury - bbox.lly + 1)) break;
   }
   else
    if(!F(post).putLine
       (post, "$p m $p L $p L $p L $p L s $p m $p L $p L $p L $p L s",
	bbox.llx, bbox.lly,
	bbox.urx, bbox.lly,
	bbox.urx, bbox.lly - height + 1,
	bbox.llx, bbox.lly - height + 1,
	bbox.llx, bbox.lly,
		
	bbox.llx, bbox.lly,
	bbox.llx - width + 1, bbox.lly,
	bbox.llx - width + 1, bbox.ury,
	bbox.llx, bbox.ury,
	bbox.llx, bbox.lly)) break;
	  
   /*--- Texts ---*/
   Xc_ASSERT(post->state.text_flag == FALSE);
   Xc_ASSERT(post->state.clip_flag == FALSE);
	  
   if(post->ai_mode == FALSE) 
    F(setup->title_style).ps_print(setup->title_style, post);
   last_x = last_y = COORD_ZERO;

   /*--- Rows titles ---*/
   for(i = region->y_min; i <= region->y_max; i++)
   {
    sprintf(buffer, "%ld", i + 1);
    tw = F(metrics).textWidth(metrics, buffer, strlen(buffer));


    y = info.row[i].pos + ((info.row[i].size - th) >> 1)
     + metrics->font_bbox.ury;
    x = bbox.llx - width + ((width - tw) >> 1);

    if(th > info.row[i].size)
    {
     /*--- Need to clip ---*/
     if(post->state.text_flag &&
	(!F(post).putLine(post, post->ai_mode? "TO Q" : "ET Q") ||
	 !F(post).popState(post)))
      break;
		  
     if(!F(post).pushState(post)) break;
     if(post->ai_mode)
     {
      if(!F(post).putLine
	 (post, "q $p m $p L $p L $p L $p L H W n",
	  bbox.llx, info.row[i].pos,
	  bbox.llx - width + 1, info.row[i].pos,
	  bbox.llx - width + 1,
	  info.row[i].pos + info.row[i].size - 1,
	  bbox.llx, info.row[i].pos + info.row[i].size - 1,
	  bbox.llx, info.row[i].pos))
       break;
     }
     else
      if(!F(post).putLine(post, "q $p $v re W n",
			  bbox.llx, info.row[i].pos,
			  -width, info.row[i].size)) break;
		  
     post->state.clip_flag = TRUE;

     F(post->matrix).translate(post->matrix, TRUE, x, y);
     F(post->matrix).scale(post->matrix, FALSE,
			   SCALE_ONE, -SCALE_ONE);
     if(!F(post).putLine(post, post->ai_mode?
			 "0 To $m 0 Tp TP" : "BT $m Tm",
			 F(post->matrix).get(post->matrix, 0)))
      break;

     F(post->matrix).pop(post->matrix);
     post->state.text_flag = TRUE;

     if(post->ai_mode) 
      F(setup->title_style).ps_print(setup->title_style, post);
    }
    else if(post->state.clip_flag || !post->state.text_flag)
    {
     if(post->state.clip_flag &&
	(!F(post).putLine(post, post->ai_mode? "TO Q" : "ET Q") ||
	 !F(post).popState(post)))
      break;
     if(!F(post).pushState(post)) break;
		  
     F(post->matrix).translate(post->matrix, TRUE, x, y);
     F(post->matrix).scale(post->matrix, FALSE,
			   SCALE_ONE, -SCALE_ONE);
     if(!F(post).putLine(post, post->ai_mode?
			 "q 0 To $m 0 Tp TP" : "q BT $m Tm",
			 F(post->matrix).get(post->matrix, 0)))
      break;

     F(post->matrix).pop(post->matrix);
     post->state.text_flag = TRUE;
     post->state.clip_flag = FALSE;

     if(post->ai_mode) 
      F(setup->title_style).ps_print(setup->title_style, post);
    }
    else if((x != last_x) || (y != last_y))
     if(!F(post).putLine(post, "$c $c Td", x - last_x, last_y - y))
      break;

    last_x = x, last_y = y;
	      
    if(!F(post).putString(post, buffer, strlen(buffer)) ||
       !F(post).putLine(post, "Tj")) break;
   }
	  
   /*--- Columns titles ---*/
   for(i = region->x_min; i <= region->x_max; i++)
   {
    char *ptr;
    int col;

    /*--- build string ---*/
    ptr = buffer + 8;
    *ptr = 0;
    col = i;
    do {
	*--ptr = (col % 26) + 'A';
	col /= 26;
       } while(col--);
	      
    tw = F(metrics).textWidth(metrics, ptr, strlen(ptr));
	      
    x = info.col[i].pos + ((info.col[i].size - tw) >> 1);
    y = bbox.lly - height + ((height - th) >> 1)
     + metrics->font_bbox.ury;

    if(tw > info.col[i].size)
    {
     /*--- Need to clip ---*/
     if(post->state.text_flag &&
	(!F(post).putLine(post, post->ai_mode? "TO Q" : "ET Q") ||
	 !F(post).popState(post)))
      break;
		  
     if(!F(post).pushState(post)) break;

     if(post->ai_mode)
     {
      if(!F(post).putLine
	 (post, "q $p m $p L $p L $p L $p L H W n",
	  info.col[i].pos, bbox.lly,
	  info.col[i].pos + info.col[i].size - 1, bbox.lly,
	  info.col[i].pos + info.col[i].size - 1,
	  bbox.lly - height + 1,
	  info.col[i].pos, bbox.lly - height + 1,
	  info.col[i].pos, bbox.lly))
       break;
     }
     else
      if(!F(post).putLine(post, "q $p $v re W n",
			  info.col[i].pos, bbox.lly,
			  info.col[i].size, -height)) break;
     post->state.clip_flag = TRUE;
		  
     F(post->matrix).translate(post->matrix, TRUE, x, y);
     F(post->matrix).scale(post->matrix, FALSE,
			   SCALE_ONE, -SCALE_ONE);
     if(!F(post).putLine(post, post->ai_mode?
			 "0 To $m 0 Tp TP" : "BT $m Tm",
			 F(post->matrix).get(post->matrix, 0)))
      break;
     F(post->matrix).pop(post->matrix);
     post->state.text_flag = TRUE;

     if(post->ai_mode) 
      F(setup->title_style).ps_print(setup->title_style, post);
    }
    else if(post->state.clip_flag || !post->state.text_flag)
    {
     if(post->state.clip_flag &&
	(!F(post).putLine(post, post->ai_mode? "TO Q" : "ET Q") ||
	 !F(post).popState(post)))
      break;

     if(!F(post).pushState(post)) break;

     F(post->matrix).translate(post->matrix, TRUE, x, y);
     F(post->matrix).scale(post->matrix, FALSE,
			   SCALE_ONE, -SCALE_ONE);
     if(!F(post).putLine(post, post->ai_mode?
			 "q 0 To $m 0 Tp TP" : "q BT $m Tm",
			 F(post->matrix).get(post->matrix, 0)))
      break;
     F(post->matrix).pop(post->matrix);
     post->state.text_flag = TRUE;
     post->state.clip_flag = FALSE;

     if(post->ai_mode) 
      F(setup->title_style).ps_print(setup->title_style, post);
    }
    else if((x != last_x) || (y != last_y))
     if(!F(post).putLine(post, "$c $c Td", x - last_x, last_y - y))
      break;
	      
    last_x = x, last_y = y;
	      
    if(!F(post).putString(post, ptr, strlen(ptr)) ||
       !F(post).putLine(post, "Tj")) break;
   }
	  
   F(post).putLine(post, post->ai_mode? "TO Q" : "ET Q");
   F(post).popState(post);
   post->state.clip_flag = post->state.text_flag = FALSE;
   post->state.stroke_color = NULL;
  }
      
  /*--- Print cells borders ---*/
  Xc_TRACE(("print borders"));
  info.w = region->x_max - region->x_min + 1;
  info.h = region->y_max - region->y_min + 1;
  i = (info.w * (info.h + 1)) + ((info.w + 1) * info.h);

  info.varray = Xc_malloc("borders", i * sizeof(CellBorderStyle_t *));
  if(info.varray == NULL) break;
  memset(info.varray, 0, i * sizeof(CellBorderStyle_t *));

  info.harray = info.varray + ((info.w + 1) * info.h);
  info.varray -= region->y_min + (info.h * region->x_min);
  info.harray -= region->x_min + (info.w * region->y_min);

  for(area = info.areas; area != NULL; area = area->Next)
   F(csbase).printAddBorders(area->cstyle, &info, &area->region);

  /*--- Get overrunning cells ---*/
  rc = TRUE;
  if((overrun_list = F(this).getOverrunCells(this, region)) != NULL)
   if((rc = F(post).pushState(post)) == TRUE)
   {
    moteur_overrun_t *ptr;

    rc = (post->ai_mode?
	  F(post).putLine(post, "q $p m $p L $p L $p L $p L H W n",
			  bbox.llx, bbox.lly, bbox.urx, bbox.lly,
			  bbox.urx, bbox.ury, bbox.llx, bbox.ury,
			  bbox.llx, bbox.lly) :
	  F(post).putLine(post, "q $r re W n", &bbox));

    for(ptr = overrun_list; (ptr != NULL) && rc; ptr = ptr->next)
    {
     Xc_TRACE(("overrun cell(%d, %d)", ptr->column, ptr->row));
		
     F(csbase).printRemoveBorder(&info, &ptr->region);

     /*--- Test if clipping needed ---*/
     if(ptr->code & 4)
     {
      bbox_t clipping;

      Xc_TRACE(("clip overrun"));
      if((rc = F(post).pushState(post)) == FALSE) continue;

      F(this).getBoundingBox(this, &ptr->region, &clipping);
      rc = (post->ai_mode?
	    F(post).putLine(post,
			    "q $p m $p L $p L $p L $p L H W n",
			    clipping.llx, clipping.lly,
			    clipping.urx, clipping.lly,
			    clipping.urx, clipping.ury,
			    clipping.llx, clipping.ury,
			    clipping.llx, clipping.lly) :
	    F(post).putLine(post, "q $r re W n", &clipping));
      if(rc == FALSE) continue;
     }

     rc = print_cell(this, post, csbase, &info,
		     ptr->column, ptr->row, ptr->ptr,
		     FALSE, ((ptr->code & 3) == 3)? FALSE : TRUE);

     /*--- Remove clipping if needed ---*/
     if(ptr->code & 4)
     {
      F(post).popState(post);
      rc = F(post).putLine(post, "Q");
     }
    }
	    
    F(post).popState(post);
    if(rc == TRUE) rc = F(post).putLine(post, "Q");
   }
	   
  /*--- Print cell content ---*/
  Xc_TRACE(("print content"));
  for(column = this->tableCL; column && rc; column = column->Next)
  {
   c_line *line;

   if(column->number < region->x_min)
    continue;
   else if(column->number > region->x_max)
    break;

   for(line = column->line; (line != NULL) && rc; line = line->Next)
   {
    moteur_overrun_t *ptr;

    if(line->number < region->y_min)
     continue;
    else if(line->number > region->y_max)
     break;

    for(ptr = overrun_list; ptr != NULL; ptr = ptr->next)
     if(&line->cell == ptr->ptr) break;

    if(ptr == NULL)
     rc = print_cell(this, post, csbase, &info,
		     column->number, line->number,
		     &line->cell, TRUE, FALSE);
   }
  }

  /*--- Free Overruning cells ---*/
  if(overrun_list != NULL) F(this).freeOverrunCells(this, overrun_list);

  if(rc == FALSE)
  {
   F(csbase).printClearBorders(&info, region);
   Xc_free(info.varray + (region->y_min + (info.h * region->x_min)));
   break;
  }

  /*--- Flush cell borders ---*/
  rc = F(csbase).printFlushBorders(&info, post, region);
  F(csbase).printClearBorders(&info, region);
  Xc_free(info.varray + (region->y_min + (info.h * region->x_min)));
  if(rc == FALSE) break;

  /*--- End of printing ---*/
  Xc_TRACE(("print done"));
  Xc_free(info.areas);
  Xc_free(info.col + region->x_min);

  return(F(post).popState(post) && F(post).putLine(post, "Q"));
 }
  
 Xc_free(info.areas);
 Xc_free(info.col + region->x_min);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** print_cell - Print one cell                                       ** 
** ----------------------------------------------------------------- */
static boolean print_cell(this, post, csbase, info, x, y, cp, clip, out)
c_Moteur *this;
c_PostScript *post;
c_CellStyle *csbase;
moteur_print_t *info;
int x;
int y;
cell *cp;
boolean clip;
boolean out;
{
 cell_style_list *area;
 int value_type;
 boolean flag, rc;
 char buffer[Xq_FORMULA_LENGTH +1];
 bbox_t bbox;
  
 Xc_TRACE(("print_cell(%d; %d)", x, y));

 /*--- Test if printable ---*/
 if((clip != FALSE) &&
    ((info->col[x].size <= (XqCELL_MARGIN << 1)) ||
     (info->row[y].size <= (XqCELL_MARGIN << 1)))) return TRUE;
  
 /*--- Get string to print ---*/
 value_type = F(this).get_string(this, cp, buffer, Xq_VALUE);
 flag = Xq_IS_VALUE(value_type);
 Xc_TRACE(("string: `%s'", buffer));
 if(!(*buffer)) return TRUE;
  
 /*--- Get cell style ---*/
 for(area = (flag? info->areas : this->style_list); area; area = area->Next)
  if((area->region.x_min <= x) && (area->region.x_max >= x) &&
     (area->region.y_min <= y) && (area->region.y_max >= y)) break;
 Xc_ASSERT(area != NULL);
  
 /*--- Set up bbox ---*/
 if(!out)
 {
  bbox.llx = bbox.lly = COORD_ZERO;
  bbox.urx = info->col[x].size - 1;
  bbox.ury = info->row[y].size - 1;
  F(post->matrix).translate(post->matrix, TRUE,
			    info->col[x].pos, info->row[y].pos);
  F(csbase).printSetCellMargins(info, &bbox, x, y);
 }
 else
 {
  region_t region;

  Xc_TRACE(("Outside overruning cell"));
  region.x_min = region.x_max = x;
  region.y_min = region.y_max = y;
  F(this).getBoundingBox(this, &region, &bbox);
  F(post->matrix).translate(post->matrix, TRUE, bbox.llx, bbox.lly);
  bbox.urx -= bbox.llx + 1; 
  bbox.ury -= bbox.lly + 1; 
  bbox.llx = bbox.lly = COORD_ZERO;
 }

 /*--- Adjust margin ---*/
 bbox.llx += XqCELL_MARGIN;
 bbox.lly += XqCELL_MARGIN;
 bbox.urx -= XqCELL_MARGIN;
 bbox.ury -= XqCELL_MARGIN;
  
 rc = F(csbase).printText(area->cstyle, post, &bbox, buffer,
			  value_type, clip);
 /*--- Rise [TODO : should be corrected in Push & Pop ---*/
 if(post->state.text_rise != 0)
 {
  post->state.text_rise = 0;
  if(!F(post).putLine(post, "0 Ts")) return FALSE;
 }

 F(post->matrix).pop(post->matrix);
 return rc;
}
