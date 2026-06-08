/*
** BG_Matrice.c for XQuad in Basic_Graph/
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
** Last update Wed May 29 15:39:49 1996 One of the authors
*/

#include "BG_Matrice.h"
#include <varargs.h>

static void *cons();
static void dest();
static void *copy();
static void change_xinfo ___PROTO((c_BG_Matrice *This, x_info *xinfo));
static error change_base ___PROTO((c_BG_Matrice *This, BaseStd_t *new_base));
static void display_redraw ___PROTO((c_BG_Matrice *This, c_Matrix *matrix,
				     coord_t scale, int dx, int dy));
static void display_handles ___PROTO((c_BG_Matrice *This, c_Matrix *matrix,
				      coord_t scale, int dx, int dy));
static void add_to_vectorgraph ___PROTO((c_BG_Matrice *This, 
					 c_VectorGraph *vectorg));
static bbox_t *get_bounding_box ___PROTO((c_BG_Matrice *This));
static boolean is_in ___PROTO((c_BG_Matrice *This, coord_t x, coord_t y));
static void setBG(); /* ___PROTO((c_BG_Matrice *This, ...)); */
static void vset();
static boolean ps_print ___PROTO((c_BG_Matrice *This, c_PostScript *post));
static boolean readBG___PROTO((c_DocFile *doc, char *keyword,
			       long param, BaseStd_t *base));
static boolean writeBG ___PROTO((c_BG_Matrice *This, c_DocFile *doc));

sf_BG_Matrice fc_BG_Matrice =
{
  cons, dest, copy,
  change_xinfo, change_base, display_redraw, display_handles,
  add_to_vectorgraph, get_bounding_box, is_in, setBG,
  ps_print, readBG, writeBG
};


static void *cons(base)
BaseStd_t	*base;
{
  c_BG_Matrice	*This;
  
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("BG_Matrice", sizeof(c_BG_Matrice)) ) == NULL)
    return NULL;
  This->f = &fc_BG_Matrice;
  
  This->type = BG_MATRICE;
  This->BaseStd = base;
  
  Xc_TRACE(("constructor done"));
  return This;
}

static void dest(This)
c_BG_Matrice	*This;
{
  Xc_HISTORY(("destructor")); 
  
  Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_BG_Matrice	*This;
BaseStd_t	*base;
{
  c_BG_Matrice	*bg_copy;
  
  if ( (bg_copy = Xc_malloc("BG_Matrice", sizeof(c_BG_Matrice)) ) == NULL)
    return NULL;
  memcpy(bg_copy, This, sizeof(c_BG_Matrice));
  
  if (bg_copy->BaseStd != base)
    BG_CHANGE_BASE(bg_copy)(bg_copy, base);
  return bg_copy;
}

static void change_xinfo(This, xinfo)
c_BG_Matrice	*This;
x_info		*xinfo;
{
  memcpy(&This->X_info, xinfo, sizeof(x_info));
}

static error change_base(This, new_base)
c_BG_Matrice	*This;
BaseStd_t	*new_base;
{
  This->BaseStd = new_base;
  return XC_NO_ERROR;
}

static void display_redraw(This, matrix, scale, dx, dy)
c_BG_Matrice	*This;
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

static void display_handles(This, matrix, scale, dx, dy)
c_BG_Matrice	*This;
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
c_BG_Matrice  *This;
c_VectorGraph *vectorg;
{
  F(vectorg).addBasicGraph(vectorg, This);
}

static bbox_t *get_bounding_box(This)
c_BG_Matrice *This;
{
  return &(This->bbox);
}

static boolean is_in(This, x, y)
c_BG_Matrice	*This;
coord_t		x;
coord_t		y;
{
  if (x >= This->bbox.llx && x <= This->bbox.urx &&
      y >= This->bbox.lly && y <= This->bbox.ury)
    return TRUE;
  return FALSE;
}

static void setBG(This, va_alist)
c_BG_Matrice *This;
va_dcl
{
  va_list ap;
  
  Xc_HISTORY(("set"));

  va_start(ap);
  vset(This, ap);
  va_end(ap);
} 

static void vset(This, ap)
c_BG_Matrice *This;
va_list ap;
{
  boolean out_flag;
  
  Xc_TRACE(("vset"));
  
  out_flag = FALSE;
  do {
    switch(va_arg(ap, BG_Matrice_set_code_t))
    {
    case XcBG_Matrice_END:
      Xc_TRACE(("XcC_END"));
      out_flag = TRUE;
      break;
    default:
      Xc_BREAK(("Unknow BG_Matrice_set_code"));
      break;
    }
  } while(!out_flag);
}

/* -------------------------------------------------------------------- **
** ps print, read & write BG_Matrice					**
** -------------------------------------------------------------------- */ 
static boolean ps_print(This, post)
c_BG_Matrice *This;
c_PostScript *post;
{
  Xc_HISTORY(("ps_print"));
  
  return TRUE;
}

static boolean read(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
BaseStd_t *base;
{
  return FALSE;
}

static boolean write(This, doc)
c_BG_Matrice	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}



