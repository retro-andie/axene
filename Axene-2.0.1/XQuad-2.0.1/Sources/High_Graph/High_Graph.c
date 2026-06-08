/*
** High_Graph.c for XQuad in High_Graph/
** Object and methods for High_Graph
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
** Started on  Fri May 16 13:54:12 1997 Antoine Buat
** Last update Fri May 16 14:59:03 1997 Antoine Buat
*/

#include "High_GraphP.h"
#include "Box_AskGraph.h"
#include "Cursor.h"
#include "Colormap.h"
#include "MainInterface.h"
#include "HighGraphStd.h"
#include "HG_Manager.h"

extern c_MainInterface  *MainInterface1;
extern c_Colormap	*GlobColormap;

static void *cons_HighGraph();
static void dest_HighGraph();
static void *copy_HighGraph();
static void HG_init();
static void HG_draw();
static void HG_calcul_and_draw();
static void HG_set_X_info();
static void HG_del_X_info();
static void HG_find_minmax();
static int HG_configure();
static void HG_set_unset_legend();
static void HG_set_unset_title();
static void HG_set_unset_grad_y();
static void HG_set_unset_abscisse();
static void HG_set_unset_ordonnee();
static void HG_change_graph_class();
static void HG_test_tstyle_hook();
static void HG_test_tstyle_apply();

static void HG_free_tab();
static void HG_alloc_tab();
static void HG_fill_tab();
static boolean HG_test_region();
static void HG_recalcul_region();
static region_t	*dup_and_check_region();
static region_t	*dup_region();
static void free_region();

sf_HighGraph fc_HighGraph =
{
 cons_HighGraph,
 dest_HighGraph,
 copy_HighGraph,
 HG_init,
 HG_draw,
 HG_calcul_and_draw,
 HG_set_X_info,
 HG_del_X_info,
 HG_find_minmax,
 HG_configure,
 HG_set_unset_legend,
 HG_set_unset_title,
 HG_set_unset_grad_y,
 HG_set_unset_abscisse,
 HG_set_unset_ordonnee,
 HG_change_graph_class,
 HG_test_tstyle_hook,
 HG_test_tstyle_apply,
 readHighGraph, writeHighGraph
};

static void *cons_HighGraph(moteur, BaseStd, region, class, sub_class, vectorg)
c_Moteur	*moteur;
BaseStd_t	*BaseStd;
region_t	*region;
HG_class_t	class;
int		sub_class;
c_VectorGraph	*vectorg;
{
 c_HighGraph	*This;
 void		*HG_obj = NULL;
 boolean	must_ask = FALSE;
  
 if ((This = (c_HighGraph *)Xc_malloc("High graph",
				      sizeof(c_HighGraph))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 memset(This, 0, sizeof(c_HighGraph));
 This->f = &fc_HighGraph;
 This->moteur = moteur;
 This->BaseStd = BaseStd;
 This->vectorg = NULL;
 This->multiple_region = This->region_direction = FALSE;
 if (moteur)
  This->orig_region = dup_and_check_region(This, region);
 else
  This->orig_region = dup_region(region);
 if (!This->orig_region)
 {
  Xc_free(This);
  return NULL;
 }
  
 This->matrix = NEW(c_Matrix)();
 This->class = class;

 if (moteur)
  must_ask = HG_test_region(This);
  
 This->legend = This->axe = FALSE;
 This->d_abscisse = This->d_ordonnee = This->d_title = This->d_legend = TRUE;
 This->graduation_y = FALSE;
  
 This->region_data = This->region_abscisse = This->region_ordonnee = NULL;
 This->tab_value = (real **)NULL;
 This->tab_abscisse = This->tab_ordonnee = (HG_string_t *)NULL;
 This->tab_title.string = NULL;
 This->tab_title.tstyle = (c_TextStyle *)NULL;
 This->tab_nb_abscisse = This->tab_nb_ordonnee = 0;
  
 if (moteur)
 {
  HG_recalcul_region(This);
  HG_alloc_tab(This);
  HG_fill_tab(This);
  
  HG_obj = NEW(c_HG_Manager)(This, class, 0);
  This->HG_object = HG_obj;
      
  if (must_ask)
   if (HG_configure(This) == -1)
   {
    /*--- should delete graph ---*/
    dest_HighGraph(This);
    return NULL;
   }
 }
 if (vectorg)
 {
  This->vectorg = vectorg;
  vectorg->highg = This;
  if (moteur)
   HG_ADD_TO_VECTORGRAPH(HG_obj)(HG_obj);
 }
  
 Xc_HISTORY(("Object HighGraph build"));
 return This;
}

static void dest_HighGraph(This)
c_HighGraph	*This;
{
 if (This->vectorg)
  HG_SUB_TO_VECTORGRAPH(This->HG_object)(This->HG_object, This->vectorg);
 DELETE(c_HG_Manager)(This->HG_object);
 This->abscisse = This->ordonnee = This->title = FALSE;
 This->nb_abscisse = This->nb_ordonnee = 0;
 HG_free_tab(This);
 DELETE(c_Matrix)(This->matrix);
 free_region(This->orig_region);
 free_region(This->region_data);
 free_region(This->region_abscisse);
 free_region(This->region_ordonnee);
 Xc_free(This);
 Xc_HISTORY(("Object HighGraph destroyed"));
}

static void *copy_HighGraph(This, vectorg)
c_HighGraph 	*This;
c_VectorGraph	*vectorg;
{
 c_HighGraph *That;
 void	      *HG_obj;

 if ((That = (c_HighGraph *)Xc_malloc("copy high graph",
				      sizeof(c_HighGraph))) == NULL)
  return NULL;
  
 memcpy(That, This, sizeof(c_HighGraph));
 That->orig_region = dup_region(This->orig_region);
 That->vectorg = NULL;
 That->matrix = NEW(c_Matrix)();
  
 That->tab_value = (real **)NULL;
 That->tab_abscisse = That->tab_ordonnee = (HG_string_t *)NULL;
 That->tab_title.string = NULL;
 That->tab_title.tstyle = (c_TextStyle *)NULL;
 That->tab_nb_abscisse = That->tab_nb_ordonnee = 0;
 This->region_data = This->region_abscisse = This->region_ordonnee = NULL;

 HG_recalcul_region(That);
 HG_alloc_tab(That);
 HG_fill_tab(That);
  
 HG_obj = NEW(c_HG_Manager)(That, That->class, 0);
 That->HG_object = HG_obj;
 HG_REGENERATE(HG_obj)(HG_obj);
  
 if (vectorg)
 {
  That->vectorg = vectorg;
  vectorg->highg = That;
  HG_ADD_TO_VECTORGRAPH(HG_obj)(HG_obj);
 }
 Xc_HISTORY(("Object HighGraph copied"));
 return That;
}

static void HG_init(This, moteur)
c_HighGraph	*This;
c_Moteur	*moteur;
{
 void		*HG_obj;
  
 This->moteur = moteur;
  
 HG_recalcul_region(This);
 HG_alloc_tab(This);
 HG_fill_tab(This);
  
 HG_obj = NEW(c_HG_Manager)(This, This->class, 0);
 This->HG_object = HG_obj;
  
 if (This->vectorg)
  HG_ADD_TO_VECTORGRAPH(HG_obj)(HG_obj);
  
 Xc_HISTORY(("Object HighGraph initialized"));
}

static void HG_draw(This)
c_HighGraph	*This;
{
 HG_fill_tab(This);
 HG_REGENERATE(This->HG_object)(This->HG_object);
 HG_DISPLAY_REDRAW(This->HG_object)(This->HG_object, TRUE);
}

static void HG_calcul_and_draw(This)
c_HighGraph	*This;
{
 HG_free_tab(This);
 HG_recalcul_region(This);
 HG_alloc_tab(This);
 HG_fill_tab(This);
 HG_REGENERATE(This->HG_object)(This->HG_object);
 HG_CHANGE_XINFO(This->HG_object)(This->HG_object);
 HG_DISPLAY_REDRAW(This->HG_object)(This->HG_object, FALSE);
}

static void HG_free_tab(This)
c_HighGraph	*This;
{
 c_Moteur	*moteur;
 region_t	*region;
 int	i, j;
 int	width, height;
  
 moteur = This->moteur;
 width = This->nb_ordonnee;
 height = This->nb_abscisse;

 /* --------------------- FREE tab_value ------------------------ */
 if (This->tab_value)
 {
  if (This->multiple_region)
  {
   region = This->region_data;
   while(region)
   {
    for(i = region->x_min; i <= region->x_max; i++)
     for(j = region->y_min; j<= region->y_max; j++)
      F(moteur).sub_graph_dep(moteur, This, j, i);
    region = region->NextRegion;
   }
   for(i = 0; i < This->tab_nb_ordonnee; i++)
    Xc_free(This->tab_value[i]);
  }
  else
  {
   for(i = 0; i < This->tab_nb_ordonnee; i++)
   {
    for(j = 0; j < This->tab_nb_abscisse; j++)
    {
     if (This->tab_col_row)
      F(moteur).sub_graph_dep(moteur, This, 
			      This->tab_data.column + i, 
			      This->tab_data.row + j);
     else
      F(moteur).sub_graph_dep(moteur, This, 
			      This->tab_data.column + j, 
			      This->tab_data.row + i);
    }
    Xc_free(This->tab_value[i]);
   }
  }
  Xc_free(This->tab_value);
  This->tab_value = NULL;
 }
  
 /* ------------------- FREE tab_abcsisse ---------------------- */
 if (This->tab_abscisse)
 {
  for(i = 0; i < This->tab_nb_abscisse; i++)
  {
   F(moteur).sub_graph_dep(moteur, This, 
			   This->tab_abscisse[i].column,
			   This->tab_abscisse[i].row);
   if (This->tab_abscisse[i].string)
    Xc_free(This->tab_abscisse[i].string);
  }
  Xc_free(This->tab_abscisse);
  This->tab_abscisse = NULL;
 }
  
 /* ------------------- FREE tab_ordonnee ---------------------- */
 if (This->tab_ordonnee)
 {
  for(i = 0; i < This->tab_nb_ordonnee; i++)
  {
   F(moteur).sub_graph_dep(moteur, This, 
			   This->tab_ordonnee[i].column,
			   This->tab_ordonnee[i].row);
   if (This->tab_ordonnee[i].string)
    Xc_free(This->tab_ordonnee[i].string);
  }
  Xc_free(This->tab_ordonnee);
  This->tab_ordonnee = NULL;
 }      
  
 /* --------------------- FREE tab_title ---------------------- */
 if (This->tab_title.tstyle)
 {
  F(moteur).sub_graph_dep(moteur, This,
			  This->tab_title.column,
			  This->tab_title.row);
  if (This->tab_title.string)
   Xc_free(This->tab_title.string);
  This->tab_title.string = NULL;
 }
}

static void HG_alloc_tab(This)
c_HighGraph	*This;
{
 c_Moteur	*moteur;
 int	i, j, i2, j2;
 int	width, height;
 region_t	*region;
  
 moteur = This->moteur;
 width = This->nb_ordonnee;
 height = This->nb_abscisse;

 /* ------------------- ALLOC tab_value ---------------------- */
 if (width && height)
 {
  This->tab_value = 
   (real **)Xc_malloc("tab value **", sizeof(real *) * width);

  This->tab_data.column = This->region_data->x_min;
  This->tab_data.row = This->region_data->y_min;
    
  if (This->multiple_region)
  {
   for(i = 0; i < This->nb_ordonnee; i++)
    This->tab_value[i] = 
     (real *)Xc_malloc("tab value *", sizeof(real) * height);
      
   if (This->region_direction)	/* HORIZONTAL */ 
   {
    for(i = This->region_data->x_min; 
	i <= This->region_data->x_max; i++)
    {
     region = This->region_data;
     while(region)
     {
      for(j = region->y_min; j <= region->y_max; j++)
      {
       F(moteur).add_graph_dep(moteur, This, i, j);
      }
      region = region->NextRegion;
     }
    }
   }
   else				/* VERTICAL */
   {
    for(j = This->region_data->y_min; 
	j <= This->region_data->y_max; j++)
    {
     region = This->region_data;
     while(region)
     {
      for(i = region->x_min; i <= region->x_max; i++)
      {
       F(moteur).add_graph_dep(moteur, This, i, j);
      }
      region = region->NextRegion;
     }
    }
   }
  }
  else
  {
   for(i = 0; i < This->nb_ordonnee; i++)
   {
    This->tab_value[i] = 
     (real *)Xc_malloc("tab value *", sizeof(real) * height);
    for(j = 0; j < This->nb_abscisse; j++)
    {
     if (This->col_row)
      F(moteur).add_graph_dep(moteur, This, 
			      This->tab_data.column + i, 
			      This->tab_data.row + j);
     else
      F(moteur).add_graph_dep(moteur, This, 
			      This->tab_data.column + j, 
			      This->tab_data.row + i);
    }
   }
  }
 }
  
 /* ------------------ ALLOC tab_abscisse -------------------- */
 if (This->abscisse)
 {
  This->tab_abscisse = 
   (HG_string_t *)Xc_malloc("tab abscisse *", 
			    sizeof(HG_string_t)* height);
  memset(This->tab_abscisse, 0, sizeof(HG_string_t)* height);
      
  if (This->multiple_region)
  {
   if (This->region_direction)	/* HORIZONTAL */
   {
    if (This->col_row)
    {
     region = This->region_abscisse;
     i = 0; i2 = region->x_min;
     while(region)
     {
      for(j2 = region->y_min; j2 <= region->y_max; j2++, i++)
      {
       This->tab_abscisse[i].column = i2;
       This->tab_abscisse[i].row = j2;
       F(moteur).add_graph_dep(moteur, This, i2, j2);
      }
      region = region->NextRegion;
     }
    }
    else 
    {
     region = This->region_abscisse;
     j2 = region->y_min;
     for(i2 = region->x_min, i = 0; i2 <= region->x_max; i2++, i++)
     {
      This->tab_abscisse[i].column = i2;
      This->tab_abscisse[i].row = j2;
      F(moteur).add_graph_dep(moteur, This, i2, j2);
     }
    }
   }
   else				/* VERTICAL */
   {
    if (This->col_row)
    {
     region = This->region_abscisse;
     i2 = region->x_min;
     for(j2 = region->y_min, i = 0; j2 <= region->y_max; j2++, i++)
     {
      This->tab_abscisse[i].column = i2;
      This->tab_abscisse[i].row = j2;
      F(moteur).add_graph_dep(moteur, This, i2, j2);
     }
    }
    else
    {
     region = This->region_abscisse;
     i = 0; j2 = region->y_min;
     while(region)
     {
      for(i2 = region->x_min; i2 <= region->x_max; i2++, i++)
      {
       This->tab_abscisse[i].column = i2;
       This->tab_abscisse[i].row = j2;
       F(moteur).add_graph_dep(moteur, This, i2, j2);
      }
      region = region->NextRegion;
     }
    }
   }
  }
  else
  {
   for(i = 0; i < This->nb_abscisse; i++)
   {
    if (This->col_row)
    {
     This->tab_abscisse[i].column = This->region_abscisse->x_min;
     This->tab_abscisse[i].row = This->region_abscisse->y_min + i;
    }
    else
    {
     This->tab_abscisse[i].column = This->region_abscisse->x_min + i;
     This->tab_abscisse[i].row = This->region_abscisse->y_min;
    }
    F(moteur).add_graph_dep(moteur, This, 
			    This->tab_abscisse[i].column,
			    This->tab_abscisse[i].row);
   }
  }
 }		       
 /* ------------------ ALLOC tab_ordonnee -------------------- */
 if (This->ordonnee)
 {
  This->tab_ordonnee = 
   (HG_string_t *)Xc_malloc("tab ordonnee *", sizeof(HG_string_t)
			    * width);
  memset(This->tab_ordonnee, 0, sizeof(HG_string_t)* width);
  if (This->multiple_region)
  {
   if (This->region_direction)	/* HORIZONTAL */
   {
    if (This->col_row)
    {
     region = This->region_ordonnee;
     j2 = region->y_min;
     for(i2 = region->x_min, i = 0; i2 <= region->x_max; i2++, i++)
     {
      This->tab_ordonnee[i].column = i2;
      This->tab_ordonnee[i].row = j2;
      F(moteur).add_graph_dep(moteur, This, i2, j2);
     }
    }
    else
    {
     region = This->region_ordonnee;
     i = 0; i2 = region->x_min;
     while(region)
     {
      for(j2 = region->y_min; j2 <= region->y_max; j2++, i++)
      {
       This->tab_ordonnee[i].column = i2;
       This->tab_ordonnee[i].row = j2;
       F(moteur).add_graph_dep(moteur, This, i2, j2);
      }
      region = region->NextRegion;
     }
    }
   }
   else				/* VERTICAL */
   {
    if (This->col_row)
    {
     region = This->region_ordonnee;
     i = 0; j2 = region->y_min;
     while(region)
     {
      for(i2 = region->x_min; i2 <= region->x_max; i2++, i++)
      {
       This->tab_ordonnee[i].column = i2;
       This->tab_ordonnee[i].row = j2;
       F(moteur).add_graph_dep(moteur, This, i2, j2);
      }
      region = region->NextRegion;
     }
    }
    else
    {
     region = This->region_ordonnee;
     i2 = region->x_min;
     for(j2 = region->y_min, i = 0; j2 <= region->y_max; j2++, i++)
     {
      This->tab_ordonnee[i].column = i2;
      This->tab_ordonnee[i].row = j2;
      F(moteur).add_graph_dep(moteur, This, i2, j2);
     }
    }
   }
  }
  else
  {
   for(i = 0; i < This->nb_ordonnee; i++)
   {
    if (This->col_row)
    {
     This->tab_ordonnee[i].column = This->region_ordonnee->x_min + i;
     This->tab_ordonnee[i].row = This->region_ordonnee->y_min;
    }
    else
    {
     This->tab_ordonnee[i].column = This->region_ordonnee->x_min;
     This->tab_ordonnee[i].row = This->region_ordonnee->y_min + i;
    }
    F(moteur).add_graph_dep(moteur, This, 
			    This->tab_ordonnee[i].column,
			    This->tab_ordonnee[i].row);
   }
  }
 }
 if (This->title)
 {
  This->tab_title.tstyle = (c_TextStyle *)1;
  This->tab_title.column = This->orig_region->x_min;
  This->tab_title.row = This->orig_region->y_min;
  F(moteur).add_graph_dep(moteur, This, This->orig_region->x_min, 
			  This->orig_region->y_min);
 }
 This->tab_nb_abscisse = height;
 This->tab_nb_ordonnee = width;
 This->tab_col_row = This->col_row;
}

static void HG_fill_tab(This)
c_HighGraph	*This;
{
 c_Moteur	*moteur;
 int	i, j, io, ja;
 int	width, height;
 real		value;
 CellStyle_t	*cstyle;
 char		*string;
 region_t	*region;
  
 moteur = This->moteur;
 width = This->nb_ordonnee;
 height = This->nb_abscisse;
  
 /* ------------------- FILL tab_value ---------------------- */
 if (This->multiple_region)
 {
  if (This->region_direction)	/* HORIZONTAL */
  {
   region = This->region_data; j = 0;
   while(region)
   {
    for(ja = region->y_min; ja <= region->y_max; ja++, j++)
    {
     for(io = region->x_min, i = 0; io <= region->x_max; io++, i++)
     {
      if ( i || j)
       F(moteur).get_val(moteur, io, ja, &value, NULL, NULL);
      else
      {
       F(moteur).get_val(moteur, io, ja, &value, NULL, &cstyle);
       This->tab_data.tstyle = cstyle->text_style;
      }
      if (This->col_row)
       This->tab_value[i][j] = value;
      else
       This->tab_value[j][i] = value;
     }
    }
    region = region->NextRegion;
   }
  }
  else				/* VERTICAL */
  {
   region = This->region_data; i = 0;
   while(region)
   {
    for(io = region->x_min; io <= region->x_max; io++, i++)
    {
     for(ja = region->y_min, j = 0; ja <= region->y_max; ja++, j++)
     {
      if ( i || j)
       F(moteur).get_val(moteur, io, ja, &value, NULL, NULL);
      else
      {
       F(moteur).get_val(moteur, io, ja, &value, NULL, &cstyle);
       This->tab_data.tstyle = cstyle->text_style;
      }
      if (This->col_row)
       This->tab_value[i][j] = value;
      else
       This->tab_value[j][i] = value;
     }
    }
    region = region->NextRegion;
   }
  }
 }
 else
 {
  if (This->col_row)
  {
   for(i = This->region_data->x_min, io = 0; 
       i <= This->region_data->x_max; i++, io++)
   {
    for(j = This->region_data->y_min, ja = 0;
	j <= This->region_data->y_max; j++, ja++)
    {
     if ( io || ja)
      F(moteur).get_val(moteur, i, j, &value, NULL, NULL);
     else
     {
      F(moteur).get_val(moteur, i, j, &value, NULL, &cstyle);
      This->tab_data.tstyle = cstyle->text_style;
     }
     This->tab_value[io][ja] = value;
    }
   }
  }
  else
  {
   for(i = This->region_data->y_min, io = 0; 
       i <= This->region_data->y_max; i++, io++)
   {
    for(j = This->region_data->x_min, ja = 0;
	j <= This->region_data->x_max; j++, ja++)
    {
     if ( io || ja)
      F(moteur).get_val(moteur, j, i, &value, NULL, NULL);
     else
     {
      F(moteur).get_val(moteur, j, i, &value, NULL, &cstyle);
      This->tab_data.tstyle = cstyle->text_style;
     }
     This->tab_value[io][ja] = value;
    }
   }
  }
 }
 /* ------------------- FILL tab_abscisse ---------------------- */
 if (This->abscisse)
 {
  if (This->multiple_region)
  {
   for(i = 0; i < This->tab_nb_abscisse; i++)
   {
    F(moteur).get_val(moteur, This->tab_abscisse[i].column,
		      This->tab_abscisse[i].row, NULL, &string, &cstyle);
    if (This->tab_abscisse[i].string)
     Xc_free(This->tab_abscisse[i].string);
    This->tab_abscisse[i].string = string;
    This->tab_abscisse[i].tstyle = cstyle->text_style;
   }
  }
  else
  {
   if (This->col_row)
   {
    for(i = This->region_abscisse->y_min, io = 0; 
	i <= This->region_abscisse->y_max; i++, io++)
    {
     F(moteur).get_val(moteur, This->region_abscisse->x_min, i, 
		       NULL, &string, &cstyle);
     if (This->tab_abscisse[io].string)
      Xc_free(This->tab_abscisse[io].string);
     This->tab_abscisse[io].string = string;
     This->tab_abscisse[io].tstyle = cstyle->text_style;
    }
   }
   else
   {
    for(i = This->region_abscisse->x_min, io = 0; 
	i <= This->region_abscisse->x_max; i++, io++)
    {
     F(moteur).get_val(moteur, i, This->region_abscisse->y_min, 
		       NULL, &string, &cstyle);
     if (This->tab_abscisse[io].string)
      Xc_free(This->tab_abscisse[io].string);
     This->tab_abscisse[io].string = string;
     This->tab_abscisse[io].tstyle = cstyle->text_style;
    }
   }
  }
 }
  
 /* ------------------- FILL tab_ordonnee ---------------------- */
 if (This->ordonnee)
 {
  if (This->multiple_region)
  {
   for(i = 0; i < This->tab_nb_ordonnee; i++)
   {
    F(moteur).get_val(moteur, This->tab_ordonnee[i].column,
		      This->tab_ordonnee[i].row, NULL, &string, &cstyle);
    if (This->tab_ordonnee[i].string)
     Xc_free(This->tab_ordonnee[i].string);
    This->tab_ordonnee[i].string = string;
    This->tab_ordonnee[i].tstyle = cstyle->text_style;
   }
  }
  else
  {
   if (This->col_row)
   {
    for(i = This->region_ordonnee->x_min, io = 0; 
	i <= This->region_ordonnee->x_max; i++, io++)
    {
     F(moteur).get_val(moteur, i, This->region_ordonnee->y_min, 
		       NULL, &string, &cstyle);
     if (This->tab_ordonnee[io].string)
      Xc_free(This->tab_ordonnee[io].string);
     This->tab_ordonnee[io].string = string;
     This->tab_ordonnee[io].tstyle = cstyle->text_style;
    }
   }
   else
   {
    for(i = This->region_ordonnee->y_min, io = 0; 
	i <= This->region_ordonnee->y_max; i++, io++)
    {
     F(moteur).get_val(moteur, This->region_ordonnee->x_min, i, 
		       NULL, &string, &cstyle);
     if (This->tab_ordonnee[io].string)
      Xc_free(This->tab_ordonnee[io].string);
     This->tab_ordonnee[io].string = string;
     This->tab_ordonnee[io].tstyle = cstyle->text_style;
    }
   }
  }
 }
  
 /* --------------------- FILL tab_title ----------------------- */
 if (This->title)
 {
  F(moteur).get_val(moteur, This->orig_region->x_min, 
		    This->orig_region->y_min, NULL, &string, &cstyle);
  if (This->tab_title.string)
   Xc_free(This->tab_title.string);
  This->tab_title.string = string;
  This->tab_title.tstyle = cstyle->text_style;
 }
}


static void HG_find_minmax(This, min, max)
c_HighGraph	*This;
real	*min;
real	*max;
{
 int		i, j;
 real		total;
  
 *min = This->tab_value[0][0];
 *max = This->tab_value[0][0];
  
 if (This->class == HG_SURFACE)
 {
  for(i = 0; i < This->nb_abscisse; i++)
  {
   total = 0.0;
   for(j = 0; j < This->nb_ordonnee; j++)
   {
    total += This->tab_value[j][i];
    if (total > *max) *max = total;
    if (total < *min) *min = total;
   } 
  }
 }
 else
 {
  for(i = 0; i < This->nb_ordonnee; i++)
  {
   for(j = 0; j < This->nb_abscisse; j++)
   {
    if (This->tab_value[i][j] > *max)
     *max = This->tab_value[i][j];
    if (This->tab_value[i][j] < *min)
     *min = This->tab_value[i][j];
   } 
  }
 }
}

static boolean HG_test_region(This)
c_HighGraph	*This;
{
 int	width, height;
 boolean	must_ask;
 region_t	*region;
  
 region = This->orig_region;
 if (This->multiple_region)
 {
  This->col_row = This->region_direction;
  must_ask = FALSE;
    
  if (This->col_row)
  {
   width = region->x_max - region->x_min + 1;
   if (width == 1)
    This->abscisse = FALSE;
   else
    This->abscisse = 
     !F(This->moteur).get_val(This->moteur, region->x_min,
			      region->y_max,  NULL, NULL, NULL);
   if (This->abscisse)
   {
    This->ordonnee = 
     !F(This->moteur).get_val(This->moteur, region->x_min + 1,
			      region->y_min, NULL, NULL, NULL);
   }
   else
   {
    This->ordonnee = 
     !F(This->moteur).get_val(This->moteur, region->x_min,
			      region->y_min, NULL, NULL, NULL);
   }
  }
  else
  {
   height = region->y_max - region->y_min + 1;
   if (height == 1)
    This->abscisse = FALSE;
   else
    This->abscisse = 
     !F(This->moteur).get_val(This->moteur, region->x_max,
			      region->y_max,  NULL, NULL, NULL);
   if (This->abscisse)
   {
    This->ordonnee =
     !F(This->moteur).get_val(This->moteur, region->x_min,
			      region->y_min + 1, NULL, NULL, NULL);
   }
   else
   {
    This->ordonnee =
     !F(This->moteur).get_val(This->moteur, region->x_min,
			      region->y_min, NULL, NULL, NULL);
   }
  }
 }
 else
 {
  width = region->x_max - region->x_min + 1;
  height = region->y_max - region->y_min + 1;
    
  if (height >= width)
  {
   This->col_row = TRUE;
   if (width == 1)
    This->abscisse = FALSE;
   else
    This->abscisse = 
     !F(This->moteur).get_val(This->moteur, region->x_min,
			      region->y_max,  NULL, NULL, NULL);
   if (This->abscisse)
   {
    This->ordonnee =
     !F(This->moteur).get_val(This->moteur, region->x_min + 1,
			      region->y_min, NULL, NULL, NULL);
    must_ask = (width > 2);
   }
   else
   {
    This->ordonnee = 
     !F(This->moteur).get_val(This->moteur, region->x_min,
			      region->y_min, NULL, NULL, NULL);
    must_ask = (width > 1);
   }
  }
  else
  {
   This->col_row = FALSE;
   if (height == 1)
    This->abscisse = FALSE;
   else
    This->abscisse = 
     !F(This->moteur).get_val(This->moteur, region->x_max,
			      region->y_max,  NULL, NULL, NULL);
   if (This->abscisse)
   {
    This->ordonnee =
     !F(This->moteur).get_val(This->moteur, region->x_min,
			      region->y_min + 1, NULL, NULL, NULL);
    must_ask = (height > 2);
   }
   else
   {
    This->ordonnee =
     !F(This->moteur).get_val(This->moteur, region->x_min,
			      region->y_min, NULL, NULL, NULL);
    must_ask = (height > 1);
   }
  }
 }
 if (This->abscisse && This->ordonnee)
  This->title = TRUE;
 else
  This->title = FALSE;
  
 return must_ask;
}

static void HG_recalcul_region(This)
c_HighGraph	*This;
{
 region_t	*region, *dregion, *aregion, *oregion;
 int		i;
  
 if (This->abscisse && This->ordonnee)
  This->title = TRUE;
 else
  This->title = FALSE;
  
 free_region(This->region_data);
 free_region(This->region_abscisse);
 free_region(This->region_ordonnee);
 This->region_data = This->region_abscisse = This->region_ordonnee = NULL;
  
 if (This->multiple_region)
 {
  if (This->region_direction)
  {				/* HORIZONTAL */
   region = This->orig_region;
   i = 0;
   while(region)
   {
    i += region->y_max - region->y_min + 1;
    region = region->NextRegion;
   }
   if (This->col_row)
   {				/* SERIE IN COLUMN */
    This->nb_abscisse = i - This->ordonnee;
	
    region = This->orig_region;
    if (This->ordonnee)
    {
     if (region->y_min != region->y_max)
     {
      This->region_data = dregion = dup_region(region);
      dregion->y_min += 1;
      if (This->abscisse)
      {
       This->region_abscisse = aregion = dup_region(region);
       aregion->y_min += 1;
      }
     }
     else
     {
      This->region_data = dregion = dup_region(region->NextRegion);
      if (This->abscisse)
       This->region_abscisse = aregion = dup_region(region->NextRegion);
     }
    }
    else
    {
     This->region_data = dregion = dup_region(region);
     if (This->abscisse)
      This->region_abscisse = aregion = dup_region(region);
    }
	
    if (This->abscisse)
    {
     while(dregion)
     {
      dregion->x_min += 1;
      dregion = dregion->NextRegion;
     }
     while(aregion)
     {
      aregion->x_max = aregion->x_min;
      aregion = aregion->NextRegion;
     }
    }
    This->nb_ordonnee = This->orig_region->x_max - 
     This->orig_region->x_min + (!This->abscisse);
    if (This->ordonnee)
    {
     This->region_ordonnee = 
      (region_t *)Xc_malloc("ord reg", sizeof(region_t));
     This->region_ordonnee->NextRegion = NULL;
	  
     This->region_ordonnee->y_min = This->region_ordonnee->y_max =
      This->orig_region->y_min;
     This->region_ordonnee->x_min = This->region_data->x_min;
     This->region_ordonnee->x_max = This->region_data->x_max;
    }
   }
   else
   {				/* SERIE IN ROW */
    This->nb_ordonnee = i - This->abscisse;
	
    region = This->orig_region;
    if (This->abscisse)
    {
     if (region->y_min != region->y_max)
     {
      This->region_data = dregion = dup_region(region);
      dregion->y_min += 1;
      if (This->ordonnee)
      {
       This->region_ordonnee = oregion = dup_region(region);
       oregion->y_min += 1;
      }
     }
     else
     {
      This->region_data = dregion = dup_region(region->NextRegion);
      if (This->ordonnee)
       This->region_ordonnee = oregion = dup_region(region->NextRegion);
     }
    }
    else
    {
     This->region_data = dregion = dup_region(region);
     if (This->ordonnee)
      This->region_ordonnee = oregion = dup_region(region);
    }
	
    if (This->ordonnee)
    {
     while(dregion)
     {
      dregion->x_min += 1;
      dregion = dregion->NextRegion;
     }
     while(oregion)
     {
      oregion->x_max = oregion->x_min;
      oregion = oregion->NextRegion;
     }
    }
    This->nb_abscisse = This->orig_region->x_max - 
     This->orig_region->x_min + (!This->ordonnee);
    if (This->abscisse)
    {
     This->region_abscisse = 
      (region_t *)Xc_malloc("abs reg", sizeof(region_t));
     This->region_abscisse->NextRegion = NULL;
	  
     This->region_abscisse->y_min = This->region_abscisse->y_max =
      This->orig_region->y_min;
     This->region_abscisse->x_min = This->region_data->x_min;
     This->region_abscisse->x_max = This->region_data->x_max;
    }	
   }
  }
  else
  {				/* VERTICAL */
   region = This->orig_region;
   i = 0;
   while(region)
   {
    i += region->x_max - region->x_min + 1;
    region = region->NextRegion;
   }
   if (This->col_row)
   {				/* SERIE IN COLUMN */
    This->nb_ordonnee = i - This->abscisse;
	
    region = This->orig_region;
    if (This->abscisse)
    {
     if (region->x_min != region->x_max)
     {
      This->region_data = dregion = dup_region(region);
      dregion->x_min += 1;
      if (This->ordonnee)
      {
       This->region_ordonnee = oregion = dup_region(region);
       oregion->x_min += 1;
      }
     }
     else
     {
      This->region_data = dregion = dup_region(region->NextRegion);
      if (This->ordonnee)
       This->region_ordonnee = oregion = dup_region(region->NextRegion);
     }
    }
    else
    {
     This->region_data = dregion = dup_region(region);
     if (This->ordonnee)
      This->region_ordonnee = oregion = dup_region(region);
    }
	
    if (This->ordonnee)
    {
     while(dregion)
     {
      dregion->y_min += 1;
      dregion = dregion->NextRegion;
     }
     while(oregion)
     {
      oregion->y_max = oregion->y_min;
      oregion = oregion->NextRegion;
     }
    }
    This->nb_abscisse = This->orig_region->y_max - 
     This->orig_region->y_min + (!This->ordonnee);
    if (This->abscisse)
    {
     This->region_abscisse = 
      (region_t *)Xc_malloc("abs reg", sizeof(region_t));
     This->region_abscisse->NextRegion = NULL;
	  
     This->region_abscisse->x_min = This->region_abscisse->x_max =
      This->orig_region->x_min;
     This->region_abscisse->y_min = This->region_data->y_min;
     This->region_abscisse->y_max = This->region_data->y_max;
    }	
   }
   else
   {				/* SERIE IN ROW */
    This->nb_abscisse = i - This->ordonnee;
	
    region = This->orig_region;
    if (This->ordonnee)
    {
     if (region->x_min != region->x_max)
     {
      This->region_data = dregion = dup_region(region);
      dregion->x_min += 1;
      if (This->abscisse)
      {
       This->region_abscisse = aregion = dup_region(region);
       aregion->x_min += 1;
      }
     }
     else
     {
      This->region_data = dregion = dup_region(region->NextRegion);
      if (This->abscisse)
       This->region_abscisse = aregion = dup_region(region->NextRegion);
     }
    }
    else
    {
     This->region_data = dregion = dup_region(region);
     if (This->abscisse)
      This->region_abscisse = aregion = dup_region(region);
    }
	
    if (This->abscisse)
    {
     while(dregion)
     {
      dregion->y_min += 1;
      dregion = dregion->NextRegion;
     }
     while(aregion)
     {
      aregion->y_max = aregion->y_min;
      aregion = aregion->NextRegion;
     }
    }
    This->nb_ordonnee = This->orig_region->y_max - 
     This->orig_region->y_min + (!This->abscisse);
    if (This->ordonnee)
    {
     This->region_ordonnee = 
      (region_t *)Xc_malloc("ord reg", sizeof(region_t));
     This->region_ordonnee->NextRegion = NULL;
	  
     This->region_ordonnee->x_min = This->region_ordonnee->x_max =
      This->orig_region->x_min;
     This->region_ordonnee->y_min = This->region_data->y_min;
     This->region_ordonnee->y_max = This->region_data->y_max;
    }
   }
  }
 }
 else
 {
  if (This->col_row)
  {
   This->nb_abscisse = This->orig_region->y_max - 
    This->orig_region->y_min + !This->ordonnee;
   This->nb_ordonnee = This->orig_region->x_max - 
    This->orig_region->x_min + (!This->abscisse);
      
   This->region_data = (region_t *)Xc_malloc("data reg", sizeof(region_t));
   This->region_data->NextRegion = NULL;
      
   This->region_data->x_min = This->orig_region->x_min + This->abscisse;
   This->region_data->y_min = This->orig_region->y_min + This->ordonnee;
   This->region_data->x_max = 
    This->region_data->x_min + This->nb_ordonnee - 1;
   This->region_data->y_max = 
    This->region_data->y_min + This->nb_abscisse - 1;
   if (This->abscisse)
   {
    This->region_abscisse = 
     (region_t *)Xc_malloc("abs reg", sizeof(region_t));
    This->region_abscisse->NextRegion = NULL;
	
    This->region_abscisse->x_min = This->region_abscisse->x_max = 
     This->orig_region->x_min;
    This->region_abscisse->y_min = This->region_data->y_min;
    This->region_abscisse->y_max = This->region_data->y_max;
   }
   if (This->ordonnee)
   {
    This->region_ordonnee = 
     (region_t *)Xc_malloc("ord reg", sizeof(region_t));
    This->region_ordonnee->NextRegion = NULL;
	
    This->region_ordonnee->y_min = This->region_ordonnee->y_max =
     This->orig_region->y_min;
    This->region_ordonnee->x_min = This->region_data->x_min;
    This->region_ordonnee->x_max = This->region_data->x_max;
   }
  }
  else
  {
   This->nb_abscisse = This->orig_region->x_max - 
    This->orig_region->x_min + !This->ordonnee;
   This->nb_ordonnee = This->orig_region->y_max - 
    This->orig_region->y_min + (!This->abscisse);
      
   This->region_data = (region_t *)Xc_malloc("data reg", sizeof(region_t));
   This->region_data->NextRegion = NULL;
      
   This->region_data->x_min = This->orig_region->x_min + This->ordonnee;
   This->region_data->y_min = This->orig_region->y_min + This->abscisse;
   This->region_data->x_max = 
    This->region_data->x_min + This->nb_abscisse - 1;
   This->region_data->y_max = 
    This->region_data->y_min + This->nb_ordonnee - 1;
   if (This->abscisse)
   {
    This->region_abscisse = 
     (region_t *)Xc_malloc("abs reg", sizeof(region_t));
    This->region_abscisse->NextRegion = NULL;
	
    This->region_abscisse->y_min = This->region_abscisse->y_max =
     This->orig_region->y_min;
    This->region_abscisse->x_min = This->region_data->x_min;
    This->region_abscisse->x_max = This->region_data->x_max;
   }
   if (This->ordonnee)
   {
    This->region_ordonnee = 
     (region_t *)Xc_malloc("ord reg", sizeof(region_t));
    This->region_ordonnee->NextRegion = NULL;
	
    This->region_ordonnee->x_min = This->region_ordonnee->x_max = 
     This->orig_region->x_min;
    This->region_ordonnee->y_min = This->region_data->y_min;
    This->region_ordonnee->y_max = This->region_data->y_max;
   }
  }
 }
}

static void HG_set_X_info(This, display, window)
c_HighGraph	*This;
Display		*display;
Window		window;
{
 GC	gc;
 XGCValues	val_gc;
  
 This->X_info.display = display;
 This->X_info.window = window;
 This->X_info.ecran = DefaultScreen(display);
  
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
}

static void HG_del_X_info(This)
c_HighGraph	*This;
{
 XFreeGC(This->X_info.display, This->X_info.gc_contour);
 XFreeGC(This->X_info.display, This->X_info.gc_inside);
}

static int HG_configure(This)
c_HighGraph	*This;
{
 c_Box_AskGraph *boxag;
 int		rc;
  
 rc = 0;
 SET_WAIT_MODE_STATIC;
 boxag = (c_Box_AskGraph *)NEW(c_Box_AskGraph)
  (MainInterface1->w_Main, "BoxAskGraph", This);
  
 switch(F(boxag->Dialog).waitForUnmap(boxag->Dialog))
 {
 case XcCD_DESTROY:
 case XcCD_CANCEL:
  F(boxag).cancel(boxag);
  HG_DISPLAY_REDRAW(This->HG_object)(This->HG_object, TRUE);
  DELETE(c_Box_AskGraph)(boxag);
  rc = -1;
  break;
 case XcCD_OK:
  HG_DISPLAY_REDRAW(This->HG_object)(This->HG_object, TRUE);
  DELETE(c_Box_AskGraph)(boxag);
  break;
 }
 UNSET_WAIT_MODE;
 return rc;
}

static void HG_set_unset_legend(This)
c_HighGraph	*This;
{
 This->d_legend ^= TRUE;
  
 HG_REGENERATE(This->HG_object)(This->HG_object);
 HG_DISPLAY_REDRAW(This->HG_object)(This->HG_object, TRUE);
}

static void HG_set_unset_title(This)
c_HighGraph	*This;
{
 This->d_title ^= TRUE;
  
 HG_REGENERATE(This->HG_object)(This->HG_object);
 HG_DISPLAY_REDRAW(This->HG_object)(This->HG_object, TRUE);
}

static void HG_set_unset_grad_y(This)
c_HighGraph	*This;
{
 This->graduation_y ^= TRUE;
  
 HG_REGENERATE(This->HG_object)(This->HG_object);
 HG_DISPLAY_REDRAW(This->HG_object)(This->HG_object, TRUE);
}

static void HG_set_unset_abscisse(This)
c_HighGraph	*This;
{
 This->d_abscisse ^= TRUE;
  
 HG_REGENERATE(This->HG_object)(This->HG_object);
 HG_DISPLAY_REDRAW(This->HG_object)(This->HG_object, TRUE);
}

static void HG_set_unset_ordonnee(This)
c_HighGraph	*This;
{
 This->d_ordonnee ^= TRUE;
  
 HG_REGENERATE(This->HG_object)(This->HG_object);
 HG_DISPLAY_REDRAW(This->HG_object)(This->HG_object, TRUE);
}

static void HG_change_graph_class(This, class)
c_HighGraph	*This;
HG_class_t	class;
{
 This->class = class;
  
 HG_REGENERATE(This->HG_object)(This->HG_object);
 HG_DISPLAY_REDRAW(This->HG_object)(This->HG_object, TRUE);
}

static void HG_test_tstyle_hook(This, tstyle)
c_HighGraph	*This;
c_TextStyle	*tstyle;
{
 int  i;
 boolean regenerate = FALSE;
  
 if (This->tab_data.tstyle == tstyle)
  regenerate = TRUE;
 else
 {
  if (This->tab_title.tstyle == tstyle)
   regenerate = TRUE;
  else
  {
   for(i=0; i<This->tab_nb_abscisse && !regenerate; i++)
   {
    if (This->tab_abscisse[i].tstyle == tstyle)
     regenerate = TRUE;
   }
   if (!regenerate)
   {
    for(i=0; i<This->tab_nb_ordonnee && !regenerate; i++)
    {
     if (This->tab_ordonnee[i].tstyle == tstyle)
      regenerate = TRUE;
    }
   }
  }
 }
  
 if (regenerate)
 {
  F(This).draw(This);
 }
}

static void HG_test_tstyle_apply(This, region)
c_HighGraph	*This;
region_t	*region;
{
 region_t	*region2;
 boolean	regenerate = FALSE;
  
 while(region && !regenerate)
 {
  region2 = This->orig_region;
  while(region2)
  {
   if (! (region->x_min > region2->x_max || 
	  region->x_max < region2->x_min ||
	  region->y_min > region2->y_max || 
	  region->y_max < region2->y_min) )
   {
    regenerate = TRUE;
    break;
   }
   region2 = region2->NextRegion;
  }
  region = region->NextRegion;
 }
  
 if (regenerate)
  F(This).draw(This);
}

static region_t	*dup_and_check_region(This, region)
c_HighGraph	*This;
region_t	*region;
{
 region_t	*dregion;
 region_t	*pregion, *ppregion;
 boolean	direction_found = FALSE;
 int		width, height;
  
 dregion = (region_t *)Xc_malloc("hgregion", sizeof(region_t));
 memcpy(dregion, region, sizeof(region_t));
 dregion->NextRegion = NULL;
 if (!F(This->moteur).get_filled_region(This->moteur, dregion))
 {
  Xc_free(dregion);
  return NULL;
 }
 This->region_direction = This->multiple_region = FALSE;
  
 pregion = dregion;
 region = region->NextRegion;
 while(region)
 {
  ppregion = pregion;
  pregion->NextRegion = 
   (region_t *)Xc_malloc("hgregion", sizeof(region_t));
  pregion = pregion->NextRegion;
  memcpy(pregion, region, sizeof(region_t));
    
  if (!F(This->moteur).get_filled_region(This->moteur, pregion))
  {
   Xc_free(pregion);
   pregion = ppregion;
  }
  else
  {
   if (!direction_found)
   {
    if (pregion->x_min == dregion->x_min &&
	pregion->x_max == pregion->x_max)
    {
     This->region_direction = TRUE;
     This->multiple_region = TRUE;
     direction_found = TRUE;
    }
    else
    {
     if (pregion->y_min == dregion->y_min &&
	 pregion->y_max == pregion->y_max)
     {
      This->region_direction = FALSE;
      This->multiple_region = TRUE;
      direction_found = TRUE;
     }
     else
     {
      Xc_free(pregion);
      pregion = ppregion;
     }
    }
   }
   else
   {
    if (This->region_direction)
    {
     if (pregion->x_min != dregion->x_min ||
	 pregion->x_max != pregion->x_max)
     {
      Xc_free(pregion);
      pregion = ppregion;
     }
    }
    else
    {
     if (pregion->y_min != dregion->y_min ||
	 pregion->y_max != pregion->y_max)
     {
      Xc_free(pregion);
      pregion = ppregion;
     }
    }
   }
  }
  pregion->NextRegion = NULL;
  region = region->NextRegion;
 }
  
 if (dregion->y_max - dregion->y_min + 1 > 2000)
 {
  pregion = dregion; width = 0;
  while(pregion)
  {
   width += pregion->x_max - pregion->x_min + 1;
   pregion = pregion->NextRegion;
  }
  height = 2000 + ((width < 125) * ((125 - width) * 40));
  if (dregion->y_max - dregion->y_min + 1 > height)
  {
   pregion = dregion;
   while(pregion)
   {
    pregion->y_max = pregion->y_min + height - 1;
    pregion = pregion->NextRegion;
   }
  }
 }
  
 return dregion;
}


static region_t	*dup_region(region)
region_t *region;
{
 region_t	*dregion;
 region_t	*pregion;
  
 pregion = dregion = NULL;
 while(region)
 {
  if (pregion)
  {
   pregion->NextRegion = 
    (region_t *)Xc_malloc("hgregion", sizeof(region_t));
   pregion = pregion->NextRegion;
  }
  else
  {
   dregion = pregion = 
    (region_t *)Xc_malloc("hgregion", sizeof(region_t));
  }
    
  memcpy(pregion, region, sizeof(region_t));
  pregion->NextRegion = NULL;
  region = region->NextRegion;
 }
 return dregion;
}

static void free_region(region)
region_t *region;
{
 region_t	*pregion;
  
 while(region)
 {
  pregion = region->NextRegion;
  Xc_free(region);
  region = pregion;
 }
}
