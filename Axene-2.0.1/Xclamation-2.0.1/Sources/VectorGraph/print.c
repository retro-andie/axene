/*
** print.c for Xclamation and XAllWrite in VectorGraph/
** PostScript print methods for VectorGraph class
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
** Started on  Wed Dec 13 16:12:44 1995 Stéphane Boisson
** Last update Tue Jan 18 16:11:13 2000 Emmanuel Paris
*/

#include "VectorGraphP.h"

/*--- Local variables ---*/
static char *render_close[] = {NULL, "f", "s", "b",
				"q f Q W n", "q s Q W n", "q b Q W n"};
static char *render_normal[] = {NULL, "f", "S", "B",
				 "q f Q W n", "q S Q W n", "q B Q W n"};

static boolean print_arc();
static boolean add_arc();

/* ----------------------------------------------------------------- ** 
** ps_print - Vector Graph rint frame standard method                ** 
** ----------------------------------------------------------------- */
boolean vectorPrint(this, post)
c_VectorGraph	*this;
c_PostScript	*post;
{
 vector_item_t *item, *last_item;
 boolean rc, close_flag;
 matrix_t *text_matrix;
  
 Xc_HISTORY(("ps_print"));

 XcM_DUMP_MATRIX("before", F(post->matrix).get(post->matrix, 0));
 rc = TRUE;
 text_matrix = NULL;
 F(post->matrix).translate(post->matrix, FALSE,
			   this->offset.dx, this->offset.dy);
 F(post->matrix).scale(post->matrix, FALSE,
		       this->width_factor, this->height_factor);

 /*--- Vertical flipping ---*/
 if(this->vflip_flag == FALSE)
 {
  F(post->matrix).translate(post->matrix, FALSE,
			    COORD_ZERO, this->bbox.ury - this->bbox.lly);
  F(post->matrix).scale(post->matrix, FALSE, SCALE_ONE, -SCALE_ONE);
 }

 /*--- Horizontal flipping ---*/
 if(this->hflip_flag != FALSE)
 {
  F(post->matrix).translate(post->matrix, FALSE,
			    this->bbox.urx - this->bbox.llx, COORD_ZERO);
  F(post->matrix).scale(post->matrix, FALSE, -SCALE_ONE, SCALE_ONE);
 }

 F(post->matrix).translate(post->matrix, FALSE,
			   -this->bbox.llx, -this->bbox.lly);
 XcM_DUMP_MATRIX("after", F(post->matrix).get(post->matrix, 0));

 rc = F(post).putLine(post, "$m cm", F(post->matrix).get(post->matrix, 0));
 F(post->matrix).identity(post->matrix, FALSE);

 /*--- For each vector item ---*/
 close_flag = FALSE;
 item = this->base_head;
 last_item = NULL;

 while((item != NULL) && (rc != FALSE))
 {
  /*--- Item specific print ---*/
  switch(item->type)
  {
   /*--- Context ---*/
  case XcVG_CONTEXT_TYPE:
   Xc_ASSERT(item->data.children.tail != NULL);

   if(post->state.text_flag && 
      ((rc = F(post).putLine(post, "ET")) == FALSE)) break;
   post->state.text_flag = FALSE;

   if(last_item != item->data.children.tail)
   {
    /*--- Push ---*/
    Xc_ASSERT(item->data.children.head != NULL);
    rc = F(post).putLine(post, "q") && F(post).pushState(post);
    last_item = item;
    item = item->data.children.head;
    continue;
   }
	  
   /*--- Pop ---*/
   rc = F(post).putLine(post, "Q") && F(post).popState(post);
   break;
	  	  
   /*--- Path decription ---*/
  case XcVG_PATH_TYPE:
   if(post->state.text_flag && 
      ((rc = F(post).putLine(post, "ET")) == FALSE)) break;
   post->state.text_flag = FALSE;

   if(item->data.path.control_count)
   {
    unsigned char *control;
    unsigned int i;
    coord_t *data;
	      
    control = item->data.path.control;
    data = item->data.path.data;
    for(i = item->data.path.control_count; i && rc; i--)
    {
     if(close_flag)
     {
      rc = F(post).putLine(post, "h");
      close_flag = FALSE;
     }
		  
     switch(*control++)
     {
      /*--- moveto ---*/
     case XcVG_MOVE_OP:
      rc = F(post).putLine(post, "$p m", data[0], data[1]);
      data += 2;
      break;
		      
      /*--- lineto ---*/
     case XcVG_LINE_OP:
      rc = F(post).putLine(post, "$p l", data[0], data[1]);
      data += 2;
      break;

      /*--- arcto ---*/
     case XcVG_ARC_OP:
      rc = print_arc(post, data[0], data[1],
		data[2], data[3],
		data[4], data[5]);
#if 0
      rc = F(post).putLine(post, "$p $p $p c",
			   data[0], data[1], data[2], data[3],
			   data[4], data[5]);
#endif
      data += 6;
      break;

      /*--- curveto ---*/
     case XcVG_CURVE_OP:
      rc = F(post).putLine(post, "$p $p $p c",
			   data[0], data[1], data[2], data[3],
			   data[4], data[5]);
      data += 6;
      break;
		      
      /*--- curve at start cap ---*/
     case XcVG_CURVE_BEGIN_OP:
      rc = F(post).putLine(post, "$p $p y", data[0], data[1],
			   data[2], data[3]);
      data += 4;
      break;
		      
      /*--- curve at end cap ---*/
     case XcVG_CURVE_END_OP:
      rc = F(post).putLine(post, "$p $p v", data[0], data[1],
			   data[2], data[3]);
      data += 4;
      break;
		      
      /*--- closepath ---*/
     case XcVG_CLOSE_OP:
      close_flag = TRUE;
      break;
     }
    }
   }
   break;
	  
   /*--- Render path ---*/
  case XcVG_RENDER_TYPE:
   if(post->state.text_flag && 
      ((rc = F(post).putLine(post, "ET")) == FALSE)) break;
   post->state.text_flag = FALSE;

   if(!item->data.render_mode)
    rc = F(post).putLine(post, "n");
   else if(close_flag)
   {
    rc = F(post).putLine(post, render_close[item->data.render_mode]);
    close_flag = FALSE;
   }
   else
    rc = F(post).putLine(post, render_normal[item->data.render_mode]);
   break;

   /*--- Add path to clipping path ---*/
  case XcVG_CLIP_TYPE:
   if(post->state.text_flag && 
      ((rc = F(post).putLine(post, "ET")) == FALSE)) break;
   post->state.text_flag = FALSE;

   rc = F(post).putLine(post, "W");
   break;
	  
   /*--- Stroke color ---*/
  case XcVG_STROKE_COLOR_TYPE:
   rc = F(item->data.color).ps_print(item->data.color, post, TRUE);
   break;
	  
   /*--- Fill color ---*/
  case XcVG_FILL_COLOR_TYPE:
   rc = F(item->data.color).ps_print(item->data.color, post, FALSE);
   break;
	  
   /*--- Line width ---*/
  case XcVG_LINE_WIDTH_TYPE:
   if(post->state.line_width != item->data.line_width)
   {
    post->state.line_width = item->data.line_width;
    rc = F(post).putLine(post, "$c w", item->data.line_width);
   }
   break;
	  
   /*--- Line join style ---*/
  case XcVG_LINE_JOIN_TYPE:
   if(post->state.line_join != item->data.line_join)
   {
    post->state.line_join = item->data.line_join;
    rc = F(post).putLine(post, "$d j", item->data.line_join);
   }
   break;
	  
   /*--- Line cap style ---*/
  case XcVG_LINE_CAP_TYPE:
   if(post->state.line_cap != item->data.line_cap)
   {
    post->state.line_cap = item->data.line_cap;
    rc = F(post).putLine(post, "$d J", item->data.line_cap);
   }
   break;
	  
   /*--- Flatness ---*/
  case XcVG_FLATNESS_TYPE:
   if(post->state.flatness != item->data.flatness)
   {
    post->state.flatness = item->data.flatness;
    rc = F(post).putLine(post, "$d i", item->data.flatness);
   }
   break;

   /*--- Miter limit ---*/
  case XcVG_MITER_LIMIT_TYPE:
   rc = F(post).putLine(post, "$f M", (double)item->data.miter_limit);
   break;

   /*--- Line dash style ---*/
  case XcVG_LINE_DASH_TYPE:
   if((post->state.dash_len == (int)item->data.line_dash.length) &&
      (post->state.dash_phase == item->data.line_dash.phase))
   {
    int i;

    i = MIN(XcPS_MAX_LINE_DASHES, item->data.line_dash.length);
    while(i--)
     if(post->state.dash[i] != item->data.line_dash.pattern[i])
      break;
    if(i == 0) break;

    i = MIN(XcPS_MAX_LINE_DASHES, item->data.line_dash.length);
    while(i--)
     post->state.dash[i] = item->data.line_dash.pattern[i];
   }

   rc = F(post).putLine(post, "$a$c d", item->data.line_dash.length,
			item->data.line_dash.pattern, 
			item->data.line_dash.phase);
   post->state.dash_len = item->data.line_dash.length;
   post->state.dash_phase = item->data.line_dash.phase;
   break;

   /*--- Text matrix ---*/
  case XcVG_TEXT_MATRIX_TYPE:
   if(!post->state.text_flag)
    text_matrix = &item->data.matrix;
   else
    rc = F(post).putLine(post, "$m Tm", &item->data.matrix);
   break;

   /*--- Text matrix translation ---*/
  case XcVG_TEXT_TRANSLATE_TYPE:
   if(!post->state.text_flag && 
      ((rc = F(post).putLine(post, "BT")) == FALSE)) break;
   post->state.text_flag = TRUE;

   if(text_matrix != NULL)
   {
    if((rc = F(post).putLine(post, "$m Tm", text_matrix)) == FALSE)
     break;
    else
     text_matrix = NULL;
   }
   rc = F(post).putLine(post, "$v Td", item->data.vector.dx,
			item->data.vector.dy);
   break;

   /*--- Text newline ---*/
  case XcVG_TEXT_NEWLINE_TYPE:
   if(!post->state.text_flag && 
      ((rc = F(post).putLine(post, "BT")) == FALSE)) break;
   post->state.text_flag = TRUE;

   if(text_matrix != NULL)
   {
    if((rc = F(post).putLine(post, "$m Tm", text_matrix)) == FALSE)
     break;
    else
     text_matrix = NULL;
   }
   rc = F(post).putLine(post, "T*");
   break;

   /*--- Text rise ---*/
  case XcVG_TEXT_RISE_TYPE:
   if(post->state.text_rise != item->data.offset)
   {
    post->state.text_rise = item->data.offset;
    rc = F(post).putLine(post, "$c Ts", item->data.offset);
   }
   break;

   /*--- Text render mode ---*/
  case XcVF_TEXT_MODE_TYPE:
   if(post->state.render_mode != item->data.render_mode)
   {
    post->state.render_mode = item->data.render_mode;
    rc = F(post).putLine(post, "$d Tr", item->data.render_mode);
   }
   break;

   /*--- Text font ---*/
  case XcVG_TEXT_FONT_TYPE:
   if((post->state.font != item->data.font.data) ||
      (post->state.point_size != item->data.font.size))
   {
    post->state.font = item->data.font.data;
    post->state.point_size = item->data.font.size;
    rc = (F(post).addFont(post, item->data.font.data) &&
	  F(post).putLine(post, "/F$d $c Tf",
			  item->data.font.data->id,
			  item->data.font.size));
   }
   break;
   /*--- text nealine leading ---*/
  case XcVG_TEXT_LEADING_TYPE:
   if(post->state.leading != item->data.offset)
   {
    post->state.leading = item->data.offset;
    rc = F(post).putLine(post, "$c TL", item->data.offset);
   }
   break;

   /*--- Text word spacing ---*/
  case XcVG_TEXT_WORD_SPACING_TYPE:
   if(post->state.word_space != item->data.offset)
   {
    post->state.word_space = item->data.offset;
    rc = F(post).putLine(post, "$c Tw", item->data.offset);
   }
   break;

   /*--- Text char spacing ---*/
  case XcVG_TEXT_CHAR_SPACING_TYPE:
   if(post->state.char_space != item->data.offset)
   {
    post->state.char_space = item->data.offset;
    rc = F(post).putLine(post, "$c Tc", item->data.offset);
   }
   break;

   /*--- Text horizontal scale ---*/
  case XcVG_TEXT_HSCALE_TYPE:
   if(post->state.horizontal_scale != item->data.scale)
   {
    post->state.horizontal_scale = item->data.scale;
    rc = F(post).putLine(post, "$f Tz", (double)
			 SCALE_TO_PERCENT(item->data.scale));
   }
   break;

   /*--- Text kerning ---*/
  case XcVG_TEXT_KERNING_TYPE:
   rc = F(post).putLine(post, "[$f]TJ", item->data.kerning);
   break;

   /*--- Text rendering ---*/
  case XcVG_TEXT_RENDER_TYPE:
   if(!post->state.text_flag && 
      ((rc = F(post).putLine(post, "BT")) == FALSE)) break;
   post->state.text_flag = TRUE;

   if(text_matrix != NULL)
   {
    if((rc = F(post).putLine(post, "$m Tm", text_matrix)) == FALSE)
     break;
    else
     text_matrix = NULL;
   }
   rc = (F(post).putString(post, item->data.text.string,
			   item->data.text.length) &&
	 F(post).putLine(post, "Tj"));
   break;

  default:
   Xc_BREAK(("Unknown item type"));
  }
      
  last_item = item;
  item = item->next;
 }

 /*--- End text object ---*/
 if(rc && post->state.text_flag)
 {
  rc = F(post).putLine(post, "ET");
  post->state.text_flag = FALSE;
 }

 Xc_TRACE(("done"));
 return rc;
}

/* ----------------------------------------------------------------- ** 
** print arc with curve                                              ** 
** ----------------------------------------------------------------- */
boolean print_arc(post, Cx, Cy, Rx, Ry, Ang2, Ang1)
c_PostScript	*post;
coord_t Cx;
coord_t Cy;
coord_t Rx;
coord_t Ry;
coord_t Ang2;
coord_t Ang1;
{
 double sin0x, cos0x;
 double sin0y, cos0y;
 double	trad;
 coord_t x0, y0;
 coord_t radius;
 coord_t x3r, y3r;
 angle_t ang1, ang2, adiff;
 boolean first;
 coord_t Step;

 /*--- Assume counter clockwise ---*/
 ang1 = Ang2;
 ang2 = Ang1;
 if (ang2 < ang1)
  Step = -SCALE_FROM_DEGREES(90.0);
 else
  Step = SCALE_FROM_DEGREES(90.0);

 first = TRUE;
 radius = Rx;
 sin0x = SCALE_TO_POINTS(Rx) * sin(SCALE_TO_RADIANS(ang1));
 cos0x = SCALE_TO_POINTS(Rx) * cos(SCALE_TO_RADIANS(ang1));
 sin0y = SCALE_TO_POINTS(Ry) * sin(SCALE_TO_RADIANS(ang1));
 cos0y = SCALE_TO_POINTS(Ry) * cos(SCALE_TO_RADIANS(ang1));
 x0 = Cx + SCALE_FROM_POINTS(cos0x);
 y0 = Cy + SCALE_FROM_POINTS(sin0y);
 Xc_TRACE(("from %g to %g", SCALE_TO_DEGREES(ang1), SCALE_TO_DEGREES(ang2)));


 while((adiff = labs((long)(ang2 - ang1))) > SCALE_FROM_DEGREES(90.0))
 {
  double w;

  w = cos0x;
  cos0x = -sin0x;
  sin0x = w;
  w = cos0y;
  cos0y = -sin0y;
  sin0y = w;

  x3r = Cx + SCALE_FROM_POINTS(cos0x);
  y3r = Cy + SCALE_FROM_POINTS(sin0y);
  if(!add_arc(post, (float)Ry/Rx, first, radius, x0, y0, x3r, y3r,
	      x0 + SCALE_FROM_POINTS(cos0x),
	      y0 + SCALE_FROM_POINTS(sin0y))) return FALSE;
  x0 = x3r, y0 = y3r;
  ang1 += Step;
  first = FALSE;
 }

 /*--- Compute the intersection of the tangents ---*/
 Xc_TRACE(("angle diff= %g", SCALE_TO_DEGREES(adiff)));
  
 if (Step > 0)
  trad = tan(SCALE_TO_RADIANS(adiff) / 2.0);
 else
  trad = -tan(SCALE_TO_RADIANS(adiff) / 2.0);
 x3r = Cx + Rx * cos(SCALE_TO_RADIANS(ang2));
 y3r = Cy + Ry * sin(SCALE_TO_RADIANS(ang2));
 if(!add_arc(post, (float)Ry/Rx, first, radius, x0, y0, x3r, y3r,
	     x0 - SCALE_FROM_POINTS(trad * sin0x),
	     y0 - SCALE_FROM_POINTS(fabs(trad) * cos0y))) 
  return FALSE;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** add_arc - Internal routine to add an arc to the path              ** 
** ----------------------------------------------------------------- */
static boolean add_arc(post, ratio, first, radius, x0, y0, x3, y3, xt, yt)
c_PostScript *post;
float	ratio;
boolean first;
coord_t radius;
coord_t x0;
coord_t y0;
coord_t x3;
coord_t y3;
coord_t xt;
coord_t yt;
{
 double fraction, dx, dy, r;

 Xc_TRACE(("add_arc"));

 dx = SCALE_TO_POINTS(xt - x0); 
 dy = SCALE_TO_POINTS(yt - y0);
 r = SCALE_TO_POINTS(radius);

 /*--- Compute the fraction coefficient for the curve ---*/
 if(fabs(r) < 1.0e-4)
 {
  /*--- Almost zero radius ---*/
  fraction = 0.0;
 }
 else
 {
  fraction = (4.0/3.0) / (1.0 + sqrt(1.0 + (dx * dx + dy * dy) / (r * r)));
 }

 if(first && !F(post).putLine(post, "$p l", x0, y0)) return FALSE;
/*
 return F(post).putLine(post, "$p $p $p c",
			x0 + (coord_t)((xt - x0) * fraction),
			(coord_t)((y0 + (coord_t)((yt - y0)*fraction))*ratio),
			x3 + (coord_t)((xt - x3) * fraction),
			(coord_t)((y3 + (coord_t)((yt - y3)*fraction))*ratio), 
			x3, 
			(coord_t)(y3*ratio));
*/
/*
 return F(post).putLine(post, "$p $p $p c",
			x0 + (coord_t)((xt - x0) * fraction),
			y0 + (coord_t)((yt - y0)*fraction*ratio),
			x3 + (coord_t)((xt - x3) * fraction),
			y3 + (coord_t)((yt - y3)*fraction*ratio),
			x3, 
			y3);
*/
 return F(post).putLine(post, "$p $p $p c",
			x0 + (coord_t)((xt - x0) * fraction),
			y0 + (coord_t)((yt - y0)*fraction),
			x3 + (coord_t)((xt - x3) * fraction),
			y3 + (coord_t)((yt - y3)*fraction),
			x3, 
			y3);
}
