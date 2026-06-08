/*
** Page.c for XAllWrite in Page/
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
** Started on  Mon Jul 17 19:58:07 1995 Emmanuel Paris
** Last update Wed Jan 12 10:57:57 2000 Emmanuel Paris
*/


#include "Page.h"
#include "Set_TextStyle.h"
#include "Set_TextRuler.h"
#include "Edit_Functions.h"
#include "FrMg_Callbacks.h"
#include <X11/keysym.h>

void *cons_Page();
void dest_Page();
void *copy_Page();
void init_Page ___PROTO((c_Page *this, Display *display, Window window,
			 scale_t sFactor));

static void resize_Page();
static boolean set_function_Page();
static status_t eventlook_Page();

static void map_window_Page ___PROTO((c_Page *This, boolean map));
static void move_window_Page ___PROTO((c_Page *This, int x, int y));
static void update_visible_part_Page ___PROTO((c_Page *This, int vxo, int vyo,
					       int vxd, int vyd));
static boolean is_active_Page ___PROTO((c_Page *This, Window window));
static void change_scaling_Page ___PROTO((c_Page *This, scale_t scale));
static void redraw_all_Page ___PROTO((c_Page *This));
static void redraw_rectangle_Page ___PROTO((c_Page *This, int x, int y,
					    int width, int height));
static void calcul_clipping_Page ___PROTO((c_Page *This));

extern boolean ps_printPage();
extern boolean readPage ___PROTO((c_DocFile *doc, char *keyword,
				  long *page_number, BaseStd_t *base));
extern boolean writePage ___PROTO((c_Page *this, c_DocFile *doc));

/*--- Private Functions ---*/
static void set_X_info();

sf_Page fc_Page =
{
 cons_Page,
 dest_Page,
 copy_Page,
 init_Page,
 resize_Page,
 set_function_Page,
 eventlook_Page,
 map_window_Page,
 move_window_Page,
 update_visible_part_Page,
 is_active_Page,
 change_scaling_Page,
 redraw_all_Page,
 redraw_rectangle_Page,
 calcul_clipping_Page,
 set_font_style,
 set_font_family,
 set_font_color,
 set_font_bgcolor,
 set_font_size,
 set_page_margin,
 set_ruler_margin,
 set_ruler_tab,
 ps_printPage,
 readPage,
 writePage
};

/* last clipboard action on object type: TRUE= text, FALSE= frame */
static boolean clipb_type = TRUE;

void *cons_Page(BasePage, BaseStd, Calibration, Redraw, select_type)
c_Page		**BasePage;
BaseStd_t	*BaseStd;
c_Calibration	*Calibration;
c_Redraw	*Redraw;
boolean		*select_type;
{
 int page_number = 1;
 c_Page *This, *Page;

 Xc_HISTORY(("Constructor Page..."));

 if((This=(c_Page *)Xc_malloc("Page",sizeof (c_Page)))==NULL) return NULL;
 memset(This, 0, sizeof(c_Page));
 This->f= &fc_Page;
  
 if (*BasePage)
 {
  This->BasePage = (*BasePage)->BasePage;
  Page = *BasePage;
  while(Page->NextPage)
   Page = Page->NextPage;
  This->PrevPage = Page;
  Page->NextPage = This;
  This->NextPage = NULL;
  page_number = Page->page_number + 1;
 }
 else
 {
  *BasePage = This;
  This->BasePage = BasePage;
  This->PrevPage = This->NextPage = NULL;
 }

 This->X_info_calc = FALSE;
 This->BaseStd = BaseStd;
 This->sScale = COORD_DEFAULT;
 This->sFactor = SCALE_FROM_PERCENT(100);
 This->cursor = NULL;
 This->page_number = page_number;
 This->page_format = &Xc_Table_Page_Format[1];
 This->page_type = Xc_PAGE_RIGHT;
 This->page_orient = Xc_PORTRAIT;

 This->left_margin = This->right_margin = This->top_margin = 
  This->bottom_margin = SCALE_FROM_CENTIMETERS(2.5);

 This->PosX = This->PosY = This->Width = This->Height = 0;
 This->mapped = FALSE;

 This->ScanLine = (c_ScanLine *)NEW(c_ScanLine)(This);
 This->Calibration = Calibration;
 This->text_line_start = This->text_line_end = NULL;
 This->Redraw = Redraw;

 This->frmg = 
  (c_FrMg *)NEW(c_FrMg)(This->BaseStd, This, 
			frmg_select_callback,
			frmg_deselect_callback,
			frmg_redraw_polygone_callback, 
			frmg_modify_cadre_callback,
			frmg_attract_point_callback);
 
 This->in_use = FALSE;
 This->select_type = select_type;
 Xc_TRACE(("Constructor Page done"));
 return This;
}

void init_Page(This, display, window, sFactor)
c_Page		*This;
Display		*display;
Window		window;
scale_t		sFactor;
{  
 Window	pgwindow;
 XSetWindowAttributes  windattrib;
 unsigned long black, white;
 int	decx;

 Xc_HISTORY(("Init Page Parameters"));
 
 This->sFactor = sFactor;
 This->sScale = (coord_t)(((double)COORD_DEFAULT *
			   (double)SCALE_FROM_PERCENT(100) )/
			  (double)sFactor);
 
 if (This->page_format != &Xc_Table_Page_Format[0])
 {
  This->sWidth = This->page_format->width;
  This->sHeight = This->page_format->height;
 }
 if (This->page_orient == Xc_LANDSCAPE)
  SWAP(coord_t, This->sWidth, This->sHeight);
 
 This->Width = This->sWidth / This->sScale;
 This->Height = This->sHeight / This->sScale;
 
 black = BlackPixel(display, DefaultScreen(display));
 white = WhitePixel(display, DefaultScreen(display));
 
 if ((This->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_LEFT)
  decx = -5; else decx = 5;
 
 This->shadowWin = 
  XCreateSimpleWindow(display, window, This->PosX + decx, This->PosY + 5,
		      This->Width, This->Height, 1, black, black);
 
 pgwindow = 
  XCreateSimpleWindow(display, window, This->PosX, This->PosY,
		      This->Width, This->Height, 1, black, white);
 
 XFlush(display);

 F(GlobDragAndDrop).select_input
  (GlobDragAndDrop, pgwindow, DD_DROP | DD_MOTION, DD_LONG, eventlook_Page,
   DD_LONG, This, DD_INT, (int)(SCALE_TO_PERCENT(This->sFactor) * 100),
   DD_INT, getpid(), NULL);

 XSelectInput(display, pgwindow, PointerMotionMask | ButtonPressMask |
	      ButtonReleaseMask | ExposureMask | LeaveWindowMask |
	      EnterWindowMask);

 windattrib.bit_gravity = StaticGravity;
 XChangeWindowAttributes(display, window, CWBitGravity, &windattrib);

 if (This->cursor)
  DELETE(c_Cursor)(This->cursor);
 This->cursor = (c_Cursor *)NEW(c_Cursor)(display, pgwindow);
 F(This->cursor).set_cursor(This->cursor, C_EDIT_TEXT);

 XFlush(display);
 
 This->X_info_calc = FALSE;
 set_X_info(This, display, pgwindow);
 change_scaling_Page(This, sFactor);

 XFlush(display);

 Xc_TRACE(("init page done"));
}

void dest_Page(This)
c_Page *This;
{
 Xc_HISTORY(("destructor"));
 
 if (This == *This->BasePage)
  *This->BasePage = This->NextPage;

 if (This->PrevPage)
  This->PrevPage->NextPage = This->NextPage;
 if (This->NextPage)
  This->NextPage->PrevPage = This->PrevPage;
 
 DELETE(c_FrMg)(This->frmg);
 DELETE(c_ScanLine)(This->ScanLine);
 if (This->cursor)
  DELETE(c_Cursor)(This->cursor);
 
 if (This->X_info_calc)
 {
  XFreeGC(_display, _gc_contour);
  XFreeGC(_display, _gc_inside);
  XDestroyWindow(_display, _window);
  XDestroyWindow(_display, This->shadowWin);
 }
 Xc_free(This);
  
 Xc_TRACE(("destroyed"));
}

void *copy_Page(Orig, recto_verso, double_page)
c_Page *Orig;
boolean recto_verso;
boolean double_page;
{
 c_Page *This;

 This = (c_Page *)NEW(c_Page)
  (Orig->BasePage, Orig->BaseStd, Orig->Calibration, Orig->Redraw, 
   Orig->select_type);
 
 This->page_number = Orig->page_number + 1;
 This->page_format = Orig->page_format;
 
 if (recto_verso || double_page)
 {
  if ((Orig->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_LEFT)
   This->page_type = Xc_PAGE_RIGHT;
  else
   This->page_type = Xc_PAGE_LEFT;
  if (double_page)
   This->page_type |= Xc_PAGE_DOUBLE;
 }
 else
  This->page_type = Xc_PAGE_RIGHT;

 This->page_orient = Orig->page_orient;
 
 if (This->page_format == &Xc_Table_Page_Format[0])
 {
  if (This->page_orient == Xc_LANDSCAPE)
  {
   This->sWidth = Orig->sHeight;
   This->sHeight = Orig->sWidth;
  }
  else
  {
   This->sWidth = Orig->sWidth;
   This->sHeight = Orig->sHeight;
  }
 }

 Xc_TRACE(("Object Page copied"));
 return This;
}

static void resize_Page(This, posx, posy, new_width, new_height)
c_Page	*This;
int	posx;
int	posy;
int	new_width;
int	new_height;
{
  
 This->PosX = posx;
 This->PosY = posy;
}

static boolean set_function_Page(This, num_function)
c_Page	*This;
int	num_function;
{
 boolean	ret_code = FALSE;
 boolean	frmg_func = FALSE;
 /* if (This->function_data)
    ABORT(This); */
 
 switch(num_function)
 {
 case SELECT_MOVE_EDIT_TEXT:
  if (This->frmg->select_type)
   num_function = MOVE_POINT_MODE;
  
  This->function_type = num_function;
  This->function = &fc_Select_Move_Edit_Text;
  break;
 case CUT_TEXT:
  SET_WAIT_MODE_STATIC;
  ret_code = TRUE;
  if (*This->select_type)
  {
   clipboard_cut_Page(This); 
   clipb_type = TRUE;
  }
  else
  {
   ret_code = F(This->frmg).set_function(This->frmg, CUT_CADRE);
   clipb_type = FALSE;
  }
  UNSET_WAIT_MODE;
  break;
 case COPY_TEXT:
  if (*This->select_type)
  {
   clipboard_copy_Page(This);
   clipb_type = TRUE;
  }
  else
  {
   ret_code = F(This->frmg).set_function(This->frmg, COPY_CADRE);
   clipb_type = FALSE;
  }
  break;
 case PASTE_TEXT:
  SET_WAIT_MODE_STATIC;
  ret_code = TRUE;
  if (clipb_type)
   clipboard_paste_Page(This);
  else
   ret_code = F(This->frmg).set_function(This->frmg, PASTE_CADRE);
  UNSET_WAIT_MODE;
  break;
 case DELETE_TEXT:
  SET_WAIT_MODE_STATIC;
  ret_code = TRUE;
  if (*This->select_type)
   delete_selection_Page(This);
  else
   ret_code = F(This->frmg).set_function(This->frmg, DELETE_SELECTED_CADRE);
  UNSET_WAIT_MODE;
  break;
 case SELECT_ALL_TEXT:
  if (*This->select_type)
   select_all_Page(This);
  else
   ret_code = F(This->frmg).set_function(This->frmg, SELECT_ALL_CADRE);
  break;
 case SET_BOLD:
  SET_WAIT_MODE_STATIC;
  set_font_bold(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_ITALIC:
  SET_WAIT_MODE_STATIC;
  set_font_italic(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_UNDERLINE:
  SET_WAIT_MODE_STATIC;
  set_font_underline(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_STRIKEOUT:
  SET_WAIT_MODE_STATIC;
  set_font_strikeout(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_SHADOW:
  SET_WAIT_MODE_STATIC;
  set_font_shadow(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_SUBSCRIPT:
  SET_WAIT_MODE_STATIC;
  set_font_subscript(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_SUPSCRIPT:
  SET_WAIT_MODE_STATIC;
  set_font_supscript(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_OUTLINE:
  SET_WAIT_MODE_STATIC;
  set_font_outline(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_SMALL_CAPS:
  SET_WAIT_MODE_STATIC;
  set_font_small_caps(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_BIG_CAPS:
  SET_WAIT_MODE_STATIC;
  set_font_big_caps(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DEC_FONT_SIZE:
  SET_WAIT_MODE_STATIC;
  set_font_dec_size(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case INC_FONT_SIZE:
  SET_WAIT_MODE_STATIC;
  set_font_inc_size(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

 case SET_JLEFT:
  SET_WAIT_MODE_STATIC;
  set_ruler_left(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_JCENTER:
  SET_WAIT_MODE_STATIC;
  set_ruler_center(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_JRIGHT:
  SET_WAIT_MODE_STATIC;
  set_ruler_right(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_JUSTIFY:
  SET_WAIT_MODE_STATIC;
  set_ruler_justify(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

 case INC_INDENT:
  SET_WAIT_MODE_STATIC;
  set_ruler_inc_indent(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DEC_INDENT:
  SET_WAIT_MODE_STATIC;
  set_ruler_dec_indent(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case INC_INTERLINE:
  SET_WAIT_MODE_STATIC;
  set_ruler_inc_interline(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DEC_INTERLINE:
  SET_WAIT_MODE_STATIC;
  set_ruler_dec_interline(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case INC_INTERPARAG:
  SET_WAIT_MODE_STATIC;
  set_ruler_inc_interparag(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DEC_INTERPARAG:
  SET_WAIT_MODE_STATIC;
  set_ruler_dec_interparag(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

 case INSERT_PAGE_BREAK:
  SET_WAIT_MODE_STATIC;
  insert_page_break_Page(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case INSERT_EURO:
  SET_WAIT_MODE_STATIC;
  insert_euro_Page(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case INSERT_SPEC_CHAR:
  SET_WAIT_MODE_STATIC;
  insert_spec_char_Page(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;

 case DISPLAY_FRAME_BORDER:
  SET_WAIT_MODE_STATIC;
  display_frame_border(This);
  UNSET_WAIT_MODE;
  break;

 default:
  if (num_function > FRMG_FUNCTION_BEGIN &&
      num_function < FRMG_FUNCTION_END)
  {
   int old_func;
   
   old_func = This->frmg->function_type;
   ret_code = F(This->frmg).set_function(This->frmg, num_function);
   frmg_func = TRUE;
  
   if (num_function == MOVE_POINT_MODE ||
       num_function == RESIZE_CADRE_MODE)
   {
    This->function_type = num_function;
    This->function = &fc_Select_Move_Edit_Text;
   }
   else if (old_func != This->frmg->function_type)
   {
    This->function = &fc_Dispatch_To_FrMg;
    This->function_type = num_function;
   }
  }
  break;
 }

 if (!frmg_func)
  This->frmg->function_type = 0;
 /* INIT(This); */
 
 return ret_code;
}

static status_t eventlook_Page(This, event)
c_Page	*This;
XEvent	*event;
{
 status_t	status;
 short		xm, ym;

 status = FUNCTION_NOT_FINISHED;
 if (event->xany.window == This->X_info.window)
 {
  switch(event->type)
  {
  case Expose:
  case GraphicsExpose:
   F(This).RedrawRectangle(This, event->xexpose.x, event->xexpose.y,
			   event->xexpose.width, event->xexpose.height);
   break;
  case ClientMessage:
   switch(DROP_FLAGS(*event) & 0xf)
   {
   case DD_DROP_FOR_SOURCE:
    if (DROP_ID(*event) == getpid())
    {
     if ((DROP_FLAGS(*event) & 0xf0) == DD_DROP_FOR_MOVE)
     {
      Xc_TRACE(("Source: Drop cadre for move"));
      move_selected_cadre_drag(This, DROP_DATA(*event));
     }
     else
     {
      Xc_TRACE(("Source: Drop cadre for copy"));
      copy_selected_cadre_drag(This, DROP_DATA(*event));
     }
     ABORT(This);
     INIT(This);
    }
    else
    {
     Xc_TRACE(("Hum... Drop to another cession not allowed"));
     ABORT(This);
     INIT(This);
    }
    break;
   case DD_DROP_FOR_TARGET:
    xm = (long)DROP_VALUE(*event) >> 16;
    ym = (long)DROP_VALUE(*event) & 0xffff;
    Xc_TRACE(("xm: %d  ym: %d", xm, ym));
    if (DROP_ID(*event) == getpid())
    {
     if ((DROP_FLAGS(*event) & 0xf0) == DD_DROP_FOR_MOVE)
     {
      Xc_TRACE(("Target: Drop cadre for move"));
      /*ABORT(This);*/
      drag_cadre_move_done(This->frmg, DROP_DATA(*event), xm, ym);
      status = FUNCTION_MODIFIED;
      /*INIT(This);*/
     }
     else
     {
      Xc_TRACE(("Target: Drop cadre for copy"));
      /*ABORT(This);*/
      drag_cadre_copy_done(This->frmg, DROP_DATA(*event), xm, ym);
      status = FUNCTION_MODIFIED;
      /*INIT(This);*/
     }
    }
    else
     Xc_TRACE(("Hum... Drop from another cession not allowed"));
    break;
   case DD_DROP_ABORTED_BY_USER:
   case DD_DROP_ON_WRONG_WINDOW:
    Xc_TRACE(("drop aborted"));
    ABORT(This);
    INIT(This);
    break;
   }
   break;  
  case LeaveNotify:
   break;
  case EnterNotify:
   break;
  default:
   break;
  }
 }
 return status;
}

static void set_X_info(This, display, window)
c_Page		*This;
Display		*display;
Window		window;
{
 GC	gc;
 XGCValues	val_gc;
  
 if (!This->X_info_calc)
 {

  This->X_info.display = display;
  This->X_info.window = window;
  This->X_info.ecran = DefaultScreen(display);

  if (_gc_contour)
   XFreeGC(_display, _gc_contour);
  if (_gc_inside)
   XFreeGC(_display, _gc_inside);
   
  if (GlobColormap->Cblack)
   val_gc.function = GXxor;
  else
   val_gc.function = GXinvert;
  val_gc.plane_mask= -1;
  val_gc.foreground = This->X_info.contour_color = GlobColormap->Cblack;
  val_gc.background = GlobColormap->Cwhite;
  val_gc.line_width = 0;
  val_gc.line_style = LineSolid; 
  val_gc.cap_style = CapNotLast;
  val_gc.join_style = JoinMiter;
  gc = XCreateGC(display, window, GCFunction | GCPlaneMask | GCForeground |
		 GCBackground | GCLineWidth | GCLineStyle | GCCapStyle |
		 GCJoinStyle, &val_gc);
  This->X_info.gc_contour = gc;
    
  val_gc.function = GXcopy;
  val_gc.line_width = 0;
  val_gc.cap_style = CapButt;
  val_gc.foreground = This->X_info.inside_color = GlobColormap->Cwhite;
  val_gc.background = GlobColormap->Cblack;
  val_gc.graphics_exposures = True;
  gc = XCreateGC(display, window, GCFunction | GCForeground | GCBackground |
		 GCLineWidth | GCLineStyle | GCCapStyle | GCJoinStyle |
		 GCGraphicsExposures, &val_gc);
  This->X_info.gc_inside = gc;
    
  This->frmg->X_info_calc = FALSE;
  F(This->frmg).set_X_info(This->frmg, display, window, This->cursor);
    
  This->X_info_calc = TRUE;
 }
}

static void map_window_Page(This, map)
c_Page	*This;
boolean	map;
{
#ifndef NTRACE
 if (map)
  Xc_TRACE(("Map Page %d", This->page_number));
 else
  Xc_TRACE(("Unmap Page %d", This->page_number));
#endif
  
 if (This->mapped == map) return;

 if (map)
 {
  XMapWindow(_display, This->shadowWin);
  XMapWindow(_display, _window);
 }
 else
 {
  if (!This->in_use)
  {
   XUnmapWindow(_display, This->shadowWin);
   XUnmapWindow(_display, _window);
  }
 }
 This->mapped = map;
}

static void move_window_Page(This, x, y)
c_Page	*This;
int x, y;
{
 if (This->PosX == x && This->PosY == y) return;

 This->PosX = x; This->PosY = y;
 
 if ((This->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_LEFT)
  XMoveWindow(_display, This->shadowWin, x - 5, y + 5);
 else
  XMoveWindow(_display, This->shadowWin, x + 5, y + 5);
 XMoveWindow(_display, _window, x, y);
}

static void update_visible_part_Page(This, vxo, vyo, vxd, vyd)
c_Page *This;
int	vxo;
int	vyo;
int	vxd;
int	vyd;
{
 coord_t scale = This->sScale;

 This->vx = (vxo - This->PosX) * scale;
 This->vy = (vyo - This->PosY) * scale;
 This->vwidth = (vxd - vxo) * scale;
 This->vheight= (vyd - vyo) * scale;
 
 F(This).CalculClipping(This);
}

static boolean is_active_Page(This, window)
c_Page *This;
Window window;
{
 if (_window == window || This->shadowWin == window)
  return TRUE;
 return FALSE;
}

static void change_scaling_Page(This, scale)
c_Page *This;
scale_t scale;
{
 This->sFactor = scale;
 This->sScale = (coord_t)(((double)COORD_DEFAULT *
			   (double)SCALE_FROM_PERCENT(100) )/
			  (double)scale);
 Xc_TRACE(("coucou %g", scale));

 F(GlobDragAndDrop).change_argument(GlobDragAndDrop, This->X_info.window, 2,
				    DD_INT,
				    (int)(SCALE_TO_PERCENT(scale) * 100));
 Xc_TRACE(("coucou2 %g", scale));
 
 This->Width = This->sWidth / This->sScale;
 This->Height = This->sHeight / This->sScale;
 
 map_window_Page(This, FALSE);

 F(This->frmg).change_frmg_scaling(This->frmg, scale, 0, 0);

 XResizeWindow(_display, This->shadowWin, This->Width, This->Height);
 XResizeWindow(_display, _window, This->Width, This->Height);
}

static void redraw_all_Page(This)
c_Page *This;
{
 redraw_rectangle_Page(This, 0, 0, This->Width, This->Height);
}

static void redraw_rectangle_Page(This, x, y, width, height)
c_Page *This;
int x;
int y;
int width;
int height;
{
 if (This->mapped)
 {
  F(This->frmg).start_draw_cache(This->frmg);
  
  XClearArea(_display, _window, x, y, width, height, False);
  F(This->Redraw).redraw_rectangle(This->Redraw, This, x, y, width, height,
				   *This->select_type);

  F(This->frmg).redraw_rectangle(This->frmg, x, y, x + width, y + height); 

  F(This->frmg).flush_draw_cache(This->frmg);
  
  if (!*This->select_type)
   F(This->frmg).redraw_rectangle_handle(This->frmg, x, y, width, height);
 }
}

static void calcul_clipping_Page(This)
c_Page *This;
{
 Polygone *clip_polygone;
 Polygone *over_polygone;
 Polygone *under_polygone;
 Pixmap   clip_mask;
 int	  clip_x, clip_y;
 
 F(This->frmg).get_clip_polygone
  (This->frmg, This->vx / This->sScale, This->vy / This->sScale, 
   This->vwidth / This->sScale, This->vheight / This->sScale,
   &clip_polygone, &over_polygone, &under_polygone);
 
 if (clip_polygone)
 {
  clip_mask = CalculPolygoneMask(clip_polygone, This->X_info);
  clip_x = clip_polygone->Orig_X;
  clip_y = clip_polygone->Orig_Y;
  FreePolygone(clip_polygone, DEL_POLYGONE);
 }
 else
 {
  clip_mask = None;
  clip_x = clip_y = 0;
 }
 
 F(This->Redraw).set_clipping(This->Redraw, clip_mask, clip_x, clip_y,
			      over_polygone, under_polygone);
}
