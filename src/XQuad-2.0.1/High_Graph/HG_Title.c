/*
** HG_Title.c for XQuad in High_Graph/
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
** Last update Sat Oct 26 20:34:35 1996 One of the authors
*/

#include "HG_Title.h"
#include <stdarg.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void regenerate ___NPROTO((c_HG_Legend *This, bbox_t *bbox));
static void change_xinfo ___PROTO((c_HG_Title *This));
static error change_base ___PROTO((c_HG_Title *This));
static void display_redraw ___PROTO((c_HG_Title *This));
static void add_to_vectorgraph ___PROTO((c_HG_Title *This));
static void sub_to_vectorgraph ___PROTO((c_HG_Title *This));
static void setHG ___PROTO((c_HG_Title *This, ...));
static void vset();
static void calcul_Title ___PROTO((c_HG_Title *This));
static void ps_print ___PROTO((c_HG_Title *This, c_PostScript *post));
static boolean readHG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeHG ___PROTO((c_HG_Title *This, c_DocFile *doc));

sf_HG_Title fc_HG_Title =
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
  c_HG_Title	*This;
  
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("HG_Title", sizeof(c_HG_Title)) ) == NULL)
    return NULL;
  This->f = &fc_HG_Title;
  
  This->highg = highg;
  This->class = HG_TITLE;
  This->sub_class = sub_class;
  
  This->line = NULL;
  This->string = NULL;
  
  Xc_TRACE(("constructor done"));
  return This;
}  

static void dest(This)
c_HG_Title	*This;
{
  Xc_HISTORY(("destructor")); 
  if (This->line)
    DELETE(c_BG_Line)(This->line);
  if (This->string)
    DELETE(c_BG_Text)(This->string);
  Xc_free(This);
  Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_HG_Title	*This;
BaseStd_t	*base;
{
  c_HG_Title	*hg_copy;
  
  if ( (hg_copy = Xc_malloc("HG_Title", sizeof(c_HG_Title)) ) == NULL)
    return NULL;
  memcpy(hg_copy, This, sizeof(c_HG_Title));
  
  return hg_copy;
}

static void regenerate(This, bbox)
c_HG_Title	*This;
bbox_t		*bbox;
{
  c_HighGraph	*highg;
  vector_t	point1, point2;
  char		*string;
  c_TextStyle	*tstyle;
  
  highg = This->highg;
  
  if (This->line)
    DELETE(c_BG_Line)(This->line);
  if (This->string)
    DELETE(c_BG_Text)(This->string);
  
  string = highg->tab_title.string;
  tstyle = highg->tab_title.tstyle;
  if (tstyle->point_size > (BG_AREA_HEIGHT / 4))
    tstyle = Get_BG_TextStyle(_BaseStd, BGS_TITLE);
  
  This->line = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
  point1.dx = bbox->llx = 0;
  point2.dx = bbox->urx = BG_AREA_WIDTH;
  point1.dy = point2.dy = bbox->lly = bbox->ury = 
    SCALE_FROM_MILLIMETERS(1) + tstyle->point_size;
  
  BG_SET(This->line)(This->line, XcBG_Line_POINT1, point1, 
		     XcBG_Line_POINT2, point2, XcBG_Line_END);
  
  This->string = (c_BG_Text *)NEW(c_BG_Text)(_BaseStd);
  point1.dy = SCALE_FROM_MILLIMETERS(0.5);
  
  BG_SET(This->string)(This->string, 
		       XcBG_Text_ORIGIN, point1,
		       XcBG_Text_SIZE, point2,
		       XcBG_Text_STRING, string, 
		       XcBG_Text_STYLE, tstyle,
		       XcBG_Text_RULER, XqR_H_ALIGN_CENTER|XqR_V_ALIGN_CENTER,
		       XcBG_Text_END);
}

static void change_xinfo(This)
c_HG_Title	*This;
{
  if (This->string)
    BG_CHANGE_XINFO(This->string)(This->string, &This->highg->X_info);
  if (This->line)
    BG_CHANGE_XINFO(This->line)(This->line, &This->highg->X_info);
}

static error change_base(This)
c_HG_Title	*This;
{
  return XC_NO_ERROR;
}

static void display_redraw(This)
c_HG_Title	*This;
{
  c_HighGraph	*highg;
  
  Xc_TRACE(("redraw title"));
  highg = This->highg;
  if (This->string)
    BG_DISPLAY_REDRAW(This->string)(This->string,
				    highg->matrix, highg->scale, 0, 0);
  if (This->line)
    BG_DISPLAY_REDRAW(This->line)(This->line,
				  highg->matrix, highg->scale, 0, 0);
}

static void add_to_vectorgraph(This)
c_HG_Title  *This;
{
  if (This->string)
    BG_ADD_TO_VECTORGRAPH(This->string)(This->string, This->highg->vectorg);
  if (This->line)
    BG_ADD_TO_VECTORGRAPH(This->line)(This->line, This->highg->vectorg);
}

static void sub_to_vectorgraph(This)
c_HG_Title  *This;
{
  if (This->string)
    BG_SUB_TO_VECTORGRAPH(This->string)(This->string, This->highg->vectorg);
  if (This->line)
    BG_SUB_TO_VECTORGRAPH(This->line)(This->line, This->highg->vectorg);
}

static void setHG(c_HG_Title *This, ...)
{
  va_list ap;

  Xc_HISTORY(("set"));

  va_start(ap, This);
  vset(This, ap);
  va_end(ap);
} 

static void vset(This, ap)
c_HG_Title *This;
va_list ap;
{
  boolean out_flag;
  boolean calcul;
  
  Xc_TRACE(("vset"));
  
  out_flag = calcul = FALSE;
  do {
    switch(va_arg(ap, HG_Title_set_code_t))
    {
    case XcHG_Title_END:
      Xc_TRACE(("XcHG_Title_END"));
      out_flag = TRUE;
      break;
    default:
      Xc_BREAK(("Unknow HG_Title_set_code"));
      break;
    }
  } while(!out_flag);
  if (calcul)
    calcul_Title(This);
}

static void calcul_Title(This)
c_HG_Title *This;
{
}

/* -------------------------------------------------------------------- **
** ps print, read & write HG_Title					**
** -------------------------------------------------------------------- */ 
static void ps_print(This, post)
c_HG_Title *This;
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
c_HG_Title	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}



