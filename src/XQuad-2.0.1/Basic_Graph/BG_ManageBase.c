/*
** BG_ManageBase.c for XQuad in Basic_Graph/
** structure de base standard
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
** Started on  Mon Dec 12 16:17:18 1994 Emmanuel Paris
** Last update Sun Feb  8 23:33:10 1998 Emmanuel Paris
*/

#define _BG_MANAGER_BASE_PRIVATE_
#include "BG_ManageBase.h"
#include "VectorFont.h"

extern c_VectorFont *GlobFontBase;

void Create_BG_Color_Base ___PROTO((BaseStd_t *basestd));
void Destroy_BG_Color_Base ___PROTO((BaseStd_t *basestd));
c_Color *Get_BG_Color ___PROTO((BaseStd_t *basestd, int color));
c_Color *Get_BG_Color_Step ___PROTO((BaseStd_t *basestd, int step));

void Create_BG_TextStyle_Base ___PROTO((BaseStd_t *basestd));
void Destroy_BG_TextStyle_Base ___PROTO((BaseStd_t *basestd));
c_TextStyle *Get_BG_TextStyle ___PROTO((BaseStd_t *basestd,int tstyle));

void Create_BG_Color_Base(basestd)
BaseStd_t	*basestd;
{
  real		step, r, v ,b;
  int		i;
  c_Color	*first;
  c_Color	**base;
  
  base = &basestd->BG_color_base;
  *base = NULL;
  first = (c_Color *)NEW(c_Color)(base, "Black");
  F(first).set(first, XcC_GRAY, XcC_SCALE_COLOR(0.0), XcC_END);
  F(*base).getColor(base, XcC_GRAY, XcC_SCALE_COLOR(0.25), XcC_END);
  F(*base).getColor(base, XcC_GRAY, XcC_SCALE_COLOR(0.5), XcC_END);
  F(*base).getColor(base, XcC_GRAY, XcC_SCALE_COLOR(0.75), XcC_END);
  F(*base).getColor(base, XcC_GRAY, XcC_SCALE_COLOR(1.0), XcC_END);
  F(*base).getColor(base, XcC_TRANSPARENCY, XcC_SCALE_COLOR(1.0), XcC_END);
  
  step = 1.0 / BG_NB_COLOR_STEP;
  r = v = 0.0; b = 1.0 ; 
  for(i=0; i<BG_NB_COLOR_STEP; i++)
  {
    F(*base).getColor(base, XcC_RGB, XcC_SCALE_COLOR(r),
		      XcC_SCALE_COLOR(v), XcC_SCALE_COLOR(b), XcC_END);
    v += step;
  }
  for(i=0; i<BG_NB_COLOR_STEP; i++)
  {
    F(*base).getColor(base, XcC_RGB, XcC_SCALE_COLOR(r),
		      XcC_SCALE_COLOR(v), XcC_SCALE_COLOR(b), XcC_END);
    b -= step;
  }
  for(i=0; i<BG_NB_COLOR_STEP; i++)
  {
    F(*base).getColor(base, XcC_RGB, XcC_SCALE_COLOR(r),
		      XcC_SCALE_COLOR(v), XcC_SCALE_COLOR(b), XcC_END);
    r += step;
  }
  for(i=0; i<BG_NB_COLOR_STEP; i++)
  {
    F(*base).getColor(base, XcC_RGB, XcC_SCALE_COLOR(r),
		      XcC_SCALE_COLOR(v), XcC_SCALE_COLOR(b), XcC_END);
    v -= step;
  }
  for(i=0; i<BG_NB_COLOR_STEP; i++)
  {
    F(*base).getColor(base, XcC_RGB, XcC_SCALE_COLOR(r),
		      XcC_SCALE_COLOR(v), XcC_SCALE_COLOR(b), XcC_END);
    b += step;
  }
  for(i=0; i<BG_NB_COLOR_STEP; i++)
  {
    F(*base).getColor(base, XcC_RGB, XcC_SCALE_COLOR(r),   
		      XcC_SCALE_COLOR(v), XcC_SCALE_COLOR(b), XcC_END);
    r -= step;
  }
  
}

void Destroy_BG_Color_Base(basestd)
BaseStd_t	*basestd;
{
  c_Color **base;
  
  base = &basestd->BG_color_base;
  while(*base != NULL)
     DELETE(c_Color)(*base);
  *base = NULL;
}

c_Color *Get_BG_Color(basestd, color)
BaseStd_t *basestd;
int color;
{
  c_Color	**base;
  c_Color	*retcolor;

  base = &basestd->BG_color_base;
  switch(color)
    {
    case BGC_BLACK:
      retcolor = 
	F(*base).getColor(base, XcC_GRAY, XcC_SCALE_COLOR(0.0), XcC_END);
      break;
    case BGC_GRAY_DARK:
      retcolor =
	F(*base).getColor(base, XcC_GRAY, XcC_SCALE_COLOR(0.25), XcC_END);
      break;
    case BGC_GRAY_MEDIUM:
      retcolor =
	F(*base).getColor(base, XcC_GRAY, XcC_SCALE_COLOR(0.5), XcC_END);
      break;
    case BGC_GRAY_LIGHT:
      retcolor =
	F(*base).getColor(base, XcC_GRAY, XcC_SCALE_COLOR(0.75), XcC_END);
	break;
    case BGC_WHITE:
      retcolor =
	F(*base).getColor(base, XcC_GRAY, XcC_SCALE_COLOR(1.0), XcC_END);
      break;
    case BGC_TRANSPARENT:
      retcolor =
	F(*base).getColor(base, XcC_TRANSPARENCY, XcC_SCALE_COLOR(1.0), 
			  XcC_END);
      break;
    default:
      Xc_WARNING(("Unknow pre-defined color"));
      retcolor = 
	F(*base).getColor(base, XcC_GRAY, XcC_SCALE_COLOR(0.0), XcC_END);
      break;
    }
  return retcolor;
}

c_Color *Get_BG_Color_Step(basestd, step)
BaseStd_t *basestd;
int step;
{
  c_Color  *base;
  int	   i, j;

  base = basestd->BG_color_base;
  j = (step * 11) % 60 + 6;
  for(i=0; i<j; i++)
    base = base->next;
  return base;
}

void Create_BG_TextStyle_Base(basestd)
BaseStd_t	*basestd;
{
  c_VectorFont	*vfont;
  c_TextStyle	**base;
  c_Color	*black;
  
  base = &basestd->BG_text_style_base;
  black = Get_BG_Color(basestd, BGC_BLACK);

  vfont = F(GlobFontBase).getFontByName(GlobFontBase, "Times-Bold");
  if(!vfont) vfont = GlobFontBase;
  NEW(c_TextStyle)(base, "Title", vfont, SCALE_FROM_POINTS(14), black);
  vfont = F(GlobFontBase).getFontByName(GlobFontBase, "Times-Roman");
  if(!vfont) vfont = GlobFontBase;
  NEW(c_TextStyle)(base, "Legend", vfont, SCALE_FROM_POINTS(12), black);
  vfont = F(GlobFontBase).getFontByName(GlobFontBase, "Helvetica");
  if(!vfont) vfont = GlobFontBase;
  NEW(c_TextStyle)(base, "Graduate", vfont, SCALE_FROM_POINTS(8), black);
  vfont = F(GlobFontBase).getFontByName(GlobFontBase, "Helvetica");
  if(!vfont) vfont = GlobFontBase;
  NEW(c_TextStyle)(base, "Normal", vfont, SCALE_FROM_POINTS(10), black);
}

void Destroy_BG_TextStyle_Base(basestd)
BaseStd_t	*basestd;
{
  c_TextStyle **base;
  
  base = &basestd->BG_text_style_base;
  while(*base != NULL)
    DELETE(c_TextStyle)(*base);
  *base = NULL;
}

c_TextStyle *Get_BG_TextStyle(basestd, tstyle)
BaseStd_t *basestd;
int tstyle;
{
  c_TextStyle **base;
  c_TextStyle	*rettstyle;
  int		i;
  
  base = &basestd->BG_text_style_base;
  
/*  switch(tstyle)
    {
    case BGS_TITLE: 
      rettstyle =
	F(*base).get(base, XcTS_NAME, "Title", XcTS_END);
      break;
    case BGS_LEGEND:
      rettstyle =
	F(*base).get(base, XcTS_NAME, "Legend", XcTS_END);
      break;
    case BGS_GRADUATE:
      rettstyle =
	F(*base).get(base, XcTS_NAME, "Graduate", XcTS_END);
      break;
    case BGS_NORMAL:
      rettstyle =
	F(*base).get(base, XcTS_NAME, "Normal", XcTS_END);
      break;
    default:
      Xc_WARNING(("Unknow pre-defined style"));
      break;
    } */
  
  rettstyle = *base;
  for(i=0; i< BGS_NORMAL-tstyle; i++)
    rettstyle = rettstyle->next;
  return rettstyle;
}
