/*
** HG_Curve.c for XQuad in High_Graph/
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

#include "HG_Curve.h"
#include <stdarg.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void regenerate ___PROTO((c_HG_Curve *This, bbox_t bbox,
				 c_HG_Axe *HG_Axe));
static void change_xinfo ___PROTO((c_HG_Curve *This));
static error change_base ___PROTO((c_HG_Curve *This));
static void display_redraw ___PROTO((c_HG_Curve *This));
static void add_to_vectorgraph ___PROTO((c_HG_Curve *This));
static void sub_to_vectorgraph ___PROTO((c_HG_Curve *This));
static void setHG ___PROTO((c_HG_Curve *This, ...));
static void vset();
static void calcul_curve ___PROTO((c_HG_Curve *This));
static void ps_print ___PROTO((c_HG_Curve *This, c_PostScript *post));
static boolean readHG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeHG ___PROTO((c_HG_Curve *This, c_DocFile *doc));
     
sf_HG_Curve fc_HG_Curve =
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
  c_HG_Curve	*This;
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("HG_Curve", sizeof(c_HG_Curve)) ) == NULL)
    return NULL;
  This->f = &fc_HG_Curve;
  
  This->highg = highg;
  This->class = HG_CURVE;
  This->sub_class = sub_class;

  This->axe = NULL;
  This->nb_curve = 0;
  This->ordonnee = NULL;
  
  This->nb_abscisse = 0;
  This->graduation = NULL;
  This->string = NULL;
  
  Xc_TRACE(("constructor done"));
  return This;
}

static void dest(This)
c_HG_Curve	*This;
{
  int i;
  Xc_HISTORY(("destructor")); 
  if (This->axe)
    BG_DELETE(This->axe)(This->axe);
  if (This->nb_curve)
  {
    for(i=0; i<This->nb_curve; i++)
    {
      if (This->ordonnee)
	BG_DELETE(This->ordonnee[i])(This->ordonnee[i]);
      BG_DELETE(This->curve[i])(This->curve[i]);
    }
    Xc_free(This->curve);
    if (This->ordonnee)
      Xc_free(This->ordonnee);
  }
  if (This->nb_abscisse)
  {
    for(i=0; i<This->nb_abscisse; i++)
    {
      BG_DELETE(This->graduation[i])(This->graduation[i]);
      BG_DELETE(This->string[i])(This->string[i]);
    }
    Xc_free(This->graduation);
    Xc_free(This->string);
  }
  Xc_free(This);
  Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_HG_Curve	*This;
BaseStd_t	*base;
{
  c_HG_Curve	*hg_copy;
  
  if ( (hg_copy = Xc_malloc("HG_Curve", sizeof(c_HG_Curve)) ) == NULL)
    return NULL;
  memcpy(hg_copy, This, sizeof(c_HG_Curve));
  
  return hg_copy;
}

static void regenerate(This, bbox, HG_Axe)
c_HG_Curve	*This;
bbox_t		bbox;
c_HG_Axe	*HG_Axe;
{
  c_HighGraph	*highg;
  int		i, j, nbs, nbc;
  coord_t	width, height, absci, ordo;
  vector_t	point1, point2, size, origin, t_orig, t_size;
  char		*string;
  c_Color	*color;
  c_TextStyle	*tstyle;
  boolean	one_absc;
  
  highg = This->highg;
  width = bbox.urx - bbox.llx;
  height = bbox.lly - bbox.ury;
  if (highg->abscisse && highg->d_abscisse)
    absci = SCALE_FROM_MILLIMETERS(15);
  else
    absci = (coord_t)0;
  
  if (highg->ordonnee && highg->d_ordonnee)
    ordo = SCALE_FROM_MILLIMETERS(10);
  else
    ordo = (coord_t)0;
  
  if (This->axe)
  {
    BG_DELETE(This->axe)(This->axe);
    This->axe = NULL;
  }
  
  if (This->nb_curve)
  {
    for(i=0; i<This->nb_curve; i++)
    {
      if (This->ordonnee)
	BG_DELETE(This->ordonnee[i])(This->ordonnee[i]);
      BG_DELETE(This->curve[i])(This->curve[i]);
    }
    if (This->ordonnee)
      Xc_free(This->ordonnee);
    Xc_free(This->curve);
    This->ordonnee = NULL;
    This->nb_curve = 0;
  }
  if (This->nb_abscisse)
  {
    for(i=0; i<This->nb_abscisse; i++)
    {
      BG_DELETE(This->graduation[i])(This->graduation[i]);
      BG_DELETE(This->string[i])(This->string[i]);
    }
    Xc_free(This->graduation);
    Xc_free(This->string);
    This->nb_abscisse = 0;
  }
    
  This->axe = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
  point1.dx = bbox.llx;
  point2.dx = bbox.urx - ordo;
  point1.dy = point2.dy = bbox.lly - SCALE_FROM_MILLIMETERS(10) - absci;
  BG_SET(This->axe)(This->axe,
		    XcBG_Line_POINT1, point1,
		    XcBG_Line_POINT2, point2,     
		    XcBG_Line_END);
 
  if (highg->nb_abscisse == 1)
  {  
    This->nb_abscisse = nbs = 2;
    one_absc = TRUE;
  }
  else
  {    
    This->nb_abscisse = nbs = highg->nb_abscisse;
    one_absc = FALSE;
  }
    
  This->graduation = (c_BG_Line **)Xc_malloc("curve grad **", 
					     sizeof(c_BG_Line *)*nbs);
  This->string = (c_BG_Text **)Xc_malloc("curve string **", 
					 sizeof(c_BG_Text *)*nbs);
  
  size.dx = (width - ordo) / (nbs - 1);
  point1.dx = point2.dx = bbox.llx;
  point1.dy = bbox.lly -  SCALE_FROM_MILLIMETERS(10) - absci;
  point2.dy = point1.dy + SCALE_FROM_MILLIMETERS(1);
  
  t_orig.dy = point1.dy + SCALE_FROM_MILLIMETERS(2); 
  t_size.dx = XcText_SIZE_NOT_DEFINED;
  t_size.dy = bbox.lly - t_orig.dy;

  for(i=0; i<nbs; i++)
  {
    This->graduation[i]=(c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
    BG_SET(This->graduation[i])(This->graduation[i],
				XcBG_Line_POINT1, point1,
				XcBG_Line_POINT2, point2,
				XcBG_Line_END);
    
    This->string[i]=(c_BG_Text *)NEW(c_BG_Text)(_BaseStd);
    if (one_absc)
    {
      if (i && highg->abscisse && highg->d_abscisse)
      {
	string = highg->tab_abscisse[0].string;
	tstyle = highg->tab_abscisse[0].tstyle;
      }
      else
      {
	string = NULL;
	tstyle = highg->tab_data.tstyle;
      }
    }
    else
    {
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
    }
    t_orig.dx = point1.dx - tstyle->point_size / 4;
    BG_SET(This->string[i])(This->string[i],
			    XcBG_Text_ORIGIN, t_orig,
			    XcBG_Text_SIZE, t_size,
			    XcBG_Text_STRING, string,
			    XcBG_Text_STYLE, tstyle,
			    XcBG_Text_RULER, XqR_H_ALIGN_RIGHT | XqR_A_90,
			    XcBG_Text_END); 
    point1.dx = ( point2.dx += size.dx );
  }
  
  This->nb_curve = nbc = highg->nb_ordonnee;
  if (highg->ordonnee && highg->d_ordonnee)
    This->ordonnee = (c_BG_Text **)Xc_malloc("curve ord **", 
					     sizeof(c_BG_Text *) * nbc);
  This->curve = (c_BG_Polyline **)Xc_malloc("curve **", 
					    sizeof(c_BG_Polyline *) * nbc);
  
  for(i=0; i<nbc; i++)
  {
    color = Get_BG_Color_Step(_BaseStd, i);
    size.dx = (width - ordo) / (nbs - 1);
    
    This->curve[i] = (c_BG_Polyline *)NEW(c_BG_Polyline)(_BaseStd);
    BG_SET(This->curve[i])(This->curve[i],
			   XcBG_Polyline_FG_COLOR, color,
			   XcBG_Polyline_LINE_THICKNESS, SCALE_FROM_POINTS(3),
			   XcBG_Polyline_END);
    
    point1.dx = bbox.llx;
    for(j = 0; j < nbs; j++)
    {
      point1.dy = 
	F(HG_Axe).convert_value(HG_Axe, highg->tab_value[i][one_absc?0:j]); 
      BG_SET(This->curve[i])(This->curve[i],
			     XcBG_Polyline_ADD_POINT, point1,
			     XcBG_Polyline_END);
      
      point1.dx += size.dx;
    }  
    if (highg->ordonnee && highg->d_ordonnee)
    {
      This->ordonnee[i] = (c_BG_Text *)NEW(c_BG_Text)(_BaseStd);
      string = highg->tab_ordonnee[i].string;
      tstyle = highg->tab_ordonnee[i].tstyle;
      origin.dx = point1.dx + SCALE_FROM_MILLIMETERS(1);
      origin.dy = point1.dy - tstyle->point_size / 2;
      size.dx = width + SCALE_FROM_MILLIMETERS(10) - origin.dx + bbox.llx;
      size.dy = XcText_SIZE_NOT_DEFINED;
      BG_SET(This->ordonnee[i])
	(This->ordonnee[i],
	 XcBG_Text_ORIGIN, origin,
	 XcBG_Text_SIZE, size,
	 XcBG_Text_STRING, string, 
	 XcBG_Text_STYLE, tstyle,
	 XcBG_Text_RULER, XqR_H_ALIGN_LEFT|XqR_MULTILINE,
	 XcBG_Text_END);
    }
  }
}

static void change_xinfo(This)
c_HG_Curve	*This;
{
  int i;
  
  if (This->axe)
    BG_CHANGE_XINFO(This->axe)(This->axe, &This->highg->X_info);
  
  for(i=0; i<This->nb_curve; i++)
  {
    if (This->ordonnee)
      BG_CHANGE_XINFO(This->ordonnee[i])(This->ordonnee[i], 
					 &This->highg->X_info);
    BG_CHANGE_XINFO(This->curve[i])(This->curve[i], 
				    &This->highg->X_info);
  }
  
  for(i=0; i<This->nb_abscisse; i++)
  {
    BG_CHANGE_XINFO(This->graduation[i])(This->graduation[i], 
					 &This->highg->X_info);
    BG_CHANGE_XINFO(This->string[i])(This->string[i], &This->highg->X_info);
  }
}

static error change_base(This)
c_HG_Curve	*This;
{
  return XC_NO_ERROR;
}

static void display_redraw(This)
c_HG_Curve	*This;
{
  int i;
  c_HighGraph	*highg;
  
  Xc_TRACE(("redraw camembert"));
  
  highg = This->highg;
  if (This->axe)
    BG_DISPLAY_REDRAW(This->axe)(This->axe, highg->matrix, highg->scale, 0, 0);
  
  for(i=0; i<This->nb_curve; i++)
  {
    if (This->ordonnee)
      BG_DISPLAY_REDRAW(This->ordonnee[i])(This->ordonnee[i], highg->matrix,
					   highg->scale, 0, 0);
    BG_DISPLAY_REDRAW(This->curve[i])(This->curve[i], highg->matrix,
				      highg->scale, 0, 0);
  }
  for(i=0; i<This->nb_abscisse; i++)
  {
    BG_DISPLAY_REDRAW(This->graduation[i])(This->graduation[i], highg->matrix,
					   highg->scale, 0, 0);
    BG_DISPLAY_REDRAW(This->string[i])(This->string[i], highg->matrix,
				       highg->scale, 0, 0);
  }
}

static void add_to_vectorgraph(This)
c_HG_Curve  *This;
{
  int	i;
  
  if (This->axe)
    BG_ADD_TO_VECTORGRAPH(This->axe)(This->axe, This->highg->vectorg);

  for(i=0; i<This->nb_curve; i++)
  {
    if (This->ordonnee)
      BG_ADD_TO_VECTORGRAPH(This->ordonnee[i])(This->ordonnee[i], 
					       This->highg->vectorg);
    BG_ADD_TO_VECTORGRAPH(This->curve[i])(This->curve[i],
					  This->highg->vectorg);
  }
  for(i=0; i<This->nb_abscisse; i++)
  {
    BG_ADD_TO_VECTORGRAPH(This->graduation[i])(This->graduation[i], 
					       This->highg->vectorg);
    BG_ADD_TO_VECTORGRAPH(This->string[i])(This->string[i], 
					   This->highg->vectorg);
  }
}

static void sub_to_vectorgraph(This)
c_HG_Curve  *This;
{
  int	i;
  
  if (This->axe)
    BG_SUB_TO_VECTORGRAPH(This->axe)(This->axe, This->highg->vectorg);

  for(i=0; i<This->nb_curve; i++)
  {
    if (This->ordonnee)
      BG_SUB_TO_VECTORGRAPH(This->ordonnee[i])(This->ordonnee[i], 
					       This->highg->vectorg);
    BG_SUB_TO_VECTORGRAPH(This->curve[i])(This->curve[i],
					  This->highg->vectorg);
  }
  for(i=0; i<This->nb_abscisse; i++)
  {
    BG_SUB_TO_VECTORGRAPH(This->graduation[i])(This->graduation[i], 
					       This->highg->vectorg);
    BG_SUB_TO_VECTORGRAPH(This->string[i])(This->string[i], 
					   This->highg->vectorg);
  }
}

static void setHG(c_HG_Curve *This, ...)
{
  va_list ap;

  Xc_HISTORY(("set"));

  va_start(ap, This);
  vset(This, ap);
  va_end(ap);
} 

static void vset(This, ap)
c_HG_Curve *This;
va_list ap;
{
  boolean out_flag;
  boolean calcul;
  
  Xc_TRACE(("vset"));
  
  out_flag = calcul = FALSE;
  do {
    switch(va_arg(ap, HG_Curve_set_code_t))
    {
    case XcHG_Curve_END:
      Xc_TRACE(("XcHG_Curve_END"));
      out_flag = TRUE;
      break;
    default:
      Xc_BREAK(("Unknow HG_Curve_set_code"));
      break;
    }
  } while(!out_flag);
  if (calcul)
    calcul_curve(This);
}

static void calcul_curve(This)
c_HG_Curve	*This;
{
}

/* -------------------------------------------------------------------- **
** ps print, read & write HG_Curve					**
** -------------------------------------------------------------------- */ 
static void ps_print(This, post)
c_HG_Curve *This;
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
c_HG_Curve	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}









