/*
** BG_Text.c for XQuad in Basic_Graph/
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
** Last update Wed Apr 23 13:41:31 1997 One of the authors
*/

#define NTRACE
#define NHISTORY

#include "BG_Text.h"
#include <stdarg.h>
#include <math.h>
#include "VectorFont.h"
#include "RasterFont.h"
#include "CharMetrics.h"
#include "FontManager.h" 

extern c_VectorFont	*GlobFontBase;
extern c_FontManager	*GlobFntMng;

static void *cons();
static void dest();
static void *copy();
static void change_xinfo ___PROTO((c_BG_Text *This, x_info *xinfo));
static error change_base ___PROTO((c_BG_Text *This, BaseStd_t *new_base));
static void display_redraw ___PROTO((c_BG_Text *This, c_Matrix *matrix,
				     coord_t scale, int dx, int dy));
static void display_handles ___PROTO((c_BG_Text *This, c_Matrix *matrix,
				      coord_t scale, int dx, int dy));
static void add_to_vectorgraph ___PROTO((c_BG_Text *This, 
					 c_VectorGraph *vectorg));
static void sub_to_vectorgraph ___PROTO((c_BG_Text *This, 
					 c_VectorGraph *vectorg));
static bbox_t *get_bounding_box ___PROTO((c_BG_Text *This));
static boolean is_in ___PROTO((c_BG_Text *This, coord_t x, coord_t y));
static void setBG ___PROTO((c_BG_Text *This, ...));
static void vset();
static void calcul_bbox ___PROTO((c_BG_Text *This));
static boolean ps_print ___PROTO((c_BG_Text *This, c_PostScript *post));
static boolean readBG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeBG ___PROTO((c_BG_Text *This, c_DocFile *doc));

typedef struct DS_line_s
{
 char		*string;
 int		length;
 coord_t	char_inc;
 coord_t	word_inc;
 coord_t	offset;
  
 struct DS_line_s	*Next;
} DS_line_t;

static int cut_text_for_multiline ___PROTO((DS_line_t	*DS_line,
					    coord_t	sWidth,
					    c_CharMetrics *metrics,
					    boolean	angle90));
static coord_t supress_space_and_get_width ___PROTO((DS_line_t *DS_line,
						     c_CharMetrics *metrics,
						     boolean angle90));
static void justify_DS_line ___PROTO((DS_line_t *DS_line, coord_t sWidth,
				      coord_t txt_width));

sf_BG_Text fc_BG_Text =
{
 cons, dest, copy,
 change_xinfo, change_base, display_redraw, display_handles,
 add_to_vectorgraph, sub_to_vectorgraph, get_bounding_box, is_in, setBG,
 ps_print, readBG, writeBG
};

static void *cons(base)
BaseStd_t	*base;
{
 c_BG_Text	*This;
  
 Xc_HISTORY(("constructor"));
  
 if ( (This = Xc_malloc("BG_Text", sizeof(c_BG_Text)) ) == NULL)
  return NULL;
 This->f = &fc_BG_Text;
  
 This->type = BG_TEXT;
 This->BaseStd = base;
  
 This->origin.dx = This->origin.dy = 0;
 This->size.dx = This->size.dy = XcText_SIZE_NOT_DEFINED;
 This->string = NULL;
 This->tstyle = Get_BG_TextStyle(base, BGS_NORMAL);
 XcH_REGISTER(This->tstyle, NULL, NULL);
 This->ruler = XqR_DEFAULT;
  
 This->rfont = (c_RasterFont *)NULL;
 This->metrics = (c_CharMetrics *)NULL;
 This->factor = (scale_t)0;
  
 Xc_TRACE(("constructor done"));
 return This;
}

static void dest(This)
c_BG_Text	*This;
{
 Xc_HISTORY(("destructor"));
 if (*(This->tstyle->base) == This->BaseStd->BG_text_style_base)
  XcH_UNREGISTER(This->tstyle, NULL, NULL);
 This->ruler = XqR_DEFAULT;
 if (This->rfont)
  F(GlobFntMng).delete_font(GlobFntMng, This->rfont);
 Xc_free(This);
 Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_BG_Text	*This;
BaseStd_t	*base;
{
 c_BG_Text	*bg_copy;
  
 if ( (bg_copy = Xc_malloc("BG_Text", sizeof(c_BG_Text)) ) == NULL)
  return NULL;
 memcpy(bg_copy, This, sizeof(c_BG_Text));
  
 if (bg_copy->BaseStd != base)
  BG_CHANGE_BASE(bg_copy)(bg_copy, base);
 return bg_copy;
}

static void change_xinfo(This, xinfo)
c_BG_Text	*This;
x_info		*xinfo;
{
 memcpy(&This->X_info, xinfo, sizeof(x_info));
}

static error change_base(This, new_base)
c_BG_Text	*This;
BaseStd_t	*new_base;
{
 This->BaseStd = new_base;
 return XC_NO_ERROR;
}

static void display_redraw(This, matrix, scale, dx, dy)
c_BG_Text	*This;
c_Matrix	*matrix;
coord_t		scale;
int		dx;
int		dy;
{
 Display	*display;
 Window	window;
 GC		gc;
 GC		gc_mask = (GC)0;
 matrix_t	*m;
 char		*value;
 int		length;
 ruler_t	ruler;
 coord_t	txt_width;
 coord_t	txt_height;
 coord_t	font_height;
 coord_t	sWidth;
 coord_t	sHeight;
 coord_t	sX;
 coord_t	sY;
 coord_t	sY_inc;
 coord_t	sY_cumul;
 coord_t	ascend, descend;
 coord_t	rWidth, rHeight;
 coord_t	sTmp;
 int		i;
 c_TextStyle	*tstyle;
 vector_t	pos, pos2;
 int		nb_DS_line;
 DS_line_t	*DS_line;
 DS_line_t	*DS_line2;
 angle_t	angle;
 scale_t	factor;
 boolean	angle90;
  
 value = This->string;
 if(!value) return;

 Xc_HISTORY(("display_redraw(`%s')", This->string));

 display = _display;
 window = _window;
 gc = _gc_inside;
 m = F(matrix).get(matrix, 0);
  
 tstyle = This->tstyle;
 ruler = This->ruler;
 factor = (scale_t)( ( (double)COORD_SCALEVALUE *
		      (double)SCALE_FROM_PERCENT(100) )/ (double)scale );
  
 /* ---------------------------------------------------------------- */
 /* ----------  test if the font must be rasterized  --------------- */
 /* ---------------------------------------------------------------- */
  
 switch(ruler & XqR_A_MASK)
 {
 case XqR_A_90:
  angle = SCALE_FROM_DEGREES(90);
  angle90 = TRUE;
  break;
 case XqR_A_270:
  angle = SCALE_FROM_DEGREES(270);
  angle90 = TRUE;
  break;
 case XqR_A_180: 
  angle = ANGLE_ZERO;
  angle90 = TRUE;
  break;
 case XqR_A_DEFAULT:
 default:
  angle = ANGLE_ZERO;
  angle90 = FALSE;
  break;
 }
 if (!This->rfont || This->metrics->angle != angle || 
     This->factor != factor)
 {
  Xc_TRACE(("regenerate"));
      
  This->metrics = F(tstyle->metrics).transform(tstyle->metrics, factor,
					       XcTS_GET_WIDTH(tstyle),
					       angle);
  if (This->rfont)
  {
   F(GlobFntMng).delete_font(GlobFntMng, This->rfont);
   This->rfont = NULL;
  }
  This->metrics->raster_font = 
   F(GlobFntMng).new_font(GlobFntMng, tstyle->font, This->metrics);
      
  if((This->rfont = This->metrics->raster_font) == NULL)
  {
   F(tstyle->metrics).purgeImpure(tstyle->metrics, factor,
				  XcTS_GET_WIDTH(tstyle), angle);
   return;
  }
  This->factor = factor;
 }
 F(This->rfont).setGC(display, window, &gc_mask, gc);
 F(tstyle->color).setGC(tstyle->color, display, gc);
  
 /* ------------------------------------------------------------------- */
 /* -------------------  miscellaneous calculs ------------------------ */
 /* ------------------------------------------------------------------- */
 length = strlen(value);
  
 if (!angle90)
  txt_width = F(This->metrics).textWidth(This->metrics, value, length);
 else
  txt_width = ABS(F(This->metrics).textHeight(This->metrics,
					      value, length));
 ascend = This->metrics->ascender;
 descend = This->metrics->descender;
 txt_height = font_height = ascend - descend;

 rWidth = This->size.dx;
 rHeight = This->size.dy;
 if (rWidth != XcText_SIZE_NOT_DEFINED)
  if (rHeight != XcText_SIZE_NOT_DEFINED)
  {
   sWidth =  SCALE_FROM_POINTS((int)XcM_DX(m, rWidth, 
					   rHeight));
   sHeight = SCALE_FROM_POINTS((int)XcM_DY(m, rWidth,
					   rHeight));
  }
  else
  {
   sWidth =  SCALE_FROM_POINTS((int)XcM_DX(m, rWidth, 
					   BG_AREA_HEIGHT));
   sHeight =  SCALE_FROM_POINTS((int)XcM_DY(m, rWidth, 
					    BG_AREA_HEIGHT));
  }
 else
  if (rHeight != XcText_SIZE_NOT_DEFINED)
  {
   sWidth =  SCALE_FROM_POINTS((int)XcM_DX(m, BG_AREA_WIDTH, 
					   rHeight));
   sHeight =  SCALE_FROM_POINTS((int)XcM_DY(m, BG_AREA_WIDTH, 
					    rHeight));
  }
  else
  {
   sWidth =  SCALE_FROM_POINTS((int)XcM_DX(m, BG_AREA_WIDTH, 
					   BG_AREA_HEIGHT));
   sHeight =  SCALE_FROM_POINTS((int)XcM_DY(m, BG_AREA_WIDTH, 
					    BG_AREA_HEIGHT));
  }
  
 if (angle != ANGLE_ZERO)
 {
  SWAP(coord_t, sWidth, sHeight);
  SWAP(coord_t, rWidth, rHeight);
 }
  
 sX = SCALE_FROM_POINTS((int)XcM_X(m, This->origin.dx, This->origin.dy) + dx);
 sY = SCALE_FROM_POINTS((int)XcM_Y(m, This->origin.dx, This->origin.dy) + dy);
 sY_inc = 0;
  
 DS_line = (DS_line_t *) Xc_malloc("ds_line base", sizeof(DS_line_t));
 memset(DS_line, 0, sizeof(DS_line_t));
 DS_line->char_inc = DS_line->word_inc = DS_line->offset = (coord_t)0; 
 nb_DS_line = 1;
 DS_line->string = value;
 DS_line->length = length;
  
 if (rWidth != XcText_SIZE_NOT_DEFINED || (ruler & XqR_MULTILINE)) 
  nb_DS_line = cut_text_for_multiline(DS_line, sWidth, This->metrics,
				      angle90);
  
 pos.dx = sX;
 if (rWidth != XcText_SIZE_NOT_DEFINED)
 {
  switch(ruler & XqR_H_ALIGN_MASK)
  {
  case XqR_DEFAULT:	
  case XqR_H_ALIGN_LEFT: 
   Xc_TRACE(("XqR_H_ALIGN_LEFT"));
   DS_line2 = DS_line;
   for(i=0; i<nb_DS_line; i++)
   {
    supress_space_and_get_width(DS_line2, This->metrics, angle90);
    DS_line2 = DS_line2->Next;
   }
   break;
  case XqR_H_ALIGN_RIGHT:
   Xc_TRACE(("XqR_H_ALIGN_RIGHT"));
   DS_line2 = DS_line;
   for(i=0; i<nb_DS_line; i++)
   {
    txt_width = supress_space_and_get_width(DS_line2, 
					    This->metrics, angle90);
    DS_line2->offset = sWidth-txt_width;
    DS_line2 = DS_line2->Next;
   }
   break;
  case XqR_H_ALIGN_CENTER:
   Xc_TRACE(("XqR_H_ALIGN_CENTER"));
   DS_line2 = DS_line;
   for(i=0; i<nb_DS_line; i++)
   {
    txt_width = supress_space_and_get_width(DS_line2, 
					    This->metrics, angle90);
    DS_line2->offset = (sWidth-txt_width)>>1;
    DS_line2 = DS_line2->Next;
   }
   break;
  case XqR_H_JUSTIFIED:
   Xc_TRACE(("XqR_H_JUSTIFIED"));
   DS_line2 = DS_line;
   for(i=0; i<nb_DS_line; i++)
   {
    txt_width = supress_space_and_get_width(DS_line2, 
					    This->metrics, angle90);
    justify_DS_line(DS_line2, sWidth, txt_width);
    DS_line2 = DS_line2->Next;
   }
   break;
  }
 }
  
 txt_height = font_height * nb_DS_line;
 if (rHeight != XcText_SIZE_NOT_DEFINED)
 {
  switch(ruler & XqR_V_ALIGN_MASK)
  {
  case XqR_V_ALIGN_CENTER:
   pos.dy = sY + ((sHeight-txt_height)>>1);
   break;
  case XqR_V_ALIGN_TOP:
   pos.dy = sY;
   break;
  case XqR_V_ALIGN_BOTTOM:
   pos.dy = sY + sHeight - txt_height;
   break;
  case XqR_V_JUSTIFIED: 
   if (nb_DS_line == 1 || 
       (sY_inc = ((sHeight - txt_height) / (nb_DS_line - 1))) 
       < font_height)
   {
    pos.dy = sY + ((sHeight-txt_height)>>1);
    sY_inc = 0;
   }
   else
    pos.dy = sY;
   break;
  }
 }
 else
  pos.dy = sY;

 sY_cumul = 0;
 for(i=0; i<nb_DS_line; i++)
 {    
  if ( !i || (ruler & XqR_MULTILINE))
  {
   pos2.dx = pos.dx + DS_line->offset;
   pos2.dy = pos.dy + sY_cumul + ascend;
   if (angle90)
   {
    if (angle == SCALE_FROM_DEGREES(90))
    {
     sTmp = pos2.dx;
     pos2.dx = sX + pos2.dy - sY;
     pos2.dy = (sY+sWidth)-(sTmp-sX);
    }
    else
    {
     sTmp = pos2.dx;
     pos2.dx = (sX+sHeight)-(pos2.dy-sY);
     pos2.dy = sY + (sTmp - sX);
    }
   }
   F(This->rfont).drawString(This->rfont, display, window,
			     gc_mask, gc, &pos2, 
			     DS_line->char_inc, DS_line->word_inc,
			     DS_line->string, DS_line->length, tstyle); 
      
   sY_cumul += sY_inc + font_height;
  }
  DS_line2 = DS_line;
  DS_line = DS_line->Next;
  Xc_free(DS_line2);
 }
  
 if (gc_mask)
  XFreeGC(display, gc_mask);
}

static void display_handles(This, matrix, scale, dx, dy)
c_BG_Text	*This;
c_Matrix	*matrix;
coord_t		scale;
int		dx;
int		dy;
{
 Display	*display;
 Window	window;
 GC		gc;
  
 display = _display;
 window = _window;
 gc = _gc_inside;
}

static void add_to_vectorgraph(This, vectorg)
c_BG_Text  *This;
c_VectorGraph *vectorg;
{
 F(vectorg).addBasicGraph(vectorg, This);
}

static void sub_to_vectorgraph(This, vectorg)
c_BG_Text  *This;
c_VectorGraph *vectorg;
{
 F(vectorg).subBasicGraph(vectorg, This);
}

static bbox_t *get_bounding_box(This)
c_BG_Text *This;
{
 return &(This->bbox);
}

static boolean is_in(This, x, y)
c_BG_Text	*This;
coord_t		x;
coord_t		y;
{
 if (x >= This->bbox.llx && x <= This->bbox.urx &&
     y >= This->bbox.lly && y <= This->bbox.ury)
  return TRUE;
 return FALSE;
}

static void setBG(c_BG_Text *This, ...)
{
 va_list ap;

 Xc_HISTORY(("set"));

 va_start(ap, This);
 vset(This, ap);
 va_end(ap);
} 

static void vset(This, ap)
c_BG_Text *This;
va_list ap;
{
 boolean out_flag;
 boolean calcul = FALSE;
 Xc_TRACE(("vset"));
  
 out_flag = FALSE;
 do {
     switch(va_arg(ap, BG_Text_set_code_t))
     {
     case XcBG_Text_ORIGIN:
      Xc_TRACE(("XcBG_Text_ORIGIN"));
      This->origin = va_arg(ap, vector_t);
      calcul = TRUE;
      break;
     case XcBG_Text_WIDTH:
      Xc_TRACE(("XcBG_Text_WIDTH"));
      This->size.dx = va_arg(ap, coord_t);
      calcul = TRUE;
      break;
     case XcBG_Text_HEIGHT:
      Xc_TRACE(("XcBG_Text_HEIGHT"));
      This->size.dy = va_arg(ap, coord_t);
      calcul = TRUE;
      break;
     case XcBG_Text_SIZE:
      Xc_TRACE(("XcBG_Text_SIZE"));
      This->size = va_arg(ap, vector_t);
      calcul = TRUE;
      break;
     case XcBG_Text_STRING:
      Xc_TRACE(("XcBG_Text_STRING"));
      This->string = va_arg(ap, char *);
      calcul = TRUE;
      break;
     case XcBG_Text_STYLE:
      Xc_TRACE(("XcBG_Text_STYLE"));
      if (*(This->tstyle->base) == This->BaseStd->BG_text_style_base)
       XcH_UNREGISTER(This->tstyle, NULL, NULL);
      This->tstyle = va_arg(ap, c_TextStyle *);
      if (*(This->tstyle->base) == This->BaseStd->BG_text_style_base)
       XcH_REGISTER(This->tstyle, NULL, NULL);
      calcul = TRUE;
      break;
     case XcBG_Text_RULER:
      Xc_TRACE(("XcBG_Text_RULER"));
      This->ruler = va_arg(ap, ruler_t);
      calcul = TRUE;
      break;
     case XcBG_Text_END:
      Xc_TRACE(("XcBG_Text_END"));
      out_flag = TRUE;
      break;
     default:
      Xc_BREAK(("Unknow BG_Text_set_code"));
      break;
     }
    } while(!out_flag);
 if (calcul)
  calcul_bbox(This);
}

static void calcul_bbox(This)
c_BG_Text	*This;
{
 if (!This->string)
 {
  This->bbox.llx = This->bbox.lly = This->bbox.urx = This->bbox.ury = 0;
  return;
 }
  
 This->bbox.llx = This->origin.dx;
 This->bbox.lly = This->origin.dy;
  
 if (This->size.dx == XcText_SIZE_NOT_DEFINED ||
     This->size.dy == XcText_SIZE_NOT_DEFINED)
 {
  switch(This->ruler & XqR_A_MASK)
  {
  case XqR_A_DEFAULT:
   This->bbox.urx = This->bbox.llx + 
    F(This->tstyle->metrics).textWidth(This->tstyle->metrics, 
				       This->string, 
				       strlen(This->string));
   This->bbox.ury = This->bbox.lly + This->tstyle->metrics->ascender -
    This->tstyle->metrics->descender;
   break;
  case XqR_A_90:
  case XqR_A_270:
  case XqR_A_180: 
   This->bbox.urx = This->bbox.llx + This->tstyle->metrics->ascender -
    This->tstyle->metrics->descender;
   This->bbox.ury = This->bbox.lly + 
    F(This->tstyle->metrics).textHeight(This->tstyle->metrics, 
					This->string, 
					strlen(This->string));
   break;
  }
 }
 else
 {
  This->bbox.urx = This->bbox.llx + This->size.dx;
  This->bbox.ury = This->bbox.lly + This->size.dy;
 }
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
c_BG_Text	 *This;
c_DocFile	 *doc;
{
 return FALSE;
}


/* -------------------------------------------------------------------- **
** functions for alignment management					**
** -------------------------------------------------------------------- */

static int cut_text_for_multiline(DS_line, sWidth, metrics, angle90)
DS_line_t	*DS_line;
coord_t		sWidth;
c_CharMetrics	*metrics;
boolean		angle90;
{
 coord_t	width_cumul;
 int		prev_space;
 int		i;
 int		cur_length;
 int		nb_DS_line;
 DS_line_t	*DS_line2;
 char	*txt;
 int	length;
  
 width_cumul = cur_length = 0;
 prev_space = -1;
 txt = DS_line->string;
 length = DS_line->length;
  
 nb_DS_line = 1;
 DS_line2 = DS_line;
 for(i=0; i<length; i++)
 {
  if (txt[i]==' ')
   prev_space = i;
  width_cumul += ((!angle90)?
		  F(metrics).textWidth(metrics, txt+i, 1):
		  ABS(F(metrics).textHeight(metrics, txt+i, 1)));
    
  if (width_cumul > sWidth)
  {
   if (!cur_length) return nb_DS_line;
   if (prev_space != -1)
   {
    DS_line2->Next = (DS_line_t *)Xc_malloc("ds_line", sizeof(DS_line_t)); 
    if (i == prev_space)
     DS_line2->length = cur_length;
    else
     DS_line2->length = cur_length - ((i-1) - prev_space);
    DS_line2 = DS_line2->Next;
    memset(DS_line2, 0, sizeof(DS_line_t));
    DS_line2->char_inc = DS_line2->word_inc = 
     DS_line2->offset = (coord_t)0; 
    DS_line2->string = txt + prev_space+1;
    DS_line2->length = length - prev_space + 1;
    nb_DS_line ++;
    width_cumul = 0;
    i = prev_space;
    prev_space = -1;
    cur_length = 0;
    continue;
   }
   else
   {
    DS_line2->Next = (DS_line_t *)Xc_malloc("ds_line", sizeof(DS_line_t)); 
    DS_line2->length = cur_length;

    DS_line2 = DS_line2->Next;
    memset(DS_line2, 0, sizeof(DS_line_t));
    DS_line2->char_inc = DS_line2->word_inc = 
     DS_line2->offset = (coord_t)0; 
    DS_line2->string = txt + i;
    DS_line2->length = length - i;
    nb_DS_line ++;
    width_cumul = 0;
    cur_length = 0;
   }
  }
  cur_length ++;
 }
 return nb_DS_line;
}

static coord_t supress_space_and_get_width(DS_line, metrics, angle90)
DS_line_t	*DS_line;
c_CharMetrics	*metrics;
boolean		angle90;
{
 int	nb_space_begin;
 int	nb_space_end;
 char	*txt;
 int	length;
  
 if (!DS_line->length) return 0;
  
 nb_space_begin = nb_space_end = 0;
 txt = DS_line->string;
 length = DS_line->length;
  
 while(nb_space_begin<length)
 {
  if (txt[nb_space_begin]!=' ')
   break;
  nb_space_begin ++;
 }
 if (nb_space_begin == length)
  return 0;
  
 while(txt[length-nb_space_end-1] == ' ')
  nb_space_end ++;
  
 DS_line->string = txt+nb_space_begin;
 DS_line->length = length - nb_space_begin - nb_space_end;
 return ( (!angle90) ? 
	 F(metrics).textWidth(metrics, DS_line->string, DS_line->length):
	 ABS(F(metrics).textHeight(metrics, DS_line->string,
				   DS_line->length)));
}

static void justify_DS_line(DS_line, sWidth, txt_width)
DS_line_t	*DS_line;     
coord_t		sWidth;
coord_t	txt_width;
{
 int		nb_space;
 int		nb_char;
 coord_t	delta_x;
 coord_t	prop;
 coord_t	char_inc;
 coord_t	word_inc;
 char		*txt;
 int		length;
 int		i;
  
 nb_char = nb_space = 0;
 txt = DS_line->string;
 length = DS_line->length;
  
 for(i=0; i<length; i++)
 {
  if (txt[i]==' ')
   nb_space ++;
  else
   nb_char ++;
 }
 nb_char += nb_space - 1;	/* last char with no space at his right */
  
 delta_x = sWidth - txt_width;

 if (nb_space)
 {
  prop = (delta_x * 70 / 100);
  word_inc = prop / nb_space;
  delta_x -= prop;
 }
 else
  word_inc = 0;
 if (nb_char)
  char_inc = delta_x / nb_char;
 else
  char_inc = 0;
  
 DS_line->char_inc = char_inc;
 DS_line->word_inc = word_inc;
}


/* ----------------------------------------------------------------- ** 
** ps_print - Print text                                             ** 
** ----------------------------------------------------------------- */
static boolean ps_print(this, post)
c_BG_Text *this;
c_PostScript *post;
{
 coord_t width, height, x, y, ws, cs, y_inc;
 c_CharMetrics *metrics;
 char *ptr, *end, *text;
 c_TextStyle *style;
 boolean init_flag;
 long len, max_len;
 int line_count;
 ruler_t ruler;
 coord_t x0;
 boolean	shad;
 coord_t	add_y;

 Xc_HISTORY(("ps_print"));
 Xc_ASSERT(post->state.text_flag == FALSE);

 add_y = 0;
 x0 = 0;
 style = this->tstyle;
 metrics = style->metrics;

 /*--- Remove spaces from left & right of text ---*/
 if((text = this->string) == NULL) return TRUE;
 Xc_TRACE(("string = `%s'", text));

 while(*text == 32) text++;
 if((max_len = strlen(text)) == 0) return TRUE;
 while((max_len > 0) && (text[max_len - 1] == 32)) max_len--;
 if(max_len == 0) return TRUE;
 end = text + max_len;

 /*--- Get ruler ---*/
 ruler = this->ruler;

 /*--- Rotation & co ---*/
 if(((ruler & XqR_A_MASK) == XqR_A_90) || ((ruler & XqR_A_MASK) == XqR_A_270))
 {
  height = this->size.dx;
  width = this->size.dy;
 }
 else
 {
  width = this->size.dx;
  height = this->size.dy;
 }

 if(width == XcText_SIZE_NOT_DEFINED)
 {
  Xc_TRACE(("width is undefined"));
  ruler &= ~XqR_H_ALIGN_MASK;
 }

 if(height == XcText_SIZE_NOT_DEFINED)
 {
  Xc_TRACE(("height is undefined"));
  ruler &= ~XqR_V_ALIGN_MASK;
 }


 /*--- Text dimensions ---*/
 if((ruler & XqR_MULTILINE) || (width != XcText_SIZE_NOT_DEFINED))
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

 Xc_TRACE(("size=%gx%g line_count = %d", SCALE_TO_POINTS(width),
	   SCALE_TO_POINTS(height), line_count));

 /*--- Vertical alignment ---*/
 y = (height - ((line_count - 1) * style->point_size) - 
      (metrics->font_bbox.ury - metrics->font_bbox.lly + 1));

 y_inc = style->point_size;
 switch(ruler & XqR_V_ALIGN_MASK)
 {
 case XqR_V_ALIGN_CENTER:
  Xc_TRACE(("XqR_V_ALIGN_CENTER"));
  y >>= 1;
  break;
 case XqR_V_JUSTIFIED: 
  Xc_TRACE(("XqR_V_JUSTIFIED"));
  if(line_count == 1)
  {
   y >>= 1;
   break;
  }
  y_inc += y / (line_count - 1);
 case XqR_V_ALIGN_TOP:
  Xc_TRACE(("XqR_V_ALIGN_TOP"));
  y = COORD_ZERO;
  break;
 case XqR_V_ALIGN_BOTTOM:
  Xc_TRACE(("XqR_V_ALIGN_BOTTOM"));
  break;
 }
 y += metrics->font_bbox.ury;
  
 /*--- init text rise ---*/
 post->state2[NORMAL_TEXT].text_rise = 0;
 post->state2[SHADOW_TEXT].text_rise = 0;
 /*--- init color ---*/
 post->state.stroke_color = NULL;
 post->state.fill_color = NULL; 
 /*--- init font ---*/
 post->state2[SHADOW_TEXT].font = NULL; 
 post->state2[NORMAL_TEXT].font = NULL; 

 /*--- Line loop ---*/
 ws = cs = COORD_ZERO;
 init_flag = FALSE;
 while(line_count)
 {
  if(ruler & XqR_MULTILINE)
  {
   len = F(metrics).calibrate(metrics, width, &text, end);
   Xc_ASSERT(len != 0);
  }
  else
   len = max_len;
  
  x = width - F(metrics).textWidth(metrics, text, len);
      
  /*--- Horizontal alignment ---*/
  Xc_TRACE(("horizontal: %d", (int)(ruler & XqR_H_ALIGN_MASK)));
  switch(ruler & XqR_H_ALIGN_MASK)
  {
  case XqR_DEFAULT:
   Xc_TRACE(("XqR_DEFAULT"));
  case XqR_H_ALIGN_LEFT:
   Xc_TRACE(("XqR_H_ALIGN_LEFT"));
   x = COORD_ZERO;
   break;
  case XqR_H_ALIGN_RIGHT:
   Xc_TRACE(("XqR_H_ALIGN_RIGHT"));
   break;
  case XqR_H_ALIGN_CENTER:
   Xc_TRACE(("XqR_H_ALIGN_CENTER"));
   x >>= 1;
   break;
  case XqR_H_JUSTIFIED:
   {
    int count;
	    
    Xc_TRACE(("XqR_H_JUSTIFIED"));
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
    if((line_count != 1) && (post->state.leading != y_inc))
    {
     F(post).putLine(post, "$c TL", y_inc);
     post->state.leading = y_inc;
    }
   }
	  
   /*--- Set matrix ---*/
   F(post->matrix).translate(post->matrix, TRUE,
			     this->origin.dx, this->origin.dy);
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
/*post->state.leading = y_inc;*/
    F(post->matrix).translate(post->matrix, FALSE, x0,
			      y - (post->ai_mode? COORD_ZERO:y_inc));
   }
   else
    F(post->matrix).translate(post->matrix, FALSE, x0,
			      y - (post->ai_mode?
				  COORD_ZERO:post->state.leading));
	  
   F(post->matrix).scale(post->matrix, FALSE, SCALE_ONE, -SCALE_ONE);

/* TODO   F(post).putLine(post, post->ai_mode? "0 To $m 0 Tp TP":"BT $m Tm",
		   F(post->matrix).get(post->matrix, 0));
*/
   post->state.text_flag = TRUE;
	  
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
   add_y += post->state.leading;
  }
  else
  {
   add_y += post->state.leading;
   F(style).ps_print_text(style, text, len, post, 
			  x-x0, -add_y,
			  "'");
  }

  /*--- Update variables ---*/
  y = x;
  line_count--;
  text += len;
 }

 /* reinit tw and tc if changed */
 if((post->state.word_space != 0) || (post->state.char_space != 0))
 {
  F(post).putLine2(post, SHADOW_TEXT, "$c$s $c$s", 
		   0, " Tw", 0, " Tc");
  F(post).putLine2(post, NORMAL_TEXT, "$c$s $c$s", 
		   0, " Tw", 0, " Tc");
 }

#if 0
 Xc_TRACE(("ps_print ok"));
 return F(post).putLine(post, post->ai_mode? "TO":"ET");
#endif

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
 post->state2[NORMAL_TEXT].stroke_color = NULL;
 post->state2[NORMAL_GRAPH].stroke_color = NULL;
 post->state2[SHADOW_TEXT].stroke_color = NULL;
 post->state2[SHADOW_GRAPH].stroke_color = NULL;
 post->state2[NORMAL_TEXT].fill_color = NULL;
 post->state2[NORMAL_GRAPH].fill_color = NULL;
 post->state2[SHADOW_TEXT].fill_color = NULL;
 post->state2[SHADOW_GRAPH].fill_color = NULL;
 F(post->matrix).pop(post->matrix);
 post->state.text_flag = FALSE;
 return TRUE;
}


