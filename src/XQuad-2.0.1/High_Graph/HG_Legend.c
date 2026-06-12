/*
** HG_Legend.c for XQuad in High_Graph/
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
** Last update Sat Oct 26 20:32:16 1996 One of the authors
*/

#include "HG_Legend.h"
#include <stdarg.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void regenerate ___PROTO((c_HG_Legend *This, bbox_t bbox));
static void change_xinfo ___PROTO((c_HG_Legend *This));
static error change_base ___PROTO((c_HG_Legend *This));
static void display_redraw ___PROTO((c_HG_Legend *This));
static void add_to_vectorgraph ___PROTO((c_HG_Legend *This));
static void sub_to_vectorgraph ___PROTO((c_HG_Legend *This));
static void setHG ___PROTO((c_HG_Legend *This, ...));
static void vset();
static void calcul_legend ___PROTO((c_HG_Legend *This));
static void ps_print ___PROTO((c_HG_Legend *This, c_PostScript *post));
static boolean readHG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeHG ___PROTO((c_HG_Legend *This, c_DocFile *doc));

sf_HG_Legend fc_HG_Legend =
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
  c_HG_Legend	*This;
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("HG_Legend", sizeof(c_HG_Legend)) ) == NULL)
    return NULL;
  This->f = &fc_HG_Legend;
  
  This->highg = highg;
  This->class = HG_LEGEND;
  This->sub_class = sub_class;
  
  This->separator = NULL;
  This->nb_legend = 0;
  This->line = NULL;
  This->box = NULL;
  This->string = NULL;
  
  Xc_TRACE(("constructor done"));
  return This;
}

static void dest(This)
c_HG_Legend	*This;
{
  int	i;
  Xc_HISTORY(("destructor")); 
  
  if (This->separator)
    BG_DELETE(This->separator)(This->separator);
  
  if (This->nb_legend)
  {
    switch(This->sub_class & XcHG_Legend_MOTIF)
    {
    case XcHG_Legend_BOX:
      for(i=0; i<This->nb_legend; i++)
      {
	BG_DELETE(This->string[i])(This->string[i]);
	BG_DELETE(This->box[i])(This->box[i]);
      }
      Xc_free(This->string);
      Xc_free(This->box);
      break;
    case XcHG_Legend_LINE:
      for(i=0; i<This->nb_legend; i++)
	{
	  BG_DELETE(This->string[i])(This->string[i]);
	  BG_DELETE(This->line[i])(This->line[i]);
	}  
      Xc_free(This->string);
      Xc_free(This->line);
      break;
    }
  }
  Xc_free(This);
  
  Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_HG_Legend	*This;
BaseStd_t	*base;
{
  c_HG_Legend	*hg_copy;
  
  if ( (hg_copy = Xc_malloc("HG_Legend", sizeof(c_HG_Legend)) ) == NULL)
    return NULL;
  memcpy(hg_copy, This, sizeof(c_HG_Legend));
  
  return hg_copy;
}

static void regenerate(This, bbox)
c_HG_Legend	*This;
bbox_t		bbox;
{
  c_HighGraph	*highg;
  int		i, nbs;
  coord_t	width, height;
  coord_t	markx, marky;
  coord_t	intervy, stringx;
  vector_t	point1, point2, tsize;
  c_Color	*color;
  char		*string;
  c_TextStyle	*tstyle;

  highg = This->highg;
  width = bbox.urx - bbox.llx;
  height = bbox.lly - bbox.ury;
  if (This->separator)
    BG_DELETE(This->separator)(This->separator);
  
  if (This->nb_legend)
  {
    switch(This->sub_class & XcHG_Legend_MOTIF)
    {
    case XcHG_Legend_BOX:
      for(i=0; i<This->nb_legend; i++)
      {
	BG_DELETE(This->string[i])(This->string[i]);
	BG_DELETE(This->box[i])(This->box[i]);
      }
      Xc_free(This->string);
      Xc_free(This->box);
      break;
    case XcHG_Legend_LINE:
      for(i=0; i<This->nb_legend; i++)
      {
	BG_DELETE(This->string[i])(This->string[i]);
	BG_DELETE(This->line[i])(This->line[i]);
      }
      Xc_free(This->string);
      Xc_free(This->line);
      break;
    }
  }
  
  This->separator = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
  point1.dx = point2.dx = bbox.llx;
  point1.dy = bbox.ury;
  point2.dy = bbox.lly;
  BG_SET(This->separator)
    (This->separator,
     XcBG_Line_POINT1, point1,
     XcBG_Line_POINT2, point2,     
     XcBG_Line_END);
  
  switch(This->sub_class & XcHG_Legend_SENS)
  {
  case XcHG_Legend_ABSC:
    This->nb_legend = nbs = highg->nb_abscisse;
    break;
  case XcHG_Legend_ORDO:
  default:
    This->nb_legend = nbs = highg->nb_ordonnee;
    break;
  }
  if (!This->nb_legend) return;
  This->string = (c_BG_Text **)Xc_malloc("legend str", 
					 sizeof(c_BG_Text *)*nbs);
  switch(This->sub_class & XcHG_Legend_MOTIF)
  {
  case XcHG_Legend_BOX:
    This->box = (c_BG_Box **)Xc_malloc("legend box", 
				       sizeof(c_BG_Box *)*nbs);
    break;
  case XcHG_Legend_LINE:
    This->line = (c_BG_Line **)Xc_malloc("legend line", 
					   sizeof(c_BG_Line *)*nbs);
    break;
  }
  markx = bbox.llx + SCALE_FROM_MILLIMETERS(2);
  stringx = markx + SCALE_FROM_MILLIMETERS(6);
  intervy = (height - nbs * SCALE_FROM_MILLIMETERS(4)) / (nbs + 1);
  marky = bbox.ury + intervy;
  for(i=0; i<nbs; i++)
  {
    color = Get_BG_Color_Step(_BaseStd, i);
    switch(This->sub_class & XcHG_Legend_MOTIF) 
    {
    case XcHG_Legend_BOX:
      point1.dx = markx;
      point1.dy = marky;
      point2.dx = SCALE_FROM_MILLIMETERS(3);
      point2.dy = SCALE_FROM_MILLIMETERS(4);
      This->box[i] = (c_BG_Box *)NEW(c_BG_Box)(_BaseStd);
      BG_SET(This->box[i])(This->box[i],
			   XcBG_Box_POINT1, point1,
			   XcBG_Box_SIZE, point2,
			   XcBG_Box_INS_FG_COLOR, color,
			   XcBG_Box_END);
      break;
    case XcHG_Legend_LINE:
      point1.dx = markx;
      point2.dx = markx + SCALE_FROM_MILLIMETERS(3);
      point1.dy = point2.dy = marky + SCALE_FROM_MILLIMETERS(2);
      This->line[i] = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
      BG_SET(This->line[i])(This->line[i],
			    XcBG_Line_POINT1, point1,
			    XcBG_Line_POINT2, point2,
			    XcBG_Line_FG_COLOR, color,
			    XcBG_Line_LINE_THICKNESS, SCALE_FROM_POINTS(3),
			    XcBG_Line_END);
      
      break;
    }
    This->string[i]=(c_BG_Text *)NEW(c_BG_Text)(_BaseStd);
    point1.dx = stringx;
    point1.dy = marky;
    switch(This->sub_class & XcHG_Legend_SENS)
    {
    case XcHG_Legend_ABSC:
      string = highg->tab_abscisse[i].string;
      tstyle = highg->tab_abscisse[i].tstyle;
      break;
    case XcHG_Legend_ORDO:
    default:
      string = highg->tab_ordonnee[i].string;
      tstyle = highg->tab_ordonnee[i].tstyle;
      break;
    }
    tsize.dx = bbox.urx - point1.dx;
    tsize.dy = SCALE_FROM_MILLIMETERS(4) + intervy;
    BG_SET(This->string[i])(This->string[i],
			    XcBG_Text_ORIGIN, point1,
			    XcBG_Text_SIZE, tsize,
			    XcBG_Text_STRING, string,
			    XcBG_Text_STYLE, tstyle,
			    XcBG_Text_RULER, 
			    XqR_H_ALIGN_LEFT | XqR_V_ALIGN_TOP | XqR_MULTILINE,
			    XcBG_Text_END);
    
    marky += tsize.dy;
  }
}

static void change_xinfo(This)
c_HG_Legend	*This;
{
  int i;
  
  if (This->separator)
    BG_CHANGE_XINFO(This->separator)(This->separator, &This->highg->X_info);
  
  for(i=0; i<This->nb_legend; i++)
    {
      BG_CHANGE_XINFO(This->string[i])(This->string[i], 
				       &This->highg->X_info);
      switch(This->sub_class & XcHG_Legend_MOTIF)
	{
	case XcHG_Legend_BOX:
	  BG_CHANGE_XINFO(This->box[i])(This->box[i], &This->highg->X_info);
	  break;
	case XcHG_Legend_LINE:
	  BG_CHANGE_XINFO(This->line[i])(This->line[i], &This->highg->X_info);
	  break;
	}
    }
}

static error change_base(This)
c_HG_Legend	*This;
{
  return XC_NO_ERROR;
}

static void display_redraw(This)
c_HG_Legend	*This;
{
  int i;
  c_HighGraph	*highg;
  
  Xc_TRACE(("redraw legend"));
  
  highg = This->highg;
  if (This->separator)
    BG_DISPLAY_REDRAW(This->separator)(This->separator, 
				       highg->matrix, highg->scale, 0, 0);  
  for(i=0; i<This->nb_legend; i++)
    {
      BG_DISPLAY_REDRAW(This->string[i])(This->string[i], 
					 highg->matrix,
					 highg->scale,
					 0, 0);
      switch(This->sub_class & XcHG_Legend_MOTIF)
	{
	case XcHG_Legend_BOX:
	  BG_DISPLAY_REDRAW(This->box[i])(This->box[i],
					  highg->matrix,
					  highg->scale,
					  0, 0);
	  break;
	case XcHG_Legend_LINE:
	  BG_DISPLAY_REDRAW(This->line[i])(This->line[i],
					   highg->matrix,
					   highg->scale,
					   0, 0);
	  break;
	}
    }
}

static void add_to_vectorgraph(This)
c_HG_Legend  *This;
{
  int	i;
  
  if (This->separator)
    BG_ADD_TO_VECTORGRAPH(This->separator)(This->separator,
					   This->highg->vectorg);
  
  for(i=0; i<This->nb_legend; i++)
    {
      BG_ADD_TO_VECTORGRAPH(This->string[i])(This->string[i],
					     This->highg->vectorg);
      switch(This->sub_class & XcHG_Legend_MOTIF)
	{
	case XcHG_Legend_BOX:
	  BG_ADD_TO_VECTORGRAPH(This->box[i])(This->box[i],
					      This->highg->vectorg);
	  break;
	case XcHG_Legend_LINE:
	  BG_ADD_TO_VECTORGRAPH(This->line[i])(This->line[i], 
					       This->highg->vectorg);
	  break;
	}
    }
}

static void sub_to_vectorgraph(This)
c_HG_Legend  *This;
{
  int	i;
  
  if (This->separator)
    BG_SUB_TO_VECTORGRAPH(This->separator)(This->separator,
					   This->highg->vectorg);
  
  for(i=0; i<This->nb_legend; i++)
    {
      BG_SUB_TO_VECTORGRAPH(This->string[i])(This->string[i],
					     This->highg->vectorg);
      switch(This->sub_class & XcHG_Legend_MOTIF)
	{
	case XcHG_Legend_BOX:
	  BG_SUB_TO_VECTORGRAPH(This->box[i])(This->box[i],
					      This->highg->vectorg);
	  break;
	case XcHG_Legend_LINE:
	  BG_SUB_TO_VECTORGRAPH(This->line[i])(This->line[i], 
					       This->highg->vectorg);
	  break;
	}
    }
}

static void setHG(c_HG_Legend *This, ...)
{
  va_list ap;

  Xc_HISTORY(("set"));

  va_start(ap, This);
  vset(This, ap);
  va_end(ap);
} 

static void vset(This, ap)
c_HG_Legend *This;
va_list ap;
{
  boolean out_flag;
  boolean calcul;
  
  Xc_TRACE(("vset"));
  
  out_flag = calcul = FALSE;
  do {
    switch(va_arg(ap, HG_Legend_set_code_t))
    {
    case XcHG_Legend_END:
      Xc_TRACE(("XcHG_Legend_END"));
      out_flag = TRUE;
      break;
    default:
      Xc_BREAK(("Unknow HG_Legend_set_code"));
      break;
    }
  } while(!out_flag);
  if (calcul)
    calcul_legend(This);
}

static void calcul_legend(This)
c_HG_Legend	*This;
{
}

/* -------------------------------------------------------------------- **
** ps print, read & write HG_Legend					**
** -------------------------------------------------------------------- */ 
static void ps_print(This, post)
c_HG_Legend *This;
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
c_HG_Legend	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}



