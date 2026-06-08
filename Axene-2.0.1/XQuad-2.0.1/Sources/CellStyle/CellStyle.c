/*
** CellStyle.c for XQuad in CellStyle/
** CellStyle Object
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
** Started on  Tue Mar 28 13:44:01 1995 Emmanuel Paris
** Last update Wed Dec 18 15:19:22 1996 Emmanuel Paris
*/

#define NTRACE
#define NHISTORY

#include <varargs.h>
#define _CELLSTYLE_PRIVATE_
#include "CellStyleP.h"
#include "Feuille.h" 

extern c_VectorFont	*GlobFontBase;

static void *constructor();
static void destructor();
static void *copy();

static void init();
static CellStyle_t *CreateNewCellStyle();
static void DeleteCellStyle();
static CellStyle_t *AddCellStyleToBase();
static void RemoveCellStyleFromBase();
static void DisplayCellStyle();
static boolean compare ___PROTO((CellStyle_t *a, CellStyle_t *b));
static void setCellStyle() /* ___PROTO((CellStyle_t *this, ...)) */;
static CellStyle_t *getCellStyle() /* ___PROTO((c_CellStyle *base, ...)) */;

/*--- Private functions ---*/
static void vset ___PROTO((CellStyle_t *thiXcFt_s, va_list ap));
static void hook_color ___PROTO((CellStyle_t *this, int reason,
                                 c_Color *old, c_Color *new));
static void hook_textstyle ___PROTO((CellStyle_t *this, int reason,
				     c_TextStyle *old, c_TextStyle *new));
static void hook_format ___PROTO((CellStyle_t *this, int reason,
				  c_FormatNbr *old, c_FormatNbr *new));

/*--- Methods list ---*/
sf_CellStyle fc_CellStyle =
{
  constructor, destructor, copy, init,
  CreateNewCellStyle,
  DeleteCellStyle,
  AddCellStyleToBase,
  RemoveCellStyleFromBase,
  DisplayCellStyle,
  compare,
  (void (*) ___PROTO((CellStyle_t *this, ...)))setCellStyle,
  (CellStyle_t *(*) ___PROTO((c_CellStyle *this, ...)))getCellStyle,
  readCellStyle, readCellStyleList,
  writeCellStyle, writeCellStyleList,
  printCSBackground, printCSAddBorders, printCSFlushBorders, printCSText,
  printCSClearBorders, printCSSetCellMargins, CalculTextOverrun,
  printCSRemoveBorder, CalculIdealCellWidth, CalculIdealCellHeight
};


/* ----------------------------------------------------------------- ** 
** constructor - Constructor                                         ** 
** ----------------------------------------------------------------- */
static void *constructor(BaseStd)
BaseStd_t	*BaseStd;
{
  c_CellStyle	*This;
  CellStyle_t	*cell_style;
  
  Xc_HISTORY(("constructor"));

  if((This = Xc_malloc("CellStyle", sizeof(c_CellStyle))) == NULL) return NULL;

  This->f = &fc_CellStyle;
  
  This->BaseStd = BaseStd;
  This->text_color = -1;
  memset(&This->cell_style_current, 0, sizeof(CellStyle_t));
  
  This->nb_style = 0;
  This->cell_style = NULL;
  
  cell_style = F(This).CreateNewCellStyle(This, CELL_STYLE_DEFAULT); 
  F(This).AddCellStyleToBase(This, cell_style);
  This->default_cstyle = cell_style;
  This->display_formula = FALSE;
  This->display_zero_cell = TRUE;
  This->init = FALSE;
  This->gc_mask = (GC)0;
  Xc_TRACE(("built"));
  return This;
}


/* ----------------------------------------------------------------- ** 
** destructor - Destructor                                           ** 
** ----------------------------------------------------------------- */
static void destructor(This)
c_CellStyle *This;
{
  CellStyle_t *ptr;
  
  Xc_HISTORY(("destructor"));

  if (This->init)
    {
      /*--- Free GCs ---*/
      Xc_TRACE(("gc_inside"));
      XFreeGC(This->display, This->gc_inside);
      Xc_TRACE(("gc_left"));
      XFreeGC(This->display, This->gc_left);
      Xc_TRACE(("gc_right"));
      XFreeGC(This->display, This->gc_right);
      Xc_TRACE(("gc_up"));
      XFreeGC(This->display, This->gc_up);
      Xc_TRACE(("gc_down"));
      XFreeGC(This->display, This->gc_down);
      Xc_TRACE(("gc_paint"));
      XFreeGC(This->display, This->gc_paint);
      Xc_TRACE(("gc_mask"));
      if (This->gc_mask != (GC)0)
	XFreeGC(This->display, This->gc_mask);
    }
  /*--- Delete Cell styles ---*/
  Xc_TRACE(("CellStyle_t"));
  for(ptr = This->cell_style; ptr != NULL; )
    {
      CellStyle_t *tmp;
      
      tmp = ptr;
      ptr = ptr->NextCellStyle;
      
      tmp->count = 0;
      F(This).DeleteCellStyle(tmp);
      This->nb_style--;
    }
  Xc_ASSERT(This->nb_style == 0);
  
  Xc_TRACE(("destroyed"));
  Xc_free(This);
}


/* ----------------------------------------------------------------- ** 
** copy - Copy method                                                ** 
** ----------------------------------------------------------------- */
static void *copy(This)
c_CellStyle *This;
{
  c_CellStyle *ObjTmp;
  
  Xc_HISTORY(("copy"));

  if((ObjTmp = Xc_malloc("CCellStyle", sizeof(c_CellStyle))) == NULL)
    return NULL;

  memcpy(ObjTmp, This, sizeof(c_CellStyle));

  Xc_TRACE(("copied"));
  return ObjTmp;
}


/* ----------------------------------------------------------------- ** 
** init - Init method                                                ** 
** ----------------------------------------------------------------- */
static void init(This, display, window, moteur, hook_callback, 
		 hook_callback_data)
c_CellStyle 	*This;
Display		*display;
Window		window;
void		*moteur;
void		(*hook_callback)();
void		*hook_callback_data;
{
  unsigned long	white, black;
  int		i;
  int		line, data;
  GC		gc;
  XGCValues	gc_values;

  This->display = display;
  This->window = window;
  This->moteur = moteur;
  This->cstyle_hook.hook_callback = hook_callback;
  This->cstyle_hook.hook_callback_data = hook_callback_data;
  
  white = 0; /* WhitePixel(display, DefaultScreen(display)); */
  black = 1; /* BlackPixel(display, DefaultScreen(display)); */
  gc_values.foreground = black;
  gc_values.background = white;
  gc_values.function = GXcopy;
  gc_values.line_width = 0;
  gc_values.line_style = LineSolid;
  gc = XCreateGC(display, window, GCForeground | GCBackground | 
		 GCFunction | GCLineWidth | GCLineStyle, &gc_values);
  This->gc_inside = gc;
  gc = XCreateGC(display, window, GCForeground | GCBackground | 
		 GCFunction | GCLineWidth | GCLineStyle, &gc_values);
  This->gc_left = gc;
  gc = XCreateGC(display, window, GCForeground | GCBackground | 
		 GCFunction | GCLineWidth | GCLineStyle, &gc_values);
  This->gc_right = gc;
  gc = XCreateGC(display, window, GCForeground | GCBackground | 
		 GCFunction | GCLineWidth | GCLineStyle, &gc_values);
  This->gc_up = gc;  
  gc = XCreateGC(display, window, GCForeground | GCBackground | 
		 GCFunction | GCLineWidth | GCLineStyle, &gc_values);
  This->gc_down = gc;
  gc = XCreateGC(display, window, GCForeground | GCBackground | 
		 GCFunction | GCLineWidth | GCLineStyle, &gc_values);
  This->gc_paint = gc;
  This->gc_mask = (GC)0;
  gc = (GC)0;
  
  if (!CellStyle_init_done)
    {
      CellStyle_init_done = TRUE;
      line = data = 0;
      for(i=0; i<NB_CELL_PATTERN; i++)
	{
	  if (Cell_Pattern[i].type == PATTERN_LINE)
	    {
	      Cell_Pattern[i].definition.line.definition=CPLineDef[i];
	    }
	  else
	    {
	      memcpy(&(Cell_Pattern[i].definition.data),
		     &(CPData[data++]), sizeof(CellPatternDataStyle_t));
	      
	      Cell_Pattern[i].definition.data.pixmap = 
		XCreateBitmapFromData(display, window, 
				      Cell_Pattern[i].definition.data.
				      definition, 
				      Cell_Pattern[i].width,
				      Cell_Pattern[i].height);
	    }
	} 
    }
  This->init = TRUE;
}


/* ----------------------------------------------------------------- ** 
** CreateNewCellStyle - Create a CellStyle_t item                    ** 
** ----------------------------------------------------------------- */
static CellStyle_t *CreateNewCellStyle(This, cell_style)
c_CellStyle	*This;
CellStyle_t	*cell_style;
{
  c_TextStyle	*text_style;
  CellStyle_t	*new_style;
  c_Color	*black;
  c_Color	*white;
  c_FormatNbr	*format;
  BaseStd_t	*base;
  boolean	flag;

  Xc_HISTORY(("CreateNewCellStyle"));

  new_style = (CellStyle_t *)Xc_malloc("new style", sizeof(CellStyle_t));
  if(new_style == NULL) return new_style;

  base = This->BaseStd;
  flag = FALSE;

  switch((long)cell_style)
    {
      /*--- No model ---*/
    case (int)CELL_STYLE_NONE:
      Xc_TRACE(("CELL_STYLE_NONE"));
      memset(new_style, 0, sizeof(CellStyle_t));
      flag = TRUE;
      break;

      /*--- Model ---*/
    case (int)CELL_STYLE_DEFAULT:
      Xc_TRACE(("CELL_STYLE_DEFAULT"));
      memset(new_style, 0, sizeof(CellStyle_t));
      black = F(base->color_base).getColor(&(base->color_base),
					   XcC_NAME, "Black",
					   XcC_GRAY, XcC_SCALE_COLOR(0.0),
					   XcC_TRANSPARENCY, 0,
					   XcC_END);
      if(black == NULL) break;
      white = F(base->color_base).getColor(&(base->color_base),
					   XcC_NAME, "White",
					   XcC_GRAY, XcC_SCALE_COLOR(1.0),
					   XcC_TRANSPARENCY, 0,
					   XcC_END);
      if(white == NULL) break;

      /*--- Border foreground ---*/
      if(!XcH_REGISTER(black, hook_color, new_style)) break;
      new_style->left.border_foreground = black;
      if(!XcH_REGISTER(black, hook_color, new_style)) break;
      new_style->right.border_foreground = black;
      if(!XcH_REGISTER(black, hook_color, new_style)) break;
      new_style->up.border_foreground = black;
      if(!XcH_REGISTER(black, hook_color, new_style)) break;
      new_style->down.border_foreground = black;

      /*--- Border background ---*/
      if(!XcH_REGISTER(white, hook_color, new_style)) break;
      new_style->left.border_background = white;
      if(!XcH_REGISTER(white, hook_color, new_style)) break;
      new_style->right.border_background = white;
      if(!XcH_REGISTER(white, hook_color, new_style)) break;
      new_style->up.border_background = white;
      if(!XcH_REGISTER(white, hook_color, new_style)) break;
      new_style->down.border_background = white;
      
      new_style->left.border_thickness = new_style->right.border_thickness = 
	new_style->up.border_thickness = new_style->down.border_thickness = 0;
      new_style->left.border_line_thickness =
	new_style->right.border_line_thickness = 
	new_style->up.border_line_thickness = 
	  new_style->down.border_line_thickness = 0;
      new_style->left.border_line_cap_style = 
	new_style->right.border_line_cap_style = 
	  new_style->up.border_line_cap_style = 
	    new_style->down.border_line_cap_style = CapButt;
      new_style->left.border_line_join_style = 
	new_style->right.border_line_join_style = 
	  new_style->up.border_line_join_style = 
	    new_style->down.border_line_join_style = JoinMiter;
      new_style->left.border_stipple_pattern = 
	new_style->right.border_stipple_pattern =
	  new_style->up.border_stipple_pattern =
	    new_style->down.border_stipple_pattern = PATTERN_NONE;
      new_style->left.border_dash_pattern = 
	new_style->right.border_dash_pattern =
	  new_style->up.border_dash_pattern =
	    new_style->down.border_dash_pattern = PATTERN_NONE;
    
      /*--- Inside cell area ---*/
      if(!XcH_REGISTER(white, hook_color, new_style)) break;
      new_style->inside_foreground = white;
      if(!XcH_REGISTER(black, hook_color, new_style)) break;
      new_style->inside_background = black;

      new_style->inside_stipple_pattern = PATTERN_NONE;
      new_style->ruler_type = XqR_DEFAULT;
    
      text_style = This->BaseStd->text_style_base;
      if(text_style == NULL) break;
      if(!XcH_REGISTER(text_style, hook_textstyle, new_style)) break;
      new_style->text_style = text_style;
      
      if ((format = 
	   F(base->format_base).get(NULL, XcFt_STANDARD, base)) != NULL)
	{
	  if(!XcH_REGISTER(format, hook_format, new_style)) break;
	  new_style->format = format;
	}
      else
	{
	  if(!XcH_REGISTER(base->format_base, hook_format, new_style)) break;
	  new_style->format = base->format_base;
	}
      flag = TRUE;
      break;
      
      /*--- Copy ---*/
    default:
      Xc_TRACE(("COPY"));
      memcpy(new_style, cell_style, sizeof(CellStyle_t));
      new_style->PrevCellStyle = new_style->NextCellStyle = NULL;
      new_style->left.border_foreground = NULL;
      new_style->right.border_foreground = NULL;
      new_style->up.border_foreground = NULL;
      new_style->down.border_foreground = NULL;
      new_style->left.border_background = NULL;
      new_style->right.border_background = NULL;
      new_style->up.border_background = NULL;
      new_style->down.border_background = NULL;
      new_style->text_style = NULL;
      new_style->inside_foreground = NULL;
      new_style->inside_background = NULL;

      /*--- Border foreground ---*/
      if(!XcH_REGISTER(cell_style->left.border_foreground,
		       hook_color, new_style)) break;
      new_style->left.border_foreground = cell_style->left.border_foreground;

      if(!XcH_REGISTER(cell_style->right.border_foreground,
		       hook_color, new_style)) break;
      new_style->right.border_foreground = cell_style->right.border_foreground;
      
      if(!XcH_REGISTER(cell_style->up.border_foreground,
		       hook_color, new_style)) break;
      new_style->up.border_foreground = cell_style->up.border_foreground;

      if(!XcH_REGISTER(cell_style->down.border_foreground,
		       hook_color, new_style)) break;
      new_style->down.border_foreground = cell_style->down.border_foreground;

      /*--- Border background ---*/
      if(!XcH_REGISTER(cell_style->left.border_background,
		       hook_color, new_style)) break;
      new_style->left.border_background = cell_style->left.border_background;

      if(!XcH_REGISTER(cell_style->right.border_background,
		       hook_color, new_style)) break;
      new_style->right.border_background = cell_style->right.border_background;

      if(!XcH_REGISTER(cell_style->up.border_background,
		       hook_color, new_style)) break;
      new_style->up.border_background = cell_style->up.border_background;

      if(!XcH_REGISTER(cell_style->down.border_background,
		       hook_color, new_style)) break;
      new_style->down.border_background = cell_style->down.border_background;

      /*--- Cell inside area ---*/
      if(!XcH_REGISTER(cell_style->inside_foreground,
		       hook_color, new_style)) break;
      new_style->inside_foreground = cell_style->inside_foreground;

      if(!XcH_REGISTER(cell_style->inside_background,
		       hook_color, new_style)) break;
      new_style->inside_background = cell_style->inside_background;

      /*--- Text style ---*/
      if(!XcH_REGISTER(cell_style->text_style, hook_textstyle, new_style))
	break;
      new_style->text_style = cell_style->text_style;

      if(!XcH_REGISTER(cell_style->format, hook_format, new_style)) break;
      new_style->format = cell_style->format;
      
      new_style->rfont = (c_RasterFont *)NULL;
      new_style->metrics = (c_CharMetrics *)NULL;
      new_style->factor = 0;
      flag = TRUE;
      break;
    }
  
  if(flag == FALSE)
    {
      F(This).DeleteCellStyle(new_style);
      return NULL;
    }

  new_style->count = 0;

  Xc_TRACE(("created (%lx)", (long)new_style));
  return new_style;
}


/* ----------------------------------------------------------------- ** 
** DeleteCellStyle - Delete a CellStyle Item                         ** 
** ----------------------------------------------------------------- */
static void DeleteCellStyle(this)
CellStyle_t *this;
{
  Xc_HISTORY(("DeleteCellStyle(%lx)", (long)this));

  /*--- Cell inside area colors ---*/
  if(this->inside_background != NULL)
    XcH_UNREGISTER(this->inside_background, hook_color, this);
  if(this->inside_foreground != NULL)
    XcH_UNREGISTER(this->inside_foreground, hook_color, this);

  /*--- Left border color ---*/
  if(this->left.border_background != NULL)
    XcH_UNREGISTER(this->left.border_background, hook_color, this);
  if(this->left.border_foreground != NULL)
    XcH_UNREGISTER(this->left.border_foreground, hook_color, this);

  /*--- Right border color ---*/
  if(this->right.border_background != NULL)
    XcH_UNREGISTER(this->right.border_background, hook_color, this);
  if(this->right.border_foreground != NULL)
    XcH_UNREGISTER(this->right.border_foreground, hook_color, this);

  /*--- Top border color ---*/
  if(this->up.border_background != NULL)
    XcH_UNREGISTER(this->up.border_background, hook_color, this);
  if(this->up.border_foreground != NULL)
    XcH_UNREGISTER(this->up.border_foreground, hook_color, this);

  /*--- Bottom border color ---*/
  if(this->down.border_background != NULL)
    XcH_UNREGISTER(this->down.border_background, hook_color, this);
  if(this->down.border_foreground != NULL)
    XcH_UNREGISTER(this->down.border_foreground, hook_color, this);

  /*--- Text style ---*/
  if(this->text_style != NULL)
    XcH_UNREGISTER(this->text_style, hook_textstyle, this);

  /*--- Format ---*/
  if(this->format != NULL)
    XcH_UNREGISTER(this->format, hook_format, this);
 
  /*--- Refererence counter ---*/
  Xc_ASSERT(this->count == 0);

  Xc_free(this);
  Xc_TRACE(("deleted"));
}


/* ----------------------------------------------------------------- ** 
** AddCellStyleToBase - Attach a CellStyle_t to a base               ** 
** ----------------------------------------------------------------- */
static CellStyle_t *AddCellStyleToBase(this, cell_style)
c_CellStyle *this;
CellStyle_t *cell_style;
{
  CellStyle_t *ptr;

  Xc_TRACE(("AddCellStyleToBase(%lx)", (long)cell_style));

  for(ptr = this->cell_style; ptr != NULL; ptr = ptr->NextCellStyle)
    if(compare(cell_style, ptr) != FALSE) 
    {
      if (cell_style != this->default_cstyle)
      {
	Xc_TRACE(("New CellStyle not added to base, already exist"));
	F(this).DeleteCellStyle(cell_style);
      }
      return ptr;
    }
  
  this->nb_style++;
  cell_style->PrevCellStyle = NULL;
  if((cell_style->NextCellStyle = this->cell_style) != NULL)
    this->cell_style->PrevCellStyle = cell_style;
  this->cell_style = cell_style;

  cell_style->hook_info = &(this->cstyle_hook);
  
  return cell_style;
}


/* ----------------------------------------------------------------- ** 
** RemoveCellStyleFromBase - Detach a CellStyle_t from a base        ** 
** ----------------------------------------------------------------- */
static void RemoveCellStyleFromBase(This, cell_style)
c_CellStyle *This;
CellStyle_t *cell_style;
{
  CellStyle_t **ptr;
  
  Xc_HISTORY(("RemoveCellStyleFromBase(%lx)", (long)cell_style));
  
  Xc_ASSERT(This->nb_style > 0);

  for(ptr = &This->cell_style; *ptr != NULL; ptr = &((*ptr)->NextCellStyle))
    if(*ptr == cell_style)
      {
	Xc_ASSERT(cell_style->count > 0 || cell_style != This->default_cstyle);
	if((cell_style == This->default_cstyle) ||
	   (cell_style->count-- > 1))
	  return;
	
	This->nb_style--;
	if(cell_style->NextCellStyle != NULL)
	  cell_style->NextCellStyle->PrevCellStyle = cell_style->PrevCellStyle;
	if(cell_style->PrevCellStyle != NULL)
	  cell_style->PrevCellStyle->NextCellStyle = cell_style->NextCellStyle;
	*ptr = cell_style->NextCellStyle;
	
	F(This).DeleteCellStyle(cell_style);
	return;
      }

  Xc_BREAK(("not found"));
  return;
}


/* ----------------------------------------------------------------- ** 
** DisplayCellStyle - Draw cell style                                ** 
** ----------------------------------------------------------------- */
static void DisplayCellStyle(This, cell_info, x, y, width, height, 
			     factor, xg, yg, part)
c_CellStyle	*This;
cell_info_t	*cell_info;
int		x;
int		y;
int		width;
int		height;
scale_t		factor;
int		xg;
int		yg;
int		part;
{
  boolean	left_done, right_done, up_done, down_done;
  
  if (cell_info->clip_type == XcCT_CLIPMASK_FULL)
    return;
  
  left_done = !cell_info->left_to_show;
  right_done = !cell_info->right_to_show;
  up_done = !cell_info->up_to_show;
  down_done = !cell_info->down_to_show;

  switch(part)
    {
    case DISPLAY_LEFT_BORDER:
      DisplayCellStyleBorders(This, cell_info, x, y, width, height,
			      left_done, TRUE, TRUE, TRUE);
      break;
    case DISPLAY_RIGHT_BORDER:
      DisplayCellStyleBorders(This, cell_info, x, y, width, height,
			      TRUE, right_done, TRUE, TRUE);
      break;
    case DISPLAY_UP_BORDER:
      DisplayCellStyleBorders(This, cell_info, x, y, width, height,
			      TRUE, TRUE,  up_done, TRUE);
      break;
    case DISPLAY_DOWN_BORDER:
      DisplayCellStyleBorders(This, cell_info, x, y, width, height,
			      TRUE, TRUE, TRUE, down_done);
      break;
    case DISPLAY_ALL_BORDERS:
      DisplayCellStyleBorders(This, cell_info, x, y, width, height,
			      left_done, right_done, up_done, down_done);
      break;
    case DISPLAY_INSIDE:
      DisplayCellStyleInside(This, cell_info, x, y, width, height, xg, yg);
      break;
    case DISPLAY_TEXT:
      DisplayCellStyleText(This, cell_info, x,y, width, height, factor);
      break;
    case DISPLAY_ALL_INSIDE:
      DisplayCellStyleInside(This, cell_info, x, y, width, height, xg, yg);
      DisplayCellStyleText(This, cell_info, x,y, width, height, factor);
      break;
    case DISPLAY_ALL:
      DisplayCellStyleInside(This, cell_info, x, y, width, height, xg, yg);
      DisplayCellStyleText(This, cell_info, x,y, width, height, factor);
      DisplayCellStyleBorders(This, cell_info, x, y, width, height,
			      left_done, right_done, up_done, down_done);
      break;
    }
}

     
     
/* ----------------------------------------------------------------- ** 
** setCellStyle - Set CellStyle parameters                           ** 
** ----------------------------------------------------------------- */
static void setCellStyle(this, va_alist)
CellStyle_t *this;
va_dcl
{
  va_list ap;

  Xc_HISTORY(("set"));

  va_start(ap);
  vset(this, ap);
  va_end(ap);
/*F(this->hook).callback(this->hook, XcH_CONTENT_CHANGED, this);*/

  Xc_TRACE(("set done"));
}


/* ----------------------------------------------------------------- ** 
** getCellStyle - Get or create a cell style                         ** 
** ----------------------------------------------------------------- */
static CellStyle_t *getCellStyle(this, va_alist)
c_CellStyle *this;
va_dcl
{
  CellStyle_t *cstyle, *ptr;

  va_list ap;

  Xc_HISTORY(("get"));

  if((cstyle = F(this).CreateNewCellStyle(this, CELL_STYLE_NONE)) == NULL)
    return NULL;
  va_start(ap);
  vset(cstyle, ap);
  va_end(ap);

  for(ptr = this->cell_style; ptr != NULL; ptr = ptr->NextCellStyle)
    if(F(this).compare(ptr, cstyle))
      {
	return ptr;
      }
  F(this).AddCellStyleToBase(this, cstyle);
  return cstyle;
}


/* ----------------------------------------------------------------- ** 
** vset - Set cell style parameters                                  ** 
** ----------------------------------------------------------------- */
static void vset(this, ap)
CellStyle_t *this;
va_list ap;
{
  c_TextStyle *style;
  c_FormatNbr *format;
  boolean out_flag;
  c_Color *color;

  Xc_TRACE(("vset(%lx)", (long)this));
  
  for(out_flag = FALSE; out_flag == FALSE; )
    switch(va_arg(ap, cellstyle_set_code_t))
      {
      case XqCS_END:
        Xc_TRACE(("XqCS_END"));

        out_flag = TRUE;
        break;

	/*--- Cell format ---*/
      case XqCS_FORMAT:
	Xc_TRACE(("XqCS_FORMAT"));
	format = va_arg(ap, c_FormatNbr *);
	if(XcH_REPLACE(this->format, format, hook_format, this))
	  this->format = format;
	continue;

	/*--- Cell textsyle ---*/
      case XqCS_TEXTSTYLE:
	Xc_TRACE(("XqCS_TEXTSTYLE"));

	style = va_arg(ap, c_TextStyle *);
	if(XcH_REPLACE(this->text_style, style, hook_textstyle, this))
	{
	  this->text_style = style;
	  this->rfont = (c_RasterFont *)NULL;
	}
	continue;
	
	/*--- Cell background color ---*/
      case XqCS_BACKGROUND:
	Xc_TRACE(("XqCS_BACKGROUND"));

	color = va_arg(ap, c_Color *);
	if(XcH_REPLACE(this->inside_background, color, hook_color, this))
	  this->inside_background = color;
	continue;

	/*--- Cell foreground ---*/
      case XqCS_FOREGROUND:
	Xc_TRACE(("XqCS_FOREGROUND"));

	color = va_arg(ap, c_Color *);
        if(XcH_REPLACE(this->inside_foreground, color, hook_color, this))
	  this->inside_foreground = color;
	continue;

	/*--- Cell pattern ---*/
      case XqCS_PATTERN:
	Xc_TRACE(("XqCS_PATTERN"));

	this->inside_stipple_pattern = va_arg(ap, int);
	continue;

	/*--- Alignement ---*/
      case XqCS_ALIGNEMENT:
	Xc_TRACE(("XqCS_ALIGNEMENT"));

	this->ruler_type = va_arg(ap, ruler_t);
	continue;

	/*--- Left border color ---*/
      case XqCS_LEFT_BACKGROUND:
	Xc_TRACE(("XqCS_LEFT_BACKGROUND"));

	color = va_arg(ap, c_Color *);
        if(XcH_REPLACE(this->left.border_background, color, hook_color, this))
	  this->left.border_background = color;
	continue;

	/*--- Left border foreground color ---*/
      case XqCS_LEFT_FOREGROUND:
	Xc_TRACE(("XqCS_LEFT_FOREGROUND"));

	color = va_arg(ap, c_Color *);
        if(XcH_REPLACE(this->left.border_foreground, color, hook_color, this))
	  this->left.border_foreground = color;
	continue;

	/*--- Left border pattern ---*/
      case XqCS_LEFT_PATTERN:
	Xc_TRACE(("XqCS_LEFT_PATTERN"));

	this->left.border_stipple_pattern = va_arg(ap, int);
	if (this->left.border_stipple_pattern != PATTERN_NONE)
	  {
	    this->left.border_line_thickness = 1;
	    this->left.border_thickness =
	      Cell_Pattern[this->left.border_stipple_pattern].height;
	  }
	else
	  {
	    this->left.border_line_thickness = 0;
	    this->left.border_thickness = 0;
	  }
	continue;

	/*--- Left border dash ---*/
      case XqCS_LEFT_DASH:
	Xc_TRACE(("XqCS_LEFT_DASH"));
	
	this->left.border_dash_pattern = va_arg(ap, int);
	continue;

	/*--- Right border color ---*/
      case XqCS_RIGHT_BACKGROUND:
	Xc_TRACE(("XqCS_RIGHT_BACKGROUND"));

	color = va_arg(ap, c_Color *);
        if(XcH_REPLACE(this->right.border_background, color, hook_color, this))
	  this->right.border_background = color;
	continue;
	
	/*--- Right border foreground color ---*/
      case XqCS_RIGHT_FOREGROUND:
	Xc_TRACE(("XqCS_RIGHT_FOREGROUND"));

	color = va_arg(ap, c_Color *);
        if(XcH_REPLACE(this->right.border_foreground, color, hook_color, this))
	  this->right.border_foreground = color;
	continue;

	/*--- Right border pattern ---*/
      case XqCS_RIGHT_PATTERN:
	Xc_TRACE(("XqCS_RIGHT_PATTERN"));

	this->right.border_stipple_pattern = va_arg(ap, int);
	if (this->right.border_stipple_pattern != PATTERN_NONE)
	  {
	    this->right.border_line_thickness = 1;
	    this->right.border_thickness =
	      Cell_Pattern[this->right.border_stipple_pattern].height;
	  }
	else
	  {
	    this->right.border_line_thickness = 0;
	    this->right.border_thickness = 0;
	  }
	continue;

	/*--- Right border dash ---*/
      case XqCS_RIGHT_DASH:
	Xc_TRACE(("XqCS_RIGHT_DASH"));
	
	this->right.border_dash_pattern = va_arg(ap, int);
	continue;

	/*--- Top border color ---*/
      case XqCS_TOP_BACKGROUND:
	Xc_TRACE(("XqCS_TOP_BACKGROUND"));

	color = va_arg(ap, c_Color *);
        if(XcH_REPLACE(this->up.border_background, color, hook_color, this))
	  this->up.border_background = color;
	continue;

	/*--- Top border foreground color ---*/
      case XqCS_TOP_FOREGROUND:
	Xc_TRACE(("XqCS_TOP_FOREGROUND"));

	color = va_arg(ap, c_Color *);
        if(XcH_REPLACE(this->up.border_foreground, color, hook_color, this))
	  this->up.border_foreground = color;
	continue;

	/*--- Top border pattern ---*/
      case XqCS_TOP_PATTERN:
	Xc_TRACE(("XqCS_TOP_PATTERN"));

	this->up.border_stipple_pattern = va_arg(ap, int);
	if (this->up.border_stipple_pattern != PATTERN_NONE)
	  {
	    this->up.border_line_thickness = 1;
	    this->up.border_thickness =
	      Cell_Pattern[this->up.border_stipple_pattern].height;
	  }
	else
	  {
	    this->up.border_line_thickness = 0;
	    this->up.border_thickness = 0;
	  }
	continue;

	/*--- Top border dash ---*/
      case XqCS_TOP_DASH:
	Xc_TRACE(("XqCS_TOP_DASH"));
	
	this->up.border_dash_pattern = va_arg(ap, int);
	continue;

	/*--- Bottom border color ---*/
      case XqCS_BOTTOM_BACKGROUND:
	Xc_TRACE(("XqCS_BOTTOM_BACKGROUND"));

	color = va_arg(ap, c_Color *);
	if(XcH_REPLACE(this->down.border_background, color, hook_color, this))
	  this->down.border_background = color;
	continue;

	/*--- Bottom border foreground color ---*/
      case XqCS_BOTTOM_FOREGROUND:
	Xc_TRACE(("XqCS_BOTTOM_FOREGROUND"));

	color = va_arg(ap, c_Color *);
        if(XcH_REPLACE(this->down.border_foreground, color, hook_color, this))
	  this->down.border_foreground = color;
	continue;

	/*--- Bottom border pattern ---*/
      case XqCS_BOTTOM_PATTERN:
	Xc_TRACE(("XqCS_BOTTOM_PATTERN"));

	this->down.border_stipple_pattern = va_arg(ap, int);
	if (this->down.border_stipple_pattern != PATTERN_NONE)
	  {
	    this->down.border_line_thickness = 1;
	    this->down.border_thickness =
	      Cell_Pattern[this->down.border_stipple_pattern].height;
	  }
	else
	  {
	    this->down.border_line_thickness = 0;
	    this->down.border_thickness = 0;
	  }
	continue;

	/*--- Bottom border dash ---*/
      case XqCS_BOTTOM_DASH:
	Xc_TRACE(("XqCS_BOTTOM_DASH"));
	
	this->down.border_dash_pattern = va_arg(ap, int);
	continue;

      default:
	Xc_BREAK(("Unknow cell-style-set-code."));
	break;
      }
}


/* ----------------------------------------------------------------- ** 
** compare - Compare two cell styles                                 ** 
** ----------------------------------------------------------------- */
static boolean compare(a, b)
CellStyle_t *a;
CellStyle_t *b;
{
  Xc_TRACE(("compare"));

  if(a == b) return TRUE;
  if((a == NULL) || (b == NULL)) return FALSE;
  if((a->ruler_type == b->ruler_type) &&
     (a->text_style == b->text_style) &&
     (a->format == b->format) &&
     (a->inside_stipple_pattern == b->inside_stipple_pattern) &&
     ((a->inside_stipple_pattern == PATTERN_NONE) ||
      ((a->inside_foreground == b->inside_foreground) &&
       (a->inside_background == b->inside_background))) &&
     /*___Left border___*/
     (a->left.border_stipple_pattern == b->left.border_stipple_pattern) &&
     ((a->left.border_stipple_pattern == PATTERN_NONE) ||
      ((a->left.border_dash_pattern == b->left.border_dash_pattern) &&
       (a->left.border_background == b->left.border_background) &&
       (a->left.border_foreground == b->left.border_foreground))) &&
     /*___Right border___*/
     (a->right.border_stipple_pattern == b->right.border_stipple_pattern) &&
     ((a->right.border_stipple_pattern == PATTERN_NONE) ||
      ((a->right.border_dash_pattern == b->right.border_dash_pattern) &&
       (a->right.border_background == b->right.border_background) &&
       (a->right.border_foreground == b->right.border_foreground))) &&
     /*___Top border___*/
     (a->up.border_stipple_pattern == b->up.border_stipple_pattern) &&
     ((a->up.border_stipple_pattern == PATTERN_NONE) ||
      ((a->up.border_dash_pattern == b->up.border_dash_pattern) &&
       (a->up.border_background == b->up.border_background) &&
       (a->up.border_foreground == b->up.border_foreground))) &&
     /*___Bottom border___*/
     (a->down.border_stipple_pattern == b->down.border_stipple_pattern) &&
     ((a->down.border_stipple_pattern == PATTERN_NONE) ||
      ((a->down.border_dash_pattern == b->down.border_dash_pattern) &&
       (a->down.border_background == b->down.border_background) &&
       (a->down.border_foreground == b->down.border_foreground))))
    return TRUE;

  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** hook_color - Color hook                                           ** 
** ----------------------------------------------------------------- */
static void hook_color(this, reason, old, new)
CellStyle_t *this;
int reason;
c_Color *old;
c_Color *new;
{
  Xc_TRACE(("hook_color reason: %d", reason));

  if(reason == XcH_CONTENT_CHANGED)
  {
    Xc_TRACE(("change color: %s to %s", old->name, new->name));
    this->hook_info->
      hook_callback(this->hook_info->hook_callback_data, 
		    this, XcH_COLOR_CHANGED);
    return;
  }
}


/* ----------------------------------------------------------------- ** 
** hook_textstyle - TextStyle hook                                   ** 
** ----------------------------------------------------------------- */
static void hook_textstyle(this, reason, old, new)
CellStyle_t *this;
int reason;
c_TextStyle *old;
c_TextStyle *new;
{
  Xc_TRACE(("hook_textstyle reason: %d", reason));

  if(reason == XcH_CONTENT_CHANGED || reason == XcH_REDRAW)
  {
    Xc_TRACE(("change text style: %s to %s", old->name, new->name));
    this->rfont = NULL;
    this->hook_info->
      hook_callback(this->hook_info->hook_callback_data, 
		    this, XcH_TEXT_STYLE_CHANGED);
    return;
  }
}


/* ----------------------------------------------------------------- ** 
** hook_format - FormatNbr hook                                      ** 
** ----------------------------------------------------------------- */
static void hook_format(this, reason, old, new)
CellStyle_t *this;
int reason;
c_FormatNbr *old;
c_FormatNbr *new;
{
  Xc_TRACE(("hook_format reason: %d", reason));

  if(reason == XcH_CONTENT_CHANGED)
    {
      Xc_TRACE(("change format: %s to %s", old->name, new->name));
      this->hook_info->
	hook_callback(this->hook_info->hook_callback_data, 
		      this, XcH_FORMAT_NBR_CHANGED);
      return;
    }
}



