/*
** HG_Axe.c for XQuad in High_Graph/
** BasicGraph Axe object
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
** Last update Sun Feb  8 21:15:13 1998 One of the authors
*/

#include "HG_Axe.h"
#include <stdarg.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void regenerate ___PROTO((c_HG_Axe *This, bbox_t bbox, void *param));
static void change_xinfo ___PROTO((c_HG_Axe *This));
static error change_base ___PROTO((c_HG_Axe *This));
static void display_redraw ___PROTO((c_HG_Axe *This));
static void add_to_vectorgraph ___PROTO((c_HG_Axe *This));
static void sub_to_vectorgraph ___PROTO((c_HG_Axe *This));
static void setHG ___PROTO((c_HG_Axe *This, ...));
static void vset();
static void calcul_axe ___PROTO((c_HG_Axe *This));
static void ps_print ___PROTO((c_HG_Axe *This, c_PostScript *post));
static boolean readHG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeHG ___PROTO((c_HG_Axe *This, c_DocFile *doc));
static coord_t convert_value ___PROTO((c_HG_Axe *This, real value));

sf_HG_Axe fc_HG_Axe =
{
 cons, dest, copy, regenerate, 
 change_xinfo, change_base, display_redraw,
 add_to_vectorgraph, sub_to_vectorgraph, setHG,
 ps_print, readHG, writeHG, convert_value,
};

static void *cons(highg, sub_class)
c_HighGraph	*highg;
int		sub_class;
{
 c_HG_Axe	*This;
 Xc_HISTORY(("constructor"));
  
 if ( (This = Xc_malloc("HG_Axe", sizeof(c_HG_Axe)) ) == NULL)
  return NULL;
 This->f = &fc_HG_Axe;
  
 This->highg = highg;
 This->class = HG_AXE;
 This->sub_class = sub_class;
  
 This->nb_axe = 0;
 This->axe = NULL;
 This->nb_grad = 0;
 This->grad_line = NULL;
 This->grad_line_mark = NULL;
 This->grad_polyline_mark = NULL;
 This->grad_string = NULL;
 This->grad_number = NULL;
  
 Xc_TRACE(("constructor done"));
 return This;
}

static void dest(This)
c_HG_Axe	*This;
{
 int	i;
 Xc_HISTORY(("destructor")); 
  
 if (This->nb_axe)
 {
  for(i=0; i<This->nb_axe; i++)
  {
   BG_DELETE(This->axe[i])(This->axe[i]);
  }
  Xc_free(This->axe);
 }
 if (This->nb_grad)
 {
  for(i=0; i<This->nb_grad; i++)
  {
   BG_DELETE(This->grad_line[i])(This->grad_line[i]);
   if (This->grad_line_mark)
    BG_DELETE(This->grad_line_mark[i])(This->grad_line_mark[i]);
   if (This->grad_polyline_mark)
    BG_DELETE(This->grad_polyline_mark[i])(This->grad_polyline_mark[i]);
   BG_DELETE(This->grad_string[i])(This->grad_string[i]);
   Xc_free(This->grad_number[i]);
  }
  Xc_free(This->grad_line);
  if (This->grad_line_mark)
  {
   Xc_free(This->grad_line_mark);
   This->grad_line_mark = NULL;
  }
  if (This->grad_polyline_mark)
  {
   Xc_free(This->grad_polyline_mark);
   This->grad_polyline_mark = NULL;
  }
  Xc_free(This->grad_string);
  Xc_free(This->grad_number);
 }
 Xc_free(This);
  
 Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_HG_Axe	*This;
BaseStd_t	*base;
{
 c_HG_Axe	*hg_copy;
  
 if ( (hg_copy = Xc_malloc("HG_Axe", sizeof(c_HG_Axe)) ) == NULL)
  return NULL;
 memcpy(hg_copy, This, sizeof(c_HG_Axe));
  
 return hg_copy;
}

static void regenerate(This, bbox, param)
c_HG_Axe	*This;
bbox_t		bbox;
void		*param;
{
 c_HighGraph	*highg;
 coord_t	width, height, step, pos, absci;
 vector_t	point1, point2, point3, t_orig, t_size;
 real		min, max, delta, grad, b, c, number;
 double	angle, angle_step, rayon;
 int		i, j, nb_grad;
 ruler_t	ruler;
 c_TextStyle	*style;
 c_Color	*color;
 vector_t	size;
 int		nb_abscisse = 0;
  
 switch(This->sub_class)
 {
 case XcHG_Axe_VERTICAL:
 case XcHG_Axe_HORIZONTAL:
  memcpy(&size, param, sizeof(vector_t)); 
  break;
 case XcHG_Axe_POLAIRE:
  nb_abscisse = *((int *)param);
  break;
 }
  
 highg = This->highg;
 width = bbox.urx - bbox.llx;
 height = bbox.lly - bbox.ury;
 if (highg->abscisse && highg->d_abscisse)
  absci = SCALE_FROM_MILLIMETERS(15);
 else
  absci = (coord_t)0;
  
 if (This->nb_axe)
 {
  for(i=0; i<This->nb_axe; i++)
  {
   BG_DELETE(This->axe[i])(This->axe[i]);
  }
  Xc_free(This->axe);
  This->axe = NULL;
  This->nb_axe = 0;
 }
  
 if (This->nb_grad)
 {
  for(i=0; i<This->nb_grad; i++)
  {
   BG_DELETE(This->grad_line[i])(This->grad_line[i]);
   if (This->grad_line_mark)
    BG_DELETE(This->grad_line_mark[i])(This->grad_line_mark[i]);
   if (This->grad_polyline_mark)
    BG_DELETE(This->grad_line_mark[i])(This->grad_line_mark[i]);
   BG_DELETE(This->grad_string[i])(This->grad_string[i]);
   Xc_free(This->grad_number[i]);
  }
  Xc_free(This->grad_line);
  if (This->grad_line_mark)
  {
   Xc_free(This->grad_line_mark);
   This->grad_line_mark = NULL;
  }
  if (This->grad_polyline_mark)
  {
   Xc_free(This->grad_polyline_mark);
   This->grad_polyline_mark = NULL;
  }
  Xc_free(This->grad_string);
  Xc_free(This->grad_number);
  This->nb_grad = 0;
  This->grad_line = NULL;
  This->grad_line_mark = NULL;
  This->grad_string = NULL;
  This->grad_number = NULL;
 }
  
 switch(This->sub_class)
 {
 case XcHG_Axe_VERTICAL:
  This->nb_axe = 1;
  This->axe = (c_BG_Line **)Xc_malloc("axe **", sizeof(c_BG_Line *));
  This->axe[0] = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
      
  point1.dx = point2.dx = bbox.urx;
  point1.dy = bbox.ury + SCALE_FROM_MILLIMETERS(10);
  point2.dy = bbox.lly - SCALE_FROM_MILLIMETERS(10) - absci;
  BG_SET(This->axe[0])(This->axe[0], XcBG_Line_POINT1, point1,
		       XcBG_Line_POINT2, point2, XcBG_Line_END);
  break;
 case XcHG_Axe_HORIZONTAL:
  This->nb_axe = 1;
  This->axe = (c_BG_Line **)Xc_malloc("axe **", sizeof(c_BG_Line *));
  This->axe[0] = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
      
  point1.dx = bbox.llx + SCALE_FROM_MILLIMETERS(10) + absci;
  point2.dx = bbox.urx - SCALE_FROM_MILLIMETERS(10);
  point1.dy =  point2.dy = bbox.ury;
  BG_SET(This->axe[0])(This->axe[0], XcBG_Line_POINT1, point1,
		       XcBG_Line_POINT2, point2, XcBG_Line_END);
  break;
 case XcHG_Axe_POLAIRE:
  This->nb_axe = nb_abscisse;
  This->axe = 
   (c_BG_Line **)Xc_malloc("axe **", sizeof(c_BG_Line *)*nb_abscisse);
      
  angle_step = M_PI * 2 / (nb_abscisse); angle = M_PI / 2;
  point1.dx = bbox.llx + (width >> 1);
  point1.dy = bbox.ury + (height >> 1);
  rayon = (height / 3);
  for(i = 0; i < nb_abscisse; i++)
  {
   This->axe[i] = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
   point2.dx = point1.dx + cos(angle) * rayon;
   point2.dy = point1.dy - sin(angle) * rayon;
   BG_SET(This->axe[i])(This->axe[i], XcBG_Line_POINT1, point1,
			XcBG_Line_POINT2, point2, XcBG_Line_END);
   angle -= angle_step;
  }
  break;
 }
  
  
 F(highg).find_minmax(highg, &min, &max);
 if (This->sub_class == XcHG_Axe_POLAIRE)
  min = 0;
 else
  min = MIN(min, 0);
 max = MAX(max, 0);
 delta = ABS(max - min);
 if (delta == 0.0)
  delta = 1.0;
 b = floor( log10(delta)); 
 c = delta / pow(10.0, b);
 if (c<=1)
  grad = 1.0;
 else
 {
  if (c<=2)
   grad = 2.0;
  else
   if (c<=5)
    grad = 5.0;
   else
    grad = 10.0;
 }
 grad *= pow(10.0, b - 1.0);
 if (grad == 0.0 || (max == 0.0 && min == 0.0))
 {
  number = grad = 1.0;
  nb_grad = 2;
 }
 else
 {
  nb_grad = (int)(ceil (max / grad) + ceil( fabs(min) / grad)) + 1;
  number = ceil( max / grad) * grad;
 }
 This->grad_value_origin = number;
  
 This->nb_grad = nb_grad;
 This->grad_line = (c_BG_Line **)Xc_malloc("grad_line **", 
					   sizeof(c_BG_Line *) * nb_grad);
 if (highg->graduation_y)
  switch(This->sub_class)
  {
  case XcHG_Axe_VERTICAL:
  case XcHG_Axe_HORIZONTAL:
   This->grad_line_mark = (c_BG_Line **)Xc_malloc
    ("grad_line_mark**", sizeof(c_BG_Line *)*nb_grad);
   break;
  case XcHG_Axe_POLAIRE:
   This->grad_polyline_mark = (c_BG_Polyline **)Xc_malloc
    ("grad_polyline_mark**", sizeof(c_BG_Polyline *)*nb_grad);
   break;
  }
 This->grad_string = (c_BG_Text **)Xc_malloc("grad_string **", 
					     sizeof(c_BG_Text *) * nb_grad);
 This->grad_number = (char **)Xc_malloc("grad_number **", 
					sizeof(char *) * nb_grad);
 switch(This->sub_class)
 {
 case XcHG_Axe_VERTICAL:
  step = (height - SCALE_FROM_MILLIMETERS(20) - absci) / (nb_grad - 1);
  This->grad_scale = (height - SCALE_FROM_MILLIMETERS(20) - absci)
   / ((nb_grad - 1) * grad);
  This->grad_origin = pos = bbox.ury + SCALE_FROM_MILLIMETERS(10);
  ruler = XqR_H_ALIGN_RIGHT;
  break;
 case XcHG_Axe_HORIZONTAL:
  step = - ( (width - SCALE_FROM_MILLIMETERS(20) - absci) / (nb_grad - 1));
  This->grad_scale = (width - SCALE_FROM_MILLIMETERS(20) - absci) 
   / ((nb_grad - 1) * grad);
  This->grad_origin = pos = bbox.urx - SCALE_FROM_MILLIMETERS(10);
  ruler = XqR_H_ALIGN_RIGHT | XqR_A_90;
  break;
 case XcHG_Axe_POLAIRE:
 default:
  step = (height / 3) / (nb_grad - 1);
  This->grad_scale = (height / 3) / ((nb_grad - 1) * grad);
  This->grad_zero = bbox.ury + (height >> 1);
  This->grad_origin = pos =  This->grad_zero - (height / 3);
  ruler = XqR_H_ALIGN_RIGHT;
  break;
 }
 style = Get_BG_TextStyle(_BaseStd, BGS_GRADUATE);
 color = Get_BG_Color(_BaseStd, BGC_GRAY_MEDIUM);
  
 for(i=0; i<nb_grad; i++)
 {
  This->grad_line[i] = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
  This->grad_string[i] = (c_BG_Text *)NEW(c_BG_Text)(_BaseStd);
  This->grad_number[i] = (char *)Xc_malloc("grad number *", 20);
  sprintf(This->grad_number[i], "%g", number);

  switch(This->sub_class)
  {
  case XcHG_Axe_VERTICAL:
   point1.dx = bbox.urx;
   point2.dx = bbox.urx - SCALE_FROM_MILLIMETERS(1); 
   point3.dx = bbox.urx + size.dx;
   point1.dy = point2.dy = point3.dy = pos;
   t_orig.dx = bbox.llx; 
   t_orig.dy = pos - style->point_size / 2; 
   t_size.dx = point2.dx - SCALE_FROM_MILLIMETERS(1) - bbox.llx;
   t_size.dy = XcText_SIZE_NOT_DEFINED;
   break;
  case XcHG_Axe_HORIZONTAL:
   point1.dx = point2.dx = point3.dx = pos;
   point1.dy = bbox.ury;
   point2.dy = bbox.ury + SCALE_FROM_MILLIMETERS(1);
   point3.dy = bbox.ury - size.dy;
   t_orig.dx = pos - style->point_size / 4; ;
   t_orig.dy = point2.dy + SCALE_FROM_MILLIMETERS(1); 
   t_size.dx = XcText_SIZE_NOT_DEFINED;
   t_size.dy = bbox.lly - point2.dy;
   break;
  case XcHG_Axe_POLAIRE:
   point1.dx = bbox.llx + (width >> 1) - SCALE_FROM_MILLIMETERS(0.5);
   point2.dx = point1.dx + SCALE_FROM_MILLIMETERS(1);
   point1.dy = point2.dy = pos;
   t_orig.dx = bbox.llx; 
   t_orig.dy = pos - style->point_size / 2; 
   t_size.dx = point2.dx - SCALE_FROM_MILLIMETERS(1) - bbox.llx;
   t_size.dy = XcText_SIZE_NOT_DEFINED;
   break;
  }
    
  BG_SET(This->grad_line[i])(This->grad_line[i],
			     XcBG_Line_POINT1, point1,
			     XcBG_Line_POINT2, point2,     
			     XcBG_Line_END);
    
  if (highg->graduation_y)
  {
   switch(This->sub_class)
   {
   case XcHG_Axe_VERTICAL:
   case XcHG_Axe_HORIZONTAL:
    This->grad_line_mark[i] = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
    BG_SET(This->grad_line_mark[i])(This->grad_line_mark[i],
				    XcBG_Line_POINT1, point1,
				    XcBG_Line_POINT2, point3,     
				    XcBG_Line_FG_COLOR, color,
				    XcBG_Line_END);
    break;
   case XcHG_Axe_POLAIRE:
    This->grad_polyline_mark[i] = 
     (c_BG_Polyline *)NEW(c_BG_Polyline)(_BaseStd);
    BG_SET(This->grad_polyline_mark[i])
     (This->grad_polyline_mark[i], XcBG_Polyline_FG_COLOR, color,
      XcBG_Polyline_LINE_THICKNESS, SCALE_FROM_POINTS(0.5),
      XcBG_Polyline_END);
    angle_step = M_PI * 2 / (nb_abscisse); angle = M_PI / 2;
    point1.dx = bbox.llx + (width >> 1);
    point1.dy = bbox.ury + (height >> 1);
    rayon = point1.dy - pos;
    for(j = 0; j < nb_abscisse; j++)
    {
     point2.dx = point1.dx + cos(angle) * rayon;
     point2.dy = point1.dy - sin(angle) * rayon;
	  
     BG_SET(This->grad_polyline_mark[i])
      (This->grad_polyline_mark[i], XcBG_Polyline_ADD_POINT, point2,
       XcBG_Polyline_END);
     angle -= angle_step;
    }
    point1.dy = pos;
    BG_SET(This->grad_polyline_mark[i])
     (This->grad_polyline_mark[i], XcBG_Polyline_ADD_POINT, point1,
      XcBG_Polyline_END);
    break;
   }
  }
  BG_SET(This->grad_string[i])(This->grad_string[i],
			       XcBG_Text_ORIGIN, t_orig,
			       XcBG_Text_SIZE, t_size,
			       XcBG_Text_STRING, This->grad_number[i],
			       XcBG_Text_STYLE, style,
			       XcBG_Text_RULER, ruler,
			       XcBG_Text_END); 
    
  pos += step;
  number -= grad;
  if (This->sub_class == XcHG_Axe_POLAIRE)
   continue;
  if ( fabs(number) < (grad/2) )
  {
   number = 0.0;
   This->grad_zero = pos;
   if ( (!highg->graduation_y) && (i != nb_grad-1) ) 
   {
    This->nb_axe = 2;
    This->axe = (c_BG_Line **)
     Xc_realloc("axe **", This->axe, sizeof(c_BG_Line *) * 2);
    This->axe[1] = (c_BG_Line *)NEW(c_BG_Line)(_BaseStd);
    switch(This->sub_class)
    {
    case XcHG_Axe_VERTICAL:
     point1.dx = bbox.urx;
     point2.dx = bbox.urx + size.dx;
     point1.dy = point2.dy = pos;
     break;
    case XcHG_Axe_HORIZONTAL:
     point1.dx = point2.dx = pos;
     point1.dy = bbox.ury;
     point2.dy = bbox.ury - size.dy;
     break;
    }
    BG_SET(This->axe[1])(This->axe[1],
			 XcBG_Line_POINT1, point1,
			 XcBG_Line_POINT2, point2,
			 XcBG_Line_FG_COLOR, color,
			 XcBG_Line_END);
   }
  }
 }
}

static coord_t convert_value(This, value)
c_HG_Axe *This;
real value;
{
 coord_t	pos;
  
 if (value==0.0)
  pos = This->grad_zero;
 else
  switch(This->sub_class)
  {
  case XcHG_Axe_VERTICAL:
   pos = (This->grad_value_origin-value) * This->grad_scale + 
    This->grad_origin;
   break;
  case XcHG_Axe_HORIZONTAL:
   pos = This->grad_origin - 
    (This->grad_value_origin-value) * This->grad_scale;
   break;
  case XcHG_Axe_POLAIRE:
  default:
   if (value <= 0.0) 
    pos = This->grad_zero;
   else
    pos = This->grad_origin +
     (This->grad_value_origin-value) * This->grad_scale;
   break;
  }
 return pos;
}

static void change_xinfo(This)
c_HG_Axe	*This;
{
 int i;
  
 if (This->nb_axe)
 {
  for(i = 0; i < This->nb_axe; i++)
   BG_CHANGE_XINFO(This->axe[i])(This->axe[i], &This->highg->X_info);
 }
 for(i=0; i<This->nb_grad; i++)
 {
  BG_CHANGE_XINFO(This->grad_line[i])(This->grad_line[i],
				      &This->highg->X_info);
  if (This->grad_line_mark)
   BG_CHANGE_XINFO(This->grad_line_mark[i])(This->grad_line_mark[i],
					    &This->highg->X_info);
  if (This->grad_polyline_mark)
   BG_CHANGE_XINFO(This->grad_polyline_mark[i])(This->grad_polyline_mark[i],
						&This->highg->X_info);
    
  BG_CHANGE_XINFO(This->grad_string[i])(This->grad_string[i],
					&This->highg->X_info);
 }
}

static error change_base(This)
c_HG_Axe	*This;
{
 return XC_NO_ERROR;
}

static void display_redraw(This)
c_HG_Axe	*This;
{
 int i;
 c_HighGraph	*highg;
  
 Xc_TRACE(("redraw axe"));
  
 highg = This->highg;
 if (This->nb_axe)
 {
  for(i = 0; i < This->nb_axe; i++)
   BG_DISPLAY_REDRAW(This->axe[i])(This->axe[i], highg->matrix,
				   highg->scale, 0, 0);
 }
 for(i=0; i<This->nb_grad; i++)
 {
  BG_DISPLAY_REDRAW(This->grad_line[i])(This->grad_line[i], 
					highg->matrix, highg->scale, 0, 0);
  if (This->grad_line_mark)
   BG_DISPLAY_REDRAW(This->grad_line_mark[i])(This->grad_line_mark[i], 
					      highg->matrix, 
					      highg->scale, 0, 0);
  if (This->grad_polyline_mark)
   BG_DISPLAY_REDRAW(This->grad_polyline_mark[i])
    (This->grad_polyline_mark[i],  highg->matrix, highg->scale, 0, 0);
  BG_DISPLAY_REDRAW(This->grad_string[i])(This->grad_string[i], 
					  highg->matrix, highg->scale, 0, 0);
 }
}

static void add_to_vectorgraph(This)
c_HG_Axe  *This;
{
 int	i;
  
 if (This->nb_axe)
 {
  for(i = 0; i < This->nb_axe; i++)
   BG_ADD_TO_VECTORGRAPH(This->axe[i])(This->axe[i], This->highg->vectorg);
 }
 for(i=0; i<This->nb_grad; i++)
 {
  BG_ADD_TO_VECTORGRAPH(This->grad_line[i])(This->grad_line[i], 
					    This->highg->vectorg);
  if (This->grad_line_mark)
   BG_ADD_TO_VECTORGRAPH(This->grad_line_mark[i])(This->grad_line_mark[i], 
						  This->highg->vectorg);
  if (This->grad_polyline_mark)
   BG_ADD_TO_VECTORGRAPH(This->grad_polyline_mark[i])
    (This->grad_polyline_mark[i], This->highg->vectorg);
  BG_ADD_TO_VECTORGRAPH(This->grad_string[i])(This->grad_string[i], 
					      This->highg->vectorg);
 }
}

static void sub_to_vectorgraph(This)
c_HG_Axe  *This;
{
 int	i;
  
 if (This->nb_axe)
 {
  for(i = 0; i < This->nb_axe; i++)
   BG_SUB_TO_VECTORGRAPH(This->axe[i])(This->axe[i], This->highg->vectorg);
 }
 for(i=0; i<This->nb_grad; i++)
 {
  BG_SUB_TO_VECTORGRAPH(This->grad_line[i])(This->grad_line[i], 
					    This->highg->vectorg);
  if (This->grad_line_mark)
   BG_SUB_TO_VECTORGRAPH(This->grad_line_mark[i])(This->grad_line_mark[i], 
						  This->highg->vectorg);
  if (This->grad_polyline_mark)
   BG_SUB_TO_VECTORGRAPH(This->grad_polyline_mark[i])
    (This->grad_polyline_mark[i], This->highg->vectorg);
  BG_SUB_TO_VECTORGRAPH(This->grad_string[i])(This->grad_string[i], 
					      This->highg->vectorg);
 }
}

static void setHG(c_HG_Axe *This, ...)
{
 va_list ap;

 Xc_HISTORY(("set"));

 va_start(ap, This);
 vset(This, ap);
 va_end(ap);
} 

static void vset(This, ap)
c_HG_Axe *This;
va_list ap;
{
 boolean out_flag;
 boolean calcul;
  
 Xc_TRACE(("vset"));
  
 out_flag = calcul = FALSE;
 do {
     switch(va_arg(ap, HG_Axe_set_code_t))
     {
     case XcHG_Axe_END:
      Xc_TRACE(("XcHG_Axe_END"));
      out_flag = TRUE;
      break;
     default:
      Xc_BREAK(("Unknow HG_Axe_set_code"));
      break;
     }
    } while(!out_flag);
 if (calcul)
  calcul_axe(This);
}

static void calcul_axe(This)
c_HG_Axe	*This;
{
}

/* -------------------------------------------------------------------- **
** ps print, read & write HG_Axe					**
** -------------------------------------------------------------------- */ 
static void ps_print(This, post)
c_HG_Axe *This;
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
c_HG_Axe	 *This;
c_DocFile	 *doc;
{
 return FALSE;
}



