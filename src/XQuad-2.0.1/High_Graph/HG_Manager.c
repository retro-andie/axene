/*
** HG_Manager.c for XQuad in High_Graph/
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
** Last update Sat Oct 26 20:31:05 1996 One of the authors
*/

#include "HG_Manager.h"
#include <stdarg.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void regenerate ___PROTO((c_HG_Manager *This));
static void change_xinfo ___PROTO((c_HG_Manager *This));
static error change_base ___PROTO((c_HG_Manager *This));
static void display_redraw ___NPROTO((c_HG_Manager *This, boolean in_frame));
static void add_to_vectorgraph ___PROTO((c_HG_Manager *This)); 
static void sub_to_vectorgraph ___PROTO((c_HG_Manager *This)); 

static void setHG ___PROTO((c_HG_Manager *This, ...));
static void vset();
static void calcul_Manager ___PROTO((c_HG_Manager *This));
static void ps_print ___PROTO((c_HG_Manager *This, c_PostScript *post));
static boolean readHG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeHG ___PROTO((c_HG_Manager *This, c_DocFile *doc));

sf_HG_Manager fc_HG_Manager =
{
  cons, dest, copy, regenerate, 
  change_xinfo, change_base, display_redraw,
  add_to_vectorgraph, sub_to_vectorgraph, setHG,
  ps_print, readHG, writeHG
};

static void *cons(highg, class, sub_class)
c_HighGraph	*highg;
HG_class_t	class;
int		sub_class;
{
  c_HG_Manager	*This;
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("HG_Manager", sizeof(c_HG_Manager)) ) == NULL)
    return NULL;
  This->f = &fc_HG_Manager;
  
  This->highg = highg;
  This->class = class;
  This->sub_class = sub_class;
  This->vectorg_attach = FALSE;
  
  This->title = NULL;
  This->legend = NULL;
  This->axe = NULL;
  This->nb_object = 0;
  This->object = NULL;
  regenerate(This);
  
  Xc_TRACE(("constructor done"));
  return This;
}

static void dest(This)
c_HG_Manager	*This;
{
  int i;
  
  Xc_HISTORY(("destructor")); 
  if (This->title)
    HG_DELETE(This->title)(This->title);
  
  if (This->legend)
    HG_DELETE(This->legend)(This->legend);

  if (This->axe)
    HG_DELETE(This->axe)(This->axe);
  
  if (This->nb_object)
    {
      for(i=0; i<This->nb_object; i++)
	HG_DELETE(This->object[i])(This->object[i]);
      Xc_free(This->object);
    }
  Xc_free(This);
  Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_HG_Manager	*This;
BaseStd_t	*base;
{
  c_HG_Manager	*hg_copy;
  
  if ( (hg_copy = Xc_malloc("HG_Manager", sizeof(c_HG_Manager)) ) == NULL)
    return NULL;
  memcpy(hg_copy, This, sizeof(c_HG_Manager));
  
  return hg_copy;
}

static void regenerate(This)
c_HG_Manager	*This;
{
  c_HighGraph	*highg;
  int		i;
  coord_t	orig_x;
  coord_t	orig_y;
  coord_t	width;
  coord_t	height;
  vector_t	size;
  bbox_t	bbox;

  highg = This->highg;
  This->class = highg->class;
  
  if (This->vectorg_attach)
    {
      sub_to_vectorgraph(This);
      This->vectorg_attach = TRUE;
    }
  
  orig_x = orig_y = (coord_t)0;
  width = BG_AREA_WIDTH;
  height = BG_AREA_HEIGHT;
  
  if (This->title)
    {
      HG_DELETE(This->title)(This->title);
      This->title = NULL;
    }
  
  if (highg->title && highg->d_title)
    {
      This->title = (c_HG_Title *)NEW(c_HG_Title)(highg, 0);
      HG_REGENERATE(This->title)(This->title, &bbox);

      orig_y = bbox.lly;
      height -= orig_y;
    }
  
  if (This->legend)
    {
      HG_DELETE(This->legend)(This->legend);
      This->legend = NULL;
    }
  
  switch(This->class)
    { 
    case HG_SECTOR: 
      if (highg->abscisse)
	highg->legend = TRUE;
      else
	highg->legend = FALSE;
      highg->axe = FALSE;
      break;
    case HG_BARS:
    case HG_HISTOGRAMME:
      if (highg->abscisse)
	highg->legend = TRUE;
      else
	highg->legend = FALSE;
      if (highg->nb_abscisse && highg->nb_ordonnee)
	highg->axe = TRUE;
      else
	highg->axe = FALSE;
      break;
    case HG_RADAR:
    case HG_SURFACE:
    case HG_CURVE:
      if (highg->ordonnee)
	highg->legend = TRUE;
      else
	highg->legend = FALSE;
      if (highg->nb_abscisse && highg->nb_ordonnee)
	highg->axe = TRUE;
      else
	highg->axe = FALSE;
      break;
    default:
      Xc_TRACE(("Unknow graph class"));
      break;
   }
  
  if (highg->legend && highg->d_legend)
    {
      bbox.llx = width  - SCALE_FROM_MILLIMETERS(20);
      bbox.urx = width;
      bbox.ury = orig_y;
      bbox.lly = orig_y + height;
      switch(This->class)
      {
      case HG_SECTOR:
      case HG_BARS:
      case HG_HISTOGRAMME:
	This->legend = (c_HG_Legend *)NEW(c_HG_Legend)
	  (highg, XcHG_Legend_BOX | XcHG_Legend_ABSC);
	break;
      case HG_SURFACE:
	This->legend = (c_HG_Legend *)NEW(c_HG_Legend)
	  (highg, XcHG_Legend_BOX | XcHG_Legend_ORDO);
	break;
      case HG_CURVE:
      case HG_RADAR:
	This->legend = (c_HG_Legend *)NEW(c_HG_Legend)
	  (highg, XcHG_Legend_LINE | XcHG_Legend_ORDO);
	break;
      default:
	Xc_TRACE(("Unknow graph class"));
	break;
      }
      HG_REGENERATE(This->legend)(This->legend, bbox);
      width -= SCALE_FROM_MILLIMETERS(20);
    }
  
  if (This->axe)
    {
      HG_DELETE(This->axe)(This->axe);
      This->axe = NULL;
    }
  
  if (highg->axe)
    {
      switch(This->class)
	{
	case HG_BARS:
	  bbox.llx = orig_x;
	  bbox.urx = orig_x + width;
	  bbox.ury = orig_y + height - SCALE_FROM_MILLIMETERS(15);
	  bbox.lly = orig_y + height;
	  size.dx = 0;
	  size.dy = height - SCALE_FROM_MILLIMETERS(25);
	  This->axe = (c_HG_Axe *)NEW(c_HG_Axe)(highg, XcHG_Axe_HORIZONTAL);
	  HG_REGENERATE(This->axe)(This->axe, bbox, &size);
	  height -= SCALE_FROM_MILLIMETERS(25);
	  orig_y += SCALE_FROM_MILLIMETERS(10);
	  break;
	case HG_HISTOGRAMME:
	case HG_SURFACE:
	case HG_CURVE:
	  bbox.llx = orig_x;
	  bbox.urx = orig_x + SCALE_FROM_MILLIMETERS(8);
	  bbox.ury = orig_y;
	  bbox.lly = orig_y + height;
	  size.dx = width - SCALE_FROM_MILLIMETERS(10 + 8);
	  size.dy = 0;
	  This->axe = (c_HG_Axe *)NEW(c_HG_Axe)(highg, XcHG_Axe_VERTICAL);
	  HG_REGENERATE(This->axe)(This->axe, bbox, &size);
	  width -= SCALE_FROM_MILLIMETERS(10 + 8);
	  orig_x += SCALE_FROM_MILLIMETERS(8);
	  break;
	case HG_RADAR:
	  bbox.llx = orig_x;
	  bbox.urx = orig_x + width;
	  bbox.ury = orig_y;
	  bbox.lly = orig_y + height;
	  This->axe = (c_HG_Axe *)NEW(c_HG_Axe)(highg, XcHG_Axe_POLAIRE);
	  HG_REGENERATE(This->axe)(This->axe, bbox, &highg->nb_abscisse);
	  break;
	default:
	  Xc_TRACE(("Unknow graph class"));
	  break;
	}
    }
  
  if (This->nb_object)
    {
      for(i=0; i<This->nb_object; i++)
	HG_DELETE(This->object[i])(This->object[i]);
      Xc_free(This->object);
      This->nb_object = 0;
    }
  if (!highg->nb_abscisse || !highg->nb_ordonnee)
    {
      if (This->vectorg_attach)
	add_to_vectorgraph(This);
      return;
    }
  switch(This->class)
    {
    case HG_SECTOR:
      This->nb_object = highg->nb_ordonnee;
      This->object = (void **)Xc_malloc("HG Sector**", 
					sizeof(void *)*This->nb_object);
      for(i=0; i<This->nb_object; i++)
	This->object[i] = NEW(c_HG_Sector)(highg, 0);
      
      {
	int	nb_perline;
	int	nb_line;
	int	i, x, y;
	int	lastline;
	coord_t	nwidth, nheight;
	
	lastline = 0;
	nb_perline = (int)sqrt((double)This->nb_object);
	nb_line = nb_perline;
	if (nb_perline*nb_line != This->nb_object)
	{
	  nb_perline++;
	  while ((nb_perline*nb_line) > This->nb_object)
	    nb_line--;
	  lastline = (This->nb_object - nb_perline*nb_line);
	} 
	nwidth = width / nb_perline;
	nheight = height / (nb_line + (lastline==0?0:1));
	i = 0;
	bbox.lly = orig_y;
	for(y=0; y<nb_line; y++)
	{
	  bbox.ury = bbox.lly;
	  bbox.lly += nheight;
	  bbox.urx = orig_x;
	  for(x=0; x<nb_perline; x++)
	  {
	    bbox.llx = bbox.urx;
	    bbox.urx += nwidth;
	    HG_REGENERATE(This->object[i])(This->object[i], i, bbox);
	    i++;
	  }
	}
	if (lastline)
	{
	  bbox.ury = bbox.lly;
	  bbox.lly += nheight;
	  bbox.urx = orig_x + (width - nwidth * lastline) / 2; 
	  for(x=0; x<lastline; x++)
	  {
	    bbox.llx = bbox.urx;
	    bbox.urx += nwidth;
	    HG_REGENERATE(This->object[i])(This->object[i], i, bbox);
	    i++;
	  }
	}
      }
      break;
    case HG_HISTOGRAMME:
      This->nb_object = highg->nb_ordonnee;
      This->object = (void **)Xc_malloc("HG Histo **", 
					sizeof(void *)*This->nb_object);
      for(i=0; i<This->nb_object; i++)
	This->object[i] = NEW(c_HG_Histo)(highg, 0);
      
      bbox.urx = orig_x;
      bbox.ury = orig_y;
      bbox.lly = orig_y + height;
      for(i=0; i<This->nb_object; i++)
      {
	bbox.llx = bbox.urx;
	bbox.urx += width / This->nb_object;
	HG_REGENERATE(This->object[i])(This->object[i], i, bbox, This->axe);
      }
      break;
    case HG_BARS:
      This->nb_object = highg->nb_ordonnee;
      This->object = (void **)Xc_malloc("HG Bars **", 
					sizeof(void *)*This->nb_object);
      for(i=0; i<This->nb_object; i++)
	This->object[i] = NEW(c_HG_Bars)(highg, 0);
      
      bbox.ury = orig_y + height;
      bbox.llx = orig_x;
      bbox.urx = orig_x + width;
      for(i=0; i<This->nb_object; i++)
      {
	bbox.lly = bbox.ury;
	bbox.ury -= height / This->nb_object;
	HG_REGENERATE(This->object[i])(This->object[i], i, bbox, This->axe);
      }
      break;
    case HG_CURVE:
      This->nb_object = 1;
      This->object = (void **)Xc_malloc("HG Curve **", 
					sizeof(void *)*This->nb_object);
      This->object[0] = NEW(c_HG_Curve)(highg, 0);
      
      bbox.llx = orig_x;
      bbox.urx = bbox.llx + width;
      bbox.ury = orig_y;
      bbox.lly = orig_y + height;
      HG_REGENERATE(This->object[0])(This->object[0], bbox, This->axe);
      break;
    case HG_SURFACE:
      This->nb_object = 1;
      This->object = (void **)Xc_malloc("HG Surface **", 
					sizeof(void *)*This->nb_object);
      This->object[0] = NEW(c_HG_Surface)(highg, 0);
      
      bbox.llx = orig_x;
      bbox.urx = bbox.llx + width;
      bbox.ury = orig_y;
      bbox.lly = orig_y + height;
      HG_REGENERATE(This->object[0])(This->object[0], bbox, This->axe);
      break;
    case HG_RADAR:
      This->nb_object = 1;
      This->object = (void **)Xc_malloc("HG Surface **", 
					sizeof(void *)*This->nb_object);
      This->object[0] = NEW(c_HG_Radar)(highg, 0);
      
      bbox.llx = orig_x;
      bbox.urx = bbox.llx + width;
      bbox.ury = orig_y;
      bbox.lly = orig_y + height;
      HG_REGENERATE(This->object[0])(This->object[0], bbox, This->axe);
      break;
    default:
      Xc_TRACE(("Unknow graph class"));
      break;
    }
  
  if (This->vectorg_attach)
    add_to_vectorgraph(This);
}

static void change_xinfo(This)
c_HG_Manager	*This;
{
  int i;
  
  if (This->title)
    HG_CHANGE_XINFO(This->title)(This->title);
  if (This->legend)
    HG_CHANGE_XINFO(This->legend)(This->legend);
  if (This->axe)
    HG_CHANGE_XINFO(This->axe)(This->axe);
  for(i=0; i<This->nb_object; i++)
    HG_CHANGE_XINFO(This->object[i])(This->object[i]);
}

static error change_base(This)
c_HG_Manager	*This;
{
  return XC_NO_ERROR;
}

static void display_redraw(This, in_frame)
c_HG_Manager	*This;
boolean		in_frame;
{
  Xc_TRACE(("redraw camembert"));
  
  if (in_frame)
    {
      d_object_std *data;
      
      if (!This->highg->vectorg) return;
      data = This->highg->vectorg->frame->data_to_object;
      data->redraw_cadre(data->page, This->highg->vectorg->frame);
    }
  else
    {
      int i;
      c_HighGraph	*highg;
      
      highg = This->highg;
      if (This->title)
	HG_DISPLAY_REDRAW(This->title)(This->title);
      
      if (This->legend)
	HG_DISPLAY_REDRAW(This->legend)(This->legend);

      if (This->axe)
	HG_DISPLAY_REDRAW(This->axe)(This->axe);

      for(i=0; i<This->nb_object; i++)
	HG_DISPLAY_REDRAW(This->object[i])(This->object[i]);
    }
}

static void add_to_vectorgraph(This)
c_HG_Manager  *This;
{
  int	i;
  
  if (This->title)
    HG_ADD_TO_VECTORGRAPH(This->title)(This->title);
  if (This->legend)
    HG_ADD_TO_VECTORGRAPH(This->legend)(This->legend);
  if (This->axe)
    HG_ADD_TO_VECTORGRAPH(This->axe)(This->axe);
  for(i=0; i<This->nb_object; i++)
    HG_ADD_TO_VECTORGRAPH(This->object[i])(This->object[i]);
  This->vectorg_attach = TRUE;
}

static void sub_to_vectorgraph(This)
c_HG_Manager  *This;
{
  int	i;
  
  if (This->title)
    HG_SUB_TO_VECTORGRAPH(This->title)(This->title);
  if (This->legend)
    HG_SUB_TO_VECTORGRAPH(This->legend)(This->legend);
  if (This->axe)
    HG_SUB_TO_VECTORGRAPH(This->axe)(This->axe);
  for(i=0; i<This->nb_object; i++)
    HG_SUB_TO_VECTORGRAPH(This->object[i])(This->object[i]);
  This->vectorg_attach = FALSE;
}

static void setHG(c_HG_Manager *This, ...)
{
  va_list ap;

  Xc_HISTORY(("set"));

  va_start(ap, This);
  vset(This, ap);
  va_end(ap);
} 

static void vset(This, ap)
c_HG_Manager *This;
va_list ap;
{
  boolean out_flag;
  boolean calcul;
  
  Xc_TRACE(("vset"));
  
  out_flag = calcul = FALSE;
  do {
    switch(va_arg(ap, HG_Manager_set_code_t))
      {
      case XcHG_Manager_END:
	Xc_TRACE(("XcHG_Manager_END"));
	out_flag = TRUE;
	break;
      default:
	Xc_BREAK(("Unknow HG_Manager_set_code"));
	break;
      }
  } while(!out_flag);
  if (calcul)
    calcul_Manager(This);
}

static void calcul_Manager(This)
c_HG_Manager	*This;
{
}

/* -------------------------------------------------------------------- **
** ps print, read & write HG_Manager					**
** -------------------------------------------------------------------- */ 
static void ps_print(This, post)
c_HG_Manager *This;
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
c_HG_Manager	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}






