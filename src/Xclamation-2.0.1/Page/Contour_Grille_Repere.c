/*
** Contour_Grille_Repere.c for Xclamation in Page/
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
** Started on  Mon Jul 17 20:28:20 1995 Emmanuel Paris
** Last update Thu Oct 22 16:51:31 1998 Emmanuel Paris
*/

#include "Page.h"

void calcul_desk_area_and_origin();
void calcul_desk_area_component();
void destroy_desk_area_component();
void draw_desk_area_rectangle();
void draw_desk_area_polygone();

void attract_point();

void display_grid();
void display_typo_mark();
void display_align_mark();
void display_regle();
void display_frame_border();
void magnetize_all();
void redraw_all();

void create_page_contour();
void destroy_page_contour();
void calcul_page_contour();
void draw_page_contour_rectangle();
void draw_page_contour_polygone();

void create_page_grid();
void destroy_page_grid();
void calcul_page_grid();
void draw_page_grid_rectangle();
void draw_page_grid_polygone();

void create_page_typo_mark();
void destroy_page_typo_mark();
void calcul_page_typo_mark();
void draw_page_typo_mark_rectangle();
void draw_page_typo_mark_polygone();

void create_page_align_mark();
void destroy_page_align_mark();
void *add_page_align_mark();
void move_page_align_mark();
void remove_page_align_mark();
void recoordinate_page_align_mark();
void *get_page_align_mark();
void calcul_page_align_mark();
void draw_page_align_mark_rectangle();
void draw_page_align_mark_polygone();

coord_t attract_point_by_list_v_line();
coord_t attract_point_by_list_h_line();
void calcul_list_v_line();
void calcul_list_h_line();
void draw_list_v_line_rectangle();
void draw_list_h_line_rectangle();
void draw_list_v_line_polygone();
void draw_list_h_line_polygone();
static void draw_shadow_v_line_rectangle();
static void draw_shadow_h_line_rectangle();
static void draw_shadow_h_line_polygone();
static void draw_shadow_v_line_polygone();
void draw_v_line();
void draw_h_line();
void clear_v_line();
void clear_h_line();
void free_list_v_line();
void free_list_h_line();
v_line *alloc_list_v_line();
h_line *alloc_list_h_line();

void calcul_desk_area_and_origin(This, origin)
c_Page	*This;
boolean origin;
{
 This->sDeskArea_Width=This->Page_Contour.sOrig_X*2 + 
  This->Page_Contour.sWidth*This->Page_Contour.nb_page_horizontal;
 This->sDeskArea_Height=This->Page_Contour.sOrig_Y*2 + 
  This->Page_Contour.sHeight*This->Page_Contour.nb_page_vertical;
 This->DeskArea_Width=This->sDeskArea_Width/This->sScale;
 This->DeskArea_Height=This->sDeskArea_Height/This->sScale;
 if (origin)
 {
  This->sOrig_X += This->Page_Contour.sOrig_X;
  This->sOrig_Y += This->Page_Contour.sOrig_Y;
  This->Orig_X = This->sOrig_X/This->sScale;
  This->Orig_Y = This->sOrig_Y/This->sScale;
 }
}

void calcul_desk_area_component(This)
c_Page	*This;
{
 if (This->Page_Contour.visible)
  calcul_page_contour(This);
  
 calcul_page_grid(This,This->Page_Grid.visible_X,This->Page_Grid.visible_Y);
 calcul_page_typo_mark(This);
 calcul_page_align_mark(This);
}

void destroy_desk_area_component(This, align)
c_Page	*This;
boolean align;
{
 destroy_page_contour(This);
 destroy_page_grid(This);
 destroy_page_typo_mark(This);
 if (align)
  destroy_page_align_mark(This);
}

void draw_desk_area_rectangle(This, x, y, width, height)
c_Page	*This;
int	x;
int	y;
int	width;
int	height;
{
 if (This->Page_Grid_actif)
  draw_page_grid_rectangle(This,x,y,width,height,
			   This->Page_Grid.visible_X,
			   This->Page_Grid.visible_Y);
 if (This->Page_Typo_Mark_actif)
  draw_page_typo_mark_rectangle(This,x,y,width,height);
  
 if (This->Page_Align_Mark_actif)
  draw_page_align_mark_rectangle(This,x,y,width,height);

 if (This->Page_Contour_actif && This->Page_Contour.visible)
  draw_page_contour_rectangle(This,x,y,width,height);
  
}

void draw_desk_area_polygone(This, polyg)
c_Page	*This;
Polygone	*polyg;
{
 if (This->Page_Grid_actif)
  draw_page_grid_polygone(This,polyg,
			  This->Page_Grid.visible_X,
			  This->Page_Grid.visible_Y);

 if (This->Page_Typo_Mark_actif)
  draw_page_typo_mark_polygone(This,polyg);

 if (This->Page_Align_Mark_actif)
  draw_page_align_mark_polygone(This,polyg);

 if (This->Page_Contour_actif && This->Page_Contour.visible)
  draw_page_contour_polygone(This,polyg);
}

void attract_point(This, sx, sy, x, y)
c_Page	*This;
coord_t	*sx;
coord_t	*sy;
int	*x;
int	*y;
{
 coord_t	svx;
 coord_t	svy;
 coord_t	svx_mini;
 coord_t	svy_mini;
 coord_t	sv_mini_v;
 coord_t	sv_mini_h;
 coord_t	mini_v;
 coord_t	mini_h;
 int		vx;
 int		vy;
 int		vx_mini;
 int		vy_mini;
 int		i,nb_page;

 if (!This->magnetic)
  return ;
  
 mini_v=MAX_ATTRACT;
 mini_h=MAX_ATTRACT;
 svx_mini= *sx;
 svy_mini= *sy;
 vx_mini= *x;
 vy_mini= *y;

 if (This->Page_Contour_actif)
 {
  if (This->Page_Contour.magnetic)
  {
   sv_mini_v=
    attract_point_by_list_v_line(This,This->Page_Contour.ListVline,
				 This->Page_Contour.magnetic_field,
				 &svx,&vx,*sx,*sy);
   if (sv_mini_v<mini_v)
   {
    mini_v=sv_mini_v;
    svx_mini=svx;
    vx_mini=vx;
   }
   sv_mini_h=
    attract_point_by_list_h_line(This,This->Page_Contour.ListHline,
				 This->Page_Contour.magnetic_field,
				 &svy,&vy,*sy,*sx);
   if (sv_mini_h<mini_h)
   {
    mini_h=sv_mini_h;
    svy_mini=svy;
    vy_mini=vy;
   }
  }
 }
 if (This->Page_Typo_Mark_actif)
 {
  nb_page=This->Page_Contour.nb_page_vertical*
   This->Page_Contour.nb_page_horizontal;
  for(i = 0; i < nb_page; i++)
  {
   if (This->Page_Typo_Mark[i].magnetic)
   {
    sv_mini_v=
     attract_point_by_list_v_line(This,
				  This->Page_Typo_Mark[i].ListVline,
				  This->Page_Typo_Mark[i].
				  magnetic_field,
				  &svx,&vx,*sx,*sy);
    if (sv_mini_v<mini_v)
    {
     mini_v=sv_mini_v;
     svx_mini=svx;
     vx_mini=vx;
    }
    sv_mini_h=
     attract_point_by_list_h_line(This,
				  This->Page_Typo_Mark[i].ListHline,
				  This->Page_Typo_Mark[i].
				  magnetic_field,
				  &svy,&vy,*sy,*sx);
    if (sv_mini_h<mini_h)
    {
     mini_h=sv_mini_h;
     svy_mini=svy;
     vy_mini=vy;
    }
   }
  }
 }
 if (This->Page_Grid_actif)
 {
  if (This->Page_Grid.magnetic_X)
  {
   sv_mini_v=
    attract_point_by_list_v_line(This,This->Page_Grid.ListVline,
				 This->Page_Grid.magnetic_field_X,
				 &svx,&vx,*sx,*sy);
   if (sv_mini_v<mini_v)
   {
    mini_v=sv_mini_v;
    svx_mini=svx;
    vx_mini=vx;
   }
  }
  if (This->Page_Grid.magnetic_Y)
  {
   sv_mini_h=
    attract_point_by_list_h_line(This,This->Page_Grid.ListHline,
				 This->Page_Grid.magnetic_field_Y,
				 &svy,&vy,*sy,*sx);
   if (sv_mini_h<mini_h)
   {
    mini_h=sv_mini_h;
    svy_mini=svy;
    vy_mini=vy;
   }
  }   
 }
 if (This->Page_Align_Mark_actif)
 {
  if (This->Page_Align_Mark.magnetic_Y)
  {
   sv_mini_v=
    attract_point_by_list_v_line(This,This->Page_Align_Mark.ListVline,
				 This->Page_Align_Mark.magnetic_field_Y,
				 &svx,&vx,*sx,*sy);
   if (sv_mini_v<mini_v)
   {
    mini_v=sv_mini_v;
    svx_mini=svx;
    vx_mini=vx;
   }
  }
  if (This->Page_Align_Mark.magnetic_X)
  {
   sv_mini_h=
    attract_point_by_list_h_line(This,This->Page_Align_Mark.ListHline,
				 This->Page_Align_Mark.magnetic_field_X,
				 &svy,&vy,*sy,*sx);
   if (sv_mini_h<mini_h)
   {
    mini_h=sv_mini_h;
    svy_mini=svy;
    vy_mini=vy;
   }
  }
 }
 *sx=svx_mini;
 *sy=svy_mini;
 *x=vx_mini;
 *y=vy_mini;
}

/*                   DISPLAY FUNCTION                      */

void display_grid(This)
c_Page	*This;
{  
 This->Page_Grid_actif ^= TRUE;
 if (This->X_info_calc)
  redraw_all(This);
}

void display_typo_mark(This)
c_Page	*This;
{
 This->Page_Typo_Mark_actif ^= TRUE;
 if (This->X_info_calc)
  redraw_all(This);
}

void display_align_mark(This)
c_Page	*This;
{
 This->Page_Align_Mark_actif ^= TRUE;
 if (This->X_info_calc)
  redraw_all(This);
}

void display_regle(This)
c_Page	*This;
{
 if (This->Regle_actif)
 {
  XUnmapWindow(This->X_info.display, This->RegleH->window);
  XUnmapWindow(This->X_info.display, This->RegleV->window);
  XUnmapWindow(This->X_info.display, This->RegleC->window);
  This->RegleH->Size += REGLE_SIZE;
  This->RegleV->Size += REGLE_SIZE;
      
  F(This).resize_Page(This, 0, 0, This->RegleH->Size, This->RegleV->Size);
 }
 else
 {
  This->RegleH->Size -= REGLE_SIZE;
  This->RegleV->Size -= REGLE_SIZE;
  F(This).resize_Page(This, REGLE_SIZE, REGLE_SIZE,
		      This->RegleH->Size, This->RegleV->Size);
  XMapWindow(This->X_info.display, This->RegleH->window);
  XMapWindow(This->X_info.display, This->RegleV->window);
  XMapWindow(This->X_info.display, This->RegleC->window);      
 }
 This->Regle_actif ^= TRUE;
}

void display_frame_border(This)
c_Page	*This;
{
 This->Frame_Border_actif ^= TRUE;
 if (This->X_info_calc)
  redraw_all(This);
}

void magnetize_all(This)
c_Page	*This;
{
 This->magnetic ^= TRUE;
}

void redraw_all(This)
c_Page	*This;
{
 Window	mere;
 int		ox,oy;
 int		width,height;
 int		thickness;
 int		depth;
 Status	code_ret;
   
 code_ret=XGetGeometry(This->X_info.display,This->X_info.window,&mere,&ox,&oy,
		       &width,&height,&thickness,&depth);
 XClearArea(This->X_info.display,This->X_info.window,0,0,width,height,False);
 F(This).redraw_rectangle(This,0, 0, width + 1, height + 1);
}

/*                      PAGE CONTOUR                        */

void create_page_contour(This, width, height, nb_page_horizontal,
			 nb_page_vertical)
c_Page	*This;
coord_t	width;
coord_t	height;
int	nb_page_horizontal;
int	nb_page_vertical;
{
 color_cells_t	cells;
 h_line	*hline; 
 v_line	*vline;
 int		nb_hline;
 int		nb_vline;
 int	i;
  
 This->Page_Contour.sWidth = width;
 This->Page_Contour.sHeight = height;
 This->Page_Contour.sOrig_X = SCALE_FROM_MILLIMETERS(210);
 This->Page_Contour.sOrig_Y = SCALE_FROM_MILLIMETERS(70);
 This->Page_Contour.nb_page_horizontal = nb_page_horizontal;
 This->Page_Contour.nb_page_vertical = nb_page_vertical;
 This->Page_Contour.visible = TRUE;
 This->Page_Contour.magnetic = FALSE;
 This->Page_Contour.magnetic_field = 5*50*COORD_SCALEVALUE;
 This->Page_Contour.color = GlobColormap->Cblack;
 F(GlobColormap).get_closest_color(GlobColormap, &cells,
				   0x8000, 0x8000, 0x8000);
 This->Page_Contour.shadow_color = cells.pixel;
 This->Page_Contour.shadow_thickness = 4;
 This->Page_Contour.ListHline = (h_line *)NULL;
 This->Page_Contour.ListVline = (v_line *)NULL;
 nb_hline = 1 + This->Page_Contour.nb_page_vertical;
 nb_vline = 1 + This->Page_Contour.nb_page_horizontal;
 hline = This->Page_Contour.ListHline = 
  (h_line *)alloc_list_h_line(nb_hline, FALSE);
 vline = This->Page_Contour.ListVline = 
  (v_line *)alloc_list_v_line(nb_vline, FALSE);
 for(i = 0; i < nb_hline; i++)
 {
  hline->sY = This->Page_Contour.sOrig_Y + This->Page_Contour.sHeight*i;
  hline->sX_min = This->Page_Contour.sOrig_X;
  hline->sX_max = This->Page_Contour.sOrig_X + This->Page_Contour.sWidth*
   This->Page_Contour.nb_page_horizontal;
  hline = hline->NextHline;
 }
 for(i = 0; i < nb_vline; i++)
 {
  vline->sX = This->Page_Contour.sOrig_X + This->Page_Contour.sWidth*i;
  vline->sY_min = This->Page_Contour.sOrig_Y;
  vline->sY_max = This->Page_Contour.sOrig_Y + This->Page_Contour.sHeight*
   This->Page_Contour.nb_page_vertical;
  vline = vline->NextVline;
 }
}

void destroy_page_contour(This)
c_Page	*This;
{
 free_list_v_line(This->Page_Contour.ListHline);
 free_list_h_line(This->Page_Contour.ListVline);
}

void calcul_page_contour(This)
c_Page	*This;
{
 calcul_list_v_line(This,This->Page_Contour.ListVline);
 calcul_list_h_line(This,This->Page_Contour.ListHline);
}

void draw_page_contour_rectangle(This, x, y, width, height)
c_Page	*This;	
int	x;
int	y;
int	width;
int	height;
{
 XSetForeground(_display,_gc_inside,This->Page_Contour.color);
 draw_list_v_line_rectangle(This, This->Page_Contour.ListVline,
			    x, y, width, height);
 draw_list_h_line_rectangle(This, This->Page_Contour.ListHline,
			    x, y, width, height);
  
 XSetForeground(_display,_gc_inside,This->Page_Contour.shadow_color);
 switch(This->page_type)
 {
 case Xc_PAGE_LEFT:
  draw_shadow_v_line_rectangle(This, This->Page_Contour.ListVline,
			       -This->Page_Contour.shadow_thickness,
			       This->Page_Contour.shadow_thickness,
			       0, x, y, width, height);
  draw_shadow_h_line_rectangle(This, This->Page_Contour.ListHline->NextHline,
			       This->Page_Contour.shadow_thickness,
			       -This->Page_Contour.shadow_thickness,
			       -This->Page_Contour.shadow_thickness,
			       x, y, width, height);
  break;
 case Xc_PAGE_RIGHT:
  draw_shadow_v_line_rectangle(This, This->Page_Contour.ListVline->NextVline,
			       This->Page_Contour.shadow_thickness,
			       This->Page_Contour.shadow_thickness,
			       0, x, y, width, height);
  draw_shadow_h_line_rectangle(This, This->Page_Contour.ListHline->NextHline,
			       This->Page_Contour.shadow_thickness,
			       This->Page_Contour.shadow_thickness,
			       This->Page_Contour.shadow_thickness,
			       x, y, width, height);
  break;
 case Xc_PAGE_DOUBLE:
  draw_shadow_v_line_rectangle
   (This, This->Page_Contour.ListVline,
    -This->Page_Contour.shadow_thickness,
    This->Page_Contour.shadow_thickness, 0, x, y, width, height);
  draw_shadow_v_line_rectangle
   (This, This->Page_Contour.ListVline->NextVline->NextVline,
    This->Page_Contour.shadow_thickness,
    This->Page_Contour.shadow_thickness, 0, x, y, width, height);
  draw_shadow_h_line_rectangle(This, This->Page_Contour.ListHline->NextHline,
			       This->Page_Contour.shadow_thickness,
			       -This->Page_Contour.shadow_thickness,
			       This->Page_Contour.shadow_thickness,
			       x, y, width, height);
  break;
 }
  
 F(GlobColormap).use_color(GlobColormap, This->X_info.gc_inside, WHITE);
} 

void draw_page_contour_polygone(This,polyg)
c_Page	*This;
Polygone	*polyg;
{
 XSetForeground(_display,_gc_inside,This->Page_Contour.color);
 draw_list_v_line_polygone(This, This->Page_Contour.ListVline, polyg);
 draw_list_h_line_polygone(This, This->Page_Contour.ListHline, polyg);
  
 XSetForeground(_display,_gc_inside,This->Page_Contour.shadow_color);
 switch(This->page_type)
 {
 case Xc_PAGE_LEFT:
  draw_shadow_v_line_polygone(This, This->Page_Contour.ListVline,
			      -This->Page_Contour.shadow_thickness,
			      This->Page_Contour.shadow_thickness,
			      0, polyg);
  draw_shadow_h_line_polygone(This, This->Page_Contour.ListHline->NextHline,
			      This->Page_Contour.shadow_thickness,
			      -This->Page_Contour.shadow_thickness,
			      -This->Page_Contour.shadow_thickness,
			      polyg);
  break;
 case Xc_PAGE_RIGHT:
  draw_shadow_v_line_polygone(This, This->Page_Contour.ListVline->NextVline,
			      This->Page_Contour.shadow_thickness,
			      This->Page_Contour.shadow_thickness,
			      0, polyg);
  draw_shadow_h_line_polygone(This, This->Page_Contour.ListHline->NextHline,
			      This->Page_Contour.shadow_thickness,
			      This->Page_Contour.shadow_thickness,
			      This->Page_Contour.shadow_thickness,
			      polyg);
  break;
 case Xc_PAGE_DOUBLE:
  draw_shadow_v_line_polygone
   (This, This->Page_Contour.ListVline,
    -This->Page_Contour.shadow_thickness,
    This->Page_Contour.shadow_thickness, 0, polyg);
  draw_shadow_v_line_polygone
   (This, This->Page_Contour.ListVline->NextVline->NextVline,
    This->Page_Contour.shadow_thickness,
    This->Page_Contour.shadow_thickness, 0, polyg);
  draw_shadow_h_line_polygone
   (This, This->Page_Contour.ListHline->NextHline,
    This->Page_Contour.shadow_thickness,
    -This->Page_Contour.shadow_thickness,
    This->Page_Contour.shadow_thickness, polyg);
  break;
 }
 F(GlobColormap).use_color(GlobColormap,This->X_info.gc_inside,WHITE);
}

/*                         PAGE GRID                      */

void create_page_grid(This, orig_x, orig_y, ecart_x, ecart_y)
c_Page	*This;
coord_t	orig_x;
coord_t	orig_y;
coord_t	ecart_x;
coord_t	ecart_y;
{
 int		nb_hline;
 int		nb_vline;
 h_line	*hline; 
 v_line	*vline;
 int	i;
  
 This->Page_Grid.sOrig_X = orig_x;
 This->Page_Grid.sOrig_Y = orig_y;
 This->Page_Grid.sEcart_X = ecart_x;
 This->Page_Grid.sEcart_Y = ecart_y;
 nb_hline = (This->Page_Contour.sHeight*This->Page_Contour.nb_page_vertical
	     -(This->Page_Grid.sOrig_Y-This->Page_Contour.sOrig_Y))
  /This->Page_Grid.sEcart_Y + 1;
 nb_vline = (This->Page_Contour.sWidth*This->Page_Contour.nb_page_horizontal
	     -(This->Page_Grid.sOrig_X-This->Page_Contour.sOrig_X))
  /This->Page_Grid.sEcart_X + 1;
 This->Page_Grid.nb_vline = nb_vline;
 This->Page_Grid.nb_hline = nb_hline;
 This->Page_Grid.visible_X = TRUE;/*FALSE;*/
 This->Page_Grid.visible_Y = TRUE;/*FALSE;*/
 This->Page_Grid.magnetic_X = TRUE;
 This->Page_Grid.magnetic_Y = TRUE;
 This->Page_Grid.magnetic_field_X = ecart_x>>2;
 This->Page_Grid.magnetic_field_Y = ecart_y>>2;
 This->Page_Grid.color_X = GlobColormap->Cmagenta;
 This->Page_Grid.color_Y = GlobColormap->Cmagenta;
 hline = This->Page_Grid.ListHline = 
  (h_line *)alloc_list_h_line(nb_hline, TRUE);
 vline = This->Page_Grid.ListVline = 
  (v_line *)alloc_list_v_line(nb_vline, TRUE);
 for(i = 0; i < nb_hline; i++)
 {
  hline->sY = This->Page_Grid.sOrig_Y + This->Page_Grid.sEcart_Y*i;
  hline->sX_min = 0;
  hline->sX_max = This->sDeskArea_Width;
  hline = hline->NextHline;
 }
 for(i = 0; i < nb_vline; i++)
 {
  vline->sX = This->Page_Grid.sOrig_X + This->Page_Grid.sEcart_X*i;
  vline->sY_min = 0;
  vline->sY_max = This->sDeskArea_Height;
  vline = vline->NextVline;
 }
}

void destroy_page_grid(This)
c_Page	*This;
{
 if (This->Page_Grid.ListHline)
  Xc_free(This->Page_Grid.ListHline);
 if (This->Page_Grid.ListVline)
  Xc_free(This->Page_Grid.ListVline);
}

void calcul_page_grid(This,is_calc_vline,is_calc_hline)
c_Page	*This;
boolean	is_calc_vline;
boolean	is_calc_hline;
{
 if (is_calc_vline)
  calcul_list_v_line(This,This->Page_Grid.ListVline);
 if (is_calc_hline)
  calcul_list_h_line(This,This->Page_Grid.ListHline);
}

void draw_page_grid_rectangle(This,x,y,width,height,
			      is_calc_vline,is_calc_hline)
c_Page	*This;	
int	x;
int	y;
int	width;
int	height;
boolean	is_calc_vline;
boolean	is_calc_hline;
{
 if (is_calc_vline)
 {
  XSetForeground(_display,_gc_inside,This->Page_Grid.color_X);
  draw_list_v_line_rectangle(This,This->Page_Grid.ListVline,
			     x,y,width,height);
 }
 if (is_calc_hline)
 {
  XSetForeground(_display,_gc_inside,This->Page_Grid.color_Y);
  draw_list_h_line_rectangle(This,This->Page_Grid.ListHline,
			     x,y,width,height);
 }
 F(GlobColormap).use_color(GlobColormap,This->X_info.gc_inside,WHITE);
} 

void draw_page_grid_polygone(This,polyg,is_calc_vline,is_calc_hline)
c_Page	*This;
Polygone	*polyg;
boolean	is_calc_vline;
boolean	is_calc_hline;
{
 if (is_calc_vline)
 {
  XSetForeground(_display,_gc_inside,This->Page_Grid.color_X);
  draw_list_v_line_polygone(This,This->Page_Grid.ListVline,polyg);
 }
 if (is_calc_hline)
 {
  XSetForeground(_display,_gc_inside,This->Page_Grid.color_Y);
  draw_list_h_line_polygone(This,This->Page_Grid.ListHline,polyg);
 }
 F(GlobColormap).use_color(GlobColormap,This->X_info.gc_inside,WHITE);
}

/*             TYPOGRAPHIC MARKS              */

void create_page_typo_mark(This,left_margin,right_margin,top_margin,
			   bottom_margin,column,row,gouttiere_x,
			   gouttiere_y)
c_Page	*This;
coord_t	left_margin;
coord_t	right_margin;
coord_t	top_margin;
coord_t bottom_margin;
int	column;
int	row;
coord_t	gouttiere_x;
coord_t	gouttiere_y;
{
 int	i,x,y;
 int	nb_page;
 int	nb_hline;
 int	nb_vline;
 coord_t	lm = COORD_ZERO;
 coord_t	rm = COORD_ZERO;
 coord_t	sWidth;
 coord_t	sHeight;
 coord_t	sOrig_X;
 coord_t	sOrig_Y;
 coord_t	sWidthColumn;
 coord_t	sHeightRow;
 coord_t	sOffset_X;
 coord_t	sOffset_Y;
 h_line	*hline; 
 v_line	*vline;
 c_Page_Typo_Mark	*typo_mark;
  
 nb_page = This->Page_Contour.nb_page_vertical*
  This->Page_Contour.nb_page_horizontal;
  
 typo_mark = This->Page_Typo_Mark = (c_Page_Typo_Mark *)Xc_malloc
  ("typo mark",sizeof(c_Page_Typo_Mark)*nb_page);
 for(i = 0; i < nb_page; i++)
 {
  switch(This->page_type)
  {
  case Xc_PAGE_LEFT:
   lm = right_margin;
   rm = left_margin;
   break;
  case Xc_PAGE_RIGHT:
   lm = left_margin;
   rm = right_margin;
   break;
  case Xc_PAGE_DOUBLE:
   if (i)
   {
    lm = left_margin;
    rm = right_margin;
   }
   else
   {
    lm = right_margin;
    rm = left_margin;
   }
   break;
  }
  typo_mark[i].left_margin = lm;
  typo_mark[i].right_margin = rm;
  typo_mark[i].top_margin = top_margin;
  typo_mark[i].bottom_margin = bottom_margin;
  typo_mark[i].column = column;
  typo_mark[i].row = row;
  typo_mark[i].gouttiere_x = gouttiere_x;
  typo_mark[i].gouttiere_y = gouttiere_y;
  typo_mark[i].visible = TRUE;
  typo_mark[i].magnetic = TRUE;
  typo_mark[i].magnetic_field = SCALE_FROM_MILLIMETERS(2);
  typo_mark[i].color = GlobColormap->Cblue;
      
  nb_vline = 2 + (column-1)*2;
  nb_hline = 2 + (row-1)*2;
  vline = typo_mark[i].ListVline = 
   (v_line *)alloc_list_v_line(nb_vline, FALSE);
  hline = typo_mark[i].ListHline = 
   (h_line *)alloc_list_h_line(nb_hline, FALSE);
  sWidth = This->Page_Contour.sWidth;
  sHeight = This->Page_Contour.sHeight;

  sOrig_X = This->Page_Contour.sOrig_X;

  sOffset_X = (i%This->Page_Contour.nb_page_horizontal)*sWidth;
  sOffset_Y = (i/This->Page_Contour.nb_page_horizontal)*sHeight;

  vline->sX = sOrig_X + lm + sOffset_X;
  vline->sY_min = This->Page_Contour.sOrig_Y + sOffset_Y;
  vline->sY_max = This->Page_Contour.sOrig_Y + 
   This->Page_Contour.sHeight + sOffset_Y;
  vline = vline->NextVline;
      
  vline->sX = This->Page_Contour.sOrig_X + 
   This->Page_Contour.sWidth-rm + sOffset_X;
  vline->sY_min = This->Page_Contour.sOrig_Y + sOffset_Y;
  vline->sY_max = This->Page_Contour.sOrig_Y + 
   This->Page_Contour.sHeight + sOffset_Y;
  vline = vline->NextVline;
      
  sOrig_X += lm;
  sWidth -= lm + rm;
  sWidthColumn = (sWidth-gouttiere_x*(column-1))/column;
  for(x = 0; x < column - 1; x++)
  {
   sOrig_X += sWidthColumn;
   vline->sX = sOrig_X + sOffset_X;
   vline->sY_min = This->Page_Contour.sOrig_Y + sOffset_Y;
   vline->sY_max = This->Page_Contour.sOrig_Y + 
    This->Page_Contour.sHeight + sOffset_Y;
   vline = vline->NextVline;
   sOrig_X += gouttiere_x;
   vline->sX = sOrig_X + sOffset_X;
   vline->sY_min = This->Page_Contour.sOrig_Y + sOffset_Y;
   vline->sY_max = This->Page_Contour.sOrig_Y + 
    This->Page_Contour.sHeight + sOffset_Y;
   vline = vline->NextVline;
  }
 
  sOrig_Y = This->Page_Contour.sOrig_Y;
  hline->sY = sOrig_Y + top_margin + sOffset_Y;
  hline->sX_min = This->Page_Contour.sOrig_X + sOffset_X;
  hline->sX_max = This->Page_Contour.sOrig_X + 
   This->Page_Contour.sWidth + sOffset_X;
  hline = hline->NextHline;
      
  hline->sY = This->Page_Contour.sOrig_Y + 
   This->Page_Contour.sHeight-bottom_margin + sOffset_Y;
  hline->sX_min = This->Page_Contour.sOrig_X + sOffset_X;
  hline->sX_max = This->Page_Contour.sOrig_X + 
   This->Page_Contour.sWidth + sOffset_X;
  hline = hline->NextHline;
      
  sOrig_Y += top_margin;
  sHeight -= top_margin + bottom_margin;
  sHeightRow = (sHeight-gouttiere_y*(row-1))/row;
  for(y = 0; y < row - 1; y++)
  {
   sOrig_Y += sHeightRow;
   hline->sY = sOrig_Y + sOffset_Y;
   hline->sX_min = This->Page_Contour.sOrig_X + sOffset_X;
   hline->sX_max = This->Page_Contour.sOrig_X + 
    This->Page_Contour.sWidth + sOffset_X;
   hline = hline->NextHline;
   sOrig_Y += gouttiere_y;
   hline->sY = sOrig_Y + sOffset_Y;
   hline->sX_min = This->Page_Contour.sOrig_X + sOffset_X;
   hline->sX_max = This->Page_Contour.sOrig_X + 
    This->Page_Contour.sWidth + sOffset_X;
   hline = hline->NextHline;
  }
 }
}

void destroy_page_typo_mark(This)
c_Page	*This;
{
 int	i,nb_page;
  
 if (This->Page_Typo_Mark)
 {
  nb_page = This->Page_Contour.nb_page_vertical*
   This->Page_Contour.nb_page_horizontal;
  for(i = 0; i < nb_page; i++)
  {
   free_list_v_line(This->Page_Typo_Mark[i].ListHline);
   free_list_h_line(This->Page_Typo_Mark[i].ListVline);
  }
  Xc_free(This->Page_Typo_Mark);
 }
}

void calcul_page_typo_mark(This)
c_Page	*This;
{
 int	i,nb_page;
  
 nb_page = This->Page_Contour.nb_page_vertical*
  This->Page_Contour.nb_page_horizontal;
 for(i = 0; i < nb_page; i++)
 {
  if (This->Page_Typo_Mark[i].visible)
   calcul_list_v_line(This,This->Page_Typo_Mark[i].ListVline);
  if (This->Page_Typo_Mark[i].visible)
   calcul_list_h_line(This,This->Page_Typo_Mark[i].ListHline);
 }
}

void draw_page_typo_mark_rectangle(This,x,y,width,height)
c_Page	*This;	
int	x;
int	y;
int	width;
int	height;
{
 int	i,nb_page;
  
 nb_page = This->Page_Contour.nb_page_vertical*
  This->Page_Contour.nb_page_horizontal;
 for(i = 0; i < nb_page; i++)
 {
  if (This->Page_Typo_Mark[i].visible)
  {
   XSetForeground(_display,_gc_inside,This->Page_Typo_Mark[i].color);
   draw_list_v_line_rectangle(This,This->Page_Typo_Mark[i].ListVline,
			      x,y,width,height);
  }
  if (This->Page_Typo_Mark[i].visible)
  {
   XSetForeground(_display,_gc_inside,This->Page_Typo_Mark[i].color);
   draw_list_h_line_rectangle(This,This->Page_Typo_Mark[i].ListHline,
			      x,y,width,height);
  }
 }
 F(GlobColormap).use_color(GlobColormap,This->X_info.gc_inside,WHITE);
}

void draw_page_typo_mark_polygone(This,polyg)
c_Page	*This;
Polygone	*polyg;
{
 int	i,nb_page;
   
 nb_page = This->Page_Contour.nb_page_vertical*
  This->Page_Contour.nb_page_horizontal;
 for(i = 0; i < nb_page; i++)
 {
  if (This->Page_Typo_Mark[i].visible)
  {
   XSetForeground(_display,_gc_inside,This->Page_Typo_Mark[i].color);
   draw_list_v_line_polygone(This,This->Page_Typo_Mark[i].ListVline,
			     polyg);
  }
  if (This->Page_Typo_Mark[i].visible)
  {
   XSetForeground(_display,_gc_inside,This->Page_Typo_Mark[i].color);
   draw_list_h_line_polygone(This,This->Page_Typo_Mark[i].ListHline,
			     polyg);
  }
 }
 F(GlobColormap).use_color(GlobColormap,This->X_info.gc_inside,WHITE);
}

/*                 ALIGN  MARKS	                      */

void create_page_align_mark(This)
c_Page	*This;
{
 This->Page_Align_Mark.nb_marks_vertical = 
  This->Page_Align_Mark.nb_marks_horizontal = 0;
 This->Page_Align_Mark.visible_X = 
  This->Page_Align_Mark.visible_Y = 
   This->Page_Align_Mark.magnetic_X = 
    This->Page_Align_Mark.magnetic_Y = TRUE;
 This->Page_Align_Mark.magnetic_field_X = 
  This->Page_Align_Mark.magnetic_field_Y = SCALE_FROM_MILLIMETERS(2);
 This->Page_Align_Mark.color_X = 
  This->Page_Align_Mark.color_Y = GlobColormap->Cgreen;
 This->Page_Align_Mark.ListHline = (h_line *)NULL;
 This->Page_Align_Mark.ListVline = (v_line *)NULL;
}

void destroy_page_align_mark(This)
c_Page	*This;
{
 This->Page_Align_Mark.nb_marks_vertical = 
  This->Page_Align_Mark.nb_marks_horizontal = 0;
 free_list_v_line(This->Page_Align_Mark.ListHline);
 free_list_h_line(This->Page_Align_Mark.ListVline);
}

void *add_page_align_mark(This,orient,sX,sY,supXY)
c_Page	*This;
int	orient;
coord_t	sX;
coord_t	sY;
coord_t	supXY;
{
 h_line	*hline;
 v_line	*vline;
  
 int		nb_page;
 coord_t	offset_p;
 coord_t	min_p;
 coord_t	max_p;
  
 switch(orient)
 {
 case ALIGN_MARK_HORIZONTAL:
  offset_p = sY;
  if ((sY<This->Page_Contour.sOrig_Y) || 
      (sY>This->Page_Contour.sOrig_Y + This->Page_Contour.sHeight*
       This->Page_Contour.nb_page_vertical) ||
      (sX<This->Page_Contour.sOrig_X) ||
      (sX>This->Page_Contour.sOrig_X + This->Page_Contour.sWidth*
       This->Page_Contour.nb_page_horizontal))
  {
   min_p=0;
   max_p=This->sDeskArea_Width;
  }
  else
  {
   sX -= This->Page_Contour.sOrig_X;
   nb_page=sX/This->Page_Contour.sWidth;
   min_p=This->Page_Contour.sOrig_X + nb_page*This->Page_Contour.sWidth;
   max_p=min_p + This->Page_Contour.sWidth;
  }
  if (This->Page_Align_Mark.nb_marks_horizontal++)
  {
   hline=This->Page_Align_Mark.ListHline;
   while(hline->NextHline)
    hline=hline->NextHline;
   hline->NextHline = (h_line *)Xc_malloc("aligh",sizeof(h_line));
   hline=hline->NextHline;
  }
  else
  {
   This->Page_Align_Mark.ListHline=hline=
    (h_line *)Xc_malloc("aligh",sizeof(h_line));
  }
  hline->sY=offset_p;
  hline->sX_min=min_p;
  hline->sX_max=max_p;
  hline->Y=(offset_p-This->sOrig_Y)/This->sScale;
  hline->X_min=(min_p-This->sOrig_X)/This->sScale;
  hline->X_max=(max_p-This->sOrig_X)/This->sScale;
  hline->NextHline=(h_line *)NULL;
  draw_h_line(This,hline);
  return (void *)hline;
 case ALIGN_MARK_VERTICAL:
  offset_p = sX;
  if ((sX<This->Page_Contour.sOrig_X) ||
      (sX>This->Page_Contour.sOrig_X + This->Page_Contour.sWidth*
       This->Page_Contour.nb_page_horizontal) ||
      (sY<This->Page_Contour.sOrig_Y) || 
      (sY>This->Page_Contour.sOrig_Y + This->Page_Contour.sHeight*
       This->Page_Contour.nb_page_vertical))
  {
   min_p=0;
   max_p=This->sDeskArea_Height;
  }
  else
  {
   sY -= This->Page_Contour.sOrig_Y;
   nb_page=sY/This->Page_Contour.sHeight;
   min_p=This->Page_Contour.sOrig_Y + nb_page*This->Page_Contour.sHeight;
   max_p=min_p + This->Page_Contour.sHeight;
  }
  if (This->Page_Align_Mark.nb_marks_vertical++)
  {
   vline=This->Page_Align_Mark.ListVline;
   while(vline->NextVline)
    vline=vline->NextVline;
   vline->NextVline = (v_line *)Xc_malloc("aligv",sizeof(v_line));
   vline=vline->NextVline;
  }
  else
  {
   This->Page_Align_Mark.ListVline=vline=
    (v_line *)Xc_malloc("aligv",sizeof(v_line));
  }
  vline->sX=offset_p;
  vline->sY_min=min_p;
  vline->sY_max=max_p;
  vline->X=(offset_p-This->sOrig_X)/This->sScale;
  vline->Y_min=(min_p-This->sOrig_Y)/This->sScale;
  vline->Y_max=(max_p-This->sOrig_Y)/This->sScale;
  vline->NextVline=(v_line *)NULL;
  draw_v_line(This,vline);
  return (void *)vline;
 case ALIGN_MARK_ABS_HORIZONTAL:
  offset_p = sY;
  if (This->Page_Align_Mark.nb_marks_horizontal++)
  {
   hline=This->Page_Align_Mark.ListHline;
   while(hline->NextHline)
    hline = hline->NextHline;
   hline->NextHline = (h_line *)Xc_malloc("aligh",sizeof(h_line));
   hline = hline->NextHline;
  }
  else
  {
   This->Page_Align_Mark.ListHline = hline=
    (h_line *)Xc_malloc("aligh", sizeof(h_line));
  }
  hline->sY = sX;
  hline->sX_min = sY;
  hline->sX_max = supXY;
  hline->Y = (sY-This->sOrig_Y) / This->sScale;
  hline->X_min = (sX-This->sOrig_X) / This->sScale;
  hline->X_max = (supXY-This->sOrig_X) / This->sScale;
  hline->NextHline = (h_line *)NULL;
  /*draw_h_line(This,hline);*/
  return (void *)hline;
 case ALIGN_MARK_ABS_VERTICAL:
  if (This->Page_Align_Mark.nb_marks_vertical++)
  {
   vline=This->Page_Align_Mark.ListVline;
   while(vline->NextVline)
    vline=vline->NextVline;
   vline->NextVline = (v_line *)Xc_malloc("aligv",sizeof(v_line));
   vline=vline->NextVline;
  }
  else
  {
   This->Page_Align_Mark.ListVline=vline=
    (v_line *)Xc_malloc("aligv", sizeof(v_line));
  }
  vline->sX = sX;
  vline->sY_min = sY;
  vline->sY_max=supXY;
  vline->X = (sX-This->sOrig_X) / This->sScale;
  vline->Y_min = (sY-This->sOrig_Y) / This->sScale;
  vline->Y_max = (supXY-This->sOrig_Y) / This->sScale;
  vline->NextVline = (v_line *)NULL;
  /*draw_v_line(This,vline);*/
  return (void *)vline;
 }
 return NULL;			/* pour faire plaisir au compilo */
}

void move_page_align_mark(This,vhline,orient,sX,sY)
c_Page	*This;
void	*vhline;
int	orient;
coord_t	sX;
coord_t	sY;
{
 h_line	*hline;
 v_line	*vline;
  
 int		nb_page;
 coord_t	offset_p;
 coord_t	min_p;
 coord_t	max_p;
  
 if (orient == ALIGN_MARK_HORIZONTAL)
 {
  hline=(h_line *)vhline;
  offset_p=sY;
  if ((sY<This->Page_Contour.sOrig_Y) || 
      (sY>This->Page_Contour.sOrig_Y + This->Page_Contour.sHeight*
       This->Page_Contour.nb_page_vertical) ||
      (sX<This->Page_Contour.sOrig_X) ||
      (sX>This->Page_Contour.sOrig_X + This->Page_Contour.sWidth*
       This->Page_Contour.nb_page_horizontal))
  {
   min_p=0;
   max_p=This->sDeskArea_Width;
  }
  else
  {
   sX -= This->Page_Contour.sOrig_X;
   nb_page=sX/This->Page_Contour.sWidth;
   min_p=This->Page_Contour.sOrig_X + nb_page*This->Page_Contour.sWidth;
   max_p=min_p + This->Page_Contour.sWidth;
  }
  clear_h_line(This,hline);
  hline->sY=offset_p;
  hline->sX_min=min_p;
  hline->sX_max=max_p;
  hline->Y=(offset_p-This->sOrig_Y)/This->sScale;
  hline->X_min=(min_p-This->sOrig_X)/This->sScale;
  hline->X_max=(max_p-This->sOrig_X)/This->sScale;
  draw_h_line(This,hline);
 }  
 else
 {
  vline=(v_line *)vhline;
  offset_p=sX;
  if ((sX<This->Page_Contour.sOrig_X) ||
      (sX>This->Page_Contour.sOrig_X + This->Page_Contour.sWidth*
       This->Page_Contour.nb_page_horizontal) ||
      (sY<This->Page_Contour.sOrig_Y) || 
      (sY>This->Page_Contour.sOrig_Y + This->Page_Contour.sHeight*
       This->Page_Contour.nb_page_vertical))
  {
   min_p=0;
   max_p=This->sDeskArea_Height;
  }
  else
  {
   sY -= This->Page_Contour.sOrig_Y;
   nb_page=sY/This->Page_Contour.sHeight;
   min_p=This->Page_Contour.sOrig_Y + nb_page*This->Page_Contour.sHeight;
   max_p=min_p + This->Page_Contour.sHeight;
  }
  clear_v_line(This,vline);
  vline->sX=offset_p;
  vline->sY_min=min_p;
  vline->sY_max=max_p;
  vline->X=(offset_p-This->sOrig_X)/This->sScale;
  vline->Y_min=(min_p-This->sOrig_Y)/This->sScale;
  vline->Y_max=(max_p-This->sOrig_Y)/This->sScale;
  draw_v_line(This,vline);
 }
}

void remove_page_align_mark(This,vhline,orient)
c_Page	*This;
void	*vhline;
int	orient;
{
 h_line	*hline;
 v_line	*vline;
 h_line	*hline2;
 v_line	*vline2;
 h_line	*hline3;
 v_line	*vline3;
  
 if (orient == ALIGN_MARK_HORIZONTAL)
 {
  hline=(h_line *)vhline;
  hline2=hline3=This->Page_Align_Mark.ListHline;
  while(hline2 != hline)
  {
   hline3=hline2;
   hline2=hline2->NextHline;
  }
  if (hline2==hline3)
  {
   This->Page_Align_Mark.ListHline=hline2->NextHline;
  }
  else
  {
   hline3->NextHline=hline2->NextHline;
  }
  clear_h_line(This,hline);
  Xc_free(hline);
  This->Page_Align_Mark.nb_marks_horizontal--;
 }
 else
 {
  vline=(v_line *)vhline;
  vline2=vline3=This->Page_Align_Mark.ListVline;
  while(vline2 != vline)
  {
   vline3=vline2;
   vline2=vline2->NextVline;
  }
  if (vline2==vline3)
  {
   This->Page_Align_Mark.ListVline=vline2->NextVline;
  }
  else
  {
   vline3->NextVline=vline2->NextVline;
  }
  clear_v_line(This,vline);
  Xc_free(vline);      
  This->Page_Align_Mark.nb_marks_vertical--;
 }
}

void recoordinate_page_align_mark(This)
c_Page	*This;
{
 h_line	*hline;
 h_line	*hline2;
 v_line	*vline;
 v_line	*vline2;
  
 hline = hline2 = This->Page_Align_Mark.ListHline;
 while(hline)
 {
  if (hline->sY > This->sDeskArea_Height)
  {
   if (hline == hline2)
   {
    hline = hline->NextHline;
    This->Page_Align_Mark.ListHline = hline;
   }
   else
   {
    hline2->NextHline = hline->NextHline;
    hline2 = hline;
    hline = hline->NextHline;
   }
   Xc_free(hline2);
   hline2 = hline;
   continue;
  }
  else
  {
   hline->sX_min = 0;
   hline->sX_max = This->sDeskArea_Width;
  }
  hline2 = hline;
  hline = hline->NextHline;
 }
  
 vline = vline2 = This->Page_Align_Mark.ListVline;
 while(vline)
 {
  if (vline->sX > This->sDeskArea_Width)
  {
   if (vline == vline2)
   {
    vline = vline->NextVline;
    This->Page_Align_Mark.ListVline = vline;
   }
   else
   {
    vline2->NextVline = vline->NextVline;
    vline2 = vline;
    vline = vline->NextVline;
   }
   Xc_free(vline2);
   vline2 = vline;
   continue;
  }
  else
  {
   vline->sY_min = 0;
   vline->sY_max = This->sDeskArea_Height;
  }
  vline2 = vline;
  vline = vline->NextVline;
 }
  
 calcul_page_align_mark(This);
}

void	*get_page_align_mark(This, x, y, orient)
c_Page	*This;
int	x;
int	y;
int	*orient;
{
 void		*mini_vhline;
 int		mini_orient = 0;
 coord_t	sX;
 coord_t	sY;
 coord_t	dist;
 coord_t	mini_dist;
 coord_t	min_dist;
 h_line	*hline;
 v_line	*vline;
 boolean	found;
  
 sX = x*This->sScale + This->sOrig_X;
 sY = y*This->sScale + This->sOrig_Y;
 found=FALSE;
 min_dist=mini_dist=3*This->sScale;
 mini_vhline = NULL;

 hline = This->Page_Align_Mark.ListHline;
 while(hline)
 {
  if (sX >= hline->sX_min &&
      sX <= hline->sX_max )
  {
   dist=ABS(sY-hline->sY);
   if (dist<min_dist && dist<mini_dist)
   {
    mini_dist=dist;
    mini_vhline=(void *)hline;
    mini_orient = ALIGN_MARK_HORIZONTAL;
    found=TRUE;
   }
  }
  hline = hline->NextHline;
 }
 vline = This->Page_Align_Mark.ListVline;
 while(vline)
 {
  if (sY >= vline->sY_min &&
      sY <= vline->sY_max )
  {   
   dist=ABS(sX-vline->sX);
   if (dist<min_dist && dist<mini_dist)
   {
    mini_dist=dist;
    mini_vhline=(void *)vline;
    mini_orient = ALIGN_MARK_VERTICAL;
    found=TRUE;
   }
  }
  vline = vline->NextVline;
 }
 if (!found)
  return NULL;
 if (orient)
  *orient = mini_orient;
 return mini_vhline;
}

void calcul_page_align_mark(This)
c_Page	*This;
{
 if (This->Page_Align_Mark.visible_X)
  calcul_list_v_line(This,This->Page_Align_Mark.ListVline);
 if (This->Page_Align_Mark.visible_Y)
  calcul_list_h_line(This,This->Page_Align_Mark.ListHline);
}

void draw_page_align_mark_rectangle(This,x,y,width,height)
c_Page	*This;	
int	x;
int	y;
int	width;
int	height;
{
  
 XSetForeground(_display,_gc_inside,This->Page_Align_Mark.color_X);
 draw_list_v_line_rectangle(This,This->Page_Align_Mark.ListVline,
			    x,y,width,height);
 XSetForeground(_display,_gc_inside,This->Page_Align_Mark.color_Y);
 draw_list_h_line_rectangle(This,This->Page_Align_Mark.ListHline,
			    x,y,width,height);
 F(GlobColormap).use_color(GlobColormap,This->X_info.gc_inside,WHITE);
}

void draw_page_align_mark_polygone(This,polyg)
c_Page	*This;
Polygone	*polyg;
{
  
 XSetForeground(_display,_gc_inside,This->Page_Align_Mark.color_X);
 draw_list_v_line_polygone(This,This->Page_Align_Mark.ListVline,
			   polyg);
 XSetForeground(_display,_gc_inside,This->Page_Align_Mark.color_Y);
 draw_list_h_line_polygone(This,This->Page_Align_Mark.ListHline,
			   polyg);
 F(GlobColormap).use_color(GlobColormap,This->X_info.gc_inside,WHITE);
}


/*               ATTRACT POINT BY LIST LINE            */

coord_t attract_point_by_list_v_line(This,vline,mg_field,svx,vx,sx,sy)
c_Page	*This;
v_line	*vline;
coord_t	mg_field;
coord_t	*svx;
int	*vx;
coord_t	sx;
coord_t	sy;
{
 boolean	found;
 coord_t	dist = MAX_ATTRACT;
  
 found = FALSE;
 while(vline && !found)
 {
  if (sy + This->sOrig_Y >= vline->sY_min &&
      sy + This->sOrig_Y <= vline->sY_max )
  {
   if ((dist = ABS(sx + This->sOrig_X-vline->sX)) <= mg_field)
   {
    *svx = vline->sX - This->sOrig_X;
    *vx = vline->X;
    found = TRUE;
   }
  }
  vline = vline->NextVline;
 }
 if (!found)
  return MAX_ATTRACT;
 return dist;
}

coord_t attract_point_by_list_h_line(This,hline,mg_field,svy,vy,sy,sx)
c_Page	*This;
h_line	*hline;
coord_t	mg_field;
coord_t	*svy;
int	*vy;
coord_t	sy;
coord_t	sx;
{
 boolean	found;
 coord_t	dist = MAX_ATTRACT;
  
 found = FALSE;
 while(hline && !found)
 {
  if (sx + This->sOrig_X >= hline->sX_min &&
      sx + This->sOrig_X <= hline->sX_max )
  {
   if ((dist=ABS(sy + This->sOrig_Y-hline->sY)) <= mg_field)
   {
    *svy = hline->sY - This->sOrig_Y;
    *vy = hline->Y;
    found = TRUE;
   }
  }
  hline = hline->NextHline;
 }
 if (!found)
  return MAX_ATTRACT;
 return dist;
}

/*                    CALCUL LIST LINE                 */      

void calcul_list_v_line(This,vline)
c_Page	*This;
v_line	*vline;
{
 while(vline)
 {
  vline->X=vline->sX/This->sScale-This->Orig_X;
  vline->Y_min=vline->sY_min/This->sScale-This->Orig_Y;
  vline->Y_max=vline->sY_max/This->sScale-This->Orig_Y;
  vline=vline->NextVline;
 }
}

void calcul_list_h_line(This,hline)
c_Page	*This;
h_line	*hline;
{
 while(hline)
 {
  hline->Y=hline->sY/This->sScale-This->Orig_Y;
  hline->X_min=hline->sX_min/This->sScale-This->Orig_X;
  hline->X_max=hline->sX_max/This->sScale-This->Orig_X;
  hline=hline->NextHline;
 }
}

/*                 DRAW LIST LINE RECTANGLE                */

void draw_list_v_line_rectangle(This, vline, x, y, width, height)
c_Page	*This;	
v_line	*vline;
int	x;
int	y;
int	width;
int	height;
{
 int		y1,y2;
  
 while(vline)
 {
  if (vline->X>=x && vline->X<x + width)
  {
   if (!((vline->Y_min<y && vline->Y_max<y) ||
	 (vline->Y_min>=y + height && vline->Y_max>=y + height)))
   {
    y1=MAX(vline->Y_min,y);
    y2=MIN(vline->Y_max,y + height-1);
    XDrawLine(_display,_window,_gc_inside,vline->X,y1,vline->X,y2);
   }
  }
  vline=vline->NextVline; 
 }
}

void draw_list_h_line_rectangle(This,hline, x, y, width, height)
c_Page	*This;	
h_line	*hline;
int	x;
int	y;
int	width;
int	height;
{
 int		x1,x2;
  
 while(hline)
 {
  if (hline->Y>=y && hline->Y<y + height)
  {
   if (!((hline->X_min<x && hline->X_max<x) ||
	 (hline->X_min>=x + width && hline->X_max>=x + width)))
   {
    x1=MAX(hline->X_min,x);
    x2=MIN(hline->X_max,x + width-1);
    XDrawLine(_display,_window,_gc_inside,x1,hline->Y,x2,hline->Y);
   }
  }
  hline=hline->NextHline; 
 }
}

/*                 DRAW LIST LINE POLYGONE                */

void draw_list_v_line_polygone(This,vline,polyg)
c_Page	*This;
v_line	*vline;
Polygone	*polyg;
{
 int 		i,j;
 int		px,py;
 int		mx,my;
 int		nx,ny;
 boolean	found;
 Ligne_h	*cline;
 Segment_h	*cseg;
 int		count_point;

 if (!polyg->nb_lig)
  return;
 px = polyg->Orig_X;
 py = polyg->Orig_Y;
 mx = px + polyg->x_min;
 my = py + polyg->y_min;
 nx = px + polyg->x_max;
 ny = py + polyg->y_max;

 count_point=0;
 while(vline)
 {
  if (vline->X>=mx && vline->X<=nx)
  {
   if (!((vline->Y_min<my && vline->Y_max<my) ||
	 (vline->Y_min>ny && vline->Y_max>ny)))
   {
    cline=polyg->ligne + MAX(vline->Y_min,my)-my;
    for(i = MAX(vline->Y_min, my); i <= MIN(vline->Y_max, ny); i++)
    {
     j=0; found=TRUE;
     cseg=cline->segment;
     while(j<cline->nb_seg && found)
     {
      if (vline->X >= cseg->x_min + px && vline->X <= cseg->x_max + px)
      {
       found=FALSE;
       points[count_point].x = vline->X;
       points[count_point++].y = i;
	      
       if (count_point == MAX_POINT)
       {
	XDrawPoints(_display, _window, _gc_inside,
		    points, MAX_POINT, CoordModeOrigin);
	count_point = 0;
       }
      }
      j++; cseg++;
     }
     cline++;
    }
   }
  }
  vline=vline->NextVline;
 }
 if (count_point)
 {
  XDrawPoints(_display,_window,_gc_inside,points,
	      count_point,CoordModeOrigin);
 }
}

void draw_list_h_line_polygone(This, hline, polyg)
c_Page	*This;
h_line	*hline;
Polygone	*polyg;
{
 int 		i;
 int		px,py;
 int		mx,my;
 int		nx,ny;
 Ligne_h	*cline;
 Segment_h	*cseg;
 int count_seg;

 if (!polyg->nb_lig)
  return;
 px=polyg->Orig_X;
 py=polyg->Orig_Y;
 mx=px + polyg->x_min;
 my=py + polyg->y_min;
 nx=px + polyg->x_max;
 ny=py + polyg->y_max;
  
 cline=polyg->ligne;
 count_seg=0;
  
 while(hline)
 {
  if (hline->Y>=my && hline->Y<=ny)
  {
   cseg=cline[hline->Y-my].segment;
   for (i = 0; i < cline[hline->Y - my].nb_seg; i++)
   {
    if (!((hline->X_min<cseg->x_min + px &&
	   hline->X_max<cseg->x_min + px) ||
	  (hline->X_min>cseg->x_max + px &&
	   hline->X_max>cseg->x_max + px)))
    {
     segments[count_seg].y1 = segments[count_seg].y2 = hline->Y;
     segments[count_seg].x1 = MAX(hline->X_min,cseg->x_min + px);
     segments[count_seg++].x2 = MIN(hline->X_max,cseg->x_max + px);
     if (count_seg == MAX_SEGMENT)
     {
      XDrawSegments(_display,_window,_gc_inside,
		    segments,MAX_SEGMENT);
      count_seg=0;
     }
    }
    cseg++;
   }	  
  }
  hline=hline->NextHline;
 }
 if (count_seg)
 {
  XDrawSegments(_display,_window,_gc_inside,segments,count_seg);
 }
}

/*                 DRAW SHADOW LINE RECTANGLE                */

static void draw_shadow_v_line_rectangle(This, vline, xsgn, ypos, yneg,
					 x, y, width, height)
c_Page	*This;	
v_line	*vline;
int	xsgn;
int	ypos;
int	yneg;
int	x;
int	y;
int	width;
int	height;
{
 int		x1, x2;
 int		y1, y2;
  
 if (xsgn > 0)
 {
  x1 = vline->X + 1;
  x2 = x1 + xsgn - 1;
 }
 else
 {
  x2 = vline->X - 1;
  x1 = x2 + xsgn + 1;    
 }
  
 y1 = vline->Y_min + ypos;
 y2 = vline->Y_max + yneg;
  
 if (x1 < (x + width) && x2 >= x && y1 < (y + height) && y2 >= y)
 {
  x = MAX(x1, x);
  width = MIN(x2, x + width - 1) - x + 1; 
  y = MAX(y1, y);
  height = MIN(y2, y + height - 1) - y + 1;
    
  XFillRectangle(_display, _window, _gc_inside, x, y, width, height);
 }
}

static void draw_shadow_h_line_rectangle(This, hline, ysgn, xpos, xneg,
					 x, y, width, height)
c_Page	*This;	
h_line	*hline;
int	ysgn;
int	xpos;
int	xneg;
int	x;
int	y;
int	width;
int	height;
{
 int		x1, x2;
 int		y1, y2;
  
 if (ysgn > 0)
 {
  y1 = hline->Y + 1;
  y2 = y1 + ysgn - 1;
 }
 else
 {
  y2 = hline->Y - 1;
  y1 = y2 + ysgn + 1;    
 }
  
 x1 = hline->X_min + xpos;
 x2 = hline->X_max + xneg;
  
 if (y1 < (y + height) && y2 >= y && x1 < (x + width) && x2 >= x)
 {
  y = MAX(y1, y);
  height = MIN(y2, y + height - 1) - y + 1;
  x = MAX(x1, x);
  width = MIN(x2, x + width - 1) - x + 1; 
    
  XFillRectangle(_display, _window, _gc_inside, x, y, width, height);
 }
}

/*                 DRAW SHADOW LINE POLYGONE		*/

static void draw_shadow_v_line_polygone(This, vline, xsgn, ypos, yneg, polyg)
c_Page		*This;	
v_line		*vline;
int		xsgn;
int		ypos;
int		yneg;
Polygone	*polyg;
{
 int 		i, j;
 int		px, py;
 int		mx, my;
 int		nx, ny;
 int		x1, x2;
 int		y1, y2;
 boolean	found;
 Ligne_h	*cline;
 Segment_h	*cseg;
 int		count_seg;

 if (!polyg->nb_lig)
  return;
 px = polyg->Orig_X;
 py = polyg->Orig_Y;
 mx = px + polyg->x_min;
 my = py + polyg->y_min;
 nx = px + polyg->x_max;
 ny = py + polyg->y_max;

  
 if (xsgn > 0)
 {
  x1 = vline->X + 1;
  x2 = x1 + xsgn - 1;
 }
 else
 {
  x2 = vline->X - 1;
  x1 = x2 + xsgn + 1;    
 }
  
 y1 = vline->Y_min + ypos;
 y2 = vline->Y_max + yneg;
  
 count_seg = 0;
 if (x1 <= nx && x2 >= mx  && y1 <= ny && y2 >= my)
 {
  cline = polyg->ligne + MAX(y1, my) - my;
    
  for(i = MAX(y1, my); i <= MIN(y2, ny); i++)
  {
   j=0; found=TRUE;
   cseg = cline->segment;
   while(j < cline->nb_seg && found)
   {
    if (x2 >= cseg->x_min + px)
    {
     if (x1 <= cseg->x_max + px)
     {
      segments[count_seg].y1 = segments[count_seg].y2 = i;
      segments[count_seg].x1 = MAX(x1, cseg->x_min + px);
      segments[count_seg++].x2 = MIN(x2, cseg->x_max + px);
      if (count_seg == MAX_SEGMENT)
      {
       XDrawSegments(_display, _window, _gc_inside,
		     segments, MAX_SEGMENT);
       count_seg = 0;
      }
     }
    }
    else
     found = FALSE;
	
    j++; cseg++;
   }
   cline++;
  }
 }
 if (count_seg)
 {
  XDrawSegments(_display, _window, _gc_inside, segments, count_seg);
 }
}

static void draw_shadow_h_line_polygone(This, hline, ysgn, xpos, xneg, polyg)
c_Page		*This;	
h_line		*hline;
int		ysgn;
int		xpos;
int		xneg;
Polygone	*polyg;
{
 int 		i, j;
 int		px, py;
 int		mx, my;
 int		nx, ny;
 int		x1, x2;
 int		y1, y2;
 boolean	found;
 Ligne_h	*cline;
 Segment_h	*cseg;
 int		count_seg;

 if (!polyg->nb_lig)
  return;
 px = polyg->Orig_X;
 py = polyg->Orig_Y;
 mx = px + polyg->x_min;
 my = py + polyg->y_min;
 nx = px + polyg->x_max;
 ny = py + polyg->y_max;

 if (ysgn > 0)
 {
  y1 = hline->Y + 1;
  y2 = y1 + ysgn - 1;
 }
 else
 {
  y2 = hline->Y - 1;
  y1 = y2 + ysgn + 1;    
 }
  
 x1 = hline->X_min + xpos;
 x2 = hline->X_max + xneg;
  
 count_seg = 0;
 if (x1 <= nx && x2 >= mx  && y1 <= ny && y2 >= my)
 {
  cline = polyg->ligne + MAX(y1, my) - my;
    
  for(i = MAX(y1, my); i <= MIN(y2, ny); i++)
  {
   j=0; found=TRUE;
   cseg = cline->segment;
   while(j < cline->nb_seg && found)
   {
    if (x2 >= cseg->x_min + px)
    {
     if (x1 <= cseg->x_max + px)
     {
      segments[count_seg].y1 = segments[count_seg].y2 = i;
      segments[count_seg].x1 = MAX(x1, cseg->x_min + px);
      segments[count_seg++].x2 = MIN(x2, cseg->x_max + px);
      if (count_seg == MAX_SEGMENT)
      {
       XDrawSegments(_display, _window, _gc_inside,
		     segments, MAX_SEGMENT);
       count_seg = 0;
      }
     }
    }
    else
     found = FALSE;
	
    j++; cseg++;
   }
   cline++;
  }
 }
 if (count_seg)
 {
  XDrawSegments(_display, _window, _gc_inside, segments, count_seg);
 }
}


/*		DRAW AND CLEAR SINGLE LINE		*/

void draw_v_line(This,vline)
c_Page	*This;
v_line	*vline;
{
 int 	y1,y2;
 int	x;

 x=vline->X;
 y1=vline->Y_min;
 y2=vline->Y_max;
 if (x>=0 && (y1>=0 || y2>=0))
  F(This).redraw_rectangle(This,x,y1,x,y2);
}

void draw_h_line(This,hline)
c_Page	*This;
h_line	*hline;
{
 int 	x1,x2;
 int	y;
  
 y=hline->Y;
 x1=hline->X_min;
 x2=hline->X_max;
 if (y>=0 && (x1>=0 || x2>=0))
  F(This).redraw_rectangle(This,x1,y,x2,y);
}

void clear_v_line(This,vline)
c_Page	*This;
v_line	*vline;
{
 int 	y1,y2;
 int	x;

 x=vline->X;
 y1=vline->Y_min;
 y2=vline->Y_max;
 if (x>=0 && (y1>=0 || y2>=0))
  XClearArea(This->X_info.display,This->X_info.window,x,y1,1,y2-y1+1,True);
}

void clear_h_line(This,hline)
c_Page	*This;
h_line	*hline;
{
 int 	x1,x2;
 int	y;

 y=hline->Y;
 x1=hline->X_min;
 x2=hline->X_max;
 if (y>=0 && (x1>=0 || x2>=0))
  XClearArea(This->X_info.display,This->X_info.window,x1,y,x2-x1+1,1,True);
}

/*                     FREE LIST LINE                    */         

void free_list_v_line(vline)
v_line	*vline;
{
 v_line	*svgvline;
  
 svgvline=vline;
 while(vline)
 {
  svgvline=vline->NextVline;
  Xc_free(vline);
  vline=svgvline;
 }
}

void free_list_h_line(hline)
h_line	*hline;
{
 h_line	*svghline;
  
 svghline=hline;
 while(hline)
 {
  svghline=hline->NextHline;
  Xc_free(hline);
  hline=svghline;
 }
}

/*                        ALLOC LIST LINE                     */

v_line	*alloc_list_v_line(nb_line, one_block)
int	nb_line;
boolean	one_block;
{
 v_line	*vline;
 v_line	*svgvline = NULL;
 int		i;
 int		block_size;
  
 if (nb_line)
 {
  if (one_block)
  {
   block_size =(( sizeof(h_line) >> sizeof(long) ) + 1 ) << sizeof(long);
   svgvline = vline = 
    (v_line *)Xc_malloc("v_line2", block_size * nb_line);
   for (i = 1; i < nb_line; i++)
   {
    vline->NextVline = (v_line *)(((char *)svgvline)+(i*block_size));
    vline = vline->NextVline;
   }
   vline->NextVline = NULL;
  }
  else
  {
   svgvline = vline = (v_line *)Xc_malloc("v_line",sizeof(v_line));
   for (i = 1; i < nb_line; i++)
   {
    vline->NextVline = (v_line *)Xc_malloc("v_line",sizeof(v_line));
    vline = vline->NextVline;
   }
   vline->NextVline = NULL;
  }
 }
 return svgvline;
}

h_line	*alloc_list_h_line(nb_line, one_block)
int	nb_line;
boolean	one_block;
{
 h_line	*hline;
 h_line	*svghline = NULL;
 int		i;
 int		block_size;

 if (nb_line)
 {
  if (one_block)
  {
   block_size =(( sizeof(h_line) >> sizeof(long) ) + 1 ) << sizeof(long);
   svghline = hline = 
    (h_line *)Xc_malloc("h_line2", block_size * nb_line);
   for(i = 1; i < nb_line; i++)
   {
    hline->NextHline = (h_line *)(((char *)svghline)+(i*block_size));
    hline=hline->NextHline;
   }
   hline->NextHline=NULL;
  }
  else
  {
   svghline = hline = (h_line *)Xc_malloc("h_line",sizeof(h_line));
   for(i = 1; i < nb_line; i++)
   {
    hline->NextHline = (h_line *)Xc_malloc("h_line",sizeof(h_line));
    hline = hline->NextHline;
   }
   hline->NextHline = NULL;
  }
 }
 return svghline;
}


