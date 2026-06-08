/*
** Feuille.c for XQuad in Feuille/
** Methods for the Feuile class
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
** Started on  Wed Jul 19 15:33:47 1995 Emmanuel Paris
** Last update Mon Jan 25 23:07:40 1999 Emmanuel Paris
*/

#define _PRIVATE_FEUILLE_
#include "FeuilleP.h"

static void *cons_Feuille();
static void dest_Feuille();
static void *copy_Feuille();
static void init_Feuille();
static void resize_Feuille();
static status_t eventlook_Feuille();
static boolean set_function();
static boolean is_selected();
static boolean is_selected_def();
static boolean is_visible();
static boolean is_in();
static void select_region();
static void deselect_region();
static boolean get_cell_by_xy();
static boolean get_column_by_x();
static boolean get_row_by_y();
static boolean get_column_border_by_x();
static boolean get_row_border_by_y();
static void get_cell_geometry();
static cell *get_active_cellule();
static void set_active_cellule();
static void set_active_cellule_relative();
static void set_active_cellule_advance_selection();
static void refresh_cellule();
static void refresh_region();

static void set_X_info();
void redraw_rectangle();
void change_feuille_origin();
void change_feuille_scaling();
void calcul_column_width();
void calcul_row_height();
static void calcul_cell_overrun_info();
void allocate_cell_info();
void calcul_frame_info();
void calcul_cell_info();
void set_region_selected();
void unset_region_selected();
static void set_cell_selected();
static void unset_cell_selected();
int  get_column_pos();
int  get_row_pos();
int  get_column_width();
int  get_row_height();
void add_region_in_list();
void remove_region_from_list();
void focus_in_region();
static void focus_in_region_def();
void focus_out_region();
static void focus_out_region_def();

static void frmg_select_callback();
static void frmg_deselect_callback();
static void frmg_redraw_polygone_callback();
static void frmg_modify_cadre_callback();
static void frmg_attract_point_callback();

static void hook_cellstyle_callback ___PROTO((c_Feuille *This, 
					      CellStyle_t *cstyle,
					      int reason));
boolean printFeuille ___PROTO((c_Feuille *This, c_PostScript *post));
void CalculPageWindow ___PROTO((c_Feuille *This, boolean redraw));

sf_Feuille fc_Feuille =
{
 cons_Feuille,
 dest_Feuille,
 copy_Feuille,
 init_Feuille,
 resize_Feuille,
 eventlook_Feuille,
 set_function,
 is_selected,
 is_visible,
 select_region,
 deselect_region,
 get_cell_by_xy,
 get_column_by_x,
 get_row_by_y,
 get_column_border_by_x,
 get_row_border_by_y,
 get_cell_geometry,
 get_active_cellule,
 set_active_cellule,
 set_active_cellule_relative,
 set_active_cellule_advance_selection,
 refresh_cellule,
 refresh_region,
 CalculPageWindow,
 feuilleGetBoundingRegion,
 feuilleGetPagesCount,
 feuilleComputeScaling,
 set_cell_inside,
 set_cell_backcolor,
 set_cell_borders,
 set_cell_formatnbr,
 set_cell_textstyle,
 set_column_and_row_size,
 set_cell_fontfamily,
 set_cell_fontcolor,
 set_cell_fontsize,
 resize_column_idealy,
 resize_row_idealy,
 hook_cellstyle_callback,
 printFeuille,
 readFeuille,
 writeFeuille
};


/*--- Default print setup ---*/
static setup_print_t print_setup_init = 
{
 FALSE, TRUE, TRUE,
 SCALE_FROM_CENTIMETERS(2), 
 SCALE_FROM_CENTIMETERS(2), 
 SCALE_FROM_CENTIMETERS(2.5), 
 SCALE_FROM_CENTIMETERS(2.5),
 &Xc_Table_Page_Format[Xc_DEFAULT_FORMAT], COORD_ZERO, COORD_ZERO,
 TRUE, TRUE, FALSE,
 FALSE, SCALE_ONE, 1, 1
};


/* ----------------------------------------------------------------- ** 
** cons_Feuille - Constructor                                        ** 
** ----------------------------------------------------------------- */
static void *cons_Feuille(BaseStd, modified)
BaseStd_t	*BaseStd;
boolean		*modified;
{
 c_Feuille *ObjTmp;
  
 if((ObjTmp=(c_Feuille *)Xc_malloc("Feuille",sizeof (c_Feuille))) == NULL)
  return NULL;
  
 ObjTmp->f		=  &fc_Feuille;
 ObjTmp->X_info_calc	= FALSE;
 ObjTmp->cursor	= NULL;
 ObjTmp->cursor_O	= NULL;
 ObjTmp->cursor_C	= NULL;
 ObjTmp->cursor_R	= NULL;
 ObjTmp->BaseStd	= BaseStd;
 ObjTmp->orig_style	= NULL;
 ObjTmp->orig_color	= NULL;
 ObjTmp->orig_font	= NULL;
 ObjTmp->orig_metrics	= NULL;
 ObjTmp->moteur		= NULL;
 ObjTmp->sScale		= COORD_DEFAULT;
 ObjTmp->sFactor	= SCALE_FROM_PERCENT(100);
 ObjTmp->sequence	= 0;
 ObjTmp->ScrollBarV	= NULL;
 ObjTmp->ScrollBarH	= NULL;
 ObjTmp->function	= NULL;
 ObjTmp->column_orig	= 0;
 ObjTmp->row_orig	= 0;
 ObjTmp->nb_column	= 0;
 ObjTmp->column_width	= NULL;
 ObjTmp->nb_row		= 0;
 ObjTmp->row_height	= NULL;
 ObjTmp->origin_width	= 0;
 ObjTmp->origin_height	= 0;
 ObjTmp->sheet_width	= 0;
 ObjTmp->sheet_height	= 0;
 ObjTmp->nb_column_ci	= 0;
 ObjTmp->nb_row_ci	= 0;
 ObjTmp->cell_info	= NULL;
 ObjTmp->column_info	= NULL;
 ObjTmp->row_info	= NULL;
 ObjTmp->origin_info	= 0;
 ObjTmp->grid		= TRUE;
 ObjTmp->Column_Origin_actif	= TRUE;
 ObjTmp->Row_Origin_actif	= TRUE;
 ObjTmp->select_type	= TRUE;
 ObjTmp->cell_x		= 0;
 ObjTmp->cell_y		= 0;
 ObjTmp->nb_cell_region = 0;
 ObjTmp->cell_region	= NULL;
 ObjTmp->nb_col_region	= 0;
 ObjTmp->col_region	= NULL;
 ObjTmp->nb_row_region	= 0;
 ObjTmp->row_region	= NULL;
 ObjTmp->last_advance 	= NULL;
 ObjTmp->origin_selected = FALSE;
 ObjTmp->function_type	= 0;
 ObjTmp->function_data	= NULL;

 ObjTmp->modified	= modified;

 /*--- Print setup ---*/
 ObjTmp->print_setup = Xc_malloc("Print setup", sizeof(setup_print_t));
 if(ObjTmp->print_setup == NULL)
 {
  Xc_free(ObjTmp);
  return NULL;
 }
 print_setup_init.width = print_setup_init.page_format->width;
 print_setup_init.height = print_setup_init.page_format->height;
 memcpy(ObjTmp->print_setup, &print_setup_init, sizeof(setup_print_t));


 ObjTmp->page_mark = FALSE;
  
 ObjTmp->frmg = 
  (c_FrMg *)NEW(c_FrMg)(ObjTmp->BaseStd, ObjTmp, 
			frmg_select_callback,
			frmg_deselect_callback,
			frmg_redraw_polygone_callback, 
			frmg_modify_cadre_callback,
			frmg_attract_point_callback);
  
 Xc_TRACE(("Object Feuille build"));
 return ObjTmp;
}

static void dest_Feuille(This)
c_Feuille *This;
{
 int	i;
  
 Xc_TRACE(("Object Feuille destroyed"));
 if (This->function)
  ABORT(This);
  
 F(This).DeselectRegion(This, Xq_ORIGIN, TRUE);
 DELETE(c_FrMg)(This->frmg);
  
 if (This->X_info_calc)
 {
  XFreeGC(This->X_info.display, This->X_info.gc_contour);
  XFreeGC(This->X_info.display, This->X_info.gc_inside);
  XFreeGC(This->X_info.display, This->X_info_O.gc_contour);
  XFreeGC(This->X_info.display, This->X_info_O.gc_inside);
  XFreeGC(This->X_info.display, This->X_info_C.gc_contour);
  XFreeGC(This->X_info.display, This->X_info_C.gc_inside);
  XFreeGC(This->X_info.display, This->X_info_R.gc_contour);
  XFreeGC(This->X_info.display, This->X_info_R.gc_inside);
  XFreeGC(This->X_info.display, This->gc_grid1);
  XFreeGC(This->X_info.display, This->gc_grid2);
  XFreeGC(This->X_info.display, This->gc_page_mark);
  XFreeGC(This->X_info.display, This->gc_select);
  XFreeGC(This->X_info.display, This->gc_font_paint);
  if (This->gc_font_mask != (GC)0)
   XFreeGC(This->X_info.display, This->gc_font_mask);
  XDestroyWindow(This->X_info.display, This->X_info.window);
  XDestroyWindow(This->X_info.display, This->X_info_O.window);
  XDestroyWindow(This->X_info.display, This->X_info_C.window);
  XDestroyWindow(This->X_info.display, This->X_info_R.window);
 }

 if (This->orig_style)
  DELETE(c_TextStyle)(This->orig_style);
  
 if (This->cursor)
  DELETE(c_Cursor)(This->cursor);
 if (This->cursor_O)
  DELETE(c_Cursor)(This->cursor_O);
 if (This->cursor_C)
  DELETE(c_Cursor)(This->cursor_C);
 if (This->cursor_R)
  DELETE(c_Cursor)(This->cursor_R);
 if (This->column_width)
  Xc_free(This->column_width);
 if (This->column_page_mark)
  Xc_free(This->column_page_mark);
 if (This->row_height)
  Xc_free(This->row_height);
 if (This->row_page_mark)
  Xc_free(This->row_page_mark);
  
 if (This->cell_info)
 {
  for(i=0; i<This->nb_column_ci; i++)
   Xc_free(This->cell_info[i]);
  Xc_free(This->cell_info);
 }
 if (This->column_info) Xc_free(This->column_info);
 if (This->row_info) Xc_free(This->row_info);
 Xc_free(This->print_setup);
 Xc_free(This);
}

static void *copy_Feuille(ObjectToCopy)
c_Feuille *ObjectToCopy;
{
 c_Feuille *ObjTmp;

 if ((ObjTmp=(c_Feuille *)Xc_malloc("Feuille",sizeof (c_Feuille)))==NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp,ObjectToCopy,sizeof (c_Feuille));
 Xc_TRACE(("Object Feuille copied"));
 return ObjTmp;
}

static void init_Feuille(This, display, mere, visible, posx, posy,
			 sizex, sizey, moteur, scrollbarv, scrollbarh, pager)
c_Feuille	*This;
Display		*display;
Window		mere;
boolean		visible;
int		posx;
int		posy;
int		sizex;
int		sizey;
c_Moteur	*moteur;
c_ScrollBarX	*scrollbarv;
c_ScrollBarX	*scrollbarh;
void		*pager;
{
 scale_t  sFactor;
 Window window;
 Window window_O;
 Window window_C;
 Window window_R;
 color_cells_t	tmpcolorc;
 c_VectorFont	*vfont;
 XSetWindowAttributes	windattrib;
  
 Xc_HISTORY(("Init Feuille Parameters"));
 This->moteur = moteur;
 This->ScrollBarV = scrollbarv;
 This->ScrollBarH = scrollbarh;
 This->Pager = pager;
  
 This->PosX = posx;
 This->PosY = posx;
 This->Width = sizex;
 This->Height= sizey;

 F(GlobColormap).get_closest_color(GlobColormap,&tmpcolorc,COL_RELIEF_LIGHT);
 This->col_relief_light=tmpcolorc.pixel;
 F(GlobColormap).get_closest_color(GlobColormap,&tmpcolorc,COL_RELIEF_DARK);
 This->col_relief_dark=tmpcolorc.pixel;
 F(GlobColormap).get_closest_color(GlobColormap,&tmpcolorc,COL_BACK_LIGHT);
 This->col_back_light=tmpcolorc.pixel;
 F(GlobColormap).get_closest_color(GlobColormap,&tmpcolorc,COL_BACK_DARK);
 This->col_back_dark=tmpcolorc.pixel;
  
 window = XCreateSimpleWindow(display, mere, posx, posy, sizex, sizey, 0,
			      GlobColormap->Cblack,
			      F(GlobColormap).get_spec_color(GlobColormap,0));
 F(GlobDragAndDrop).select_input
  (GlobDragAndDrop, window, 0, DD_LONG,eventlook_Feuille,
   DD_LONG,This, DD_INT,(int)(SCALE_TO_PERCENT(This->sFactor)*100),
   DD_INT, getpid(), NULL);
 This->BaseStd->cell_style_base->window = window;
 window_O = XCreateSimpleWindow(display, mere, posx, posy, 1, 1, 0,
				GlobColormap->Cblack, This->col_back_light);
 window_C = XCreateSimpleWindow(display, mere, posx, posy, 1, 1, 0,
				GlobColormap->Cblack, This->col_back_light);
 window_R = XCreateSimpleWindow(display, mere, posx, posy, 1, 1, 0,
				GlobColormap->Cblack, This->col_back_light);
 
 This->orig_color = 
  F(This->BaseStd->color_base).getColor(&(This->BaseStd->color_base),
					XcC_GRAY, XcC_SCALE_COLOR(0.0),
					XcC_TRANSPARENCY, 0,
					XcC_END);
 vfont = F(GlobFontBase).getFontByName(GlobFontBase, "Helvetica");
 if(!vfont) vfont = GlobFontBase;
 This->orig_style = NEW(c_TextStyle)(&This->orig_style, 
				     "ts Orig", vfont,
				     SCALE_FROM_POINTS(12),
				     This->orig_color);
  
 This->cursor = (c_Cursor *)NEW(c_Cursor)(display, window);
 This->cursor_O = (c_Cursor *)NEW(c_Cursor)(display, window_O);
 This->cursor_C = (c_Cursor *)NEW(c_Cursor)(display, window_C);
 This->cursor_R = (c_Cursor *)NEW(c_Cursor)(display, window_R);
  
 F(This->cursor).set_cursor(This->cursor, C_SELECT_CELLS);
 F(This->cursor_O).set_cursor(This->cursor_O, C_SELECT_ALL);
 F(This->cursor_C).set_cursor(This->cursor_C, C_SELECT_COL);
 F(This->cursor_R).set_cursor(This->cursor_R, C_SELECT_ROW);
  
 set_X_info(This, display, window, window_O, window_C, window_R);

 F(This).CalculPageWindow(This, FALSE);

 if (!This->nb_cell_region)
 {
  if (This->select_type)
   F(This).SelectRegion(This, This->cell_x, This->cell_y, 
			This->cell_x, This->cell_y, Xq_CELL, FALSE);
  else	
  {				/* if while loading some frames are selected */
   This->select_type = TRUE;
   F(This).SelectRegion(This, This->cell_x, This->cell_y, 
			This->cell_x, This->cell_y, Xq_CELL, FALSE);
      
   This->select_type = FALSE;
  }
 }
 sFactor = This->sFactor; This->sFactor = 0;
 change_feuille_scaling(This, sFactor, This->column_orig, This->row_orig);
 F(GlobEditBar).query_aff(GlobEditBar, This->cell_x, This->cell_y);
 F(GlobEditBar).refresh_cell(GlobEditBar, This);
  
 /*  F(This).SetActiveCellule(This, 0, 0, TRUE); */
  
 XSelectInput(display, window, PointerMotionMask|ButtonPressMask
	      |ButtonReleaseMask|ExposureMask|LeaveWindowMask
	      |EnterWindowMask|KeyPressMask|KeyReleaseMask);
 windattrib.bit_gravity = StaticGravity;
 XChangeWindowAttributes(display, window, CWBitGravity, &windattrib);
  
 XSelectInput(display, window_O, PointerMotionMask|ButtonPressMask
	      |ButtonReleaseMask|ExposureMask|LeaveWindowMask
	      |EnterWindowMask|KeyPressMask|KeyReleaseMask);
 windattrib.bit_gravity=StaticGravity;
 XChangeWindowAttributes(display, window_O, CWBitGravity, &windattrib);
  
 XSelectInput(display, window_C, PointerMotionMask|ButtonPressMask
	      |ButtonReleaseMask|ExposureMask|LeaveWindowMask
	      |EnterWindowMask|KeyPressMask|KeyReleaseMask);
 windattrib.bit_gravity=StaticGravity;
 XChangeWindowAttributes(display, window_C, CWBitGravity, &windattrib);
  
 XSelectInput(display, window_R, PointerMotionMask|ButtonPressMask
	      |ButtonReleaseMask|ExposureMask|LeaveWindowMask
	      |EnterWindowMask|KeyPressMask|KeyReleaseMask);
 windattrib.bit_gravity=StaticGravity;
 XChangeWindowAttributes(display, window_R, CWBitGravity, &windattrib);
  
 if (visible)
 {
  XMapWindow(display, window);
  if (This->Column_Origin_actif)
  {
   XMapWindow(display, window_C);
   if (This->Row_Origin_actif)
   {
    XMapWindow(display, window_O);
    XMapWindow(display, window_R);
   }
  }
  else
  {
   if (This->Row_Origin_actif)
    XMapWindow(display, window_R);
  }
 }    
  
 F(This->ScrollBarV).Set_Callback(This->ScrollBarV,change_feuille_origin,
				  This);
 F(This->ScrollBarH).Set_Callback(This->ScrollBarH,change_feuille_origin,
				  This);
 F(This->ScrollBarH).AMove(This->ScrollBarH, This->column_orig);
 F(This->ScrollBarV).AMove(This->ScrollBarV, This->row_orig);
  
 set_function(This, SELECT_MOVE_RESIZE_CELL);
 Xc_TRACE(("init feuille done"));
}

static void resize_Feuille(This, posx, posy, new_width, new_height)
c_Feuille	*This;
int		posx;
int		posy;
int		new_width;
int		new_height;
{ 
 This->PosX=posx;
 This->PosY=posy;
 This->Width= new_width;
 This->Height=new_height;
 This->sheet_width  = new_width  - This->origin_width;
 This->sheet_height = new_height - This->origin_height;
  
 calcul_column_width(This);
 calcul_row_height(This);
 calcul_cell_info(This);
  
 F(This->frmg).resize_frmg(This->frmg);
  
 if (This->Column_Origin_actif)
  XResizeWindow(_display, _C_window, This->sheet_width, This->origin_height);
 if (This->Row_Origin_actif)
  XResizeWindow(_display, _R_window, This->origin_width, This->sheet_height);
 XResizeWindow(_display, _window, This->sheet_width, This->sheet_height);
}

static status_t eventlook_Feuille(This, event)
c_Feuille	*This;
XEvent		*event;
{
 int		type;
 short		xm,ym;
 status_t	status;
 boolean	edit_mode;
 KeySym        symb;
 XEvent	none;
  
 status = FUNCTION_NOT_FINISHED;
 if (event->xany.window == _window || event->xany.window == _O_window ||
     event->xany.window == _C_window || event->xany.window == _R_window ||
     event->type == KeyPress || event->type == KeyRelease)
 {
  if (event->xany.window == _window) 
   type = Xq_CELL;
  else
  {
   if (event->xany.window == _O_window)
    type = Xq_ORIGIN;
   else
   {
    if (event->xany.window == _C_window)
     type = Xq_COLUMN;
    else
     type = Xq_ROW;
   }
  }
  switch(event->type)
  {
  case Expose:
  case GraphicsExpose:
   /*	Xc_TRACE(("%s rect x:%d y:%d <-> x:%d y:%d",
	(event->type==Expose?"Expose":"Gfx Expose"),		
	event->xexpose.x,event->xexpose.y,
	event->xexpose.x+event->xexpose.width,
	event->xexpose.y+event->xexpose.height)); */
   redraw_rectangle(This, type,
		    event->xexpose.x, event->xexpose.y,
		    event->xexpose.x + event->xexpose.width,
		    event->xexpose.y + event->xexpose.height);
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
      ABORT(This);
      drag_cadre_move_done(This->frmg, DROP_DATA(*event), xm, ym);
      status = FUNCTION_MODIFIED;
      INIT(This);
     }
     else
     {
      Xc_TRACE(("Target: Drop cadre for copy"));
      ABORT(This);
      drag_cadre_copy_done(This->frmg, DROP_DATA(*event), xm, ym);
      status = FUNCTION_MODIFIED;
      INIT(This);
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
  case KeyPress:
   if (XEventsQueued(_display, QueuedAfterReading))
   {
    if (XCheckTypedEvent(_display, KeyPress, &none))
     return status;
   }
   symb = XKeycodeToKeysym(event->xany.display,
			   event->xkey.keycode, 0);
   edit_mode = GlobEditBar->edit_mode;
	
   if (This->select_type)
   {
    switch (symb)
    {
    case XK_KP_Enter:
     F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar);
     GlobEditBar->edit_mode = edit_mode;
     event->xany.window = (Window)0;
     break;
    case XK_Home:
    case XK_End:
    case XK_Right:
    case XK_Left:
     if (edit_mode)
      break;
    case XK_Return:
    case XK_Up:
    case XK_Down:
    case XK_Prior:
    case XK_Next:
     F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar);
    case XK_Control_L:
    case XK_Control_R:
    case XK_Shift_L:
    case XK_Shift_R:
     event->xany.serial = Xq_CELL;
     status = EVENTLOOK(This, *event);  
     event->xany.window = (Window)0;
     break;
    default:
     break;
    }
   }
   else
   {
    switch (symb)
    {
    case XK_Control_L:
    case XK_Control_R:
    case XK_Shift_L:
    case XK_Shift_R:
     event->xany.serial = Xq_CELL;
     status = EVENTLOOK(This, *event);  
     break;
    default:
     status = F(This->frmg).eventlook(This->frmg, event);
     break;
    }
   }
   break;
  case NoExpose:
   break;
  default:
   event->xany.serial = type;
   status = EVENTLOOK(This, *event);  
   break;
  }
 }
 return status;
}

static boolean set_function(This,num_function)
c_Feuille	*This;
int		num_function;
{
 int	old_func;
 boolean	ret_code = FALSE;
  
 if (This->function_data)
 {
  ABORT(This);
 }
 switch(num_function)
 {
 case SELECT_MOVE_RESIZE_CELL:
  This->function = &fc_Select_Move_Resize_Cell;
  This->function_type=num_function;
  break;
 case UNSET_ALL_BORDER:
  SET_WAIT_MODE_STATIC;
  unset_all_border(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_BORDER_LEFT:
  SET_WAIT_MODE_STATIC;
  set_border_left(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_BORDER_DOWN:
  SET_WAIT_MODE_STATIC;
  set_border_down(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_BORDER_RIGHT:
  SET_WAIT_MODE_STATIC;
  set_border_right(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_BORDER_UP:
  SET_WAIT_MODE_STATIC;
  set_border_up(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_ALL_BORDER:
  SET_WAIT_MODE_STATIC;
  set_all_border(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case INVERSE_COLOR:
  SET_WAIT_MODE_STATIC;
  set_cell_inverse(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_BACKGROUND:
  SET_WAIT_MODE_STATIC;
  set_cell_background(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SELECT_REGION_FOR_EDITBAR:
  This->function = &fc_Select_Region_For_EditBar;
  This->function_type=num_function;
  break;
 case UNSET_INSIDE_PATTERN:
  SET_WAIT_MODE_STATIC;
  unset_cell_inside(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case UNSET_ALL_CELLSTYLE:
  SET_WAIT_MODE_STATIC;
  unset_cellstyle_all(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case UNSET_FORMATNBR:
  SET_WAIT_MODE_STATIC;
  unset_cell_formatnbr(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case UNSET_TEXTSTYLE:
  SET_WAIT_MODE_STATIC;
  unset_cell_textstyle(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SET_UNSET_GRID:
  SET_WAIT_MODE_STATIC;
  set_unset_grid(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ZOOM_50_SHEET:
  zoom_feuille_at_percent(This, 50);
  break;
 case ZOOM_75_SHEET:
  zoom_feuille_at_percent(This, 75);
  break;
 case ZOOM_100_SHEET:
  zoom_feuille_at_percent(This, 100);
  break;
 case ZOOM_200_SHEET:
  zoom_feuille_at_percent(This, 200);
  break;
 case ZOOM_400_SHEET:
  zoom_feuille_at_percent(This, 400);
  break;
 case ZOOM_SELECTION:
  zoom_feuille_selection(This);
  break;
 case ZOOM_PLUS_SHEET:
  zoom_feuille_plus(This);
  break;
 case ZOOM_MINUS_SHEET:
  zoom_feuille_minus(This);
  break;
 case REDRAW_ALL:
  SET_WAIT_MODE_STATIC;
  redraw_all_feuille(This);
  UNSET_WAIT_MODE;
  break;
 case CUT_REGION:
  cut_region(This);
  break;
 case COPY_REGION:
  copy_region(This);
  break;
 case PASTE_REGION:
  SET_WAIT_MODE_STATIC;
  paste_region(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ALIGN_DEFAULT:
  SET_WAIT_MODE_STATIC;
  set_ruler_default(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ALIGN_H_LEFT:
  SET_WAIT_MODE_STATIC;
  set_ruler_align_h_left(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ALIGN_H_RIGHT:
  SET_WAIT_MODE_STATIC;
  set_ruler_align_h_right(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ALIGN_H_CENTER:
  SET_WAIT_MODE_STATIC;
  set_ruler_align_h_center(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case JUSTIFY_H:
  SET_WAIT_MODE_STATIC;
  set_ruler_justify_h(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ALIGN_V_TOP:
  SET_WAIT_MODE_STATIC;
  set_ruler_align_v_top(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ALIGN_V_BOTTOM:
  SET_WAIT_MODE_STATIC;
  set_ruler_align_v_bottom(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ALIGN_V_CENTER:
  SET_WAIT_MODE_STATIC;
  set_ruler_align_v_center(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case JUSTIFY_V:
  SET_WAIT_MODE_STATIC;
  set_ruler_justify_v(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case  MONOLINE:
  SET_WAIT_MODE_STATIC;
  set_ruler_monoline(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case MULTILINES:
  SET_WAIT_MODE_STATIC;
  set_ruler_multiline(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ANGLE_0:
  SET_WAIT_MODE_STATIC;
  set_ruler_angle_0(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ANGLE_90:
  SET_WAIT_MODE_STATIC;
  set_ruler_angle_90(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ANGLE_270:
  SET_WAIT_MODE_STATIC;
  set_ruler_angle_270(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
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
 case SET_SHADOW:
  SET_WAIT_MODE_STATIC;
  set_font_shadow(This);
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
 case INC_FONT_SIZE:
  SET_WAIT_MODE_STATIC;
  set_font_inc_size(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DEC_FONT_SIZE:
  SET_WAIT_MODE_STATIC;
  set_font_dec_size(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case PERCENT_FORMAT:
  SET_WAIT_MODE_STATIC;
  set_format_percent(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case CURRENCY_FORMAT:
  SET_WAIT_MODE_STATIC;
  set_format_currency(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case THOUSANDS_FORMAT:
  SET_WAIT_MODE_STATIC;
  set_format_thousands(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SCIENTIFIC_FORMAT:
  SET_WAIT_MODE_STATIC;
  set_format_scientific(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ENGINEER_FORMAT:
  SET_WAIT_MODE_STATIC;
  set_format_engineer(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case FORMAT_ADD_DIGIT:
  SET_WAIT_MODE_STATIC;
  set_format_add_digit(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case FORMAT_SUB_DIGIT:
  SET_WAIT_MODE_STATIC;
  set_format_sub_digit(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DISPLAY_COLUMN_ORIGIN:
  SET_WAIT_MODE_STATIC;
  display_column_origin(This);
  UNSET_WAIT_MODE;
  break;
 case DISPLAY_ROW_ORIGIN:
  SET_WAIT_MODE_STATIC;
  display_row_origin(This);
  UNSET_WAIT_MODE;
  break;
 case UNSET_FORMULAS:
  SET_WAIT_MODE_STATIC;
  unset_cell_formulas(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case UNSET_ALL_REGION:
  SET_WAIT_MODE_STATIC;
  unset_cell_all(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case SELECT_ALL_REGION:
  select_all_region(This);
  break;
 case DELETE_SELECTION:
  SET_WAIT_MODE_STATIC;
  delete_selection(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case INSERT_COLUMN:
  SET_WAIT_MODE_STATIC;
  sheet_insert_column(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case INSERT_ROW:
  SET_WAIT_MODE_STATIC;
  sheet_insert_row(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DELETE_COLUMN:
  SET_WAIT_MODE_STATIC;
  sheet_delete_column(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DELETE_ROW:
  SET_WAIT_MODE_STATIC;
  sheet_delete_row(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case COPY_DOWN:
  SET_WAIT_MODE_STATIC;
  copy_region_r_or_d(This, Xc_ROW);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case COPY_RIGHT:
  SET_WAIT_MODE_STATIC;
  copy_region_r_or_d(This, Xc_COLUMN);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DISPLAY_FRAME_BORDER:
  SET_WAIT_MODE_STATIC;
  display_frame_border(This);
  UNSET_WAIT_MODE;
  break;
 case DISPLAY_PAGE_MARK:
  SET_WAIT_MODE_STATIC;
  display_page_mark(This);
  UNSET_WAIT_MODE;
  break;
 case DISPLAY_FORMULA:
  SET_WAIT_MODE_STATIC;
  display_formula(This);
  UNSET_WAIT_MODE;
  break;
 case DISPLAY_ZERO_CELL:
  SET_WAIT_MODE_STATIC;
  display_zero_cell(This);
  UNSET_WAIT_MODE;
  break;
 case GOTO_LAST_CELL:
  SET_WAIT_MODE_STATIC;
  goto_last_cell(This);
  UNSET_WAIT_MODE;
  break;
 case GOTO_ACTIVE_CELL:
  SET_WAIT_MODE_STATIC;
  goto_active_cell(This);
  UNSET_WAIT_MODE;
  break;
 case SELECT_TO_LAST_CELL:
  SET_WAIT_MODE_STATIC;
  select_to_last_cell(This);
  UNSET_WAIT_MODE;
  break;
 case GRAPH_HISTO_V:
 case GRAPH_HISTO_H:
 case GRAPH_CURVE:
 case GRAPH_SURFACE:
 case GRAPH_SECTOR:
 case GRAPH_RADAR:
  if (!This->select_type && This->frmg->SelectedCadre && 
      !This->frmg->SelectedCadre->NextCadre && 
      This->frmg->SelectedCadre->cadre->object_type == CADRE_VECTOR &&
      ((c_VectorGraph *)(This->frmg->SelectedCadre->cadre->object))
      ->highg)
  {
   c_HighGraph	*highg;
   HG_class_t	class;
	  
   switch(num_function)
   {
   case GRAPH_HISTO_V:
    class = HG_HISTOGRAMME; 
    break;
   case GRAPH_HISTO_H:
    class = HG_BARS;
    break;
   case GRAPH_CURVE:
    class = HG_CURVE;
    break;
   case GRAPH_SURFACE:
    class = HG_SURFACE;
    break;
   case GRAPH_SECTOR:
    class = HG_SECTOR;
    break; 
   case GRAPH_RADAR:
   default:
    class = HG_RADAR;
    break;
   }
   highg = ((c_VectorGraph *)(This->frmg->SelectedCadre
			      ->cadre->object))->highg;
   SET_WAIT_MODE_STATIC;
   F(highg).change_graph_class(highg, class);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  else
  {
   This->function = &fc_Create_Graph;
   This->function_type = num_function;
  }
  break;
 case GRAPH_LEGEND:
  if (!This->select_type && This->frmg->SelectedCadre && 
      !This->frmg->SelectedCadre->NextCadre && 
      This->frmg->SelectedCadre->cadre->object_type == CADRE_VECTOR &&
      ((c_VectorGraph *)(This->frmg->SelectedCadre->cadre->object))
      ->highg)
  {
   c_HighGraph	*highg;
   highg = ((c_VectorGraph *)(This->frmg->SelectedCadre
			      ->cadre->object))->highg;
   SET_WAIT_MODE_STATIC;
   F(highg).set_unset_legend(highg);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case GRAPH_AXES:
  if (!This->select_type && This->frmg->SelectedCadre && 
      !This->frmg->SelectedCadre->NextCadre && 
      This->frmg->SelectedCadre->cadre->object_type == CADRE_VECTOR &&
      ((c_VectorGraph *)(This->frmg->SelectedCadre->cadre->object))
      ->highg)
  {
   c_HighGraph	*highg;
   highg = ((c_VectorGraph *)(This->frmg->SelectedCadre
			      ->cadre->object))->highg;
   SET_WAIT_MODE_STATIC;
   F(highg).set_unset_grad_y(highg);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case GRAPH_TITLE:
  if (!This->select_type && This->frmg->SelectedCadre && 
      !This->frmg->SelectedCadre->NextCadre && 
      This->frmg->SelectedCadre->cadre->object_type == CADRE_VECTOR &&
      ((c_VectorGraph *)(This->frmg->SelectedCadre->cadre->object))
      ->highg)
  {
   c_HighGraph	*highg;
   highg = ((c_VectorGraph *)(This->frmg->SelectedCadre
			      ->cadre->object))->highg;
   SET_WAIT_MODE_STATIC;
   F(highg).set_unset_title(highg);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case GRAPH_ABSCISSE:
  if (!This->select_type && This->frmg->SelectedCadre && 
      !This->frmg->SelectedCadre->NextCadre && 
      This->frmg->SelectedCadre->cadre->object_type == CADRE_VECTOR &&
      ((c_VectorGraph *)(This->frmg->SelectedCadre->cadre->object))
      ->highg)
  {
   c_HighGraph	*highg;
   highg = ((c_VectorGraph *)(This->frmg->SelectedCadre
			      ->cadre->object))->highg;
   SET_WAIT_MODE_STATIC;
   F(highg).set_unset_abscisse(highg);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case GRAPH_ORDONNEE:
  if (!This->select_type && This->frmg->SelectedCadre && 
      !This->frmg->SelectedCadre->NextCadre && 
      This->frmg->SelectedCadre->cadre->object_type == CADRE_VECTOR &&
      ((c_VectorGraph *)(This->frmg->SelectedCadre->cadre->object))
      ->highg)
  {
   c_HighGraph	*highg;
   highg = ((c_VectorGraph *)(This->frmg->SelectedCadre
			      ->cadre->object))->highg;
   SET_WAIT_MODE_STATIC;
   F(highg).set_unset_ordonnee(highg);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case GRAPH_CONFIG:
  if (!This->select_type && This->frmg->SelectedCadre && 
      !This->frmg->SelectedCadre->NextCadre && 
      This->frmg->SelectedCadre->cadre->object_type == CADRE_VECTOR &&
      ((c_VectorGraph *)(This->frmg->SelectedCadre->cadre->object))
      ->highg)
  {
   c_HighGraph	*highg;
   highg = ((c_VectorGraph *)(This->frmg->SelectedCadre
			      ->cadre->object))->highg;
   INIT(This);			/* becoze event loop occured */
   F(highg).configure(highg);
   return TRUE;
  }
  break;
 default:
  if (num_function > FRMG_FUNCTION_BEGIN &&
      num_function < FRMG_FUNCTION_END)
  {
   old_func = This->frmg->function_type;
   ret_code = F(This->frmg).set_function(This->frmg, num_function);
   if (old_func != This->frmg->function_type)
   {
    This->function = &fc_Dispatch_To_FrMg;
    This->function_type = FRMG_FUNCTION;
   }
  }
  break;
 }
 if (This->function_type != FRMG_FUNCTION)
  This->frmg->function_type = 0;
 Xc_TRACE(("init function %d",This->function_type));
 INIT(This); 
  
 return ret_code;
}

static void select_region(This, x_min, y_min, x_max, y_max, type, draw)
c_Feuille	*This;
int		x_min;
int		y_min;
int		x_max;
int		y_max;
int		type;
boolean		draw;
{
 region_t	*new_region;
 region_t	*new_region2;
 region_t	*new_region3;

 if (!This->select_type)
 {
  F(This->frmg).deselect_all_cadre(This->frmg, TRUE);
  This->select_type = TRUE;
 }
 switch(type)
 {
 case Xq_CELL:
  new_region = (region_t *)Xc_malloc("new_region",sizeof(region_t));
  new_region->x_min = x_min;
  new_region->y_min = y_min;
  new_region->x_max = x_max;
  new_region->y_max = y_max;
  new_region->NextRegion = NULL;
  add_region_in_list(This, new_region, Xq_CELL);
  if (draw)
   focus_in_region(This, new_region, Xq_CELL);
  break;
 case Xq_COLUMN:
  new_region  = (region_t *)Xc_malloc("new_region",sizeof(region_t));
  new_region2 = (region_t *)Xc_malloc("new_region",sizeof(region_t));
  new_region->x_min = x_min;
  new_region->y_min = 0;
  new_region->x_max = x_max;
  new_region->y_max = ROW_MAX;
  new_region->NextRegion = NULL;
  memcpy(new_region2,new_region,sizeof(region_t));
  add_region_in_list(This, new_region, Xq_CELL);
  add_region_in_list(This, new_region2, Xq_COLUMN);
  if (draw)
  {
   focus_in_region(This, new_region, Xq_CELL);
   focus_in_region(This, new_region2, Xq_COLUMN);
  }
  break;
 case Xq_ROW:
  new_region = (region_t *)Xc_malloc("new_region",sizeof(region_t));
  new_region2 = (region_t *)Xc_malloc("new_region",sizeof(region_t));
  new_region->x_min = 0;
  new_region->y_min = y_min;
  new_region->x_max = COLUMN_MAX;
  new_region->y_max = y_max;
  new_region->NextRegion = NULL;
  memcpy(new_region2,new_region,sizeof(region_t));
  add_region_in_list(This, new_region, Xq_CELL);
  add_region_in_list(This, new_region2, Xq_ROW);
  if (draw)
  {
   focus_in_region(This, new_region, Xq_CELL);
   focus_in_region(This, new_region2, Xq_ROW);
  }
  break;
 case Xq_ORIGIN:
  new_region  = (region_t *)Xc_malloc("new_region",sizeof(region_t));
  new_region2 = (region_t *)Xc_malloc("new_region",sizeof(region_t));
  new_region3 = (region_t *)Xc_malloc("new_region",sizeof(region_t));
  new_region->x_min = 0;
  new_region->y_min = 0;
  new_region->x_max = COLUMN_MAX;
  new_region->y_max = ROW_MAX;
  new_region->NextRegion = NULL;
  memcpy(new_region2,new_region,sizeof(region_t));
  memcpy(new_region3,new_region,sizeof(region_t));
  add_region_in_list(This, new_region, Xq_CELL);
  add_region_in_list(This, new_region2, Xq_COLUMN);
  add_region_in_list(This, new_region3, Xq_ROW);
  This->origin_selected = TRUE;
  if (draw)
  {
   focus_in_region(This, ALL_REGION, Xq_ORIGIN);
   focus_in_region(This, new_region, Xq_CELL);
   focus_in_region(This, new_region2, Xq_COLUMN);
   focus_in_region(This, new_region3, Xq_ROW);
  }
  break;
 }
}

static void deselect_region(This, type, draw) 
c_Feuille *This;
int	  type;
boolean	  draw;
{
 region_t	*region;
 region_t	*nregion;

 This->origin_selected = FALSE;
 if (This->select_type && draw)
 {
  focus_out_region(This, ALL_REGION, Xq_ORIGIN);
  focus_out_region(This, ALL_REGION, Xq_COLUMN);
  focus_out_region(This, ALL_REGION, Xq_ROW);
  focus_out_region(This, ALL_REGION, Xq_CELL);
 }

 region = This->cell_region;
 while(region)
 {
  nregion = region->NextRegion;
  Xc_free(region);
  region = nregion;
 }
 This->cell_region = NULL;
 This->nb_cell_region = 0;

 region = This->col_region;
 while(region)
 {
  nregion = region->NextRegion;
  Xc_free(region);
  region = nregion;
 }
 This->col_region = NULL;
 This->nb_col_region = 0;

 region = This->row_region;
 while(region)
 {
  nregion = region->NextRegion;
  Xc_free(region);
  region = nregion;
 }
 This->row_region = NULL;
 This->nb_row_region = 0;
}

static boolean is_selected(This, column, row, type)
c_Feuille	*This;
int		row;
int		column;
int		type;
{
 if (F(This).IsVisible(This, column, row, type))
 {
  switch(type)
  {
  case Xq_CELL:
   return This->cell_info[column-This->column_orig]
    [row-This->row_orig].selected!=0;
   break;
  case Xq_COLUMN:
   return This->column_info[column-This->column_orig]!=0;
   break;
  case Xq_ROW:
   return This->row_info[row-This->row_orig]!=0;
   break;
  case Xq_ORIGIN:
   return This->origin_info!=0;
   break;
  }
 }
 else 
  return is_selected_def(This, column, row, type);
 return FALSE;
}

static boolean is_selected_def(This, column, row, type)
c_Feuille	*This;
int		row;
int		column;
int		type;
{
 region_t	*region;

 switch(type) 
 {
 case Xq_CELL:
  region = This->cell_region;
  while(region)
  {
   if ( column>=region->x_min && column<=region->x_max && 
       row>=region->y_min && row<=region->y_max)
    return TRUE;
   region = region->NextRegion;
  }
  break;
 case Xq_COLUMN:
  region = This->col_region;
  while(region)
  {
   if ( column>=region->x_min && column<=region->x_max)
    return TRUE;
   region = region->NextRegion;
  }
  break;
 case Xq_ROW:
  region = This->row_region;
  while(region)
  {
   if ( row>=region->y_min && row<=region->y_max)
    return TRUE;
   region = region->NextRegion;
  }
  break;
 case Xq_ORIGIN:
  return This->origin_selected;
  break;
 }
 return FALSE;
}

static boolean is_visible(This, column, row, type)
c_Feuille	*This;
int		row;
int		column;
int		type;
{
 switch(type)
 {
 case Xq_CELL:
  if (column>=This->column_orig && 
      column<This->column_orig+This->nb_column &&
      row>=This->row_orig &&
      row<This->row_orig+This->nb_row)
   return TRUE;
  break;
 case Xq_COLUMN:
  if (column>=This->column_orig && 
      column<This->column_orig+This->nb_column)
   return TRUE;
  break;
 case Xq_ROW:
  if (row>=This->row_orig &&
      row<This->row_orig+This->nb_row)
   return TRUE;
  break;
 case Xq_ORIGIN:
  return TRUE;			/* not needed normaly !! */
  break;
 }
 return FALSE;
}

static boolean is_in(region, column, row)
region_t	*region;
int		row;
int		column;
{
 while(region)
 {
  if (column >= region->x_min && column <= region->x_max &&
      row >= region->y_min && row <= region->y_max)
   return TRUE;
  region = region->NextRegion;
 }
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** GET CELL BY XY                                                    ** 
** ----------------------------------------------------------------- */

static boolean get_cell_by_xy(This, x, y, column, row)
c_Feuille	*This;     
int		x;
int		y;
int		*column;
int		*row;
{
 int	x_ind, y_ind;
 int	x_cum, y_cum;
  
 if (x<0)
 {
  if (column) *column = -1;
  return FALSE;
 }
 if (x>=This->sheet_width)
 {
  if (column) *column = COLUMN_MAX;
  return FALSE;
 }
 if (y<0)
 {
  if (row) *row = -1;
  return FALSE;
 }
 if (y>This->sheet_height)
 {
  if (row) *row = ROW_MAX;
  return FALSE;
 }
 x_ind = -1;
 x_cum = 0;
 do
 {
  x_ind ++;
  x_cum += This->column_width[x_ind];
 } 
 while( x_ind<This->nb_column  && x>x_cum);
 y_ind = -1;
 y_cum = 0;
 do
 {
  y_ind++;
  y_cum += This->row_height[y_ind];
 }
 while( y_ind<This->nb_row  && y>y_cum);
  
 if (column)
  *column = x_ind + This->column_orig;
 if (row)
  *row = y_ind + This->row_orig;
 return TRUE;
}

static boolean get_column_by_x(This, x, column)
c_Feuille	*This;     
int		x;
int		*column;
{
 int	x_ind, x_cum;
  
 if (x<0)
 {
  if (column) *column = -1;
  return FALSE;
 }
 if (x>=This->sheet_width)
 {
  if (column) *column = COLUMN_MAX;
  return FALSE;
 }
 x_ind = -1;
 x_cum = 0;
 do
 {
  x_ind ++;
  x_cum += This->column_width[x_ind];
 } 
 while( x_ind<This->nb_column  && x>x_cum);

 if (column)
  *column = x_ind + This->column_orig;
 return TRUE;
}
static boolean get_row_by_y(This, y, row)
c_Feuille	*This;     
int		y;
int		*row;
{
 int	y_ind, y_cum;
  
 if (y<0)
 {
  if (row) *row = -1;
  return FALSE;
 }
 if (y>This->sheet_height)
 {
  if (row) *row = ROW_MAX;
  return FALSE;
 }
 y_ind = -1;
 y_cum = 0;
 do
 {
  y_ind++;
  y_cum += This->row_height[y_ind];
 }
 while( y_ind<This->nb_row  && y>y_cum);
 if (row)
  *row = y_ind + This->row_orig;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** GET COLUMN BORDER BY X                                            ** 
** ----------------------------------------------------------------- */

static boolean get_column_border_by_x(This, x, column)
c_Feuille	*This;
int		x;
int		*column;
{
 int	x_ind;
 int	x_cum;

 if (x<0)
 {
  if (column) *column = -1;
  return FALSE;
 }
 if (x >= This->sheet_width)
 {
  if (column) *column = -1;
  return FALSE;
 }
 x_ind = -1;
 x_cum = 0;
 do
 {
  x_ind++;
  x_cum += This->column_width[x_ind];
 } 
 while( x_ind<This->nb_column  && x>x_cum);
  
 if (column) 
  *column = x_ind + This->column_orig;
  
 if (x_cum-x <= 3)
  return TRUE;
 if (x_ind && x-x_cum+This->column_width[x_ind] <= 3)
 {
  if (column)
   *column = *column - 1;
  return TRUE;
 }
    
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** GET ROW BORDER BY Y                                               ** 
** ----------------------------------------------------------------- */

static boolean get_row_border_by_y(This, y, row)
c_Feuille	*This;
int		y;
int		*row;
{
 int	y_ind;
 int	y_cum;

 if (y<0)
 {
  if (row) *row = -1;
  return FALSE;
 }
 if (y >= This->sheet_height)
 {
  if (row) *row = -1;
  return FALSE;
 }
 y_ind = -1;
 y_cum = 0;
 do
 {
  y_ind++;
  y_cum += This->row_height[y_ind];
 } 
 while( y_ind<This->nb_row  && y>y_cum);
  
 if (row) 
  *row = y_ind + This->row_orig;
  
 if (y_cum-y <= 3)
  return TRUE;
  
 if (y_ind && y-y_cum+This->row_height[y_ind] <= 3)
 {
  if (row)
   *row = *row - 1;
  return TRUE;
 }

 return FALSE;
}
/* ----------------------------------------------------------------- ** 
** GET CELL GEOMETRY                                                 ** 
** ----------------------------------------------------------------- */

static void get_cell_geometry(This, column, row, type, x, y, width, height)
c_Feuille	*This;
int		column;
int		row;
int		type;
int		*x;
int		*y;
int		*width;
int		*height;
{
 int	width_cumul;
 int	height_cumul;
 int	i;
  
 if (!F(This).IsVisible(This, column, row, type))
 {
  *x= -1;
  *y= -1;
  return;
 }
 switch(type)
 {
 case Xq_CELL:
  width_cumul=0;
  for(i=0; i<column-This->column_orig; i++)
   width_cumul += This->column_width[i];
  *x = width_cumul;
  *width = This->column_width[i];
  height_cumul=0;
  for(i=0; i<row-This->row_orig; i++)
   height_cumul += This->row_height[i];
  *y = height_cumul;
  *height = This->row_height[i];
  break;
 case Xq_COLUMN:
  width_cumul=0;
  for(i=0 ;i<column-This->column_orig; i++)
   width_cumul += This->column_width[i];
  *x = width_cumul;
  *width = This->column_width[i];
  *y=0;
  *height= This->origin_height;
  break;
 case Xq_ROW:
  *x=0;
  *width=This->origin_width;
  height_cumul=0;
  for(i=0; i<row-This->row_orig; i++)
   height_cumul += This->row_height[i];
  *y = height_cumul;
  *height = This->row_height[i];
  break;
 case Xq_ORIGIN:
  *x=0;
  *width=This->origin_width;
  *y=0;
  *height=This->origin_height;
  break;
 }
}

static void set_X_info(This,display,window, window_O, window_C, window_R)
c_Feuille	*This;
Display		*display;
Window		window;
Window		window_O;
Window		window_C;
Window		window_R;
{
 GC	gc;
 XGCValues	val_gc;
 color_cells_t	tmpcolorc;
 static char	hachure[2]={ 2, 1};
  
 if (!This->X_info_calc)
 {
  This->X_info.display = This->X_info_O.display =
   This->X_info_C.display = This->X_info_R.display = display;
  This->X_info.window = window;
  This->X_info_O.window = window_O;
  This->X_info_C.window = window_C;
  This->X_info_R.window = window_R;
  This->X_info.ecran = This->X_info_O.ecran =
   This->X_info_C.ecran = This->X_info_R.ecran = DefaultScreen(display);
    
  if (GlobColormap->Cblack)
   val_gc.function = GXxor;
  else
   val_gc.function = GXinvert;
  val_gc.plane_mask = -1;
  val_gc.background = This->X_info.contour_color = GlobColormap->Cwhite;
  val_gc.foreground = GlobColormap->Cblack;
  val_gc.line_width = 0;
  val_gc.line_style = LineSolid; 
  val_gc.cap_style = CapNotLast;
  val_gc.join_style = JoinMiter;
  gc = XCreateGC(display,window,GCFunction|GCPlaneMask|GCForeground|
	       GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	       GCJoinStyle,&val_gc);
  This->X_info.gc_contour = gc;
    
  val_gc.foreground = GlobColormap->Cblack;
  gc = XCreateGC(display,window,GCFunction|GCPlaneMask|GCForeground|
	       GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	       GCJoinStyle,&val_gc);
  This->gc_select = gc;
    
  /* -------------------------------------------------------------------- */
  /* ------------  X_info of Origin, Column and Row windows  ------------ */
  /* -------------------------------------------------------------------- */
    
  val_gc.function = GXcopy;
  val_gc.line_width = 0;
  val_gc.foreground =  
   This->X_info_O.contour_color = This->X_info_C.contour_color = 
    This->X_info_R.contour_color = GlobColormap->Cblack;
  This->X_info_O.gc_contour =
   XCreateGC(display, window_O, GCFunction|GCPlaneMask|GCForeground|
	     GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	     GCJoinStyle, &val_gc);
  This->X_info_C.gc_contour =
   XCreateGC(display ,window_C, GCFunction|GCPlaneMask|GCForeground|
	     GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	     GCJoinStyle, &val_gc);
  This->X_info_R.gc_contour =
   XCreateGC(display, window_R, GCFunction|GCPlaneMask|GCForeground|
	     GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	     GCJoinStyle,&val_gc);
    
  val_gc.foreground =  
   This->X_info_O.inside_color = This->X_info_C.inside_color = 
    This->X_info_R.inside_color = This->col_back_light;
  This->X_info_O.gc_inside =
   XCreateGC(display, window_O, GCFunction|GCPlaneMask|GCForeground|
	     GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	     GCJoinStyle, &val_gc);
  This->X_info_C.gc_inside =
   XCreateGC(display, window_C, GCFunction|GCPlaneMask|GCForeground|
	     GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	     GCJoinStyle,&val_gc);
  This->X_info_R.gc_inside =
   XCreateGC(display, window_R, GCFunction|GCPlaneMask|GCForeground|
	     GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	     GCJoinStyle,&val_gc);
    
  val_gc.function = GXcopy;
  val_gc.line_width = 0;
  val_gc.cap_style = CapButt;
  val_gc.foreground = This->X_info.inside_color = GlobColormap->Cblack;
  val_gc.background = GlobColormap->Cwhite;
  val_gc.graphics_exposures = True;
  gc = XCreateGC(display,window,GCFunction|GCForeground|GCBackground|
	       GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle|
	       GCGraphicsExposures,&val_gc);
  This->X_info.gc_inside = gc;
    
  val_gc.background = GlobColormap->Cblack;
  val_gc.foreground = GlobColormap->Cwhite;
  gc = XCreateGC(display,window,GCFunction|GCForeground|GCBackground|
	       GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle,&val_gc);
  This->gc_grid1 = gc;
    
  val_gc.line_style = LineDoubleDash;
  gc=XCreateGC(display,window,GCFunction|GCForeground|GCBackground|
	       GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle,&val_gc);
  XSetDashes(display, gc, 0, hachure, 2);
  This->gc_page_mark = gc;

  if (GlobColormap->depth == 1)
   val_gc.foreground = GlobColormap->Cblack;
  else
  {
   F(GlobColormap).get_closest_color(GlobColormap, 
				     &tmpcolorc, COL_GRID_RELIEF);
   val_gc.foreground = tmpcolorc.pixel;
  }
  val_gc.line_style = LineSolid; 
  gc = XCreateGC(display, window, GCFunction|GCForeground|GCBackground|
		 GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle,&val_gc);
  This->gc_grid2 = gc;
    
  val_gc.function = GXcopy;
  val_gc.background = GlobColormap->Cblack;
  val_gc.foreground = GlobColormap->Cwhite;
  gc = XCreateGC(display, window_C, 
		 GCFunction|GCForeground|GCBackground, &val_gc);
  This->gc_font_paint = gc;
  This->gc_font_mask = (GC)0;
    
  F(This->frmg).set_X_info(This->frmg, display, window, This->cursor);
    
  This->X_info_calc = TRUE;
 }
}

void redraw_rectangle(This, type, x1, y1, x2, y2)
c_Feuille	*This;
int		type;
int		x1;
int		y1;
int		x2;
int		y2;
{
 int	x_ind;
 int	y_ind;
 int	x_min,x_max,y_min,y_max;
 int	r_xmin, r_xmax, r_ymin, r_ymax; 
 XRectangle	rectangle;
  
 switch(type)
 {
 case Xq_CELL:
  F(This->frmg).start_draw_cache(This->frmg);
  x_max = r_xmax = r_ymax = 0;
  r_xmin = This->sheet_width;
  r_ymin = This->sheet_height;
  This->sequence++;
  for(x_ind=0; x_ind<This->nb_column; x_ind++)
  {
   x_min = x_max;
   x_max += This->column_width[x_ind];
   if (x1<x_max && x2>x_min)
   {
    y_max = 0;
    for(y_ind=0; y_ind<This->nb_row; y_ind++)
    {
     y_min = y_max;
     y_max += This->row_height[y_ind];
     if (y1<y_max && y2>y_min)
     {
      if (x_min<r_xmin) r_xmin = x_min;
      if (x_max>r_xmax)	r_xmax = x_max;
      if (y_min<r_ymin) r_ymin = y_min;
      if (y_max>r_ymax)	r_ymax = y_max;
      draw_cellule(This,
		   x_ind+This->column_orig,
		   y_ind+This->row_orig,
		   x_min, y_min,
		   This->column_width[x_ind],
		   This->row_height[y_ind]);
     }
    }
   }
  }
  if ( r_xmin<r_xmax && r_ymin<r_ymax )
  {
   F(This->frmg).redraw_rectangle(This->frmg, r_xmin, r_ymin, 
				  r_xmax, r_ymax); 
   F(This->frmg).flush_draw_cache(This->frmg);
   if (!This->select_type)
    F(This->frmg).redraw_rectangle_handle(This->frmg, r_xmin, r_ymin,
					  r_xmax - r_xmin + 1,
					  r_ymax - r_ymin + 1);
   if(This->function_data && This->function_type != FRMG_FUNCTION)
   {
    rectangle.x=r_xmin;
    rectangle.y=r_ymin;
    rectangle.width=r_xmax - r_xmin;
    rectangle.height= r_ymax - r_ymin;
    XSetClipRectangles(_display, _gc_contour, 
		       0, 0, &rectangle, 1, Unsorted);
    if (This->function_type == SELECT_MOVE_RESIZE_CELL)
    {
     XSetClipRectangles(_display, This->frmg->X_info.gc_contour, 
			0, 0, &rectangle, 1, Unsorted);
     REDRAW(This);
     XSetClipMask(_display, This->frmg->X_info.gc_contour, None);
    }
    else
     REDRAW(This);
    XSetClipMask(_display, _gc_contour, None);
   }
  }
  else
   F(This->frmg).flush_draw_cache(This->frmg);
  break;
 case Xq_ORIGIN:
  if (This->Column_Origin_actif && This->Row_Origin_actif)
   draw_origin(This, 0, 0, This->origin_width, This->origin_height);
  break;
 case Xq_COLUMN:
  if (!This->Column_Origin_actif) return; 
  x_max = 0;
  for(x_ind=0; x_ind<This->nb_column; x_ind++)
  {
   x_min = x_max;
   x_max += This->column_width[x_ind];
   if (x1<x_max && x2>x_min)
   {
    draw_column(This, x_ind+This->column_orig, x_min, 0,
		This->column_width[x_ind], This->origin_height);
   }
  }
  break;
 case Xq_ROW:
  if (!This->Row_Origin_actif) return;
  y_max = 0;
  for(y_ind=0; y_ind<This->nb_row; y_ind++)
  {
   y_min = y_max;
   y_max += This->row_height[y_ind];
   if (y1<y_max && y2>y_min)
   {
    draw_row(This, y_ind+This->row_orig, 0, y_min,
	     This->origin_width, This->row_height[y_ind]);
   }
  }
  break;
 }
}

void change_feuille_origin(This, x, y)
c_Feuille	*This;
int		x;
int		y;
{
 int	col_pos,col_pos2;
 int	row_pos,row_pos2;
 int	delta_x, delta_y, i;
  
 /*Xc_TRACE(("change feuille origine to x: %d  y: %d",x,y));*/
 if (!x && !y)
  return;
  
 SET_WAIT_MODE_STATIC;
 if ( (x && y)			/* if deplacement in diagonal */ 
     || (ABS(x) >= This->nb_column )/* if deplacement>=length of screen */
     || (ABS(y) >= This->nb_row))
 {
  delta_x =delta_y = 0;
  if (x>0)
  {
   for(i=0; i<x; i++)
    delta_x -= F(This->moteur).
     get_size(This->moteur, Xc_COLUMN, This->column_orig+i) /This->sScale;
  }
  else
  {
   for(i= -1; i>=x; i--)
    delta_x += F(This->moteur).
     get_size(This->moteur, Xc_COLUMN, This->column_orig+i) /This->sScale;
  }
  if (y>0)
  {
   for(i=0; i<y; i++)
    delta_y -= F(This->moteur).
     get_size(This->moteur, Xc_ROW, This->row_orig+i) /This->sScale;
  }
  else
  {
   for(i= -1; i>=y; i--)
    delta_y += F(This->moteur).
     get_size(This->moteur, Xc_ROW, This->row_orig+i) /This->sScale;
  }
    
  This->column_orig += x;
  This->row_orig += y;
  F(This->frmg).change_frmg_origin(This->frmg, delta_x, delta_y, 0);

  calcul_column_width(This);
  calcul_row_height(This);
  calcul_cell_info(This);
  redraw_rectangle(This, Xq_CELL, 0, 0, 
		   This->sheet_width, This->sheet_height);
  redraw_rectangle(This, Xq_COLUMN, 0, 0, 
		   This->sheet_width, This->origin_height);
  redraw_rectangle(This, Xq_ROW, 0, 0, 
		   This->origin_width, This->sheet_height);
  UNSET_WAIT_MODE;
  return;
 }
 if (x)
 {
  if (x>0)
  {
   col_pos = get_column_pos(This, This->column_orig + x);
   col_pos2 = get_column_pos(This, This->column_orig + 
			     This->nb_column - 1);
   XCopyArea(_display, _window, _window, _gc_inside, col_pos, 0,
	     col_pos2-col_pos, This->sheet_height,
	     get_column_pos(This, This->column_orig), 0);
   XCopyArea(_display, _C_window, _C_window, _C_gc_inside, col_pos, 0,
	     col_pos2-col_pos, This->origin_height,
	     get_column_pos(This, This->column_orig), 0);
   This->column_orig += x;
	  
   F(This->frmg).change_frmg_origin(This->frmg, -col_pos, 0);
	  
   calcul_column_width(This);
   calcul_cell_info(This);
	  
   col_pos = col_pos2 - col_pos +
    get_column_pos(This, This->column_orig);
   redraw_rectangle(This, Xq_CELL, col_pos, 0,
		    This->sheet_width, This->sheet_height);
   redraw_rectangle(This, Xq_COLUMN, col_pos, 0,
		    This->sheet_width, This->origin_height);
  }
  else
  {
   This->column_orig += x;
	  
   calcul_column_width(This);
   col_pos = get_column_pos(This, This->column_orig - x);
   F(This->frmg).change_frmg_origin(This->frmg, col_pos, 0);
   calcul_cell_info(This);

   XCopyArea(_display, _window, _window, _gc_inside, 0, 0,
	     This->sheet_width - col_pos, This->sheet_height,
	     col_pos, 0);
   XCopyArea(_display, _C_window, _C_window, _C_gc_inside, 0, 0,
	     This->sheet_width - col_pos, This->origin_height,
	     col_pos, 0);
	  
   redraw_rectangle(This, Xq_CELL, 0, 0, col_pos, This->sheet_height);
   redraw_rectangle(This, Xq_COLUMN, 0, 0, col_pos, 
		    This->origin_height);
  }
 }
 if (y)
 {
  if (y>0)
  {
   row_pos = get_row_pos(This, This->row_orig + y);
   row_pos2 = get_row_pos(This, This->row_orig + This->nb_row - 1);

   XCopyArea(_display, _window, _window, _gc_inside, 0, row_pos,
	     This->sheet_width, row_pos2 - row_pos, 0,
	     get_row_pos(This, This->row_orig));
   XCopyArea(_display, _R_window, _R_window, _R_gc_inside, 0, row_pos,
	     This->origin_width, row_pos2 - row_pos, 0,
	     get_row_pos(This, This->row_orig));
   This->row_orig += y;
	  
   F(This->frmg).change_frmg_origin(This->frmg, 0, -row_pos);
	  
   calcul_row_height(This);
   calcul_cell_info(This);
	  
   row_pos = row_pos2 - row_pos + get_row_pos(This, This->row_orig);
   redraw_rectangle(This, Xq_CELL, 0, row_pos,
		    This->sheet_width, This->sheet_height);
   redraw_rectangle(This, Xq_ROW, 0, row_pos,
		    This->origin_width, This->sheet_height);
  }
  else
  {
   This->row_orig += y;
   calcul_row_height(This);
   row_pos = get_row_pos(This, This->row_orig - y);
   F(This->frmg).change_frmg_origin(This->frmg, 0, row_pos);
   calcul_cell_info(This);
	  
   XCopyArea(_display, _window, _window, _gc_inside, 0, 0,
	     This->sheet_width, This->sheet_height - row_pos,
	     0, row_pos);
   XCopyArea(_display, _R_window, _R_window, _R_gc_inside, 0, 0,
	     This->origin_width, This->sheet_height - row_pos,
	     0, row_pos);
	  	  
   redraw_rectangle(This, Xq_CELL, 0, 0, This->sheet_width, row_pos);
   redraw_rectangle(This, Xq_ROW, 0, 0, This->origin_width, row_pos);
  }
 }
 UNSET_WAIT_MODE;
}

void change_feuille_scaling(This, new_factor, x ,y)
c_Feuille	*This;
scale_t		new_factor;
int		x;
int		y;
{
 coord_t	new_scale;
 scale_t	font_factor;

 Xc_HISTORY(("change feuille to zoom factor %f %%",
	     SCALE_TO_PERCENT(new_factor)));
  
 if (This->sFactor != new_factor)
 {
  SET_WAIT_MODE_STATIC;
  new_scale=(coord_t)( ( (double)COORD_DEFAULT*
			(double)SCALE_FROM_PERCENT(100) )/
		      (double)new_factor );
  This->sFactor = new_factor;
  This->sScale  = new_scale;
  font_factor = new_factor * COORD_SCALEVALUE / COORD_DEFAULT;
    
  This->column_orig = x;
  This->row_orig = y;
    
  if (This->Row_Origin_actif)
   This->origin_width  = DEFAULT_COL_SIZE / new_scale;
  if (This->Column_Origin_actif)
   This->origin_height = DEFAULT_ROW_SIZE / new_scale;
    
  This->sheet_width = This->Width - This->origin_width;
  This->sheet_height = This->Height - This->origin_height;
    
  This->orig_metrics = 
   F(This->orig_style->metrics).transform(This->orig_style->metrics,
					  font_factor, SCALE_ONE, ANGLE_ZERO);
  if((This->orig_font = This->orig_metrics->raster_font) == NULL)
  {
   if (This->orig_font)
   {
    F(GlobFntMng).delete_font(GlobFntMng, This->orig_font);
    This->orig_font = NULL;
   }
   This->orig_metrics->raster_font = 
    F(GlobFntMng).new_font(GlobFntMng, This->orig_style->font, 
			   This->orig_metrics);
      
   if((This->orig_font = This->orig_metrics->raster_font) == NULL)
   {
    F(This->orig_style->metrics).
     purgeImpure(This->orig_style->metrics, font_factor,
		 SCALE_ONE, ANGLE_ZERO);
   }
  }
  if (_gc_font_mask != (GC)0)
   XFreeGC(_display, _gc_font_mask);
  _gc_font_mask = (GC)0;
  F(This->orig_font).setGC(_display, _window, 
			   &_gc_font_mask, _gc_font_paint);
  F(This->orig_style->color).setGC(This->orig_style->color, 
				   _display, _gc_font_paint);
    
  F(This->frmg).change_frmg_scaling(This->frmg, new_factor, 0, 0);

  calcul_column_width(This);
  calcul_row_height(This);
  calcul_cell_info(This);
    
  F(This->ScrollBarH).Change_Rsize(This->ScrollBarH,
				   (int)(SCROLLB_H_AREA /
					 SCALE_TO_COEF(This->sFactor)),
				   This->nb_column);
  F(This->ScrollBarV).Change_Rsize(This->ScrollBarV,
				   (int)(SCROLLB_V_AREA /
					 SCALE_TO_COEF(This->sFactor)),
				   This->nb_row);
    
  if (This->Column_Origin_actif && This->Row_Origin_actif)
   XMoveResizeWindow(_display, _O_window, 0, 0, 
		     This->origin_width, This->origin_height);
  if (This->Column_Origin_actif)
   XMoveResizeWindow(_display, _C_window, This->origin_width, 0, 
		     This->sheet_width, This->origin_height);
  if (This->Row_Origin_actif)
   XMoveResizeWindow(_display, _R_window, 0, This->origin_height, 
		     This->origin_width, This->sheet_height);
  XMoveResizeWindow(_display, _window, This->origin_width, 
		    This->origin_height, This->sheet_width, 
		    This->sheet_height);
    
  if (This->Column_Origin_actif && This->Row_Origin_actif)
   XClearArea(_display, _O_window, 0, 0, This->origin_width, 
	      This->origin_height, True);
  if (This->Column_Origin_actif)
   XClearArea(_display, _C_window, 0, 0, This->sheet_width, 
	      This->origin_height, True);
  if (This->Row_Origin_actif)
   XClearArea(_display, _R_window, 0, 0, This->origin_width, 
	      This->sheet_height, True);
  XClearArea(_display, _window, 0, 0, This->sheet_width, 
	     This->sheet_height, True); 
  UNSET_WAIT_MODE;
 }
}

/* ----------------------------------------------------------------- ** 
** GET/SET ACTIVE CELLULE                                            ** 
** ----------------------------------------------------------------- */

static cell *get_active_cellule(This, column, row)
c_Feuille *This;
int *column;
int *row;
{
 if (column)
  *column = This->cell_x;
 if (row)
  *row = This->cell_y;
 return  F(This->moteur).get_cell(This->moteur, This->cell_x, This->cell_y);
}

static void set_active_cellule(This, column, row, draw)
c_Feuille *This;
int column;
int row;
boolean	draw;
{
 int	delta_x;
 int	delta_y;
 boolean	scroll = TRUE;
  
 if (column != This->cell_x || row != This->cell_y || 
     (!F(This).IsVisible(This, column, row, Xq_CELL) || !This->select_type) )
 {
  if (This->cell_x>=0 || This->cell_y>=0)
  {
   if (column<0) column = 0;
   if (column>COLUMN_MAX) 
    column=COLUMN_MAX;
	  
   if (row<0) row = 0;
   if (row>ROW_MAX) 
    row=ROW_MAX;
	  
   delta_x = column - This->column_orig;
   delta_y = row - This->row_orig;
	  
   if (F(This).IsVisible(This, This->cell_x, This->cell_y, Xq_CELL) &&
       This->select_type)
   {
    focus_out_active_cellule(This, This->cell_x, This->cell_y);
   }
   if (draw)
    F(This).DeselectRegion(This,Xq_ORIGIN,TRUE);
	  
   This->cell_x = column;
   This->cell_y = row;
   if ((This->cell_x >= This->column_orig+This->nb_column ||
	This->cell_x < This->column_orig) && delta_x)
   {
    F(This->ScrollBarH).Move(This->ScrollBarH, delta_x);
    change_feuille_origin(This, delta_x, 0); 
    scroll = FALSE;
   }
   if ((This->cell_y >= This->row_orig+This->nb_row ||
	This->cell_y < This->row_orig) && delta_y)
   {
    F(This->ScrollBarV).Move(This->ScrollBarV, delta_y);
    change_feuille_origin(This, 0, delta_y); 
    scroll = FALSE;
   }
  }
  else
  {
   This->cell_x = column;
   This->cell_y = row;
  }
  if (draw)
  {
   F(This).SelectRegion(This, column, row, column, row, Xq_CELL, TRUE);
  }
  if (scroll)
   focus_in_active_cellule(This, This->cell_x, This->cell_y);
  F(GlobEditBar).query_aff(GlobEditBar, column, row);
  F(GlobEditBar).refresh_cell(GlobEditBar, This);
 }
}

static void set_active_cellule_relative(This, column, row, draw)
c_Feuille *This;
int column;
int row;
boolean	draw;
{
 int	delta_x;
 int	delta_y;
 boolean	scroll = TRUE;

 delta_x = column;
 delta_y = row;
 column += This->cell_x;
 row += This->cell_y;   
  
 if (!F(This).IsVisible(This, This->cell_x, This->cell_y, Xq_CELL))
 {
  set_active_cellule(This, column, row, draw);
  return;
 }
 if (column < This->column_orig && This->column_orig + delta_x < 0)
 {
  if (This->cell_x == 0 || This->column_orig == 0)
   F(This->ScrollBarH).Move(This->ScrollBarH, delta_x);
  delta_x = -MIN(This->cell_x, This->column_orig);
  column = 0;
 }
 if (column > COLUMN_MAX) 
 {
  column = COLUMN_MAX;
  delta_x = column - This->cell_x;
 }
 if (row < This->row_orig && This->row_orig + delta_y < 0)
 {
  row = 0;
  if (This->cell_y == 0 || This->row_orig == 0)
   F(This->ScrollBarV).Move(This->ScrollBarV, delta_y);
  delta_y = -MIN(This->cell_y, This->row_orig);
 }
 if (row > ROW_MAX) 
 {
  row = ROW_MAX;
  delta_y = row - This->cell_y;
 }
  
 if (column != This->cell_x || row != This->cell_y)
 {
  if (F(This).IsVisible(This, This->cell_x, This->cell_y, Xq_CELL) &&
      This->select_type)
   focus_out_active_cellule(This, This->cell_x, This->cell_y);
  if (draw)
   F(This).DeselectRegion(This,Xq_ORIGIN,TRUE);
  This->cell_x = column;
  This->cell_y = row;
  if (This->cell_x >= This->column_orig+This->nb_column-1 && 
      delta_x && (This->cell_x != COLUMN_MAX || 
		  This->sheet_width >= This->Width-This->origin_width))
  {
   F(This->ScrollBarH).Move(This->ScrollBarH, delta_x);
   change_feuille_origin(This, delta_x, 0);
   scroll = !This->select_type;
  }
  if (This->cell_x < This->column_orig && delta_x)
  {
   F(This->ScrollBarH).Move(This->ScrollBarH, delta_x);
   change_feuille_origin(This, delta_x, 0);
   scroll = !This->select_type;
  }

  if (This->cell_y >= This->row_orig+This->nb_row-1 && 
      delta_y && (This->cell_y != ROW_MAX || 
		  This->sheet_height>=This->Height-This->origin_height))
  {
   F(This->ScrollBarV).Move(This->ScrollBarV, delta_y);
   change_feuille_origin(This, 0, delta_y);
   scroll = !This->select_type;
  }
  if (This->cell_y < This->row_orig && delta_y)
  {
   F(This->ScrollBarV).Move(This->ScrollBarV, delta_y);
   change_feuille_origin(This, 0, delta_y);
   scroll = !This->select_type;
  }

  if (draw)
   F(This).SelectRegion(This, column, row, column, row, Xq_CELL, TRUE);
  if (scroll)
   focus_in_active_cellule(This, This->cell_x, This->cell_y);
  F(GlobEditBar).query_aff(GlobEditBar, column, row);
  F(GlobEditBar).refresh_cell(GlobEditBar, This);
 }
}

static void set_active_cellule_advance_selection(This)
c_Feuille *This;
{
 region_t	*ptr_reg;
 int 		column;
 int 		row;

 ptr_reg = This->last_advance;
 column = This->cell_x;
 row = This->cell_y;
 if (!ptr_reg)
 {
  F(This).SetActiveCelluleRelative(This, 0, 1, TRUE);
  return;
 }

 if ((ptr_reg->x_min == ptr_reg->x_max) &&
     (ptr_reg->y_min == ptr_reg->y_max))
 {
  F(This).SetActiveCelluleRelative(This, 0, 1, TRUE);
  return;
 }
  
 if (IN_REGION(ptr_reg, column, row))
 {
  if (row+1 > ptr_reg->y_max)
  {
   if (column+1 > ptr_reg->x_max)
   {
    if (ptr_reg->NextRegion == NULL)
     ptr_reg = This->cell_region;
    else
     ptr_reg = ptr_reg->NextRegion;
    This->last_advance = ptr_reg;
    column = ptr_reg->x_min;
   }
   else
    column++;
   row = ptr_reg->y_min;
  }
  else
   row++;
  F(This).SetActiveCellule(This, column, row, FALSE);
 }
 else
  F(This).SetActiveCelluleRelative(This, 0, 1, TRUE);
}

static void refresh_cellule(This, column, row)
c_Feuille *This;
int	  column;
int	  row;
{
 int	i, j, io, jo, n;
 int   x,y,width,height;
 cell_info_t	*cinfo;
 int	px, py, dx, dy;
 region_t	region;
 boolean	cache_actif;
  
 px = This->column_orig;
 py = This->row_orig;
 dx = px + This->nb_column-1;
 dy = py + This->nb_row-1;
  
 region.x_min = px; region.x_max = dx + 1;
 region.y_min = py; region.y_max = dy + 1;
 F(This->moteur).fill_cell_info
  (This->moteur, This->cell_info, &region, px, py);
  
 if (F(This).IsVisible(This, column, row, Xq_CELL))
  cinfo = &This->cell_info[column-px][row-py]; 
 else
  cinfo = NULL;
  
 region.x_min = region.x_max = column;
 region.y_min = region.y_max = row;
 region.NextRegion = NULL;
 calcul_cell_overrun_info(This, &region);
  
 cache_actif = This->frmg->redraw_cache_actif;
 if (!cache_actif)
  F(This->frmg).start_draw_cache(This->frmg);
  
 This->sequence++;
  
 for (i=px, io=0; i<=dx; i++, io++)
 {
  for (j=py, jo=0; j<=dy; j++, jo++)
  {
   if (This->cell_info[io][jo].modified )
   {
    F(This).GetCellGeometry(This, i,j, Xq_CELL, &x, &y,&width,&height);
    draw_cellule(This, i, j, x, y, width, height);
   }
  }
 }
  
 if (!cinfo)
 {
  if (!cache_actif)
   F(This->frmg).flush_draw_cache(This->frmg);
  return;
 }

 i = column - px;
 j = row - py;
 if (cinfo->cellule == cinfo->cell_over_left)
 {
  n = i+1;
  while(n < This->nb_column && 
	This->cell_info[n][j].cell_over_left == cinfo->cellule)
  {
   F(This).GetCellGeometry(This, n + This->column_orig, 
			   row, Xq_CELL, &x, &y,&width,&height);
   draw_cellule(This, n + This->column_orig, row, x, y, width, height);
   n++;
  }
 }
 if (cinfo->cellule == cinfo->cell_over_up)
 {
  n = j+1;
  while(n < This->nb_row && 
	This->cell_info[i][n].cell_over_up == cinfo->cellule)
  {
   F(This).GetCellGeometry(This, column, n + This->row_orig, 
			   Xq_CELL, &x, &y,&width,&height);
   draw_cellule(This, column, n + This->row_orig, x, y, width, height);
   n++;
  }
 }
 if (cinfo->cellule == cinfo->cell_over_right)
 {
  n = i-1;
  while(n >= 0 && 
	This->cell_info[n][j].cell_over_right == cinfo->cellule)
  {
   F(This).GetCellGeometry(This, n + This->column_orig, 
			   row, Xq_CELL, &x, &y,&width,&height);
   draw_cellule(This, n + This->column_orig, row, x, y, width, height);
   n--;
  }
 }
 if (cinfo->cellule == cinfo->cell_over_down)
 {
  n = j-1;
  while(n >= 0 && 
	This->cell_info[i][n].cell_over_down == cinfo->cellule)
  {
   F(This).GetCellGeometry(This, column, n + This->row_orig, 
			   Xq_CELL, &x, &y,&width,&height);
   draw_cellule(This, column, n + This->row_orig, x, y, width, height);
   n--;
  }
 }
 F(This).GetCellGeometry(This, column, row, Xq_CELL, &x, &y,&width,&height);
 draw_cellule(This, column, row, x, y, width, height);
  
 if (!cache_actif)
  F(This->frmg).flush_draw_cache(This->frmg);
}

static void refresh_region(This, region, type)
c_Feuille	*This;
region_t	*region;
int		type;
{
 region_t	*nregion, fregion;
 int	i,j, io, jo;
 int   x, y, width, height;
 int	x_min, x_max;
 int	y_min, y_max;
 int	px, py, dx, dy;
 boolean	draw;
  
 px = This->column_orig;
 py = This->row_orig;
 dx = px + This->nb_column-1;
 dy = py + This->nb_row-1;
  
 switch(type)
 {
 case Xq_CELL:
  if (region->x_min <= dx && region->x_max >= px &&
      region->y_min <= dy && region->y_max >= py)
  {
   x_min = MAX(region->x_min, px);
   x_max = MIN(region->x_max, dx);
   y_min = MAX(region->y_min, py);
   y_max = MIN(region->y_max, dy);
   This->sequence++;
   F(This->frmg).start_draw_cache(This->frmg);
      
   fregion.x_min = x_min; fregion.x_max = x_max + 1;
   fregion.y_min = y_min; fregion.y_max = y_max + 1;
   F(This->moteur).fill_cell_info
    (This->moteur, This->cell_info, &fregion, px, py);
      
   nregion = region->NextRegion;
   region->NextRegion = NULL;
   calcul_cell_overrun_info(This, region);
   region->NextRegion = nregion;
      
   /*Xc_TRACE(("xo: %d yo: %d XO: %d YO: %d x: %d y: %d X: %d Y: %d",*/
   /*region->x_min, region->y_min, region->x_max, region->y_max,*/
   /*x_min, y_min, x_max, y_max));*/
      
   for (i=px, io=0; i<=dx; i++, io++)
   {
    draw = (i>=x_min && i<=x_max);
    for (j=py, jo=0; j<=dy; j++, jo++)
    {
     if ( (draw && j>=y_min && j<=y_max) || 
	 This->cell_info[io][jo].modified )
     {
      F(This).GetCellGeometry(This, i,j, Xq_CELL, &x, &y,&width,&height);
      draw_cellule(This, i, j, x, y, width, height);
     }
    }
   }
   F(This->frmg).flush_draw_cache(This->frmg);
  }
  break;
 case Xq_CELLS:
  {
   boolean	none = TRUE;
      
   nregion = region;
   while(nregion)
   {
    x_min = MAX(nregion->x_min, px);
    x_max = MIN(nregion->x_max, dx);
    y_min = MAX(nregion->y_min, py);
    y_max = MIN(nregion->y_max, dy);

    if (x_min <= x_max && y_min <= y_max)
    {
     fregion.x_min = x_min; fregion.x_max = x_max + 1;
     fregion.y_min = y_min; fregion.y_max = y_max + 1;
     F(This->moteur).fill_cell_info
      (This->moteur, This->cell_info, &fregion, px, py);
     none = FALSE;
    }
	  
    nregion = nregion->NextRegion;
   }
   if (none) return;
      
   This->sequence++;
   F(This->frmg).start_draw_cache(This->frmg);
   calcul_cell_overrun_info(This, region);
      
   for (i=px, io=0; i<=dx; i++, io++)
   {
    for (j=py, jo=0; j<=dy; j++, jo++)
    {
     if ( This->cell_info[io][jo].modified || is_in(region, i, j))
     {
      F(This).GetCellGeometry(This, i, j, Xq_CELL,
			      &x, &y, &width, &height);
      draw_cellule(This, i, j, x, y, width, height);
     }
    }
   }
   F(This->frmg).flush_draw_cache(This->frmg);
  }    
  break;
 case Xq_COLUMN:
  if (region->x_min <= dx && region->x_max >= px)
  {
   x_min = MAX(region->x_min, px);
   x_max = MIN(region->x_max, dx+1);
   if (x_min) 
   {
    x_min--;
    redraw_rectangle(This, Xq_COLUMN, get_column_pos(This, x_min), 0,
		     get_column_pos(This, x_max), This->origin_height);
   }
   else
   {
    redraw_rectangle(This, Xq_COLUMN, 0, 0,
		     get_column_pos(This, x_max), This->origin_height);
   }
  }
  break;
 case Xq_ROW:
  if (region->y_min <= dy && region->y_max >= py)
  {
   y_min = MAX(region->y_min, py);
   y_max = MIN(region->y_max, dy+1);
   if (y_min)
   {
    y_min--;
    redraw_rectangle(This, Xq_ROW, 0, get_row_pos(This, y_min),
		     This->origin_width, get_row_pos(This, y_max));
   }
   else
   {
    redraw_rectangle(This, Xq_ROW, 0, 0,
		     This->origin_width, get_row_pos(This, y_max));
   }
  }
  break;
 }
}

/* ----------------------------------------------------------------- ** 
** CALCUL COLUMN/ROW LENGTH                                          ** 
** ----------------------------------------------------------------- */

void calcul_column_width(This)
c_Feuille	*This;    
{
 int		nb_cols;
 int		orig_cols;
 int		Width, rWidth;
 coord_t	size;
 coord_t	offset;
  
 orig_cols = This->column_orig;
 nb_cols = Width = 0;
 rWidth = This->Width;
 if (This->Row_Origin_actif)
  rWidth -= This->origin_width;
 while(Width <= rWidth && orig_cols <= COLUMN_MAX)
 {
  Width += F(This->moteur).get_size(This->moteur, Xc_COLUMN, orig_cols++)
   /This->sScale;
  nb_cols++;
 }
 if (nb_cols != This->nb_column)
 {
  if (This->column_width)
  {
   This->column_width = (short *)Xc_realloc("col width",
					    This->column_width,
					    sizeof(short)*nb_cols);
   This->column_page_mark = 
    (boolean *)Xc_realloc("col width", This->column_page_mark,
			  sizeof(boolean)*nb_cols);
  }
  else
  {
   This->column_width = (short *)Xc_malloc("col width", 
					   sizeof(short)*nb_cols);
   This->column_page_mark = 
    (boolean *)Xc_malloc("col width", sizeof(boolean)*nb_cols);
  }
  This->nb_column = nb_cols;
  F(This->ScrollBarH).Change_Rsize(This->ScrollBarH,
				   (int)(SCROLLB_H_AREA /
					 SCALE_TO_COEF(This->sFactor)),
				   This->nb_column);
 }
 if (Width <= rWidth)
 {
  This->sheet_width = Width;
  if (This->Row_Origin_actif)
   XResizeWindow(_display, _C_window, 
		 This->sheet_width, This->origin_height);
  XResizeWindow(_display, _window, This->sheet_width, This->sheet_height);
 }
 else
  if (This->sheet_width+This->origin_width != This->Width)
  {
   This->sheet_width = This->Width - This->origin_width;
   if (This->Row_Origin_actif)
    XResizeWindow(_display, _C_window, 
		  This->sheet_width, This->origin_height);
   XResizeWindow(_display, _window, This->sheet_width, This->sheet_height);
  }
 orig_cols = This->column_orig;
 offset = 
  F(This->moteur).calcul_page_origin(This->moteur, Xc_COLUMN, 
				     orig_cols, This->page_window_width);
 size = F(This->moteur).get_size(This->moteur, Xc_COLUMN, orig_cols++);
 offset += size;
  
 for(nb_cols = 0; nb_cols < This->nb_column; nb_cols++)
 {
  This->column_width[nb_cols] = size / This->sScale;
  size = F(This->moteur).get_size(This->moteur, Xc_COLUMN, orig_cols++);
  offset += size;
  if (offset >= This->page_window_width)
  {
   This->column_page_mark[nb_cols] = TRUE;
   offset = size;
  }
  else
   This->column_page_mark[nb_cols] = FALSE;
 }
}

void calcul_row_height(This)
c_Feuille	*This;    
{
 int		nb_rows;
 int		orig_rows;
 int		Height, rHeight;
 coord_t	size;
 coord_t	offset;
  
 orig_rows = This->row_orig;
 nb_rows = Height = 0;
 rHeight = This->Height;
 if (This->Column_Origin_actif)
  rHeight -= This->origin_height;
 while(Height <= rHeight && orig_rows <= ROW_MAX)
 {
  Height += 
   F(This->moteur).get_size(This->moteur, Xc_ROW, orig_rows++)/This->sScale;
  nb_rows++;
 }
 if (nb_rows != This->nb_row)
 {
  if (This->row_height)
  {
   This->row_height = (short *)Xc_realloc("col height",
					  This->row_height,
					  sizeof(short)*nb_rows);
   This->row_page_mark = 
    (boolean *)Xc_realloc("col height", This->row_page_mark,
			  sizeof(boolean)*nb_rows);
  }
  else
  {
   This->row_height = (short *)Xc_malloc("col height",
					 sizeof(short)*nb_rows);
   This->row_page_mark = (boolean *)Xc_malloc("col height",
					      sizeof(boolean)*nb_rows);
  }
  This->nb_row = nb_rows;
  F(This->ScrollBarV).Change_Rsize(This->ScrollBarV,
				   (int)(SCROLLB_V_AREA /
					 SCALE_TO_COEF(This->sFactor)),
				   This->nb_row);
 }
 if (Height <= rHeight)
 {
  This->sheet_height = Height;
  if (This->Column_Origin_actif)
   XResizeWindow(_display, _R_window, 
		 This->origin_width, This->sheet_height);
  XResizeWindow(_display, _window, This->sheet_width, This->sheet_height);
 }
 else
  if (This->sheet_height+This->origin_height != This->Height)
  {
   This->sheet_height = This->Height - This->origin_height;
   if (This->Column_Origin_actif)
    XResizeWindow(_display, _R_window, 
		  This->origin_width, This->sheet_height);
   XResizeWindow(_display, _window, This->sheet_width, This->sheet_height);
  }
 orig_rows = This->row_orig;
 offset =    
  F(This->moteur).calcul_page_origin(This->moteur, Xc_ROW, 
				     orig_rows, This->page_window_height);
 size = F(This->moteur).get_size(This->moteur, Xc_ROW, orig_rows++);
 offset += size;
  
 for(nb_rows = 0; nb_rows < This->nb_row; nb_rows++)
 {
  This->row_height[nb_rows] = size / This->sScale;
  size = F(This->moteur).get_size(This->moteur, Xc_ROW, orig_rows++);
  offset += size; 
  if (offset >= This->page_window_height)
  {
   This->row_page_mark[nb_rows] = TRUE;
   offset = size;
  }
  else
   This->row_page_mark[nb_rows] = FALSE;
 }
}

/* ----------------------------------------------------------------- ** 
** CELL INFO FUNCTION                                                ** 
** ----------------------------------------------------------------- */
static void calcul_cell_overrun_info(This, region)
c_Feuille	*This;
region_t	*region;
{
 int	      i, j, io, jo, col, row, bth, bth2;
 coord_t     length, max_width, max_height;
 cell	      *cell_s;
 cell_info_t *cell_info, *cell_info2, cell_orig;
 scale_t     factor;
 int	      cell_offset1, cell_offset2, cell_offset3;
 int	      cell_size1, cell_size2, cell_size3;
 CellStyle_t *cstyle;
  
 factor = This->sFactor * COORD_SCALEVALUE / COORD_DEFAULT;
 max_width = DEFAULT_COL_SIZE * 20 * factor;
 max_height = DEFAULT_ROW_SIZE * 40 * factor;
 cell_offset1 = offsetof(cell_info_t, left_border);
 cell_offset2 = offsetof(cell_info_t, cell_over_left);
 cell_offset3 = offsetof(cell_info_t, cell_over_right);
  
 cell_size1 = cell_offset2 - cell_offset1;
 cell_size2 = cell_offset3 - cell_offset2;
 cell_size3 = sizeof(cell_info_t) - cell_offset3;
  
 for(i = 0; i < This->nb_column; i++)
 {
  for(j = 0; j < This->nb_row; j++)
  {
   io = i + This->column_orig;
   jo = j + This->row_orig;
   cell_info = &This->cell_info[i][j];
      
   memcpy(&cell_orig, cell_info, sizeof(cell_info_t)); 
   memset((char *)cell_info + cell_offset2, 0, cell_size2);
   cell_info->modified = FALSE; 
      
   cell_s = cell_info->cellule;
   if (cell_s->type != Xq_VOID)
   {
    if (F(This->moteur).test_cell_overrun(This->moteur, 
					  cell_s, cell_info->cstyle,
					  io, jo, OVERRUN_HORIZONTALY, 
					  OVERRUN_POSITIVE, factor))
    {
     cell_info->cell_over_left = cell_s;
     cell_info->offset_over_left = 
      cell_s->overrun_size + cell_s->overrun_pos;
     cell_info->col_over_left = io;
    }	
    if (F(This->moteur).test_cell_overrun(This->moteur, 
					  cell_s, cell_info->cstyle,
					  io, jo, OVERRUN_VERTICALY, 
					  OVERRUN_POSITIVE, factor))
    {
     cell_info->cell_over_up = cell_s;
     cell_info->offset_over_up = 
      cell_s->overrun_size + cell_s->overrun_pos;
     cell_info->row_over_up = jo;
    }	
    continue;
   }
   if (i == 0)
   {
    col = io - 1; length = 0;
    while(1)
    {
     if (col < 0) break;
     if (length > (max_width << 1)) break;
     cell_s = NULL;
     if (F(This->moteur).test_overrun(This->moteur, &cell_s, col, jo, 
				      OVERRUN_HORIZONTALY, 
				      OVERRUN_POSITIVE, factor))
     {
      if (cell_s == NULL) break;
      if (cell_s->overrun_pos < length) break;
      cell_info->cell_over_left = cell_s;
      cell_info->offset_over_left = cell_s->overrun_pos - length;
      cell_info->col_over_left = col;
      break;
     }
     else
      if (cell_s && cell_s->type != Xq_VOID)
       break;
     length += (F(This->moteur).get_size(This->moteur, Xc_COLUMN, col)
		* factor);
     col--;
    }/* while(1) */
   }/* if (i == This->column_orig) */
   else
   {
    cell_info2 = &This->cell_info[i-1][j];
    if (cell_info2->cell_over_left)
    {
     length = SCALE_FROM_POINTS(This->column_width[i - 1]);
     if (cell_info2->offset_over_left > length)
     {
      cell_info->cell_over_left = cell_info2->cell_over_left;
      cell_info->offset_over_left = cell_info2->offset_over_left-length;
      cell_info->col_over_left = cell_info2->col_over_left;
     }
    }
   }
   if (j == 0)
   {
    row = jo - 1; length = 0;
    while(1)
    {
     if (row < 0) break;
     if (length > max_height) break;
     cell_s = NULL;
     if (F(This->moteur).test_overrun(This->moteur, &cell_s, io, row, 
				      OVERRUN_VERTICALY, 
				      OVERRUN_POSITIVE, factor))
     {
      if (cell_s == NULL) break;
      if (cell_s->overrun_pos < length) break;
      cell_info->cell_over_up = cell_s;
      cell_info->offset_over_up = cell_s->overrun_pos - length;
      cell_info->row_over_up = row;
      break;
     }
     else
      if (cell_s && cell_s->type != Xq_VOID)
       break;
     length += (F(This->moteur).get_size(This->moteur, Xc_ROW, row)
		* factor);
     row--;
    }/* while(1) */
   }/* if (j == This->row_orig) */
   else
   {
    cell_info2 = &This->cell_info[i][j-1];
    if (cell_info2->cell_over_up)
    {
     length = SCALE_FROM_POINTS(This->row_height[j-1]);
     if (cell_info2->offset_over_up > length)
     {
      cell_info->cell_over_up = cell_info2->cell_over_up;
      cell_info->offset_over_up = cell_info2->offset_over_up - length;
      cell_info->row_over_up = cell_info2->row_over_up;
     }
    }
   }
   if (!cell_info->modified)
   {
    if (memcmp( ((char *)&cell_orig)+cell_offset2, 
	       (char *)cell_info+cell_offset2, cell_size2) != 0)
     cell_info->modified = TRUE;
    else
    {
     register region_t *lregion;
	  
     lregion = region;
     while(lregion)
     {
      if ((cell_info->cell_over_left &&
	   cell_info->col_over_left >= lregion->x_min && 
	   cell_info->col_over_left <= lregion->x_max && 
	   jo >= lregion->y_min && jo <= lregion->y_max ) ||
	  (cell_info->cell_over_up &&
	   cell_info->row_over_up >= lregion->y_min && 
	   cell_info->row_over_up <= lregion->y_max &&
	   io >= lregion->x_min && io <= lregion->x_max))
      {
       cell_info->modified = TRUE;
       break;
      }
      lregion = lregion->NextRegion;
     }
    }
   }
  }/* for(j=... */
 }/* for(i=... */
 for(i = This->nb_column-1; i >= 0; i--)
 {
  for(j = This->nb_row-1; j >= 0; j--)
  {
   io = i + This->column_orig;
   jo = j + This->row_orig;
   cell_info = &This->cell_info[i][j];
	  
   memcpy(&cell_orig, cell_info, sizeof(cell_info_t)); 
   memset((char *)cell_info + cell_offset3, 0, cell_size3);
      
   cell_s = cell_info->cellule;
   if (cell_s->type != Xq_VOID)
   {
    if (F(This->moteur).test_cell_overrun(This->moteur, 
					  cell_s, cell_info->cstyle,
					  io,jo,OVERRUN_HORIZONTALY, 
					  OVERRUN_NEGATIVE, factor))
    {
     cell_info->cell_over_right = cell_s;
     cell_info->offset_over_right = 
      cell_s->overrun_size + cell_s->overrun_neg;
     cell_info->col_over_right = io;
    }	
    if (F(This->moteur).test_cell_overrun(This->moteur, 
					  cell_s, cell_info->cstyle,
					  io, jo, OVERRUN_VERTICALY, 
					  OVERRUN_NEGATIVE, factor))
    {
     cell_info->cell_over_down = cell_s;
     cell_info->offset_over_down = 
      cell_s->overrun_size + cell_s->overrun_neg;
     cell_info->row_over_down = jo;
    }	
    continue;
   }
   if (i == This->nb_column-1)
   {
    col = io + 1; length = 0;
    while(1)
    {
     if (col > COLUMN_MAX) break;
     if (length > max_width) break;
     cell_s = NULL;
     if (F(This->moteur).test_overrun(This->moteur, 
				      &cell_s, col, jo, 
				      OVERRUN_HORIZONTALY, 
				      OVERRUN_NEGATIVE, factor))
     {
      if (cell_s == NULL) break;
      if (cell_s->overrun_neg < length) break;
      cell_info->cell_over_right = cell_s;
      cell_info->offset_over_right = 
       cell_s->overrun_neg - length;
      cell_info->col_over_right = col;
      break;
     }
     else
      if (cell_s && cell_s->type != Xq_VOID)
       break;
     length += (F(This->moteur).get_size(This->moteur, Xc_COLUMN, col) 
		* factor);
     col++;
    }/* while(1) */
   }/* if (i == This->column_orig) */
   else
   {
    cell_info2 = &This->cell_info[i+1][j];
    if (cell_info2->cell_over_right)
    {
     length = SCALE_FROM_POINTS(This->column_width[i + 1]);
     if (cell_info2->offset_over_right > length)
     {
      cell_info->cell_over_right = cell_info2->cell_over_right;
      cell_info->offset_over_right = 
       cell_info2->offset_over_right-length;
      cell_info->col_over_right = cell_info2->col_over_right;
     }
    }
   }
   if (j == This->nb_row - 1)
   {
    row = jo + 1; length = 0;
    while(1)
    {
     if (row > ROW_MAX) break;
     if (length > max_height) break;
     cell_s = NULL;
     if (F(This->moteur).test_overrun(This->moteur, 
				      &cell_s, io, row, 
				      OVERRUN_VERTICALY, 
				      OVERRUN_NEGATIVE, factor))
     {
      if (cell_s == NULL) break;
      if (cell_s->overrun_neg < length) break;
      cell_info->cell_over_down = cell_s;
      cell_info->offset_over_down = cell_s->overrun_neg - length;
      cell_info->row_over_down = row;
      break;
     }
     else
      if (cell_s && cell_s->type != Xq_VOID)
       break;
     length += 
      (F(This->moteur).get_size(This->moteur, Xc_ROW, row)
       * factor);
     row++;
    }/* while(1) */
   }/* if (j == This->row_orig) */
   else
   {
    cell_info2 = &This->cell_info[i][j+1];
    if (cell_info2->cell_over_down)
    {
     length = SCALE_FROM_POINTS(This->row_height[j + 1]);
     if (cell_info2->offset_over_down > length)
     {
      cell_info->cell_over_down = cell_info2->cell_over_down;
      cell_info->offset_over_down = 
       cell_info2->offset_over_down - length;
      cell_info->row_over_down = cell_info2->row_over_down;
     }
    }
   }
   if (!cell_info->modified)
   {
    if (memcmp( ((char *)&cell_orig) + cell_offset3, 
	       (char *)cell_info+cell_offset3, cell_size3) != 0)
     cell_info->modified = TRUE;
    else
    {
     register region_t *lregion;
	  
     lregion = region;
     while(lregion)
     {
      if ((cell_info->cell_over_right &&
	   cell_info->col_over_right >= lregion->x_min && 
	   cell_info->col_over_right <= lregion->x_max && 
	   jo >= lregion->y_min && jo <= lregion->y_max) ||
	  (cell_info->cell_over_down &&
	   cell_info->row_over_down >= lregion->y_min && 
	   cell_info->row_over_down <= lregion->y_max &&
	   io >= lregion->x_min && io <= lregion->x_max))
      {
       cell_info->modified = TRUE;
       break;
      }
      lregion = lregion->NextRegion;
     }
    }
   }
  }/* for(j=... */
 }/* for(i=... */
  
 /* border compute */
 for(i = 0; i < This->nb_column; i++)
 {
  for(j = 0; j < This->nb_row; j++)
  {
   io = i + This->column_orig;
   jo = j + This->row_orig;
      
   cell_info = &This->cell_info[i][j];
   memcpy(&cell_orig, cell_info, sizeof(cell_info_t)); 
   memset((char *)cell_info + cell_offset1, 0, cell_size1);
      
   cstyle = cell_info->cstyle;
      
   if (i > 0)
   {
    if ((!cell_info->cell_over_left || cell_info->col_over_left == io) &&
	(!This->cell_info[i-1][j].cell_over_right ||
	 This->cell_info[i-1][j].col_over_right == io-1))
    {
     bth = cstyle->left.border_thickness;
     bth2 = This->cell_info[i-1][j].cstyle->right.border_thickness;
     if (bth && bth > bth2)
      cell_info->left_to_show = TRUE;
     cell_info->left_border = MAX(bth, bth2);
    }
   }
      
   if (i < This->nb_column-1)
   {
    if ((!cell_info->cell_over_right || cell_info->col_over_right == io) &&
	(!This->cell_info[i+1][j].cell_over_left ||
	 This->cell_info[i+1][j].col_over_left == io+1))
    { 
     bth = cstyle->right.border_thickness;
     bth2 = This->cell_info[i+1][j].cstyle->left.border_thickness;
     if (bth && bth >= bth2)
      cell_info->right_to_show = TRUE;
     cell_info->right_border = MAX(bth, bth2);
    }
   }
      
   if (j > 0)
   {
    if ((!cell_info->cell_over_up || cell_info->row_over_up == jo) &&
	(!This->cell_info[i][j-1].cell_over_down ||
	 This->cell_info[i][j-1].row_over_down == jo-1))
    { 
     bth = cstyle->up.border_thickness;
     bth2 = This->cell_info[i][j-1].cstyle->down.border_thickness;
     if (bth && bth>bth2)
      cell_info->up_to_show = TRUE;
     cell_info->up_border = MAX(bth, bth2);
    }
   }
      
   if (j < This->nb_row-1)
   {
    if ((!cell_info->cell_over_down || cell_info->row_over_down == jo) &&
	(!This->cell_info[i][j+1].cell_over_up || 
	 This->cell_info[i][j+1].row_over_up == jo+1))
    { 
     bth = cstyle->down.border_thickness;
     bth2 = This->cell_info[i][j+1].cstyle->up.border_thickness;
     if (bth && bth>=bth2)
      cell_info->down_to_show = TRUE;
     cell_info->down_border = MAX(bth, bth2);
    }
   }
      
   if (!cell_info->modified && 
       (memcmp( ((char *)&cell_orig) + cell_offset1, 
	       (char *)cell_info+cell_offset1, cell_size1) != 0))
    cell_info->modified = TRUE;
  }/* for(j=... */
 }/* for(i=... */
  
 /*  for(i=0; i<This->nb_row; i++)
     {
     printf("row: %d ",i);
     for(j=0; j<This->nb_column; j++)
     {
     printf("%c", This->cell_info[j][i].modified?'M':'.');
     }
     printf("\n");
     } */
}

void allocate_cell_info(This)
c_Feuille	*This;
{ 
 int	i,j,io,jo;
 region_t *region;
 region_t fregion;
  
 if ((This->nb_row != This->nb_row_ci) ||
     (This->nb_column != This->nb_column_ci))
 {
  if (This->nb_row_ci && This->nb_column_ci)
  {
   for(i=0; i<This->nb_column_ci; i++)
   {
    for(j=0; j<This->nb_row_ci; j++)
    {
     if (This->cell_info[i][j].clip_type & XcCTM_CLIPMASK)
      XFreePixmap(_display, This->cell_info[i][j].clip_mask);
    }
   }
   if (This->nb_column > This->nb_column_ci)
   {
    This->cell_info = 
     (cell_info_t **)Xc_realloc("cell_info**", 
				This->cell_info,
				sizeof(cell_info_t *) * 
				This->nb_column);
    for(i=0; i<This->nb_column_ci; i++)
    {
     This->cell_info[i] = 
      (cell_info_t *)Xc_realloc("cell_info*", 
				This->cell_info[i],
				sizeof(cell_info_t) * 
				This->nb_row);
     if (This->nb_row > This->nb_row_ci)
      memset(This->cell_info[i] + This->nb_row_ci, 0,
	     sizeof(cell_info_t) *
	     (This->nb_row- This->nb_row_ci));
    }
    for(i=This->nb_column_ci; i<This->nb_column; i++)
    {
     This->cell_info[i] = 
      (cell_info_t *)Xc_malloc
       ("cell_info*", sizeof(cell_info_t)*This->nb_row);
     memset(This->cell_info[i], 0,
	    sizeof(cell_info_t) * This->nb_row);
    }
   }
   else
   {
    for(i = 0; i < This->nb_column; i++)
    {
     This->cell_info[i] = 
      (cell_info_t *)Xc_realloc("cell_info*", 
				This->cell_info[i],
				sizeof(cell_info_t) * 
				This->nb_row);
     if (This->nb_row > This->nb_row_ci)
      memset(This->cell_info[i] + This->nb_row_ci, 0,
	     sizeof(cell_info_t) *
	     (This->nb_row- This->nb_row_ci));
    }
    for(i = This->nb_column; i < This->nb_column_ci; i++)
     Xc_free(This->cell_info[i]);
    This->cell_info = 
     (cell_info_t **)Xc_realloc("cell_info**", 
				This->cell_info,
				sizeof(cell_info_t *) * 
				This->nb_column);
   }
   This->column_info = 
    (int *)Xc_realloc("col info", 
		      This->column_info,
		      sizeof(int)*This->nb_column);
   This->row_info = 
    (int *)Xc_realloc("row info", 
		      This->row_info,
		      sizeof(int)*This->nb_row);
  }
  else
  {
   This->cell_info = (cell_info_t **)Xc_malloc
    ("cell_info**", sizeof(cell_info_t *)*This->nb_column);
   for(i=0; i<This->nb_column; i++)
   {
    This->cell_info[i] = (cell_info_t *)Xc_malloc
     ("cell_info*", sizeof(cell_info_t)*This->nb_row);
    memset(This->cell_info[i], 0,
	   sizeof(cell_info_t) * This->nb_row);
   }
   This->column_info = (int *)Xc_malloc("col info", 
					sizeof(int)*This->nb_column);
   This->row_info = (int *)Xc_malloc("row info", 
				     sizeof(int)*This->nb_row);
  }
  This->nb_column_ci = This->nb_column;
  This->nb_row_ci = This->nb_row;
 }
 else
 {
  for(i=0; i<This->nb_column_ci; i++)
  {
   for(j=0; j<This->nb_row_ci; j++)
   {
    if (This->cell_info[i][j].clip_type & XcCTM_CLIPMASK)
    {
     XFreePixmap(_display, This->cell_info[i][j].clip_mask);
    }
   }
  }
 }
 /*  for(i=0; i<This->nb_column; i++)
     memset(This->cell_info[i],0,sizeof(cell_info_t)*This->nb_row); */
 memset(This->column_info, 0, sizeof(int)*This->nb_column);
 memset(This->row_info, 0, sizeof(int)*This->nb_row);
  
 This->origin_info = is_selected_def(This,0,0,Xq_ORIGIN);
 
 if (This->select_type)
 {
  for(i=0; i<This->nb_column; i++)
  {
   io = i + This->column_orig;
   region = This->col_region;
   while(region)
   {
    if (io>=region->x_min && io<=region->x_max)
     This->column_info[i] += 1;
    region = region->NextRegion;
   }
  }
    
  for(i=0; i<This->nb_row; i++)
  {
   io = i + This->row_orig;
   region = This->row_region;
   while(region)
   {
    if (io>=region->y_min && io<=region->y_max)
     This->row_info[i] += 1;
    region = region->NextRegion;
   }
  }
    
  for(io=This->column_orig, i=0; i<This->nb_column; io++, i++)
  {
   for(jo=This->row_orig, j=0; j<This->nb_row; jo++, j++)
   {
    This->cell_info[i][j].clip_type = XcCT_NO_CLIPMASK_NO_REDRAW;
    This->cell_info[i][j].selected = 0;
    region = This->cell_region;
    while(region)
    {
     if (io>=region->x_min && io<=region->x_max
	 && jo>=region->y_min && jo<=region->y_max)
      This->cell_info[i][j].selected++;
     region = region->NextRegion;
    }
    This->cell_info[i][j].sequence = This->sequence;
   }
  }
  fregion.x_min = fregion.x_max = This->column_orig;
  fregion.x_max += This->nb_column;
  fregion.y_min = fregion.y_max = This->row_orig;
  fregion.y_max += This->nb_row;
  F(This->moteur).fill_cell_info(This->moteur, This->cell_info, &fregion,
				 This->column_orig, This->row_orig);
    
 }
 else
 {
  for(io=This->column_orig, i=0; i<This->nb_column; io++, i++)
  {
   for(jo=This->row_orig, j=0; j<This->nb_row; jo++, j++)
   {
    This->cell_info[i][j].clip_type = XcCT_NO_CLIPMASK_NO_REDRAW;
    This->cell_info[i][j].selected = 0;
    This->cell_info[i][j].sequence = This->sequence;
   }   
  }
  fregion.x_min = fregion.x_max = This->column_orig;
  fregion.x_max += This->nb_column;
  fregion.y_min = fregion.y_max = This->row_orig;
  fregion.y_max += This->nb_row;
  F(This->moteur).fill_cell_info(This->moteur, This->cell_info, &fregion,
				 This->column_orig, This->row_orig);
 }
}

void calcul_frame_info(This)
c_Feuille	*This;
{
 frmg_modify_cadre_callback(This);
}

void calcul_cell_info(This)
c_Feuille	*This;
{
 allocate_cell_info(This);
  
 calcul_cell_overrun_info(This, NULL);
  
 calcul_frame_info(This);
}

void set_region_selected(This, region, type)
c_Feuille	*This;
region_t	*region;
int		type;
{
 int	i,j;
  
 switch(type)
 {
 case Xq_CELL:
  for(i=MAX(This->column_orig,region->x_min);
      i<=MIN(This->column_orig+This->nb_column,region->x_max); i++)
  {
   for(j=MAX(This->row_orig,region->y_min);
       j<=MIN(This->row_orig+This->nb_row,region->y_max); j++)
   {
    set_cell_selected(This, i, j, type);
   }
  }
  break;
 case Xq_COLUMN:
  for(i=MAX(This->column_orig,region->x_min);
      i<=MIN(This->column_orig+This->nb_column,region->x_max); i++)
  {
   set_cell_selected(This, i, 0, type);
  }
  break;
 case Xq_ROW:
  for(i=MAX(This->row_orig,region->y_min);
      i<=MIN(This->row_orig+This->nb_row,region->y_max); i++)
  {
   set_cell_selected(This, 0, i, type);
  }
  break;
 case Xq_ORIGIN:		/* useless */
  break;
 }
}

void unset_region_selected(This, region, type)
c_Feuille	*This;
region_t	*region;
int		type;
{
 int	i,j;
  
 switch(type)
 {
 case Xq_CELL:
  for(i=MAX(This->column_orig,region->x_min);
      i<=MIN(This->column_orig+This->nb_column,region->x_max); i++)
  {
   for(j=MAX(This->row_orig,region->y_min);
       j<=MIN(This->row_orig+This->nb_row,region->y_max); j++)
   {
    unset_cell_selected(This, i, j, type);
   }
  }
  break;
 case Xq_COLUMN:
  for(i=MAX(This->column_orig,region->x_min);
      i<=MIN(This->column_orig+This->nb_column,region->x_max); i++)
  {
   unset_cell_selected(This, i, 0, type);
  }
  break;
 case Xq_ROW:
  for(i=MAX(This->row_orig,region->y_min);
      i<=MIN(This->row_orig+This->nb_row,region->y_max); i++)
  {
   unset_cell_selected(This, 0, i, type);
  }
  break;
 case Xq_ORIGIN:		/* useless */
  break;
 }
}

static void set_cell_selected(This, column, row, type)
c_Feuille	*This;
int		column;
int		row;
int		type;
{
 if (F(This).IsVisible(This, column, row, type))
 {
  column -= This->column_orig;
  row -= This->row_orig;
  switch(type)
  {
  case Xq_CELL:
   This->cell_info[column][row].selected++;
   break;
  case Xq_COLUMN:
   This->column_info[column] += 1;
   break;
  case Xq_ROW:
   This->row_info[row] += 1;
   break;
  case Xq_ORIGIN:
   This->origin_info++;
   break;
  }
 }
}

static void unset_cell_selected(This, column, row, type)
c_Feuille	*This;
int		column;
int		row;
int		type;
{
 if (F(This).IsVisible(This, column, row, type))
 {
  column -= This->column_orig;
  row -= This->row_orig;
  switch(type)
  {
  case Xq_CELL:
   if (This->cell_info[column][row].selected>0)
    This->cell_info[column][row].selected--;
   break;
  case Xq_COLUMN:
   if (This->column_info[column] > 0)
    This->column_info[column] -= 1;
   break;
  case Xq_ROW:
   if (This->row_info[row] > 0)
    This->row_info[row] -= 1;
   break;
  case Xq_ORIGIN:
   if (This->origin_info > 0)
    This->origin_info --;
   break;
  }
 }
}


/* ----------------------------------------------------------------- ** 
**  GET COLUMN/ROW POSITION                                          ** 
** ----------------------------------------------------------------- */

int get_column_pos(This, column)
c_Feuille *This;
int	column;
{
 int		i,j;
 int		col_pos;
  
 col_pos = 0;
 if (column < 0 || column<This->column_orig)
  return -1;
 if (column >=This->column_orig + This->nb_column)
  return This->sheet_width;
  
 j = 0 ;
 i = This->column_orig;
 while(i++!=column)
 {
  col_pos += This->column_width[j++];
 }
 return col_pos;
}

int get_row_pos(This, row)
c_Feuille	*This;
int	row;
{
 int		i;
 int		j;
 int		row_pos;
  
 row_pos = 0;
 if (row < 0 || row<This->row_orig)
  return -1;
 if (row>ROW_MAX || row >=This->row_orig + This->nb_row)
  return This->sheet_height;
  
 j = 0;
 i=This->row_orig;
 while(i++!=row)
 {
  row_pos += This->row_height[j++];
 }
 return row_pos;
}

/*--- GET COLUMN/ROW LENGTH ---*/

int get_column_width(This, column)
c_Feuille *This;
int	column;
{
  
 if (column < 0 || column>COLUMN_MAX || column<This->column_orig ||
     column >=This->column_orig + This->nb_column)
  return 0;
 return This->column_width[column-This->column_orig];
}

int get_row_height(This, row)
c_Feuille *This;
int	row;
{
 if (row < 0 || row>ROW_MAX || row<This->row_orig ||
     row >=This->row_orig + This->nb_row)
  return 0;
 return This->row_height[row-This->row_orig];
}

/* ----------------------------------------------------------------- ** 
** Region Functions                                                  ** 
** ----------------------------------------------------------------- */

void add_region_in_list(This, region, type)
c_Feuille	*This;
region_t	*region;
int		type;
{
 region_t	*reglist;
 int		*nb_region;
 region_t	**base;
  
 switch(type)
 {
 case Xq_CELL:
  nb_region = &This->nb_cell_region;
  base = &This->cell_region;
  This->last_advance = region;
  break;
 case Xq_COLUMN:
  nb_region = &This->nb_col_region;
  base = &This->col_region;
  break;
 case Xq_ROW:
  nb_region = &This->nb_row_region;
  base = &This->row_region;
  break;
 case Xq_ORIGIN:
 default:
  Xc_WARNING(("unproper use of add_region_in_list"));
  return;
  break;
 }
  
 if (!(*base))
 {
  *base=region;
 }
 else
 {
  reglist= *base;
  while(reglist->NextRegion)
   reglist=reglist->NextRegion;
  reglist->NextRegion=region;
 }
  
  
 *nb_region = *nb_region + 1;
}

void remove_region_from_list(This, region, type)
c_Feuille	*This;
region_t	*region;
int		type;
{
 region_t	*reglist;
 int		*nb_region;
 region_t	**base;
  
 switch(type)
 {
 case Xq_CELL:
  nb_region = &This->nb_cell_region;
  base = &This->cell_region;
  break;
 case Xq_COLUMN:
  nb_region = &This->nb_col_region;
  base = &This->col_region;
  break;
 case Xq_ROW:
  nb_region = &This->nb_row_region;
  base = &This->row_region;
  break;
 case Xq_ORIGIN:
 default:
  Xc_WARNING(("unproper use of add_region_in_list"));
  return;
  break;
 }
  
 reglist= *base;
 if (!reglist)
  return;
 if (reglist == region)
 {
  *base = reglist->NextRegion;
  *nb_region = *nb_region - 1;
 }
 else
 {
  while(reglist->NextRegion && reglist->NextRegion!=region)
   reglist = reglist->NextRegion;
  if (reglist->NextRegion)
  {
   reglist->NextRegion = reglist->NextRegion->NextRegion;
   *nb_region = *nb_region - 1;
  }
 }
}

void focus_in_region(This, pregion, type)
c_Feuille *This;
region_t  *pregion;
int	  type;
{
 region_t	*region;

 switch((int)pregion)
 {
 case (int)NONE_REGION:
  return;
 case (int)ALL_REGION:
  region = NULL;
  switch(type)
  {
  case Xq_CELL:
   region = This->cell_region;
   break;
  case Xq_COLUMN:
   region = This->col_region;
   break;
  case Xq_ROW:
   region = This->row_region;
   break;
  case Xq_ORIGIN:
   if (This->origin_selected)
   {
    focus_in_region_def(This, NULL, Xq_ORIGIN);
    return;
   }
   break;
  }
  while(region)
  {
   focus_in_region_def(This, region, type);
   region = region->NextRegion;
  }
  break;
 default:
  focus_in_region_def(This, pregion, type);
 }
}

static void focus_in_region_def(This, region, type)
c_Feuille *This;
region_t  *region;
int	  type;
{
 int	i,j;
 boolean select;
  
 switch(type)
 {
 case Xq_CELL:
  F(This->frmg).start_draw_cache(This->frmg);
  for(i = MAX(region->x_min, This->column_orig);
      i <= MIN(region->x_max, This->column_orig+This->nb_column-1); i++)
  {
   for(j = MAX(region->y_min, This->row_orig);
       j <= MIN(region->y_max, This->row_orig+This->nb_row-1); j++)
   {
    select = F(This).IsSelected(This, i, j, Xq_CELL);
    set_cell_selected(This, i, j, Xq_CELL);
    if (!select)
     focus_in_cellule(This, i, j);
   }
  }
  F(This->frmg).flush_draw_cache(This->frmg);
  break;
 case Xq_COLUMN:
  for(i = MAX(region->x_min, This->column_orig-1);
      i <= MIN(region->x_max, This->column_orig+This->nb_column-1); i++)
  {
   select = F(This).IsSelected(This, i, 0, Xq_COLUMN);
   set_cell_selected(This, i, 0, Xq_COLUMN);
   if (!select)
    focus_in_column(This, i);
  }
  break;
 case Xq_ROW:
  for(i = MAX(region->y_min, This->row_orig);
      i <= MIN(region->y_max, This->row_orig+This->nb_row-1); i++)
  {
   select = F(This).IsSelected(This, 0, i, Xq_ROW);
   set_cell_selected(This, 0, i, Xq_ROW);
   if (!select)
    focus_in_row(This, i);
  }
  break;
 case Xq_ORIGIN:
  select = F(This).IsSelected(This, 0, 0, Xq_ORIGIN);
  set_cell_selected(This, 0, 0, Xq_ORIGIN);
  if (!select)
   focus_in_origin(This);
  break;
 }
}

void focus_out_region(This, pregion, type)
c_Feuille *This;
region_t  *pregion;
int	  type;
{
 region_t	*region;

 switch((long)pregion)
 {
 case (int)NONE_REGION:
  return;
 case (int)ALL_REGION:
  region = NULL;
  switch(type)
  {
  case Xq_CELL:
   region = This->cell_region;
   break;
  case Xq_COLUMN:
   region = This->col_region;
   break;
  case Xq_ROW:
   region = This->row_region;
   break;
  case Xq_ORIGIN:
   focus_out_region_def(This, NULL, Xq_ORIGIN);
   return;
   break;
  }
  while(region)
  {
   focus_out_region_def(This, region, type);
   region = region->NextRegion;
  }
  break;
 default:
  focus_out_region_def(This, pregion, type);
 }
}

static void focus_out_region_def(This, region, type)
c_Feuille *This;
region_t  *region;
int	  type;
{
 int	i,j;
 boolean select;
  
 switch(type)
 {
 case Xq_CELL:
  F(This->frmg).start_draw_cache(This->frmg);
  for(i = MAX(region->x_min, This->column_orig);
      i <= MIN(region->x_max, This->column_orig+This->nb_column-1); i++)
  {
   for(j=MAX(region->y_min, This->row_orig);
       j<=MIN(region->y_max, This->row_orig+This->nb_row); j++)
   {
    select = F(This).IsSelected(This, i, j, Xq_CELL);
    unset_cell_selected(This, i, j, Xq_CELL);
    if (select && !F(This).IsSelected(This, i, j, Xq_CELL))
     focus_in_cellule(This, i, j);
   }
  }
  F(This->frmg).flush_draw_cache(This->frmg);
  break;
 case Xq_COLUMN:
  for(i = MAX(region->x_min, This->column_orig);
      i <= MIN(region->x_max, This->column_orig+This->nb_column-1); i++)
  {
   select = F(This).IsSelected(This, i, 0, Xq_COLUMN);
   unset_cell_selected(This, i, 0, Xq_COLUMN);
   if (select && !F(This).IsSelected(This, i, 0, Xq_COLUMN))
    focus_in_column(This, i);
  }
  break;
 case Xq_ROW:
  for(i = MAX(region->y_min, This->row_orig);
      i <= MIN(region->y_max, This->row_orig+This->nb_row-1); i++)
  {
   select = F(This).IsSelected(This, 0, i, Xq_ROW);
   unset_cell_selected(This, 0, i, Xq_ROW);
   if (select && !F(This).IsSelected(This, 0, i, Xq_ROW))
    focus_in_row(This, i);
  }
  break;
 case Xq_ORIGIN:
  select = F(This).IsSelected(This, 0, 0, Xq_ORIGIN);
  unset_cell_selected(This, 0, 0, Xq_ORIGIN);
  if (select && !F(This).IsSelected(This, 0, 0, Xq_ORIGIN))
   focus_in_origin(This);
  break;
 }
}

/* ----------------------------------------------------------------- **
** frame manager callbacks                                           ** 
** ----------------------------------------------------------------- */

static void frmg_select_callback(This)
c_Feuille	*This;
{
 Xc_TRACE(("frmg_select_callback"));
 if (This->select_type)
 {
  GlobEditBar->mode_sel = MODE_MOVE;
  F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar);
  focus_out_region(This, ALL_REGION, Xq_ORIGIN);
  focus_out_region(This, ALL_REGION, Xq_COLUMN);
  focus_out_region(This, ALL_REGION, Xq_ROW);
  focus_out_region(This, ALL_REGION, Xq_CELL);
  if (F(This).IsVisible(This, This->cell_x, This->cell_y, Xq_CELL))
   focus_out_active_cellule(This, This->cell_x, This->cell_y);
  This->select_type = FALSE;
 }
}

static void frmg_deselect_callback(This)
c_Feuille	*This;
{
 Xc_TRACE(("frmg_deselect_callback"));
 if (!This->select_type)
 {
  focus_in_region(This, ALL_REGION, Xq_ORIGIN);
  focus_in_region(This, ALL_REGION, Xq_COLUMN);
  focus_in_region(This, ALL_REGION, Xq_ROW);
  focus_in_region(This, ALL_REGION, Xq_CELL);
  if (F(This).IsVisible(This, This->cell_x, This->cell_y, Xq_CELL))
   focus_in_active_cellule(This, This->cell_x, This->cell_y);
  This->select_type = TRUE;
 }
}

static void frmg_redraw_polygone_callback(This, polygone)
c_Feuille	*This;
Polygone	*polygone;
{
 int	i,j;
 int	x, y, w, h;
  
 if (!polygone->memory)
  return;
 x = 0;
 allocate_cell_info(This);
 calcul_frame_info(This);
 This->sequence++;
 F(This->frmg).start_draw_cache(This->frmg);
 for(i=0; i<This->nb_column_ci; i++)
 {
  w = This->column_width[i];
  y = 0;
  for(j=0; j<This->nb_row_ci; j++)
  {
   h = This->row_height[j];
   if (IsRectIntersPolygone(polygone, x, y, w, h))
   {
    draw_cellule(This, i+This->column_orig, j+This->row_orig,
		 x, y, w, h);
   }
   y += h;
  }
  x += w;
 }
 F(This->frmg).flush_draw_cache(This->frmg);
 /*Xc_TRACE(("frmg_redraw_polygone_callback"));*/
}

static void frmg_modify_cadre_callback(This)
c_Feuille	*This;
{
 l_Cadre	*lcadre;
 Polygone	*forme;
 Polygone	*forme2;
 Polygone	*clip_polyg;
 int		i, j;
 int		x, y, w, h;
 int		x2, y2, w2, h2;
 int		lb, ub;
 cell_info_t	*cell_info;
  
 lcadre = This->frmg->ListCadre;
 forme = forme2 = NULL;
 while(lcadre)
 {
  if (lcadre->cadre->inside_color->transparency)
  {
   if (forme2)
    UnionPolygone(&forme2, lcadre->cadre->Forme);
   else
    forme2 = DuplicatePolygone(lcadre->cadre->Forme);
  }
  else
  {
   if (forme)
    UnionPolygone(&forme, lcadre->cadre->Forme);
   else
    forme = DuplicatePolygone(lcadre->cadre->Forme);
  }
  lcadre = lcadre->NextCadre;
 }
 
 if (forme2 && forme)
  SubtractPolygone(&forme2, forme);
 x = 0;
 for(i=0; i<This->nb_column_ci; i++)
 {
  w = This->column_width[i];
    
  if (w <= 0)
  {
   for(j=0; j<This->nb_row_ci; j++)
   {
    cell_info = &(This->cell_info[i][j]);
	    
    if (cell_info->clip_type & XcCTM_CLIPMASK)
     XFreePixmap(_display, cell_info->clip_mask);
	    
    cell_info->clip_type = XcCT_NO_CLIPMASK_NO_REDRAW;
   }
   continue;
  }
    
  y = 0;
  for(j=0; j<This->nb_row_ci; j++)
  {
   h = This->row_height[j];
   cell_info = &(This->cell_info[i][j]);
      
   if (cell_info->clip_type & XcCTM_CLIPMASK)
    XFreePixmap(_display, cell_info->clip_mask);

   cell_info->clip_type = XcCT_NO_CLIPMASK_NO_REDRAW;
      
   if (h <= 0)
    continue;
      
   x2 = x; y2=y; w2=w; h2=h;
   if (cell_info->left_to_show)
   {
    lb = ABS(cell_info->left_border+2)>>1;
    x2 -= lb;
    w2 += lb;
   }
   if (cell_info->right_to_show)
    w2 += (cell_info->right_border-1)>>1;
   if (cell_info->up_to_show)
   {
    ub = ABS(cell_info->up_border+2)>>1;
    y2 -= ub;
    h2 += ub;
   }
   if (cell_info->down_to_show)
    h2 += (cell_info->down_border-1)>>1;
            
   if (forme2 && IsRectIntersPolygone(forme2, x2, y2, w2, h2))
   {
    cell_info->clip_type |= XcCTM_REDRAW;
   }
   if (forme && IsRectIntersPolygone(forme, x2, y2, w2, h2))
   {
    clip_polyg = SubtractRectPolygone(forme, x2, y2, w2, h2);
    if (clip_polyg->memory)
    {
     cell_info->clip_type |= XcCTM_CLIPMASK;
     cell_info->clip_mask = 
      CalculRectPolygoneMask(clip_polyg, x2, y2, w2, h2,
			     _display, _window);
     cell_info->clip_origin_x = x2;
     cell_info->clip_origin_y = y2;
    }
    else
     cell_info->clip_type |= XcCT_CLIPMASK_FULL;
	  
    FreePolygone(clip_polyg, DEL_POLYGONE);
   }
   y += h;
  }
  x += w;
 }
 if (forme)
  FreePolygone(forme, DEL_POLYGONE);
 if (forme2)
  FreePolygone(forme2, DEL_POLYGONE);
 
 /*Xc_TRACE(("frmg_modify_cadre_callback"));*/
}

static void frmg_attract_point_callback(This, sx, sy, x, y)
c_Feuille	*This;
coord_t	*sx;
coord_t	*sy;
int	*x;
int	*y;
{
 int		i;
 coord_t	Width, Height;
 coord_t	mg_field;
  
 i = Width = 0;
 mg_field = SCALE_FROM_MILLIMETERS(2);
 while(i<This->nb_column)
 {
  if (ABS(*sx - Width) < mg_field)
  {
   *sx = Width;
   *x = *sx / This->sScale;
   break;
  }
  Width += (F(This->moteur).get_size(This->moteur, Xc_COLUMN, 
				     i+This->column_orig)
	    /This->sScale) * This->sScale;
  i++;
 }
 i = Height = 0;
 while(i<This->nb_row)
 {
  if (ABS(*sy - Height) < mg_field)
  {
   *sy = Height;
   *y = *sy / This->sScale;
   break;
  }
  Height += (F(This->moteur).get_size(This->moteur, Xc_ROW, i+This->row_orig)
	     /This->sScale) * This->sScale;
  i++;
 }
}

static void hook_cellstyle_callback(This, cstyle, reason)
c_Feuille *This;
CellStyle_t *cstyle;
int	reason;
{
 int	i,j;
 l_Cadre	*lcadre;
  
 Xc_TRACE(("hook cell style %p callback", cstyle));
  
 for(i=0; i<This->nb_column_ci; i++)
 {
  for(j=0; j<This->nb_row_ci; j++)
  {
   if (This->cell_info[i][j].cstyle == cstyle)
   {
    refresh_cellule(This, i+This->column_orig, j+This->row_orig);
   }
  }
 }
 if (reason == XcH_TEXT_STYLE_CHANGED)
 {
  lcadre = This->frmg->ListCadre;
  while(lcadre)
  {
   if (lcadre->cadre->object_type == CADRE_VECTOR &&
       ((c_VectorGraph *)(lcadre->cadre->object))->highg)
   {
    c_HighGraph	*highg;
    highg = ((c_VectorGraph *)(lcadre->cadre->object))->highg;
    F(highg).test_tstyle_hook(highg, cstyle->text_style);
   }
   lcadre = lcadre->NextCadre;
  }
 }
}
