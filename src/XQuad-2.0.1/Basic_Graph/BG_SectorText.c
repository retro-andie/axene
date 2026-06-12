/*
** BG_SectorText.c for XQuad in Basic_Graph/
**
** Copyright (C) 1995-2000 Axene.
** Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
** Started on  Sat Jun 24 18:07:33 1995 Emmanuel Paris
** Last update Sat Oct 26 20:26:30 1996 One of the authors
*/

#define NTRACE
#define NHISTORY

#include "BG_SectorText.h"
#include <stdarg.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void change_xinfo ___PROTO((c_BG_SectorText *This, x_info *xinfo));
static error change_base ___PROTO((c_BG_SectorText *This, BaseStd_t *new_base));
static void display_redraw ___PROTO((c_BG_SectorText *This, c_Matrix *matrix,
				     coord_t scale, int dx, int dy));
static void display_handles ___PROTO((c_BG_SectorText *This, c_Matrix *matrix,
				      coord_t scale, int dx, int dy));
static void add_to_vectorgraph ___PROTO((c_BG_SectorText *This, 
					 c_VectorGraph *vectorg));
static void sub_to_vectorgraph ___PROTO((c_BG_SectorText *This, 
					 c_VectorGraph *vectorg));
static bbox_t *get_bounding_box ___PROTO((c_BG_SectorText *This));
static boolean is_in ___PROTO((c_BG_SectorText *This, coord_t x, coord_t y));
static void setBG ___PROTO((c_BG_SectorText *This, ...));
static void vset();
static void calcul_sectorText ___PROTO((c_BG_SectorText *This));
static boolean ps_print ___PROTO((c_BG_SectorText *This, c_PostScript *post));
static boolean readBG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeBG ___PROTO((c_BG_SectorText *This, c_DocFile *doc));

sf_BG_SectorText fc_BG_SectorText =
{
  cons, dest, copy,
  change_xinfo, change_base, display_redraw, display_handles,
  add_to_vectorgraph, sub_to_vectorgraph, get_bounding_box, is_in, setBG,
  ps_print, readBG, writeBG
};

static void *cons(base)
BaseStd_t	*base;
{
  c_BG_SectorText	*This;
  
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("BG_SectorText", sizeof(c_BG_SectorText)) ) == NULL)
    return NULL;
  This->f = &fc_BG_SectorText;
  
  This->type = BG_SECTORTEXT;
  This->BaseStd = base;
  
  This->sector = (c_BG_Sector *)NEW(c_BG_Sector)(base);
  This->string = (c_BG_Text *)NEW(c_BG_Text)(base);
  This->percent_string = (c_BG_Text *)NEW(c_BG_Text)(base);
  This->percent_value = 0.0;
  This->bbox_clip.llx = This->bbox_clip.ury = 0.0;
  This->bbox_clip.urx = BG_AREA_WIDTH;
  This->bbox_clip.lly = BG_AREA_HEIGHT;
  calcul_sectorText(This);
  
  Xc_TRACE(("constructor done"));
  return This;
}

static void dest(This)
c_BG_SectorText	*This;
{
  Xc_HISTORY(("destructor")); 
  DELETE(c_BG_Sector)(This->sector);
  DELETE(c_BG_Text)(This->percent_string);
  DELETE(c_BG_Text)(This->string);
  Xc_free(This);
  Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_BG_SectorText	*This;
BaseStd_t	*base;
{
  c_BG_SectorText	*bg_copy;
  
  if ( (bg_copy = Xc_malloc("BG_SectorText", sizeof(c_BG_SectorText)) ) == NULL)
    return NULL;
  memcpy(bg_copy, This, sizeof(c_BG_SectorText));
  
  if (bg_copy->BaseStd != base)
    BG_CHANGE_BASE(bg_copy)(bg_copy, base);
  return bg_copy;
}

static void change_xinfo(This, xinfo)
c_BG_SectorText	*This;
x_info		*xinfo;
{
  memcpy(&This->X_info, xinfo, sizeof(x_info));
  if (This->sector)
    BG_CHANGE_XINFO(This->sector)(This->sector, xinfo);
  if (This->percent_string)
    BG_CHANGE_XINFO(This->percent_string)(This->percent_string, xinfo);
  if (This->string)
    BG_CHANGE_XINFO(This->string)(This->string, xinfo);
}

static error change_base(This, new_base)
c_BG_SectorText	*This;
BaseStd_t	*new_base;
{
  This->BaseStd = new_base;
  return XC_NO_ERROR;
}

static void display_redraw(This, matrix, scale, dx, dy)
c_BG_SectorText	*This;
c_Matrix	*matrix;
coord_t		scale;
int		dx;
int		dy;
{
  if (This->sector)
    BG_DISPLAY_REDRAW(This->sector)(This->sector, matrix, scale, dx, dy);
  if (This->percent_string)
    BG_DISPLAY_REDRAW(This->percent_string)(This->percent_string,
					    matrix, scale, dx, dy);
  if (This->string)
    BG_DISPLAY_REDRAW(This->string)(This->string, matrix, scale, dx, dy);
}

static void display_handles(This, matrix, scale, dx, dy)
c_BG_SectorText	*This;
c_Matrix	*matrix;
coord_t		scale;
int		dx;
int		dy;
{
  if (This->sector)
    BG_DISPLAY_HANDLES(This->sector)(This->sector, matrix, scale, dx, dy);
  if (This->percent_string)
    BG_DISPLAY_HANDLES(This->percent_string)(This->percent_string,
					     matrix, scale, dx, dy);
  if (This->string)
    BG_DISPLAY_HANDLES(This->string)(This->string, matrix, scale, dx, dy);
}

static void add_to_vectorgraph(This, vectorg)
c_BG_SectorText  *This;
c_VectorGraph *vectorg;
{
  if (This->sector)
    BG_ADD_TO_VECTORGRAPH(This->sector)(This->sector, vectorg);
  if (This->percent_string)
    BG_ADD_TO_VECTORGRAPH(This->percent_string)(This->percent_string, vectorg);
  if (This->string)
    BG_ADD_TO_VECTORGRAPH(This->string)(This->string, vectorg);
}

static void sub_to_vectorgraph(This, vectorg)
c_BG_SectorText  *This;
c_VectorGraph *vectorg;
{
  if (This->sector)
    BG_SUB_TO_VECTORGRAPH(This->sector)(This->sector, vectorg);
  if (This->percent_string)
    BG_SUB_TO_VECTORGRAPH(This->percent_string)(This->percent_string, vectorg);
  if (This->string)
    BG_SUB_TO_VECTORGRAPH(This->string)(This->string, vectorg);
}

static bbox_t *get_bounding_box(This)
c_BG_SectorText *This;
{
  return &(This->bbox);
}

static boolean is_in(This, x, y)
c_BG_SectorText	*This;
coord_t		x;
coord_t		y;
{
  if (x >= This->bbox.llx && x <= This->bbox.urx &&
      y >= This->bbox.lly && y <= This->bbox.ury)
    return TRUE;
  return FALSE;
}

static void setBG(c_BG_SectorText *This, ...)
{
  va_list ap;

  Xc_HISTORY(("set"));

  va_start(ap, This);
  vset(This, ap);
  va_end(ap);
} 

static void vset(This, ap)
c_BG_SectorText *This;
va_list ap;
{
  boolean out_flag;
  boolean calcul;
  c_TextStyle	*tstyle;
  
  Xc_TRACE(("vset"));
  
  out_flag = calcul = FALSE;
  do {
    switch(va_arg(ap, BG_SectorText_set_code_t))
    {
    case XcBG_SectorText_ANGLE1:
      Xc_TRACE(("XcBG_SectorText_ANGLE1"));
      calcul = TRUE;
      if (This->sector)
	BG_SET(This->sector)(This->sector, XcBG_Sector_ANGLE1,
			     va_arg(ap, vangle_t), XcBG_Sector_END );
      break;
    case XcBG_SectorText_ANGLE2:
      Xc_TRACE(("XcBG_SectorText_ANGLE2"));
      if (This->sector)
	BG_SET(This->sector)(This->sector, XcBG_Sector_ANGLE2,
			     va_arg(ap, vangle_t), XcBG_Sector_END );
      calcul = TRUE;
      break;
    case XcBG_SectorText_BBOX_CLIP:
      Xc_TRACE(("XcBG_SectorText_BBOX_CLIP"));
      This->bbox_clip = va_arg(ap, bbox_t);
      calcul = TRUE;
      break;
    case XcBG_SectorText_RAYON:
      Xc_TRACE(("XcBG_SectorText_RAYON"));
      BG_SET(This->sector)(This->sector, XcBG_Sector_RAYON, 
			   va_arg(ap, vector_t), XcBG_Sector_END);
      calcul = TRUE;
      break;
    case XcBG_SectorText_CENTER:
      Xc_TRACE(("XcBG_SectorText_CENTER"));
      BG_SET(This->sector)(This->sector, XcBG_Sector_CENTER, 
			   va_arg(ap, vector_t), XcBG_Sector_END);
      calcul = TRUE;
      break;
    case XcBG_SectorText_COLOR:
      Xc_TRACE(("XcBG_SectorText_COLOR"));
      BG_SET(This->sector)(This->sector, XcBG_Sector_INS_FG_COLOR, 
			   va_arg(ap, c_Color *), XcBG_Sector_END);
      break;
    case XcBG_SectorText_STRING:
      Xc_TRACE(("XcBG_SectorText_STRING"));
      BG_SET(This->string)(This->string, XcBG_Text_STRING, 
			   va_arg(ap, char *), XcBG_Text_END);
      calcul = TRUE;
      break;
    case XcBG_SectorText_PERCENT:
      Xc_TRACE(("XcBG_SectorText_PERCENT"));
      This->percent_value = SCALE_FROM_VSCALE( va_arg(ap, vscale_t));
      sprintf(This->percent_str, "%.2f%%", 
	      SCALE_TO_PERCENT(This->percent_value) ); 
      BG_SET(This->percent_string)(This->percent_string, XcBG_Text_STRING, 
				   This->percent_str, XcBG_Text_END);
      calcul = TRUE;
      break;
    case XcBG_SectorText_STYLE:
      Xc_TRACE(("XcBG_SectorText_STYLE"));
      tstyle =  va_arg(ap, c_TextStyle *);
      BG_SET(This->string)(This->string, XcBG_Text_STYLE, 
			   tstyle, XcBG_Text_END);
      BG_SET(This->percent_string)(This->percent_string, XcBG_Text_STYLE, 
				   tstyle, XcBG_Text_END);
      calcul = TRUE;
      break; 
    case XcBG_SectorText_END:
      Xc_TRACE(("XcBG_SectorText_END"));
      out_flag = TRUE;
      break;
    default:
      Xc_BREAK(("Unknow BG_SectorText_set_code"));
      break;
    }
  } while(!out_flag);
  if (calcul)
    calcul_sectorText(This);
}

static void calcul_sectorText(This)
c_BG_SectorText	*This;
{
  real	cs, ss;
  real	angle;
  vector_t	point1, size;
  ruler_t	ruler;
  
  angle = SCALE_TO_RADIANS((This->sector->angle2 - This->sector->angle1) / 2 +
			   This->sector->angle1);
  cs = cos(angle);
  ss = sin(angle);
  
  point1.dx = This->sector->center.dx + cs * ( This->sector->rayon.dx * 1.15);
  point1.dy = This->sector->center.dy - ss * ( This->sector->rayon.dy * 1.15);
  
  if (point1.dx > This->sector->center.dx)
  {
    ruler = XqR_H_ALIGN_LEFT | XqR_V_ALIGN_TOP;
    size.dx = This->bbox_clip.urx - point1.dx;
  }
  else
  {
    ruler = XqR_H_ALIGN_RIGHT | XqR_V_ALIGN_TOP;
    size.dx = point1.dx - This->bbox_clip.llx;
    point1.dx = This->bbox_clip.llx;
  }
  if (point1.dy > This->sector->center.dy)
  {
    size.dy = This->percent_string->tstyle->point_size;
    BG_SET(This->percent_string)(This->percent_string, 
				 XcBG_Text_ORIGIN, point1, 
				 XcBG_Text_SIZE, size,
				 XcBG_Text_RULER, ruler,
				 XcBG_Text_END);
    if (This->string->string)
    {
      point1.dy += size.dy;
      size.dy = This->string->tstyle->point_size;
      BG_SET(This->string)(This->string, 
			   XcBG_Text_ORIGIN, point1,
			   XcBG_Text_SIZE, size,
			   XcBG_Text_RULER, ruler,
			   XcBG_Text_END);
    }
  }
  else
  {
    size.dy = This->string->tstyle->point_size;
    if (This->string->string)
    {
      point1.dy -= size.dy;
      BG_SET(This->string)(This->string, 
			   XcBG_Text_ORIGIN, point1,
			   XcBG_Text_SIZE, size,
			 XcBG_Text_RULER, ruler,
			   XcBG_Text_END);
    }
    size.dy = This->percent_string->tstyle->point_size;
    point1.dy -= size.dy;
    BG_SET(This->percent_string)(This->percent_string, 
				 XcBG_Text_ORIGIN, point1, 
				 XcBG_Text_SIZE, size,
				 XcBG_Text_RULER, ruler,
				 XcBG_Text_END);
  }
}

/* -------------------------------------------------------------------- **
** ps print, read & write BG_SectorText					**
** -------------------------------------------------------------------- */ 
static boolean ps_print(This, post)
c_BG_SectorText *This;
c_PostScript *post;
{
  Xc_HISTORY(("ps_print"));

  return TRUE;
}

static boolean readBG(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
BaseStd_t *base;
{
  return FALSE;
}

static boolean writeBG(This, doc)
c_BG_SectorText	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}



