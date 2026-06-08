/*
** Page.c for Xclamation in Page/
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
** Started on  Mon Jul 17 19:58:07 1995 Emmanuel Paris
** Last update Wed Nov  4 18:45:54 1998 Emmanuel Paris
*/


#include "Page.h"
#include <X11/keysym.h>

void *cons_Page();
void dest_Page();
void *copy_Page();
void init_Page ___PROTO((c_Page *this,
			 unsigned int number,
			 page_format_t *format,
			 page_type_t type,
			 page_orient_t orientation,
			 coord_t left_margin, coord_t right_margin,
			 coord_t upper_margin, coord_t lower_margin,
			 int nb_column, int nb_row,
			 coord_t column_spacing, coord_t row_spacing,
			 coord_t grid_origx, coord_t grid_origy,
			 coord_t grid_deltax, coord_t grid_deltay, int mode));

void resize_Page();
boolean set_function();
status_t eventlook_page();
static void select_page();
static void deselect_page();
void set_X_info();
error add_Cadre();
void delete_Cadre();
void list_Cadre();
void destroy_cadre();
void redisplay_cadre();

void cut_selected_cadre();
void copy_selected_cadre();
void paste_selected_cadre();

char	*copy_cadre_name();
l_Cadre *get_new_cadre();
l_Cadre *get_copy_cadre();
l_Cadre *get_cadre_by_xy();
l_Cadre *search_cadre_by_name();
l_Cadre *get_cadre_and_point_by_xy();
l_Cadre *get_cadre_and_point_by_line();
l_Point *get_handle_by_xy();
static l_Cadre	*list_cadre_by_page ___PROTO((c_Page *This,
					      int num_page,
					      coord_t *orig_x,
					      coord_t *orig_y));
void calcul_select_handle();
void change_page_origin();
void change_page_scaling();
void recoordinate_cadre();
void recoordinate_scrollbar_regle();
void init_recoordinate_point();
void recoordinate_point();
void move_cadre_to_xy();
void redraw_rectangle();
void draw_cadre_contour();
void draw_cadre();

void select_cadre();
void deselect_cadre();
void select_all_cadre();
void deselect_all_cadre();

void RectangleDisplaySelectedlCadre();
void DisplaySelectedlCadre();
void UndisplaySelectedlCadre();
void DisplaySelectedCadre();
void UndisplaySelectedCadre();
void draw_handle();
l_Cadre *get_lcadre_by_cadre();

/*
  static void display_link_textframe();
  static void display_link_arrow();
  */

void  object_redraw_cadre();
void  object_redraw_rectangle();
Polygone *object_get_clip_polygone();

void free_list_cadre();
void get_center_list_cadre();
void get_limit_list_cadre();
void add_cadre_in_list();
void delete_cadre_in_list();
l_Cadre *duplicate_list_cadre();
l_Cadre *list_cadre_inters();
void list_cadre_low_high_priority();

l_Cadre *sort_asc_list_cadre ___PROTO((l_Cadre *lcadre));
l_Cadre *sort_desc_list_cadre ___PROTO((l_Cadre *lcadre));

static boolean ps_print();
boolean readPage ___PROTO((c_DocFile *doc, char *keyword,
			   long *page_number, BaseStd_t *base));
static boolean writePage ___PROTO((c_Page *this, c_DocFile *doc,
				   boolean selected));

sf_Page fc_Page =
{
 cons_Page,
 dest_Page,
 copy_Page,
 init_Page,
 resize_Page,
 set_function,
 eventlook_page,
 select_page,
 deselect_page,
 set_X_info,
 add_Cadre,
 delete_Cadre,
 list_Cadre,
 destroy_cadre,
 redisplay_cadre,
 set_priority_Cadre,
 get_priority_Cadre,
 lower_priority,
 higher_priority,
 lowest_priority,
 highest_priority,
 delete_selected_cadre,
 delete_selected_object,
 cut_selected_cadre,
 copy_selected_cadre,
 paste_selected_cadre,
 set_lower_priority,
 set_higher_priority,
 set_lowest_priority,
 set_highest_priority,
 get_highest_priority,
 get_lowest_priority,
 zoom_page_fit_in_width,
 zoom_page_fit_in_screen,
 zoom_page_at_real_size,
 zoom_page_at_percent,
 move_point_mode,
 resize_cadre_mode,
 add_page_align_mark,
 move_page_align_mark,
 remove_page_align_mark,
 get_page_align_mark,
 lock_cadre,
 unlock_cadre,
 fusion_cadre,
 substract_cadre,
 addition_cadre,
 outline_cadre,
 align_cadre,
 copy_cadre_name,
 get_new_cadre,
 get_copy_cadre,
 get_cadre_by_xy,
 search_cadre_by_name,
 get_cadre_and_point_by_xy,
 get_cadre_and_point_by_line,
 get_handle_by_xy,
 list_cadre_by_page,
 free_list_cadre,
 duplicate_list_cadre,
 get_center_list_cadre,
 calcul_select_handle,
 change_page_origin,
 change_page_scaling,
 init_recoordinate_point,
 recoordinate_point,
 attract_point,
 move_cadre_to_xy,
 redraw_rectangle,
 draw_cadre_contour,
 draw_cadre,
 select_cadre,
 deselect_cadre,
 select_all_cadre,
 deselect_all_cadre,
 DisplaySelectedlCadre,
 UndisplaySelectedlCadre,
 ps_print,
 readPage,
 writePage
};

void *cons_Page(BaseStd)
BaseStd_t	*BaseStd;
{
 c_Page *This;

 if((This=(c_Page *)Xc_malloc("Page",sizeof (c_Page)))==NULL) return NULL;
 memset(This, 0, sizeof(c_Page));
 This->f= &fc_Page;
  
 This->X_info_calc = FALSE;
 This->BaseStd = BaseStd;
 This->sScale = COORD_DEFAULT;
 This->sFactor = SCALE_FROM_PERCENT(100);
 This->sOrig_X = This->sOrig_Y = -SCALE_FROM_MILLIMETERS(10);
 This->Orig_X = This->Orig_Y = 0;
 This->page_number = 1;
 This->page_format = &Xc_Table_Page_Format[1];
 This->page_type = Xc_PAGE_LEFT;
 This->page_orient = Xc_PORTRAIT;
 This->nb_cadre = 0;
 This->verrou = FALSE;
 This->select_type = FALSE;
 This->select_group = TRUE;
 This->select_sX_min = This->select_sY_min =
  This->select_sX_max = This->select_sY_max = 0;
 This->SelectedCadre = NULL;
 This->ListCadre = NULL;
 This->function = NULL;
 This->function_data = NULL;
 This->data_to_object.page = (void *)This;
 This->data_to_object.redraw_cadre = object_redraw_cadre;
 This->data_to_object.redraw_rectangle = object_redraw_rectangle;
 This->data_to_object.get_clip_polygone = object_get_clip_polygone;
 This->PosX = This->PosY = This->Width = This->Height = 
  This->Page_Contour.nb_page_vertical = 
   This->Page_Contour.nb_page_horizontal = 0;
 create_page_align_mark(This);
 This->Page_Grid_actif = FALSE;
 This->Page_Contour_actif = TRUE;
 This->Page_Typo_Mark_actif = TRUE;
 This->Page_Align_Mark_actif = TRUE;
 This->Regle_actif = TRUE;
 This->Frame_Border_actif = TRUE;
 This->ScrollBarH_actif = TRUE;
 This->ScrollBarV_actif = TRUE;
 This->Pager_actif = TRUE;
 This->magnetic = TRUE;
 Xc_TRACE(("done"));
 return This;
}

void init_Page(This,page_number,page_format,page_type,page_orient,
	       left_margin, right_margin, upper_margin, lower_margin,
	       nb_column, nb_row, column_spacing, row_spacing, 
	       grid_origx, grid_origy, grid_deltax, grid_deltay, mode)
c_Page		*This;
unsigned int	page_number;
page_format_t	*page_format;
page_type_t	page_type;
page_orient_t	page_orient;
coord_t		left_margin;
coord_t		right_margin;
coord_t		upper_margin;
coord_t		lower_margin;
int		nb_column;
int		nb_row;
coord_t		column_spacing;
coord_t		row_spacing;
coord_t		grid_origx;
coord_t		grid_origy;
coord_t		grid_deltax;
coord_t		grid_deltay;
int		mode;
{  
 coord_t	width;
 coord_t	height;
 int		x_orig;
 int		y_orig;
 int		nb_page;

 Xc_HISTORY(("Init Page Parameters"));

 This->page_number = page_number;
 This->page_format = page_format;
 This->page_type   = page_type;
 This->page_orient = page_orient;
  
 width = page_format->width;
 height = page_format->height;
 if (page_orient == Xc_LANDSCAPE)
 {
  coord_t swap;

  swap = width;
  width = height;
  height = swap;
 }

 nb_page = (page_type == Xc_PAGE_DOUBLE)? 2 : 1;
  
 if (mode == XPi_MODIFY)
  destroy_desk_area_component(This, FALSE);

 create_page_contour(This, width, height, nb_page, 1);
  
 calcul_desk_area_and_origin(This,mode != XPi_MODIFY);

 create_page_grid(This,
		  This->Page_Contour.sOrig_X + grid_origx,
		  This->Page_Contour.sOrig_Y + grid_origy,
		  grid_deltax, grid_deltay);

 if (page_number)
  create_page_typo_mark(This, left_margin, right_margin, upper_margin,
			lower_margin, nb_column, nb_row, column_spacing,
			row_spacing);
 else
  create_page_typo_mark(This,
			This->Page_Typo_Mark->left_margin,
			This->Page_Typo_Mark->right_margin,
			This->Page_Typo_Mark->top_margin,
			This->Page_Typo_Mark->bottom_margin,
			This->Page_Typo_Mark->column,
			This->Page_Typo_Mark->row,
			This->Page_Typo_Mark->gouttiere_x,
			This->Page_Typo_Mark->gouttiere_y);
  
 calcul_desk_area_component(This);

 if (mode == XPi_LOAD)
 {
  x_orig = This->Orig_X;	/* coordinate calculated from Page_contour*/
  y_orig = This->Orig_Y;
  This->sOrig_X = This->Page_Contour.sOrig_X;
  This->sOrig_Y = This->Page_Contour.sOrig_Y;
  This->Orig_X = This->sOrig_X / This->sScale;
  This->Orig_Y = This->sOrig_Y / This->sScale;
  change_page_origin(This, x_orig, y_orig);
 } 
 if (mode == XPi_MODIFY)
  recoordinate_page_align_mark(This);
  
 Xc_TRACE(("init page done"));
}
static void select_page(This, display, mere, posx, posy, sizex, sizey,
			scrollbarv, scrollbarh, reglev, regleh, reglec, pager)
c_Page	*This;
Display		*display;
Window		mere;
int		posx;
int		posy;
int		sizex;
int		sizey;
c_ScrollBarX	*scrollbarv;
c_ScrollBarX	*scrollbarh;
c_Regle		*reglev;
c_Regle		*regleh;
c_Regle		*reglec;
c_Pager		*pager;
{
 Window window;
 XSetWindowAttributes	windattrib;
  
 Xc_TRACE(("Select Page: %d",This->page_number));
  
 This->RegleV = reglev;
 This->RegleH = regleh;
 This->RegleC = reglec;
 This->ScrollBarV = scrollbarv;
 This->ScrollBarH = scrollbarh;
 This->Pager = pager;
  
 This->PosX = This->PosY = This->Width = This->Height= 0;
 This->in_window = FALSE;
 window = XCreateSimpleWindow(display, mere, posx, posy, sizex, sizey, 0,
			    GlobColormap->Cblack, GlobColormap->Cwhite);
 F(GlobDragAndDrop).select_input
  (GlobDragAndDrop, window, DD_DROP | DD_MOTION, DD_LONG, eventlook_page, 
   DD_LONG, This, DD_INT, (int)(SCALE_TO_PERCENT(This->sFactor) * 100),
   DD_INT, getpid(), NULL);

 Xc_TRACE(("event look: %ld   page: %ld", eventlook_page,This));
 set_X_info(This, display,window);
  
 This->cursor = (c_Cursor *)NEW(c_Cursor)(display, window);
  
 XSelectInput(display, window, KeyPressMask | KeyReleaseMask |
	      PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
	      ExposureMask | LeaveWindowMask | EnterWindowMask);
 windattrib.bit_gravity = StaticGravity;
 XChangeWindowAttributes(display, window, CWBitGravity, &windattrib);
 
 resize_Page(This, posx, posy, sizex, sizey);

 calcul_desk_area_component(This);
 {
  l_Cadre	*lcadre;
  c_Cadre	*cadre;
  scale_t	new_factor = This->sFactor;
  coord_t	new_scale = This->sScale;

  lcadre = This->ListCadre;
  while(lcadre)
  {
   cadre = lcadre->cadre;
   if (cadre->sFactor != new_factor)
   {
    cadre->sScale = new_scale;
    cadre->sFactor = new_factor;
    F(cadre).calculscaling(cadre);
    F(cadre).calculorigine(cadre);
    F(cadre).calculforme(cadre, CONTOUR);
    if (cadre->object_type != CADRE_EMPTY)
    {
     if (DISPLAY_INIT(cadre->object)(cadre->object, cadre) != XC_NO_ERROR)
      F(cadre).detachobject(cadre);
    }
   }
   lcadre = lcadre->NextCadre;
  }
 }
 F(This).calcul_select_handle(This);


 XMapWindow(display, window);

 /*  draw_desk_area_rectangle(This,0,0,sizex,sizey); */
 F(This->ScrollBarV).Set_Callback(This->ScrollBarV,change_page_origin,
				  This);
 F(This->ScrollBarH).Set_Callback(This->ScrollBarH,change_page_origin,
				  This);
 Xc_TRACE(("w: %d h: %d",This->DeskArea_Width, This->DeskArea_Height ));
 F(This->ScrollBarV).Change_Rsize
  (This->ScrollBarV, _display, This->DeskArea_Width, This->DeskArea_Height);
 F(This->ScrollBarH).Change_Rsize
  (This->ScrollBarH, _display, This->DeskArea_Width, This->DeskArea_Height);
 F(This->ScrollBarH).AMove(This->ScrollBarH, This->Orig_X);
 F(This->ScrollBarV).AMove(This->ScrollBarV, This->Orig_Y);
 F(This->RegleH).Init_Regle_By_Page(This->RegleH, This);
 F(This->RegleV).Init_Regle_By_Page(This->RegleV, This);
 F(This->RegleC).Init_Regle_By_Page(This->RegleC, This);
 F(This->RegleH).Draw(This->RegleH, TRUE);
 F(This->RegleV).Draw(This->RegleV, TRUE);

 if (LinkFunction)
  set_function(This,LinkFunction);
 else
  set_function(This,SELECT_MOVE_RESIZE_CADRE);
 Xc_TRACE(("end select"));
}

static void deselect_page(This)
c_Page	*This;
{
 l_Cadre	*lcadre;
 int	      	LinkFuncSvg;
 Xc_TRACE(("Unselect Page: %d",This->page_number));

 LinkFuncSvg = LinkFunction;
 LinkFunction = 0;
 ABORT(This);
 LinkFunction = LinkFuncSvg;
 This->function = NULL;
 This->function_data = NULL;
 if(This->cursor != NULL)
 {
  DELETE(c_Cursor)(This->cursor);
  This->cursor = NULL;
 }
 if(This->X_info_calc)
 {
  lcadre = This->ListCadre;
  while(lcadre)
  {
   lcadre->cadre->X_info_calc=FALSE;
   XFreeGC(This->X_info.display, lcadre->cadre->X_info.gc_contour);
   XFreeGC(This->X_info.display, lcadre->cadre->X_info.gc_inside);
   lcadre->cadre->X_info.gc_contour = 0;
   lcadre->cadre->X_info.gc_inside = 0;
   lcadre = lcadre->NextCadre;
  }
  XFreeGC(This->X_info.display, This->X_info.gc_contour);
  XFreeGC(This->X_info.display, This->X_info.gc_inside);
  XDestroyWindow(This->X_info.display, This->X_info.window);
  This->X_info_calc = FALSE;
 }
 F(This->ScrollBarV).Unset_Callback(This->ScrollBarV);
 F(This->ScrollBarH).Unset_Callback(This->ScrollBarH);
}

void dest_Page(This)
c_Page *This;
{
 l_Cadre	*lcadre;

 Xc_HISTORY(("destructor"));
  
 if (This->X_info_calc)
  F(This).deselect_Page(This);
 else
  if (This->function) ABORT(This);

 lcadre = This->ListCadre;
 This->ListCadre = NULL;
 This->nb_cadre = 0;
 free_list_cadre(lcadre, DEL_CADRE);

 destroy_desk_area_component(This, TRUE);
 deselect_all_cadre(This, FALSE);
 Xc_free(This);
  
 Xc_TRACE(("destroyed"));
}

void *copy_Page(ObjectToCopy)
c_Page *ObjectToCopy;
{
 c_Page *ObjTmp;

 if ((ObjTmp=(c_Page *)Xc_malloc("Page",sizeof (c_Page)))==NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp,ObjectToCopy,sizeof (c_Page));
 Xc_TRACE(("Object Page copied"));
 return ObjTmp;
}

void resize_Page(This, posx, posy, new_width, new_height)
c_Page	*This;
int	posx;
int	posy;
int	new_width;
int	new_height;
{
 XSetWindowAttributes	windattrib;

 if (This->DeskArea_Height >= new_height && !This->ScrollBarV_actif)
 {
  Map_ScrollBarV(This);
  F(This->ScrollBarV).AMove(This->ScrollBarV, This->Orig_Y);
  This->ScrollBarV_actif=TRUE;
  new_width -= SCROLLB_SIZE;
 }
 if (This->DeskArea_Width >= new_width && !This->ScrollBarH_actif)
 {
  Map_ScrollBarH(This);
  F(This->ScrollBarH).AMove(This->ScrollBarH, This->Orig_X);
  This->ScrollBarH_actif=TRUE;
  new_height -= SCROLLB_SIZE;
 }
 if (This->DeskArea_Width < new_width && This->ScrollBarH_actif)
 {
  Unmap_ScrollBarH(This);
  F(This).change_page_origin(This, 0, This->Orig_Y);
  This->ScrollBarH_actif=FALSE;
  new_height += SCROLLB_SIZE;
 }
 if (This->DeskArea_Height < new_height && This->ScrollBarV_actif)
 {
  Unmap_ScrollBarV(This);
  F(This).change_page_origin(This, This->Orig_X, 0);
  This->ScrollBarV_actif = FALSE;
  new_width += SCROLLB_SIZE;
 }
 This->Width = MIN(This->DeskArea_Width, new_width);
 This->Height = MIN(This->DeskArea_Height, new_height);
 posx += (new_width - This->Width) / 2;
 posy += (new_height - This->Height) / 2;
 if (posx != This->PosX || posy != This->PosY)
 {
  windattrib.bit_gravity = ForgetGravity;
  XChangeWindowAttributes(_display, _window, CWBitGravity, &windattrib);
  XMoveResizeWindow(_display, _window, posx, posy, 
		    This->Width, This->Height);
  windattrib.bit_gravity = StaticGravity;
  XChangeWindowAttributes(_display, _window, CWBitGravity, &windattrib);
 }
 else
 {
  XMoveResizeWindow(_display, _window, posx, posy, 
		    This->Width, This->Height);
 }
 Xc_TRACE(("EXPOSE VENANT DU RESIZE PAGE"));
  
 This->PosX = posx;
 This->PosY = posy;
}

boolean set_function(This, num_function)
c_Page	*This;
int	num_function;
{
 l_Cadre	*lcadre;
 boolean	ret_code = FALSE;
  
 if (This->function_data)
  ABORT(This);
    
 switch(num_function)
 {
 case SELECT_MOVE_RESIZE_CADRE:
  This->function = &fc_Select_Move_Resize_Cadre;
  if (This->select_type)
   This->function_type = MOVE_POINT_MODE;
  else
   This->function_type = RESIZE_CADRE_MODE;
  break;
 case CREATE_CADRE_RECTANGLE:
  This->function = &fc_Create_Cadre_Rectangle;
  This->function_type = num_function;
  break;
 case CREATE_CADRE_ELIPSE:
  This->function = &fc_Create_Cadre_Elipse;
  This->function_type = num_function;
  break;
 case CREATE_CADRE_POLYGONAL:
  This->function = &fc_Create_Cadre_Polygonal;
  This->function_type = num_function;
  break;
 case DELETE_SELECTED_CADRE:
  if (!This->verrou && This->SelectedCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).delete_selected_cadre(This);
   This->function = &fc_Select_Move_Resize_Cadre;
   This->function_type = SELECT_MOVE_RESIZE_CADRE;
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case SET_LOWER_PRIORITY:
  if (!This->verrou && This->SelectedCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).set_lower_priority(This);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case SET_HIGHER_PRIORITY:
  if (!This->verrou && This->SelectedCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).set_higher_priority(This);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case SET_LOWEST_PRIORITY:
  if (!This->verrou && This->SelectedCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).set_lowest_priority(This);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case SET_HIGHEST_PRIORITY:
  if (!This->verrou && This->SelectedCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).set_highest_priority(This);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case SELECT_ALL_CADRE:
  SET_WAIT_MODE_STATIC;
  F(This).deselect_all_cadre(This,TRUE);
  F(This).select_all_cadre(This);
  lcadre=This->SelectedCadre;
  while(lcadre)
  {
   This->verrou|=lcadre->cadre->verrou;
   lcadre=lcadre->NextCadre;
  }
  UNSET_WAIT_MODE;
  break;
 case ZOOM_PLUS_PAGE:
  if (This->sFactor < SCALE_FROM_PERCENT(ZOOM_PLUS_LIMIT))
  {
   This->function = &fc_Zoom_Plus_Page;
   This->function_type = num_function;
  }
  break;
 case ZOOM_MINUS_PAGE:
  if (This->sFactor>SCALE_FROM_PERCENT(ZOOM_MINUS_LIMIT))
  {
   This->function = &fc_Zoom_Minus_Page;
   This->function_type = num_function;
  }
  break;
 case ZOOM_IN_ZONE:
  This->function = &fc_Zoom_Page_In_Zone;
  This->function_type = num_function;
  break;
 case ZOOM_FIT_IN_WIDTH:
  F(This).zoom_page_fit_in_width(This);
  break;
 case ZOOM_FIT_IN_SCREEN:
  F(This).zoom_page_fit_in_screen(This); 
  break;
 case ZOOM_AT_REAL_SIZE:
  F(This).zoom_page_at_real_size(This);
  break;
 case ZOOM_25_PAGE:
  F(This).zoom_page_at_percent(This, SCALE_FROM_PERCENT(25.0));
  break;
 case ZOOM_50_PAGE:
  F(This).zoom_page_at_percent(This, SCALE_FROM_PERCENT(50.0));
  break;
 case ZOOM_75_PAGE:
  F(This).zoom_page_at_percent(This, SCALE_FROM_PERCENT(75.0));
  break;
 case ZOOM_100_PAGE:
  F(This).zoom_page_at_percent(This, SCALE_FROM_PERCENT(100.0));
  break;
 case ZOOM_200_PAGE:
  F(This).zoom_page_at_percent(This, SCALE_FROM_PERCENT(200.0));
  break;
 case ZOOM_400_PAGE:
  F(This).zoom_page_at_percent(This, SCALE_FROM_PERCENT(400.0));
  break;
 case ROTATE_CADRE:
  if (!This->verrou && This->SelectedCadre)
  {
   This->function = &fc_Rotate_Cadre;
   This->function_type = num_function;
  }
  break;
 case SCALE_CADRE:
  if (!This->verrou && This->SelectedCadre)
  {
   This->function = &fc_Scale_Cadre;
   This->function_type = num_function;
  }
  break;
 case LOCK_CADRE:
  F(This).lock_cadre(This);
  ret_code = TRUE;
  break;
 case UNLOCK_CADRE:
  F(This).unlock_cadre(This);
  ret_code = TRUE;
  break;
 case INSERT_POINT:
  if (!This->verrou && This->SelectedCadre)
  {
   This->function = &fc_Insert_Point;
   This->function_type = num_function;
  }
  break;
 case DELETE_POINT:
  if (!This->verrou && This->SelectedCadre)
  {
   This->function = &fc_Delete_Point;
   This->function_type = num_function;
  }
  break;
 case MOVE_POINT_MODE:
  if (!This->verrou && This->SelectedCadre)
  {
   F(This).move_point_mode(This);
  }
  This->function = &fc_Select_Move_Resize_Cadre;
  This->function_type = num_function;
  break;
 case RESIZE_CADRE_MODE:
  if (!This->verrou && This->SelectedCadre)
  {
   F(This).resize_cadre_mode(This);
  }
  This->function = &fc_Select_Move_Resize_Cadre;
  This->function_type = num_function;
  break;
 case FUSION_CADRE:
  if (!This->verrou && This->SelectedCadre &&
      This->SelectedCadre->NextCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).fusion_cadre(This);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case SUBSTRACT_CADRE:
  if (!This->verrou && This->SelectedCadre &&
      This->SelectedCadre->NextCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).substract_cadre(This);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case ADDITION_CADRE:
  if (!This->verrou && This->SelectedCadre &&
      This->SelectedCadre->NextCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).addition_cadre(This);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case OUTLINE_CADRE:
  if (!This->verrou && This->SelectedCadre &&
      This->SelectedCadre->NextCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).outline_cadre(This);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case CUT_OBJECT:
  if (!This->verrou && This->SelectedCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).cut_selected_cadre(This);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  break;
 case COPY_OBJECT:
  if (!This->verrou && This->SelectedCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).copy_selected_cadre(This);
   UNSET_WAIT_MODE;
  }
  break;
 case PASTE_OBJECT:
  SET_WAIT_MODE_STATIC;
  F(This).paste_selected_cadre(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DELETE_OBJECT:
  if (!This->verrou && This->SelectedCadre)
  {
   SET_WAIT_MODE_STATIC;
   F(This).delete_selected_object(This);
   UNSET_WAIT_MODE;
   ret_code = TRUE;
  }
  This->function = &fc_Select_Move_Resize_Cadre;
  This->function_type = SELECT_MOVE_RESIZE_CADRE;
  break;
 case DISPLAY_GRID:
  SET_WAIT_MODE_STATIC;
  display_grid(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DISPLAY_TYPO_MARK:
  SET_WAIT_MODE_STATIC;
  display_typo_mark(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DISPLAY_ALIGN_MARK:
  SET_WAIT_MODE_STATIC;
  display_align_mark(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DISPLAY_REGLE:
  SET_WAIT_MODE_STATIC;
  display_regle(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DISPLAY_FRAME_BORDER:
  SET_WAIT_MODE_STATIC;
  display_frame_border(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case MAGNETIZE_ALL:
  SET_WAIT_MODE_STATIC;
  magnetize_all(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case REDRAW_ALL:
  SET_WAIT_MODE_STATIC;
  redraw_all(This);
  UNSET_WAIT_MODE;
  break;
 case ADD_LINK_AT_BEGINNING:
 case ADD_LINK_AT_END:
 case INSERT_LINK_BEFORE:
 case INSERT_LINK_BEHIND:
  This->function = &fc_Link_TextFrame;
  This->function_type = num_function;
  break;
 default:  
  if ((num_function > IMAGE_FUNCTION_BEGIN &&
       num_function < IMAGE_FUNCTION_END) || 
      (num_function > VECTOR_FUNCTION_BEGIN &&
       num_function < VECTOR_FUNCTION_END))
  {
   if (This->SelectedCadre && 
       This->SelectedCadre->cadre->object_type == CADRE_VECTOR &&
       (num_function > IMAGE_FUNCTION_BEGIN && 
	num_function < IMAGE_FUNCTION_END))
    num_function += VECTOR_FUNCTION_BEGIN - IMAGE_FUNCTION_BEGIN;
 
   if (This->SelectedCadre && !This->SelectedCadre->NextCadre &&
       ((num_function > IMAGE_FUNCTION_BEGIN && 
	 num_function < IMAGE_FUNCTION_END && 
	 This->SelectedCadre->cadre->object_type == CADRE_IMAGE_BITMAP) ||
	(num_function > VECTOR_FUNCTION_BEGIN &&
	 num_function < VECTOR_FUNCTION_END && 
	 This->SelectedCadre->cadre->object_type == CADRE_VECTOR)))
   {
    This->function = &fc_Dispatch_To_Object;
    This->function_data = (void *)((long)This->function_type);
    This->function_type = num_function;
    ret_code = TRUE;
   }    
  }
  break;
 }
 Xc_TRACE(("init function %d", This->function_type));
 if (This->X_info_calc)		/* test if page selected */
  INIT(This);

 return ret_code;
}

status_t eventlook_page(This, event)
c_Page	*This;
XEvent	*event;
{
 XEvent	none;
 status_t	status;
 short		xm,ym;
 KeySym        symb;
 boolean	up, left, right, down, prior, next, select, move;
 l_Cadre	*lcadre;
 l_Cadre	*svglcadre;
 coord_t	dep_x, dep_y;

 status=FUNCTION_NOT_FINISHED;
 if (event->xany.window == This->X_info.window)
 {
  switch(event->type)
  {
  case Expose:
  case GraphicsExpose:
   Xc_TRACE(("%s rect x:%d y:%d <-> x:%d y:%d",
	     (event->type==Expose?"Expose":"Gfx Expose"),		
	     event->xexpose.x,event->xexpose.y,
	     event->xexpose.x+event->xexpose.width-1,
	     event->xexpose.y+event->xexpose.height-1));
   F(This).redraw_rectangle(This,event->xexpose.x,event->xexpose.y,
			    event->xexpose.x+event->xexpose.width-1,
			    event->xexpose.y+event->xexpose.height-1); 
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
     if ((DROP_FLAGS(*event) & 0xf0)==DD_DROP_FOR_MOVE)
     {
      Xc_TRACE(("Target: Drop cadre for move"));
      ABORT(This);
      drag_cadre_move_done(This, DROP_DATA(*event), xm, ym);
      status = FUNCTION_MODIFIED;
      INIT(This);
     }
     else
     {
      Xc_TRACE(("Target: Drop cadre for copy"));
      ABORT(This);
      drag_cadre_copy_done(This,DROP_DATA(*event), xm, ym);
      status = FUNCTION_MODIFIED;
      INIT(This);
     }
    }
#ifndef NTRACE
    else
     Xc_TRACE(("Hum... Drop from another cession not allowed"));
#endif
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
   /*Xc_TRACE(("leave window"));*/
   if (This->in_window)
   {
    F(This->RegleV).Clear_Repere(This->RegleV, REGLE_REPERE_2);
    F(This->RegleH).Clear_Repere(This->RegleH, REGLE_REPERE_2);
    status=EVENTLOOK(This,*event);
    This->in_window = FALSE;
   }
   break;
  case EnterNotify:
   /*Xc_TRACE(("enter window"));*/
   if (!This->in_window)
   {
    xm = event->xcrossing.x;
    ym = event->xcrossing.y;
    F(This->RegleV).Draw_Repere(This->RegleV, REGLE_REPERE_2, ym);
    F(This->RegleH).Draw_Repere(This->RegleH, REGLE_REPERE_2, xm);
    status=EVENTLOOK(This,*event);
    This->in_window = TRUE;
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
   up = right = left = down = prior = next = select = move = FALSE;
   switch (symb)
   { 
   case XK_Up:
    up = move = TRUE;
    break;
   case XK_Down:
    down = move = TRUE;
    break;
   case XK_Right:
    right = move = TRUE;
    break;
   case XK_Left:
    left = move = TRUE;
    break;
   case XK_Prior:
    prior = move = select = TRUE;
    break;
   case XK_Next:
    next = move = select = TRUE;
    break;
   case XK_BackSpace:
    Xc_TRACE(("Backspace direct"));
    if (set_function(This, DELETE_OBJECT))
     return FUNCTION_MODIFIED;
    break;
   case XK_Delete:
    if (set_function(This, DELETE_SELECTED_CADRE))
     return FUNCTION_MODIFIED;
    break;
   case XK_Undo:
   case XK_Escape:
    return FUNCTION_ABORTED;
    break;
   default:
    status=EVENTLOOK(This,*event);
    break;
   }
   if (move)
   {
    if (This->SelectedCadre && !select)
    {
     dep_x = dep_y = 0;
     if (left)
      dep_x = -SCALE_FROM_MILLIMETERS(4);
     if (right)
      dep_x = SCALE_FROM_MILLIMETERS(4);
     if (up)
      dep_y = -SCALE_FROM_MILLIMETERS(4);
     if (down)
      dep_y = SCALE_FROM_MILLIMETERS(4);
     if (This->function_data)
      ABORT(This);
     SET_WAIT_MODE_STATIC;
     F(This).undisplay_selected_lcadre(This);
     lcadre=svglcadre=(l_Cadre *)
      sort_asc_list_cadre(This->SelectedCadre);
     while(lcadre)
     {
      F(This).move_cadre_to_xy(This, lcadre,
			       lcadre->cadre->sOrig_X+dep_x,
			       lcadre->cadre->sOrig_Y+dep_y);
      lcadre=lcadre->NextCadre;
     }
     free_list_cadre(svglcadre, NO_DEL_CADRE);
     F(This).display_selected_lcadre(This);
     UNSET_WAIT_MODE;
     INIT(This);
    }
    else
    {
     if (left)
      F(This->ScrollBarH).RequestMove(This->ScrollBarH, 
				      SCROLL_LEFT, NULL, FALSE,
				      This->Orig_X,This->Orig_Y,0);
     if (right)
      F(This->ScrollBarH).RequestMove(This->ScrollBarH, 
				      SCROLL_RIGHT, NULL, FALSE,
				      This->Orig_X,This->Orig_Y,0);
     if (up)
      F(This->ScrollBarV).RequestMove(This->ScrollBarV, 
				      SCROLL_UP ,NULL, FALSE,
				      This->Orig_X,This->Orig_Y,0);
     if (down)
      F(This->ScrollBarV).RequestMove(This->ScrollBarV,
				      SCROLL_DOWN, NULL, FALSE,
				      This->Orig_X,This->Orig_Y,0);
     if (prior)
      F(This->ScrollBarV).RequestMove(This->ScrollBarV,
				      SCROLL_UP, NULL, FALSE,
				      This->Orig_X, This->Orig_Y,
				      This->Height);
     if (next)
      F(This->ScrollBarV).RequestMove(This->ScrollBarV,
				      SCROLL_DOWN, NULL, FALSE,
				      This->Orig_X, This->Orig_Y,
				      This->Height);
     xm = event->xkey.x;
     ym = event->xkey.y;
     F(This->RegleV).Draw_Repere(This->RegleV, REGLE_REPERE_2, ym);
     F(This->RegleH).Draw_Repere(This->RegleH, REGLE_REPERE_2, xm);
     status=EVENTLOOK(This,*event);
    }
   }
   break;
  case MotionNotify:
   xm = event->xmotion.x;
   ym = event->xmotion.y;
   if (xm>=0 && xm<This->Width && ym>=0 && ym<This->Height)
   {
    F(This->RegleV).Draw_Repere(This->RegleV, REGLE_REPERE_2, ym);
    F(This->RegleH).Draw_Repere(This->RegleH, REGLE_REPERE_2, xm);
   }
   This->in_window = TRUE;
  default:
   status=EVENTLOOK(This,*event);
   break;
  }
 }
 return status;
}

void set_X_info(This,display,window)
c_Page		*This;
Display		*display;
Window		window;
{
 GC	gc;
 XGCValues	val_gc;
 l_Cadre	*lcadre;
  
 if (!This->X_info_calc)
 {
  This->X_info.display=display;
  This->X_info.window=window;
  This->X_info.ecran=DefaultScreen(display);
    
  if (GlobColormap->Cblack)
   val_gc.function=GXxor;
  else
   val_gc.function=GXinvert;
  val_gc.plane_mask= -1;
  val_gc.background=GlobColormap->Cwhite;
  val_gc.foreground=GlobColormap->Cblack;
  val_gc.line_width=0;
  val_gc.line_style=LineSolid;
  val_gc.cap_style=CapNotLast;
  val_gc.join_style=JoinMiter;
  gc=XCreateGC(display,window,GCFunction|GCPlaneMask|GCForeground|
	       GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	       GCJoinStyle,&val_gc);
  This->X_info.gc_contour=gc;
  
  val_gc.function=GXcopy;
  val_gc.cap_style=CapButt;
  val_gc.foreground=GlobColormap->Cwhite;
  val_gc.background=GlobColormap->Cblack;
  val_gc.graphics_exposures=True;
  gc=XCreateGC(display,window,GCFunction|GCForeground|GCBackground|
	       GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle|
	       GCGraphicsExposures,&val_gc);
  This->X_info.gc_inside=gc;
  This->X_info_calc=TRUE;
 }
 lcadre=This->ListCadre;
 while(lcadre)
 {
  F(lcadre->cadre).set_X_info(lcadre->cadre,display,window);
  lcadre=lcadre->NextCadre;
 }
}

error add_Cadre(This, name, priority, frame)
c_Page 	*This;
char	*name;
int	priority;
c_Cadre	*frame;
{
 l_Cadre *lcadre, **link;

 Xc_HISTORY(("Adding frame `%s' to Page", name));

 for(link = &This->ListCadre; *link != NULL; link = &((*link)->NextCadre))
  if(Xstrcmp(name, (*link)->name))
  {
   Xc_TRACE(("Cadre name already exists"));
   return XC_NAME_ALREADY_EXIST;
  }

 if((lcadre = Xc_malloc("Frame in list", sizeof(l_Cadre))) == NULL)
  return XC_MEMORY_ALLOCATION;

 if(frame == NULL)
  if((frame = (c_Cadre *)NEW(c_Cadre)(This->BaseStd)) == NULL)
  {
   Xc_free(lcadre);
   return XC_MEMORY_ALLOCATION;
  }
  
 if(frame->name != name) memcpy(frame->name, name, XcMAX_NAME_LENGTH);

 This->nb_cadre++;
 *link = lcadre;

 lcadre->cadre = frame;
 lcadre->name = frame->name;
 lcadre->priority = priority;
 lcadre->NextCadre = NULL;
    
 frame->sScale = This->sScale;
 frame->sFactor = This->sFactor;
 frame->data_to_object = &(This->data_to_object);

 Xc_TRACE(("Cadre `%s' added with no error", lcadre->name));
 return XC_NO_ERROR;
}


void free_list_cadre(lcadre,iscadre)
l_Cadre	*lcadre;
boolean	iscadre;
{
 l_Cadre *tmp;
 l_Cadre *tmp2;

 tmp=lcadre;
 while(tmp)
 {
  tmp2=tmp->NextCadre;
  if (iscadre==DEL_CADRE)
  {
   DELETE(c_Cadre)(tmp->cadre);
  }
  Xc_free(tmp);
  tmp=tmp2;
 }
}

void get_center_list_cadre(lcadre,xcent,ycent)
l_Cadre	*lcadre;
coord_t	*xcent;
coord_t	*ycent;
{
 coord_t	x_min;
 coord_t	y_min;
 coord_t	x_max;
 coord_t	y_max;
  
 x_min=lcadre->cadre->sOrig_X;
 x_max=x_min+lcadre->cadre->sWidth;
 y_min=lcadre->cadre->sOrig_Y;
 y_max=y_min+lcadre->cadre->sHeight;
 lcadre=lcadre->NextCadre;
 while(lcadre)
 {
  if (x_min>lcadre->cadre->sOrig_X)
   x_min=lcadre->cadre->sOrig_X;
  if (x_max<lcadre->cadre->sOrig_X+lcadre->cadre->sWidth)
   x_max=lcadre->cadre->sOrig_X+lcadre->cadre->sWidth;
  if (y_min>lcadre->cadre->sOrig_Y)
   y_min=lcadre->cadre->sOrig_Y;
  if (y_max<lcadre->cadre->sOrig_Y+lcadre->cadre->sHeight)
   y_max=lcadre->cadre->sOrig_Y+lcadre->cadre->sHeight;
  lcadre=lcadre->NextCadre;
 } 
 *xcent=(x_max+x_min)>>1;
 *ycent=(y_max+y_min)>>1;
}
void get_limit_list_cadre(lcadre,x_min,y_min,x_max,y_max)
l_Cadre	*lcadre;
coord_t	*x_min;
coord_t	*y_min;
coord_t	*x_max;
coord_t	*y_max;
{
 if (!lcadre)
 {
  *x_min= *x_max= *y_min= *y_max=0;
 }
 else
 {
  *x_min=lcadre->cadre->sOrig_X;
  *x_max= *x_min+lcadre->cadre->sWidth;
  *y_min=lcadre->cadre->sOrig_Y;
  *y_max= *y_min+lcadre->cadre->sHeight;
  lcadre=lcadre->NextCadre;
  while(lcadre)
  {
   if (*x_min>lcadre->cadre->sOrig_X)
    *x_min=lcadre->cadre->sOrig_X;
   if (*x_max<lcadre->cadre->sOrig_X+lcadre->cadre->sWidth)
    *x_max=lcadre->cadre->sOrig_X+lcadre->cadre->sWidth;
   if (*y_min>lcadre->cadre->sOrig_Y)
    *y_min=lcadre->cadre->sOrig_Y;
   if (*y_max<lcadre->cadre->sOrig_Y+lcadre->cadre->sHeight)
    *y_max=lcadre->cadre->sOrig_Y+lcadre->cadre->sHeight;
   lcadre=lcadre->NextCadre;
  } 
 }
}

void delete_Cadre(This, lcadre, delcadre)
c_Page		*This;
l_Cadre		*lcadre;
boolean		delcadre;
{
 l_Cadre	*tmp;
 l_Cadre	*cadre;
  
 Xc_HISTORY(("deleting cadre %s", lcadre->name));

 cadre = tmp = This->ListCadre;
 while(tmp->cadre != lcadre->cadre)
 {
  cadre=tmp;
  tmp = tmp->NextCadre;
 }
 F(This).deselect_cadre(This, lcadre, FALSE);
 if (cadre != tmp)
  cadre->NextCadre = tmp->NextCadre;
 else
  This->ListCadre = tmp->NextCadre;
 Xc_free(tmp);
 This->nb_cadre--;
  
 if (delcadre==DEL_CADRE)
  DELETE(c_Cadre)(lcadre->cadre);
}

void list_Cadre(This)
c_Page       *This;
{
 int   i;
 l_Cadre       *tmp;

 Xc_TRACE(("Listing des cadres:"));
 tmp=This->ListCadre;
 for(i=0;i<This->nb_cadre;i++)
 {
  Xc_TRACE(("%d) name: %s / priority: %d", i, tmp->name, tmp->priority));
  tmp=tmp->NextCadre;
 }
}

void add_cadre_in_list(lcadre,lcadre2)
l_Cadre	**lcadre;
l_Cadre	*lcadre2;
{
 l_Cadre	*tmplcadre;
 l_Cadre	*ptrlcadre;
  
 if (lcadre2)
 {
  tmplcadre=(l_Cadre *)Xc_malloc("addlcadre",sizeof(l_Cadre));
  memcpy(tmplcadre,lcadre2,sizeof(l_Cadre));
  tmplcadre->NextCadre=NULL;
      
  if (*lcadre)
  {
   ptrlcadre = *lcadre;
   while(ptrlcadre->NextCadre)
    ptrlcadre=ptrlcadre->NextCadre;
   ptrlcadre->NextCadre=tmplcadre;
  }
  else
   *lcadre=tmplcadre;
 }
}

void delete_cadre_in_list(lcadre,cadre)
l_Cadre	**lcadre;
c_Cadre	*cadre;
{
 l_Cadre	*tmp;
 l_Cadre	*previous;
    
 tmp= *lcadre;
 previous=NULL;
 while(tmp)
 {
  if (tmp->cadre==cadre)
  {
   if (previous)
    previous->NextCadre=tmp->NextCadre;
   else
    *lcadre=tmp->NextCadre;
   Xc_free(tmp);
   return;
  }
  previous=tmp;
  tmp=tmp->NextCadre;
 }
}

l_Cadre *duplicate_list_cadre(lcadre)
l_Cadre	*lcadre;
{
 l_Cadre	*tmplist;
 l_Cadre	*returnl;
 l_Cadre	*retlist;
 boolean	prems;
  
 prems=TRUE;
 tmplist=lcadre;
 if (tmplist)
 {
  retlist=(l_Cadre *)Xc_malloc("dup lcadre",sizeof(l_Cadre));
  returnl=retlist;
  while(tmplist)
  {
   memcpy(retlist,tmplist,sizeof(l_Cadre));
   tmplist=tmplist->NextCadre;
   if (tmplist)
   {
    retlist->NextCadre=(l_Cadre *)Xc_malloc("dup lcadre",sizeof(l_Cadre));
    retlist=retlist->NextCadre;
   }
   else
    retlist->NextCadre=NULL;
  }
  return returnl;
 }
 else
  return NULL;
}

l_Cadre *search_cadre_by_name(This,name)
c_Page *This;
char            *name;
{
 int i;
 l_Cadre *tmp;
  
 tmp=This->ListCadre;
 i=0;
 while((i<This->nb_cadre)&&(!Xstrcmp(name, tmp->name)))
 {
  i++;
  tmp=tmp->NextCadre;
 }
 if (i<This->nb_cadre)
 {
  return tmp;
 }
 return NULL;
}

void destroy_cadre(This,lcadre,delcadre)
c_Page	*This;
l_Cadre	*lcadre;
boolean	delcadre;
{
 l_Cadre *lowlist;
 l_Cadre *lowsort;
 l_Cadre *highlist;
 l_Cadre *tmplist;
 l_Cadre *tmplist2;
 l_Cadre *transp;
 c_Cadre *this_cadre;
 c_Cadre **lowcadre = NULL;
 Polygone *forme;
 Polygone *lowforme;
 Polygone *ftransp;
 Polygone *oldforme = NULL;
 int	count_cadre;
 int	i;
  
 this_cadre=lcadre->cadre;
 list_cadre_low_high_priority(This,lcadre,&lowlist,&highlist,INTERS_CADRE);
 forme = DuplicatePolygone(lcadre->cadre->Forme);
 tmplist = highlist;
 transp = NULL;
 while(tmplist)
 { 
  if (tmplist->cadre->inside_color->transparency)
   add_cadre_in_list(&transp, tmplist);
  else
   SubtractPolygone(&forme, tmplist->cadre->Forme);
  tmplist = tmplist->NextCadre;
 }
 if (transp)
  oldforme = DuplicatePolygone(lcadre->cadre->Forme);
 lowsort = sort_desc_list_cadre(lowlist);
 tmplist = lowsort;
 count_cadre=0;
 while(tmplist)
 {
  count_cadre++;
  tmplist=tmplist->NextCadre;
 }
 if (count_cadre)
 {
  lowcadre=(c_Cadre **)Xc_malloc("mv lcadre",sizeof(c_Cadre *)*count_cadre);
  tmplist=lowsort;
  for(i=0;i<count_cadre;i++)
  {
   lowcadre[i]=(c_Cadre *)Xc_malloc("mv cadre",sizeof(c_Cadre));
   memcpy(lowcadre[i],tmplist->cadre,sizeof(c_Cadre));
   lowforme=IntersectPolygone(forme,lowcadre[i]->Forme);
   if (!lowcadre[i]->inside_color->transparency)
    SubtractPolygone(&forme,lowforme);
   lowcadre[i]->Forme=lowforme;
   tmplist=tmplist->NextCadre;
  }
 }
  
 DrawPolygone(forme,This->X_info);
 draw_desk_area_polygone(This,forme);
 FreePolygone(forme,DEL_POLYGONE);
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(highlist,NO_DEL_CADRE);

 if (count_cadre)
 {
  tmplist = lowlist = sort_asc_list_cadre(lowsort);
  for(i = count_cadre - 1; i >= 0; i--)
  {
   Xc_TRACE(("cadre pos: %d name: %s", i, lowcadre[i]->name));

   F(tmplist->cadre).drawcadre(tmplist->cadre, lowcadre[i]->Forme);
   F(tmplist->cadre).drawcontour(tmplist->cadre, lowcadre[i]->Forme,
				 This->Frame_Border_actif);
   FreePolygone(lowcadre[i]->Forme, DEL_POLYGONE);
   Xc_free(lowcadre[i]);
   tmplist = tmplist->NextCadre;
  }
  Xc_free(lowcadre);
 }
 free_list_cadre(lowlist, NO_DEL_CADRE);
 free_list_cadre(lowsort, NO_DEL_CADRE);
  
 tmplist = transp;
 if (tmplist)
 {
  do
  {
   list_cadre_low_high_priority(This, lcadre, &lowlist,
				&highlist, INTERS_CADRE);
   ftransp = IntersectPolygone(tmplist->cadre->Forme, oldforme);
   tmplist2 = highlist;
   while(tmplist2)
   {
    if (!tmplist2->cadre->inside_color->transparency)
     SubtractPolygone(&ftransp, tmplist2->cadre->Forme);
    tmplist2=tmplist2->NextCadre;
   }
   F(tmplist->cadre).drawcadre(tmplist->cadre,ftransp);
   F(tmplist->cadre).drawcontour(tmplist->cadre,ftransp,
				 This->Frame_Border_actif);
	  
   free_list_cadre(lowlist,NO_DEL_CADRE);
   free_list_cadre(highlist,NO_DEL_CADRE);
   FreePolygone(ftransp,DEL_POLYGONE);
   tmplist=tmplist->NextCadre;
  }
  while(tmplist);
  FreePolygone(oldforme,DEL_POLYGONE);
  free_list_cadre(transp,NO_DEL_CADRE);
 }
 delete_Cadre(This,lcadre,delcadre);
}

void redisplay_cadre(This,lcadre)
c_Page	*This;
l_Cadre	*lcadre;
{
 l_Cadre	*lowlist;
 l_Cadre	*lowsort;
 l_Cadre	*highlist;
 l_Cadre	*tmplist;
 l_Cadre	*tmplist2;
 l_Cadre	*transp;
 c_Cadre	*this_cadre;
 c_Cadre	**lowcadre = NULL;
 Polygone	*forme;
 Polygone	*lowforme;
 Polygone	*ftransp;
 Polygone	*oldforme;
 int		count_cadre;
 int		i;
 coord_t	x_orig;
 coord_t	y_orig;
  
 SET_WAIT_MODE_STATIC;
 this_cadre=lcadre->cadre;
 list_cadre_low_high_priority(This,lcadre,&lowlist,&highlist,INTERS_CADRE);
 forme=DuplicatePolygone(this_cadre->Forme);
 tmplist=highlist;
 transp=NULL;
 while(tmplist)
 {
  if (tmplist->cadre->inside_color->transparency)
   add_cadre_in_list(&transp,tmplist);
  else
   SubtractPolygone(&forme,tmplist->cadre->Forme);
  tmplist=tmplist->NextCadre;
 }
 if (transp)
  oldforme=DuplicatePolygone(this_cadre->Forme);
 lowsort=sort_desc_list_cadre(lowlist);
 tmplist=lowsort;
 count_cadre=0;
 while(tmplist)
 {
  count_cadre++;
  tmplist=tmplist->NextCadre;
 }
 if (count_cadre)
 {
  lowcadre=(c_Cadre **)Xc_malloc("mv lcadre",sizeof(c_Cadre *)*count_cadre);
  tmplist=lowsort;
  for(i=0;i<count_cadre;i++)
  {
   lowcadre[i]=(c_Cadre *)Xc_malloc("mv cadre",sizeof(c_Cadre));
   memcpy(lowcadre[i],tmplist->cadre,sizeof(c_Cadre));
   lowforme=IntersectPolygone(forme,lowcadre[i]->Forme);
   if (!lowcadre[i]->inside_color->transparency)
    SubtractPolygone(&forme,lowforme);
   lowcadre[i]->Forme=lowforme;
   tmplist=tmplist->NextCadre;
  }
 }
 x_orig = this_cadre->sOrig_X + this_cadre->saOrig_X;
 y_orig = this_cadre->sOrig_Y + this_cadre->saOrig_Y;
 F(this_cadre).calculorigine(this_cadre);
 F(this_cadre).calculforme(this_cadre,CONTOUR);
 F(This).calcul_select_handle(This);

 if (!this_cadre->inside_color->transparency)
  SubtractPolygone(&forme,this_cadre->Forme);
 DrawPolygone(forme,This->X_info);
 draw_desk_area_polygone(This,forme);
 FreePolygone(forme,DEL_POLYGONE);
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(highlist,NO_DEL_CADRE);
  
 if (count_cadre)
 {
  tmplist=lowlist=sort_asc_list_cadre(lowsort);
  for(i=count_cadre-1;i>=0;i--)
  {
   Xc_TRACE(("cadre pos: %d name: %s",i,lowcadre[i]->name));

   if (!this_cadre->inside_color->transparency)
    SubtractPolygone(&(lowcadre[i]->Forme),this_cadre->Forme);
   F(tmplist->cadre).drawcadre(tmplist->cadre,lowcadre[i]->Forme);
   F(tmplist->cadre).drawcontour(tmplist->cadre,lowcadre[i]->Forme,
				 This->Frame_Border_actif);
   FreePolygone(lowcadre[i]->Forme,DEL_POLYGONE);
   Xc_free(lowcadre[i]);
   tmplist=tmplist->NextCadre;
  }
  Xc_free(lowcadre);
 }
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(lowsort,NO_DEL_CADRE);

 tmplist=transp;
 if (tmplist)
 {
  SubtractPolygone(&oldforme,this_cadre->Forme);
  do
  {
   list_cadre_low_high_priority(This, tmplist, &lowlist,
				&highlist, INTERS_CADRE);
   ftransp=IntersectPolygone(tmplist->cadre->Forme,oldforme);
   tmplist2=highlist;
   while(tmplist2)
   {
    if (!tmplist2->cadre->inside_color->transparency)
     SubtractPolygone(&ftransp,tmplist2->cadre->Forme);
    tmplist2=tmplist2->NextCadre;
   }
   F(tmplist->cadre).drawcadre(tmplist->cadre,ftransp);
   F(tmplist->cadre).drawcontour(tmplist->cadre,ftransp,
				 This->Frame_Border_actif);
	  
   free_list_cadre(lowlist,NO_DEL_CADRE);
   free_list_cadre(highlist,NO_DEL_CADRE);
   FreePolygone(ftransp,DEL_POLYGONE);
   tmplist=tmplist->NextCadre;
  }
  while(tmplist);
  FreePolygone(oldforme,DEL_POLYGONE);
  free_list_cadre(transp,NO_DEL_CADRE);
 }
 if (this_cadre->object_type)
 {
  F(This->cursor).push_and_set_cursor(This->cursor,C_WAIT);
  if (!this_cadre->sAngle)
  {
   this_cadre->sConfig_X=this_cadre->sOrig_X+
    this_cadre->saOrig_X-x_orig;
   this_cadre->sConfig_Y=this_cadre->sOrig_Y+
    this_cadre->saOrig_Y-y_orig;
  }
  else
  {
   this_cadre->sConfig_X = 0;
   this_cadre->sConfig_Y = 0;
  }	  
  DISPLAY_CONFIGURE(this_cadre->object)(this_cadre->object,this_cadre,
					this_cadre->Forme);
  this_cadre->sConfig_X = 0;
  this_cadre->sConfig_Y = 0;
  F(This->cursor).pop_cursor(This->cursor);
 }
 draw_cadre(This,lcadre);
 UNSET_WAIT_MODE;
}


char *copy_cadre_name(This, name_org)
c_Page	*This;
char	*name_org;
{
 char	*name;
 int	pnt_name;
 int	i;
  
 Xc_TRACE(("copy_cadre_name(`%s')", name_org));

 name = (char *)Xc_malloc("cadre name", XcMAX_NAME_LENGTH);
 pnt_name = 0;

 if((i = Xstrnchr(name_org, '.')) >= 0)
 {
  char *ptr;

  for(ptr = name_org + i + 1; *ptr ; ptr++)
   if(!isdigit(*ptr))
   {
    i = strlen(name_org);
    break;
   }
 }
 else
  i = strlen(name_org);

 memcpy(name, name_org, i);

 do {
     sprintf(name + i, ".%d", pnt_name++);
    } while(F(This).get_cadre_by_name(This, name));

 return name;
}

l_Cadre *get_new_cadre(This)
c_Page	*This;
{ 
 l_Cadre *lcadre;
 c_Cadre *frame;
 int   priority;
 int	pnt_name;

 if((frame = (c_Cadre *)NEW(c_Cadre)(This->BaseStd)) == NULL) return NULL;
 priority = F(This).get_highest_priority(This);
 
 pnt_name = This->nb_cadre;
 do
 {
  sprintf(frame->name, "%s %d", defaultFrameName, pnt_name++);
 }
 while(F(This).add_cadre(This, frame->name,
			 priority, frame) == XC_NAME_ALREADY_EXIST);

 lcadre = F(This).get_cadre_by_name(This, frame->name);
 return lcadre;
}

l_Cadre *get_copy_cadre(This,lcadre,cadre)
c_Page	*This;
l_Cadre	*lcadre;
c_Cadre	*cadre;
{ 
 char	*name;
 int   priority;
  
 name = F(This).copy_cadre_name(This, lcadre->name);
 priority = F(This).get_highest_priority(This);
 F(This).add_cadre(This, name, priority, cadre);
 lcadre = F(This).get_cadre_by_name(This, name);
 Xc_free(name);
 return lcadre;
}

l_Cadre *get_cadre_by_xy(This,X,Y)
c_Page *This;
int X;
int Y;
{
 l_Cadre *list_cadre;
 l_Cadre *ret_cadre;
 l_Point *lpoint;
 l_Point *lstpnt;
 l_Point tmppoint;
 int ret_dist;
 int nb_point;
 int px,py;
  
 list_cadre=This->ListCadre;
 ret_cadre=NULL;
 while(list_cadre)
 {
  if (F(list_cadre->cadre).isincadre(list_cadre->cadre,X,Y))
  {
   if (ret_cadre)
   {
    if (ret_cadre->priority<list_cadre->priority)
     ret_cadre=list_cadre;
   }
   else
    ret_cadre=list_cadre;
  }
  else
  {
   tmppoint.X = X-list_cadre->cadre->Forme->Orig_X;
   tmppoint.Y = Y-list_cadre->cadre->Forme->Orig_Y;
   lpoint = lstpnt = list_cadre->cadre->ListPoint;
   nb_point = 0;
   while(lpoint)
   {
    if (lpoint->NextPoint && lpoint->NextPoint->cont)
     nb_point++;
    else
    {
     if (nb_point<2)
     {
      if (nb_point == 0)
      {
       px=list_cadre->cadre->Forme->Orig_X;
       py=list_cadre->cadre->Forme->Orig_Y;
       if (lpoint->X+px<X+HANDLE_X_ACT && 
	   lpoint->X+px>X-HANDLE_X_ACT &&
	   lpoint->Y+py<Y+HANDLE_Y_ACT && 
	   lpoint->Y+py>Y-HANDLE_Y_ACT )
       {
	if (ret_cadre)
	{
	 if (ret_cadre->priority<list_cadre->priority)
	  ret_cadre=list_cadre;
	}
	else
	 ret_cadre=list_cadre;
	break;
       }
      }
      else
      {
       ret_dist = distance_line_point( lpoint, lstpnt, &tmppoint);
       if (ret_dist >= 0 && ret_dist < 36)
       {
	if (ret_cadre)
	{
	 if (ret_cadre->priority<list_cadre->priority)
	  ret_cadre=list_cadre;
	}
	else
	 ret_cadre=list_cadre;
	break;
       }
      }
     }
     nb_point = 0;
    }
    lstpnt = lpoint;
    lpoint = lpoint->NextPoint;
   }
  }
  list_cadre=list_cadre->NextCadre;
 }
 return ret_cadre;
}

l_Cadre *get_cadre_and_point_by_xy(This,listsel,lstpnt,X,Y)
c_Page	*This;
l_Cadre	*listsel;
l_Point	***lstpnt;
int	X;
int	Y;
{
 boolean	found;
 l_Cadre	*tmplcadre;
 l_Cadre	*retcadre;
 l_Point	*tmplpoint;
 l_Cadre	*foundlcadre = NULL;
 l_Point	*foundlpoint = NULL;
 l_Point	*foundprevlpoint = NULL;
 l_Point	*foundnextlpoint = NULL;
 l_Point	*savelpoint;
 l_Point	*nextlpoint;
 l_Point	*prevlpoint;
 l_Point	**listpnt;
 int		px,py;

 if (!listsel)
 {
  listsel=This->SelectedCadre;
  if (!listsel)
  {
   return NULL;
  }
 }
 found=FALSE;
  
 tmplcadre=listsel;
 while(!found && tmplcadre)
 {
  px=tmplcadre->cadre->Forme->Orig_X;
  py=tmplcadre->cadre->Forme->Orig_Y;
  foundprevlpoint=NULL;
  foundnextlpoint=NULL;
  savelpoint=prevlpoint=NULL;
  tmplpoint=tmplcadre->cadre->ListPoint;
  while(!found && tmplpoint)
  {
   if (!tmplpoint->cont)
   {
    savelpoint=tmplpoint;
   }    
   if (tmplpoint->X+px<X+HANDLE_X_ACT && 
       tmplpoint->X+px>X-HANDLE_X_ACT &&
       tmplpoint->Y+py<Y+HANDLE_Y_ACT && 
       tmplpoint->Y+py>Y-HANDLE_Y_ACT )
   {
    found = TRUE;
    foundlcadre = tmplcadre;
    foundlpoint = tmplpoint;
    if (!tmplpoint->NextPoint || !tmplpoint->NextPoint->cont)
     foundnextlpoint=savelpoint;
    else
     foundnextlpoint=tmplpoint->NextPoint;
    if (!tmplpoint->cont)
    {
     nextlpoint=tmplpoint;
     while (nextlpoint->NextPoint && nextlpoint->NextPoint->cont)
     { 
      nextlpoint=nextlpoint->NextPoint;
     }
     foundprevlpoint=nextlpoint;
    }
    else
     foundprevlpoint=prevlpoint;
   }
   prevlpoint=tmplpoint;
   tmplpoint=tmplpoint->NextPoint;
  }
  tmplcadre=tmplcadre->NextCadre;
 }
 if (!found)
 {
  return NULL;
 }
 listpnt=(l_Point **)Xc_malloc("point 1",sizeof(l_Point *)*3);
 listpnt[0]=foundlpoint;
 if (!foundprevlpoint || foundprevlpoint==foundlpoint)
  listpnt[1]=NULL;
 else
  listpnt[1]=foundprevlpoint;
 if (!foundnextlpoint || foundprevlpoint==foundnextlpoint)
  listpnt[2]=NULL;
 else
  listpnt[2]=foundnextlpoint;
  
 retcadre=search_cadre_by_name(This, foundlcadre->name);
 *lstpnt=listpnt;
  
 return retcadre;
}

l_Cadre *get_cadre_and_point_by_line(This,listsel,lstpnt,X,Y)
c_Page	*This;
l_Cadre	*listsel;
l_Point	***lstpnt;
int	X;
int	Y;
{
 l_Cadre	*slistsel;
 l_Cadre	*retcadre;
 l_Cadre	*found_cadre = NULL;
 l_Point	*lpoint;
 l_Point	*spoint;
 l_Point	*sspoint;
 l_Point	*A;
 l_Point	*B;
 l_Point	M;
 l_Point	*found_A = NULL;
 l_Point	*found_B = NULL;
 l_Point	**listpnt;
 boolean	found;
 int		ret_dist;
 int		min_dist;
  
 if (!listsel)
 {
  listsel=This->SelectedCadre;
  if (!listsel)
  {
   return NULL;
  }
 }
 min_dist=36;
 found=FALSE;
 slistsel=listsel;
 while(slistsel)
 {
  M.X=X-slistsel->cadre->Forme->Orig_X;
  M.Y=Y-slistsel->cadre->Forme->Orig_Y;
    
  sspoint=spoint=lpoint=slistsel->cadre->ListPoint;
  lpoint=lpoint->NextPoint;
  while(1)
  {
   if (!lpoint || lpoint->cont==FALSE)
   {
    A=spoint;
    B=sspoint;
    if (A!=B)
    {
     ret_dist=distance_line_point(A,B,&M);
     if (ret_dist>=0)
     {
      if (ret_dist<min_dist)
      {
       found_A=A;
       found_B=B;
       found_cadre=slistsel;
       min_dist=ret_dist;
       found=TRUE;
      }
     }
    }
    if (lpoint)
    {
     sspoint=lpoint;
    }
    else
    {
     break;
    }
   }
   else
   {
    A=spoint;
    B=lpoint;
    ret_dist=distance_line_point(A,B,&M);
    if (ret_dist>=0)
    {
     if (ret_dist<min_dist)
     {
      found_A=A;
      found_B=B;
      found_cadre=slistsel;
      min_dist=ret_dist;
      found=TRUE;
     }
    }
   }
   spoint=lpoint;
   lpoint=lpoint->NextPoint;
  }
  slistsel=slistsel->NextCadre;
 }
 if (found)
 {
  listpnt=(l_Point **)Xc_malloc("pnt line",sizeof(l_Point *)*2);
  listpnt[0]=found_A;
  listpnt[1]=found_B;
  *lstpnt=listpnt;
  retcadre=found_cadre;
 }
 else
 {
  retcadre=NULL;
  *lstpnt=NULL;
 }
 return retcadre;
}

/* ----------------------------------------------------------------- ** 
** GET HANDLE SUBROUTINES                                            ** 
** ----------------------------------------------------------------- */

typedef struct {
		int		x_point;
		int		y_point;
		coord_t	x_src;
		coord_t	y_src;
		coord_t	x_dst;
		coord_t	y_dst;
		angle_t	angle;
	       } handle_t;

int Is_in_handle();
int Convert_angle();

l_Point	*get_handle_by_xy(This,X,Y,angle,rlcadre)
c_Page	*This;
int	X;
int	Y;
int	*angle;      
l_Cadre	**rlcadre;
{
 int		i,pos;
 int		x_min,y_min;
 int		x_max,y_max;
 int		x_mid,y_mid;
 int		iclx,icly,ichx,ichy;
 coord_t	px,py;
 coord_t	x1,y1,x2,y2,x3,y3,x4,y4;
 l_Point	*foundlpoint;
 handle_t	handles[8];
 l_Cadre	*lcadre;
 l_Cadre	duplcadre;
 l_Cadre	*retlcadre;
 c_Cadre	*cadre;
 double	sAngle;
  
 if (!This->SelectedCadre)
  return NULL;

 if (This->select_group)
 {
  cadre=This->SelectedCadre->cadre;
  if ((!This->SelectedCadre->NextCadre) && 
      ((!cadre->ListPoint->NextPoint) || 
       (!cadre->ListPoint->NextPoint->NextPoint) ||
       (!cadre->sWidth) || (!cadre->sHeight)))
  {
   if (!angle)
    return NULL;
   lcadre=F(This).get_cadre_and_point_by_xy(This,This->SelectedCadre,
					    &foundlpoint,X,Y);
   if (lcadre)
   {
    *angle=9;
    return foundlpoint;
   }
   else
   {
    lcadre=F(This).get_cadre_and_point_by_line(This,
					       This->SelectedCadre,
					       &foundlpoint,X,Y);
    if (lcadre) 
    {
     *angle=10;
     return foundlpoint;
    }
   }
   return NULL;
  }
  if (This->select_type)
   return NULL;
      
  x_min=This->select_sX_min/This->sScale;
  y_min=This->select_sY_min/This->sScale;
  x_max=This->select_sX_max/This->sScale;
  y_max=This->select_sY_max/This->sScale;
  x_mid=(x_max+x_min)>>1;
  y_mid=(y_max+y_min)>>1;
      
  handles[0].x_point=handles[6].x_point=handles[7].x_point = x_min-RHANDLE_X-1;
  handles[0].x_src=handles[6].x_src=handles[7].x_src = This->select_sX_min;
  handles[0].x_dst=handles[6].x_dst=handles[7].x_dst = This->select_sX_max;
      
  handles[2].x_point=handles[3].x_point=handles[4].x_point = x_max+RHANDLE_X+1;
  handles[2].x_src=handles[3].x_src=handles[4].x_src = This->select_sX_max;
  handles[2].x_dst=handles[3].x_dst=handles[4].x_dst = This->select_sX_min;

  handles[1].x_point=handles[5].x_point=x_mid;
  handles[1].x_src=handles[5].x_dst=handles[1].x_src = handles[5].x_dst=
   (This->select_sX_min+This->select_sX_max)/2;

  handles[0].y_point=handles[1].y_point=handles[2].y_point = y_min-RHANDLE_Y-1;
  handles[0].y_src=handles[1].y_src=handles[2].y_src = This->select_sY_min;
  handles[0].y_dst=handles[1].y_dst=handles[2].y_dst = This->select_sY_max;

  handles[4].y_point=handles[5].y_point=handles[6].y_point = y_max+RHANDLE_Y+1;
  handles[4].y_src=handles[5].y_src=handles[6].y_src = This->select_sY_max;
  handles[4].y_dst=handles[5].y_dst=handles[6].y_dst = This->select_sY_min;

  handles[3].y_point=handles[7].y_point=y_mid;
  handles[3].y_src=handles[7].y_dst=handles[3].y_src=handles[7].y_dst=
   (This->select_sY_min+This->select_sY_max)/2;

  handles[0].angle=handles[4].angle=SCALE_FROM_DEGREES(135);
  handles[1].angle=handles[5].angle=SCALE_FROM_DEGREES(90);
  handles[2].angle=handles[6].angle=SCALE_FROM_DEGREES(45);
  handles[3].angle=handles[7].angle=SCALE_FROM_DEGREES(0);

  pos=Is_in_handle(handles,X,Y);
  if (pos==-1)
  {
   return NULL;
  }
  else
  {
   foundlpoint=(l_Point *)Xc_malloc("pnt in handle",sizeof(l_Point)*2);
   foundlpoint[0].sX=handles[pos].x_src;
   foundlpoint[0].sY=handles[pos].y_src;
   foundlpoint[0].X=foundlpoint[0].sY/This->sScale;
   foundlpoint[0].Y=foundlpoint[0].sY/This->sScale;
   foundlpoint[0].cont=(pos==1?FALSE:(pos==5?FALSE:TRUE));
   foundlpoint[1].sX=handles[pos].x_dst;
   foundlpoint[1].sY=handles[pos].y_dst;
   foundlpoint[1].X=foundlpoint[1].sX/This->sScale;
   foundlpoint[1].Y=foundlpoint[1].sY/This->sScale;
   foundlpoint[1].cont=(pos==3?FALSE:(pos==7?FALSE:TRUE));
   if (angle)
    *angle=Convert_angle(handles[pos].angle);
   if (rlcadre)
    *rlcadre=NULL;
   return foundlpoint;	  
  }
 }
 else
 {
  lcadre=This->SelectedCadre;
  duplcadre.NextCadre=NULL;
  duplcadre.name=lcadre->name;
  while(lcadre)
  {
   cadre=lcadre->cadre;
   if ((!cadre->ListPoint->NextPoint) || 
       (!cadre->ListPoint->NextPoint->NextPoint) ||
       (!cadre->sWidth) || (!cadre->sHeight))
   {
    if (!angle)
     return NULL;
    duplcadre.cadre=cadre;
    retlcadre=F(This).get_cadre_and_point_by_xy(This,&duplcadre,
						&foundlpoint,X,Y);
    if (retlcadre)
    {
     if (retlcadre->cadre->ListPoint->NextPoint)
      *angle=9; 
     return foundlpoint;
    }
    else
    {
     retlcadre=F(This).get_cadre_and_point_by_line(This,
						   &duplcadre,
						   &foundlpoint,X,Y);
     if (retlcadre) 
     {
      *angle=10;
      return foundlpoint;
     }
    }
   }
   else 
   {
    if (This->select_type)
    {
     lcadre=lcadre->NextCadre;
     continue;
    }
	    
    sAngle=SCALE_TO_RADIANS(cadre->sAngle);
	    
    iclx=(int)((-(RHANDLE_X+1)*cos(sAngle))/2+.5);
    icly=(int)(( (RHANDLE_X+1)*sin(sAngle))/2+.5);
    ichx=(int)((-(RHANDLE_Y+1)*sin(sAngle))/2+.5);
    ichy=(int)((-(RHANDLE_Y+1)*cos(sAngle))/2+.5);
	    
    px=cadre->sOrig_X;
    py=cadre->sOrig_Y;
    x1=cadre->saOrig_X;
    y1=cadre->saOrig_Y;
    x2=x1+(coord_t)(cos(sAngle)*(double)cadre->saWidth+.5);
    y2=y1-(coord_t)(sin(sAngle)*(double)cadre->saWidth+.5);
    x3=x2+(coord_t)(sin(sAngle)*(double)cadre->saHeight+.5);
    y3=y2+(coord_t)(cos(sAngle)*(double)cadre->saHeight+.5);
    x4=x3-(coord_t)(cos(sAngle)*(double)cadre->saWidth+.5);
    y4=y3+(coord_t)(sin(sAngle)*(double)cadre->saWidth+.5);
	    
    handles[0].x_point=(x1+px)/cadre->sScale+iclx+ichx;
    handles[0].y_point=(y1+py)/cadre->sScale+icly+ichy;
    handles[4].x_point=(x3+px)/cadre->sScale-iclx-ichx;
    handles[4].y_point=(y3+py)/cadre->sScale-icly-ichy;
    handles[0].x_src=handles[4].x_dst=x1+px;
    handles[0].y_src=handles[4].y_dst=y1+py;
    handles[0].x_dst=handles[4].x_src=x3+px;
    handles[0].y_dst=handles[4].y_src=y3+py;
	    
    handles[2].x_point=(x2+px)/cadre->sScale-iclx+ichx;
    handles[2].y_point=(y2+py)/cadre->sScale-icly+ichy;
    handles[6].x_point=(x4+px)/cadre->sScale+iclx-ichx;
    handles[6].y_point=(y4+py)/cadre->sScale+icly-ichy;
    handles[2].x_src=handles[6].x_dst=x2+px;
    handles[2].y_src=handles[6].y_dst=y2+py;
    handles[2].x_dst=handles[6].x_src=x4+px;
    handles[2].y_dst=handles[6].y_src=y4+py;
	    
    handles[1].x_point=((x1+x2)/2+px)/cadre->sScale+ichx;
    handles[1].y_point=((y1+y2)/2+py)/cadre->sScale+ichy;
    handles[5].x_point=((x3+x4)/2+px)/cadre->sScale-ichx;
    handles[5].y_point=((y3+y4)/2+py)/cadre->sScale-ichy;
    handles[1].x_src=handles[5].x_dst=(x1+x2)/2+px;
    handles[1].y_src=handles[5].y_dst=(y1+y2)/2+py;
    handles[1].x_dst=handles[5].x_src=(x3+x4)/2+px;
    handles[1].y_dst=handles[5].y_src=(y3+y4)/2+py;
	    
    handles[3].x_point=((x2+x3)/2+px)/cadre->sScale-iclx;
    handles[3].y_point=((y2+y3)/2+py)/cadre->sScale-icly;
    handles[7].x_point=((x4+x1)/2+px)/cadre->sScale+iclx;
    handles[7].y_point=((y4+y1)/2+py)/cadre->sScale+icly;
    handles[3].x_src=handles[7].x_dst=(x2+x3)/2+px;
    handles[3].y_src=handles[7].y_dst=(y2+y3)/2+py;
    handles[3].x_dst=handles[7].x_src=(x4+x1)/2+px;
    handles[3].y_dst=handles[7].y_src=(y4+y1)/2+py;
	    
    handles[0].angle=handles[4].angle=SCALE_FROM_DEGREES(135);
    handles[1].angle=handles[5].angle=SCALE_FROM_DEGREES(90);
    handles[2].angle=handles[6].angle=SCALE_FROM_DEGREES(45);
    handles[3].angle=handles[7].angle=SCALE_FROM_DEGREES(0);
	    
    for(i=0;i<8;i++)
    {
     handles[i].angle += cadre->sAngle;
    }
	    
    pos=Is_in_handle(handles,X,Y);
    if (pos!=-1)
    {
     foundlpoint=(l_Point *)Xc_malloc("pnt in handle",
				      sizeof(l_Point)*2);
     foundlpoint[0].sX=handles[pos].x_src;
     foundlpoint[0].sY=handles[pos].y_src;
     foundlpoint[0].X=foundlpoint[0].sY/cadre->sScale;
     foundlpoint[0].Y=foundlpoint[0].sY/cadre->sScale;
     foundlpoint[0].cont=(pos==1?FALSE:(pos==5?FALSE:TRUE));
     foundlpoint[1].sX=handles[pos].x_dst;
     foundlpoint[1].sY=handles[pos].y_dst;
     foundlpoint[1].X=foundlpoint[1].sX/cadre->sScale;
     foundlpoint[1].Y=foundlpoint[1].sY/cadre->sScale;
     foundlpoint[1].cont=(pos==3?FALSE:(pos==7?FALSE:TRUE));
     if (angle)
      *angle=Convert_angle(handles[pos].angle);
     if (rlcadre)
      *rlcadre=lcadre;
     return foundlpoint;	  
    }
   }
   lcadre=lcadre->NextCadre;
  }
 }
 return NULL;
}

int Is_in_handle(handles,X,Y)
handle_t *handles;
int	 X;
int	 Y;
{
 int	i;
  
 for(i=0;i<8;i++)
 {
  if (handles[i].x_point >= X-RHANDLE_X-RHANDLE_X_ACT &&
      handles[i].x_point <= X+RHANDLE_X+RHANDLE_X_ACT &&
      handles[i].y_point >= Y-RHANDLE_Y-RHANDLE_Y_ACT &&
      handles[i].y_point <= Y+RHANDLE_Y+RHANDLE_Y_ACT)
   return i;
 }
 return -1;
}

int Convert_angle(anglet)
angle_t	anglet;
{
 static int conv[9]={5,4,3,2,1,8,7,6,5};
 int angle;
  
 angle=(int)SCALE_FROM_DEGREES(anglet)%180;
 if (angle<0)
  angle+=180;
 angle=((angle<<2)+45)/90;
 return conv[angle];
}

static l_Cadre *list_cadre_by_page(This, num_page, orig_x, orig_y)
c_Page	*This;
int	num_page;
coord_t	*orig_x;
coord_t	*orig_y;
{
 coord_t  x_min;
 coord_t  x_max;
 coord_t  y_min;
 coord_t  y_max;
 coord_t  c_x_min;
 coord_t  c_x_max;
 coord_t  c_y_min;
 coord_t  c_y_max;
 int	   page_x;
 int	   page_y;
 l_Cadre  *ret_lcadre;
 l_Cadre  *temp_lcadre;
 l_Cadre  *lcadre;
 l_Point  *lpoint;
 boolean  in_page;

 page_x = num_page % This->Page_Contour.nb_page_horizontal;
 page_y = num_page / This->Page_Contour.nb_page_horizontal;
  
 x_min = This->Page_Contour.sOrig_X + This->Page_Contour.sWidth*page_x;
 y_min = This->Page_Contour.sOrig_Y + This->Page_Contour.sHeight*page_y;
 x_max = x_min + This->Page_Contour.sWidth;
 y_max = y_min + This->Page_Contour.sHeight;

 if (orig_x)
  *orig_x = x_min;
 if (orig_y)
  *orig_y = y_min;
  
 if (page_y >= This->Page_Contour.nb_page_vertical)
  return NULL;

 ret_lcadre = NULL;
 lcadre = This->ListCadre;
 while(lcadre)
 {
  c_x_min = lcadre->cadre->sOrig_X + This->sOrig_X;
  c_y_min = lcadre->cadre->sOrig_Y + This->sOrig_Y;
  c_x_max = c_x_min + lcadre->cadre->sWidth;
  c_y_max = c_y_min + lcadre->cadre->sHeight;
  if (!((c_x_min<x_min && c_x_max<x_min) || 
	(c_x_min>x_max && c_x_max>x_max) ||
	(c_y_min<y_min && c_y_max<y_min) ||
	(c_y_min>y_max && c_y_max>y_max)))
  {
   lpoint = lcadre->cadre->ListPoint;
   in_page = FALSE;
   while(lpoint && !in_page)
   {
    if (lpoint->sX+c_x_min>=x_min &&
	lpoint->sX+c_x_min<=x_max &&
	lpoint->sY+c_y_min>=y_min &&
	lpoint->sY+c_y_min<=y_max)
     in_page = TRUE;
    lpoint = lpoint->NextPoint;
   }
   if (!in_page &&
       (( c_x_min<=x_min && c_x_max>=x_max) || 
	( c_y_min<=y_min && c_y_max>=y_max)))
    in_page = TRUE;
   if (in_page)
   {
    temp_lcadre = (l_Cadre *)Xc_malloc("ls/pg",sizeof(l_Cadre));
    memcpy(temp_lcadre, lcadre, sizeof(l_Cadre));
    temp_lcadre->NextCadre = ret_lcadre;
    ret_lcadre = temp_lcadre;
   }
  }
  lcadre = lcadre->NextCadre;
 }
 return ret_lcadre;
}

l_Cadre *list_cadre_inters(This,my_cadre)
c_Page *This;
l_Cadre *my_cadre;
{
 l_Cadre *list_cdr;
 l_Cadre *ret_cdr;
 l_Cadre *lcadre;
 boolean prems;
  
 prems=TRUE;
 lcadre=This->ListCadre;
 list_cdr=ret_cdr=NULL;
 while(lcadre)
 {
  if ((lcadre->cadre==my_cadre->cadre) ||
      !is_inters(lcadre->cadre->Forme,my_cadre->cadre->Forme))
  {
   lcadre=lcadre->NextCadre;
  }
  else
  {
   if (prems)
   {
    list_cdr=(l_Cadre *)Xc_malloc("lcadre inters",sizeof(l_Cadre));
    ret_cdr=list_cdr;
    memcpy(list_cdr,lcadre,sizeof(l_Cadre));
    prems=FALSE;
   }
   else
   {
    list_cdr->NextCadre=(l_Cadre *)Xc_malloc("lcadre inters",sizeof(l_Cadre));
    list_cdr=list_cdr->NextCadre;
    memcpy(list_cdr,lcadre,sizeof(l_Cadre));
   }
   lcadre=lcadre->NextCadre;
  }
 }
 if (list_cdr)
 {
  list_cdr->NextCadre=NULL;
  return ret_cdr;
 }
 else
  return NULL;
}

void select_cadre(This, lcadre, draw)
c_Page	*This;
l_Cadre	*lcadre;
boolean	draw;
{
 l_Cadre	*duplist;
 l_Cadre	*svglist;
 l_Cadre	*newone;
 l_Cadre	*seektend;
 boolean	reaff=FALSE;
 coord_t	x_min,y_min,x_max,y_max;
  
 if (!lcadre->cadre->selected)
 {
  duplist = (l_Cadre *)duplicate_list_cadre(This->SelectedCadre);
  newone = (l_Cadre *)Xc_malloc("sel lcadre",sizeof(l_Cadre));
  memcpy(newone, lcadre, sizeof(l_Cadre));
  newone->NextCadre = NULL;
  lcadre->cadre->selected = TRUE;
  seektend = This->SelectedCadre;
  if (seektend)
  {
   while(seektend->NextCadre)
    seektend = seektend->NextCadre;

   seektend->NextCadre = newone;
  }
  else
   This->SelectedCadre=newone;

  get_limit_list_cadre(This->SelectedCadre, &x_min, &y_min, &x_max, &y_max);
  if (draw)
  {
   if (This->select_type || !This->select_group)
    DisplaySelectedCadre(lcadre, This->select_type);
   else
    if (This->select_sX_min != x_min || This->select_sY_min != y_min ||
	This->select_sX_max != x_max || This->select_sY_max != y_max)
    {
     if (seektend)
     {
      svglist = This->SelectedCadre;
      This->SelectedCadre = duplist;
      F(This).undisplay_selected_lcadre(This);
      This->SelectedCadre = svglist;
     }
     reaff = TRUE;
    }
  }
  This->select_sX_min = x_min;
  This->select_sY_min = y_min;
  This->select_sX_max = x_max;
  This->select_sY_max = y_max;
  if (reaff)
   F(This).display_selected_lcadre(This);
  free_list_cadre(duplist, NO_DEL_CADRE);
 }
}

void deselect_cadre(This, lcadre, draw)
c_Page	*This;
l_Cadre	*lcadre;
boolean	draw;
{
 l_Cadre	*seeklcd;
 l_Cadre	*preclcd;
 boolean	found=FALSE;
 boolean	reaff=FALSE;
 coord_t	x_min,y_min,x_max,y_max;
  
 preclcd=seeklcd=This->SelectedCadre;
 while(seeklcd)
 {
  if (seeklcd->cadre==lcadre->cadre)
  {
   found=TRUE;
   break;
  }
  preclcd=seeklcd;
  seeklcd=seeklcd->NextCadre;
 }
 if (draw)
 {
  if (This->select_type || !This->select_group)
   UndisplaySelectedCadre(lcadre,This->select_type);
  else
  {
   F(This).undisplay_selected_lcadre(This);
   reaff=TRUE;
  }
 }
 if (found)
 {
  if (preclcd==seeklcd)
  {
   This->SelectedCadre=seeklcd->NextCadre;
  }
  else
  {
   preclcd->NextCadre=seeklcd->NextCadre;
  }
  Xc_free(seeklcd);
    
  get_limit_list_cadre(This->SelectedCadre,&x_min,&y_min,&x_max,&y_max);
  This->select_sX_min=x_min;
  This->select_sY_min=y_min;
  This->select_sX_max=x_max;
  This->select_sY_max=y_max;
  if (reaff)
   F(This).display_selected_lcadre(This);
  lcadre->cadre->selected=FALSE;
 }
}

void select_all_cadre(This)
c_Page	*This;
{
 l_Cadre	*lcadre;
 coord_t	x_min,y_min,x_max,y_max;
  
 UndisplaySelectedlCadre(This);
 This->SelectedCadre=duplicate_list_cadre(This->ListCadre);
 lcadre=This->SelectedCadre;
 while(lcadre)
 {
  lcadre->cadre->selected=TRUE;
  lcadre=lcadre->NextCadre;
 }
 get_limit_list_cadre(This->SelectedCadre,&x_min,&y_min,&x_max,&y_max);
 This->select_sX_min=x_min;
 This->select_sY_min=y_min;
 This->select_sX_max=x_max;
 This->select_sY_max=y_max;
 DisplaySelectedlCadre(This);
}

void deselect_all_cadre(This,draw)
c_Page	*This;
boolean	draw;
{
 l_Cadre	*lcadre;
  
 if (draw)
  UndisplaySelectedlCadre(This);
 lcadre=This->SelectedCadre;
 while(lcadre)
 {
  lcadre->cadre->selected=FALSE;
  lcadre=lcadre->NextCadre;
 }
 free_list_cadre(This->SelectedCadre,NO_DEL_CADRE);
 This->SelectedCadre=NULL;
 This->select_sX_min=This->select_sY_min=
  This->select_sX_max=This->select_sY_max=0;
}

void list_cadre_low_high_priority(This,lcadre,lowlist,highlist,isall)
c_Page	*This;
l_Cadre	*lcadre;
l_Cadre	**lowlist;
l_Cadre	**highlist;
boolean	isall;
{
 l_Cadre *list_cadre;
 l_Cadre *tmp_cadre;
 l_Cadre *lowcadre;
 l_Cadre *highcadre;
 int	  prior;
  
 if (isall==INTERS_CADRE)
  list_cadre=(l_Cadre *)list_cadre_inters(This,lcadre);
 else
 {
  list_cadre=(l_Cadre *)duplicate_list_cadre(This->ListCadre);
  delete_cadre_in_list(&list_cadre,lcadre->cadre);
 }
 tmp_cadre=list_cadre;
 prior=lcadre->priority;
 lowcadre=highcadre=NULL;
 *lowlist= *highlist=NULL;
 while(tmp_cadre)
 {
  if (tmp_cadre->priority<prior)
  {
   if (lowcadre)
   {
    lowcadre->NextCadre=(l_Cadre *)Xc_malloc("low cadre",sizeof(l_Cadre));
    memcpy(lowcadre->NextCadre,tmp_cadre,sizeof(l_Cadre));
    lowcadre=lowcadre->NextCadre;
    lowcadre->NextCadre=NULL;
   }
   else
   {
    lowcadre=(l_Cadre *)Xc_malloc("low cadre",sizeof(l_Cadre));
    memcpy(lowcadre,tmp_cadre,sizeof(l_Cadre));
    *lowlist=lowcadre;
    lowcadre->NextCadre=NULL;
   }
  }
  else
  {
   if (highcadre)
   {
    highcadre->NextCadre=(l_Cadre *)Xc_malloc("high cadre",sizeof(l_Cadre));
    memcpy(highcadre->NextCadre,tmp_cadre,sizeof(l_Cadre));
    highcadre=highcadre->NextCadre;
    highcadre->NextCadre=NULL;
   }
   else
   {
    highcadre=(l_Cadre *)Xc_malloc("high cadre",sizeof(l_Cadre));
    memcpy(highcadre,tmp_cadre,sizeof(l_Cadre));
    *highlist=highcadre;
    highcadre->NextCadre=NULL;
   }
  }
  tmp_cadre=tmp_cadre->NextCadre;
 }
 free_list_cadre(list_cadre,NO_DEL_CADRE);
}

l_Cadre *sort_asc_list_cadre(list)
l_Cadre *list;
{
 l_Cadre *tmplist;
 l_Cadre **tablist;
 l_Cadre *swplist;
 boolean utils;
 int priority = 0;
 int nb_cadre;
 int pivo;
 int i,j;
  
 tmplist = list;
 nb_cadre = 0;
 utils = TRUE;
 while(tmplist)
 {
  if (nb_cadre)
  {
   if (priority > tmplist->priority)
    utils = FALSE;
  }
  priority = tmplist->priority;
  nb_cadre++;
  tmplist=tmplist->NextCadre;
 }
 if (utils)
 {
  tmplist=duplicate_list_cadre(list);
  return tmplist;
 }
 tablist=(l_Cadre **)Xc_malloc("sort lcadre",sizeof(l_Cadre *)*nb_cadre);
 tmplist=list;
 i=0;
 while(tmplist)
 {
  tablist[i]=(l_Cadre *)Xc_malloc("sort cadre",sizeof(l_Cadre));
  memcpy(tablist[i++],tmplist,sizeof(l_Cadre));
  tmplist=tmplist->NextCadre;
 }
 for(i=0;i<nb_cadre-1;i++)
 {
  priority=tablist[i]->priority;
  pivo=i;
  for(j=i+1;j<nb_cadre;j++)
  {
   if (tablist[j]->priority<priority)
   {
    priority=tablist[j]->priority;
    pivo=j;
   }
  }
  swplist=tablist[pivo];
  tablist[pivo]=tablist[i];
  tablist[i]=swplist;
 }
 for(i=0;i<nb_cadre-1;i++)
 {
  tablist[i]->NextCadre=tablist[i+1];
 }
 tablist[i]->NextCadre=NULL;
 tmplist=tablist[0];
 Xc_free(tablist);
 return tmplist;
}

l_Cadre *sort_desc_list_cadre(list)
l_Cadre *list;
{
 l_Cadre *tmplist;
 l_Cadre **tablist;
 l_Cadre *swplist;
 boolean utils;
 int priority = 0;
 int nb_cadre;
 int pivo;
 int i,j;
  
 tmplist=list;
 nb_cadre=0;
 utils=TRUE;
 while(tmplist)
 {
  if (nb_cadre)
  {
   if (priority<tmplist->priority)
    utils=FALSE;
  }
  priority=tmplist->priority;
  nb_cadre++;
  tmplist=tmplist->NextCadre;
 }
 if (utils)
 {
  tmplist=duplicate_list_cadre(list);
  return tmplist;
 }
 tablist=(l_Cadre **)Xc_malloc("sort lcadre",sizeof(l_Cadre *)*nb_cadre);
 tmplist=list;
 i=0;
 while(tmplist)
 {
  tablist[i]=(l_Cadre *)Xc_malloc("sort cadre",sizeof(l_Cadre));
  memcpy(tablist[i++],tmplist,sizeof(l_Cadre));
  tmplist=tmplist->NextCadre;
 }
 for(i=0;i<nb_cadre;i++)
 {
  priority=tablist[i]->priority;
  pivo=i;
  for(j=i+1;j<nb_cadre;j++)
  {
   if (tablist[j]->priority>priority)
   {
    priority=tablist[j]->priority;
    pivo=j;
   }
  }
  swplist=tablist[pivo];
  tablist[pivo]=tablist[i];
  tablist[i]=swplist;
 }
 for(i=0;i<nb_cadre-1;i++)
 {
  tablist[i]->NextCadre=tablist[i+1];
 }
 tablist[i]->NextCadre=NULL;
 tmplist=tablist[0];
 Xc_free(tablist);
 return tmplist;
}

void calcul_select_handle(This)
c_Page	*This;
{
 coord_t	x_min,y_min,x_max,y_max;
  
 get_limit_list_cadre(This->SelectedCadre,&x_min,&y_min,&x_max,&y_max);
 This->select_sX_min=x_min;
 This->select_sY_min=y_min;
 This->select_sX_max=x_max;
 This->select_sY_max=y_max;
}

void change_page_origin(This,x,y)
c_Page	*This;
int	x;
int	y;
{
 l_Cadre	*lcadre;
 int		delta_x;
 int		delta_y;
 coord_t	delta_sx;
 coord_t	delta_sy;
 Window	mere;
 int		ox,oy;
 int		width,height;
 int		thickness;
 int		depth;
 Status	code_ret;
 int		src_x,src_y;
 int		dst_x,dst_y;
  
 delta_x=This->Orig_X-x;
 delta_y=This->Orig_Y-y;
 if (delta_x==0 && delta_y==0)
  return;
 SET_WAIT_MODE_STATIC;
 delta_sx=This->sOrig_X-x*This->sScale;
 delta_sy=This->sOrig_Y-y*This->sScale;
 This->Orig_X=x;
 This->Orig_Y=y;
 This->sOrig_X=x*This->sScale;
 This->sOrig_Y=y*This->sScale;
 This->scroll_delta_sX+=delta_sx;
 This->scroll_delta_sY+=delta_sy;
 This->scroll_delta_X+=delta_x;
 This->scroll_delta_Y+=delta_y;
 This->select_sX_min+=delta_sx;
 This->select_sY_min+=delta_sy;
 This->select_sX_max+=delta_sx;
 This->select_sY_max+=delta_sy;
  
 calcul_desk_area_component(This);

 if (This->Regle_actif && This->X_info_calc)
 {
  F(This->RegleV).Draw(This->RegleV,TRUE);
  F(This->RegleH).Draw(This->RegleH,TRUE);
 }
  
 lcadre=This->ListCadre;
 while(lcadre)
 {
  lcadre->cadre->Forme->Orig_X+=delta_x;
  lcadre->cadre->Forme->Orig_Y+=delta_y;
  lcadre->cadre->sOrig_X+=delta_sx;
  lcadre->cadre->sOrig_Y+=delta_sy;
  lcadre=lcadre->NextCadre;
 }
 if (!This->X_info_calc) 
 {
  UNSET_WAIT_MODE;
  return;
 }
  
 code_ret=XGetGeometry(_display,_window,&mere,&ox,&oy,
		       &width,&height,&thickness,&depth);
 if (width>ABS(delta_x) && height>ABS(delta_y))
 {
  width-=ABS(delta_x);
  height-=ABS(delta_y);
  src_x=dst_x=src_y=dst_y=0;
  if (delta_x>0)
   dst_x=delta_x;
  else
   src_x=ABS(delta_x);
  if (delta_y>0)
   dst_y=delta_y;
  else
   src_y=ABS(delta_y);
  XCopyArea(_display,_window,_window,_gc_inside,src_x,src_y,
	    width,height,dst_x,dst_y);

  if (delta_x>0)
  {
   XClearArea(_display,_window,0,0,delta_x,height,False);
   F(This).redraw_rectangle(This,0,0,delta_x-1,height);
  }
  else
  {
   XClearArea(_display,_window,width,0,width-delta_x,height,False);
   F(This).redraw_rectangle(This,width,0,width-delta_x,height);
  }
  if (delta_y>0)
  {
   XClearArea(_display,_window,0,0,width,delta_y,False);
   F(This).redraw_rectangle(This,0,0,width,delta_y-1);
  }
  else
  {
   XClearArea(_display,_window,0,height,width,height-delta_y,False);
   F(This).redraw_rectangle(This,0,height,width,height-delta_y);
  }
 }
 else
 {
  XClearArea(_display,_window,0,0,width,height,False);
  F(This).redraw_rectangle(This,0,0,width,height);
 }
 UNSET_WAIT_MODE;
}

void change_page_scaling(This,new_factor,x,y)
c_Page	*This;
scale_t	new_factor;
int	x;
int	y;
{
 l_Cadre	*lcadre;
 c_Cadre	*cadre;
 coord_t	new_scale;
 coord_t	new_width;
 coord_t	new_height;
 coord_t	old_scale;
 coord_t	old_sOrig_X;
 coord_t	old_sOrig_Y;
 coord_t	sx,sy;
 int		width,height;
 boolean	resize;
  
 /*  new_scale=(coord_t)(((double)This->sScale*(double)This->sFactor)/(double)new_factor); */
  
 new_scale = (coord_t)( ( (double)COORD_DEFAULT*
			 (double)SCALE_FROM_PERCENT(100) )/
		       (double)new_factor );
  
 width = This->RegleH->Size;
 height = This->RegleV->Size;
 new_width = MIN(This->sDeskArea_Width, width * new_scale);
 new_height = MIN(This->sDeskArea_Height, height * new_scale);
  
 This->sFactor = new_factor;
 old_scale = This->sScale;
 old_sOrig_X = This->sOrig_X;
 old_sOrig_Y = This->sOrig_Y;
 sx = (x + This->Orig_X) * old_scale;
 sy = (y + This->Orig_Y) * old_scale;
 resize = FALSE;
 if (This->sDeskArea_Width <= width * new_scale)
 {
  resize = TRUE;
  sx = (This->sDeskArea_Width) / 2;
 }
 else
 {
  if (sx < new_width / 2) 
  {
   sx = new_width / 2;
  }
  else
  {
   if (sx > This->sDeskArea_Width - (new_width >> 1)) 
   {
    sx = This->sDeskArea_Width - (new_width >> 1);
   }
  }
  if (This->Width < This->RegleH->Size)
   resize = TRUE;
 }

 if (This->sDeskArea_Height <= height*new_scale)
 {
  resize = TRUE;
  sy = (This->sDeskArea_Height) / 2;
 }
 else
 {
  if (sy < new_height / 2) 
   sy = new_height/2;
  else
  {
   if (sy > This->sDeskArea_Height - (new_height >> 1)) 
   {
    sy = This->sDeskArea_Height - (new_height >> 1);
   }
  }
  if (This->Height < This->RegleV->Size)
   resize = TRUE;
 }
  
 This->sScale = new_scale;
 if (This->X_info_calc)
  F(GlobDragAndDrop).change_argument(GlobDragAndDrop, This->X_info.window, 2,
				     DD_INT,
				     (int)(SCALE_TO_PERCENT(new_factor)* 100));

 Xc_TRACE(("new_width: %d sx: %d scale: %d", new_width, sx, new_scale));
 This->DeskArea_Width = This->sDeskArea_Width / This->sScale;
 This->DeskArea_Height = This->sDeskArea_Height / This->sScale;
 if (This->DeskArea_Width < This->RegleH->Size)
  This->sOrig_X = 0;
 else
  This->sOrig_X = (sx - (new_width) / 2);
 if (This->DeskArea_Height < This->RegleV->Size)
  This->sOrig_Y = 0;
 else
  This->sOrig_Y = (sy-(new_height) / 2);
 This->Orig_X = This->sOrig_X / new_scale;
 This->sOrig_X = This->Orig_X * new_scale;
 if ((This->DeskArea_Width > This->RegleH->Size) &&
     (This->RegleH->Size + This->Orig_X > This->DeskArea_Width))
 {
  This->Orig_X = This->DeskArea_Width - This->RegleH->Size;
  This->sOrig_X = This->Orig_X * new_scale;
 }
 This->Orig_Y = This->sOrig_Y / new_scale;
 This->sOrig_Y = This->Orig_Y * new_scale;
 if ( (This->DeskArea_Height > This->RegleV->Size) &&
     (This->RegleV->Size + This->Orig_Y > This->DeskArea_Height))
 {
  This->Orig_Y = This->DeskArea_Height - This->RegleV->Size;
  This->sOrig_Y = This->Orig_Y * new_scale;
 }
 Xc_TRACE(("Orig_X: %d Orig_Y: %d", This->Orig_X, This->Orig_Y));

 if (This->X_info_calc)
 {
  SET_WAIT_MODE_STATIC;
  if (resize)
  {
   F(This).resize_Page(This,
		       REGLE_SIZE * This->Regle_actif, 
		       REGLE_SIZE * This->Regle_actif,
		       This->RegleH->Size, This->RegleV->Size);
  }
  
  F(This->ScrollBarV).Change_Rsize
   (This->ScrollBarV,_display,This->DeskArea_Width, This->DeskArea_Height);
  F(This->ScrollBarV).AMove(This->ScrollBarV, This->Orig_Y);
  F(This->ScrollBarH).Change_Rsize
   (This->ScrollBarH,_display,This->DeskArea_Width, This->DeskArea_Height);
  F(This->ScrollBarH).AMove(This->ScrollBarH, This->Orig_X);  
  if (This->Regle_actif)
  {
   F(This->RegleV).Draw(This->RegleV,TRUE);
   F(This->RegleH).Draw(This->RegleH,TRUE);
  }

  calcul_desk_area_component(This);
  
  lcadre = This->ListCadre;
  while(lcadre)
  {
   cadre = lcadre->cadre;
   cadre->sScale = new_scale;
   cadre->sFactor = new_factor;
   sx = (coord_t)(cadre->sOrig_X + old_sOrig_X);
   sy = (coord_t)(cadre->sOrig_Y + old_sOrig_Y);
   sx -= This->sOrig_X;
   sy -= This->sOrig_Y;
   cadre->sOrig_X = sx;
   cadre->sOrig_Y = sy;
   cadre->Forme->Orig_X = sx / new_scale;
   cadre->Forme->Orig_Y = sy / new_scale;
   F(cadre).calculscaling(cadre);
   F(cadre).calculorigine(cadre);
   F(cadre).calculforme(cadre, CONTOUR);
   if (cadre->object_type != CADRE_EMPTY)
   {
    if (DISPLAY_INIT(cadre->object)(cadre->object, cadre) != XC_NO_ERROR)
     F(cadre).detachobject(cadre);
   }
   lcadre = lcadre->NextCadre;
  }
  F(This).calcul_select_handle(This);
  XClearArea(_display, _window, 0, 0, width, height, False);
  if (!resize)
   F(This).redraw_rectangle(This, 0, 0, width-1, height-1);
  UNSET_WAIT_MODE;
 }
 else
 {
  lcadre = This->ListCadre;
  while(lcadre)
  {
   cadre = lcadre->cadre;
   sx = (coord_t)(cadre->sOrig_X + old_sOrig_X);
   sy = (coord_t)(cadre->sOrig_Y + old_sOrig_Y);
   sx -= This->sOrig_X;
   sy -= This->sOrig_Y;
   cadre->sOrig_X = sx;
   cadre->sOrig_Y = sy;
   cadre->Forme->Orig_X = sx / new_scale;
   cadre->Forme->Orig_Y = sy / new_scale;
   lcadre = lcadre->NextCadre;
  }
 }
}

void init_recoordinate_point(This)
c_Page	*This;
{
 This->scroll_delta_sX=0;
 This->scroll_delta_sY=0;
 This->scroll_delta_X=0;
 This->scroll_delta_Y=0;
}

void recoordinate_point(This,sX,sY,x,y,init)
c_Page	*This;
coord_t	*sX;
coord_t	*sY;
int	*x;
int	*y;
boolean	init;
{
 if (sX)
  *sX+=This->scroll_delta_sX;
 if (sY)
  *sY+=This->scroll_delta_sY;
 if (x)
  *x+=This->scroll_delta_X;
 if (y)
  *y+=This->scroll_delta_Y;
 if (init)
 {
  This->scroll_delta_sX=0;
  This->scroll_delta_sY=0;
  This->scroll_delta_X=0;
  This->scroll_delta_Y=0;
 }
}

void move_cadre_to_xy(This,lcadre,sx,sy)
c_Page	*This;
l_Cadre	*lcadre;
coord_t	sx;
coord_t	sy;
{
 l_Cadre	*lowlist;
 l_Cadre	*lowsort;
 l_Cadre	*highlist;
 l_Cadre	*tmplist;
 l_Cadre	*tmplist2;
 l_Cadre	*transp;
 c_Cadre	*this_cadre;
 c_Cadre	**lowcadre = NULL;
 Polygone	*forme;
 Polygone	*lowforme;
 Polygone	*ftransp;
 Polygone	*oldforme;
 int		count_cadre;
 int		i;

 SET_WAIT_MODE_STATIC;
 this_cadre=lcadre->cadre;
 list_cadre_low_high_priority(This,lcadre,&lowlist,&highlist,INTERS_CADRE);
 forme=DuplicatePolygone(this_cadre->Forme);
 tmplist=highlist;
 transp=NULL;
 while(tmplist)
 {
  if (tmplist->cadre->inside_color->transparency)
   add_cadre_in_list(&transp,tmplist);
  else
   SubtractPolygone(&forme,tmplist->cadre->Forme);
  tmplist=tmplist->NextCadre;
 }
 if (transp)
  oldforme=DuplicatePolygone(this_cadre->Forme);
 lowsort=sort_desc_list_cadre(lowlist);
 tmplist=lowsort;
 count_cadre=0;
 while(tmplist)
 {
  count_cadre++;
  tmplist=tmplist->NextCadre;
 }
 if (count_cadre)
 {
  lowcadre=(c_Cadre **)Xc_malloc("mv lcadre",sizeof(c_Cadre *)*count_cadre);
  tmplist=lowsort;
  for(i=0;i<count_cadre;i++)
  {
   lowcadre[i]=(c_Cadre *)Xc_malloc("mv cadre",sizeof(c_Cadre));
   memcpy(lowcadre[i],tmplist->cadre,sizeof(c_Cadre));
   lowforme=IntersectPolygone(forme,lowcadre[i]->Forme);
   if (!lowcadre[i]->inside_color->transparency)
    SubtractPolygone(&forme,lowforme);
   lowcadre[i]->Forme=lowforme;
   tmplist=tmplist->NextCadre;
  }
 }
 this_cadre->Forme->Orig_X=sx/this_cadre->sScale;
 this_cadre->Forme->Orig_Y=sy/this_cadre->sScale;
 this_cadre->sOrig_X=sx;
 this_cadre->sOrig_Y=sy;
 F(This).calcul_select_handle(This);
 if (!this_cadre->inside_color->transparency)
  SubtractPolygone(&forme,this_cadre->Forme);
 DrawPolygone(forme,This->X_info);
 draw_desk_area_polygone(This,forme);
 FreePolygone(forme,DEL_POLYGONE);
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(highlist,NO_DEL_CADRE);
    
 if (count_cadre)
 {
  tmplist=lowlist=sort_asc_list_cadre(lowsort);
  for(i=count_cadre-1;i>=0;i--)
  {
   Xc_TRACE(("cadre pos: %d name: %s",i,lowcadre[i]->name));

   if (!this_cadre->inside_color->transparency)
    SubtractPolygone(&(lowcadre[i]->Forme),this_cadre->Forme);
   F(tmplist->cadre).drawcadre(tmplist->cadre, lowcadre[i]->Forme);
   F(tmplist->cadre).drawcontour(tmplist->cadre, lowcadre[i]->Forme,
				 This->Frame_Border_actif);
   FreePolygone(lowcadre[i]->Forme,DEL_POLYGONE);
   Xc_free(lowcadre[i]);
   tmplist=tmplist->NextCadre;
  }
  Xc_free(lowcadre);
 }
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(lowsort,NO_DEL_CADRE);
  
 tmplist=transp;
 if (tmplist)
 {
  SubtractPolygone(&oldforme,this_cadre->Forme);
  do
  {
   list_cadre_low_high_priority(This,tmplist,&lowlist,
				&highlist,INTERS_CADRE);
   ftransp=IntersectPolygone(tmplist->cadre->Forme, oldforme);
   tmplist2=highlist;
   while(tmplist2)
   {
    if (!tmplist2->cadre->inside_color->transparency)
     SubtractPolygone(&ftransp, tmplist2->cadre->Forme);
    tmplist2=tmplist2->NextCadre;
   }
   F(tmplist->cadre).drawcadre(tmplist->cadre, ftransp);
   F(tmplist->cadre).drawcontour(tmplist->cadre, ftransp,
				 This->Frame_Border_actif);
	  
   free_list_cadre(lowlist, NO_DEL_CADRE);
   free_list_cadre(highlist, NO_DEL_CADRE);
   FreePolygone(ftransp, DEL_POLYGONE);
   tmplist=tmplist->NextCadre;
  }
  while(tmplist);
  FreePolygone(oldforme,DEL_POLYGONE);
  free_list_cadre(transp,NO_DEL_CADRE);
 }
 draw_cadre(This,lcadre);
 UNSET_WAIT_MODE;
}


void draw_cadre_contour(This,lcadre)
c_Page	*This;
l_Cadre	*lcadre;	
{
 l_Cadre   *lowlist;	
 l_Cadre   *highlist;	
 l_Cadre   *tmp_list;
 c_Cadre   *this_cadre;
 Polygone  *forme;
  
 this_cadre=lcadre->cadre;
 list_cadre_low_high_priority(This,lcadre,&lowlist,&highlist,INTERS_CADRE);
 tmp_list=highlist;
 forme=DuplicatePolygone(this_cadre->Forme);
 while(tmp_list)
 {
  SubtractPolygone(&forme,tmp_list->cadre->Forme);
  tmp_list=tmp_list->NextCadre;
 }
 F(this_cadre).drawcontour(this_cadre,forme,
			   This->Frame_Border_actif);
 FreePolygone(forme,DEL_POLYGONE);
  
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(highlist,NO_DEL_CADRE);
}

void draw_cadre(This,lcadre)
c_Page	*This;
l_Cadre	*lcadre;	
{
 l_Cadre   *lowsort;	
 l_Cadre   *lowlist;	
 l_Cadre   *highlist;	
 l_Cadre   *tmp_list;
 l_Cadre   *tmp_list2;
 l_Cadre   *transp;
 c_Cadre   *this_cadre;
 c_Cadre   **lowcadre;
 Polygone  *forme;
 Polygone  *lowforme;
 Polygone  *ftransp;
 Polygone  *highforme;
 int	count_cadre;
 int	i;

 Xc_TRACE(("draw cadre %s", lcadre->name));
 this_cadre = lcadre->cadre;
 list_cadre_low_high_priority(This, lcadre, &lowlist, &highlist,INTERS_CADRE);
 forme = DuplicatePolygone(this_cadre->Forme);
  
 tmp_list = highlist;
 transp = NULL;
 while(tmp_list)
 {
  if (tmp_list->cadre->inside_color->transparency)
   add_cadre_in_list(&transp, tmp_list);
  else
   SubtractPolygone(&forme, tmp_list->cadre->Forme);
  tmp_list = tmp_list->NextCadre;
 }
 highforme = DuplicatePolygone(forme);
  
 if (this_cadre->inside_color->transparency)
 {
  lowsort = sort_desc_list_cadre(lowlist);
  tmp_list = lowsort;
  count_cadre = 0;
  while(tmp_list)
  {
   count_cadre++;
   tmp_list = tmp_list->NextCadre;
  }
  if (count_cadre)
  {
   lowcadre = (c_Cadre **)Xc_malloc("mv lcadre",
				    sizeof(c_Cadre *)*count_cadre);
   tmp_list = lowsort;
   for(i = 0; i < count_cadre; i++)
   {
    lowcadre[i] = (c_Cadre *)Xc_malloc("mv cadre",sizeof(c_Cadre));
    memcpy(lowcadre[i], tmp_list->cadre, sizeof(c_Cadre));
    lowforme = IntersectPolygone(forme, lowcadre[i]->Forme);
    SubtractPolygone(&forme, lowforme);
    lowcadre[i]->Forme = lowforme;
    tmp_list = tmp_list->NextCadre;
   }
   free_list_cadre(lowsort, NO_DEL_CADRE);
   tmp_list = lowsort=sort_asc_list_cadre(lowlist);
   for(i = count_cadre - 1; i >= 0; i--)
   {
    F(tmp_list->cadre).drawcadre(tmp_list->cadre, 
				 lowcadre[i]->Forme);
    F(tmp_list->cadre).drawcontour(tmp_list->cadre,
				   lowcadre[i]->Forme,
				   This->Frame_Border_actif);
    FreePolygone(lowcadre[i]->Forme, DEL_POLYGONE);
    Xc_free(lowcadre[i]);
    tmp_list=tmp_list->NextCadre;
   }
   Xc_free(lowcadre);
   free_list_cadre(lowsort, NO_DEL_CADRE);
  }
 }
  
 DrawPolygone(forme, This->X_info);
 draw_desk_area_polygone(This, forme); 
  
 F(this_cadre).drawcadre(this_cadre, highforme);
 F(this_cadre).drawcontour(this_cadre,highforme,
			   This->Frame_Border_actif);
 FreePolygone(forme, DEL_POLYGONE);
  
 free_list_cadre(lowlist, NO_DEL_CADRE);
 free_list_cadre(highlist, NO_DEL_CADRE);
  
 tmp_list = transp;
 while(tmp_list)
 {
  list_cadre_low_high_priority(This,lcadre, &lowlist,
			       &highlist, INTERS_CADRE);
  ftransp = IntersectPolygone(tmp_list->cadre->Forme, highforme);
  tmp_list2 = highlist;
  while(tmp_list2)
  {
   if (!tmp_list2->cadre->inside_color->transparency)
    SubtractPolygone(&ftransp, tmp_list2->cadre->Forme);
   tmp_list2=tmp_list2->NextCadre;
  }
  F(tmp_list->cadre).drawcadre(tmp_list->cadre, ftransp);
  F(tmp_list->cadre).drawcontour(tmp_list->cadre, ftransp,
				 This->Frame_Border_actif);

  free_list_cadre(lowlist, NO_DEL_CADRE);
  free_list_cadre(highlist, NO_DEL_CADRE);
  FreePolygone(ftransp, DEL_POLYGONE);
  tmp_list = tmp_list->NextCadre;
 }

 if (transp)
  free_list_cadre(transp, NO_DEL_CADRE);
 FreePolygone(highforme, DEL_POLYGONE);
}

void redraw_rectangle(This,x1,y1,x2,y2)
c_Page	*This;
int x1;
int y1;
int x2;
int y2;
{
 Polygone *rectangle;
 Polygone *forme;
 l_Cadre *ListCadre;
 l_Cadre *lcadre;
 l_Cadre *tmpcadre;
 int i,j;
  
 draw_desk_area_rectangle(This,x1,y1,x2-x1+1,y2-y1+1);
  
 rectangle=CreateRectPolygone(x1,y1,x2,y2);
 lcadre=ListCadre=sort_asc_list_cadre(This->ListCadre);
  
 for(i=0;i<This->nb_cadre;i++)
 {
  forme=IntersectPolygone(ListCadre->cadre->Forme,rectangle);
  if (forme->memory)
  {
   tmpcadre=ListCadre;
   for(j=i+1;j<This->nb_cadre;j++)
   {
    tmpcadre=tmpcadre->NextCadre;
    if (!tmpcadre->cadre->inside_color->transparency)
     SubtractPolygone(&forme,tmpcadre->cadre->Forme);
   }
   F(ListCadre->cadre).drawcadre(ListCadre->cadre,forme);
   if (!ListCadre->cadre->in_work)
    F(ListCadre->cadre).drawcontour(ListCadre->cadre,forme,
				    This->Frame_Border_actif);
  }
  FreePolygone(forme,DEL_POLYGONE);
  ListCadre=ListCadre->NextCadre;
 }
 free_list_cadre(lcadre,NO_DEL_CADRE);
 FreePolygone(rectangle,DEL_POLYGONE);

 RectangleDisplaySelectedlCadre(This,x1,y1,x2-x1+1,y2-y1+1); 

 /*  display_link_textframe(This); */
}

void RectangleDisplaySelectedlCadre(This,x,y,w,h)
c_Page	*This;
unsigned short	x;
unsigned short	y;
unsigned short	w;
unsigned short	h;
{
 l_Cadre *tmpcadre;
 c_Cadre *cadre;
 XRectangle	rect;
  
 rect.x=x;
 rect.y=y;
 rect.width=w;
 rect.height=h;
 XSetClipRectangles(_display,_gc_contour,0,0,&rect,1,Unsorted);
 if (This->select_type || !This->select_group)
 {
  tmpcadre=This->SelectedCadre;
  while(tmpcadre)
  {
   if (!tmpcadre->cadre->in_work)
   {
    XCopyGC(_display,_gc_contour,(GCClipMask|GCClipXOrigin|GCClipYOrigin),
	    tmpcadre->cadre->X_info.gc_contour);
    if (This->select_type)
     F(tmpcadre->cadre).draw_sommet(tmpcadre->cadre);
    else
     F(tmpcadre->cadre).draw_handle(tmpcadre->cadre);
    XSetClipMask(_display,tmpcadre->cadre->X_info.gc_contour,None);
   }
   tmpcadre=tmpcadre->NextCadre;
  }
 }
 else
  if (This->SelectedCadre && !This->SelectedCadre->cadre->in_work)
  {
   cadre=This->SelectedCadre->cadre;
   if ((!This->SelectedCadre->NextCadre) && 
       ((!cadre->ListPoint->NextPoint) || 
	(!cadre->ListPoint->NextPoint->NextPoint) ||
	(!cadre->sWidth) || (!cadre->sHeight)))
    F(cadre).draw_sommet(cadre);
   else
    draw_handle(This);
  }
 if(This->function_data)
  REDRAW(This);
 XSetClipMask(_display,_gc_contour,None);
}

void DisplaySelectedlCadre(This)
c_Page	*This;
{
 l_Cadre *tmpcadre;
 c_Cadre *cadre;
  
 if (This->select_type || !This->select_group)
 {
  tmpcadre=This->SelectedCadre;
  while(tmpcadre)
  {
   if (This->select_type)
    F(tmpcadre->cadre).draw_sommet(tmpcadre->cadre);
   else
    F(tmpcadre->cadre).draw_handle(tmpcadre->cadre);
   tmpcadre=tmpcadre->NextCadre;
  }
 }
 else
  if (This->SelectedCadre && !This->SelectedCadre->cadre->in_work)
  {
   cadre=This->SelectedCadre->cadre;
   if ((!This->SelectedCadre->NextCadre) && 
       ((!cadre->ListPoint->NextPoint) || 
	(!cadre->ListPoint->NextPoint->NextPoint) ||
	(!cadre->sWidth) || (!cadre->sHeight)))
   {
    F(cadre).draw_sommet(cadre);
   }
   else
   {
    draw_handle(This);
   }
  }
}

void UndisplaySelectedlCadre(This)
c_Page	*This;
{
 l_Cadre *tmpcadre;
 c_Cadre *cadre;
  
 if (This->select_type || !This->select_group)
 {
  tmpcadre=This->SelectedCadre;
  while(tmpcadre)
  {
   if (This->select_type)
    F(tmpcadre->cadre).draw_sommet(tmpcadre->cadre);
   else
    F(tmpcadre->cadre).draw_handle(tmpcadre->cadre);
   tmpcadre=tmpcadre->NextCadre;
  }
 }
 else
  if (This->SelectedCadre && !This->SelectedCadre->cadre->in_work)
  {
   cadre=This->SelectedCadre->cadre;
   if ((!This->SelectedCadre->NextCadre) && 
       ((!cadre->ListPoint->NextPoint) || 
	(!cadre->ListPoint->NextPoint->NextPoint) ||
	(!cadre->sWidth) || (!cadre->sHeight)))
   {
    F(cadre).draw_sommet(cadre);
   }
   else
   {
    draw_handle(This);
   }
  }
}

void DisplaySelectedCadre(This,witch)
l_Cadre	*This;
boolean	witch;
{
 if (This->cadre->selected)
 {
  if (witch)
   F(This->cadre).draw_sommet(This->cadre);
  else
   F(This->cadre).draw_handle(This->cadre);
 }
}

void UndisplaySelectedCadre(This,witch)
l_Cadre	*This;
boolean	witch;
{
 if (This->cadre->selected)
 {
  if (witch)
   F(This->cadre).draw_sommet(This->cadre);
  else
   F(This->cadre).draw_handle(This->cadre);
 }
}

void draw_handle(This)
c_Page 	*This;
{
 int x_min,y_min,x_mid,y_mid,x_max,y_max;
  
 x_min=This->select_sX_min/This->sScale;
 y_min=This->select_sY_min/This->sScale;
 x_max=This->select_sX_max/This->sScale;
 y_max=This->select_sY_max/This->sScale;
 x_mid=(x_max+x_min)>>1;
 y_mid=(y_max+y_min)>>1;
 TRACE_RHANDLE(_display,_window,_gc_contour,x_min-RHANDLE_X-1,
	       y_min-RHANDLE_Y-1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_min-RHANDLE_X-1,
	       y_max+RHANDLE_Y+1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_max+RHANDLE_X+1,
	       y_min-RHANDLE_Y-1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_max+RHANDLE_X+1,
	       y_max+RHANDLE_Y+1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_mid,y_min-RHANDLE_Y-1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_mid,y_max+RHANDLE_Y+1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_min-RHANDLE_X-1,y_mid);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_max+RHANDLE_X+1,y_mid);
}

l_Cadre *get_lcadre_by_cadre(This,cadre)
c_Page	*This;
c_Cadre	*cadre;
{
 l_Cadre	*lcadre;
  
 lcadre=This->ListCadre;
 while(lcadre)
 {
  if (lcadre->cadre==cadre)
   return lcadre;
  lcadre=lcadre->NextCadre;
 }
 return NULL;
}

#if 0
static void display_link_textframe(This)
c_Page	*This;
{
 l_Cadre	*lcadre;

 /*  lcadre=This->ListCadre;
     while(lcadre && lcadre->cadre->object_type!=CADRE_TEXT)
     {
     lcadre = lcadre->NextCadre;
     }
     if (!lcadre) return; */
  
 lcadre=This->ListCadre;
 if (!lcadre) return;
 while(lcadre->NextCadre)
 {
  display_link_arrow(This, 
		     lcadre->NextCadre->cadre->Forme->Orig_X,
		     lcadre->NextCadre->cadre->Forme->Orig_Y,
		     lcadre->cadre->Forme->x_max+
		     lcadre->cadre->Forme->Orig_X,
		     lcadre->cadre->Forme->y_max+
		     lcadre->cadre->Forme->Orig_Y);
      
  lcadre=lcadre->NextCadre;
 }

 /*   display_link_arrow(This, 100, 100, 200, 200); */
}

static void display_link_arrow(This, x1, y1, x2, y2)
c_Page	*This;
int	x1;
int	y1;
int	x2;
int	y2;
{
 XGCValues	gc_values;
 XSegment	xseg[5];
 double	angle;
  
 XGetGCValues(_display, _gc_contour, GCFunction|GCForeground| 
	      GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	      GCJoinStyle, &gc_values);
  
 XSetLineAttributes(_display, _gc_contour, 10, LineSolid, 
		    CapButt,JoinMiter);
 XSetForeground(_display, _gc_contour, GlobColormap->Cblack);
 XSetFunction(_display, _gc_contour, GXxor);
  
 xseg[0].x1=xseg[1].x1=xseg[2].x1=x1;
 xseg[0].y1=xseg[1].y1=xseg[2].y1=y1;
 xseg[0].x2=x2;
 xseg[0].y2=y2;
 angle = atan2((double)(y2-y1), (double)(x1-x2));
 xseg[1].x2=x1+(int)(cos(angle-M_PI/6)*(double)20);
 xseg[1].y2=y1+(int)(sin(angle-M_PI/6)*(double)20);
 xseg[2].x2=x1+(int)(cos(angle+M_PI/6)*(double)20);
 xseg[2].y2=y1+(int)(sin(angle+M_PI/6)*(double)20);
  
 XDrawSegments(_display, _window, _gc_contour, xseg, 3);

 XChangeGC(_display, _gc_contour, GCFunction|GCForeground| 
	   GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	   GCJoinStyle, &gc_values);
}
#endif

void  object_redraw_cadre(cpage,cadre)
c_Page	*cpage;
c_Cadre	*cadre;
{
 l_Cadre	*lcadre;
  
 F(cpage).undisplay_selected_lcadre(cpage);
 lcadre=get_lcadre_by_cadre(cpage,cadre);
 if (lcadre)
  F(cpage).draw_cadre(cpage,lcadre);
 F(cpage).display_selected_lcadre(cpage);
}

void  object_redraw_rectangle(cpage,cadre,x1,y1,x2,y2)
c_Page	*cpage;
c_Cadre	*cadre;
coord_t	x1,y1,x2,y2;
{
 l_Cadre	*lcadre;
  
 lcadre=get_lcadre_by_cadre(cpage,cadre);
}

Polygone *object_get_clip_polygone(cpage,cadre)
c_Page	*cpage;
c_Cadre	*cadre;
{
 l_Cadre	*lcadre;
 l_Cadre   *lowlist;	
 l_Cadre   *highlist;	
 l_Cadre   *tmp_list;
 c_Cadre   *this_cadre;
 Polygone  *forme;
  
 lcadre=get_lcadre_by_cadre(cpage,cadre);
 this_cadre=lcadre->cadre;
 list_cadre_low_high_priority(cpage,lcadre,&lowlist,&highlist,INTERS_CADRE);
 tmp_list=highlist;
 forme=DuplicatePolygone(this_cadre->Forme);
 while(tmp_list)
 {
  if (!tmp_list->cadre->inside_color->transparency)
   SubtractPolygone(&forme,tmp_list->cadre->Forme);
  tmp_list=tmp_list->NextCadre;
 }
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(highlist,NO_DEL_CADRE);
 return forme;
}

/* ----------------------------------------------------------------- ** 
** generate a ps file                                                ** 
** ----------------------------------------------------------------- */
static boolean ps_print(this, post)
c_Page		*this;
c_PostScript	*post;
{
 int i;

 Xc_HISTORY(("ps_print(%d)", this->page_number));

 for(i = 0; i < ((this->page_type == Xc_PAGE_DOUBLE)? 2 : 1); i++)
 {
  l_Cadre *ptr, *frames;
  coord_t x, y;
      
  if (this->page_type == Xc_PAGE_DOUBLE)
  {
   if (i==0)
   {
    if(!post->print_param->Left)
     continue;
   }
   else
   {
    if (!post->print_param->Right || 
	(int)this->page_number == post->print_param->EndPage)
     continue;
   }
  }
  Xc_TRACE(("page %d", this->page_number + i));

  if((ptr = F(this).list_cadre_by_page(this, i, &x, &y)) != NULL)
  {
   if(!F(post).beginPage(post, this->page_number + i)) return FALSE;

   /*--- Landscape printing ---*/
   if(this->page_orient == Xc_LANDSCAPE)
   {
    if(!F(post->matrix).rotate(post->matrix, TRUE,
			       SCALE_FROM_DEGREES(90.0)))
     return FALSE;
   }
   else
    if(!F(post->matrix).translate(post->matrix, TRUE, COORD_ZERO,
				  this->Page_Contour.sHeight))
     return FALSE;
	  
   F(post->matrix).scale(post->matrix, FALSE, SCALE_ONE, -SCALE_ONE);
   F(post->matrix).translate(post->matrix, FALSE,
			     this->sOrig_X - x, this->sOrig_Y - y);

   frames = sort_asc_list_cadre(ptr);
   free_list_cadre(ptr, NO_DEL_CADRE);

   if(frames != NULL)
   {
    for(ptr = frames; ptr != NULL; ptr = ptr->NextCadre)
     if(!F(ptr->cadre).ps_print(ptr->cadre, post)) break;
    free_list_cadre(frames, NO_DEL_CADRE);
    if(ptr != NULL) return FALSE;
   }
	  
   F(post->matrix).pop(post->matrix);
   if(!F(post).endPage(post)) return FALSE;
  }
 }

 Xc_TRACE(("printed"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** writePage - Page saving                                           ** 
** ----------------------------------------------------------------- */
static boolean writePage(this, doc, selected)
c_Page *this;
c_DocFile *doc;
boolean	selected;
{
 c_Page_Typo_Mark *marks;
 long bitfield;
 l_Cadre *ptr, *tmp;
 v_line *vline;
 h_line *hline;

 Xc_HISTORY(("write(%d)", this->page_number));

 if(!F(doc).writeKeyword(doc, XcDF_PAGE_KEYWORD, this->page_number) ||
    !F(doc).startSequence(doc) ||
    !F(doc).writeKeyword(doc, this->page_format->keyword,
			 this->page_format->param)) return FALSE;

 /*--- Write page dimension if custom format ---*/
 Xc_TRACE(("page dimension"));
 if (this->page_format == &Xc_Table_Page_Format[Xc_CUSTOM_FORMAT])
 {
  if (this->page_orient == Xc_LANDSCAPE)
  {
   if (!F(doc).startSequence(doc) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(this->Page_Contour.sHeight)) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(this->Page_Contour.sWidth)) ||
       !F(doc).endSequence(doc)) return FALSE;
  }
  else
  {
   if (!F(doc).startSequence(doc) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(this->Page_Contour.sWidth)) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(this->Page_Contour.sHeight)) ||
       !F(doc).endSequence(doc)) return FALSE;
  }
 }
 /*--- Write page zoom factor ---*/
 Xc_TRACE(("page zoom"));
 if (!F(doc).writeKeyword(doc, XcDF_PAGE_ZOOM_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->sFactor)) ||
     !F(doc).endSequence(doc)) return FALSE;
  
 /*--- Write page origin ---*/
 Xc_TRACE(("page origin"));
 if (!F(doc).writeKeyword(doc, XcDF_PAGE_ORIGIN_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->sOrig_X - 
					    this->Page_Contour.sOrig_X)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->sOrig_Y -
					    this->Page_Contour.sOrig_Y)) ||
     !F(doc).endSequence(doc)) return FALSE;
 
 /*--- Write page type ---*/
 Xc_TRACE(("page type"));
 switch(this->page_type)
 {
 case Xc_PAGE_LEFT:
  if(!F(doc).writeKeyword(doc, XcDF_LEFT_KEYWORD, -1L))
   return FALSE;
  break;
 case Xc_PAGE_RIGHT:
  if(!F(doc).writeKeyword(doc, XcDF_RIGHT_KEYWORD, -1L))
   return FALSE;
  break;
 case Xc_PAGE_DOUBLE:
  if(!F(doc).writeKeyword(doc, XcDF_DOUBLE_KEYWORD, -1L))
   return FALSE;
  break;
 }
  
 /*--- Write page orientation ---*/
 Xc_TRACE(("orientation"));
 if(!F(doc).writeKeyword(doc, (this->page_orient != Xc_LANDSCAPE)?
			 XcDF_PORTRAIT_KEYWORD : XcDF_LANDSCAPE_KEYWORD,
			 -1L)) return FALSE;
  
 /*--- Write page gridline setup ---*/
 Xc_TRACE(("page gridline"));
 if (!F(doc).writeKeyword(doc, XcDF_GRIDLINE_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->Page_Grid.sOrig_X - 
					    this->Page_Contour.sOrig_X)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->Page_Grid.sOrig_Y - 
					    this->Page_Contour.sOrig_Y)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->Page_Grid.sEcart_X)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->Page_Grid.sEcart_Y)) ||
     !F(doc).endSequence(doc)) return FALSE;
 
 /*--- Write page margins ---*/
 Xc_TRACE(("page margins"));
 marks = this->Page_Typo_Mark;
 if(!F(doc).writeKeyword(doc, XcDF_MARGINS_KEYWORD, -1L) ||
    !F(doc).startSequence(doc) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(marks->left_margin)) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(marks->right_margin)) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(marks->top_margin)) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(marks->bottom_margin)) ||
    !F(doc).endSequence(doc)) return FALSE;
  
 /*--- Write page grid ---*/
 Xc_TRACE(("page grid"));
 if(!F(doc).writeKeyword(doc, XcDF_GRID_KEYWORD, -1L) ||
    !F(doc).startSequence(doc) ||
    !F(doc).writeInteger(doc, (long)marks->column) ||
    !F(doc).writeInteger(doc, (long)marks->row) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(marks->gouttiere_x)) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(marks->gouttiere_y)))
  return FALSE;
  
 /*--- Write magnetic lines ---*/
 Xc_TRACE(("magnetic v-lines"));
 vline = this->Page_Align_Mark.ListVline;
 for( ; vline != NULL; vline = vline->NextVline)
  if(!F(doc).writeKeyword(doc, XcDF_VLINE_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(vline->sX)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(vline->sY_min)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(vline->sY_max)) ||
     !F(doc).endSequence(doc)) return FALSE;

 Xc_TRACE(("magnetic h-lines"));
 hline = this->Page_Align_Mark.ListHline;
 for( ; hline != NULL; hline = hline->NextHline)
  if(!F(doc).writeKeyword(doc, XcDF_HLINE_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(hline->sY)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(hline->sX_min)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(hline->sX_max)) ||
     !F(doc).endSequence(doc)) return FALSE;
  
 if(!F(doc).endSequence(doc)) return FALSE;
  
 bitfield = this->Page_Grid_actif ? 1 : 0;
 bitfield <<= 1; bitfield |= this->Page_Typo_Mark_actif ? 1 : 0;
 bitfield <<= 1; bitfield |= this->Page_Align_Mark_actif ? 1 : 0;
 bitfield <<= 1; bitfield |= this->Frame_Border_actif ? 1 : 0;
 bitfield <<= 1; bitfield |= this->magnetic ? 1 : 0;
 if(!F(doc).writeKeyword(doc, XcDF_BITFIELD_KEYWORD, bitfield))
  return FALSE;
  
 /*--- Write frames ---*/
 Xc_TRACE(("write frames"));
 if(this->ListCadre != NULL)
 {
  Xc_TRACE(("sorting"));
  if((tmp = sort_asc_list_cadre(this->ListCadre)) == NULL) return FALSE;
      
  Xc_TRACE(("frame loop"));
  for(ptr = tmp; ptr != NULL; ptr = ptr->NextCadre)
  {
   Xc_TRACE(("frame $%lx", (long)ptr->cadre));
   if(!F(ptr->cadre).write(ptr->cadre, doc,
			   this->sOrig_X - this->Page_Contour.sOrig_X,
			   this->sOrig_Y - this->Page_Contour.sOrig_Y))
    return FALSE;
  }

  Xc_TRACE(("freeing"));
  free_list_cadre(tmp, NO_DEL_CADRE);
 }
  
 /*--- Write if page selected ---*/
  
 if (selected && !F(doc).writeKeyword(doc, XcDF_PAGE_SELECTED_KEYWORD, -1L))
  return FALSE;
  
 Xc_TRACE(("done"));
  
 return F(doc).endSequence(doc);
}










