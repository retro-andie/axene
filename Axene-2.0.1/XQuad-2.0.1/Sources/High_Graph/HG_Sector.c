/*
** HG_Sector.c for XQuad in High_Graph/
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
** Started on  Sat Jun 24 18:07:33 1995 Emmanuel Paris
** Last update Wed May 29 15:40:25 1996 One of the authors
*/

#include "HG_Sector.h"
#include <varargs.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void regenerate ___PROTO((c_HG_Sector *This, int ordonnee, 
				 bbox_t	bbox));
static void change_xinfo ___PROTO((c_HG_Sector *This));
static error change_base ___PROTO((c_HG_Sector *This));
static void display_redraw ___PROTO((c_HG_Sector *This));
static void add_to_vectorgraph ___PROTO((c_HG_Sector *This));
static void sub_to_vectorgraph ___PROTO((c_HG_Sector *This));
static void setHG (); /* ___PROTO((c_HG_Sector *This, ...)); */
static void vset();
static void calcul_sector ___PROTO((c_HG_Sector *This));
static void ps_print ___PROTO((c_HG_Sector *This, c_PostScript *post));
static boolean readHG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeHG ___PROTO((c_HG_Sector *This, c_DocFile *doc));

sf_HG_Sector fc_HG_Sector =
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
  c_HG_Sector	*This;
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("HG_Sector", sizeof(c_HG_Sector)) ) == NULL)
    return NULL;
  This->f = &fc_HG_Sector;
  
  This->highg = highg;
  This->class = HG_SECTOR;
  This->sub_class = sub_class;

  This->nb_sector = 0;
  This->ordonnee = NULL;
  
  Xc_TRACE(("constructor done"));
  return This;
}

static void dest(This)
c_HG_Sector	*This;
{
  int i;
  Xc_HISTORY(("destructor")); 
  if (This->ordonnee)
    BG_DELETE(This->ordonnee)(This->ordonnee);
  
  if (This->nb_sector)
  {
    for(i=0; i<This->nb_sector; i++)
      BG_DELETE(This->sector[i])(This->sector[i]);
    Xc_free(This->sector);
  }
  Xc_free(This);
  Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_HG_Sector	*This;
BaseStd_t	*base;
{
  c_HG_Sector	*hg_copy;
  
  if ( (hg_copy = Xc_malloc("HG_Sector", sizeof(c_HG_Sector)) ) == NULL)
    return NULL;
  memcpy(hg_copy, This, sizeof(c_HG_Sector));
  
  return hg_copy;
}

static void regenerate(This, ordonnee, bbox)
c_HG_Sector	*This;
int		ordonnee;
bbox_t		bbox;
{
  c_HighGraph	*highg;
  real		total;
  int		i, j, nbs;
  angle_t	angle1, angle2;
  scale_t	percent;
  coord_t	width, height;
  vector_t	rayon, center, size, origin;
  char		*string;
  c_Color	*color;
  c_TextStyle	*tstyle;

  highg = This->highg;
  width = bbox.urx - bbox.llx;
  height = bbox.lly - bbox.ury;
  if (This->ordonnee)
  {
    BG_DELETE(This->ordonnee)(This->ordonnee);
    This->ordonnee = NULL;
  }
  if (This->nb_sector)
  {
    for(i=0; i<This->nb_sector; i++)
      BG_DELETE(This->sector[i])(This->sector[i]);
    Xc_free(This->sector);
    This->nb_sector = 0;
  }
  
  if (highg->ordonnee && highg->d_ordonnee)
  {
    This->ordonnee = (c_BG_Text *)NEW(c_BG_Text)(_BaseStd);
    string = highg->tab_ordonnee[ordonnee].string;
    tstyle = highg->tab_ordonnee[ordonnee].tstyle;
    size.dx = width;
    size.dy = tstyle->point_size;
    origin.dx = bbox.llx;
    origin.dy = bbox.lly - size.dy;
    BG_SET(This->ordonnee)
      (This->ordonnee,
       XcBG_Text_ORIGIN, origin,
       XcBG_Text_SIZE, size,
       XcBG_Text_STRING, string, 
       XcBG_Text_STYLE, tstyle,
       XcBG_Text_RULER, XqR_H_ALIGN_CENTER|XqR_V_ALIGN_CENTER,
       XcBG_Text_END);
  }
  
  nbs = 0; total = 0.0;
  for(i=0; i < highg->nb_abscisse; i++)
    {
      if ( highg->tab_value[ordonnee][i] > 0.0)
	{
	  total += highg->tab_value[ordonnee][i];
	  nbs ++;
	}
    }
  
  This->nb_sector = nbs;
  if (!nbs) return;
  This->sector = (c_BG_SectorText **)Xc_malloc("tab sector", 
					       sizeof(c_BG_SectorText *)*nbs);
  rayon.dx = width / 4; 
  rayon.dy = height / 4;
  center.dx = width / 2 + bbox.llx; 
  center.dy = height / 2 + bbox.ury;
  angle2 = ANGLE_ZERO;
  for(i=j=0; i<highg->nb_abscisse; i++)
    {
      if (highg->tab_value[ordonnee][i] > 0.0)
	{
	  This->sector[j]=(c_BG_SectorText *)NEW(c_BG_SectorText)(_BaseStd);
	  angle1 = angle2;
	  percent = SCALE_FROM_COEF(highg->tab_value[ordonnee][i] / total);
	  angle2 += ( SCALE_TO_COEF(percent) * SCALE_FROM_DEGREES(360.0));
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
	  color = Get_BG_Color_Step(_BaseStd, i);
	  BG_SET(This->sector[j])
	    (This->sector[j],
	     XcBG_SectorText_ANGLE1, SCALE_TO_VDEGREES(angle1),
	     XcBG_SectorText_ANGLE2, SCALE_TO_VDEGREES(angle2),
	     XcBG_SectorText_RAYON, rayon,
	     XcBG_SectorText_CENTER, center,
	     XcBG_SectorText_STRING, string, 
	     XcBG_SectorText_COLOR, color,
	     XcBG_SectorText_PERCENT, SCALE_TO_VSCALE(percent),
	     XcBG_SectorText_STYLE, tstyle,
	     XcBG_SectorText_BBOX_CLIP, bbox,
	     XcBG_SectorText_END);
	  j++;
	}
    }
}

static void change_xinfo(This)
c_HG_Sector	*This;
{
  int i;
  
  if (This->ordonnee)
    BG_CHANGE_XINFO(This->ordonnee)(This->ordonnee, &This->highg->X_info);
  
  for(i=0; i<This->nb_sector; i++)
    BG_CHANGE_XINFO(This->sector[i])(This->sector[i], &This->highg->X_info);
}

static error change_base(This)
c_HG_Sector	*This;
{
  return XC_NO_ERROR;
}

static void display_redraw(This)
c_HG_Sector	*This;
{
  int i;
  c_HighGraph	*highg;
  
  Xc_TRACE(("redraw camembert"));
  
  highg = This->highg;
  if (This->ordonnee)
    BG_DISPLAY_REDRAW(This->ordonnee)(This->ordonnee, 
				      highg->matrix, highg->scale, 0, 0);
  for(i=0; i<This->nb_sector; i++)
    BG_DISPLAY_REDRAW(This->sector[i])(This->sector[i], 
				       highg->matrix,
				       highg->scale,
				       0, 0);
}

static void add_to_vectorgraph(This)
c_HG_Sector  *This;
{
  int	i;
  
  if (This->ordonnee)
    BG_ADD_TO_VECTORGRAPH(This->ordonnee)(This->ordonnee, 
					  This->highg->vectorg);

  for(i=0; i<This->nb_sector; i++)
    BG_ADD_TO_VECTORGRAPH(This->sector[i])(This->sector[i], 
					   This->highg->vectorg);
}

static void sub_to_vectorgraph(This)
c_HG_Sector  *This;
{
  int	i;
  
  if (This->ordonnee)
    BG_SUB_TO_VECTORGRAPH(This->ordonnee)(This->ordonnee, 
					  This->highg->vectorg);
  
  for(i=0; i<This->nb_sector; i++)
    BG_SUB_TO_VECTORGRAPH(This->sector[i])(This->sector[i], 
					   This->highg->vectorg);
}

static void setHG(This, va_alist)
c_HG_Sector *This;
va_dcl
{
  va_list ap;
  
  Xc_HISTORY(("set"));

  va_start(ap);
  vset(This, ap);
  va_end(ap);
} 

static void vset(This, ap)
c_HG_Sector *This;
va_list ap;
{
  boolean out_flag;
  boolean calcul;
  
  Xc_TRACE(("vset"));
  
  out_flag = calcul = FALSE;
  do {
    switch(va_arg(ap, HG_Sector_set_code_t))
    {
    case XcHG_Sector_END:
      Xc_TRACE(("XcHG_Sector_END"));
      out_flag = TRUE;
      break;
    default:
      Xc_BREAK(("Unknow HG_Sector_set_code"));
      break;
    }
  } while(!out_flag);
  if (calcul)
    calcul_sector(This);
}

static void calcul_sector(This)
c_HG_Sector	*This;
{
}

/* -------------------------------------------------------------------- **
** ps print, read & write HG_Sector					**
** -------------------------------------------------------------------- */ 
static void ps_print(This, post)
c_HG_Sector *This;
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
c_HG_Sector	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}



