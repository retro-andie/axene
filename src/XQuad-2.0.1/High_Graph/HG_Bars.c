/*
** HG_Bars.c for XQuad in High_Graph/
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
** Last update Wed May 29 15:40:25 1996 One of the authors
*/

#include "HG_Bars.h"
#include <stdarg.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void regenerate ___PROTO((c_HG_Bars *This, int ordonnee, 
				 bbox_t	bbox, c_HG_Axe *HG_Axe));
static void change_xinfo ___PROTO((c_HG_Bars *This));
static error change_base ___PROTO((c_HG_Bars *This));
static void display_redraw ___PROTO((c_HG_Bars *This));
static void add_to_vectorgraph ___PROTO((c_HG_Bars *This));
static void sub_to_vectorgraph ___PROTO((c_HG_Bars *This));
static void setHG ___PROTO((c_HG_Bars *This, ...));
static void vset();
static void calcul_bars ___PROTO((c_HG_Bars *This));
static void ps_print ___PROTO((c_HG_Bars *This, c_PostScript *post));
static boolean readHG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeHG ___PROTO((c_HG_Bars *This, c_DocFile *doc));
     
sf_HG_Bars fc_HG_Bars =
{
  cons, dest, copy, regenerate, 
  change_xinfo, change_base, display_redraw,
  add_to_vectorgraph, sub_to_vectorgraph, setHG,
  ps_print, readHG, writeHG
};

static void *cons(highg, sub_class)
c_HighGraph	*highg;
int		sub_class;
{
  c_HG_Bars	*This;
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("HG_Bars", sizeof(c_HG_Bars)) ) == NULL)
    return NULL;
  This->f = &fc_HG_Bars;
  
  This->highg = highg;
  This->class = HG_BARS;
  This->sub_class = sub_class;

  This->axe = NULL;
  This->separator = NULL;
  This->nb_bars = 0;
  This->ordonnee = NULL;
  
  Xc_TRACE(("constructor done"));
  return This;
}

static void dest(This)
c_HG_Bars	*This;
{
  int i;
  Xc_HISTORY(("destructor")); 
  if (This->ordonnee)
    BG_DELETE(This->ordonnee)(This->ordonnee);
  if (This->axe)
    BG_DELETE(This->axe)(This->axe);
  if (This->separator)
    BG_DELETE(This->separator)(This->separator);
  
  if (This->nb_bars)
  {
    for(i=0; i<This->nb_bars; i++)
    {
      BG_DELETE(This->box[i])(This->box[i]);
      BG_DELETE(This->string[i])(This->string[i]);
    }
    Xc_free(This->box);
    Xc_free(This->string);
  }
  Xc_free(This);
  Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_HG_Bars	*This;
BaseStd_t	*base;
{
  c_HG_Bars	*hg_copy;
  
  if ( (hg_copy = Xc_malloc("HG_Bars", sizeof(c_HG_Bars)) ) == NULL)
    return NULL;
  memcpy(hg_copy, This, sizeof(c_HG_Bars));
  
  return hg_copy;
}

static void regenerate(This, ordonnee, bbox, HG_Axe)
c_HG_Bars	*This;
int		ordonnee;
bbox_t		bbox;
c_HG_Axe	*HG_Axe;
{
  c_HighGraph	*highg;
  int		i, nbs;
  coord_t	width, height, absci;
  vector_t	point1, point2, size, origin, t_orig, t_size;
  char		*string;
  c_Color	*color;
  c_TextStyle	*tstyle;

  highg = This->highg;
  width = bbox.urx - bbox.llx;
  height = bbox.lly - bbox.ury;
  if (highg->abscisse && highg->d_abscisse)
    absci = SCALE_FROM_MILLIMETERS(15);
  else
    absci = (coord_t)0;
  
  if (This->ordonnee)
  {
    BG_DELETE(This->ordonnee)(This->ordonnee);
    This->ordonnee = NULL;
  }
  if (This->axe)
  {
    BG_DELETE(This->axe)(This->axe);
    This->axe = NULL;
  }
  if (This->separator)
  {
    BG_DELETE(This->separator)(This->separator);
    This->separator = NULL;
  }
  if (This->nb_bars)
  {
    for(i=0; i<This->nb_bars; i++)
    {
      BG_DELETE(This->box[i])(This->box[i]);
      BG_DELETE(This->string[i])(This->string[i]);
    }
    Xc_free(This->box);
    Xc_free(This->string);
    This->nb_bars = 0;
  }
  
  if (highg->ordonnee && highg->d_ordonnee)
  {
    This->ordonnee = (c_BG_Text *)NEW(c_BG_Text)(_BaseStd);
    string = highg->tab_ordonnee[ordonnee].string;
    tstyle = highg->tab_ordonnee[ordonnee].tstyle;
    size.dy = height;
    size.dx = XcText_SIZE_NOT_DEFINED;
    origin.dy = bbox.ury;
    origin.dx = bbox.urx - tstyle->point_size - SCALE_FROM_MILLIMETERS(1.5);
    BG_SET(This->ordonnee)
      (This->ordonnee,
       XcBG_Text_ORIGIN, origin,
       XcBG_Text_SIZE, size,
       XcBG_Text_STRING, string, 
       XcBG_Text_STYLE, tstyle,
       XcBG_Text_RULER, XqR_H_ALIGN_CENTER | XqR_V_ALIGN_CENTER |
       XqR_A_90 | XqR_MULTILINE,
       XcBG_Text_END);
  }
  
  This->axe = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
  point1.dy = bbox.lly;
  point2.dy = bbox.ury;
  point1.dx = point2.dx = bbox.llx + SCALE_FROM_MILLIMETERS(10) + absci;
  BG_SET(This->axe)(This->axe,
		    XcBG_Line_POINT1, point1,
		    XcBG_Line_POINT2, point2,     
		    XcBG_Line_END);
  
  This->separator = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
  point1.dy = point2.dy = bbox.ury;
  point1.dx = bbox.llx + SCALE_FROM_MILLIMETERS(10) + absci;
  point2.dx = point1.dx + SCALE_FROM_MILLIMETERS(1);
  BG_SET(This->separator)(This->separator,
			  XcBG_Line_POINT1, point1,
			  XcBG_Line_POINT2, point2,     
			  XcBG_Line_END);
  
  This->nb_bars = nbs = highg->nb_abscisse;
  This->box = (c_BG_Box **)Xc_malloc("bars box **", 
				     sizeof(c_BG_Box *)*nbs);
  This->string = (c_BG_Text **)Xc_malloc("bars string **", 
					sizeof(c_BG_Text *)*nbs);
  
  size.dy = height / (nbs + 1);
  origin.dy = bbox.lly - size.dy - size.dy / 2;
  origin.dx = F(HG_Axe).convert_value(HG_Axe, 0.0);
  
  t_orig.dx = bbox.llx;
  t_size.dy = XcText_SIZE_NOT_DEFINED;
  t_size.dx = point1.dx - SCALE_FROM_MILLIMETERS(1) - bbox.llx;

  for(i=0; i<highg->nb_abscisse; i++)
  {
    This->box[i]=(c_BG_Box *)NEW(c_BG_Box)(_BaseStd);
    size.dx = F(HG_Axe).convert_value(HG_Axe, highg->tab_value[ordonnee][i]) 
      - origin.dx;
    color = Get_BG_Color_Step(_BaseStd, i);
    BG_SET(This->box[i])(This->box[i],
			 XcBG_Box_POINT1, origin,
			 XcBG_Box_SIZE, size,
			 XcBG_Box_INS_FG_COLOR, color,
			 XcBG_Box_END);
    
    This->string[i]=(c_BG_Text *)NEW(c_BG_Text)(_BaseStd);
    if (highg->abscisse && highg->d_abscisse)
    {
      string = highg->tab_abscisse[i].string;
      tstyle = highg->tab_abscisse[i].tstyle;
    }
    else
    {
      string = NULL;
      tstyle = highg->tab_data.tstyle;
    }
    t_orig.dy = origin.dy + size.dy / 2 - tstyle->point_size / 2;
    BG_SET(This->string[i])(This->string[i],
			    XcBG_Text_ORIGIN, t_orig,
			    XcBG_Text_SIZE, t_size,
			    XcBG_Text_STRING, string,
			    XcBG_Text_STYLE, tstyle,
			    XcBG_Text_RULER, XqR_H_ALIGN_RIGHT,
			    XcBG_Text_END); 
    origin.dy -= size.dy;
  }
}

static void change_xinfo(This)
c_HG_Bars	*This;
{
  int i;
  
  if (This->ordonnee)
    BG_CHANGE_XINFO(This->ordonnee)(This->ordonnee, &This->highg->X_info);
  if (This->axe)
    BG_CHANGE_XINFO(This->axe)(This->axe, &This->highg->X_info);
  if (This->separator)
    BG_CHANGE_XINFO(This->separator)(This->separator, &This->highg->X_info);
  
  for(i=0; i<This->nb_bars; i++)
  {
    BG_CHANGE_XINFO(This->box[i])(This->box[i], &This->highg->X_info);
    BG_CHANGE_XINFO(This->string[i])(This->string[i], &This->highg->X_info);
  }
}

static error change_base(This)
c_HG_Bars	*This;
{
  return XC_NO_ERROR;
}

static void display_redraw(This)
c_HG_Bars	*This;
{
  int i;
  c_HighGraph	*highg;
  
  Xc_TRACE(("redraw camembert"));
  
  highg = This->highg;
  if (This->ordonnee)
    BG_DISPLAY_REDRAW(This->ordonnee)(This->ordonnee, 
				      highg->matrix, highg->scale, 0, 0);
  if (This->axe)
    BG_DISPLAY_REDRAW(This->axe)(This->axe, highg->matrix, highg->scale, 0, 0);
  if (This->separator)
    BG_DISPLAY_REDRAW(This->separator)(This->separator, 
				       highg->matrix, highg->scale, 0, 0);
  for(i=0; i<This->nb_bars; i++)
  {
    BG_DISPLAY_REDRAW(This->box[i])(This->box[i], highg->matrix,
				       highg->scale, 0, 0);
    BG_DISPLAY_REDRAW(This->string[i])(This->string[i], highg->matrix,
				       highg->scale, 0, 0);
  }
}

static void add_to_vectorgraph(This)
c_HG_Bars  *This;
{
  int	i;
  
  if (This->ordonnee)
    BG_ADD_TO_VECTORGRAPH(This->ordonnee)(This->ordonnee, 
					  This->highg->vectorg);
  if (This->axe)
    BG_ADD_TO_VECTORGRAPH(This->axe)(This->axe, This->highg->vectorg);
  if (This->separator)
    BG_ADD_TO_VECTORGRAPH(This->separator)(This->separator, 
					   This->highg->vectorg);

  for(i=0; i<This->nb_bars; i++)
  {
    BG_ADD_TO_VECTORGRAPH(This->box[i])(This->box[i], This->highg->vectorg);
    BG_ADD_TO_VECTORGRAPH(This->string[i])(This->string[i], 
					   This->highg->vectorg);
  }
}

static void sub_to_vectorgraph(This)
c_HG_Bars  *This;
{
  int	i;
  
  if (This->ordonnee)
    BG_SUB_TO_VECTORGRAPH(This->ordonnee)(This->ordonnee, 
					  This->highg->vectorg);
  if (This->axe)
    BG_SUB_TO_VECTORGRAPH(This->axe)(This->axe, This->highg->vectorg);
  if (This->separator)
    BG_SUB_TO_VECTORGRAPH(This->separator)(This->separator, 
					   This->highg->vectorg);
  
  for(i=0; i<This->nb_bars; i++)
  {
    BG_SUB_TO_VECTORGRAPH(This->box[i])(This->box[i], This->highg->vectorg);
    BG_SUB_TO_VECTORGRAPH(This->string[i])(This->string[i],
					   This->highg->vectorg);
  }
}

static void setHG(c_HG_Bars *This, ...)
{
  va_list ap;

  Xc_HISTORY(("set"));

  va_start(ap, This);
  vset(This, ap);
  va_end(ap);
} 

static void vset(This, ap)
c_HG_Bars *This;
va_list ap;
{
  boolean out_flag;
  boolean calcul;
  
  Xc_TRACE(("vset"));
  
  out_flag = calcul = FALSE;
  do {
    switch(va_arg(ap, HG_Bars_set_code_t))
    {
    case XcHG_Bars_END:
      Xc_TRACE(("XcHG_Bars_END"));
      out_flag = TRUE;
      break;
    default:
      Xc_BREAK(("Unknow HG_Bars_set_code"));
      break;
    }
  } while(!out_flag);
  if (calcul)
    calcul_bars(This);
}

static void calcul_bars(This)
c_HG_Bars	*This;
{
}

/* -------------------------------------------------------------------- **
** ps print, read & write HG_Bars					**
** -------------------------------------------------------------------- */ 
static void ps_print(This, post)
c_HG_Bars *This;
c_PostScript *post;
{
}

static boolean readHG(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
BaseStd_t *base;
{
  return FALSE;
}

static boolean writeHG(This, doc)
c_HG_Bars	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}



