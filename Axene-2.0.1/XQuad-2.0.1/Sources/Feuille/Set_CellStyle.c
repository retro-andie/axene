/*
** Set_CellStyle.c for XQuad in Feuille/
** Set cellules style
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
** Started on  Mon Apr  3 15:33:22 1995 Emmanuel Paris
** Last update Mon Apr 14 18:18:57 1997 Emmanuel Paris
*/

#define _CELLSTYLE_PATTERN_
#include "Feuille.h"

enum
{
 SET_NONE = -1, SET_LBS = 0, UNSET_LBS, SET_RBS, UNSET_RBS, SET_UBS, UNSET_UBS,
 SET_DBS, UNSET_DBS, SET_LBD, SET_RBD, SET_UBD, SET_DBD, SET_LUBD, SET_LDBD,
 SET_RUBD, SET_RDBD, SET_LRBD, SET_UDBD, SET_LURBD, SET_URDBD, SET_RDLBD,
 SET_DLUBD, SET_ALL_B, UNSET_ALL_B, ALG_DEFAULT, ALG_H_DEFAULT, ALG_H_LEFT,
 ALG_H_RIGHT, ALG_H_CENTER, JUSTIF_H, ALG_V_TOP, ALG_V_BOTTOM, ALG_V_CENTER,
 JUSTIF_V, MULTI_L, MONO_L, ANGL_0, ANGL_90, ANGL_270, BACK_PATTERN
};

#define NB_CONST_FONT_SIZE	21
static coord_t dft_font_size[NB_CONST_FONT_SIZE] =
{
 SCALE_FROM_POINTS(4), SCALE_FROM_POINTS(6), SCALE_FROM_POINTS(8),
 SCALE_FROM_POINTS(9), SCALE_FROM_POINTS(10), SCALE_FROM_POINTS(12),
 SCALE_FROM_POINTS(14), SCALE_FROM_POINTS(16), SCALE_FROM_POINTS(18),
 SCALE_FROM_POINTS(24),
 SCALE_FROM_POINTS(36), SCALE_FROM_POINTS(64), SCALE_FROM_POINTS(100),
 SCALE_FROM_POINTS(200), SCALE_FROM_POINTS(300), SCALE_FROM_POINTS(400),
 SCALE_FROM_POINTS(500), SCALE_FROM_POINTS(600), SCALE_FROM_POINTS(700),
 SCALE_FROM_POINTS(800), SCALE_FROM_POINTS(900)
};

static void push_region_stack();
static void flush_region_stack();

void unset_all_border();
void set_border_left();
void set_border_right();
void set_border_up();
void set_border_down();
void set_all_border();

void set_ruler_default ___PROTO((c_Feuille *This));
void set_ruler_align_h_left ___PROTO((c_Feuille *This));
void set_ruler_align_h_right ___PROTO((c_Feuille *This));
void set_ruler_align_h_center ___PROTO((c_Feuille *This));
void set_ruler_justify_h ___PROTO((c_Feuille *This));
void set_ruler_align_v_top ___PROTO((c_Feuille *This));
void set_ruler_align_v_bottom ___PROTO((c_Feuille *This));
void set_ruler_align_v_center ___PROTO((c_Feuille *This));
void set_ruler_justify_v ___PROTO((c_Feuille *This));
void set_ruler_multiline ___PROTO((c_Feuille *This));
void set_ruler_monoline ___PROTO((c_Feuille *This));
void set_ruler_angle_0 ___PROTO((c_Feuille *This));
void set_ruler_angle_90 ___PROTO((c_Feuille *This));
void set_ruler_angle_270 ___PROTO((c_Feuille *This));

void set_font_bold ___PROTO((c_Feuille *This));
void set_font_italic ___PROTO((c_Feuille *This));
void set_font_underline ___PROTO((c_Feuille *This));
void set_font_strikeout ___PROTO((c_Feuille *This));
void set_font_subscript ___PROTO((c_Feuille *This));
void set_font_supscript ___PROTO((c_Feuille *This));
void set_font_shadow ___PROTO((c_Feuille *This));
void set_font_outline ___PROTO((c_Feuille *This));
void set_font_small_caps ___PROTO((c_Feuille *This));
void set_font_big_caps ___PROTO((c_Feuille *This));

static void set_font_bold_callback();
static void set_font_italic_callback();
static void set_font_underline_callback();
static void set_font_strikeout_callback();
static void set_font_subscript_callback();
static void set_font_supscript_callback();
static void set_font_shadow_callback();
static void set_font_outline_callback();
static void set_font_small_caps_callback();
static void set_font_big_caps_callback();

void set_font_inc_size ___PROTO((c_Feuille *This));
void set_font_dec_size ___PROTO((c_Feuille *This));
static void set_font_inc_dec_size_callback();

void set_format_standard ___PROTO((c_Feuille *This));
void set_format_percent ___PROTO((c_Feuille *This));
void set_format_currency ___PROTO((c_Feuille *This));
void set_format_thousands ___PROTO((c_Feuille *This));
void set_format_scientific ___PROTO((c_Feuille *This));
void set_format_engineer ___PROTO((c_Feuille *This));
void set_format_add_digit ___PROTO((c_Feuille *This));
void set_format_sub_digit ___PROTO((c_Feuille *This));
static void set_format_add_sub_digit_callback();

void set_cell_backcolor ___PROTO((c_Feuille *This, c_Color *color));
static void set_cell_backcolor_callback();
void set_cell_background ___PROTO((c_Feuille *This));
void set_cell_inverse ___PROTO((c_Feuille *This));
static void set_cell_inverse_callback();

static void set_cellstyle_with_callback();
static void set_cellstyle_callback();
void set_cell_inside();
static void set_cell_inside_callback();
void set_cell_borders();
static void set_cell_borders_callback();
void unset_cell_inside();
static void unset_cell_inside_callback();
void unset_cellstyle_all();

void unset_cell_formatnbr();
static void unset_cell_formatnbr_callback();
void unset_cell_textstyle();
static void unset_cell_textstyle_callback();
void unset_cell_formulas();
void unset_cell_all();
void set_cell_formatnbr();
static void set_cell_formatnbr_callback();
void set_cell_textstyle();
static void set_cell_textstyle_callback();
void set_cell_fontfamily();
static void set_cell_fontfamily_callback();
void set_cell_fontcolor();
static void set_cell_fontcolor_callback();
void set_cell_fontsize();
static void set_cell_fontsize_callback();


static void push_region_stack(stack, region)
region_t	**stack;
region_t	*region;
{
 region_t	*dup_region;
  
 dup_region = (region_t *)Xc_malloc("dup region", sizeof(region_t));
 memcpy(dup_region, region, sizeof(region_t));
  
 dup_region->NextRegion = *stack;
 *stack = dup_region;
}

static void flush_region_stack(stack)
region_t	*stack;     
{
 region_t	*region;
  
 while(stack)
 {
  region = stack->NextRegion;
  Xc_free(stack);
  stack = region;
 }
}

void unset_all_border(This)
c_Feuille	*This;
{
 set_cellstyle_with_callback(This,  UNSET_ALL_B);
 Xc_TRACE(("unset all border"));
}

void set_border_left(This)
c_Feuille	*This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if (cellstyle->left.border_line_thickness)
  set_cellstyle_with_callback(This, UNSET_LBS);
 else
  set_cellstyle_with_callback(This, SET_LBS);
 Xc_TRACE(("set border left"));
}

void set_border_right(This)
c_Feuille	*This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
  
 if (cellstyle->right.border_line_thickness)
  set_cellstyle_with_callback(This, UNSET_RBS);
 else
  set_cellstyle_with_callback(This, SET_RBS);
 Xc_TRACE(("set border right"));
}

void set_border_up(This)
c_Feuille	*This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if (cellstyle->up.border_line_thickness)
  set_cellstyle_with_callback(This, UNSET_UBS);
 else
  set_cellstyle_with_callback(This, SET_UBS);
 Xc_TRACE(("set border up"));
}

void set_border_down(This)
c_Feuille	*This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
  
 if (cellstyle->down.border_line_thickness)
  set_cellstyle_with_callback(This, UNSET_DBS);
 else
  set_cellstyle_with_callback(This, SET_DBS);
 Xc_TRACE(("set border down"));
}

void set_all_border(This)
c_Feuille	*This;
{
 region_t	*stack;
 region_t	*region;
 region_t	regional;
  
 region = This->cell_region;
 stack = NULL;
 while(region)
 {
  if (region->x_min == region->x_max)
  {
   if (region->y_min == region->y_max)
   {
    F(This->moteur).or_cell_style(This->moteur, region, 
				  set_cellstyle_callback, 
				  SET_ALL_B);
    push_region_stack(&stack, region);
   }
   else
   {
    if (region->y_min == 0 && region->y_max == ROW_MAX)
    {
     F(This->moteur).or_cell_style(This->moteur, region, 
				   set_cellstyle_callback, 
				   SET_LRBD);
     push_region_stack(&stack, region);
    }
    else
    {
     regional.x_min = regional.x_max = region->x_min;
     regional.y_min = regional.y_max = region->y_min;
     F(This->moteur).or_cell_style(This->moteur, &regional, 
				   set_cellstyle_callback, 
				   SET_LURBD);
     push_region_stack(&stack, &regional);
     regional.y_min++;
     regional.y_max = region->y_max - 1;
     if (regional.y_max >= regional.y_min)
     {
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cellstyle_callback, 
				    SET_LRBD);
      push_region_stack(&stack, &regional);
     }
     regional.y_min = ++regional.y_max;
     F(This->moteur).or_cell_style(This->moteur, &regional, 
				   set_cellstyle_callback, 
				   SET_RDLBD);
     push_region_stack(&stack, &regional);
    }
   }
  }
  else
  {
   if (region->y_min == region->y_max)
   {
    if (region->x_min == 0 && region->x_max == COLUMN_MAX)
    {
     F(This->moteur).or_cell_style(This->moteur, region, 
				   set_cellstyle_callback, 
				   SET_UDBD);	  
     push_region_stack(&stack, region);
    }
    else
    {
     regional.x_min = regional.x_max = region->x_min;
     regional.y_min = regional.y_max = region->y_min;
     F(This->moteur).or_cell_style(This->moteur, &regional, 
				   set_cellstyle_callback, 
				   SET_DLUBD);
     push_region_stack(&stack, &regional);
     regional.x_min++;
     regional.x_max = region->x_max - 1;
     if (regional.x_max >= regional.x_min)
     {
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cellstyle_callback, 
				    SET_UDBD);
      push_region_stack(&stack, &regional);
     }
     regional.x_min = ++regional.x_max;
     F(This->moteur).or_cell_style(This->moteur, &regional, 
				   set_cellstyle_callback, 
				   SET_URDBD);
     push_region_stack(&stack, &regional);
    }
   }
   else
   {
    if (region->x_min == 0 && region->x_max == COLUMN_MAX)
    {
     regional.x_min = region->x_min;
     regional.x_max = region->x_max;
     if (region->y_min != 0)
     {
      regional.y_min = regional.y_max = region->y_min;
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cellstyle_callback, 
				    SET_UBD);
      push_region_stack(&stack, &regional);
     }
     if (region->y_max != ROW_MAX)
     {
      regional.y_min = regional.y_max = region->y_max;
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cellstyle_callback, 
				    SET_DBD);
      push_region_stack(&stack, &regional);
     }
    }
    else
    {
     if (region->y_min == 0 && region->y_max == ROW_MAX)
     {
      regional.y_min = region->y_min;
      regional.y_max = region->y_max;
      if (region->x_min != 0)
      {
       regional.x_min = regional.x_max = region->x_min;
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cellstyle_callback, 
				     SET_LBD);
       push_region_stack(&stack, &regional);
      }
      if (region->x_max != COLUMN_MAX)
      {
       regional.x_min = regional.x_max = region->x_max;
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cellstyle_callback, 
				     SET_RBD);
       push_region_stack(&stack, &regional);
      }
     }	
     else
     {
      regional.x_min = regional.x_max = region->x_min;
      regional.y_min = regional.y_max = region->y_min;
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cellstyle_callback, 
				    SET_LUBD);
      push_region_stack(&stack, &regional);
      regional.x_min++;
      regional.x_max = region->x_max - 1;
      if (regional.x_max >= regional.x_min)
      {
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cellstyle_callback, 
				     SET_UBD);
       push_region_stack(&stack, &regional);
      }  
      regional.x_min = ++regional.x_max;
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cellstyle_callback, 
				    SET_RUBD);
      push_region_stack(&stack, &regional);
      regional.y_min++;
      regional.y_max = region->y_max - 1;
      if (regional.y_max >= regional.y_min)
      {
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cellstyle_callback, 
				     SET_RBD);
       push_region_stack(&stack, &regional);
      }
      regional.y_min = ++regional.y_max;
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cellstyle_callback, 
				    SET_RDBD);
      push_region_stack(&stack, &regional);
      regional.x_max --;
      regional.x_min = region->x_min + 1;
      if (regional.x_max >= regional.x_min)
      {
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cellstyle_callback, 
				     SET_DBD);
       push_region_stack(&stack, &regional);
      }
      regional.x_max = --regional.x_min;
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cellstyle_callback, 
				    SET_LDBD);
      push_region_stack(&stack, &regional);
      regional.y_max --;
      regional.y_min = region->y_min + 1;
      if (regional.y_max >= regional.y_min)
      {
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cellstyle_callback, 
				     SET_LBD);
       push_region_stack(&stack, &regional);
      }
     }
    }
   }
  }
  region = region->NextRegion;
 }
 if (stack)
 {
  F(This).RefreshRegion(This, stack, Xq_CELLS);
  flush_region_stack(stack);
 }
 Xc_TRACE(("set all border"));
}

void set_ruler_default(This)
c_Feuille *This;
{
 set_cellstyle_with_callback(This,  ALG_DEFAULT);
 Xc_TRACE(("set alignment default"));
}

void set_ruler_align_h_left(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if ((cellstyle->ruler_type & XqR_H_ALIGN_MASK) == XqR_H_ALIGN_LEFT)
 {
  set_cellstyle_with_callback(This, ALG_H_DEFAULT);
 }
 else
 {
  set_cellstyle_with_callback(This, ALG_H_LEFT);
 }

 Xc_TRACE(("set alignment horizontal left"));
}

void set_ruler_align_h_right(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if ((cellstyle->ruler_type & XqR_H_ALIGN_MASK) == XqR_H_ALIGN_RIGHT)
 {
  set_cellstyle_with_callback(This, ALG_H_DEFAULT);
 }
 else
 {
  set_cellstyle_with_callback(This, ALG_H_RIGHT);
 }
 Xc_TRACE(("set alignment horizontal right"));
}

void set_ruler_align_h_center(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if ((cellstyle->ruler_type & XqR_H_ALIGN_MASK) == XqR_H_ALIGN_CENTER)
 {
  set_cellstyle_with_callback(This, ALG_H_DEFAULT);
 }
 else
 {
  set_cellstyle_with_callback(This, ALG_H_CENTER);
 }
 Xc_TRACE(("set alignment horizontal center"));
}

void set_ruler_justify_h(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if ((cellstyle->ruler_type & XqR_H_ALIGN_MASK) == XqR_H_JUSTIFIED)
 {
  set_cellstyle_with_callback(This, ALG_H_DEFAULT);
 }
 else
 {
  set_cellstyle_with_callback(This, JUSTIF_H);
 }
 Xc_TRACE(("set justification in horizontal"));
}

void set_ruler_align_v_top(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if ((cellstyle->ruler_type & XqR_V_ALIGN_MASK) == XqR_V_ALIGN_TOP)
 {
  set_cellstyle_with_callback(This, ALG_V_CENTER);
 }
 else
 {
  set_cellstyle_with_callback(This, ALG_V_TOP);
 }
 Xc_TRACE(("set alignment vertical top"));
}

void set_ruler_align_v_bottom(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if ((cellstyle->ruler_type & XqR_V_ALIGN_MASK) == XqR_V_ALIGN_BOTTOM)
 {
  set_cellstyle_with_callback(This, ALG_V_CENTER);
 }
 else
 {
  set_cellstyle_with_callback(This, ALG_V_BOTTOM);
 }
 Xc_TRACE(("set alignment vertical bottom"));
}

void set_ruler_align_v_center(This)
c_Feuille *This;
{
 set_cellstyle_with_callback(This, ALG_V_CENTER);
 Xc_TRACE(("set alignment vertical center"));
}

void set_ruler_justify_v(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if ((cellstyle->ruler_type & XqR_V_ALIGN_MASK) == XqR_V_JUSTIFIED)
 {
  set_cellstyle_with_callback(This, ALG_V_CENTER);
 }
 else
 {
  set_cellstyle_with_callback(This, JUSTIF_V);
 }
 Xc_TRACE(("set justification in vertical"));
}

void set_ruler_multiline(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if (cellstyle->ruler_type & XqR_MULTILINE)
 {
  set_cellstyle_with_callback(This, MONO_L);
 }
 else
 {
  set_cellstyle_with_callback(This, MULTI_L);
 }
 Xc_TRACE(("set multiline mode"));
}

void set_ruler_monoline(This)
c_Feuille *This;
{
 set_cellstyle_with_callback(This,  MONO_L);
 Xc_TRACE(("set monoline mode"));
}

void set_ruler_angle_0(This)
c_Feuille *This;
{
 set_cellstyle_with_callback(This,  ANGL_0);
 Xc_TRACE(("set angle 0 mode"));
}

void set_ruler_angle_90(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if ((cellstyle->ruler_type & XqR_A_MASK) == XqR_A_90)
 {
  set_cellstyle_with_callback(This, ANGL_0);
 }
 else
 {
  set_cellstyle_with_callback(This, ANGL_90);
 }
 Xc_TRACE(("set angle 90 mode"));
}

void set_ruler_angle_270(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);

 if ((cellstyle->ruler_type & XqR_A_MASK) == XqR_A_270)
 {
  set_cellstyle_with_callback(This, ANGL_0);
 }
 else
 {
  set_cellstyle_with_callback(This, ANGL_270);
 }
 Xc_TRACE(("set angle 270 mode"));
}

static void set_cellstyle_with_callback(This, type)
c_Feuille	*This;
int		type;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_cellstyle_callback, type);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
}

static void set_cellstyle_callback(moteur, cellstyle, region, data)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
int		data;
{
 CellStyle_t	*new_style;
 c_CellStyle	*cs_base;
 int		ruler;

 cs_base = moteur->BaseStd->cell_style_base;
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 switch(data)
 {
 case SET_LBS:
  SET_CELL_STYLE(new_style, XqCS_LEFT_PATTERN, CSP_SINGLE_LINE,
		 XqCS_END);
  break;
 case UNSET_LBS:
  SET_CELL_STYLE(new_style, XqCS_LEFT_PATTERN, PATTERN_NONE,
		 XqCS_END);
  break;
 case SET_RBS:
  SET_CELL_STYLE(new_style, XqCS_RIGHT_PATTERN, CSP_SINGLE_LINE,
		 XqCS_END);
  break;
 case UNSET_RBS:
  SET_CELL_STYLE(new_style, XqCS_RIGHT_PATTERN, PATTERN_NONE,
		 XqCS_END);
  break;
 case SET_UBS:
  SET_CELL_STYLE(new_style, XqCS_TOP_PATTERN, CSP_SINGLE_LINE,
		 XqCS_END);
  break;
 case UNSET_UBS:
  SET_CELL_STYLE(new_style, XqCS_TOP_PATTERN, PATTERN_NONE,
		 XqCS_END);
  break;
 case SET_DBS:
  SET_CELL_STYLE(new_style, XqCS_BOTTOM_PATTERN, CSP_SINGLE_LINE,
		 XqCS_END);
  break;
 case UNSET_DBS:
  SET_CELL_STYLE(new_style, XqCS_BOTTOM_PATTERN, PATTERN_NONE,
		 XqCS_END);
  break;
 case SET_LBD:
  SET_CELL_STYLE(new_style, XqCS_LEFT_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_END);
  break;
 case SET_RBD:
  SET_CELL_STYLE(new_style, XqCS_RIGHT_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_END);
  break;
 case SET_UBD:
  SET_CELL_STYLE(new_style, XqCS_TOP_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_END);
  break;
 case SET_DBD:
  SET_CELL_STYLE(new_style, XqCS_BOTTOM_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_END);
  break;
 case SET_LRBD:
  SET_CELL_STYLE(new_style, XqCS_LEFT_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_RIGHT_PATTERN, CSP_DOUBLE_LINE, XqCS_END);
  break;
 case SET_UDBD:
  SET_CELL_STYLE(new_style, XqCS_TOP_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_BOTTOM_PATTERN, CSP_DOUBLE_LINE, XqCS_END);
  break;
 case SET_LUBD:
  SET_CELL_STYLE(new_style, XqCS_LEFT_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_TOP_PATTERN, CSP_DOUBLE_LINE, XqCS_END);
  break;
 case SET_LDBD:
  SET_CELL_STYLE(new_style, XqCS_LEFT_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_BOTTOM_PATTERN, CSP_DOUBLE_LINE, XqCS_END);
  break;
 case SET_RUBD:
  SET_CELL_STYLE(new_style, XqCS_RIGHT_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_TOP_PATTERN, CSP_DOUBLE_LINE, XqCS_END);
  break;
 case SET_RDBD:
  SET_CELL_STYLE(new_style, XqCS_RIGHT_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_BOTTOM_PATTERN, CSP_DOUBLE_LINE, XqCS_END);
  break;
 case SET_LURBD:
  SET_CELL_STYLE(new_style, XqCS_LEFT_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_TOP_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_RIGHT_PATTERN, CSP_DOUBLE_LINE, XqCS_END);
  break;
 case SET_URDBD:
  SET_CELL_STYLE(new_style, XqCS_TOP_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_RIGHT_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_BOTTOM_PATTERN, CSP_DOUBLE_LINE, XqCS_END);
  break;
 case SET_RDLBD:
  SET_CELL_STYLE(new_style, XqCS_RIGHT_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_BOTTOM_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_LEFT_PATTERN, CSP_DOUBLE_LINE, XqCS_END);
  break;
 case SET_DLUBD:
  SET_CELL_STYLE(new_style, XqCS_BOTTOM_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_LEFT_PATTERN, CSP_DOUBLE_LINE, 
		 XqCS_TOP_PATTERN, CSP_DOUBLE_LINE, XqCS_END);
  break;
 case SET_ALL_B:
  SET_CELL_STYLE(new_style, XqCS_LEFT_PATTERN, CSP_DOUBLE_LINE, 
		 XqCS_TOP_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_RIGHT_PATTERN, CSP_DOUBLE_LINE,
		 XqCS_BOTTOM_PATTERN, CSP_DOUBLE_LINE, XqCS_END);
  break;
 case UNSET_ALL_B:
  SET_CELL_STYLE(new_style, XqCS_LEFT_PATTERN, PATTERN_NONE,
		 XqCS_TOP_PATTERN, PATTERN_NONE,
		 XqCS_RIGHT_PATTERN, PATTERN_NONE,
		 XqCS_BOTTOM_PATTERN, PATTERN_NONE, XqCS_END);
  break;
 case ALG_DEFAULT:
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, XqR_DEFAULT, XqCS_END);
  break;
 case ALG_H_DEFAULT:
  ruler = (new_style->ruler_type & (~XqR_H_ALIGN_MASK));
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case ALG_H_LEFT:
  ruler = (new_style->ruler_type & (~XqR_H_ALIGN_MASK)) | XqR_H_ALIGN_LEFT;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case ALG_H_RIGHT:
  ruler = (new_style->ruler_type & (~XqR_H_ALIGN_MASK)) |XqR_H_ALIGN_RIGHT;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case ALG_H_CENTER:
  ruler = (new_style->ruler_type & (~XqR_H_ALIGN_MASK))|XqR_H_ALIGN_CENTER;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case JUSTIF_H:
  ruler = (new_style->ruler_type & (~XqR_H_ALIGN_MASK)) | XqR_H_JUSTIFIED;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case ALG_V_TOP:
  ruler = (new_style->ruler_type & (~XqR_V_ALIGN_MASK)) | XqR_V_ALIGN_TOP;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case ALG_V_BOTTOM:
  ruler = (new_style->ruler_type & (~XqR_V_ALIGN_MASK))|XqR_V_ALIGN_BOTTOM;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case ALG_V_CENTER:
  ruler = (new_style->ruler_type & (~XqR_V_ALIGN_MASK))|XqR_V_ALIGN_CENTER;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case JUSTIF_V:
  ruler = (new_style->ruler_type & (~XqR_V_ALIGN_MASK)) | XqR_V_JUSTIFIED;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case MULTI_L:
  ruler = new_style->ruler_type | XqR_MULTILINE;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case MONO_L:
  ruler = new_style->ruler_type & (~XqR_MULTILINE);
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case ANGL_0:
  ruler = (new_style->ruler_type & (~XqR_A_MASK)) | XqR_A_DEFAULT;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case ANGL_90:
  ruler = (new_style->ruler_type & (~XqR_A_MASK)) | XqR_A_90;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case ANGL_270:
  ruler = (new_style->ruler_type & (~XqR_A_MASK)) | XqR_A_270;
  SET_CELL_STYLE(new_style, XqCS_ALIGNEMENT, ruler, XqCS_END);
  break;
 case BACK_PATTERN:
  SET_CELL_STYLE(new_style, XqCS_PATTERN, CELL_PATTERN_DATA_FIRST + 2,
		 XqCS_END);
  break;
 }
 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 Xc_TRACE(("cstyle: %p", new_style));
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_font_bold(This)
c_Feuille *This;
{ 
 CellStyle_t	*cstyle;
 c_TextStyle	*tstyle;
 region_t	*region;
 int		set_bold;

 cstyle = 
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
  
 tstyle = cstyle->text_style;
  
 if ((tstyle->attributes & XcTS_B_BOLD) != 0)
  set_bold = FALSE;
 else
  set_bold = TRUE;
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_font_bold_callback, set_bold);
  region = region->NextRegion;
 }

 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set in region in bold font"));
}

static void set_font_bold_callback(moteur, cellstyle, region, set_bold)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		set_bold;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
  
 cs_base = moteur->BaseStd->cell_style_base;
  
 tstyle = cellstyle->text_style;
  
 if (set_bold)
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_BOLD_ON,
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_BOLD,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_BOLD_OFF, 
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_BOLD,
			     XcTS_END);

 Xc_TRACE(("new style name: %s", new_tstyle->name));
  
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}


void set_font_italic(This)
c_Feuille *This;
{
 CellStyle_t	*cstyle;
 c_TextStyle	*tstyle;
 region_t	*region;
 int		set_italic;
  
 cstyle = 
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
  
 tstyle = cstyle->text_style;
  
 if ((tstyle->attributes & XcTS_B_ITALIC) != 0)
  set_italic = FALSE;
 else
  set_italic = TRUE;

 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_font_italic_callback, set_italic);
  region = region->NextRegion;
 }

 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set in region in italic font"));
}

static void set_font_italic_callback(moteur, cellstyle, region, set_italic)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		set_italic;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
  
 cs_base = moteur->BaseStd->cell_style_base;

 tstyle = cellstyle->text_style;
  
 if (set_italic)
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_ITALIC_ON,
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_ITALIC,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_ITALIC_OFF, 
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_ITALIC,
			     XcTS_END);

 Xc_TRACE(("new style name: %s", new_tstyle->name));
  
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_font_underline(This)
c_Feuille *This;
{
 CellStyle_t	*cstyle;
 c_TextStyle	*tstyle;
 region_t	*region;
 int		set_underline;
  
 cstyle = 
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
  
 tstyle = cstyle->text_style;
  
 if ((tstyle->attributes & XcTS_B_UNDERLINE/* XcTS_B_STRIKEOUT */) != 0)
  set_underline = FALSE;
 else
  set_underline = TRUE;

 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style
   (This->moteur, region, set_font_underline_callback, set_underline);
  region = region->NextRegion;
 }
  
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set in region in underline font"));
}

static void set_font_underline_callback(moteur, cellstyle,
					region, set_underline)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		set_underline;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
  
 cs_base = moteur->BaseStd->cell_style_base;

 tstyle = cellstyle->text_style;
  
 if (set_underline)
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_UNDERLINE_ON,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_UNDERLINE,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_UNDERLINE_OFF,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_UNDERLINE,
			     XcTS_END);
  
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_font_strikeout(This)
c_Feuille *This;
{
 CellStyle_t	*cstyle;
 c_TextStyle	*tstyle;
 region_t	*region;
 int		set_strikeout;
  
 cstyle = 
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
  
 tstyle = cstyle->text_style;
  
 if ((tstyle->attributes & XcTS_B_STRIKEOUT/* XcTS_B_STRIKEOUT */) != 0)
  set_strikeout = FALSE;
 else
  set_strikeout = TRUE;

 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style
   (This->moteur, region, set_font_strikeout_callback, set_strikeout);
  region = region->NextRegion;
 }
  
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set in region in strikeout font"));
}

static void set_font_strikeout_callback(moteur, cellstyle,
					region, set_strikeout)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		set_strikeout;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
  
 cs_base = moteur->BaseStd->cell_style_base;

 tstyle = cellstyle->text_style;
  
 if (set_strikeout)
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_STRIKEOUT_ON,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_STRIKEOUT,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_STRIKEOUT_OFF,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_STRIKEOUT,
			     XcTS_END);
  
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_font_subscript(This)
c_Feuille *This;
{
 CellStyle_t	*cstyle;
 c_TextStyle	*tstyle;
 region_t	*region;
 int		set_subscript;
  
 cstyle = 
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
  
 tstyle = cstyle->text_style;
  
 if ((tstyle->attributes & XcTS_B_SUBSCRIPT/* XcTS_B_STRIKEOUT */) != 0)
  set_subscript = FALSE;
 else
  set_subscript = TRUE;

 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style
   (This->moteur, region, set_font_subscript_callback, set_subscript);
  region = region->NextRegion;
 }
 
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set in region in subscript font"));
}

static void set_font_subscript_callback(moteur, cellstyle,
					region, set_subscript)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		set_subscript;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
  
 cs_base = moteur->BaseStd->cell_style_base;

 tstyle = cellstyle->text_style;
 if (set_subscript)
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SUBSCRIPT_ON,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_SUBSCRIPT,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SUBSCRIPT_OFF,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_SUBSCRIPT,
			     XcTS_END);
  
 Xc_TRACE(("new style name: %s", new_tstyle->name));
  
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_font_supscript(This)
c_Feuille *This;
{
 CellStyle_t	*cstyle;
 c_TextStyle	*tstyle;
 region_t	*region;
 int		set_supscript;
  
 cstyle = 
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
  
 tstyle = cstyle->text_style;
  
 if ((tstyle->attributes & XcTS_B_SUPSCRIPT/* XcTS_B_STRIKEOUT */) != 0)
  set_supscript = FALSE;
 else
  set_supscript = TRUE;

 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style
   (This->moteur, region, set_font_supscript_callback, set_supscript);
  region = region->NextRegion;
 }
  
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set in region in supscript font"));
}

static void set_font_supscript_callback(moteur, cellstyle,
					region, set_supscript)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		set_supscript;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
  
 cs_base = moteur->BaseStd->cell_style_base;

 tstyle = cellstyle->text_style;
 if (set_supscript)
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SUPSCRIPT_ON,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_SUPSCRIPT,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SUPSCRIPT_OFF,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_SUPSCRIPT,
			     XcTS_END);
 
 Xc_TRACE(("new style name: %s", new_tstyle->name));
  
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_font_shadow(This)
c_Feuille *This;
{
 CellStyle_t	*cstyle;
 c_TextStyle	*tstyle;
 region_t	*region;
 int		set_shadow;
  
 cstyle = 
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
  
 tstyle = cstyle->text_style;
  
 if ((tstyle->attributes & XcTS_B_SHADOW/* XcTS_B_STRIKEOUT */) != 0)
  set_shadow = FALSE;
 else
  set_shadow = TRUE;

 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style
   (This->moteur, region, set_font_shadow_callback, set_shadow);
  region = region->NextRegion;
 }
  
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set in region in shadow font"));
}

static void set_font_shadow_callback(moteur, cellstyle,
				     region, set_shadow)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		set_shadow;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
  
 cs_base = moteur->BaseStd->cell_style_base;

 tstyle = cellstyle->text_style;
  
 if (set_shadow)
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SHADOW_ON,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_SHADOW,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SHADOW_OFF,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_SHADOW,
			     XcTS_END);
  
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_font_outline(This)
c_Feuille *This;
{
 CellStyle_t	*cstyle;
 c_TextStyle	*tstyle;
 region_t	*region;
 int		set_outline;
  
 cstyle = 
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
  
 tstyle = cstyle->text_style;
  
 if ((tstyle->attributes & XcTS_B_OUTLINE) != 0)
  set_outline = FALSE;
 else
  set_outline = TRUE;

 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style
   (This->moteur, region, set_font_outline_callback, set_outline);
  region = region->NextRegion;
 }
  
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set in region in outline font"));
}

static void set_font_outline_callback(moteur, cellstyle,
				      region, set_outline)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		set_outline;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
 
 cs_base = moteur->BaseStd->cell_style_base;

 tstyle = cellstyle->text_style;

 if (set_outline)
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_OUTLINE_ON,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_OUTLINE,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_OUTLINE_OFF,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_OUTLINE,
			     XcTS_END);
 
 Xc_TRACE(("new style name: %s", new_tstyle->name));
  
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_font_small_caps(This)
c_Feuille *This;
{
 CellStyle_t	*cstyle;
 c_TextStyle	*tstyle;
 region_t	*region;
 int		set_small_caps;
  
 cstyle = 
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
 tstyle = cstyle->text_style;
  
 if ((tstyle->attributes & XcTS_B_SMALL_CAPS/* XcTS_B_STRIKEOUT */) != 0)
  set_small_caps = FALSE;
 else
  set_small_caps = TRUE;

 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style
   (This->moteur, region, set_font_small_caps_callback, set_small_caps);
  region = region->NextRegion;
 }
  
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set in region in small caps font"));
}

static void set_font_small_caps_callback(moteur, cellstyle,
					 region, set_small_caps)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		set_small_caps;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
  
 cs_base = moteur->BaseStd->cell_style_base;
 tstyle = cellstyle->text_style;

 if (set_small_caps)
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SMALL_CAPS_ON,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_SMALL_CAPS,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SMALL_CAPS_OFF,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_SMALL_CAPS,
			     XcTS_END);
  
 Xc_TRACE(("new style name: %s", new_tstyle->name));
 
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_font_big_caps(This)
c_Feuille *This;
{
 CellStyle_t	*cstyle;
 c_TextStyle	*tstyle;
 region_t	*region;
 int		set_big_caps;
  
 cstyle = 
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
 tstyle = cstyle->text_style;
  
 if ((tstyle->attributes & XcTS_B_BIG_CAPS) != 0)
  set_big_caps = FALSE;
 else
  set_big_caps = TRUE;

 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style
   (This->moteur, region, set_font_big_caps_callback, set_big_caps);
  region = region->NextRegion;
 }
  
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set in region in big caps font"));
}

static void set_font_big_caps_callback(moteur, cellstyle,
				       region, set_big_caps)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		set_big_caps;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
  
 cs_base = moteur->BaseStd->cell_style_base;
 tstyle = cellstyle->text_style;

 if (set_big_caps)
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_BIG_CAPS_ON,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_BIG_CAPS,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_BIG_CAPS_OFF,
			     XcTS_INHERIT, tstyle,
			     (unsigned int)XcTS_H_BIG_CAPS,
			     XcTS_END);
  
 Xc_TRACE(("new style name: %s", new_tstyle->name));
  
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_font_inc_size(This)
c_Feuille *This;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_font_inc_dec_size_callback,
				TRUE);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
}

void set_font_dec_size(This)
c_Feuille *This;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 { 
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_font_inc_dec_size_callback,
				FALSE);
  region = region->NextRegion;
 }  
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
}

static void set_font_inc_dec_size_callback( moteur, cellstyle, region, incr)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		incr;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
 coord_t	size;
 int		i;
  
 cs_base = moteur->BaseStd->cell_style_base;
  
 tstyle = cellstyle->text_style;
  
 size = tstyle->point_size;
 if (incr)
 {
  i = 0;
  while(i < NB_CONST_FONT_SIZE)
  {
   if (size < dft_font_size[i])
    break;
   i++;
  }
  if (i == NB_CONST_FONT_SIZE)
  {
   new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
   new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
   F(moteur).set_cell_style2(moteur, new_style, region);
   return;
  }
  size = dft_font_size[i];
 }
 else
 {
  i = NB_CONST_FONT_SIZE -1;
  while(i >= 0)
  {
   if (size > dft_font_size[i])
    break;
   i--;
  }
  if (i < 0)
  {
   new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
   new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
   F(moteur).set_cell_style2(moteur, new_style, region);
   return;
  }
  size = dft_font_size[i];
 }
  
 new_tstyle =  F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SIZE, size,
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_SIZE,
			     XcTS_END);
  
 Xc_TRACE(("new style name: %s", new_tstyle->name));
 
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);
  
 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_format_percent(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
 c_FormatNbr	*format;

 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
  
 if (cellstyle->format->special == XcFt_PERCENT)
 {
  format = F(This->BaseStd->format_base).get(NULL, XcFt_STANDARD, 
					     This->BaseStd);
  if (format)
   set_cell_formatnbr(This, format);
 }
 else
 {
  format = F(This->BaseStd->format_base).get(NULL, XcFt_PERCENT, 
					     This->BaseStd);
  if (format)
   set_cell_formatnbr(This, format);
 }
}

void set_format_currency(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
 c_FormatNbr	*format;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
 if (cellstyle->format->special == XcFt_CURRENCY)
 {
  format = F(This->BaseStd->format_base).get(NULL, XcFt_STANDARD, 
					     This->BaseStd);
  if (format)
   set_cell_formatnbr(This, format);
 }
 else
 {
  format = F(This->BaseStd->format_base).get(NULL, XcFt_CURRENCY, 
					     This->BaseStd);
  if (format)
   set_cell_formatnbr(This, format);
 }
}

void set_format_thousands(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
 c_FormatNbr	*format;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
 if (cellstyle->format->special == XcFt_THOUSANDS)
 {
  format = F(This->BaseStd->format_base).get(NULL, XcFt_STANDARD, 
					     This->BaseStd);
  if (format)
   set_cell_formatnbr(This, format);
 }
 else
 {
  format = F(This->BaseStd->format_base).get(NULL, XcFt_THOUSANDS, 
					     This->BaseStd);
  if (format)
   set_cell_formatnbr(This, format);
 }
}

void set_format_scientific(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
 c_FormatNbr	*format;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
 if (cellstyle->format->special == XcFt_SCIENTIFIC)
 {
  format = F(This->BaseStd->format_base).get(NULL, XcFt_STANDARD, 
					     This->BaseStd);
  if (format)
   set_cell_formatnbr(This, format);
 }
 else
 {
  format = F(This->BaseStd->format_base).get(NULL, XcFt_SCIENTIFIC, 
					     This->BaseStd);
  if (format)
   set_cell_formatnbr(This, format);
 }
}

void set_format_engineer(This)
c_Feuille *This;
{
 CellStyle_t	*cellstyle;
 c_FormatNbr	*format;
  
 cellstyle =
  F(This->moteur).get_cell_style(This->moteur, This->cell_x, This->cell_y);
 if (cellstyle->format->special == XcFt_ENGINEER)
 {
  format = F(This->BaseStd->format_base).get(NULL, XcFt_STANDARD, 
					     This->BaseStd);
  if (format)
   set_cell_formatnbr(This, format);
 }
 else
 {
  format = F(This->BaseStd->format_base).get(NULL, XcFt_ENGINEER, 
					     This->BaseStd);
  if (format)
   set_cell_formatnbr(This, format);
 }
}

void set_format_add_digit(This)
c_Feuille *This;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 { 
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_format_add_sub_digit_callback,
				TRUE);
  region = region->NextRegion;
 }  
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
}

void set_format_sub_digit(This)
c_Feuille *This;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 { 
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_format_add_sub_digit_callback,
				FALSE);
  region = region->NextRegion;
 }  
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
}

static void set_format_add_sub_digit_callback(moteur, cellstyle, region, incr)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
boolean		incr;
{
 CellStyle_t	*new_style; 
 c_FormatNbr	*format, *nformat;
 c_CellStyle	*cs_base;
 char	name[XcMAX_NAME_LENGTH];
 int	min_prec;
  
 cs_base = moteur->BaseStd->cell_style_base;
 format = cellstyle->format;
 min_prec = format->min_prec;
  
 if (incr)
 {
  if (++min_prec > MAX_MAX_PREC)
  {
   new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
   new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
   F(moteur).set_cell_style2(moteur, new_style, region);
   return;
  }
 }
 else
 {
  if (--min_prec < 0)
  {
   new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
   new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
   F(moteur).set_cell_style2(moteur, new_style, region);
   return;
  }
 }
 sprintf(name, "generated_%d", min_prec);
  
 Xc_TRACE(("new format name: %s", name));
 nformat =  
  F(format).get(format, XcFt_NORMAL, moteur->BaseStd, 
		XcF_NAME, name, XcF_HIDE_ON, XcF_MINPREC, min_prec,
		XcF_MAXPREC, min_prec, XcF_END);
  
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_FORMAT, nformat, XqCS_END);
  
 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_cell_backcolor(This, color)
c_Feuille *This;
c_Color *color;
{
 region_t	*region;
 
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_cell_backcolor_callback, color);
  region = region->NextRegion;
 }
 
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set cell background color"));
}

static void set_cell_backcolor_callback(moteur, cellstyle, region, color)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
c_Color		*color;
{
 CellStyle_t	*new_style;
 c_CellStyle	*cs_base;
 
 cs_base = moteur->BaseStd->cell_style_base;
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 
 if (cellstyle->inside_stipple_pattern == PATTERN_NONE)
  SET_CELL_STYLE(new_style, XqCS_FOREGROUND, color,
		 XqCS_PATTERN, CELL_PATTERN_DATA_FIRST,
		 XqCS_END);
 else
  SET_CELL_STYLE(new_style, XqCS_FOREGROUND, color, XqCS_END);
 
 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_cell_background(This)
c_Feuille *This;
{
 set_cellstyle_with_callback(This, BACK_PATTERN);
 Xc_TRACE(("set cell background with default pattern"));
}

void set_cell_inverse(This)
c_Feuille *This;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_cell_inverse_callback, NULL);
  region = region->NextRegion;
 }

 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set cell inverse color"));
}

static void set_cell_inverse_callback(moteur, cellstyle, region, none)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
void		*none;
{
 CellStyle_t	*new_style;
 c_CellStyle	*cs_base;
 c_TextStyle	*tstyle, *new_tstyle;
 c_Color	*color_back;
 c_Color	*color_font;
 c_Color	*white;
  
 cs_base = moteur->BaseStd->cell_style_base;
 tstyle = cellstyle->text_style;
  
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
  
 color_back = cellstyle->inside_foreground;
 color_font = cellstyle->text_style->color;
  
 new_tstyle = F(tstyle).get(tstyle, moteur->BaseStd,  XcTS_NAME, NULL,
			    XcTS_COLOR, color_back, XcTS_END); 
  
 if (cellstyle->inside_stipple_pattern == PATTERN_NONE)
  SET_CELL_STYLE(new_style, XqCS_FOREGROUND, color_font, XqCS_TEXTSTYLE, 
		 new_tstyle, XqCS_PATTERN, CELL_PATTERN_DATA_FIRST,
		 XqCS_END);
 else
 {
  white = F(moteur->BaseStd->color_base).getColor
   (&(moteur->BaseStd->color_base), XcC_NAME, "White",
    XcC_GRAY, XcC_SCALE_COLOR(1.0), XcC_TRANSPARENCY, 0, XcC_END);
  
  if (cellstyle->inside_stipple_pattern == CELL_PATTERN_DATA_FIRST && 
      color_font == white)
   SET_CELL_STYLE(new_style, XqCS_FOREGROUND, color_font, XqCS_TEXTSTYLE, 
		  new_tstyle, XqCS_PATTERN, PATTERN_NONE, XqCS_END);
  else
   SET_CELL_STYLE(new_style, XqCS_FOREGROUND, color_font, XqCS_TEXTSTYLE, 
		  new_tstyle, XqCS_END);
 }
 
 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_cell_inside(This, inside)
c_Feuille *This;
cell_inside_t *inside;
{
 region_t	*region;
  
 if (inside->pattern == PATTERN_NONE)
 {
  unset_cell_inside(This);
  return;
 }
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_cell_inside_callback, inside);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set cell inside"));
}

static void set_cell_inside_callback(moteur, cellstyle, region, inside)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
cell_inside_t	*inside;
{
 CellStyle_t	*new_style;
 c_CellStyle	*cs_base;

 cs_base = moteur->BaseStd->cell_style_base;

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
  
 SET_CELL_STYLE(new_style, 
		XqCS_FOREGROUND, inside->foreground,
		XqCS_BACKGROUND, inside->background,
		XqCS_PATTERN, inside->pattern, XqCS_END);
  
 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_cell_borders(This, borders)
c_Feuille *This;
cell_border_t *borders;
{
 region_t	*stack;
 region_t	*region;
 region_t	regional;
  
  
 if ( !(borders->left || borders->right || borders->up 
	|| borders->down || borders->outline))
  return;

 stack = NULL;
 region = This->cell_region;
 if (borders->outline)
 {
  while(region)
  {
   if (region->x_min == region->x_max)
   {
    if (region->y_min == region->y_max)
    {
     borders->set_border = SET_ALL_B;
     F(This->moteur).or_cell_style(This->moteur, region, 
				   set_cell_borders_callback, 
				   borders);
     push_region_stack(&stack, region);
    }
    else
    {
     if (region->y_min == 0 && region->y_max == ROW_MAX)
     {
      borders->set_border = SET_LRBD;
      F(This->moteur).or_cell_style(This->moteur, region, 
				    set_cell_borders_callback, 
				    borders);
      push_region_stack(&stack, region);
     }
     else
     {
      regional.x_min = regional.x_max = region->x_min;
      regional.y_min = regional.y_max = region->y_min;
      borders->set_border = SET_LURBD;
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cell_borders_callback, 
				    borders);
      push_region_stack(&stack, &regional);
      regional.y_min++;
      regional.y_max = region->y_max - 1;
      if (regional.y_max >= regional.y_min)
      {
       borders->set_border = SET_LRBD;
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cell_borders_callback, 
				     borders);
       push_region_stack(&stack, &regional);
      }
      regional.y_min = ++regional.y_max;
      borders->set_border = SET_RDLBD;
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cell_borders_callback, 
				    borders);
      push_region_stack(&stack, &regional);
     }
    }
   }
   else
   {
    if (region->y_min == region->y_max)
    {
     if (region->x_min == 0 && region->x_max == COLUMN_MAX)
     {
      borders->set_border = SET_UDBD;
      F(This->moteur).or_cell_style(This->moteur, region, 
				    set_cell_borders_callback, 
				    borders);
      push_region_stack(&stack, region);
     }
     else
     {
      regional.x_min = regional.x_max = region->x_min;
      regional.y_min = regional.y_max = region->y_min;
      borders->set_border = SET_DLUBD;
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cell_borders_callback, 
				    borders);
      push_region_stack(&stack, &regional);
      regional.x_min++;
      regional.x_max = region->x_max - 1;
      if (regional.x_max >= regional.x_min)
      {
       borders->set_border = SET_UDBD;
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cell_borders_callback, 
				     borders);
       push_region_stack(&stack, &regional);
      }
      regional.x_min = ++regional.x_max;
      borders->set_border = SET_URDBD;
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cell_borders_callback, 
				    borders);
      push_region_stack(&stack, &regional);
     }
    }
    else
    {
     if (region->x_min == 0 && region->x_max == COLUMN_MAX)
     {
      regional.x_min = region->x_min;
      regional.x_max = region->x_max;
      if (region->y_min != 0)
      {
       regional.y_min = regional.y_max = region->y_min;
       borders->set_border = SET_UBD;
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cell_borders_callback, 
				     borders);
       push_region_stack(&stack, &regional);
      }
      if (region->y_max != ROW_MAX)
      {
       regional.y_min = regional.y_max = region->y_max;
       borders->set_border = SET_DBD;
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cell_borders_callback, 
				     borders);
       push_region_stack(&stack, &regional);
      }
     }
     else
     {
      if (region->y_min == 0 && region->y_max == ROW_MAX)
      {
       regional.y_min = region->y_min;
       regional.y_max = region->y_max;
       if (region->x_min != 0)
       {
	regional.x_min = regional.x_max = region->x_min;
	borders->set_border = SET_LBD;
	F(This->moteur).or_cell_style(This->moteur, &regional, 
				      set_cell_borders_callback, 
				      borders);
	push_region_stack(&stack, &regional);
       }
       if (region->x_max != COLUMN_MAX)
       {
	regional.x_min = regional.x_max = region->x_max;
	borders->set_border = SET_RBD;
	F(This->moteur).or_cell_style(This->moteur, &regional, 
				      set_cell_borders_callback, 
				      borders);
	push_region_stack(&stack, &regional);
       }
      }	
      else
      {
       regional.x_min = regional.x_max = region->x_min;
       regional.y_min = regional.y_max = region->y_min;
       borders->set_border = SET_LUBD;
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cell_borders_callback, 
				     borders);
       push_region_stack(&stack, &regional);
       regional.x_min++;
       regional.x_max = region->x_max - 1;
       if (regional.x_max >= regional.x_min)
       {
	borders->set_border = SET_UBD;
	F(This->moteur).or_cell_style(This->moteur, &regional, 
				      set_cell_borders_callback, 
				      borders);
	push_region_stack(&stack, &regional);
       }  
       regional.x_min = ++regional.x_max;
       borders->set_border = SET_RUBD;
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cell_borders_callback, 
				     borders);
       push_region_stack(&stack, &regional);
       regional.y_min++;
       regional.y_max = region->y_max - 1;
       if (regional.y_max >= regional.y_min)
       {
	borders->set_border = SET_RBD;
	F(This->moteur).or_cell_style(This->moteur, &regional, 
				      set_cell_borders_callback, 
				      borders);
	push_region_stack(&stack, &regional);
       }
       regional.y_min = ++regional.y_max;
       borders->set_border = SET_RDBD;
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cell_borders_callback, 
				     borders);
       push_region_stack(&stack, &regional);
       regional.x_max --;
       regional.x_min = region->x_min + 1;
       if (regional.x_max >= regional.x_min)
       {
	borders->set_border = SET_DBD;
	F(This->moteur).or_cell_style(This->moteur, &regional, 
				      set_cell_borders_callback, 
				      borders);
	push_region_stack(&stack, &regional);
       }
       regional.x_max = --regional.x_min;
       borders->set_border = SET_LDBD;
       F(This->moteur).or_cell_style(This->moteur, &regional, 
				     set_cell_borders_callback, 
				     borders);
       push_region_stack(&stack, &regional);
       regional.y_max --;
       regional.y_min = region->y_min + 1;
       if (regional.y_max >= regional.y_min)
       {
	borders->set_border = SET_LBD;
	F(This->moteur).or_cell_style(This->moteur, &regional, 
				      set_cell_borders_callback, 
				      borders);
	push_region_stack(&stack, &regional);
       }
      }
     }
     regional.x_min = region->x_min + 1;
     regional.x_max = region->x_max - 1;
     regional.y_min = region->y_min + 1;
     regional.y_max = region->y_max - 1;
     if (regional.x_min <= regional.x_max &&
	 regional.y_min <= regional.y_max)
     {
      borders->set_border = SET_NONE;
      F(This->moteur).or_cell_style(This->moteur, &regional, 
				    set_cell_borders_callback, 
				    borders);
      push_region_stack(&stack, &regional);
     }
    }
   }
   region = region->NextRegion;
  }
  if (stack)
  {
   F(This).RefreshRegion(This, stack, Xq_CELLS);
   flush_region_stack(stack);
  }
 }
 else
 {
  borders->set_border = SET_NONE;
  while(region)
  {
   F(This->moteur).or_cell_style(This->moteur, region, 
				 set_cell_borders_callback, borders);
   region = region->NextRegion; 
  } 
  F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 }
 Xc_TRACE(("set cell borders"));
}

#define SET_LEFT_BORDER \
if (borders->left) \
{ \
   SET_CELL_STYLE(new_style, \
		  XqCS_LEFT_FOREGROUND, \
		  borders->cellb_left.border_foreground, \
		  XqCS_LEFT_BACKGROUND, \
		  borders->cellb_left.border_background, \
		  XqCS_LEFT_PATTERN, \
		  borders->cellb_left.border_stipple_pattern, \
		  XqCS_LEFT_DASH, \
		  borders->cellb_left.border_dash_pattern, \
		  XqCS_END); \
}

#define SET_RIGHT_BORDER \
if (borders->right) \
{ \
   SET_CELL_STYLE(new_style, \
		  XqCS_RIGHT_FOREGROUND, \
		  borders->cellb_right.border_foreground, \
		  XqCS_RIGHT_BACKGROUND, \
		  borders->cellb_right.border_background, \
		  XqCS_RIGHT_PATTERN, \
		  borders->cellb_right.border_stipple_pattern, \
		  XqCS_RIGHT_DASH, \
		  borders->cellb_right.border_dash_pattern, \
		  XqCS_END); \
}

#define SET_UP_BORDER \
if (borders->up) \
{ \
   SET_CELL_STYLE(new_style, \
		  XqCS_TOP_FOREGROUND, \
		  borders->cellb_up.border_foreground, \
		  XqCS_TOP_BACKGROUND, \
		  borders->cellb_up.border_background, \
		  XqCS_TOP_PATTERN, \
		  borders->cellb_up.border_stipple_pattern, \
		  XqCS_TOP_DASH, \
		  borders->cellb_up.border_dash_pattern, \
		  XqCS_END); \
}

#define SET_DOWN_BORDER \
if (borders->down) \
{ \
   SET_CELL_STYLE(new_style, \
		  XqCS_BOTTOM_FOREGROUND, \
		  borders->cellb_down.border_foreground, \
		  XqCS_BOTTOM_BACKGROUND, \
		  borders->cellb_down.border_background, \
		  XqCS_BOTTOM_PATTERN, \
		  borders->cellb_down.border_stipple_pattern, \
		  XqCS_BOTTOM_DASH, \
		  borders->cellb_down.border_dash_pattern, \
		  XqCS_END); \
}

#define SET_LEFT_BORDER_OUTLINE \
SET_CELL_STYLE(new_style, \
	       XqCS_LEFT_FOREGROUND, \
	       borders->cellb_outline.border_foreground, \
	       XqCS_LEFT_BACKGROUND, \
	       borders->cellb_outline.border_background, \
	       XqCS_LEFT_PATTERN, \
	       borders->cellb_outline.border_stipple_pattern, \
	       XqCS_LEFT_DASH, \
	       borders->cellb_outline.border_dash_pattern, \
	       XqCS_END)

#define SET_RIGHT_BORDER_OUTLINE \
SET_CELL_STYLE(new_style, \
	       XqCS_RIGHT_FOREGROUND, \
	       borders->cellb_outline.border_foreground, \
	       XqCS_RIGHT_BACKGROUND, \
	       borders->cellb_outline.border_background, \
	       XqCS_RIGHT_PATTERN, \
	       borders->cellb_outline.border_stipple_pattern, \
	       XqCS_RIGHT_DASH, \
	       borders->cellb_outline.border_dash_pattern, \
	       XqCS_END) 

#define SET_UP_BORDER_OUTLINE \
SET_CELL_STYLE(new_style, \
	       XqCS_TOP_FOREGROUND, \
	       borders->cellb_outline.border_foreground, \
	       XqCS_TOP_BACKGROUND, \
	       borders->cellb_outline.border_background, \
	       XqCS_TOP_PATTERN, \
	       borders->cellb_outline.border_stipple_pattern, \
	       XqCS_TOP_DASH, \
	       borders->cellb_outline.border_dash_pattern, \
	       XqCS_END)

#define SET_DOWN_BORDER_OUTLINE \
SET_CELL_STYLE(new_style, \
	       XqCS_BOTTOM_FOREGROUND, \
	       borders->cellb_outline.border_foreground, \
	       XqCS_BOTTOM_BACKGROUND, \
	       borders->cellb_outline.border_background, \
	       XqCS_BOTTOM_PATTERN, \
	       borders->cellb_outline.border_stipple_pattern, \
	       XqCS_BOTTOM_DASH, \
	       borders->cellb_outline.border_dash_pattern, \
	       XqCS_END)

static void set_cell_borders_callback(moteur, cellstyle, region, borders)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
cell_border_t	*borders;
{
 CellStyle_t	*new_style;
 c_CellStyle	*cs_base;

 cs_base = moteur->BaseStd->cell_style_base;
 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
  
 Xc_TRACE(("set border: %d", borders->set_border));
 switch(borders->set_border)
 {
 case SET_NONE:
  SET_LEFT_BORDER;
  SET_RIGHT_BORDER;
  SET_UP_BORDER;
  SET_DOWN_BORDER;
  break;
 case SET_LBD:
  SET_LEFT_BORDER_OUTLINE;
  SET_RIGHT_BORDER;
  SET_UP_BORDER;
  SET_DOWN_BORDER;
  break;
 case SET_RBD:
  SET_LEFT_BORDER;
  SET_RIGHT_BORDER_OUTLINE;
  SET_UP_BORDER;
  SET_DOWN_BORDER;
  break;
 case SET_UBD:
  SET_LEFT_BORDER;
  SET_RIGHT_BORDER;
  SET_UP_BORDER_OUTLINE;
  SET_DOWN_BORDER;
  break;
 case SET_DBD:
  SET_LEFT_BORDER;
  SET_RIGHT_BORDER;
  SET_UP_BORDER;
  SET_DOWN_BORDER_OUTLINE;
  break;
 case SET_LUBD:
  SET_LEFT_BORDER_OUTLINE;
  SET_RIGHT_BORDER;
  SET_UP_BORDER_OUTLINE;
  SET_DOWN_BORDER;
  break;
 case SET_LDBD:
  SET_LEFT_BORDER_OUTLINE;
  SET_RIGHT_BORDER;
  SET_UP_BORDER;
  SET_DOWN_BORDER_OUTLINE;
  break;
 case SET_RUBD:
  SET_LEFT_BORDER;
  SET_RIGHT_BORDER_OUTLINE;
  SET_UP_BORDER_OUTLINE;
  SET_DOWN_BORDER;
  break;
 case SET_RDBD:
  SET_LEFT_BORDER;
  SET_RIGHT_BORDER_OUTLINE;
  SET_UP_BORDER;
  SET_DOWN_BORDER_OUTLINE;
  break;
 case SET_LRBD:
  SET_LEFT_BORDER_OUTLINE;
  SET_RIGHT_BORDER_OUTLINE;
  SET_UP_BORDER;
  SET_DOWN_BORDER;
  break;
 case SET_UDBD:
  SET_LEFT_BORDER;
  SET_RIGHT_BORDER;
  SET_UP_BORDER_OUTLINE;
  SET_DOWN_BORDER_OUTLINE;
  break;
 case SET_LURBD:
  SET_LEFT_BORDER_OUTLINE;
  SET_RIGHT_BORDER_OUTLINE;
  SET_UP_BORDER_OUTLINE;
  SET_DOWN_BORDER;
  break;
 case SET_URDBD:
  SET_LEFT_BORDER;
  SET_RIGHT_BORDER_OUTLINE;
  SET_UP_BORDER_OUTLINE;
  SET_DOWN_BORDER_OUTLINE;
  break;
 case SET_RDLBD:
  SET_LEFT_BORDER_OUTLINE;
  SET_RIGHT_BORDER_OUTLINE;
  SET_UP_BORDER;
  SET_DOWN_BORDER_OUTLINE;
  break;
 case SET_DLUBD:
  SET_LEFT_BORDER_OUTLINE;
  SET_RIGHT_BORDER;
  SET_UP_BORDER_OUTLINE;
  SET_DOWN_BORDER_OUTLINE;
  break;
 case SET_ALL_B:
  SET_LEFT_BORDER_OUTLINE;
  SET_RIGHT_BORDER_OUTLINE;
  SET_UP_BORDER_OUTLINE;
  SET_DOWN_BORDER_OUTLINE;
  break;
 }
  
 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void unset_cell_inside(This)
c_Feuille *This;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				unset_cell_inside_callback, NULL);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("unset cell inside"));
}

static void unset_cell_inside_callback(moteur, cellstyle, region, none)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
void		*none;
{
 CellStyle_t	*new_style;
 c_CellStyle	*cs_base;

 cs_base = moteur->BaseStd->cell_style_base;

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
  
 SET_CELL_STYLE(new_style, 
		XqCS_FOREGROUND, cs_base->default_cstyle->inside_foreground,
		XqCS_BACKGROUND, cs_base->default_cstyle->inside_background,
		XqCS_PATTERN, cs_base->default_cstyle->inside_stipple_pattern,
		XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}


void unset_cellstyle_all(This)
c_Feuille *This;
{
 region_t	*region;
 c_CellStyle	*cs_base;
 CellStyle_t	*new_style;
  
 cs_base = This->BaseStd->cell_style_base;
 region = This->cell_region;
 new_style = F(cs_base).AddCellStyleToBase(cs_base, cs_base->default_cstyle);
 while(region)
 {
  F(This->moteur).set_cell_style2(This->moteur, new_style, region);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("unset cell inside"));
}

void unset_cell_formatnbr(This)
c_Feuille *This;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				unset_cell_formatnbr_callback, NULL);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("unset cell format number"));
}

static void unset_cell_formatnbr_callback(moteur, cellstyle, region, none)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
void		*none;
{
 CellStyle_t	*new_style;
 c_CellStyle	*cs_base;

 cs_base = moteur->BaseStd->cell_style_base;

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
  
 SET_CELL_STYLE(new_style, 
		XqCS_FORMAT, cs_base->default_cstyle->format,
		XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void unset_cell_textstyle(This)
c_Feuille *This;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				unset_cell_textstyle_callback, NULL);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("unset cell format number"));
}

static void unset_cell_textstyle_callback(moteur, cellstyle, region, none)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
void		*none;
{
 CellStyle_t	*new_style; 
 c_CellStyle	*cs_base;

 cs_base = moteur->BaseStd->cell_style_base;

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
  
 SET_CELL_STYLE(new_style, 
		XqCS_TEXTSTYLE, cs_base->default_cstyle->text_style,
		XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void unset_cell_formulas(This)
c_Feuille *This;
{
 region_t	*region;

 region = This->cell_region;
 while(region)
 {
  F(This->moteur).delete_zone(This->moteur, region);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 F(GlobEditBar).refresh_cell(GlobEditBar, This);
 Xc_TRACE(("unset cell formulas"));
}

void unset_cell_all(This)
c_Feuille *This;
{
 region_t	*region;
 c_CellStyle	*cs_base;
 CellStyle_t	*new_style;
  
 cs_base = This->BaseStd->cell_style_base;
 region = This->cell_region;
 new_style = F(cs_base).AddCellStyleToBase(cs_base, cs_base->default_cstyle);
 while(region)
 {
  F(This->moteur).set_cell_style2(This->moteur, new_style, region);
  F(This->moteur).delete_zone(This->moteur, region);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 F(GlobEditBar).refresh_cell(GlobEditBar, This);
 Xc_TRACE(("unset cell all"));
}

void set_cell_formatnbr(This, formatnbr)
c_Feuille *This;
c_FormatNbr	*formatnbr;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_cell_formatnbr_callback, formatnbr);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set cell format number"));
}

static void set_cell_formatnbr_callback(moteur, cellstyle, region, formatnbr)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
c_FormatNbr	*formatnbr;
{
 CellStyle_t	*new_style; 
 c_CellStyle	*cs_base;

 cs_base = moteur->BaseStd->cell_style_base;

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
  
 SET_CELL_STYLE(new_style, XqCS_FORMAT, formatnbr, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_cell_textstyle(This, textstyle)
c_Feuille *This;
c_TextStyle	*textstyle;
{
 region_t	*region;
 l_Cadre	*lcadre;
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_cell_textstyle_callback, textstyle);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 lcadre = This->frmg->ListCadre;
 while(lcadre)
 {
  if (lcadre->cadre->object_type == CADRE_VECTOR &&
      ((c_VectorGraph *)(lcadre->cadre->object))->highg)
  {
   c_HighGraph	*highg;
   highg = ((c_VectorGraph *)(lcadre->cadre->object))->highg;
   F(highg).test_tstyle_apply(highg, This->cell_region);
  }
  lcadre = lcadre->NextCadre;
 }
 Xc_TRACE(("set cell textstyle"));
}

static void set_cell_textstyle_callback(moteur, cellstyle, region, textstyle)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
c_TextStyle	*textstyle;
{
 CellStyle_t	*new_style; 
 c_CellStyle	*cs_base;

 cs_base = moteur->BaseStd->cell_style_base;

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
  
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, textstyle, XqCS_END);

 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}


void set_cell_fontfamily(This, family)
c_Feuille	*This;
family_tree	*family;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_cell_fontfamily_callback, family);
  region = region->NextRegion;
 }
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
  
 Xc_TRACE(("set cell font"));
}

static void set_cell_fontfamily_callback(moteur, cellstyle, region, family)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
family_tree	*family;
{  
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 font_list	*fontl;
 c_CellStyle	*cs_base;
 boolean	bold;
 boolean	italic; 
 
 cs_base = moteur->BaseStd->cell_style_base;
  
 tstyle = cellstyle->text_style;
 fontl = tstyle->font->specific;
  
 bold = italic = FALSE;
 if (fontl->is_bold)
  bold = TRUE;
 if (fontl->is_italic)
  italic = TRUE;
 if (fontl->is_bold_italic)
  bold = italic = TRUE;
  
 fontl = family->list;
 if (family->can_bold_italic && bold && italic)
 {
  while(fontl && !fontl->is_bold_italic)
   fontl = fontl->Next;
 }
 else
 {
  if (family->can_bold && bold)
  {
   while(fontl && !fontl->is_bold)
    fontl = fontl->Next;
  }
  else
  {
   if (family->can_italic && italic)
   {
    while(fontl && !fontl->is_italic)
     fontl = fontl->Next;
   }
   else
   {
    while(fontl && !fontl->is_normal)
     fontl = fontl->Next;
   }
  }
 }
 if (fontl == NULL)
 {
  new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
  new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
  F(moteur).set_cell_style2(moteur, new_style, region);
  return;
 }
  
 new_tstyle =  F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_FONT, fontl->font, 
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_FONT,
			     XcTS_END);
  
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);
  
 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_cell_fontcolor(This, color)
c_Feuille *This;
c_Color *color;
{
 region_t	*region;
  
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_cell_fontcolor_callback, color);
  region = region->NextRegion;
 }
  
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set cell font color"));
}

static void set_cell_fontcolor_callback(moteur, cellstyle, region, color)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
c_Color		*color;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
  
 cs_base = moteur->BaseStd->cell_style_base;
  
 tstyle = cellstyle->text_style;
 
 new_tstyle =  F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_COLOR, color, 
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_COLOR,
			     XcTS_END);
  
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);
  
 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

void set_cell_fontsize(This, size)
c_Feuille *This;
coord_t size;
{
 region_t	*region;
 
 region = This->cell_region;
 while(region)
 {
  F(This->moteur).or_cell_style(This->moteur, region, 
				set_cell_fontsize_callback, size);
  region = region->NextRegion;
 }
 
 F(This).RefreshRegion(This, This->cell_region, Xq_CELLS);
 Xc_TRACE(("set cell font size"));
}

static void set_cell_fontsize_callback(moteur, cellstyle, region, size)
c_Moteur	*moteur;
CellStyle_t	*cellstyle;
region_t	*region;
coord_t		size;
{
 CellStyle_t	*new_style; 
 c_TextStyle	*tstyle, *new_tstyle;
 c_CellStyle	*cs_base;
 
 cs_base = moteur->BaseStd->cell_style_base;
 
 tstyle = cellstyle->text_style;
 
 new_tstyle =  F(tstyle).get(tstyle, moteur->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SIZE, size, 
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_SIZE,
			     XcTS_END);
 
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 new_style = F(cs_base).CreateNewCellStyle(cs_base, cellstyle);
 SET_CELL_STYLE(new_style, XqCS_TEXTSTYLE, new_tstyle, XqCS_END);
 
 new_style = F(cs_base).AddCellStyleToBase(cs_base, new_style);
 F(moteur).set_cell_style2(moteur, new_style, region);
}

