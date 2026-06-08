/*
** docfile.c for XQuad in CellStyle/
** Loading & saving methods
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
** Started on  Thu Apr  6 12:51:35 1995 Stéphane Boisson
** Last update Wed Jun 28 11:19:19 1995 Emmanuel Paris
*/

#define NTRACE

#include "CellStyleP.h"

/*--- Structures ---*/
typedef struct {
  CellStyle_t *cell_style;
  BaseStd_t *base;
} load_data_t;

typedef struct {
  c_Color *base;
  c_Color *color;
} load_color_t;

typedef struct {
  load_color_t foreground;
  int pattern;
} load_pattern_t;

typedef struct {
  boolean loaded_flag;
  load_pattern_t pattern;
  load_color_t background;
  int dash;
} load_border_t;


/*--- Local prototypes ---*/
static boolean writeCellBorder ___PROTO((CellBorderStyle_t *this,
					 c_DocFile *doc));

static boolean cb_dash ___PROTO((c_DocFile *doc, char *keyword,
				 long param, int *data));
static boolean cb_color ___PROTO((c_DocFile *doc, char *keyword,
				  long param, load_color_t *data));
static boolean cb_pattern ___PROTO((c_DocFile *doc, char *keyword,
				    long param, load_pattern_t *data));
static boolean cb_border ___PROTO((c_DocFile *doc, char *keyword,
				   long param, load_border_t *data));
static boolean cb_textstyle ___PROTO((c_DocFile *doc, char *keyword,
				      long param, load_data_t *data));
static boolean cb_format ___PROTO((c_DocFile *doc, char *keyword,
				   long param, load_data_t *data));
static boolean cb_ruler ___PROTO((c_DocFile *doc, char *keyword,
				  long param, load_data_t *data));


/* ----------------------------------------------------------------- ** 
** writeCellStyle - Saving method                                    ** 
** ----------------------------------------------------------------- */
boolean writeCellStyle(this, doc)
CellStyle_t *this;
c_DocFile *doc;
{
  Xc_HISTORY(("write(%lx)", this));

  if(!F(doc).writeKeyword(doc, XcDF_CELLSTYLE_KEYWORD, this->id) ||
     !F(doc).startSequence(doc)) return FALSE;

  /*--- Cell background color ---*/
  if(!F(doc).writeKeyword(doc, XcDF_STYLE_KEYWORD, this->text_style->id) ||
     !F(doc).writeKeyword(doc, XcDF_FORMAT_KEYWORD, this->format->id) ||
     !F(doc).writeKeyword(doc, XcDF_RULER_KEYWORD, (long)this->ruler_type) ||
     !F(doc).writeKeyword(doc, XcDF_COLOR_KEYWORD,
			  this->inside_background->id)) return FALSE;

  /*--- Write left border info ---*/
  if((this->left.border_thickness != 0) &&
     (!F(doc).writeKeyword(doc, XcDF_LEFT_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !writeCellBorder(&this->left, doc) ||
      !F(doc).endSequence(doc))) return FALSE;

  /*--- Write right border info ---*/
  if((this->right.border_thickness != 0) &&
     (!F(doc).writeKeyword(doc, XcDF_RIGHT_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !writeCellBorder(&this->right, doc) ||
      !F(doc).endSequence(doc))) return FALSE;
  
  /*--- Write top border info ---*/
  if((this->up.border_thickness != 0) &&
     (!F(doc).writeKeyword(doc, XcDF_TOP_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !writeCellBorder(&this->up, doc) ||
      !F(doc).endSequence(doc))) return FALSE;

  /*--- Write bottom border info ---*/
  if((this->down.border_thickness != 0) &&
     (!F(doc).writeKeyword(doc, XcDF_BOTTOM_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !writeCellBorder(&this->down, doc) ||
      !F(doc).endSequence(doc))) return FALSE;

  /*--- Pattern ---*/
  if((this->inside_stipple_pattern != PATTERN_NONE) &&
     (!F(doc).writeKeyword(doc, XcDF_PATTERN_KEYWORD,
			   (long)this->inside_stipple_pattern) ||
      !F(doc).startSequence(doc) ||
      !F(doc).writeKeyword(doc, XcDF_COLOR_KEYWORD,
			   this->inside_foreground->id) ||
      !F(doc).endSequence(doc))) return FALSE;
  
  Xc_TRACE(("wrote"));
  return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** writeCellBorder - Write a cell border style                       ** 
** ----------------------------------------------------------------- */
static boolean writeCellBorder(this, doc)
CellBorderStyle_t *this;
c_DocFile *doc;
{
  Xc_TRACE(("writeCellBorder"));
 
  /*--- Border thickness & color ---*/
  if(!F(doc).writeKeyword(doc, XcDF_COLOR_KEYWORD,
			  this->border_background->id)) return FALSE;
  
  /*--- Pattern ---*/
  if((this->border_stipple_pattern != PATTERN_NONE) &&
     (!F(doc).writeKeyword(doc, XcDF_PATTERN_KEYWORD,
			   (long)this->border_stipple_pattern) ||
      !F(doc).startSequence(doc) ||
      !F(doc).writeKeyword(doc, XcDF_COLOR_KEYWORD,
			   this->border_foreground->id) ||
      !F(doc).endSequence(doc))) return FALSE;
  
  /*--- Dash ---*/
  if((this->border_dash_pattern != PATTERN_NONE) &&
     !F(doc).writeKeyword(doc, XcDF_DASH_KEYWORD,
			  (long)this->border_dash_pattern))
    return FALSE;

  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** writeCellStyleList - List saving method                           ** 
** ----------------------------------------------------------------- */
boolean writeCellStyleList(this, doc)
c_CellStyle *this;
c_DocFile *doc;
{
  CellStyle_t *ptr;
  long id;

  Xc_HISTORY(("writeList"));

  if(!F(doc).writeKeyword(doc, XcDF_CELLSTYLELIST_KEYWORD, -1L) ||
     !F(doc).startSequence(doc)) return FALSE;

  for(id = 0, ptr = this->cell_style; ptr != NULL; ptr = ptr->NextCellStyle)
    {
      ptr->id = id++;
      if(!F(this).write(ptr, doc)) return FALSE;
    }

  return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** readCellStyleList - List loading method                           ** 
** ----------------------------------------------------------------- */
boolean readCellStyleList(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
BaseStd_t *base;
{
  Xc_TRACE(("readList"));

  if(!F(doc).addCallbacks(doc,
			  XcDF_CELLSTYLE_KEYWORD, readCellStyle, base,
			  NULL)) return FALSE;
  if(F(doc).expectKeyword(doc, XcDF_CELLSTYLE_KEYWORD, TRUE))
    {
      base->cell_style_base->default_cstyle->count = 1;
      return TRUE;
    }
  else
    return FALSE;
}


/* ----------------------------------------------------------------- ** 
** readCellStyle - Loading method                                    ** 
** ----------------------------------------------------------------- */
boolean readCellStyle(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
BaseStd_t *base;
{
  CellStyle_t **link, *previous;
  load_data_t data;
  c_CellStyle *cs;

  Xc_TRACE(("read"));
  Xc_ASSERT(base->color_base != NULL);

  cs = base->cell_style_base;

  data.cell_style = F(cs).CreateNewCellStyle(cs, CELL_STYLE_DEFAULT);
  if(data.cell_style == NULL) return FALSE;

  previous = NULL;
  for(link = &cs->cell_style; *link != NULL; link = &(*link)->NextCellStyle)
    previous = *link;

  data.base = base;
  while(42)
    {
      load_pattern_t pattern;
      load_color_t background;
      load_border_t bottom;
      load_border_t right;
      load_border_t left;
      load_border_t top;

      /*--- Init loading structures ---*/
      background.base = pattern.foreground.base = base->color_base;
      bottom.background.base = top.background.base = base->color_base;
      left.background.base = right.background.base = base->color_base;
      background.color = pattern.foreground.color = NULL;
      bottom.loaded_flag = top.loaded_flag = FALSE;
      left.loaded_flag = right.loaded_flag = FALSE;
      pattern.pattern = PATTERN_NONE;
      
      if(!F(doc).addCallbacks(doc,
			      XcDF_TOP_KEYWORD, cb_border, &top,
			      XcDF_BOTTOM_KEYWORD, cb_border, &bottom,
			      XcDF_LEFT_KEYWORD, cb_border, &left,
			      XcDF_RIGHT_KEYWORD, cb_border, &right,
			      XcDF_COLOR_KEYWORD, cb_color, &background,
			      XcDF_PATTERN_KEYWORD, cb_pattern, &pattern,
			      XcDF_STYLE_KEYWORD, cb_textstyle, &data,
			      XcDF_FORMAT_KEYWORD, cb_format, &data,
			      XcDF_RULER_KEYWORD, cb_ruler, &data,
			      NULL)) return FALSE;

      if(!F(doc).expectKeyword(doc, NULL, TRUE)) break;

      /*--- Set cell style attributes ---*/
      if(background.color != NULL)
	F(cs).set(data.cell_style, XqCS_BACKGROUND,
		  background.color, XqCS_END);
      
      if(pattern.pattern != PATTERN_NONE)
	F(cs).set(data.cell_style, XqCS_PATTERN, pattern.pattern, XqCS_END);
      
      if(pattern.foreground.color != NULL)
	F(cs).set(data.cell_style, XqCS_FOREGROUND,
		  pattern.foreground.color, XqCS_END);

      /*--- Left border parameters ---*/
      if(left.loaded_flag != FALSE)
	{
	  F(cs).set(data.cell_style,
		    XqCS_LEFT_PATTERN, left.pattern.pattern,
		    XqCS_LEFT_DASH, left.dash, XqCS_END);
	  if(left.background.color != NULL)
	    F(cs).set(data.cell_style, XqCS_LEFT_BACKGROUND,
		      left.background.color, XqCS_END);
	  if(left.pattern.foreground.color != NULL)
	    F(cs).set(data.cell_style, XqCS_LEFT_FOREGROUND,
		      left.pattern.foreground.color, XqCS_END);
	}

      /*--- Right border parameters ---*/
      if(right.loaded_flag != FALSE)
	{
	  F(cs).set(data.cell_style,
		    XqCS_RIGHT_PATTERN, right.pattern.pattern,
		    XqCS_RIGHT_DASH, right.dash, XqCS_END);
	  if(right.background.color != NULL)
	    F(cs).set(data.cell_style, XqCS_RIGHT_BACKGROUND,
		      right.background.color, XqCS_END);
	  if(right.pattern.foreground.color != NULL)
	    F(cs).set(data.cell_style, XqCS_RIGHT_FOREGROUND,
		      right.pattern.foreground.color, XqCS_END);
	}

      /*--- Top border parameters ---*/
      if(top.loaded_flag != FALSE)
	{
	  F(cs).set(data.cell_style,
		    XqCS_TOP_PATTERN, top.pattern.pattern,
		    XqCS_TOP_DASH, top.dash, XqCS_END);
	  if(top.background.color != NULL)
	    F(cs).set(data.cell_style, XqCS_TOP_BACKGROUND,
		      top.background.color, XqCS_END);
	  if(top.pattern.foreground.color != NULL)
	    F(cs).set(data.cell_style, XqCS_TOP_FOREGROUND,
		      top.pattern.foreground.color, XqCS_END);
	}
      /*--- Bottom border parameters ---*/
      if(bottom.loaded_flag != FALSE)
	{
	  F(cs).set(data.cell_style,
		    XqCS_BOTTOM_PATTERN, bottom.pattern.pattern,
		    XqCS_BOTTOM_DASH, bottom.dash, XqCS_END);
	  if(bottom.background.color != NULL)
	    F(cs).set(data.cell_style, XqCS_BOTTOM_BACKGROUND,
		      bottom.background.color, XqCS_END);
	  if(bottom.pattern.foreground.color != NULL)
	    F(cs).set(data.cell_style, XqCS_BOTTOM_FOREGROUND,
		      bottom.pattern.foreground.color, XqCS_END);
	}

      data.cell_style->id = param;
      data.cell_style->hook_info = &(cs->cstyle_hook);
      
      /*--- Add to linked list ---*/
      *link = data.cell_style;
      data.cell_style->NextCellStyle = NULL;
      data.cell_style->PrevCellStyle = previous;
      previous = data.cell_style;
      link = &data.cell_style->NextCellStyle;
      cs->nb_style++;

      /*--- Last cellstyle is the default one ---*/
      cs->default_cstyle = data.cell_style;

      return TRUE;
    }

  Xc_TRACE(("error"));
  F(cs).DeleteCellStyle(data.cell_style);
  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_color - Color loading callback                                 ** 
** ----------------------------------------------------------------- */
static boolean cb_color(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
load_color_t *data;
{
  c_Color *ptr;

  Xc_TRACE(("cb_color(%ld)", param));

  if(param < 0) param = 0;
  for(ptr = data->base; ptr != NULL; ptr = ptr->next)
    if(ptr->id == param)
      {
	data->color = ptr;
        return TRUE;
      }

  Xc_ERROR(("Bad color #%ld", param));
  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_pattern - Pattern loading callback                             ** 
** ----------------------------------------------------------------- */
static boolean cb_pattern(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
load_pattern_t *data;
{
  Xc_TRACE(("cb_pattern(%ld)", param));

  if(!F(doc).addCallbacks(doc,
			  XcDF_COLOR_KEYWORD, cb_color, &data->foreground,
			  NULL)) return FALSE;
  data->pattern = (int)param;
  return F(doc).expectKeyword(doc, XcDF_COLOR_KEYWORD, TRUE);
}


/* ----------------------------------------------------------------- ** 
** cb_border - Border loading callback                               ** 
** ----------------------------------------------------------------- */
static boolean cb_border(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
load_border_t *data;
{
  Xc_TRACE(("cb_border(%s)", keyword));

  data->loaded_flag = TRUE;
  data->pattern.pattern = data->dash = PATTERN_NONE;
  data->pattern.foreground.base = data->background.base;
  data->pattern.foreground.color = data->background.color = NULL;

  if(!F(doc).addCallbacks(doc,
			  XcDF_COLOR_KEYWORD, cb_color, &data->background,
			  XcDF_PATTERN_KEYWORD, cb_pattern, &data->pattern,
			  XcDF_DASH_KEYWORD, cb_dash, &data->dash,
			  NULL)) return FALSE;

  return F(doc).expectKeyword(doc, NULL, TRUE);
}


/* ----------------------------------------------------------------- ** 
** cb_textstyle - TextStyle loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_textstyle(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
load_data_t *data;
{
  c_TextStyle *ptr;

  Xc_TRACE(("cb_textstyle(%ld)", param));

  if(param < 0) param = 0;
  for(ptr = data->base->text_style_base; ptr != NULL; ptr = ptr->next)
    if(ptr->id == param)
      {
	F(data->base->cell_style_base).set(data->cell_style,
				XqCS_TEXTSTYLE, ptr, XqCS_END);
        return TRUE;
      }

  Xc_ERROR(("Bad text style #%ld", param));
  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_format - Format loading callback                               ** 
** ----------------------------------------------------------------- */
static boolean cb_format(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
load_data_t *data;
{
  c_FormatNbr *ptr;

  Xc_TRACE(("cb_format(%ld)", param));

  if(param < 0) param = 0;
  for(ptr = data->base->format_base; ptr != NULL; ptr = ptr->next)
    if(ptr->id == param)
      {
	F(data->base->cell_style_base).set(data->cell_style,
					   XqCS_FORMAT, ptr, XqCS_END);
        return TRUE;
      }

  Xc_ERROR(("Bad format #%ld", param));
  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_ruler - Format loading callback                                ** 
** ----------------------------------------------------------------- */
static boolean cb_ruler(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
load_data_t *data;
{
  Xc_TRACE(("cb_ruler(%ld)", param));

  F(data->base->cell_style_base).set(data->cell_style, XqCS_ALIGNEMENT,
				     (ruler_t)param, XqCS_END);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_dash - Dash loading callback                                   ** 
** ----------------------------------------------------------------- */
static boolean cb_dash(doc, keyword, param, ptr)
c_DocFile *doc;
char *keyword;
long param;
int *ptr;
{
  Xc_TRACE(("cb_dash(%ld)", param));

  *ptr = (int) param;
  return TRUE;
}
