/*
** Align_Cadre.c for Xclamation in Page/
** Align cadres
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
** Started on  Fri Jan 20 17:06:40 1995 Emmanuel Paris
** Last update Thu Oct 24 16:23:54 1996 Emmanuel Paris
*/

#include "Page.h"

static l_Cadre *sort_list_cadre();
static align_coord_t *create_new_orig();
static int	get_lcadre_index();
static coord_t	get_position();
static l_Cadre *get_last_lcadre();

void align_cadre(This,align_param)
c_Page		*This;
align_param_t	*align_param;
{
  l_Cadre	*lastlcadre;
  l_Cadre	*lcadre;
  l_Cadre	*svglcadre;
  align_coord_t	*new_orig;
  coord_t	sOrig_X;
  coord_t	sOrig_Y;
  coord_t	range;
  coord_t	incr;
  coord_t	sx,sy;
  int		nb_cadre;
  
  Xc_TRACE(("gato"));
  if (!align_param->horizontal && !align_param->vertical)
    return;
  
  F(This).undisplay_selected_lcadre(This);
  new_orig = create_new_orig(This, &nb_cadre);
  
  if (align_param->horizontal)
    {
      lcadre = svglcadre = sort_list_cadre(This->SelectedCadre, TRUE);
					   
      lastlcadre = get_last_lcadre(lcadre);
      
      switch(align_param->type_h)
	{
	case ALIGN_LEFT:
	  if (align_param->distribution_h)
	    {
	      range = lastlcadre->cadre->sOrig_X - lcadre->cadre->sOrig_X;
	      incr = range / (nb_cadre-1);
	    }
	  else
	    {
	      incr = align_param->offset_h;
	    }
	  sOrig_X = lcadre->cadre->sOrig_X;
	  while(lcadre)
	    {
	      new_orig[get_lcadre_index(This,lcadre)].sOrig_X = sOrig_X;
	      sOrig_X += incr;
	      lcadre = lcadre->NextCadre;
	    }
	  break;
	case ALIGN_RIGHT:
	  if (align_param->distribution_h)
	    {
	      range = (lastlcadre->cadre->sOrig_X +
		       lastlcadre->cadre->sWidth) -
			 (lcadre->cadre->sOrig_X +
			  lcadre->cadre->sWidth);
	       incr = range / (nb_cadre-1);
	    }
	  else
	    {
	      incr = align_param->offset_h;
	    }
	  sOrig_X = lcadre->cadre->sOrig_X + lcadre->cadre->sWidth;
	  while(lcadre)
	    {
	      new_orig[get_lcadre_index(This,lcadre)].sOrig_X = 
		sOrig_X - lcadre->cadre->sWidth; 
	      sOrig_X += incr;
	      lcadre = lcadre->NextCadre;
	    }
	  break;
	case ALIGN_CENTER:
	  if (align_param->distribution_h)
	    {
	      range = (lastlcadre->cadre->sOrig_X +
		       lastlcadre->cadre->sWidth/2) -
			 (lcadre->cadre->sOrig_X +
			  lcadre->cadre->sWidth/2);
	      incr = range / (nb_cadre-1);
	    }
	  else
	     {
	       incr = align_param->offset_h;
	     }
	  sOrig_X = lcadre->cadre->sOrig_X + lcadre->cadre->sWidth/2;
	  while(lcadre)
	    {
	      new_orig[get_lcadre_index(This,lcadre)].sOrig_X = 
		sOrig_X - lcadre->cadre->sWidth/2; 
	      sOrig_X += incr;
	      lcadre = lcadre->NextCadre;
	    }
	  break;
	case ALIGN_BLOCK:
	  if (align_param->distribution_h)
	    {
	      coord_t	width;
	      l_Cadre	*lcadre2;

	      range = (lastlcadre->cadre->sOrig_X - 
		       lcadre->cadre->sOrig_X -
		       lcadre->cadre->sWidth);
	      width = (coord_t)0;
	      lcadre2 = lcadre->NextCadre;
	      while(lcadre2 && lcadre2->NextCadre)
		{
		  width += lcadre2->cadre->sWidth;
		  lcadre2 = lcadre2->NextCadre;
		}
	      incr = (range - width)/(nb_cadre-1);
	    }
	  else
	    {
	      incr = align_param->offset_h;
	    }
	  sOrig_X = lcadre->cadre->sOrig_X;
	  while(lcadre)
	    {
	      new_orig[get_lcadre_index(This, lcadre)].sOrig_X = sOrig_X;
	      sOrig_X += lcadre->cadre->sWidth + incr;
	      lcadre = lcadre->NextCadre;
	    }
	default:
	  break;
	}
      free_list_cadre(svglcadre, NO_DEL_CADRE);
    }

  if (align_param->vertical)
    {
      lcadre = svglcadre = sort_list_cadre(This->SelectedCadre, FALSE);
					   
      lastlcadre = get_last_lcadre(lcadre);
      
      switch(align_param->type_v)
	{
	case ALIGN_TOP:
	  if (align_param->distribution_v)
	    {
	      range = lastlcadre->cadre->sOrig_Y - lcadre->cadre->sOrig_Y;
	      incr = range / (nb_cadre-1);
	    }
	  else
	    {
	      incr = align_param->offset_v;
	    }
	  sOrig_Y = lcadre->cadre->sOrig_Y;
	  while(lcadre)
	    {
	      new_orig[get_lcadre_index(This,lcadre)].sOrig_Y = sOrig_Y;
	      sOrig_Y += incr;
	      lcadre = lcadre->NextCadre;
	    }
	  break;
	case ALIGN_BOTTOM:
	  if (align_param->distribution_v)
	    {
	      range = (lastlcadre->cadre->sOrig_Y +
		       lastlcadre->cadre->sHeight) -
			 (lcadre->cadre->sOrig_Y +
			  lcadre->cadre->sHeight);
	       incr = range / (nb_cadre-1);
	    }
	  else
	    {
	      incr = align_param->offset_v;
	    }
	  sOrig_Y = lcadre->cadre->sOrig_Y + lcadre->cadre->sHeight;
	  while(lcadre)
	    {
	      new_orig[get_lcadre_index(This,lcadre)].sOrig_Y = 
		sOrig_Y - lcadre->cadre->sHeight;
	      sOrig_Y += incr;
	      lcadre = lcadre->NextCadre;
	    }
	  break;
	case ALIGN_CENTER:
	  if (align_param->distribution_v)
	    {
	      range = (lastlcadre->cadre->sOrig_Y +
		       lastlcadre->cadre->sHeight/2) -
			 (lcadre->cadre->sOrig_Y +
			  lcadre->cadre->sHeight/2);
	      incr = range / (nb_cadre-1);
	    }
	  else
	     {
	       incr = align_param->offset_v;
	     }
	  sOrig_Y = lcadre->cadre->sOrig_Y + lcadre->cadre->sHeight/2;
	  while(lcadre)
	    {
	      new_orig[get_lcadre_index(This,lcadre)].sOrig_Y = 
		sOrig_Y - lcadre->cadre->sHeight/2; 
	      sOrig_Y += incr;
	      lcadre = lcadre->NextCadre;
	    }
	  break;
	case ALIGN_BLOCK:
	  if (align_param->distribution_v)
	    {
	      coord_t	height;
	      l_Cadre	*lcadre2;

	      range = (lastlcadre->cadre->sOrig_Y - 
		       lcadre->cadre->sOrig_Y -
		       lcadre->cadre->sHeight);
	      height = (coord_t)0;
	      lcadre2 = lcadre->NextCadre;
	      while(lcadre2 && lcadre2->NextCadre)
		{
		  height += lcadre2->cadre->sHeight;
		  lcadre2 = lcadre2->NextCadre;
		}
	      incr = (range - height)/(nb_cadre-1);
	    }
	  else
	    {
	      incr = align_param->offset_v;
	    }
	  sOrig_Y = lcadre->cadre->sOrig_Y;
	  while(lcadre)
	    {
	      new_orig[get_lcadre_index(This, lcadre)].sOrig_Y = sOrig_Y;
	      sOrig_Y += lcadre->cadre->sHeight + incr;
	      lcadre = lcadre->NextCadre;
	    }
	default:
	  break;
	}
      free_list_cadre(svglcadre, NO_DEL_CADRE);
    }
  
  lcadre = This->SelectedCadre;
  while(lcadre)
    {
      sx = lcadre->cadre->sOrig_X;
      sy = lcadre->cadre->sOrig_Y;
      if (align_param->horizontal)
	sx = new_orig[get_lcadre_index(This,lcadre)].sOrig_X;
      if (align_param->vertical)
	sy = new_orig[get_lcadre_index(This,lcadre)].sOrig_Y;
      F(This).move_cadre_to_xy(This, lcadre, sx, sy);
      lcadre = lcadre->NextCadre;
    }
  F(This).calcul_select_handle(This);
  F(This).display_selected_lcadre(This);
  Xc_free(new_orig);
}

static align_coord_t *create_new_orig(This, nb_cadre)
c_Page	*This;
int	*nb_cadre;
{
  l_Cadre	*lcadre;
  align_coord_t *new_orig;
  int		count = 0;
  
  lcadre = This->SelectedCadre;
  while(lcadre)
    {
      count ++;
      lcadre = lcadre->NextCadre;
    }
  new_orig = (align_coord_t *)Xc_malloc("align_coord_t",
					sizeof(align_coord_t)*count);
  memset(new_orig, 0, sizeof(align_coord_t) * count);
  if (nb_cadre)
    *nb_cadre = count;
  return new_orig;
}

static int get_lcadre_index(This, lcadre)
c_Page	*This;
l_Cadre	*lcadre;
{
  int	index = 0;
  l_Cadre *tmplcadre;
  
  tmplcadre = This->SelectedCadre;
  while(tmplcadre)
  {
    if (tmplcadre->cadre == lcadre->cadre)
      return index;
    index++;
    tmplcadre = tmplcadre->NextCadre;
  }
  return index;
}

static l_Cadre	*get_last_lcadre(lcadre)
l_Cadre	*lcadre;
{
  while(lcadre->NextCadre)
    lcadre = lcadre->NextCadre;
  return lcadre;
}

static l_Cadre *sort_list_cadre(list,orient)
l_Cadre *list;
boolean	orient;
{
  l_Cadre *tmplist;
  l_Cadre **tablist;
  l_Cadre *swplist;
  boolean utils;
  int minorant = 0;
  int posit;
  int nb_cadre;
  int pivo;
  int i,j;
  
  tmplist = list;
  nb_cadre = 0;
  utils = TRUE;
  while(tmplist)
    {
      posit = get_position(tmplist, orient);
      if (nb_cadre)
	{
	  if (minorant > posit)
	    utils = FALSE;
	}
      minorant = posit;
      nb_cadre++;
      tmplist = tmplist->NextCadre;
    }
  if (utils)
    {
      tmplist = duplicate_list_cadre(list);
      return tmplist;
    }
  tablist=(l_Cadre **)Xc_malloc("sort lcadre",sizeof(l_Cadre *)*nb_cadre);
  tmplist = list;
  i = 0;
  while(tmplist)
    {
      tablist[i]=(l_Cadre *)Xc_malloc("sort cadre",sizeof(l_Cadre));
      memcpy(tablist[i++],tmplist,sizeof(l_Cadre));
      tmplist = tmplist->NextCadre;
    }
  for(i = 0;i<nb_cadre;i++)
    {
      minorant = get_position(tablist[i], orient);
      pivo = i;
      for(j = i+1;j<nb_cadre;j++)
	{
	  posit = get_position(tablist[j], orient);
	  if (posit<minorant)
	    {
	      minorant = posit;
	      pivo = j;
	    }
	}
      swplist = tablist[pivo];
      tablist[pivo]=tablist[i];
      tablist[i]=swplist;
    }
  for(i = 0;i<nb_cadre-1;i++)
    {
      tablist[i]->NextCadre = tablist[i+1];
    }
  tablist[i]->NextCadre = NULL;
  tmplist = tablist[0];
  Xc_free(tablist);
  return tmplist;
}

static coord_t	get_position(lcadre, orient)
l_Cadre *lcadre;
boolean	orient;
{
  if (orient)
    return lcadre->cadre->sOrig_X;
  else
    return lcadre->cadre->sOrig_Y;
}


