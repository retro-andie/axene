/*
** print.c for XQuad in CellStyle/
** Printing methods for CellStyle class
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
** Started on  Mon Jun 12 19:32:34 1995 Stéphane Boisson
** Last update Thu Jan  6 23:36:12 2000 Emmanuel Paris
*/

#define NHISTORY
#define NTRACE

#define _CELLSTYLE_PATTERN_
#define _CELLSTYLE_DASHES_

#include <varargs.h>
#include "CellStyleP.h"
#include "Moteur.h"
#include "char_translate.h"

#define IS_BORDER_INVISIBLE(border) \
(((border) == NULL) || ((border)->border_stipple_pattern == PATTERN_NONE) || \
 (((border)->border_foreground->transparency != 0) && \
 (((border)->border_background->transparency != 0) || \
 ((border)->border_stipple_pattern == 0) || \
   ((border)->border_stipple_pattern == 1))))

#define IS_OUTLINED_BORDER(border) \
((border != NULL) && (abs(border->border_stipple_pattern) >= 2))

/*--- Clockwise orientations ---*/
#define Xq_TOP 0
#define Xq_RIGHT 1
#define Xq_BOTTOM 2
#define Xq_LEFT 3

/*--- Prototypes ---*/
static CellBorderStyle_t *getVisibleBorder ___PROTO((CellBorderStyle_t *a,
						     CellBorderStyle_t *b));
static boolean drawBorderForeground ___PROTO((c_PostScript *post,
					      CellBorderStyle_t *border,
					      coord_t len,
					      coord_t start, coord_t end));

static boolean drawBorderBackground ___PROTO((c_PostScript *post,
					      CellBorderStyle_t *border,
					      coord_t len,
					      coord_t start, coord_t end));
___INLINE boolean compareBorderBackground ___PROTO((CellBorderStyle_t *a,
						    CellBorderStyle_t *b));
___INLINE boolean compareBorderForeground ___PROTO((CellBorderStyle_t *a,
						    CellBorderStyle_t *b));
___INLINE coord_t adjustEnd ___PROTO((moteur_print_t *info, int orientation,
				      int x, int y, region_t *region));
static boolean compBorder ___PROTO((CellBorderStyle_t *a,
				    CellBorderStyle_t *b));		   
static boolean set_dash ___PROTO((c_PostScript *post,
				  CellBorderStyle_t *border));

#define STROKE_HLINE(post, width, x, y, wd) \
(((width) == (post)->state.line_width)? \
 F(post).putLine(post, "$p m $p l S", (x), (y), (x) + (wd), (y)) : \
 F(post).putLine(post, "$c w $p m $p l S", (post)->state.line_width = (width),\
		 (x), (y), (x) + (wd), (y)))

/* ----------------------------------------------------------------- ** 
** printCSSetCellMargins - Set bbox margin                           ** 
** ----------------------------------------------------------------- */
void printCSSetCellMargins(info, bbox, x, y)
moteur_print_t *info;
bbox_t *bbox;
int x;
int y;
{
 CellBorderStyle_t *border;
 int i;

 Xc_HISTORY(("printSetCellMargins(%d, %d)", x, y));

 /*--- Left margin ---*/
 border = (info->varray + (x * info->h))[y];
 i = (border != NULL)? ABS(border->border_stipple_pattern) : PATTERN_NONE;
 if(i != PATTERN_NONE) bbox->llx += (Cell_Pattern[i].height * COORD_ONE) >> 1;
 Xc_TRACE(("llx=%d", (int)Cell_Pattern[i].height));

 /*--- Right margin ---*/
 border = (info->varray + ((x + 1) * info->h))[y];
 i = (border != NULL)? ABS(border->border_stipple_pattern) : PATTERN_NONE;
 if(i != PATTERN_NONE) bbox->urx -= (Cell_Pattern[i].height * COORD_ONE) >> 1;
 Xc_TRACE(("urx=%d", (int)Cell_Pattern[i].height));
  
 /*--- Top margin ---*/
 border = (info->harray + (y * info->w))[x];
 i = (border != NULL)? ABS(border->border_stipple_pattern) : PATTERN_NONE;
 if(i != PATTERN_NONE) bbox->lly += (Cell_Pattern[i].height * COORD_ONE) >> 1;
 Xc_TRACE(("lly=%d", (int)Cell_Pattern[i].height));
  
 /*--- Bottom margin ---*/
 border = (info->harray + ((y + 1) * info->w))[x];
 i = (border != NULL)? ABS(border->border_stipple_pattern) : PATTERN_NONE;
 if(i != PATTERN_NONE) bbox->ury -= (Cell_Pattern[i].height * COORD_ONE) >> 1;
 Xc_TRACE(("ury=%d", (int)Cell_Pattern[i].height));
}


/* ----------------------------------------------------------------- ** 
** set_dash - Set border dash                                        ** 
** ----------------------------------------------------------------- */
static boolean set_dash(post, border)
c_PostScript *post;
CellBorderStyle_t *border;
{
 boolean flag;
 int dash, i;

 if(border->border_dash_pattern == PATTERN_NONE)
 {
  if(post->state.dash_len != 0)
  {
   post->state.dash_len = 0;
   return F(post).putLine(post, "[]0 d");
  }
 }
 else
 {
  dash = border->border_dash_pattern;

  if(post->state.dash_len == Cell_Dashes[dash].dash_length)
  {
   for(i = 0; i < dash; i++)
    if((Cell_Dashes[dash].dash_list[i] * COORD_ONE) !=
       post->state.dash[i]) break;
	  
   flag = (i == dash)? TRUE : FALSE;
  }
  else 
   flag = FALSE;

  if(flag == FALSE)
  {
   for(i = 0; i < dash; i++)
    post->state.dash[i] = Cell_Dashes[dash].dash_list[i] * COORD_ONE;
   post->state.dash_len = Cell_Dashes[dash].dash_length;
	  
   if(dash == 3)
    return F(post).putLine(post, "[$c $c $c $c]0 d",
			   post->state.dash[0], post->state.dash[1], 
			   post->state.dash[2], post->state.dash[3]);
   else
    return F(post).putLine(post, "[$c $c]0 d",
			   post->state.dash[0], post->state.dash[1]);
  }
 }
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** compBorder - Compare Borders                                      ** 
** ----------------------------------------------------------------- */
static boolean compBorder(a, b)
CellBorderStyle_t *a;
CellBorderStyle_t *b;
{
 if(a == b) return TRUE;
 if((a == NULL) || (b == NULL)) return FALSE;

 return((abs(a->border_stipple_pattern) == abs(b->border_stipple_pattern)) &&
	((a->border_stipple_pattern == PATTERN_NONE) ||
	 ((a->border_dash_pattern == b->border_dash_pattern) &&
	  (a->border_background == b->border_background) &&
	  (a->border_foreground == b->border_foreground))));
}


/* ----------------------------------------------------------------- ** 
** adjustEnd - Adjust end of border                                  ** 
** ----------------------------------------------------------------- */
___INLINE coord_t adjustEnd(info, orientation, x, y, region)
moteur_print_t *info;
int orientation;
int x;
int y;
region_t *region;
{
 CellBorderStyle_t *leftp, *rightp, *topp, *bottomp;
 int left, right, top, bottom, i;

 Xc_TRACE(("adjustEnd(%d, %d)", x, y));

 /*--- Horizontal ---*/
 rightp = (x > region->x_max)? NULL : info->harray[(y * info->w) + x];
 leftp = (x <= region->x_min)? NULL : info->harray[(y * info->w) + x - 1];
  
 /*--- Vertical ---*/
 bottomp = (y > region->y_max)? NULL : info->varray[(x * info->h) + y];
 topp = (y <= region->y_min)? NULL : info->varray[(x * info->h) + y];

 /*--- Rotate ---*/
 Xc_TRACE(("top=%lx bottom=%lx left=%lx right=%lx",
	   topp, bottomp, leftp, rightp));

 Xc_TRACE(("orientation %d", orientation));
 for(i = orientation; i != Xq_TOP; i--)
 {
  CellBorderStyle_t *tmp;

  tmp = topp;
  topp = rightp;
  rightp = bottomp;
  bottomp = leftp;
  leftp = tmp;
 }
 Xc_TRACE(("top=%lx bottom=%lx left=%lx right=%lx",
	   topp, bottomp, leftp, rightp));

 if(topp == NULL) return COORD_ZERO;
 top = abs(topp->border_stipple_pattern);
 Xc_ASSERT(top != PATTERN_NONE);

 bottom = ((bottomp != NULL)?
	   Cell_Pattern[abs(bottomp->border_stipple_pattern)].height : 0);
 right = ((rightp != NULL)?
	  Cell_Pattern[abs(rightp->border_stipple_pattern)].height : 0);
 left = ((leftp != NULL)?
	 Cell_Pattern[abs(leftp->border_stipple_pattern)].height : 0);
 top = Cell_Pattern[top].height;

 if(!bottom)
 {
  /*--- Not straight ---*/
  if(left == right)
  {
   if(left)
   {
    /*--- 'T' ---*/
    if(compBorder(leftp, rightp))
    {
     if(compBorder(topp, rightp))
      return((top * COORD_ONE) >> 1);
     else if((IS_OUTLINED_BORDER(rightp) ||
	      IS_OUTLINED_BORDER(leftp)) &&
	     IS_OUTLINED_BORDER(topp)) return COORD_ZERO; 
    }
    return((-left * COORD_ONE) >> 1);
   }
  }
  else
  {
   if(!left)
   {
    /*--- Right corner ---*/
    left = right;
    leftp = rightp;
    right = 0;
    rightp = NULL;
   }

   if(!right)
   {
    /*--- Left corner ---*/
    if(compBorder(topp, leftp))
     return((top * COORD_ONE) >> 1);
    else if((top < left) ||
	    (IS_OUTLINED_BORDER(leftp) && IS_OUTLINED_BORDER(topp)))
     return((left * COORD_ONE) >> 1);
    else if(top > left)
     return((-left * COORD_ONE) >> 1);
   }
  }
 }
 else
 {
  /*Xc_ASSERT(compBorder(topp, bottomp) == FALSE);*/

  if((left == right) && (left != 0))
  {
   if(compBorder(leftp, rightp) && compBorder(topp, rightp))
    return((top * COORD_ONE) >> 1);
   return((-left * COORD_ONE) >> 1);
  }

  if(compBorder(topp, rightp))
  {
   if(compBorder(bottomp, leftp))
   {
    if(bottom < top)
     return((top * COORD_ONE) >> 1);
    else if(bottom > top)
     return((-left * COORD_ONE) >> 1);
   }
   else if((top >= bottom) && (top >= left))
    return((top * COORD_ONE) >> 1);
  }
  else if(compBorder(topp, leftp))
  {
   if(compBorder(bottomp, rightp))
   {
    if(bottom < top)
     return((top * COORD_ONE) >> 1);
    else if(bottom > top)
     return((-right * COORD_ONE) >> 1);
   }
   else if((top >= bottom) && (top >= right))
    return((top * COORD_ONE) >> 1);
  }
 }
 return COORD_ZERO;
}


/* ----------------------------------------------------------------- ** 
** compareBorderBackground - Compare two borders                     ** 
** ----------------------------------------------------------------- */
___INLINE boolean compareBorderBackground(a, b)
CellBorderStyle_t *a;
CellBorderStyle_t *b;
{
 int sa, sb;

 if(a == b)
  return TRUE;
 else if(a == NULL)
 {
  a = b;
  b = NULL;
 }
  
 sa = a->border_stipple_pattern;
 if(a->border_background->transparency || (sa == 0) || (sa == 1))
  sa = PATTERN_NONE;

 if(b != NULL)
 {
  sb = b->border_stipple_pattern;
  if(b->border_background->transparency || (sb == 0) || (sb == 1))
   sb = PATTERN_NONE;
 }
 else
  sb = PATTERN_NONE;
  
 if(sa != sb)
  return FALSE;
 else if(sa == PATTERN_NONE)
  return TRUE;

 Xc_ASSERT(a != NULL);
 Xc_ASSERT(b != NULL);

 if(!F(a->border_background).compare
    (a->border_background, b->border_background, CMP_WITH_NAME))
  return FALSE;
  
 return (a->border_dash_pattern == b->border_dash_pattern)? TRUE : FALSE;
}


/* ----------------------------------------------------------------- ** 
** compareBorderForeground - Compare two borders                     ** 
** ----------------------------------------------------------------- */
___INLINE boolean compareBorderForeground(a, b)
CellBorderStyle_t *a;
CellBorderStyle_t *b;
{
 int sa, sb;

 if(a == b)
  return TRUE;
 else if(a == NULL)
 {
  a = b;
  b = NULL;
 }
  
 sa = a->border_stipple_pattern;
 if(a->border_foreground->transparency) sa = PATTERN_NONE;

 if(b != NULL)
 {
  sb = b->border_stipple_pattern;
  if(b->border_foreground->transparency) sb = PATTERN_NONE;
 }
 else
  sb = PATTERN_NONE;
  
 if(sa != sb)
  return FALSE;
 else if(sa == PATTERN_NONE)
  return TRUE;

 Xc_ASSERT(a != NULL);
 Xc_ASSERT(b != NULL);

 if(!F(a->border_foreground).compare
    (a->border_foreground, b->border_foreground, CMP_WITH_NAME))
  return FALSE;

 return (a->border_dash_pattern == b->border_dash_pattern)? TRUE : FALSE;
}


/* ----------------------------------------------------------------- ** 
** getVisibleBorder - Get real border from two adjacent border       ** 
** ----------------------------------------------------------------- */
static CellBorderStyle_t *getVisibleBorder(a, b)
CellBorderStyle_t *a;
CellBorderStyle_t *b;
{
 int ap, bp;

 Xc_TRACE(("getVisibleBorder(%lx, %lx)", (long)a, (long)b));

 ap = (a == NULL)? PATTERN_NONE : a->border_stipple_pattern;
 if(ap < -2) ap = -ap;
 Xc_ASSERT((ap == PATTERN_NONE) || (ap < CELL_PATTERN_DATA_FIRST));

 bp = (b == NULL)? PATTERN_NONE : b->border_stipple_pattern;
 if(bp < -2) bp = -bp;
 Xc_ASSERT((bp == PATTERN_NONE) || (bp < CELL_PATTERN_DATA_FIRST));

 /*Xc_TRACE(("  %d/%d (%d)", ap, bp, PATTERN_NONE));*/
#if 0
 return((ap == PATTERN_NONE)? 
	((bp == PATTERN_NONE)? NULL : b) :
	((bp == PATTERN_NONE)? a : 
	 ((Cell_Pattern[ap].height >= Cell_Pattern[bp].height)? a : b)));
#else
 if(ap == PATTERN_NONE)
 {
  if(bp == PATTERN_NONE)
   return NULL;
  else
   return b;
 }
 else
 {
  if(bp == PATTERN_NONE)
   return a;
  else if(Cell_Pattern[ap].height >= Cell_Pattern[bp].height)
   return a;
  else
   return b;
 }
#endif
}


/* ----------------------------------------------------------------- ** 
** drawBorderBackground - Draw border background                     ** 
** ----------------------------------------------------------------- */
static boolean drawBorderBackground(post, border, len, start, end)
c_PostScript *post;
CellBorderStyle_t *border;
coord_t len;
coord_t start;
coord_t end;
{
 coord_t cap;
 int type;
  
 Xc_TRACE(("drawBorderBackground(%lx len=%g start=%g end=%g",
	   (long)border, SCALE_TO_POINTS(len),
	   SCALE_TO_POINTS(start),SCALE_TO_POINTS(end)));
  
 Xc_ASSERT(border->border_stipple_pattern != PATTERN_NONE);

 type = abs(border->border_stipple_pattern);
 if((border->border_background->transparency != 0) || (type < 2)) return TRUE;
  
 if(!F(border->border_background).ps_print(border->border_background,
					   post, TRUE)) return FALSE;

 if(!set_dash(post, border)) return FALSE;

 if((type == 4) || (type >= 8)) 
  cap = COORD_ONE << 1;
 else if(type == 7)
  cap = COORD_ZERO;
 else
  cap = COORD_ONE;

 if(start > COORD_ZERO) start -= cap;
 if(end > COORD_ZERO) end -= cap;
  
 len += end + start;

 if(type <= 3)
  return STROKE_HLINE(post, COORD_ONE * (type - 1), -start, COORD_ZERO, len);
  
 switch(border->border_stipple_pattern)
 {
  /*--- Simple line outlined outside-shadowed ---*/
 case -5:
 case 4:
 case 6:
  return STROKE_HLINE(post, COORD_ONE, -start, COORD_ONE >> 1, len);

  /*--- Simple line outlined inside-shadowed ---*/
 case -4:
 case -6:
 case 5:
  return STROKE_HLINE(post, COORD_ONE, -start, -COORD_ONE >> 1, len);

  /*--- Double line double outside-shadowed ---*/
 case -8:
 case 7:
  return STROKE_HLINE(post, COORD_ONE, -start, -COORD_ONE, len);

  /*--- Double line double inside-shadowed ---*/
 case -7:
 case 8:
  return STROKE_HLINE(post, COORD_ONE, -start, COORD_ONE, len);

  /*--- Double line double shadowed ---*/
 case -9:
  return STROKE_HLINE(post, COORD_ONE << 1, -start, -COORD_ONE, len);
 case 9:
  return STROKE_HLINE(post, COORD_ONE << 1, -start, COORD_ONE, len);
 }
 Xc_BREAK(("Oops"));
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** drawBorderForeground - Draw border foreground                     ** 
** ----------------------------------------------------------------- */
static boolean drawBorderForeground(post, border, len, start, end)
c_PostScript *post;
CellBorderStyle_t *border;
coord_t len;
coord_t start;
coord_t end;
{
 boolean rc;
 int type;

 Xc_TRACE(("drawBorderForeground(%lx len=%g start=%g end=%g, border=%lx)",
	   (long)border, SCALE_TO_POINTS(len),
	   SCALE_TO_POINTS(start), SCALE_TO_POINTS(end), (long)border));
  
 Xc_ASSERT(border != NULL);
 Xc_ASSERT(border->border_stipple_pattern != PATTERN_NONE);

 if(border->border_foreground->transparency != 0) return TRUE;

 rc = TRUE;
 if(!F(border->border_foreground).ps_print(border->border_foreground,
					   post, TRUE)) return FALSE;
  
 if(!set_dash(post, border)) return FALSE;
  
 type = abs(border->border_stipple_pattern);

 len += end + start;

 if((border->border_background->transparency == 0) || (type <= 1))
  return STROKE_HLINE(post, Cell_Pattern[type].height * COORD_ONE,
		      -start, COORD_ZERO, len);
  
 switch(border->border_stipple_pattern)
 {
  /*--- Simple line outlined ---*/
 case -2:
 case 2:
  rc = STROKE_HLINE(post, COORD_ONE, COORD_ZERO, COORD_ZERO, len);
  rc = STROKE_HLINE(post, COORD_ONE, COORD_ZERO, COORD_ZERO, len);
  break;

  /*--- Double line outlined ---*/
 case -3:
 case 3:
  rc = STROKE_HLINE(post, COORD_ONE << 2, COORD_ZERO, COORD_ZERO, len);
  break;

  /*--- Simple line outlined outside-shadowed ---*/
 case -5:
 case 4:
  rc = STROKE_HLINE(post, COORD_ONE << 2, COORD_ZERO, COORD_ZERO, len);
  break;

  /*--- Simple line outlined inside-shadowed ---*/
 case -4:
 case 5:
  rc = STROKE_HLINE(post, COORD_ONE << 2, COORD_ZERO, COORD_ZERO, len);
  break;

  /*--- Simple line outside-shadowed ---*/
 case -6:
  rc = STROKE_HLINE(post, COORD_ONE << 1, COORD_ZERO, COORD_ZERO, len);
  break;
 case 6:
  rc = STROKE_HLINE(post, COORD_ONE << 1, COORD_ZERO, COORD_ZERO, len);
  break;

  /*--- Double line double outside-shadowed ---*/
 case -8:
 case 7:
  rc = STROKE_HLINE(post, COORD_ONE * 3, COORD_ZERO, COORD_ZERO, len);
  break;

  /*--- Double line double inside-shadowed ---*/
 case -7:
 case 8:
  rc = STROKE_HLINE(post, COORD_ONE * 3, COORD_ZERO, COORD_ZERO, len);
  break;

  /*--- Double line double shadowed ---*/
 case -9:
  rc = STROKE_HLINE(post, COORD_ONE << 2, COORD_ZERO, COORD_ZERO, len);
  break;
 case 9:
  rc = STROKE_HLINE(post, COORD_ONE << 2, COORD_ZERO, COORD_ZERO, len);
  break;
 }
 return rc;
}


/* ----------------------------------------------------------------- ** 
** printCSBackground - Print CellStyle background                    ** 
** ----------------------------------------------------------------- */
boolean printCSBackground(this, post, bbox)
CellStyle_t *this;
c_PostScript *post;
bbox_t *bbox;
{
 Xc_HISTORY(("printBackground"));
 Xc_DUMP_BBOX("region", bbox);

 if(this->inside_stipple_pattern == PATTERN_NONE) return TRUE;
 Xc_ASSERT(this->inside_stipple_pattern >= CELL_PATTERN_DATA_FIRST);

 if(this->inside_stipple_pattern == CELL_PATTERN_DATA_FIRST)
 {
  /*--- No pattern ---*/
  if(this->inside_foreground->transparency != 0) return TRUE;
  if(!F(this->inside_background).ps_print(this->inside_foreground,
					  post, FALSE)) return FALSE;
  if(post->ai_mode?
     !F(post).putLine(post, "$p m $p L $p L $p L $p L",
		      bbox->llx, bbox->lly, bbox->urx, bbox->lly,
		      bbox->urx, bbox->ury, bbox->llx, bbox->ury,
		      bbox->llx, bbox->lly) :
     !F(post).putLine(post, "$r re", bbox)) return FALSE;
  return F(post).putLine(post, "f");
 }
  
 if(this->inside_background->transparency == 0)
  if(!F(this->inside_background).ps_print(this->inside_background,
					  post, FALSE)) return FALSE;
  
 if(post->ai_mode?
    !F(post).putLine(post, "q $p m $p L $p L $p L $p L H",
		     bbox->llx, bbox->lly, bbox->urx, bbox->lly,
		     bbox->urx, bbox->ury, bbox->llx, bbox->ury,
		     bbox->llx, bbox->lly) :
    !F(post).putLine(post, "q $r re", bbox)) return FALSE;
  
 if(this->inside_background->transparency == 0)
  if(!F(post).putLine(post, "q f Q")) return FALSE;

 if(!F(post).putLine(post, "W n")) return FALSE;
  
 return F(post).putLine(post, "Q");
}


/* ----------------------------------------------------------------- ** 
** printCSAddBorders - Print CellStyle borders                       ** 
** ----------------------------------------------------------------- */
void printCSAddBorders(this, info, region)
CellStyle_t *this;
moteur_print_t *info;
region_t *region;
{
 CellBorderStyle_t **ptr, *border;
 rowcol_info_t *col, *row;
 int x, y;

 Xc_HISTORY(("printAddBorders(%lx)", (long)this));
 Xc_DUMP_REGION("region", region);
  
 /*--- Vertical border inside region ---*/
 if(region->x_min != region->x_max)
 {
  border = getVisibleBorder(&this->right, &this->left);
  if(!IS_BORDER_INVISIBLE(border))
  {
   if((border == &this->left) && (border->border_stipple_pattern > 3))
    border->border_stipple_pattern = -border->border_stipple_pattern;
  }
  else
   border = NULL;
  Xc_TRACE(("V-Border inside: %lx", (long)border));
      
  for(x = region->x_min + 1; x <= region->x_max; x++)
  {
   ptr = info->varray + ((x * info->h) + region->y_min);
   for(y = region->y_min; y <= region->y_max; y++)
    *ptr++ = border;
  }
 }

 /*--- Horizontal border inside region ---*/
 if(region->y_min != region->y_max)
 {
  border = getVisibleBorder(&this->down, &this->up);
  if(!IS_BORDER_INVISIBLE(border))
  {
   if((border == &this->down) && (border->border_stipple_pattern > 3))
    border->border_stipple_pattern = -border->border_stipple_pattern;
  }
  else
   border = NULL;
  /*Xc_TRACE(("H-Border inside: %lx", (long)border));*/
      
  for(y = region->y_min + 1; y <= region->y_max; y++)
  {
   ptr = info->harray + ((y * info->w) + region->x_min);
   for(x = region->x_min; x <= region->x_max; x++)
    *ptr++ = border;
  }
 }

 /*--- Left border ---*/
 Xc_TRACE(("left border"));
 ptr = info->varray + ((region->x_min * info->h) + region->y_min);
 row = info->row + region->y_min;
 for(y = region->y_min; y <= region->y_max; y++, row++)
 {
  border = getVisibleBorder(row->border, &this->left);
  Xc_TRACE(("(%d; %d): %lx", region->x_min, y, (long)border));

  if(!IS_BORDER_INVISIBLE(border))
  {
   Xc_TRACE(("visible"));
   if((border == &this->left) && (border->border_stipple_pattern > 3))
    border->border_stipple_pattern = -border->border_stipple_pattern;
   *ptr++ = border;
  }
  else
   *ptr++ = NULL;

  border = &this->right;
  if(border != NULL)
  {
   Xc_TRACE(("  right border: %lx", (long)border));
   Xc_TRACE(("  pattern: %d", (int)border->border_stipple_pattern));
   Xc_TRACE(("  foreground transparency %d",
	     (int)border->border_foreground->transparency));
   Xc_TRACE(("  background transparency %d",
	     (int)border->border_background->transparency));
  }

  row->border = &this->right;
 }
  
 /*--- Top border ---*/
 ptr = info->harray + ((region->y_min * info->w) + region->x_min);
 col = info->col + region->x_min;
 for(x = region->x_min; x <= region->x_max; x++, col++)
 {
  border = getVisibleBorder(col->border, &this->up);
  if(!IS_BORDER_INVISIBLE(border))
  {
   if((border == col->border) && (border->border_stipple_pattern > 3))
    border->border_stipple_pattern = -border->border_stipple_pattern;
   *ptr++ = border;
  }
  else
   *ptr++ = NULL;

  col->border = &this->down;
 }
}


/* ----------------------------------------------------------------- ** 
** printCSFlushBorders - Print CellStyle borders                     ** 
** ----------------------------------------------------------------- */
boolean printCSFlushBorders(info, post, region)
moteur_print_t *info;
c_PostScript *post;
region_t *region;
{
 CellBorderStyle_t **ptr;
 coord_t len, pos, s, e;
 int x, y, last;
	
 Xc_HISTORY(("printFlushBorders"));
 Xc_DUMP_REGION("region", region);

 /*--- Print region right border ---*/
 ptr = info->varray + (((region->x_max + 1) * info->h) + region->y_min);
 for(y = region->y_min; y <= region->y_max; y++)
  *ptr++ = (IS_BORDER_INVISIBLE(info->row[y].border)?
	    NULL : info->row[y].border);
  
 /*--- Print region bottom border ---*/
 ptr = info->harray + (((region->y_max + 1) * info->w) + region->x_min);
 for(x = region->x_min; x <= region->x_max; x++)
  *ptr++ = (IS_BORDER_INVISIBLE(info->col[x].border)?
	    NULL : info->col[x].border);
  
 /*--- Print vertical borders foreground ---*/
 Xc_TRACE(("vertical borders foreground"));
 for(x = region->x_min; x <= (region->x_max + 1); x++)
 {
  ptr = info->varray + (x * info->h);
  last = region->y_min;
  len = info->row[last].size;
  pos = ((x <= region->x_max)?
	 info->col[x].pos :
	 (info->col[x - 1].pos + info->col[x - 1].size));
      
  for(y = last + 1; y <= region->y_max; y++)
   if(!compareBorderForeground(ptr[last], ptr[y]))
   {
    Xc_TRACE(("v-border=%lx (%d; %d) -> (%d; %d)", ptr[last],
	      x, last, x, y));
    if(ptr[last] != NULL)
    {
     /*Xc_TRACE(("v-border=%lx", ptr[last]));*/
     s = adjustEnd(info, Xq_BOTTOM, x, last, region);
     e = adjustEnd(info, Xq_TOP, x, y, region);
		
     F(post->matrix).translate(post->matrix, TRUE,
			       pos, info->row[last].pos);
     F(post->matrix).rotate(post->matrix, FALSE,
			    SCALE_FROM_DEGREES(90.0));
     if(!drawBorderForeground(post, ptr[last], len, s, e))
      return FALSE;
     F(post->matrix).pop(post->matrix);
    }
    last = y;
    len = info->row[last].size;
   }
   else
    len += info->row[y].size;
      
  Xc_TRACE(("v-border=%lx (%d; %d) -> (%d; %d)", ptr[last],
	    x, last, x, y));
  if(ptr[last] != NULL)
  {
   /*Xc_TRACE(("v-border=%lx", ptr[last]));*/
   s = adjustEnd(info, Xq_BOTTOM, x, last, region);
   e = adjustEnd(info, Xq_TOP, x, y, region);

   F(post->matrix).translate(post->matrix, TRUE,
			     pos, info->row[last].pos);
   F(post->matrix).rotate(post->matrix, FALSE,SCALE_FROM_DEGREES(90.0));
   if(!drawBorderForeground(post, ptr[last], len, s, e)) return FALSE;
   F(post->matrix).pop(post->matrix);
  }
 }
  
 /*--- Print horizontal borders foreground ---*/
 Xc_TRACE(("horizontal borders foreground"));
 for(y = region->y_min; y <= (region->y_max + 1); y++)
 {
  ptr = info->harray + (y * info->w);
  last = region->x_min;
  len = info->col[last].size;
  pos = ((y <= region->y_max)?
	 info->row[y].pos :
	 (info->row[y - 1].pos + info->row[y - 1].size));
      
  for(x = last + 1; x <= region->x_max; x++)
   if(!compareBorderForeground(ptr[last], ptr[x]))
   {
    Xc_TRACE(("h-border=%lx (%d; %d) -> (%d; %d)", ptr[last],
	      last, y, x, y));
    if(ptr[last] != NULL)
    {
     /*Xc_TRACE(("h-border=%lx", ptr[last]));*/
     s = adjustEnd(info, Xq_RIGHT, last, y, region);
     e = adjustEnd(info, Xq_LEFT, x, y, region);

     F(post->matrix).translate(post->matrix, TRUE,
			       info->col[last].pos, pos);
     if(!drawBorderForeground(post, ptr[last], len, s ,e))
      return FALSE;
     F(post->matrix).pop(post->matrix);
    }
    last = x;
    len = info->col[last].size;
   }
   else
    len += info->col[x].size;
      
  Xc_TRACE(("h-border=%lx (%d; %d) -> (%d; %d)", ptr[last],
	    last, y, x, y));
  if(ptr[last] != NULL)
  {
   /*Xc_TRACE(("h-border=%lx", ptr[last]));*/
   s = adjustEnd(info, Xq_RIGHT, last, y, region);
   e = adjustEnd(info, Xq_LEFT, x, y, region);

   F(post->matrix).translate(post->matrix, TRUE,
			     info->col[last].pos, pos);
   if(!drawBorderForeground(post, ptr[last], len, s, e)) return FALSE;
   F(post->matrix).pop(post->matrix);
  }
 }
  
 /*--- Print vertical borders background ---*/
 Xc_TRACE(("vertical borders background"));
 for(x = region->x_min; x <= (region->x_max + 1); x++)
 {
  ptr = info->varray + (x * info->h);
  last = region->y_min;
  len = info->row[last].size;
  pos = ((x <= region->x_max)?
	 info->col[x].pos :
	 (info->col[x - 1].pos + info->col[x - 1].size));
      
  for(y = last + 1; y <= region->y_max; y++)
   if(!compareBorderBackground(ptr[last], ptr[y]))
   {
    if(ptr[last] != NULL)
    {
     Xc_TRACE(("border=%lx", ptr[last]));
     s = adjustEnd(info, Xq_BOTTOM, x, last, region);
     e = adjustEnd(info, Xq_TOP, x, y, region);

     F(post->matrix).translate(post->matrix, TRUE,
			       pos, info->row[last].pos);
     F(post->matrix).rotate(post->matrix, FALSE,
			    SCALE_FROM_DEGREES(90.0));
     if(!drawBorderBackground(post, ptr[last], len, s, e))
      return FALSE;
     F(post->matrix).pop(post->matrix);
    }
    last = y;
    len = info->row[last].size;
   }
   else
    len += info->row[y].size;
      
  if(ptr[last] != NULL)
  {
   Xc_TRACE(("border=%lx", ptr[last]));
   s = adjustEnd(info, Xq_BOTTOM, x, last, region);
   e = adjustEnd(info, Xq_TOP, x, y, region);

   F(post->matrix).translate(post->matrix, TRUE,
			     pos, info->row[last].pos);
   F(post->matrix).rotate(post->matrix, FALSE,SCALE_FROM_DEGREES(90.0));
   if(!drawBorderBackground(post, ptr[last], len, s, e)) return FALSE;
   F(post->matrix).pop(post->matrix);
  }
 }
  
 /*--- Print horizontal borders background ---*/
 Xc_TRACE(("horizontal borders background"));
 for(y = region->y_min; y <= (region->y_max + 1); y++)
 {
  ptr = info->harray + (y * info->w);
  last = region->x_min;
  len = info->col[last].size;
  pos = ((y <= region->y_max)?
	 info->row[y].pos :
	 (info->row[y - 1].pos + info->row[y - 1].size));
      
  for(x = last + 1; x <= region->x_max; x++)
   if(!compareBorderBackground(ptr[last], ptr[x]))
   {
    if(ptr[last] != NULL)
    {
     Xc_TRACE(("border=%lx", ptr[last]));
     s = adjustEnd(info, Xq_RIGHT, last, y, region);
     e = adjustEnd(info, Xq_LEFT, x, y, region);

     F(post->matrix).translate(post->matrix, TRUE,
			       info->col[last].pos, pos);
     if(!drawBorderBackground(post, ptr[last], len, s, e))
      return FALSE;
     F(post->matrix).pop(post->matrix);
    }
    last = x;
    len = info->col[last].size;
   }
   else
    len += info->col[x].size;
      
  if(ptr[last] != NULL)
  {
   Xc_TRACE(("border=%lx", ptr[last]));
   s = adjustEnd(info, Xq_RIGHT, last, y, region);
   e = adjustEnd(info, Xq_LEFT, x, y, region);

   F(post->matrix).translate(post->matrix, TRUE,
			     info->col[last].pos, pos);
   if(!drawBorderBackground(post, ptr[last], len, s, e)) return FALSE;
   F(post->matrix).pop(post->matrix);
  }
 }
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** printCSRemoveBorder - Remove border inside region                 ** 
** ----------------------------------------------------------------- */
void printCSRemoveBorder(info, region)
moteur_print_t *info;
region_t *region;
{
 CellBorderStyle_t **ptr;
 int x, y;

 Xc_HISTORY(("printRemoveBorder"));
 Xc_DUMP_REGION("region", region);

 /*--- Vertical borders ---*/
 if(region->x_min < region->x_max)
 {
  for(x = region->x_min + 1; x <= region->x_max; x++)
  {
   ptr = info->varray + (x * info->h);
	  
   for(y = region->y_min; y <= region->y_max; y++) ptr[y] = NULL;
  }
 }

 /*--- Horizontal borders ---*/
 if(region->y_min < region->y_max)
 {
  for(y = region->y_min + 1; y <= region->y_max; y++)
  {
   ptr = info->harray + (y * info->w);
	  
   for(x = region->x_min; x <= region->x_max; x++) ptr[x] = NULL;
  }
 }

 Xc_TRACE(("done"));
}


/* ----------------------------------------------------------------- ** 
** printCSClearBorders - Clear borders data                          ** 
** ----------------------------------------------------------------- */
void printCSClearBorders(info, region)
moteur_print_t *info;
region_t *region;
{
 CellBorderStyle_t **ptr;
 int x, y;

 Xc_HISTORY(("printClearBorders"));
 Xc_DUMP_REGION("region", region);

 for(x = region->x_min; x <= (region->x_max + 1); x++)
 {
  ptr = info->varray + (x * info->h);
      
  for(y = region->y_min; y <= region->y_max; y++)
   if((ptr[y] != NULL ) && (ptr[y]->border_stipple_pattern < -2))
    ptr[y]->border_stipple_pattern = -ptr[y]->border_stipple_pattern;
 }
  
 for(y = region->y_min; y <= (region->y_max + 1); y++)
 {
  ptr = info->harray + (y * info->w);

  for(x = region->x_min; x <= region->x_max; x++)
   if((ptr[x] != NULL ) && (ptr[x]->border_stipple_pattern < -2))
    ptr[x]->border_stipple_pattern = -ptr[x]->border_stipple_pattern;
 }
}


/* ----------------------------------------------------------------- ** 
** printCSText - Print CellStyle Text                                ** 
** ----------------------------------------------------------------- */
boolean printCSText(this, post, bbox, text, value_type, clip_flag)
CellStyle_t *this;
c_PostScript *post;
bbox_t *bbox;
char *text;
int value_type;
boolean clip_flag;
{
 coord_t width, height, x, y, ws, cs, y_inc, x0;
 boolean value_flag, init_flag, clip2_flag;
 c_CharMetrics *metrics;
 c_TextStyle *style;
 long len, max_len;
 char *ptr, *end;
 c_Color *color;
 int line_count;
 ruler_t ruler;
 
 char	svg_text[300];
 int	trunc_beg, trunc_end, comma_pos, length;
 int	i, j, k;
 conv_info_t	conv_info;
 boolean	set_diese, comma, carry;
 coord_t	string_width, inc_width, txt_width;
 boolean	first_show;
 c_Color	*tmp = NULL;
 boolean	shad;
 coord_t	add_y;
 
 Xc_HISTORY(("printText(`%s')", text));
 Xc_ASSERT(post->state.text_flag == FALSE);
 
 add_y = 0;
 x0 = 0;
 style = this->text_style;
 metrics = style->metrics;
 value_flag = Xq_IS_VALUE(value_type);
 
 /*--- Get ruler ---*/
 ruler = this->ruler_type;
 if((ruler & XqR_H_ALIGN_MASK) == XqR_DEFAULT)
 {
  ruler = (ruler & (~XqR_H_ALIGN_MASK)) |
   (value_flag? XqR_H_ALIGN_RIGHT : XqR_H_ALIGN_LEFT);
 }
 
 /*--- Get color ---*/
 if(value_flag != FALSE)
 {
  memcpy(svg_text, text, strlen(text) + 1);
  F(this->format).convert_string(this->format, text, value_type,
				 &conv_info); 
  comma_pos = conv_info.comma_pos;
  trunc_beg = conv_info.trunc_begin;
  trunc_end = conv_info.trunc_end;
  color = conv_info.color;
  Xc_TRACE(("---->before: '%s'  after: '%s'", svg_text, text));
 }
 else
 {
  color = NULL;
  trunc_beg = trunc_end = comma_pos = 0;
 }
 /*--- Remove spaces from left & right of text ---*/
 while(*text == 32) text++;
 if((max_len = strlen(text)) == 0) return TRUE;
 while((max_len > 0) && (text[max_len - 1] == 32)) max_len--;
 if(max_len == 0) return TRUE;
 end = text + max_len;
 
 /*--- Rotation & co ---*/
 if((ruler & XqR_A_MASK) == XqR_A_90)
 {
  height = bbox->urx - bbox->llx + 1;
  width = bbox->ury - bbox->lly + 1;
 }
 else if((ruler & XqR_A_MASK) == XqR_A_270)
 {
  height = bbox->urx - bbox->llx + 1;
  width = bbox->ury - bbox->lly + 1;
 }
 else
 {
  width = bbox->urx - bbox->llx + 1;
  height = bbox->ury - bbox->lly + 1;
 }
 
 /*--- Text dimensions ---*/
 if((ruler & XqR_MULTILINE) && !value_flag)
 {
  long i;

  line_count = 0;
  ptr = text;
  while((i = F(metrics).calibrate(metrics, width, &ptr, end)) != 0)
  {
   ptr += i;
   line_count++;
  }
 }
 else
  line_count = 1;
 
 Xc_TRACE(("line_count = %d", line_count));
 
 /*--- Vertical alignment ---*/
 y = height - (line_count - 1) * metrics->point_size - (metrics->ascender - metrics->descender);
 clip2_flag = (y < 0)? TRUE : FALSE; 
 
 y_inc = style->point_size;
 switch(ruler & XqR_V_ALIGN_MASK)
 {
 case XqR_V_ALIGN_CENTER:
  y >>= 1;
  break;
 case XqR_V_JUSTIFIED: 
  if(line_count == 1)
  {
   y >>= 1;
   break;
  }
  y_inc += y / (line_count - 1);
 case XqR_V_ALIGN_TOP:
  y = COORD_ZERO;
 case XqR_V_ALIGN_BOTTOM:
  break;
 }
 y += metrics->ascender;

 /*--- init text rise ---*/
 post->state2[NORMAL_TEXT].text_rise = 0;
 post->state2[SHADOW_TEXT].text_rise = 0;
/*
 if ((post->state2[NORMAL_TEXT].text_rise) || 
     (post->state2[SHADOW_TEXT].text_rise))
 {
  if(!F(post).putLine2(post, NORMAL_TEXT, "$c Ts", 
		       0)) return FALSE;
  if(!F(post).putLine2(post, SHADOW_TEXT, "$c Ts", 
		       0)) return FALSE;
  post->state2[NORMAL_TEXT].text_rise = 0;
  post->state2[SHADOW_TEXT].text_rise = 0;
 }
*/

 /*--- Line loop ---*/
 ws = cs = COORD_ZERO;
 init_flag = FALSE;
 while(line_count)
 {
  first_show = TRUE;
  if((ruler & XqR_MULTILINE) && !value_flag)
  {
   len = F(metrics).calibrate(metrics, width, &text, end);
   Xc_ASSERT(len != 0);
  }
  else
   len = max_len;
  
  x = width - F(metrics).textWidth(metrics, text, len);
      
  /*--- Numeric value doesn't fit in cell ---*/
  if(value_flag && (x < 0))
  {
   set_diese = TRUE;
   if (conv_info.truncable)
   {
    Xc_TRACE(("text before convertion: %s", text));
    length = strlen(text);
    string_width = 
     F(metrics).textWidth(metrics, text, trunc_beg);
    string_width += 
     F(metrics).textWidth(metrics, text + trunc_end,
			  length - trunc_end);
    if (string_width < width)
    {
     inc_width = COORD_ZERO; i = trunc_beg;
     while( (inc_width + string_width) < width && i < trunc_end)
     {
      inc_width += 
       F(metrics).textWidth(metrics, text + i, 1);
      i++;
     }
     if ( ((--i) == (trunc_beg + 1)) && (trunc_beg == comma_pos)) 
      i--;
     if ( (i == trunc_beg) && (trunc_beg == comma_pos))
      j = i+1;
     else
      j = i;
     if (text[j] >= '5')
     {
      k = 0;
      while(svg_text[k] != '.')
       k++;
      k += i - 1 - comma_pos;
      svg_text[k+1] = '\0';
      comma = FALSE; carry = TRUE;
      do
      {
       if (svg_text[k] == '.')
       {
	k--;
	comma = TRUE;
	continue;
       }
       if (svg_text[k] != '9')
       {
	if (svg_text[k]>='0' && svg_text[k]<='8')
	{
	 svg_text[k] += 1;
	 carry = FALSE;
	}
	else
	{
	 k++;
	 carry = TRUE;
	 break;
	}
       }
       else
       {
	svg_text[k] = '0';
	carry = TRUE;
	if (k==0) 
	 break;
	k--;
       }
      }
      while(carry);
      if (carry)
      {
       memmove(svg_text + k + 1, svg_text + k, 
	       strlen(svg_text)+1);
       svg_text[k] = '1';
      }
      memcpy(text, svg_text, strlen(svg_text) + 1);
      if (comma)
      {
       k = strlen(text)-1;
       while(text[k] == '0')
	k--;
       if (text[k]=='.')
	k--;
       text[k+1]='\0';
      }
		
      Xc_TRACE(("before the second convertion: %s", text));
      F(this->format).
       convert_string(this->format, text, Xq_VALUE, NULL); 
      len = strlen(text);
      txt_width = 
       F(metrics).textWidth(metrics, text, len);
      if (txt_width < width)
      {
       x = width - txt_width; 
       set_diese = FALSE;
      }
     }
     else
     {
      if (length - trunc_end + 1)
       memmove(text + i, text + trunc_end, 
	       length - trunc_end + 1);
      Xc_TRACE(("value truncated: result: %s", text));
		
      len = strlen(text);
      x = width - F(metrics).textWidth(metrics, text, len);
      set_diese = FALSE;
     }
    }
   }
   if (set_diese)
   {
    long i;
	    
    if((x = F(metrics).textWidth(metrics, "#", 1)) > 0)
    {
     len = width / x;
     x = width - (len * x);
    }
    if((x <= 0) || (len <= 0))
     return TRUE;
	    
    for(i = len; i--; text[i] = '#');
   }
  }
      
  /*--- Test if clipping needed ---*/
  if(!init_flag)
   clip2_flag |= ((ruler & XqR_MULTILINE) || (x < 0))? TRUE : FALSE;

  /*--- Horizontal alignment ---*/
  switch(ruler & XqR_H_ALIGN_MASK)
  {
  case XqR_DEFAULT:     
  case XqR_H_ALIGN_LEFT:
   x = COORD_ZERO;
  case XqR_H_ALIGN_RIGHT:
   break;
  case XqR_H_ALIGN_CENTER:
   x >>= 1;
   break;
  case XqR_H_JUSTIFIED:
   {
    int count;
	    
    for(ptr = text, count = 0; ptr < (text + len); ptr++)
     if(*ptr == 32) count++;
	    
    Xc_TRACE(("got %d space chars", count));
	    
    if(count)
    {
     Xc_ASSERT(len > 1);

     ws = (x * 70) / 100;
     cs = (x - ws) / (len - 1);
     ws /= count;
     x = COORD_ZERO;
    }
    else if(len > 1)
    {
     cs = x / (len - 1);
     x = COORD_ZERO;
    }
    else
     x >>= 1;

    break;
   }
  }
      
  if(init_flag == FALSE)
  {
   if(post->ai_mode == FALSE)
   {
    /*--- Text leading ---*/
    if((line_count != 1) && (post->state.leading != y_inc))
    {
     F(post).putLine(post, "$c TL", y_inc);
     post->state.leading = y_inc;
    }
   }

   /*--- Set clipping ---*/
   if(clip_flag & clip2_flag)
   {
    if(post->ai_mode)
     F(post).putLine(post, "q $p m $p L $p L $p L $p L H W n",
		     bbox->llx, bbox->lly, bbox->urx, bbox->lly,
		     bbox->urx, bbox->ury, bbox->llx, bbox->ury,
		     bbox->llx, bbox->lly);
    else
     F(post).putLine(post, "q $r re W n", bbox);
    if (!F(post).pushState(post)) return FALSE;
   }
   /*--- Set matrix ---*/
   F(post->matrix).translate(post->matrix, FALSE,
			     bbox->llx, bbox->lly);
   if((ruler & XqR_A_MASK) == XqR_A_90)
   {
    F(post->matrix).translate(post->matrix, FALSE,
			      COORD_ZERO, width);
    F(post->matrix).rotate(post->matrix, FALSE,
			   SCALE_FROM_DEGREES(-90.0));
   }
   else if((ruler & XqR_A_MASK) == XqR_A_270)
   {
    F(post->matrix).translate(post->matrix, FALSE,
			      height, COORD_ZERO);
    F(post->matrix).rotate(post->matrix, FALSE,
			   SCALE_FROM_DEGREES(90.0));
   }

   if(post->ai_mode)
    switch(ruler & XqR_H_ALIGN_MASK)
    {
    case XqR_H_ALIGN_RIGHT:
     x0 = width;
     break;
    case XqR_H_ALIGN_CENTER:
     x0 = width >> 1;
     break;
    case XqR_DEFAULT:     
    case XqR_H_ALIGN_LEFT:
    case XqR_H_JUSTIFIED:
    default:
     x0 = COORD_ZERO;
     break;
    }
   else
    x0 = x;
   
   if(line_count != 1)
   {
    F(post->matrix).translate(post->matrix, FALSE, x0,
			      y - (post->ai_mode? COORD_ZERO:y_inc));
   }
   else
   {
    F(post->matrix).translate(post->matrix, FALSE, x0,
			      y - (post->ai_mode?
				  COORD_ZERO:post->state.leading));
   }
	  
   F(post->matrix).scale(post->matrix, FALSE, SCALE_ONE, -SCALE_ONE);

   post->state.text_flag = TRUE;
   /*--- new object should have is one color ---*/
   post->state.stroke_color = NULL;
   post->state.fill_color = NULL; 
   post->state2[SHADOW_TEXT].font = NULL; 
   post->state2[NORMAL_TEXT].font = NULL; 
	  
   if(post->ai_mode != FALSE)
   {
    int i;

    /*--- Print text ruler ---*/
    switch(ruler & XqR_H_ALIGN_MASK)
    {
    case XqR_H_ALIGN_RIGHT:
     i = 2;
     break;
    case XqR_H_ALIGN_CENTER:
     i = 1;
     break;
    case XqR_H_JUSTIFIED:
     i = 4;
     break;
    case XqR_DEFAULT:     
    case XqR_H_ALIGN_LEFT:
    default:
     i = 0;
     break;
    }
    F(post).putLine2(post, SHADOW_TEXT, "$d Ta $c 0 Td", i, x - x0);
    F(post).putLine2(post, NORMAL_TEXT, "$d Ta $c 0 Td", i, x - x0);
   }

   y = 0;
   init_flag = TRUE;
  }
  else if(post->ai_mode != FALSE)
  {
   F(post).putLine2(post, SHADOW_TEXT, "$c $c Td", x - y, -y_inc);
   F(post).putLine2(post, NORMAL_TEXT, "$c $c Td", x - y, -y_inc);
  }
  else if(x != y)
  {
   F(post).putLine2(post, SHADOW_TEXT, "$c 0 Td", x - y);
   F(post).putLine2(post, NORMAL_TEXT, "$c 0 Td", x - y);
  }
  if(ws != COORD_ZERO)
   ws = ws / SCALE_TO_COEF(post->state.horizontal_scale);
  if(cs != COORD_ZERO)
   cs = cs / SCALE_TO_COEF(post->state.horizontal_scale);

  if((post->state.word_space != ws) || (post->state.char_space != cs))
  {
   F(post).putLine2(post, SHADOW_TEXT, "$c$s $c$s", 
		    ws, " Tw", cs, " Tc");
   F(post).putLine2(post, NORMAL_TEXT, "$c$s $c$s", 
		    ws, " Tw", cs, " Tc");
   post->state.word_space = ws;
   post->state.char_space = cs;
  }
  if(post->ai_mode != FALSE)
  {
   char *tmp;

   if((tmp = Xc_malloc("string", len + 1)) != NULL)
   {
    memcpy(tmp, text, len);
    tmp[len] = '\r';
    tmp[len+1] = 0;
    F(style).ps_print_text(style, tmp, len+1, post,
			   x-x0, -add_y,
			   "Tj");

    Xc_free(tmp);
   }
   add_y += y_inc;  /* coz no TL in AI */
  }
  else
  {
   /*--- particular text style ---*/
   if(color != NULL)
   {
    tmp = style->color;
    style->color = color;
   }
   add_y += post->state.leading;
   F(style).ps_print_text(style, text, len, post, 
			  x-x0, -add_y,
			  "'");
   if(color != NULL) style->color = tmp;
  }

  /*--- Update variables ---*/
  y = x;
  line_count--;
  text += len;
 }

 if((post->state.word_space != 0) || (post->state.char_space != 0))
 {
  F(post).putLine2(post, SHADOW_TEXT, "$c$s $c$s", 
		   0, " Tw", 0, " Tc");
  F(post).putLine2(post, NORMAL_TEXT, "$c$s $c$s", 
		   0, " Tw", 0, " Tc");
 }

 F(post).putLine(post, post->ai_mode? "0 To $m 0 Tp TP":"BT $m Tm",
		 F(post->matrix).get(post->matrix, 0));
 shad = FALSE;
 if (post->shadow_text)
 {
  F(post).deferredWrite(post, SHADOW_TEXT);
  F(post).putLine(post, post->ai_mode? "TO" : "ET");
  shad = TRUE;
  if (post->state2[SHADOW_TEXT].fill_color)
   post->state.fill_color = post->state2[SHADOW_TEXT].fill_color;
  if (post->state2[SHADOW_TEXT].stroke_color)
   post->state.stroke_color = post->state2[SHADOW_TEXT].stroke_color;
 }
 if (post->shadow_graph)
 {
  F(post).deferredWrite(post, SHADOW_GRAPH);
  if (post->state2[SHADOW_GRAPH].fill_color)
   post->state.fill_color = post->state2[SHADOW_GRAPH].fill_color;
  if (post->state2[SHADOW_GRAPH].stroke_color)
   post->state.stroke_color = post->state2[SHADOW_GRAPH].stroke_color;
 }
 if (post->normal_text)
 {
  if (shad)
   F(post).putLine(post, post->ai_mode? "0 To $m 0 Tp TP":"BT $m Tm",
		   F(post->matrix).get(post->matrix, 0));
  F(post).deferredWrite(post, NORMAL_TEXT);
  F(post).putLine(post, post->ai_mode? "TO" : "ET");
  if (post->state2[NORMAL_TEXT].fill_color)
   post->state.fill_color = post->state2[NORMAL_TEXT].fill_color;
  if (post->state2[NORMAL_TEXT].stroke_color)
   post->state.stroke_color = post->state2[NORMAL_TEXT].stroke_color;
 }
 if (post->normal_graph)
 {
  F(post).deferredWrite(post, NORMAL_GRAPH);
  if (post->state2[NORMAL_GRAPH].fill_color)
   post->state.fill_color = post->state2[NORMAL_GRAPH].fill_color;
  if (post->state2[NORMAL_GRAPH].stroke_color)
   post->state.stroke_color = post->state2[NORMAL_GRAPH].stroke_color;
 }
 post->state.word_space = 0;
 post->state.char_space = 0;
 post->state2[NORMAL_TEXT].stroke_color = NULL;
 post->state2[NORMAL_GRAPH].stroke_color = NULL;
 post->state2[SHADOW_TEXT].stroke_color = NULL;
 post->state2[SHADOW_GRAPH].stroke_color = NULL;
 post->state2[NORMAL_TEXT].fill_color = NULL;
 post->state2[NORMAL_GRAPH].fill_color = NULL;
 post->state2[SHADOW_TEXT].fill_color = NULL;
 post->state2[SHADOW_GRAPH].fill_color = NULL;

 if (clip_flag & clip2_flag)
 {
  if (!post->ai_mode)
   F(post).popState(post);
  return F(post).putLine(post, "Q");
 }
 return TRUE;
}

