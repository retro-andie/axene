/*
** moteur.c for XQuad in Moteur/
** Object and methods for Moteur
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
** Started on  Fri Dec 16 15:12:00 1994 Antoine Buat
** Last update Thu Sep 11 20:53:22 1997 Robin Castanier
*/

/*TODO  correct inversion of x and y in zone_dep */
/*TODO  correct inversion of x and y in add_cell */
/*TODO test if exist zone_dep in add_depend_zone */
/*TODO should redraw depend, cas a traiter */
/*
ADD_DEP n'ajoute que si la cellule n'est pas présente dans la chaine.
*/

#include "MoteurP.h"
#include "Feuille.h"
#include "Parse_token.h"

static void *cons_Moteur();
static void dest_Moteur();
static void *copy_Moteur();

void store_refresh();
void pop_refresh();

void store_depend();
void pop_depend();
void attach_depend();

static cell *Moteur_add_cell();
static void Moteur_del_cell();

static boolean Moteur_get_val();
static cell *Moteur_get_cell();

static coord_t Moteur_get_size();
static void Moteur_set_size();
static void Moteur_set_size_zone ___PROTO((c_Moteur *This, int type,
				    region_t *region, coord_t value));
static void Moteur_insert_size_zone ___PROTO((c_Moteur *This, int type, 
					      int z_start, int z_end));
static void Moteur_delete_size_zone ___PROTO((c_Moteur *This, int type, 
					      int z_start, int z_end));

static int Moteur_get_string();
static void exec_depend();

static void store_exec();
static void exec_stored();
static void exec_depend2();

static void Moteur_set_string();
static void Moteur_deffered_set_string();

static void Moteur_add_depend();
static void Moteur_sub_depend();
static void Moteur_add_graph_dep();
static void Moteur_sub_graph_dep();
static void Moteur_add_depend_zone();
static void Moteur_sub_depend_zone();
static boolean Moteur_check_c_depend();
static boolean Moteur_check_c_depend_cell();

static void frag_style();
static void view_style();
static void Moteur_set_cell_style2();
static CellStyle_t *Moteur_get_cell_style();
static void Moteur_or_cell_style();
static int Moteur_move_style();
static int Moteur_copy_style();

static void translate();
static int Moteur_move_zone();
static int Moteur_copy_zone();
static void Moteur_cut_zone();
static region_t *Moteur_paste_zone();
static void Moteur_delete_zone();
static void copy_zone_r_or_d();
static void Moteur_extend_zone();

static void Moteur_move_cell();

static void Moteur_insere_zone_col();
static void Moteur_insere_zone_row();
static void Moteur_delete_zone_col();
static void Moteur_delete_zone_row();

static coord_t Moteur_calcul_page_origin ___PROTO((c_Moteur *This, 
						   int type, int pos, 
						   coord_t page_size));

static coord_t Moteur_get_ideal_column_width();
static coord_t Moteur_get_ideal_row_height();

static boolean get_filled_region();

sf_Moteur fc_Moteur =
{
  cons_Moteur,
  dest_Moteur,
  copy_Moteur,
  Moteur_add_cell,
  Moteur_del_cell,
  Moteur_get_val,
  Moteur_get_cell,
  
  Moteur_get_size,
  Moteur_set_size,
  Moteur_set_size_zone,
  
  Moteur_get_string,
  Moteur_set_string,
  
  Moteur_add_depend,
  Moteur_sub_depend,
  Moteur_add_graph_dep,
  Moteur_sub_graph_dep,
  Moteur_add_depend_zone,
  Moteur_sub_depend_zone,
  Moteur_check_c_depend,
  Moteur_check_c_depend_cell,
  
  Moteur_set_cell_style2,
  Moteur_get_cell_style,
  Moteur_or_cell_style,
  Moteur_move_style,
  Moteur_copy_style,
  
  Moteur_move_zone,
  Moteur_copy_zone,
  Moteur_cut_zone,
  Moteur_paste_zone,
  Moteur_delete_zone,
  copy_zone_r_or_d,
  Moteur_extend_zone,
  
  Moteur_insere_zone_col,
  Moteur_insere_zone_row,
  Moteur_delete_zone_col,
  Moteur_delete_zone_row,
  
  Moteur_calcul_page_origin,
  
  moteurTestOverrun,
  moteurComputeOverrun,
  moteurGetOverrunRegion,
  moteurGetOverrunCells,
  moteurFreeOverrunCells,

  Moteur_get_ideal_column_width, 
  Moteur_get_ideal_row_height,

  get_filled_region,

  readMoteur,
  writeMoteur,

  printMoteur,
  getMoteurBoundingRegion,
  getMoteurBoundingBox,
  getMoteurCellByXY,
  getMoteurCellStyleList,

  Moteur_insert_size_zone,
  Moteur_delete_size_zone,
  moteurTestCellOverrun
};

/*--- null cell ---*/
cell		voidcell = 
{
  Xq_VOID,
  NULL,
  NULL,
  0,
  0,
  NULL,
  NULL,
  NULL,
  0,
  0,
  FALSE,
  NULL,
  0,
};

int ptom_convert_type[] = { 
  Xq_VOID, Xq_TEXT, Xq_VALUE, Xq_VALUE, Xq_VALUE, Xq_VALUE, Xq_VALUE,
  Xq_VOID, Xq_VOID, Xq_VOID, Xq_VOID, Xq_VOID, Xq_VOID, Xq_VOID,
  Xq_PERCENT, Xq_UNIT, Xq_FRACTION, Xq_BOOLEAN, Xq_DAY_IN_WEEK, Xq_MONTH,
  Xq_TIME, Xq_DATE, Xq_DATE_TIME, Xq_EXPONANTIAL, Xq_FORMERR,
  Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, 
  Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, 
  Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, 
  Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR,
  Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR,
  Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR,
  Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR,
  Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR, Xq_FORMERR,
};

static int graph_value_type[] = 
{
  FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, TRUE
};

/* ----------------------------------------------------------------- ** 
** constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *cons_Moteur(base)
BaseStd_t	*base;
{
  c_Moteur	*ObjTmp;
  
  if ((ObjTmp = (c_Moteur *)Xc_malloc("Moteur",
				      sizeof(c_Moteur))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_Moteur;
  ObjTmp->tableCL = NULL;
  ObjTmp->parser = (c_Parser *)NEW(c_Parser)(ObjTmp);

  ObjTmp->style_list = (cell_style_list *)Xc_malloc
    ("list_style", sizeof(cell_style_list));
  ObjTmp->style_list->cstyle = base->cell_style_base->default_cstyle;
  ObjTmp->style_list->region.x_min = 0;
  ObjTmp->style_list->region.x_max = COLUMN_MAX;
  ObjTmp->style_list->region.y_min = 0;
  ObjTmp->style_list->region.y_max = ROW_MAX;
  ObjTmp->style_list->Next = NULL;
  ObjTmp->BaseStd = base;
  ObjTmp->id_func = 0;

  ObjTmp->sel_region.x_max = 0;
  ObjTmp->sel_region.x_min = 0;
  ObjTmp->sel_region.y_max = 0;
  ObjTmp->sel_region.y_min = 0;
  ObjTmp->sel_region.NextRegion = NULL;
  ObjTmp->mode = Xc_MODE_NOTHING;

  ObjTmp->force = 0;

  ObjTmp->col_size = (cell_size_list *)Xc_malloc
    ("col_size", sizeof(cell_size_list));
  ObjTmp->col_size->Amin = 0;
  ObjTmp->col_size->Amax = COLUMN_MAX;
  ObjTmp->col_size->size = DEFAULT_COL_SIZE;
  ObjTmp->col_size->Next = NULL;
  ObjTmp->line_size = (cell_size_list *)Xc_malloc
    ("line_size", sizeof(cell_size_list));
  ObjTmp->line_size->Amin = 0;
  ObjTmp->line_size->Amax = ROW_MAX;
  ObjTmp->line_size->size = DEFAULT_ROW_SIZE;
  ObjTmp->line_size->Next = NULL;

  ObjTmp->zone_dep = NULL;
  Xc_HISTORY(("Object Moteur build"));
  return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
static void dest_Moteur(This)
c_Moteur	*This;
{
  cell_style_list	*l_tmp;
  cell_style_list	*l_tmp2;

  c_col			*cc_tmp;
  c_col			*cc_tmp2;

  c_line		*cl_tmp;
  c_line		*cl_tmp2;

  cell_size_list	*listsize;
  cell_size_list	*listsize2;

  zone_depend		*zonedep;
  zone_depend		*zonedep2;

  cc_tmp = This->tableCL;
  while (cc_tmp)
    {
      cc_tmp2 = cc_tmp->Next;
      cl_tmp = cc_tmp->line;
      while (cl_tmp)
	{
	  cell_depend	*cdep;
	  cell_depend	*cdep2;
	  HG_list	*gdep;
	  HG_list	*gdep2;

	  cl_tmp2 = cl_tmp->Next;
	  if (cl_tmp->cell.text)
	    Xc_free(cl_tmp->cell.text);
	  if (cl_tmp->cell.formula)
	    Xc_free(cl_tmp->cell.formula);
	  if (cl_tmp->cell.value2)
	    Xc_free(cl_tmp->cell.value2);

	  cdep = cl_tmp->cell.depend;
	  while (cdep)
	    {
	      cdep2 = cdep->Next;
	      Xc_free(cdep);
	      cdep = cdep2;
	    }
	  gdep = cl_tmp->cell.graph_list;
	  while (gdep)
	    {
	      gdep2 = gdep->Next;
	      Xc_free(gdep);
	      gdep = gdep2;
	    }
	  Xc_free(cl_tmp);
	  cl_tmp = cl_tmp2;
	}
      Xc_free(cc_tmp);
      cc_tmp = cc_tmp2;
    }

  l_tmp = This->style_list;
  while (l_tmp)
    {
      l_tmp2 = l_tmp->Next;
      Xc_free(l_tmp);
      l_tmp = l_tmp2;
    }

  listsize = This->col_size;
  while (listsize)
    {
      listsize2 = listsize->Next;
      Xc_free(listsize);
      listsize = listsize2;
    }

  listsize = This->line_size;
  while (listsize)
    {
      listsize2 = listsize->Next;
      Xc_free(listsize);
      listsize = listsize2;

    }

  zonedep = This->zone_dep;
  while (zonedep)
    {
      zonedep2 = zonedep->Next;
      Xc_free(zonedep);
      zonedep = zonedep2;
    }

  DELETE(c_Parser)(This->parser);
  Xc_free(This);
  Xc_HISTORY(("Object Moteur destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy_Moteur(This)
c_Moteur *This;
{
  Xc_HISTORY(("Object Moteur copy not implemented"));
  return NULL;
}

/* ----------------------------------------------------------------- ** 
** store_refresh - store refresh in list (internal)                  ** 
** ----------------------------------------------------------------- */
void store_refresh(ref_list, line, column)
refresh_list	**ref_list;
int		line;
int		column;
{
  refresh_list	**tmp_list;

  tmp_list = ref_list;
  while (*tmp_list && 
	 ((*tmp_list)->line != line) &&
	 ((*tmp_list)->column != column))
    {
      tmp_list = &((*tmp_list)->Next);
    }

  if (*tmp_list)
    {
      return;
    }

  *tmp_list = (refresh_list *)Xc_malloc
    ("refresh list", sizeof(refresh_list));
  (*tmp_list)->line = line;
  (*tmp_list)->column = column;
  (*tmp_list)->Next = NULL;
}

/* ----------------------------------------------------------------- ** 
** pop_refresh - pop refresh stored in list except in zone(internal) ** 
** ----------------------------------------------------------------- */
void pop_refresh(ref_list, x_min, y_min, x_max, y_max)
refresh_list	*ref_list;
int		x_min;
int		y_min;
int		x_max;
int		y_max;
{
  refresh_list	*tmp_list;

  tmp_list = ref_list;
  while (tmp_list)
    {
      if ((tmp_list->column < x_min) || (tmp_list->column > x_max) ||
	  (tmp_list->line < y_min) || (tmp_list->line > y_max))
	{
	  F(GlobEditBar->sel_feuille).RefreshCellule(GlobEditBar->sel_feuille,
						     tmp_list->column,
						     tmp_list->line);
	}
      ref_list = tmp_list;
      tmp_list = tmp_list->Next;
      Xc_free(ref_list);
    }
}

/* ----------------------------------------------------------------- ** 
** store_depend - store depend in list (internal)                    ** 
** ----------------------------------------------------------------- */
void store_depend(cell_d, all_dep, graph_d, all_gdep)
cell_depend	*cell_d;
cell_depend	**all_dep;
HG_list		*graph_d;
HG_list		**all_gdep;
{
  HG_list	**gdep;
  cell_depend	**dep;

  while (cell_d)
    {
      dep = all_dep;
      while (*dep && ((*dep)->cell_p != cell_d->cell_p))
	{
	  dep = &((*dep)->Next);
	}
      if (*dep)
	{
	  if ((*dep)->cell_p == cell_d->cell_p)
	    {
	      cell_d = cell_d->Next;
	      continue;
	    }
	}
      *dep = (cell_depend *)Xc_malloc
	("celldep", sizeof(cell_depend));
      (*dep)->line = cell_d->line;
      (*dep)->column = cell_d->column;
      (*dep)->cell_p = cell_d->cell_p;
      (*dep)->Next = NULL;
      cell_d = cell_d->Next;
    }
  while (graph_d)
    {
      gdep = all_gdep;
      while (*gdep && ((*gdep)->Graph != graph_d->Graph))
	{
	  gdep = &((*gdep)->Next);
	}
      if (*gdep)
	{
	  if ((*gdep)->Graph == graph_d->Graph)
	    {
	      graph_d = graph_d->Next;
	      continue;
	    }
	}
      *gdep = (HG_list *)Xc_malloc
	("celldep", sizeof(HG_list));
      (*gdep)->Graph = graph_d->Graph;
      (*gdep)->Next = NULL;
      graph_d = graph_d->Next;
    }
}


/* ----------------------------------------------------------------- ** 
** pop_depend - pop depend stored in list (internal)                 ** 
** ----------------------------------------------------------------- */
void pop_depend(This, all_dep, all_gdep)
c_Moteur	*This;
cell_depend	*all_dep;
HG_list		**all_gdep;
{
  HG_list		*graph_d;
  cell_depend_list	*list_d;
  int			depth;
  cell_depend		**start_d;
  cell_depend		*tmp_dep;
  cell_depend		*r_list;
  cell_depend		*tmp_list;
  cell			*tmpcell;

  /* expunge list */
  r_list = NULL;
  start_d = &all_dep;
  while(*start_d)
    {
      tmpcell = F(This).get_cell(This, (*start_d)->column, (*start_d)->line);
      if (((*start_d)->cell_p != tmpcell) || (!(*start_d)->cell_p->formula))
	{
	  tmp_dep = *start_d;
	  *start_d = (*start_d)->Next;
	  Xc_free(tmp_dep);
	}
      else
	{
	  if (!r_list)
	    r_list = *start_d;
	  start_d = &((*start_d)->Next);
	}
    }

  list_d = NULL;
  depth = 0;
  store_exec(r_list, &list_d, &list_d, depth);
  exec_stored(This, list_d, GlobEditBar->sel_feuille, all_gdep);

  while(r_list)
    {
      tmp_list = r_list;
      r_list = r_list->Next;
      Xc_free(tmp_list);
    }
  
  graph_d = *all_gdep;
  while (graph_d)
    {
    F(graph_d->Graph).draw(graph_d->Graph);
    graph_d = graph_d->Next;
  }

}

/* ----------------------------------------------------------------- ** 
** attach_depend - look for depend in depend zone list (internal)    ** 
** ----------------------------------------------------------------- */
void attach_depend(This, c_dep, col_num, line_num)
c_Moteur	*This;
cell_depend	**c_dep;
int		col_num;
int		line_num;
{
  zone_depend	*zone_dep;

  zone_dep = This->zone_dep;
  while (zone_dep)
    {
      if ((col_num >= zone_dep->region.y_min) &&
	  (col_num <= zone_dep->region.y_max) &&
	  (line_num >= zone_dep->region.x_min) &&
	  (line_num <= zone_dep->region.x_max))
	{
	  *c_dep = Xc_malloc("full_dep", sizeof(cell_depend));
	  (*c_dep)->line = zone_dep->line;
	  (*c_dep)->column = zone_dep->column;
	  (*c_dep)->cell_p = zone_dep->cell_p;
	  (*c_dep)->Next = NULL;
	  c_dep = &((*c_dep)->Next);
	}
      zone_dep = zone_dep->Next;
    }
}

/* ----------------------------------------------------------------- ** 
** Moteur_add_cell - add a cell                                      ** 
** ----------------------------------------------------------------- */
static cell *Moteur_add_cell(This, line, col)
c_Moteur	*This;
int		line;
int		col;
{
  c_col		**tmpccol;
  c_col		*oldccol;
  c_col		*newccol;
  c_col		*prevcol;
  c_line	**tmpcline;
  c_line	*oldcline;
  c_line	*newcline;
  c_line	*prevcline;
  zone_depend	*zone_dep;
  cell_depend	**c_dep;

  tmpccol = &This->tableCL;
  if (*tmpccol == NULL)
    {
      if ((*tmpccol = (c_col *)Xc_malloc("cell Graph",
					 sizeof (c_col))) == NULL)
	{
	  Xc_FATAL(("error can't alloc memory"));
	}
      (*tmpccol)->number = col;
      (*tmpccol)->line = NULL;
      (*tmpccol)->Next = NULL;
      (*tmpccol)->Prev = NULL;
      (*tmpccol)->last_pointed = NULL;
      (*tmpccol)->last_added = &((*tmpccol)->line);
    }
  else
    {
      prevcol = NULL;
      while (*tmpccol && ((*tmpccol)->number < col))
	{
	  prevcol = *tmpccol;
	  tmpccol = &((*tmpccol)->Next);
	}

      if (!*tmpccol || ((*tmpccol)->number != col))
	{
	  oldccol = *tmpccol;
	  if ((newccol = (c_col *)Xc_malloc("ccol",
					    sizeof (c_col))) == NULL)
	    {
	      Xc_FATAL(("Can't alloc memory for column"));
	    }
	  newccol->number = col;
	  newccol->line = NULL;
	  newccol->Next = oldccol;
	  newccol->last_pointed = NULL;
	  newccol->last_added = &(newccol->line);
	  if (oldccol)
	    {
	      newccol->Prev = oldccol->Prev;
 	      oldccol->Prev = newccol;
	    }
	  else
	    newccol->Prev = prevcol;
	  *tmpccol = newccol;
	}
    }

  /* at this point the column is found *
   *   and placed int *tmpccol         *
   * just have to found line place    */

  tmpcline = &(*tmpccol)->line;
  if (*tmpcline == NULL)
    {
      if ((*tmpcline = (c_line *)Xc_malloc("cline",
					   sizeof (c_line))) == NULL)
	{
	  Xc_FATAL(("error can't alloc memory"));
	}
      (*tmpcline)->number = line;
      (*tmpcline)->cell.type = Xq_VOID;
      (*tmpcline)->cell.text = NULL;
      (*tmpcline)->cell.formula = NULL;
      (*tmpcline)->cell.ret_type = Xq_VOID;
      (*tmpcline)->cell.status = TYPE_NONE;
      (*tmpcline)->cell.value2 = NULL;
      (*tmpcline)->cell.depend = NULL;
      c_dep = &((*tmpcline)->cell.depend);
      zone_dep = This->zone_dep;
      while (zone_dep)
	{
	  if ((line >= zone_dep->region.x_min) &&
	      (line <= zone_dep->region.x_max) &&
	      (col >= zone_dep->region.y_min) &&
	      (col <= zone_dep->region.y_max))
	    {
	      *c_dep = Xc_malloc("full_dep", sizeof(cell_depend));
	      (*c_dep)->line = zone_dep->line;
	      (*c_dep)->column = zone_dep->column;
	      (*c_dep)->cell_p = zone_dep->cell_p;
	      (*c_dep)->Next = NULL;
	      c_dep = &((*c_dep)->Next);
	    }
	  zone_dep = zone_dep->Next;
	}
      (*tmpcline)->cell.graph_list = NULL;

      (*tmpcline)->cell.overrun_pos = 0;
      (*tmpcline)->cell.overrun_neg = 0;
      (*tmpcline)->cell.overrun_direction = 0;
      (*tmpcline)->cell.overrun_cstyle = NULL;
      (*tmpcline)->cell.overrun_size = 0;

      (*tmpcline)->Next = NULL;
      (*tmpcline)->Prev = NULL;
      (*tmpccol)->last_added = NULL;
    }
  else
    {
      tmpcline = (*tmpccol)->last_added;
      if (tmpcline)
	{
	  if (*tmpcline)
	    {
	      if (abs(line - (*tmpcline)->number) >
		  abs(line - (*tmpccol)->line->number))
		tmpcline = &(*tmpccol)->line;
	    }
	  else
	    tmpcline = &(*tmpccol)->line;
	}
      else
	tmpcline = &(*tmpccol)->line;
      
      prevcline = (*tmpcline)->Prev;
      if ((*tmpcline)->number > line)
	{
	  while ((*tmpcline)->Prev && ((*tmpcline)->number > line))
	    {
	      if ((*tmpcline)->Prev->Prev)
		tmpcline = &((*tmpcline)->Prev->Prev->Next);
	      else
		tmpcline = &(*tmpccol)->line;
	    }
	  if (*tmpcline)
	    prevcline = (*tmpcline)->Prev;
	}
      while ((*tmpcline) && ((*tmpcline)->number < line))
	{
	  prevcline = *tmpcline;
	  tmpcline = &((*tmpcline)->Next);
	}
      
      if (!*tmpcline || ((*tmpcline)->number != line))
	{
	  oldcline = *tmpcline;
	  if ((newcline = (c_line *)Xc_malloc("cline",
					      sizeof (c_line))) == NULL)
	    {
	      Xc_FATAL(("error can't alloc memory"));
	    }
	  newcline->number = line;
	  newcline->cell.type = Xq_VOID;
	  newcline->cell.text = NULL;
	  newcline->cell.formula = NULL;
	  newcline->cell.ret_type = Xq_VOID;
	  newcline->cell.status = TYPE_NONE;
	  newcline->cell.value2 = NULL;
	  newcline->cell.depend = NULL;
	  c_dep = &(newcline->cell.depend);
	  zone_dep = This->zone_dep;
	  while (zone_dep)
	    {
	      if ((line >= zone_dep->region.x_min) &&
		  (line <= zone_dep->region.x_max) &&
		  (col >= zone_dep->region.y_min) &&
		  (col <= zone_dep->region.y_max))
		{
		  *c_dep = Xc_malloc("full_dep", sizeof(cell_depend));
		  (*c_dep)->line = zone_dep->line;
		  (*c_dep)->column = zone_dep->column;
		  (*c_dep)->cell_p = zone_dep->cell_p;
		  (*c_dep)->Next = NULL;
		  c_dep = &((*c_dep)->Next);
		}
	      zone_dep = zone_dep->Next;
	    }
	  newcline->cell.graph_list = NULL;

	  newcline->cell.overrun_pos = 0;
	  newcline->cell.overrun_neg = 0;
	  newcline->cell.overrun_direction = 0;
	  newcline->cell.overrun_cstyle = NULL;
	  newcline->cell.overrun_size = 0;

	  newcline->Next = oldcline;
	  if (oldcline)
	    {
	      newcline->Prev = prevcline;
	      oldcline->Prev = newcline;
	    }
	  else
	    newcline->Prev = prevcline;
	  *tmpcline = newcline;
	  (*tmpccol)->last_added = tmpcline;
	}
    }
  return &((*tmpcline)->cell);
}

/* ----------------------------------------------------------------- ** 
** Moteur_del_cell - delete cell                                     ** 
** ----------------------------------------------------------------- */
static void Moteur_del_cell(This, line, col)
c_Moteur	*This;
int		line;
int		col;
{
  c_col		*tmpcol;
  c_line	**tmplines;
  c_line	*tmpline;
  c_line	*tmpline2;
  c_line	*oldline;
  
  tmpcol = This->tableCL;
  while (tmpcol && (tmpcol->number < col))
    {
      tmpcol = tmpcol->Next;
    }
  if (!tmpcol)
    return;

  if (tmpcol->number == col)
    {
      tmplines = &(tmpcol->line);
      oldline = NULL;
      while (*tmplines && ((*tmplines)->number < line))
	{
	  oldline = *tmplines;
	  tmplines = &(*tmplines)->Next;
	}
      tmpline = *tmplines;

      if (tmpline && (tmpline->number == line))
	{
	  tmpcol->last_pointed = NULL;
	  tmpcol->last_added = NULL;
	  tmpline2 = tmpline->Next;
	  if (tmpline->cell.formula)
	    {
	      F(This->parser).parse(This->parser, tmpline->cell.formula+1,
				    tmpcol->number, 
				    tmpline->number,
				    &(tmpline->cell), SUB_DEP, 
				    GlobEditBar->sel_feuille);
	      Xc_free(tmpline->cell.formula);
	      tmpline->cell.formula = NULL;
	    }
	  if (tmpline->cell.text)
	    {
	      Xc_free(tmpline->cell.text);
	      tmpline->cell.text = NULL;
	    }
	  if (tmpline->cell.value2)
	    {
	      Xc_free(tmpline->cell.value2);
	      tmpline->cell.value2 = NULL;
	    }
	  if (tmpline->cell.depend || tmpline->cell.graph_list)
	    {
	      tmpline->cell.type = Xq_VOID;
	      /* should redraw depend */
	      return;
	    }
	  Xc_free(tmpline);
	  tmpline = tmpline2;
	  if (tmpline)
	    tmpline->Prev = oldline;
	  *tmplines = tmpline;
	}
    }
}

/* ----------------------------------------------------------------- **
** Moteur_get_val - get val in real and string converted format      **
** ----------------------------------------------------------------- */
static boolean Moteur_get_val(This, x, y, preal, string, cellstyle)
c_Moteur	*This;
int		x;
int		y;
real		*preal;
char		**string;
CellStyle_t	**cellstyle;
{
  c_col		*tmpccol;
  c_line	*tmpcline;
  CellStyle_t	*cstyle;
  
  tmpccol = This->tableCL;
  if (string)
    *string = NULL;
  if (preal)
    *preal = 0.0;

  cstyle = NULL;
  if (string || cellstyle)
  {
    cstyle = F(This).get_cell_style(This, x ,y);
    if (cellstyle)
      *cellstyle = cstyle;
  }
  
  while (tmpccol && (tmpccol->number < x))
    tmpccol = tmpccol->Next;
  if (!tmpccol || (tmpccol->number != x))
    return FALSE;

  if (tmpccol->last_pointed)
    if (abs(y - tmpccol->last_pointed->number) < 
	abs(y - tmpccol->line->number))
      tmpcline = tmpccol->last_pointed;
    else
      tmpcline = tmpccol->line;
  else
    tmpcline = tmpccol->line;
  
  if (tmpcline)
    {
      if (tmpcline->number > y)
	{
	  while (tmpcline && (tmpcline->number > y))
	    tmpcline = tmpcline->Prev;
	}
      else
	{
	  while (tmpcline && (tmpcline->number < y))
	    tmpcline = tmpcline->Next;
	}
    }

  if (!tmpcline || (tmpcline->number != y))
    return FALSE;

  tmpccol->last_pointed = tmpcline;

  switch (tmpcline->cell.type)
    {
    case Xq_TEXT:
      if (tmpcline->cell.text)
	{
	  if (string)
	    {
	      *string = (char *)Xc_malloc("str_text", 
					  strlen(tmpcline->cell.text)+1);
	      strcpy(*string, tmpcline->cell.text);
	    }
	  return FALSE;
	}
      break;
    case Xq_FORMULA:
      if (!tmpcline->cell.value2 && tmpcline->cell.text)
	{
	  if (string)
	    {
	      *string = (char *)Xc_malloc("str_text", 
					  strlen(tmpcline->cell.text)+1);
	      strcpy(*string, tmpcline->cell.text);
	    }
	  return FALSE;
	}
    case Xq_VALUE:
      if (tmpcline->cell.value2)
	{
	  char		*tmp_str;
	  int		slen;
	  
	  if (preal)
	    *preal = (real)gtodouble(tmpcline->cell.value2);
	  if (string)
	    {
	      *string = (char *)Xc_malloc("get_form_str", Xq_FORMULA_LENGTH);
	      tmp_str = g2str(tmpcline->cell.value2, BIG_PREC);
	      slen = 0;
	      while (tmp_str[slen] && tmp_str[slen]!='.')
		slen++;
	      if (tmp_str[slen])
		{
		  slen = strlen(tmp_str)-1;
		  while (tmp_str[slen] == '0')
		    slen--;
		  if (tmp_str[slen]=='.')
		    slen--;
		  tmp_str[slen+1]=0;
		}
	      strcpy(*string, tmp_str);
	      F(cstyle->format).convert_string
		(cstyle->format, *string,
		 ptom_convert_type[(int)tmpcline->cell.ret_type], NULL);
	      *string = Xc_realloc("rea_form_str", *string, strlen(*string)+1);
	    }
	  return graph_value_type[ptom_convert_type[(int)tmpcline->cell.ret_type]];
	}
      break;
    }
  return FALSE;
}

/* ----------------------------------------------------------------- ** 
** get_cell - get a pointer on cell                                  ** 
** ----------------------------------------------------------------- */
static cell *Moteur_get_cell(This, col, line)
c_Moteur	*This;
int		col;
int		line;
{
  c_col		*tmpccol;
  c_line	*tmpcline;

  tmpccol = This->tableCL;
  while (tmpccol && (tmpccol->number < col))
    tmpccol = tmpccol->Next;
  if (!tmpccol || (tmpccol->number != col) || !tmpccol->line)
    return &voidcell;

  if (tmpccol->last_pointed)
    if (abs(line - tmpccol->last_pointed->number) < 
	abs(line - tmpccol->line->number))
      tmpcline = tmpccol->last_pointed;
    else
      tmpcline = tmpccol->line;
  else
    tmpcline = tmpccol->line;

  if (tmpcline)
    {
      if (tmpcline->number > line)
	{
	  while (tmpcline && (tmpcline->number > line))
	    tmpcline = tmpcline->Prev;
	}
      else
	{
	  while (tmpcline && (tmpcline->number < line))
	    tmpcline = tmpcline->Next;
	}
    }

  if (tmpcline && (tmpcline->number == line))
    {
      tmpccol->last_pointed = tmpcline;
	return &(tmpcline->cell);
    }
  return &voidcell;
}

/* ----------------------------------------------------------------- ** 
** get_size - get size about line or column                          ** 
** ----------------------------------------------------------------- */
static coord_t Moteur_get_size(This, type, num)
c_Moteur	*This;
int		type;
int		num;
{
  cell_size_list	*listsize;

  if (type == Xc_LINE)
    listsize = This->line_size;
  else
    listsize = This->col_size;

  while (listsize && (listsize->Amax < num))
    listsize = listsize->Next;

  if (listsize)
    return listsize->size;
  return 0;
}

/* ----------------------------------------------------------------- ** 
** set_size - set size on line or column                             ** 
** ----------------------------------------------------------------- */
static void Moteur_set_size(This, type, num, value)
c_Moteur	*This;
int		type;
int		num;
coord_t		value;
{
  region_t	reg;
  
  reg.x_max = num;
  reg.y_max = num;
  reg.x_min = num;
  reg.y_min = num;
  F(This).set_size_zone(This, type, &reg, value);
}

/* ----------------------------------------------------------------- ** 
** set_size_zone - set size of lines or columns zones                ** 
** ----------------------------------------------------------------- */
static void Moteur_set_size_zone(This, type, region, value)
c_Moteur	*This;
int		type;
region_t	*region;
coord_t		value;
{
  cell_size_list	**listsize;
  cell_size_list	*nextsize;
  cell_size_list	*newsize;
  int			up;
  int			down;

  Xc_HISTORY(("set_size_zone(%g)", SCALE_TO_POINTS(value)));

  if (type == Xc_LINE)
    {
      listsize = &(This->line_size);
      down = region->y_min;
      up = region->y_max;
      Xc_TRACE(("row[%d; %d]", down, up));
    }
  else
    {
      listsize = &(This->col_size);
      down = region->x_min;
      up = region->x_max;
      Xc_TRACE(("col[%d; %d]", down, up));
    }
    
  while (*listsize && ((*listsize)->Amax < down))
    {
      listsize = &((*listsize)->Next);
    }

  if (*listsize)
    {
      if ((*listsize)->Amax < up)
	{
	  if ((*listsize)->Amin == down)
	    {
	      (*listsize)->Amax = up;
	      (*listsize)->size = value;
	    }
	  else
	    {
	      /* create new one */
	      newsize = (cell_size_list *)Xc_malloc
		("add_size", sizeof(cell_size_list));
	      newsize->Amin = down;
	      newsize->Amax = up;
	      newsize->size = value;
	      newsize->Next = (*listsize)->Next;
	      (*listsize)->Next = newsize;
	      (*listsize)->Amax = down-1;
	      listsize = &((*listsize)->Next);
	    }
	  nextsize = (*listsize)->Next;
	  while (nextsize && (nextsize->Amax <= up))
	    {
	      (*listsize)->Next = nextsize->Next;
	      Xc_free(nextsize);
	      nextsize = (*listsize)->Next;
	    }
	  if (nextsize && (nextsize->Amin <= up))
	    {
	      nextsize->Amin = up+1;
	    }
	}
      else
	{
	  int	oldmax;

	  nextsize = *listsize;
	  oldmax = nextsize->Amax;
	  if (nextsize->Amin != down)
	    {
	      newsize = (cell_size_list *)Xc_malloc
		("add_size", sizeof(cell_size_list));
	      newsize->Amin = down;
	      newsize->Amax = up;
	      newsize->size = value;
	      newsize->Next = (*listsize)->Next;
	      nextsize->Next = newsize;
	      nextsize->Amax = down-1;
	      listsize = &((*listsize)->Next);
	    }
	  if (oldmax != up)
	    {
	      newsize = (cell_size_list *)Xc_malloc
		("add_size", sizeof(cell_size_list));
	      newsize->Amin = up+1;
	      newsize->Amax = oldmax;
	      newsize->size = nextsize->size;
	      newsize->Next = (*listsize)->Next;
	      (*listsize)->Next = newsize;
	      (*listsize)->Amax = up;
	    }
	  (*listsize)->size = value;
	}
    }
}

/* ----------------------------------------------------------------- ** 
** insert_size_zone - insert lines or columns zones for size         ** 
** ----------------------------------------------------------------- */
static void Moteur_insert_size_zone(This, type, z_start, z_end)
c_Moteur	*This;
int		type;
int		z_start;
int		z_end;
{
  cell_size_list	**listsize;
  int			step;

  Xc_HISTORY(("insert_size_zone(%d-%d)", z_start, z_end));
  step = z_end - z_start + 1;

  if (type == Xc_LINE)
    {
      listsize = &(This->line_size);
    }
  else
    {
      listsize = &(This->col_size);
    }
    
  while (*listsize && ((*listsize)->Amax < z_start))
    {
      listsize = &((*listsize)->Next);
    }

  if (*listsize)
    {
      (*listsize)->Amax+=step;
      listsize = &((*listsize)->Next);
      while (*listsize)
	{
	  (*listsize)->Amin+=step;
	  (*listsize)->Amax+=step;
	  listsize = &((*listsize)->Next);
	}
    }
}

/* ----------------------------------------------------------------- ** 
** delete_size_zone - delete lines or columns zones for size         ** 
** ----------------------------------------------------------------- */
static void Moteur_delete_size_zone(This, type, z_start, z_end)
c_Moteur	*This;
int		type;
int		z_start;
int		z_end;
{
  cell_size_list	**listsize;
  cell_size_list	*delsize;
  int			step;
  boolean		test_me;

  Xc_HISTORY(("delete_size_zone(%d-%d)", z_start, z_end));
  step = z_end - z_start + 1;

  if (type == Xc_LINE)
    {
      listsize = &(This->line_size);
    }
  else
    {
      listsize = &(This->col_size);
    }
    
  while (*listsize && ((*listsize)->Amax < z_start))
    {
      listsize = &((*listsize)->Next);
    }

  test_me = TRUE;
  while ((*listsize) && test_me)
    {
      if ((z_start >= (*listsize)->Amin) &&
	  (z_start <= (*listsize)->Amax))
	{
	  if ((z_start <= (*listsize)->Amin) &&
	      (z_end >= (*listsize)->Amax))
	    {
	      /* full inside destroy */
	      delsize = *listsize;
	      *listsize = delsize->Next;
	      Xc_free(delsize);
	      if (z_end == delsize->Amax)
		test_me = FALSE;
	    }
	  else
	    {
	      if ((z_end >= (*listsize)->Amin) &&
		  (z_end <= (*listsize)->Amax))
		{
		  /* full include reduce */
		  (*listsize)->Amax -= step;
		  listsize = &((*listsize)->Next);
		  test_me = FALSE;
		}
	      else
		{
		  /* start inside reduce */
		  (*listsize)->Amax = z_start;
		  listsize = &((*listsize)->Next);
		}
	    }
	}
      else
	{
	  if ((z_end >= (*listsize)->Amin) &&
	      (z_end < (*listsize)->Amax))
	    {
	      /* end inside reduce */
	      (*listsize)->Amin = z_start+1;
	      (*listsize)->Amax -= step;
	      listsize = &((*listsize)->Next);
	      test_me = FALSE;
	    }
	  else
	    {
	      if ((z_start <= (*listsize)->Amin) &&
		  (z_end >= (*listsize)->Amax))
		{
		  /* full inside destroy */
		  delsize = *listsize;
		  *listsize = delsize->Next;
		  Xc_free(delsize);
		  if (z_end == delsize->Amax)
		    test_me = FALSE;
		}
	    }
	}
    }
  while (*listsize)
    {
      (*listsize)->Amin -= step;
      (*listsize)->Amax -= step;
      listsize = &((*listsize)->Next);
    }
}

/* ----------------------------------------------------------------- ** 
** get_string - convert cell value in string                         ** 
** ----------------------------------------------------------------- */
static int Moteur_get_string(This, cell_s, str, type)
c_Moteur	*This;
cell 		*cell_s;
char		*str;
int		type;
{
  int		ret_type;

  ret_type = Xq_VOID;
  if (!str)
    {
      str = (char *)Xc_malloc("get_str", Xq_FORMULA_LENGTH);
    }
  if (cell_s->type == Xq_VOID)
    {
      str[0] = 0;
      return  Xq_VOID;
    }
  if (cell_s->type == Xq_TEXT)
    {
      if (cell_s->text)
	{
	  if ((type != Xq_FORMULA) && (cell_s->text[0]=='\''))
	    sprintf(str, "%s", cell_s->text+1);
	  else
	    sprintf(str, "%s", cell_s->text);
	  return Xq_TEXT;
	}
      else
	{
	  str[0] = 0;
	  return  Xq_VOID;
	}
    }

  switch(type)
    {
    case Xq_TEXT:
      if (cell_s->text)
	sprintf(str, "%s", cell_s->text);
      ret_type = Xq_TEXT;
      break;
    case Xq_FORMULA:
      if (cell_s->formula)
	{
	  sprintf(str, "%s", cell_s->formula);
	  return Xq_FORMULA;
	}
      else
	{
	  if (cell_s->value2)
	    {
	      switch(ptom_convert_type[(int)cell_s->ret_type])
	      {
	      case Xq_PERCENT:
		{
		  char	*tmp_str, mstr[Xq_FORMULA_LENGTH];
		  int	slen;
		  int	indexs;
		  
		  tmp_str = g2str(cell_s->value2, LOW_PREC);
		  indexs = 0;
		  strcpy(mstr, tmp_str);
		  while(mstr[indexs] != '.' && mstr[indexs] != '\0')
		    indexs++;
		  if (mstr[indexs] == '\0' || mstr[indexs + 1] == '\0')
		  {
		    mstr[indexs] = '0';
		    mstr[indexs+1] = '0';
		    mstr[indexs+2] = '\0';
		  }
		  else
		  {
		    if (mstr[indexs + 2] == '\0')
		    {
		      mstr[indexs+2] = '0';
		      mstr[indexs+3] = '\0';	  
		    }
		    mstr[indexs] = mstr[indexs + 1];
		    mstr[indexs + 1] = mstr[indexs + 2];
		    if (mstr[indexs + 3] != '\0')
		      mstr[indexs + 2] = '.';
		    else
		      mstr[indexs + 2] = '\0';
		  }
		  if (mstr[0] == '0' && indexs == 1)
		  {
		    if (mstr[1] == '0')
		      memcpy(mstr, mstr + 2, Xq_FORMULA_LENGTH - 2);
		    else
		      memcpy(mstr, mstr + 1, Xq_FORMULA_LENGTH - 1);
		  }
		  else
		    if (mstr[0] == '-' && mstr[1] == '0' && indexs == 2)
		    {
		      if (mstr[2] == '0')
			memcpy(mstr + 1, mstr + 3,
			       Xq_FORMULA_LENGTH - 3);
		      else
			memcpy(mstr + 1, mstr + 2,
			       Xq_FORMULA_LENGTH - 2);
		    }
		  
		  slen = 0;
		  while (mstr[slen] && mstr[slen]!='.')
		    slen++;
		  if (mstr[slen])
		  {
		    slen = strlen(mstr)-1;
		    while (mstr[slen] == '0')
		      slen--;
		    if (mstr[slen]=='.')
		      slen--;
		    mstr[slen+1]=0;
		  }
		  sprintf(str, "%s%%", mstr);
		  free(tmp_str);
		  
		  ret_type = Xq_TEXT;
		}
		break;
	      case Xq_BOOLEAN:
		{
		  double	value;
		  
		  value = gtodouble(cell_s->value2);
		  if (value == 0.0)
		    strcpy(str, F(GlobResources).getString(GlobResources,
							     XcR_false));
		  else
		    strcpy(str, F(GlobResources).getString(GlobResources,
							   XcR_true));
		  ret_type = Xq_TEXT;
		}
		break;
	      case Xq_DAY_IN_WEEK:
		{
		  double value;
		  char *day_name;
		  int day;
		  
		  value = gtodouble(cell_s->value2);
		  if (value > 0.0) 
		    value += 0.5;
		  else
		    value -= 0.5;
		  day = (((int)value) - 1) % 7;
		  if (day < 0) day = 7 + day;
		  
		  day_name = F(GlobResources).getString
		    (GlobResources, XcR_dateDayMon + day);
		  strncpy(str, day_name, Xq_FORMULA_LENGTH);
		  ret_type = Xq_TEXT;
		}
		break;
	      case Xq_MONTH:
		{
		  double value;
		  char *month_name;
		  int month;
		  
		  value = gtodouble(cell_s->value2);
		  if (value > 0.0) 
		    value += 0.5;
		  else
		    value -= 0.5;
		  month = (((int)value) - 1) % 12;
		  if (month < 0) month = 12 + month;
		  
		  month_name = F(GlobResources).getString
		    (GlobResources, XcR_dateMonthJan + month);
		  strncpy(str, month_name, Xq_FORMULA_LENGTH);
		  ret_type = Xq_TEXT;
		}
		break;
	      case Xq_DATE:
		{
		  double	value;
		  xc_date_t	xc_date;
		  
		  value = gtodouble(cell_s->value2);
		  convert_to_date(value, &xc_date);
		  if (xc_date.year >= 1920 && xc_date.year < 2000)
		    sprintf(str, "%.2d/%.2d/%.2d", xc_date.day,
			    xc_date.month, xc_date.year - 1900);
		  else
		  {
		    if (xc_date.year >= 2000 && xc_date.year < 2020)
		      sprintf(str, "%.2d/%.2d/%.2d", xc_date.day,
			      xc_date.month, xc_date.year - 2000);
		    else
		      sprintf(str, "%.2d/%.2d/%d", xc_date.day,
			      xc_date.month, xc_date.year);
		  }
		  ret_type = Xq_TEXT;
		}
		break;
	      case Xq_TIME:
		{
		  double	value;
		  xc_time_t	xc_time;
		  
		  value = gtodouble(cell_s->value2);
		  convert_to_time(value, &xc_time);
		  sprintf(str, "%.2d:%.2d:%.2d", xc_time.hour,
			    xc_time.minute, xc_time.second);
		  
		  ret_type = Xq_TEXT;
		}
		break;
	      case Xq_DATE_TIME:
		{
		  double	value;
		  xc_date_time_t	xc_date_time;
		  char		*format;
		  
		  value = gtodouble(cell_s->value2);
		  convert_to_date_and_time(value, &xc_date_time);
		  
		  if (xc_date_time.date.year >= 1920 &&
		      xc_date_time.date.year < 2000)
		  {
		    format = "%.2d/%.2d/%.2d %.2d:%.2d:%.2d";
		    xc_date_time.date.year -= 1900;
		  }
		  else
		  {
		    if (xc_date_time.date.year >= 2000 &&
			xc_date_time.date.year < 2020)
		    {
		      format = "%.2d/%.2d/%.2d %.2d:%.2d:%.2d";
		      xc_date_time.date.year -= 2000;
		    }
		    else
		      format = "%.2d/%.2d/%d %.2d:%.2d:%.2d";
		  }
		  sprintf(str, format, xc_date_time.date.day,
			  xc_date_time.date.month, xc_date_time.date.year,
			  xc_date_time.time.hour,  xc_date_time.time.minute,
			  xc_date_time.time.second);
		  
		  ret_type = Xq_TEXT;
		}
		break;
	      default:
		{
		  char	*tmp_str;
		  int	slen;
		  
		  tmp_str = g2str(cell_s->value2, LOW_PREC);
		  slen = 0;
		  while (tmp_str[slen] && tmp_str[slen]!='.')
		    slen++;
		  if (tmp_str[slen])
		  {
		    slen = strlen(tmp_str)-1;
		    while (tmp_str[slen] == '0')
		      slen--;
		    if (tmp_str[slen]=='.')
		      slen--;
		    tmp_str[slen+1]=0;
		  }
		  strcpy(str, tmp_str);
		  free(tmp_str);
		  
		  ret_type = Xq_VALUE;
		}
		break;
	      }
	    }
	  else
	    {
	      if (cell_s->text)
		sprintf(str, "%s", cell_s->text);
	      ret_type = Xq_TEXT;
	    }
	}
      break;
    case Xq_VALUE:
      if (cell_s->type == Xq_FORMERR)
	{
	  switch (cell_s->status)
	    {
	    case TYPE_CIRC_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaCircError));
	      break;
	    case TYPE_VAL_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaValError));
	      break;
	    case TYPE_NAME_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaNameError));
	      break;
	    case TYPE_CHAR_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaCharError));
	      break;
	    case TYPE_CALC_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaCalcError));
	      break;
	    case TYPE_DIVZ_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaDivzError));
	      break;
	    case TYPE_PARAM_LESS_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaParamLessError));
	      break;
	    case TYPE_PARAM_MORE_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaParamMoreError));
	      break;
	    case TYPE_CELLRANGE_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaCellRangeError));
	      break;
	    case TYPE_D_INT_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaDIntError));
	      break;
	    case TYPE_D_ONEONE_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaDOneOneError));
	      break;
	    case TYPE_D_OVERONE_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaDOverOneError));
	      break;
	    case TYPE_D_ONEONEOPEN_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaDOneOneOpenError));
	      break;
	    case TYPE_D_ALLEZERO_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaDAlleZeroError));
	      break;
	    case TYPE_D_OVERZERO_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaDOverZeroError));
	      break;
	    case TYPE_D_POSITIVE_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaDPositiveError));
	      break;
	    case TYPE_MISSING_P_ERROR:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaMissingParamError));
	      break;
	    case TYPE_UNTERM_STRING:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaMissingParamError));
	      break;
	    case TYPE_NO_PARAM_STRING:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaMissingParamError));
	      break;

	    case TYPE_ERROR:
	    default:
	      sprintf(str, F(GlobResources).
		      getString(GlobResources, XcR_formulaError));
	      break;
	    }
	  return Xq_FORMERR;
	}

      if (cell_s->value2)
	{
	  char	*tmp_str;
	  int	slen;
	  
	  tmp_str = g2str(cell_s->value2, LOW_PREC);
	  slen = 0;
	  while (tmp_str[slen] && tmp_str[slen]!='.')
	    slen++;
	  if (tmp_str[slen])
	    {
	      slen = strlen(tmp_str)-1;
	      while (tmp_str[slen] == '0')
		slen--;
	      if (tmp_str[slen]=='.')
		slen--;
	      tmp_str[slen+1]=0;
	    }
	  strcpy(str, tmp_str);
	  free(tmp_str);
	  ret_type = ptom_convert_type[(int)cell_s->ret_type];
	  Xc_HISTORY(("type retourné: %d", cell_s->ret_type));
	}
      else
	{
	  if (cell_s->text)
	    sprintf(str, "%s", cell_s->text);
	  ret_type = Xq_TEXT;
	}
    }
  return ret_type;
}

static cell_depend_list *dep_is_in_tree(cell_s, root, depth, max_depth)
cell 		*cell_s;
cell_depend_list	**root;
int		depth;
int		max_depth;
{
  cell_depend_list	*result;

  while (*root)
    {
      if ((*root)->depend.cell_p == cell_s)
	{
	  if (depth > max_depth)
	    return (void *)-1;
	  else
	    {
	      result = *root;
	      *root = (*root)->Next;
	      result->Next = NULL;
	      return result;
	    }
	}
      if ((*root)->depend.Next)
	{
	  if ((result = dep_is_in_tree(cell_s, &((*root)->depend.Next),
				       depth+1, max_depth)))
	    return result;
	}
      root = &((*root)->Next);
    }
  return NULL;
}

static void store_exec(cell_d, list_tmp, root, depth)
cell_depend		*cell_d;
cell_depend_list	**list_tmp;
cell_depend_list	**root;
int			depth;
{
  cell_depend_list	**list_tmp1;
  cell_depend_list	*branch;

  Xc_TRACE(("Store Tree"));
  list_tmp1 = list_tmp;
  while (cell_d)
    {
      Xc_TRACE(("%c%d", cell_d->column+'a', 
		cell_d->line+1));
      if ((branch = dep_is_in_tree(cell_d->cell_p, root, 0, depth+1)))
	{
	  /* if -1 it exist but is already considered by another branch */
	  if (branch != (void *)-1)
	    *list_tmp = branch;
	  else
	    {
	      cell_d = cell_d->Next;
	      continue;
	    }
	}
      else
	{
	  *list_tmp = 
	    (cell_depend_list *)Xc_malloc("dep_son", sizeof(cell_depend_list));
	  (*list_tmp)->depend.line = cell_d->line;
	  (*list_tmp)->depend.column = cell_d->column;
	  (*list_tmp)->depend.cell_p = cell_d->cell_p;
	  (*list_tmp)->depend.Next = NULL;
	  (*list_tmp)->expanded = FALSE;
	  (*list_tmp)->Next = NULL;
	}
      list_tmp = &((*list_tmp)->Next);
      cell_d = cell_d->Next;
    }
  *list_tmp = NULL;

  branch = *list_tmp1;
  while (branch)
    {
      if (!branch->expanded)
	{
	  if (branch->depend.cell_p->depend)
	    {
	      Xc_TRACE(("Branch %c%d",
			branch->depend.column+'a',
			branch->depend.line+1));
	      store_exec(branch->depend.cell_p->depend,
			 &(branch->depend.Next), root, depth+1);
	    }
	  branch->expanded = TRUE;
	}
      branch = branch->Next;
    }
}

static void exec_stored(This, list_tmp, sel_feuille, all_gdep)
c_Moteur	*This;
cell_depend_list	*list_tmp;
c_Feuille	*sel_feuille;
HG_list		**all_gdep;
{
  cell_depend_list	*list_tmp1;
  cell_depend_list	*list_destroy;
  cell_depend_list	**list_tmp2;
  cell_depend_list	*list_restart;

  Xc_TRACE(("Exec Tree"));
  list_tmp1 = list_tmp;
  list_restart = NULL;
  while (list_tmp1)
    {
      Xc_TRACE(("%c%d", list_tmp1->depend.column+'a', 
		list_tmp1->depend.line+1));
      if (list_tmp1->depend.cell_p->type == Xq_FORMERR)
	{
	  /* don't need to remove depend because
	     if exist the depend is not added */
	  if (F(This->parser).parse(This->parser, 
				    list_tmp1->depend.cell_p->formula+1,
				    list_tmp1->depend.column, 
				    list_tmp1->depend.line,
				    list_tmp1->depend.cell_p, ADD_DEP, 
				    sel_feuille) == -1)
	    {
	      list_tmp1->depend.cell_p->type = Xq_FORMERR;
	      if (list_tmp1->depend.cell_p->value2)
		{
		  Xc_free(list_tmp1->depend.cell_p->value2);
		  list_tmp1->depend.cell_p->value2 = NULL;
		}
	    }
	  else
	    list_tmp1->depend.cell_p->type = Xq_FORMULA;
	}
      else
	{
	  if (F(This->parser).parse(This->parser, 
				    list_tmp1->depend.cell_p->formula+1,
				    list_tmp1->depend.column, 
				    list_tmp1->depend.line,
				    list_tmp1->depend.cell_p, IGN_DEP, 
				    sel_feuille) == -1)
	    {
	      list_tmp1->depend.cell_p->type = Xq_FORMERR;
	      if (list_tmp1->depend.cell_p->value2)
		{
		  Xc_free(list_tmp1->depend.cell_p->value2);
		  list_tmp1->depend.cell_p->value2 = NULL;
		}
	    }
	}
      if (sel_feuille)
	F(sel_feuille).RefreshCellule(sel_feuille,
				      list_tmp1->depend.column, 
				      list_tmp1->depend.line);

      store_depend(NULL, NULL, 
		   list_tmp1->depend.cell_p->graph_list, all_gdep);

      if (list_tmp1->depend.Next)
	{
	  if (!list_restart)
	    {
	      list_tmp2 = &(list_tmp1->depend.Next);
	      list_restart = *list_tmp2;
	    }
	  else
	    {
	      *list_tmp2 = list_tmp1->depend.Next;
	    }
	  while (*list_tmp2)
	    {
	      list_tmp2 = &((*list_tmp2)->Next);
	    }
	}
      list_destroy = list_tmp1;
      list_tmp1 = list_tmp1->Next;
      Xc_free(list_destroy);
    }
  if (list_restart)
    exec_stored(This, list_restart, sel_feuille, all_gdep);
}

static void exec_depend2(This, cell_s, sel_feuille)
c_Moteur	*This;
cell 		*cell_s;
c_Feuille	*sel_feuille;
{
  HG_list		*tmp_gdep;
  HG_list		*all_gdep;
  cell_depend_list	*list_d;
  int			depth;

  list_d = NULL;
  all_gdep = NULL;
  depth = 0;
  store_exec(cell_s->depend, &list_d, &list_d, depth);
  exec_stored(This, list_d, sel_feuille, &all_gdep);
  
  store_depend(NULL, NULL, cell_s->graph_list, &all_gdep);
  while (all_gdep)
  {
    F(all_gdep->Graph).draw(all_gdep->Graph);
    tmp_gdep = all_gdep;
    all_gdep = all_gdep->Next;
    Xc_free(tmp_gdep);
  }
}

static void exec_depend(This, cell_s, sel_feuille)
c_Moteur	*This;
cell 		*cell_s;
c_Feuille	*sel_feuille;
{
  cell_depend	*cell_d;
  HG_list	*graph_d;

  cell_d = cell_s->depend;
  while (cell_d)
    {
      if (cell_d->cell_p->type == Xq_FORMERR)
	{
	  /* don't need to remove depend because
	     if exist the depend is not added */
	  if (F(This->parser).parse(This->parser, 
				    cell_d->cell_p->formula+1,
				    cell_d->column, cell_d->line,
				    cell_d->cell_p, ADD_DEP, 
				    sel_feuille) == -1)
	    {
	      cell_d->cell_p->type = Xq_FORMERR;
	      if (cell_d->cell_p->value2)
		{
		  Xc_free(cell_d->cell_p->value2);
		  cell_d->cell_p->value2 = NULL;
		}
	    }
	  else
	    cell_d->cell_p->type = Xq_FORMULA;
	}
      else
	{
	  if (F(This->parser).parse(This->parser, 
				    cell_d->cell_p->formula+1,
				    cell_d->column, cell_d->line,
				    cell_d->cell_p, IGN_DEP,
				    sel_feuille) == -1)
	    {
	      cell_d->cell_p->type = Xq_FORMERR;
	      if (cell_d->cell_p->value2)
		{
		  Xc_free(cell_d->cell_p->value2);
		  cell_d->cell_p->value2 = NULL;
		}
	    }
	}
      exec_depend(This, cell_d->cell_p, sel_feuille);
      if (sel_feuille)
	F(sel_feuille).RefreshCellule(sel_feuille,
				      cell_d->column,
				      cell_d->line);
      cell_d = cell_d->Next;
    }
  graph_d = cell_s->graph_list;
  while (graph_d)
    {
      F(graph_d->Graph).draw(graph_d->Graph);
      graph_d = graph_d->Next;
    }
}

/* ----------------------------------------------------------------- ** 
** set_string - convert string in cell value                         ** 
** ----------------------------------------------------------------- */
static void Moteur_set_string(This, x, y, cell_s, str, sel_feuille)
c_Moteur	*This;
int		x;
int		y;
cell 		*cell_s;
char		*str;
c_Feuille	*sel_feuille;
{
  int		i_space;
  int		i;
  char		*str_val;
  boolean	null_set;
  int		sign;
  int		comma_pos;
  int		last_digit;
  char		*fin_str;

  int		ret_tok;
  int		type;
  char		strf[300];
  int		indexf;

  null_set = FALSE;
  i_space = 0;
  if (str == NULL)
    null_set = TRUE;
  else
    {
      while(str[i_space]==' ')
	i_space++;
      if (str[i_space]==0)
	null_set = TRUE;
    }

  if (null_set)
    {
      if (cell_s->type == Xq_VOID)
	return;

      if (cell_s->formula)
	{
	  F(This->parser).parse(This->parser, cell_s->formula+1,
				x, y, 
				cell_s, SUB_DEP, sel_feuille);
	  Xc_free(cell_s->formula);
	  cell_s->formula = NULL;
	}

      if (cell_s->text)
	{
	  Xc_free(cell_s->text);
	  cell_s->text = NULL;
	}

      if (cell_s->value2)
	{
	  Xc_free(cell_s->value2);
	  cell_s->value2 = NULL;
	}

      cell_s->type = Xq_VOID;
      exec_depend2(This, cell_s, sel_feuille);
      return;
    }

  /* cas a traiter avec les format de cellules */
  /* penser au cas des cellules non affichable -> flag particulier */
  if (cell_s == &voidcell)
    cell_s = F(This).add_cell(This, y, x);

  if (cell_s->text && (cell_s->type != Xq_VALUE))
    {
      if (strcmp(cell_s->text, str) == 0)
	return;
    }
  
  if (cell_s->formula)
    {
      if (!This->force)
	{
	  if (strcmp(cell_s->formula, str) == 0)
	    return;
	}
      This->force = 0;
      F(This->parser).parse(This->parser, cell_s->formula+1,
			    x, y, 
			    cell_s, SUB_DEP, sel_feuille);
      Xc_free(cell_s->formula);
      cell_s->formula = NULL;
    }

  /* cas des formules (commence par =)*/
  i=i_space;
  if (str[i]=='=')
    {
      i_space = 1;
      while (str[i+i_space]==' ')
	i_space++;
      if (!str[i+i_space])
	{
	  cell_s->type = Xq_FORMERR;
	  cell_s->formula = (char *)Xc_malloc("set_str_formula", 
					      2);
	  strcpy(cell_s->formula, str+i);
	  cell_s->status = TYPE_ERROR;
	  if (cell_s->value2)
	    {
	      Xc_free(cell_s->value2);
	      cell_s->value2 = NULL;
	    }
	  exec_depend2(This, cell_s, sel_feuille);
	  return;
	}
      cell_s->formula = (char *)Xc_malloc("set_str_formula", 
					  strlen(str+i)+1);
      strcpy(cell_s->formula, str+i);
      cell_s->type = Xq_FORMULA;
      if (F(This->parser).parse(This->parser, str+i+1,
				x, y,
				cell_s, ADD_DEP, sel_feuille) == -1)
	{
	  cell_s->type = Xq_FORMERR;
	  if (cell_s->value2)
	    {
	      Xc_free(cell_s->value2);
	      cell_s->value2 = NULL;
	    }
	}
      exec_depend2(This, cell_s, sel_feuille);
      return;
    }

  /* cas des nombres ecrit sans = */
  if (cell_s->value2)
    {
      str_val = g2str(cell_s->value2, BIG_PREC);
      /* TODO se teste n'est jamais bon etant donne se que renvoi g2str*/
      if (strcmp(str_val, str) == 0)
	{
	  free(str_val);
	  return;
	}
      free(str_val);
    }
    
  i=i_space;
  comma_pos = 0;
  if ((str[i]=='+') || (str[i]=='-') || (str[i]>='0' && str[i]<='9'))
    {
      if ((str[i]=='+') || (str[i]=='-'))
	sign = ++i;
      else
	sign = i;
      if ((str[i]>='0' && str[i]<='9'))
	{
	  i++;
	  while ((str[i] >= '0') && (str[i] <= '9'))
	    {
	      i++;
	    }
	  if (str[i] == '.' || str[i] == ',')
	    {
	      str[i] = '.';
	      comma_pos = i;
	      i++;
	      while ((str[i] >= '0') && (str[i] <= '9'))
		{
		  i++;
		}
	    }
	  if (!comma_pos)
	    comma_pos = i;
	  
	  last_digit = i;
	  while(str[i]==' ')
	    i++;
	  if (str[i]=='%')
	    {
	      i++;
	      fin_str = Xc_malloc("sub_str", strlen(str)+10);
	      comma_pos -= sign;
	      if (str[i_space] == '-')
		strcpy(fin_str, "-");
	      else
		fin_str[0] = '\0';
	      
	      if ((comma_pos)-3 < 0)
	      {
		if (comma_pos == 2)
		  strcat(fin_str, "0.");
		else
		  strcat(fin_str, "0.0");
		
		strncat(fin_str, str + sign, comma_pos);
		strncat(fin_str, str + sign + comma_pos + 1,
			last_digit - sign - comma_pos - 1); 
	      }
	      else
	      {
		if (fin_str[0]) comma_pos++;
		strncat(fin_str, str + sign, last_digit - sign); 
		
		fin_str[comma_pos] = fin_str[comma_pos - 1];
		fin_str[comma_pos - 1] = fin_str[comma_pos - 2];
		fin_str[comma_pos - 2] = '.';
	      }
	      cell_s->ret_type = TYPE_PERCENT;
	    }
	  else
	    {
	      fin_str = Xc_malloc("sub_str", strlen(str)+1);
	      strncpy(fin_str, str + i_space, last_digit - i_space);
	      fin_str[last_digit - i_space] = 0;
	      cell_s->ret_type = TYPE_NUM;
	    }
	  
	  if (!str[i])
	    {
	      str2g2(fin_str, &(cell_s->value2));
	      Xc_free(fin_str);
	      cell_s->type = Xq_VALUE;
	      exec_depend2(This, cell_s, sel_feuille);
	      return;
	    }
	  else
	    Xc_free(fin_str);
	}
    }

  indexf = 0;
  /* string is a boolean? */
  ret_tok = get_token_boolean(NULL, str, &type, strf, &indexf);
  if (ret_tok != -1)
    {
      strf[indexf] = 0;
      cell_s->type = Xq_VALUE;
      str2g2(strf, &(cell_s->value2));
      cell_s->ret_type = type;
      exec_depend2(This, cell_s, sel_feuille);
      return;
    }
  /* string is a date? */
  ret_tok = get_token_date(This->parser, str, &type, strf, &indexf);
  if (ret_tok != -1)
    {
      strf[indexf] = 0;
      cell_s->type = Xq_VALUE;
      str2g2(strf, &(cell_s->value2));
      cell_s->ret_type = type;
      exec_depend2(This, cell_s, sel_feuille);
      return;
    }
  /* string is a hour? */
  ret_tok = get_token_hour(NULL, str, &type, strf, &indexf);
  if (ret_tok != -1)
    {
      strf[indexf] = 0;
      cell_s->type = Xq_VALUE;
      str2g2(strf, &(cell_s->value2));
      cell_s->ret_type = type;
      exec_depend2(This, cell_s, sel_feuille);
      return;
    }
  /* string is a day in week? */
  ret_tok = get_token_day(NULL, str, &type, strf, &indexf);
  if (ret_tok != -1)
    {
      strf[indexf] = 0;
      cell_s->type = Xq_VALUE;
      str2g2(strf, &(cell_s->value2));
      cell_s->ret_type = type;
      exec_depend2(This, cell_s, sel_feuille);
      return;
    }
  /* string is a month? */
  ret_tok = get_token_month(NULL, str, &type, strf, &indexf);
  if (ret_tok != -1)
    {
      strf[indexf] = 0;
      cell_s->type = Xq_VALUE;
      str2g2(strf, &(cell_s->value2));
      cell_s->ret_type = type;
      exec_depend2(This, cell_s, sel_feuille);
      return;
    }

  /* other -> case of text */
  i=i_space;
  if (str[i] != '\'')
    i = 0;
  if (cell_s->text)
    Xc_free(cell_s->text);
  cell_s->text = (char *)Xc_malloc("set_str_txt", 
				   strlen(str+i)+1);
  if (cell_s->value2)
    {
      Xc_free(cell_s->value2);
      cell_s->value2 = NULL;
    }
  cell_s->type = Xq_TEXT;
  cell_s->ret_type = TYPE_STRING;
  strcpy(cell_s->text, str+i);
  if (sel_feuille)
    F(This).computeOverrun(This, cell_s, x, y, 
			   sel_feuille->sFactor * COORD_SCALEVALUE
			   / COORD_DEFAULT);
  else
    F(This).computeOverrun(This, cell_s, x, y, SCALE_FROM_COEF(1.0));
  
  exec_depend2(This, cell_s, sel_feuille);
}

/* ----------------------------------------------------------------- ** 
** deffered_set_string - convert string in cell value                ** 
** ----------------------------------------------------------------- */
static void Moteur_deffered_set_string(This, x, y, cell_s, str, sel_feuille,
				       dep, gdep)
c_Moteur	*This;
int		x;
int		y;
cell 		*cell_s;
char		*str;
c_Feuille	*sel_feuille;
cell_depend	**dep;
HG_list		**gdep;
{
  int		i_space;
  int		i;
  char		*str_val;
  boolean	null_set;
  int		sign;
  int		comma_pos;
  int		last_digit;
  char		*fin_str;

  int		ret_tok;
  int		type;
  char		strf[300];
  int		indexf;

  null_set = FALSE;
  i_space = 0;
  if (str == NULL)
    null_set = TRUE;
  else
    {
      while(str[i_space]==' ')
	i_space++;
      if (str[i_space]==0)
	null_set = TRUE;
    }

  if (null_set)
    {
      if (cell_s->type == Xq_VOID)
	return;

      if (cell_s->formula)
	{
	  F(This->parser).parse(This->parser, cell_s->formula+1,
				x, y, 
				cell_s, SUB_DEP, sel_feuille);
	  Xc_free(cell_s->formula);
	  cell_s->formula = NULL;
	}

      if (cell_s->text)
	{
	  Xc_free(cell_s->text);
	  cell_s->text = NULL;
	}

      if (cell_s->value2)
	{
	  Xc_free(cell_s->value2);
	  cell_s->value2 = NULL;
	}

      cell_s->type = Xq_VOID;
      store_depend(cell_s->depend, dep, cell_s->graph_list, gdep);
      return;
    }

  /* cas a traiter avec les format de cellules */
  /* penser au cas des cellules non affichable -> flag particulier */
  if (cell_s == &voidcell)
    cell_s = F(This).add_cell(This, y, x);

  if (cell_s->text && (cell_s->type != Xq_VALUE))
    {
      if (strcmp(cell_s->text, str) == 0)
	return;
    }
  
  if (cell_s->formula)
    {
      if (!This->force)
	{
	  if (strcmp(cell_s->formula, str) == 0)
	    return;
	}
      This->force = 0;
      F(This->parser).parse(This->parser, cell_s->formula+1,
			    x, y, 
			    cell_s, SUB_DEP, sel_feuille);
      Xc_free(cell_s->formula);
      cell_s->formula = NULL;
    }

  /* cas des formules (commence par =)*/
  i=i_space;
  if (str[i]=='=')
    {
      i_space = 1;
      while (str[i+i_space]==' ')
	i_space++;
      if (!str[i+i_space])
	{
	  cell_s->type = Xq_FORMERR;
	  cell_s->formula = (char *)Xc_malloc("set_str_formula", 
					      2);
	  strcpy(cell_s->formula, str+i);
	  cell_s->status = TYPE_ERROR;
	  if (cell_s->value2)
	    {
	      Xc_free(cell_s->value2);
	      cell_s->value2 = NULL;
	    }
	  store_depend(cell_s->depend, dep, cell_s->graph_list, gdep);
	  return;
	}
      cell_s->formula = (char *)Xc_malloc("set_str_formula", 
					  strlen(str+i)+1);
      strcpy(cell_s->formula, str+i);
      cell_s->type = Xq_FORMULA;
      if (F(This->parser).parse(This->parser, str+i+1,
				x, y,
				cell_s, ADD_DEP, sel_feuille) == -1)
	{
	  cell_s->type = Xq_FORMERR;
	  if (cell_s->value2)
	    {
	      Xc_free(cell_s->value2);
	      cell_s->value2 = NULL;
	    }
	}
      store_depend(cell_s->depend, dep, cell_s->graph_list, gdep);
      return;
    }

  /* cas des nombres ecrit sans = */
  if (cell_s->value2)
    {
      str_val = g2str(cell_s->value2, BIG_PREC);
      /* TODO se teste n'est jamais bon etant donne se que renvoi g2str*/
      if (strcmp(str_val, str) == 0)
	{
	  free(str_val);
	  return;
	}
      free(str_val);
    }
    
  i=i_space;
  comma_pos = 0;
  if ((str[i]=='+') || (str[i]=='-') || (str[i]>='0' && str[i]<='9'))
    {
      if ((str[i]=='+') || (str[i]=='-'))
	sign = ++i;
      else
	sign = i;
      if ((str[i]>='0' && str[i]<='9'))
	{
	  i++;
	  while ((str[i] >= '0') && (str[i] <= '9'))
	    {
	      i++;
	    }
	  if (str[i] == '.' || str[i] == ',')
	    {
	      str[i] = '.';
	      comma_pos = i;
	      i++;
	      while ((str[i] >= '0') && (str[i] <= '9'))
		{
		  i++;
		}
	    }
	  if (!comma_pos)
	    comma_pos = i;
	  
	  last_digit = i;
	  while(str[i]==' ')
	    i++;
	  if (str[i]=='%')
	    {
	      i++;
	      fin_str = Xc_malloc("sub_str", strlen(str)+10);
	      comma_pos -= sign;
	      if (str[i_space] == '-')
		strcpy(fin_str, "-");
	      else
		fin_str[0] = '\0';
	      
	      if ((comma_pos)-3 < 0)
	      {
		if (comma_pos == 2)
		  strcat(fin_str, "0.");
		else
		  strcat(fin_str, "0.0");
		
		strncat(fin_str, str + sign, comma_pos);
		strncat(fin_str, str + sign + comma_pos + 1,
			last_digit - sign - comma_pos - 1); 
	      }
	      else
	      {
		if (fin_str[0]) comma_pos++;
		strncat(fin_str, str + sign, last_digit - sign); 
		
		fin_str[comma_pos] = fin_str[comma_pos - 1];
		fin_str[comma_pos - 1] = fin_str[comma_pos - 2];
		fin_str[comma_pos - 2] = '.';
	      }
	      cell_s->ret_type = TYPE_PERCENT;
	    }
	  else
	    {
	      fin_str = Xc_malloc("sub_str", strlen(str)+1);
	      strncpy(fin_str, str + i_space, last_digit - i_space);
	      fin_str[last_digit - i_space] = 0;
	      cell_s->ret_type = TYPE_NUM;
	    }
	  
	  if (!str[i])
	    {
	      str2g2(fin_str, &(cell_s->value2));
	      Xc_free(fin_str);
	      cell_s->type = Xq_VALUE;
	      store_depend(cell_s->depend, dep, cell_s->graph_list, gdep);
	      return;
	    }
	  else
	    Xc_free(fin_str);
	}
    }

  indexf = 0;
  /* string is a boolean? */
  ret_tok = get_token_boolean(NULL, str, &type, strf, &indexf);
  if (ret_tok != -1)
    {
      strf[indexf] = 0;
      cell_s->type = Xq_VALUE;
      str2g2(strf, &(cell_s->value2));
      cell_s->ret_type = type;
      store_depend(cell_s->depend, dep, cell_s->graph_list, gdep);
      return;
    }
  /* string is a date? */
  ret_tok = get_token_date(This->parser, str, &type, strf, &indexf);
  if (ret_tok != -1)
    {
      strf[indexf] = 0;
      cell_s->type = Xq_VALUE;
      str2g2(strf, &(cell_s->value2));
      cell_s->ret_type = type;
      store_depend(cell_s->depend, dep, cell_s->graph_list, gdep);
      return;
    }
  /* string is a hour? */
  ret_tok = get_token_hour(NULL, str, &type, strf, &indexf);
  if (ret_tok != -1)
    {
      strf[indexf] = 0;
      cell_s->type = Xq_VALUE;
      str2g2(strf, &(cell_s->value2));
      cell_s->ret_type = type;
      store_depend(cell_s->depend, dep, cell_s->graph_list, gdep);
      return;
    }
  /* string is a day in week? */
  ret_tok = get_token_day(NULL, str, &type, strf, &indexf);
  if (ret_tok != -1)
    {
      strf[indexf] = 0;
      cell_s->type = Xq_VALUE;
      str2g2(strf, &(cell_s->value2));
      cell_s->ret_type = type;
      store_depend(cell_s->depend, dep, cell_s->graph_list, gdep);
      return;
    }
  /* string is a month? */
  ret_tok = get_token_month(NULL, str, &type, strf, &indexf);
  if (ret_tok != -1)
    {
      strf[indexf] = 0;
      cell_s->type = Xq_VALUE;
      str2g2(strf, &(cell_s->value2));
      cell_s->ret_type = type;
      store_depend(cell_s->depend, dep, cell_s->graph_list, gdep);
      return;
    }

  /* other -> case of text */
  i=i_space;
  if (str[i] != '\'')
    i = 0;
  if (cell_s->text)
    Xc_free(cell_s->text);
  cell_s->text = (char *)Xc_malloc("set_str_txt", 
				   strlen(str+i)+1);
  if (cell_s->value2)
    {
      Xc_free(cell_s->value2);
      cell_s->value2 = NULL;
    }
  cell_s->type = Xq_TEXT;
  cell_s->ret_type = TYPE_STRING;
  strcpy(cell_s->text, str+i);
  if (sel_feuille)
    F(This).computeOverrun(This, cell_s, x, y, 
			   sel_feuille->sFactor * COORD_SCALEVALUE
			   / COORD_DEFAULT);
  else
    F(This).computeOverrun(This, cell_s, x, y, SCALE_FROM_COEF(1.0));
  
  store_depend(cell_s->depend, dep, cell_s->graph_list, gdep);
}


/* ----------------------------------------------------------------- ** 
** add_depend - add dependance for cell to a cell                    ** 
** ----------------------------------------------------------------- */
static void Moteur_add_depend(This, cell_s, line, col, dep_line, dep_col)
c_Moteur	*This;
cell 		*cell_s;
int		line;
int		col;
int		dep_line;
int		dep_col;
{
  cell		*tmp_cell; /* this is the cell to add depend */
  cell_depend	**dep;

  tmp_cell = F(This).get_cell(This, dep_col, dep_line);
  if (tmp_cell == &voidcell)
    {
      tmp_cell = F(This).add_cell(This, dep_line, dep_col);
    }
  dep = &(tmp_cell->depend);
  while (*dep)
    {
      /*--- this should happen for error formula ---*/
      if (((*dep)->line == line) &&
	  ((*dep)->column == col))
	{
	  Xc_TRACE(("depend exist adresse %p to adress %p", 
		    (*dep)->cell_p, cell_s));
	  (*dep)->cell_p = cell_s;
	  return;
	}
      dep = &((*dep)->Next);
    }
  *dep = (cell_depend *)Xc_malloc("celldep", sizeof(cell_depend));
  (*dep)->line = line;
  (*dep)->column = col;
  (*dep)->cell_p = cell_s;
  (*dep)->Next = NULL;
  Xc_TRACE(("Add depend %p to %d-%d", cell_s, dep_col, dep_line));
}

/* ----------------------------------------------------------------- ** 
** sub_depend - sub dependance for cell to a cell                     ** 
** ----------------------------------------------------------------- */
static void Moteur_sub_depend(This, cell_s, line, col)
c_Moteur	*This;
cell		*cell_s;
int		line;
int		col;
{
  cell		*tmp_cell;
  cell_depend	**dep_ch;
  cell_depend	*dep;
  
  tmp_cell = F(This).get_cell(This, col, line);
  dep_ch = &(tmp_cell->depend);
  dep = tmp_cell->depend;
  while (dep)
    {
      if (dep->cell_p == cell_s)
	{
	  (*dep_ch) = dep->Next;
	  Xc_free(dep);
	  Xc_TRACE(("Sub depend %p to %d-%d", cell_s, col, line));
	  break;
	}
      dep_ch = &(dep->Next);
      dep = dep->Next;
    }
  if ((tmp_cell == Xq_VOID) && 
      (tmp_cell->depend == NULL) &&
      (tmp_cell->graph_list == NULL))
    {
      /*--- this should never happen ---*/
      if (tmp_cell->text)
	{
	  Xc_TRACE(("beep-beep"));
	  Xc_free(tmp_cell->text);
	  tmp_cell->text=NULL;	  
	}
      if (tmp_cell->formula)
	{
	  Xc_TRACE(("beep-beep"));
	  Xc_free(tmp_cell->formula);
	  tmp_cell->formula=NULL;
	}
      /*--- end of never happen ---*/
      F(This).del_cell(This, line, col);
    }
}

/* ----------------------------------------------------------------- ** 
** add_graph_dep - add dependance for graph to a cell                ** 
** ----------------------------------------------------------------- */
static void Moteur_add_graph_dep(This, graph, dep_col, dep_line)
c_Moteur	*This;
struct _c_HighGraph	*graph;
int		dep_col;
int		dep_line;
{
  cell		*tmp_cell;
  HG_list	**dep;
  
  tmp_cell = F(This).get_cell(This, dep_col, dep_line);
  if (tmp_cell == &voidcell)
    {
      tmp_cell = F(This).add_cell(This, dep_line, dep_col);
    }
  dep = &(tmp_cell->graph_list);
  while (*dep)
    {
      dep = &((*dep)->Next);
    }
  *dep = (HG_list *)Xc_malloc("HG_LIST", sizeof(HG_list));
  (*dep)->Graph = graph;
  (*dep)->Next = NULL;
}

/* ----------------------------------------------------------------- ** 
** sub_graph_dep - sub dependance for graph to a cell                ** 
** ----------------------------------------------------------------- */
static void Moteur_sub_graph_dep(This, graph, col, line)
c_Moteur	*This;
c_HighGraph	*graph;
int		col;
int		line;
{
  cell		*tmp_cell;
  HG_list	**dep_ch;
  HG_list	*dep;
  
  tmp_cell = F(This).get_cell(This, col, line);
  dep_ch = &(tmp_cell->graph_list);
  dep = tmp_cell->graph_list;
  while (dep)
    {
      if (dep->Graph == graph)
	{
	  (*dep_ch) = dep->Next;
	  Xc_free(dep);
	  Xc_TRACE(("Sub graph depend %p to %d-%d", graph, col, line));
	  break;
	}
      dep_ch = &(dep->Next);
      dep = dep->Next;
    }
  if ((tmp_cell == Xq_VOID) && 
      (tmp_cell->depend == NULL) &&
      (tmp_cell->graph_list == NULL))
    {
      /*--- this should never happen ---*/
      if (tmp_cell->text)
	{
	  Xc_TRACE(("beep-beep"));
	  Xc_free(tmp_cell->text);
	  tmp_cell->text=NULL;	  
	}
      if (tmp_cell->formula)
	{
	  Xc_TRACE(("beep-beep"));
	  Xc_free(tmp_cell->formula);
	  tmp_cell->formula=NULL;
	}
      /*--- end of never happen ---*/
      F(This).del_cell(This, line, col);
    }
}

/* ----------------------------------------------------------------- ** 
** add_depend_zone - add dependance for cells zone to a cell         ** 
** ----------------------------------------------------------------- */
static void Moteur_add_depend_zone(This, cell_s,
				   line, col,
				   dep_line_s, dep_col_s,
				   dep_line_e, dep_col_e)
c_Moteur	*This;
cell 		*cell_s;
int		line;
int		col;
int		dep_line_s;
int		dep_col_s;
int		dep_line_e;
int		dep_col_e;
{
  zone_depend	**depend;
  int		tmp;

  if (dep_line_s > dep_line_e)
    {
      tmp = dep_line_s;
      dep_line_s = dep_line_e;
      dep_line_e = tmp;
    }
  if (dep_col_s > dep_col_e)
    {
      tmp = dep_col_s;
      dep_col_s = dep_col_e;
      dep_col_e = tmp;
    }

  depend = &(This->zone_dep);
  while (*depend)
    {
      depend = &((*depend)->Next);
    }
  *depend = Xc_malloc("dep_zone", sizeof(zone_depend));

  (*depend)->region.y_min = dep_line_s;
  (*depend)->region.y_max = dep_line_e;
  (*depend)->region.x_min = dep_col_s;
  (*depend)->region.x_max = dep_col_e;
  (*depend)->region.NextRegion = NULL;
  (*depend)->line = line;
  (*depend)->column = col;
  (*depend)->cell_p = cell_s;
  (*depend)->Next = NULL;
}

/* ----------------------------------------------------------------- ** 
** sub_depend_zone - sub dependance for cells zone to a cell         ** 
** ----------------------------------------------------------------- */
static void Moteur_sub_depend_zone(This, cell_s)
c_Moteur	*This;
cell 		*cell_s;
{
  zone_depend	**dep_ch;
  zone_depend	*dep;
  
  dep_ch = &(This->zone_dep);
  dep = This->zone_dep;
  while (dep)
    {
      if (dep->cell_p == cell_s)
	{
	  (*dep_ch) = dep->Next;
	  Xc_free(dep);
	  Xc_TRACE(("cell changed"));
	  break;
	}
      dep_ch = &(dep->Next);
      dep = dep->Next;
    }
}

/* ----------------------------------------------------------------- ** 
** check_c_depend - check for circular dependances (on x,y)          ** 
** ----------------------------------------------------------------- */
static boolean Moteur_check_c_depend(This, cell_s, x, y)
c_Moteur	*This;
cell 		*cell_s;
int		x;
int		y;
{
  cell		*tmp_cell;
  cell_depend	*dep;
  
  tmp_cell = F(This).get_cell(This, x, y);
  Xc_TRACE (("check depend (%p, %p)", tmp_cell, cell_s));
  if (tmp_cell == cell_s)
    return TRUE;
  dep = cell_s->depend;
  while (dep)
    {
      if (F(This).check_c_depend_cell(This, tmp_cell, dep->cell_p) == TRUE)
	{
	  Xc_TRACE(("depend : TRUE"));
	  return TRUE;
	}
      dep = dep->Next;
    }
  return FALSE;
}

/* ----------------------------------------------------------------- ** 
** check_c_depend_cell - check for circular dependances (on adress)  ** 
** ----------------------------------------------------------------- */
static boolean Moteur_check_c_depend_cell(This, cell_s, cell_d)
c_Moteur	*This;
cell 		*cell_s;
cell 		*cell_d;
{
  cell_depend	*dep;
  
  if (cell_d == cell_s)
    return TRUE;
  dep = cell_d->depend;
  Xc_TRACE (("depend2: %p", cell_d));
  while (dep)
    {
      if (F(This).check_c_depend_cell(This, cell_s, dep->cell_p) == TRUE)
	return TRUE;
      dep = dep->Next;
    }
  return FALSE;
}

/* ----------------------------------------------------------------- ** 
** frag_style - fragmentation du style (internal)                    ** 
** ----------------------------------------------------------------- */
static void frag_style(This, region)
c_Moteur	*This;
region_t	*region;
{
  cell_style_list	**list;
  cell_style_list	**list_tmp;
  cell_style_list	*new_listed;
  int			progress_ok;

  Xc_TRACE(("frag style"));
  list = &(This->style_list);
  /* cherche premiere qui peut intersecter */
  progress_ok = 0;

  while (*list && ((*list)->region.x_min <= region->x_max))
    {
      if (region->x_min > (*list)->region.x_min)
	{
	  if (region->x_min <= (*list)->region.x_max)
	    {
	      if (region->x_max >= (*list)->region.x_max)
		{
		  /* cas 1*/
		  if (region->y_min > (*list)->region.y_min)
		    {
		      if (region->y_min <= (*list)->region.y_max)
			{
			  if (region->y_max >= (*list)->region.y_max)
			    {
			      /* cas 1.1 */
			      new_listed = (cell_style_list *)Xc_malloc
				("list_style", sizeof(cell_style_list));
			      XqCS_INC_REFERENCE((*list)->cstyle);
			      new_listed->cstyle = (*list)->cstyle;
			      new_listed->region.x_min = 
				region->x_min;
			      new_listed->region.x_max = 
				(*list)->region.x_max;
			      new_listed->region.y_min = 
				(*list)->region.y_min;
			      new_listed->region.y_max = 
				region->y_min-1;
			      new_listed->id = This->id_func;
			      list_tmp = list;
			      while (*list_tmp && 
				     ((*list_tmp)->region.x_min <
				      new_listed->region.x_min))
				{
				  list_tmp = &((*list_tmp)->Next);
				}
			      new_listed->Next = *list_tmp;
			      *list_tmp = new_listed;
			      if (list == list_tmp)
				list = &(new_listed->Next);
			      (*list)->region.x_max = region->x_min-1;
			    }
			  else
			    {
			      /* cas 1.2 */
			      new_listed = (cell_style_list *)Xc_malloc
				("list_style", sizeof(cell_style_list));
			      XqCS_INC_REFERENCE((*list)->cstyle);
			      new_listed->cstyle = (*list)->cstyle;
			      new_listed->region.x_min = 
				(*list)->region.x_min;
			      new_listed->region.x_max = 
				region->x_min-1;
			      new_listed->region.y_min = 
				region->y_min;
			      new_listed->region.y_max = 
				region->y_max;
			      new_listed->id = This->id_func;
			      list_tmp = list;
			      new_listed->Next = *list_tmp;
			      *list_tmp = new_listed;
			      list = &(new_listed->Next);
			      new_listed = (cell_style_list *)Xc_malloc
				("list_style", sizeof(cell_style_list));
			      XqCS_INC_REFERENCE((*list)->cstyle);
			      new_listed->cstyle = (*list)->cstyle;
			      new_listed->region.x_min = 
				(*list)->region.x_min;
			      new_listed->region.x_max = 
				(*list)->region.x_max;
			      new_listed->region.y_min = 
				region->y_max+1;
			      new_listed->region.y_max = 
				(*list)->region.y_max;
			      new_listed->id = This->id_func;
			      list_tmp = list;
			      new_listed->Next = *list_tmp;
			      *list_tmp = new_listed;
			      list = &(new_listed->Next);
			      (*list)->region.y_max = region->y_min-1;
			    }
			}
		    }
		  else
		    {
		      if (region->y_max >= (*list)->region.y_max)
			{
			  /* cas 1.3*/
			  (*list)->region.x_max = region->x_min-1;
			}
		      else
			if (region->y_max >= (*list)->region.y_min)
			  {
			    /* cas 1.4*/
			    new_listed = (cell_style_list *)Xc_malloc
			      ("list_style", sizeof(cell_style_list));
			    XqCS_INC_REFERENCE((*list)->cstyle);
			    new_listed->cstyle = (*list)->cstyle;
			    new_listed->region.x_min = 
			      region->x_min;
			    new_listed->region.x_max = 
			      (*list)->region.x_max;
			    new_listed->region.y_min = 
			      region->y_max+1;
			    new_listed->region.y_max = 
			      (*list)->region.y_max;
			    new_listed->id = This->id_func;
			    list_tmp = list;
			    while (*list_tmp && 
				   ((*list_tmp)->region.x_min <
				    new_listed->region.x_min))
			      {
				list_tmp = &((*list_tmp)->Next);
			      }
			    new_listed->Next = *list_tmp;
			    *list_tmp = new_listed;
			    if (list == list_tmp)
			      list = &(new_listed->Next);
			    (*list)->region.x_max = region->x_min-1;
			  }
		    }
		}
	      else
		{
		  /* cas 2*/
		  if (region->y_min > (*list)->region.y_min)
		    {
		      if (region->y_min <= (*list)->region.y_max)
			{
			  if (region->y_max >= (*list)->region.y_max)
			    {
			      /* cas 2.1 */
			      new_listed = (cell_style_list *)Xc_malloc
				("list_style", sizeof(cell_style_list));
			      XqCS_INC_REFERENCE((*list)->cstyle);
			      new_listed->cstyle = (*list)->cstyle;
			      new_listed->region.x_min = 
				(*list)->region.x_min;
			      new_listed->region.x_max = 
				region->x_min-1;
			      new_listed->region.y_min = 
				region->y_min;
			      new_listed->region.y_max = 
				(*list)->region.y_max;
			      new_listed->id = This->id_func;
			      list_tmp = list;
			      new_listed->Next = *list_tmp;
			      *list_tmp = new_listed;
			      list = &(new_listed->Next);
			      new_listed = (cell_style_list *)Xc_malloc
				("list_style", sizeof(cell_style_list));
			      XqCS_INC_REFERENCE((*list)->cstyle);
			      new_listed->cstyle = (*list)->cstyle;
			      new_listed->region.x_min = 
				region->x_max+1;
			      new_listed->region.x_max = 
				(*list)->region.x_max;
			      new_listed->region.y_min = 
				region->y_min;
			      new_listed->region.y_max = 
				(*list)->region.y_max;
			      new_listed->id = This->id_func;
			      list_tmp = list;
			      while (*list_tmp && 
				     ((*list_tmp)->region.x_min <
				      new_listed->region.x_min))
				{
				  list_tmp = &((*list_tmp)->Next);
				}
			      new_listed->Next = *list_tmp;
			      *list_tmp = new_listed;	
			      if (list == list_tmp)
				list = &(new_listed->Next);
			      (*list)->region.y_max = region->y_min-1;
			    }
			  else
			    {
			      /* cas 2.2 */
			      new_listed = (cell_style_list *)Xc_malloc
				("list_style", sizeof(cell_style_list));
			      XqCS_INC_REFERENCE((*list)->cstyle);
			      new_listed->cstyle = (*list)->cstyle;
			      new_listed->region.x_min = 
				(*list)->region.x_min;
			      new_listed->region.x_max = 
				region->x_min-1;
			      new_listed->region.y_min = 
				region->y_min;
			      new_listed->region.y_max = 
				region->y_max;
			      new_listed->id = This->id_func;
			      list_tmp = list;
			      new_listed->Next = *list_tmp;
			      *list_tmp = new_listed;
			      list = &(new_listed->Next);
			      new_listed = (cell_style_list *)Xc_malloc
				("list_style", sizeof(cell_style_list));
			      XqCS_INC_REFERENCE((*list)->cstyle);
			      new_listed->cstyle = (*list)->cstyle;
			      new_listed->region.x_min = 
				(*list)->region.x_min;
			      new_listed->region.x_max = 
				(*list)->region.x_max;
			      new_listed->region.y_min = 
				region->y_max+1;
			      new_listed->region.y_max = 
				(*list)->region.y_max;
			      new_listed->id = This->id_func;
			      list_tmp = list;
			      new_listed->Next = *list_tmp;
			      *list_tmp = new_listed;
			      list = &(new_listed->Next);
			      new_listed = (cell_style_list *)Xc_malloc
				("list_style", sizeof(cell_style_list));
			      XqCS_INC_REFERENCE((*list)->cstyle);
			      new_listed->cstyle = (*list)->cstyle;
			      new_listed->region.x_min = 
				region->x_max+1;
			      new_listed->region.x_max = 
				(*list)->region.x_max;
			      new_listed->region.y_min = 
				region->y_min;
			      new_listed->region.y_max = 
				region->y_max;
			      new_listed->id = This->id_func;
			      list_tmp = list;
			      while (*list_tmp && 
				     ((*list_tmp)->region.x_min <
				      new_listed->region.x_min))
				{
				  list_tmp = &((*list_tmp)->Next);
				}
			      new_listed->Next = *list_tmp;
			      *list_tmp = new_listed;
			      if (list == list_tmp)
				list = &(new_listed->Next);
			      (*list)->region.y_max = region->y_min-1;
			    }
			}
		    }
		  else
		    {
		      if (region->y_max >= (*list)->region.y_max)
			{
			  /* cas 2.3*/
			  new_listed = (cell_style_list *)Xc_malloc
			    ("list_style", sizeof(cell_style_list));
			  XqCS_INC_REFERENCE((*list)->cstyle);
			  new_listed->cstyle = (*list)->cstyle;
			  new_listed->region.x_min = 
			    region->x_max+1;
			  new_listed->region.x_max = 
			    (*list)->region.x_max;
			  new_listed->region.y_min = 
			    (*list)->region.y_min;
			  new_listed->region.y_max = 
			    (*list)->region.y_max;
			  new_listed->id = This->id_func;
			  list_tmp = list;
			  while (*list_tmp && 
				 ((*list_tmp)->region.x_min <
				  new_listed->region.x_min))
			    {
			      list_tmp = &((*list_tmp)->Next);
			    }
			  new_listed->Next = *list_tmp;
			  *list_tmp = new_listed;
			  if (list == list_tmp)
			    list = &(new_listed->Next);
			  (*list)->region.x_max = region->x_min-1;
			}
		      else
			if (region->y_max >= (*list)->region.y_min)
			  {
			    /* cas 2.4*/
			    new_listed = (cell_style_list *)Xc_malloc
			      ("list_style", sizeof(cell_style_list));
			    XqCS_INC_REFERENCE((*list)->cstyle);
			    new_listed->cstyle = (*list)->cstyle;
			    new_listed->region.x_min = 
			      (*list)->region.x_min;
			    new_listed->region.x_max = 
			      region->x_min-1;
			    new_listed->region.y_min = 
			      (*list)->region.y_min;
			    new_listed->region.y_max = 
			      region->y_max;
			    new_listed->id = This->id_func;
			    list_tmp = list;
			    new_listed->Next = *list_tmp;
			    *list_tmp = new_listed;
			    list = &(new_listed->Next);
			    new_listed = (cell_style_list *)Xc_malloc
			      ("list_style", sizeof(cell_style_list));
			    XqCS_INC_REFERENCE((*list)->cstyle);
			    new_listed->cstyle = (*list)->cstyle;
			    new_listed->region.x_min = 
			      region->x_max+1;
			    new_listed->region.x_max = 
			      (*list)->region.x_max;
			    new_listed->region.y_min = 
			      (*list)->region.y_min;
			    new_listed->region.y_max = 
			      region->y_max;
			    new_listed->id = This->id_func;
			    list_tmp = list;
			    while (*list_tmp && 
				   ((*list_tmp)->region.x_min <
				    new_listed->region.x_min))
			      {
				list_tmp = &((*list_tmp)->Next);
			      }
			    new_listed->Next = *list_tmp;
			    *list_tmp = new_listed;
			    if (list == list_tmp)
			      list = &(new_listed->Next);
			    (*list)->region.y_min = region->y_max+1;
			  }
		    }
		}
	    }
	}
      else
	{
	  if (region->x_max >= (*list)->region.x_max)
	    {
	      /* cas 3*/
	      if (region->y_min > (*list)->region.y_min)
		{
		  if (region->y_min <= (*list)->region.y_max)
		    {
		      if (region->y_max >= (*list)->region.y_max)
			{
			  /* cas 3.1 */
			  (*list)->region.y_max = region->y_min-1;
			}
		      else
			{
			  /* cas 3.2 */
			  new_listed = (cell_style_list *)Xc_malloc
			    ("list_style", sizeof(cell_style_list));
			  XqCS_INC_REFERENCE((*list)->cstyle);
			  new_listed->cstyle = (*list)->cstyle;
			  new_listed->region.x_min = 
			    (*list)->region.x_min;
			  new_listed->region.x_max = 
			    (*list)->region.x_max;
			  new_listed->region.y_min = 
			    region->y_max+1;
			  new_listed->region.y_max = 
			    (*list)->region.y_max;
			  new_listed->id = This->id_func;
			  list_tmp = list;
			  new_listed->Next = *list_tmp;
			  *list_tmp = new_listed;
			  list = &(new_listed->Next);
			  (*list)->region.y_max = region->y_min-1;
			}
		    }
		}
	      else
		{
		  if (region->y_max >= (*list)->region.y_max)
		    {
		      /* cas 3.3*/
		      new_listed = *list;
		      *list = (*list)->Next;
		      F(This->BaseStd->cell_style_base).
			RemoveCellStyleFromBase(This->BaseStd->cell_style_base,
						new_listed->cstyle);
		      progress_ok = 1;
		      Xc_free(new_listed);
		    }
		  else
		    if (region->y_max >= (*list)->region.y_min)
		      {
			/* cas 3.4*/
			(*list)->region.y_min = region->y_max+1;
		      }
		}
	    }
	  else
	    if (region->x_max >= (*list)->region.x_min)
	      {
		/* cas 4*/
		if (region->y_min > (*list)->region.y_min)
		  {
		    if (region->y_min <= (*list)->region.y_max)
		      {
			if (region->y_max >= (*list)->region.y_max)
			  {
			    /* cas 4.1 */
			    new_listed = (cell_style_list *)Xc_malloc
			      ("list_style", sizeof(cell_style_list));
			    XqCS_INC_REFERENCE((*list)->cstyle);
			    new_listed->cstyle = (*list)->cstyle;
			    new_listed->region.x_min = 
			      region->x_max+1;
			    new_listed->region.x_max = 
			      (*list)->region.x_max;
			    new_listed->region.y_min = 
			      region->y_min;
			    new_listed->region.y_max = 
			      (*list)->region.y_max;
			    new_listed->id = This->id_func;
			    list_tmp = list;
			    while (*list_tmp && 
				   ((*list_tmp)->region.x_min <
				    new_listed->region.x_min))
			      {
				list_tmp = &((*list_tmp)->Next);
			      }
			    new_listed->Next = *list_tmp;
			    *list_tmp = new_listed;
			    if (list == list_tmp)
			      list = &(new_listed->Next);
			    (*list)->region.y_max = region->y_min-1;
			  }
			else
			  {
			    /* cas 4.2 */
			    new_listed = (cell_style_list *)Xc_malloc
			      ("list_style", sizeof(cell_style_list));
			    XqCS_INC_REFERENCE((*list)->cstyle);
			    new_listed->cstyle = (*list)->cstyle;
			    new_listed->region.x_min = 
			      (*list)->region.x_min;
			    new_listed->region.x_max = 
			      (*list)->region.x_max;
			    new_listed->region.y_min = 
			      region->y_max+1;
			    new_listed->region.y_max = 
			      (*list)->region.y_max;
			    new_listed->id = This->id_func;
			    list_tmp = list;
			    new_listed->Next = *list_tmp;
			    *list_tmp = new_listed;
			    list = &(new_listed->Next);
			    new_listed = (cell_style_list *)Xc_malloc
			      ("list_style", sizeof(cell_style_list));
			    XqCS_INC_REFERENCE((*list)->cstyle);
			    new_listed->cstyle = (*list)->cstyle;
			    new_listed->region.x_min = 
			      region->x_max+1;
			    new_listed->region.x_max = 
			      (*list)->region.x_max;
			    new_listed->region.y_min = 
			      region->y_min;
			    new_listed->region.y_max = 
			      region->y_max;
			    new_listed->id = This->id_func;
			    list_tmp = list;
			    while (*list_tmp && 
				   ((*list_tmp)->region.x_min <
				    new_listed->region.x_min))
			      {
				list_tmp = &((*list_tmp)->Next);
			      }
			    new_listed->Next = *list_tmp;
			    *list_tmp = new_listed;
			    if (list == list_tmp)
			      list = &(new_listed->Next);
			    (*list)->region.y_max = region->y_min-1;
			  }
		      }
		  }
		else
		  {
		    if (region->y_max >= (*list)->region.y_max)
		      {
			/* cas 4.3*/
			(*list)->region.x_min = region->x_max+1;
			new_listed = *list;
			*list = (*list)->Next;
			progress_ok = 1;
			list_tmp = list;
			while (*list_tmp && 
			       ((*list_tmp)->region.x_min <
				new_listed->region.x_min))
			  {
			    list_tmp = &((*list_tmp)->Next);
			  }
			new_listed->Next = *list_tmp;
			*list_tmp = new_listed;
		      }
		    else
		      if (region->y_max >= (*list)->region.y_min)
			{
			  /* cas 4.4*/
			  new_listed = (cell_style_list *)Xc_malloc
			    ("list_style", sizeof(cell_style_list));
			  XqCS_INC_REFERENCE((*list)->cstyle);
			  new_listed->cstyle = (*list)->cstyle;
			  new_listed->region.x_min = 
			    region->x_max+1;
			  new_listed->region.x_max = 
			    (*list)->region.x_max;
			  new_listed->region.y_min = 
			    (*list)->region.y_min;
			  new_listed->region.y_max = 
			    region->y_max;
			  new_listed->id = This->id_func;
			  list_tmp = list;
			  while (*list_tmp && 
				 ((*list_tmp)->region.x_min <
				  new_listed->region.x_min))
			    {
			      list_tmp = &((*list_tmp)->Next);
			    }
			  new_listed->Next = *list_tmp;
			  *list_tmp = new_listed;
			  if (list == list_tmp)
			    list = &(new_listed->Next);
			  (*list)->region.y_min = region->y_max+1;
			}
		  }
	      }
	}
      if (!progress_ok)
	{
	  if (*list)
	    {
	      list = &((*list)->Next);
	    }
	  else
	    break;
	}
      else
	progress_ok = 0;
    }
}

/* ----------------------------------------------------------------- ** 
** view_style - show style for debugging (internal)                  ** 
** ----------------------------------------------------------------- */
static void view_style(This)
c_Moteur	*This;
{
  cell_style_list	*list_tmp;

  list_tmp = This->style_list;
  while (list_tmp)
    {
      Xc_TRACE(("xmin: %d, xmax:%d, ymin:%d, ymax:%d", 
		list_tmp->region.x_min, list_tmp->region.x_max,
		list_tmp->region.y_min, list_tmp->region.y_max));
      list_tmp = list_tmp->Next;
    }
}

/* ----------------------------------------------------------------- ** 
** set_cell_style - set style in region                              ** 
** ----------------------------------------------------------------- */
static void Moteur_set_cell_style2(This, cstyle, region)
c_Moteur	*This;
CellStyle_t	*cstyle;
region_t	*region;
{
  cell_style_list	**list_tmp;
  cell_style_list	*new_listed;

  Xc_TRACE(("cell style 2"));
  Xc_TRACE(("xmin: %d, xmax:%d, ymin:%d, ymax:%d", 
	    region->x_min, region->x_max,
	    region->y_min, region->y_max));
  frag_style(This, region);
  Xc_TRACE(("Set: %d", cstyle->down.border_thickness));
  new_listed = (cell_style_list *)Xc_malloc
    ("list_style", sizeof(cell_style_list));
  new_listed->cstyle = cstyle;
  new_listed->region.x_min = 
    region->x_min;
  new_listed->region.x_max = 
    region->x_max;
  new_listed->region.y_min = 
    region->y_min;
  new_listed->region.y_max = 
    region->y_max;
  new_listed->id = This->id_func;
  list_tmp = &(This->style_list);
  while (*list_tmp && 
	 ((*list_tmp)->region.x_min <
	  new_listed->region.x_min))
    {
      list_tmp = &((*list_tmp)->Next);
    }
  new_listed->Next = *list_tmp;
  *list_tmp = new_listed;  
}

/* ----------------------------------------------------------------- ** 
** get_cell_style - get style from cell x,y                          ** 
** ----------------------------------------------------------------- */
static CellStyle_t *Moteur_get_cell_style(This, x, y)
c_Moteur	*This;
int		x;
int		y;
{
  cell_style_list	*list_tmp;

  list_tmp = This->style_list;
  while (list_tmp)
    {
      if ((x>=list_tmp->region.x_min) && (x<=list_tmp->region.x_max) &&
	  (y>=list_tmp->region.y_min) && (y<=list_tmp->region.y_max))
	{
	  return list_tmp->cstyle;
	}
      list_tmp = list_tmp->Next;
    }
  return This->BaseStd->cell_style_base->default_cstyle;
}

/* ----------------------------------------------------------------- ** 
** or_cell_style - or style in region                                ** 
** ----------------------------------------------------------------- */
static void Moteur_or_cell_style(This, region, func, data)
c_Moteur	*This;
region_t	*region;
void		(*func)();
void		*data;
{
  cell_style_list	*list_tmp;
  region_t		tmp_reg;

  Xc_TRACE(("or style"));
  list_tmp = This->style_list;
  This->id_func ++;
  while (list_tmp)
    { 
      if (list_tmp->id == This->id_func)
	{
	  list_tmp = list_tmp->Next;
	  continue;
	}
      if (((list_tmp->region.x_min >= region->x_min) &&
	   (list_tmp->region.x_min <= region->x_max)) ||
	  ((list_tmp->region.x_max >= region->x_min) &&
	   (list_tmp->region.x_max <= region->x_max)) ||
	  ((list_tmp->region.x_min <= region->x_min) &&
	   (list_tmp->region.x_max >= region->x_max)))
	{
	  if (((list_tmp->region.y_min >= region->y_min) &&
	       (list_tmp->region.y_min <= region->y_max)) ||
	      ((list_tmp->region.y_max >= region->y_min) &&
	       (list_tmp->region.y_max <= region->y_max)) ||
	      ((list_tmp->region.y_min <= region->y_min) &&
	       (list_tmp->region.y_max >= region->y_max)))
	    {
	      Xc_TRACE(("func or call"));
	      tmp_reg.x_min = MAX(list_tmp->region.x_min, region->x_min);
	      tmp_reg.x_max = MIN(list_tmp->region.x_max, region->x_max);
	      tmp_reg.y_min = MAX(list_tmp->region.y_min, region->y_min);
	      tmp_reg.y_max = MIN(list_tmp->region.y_max, region->y_max);
	      func(This, list_tmp->cstyle, &tmp_reg, data);
	      list_tmp = This->style_list;
	      continue;
	    }
	  else
	    list_tmp->id = This->id_func;
	}
      else
	list_tmp->id = This->id_func;
      list_tmp = list_tmp->Next;
    }
}

/* ----------------------------------------------------------------- ** 
** move_style - move style region                                    ** 
** ----------------------------------------------------------------- */
static int Moteur_move_style(This, region, x, y)
c_Moteur	*This;
region_t	*region;
int		x;
int		y;
{
  int		deltacol;
  int		deltaline;

  cell_style_list	*list;
  cell_style_list	*list_start;
  cell_style_list	*list_stmp;
  cell_style_list	**list_tmp;
  CellStyle_t		*default_style;
  int			min_x;
  int			min_y;
  
  Xc_TRACE(("move style"));
  Xc_TRACE(("xmin: %d, xmax:%d, ymin:%d, ymax:%d", 
	    region->x_min, region->x_max, region->y_min, region->y_max));

  deltacol = x - region->x_min;
  deltaline = y - region->y_min;

  list = This->style_list;
  list_start = NULL;
  list_tmp = &list_start;

  while (list && (list->region.x_min <= region->x_max))
    {
      if (list->region.x_max < region->x_min)
	{
	  list = list->Next;
	  continue;
	}
      if ((list->region.y_max < region->y_min) ||
	  (list->region.y_min > region->y_max))
	{
	  list = list->Next;
	  continue;
	}

      min_x = MAX(list->region.x_min, region->x_min) + deltacol;
      if (min_x > COLUMN_MAX)
	{
	  list = list->Next;
	  continue;
	}
      min_y = MAX(list->region.y_min, region->y_min) + deltaline;
      if (min_y > ROW_MAX)
	{
	  list = list->Next;
	  continue;
	}

      /* store the exact reference */
      while (*list_tmp)
	{
	  list_tmp = &((*list_tmp)->Next);
	}
      *list_tmp =(cell_style_list *)Xc_malloc
	("list_style", sizeof(cell_style_list));

      /* this region instersect, compute intersection */
      /* and translate this region */
      (*list_tmp)->region.x_min = min_x;
      (*list_tmp)->region.x_max = MIN(list->region.x_max, region->x_max)
	+ deltacol;
      if ((*list_tmp)->region.x_max > COLUMN_MAX)
	(*list_tmp)->region.x_max = COLUMN_MAX;

      (*list_tmp)->region.y_min = min_y;
      (*list_tmp)->region.y_max = MIN(list->region.y_max, region->y_max)
	+ deltaline;
      if ((*list_tmp)->region.y_max > ROW_MAX)
	(*list_tmp)->region.y_max = ROW_MAX;

#if 0
      (*list_tmp)->region.x_min = min_x;
      if (list->region.x_max == COLUMN_MAX)
	{
	  (*list_tmp)->region.x_max = COLUMN_MAX;
	}
      else
	{
	  (*list_tmp)->region.x_max = MIN(list->region.x_max, region->x_max)
	    + deltacol;
	  if ((*list_tmp)->region.x_max > COLUMN_MAX)
	    (*list_tmp)->region.x_max = COLUMN_MAX;
	}

      (*list_tmp)->region.y_min = min_y;
      if (list->region.y_max == ROW_MAX)
	{
	  (*list_tmp)->region.y_max = ROW_MAX;
	}
      else
	{
	  (*list_tmp)->region.y_max = MIN(list->region.y_max, region->y_max)
	    + deltaline;
	  if ((*list_tmp)->region.y_max > ROW_MAX)
	    (*list_tmp)->region.y_max = ROW_MAX;
	}
#endif

      XqCS_INC_REFERENCE(list->cstyle);
      (*list_tmp)->cstyle = list->cstyle;
      (*list_tmp)->Next = NULL;
      list = list->Next;
    }
  
  default_style = F(This->BaseStd->cell_style_base).AddCellStyleToBase
    (This->BaseStd->cell_style_base,
     This->BaseStd->cell_style_base->default_cstyle);
  F(This).set_cell_style2(This,
			  default_style,
			  region);

  /* now put back this regions */
  while (list_start)
    {
      F(This).set_cell_style2(This, list_start->cstyle, &(list_start->region));
      list_stmp = list_start->Next;
      Xc_free(list_start);
      list_start = list_stmp;
    }
  return 1;
}

/* ----------------------------------------------------------------- ** 
** copy_style - copy style region                                    ** 
** ----------------------------------------------------------------- */
static int Moteur_copy_style(This, region, x, y)
c_Moteur	*This;
region_t	*region;
int		x;
int		y;
{
  int		deltacol;
  int		deltaline;

  cell_style_list	*list;
  cell_style_list	*list_start;
  cell_style_list	*list_stmp;
  cell_style_list	**list_tmp;
  
  Xc_TRACE(("copy style"));

  deltacol = x - region->x_min;
  deltaline = y - region->y_min;

  list = This->style_list;
  list_start = NULL;
  list_tmp = &list_start;

  while (list && (list->region.x_min <= region->x_max))
    {
      if (list->region.x_max < region->x_min)
	{
	  list = list->Next;
	  continue;
	}
      if ((list->region.y_max < region->y_min) ||
	  (list->region.y_min > region->y_max))
	{
	  list = list->Next;
	  continue;
	}
      /* store the exact reference */
      while (*list_tmp)
	{
	  list_tmp = &((*list_tmp)->Next);
	}
      *list_tmp =(cell_style_list *)Xc_malloc
	("list_style", sizeof(cell_style_list));

      /* this region instersect, compute intersection */
      /* and translate this region */
      (*list_tmp)->region.x_min = MAX(list->region.x_min, region->x_min) 
	+ deltacol;
      (*list_tmp)->region.x_max = MIN(list->region.x_max, region->x_max)
	+ deltacol;
      (*list_tmp)->region.y_min = MAX(list->region.y_min, region->y_min)
	+ deltaline;
      (*list_tmp)->region.y_max = MIN(list->region.y_max, region->y_max)
	+ deltaline;
      XqCS_INC_REFERENCE(list->cstyle);
      (*list_tmp)->cstyle = list->cstyle;
      (*list_tmp)->Next = NULL;
      list = list->Next;
    }
  
  /* now put back this regions */
  while (list_start)
    {
      F(This).set_cell_style2(This, list_start->cstyle, &(list_start->region));
      list_stmp = list_start->Next;
      Xc_free(list_start);
      list_start = list_stmp;
    }
  return 1;
}

/* ----------------------------------------------------------------- ** 
** translate - translate indexing of cell (internal)                 ** 
** ----------------------------------------------------------------- */
static void translate(src, dest, dx, dy)
char 	*src;
char 	**dest;
int	dx;
int	dy;
{
  char	buffer[500];
  int	sindex;
  int	dindex;
  int	next_char;
  int	donottrans_col;
  int	donottrans_lin;
  int	col_char;
  int	lin_char;
  int	end_col;
  int	str_lin;
  int	j,k,n;
  int	new_x;
  int	new_y;
  char	aff[8];
  int	indext;
  
  
  Xc_HISTORY(("translate formula %s", src));
  sindex = 0;
  dindex = 0;
  
  while (src[sindex]!=0)
    {
      while (((src[sindex] < 'a') ||
	      (src[sindex] > 'z')) &&
	     ((src[sindex] < 'A') ||
	      (src[sindex] > 'Z')) &&
	     (src[sindex] != '$') &&
	     (src[sindex] != '\"') &&
	     (src[sindex] != 0))
	{
	  buffer[dindex++] = src[sindex++];
	}
      
      if (src[sindex] == '\"')
	{
	  buffer[dindex++] = src[sindex++];
	  while ((src[sindex] != '\"') &&
		 (src[sindex] != 0))
	    buffer[dindex++] = src[sindex++];
	  if (src[sindex] == '\"')
	    {
	      buffer[dindex++] = src[sindex++];
	      continue;
	    }
	  else
	    break;
	}

      if (src[sindex] == 0)
	break;
      
      next_char = 1;
      if (src[sindex] == '$')
	{
	  donottrans_col = 1;
	  col_char = 0;
	}
      else
	{
	  donottrans_col = 0;
	  col_char = 1;
	}
      
      while (col_char < 2)
	{
	  if (((src[sindex+next_char] >= 'a') &&
	       (src[sindex+next_char] <= 'z')) ||
	      ((src[sindex+next_char] >= 'A') &&
	       (src[sindex+next_char] <= 'Z')))
	    {
	      col_char ++;
	      next_char ++;
	    }
	  else
	    break;
	}

      if (col_char == 0)
	{
	  /* garbage after dollar */
	  while(next_char)
	    {
	      buffer[dindex++] = src[sindex++];
	      next_char--;
	    }
	  continue;
	}

      if (((src[sindex+next_char] >= 'a') &&
	   (src[sindex+next_char] <= 'z')) ||
	  ((src[sindex+next_char] >= 'A') &&
	   (src[sindex+next_char] <= 'Z')))
	{
	  /* trop de lettre : copy toute celle tester*/
	  while(next_char)
	    {
	      buffer[dindex++] = src[sindex++];
	      next_char--;
	    }
	  /* finit toute les lettres */
	  while (((src[sindex] >= 'a') &&
		  (src[sindex] <= 'z')) ||
		 ((src[sindex] >= 'A') &&
		  (src[sindex] <= 'Z')))
	    buffer[dindex++] = src[sindex++];
	  continue;
	}
      end_col = next_char;
      str_lin = next_char;
      
      if (src[sindex+next_char] == '$')
	{
	  donottrans_lin = 1;
	  next_char ++;
	}
      else
	donottrans_lin = 0;

      lin_char = 0;
      while ((src[sindex+next_char] >= '0') &&
	     (src[sindex+next_char] <= '9'))
	{
	  next_char++;
	  lin_char++;
	}
      if (lin_char)
	{
	  /* ok translate now!!*/
	  if (donottrans_col)
	    {
	      while(end_col)
		{
		  buffer[dindex++] = src[sindex++];
		  end_col--;
		  next_char--;
		}
	    }
	  else
	    {
	      new_x = 0;
	      j = col_char-1;
	      k = 0;
	      n = 1;
	      while(j>=k)
		{
		  if (src[sindex+j] < 'a')
		    new_x += (src[sindex+j]-'A'+(n==1?0:1))*n;
		  else
		    new_x += (src[sindex+j]-'a'+(n==1?0:1))*n;
		  n = n*26;
		  j--;
		}
	      next_char -= col_char;
	      sindex += col_char;
	      new_x += dx;
	      if (new_x < 0)
		new_x = 0;
	      if (new_x > COLUMN_MAX)
		new_x = COLUMN_MAX;
	      Xc_TRACE(("foundx %d", new_x));
	      indext = 6;
	      do
		{
		  aff[indext]=(new_x % 26)+'A';
		  indext--;
		  new_x /= 26;
		}while(new_x--);
	      aff[7] = '\0';
	      sprintf(buffer+dindex, "%s", aff+indext+1);
	      dindex += strlen (buffer+dindex);
	    }

	  if (donottrans_lin)
	    {
	      while(next_char)
		{
		  buffer[dindex++] = src[sindex++];
		  next_char--;
		}
	    }
	  else
	    {
	      new_y=atoi(src+sindex)+dy;
	      if (new_y < 1)
		new_y = 1;
	      if (new_y > ROW_MAX)
		new_y = ROW_MAX;
	      
	      sindex += next_char;
	      sprintf(buffer+dindex, "%d", new_y);
	      dindex += strlen (buffer+dindex);
	      Xc_TRACE(("foundy %d", new_y));
	    }
	}
      else
	{
	  while(next_char)
	    {
	      buffer[dindex++] = src[sindex++];
	      next_char--;
	    }
	}
    }
  buffer[dindex] = 0;
  Xc_TRACE(("result %s", buffer));
  *dest = (char *)Xc_malloc("a t f", strlen(buffer)+1);
  strcpy(*dest, buffer);
}


/* ----------------------------------------------------------------- ** 
** move_zone - move region of cell                                   ** 
** ----------------------------------------------------------------- */
static int Moteur_move_zone(This, region, x, y)
c_Moteur	*This;
region_t	*region;
int		x;
int		y;
{
  int		deltacol;
  int		deltaline;
  c_col		*tmpcol;
  c_col		*lastcol;
  c_line	*tmpline;
  c_line	*lastline;
  cell_depend	*all_dep;
  HG_list	*all_gdep;

  Xc_TRACE(("move zone"));
  Xc_TRACE(("%d-%d:%d-%d", region->x_min,
	    region->y_min,
	    region->x_max,
	    region->y_max));
  all_dep = NULL;
  all_gdep = NULL;
  deltacol = x - region->x_min;
  deltaline = y - region->y_min;

  if ((deltacol == 0) && (deltaline == 0))
    return 1;

  /* 8 cas reductible a 4 : un cas par coin en fonction de la direction */
  /* handle haut gauche, haut et gauche*/
  if ((deltacol<=0) && (deltaline<=0))
    {
      /* find first col >= x_min */
      tmpcol = This->tableCL;
      while (tmpcol && (tmpcol->number < region->x_min))
	{
	  tmpcol = tmpcol->Next;
	}
      while (tmpcol && (tmpcol->number <= region->x_max))
	{
	  tmpline = tmpcol->line;
	  while (tmpline && (tmpline->number < region->y_min))
	    tmpline = tmpline->Next;
	  while (tmpline && (tmpline->number <= region->y_max))
	    {
	      lastline = tmpline->Next;
	      Moteur_move_cell(This, tmpcol->number, tmpline->number, 
			       (tmpcol->number)+deltacol, 
			       tmpline->number+deltaline, 
			       &all_dep, &all_gdep);
	      tmpline = lastline;
	    }
	  tmpcol = tmpcol->Next;
	}
    }

  /* handle bas gauche et bas */ 
  if ((deltacol<=0) && (deltaline>0))
    {
      /* find first col >= x_min */
      tmpcol = This->tableCL;
      while (tmpcol && (tmpcol->number < region->x_min))
	{
	  tmpcol = tmpcol->Next;
	}
      while (tmpcol && (tmpcol->number <= region->x_max))
	{
	  lastline = tmpcol->line;
	  while (lastline && (lastline->number <= region->y_max))
	    {
	      tmpline = lastline;
	      lastline = lastline->Next;
	    }
	  while (tmpline && (tmpline->number >= region->y_min))
	    {
	      lastline = tmpline->Prev;
	      Moteur_move_cell(This, tmpcol->number, tmpline->number, 
			       (tmpcol->number)+deltacol, 
			       tmpline->number+deltaline, 
			       &all_dep, &all_gdep);
	      tmpline = lastline;
	    }
	  tmpcol = tmpcol->Next;
	}
    }

  /* handle haut droite et droite */ 
  if ((deltacol>0) && (deltaline<=0))
    {
      /* find last col <= x_max */
      tmpcol = This->tableCL;
      lastcol = NULL;
      while (tmpcol && (tmpcol->number <= region->x_max))
	{
	  lastcol = tmpcol;
	  tmpcol = tmpcol->Next;
	}

      if (!lastcol)
	{
	  return 1;
	}

      while (lastcol && (lastcol->number >= region->x_min))
	{
	  tmpline = lastcol->line;
	  while (tmpline && (tmpline->number < region->y_min))
	    tmpline = tmpline->Next;
	  while (tmpline && (tmpline->number <= region->y_max))
	    {
	      lastline = tmpline->Next;
	      Moteur_move_cell(This, lastcol->number, tmpline->number, 
			       (lastcol->number)+deltacol, 
			       tmpline->number+deltaline, 
			       &all_dep, &all_gdep);
	      tmpline = lastline;
	    }
	  lastcol = lastcol->Prev;
	}
    }

  /* handle bas droite */ 
  if ((deltacol>0) && (deltaline>0))
    {
      /* find last col <= x_max */
      tmpcol = This->tableCL;
      lastcol = NULL;
      while (tmpcol && (tmpcol->number <= region->x_max))
	{
	  lastcol = tmpcol;
	  tmpcol = tmpcol->Next;
	}

      if (!lastcol)
	{
	  return 1;
	}

      while (lastcol && (lastcol->number >= region->x_min))
	{
	  lastline = lastcol->line;
	  while (lastline && (lastline->number <= region->y_max))
	    {
	      tmpline = lastline;
	      lastline = lastline->Next;
	    }
	  while (tmpline && (tmpline->number >= region->y_min))
	    {
	      lastline = tmpline->Prev;
	      Moteur_move_cell(This, lastcol->number, tmpline->number, 
			       (lastcol->number)+deltacol, 
			       tmpline->number+deltaline, 
			       &all_dep, &all_gdep);
	      tmpline = lastline;
	    }
	  lastcol = lastcol->Prev;
	}
    }

  pop_depend(This, all_dep, &all_gdep);
  return 1;
}

/* ----------------------------------------------------------------- ** 
** copy_zone - copy_zone of cells                                    ** 
** ----------------------------------------------------------------- */
static int Moteur_copy_zone(This, region, x, y)
c_Moteur	*This;
region_t	*region;
int		x;
int		y;
{
  c_col		*tmpcol;
  c_line	*tmpline;
  cell		*tcell;
  char		*tmpptr;
  cell_depend	*all_dep;
  HG_list	*all_gdep;

  tmpcol = This->tableCL;
  all_dep = NULL;
  all_gdep = NULL;
  while (tmpcol && (tmpcol->number < region->x_min))
    {
      tmpcol = tmpcol->Next;
    }
  while (tmpcol && (tmpcol->number <= region->x_max))
    {
      if (x+(tmpcol->number-region->x_min) > COLUMN_MAX)
	break;

      tmpline = tmpcol->line;
      while (tmpline && (tmpline->number < region->y_min))
	{
	  tmpline = tmpline->Next;
	}
      if (!tmpline)
	{
	  tmpcol = tmpcol->Next;
	  continue;
	}

      while (tmpline && (tmpline->number <= region->y_max))
	{
	  if (y+(tmpline->number-region->y_min) > ROW_MAX)
	    break;

	  /* a copier !! */
	  /* eventuellement dans un tableau temporaire */
	  /* pour permettre les intersection */
	  Xc_TRACE(("copie %d-%d",
		    x+(tmpcol->number - region->x_min),
		    y+(tmpline->number - region->y_min)));
	  tcell = F(This).add_cell(This,
				   y+(tmpline->number - region->y_min),
				   x+(tmpcol->number - region->x_min));
	  if (tmpline->cell.text)
	    {
	      tcell->text = Xc_malloc("c_text", strlen(tmpline->cell.text)+1);
	      strcpy(tcell->text, tmpline->cell.text);
	    }
	  tcell->type = tmpline->cell.type;
	  switch (tcell->type)
	    {
	    case Xq_FORMULA:
	    case Xq_FORMERR:
	      if (tmpline->cell.formula)
		{
		  translate(tmpline->cell.formula, &tmpptr,
			    x - region->x_min,
			    y - region->y_min);
		  This->force = 1;
		  F(This).set_string(This, 
				     x+(tmpcol->number - region->x_min),
				     y+(tmpline->number - region->y_min),
				     tcell,
				     tmpptr,
				     GlobEditBar->sel_feuille);
		  Xc_free(tmpptr);
		}
	      break;
	    case Xq_VOID:
	    case Xq_TEXT:
	      break;
	    case Xq_VALUE:
	      if (tmpline->cell.value2)
		{
		  gduplicate(tmpline->cell.value2, &(tcell->value2));
		  tcell->ret_type = tmpline->cell.ret_type;
		}
	    }
	  store_depend(tcell->depend, &all_dep,
		       tcell->graph_list, &all_gdep);
	  tmpline = tmpline->Next;
	}
      tmpcol = tmpcol->Next;
    }
  pop_depend(This, all_dep, &all_gdep);
  return 1;
}

/* ----------------------------------------------------------------- ** 
** cut_zone - cut zone in clipboard                                  ** 
** ----------------------------------------------------------------- */
static void Moteur_cut_zone(This, region, mode)
c_Moteur	*This;
region_t	*region;
int		mode;
{
  Xc_HISTORY(("cut zone %d-%d : %d-%d", 
	      region->x_min, region->y_min,
	      region->x_max, region->y_max));
  memcpy(&(This->sel_region), region, sizeof(region_t));
  This->mode = mode;
}

/* ----------------------------------------------------------------- ** 
** paste_zone - paste zone from clipboard                            ** 
** ----------------------------------------------------------------- */
static region_t *Moteur_paste_zone(This, x, y)
c_Moteur	*This;
int		x;
int		y;
{
  region_t	*region;
  region_t	*nregion;
  region_t	rregion;

  if (This->mode == Xc_MODE_NOTHING)
    return NULL;

  region = &(This->sel_region);
  Xc_HISTORY(("paste zone %d-%d : %d-%d to %d-%d",
	      region->x_min, region->y_min,
	      region->x_max, region->y_max, x, y));
  

  /*--- case of no move ---*/
  if ((x == region->x_min) && (y == region->y_min))
    return BOINK;

  nregion = Xc_malloc("region pasted", sizeof(region_t));
  
  /*--- cas eventuelle depassement de la zone autorise ---*/
  rregion.x_min = region->x_min;
  rregion.y_min = region->y_min;
  rregion.x_max = x+(region->x_max-region->x_min);
  if (rregion.x_max > COLUMN_MAX)
    rregion.x_max = region->x_max-(rregion.x_max-COLUMN_MAX);
  else
    rregion.x_max = region->x_max;

  rregion.y_max = y+(region->y_max-region->y_min);
  if (rregion.y_max > ROW_MAX)
    rregion.y_max = region->y_max-(rregion.y_max-COLUMN_MAX);
  else
    rregion.y_max = region->y_max;
  nregion->x_min = x;
  nregion->y_min = y;
  nregion->x_max = x+(rregion.x_max-rregion.x_min);
  nregion->y_max = y+(rregion.y_max-rregion.y_min);

  if (This->mode == Xc_MODE_CUT)
    {
      Xc_TRACE(("cut-paste"));
      nregion->NextRegion = Xc_malloc("region cuted", sizeof(region_t));
      nregion->NextRegion->x_min = region->x_min;
      nregion->NextRegion->y_min = region->y_min;
      nregion->NextRegion->x_max = region->x_max;
      nregion->NextRegion->y_max = region->y_max;
      nregion->NextRegion->NextRegion = NULL;
      F(This).move_style(This, &rregion, x, y);
      F(This).move_zone(This, &rregion, x, y);
    }
  else
    {
      Xc_TRACE(("copy-paste"));
      nregion->NextRegion = NULL;
      F(This).copy_style(This, &rregion, x, y);
      F(This).copy_zone(This, &rregion, x, y);
    }
  return nregion;
}

/* ----------------------------------------------------------------- ** 
** delete_zone - delete zone                                         ** 
** ----------------------------------------------------------------- */
static void Moteur_delete_zone(This, region)
c_Moteur	*This;
region_t	*region;
{
  c_col		*tmpcol;
  c_line	**tmplines;
  c_line	*tmpline;
  c_line	*tmpline2;
  c_line	*oldline;
  cell_depend	*all_dep;
  HG_list	*all_gdep;
  
  Xc_HISTORY(("moteur delete zone"));
  Xc_TRACE(("%d-%d:%d-%d", region->x_min,
	    region->y_min,
	    region->x_max,
	    region->y_max));
  all_dep = NULL;
  all_gdep = NULL;
  tmpcol = This->tableCL;
  while (tmpcol && (tmpcol->number < region->x_min))
    {
      tmpcol = tmpcol->Next;
    }

  while (tmpcol && (tmpcol->number <= region->x_max))
    {
      tmplines = &(tmpcol->line);
      oldline = NULL;

      while (*tmplines && ((*tmplines)->number < region->y_min))
	{
	  oldline = *tmplines;
	  tmplines = &(*tmplines)->Next;
	}

      if (*tmplines)
	{
	  tmpcol->last_pointed = NULL;
	  tmpcol->last_added = NULL;
	}

      while (*tmplines && ((*tmplines)->number <= region->y_max))
	{
	  tmpline = *tmplines;
	  tmpline2 = tmpline->Next;
	  if (tmpline->cell.formula)
	    {
	      F(This->parser).parse(This->parser, tmpline->cell.formula+1,
				    tmpcol->number, 
				    tmpline->number, 
				    &(tmpline->cell), SUB_DEP, 
				    GlobEditBar->sel_feuille);
	      Xc_free(tmpline->cell.formula);
	      tmpline->cell.formula = NULL;
	    }
	  if (tmpline->cell.text)
	    {
	      Xc_free(tmpline->cell.text);
	      tmpline->cell.text = NULL;
	    }
	  if (tmpline->cell.depend || tmpline->cell.graph_list)
	    {
	      tmpline->cell.type = Xq_VOID;
	      if (tmpline->cell.value2)
		{
		  Xc_free(tmpline->cell.value2);
		  tmpline->cell.value2 = NULL;
		}
	      store_depend(tmpline->cell.depend, &all_dep,
			   tmpline->cell.graph_list, &all_gdep);
	      tmplines = &(*tmplines)->Next;
	      continue;
	    }
	  Xc_free(tmpline);
	  tmpline = tmpline2;
	  if (tmpline)
	    tmpline->Prev = oldline;
	  *tmplines = tmpline;
	}
      tmpcol = tmpcol->Next;
    }
  pop_depend(This, all_dep, &all_gdep);
}

/* ----------------------------------------------------------------- ** 
** copy_zone_r_or_d - copy zone right or down                        ** 
** ----------------------------------------------------------------- */
static void copy_zone_r_or_d(This, type, region)
c_Moteur	*This;
int		type;
region_t	*region;
{
  c_line **tmpline;
  c_line *prevline;
  c_line *nextline;
  int	line_num;
  int	add_line;
  cell	*cellp;
  char	*tmpptr;
  cell_depend	*all_dep;
  HG_list	*all_gdep;
  region_t	region_style;
  int		num_style;
  
  all_dep = NULL;
  all_gdep = NULL;  
  if (type == Xc_LINE)
    {
      c_col	*tmpcol;

      Xc_TRACE(("copy vers le bas region [%d-%d;%d-%d]",
		region->x_min,
		region->y_min,
		region->x_max,
		region->y_max));

      if (region->y_min == region->y_max)
	return;

      region_style.x_min = region->x_min;
      region_style.y_min = region->y_min;
      region_style.x_max = region->x_max;
      region_style.y_max = region->y_min;
      num_style = region->y_min+1;
      
      while (num_style <= region->y_max)
	{
	  F(This).copy_style(This, &region_style, region->x_min, num_style);
	  num_style++;
	}
      
      tmpcol = This->tableCL;
      while (tmpcol && (tmpcol->number < region->x_min))
	{
	  tmpcol = tmpcol->Next;
	}

      while (tmpcol && (tmpcol->number <= region->x_max))
	{
	  tmpline = &(tmpcol->line);
	  line_num = region->y_min;
	  prevline = NULL;
	  while (*tmpline && ((*tmpline)->number < line_num))
	    {
	      prevline = *tmpline;
	      tmpline = &((*tmpline)->Next);
	    }

	  if (*tmpline)
	    {
	      if ((*tmpline)->number == line_num)
		cellp = &((*tmpline)->cell);
	      else
		cellp = NULL;
	      prevline = *tmpline;
	      tmpline = &((*tmpline)->Next);
	      line_num++;
	    }
	  else
	    {
	      tmpcol = tmpcol->Next;
	      continue;
	    }

	  while (line_num <= region->y_max)
	    {
	      if (*tmpline && ((*tmpline)->number == line_num))
		add_line = 0;
	      else
		add_line = 1;
		
	      if (add_line && cellp)
		{
		  nextline = *tmpline;
		  (*tmpline) = (c_line *)Xc_malloc("addlinemove",
						   sizeof(c_line));
		  (*tmpline)->number = line_num;
		  (*tmpline)->cell.text = NULL;
		  (*tmpline)->cell.formula = NULL;
		  (*tmpline)->cell.status = 0;
		  (*tmpline)->cell.value2 = NULL;
		  (*tmpline)->cell.depend = NULL;
		  (*tmpline)->cell.graph_list = NULL;
		  (*tmpline)->Next = nextline;
		  (*tmpline)->Prev = prevline;
		  if (nextline)
		    nextline->Prev = *tmpline;
		  attach_depend(This, &((*tmpline)->cell.depend),
				tmpcol->number, line_num);
		}
	      if (!add_line && !cellp)
		{
		  nextline = *tmpline;
		  if (nextline->cell.formula)
		    {
		      F(This->parser).parse(This->parser,
					    nextline->cell.formula+1,
					    tmpcol->number,
					    line_num,
					    &(nextline->cell),
					    SUB_DEP, GlobEditBar->sel_feuille);
		      Xc_free(nextline->cell.formula);
		      nextline->cell.formula = NULL;
		    }
		  if (nextline->cell.text)
		    {
		      Xc_free(nextline->cell.text);
		      nextline->cell.text = NULL;
		    }
		  if (nextline->cell.depend || nextline->cell.graph_list)
		    {
		      nextline->cell.type = Xq_VOID;
		      if (nextline->cell.value2)
			{
			  Xc_free(nextline->cell.value2);
			  nextline->cell.value2 = NULL;
			}
		      store_depend(nextline->cell.depend, &all_dep,
				   nextline->cell.graph_list, &all_gdep);
		    }
		  else
		    {
		      *tmpline = (*tmpline)->Next;
		      if (*tmpline)
			(*tmpline)->Prev = nextline->Prev;
		      tmpcol->last_pointed=NULL;
		      tmpcol->last_added=NULL;
		      Xc_free(nextline);
		    }
		}

	      if (cellp)
		{
		  if ((*tmpline)->cell.formula)
		    {
		      F(This->parser).parse(This->parser,
					    (*tmpline)->cell.formula+1,
					    tmpcol->number, 
					    (*tmpline)->number, 
					    &((*tmpline)->cell), SUB_DEP, 
					    GlobEditBar->sel_feuille);
		      Xc_free((*tmpline)->cell.formula);
		      (*tmpline)->cell.formula = NULL;
		    }
		  if ((*tmpline)->cell.text)
		    {
		      Xc_free((*tmpline)->cell.text);
		      (*tmpline)->cell.text = NULL;
		    }
		  if ((*tmpline)->cell.depend || (*tmpline)->cell.graph_list)
		    {
		      store_depend((*tmpline)->cell.depend, &all_dep,
				   (*tmpline)->cell.graph_list, &all_gdep);
		    }

		  (*tmpline)->cell.type = cellp->type;
		  if ((*tmpline)->cell.value2)
		    {
		      Xc_free((*tmpline)->cell.value2);
		      (*tmpline)->cell.value2 = NULL;
		    }
		  switch (cellp->type)
		    {
		    case Xq_VOID:
		      break;
		    case Xq_FORMULA:
		    case Xq_FORMERR:
		      if (cellp->formula)
			{
			  translate(cellp->formula, &tmpptr,
				    0,
				    line_num - region->y_min);
			  F(This).set_string(This, 
					     tmpcol->number,
					     line_num,
					     &((*tmpline)->cell),
					     tmpptr,
					     GlobEditBar->sel_feuille);
			  Xc_free(tmpptr);
			}
		      break;
		    case Xq_TEXT:
		      if (cellp->text)
			{
			  (*tmpline)->cell.text = Xc_malloc
			    ("c_text", strlen(cellp->text)+1);
			  strcpy((*tmpline)->cell.text, cellp->text);
			}
		      break;
		    case Xq_VALUE:
		      if (cellp->value2)
			{
			  gduplicate(cellp->value2, 
				     &((*tmpline)->cell.value2));
			  (*tmpline)->cell.ret_type = cellp->ret_type;
			}
		    }
		}
	      line_num++;
	      prevline = *tmpline;
	      tmpline = &((*tmpline)->Next);
	    }
	  tmpcol = tmpcol->Next;
	}
    }
  else
    {
      c_col	**tmpcol;
      c_col	*prevcol;
      c_col	*nextcol;
      int       col_num;
      int	add_col;

      Xc_TRACE(("copy vers la droite region [%d-%d;%d-%d]",
		region->x_min,
		region->y_min,
		region->x_max,
		region->y_max));
      if (region->x_min == region->x_max)
	return;

      region_style.x_min = region->x_min;
      region_style.y_min = region->y_min;
      region_style.x_max = region->x_min;
      region_style.y_max = region->y_max;
      num_style = region->x_min+1;
      
      while (num_style <= region->x_max)
	{
	  F(This).copy_style(This, &region_style, num_style, region->y_min);
	  num_style++;
	}
      
      line_num = region->y_min;
      while (line_num <= region->y_max)
	{
	  tmpcol = &(This->tableCL);
	  prevcol = NULL;
	  while (*tmpcol && ((*tmpcol)->number < region->x_min))
	    {
	      prevcol = *tmpcol;
	      tmpcol = &((*tmpcol)->Next);
	    }

	  if (*tmpcol)
	    {
	      if ((*tmpcol)->number == region->x_min)
		{
		  tmpline = &((*tmpcol)->line);
		  prevline = NULL;
		  while (*tmpline && ((*tmpline)->number < line_num))
		    {
		      prevline = *tmpline;
		      tmpline = &((*tmpline)->Next);
		    }
		  if (*tmpline && ((*tmpline)->number == line_num))
		    cellp = &((*tmpline)->cell);
		  else
		    cellp = NULL;
		}
	      else
		cellp = NULL;
	    }
	  else
	    return;

	  col_num = region->x_min+1;
	  while (*tmpcol && ((*tmpcol)->number < col_num))
	    {
	      prevcol = *tmpcol;
	      tmpcol = &((*tmpcol)->Next);
	    }
	  while (col_num <= region->x_max)
	    {
	      if (*tmpcol && ((*tmpcol)->number == col_num))
		add_col = 0;
	      else
		add_col = 1;

	      if (add_col && !cellp)
		{
		  col_num++;
		  continue;
		}
	      if (add_col && cellp)
		{
		  nextcol = *tmpcol;
		  (*tmpcol) = (c_col *)Xc_malloc("addcolmove_r",
						 sizeof(c_col));
		  (*tmpcol)->number = col_num;
		  (*tmpcol)->line = NULL;
		  (*tmpcol)->Next = nextcol;
		  (*tmpcol)->Prev = prevcol;
		  (*tmpcol)->last_pointed = NULL;
		  (*tmpcol)->last_added = NULL;
		  if (nextcol)
		    nextcol->Prev = *tmpcol;
		}
	      tmpline = &((*tmpcol)->line);
	      prevline = NULL;
	      while (*tmpline && ((*tmpline)->number < line_num))
		{
		  prevline = *tmpline;
		  tmpline = &((*tmpline)->Next);
		}

	      if (*tmpline && ((*tmpline)->number == line_num))
		add_line = 0;
	      else
		add_line = 1;

	      if (add_line && cellp)
		{
		  nextline = *tmpline;
		  (*tmpline) = (c_line *)Xc_malloc("addlinemove",
						   sizeof(c_line));
		  (*tmpline)->number = line_num;
		  (*tmpline)->cell.text = NULL;
		  (*tmpline)->cell.formula = NULL;
		  (*tmpline)->cell.status = 0;
		  (*tmpline)->cell.value2 = NULL;
		  (*tmpline)->cell.depend = NULL;
		  (*tmpline)->cell.graph_list = NULL;
		  (*tmpline)->Next = nextline;
		  (*tmpline)->Prev = prevline;
		  if (nextline)
		    nextline->Prev = *tmpline;
		  attach_depend(This, &((*tmpline)->cell.depend),
				col_num, line_num);
		}
	      if (!add_line && !cellp)
		{
		  nextline = *tmpline;
		  if (nextline->cell.formula)
		    {
		      F(This->parser).parse(This->parser,
					    nextline->cell.formula+1,
					    (*tmpcol)->number,
					    line_num,
					    &(nextline->cell),
					    SUB_DEP, GlobEditBar->sel_feuille);
		      Xc_free(nextline->cell.formula);
		      nextline->cell.formula = NULL;
		    }
		  if (nextline->cell.text)
		    {
		      Xc_free(nextline->cell.text);
		      nextline->cell.text = NULL;
		    }
		  if (nextline->cell.depend || nextline->cell.graph_list)
		    {
		      nextline->cell.type = Xq_VOID;
		      if (nextline->cell.value2)
			{
			  Xc_free(nextline->cell.value2);
			  nextline->cell.value2 = NULL;
			}
		      store_depend(nextline->cell.depend, &all_dep,
				   nextline->cell.graph_list, &all_gdep);
		    }
		  else
		    {
		      *tmpline = (*tmpline)->Next;
		      if (*tmpline)
			(*tmpline)->Prev = nextline->Prev;
		      (*tmpcol)->last_pointed = NULL;
		      (*tmpcol)->last_added = NULL;
		      Xc_free(nextline);
		    }
		}
	      if (cellp)
		{
		  if ((*tmpline)->cell.formula)
		    {
		      F(This->parser).parse(This->parser,
					    (*tmpline)->cell.formula+1,
					    (*tmpcol)->number, 
					    (*tmpline)->number, 
					    &((*tmpline)->cell), SUB_DEP, 
					    GlobEditBar->sel_feuille);
		      Xc_free((*tmpline)->cell.formula);
		      (*tmpline)->cell.formula = NULL;
		    }
		  if ((*tmpline)->cell.text)
		    {
		      Xc_free((*tmpline)->cell.text);
		      (*tmpline)->cell.text = NULL;
		    }
		  if ((*tmpline)->cell.depend || (*tmpline)->cell.graph_list)
		    {
		      store_depend((*tmpline)->cell.depend, &all_dep,
				   (*tmpline)->cell.graph_list, &all_gdep);
		    }
		  
		  (*tmpline)->cell.type = cellp->type;
		  if ((*tmpline)->cell.value2)
		    {
		      Xc_free((*tmpline)->cell.value2);
		      (*tmpline)->cell.value2 = NULL;
		    }
		  switch (cellp->type)
		    {
		    case Xq_VOID:
		      break;
		    case Xq_FORMULA:
		    case Xq_FORMERR:
		      if (cellp->formula)
			{
			  translate(cellp->formula, &tmpptr,
				    col_num - region->x_min,
				    0);
			  F(This).set_string(This, 
					     (*tmpcol)->number,
					     line_num,
					     &((*tmpline)->cell),
					     tmpptr,
					     GlobEditBar->sel_feuille);
			  Xc_free(tmpptr);
			}
		      break;
		    case Xq_TEXT:
		      if (cellp->text)
			{
			  (*tmpline)->cell.text = Xc_malloc
			    ("c_text", strlen(cellp->text)+1);
			  strcpy((*tmpline)->cell.text, cellp->text);
			}
		      break;
		    case Xq_VALUE:
		      if (cellp->value2)
			{
			  gduplicate(cellp->value2,
				     &((*tmpline)->cell.value2));
			  (*tmpline)->cell.ret_type = cellp->ret_type;
			}
		    }
		}
	      col_num++;
	      prevcol = *tmpcol;
	      tmpcol = &((*tmpcol)->Next);
	    }
	  line_num++;
	}
    }
  pop_depend(This, all_dep, &all_gdep);
}

/* ----------------------------------------------------------------- ** 
** extend_zone - extend a zone of cells                              ** 
** ----------------------------------------------------------------- */
static void Moteur_extend_zone(This, region, regionend)
c_Moteur	*This;
region_t	*region;
region_t	*regionend;
{
  c_col	*tmpcol;
  c_line **tmpline;
  c_line *tmplinestart;
  c_line *tmplinezone;
  c_line *nextline;
  c_line *prevline;
  char	*tmpptr;
  int	numline;
  int	add_line;
  int	numsel;
  real	real_s;
  real	real_e;
  real	real_d;
  real	real_m;
  boolean lastwasval;
  GEN	gentmp;
  GEN	genlast;
  cell_depend	*all_dep;
  HG_list	*all_gdep;
  int	test;
  int	inc;
  int	testsel;
  region_t	region_style;
  int		div_style;
  int		pos_style;
  int		sortie;

  Xc_TRACE(("extend zone"));
  tmpcol = This->tableCL;
  if ((region->x_min == regionend->x_min) &&
      (region->y_min == regionend->y_min) &&
      (region->x_max == regionend->x_max) &&
      (region->y_max == regionend->y_max))
    {
      Xc_TRACE(("no extend"));
      return;
    }
  all_dep = NULL;
  all_gdep = NULL;
  Xc_TRACE(("region:[%d-%d;%d-%d] regionend:[%d-%d;%d-%d]",
	    region->x_min,
	    region->y_min,
	    region->x_max,
	    region->y_max,
	    regionend->x_min,
	    regionend->y_min,
	    regionend->x_max,
	    regionend->y_max));

  if ((region->x_min == regionend->x_min) &&
      (region->x_max == regionend->x_max))
    {
      Xc_TRACE(("extend vertically"));

      /* extend style vertically */
      region_style.x_min = region->x_min;
      region_style.y_min = region->y_min;
      region_style.x_max = region->x_max;
      region_style.y_max = region->y_max;

      div_style = region->y_max - region->y_min + 1;
      if (region->y_min > regionend->y_min)
	{
	  div_style = -div_style;
	  pos_style = region->y_min;
	}
      else
	pos_style = region->y_max + 1;
	
      sortie = 0;
      while (!sortie)
	{
	  if (div_style > 0)
	    {
	      if (pos_style > regionend->y_max)
		{
		  sortie = 1;
		  continue;
		}
	      if (regionend->y_max - pos_style + 1< div_style)
		region_style.y_max = 
		  region->y_min+regionend->y_max-pos_style;
	      F(This).copy_style(This, &region_style, 
				 region->x_min, pos_style);
	      pos_style += div_style;
	    }
	  else
	    {
	      if (pos_style < regionend->y_min)
		{
		  sortie = 1;
		  continue;
		}

	      if (pos_style - regionend->y_min > -div_style)
		pos_style += div_style;
	      else
		{
		  region_style.y_min = 
		    region->y_max - (pos_style - regionend->y_min) + 1;
		  pos_style = regionend->y_min;
		  sortie = 1;
		}
	      F(This).copy_style(This, &region_style, 
				 region->x_min, pos_style);
	    }
	}
 
      /* find selected zone start_X */
      while (tmpcol && (tmpcol->number < region->x_min))
	{
	  tmpcol = tmpcol->Next;
	}
 
      /* treat each column independent */
      while (tmpcol && (tmpcol->number <= region->x_max))
	{
	  tmpline = &(tmpcol->line);
	  prevline = NULL;
	  lastwasval = FALSE;

	  /* find selected zone start_Y */
	  if (regionend->y_min < region->y_min)
	    {
	      numline = region->y_min-1;
	      test = regionend->y_min-1;
	      numsel = region->y_max;
	      testsel = region->y_min-1;
	      inc = -1;
	    }
	  else
	    {
	      numline = region->y_max+1;
	      test = regionend->y_max+1;
	      numsel = region->y_min;
	      testsel = region->y_max+1;
	      inc = 1;
	    }
	  while (*tmpline && ((*tmpline)->number < numsel))
	    {
	      prevline = *tmpline;
	      tmpline = &((*tmpline)->Next);
	    }
	  tmplinestart = *tmpline;
	  tmplinezone = tmplinestart;

	  while (numline != test)
	    {
	      Xc_TRACE(("INFO: %d %d", numline, numsel));
	      if (testsel == region->y_max+1)
		{
		  if (numsel == testsel)
		    {
		      numsel = region->y_min;
		      tmplinezone = tmplinestart;
		    }
		  while (tmplinezone && (tmplinezone->number < numsel))
		    tmplinezone = tmplinezone->Next;
		}
	      else
		{
		  if (numsel == testsel)
		    {
		      numsel = region->y_max;
		      tmplinezone = tmplinestart;
		    }
		  while (tmplinezone && (tmplinezone->number > numsel))
		    tmplinezone = tmplinezone->Prev;
		}

	      if (!tmplinezone ||
		  (tmplinezone->number != numsel) ||
		  (tmplinezone->cell.type == Xq_VOID))
		{
		  numsel += inc;
		  numline += inc;
		  continue;
		}
	      if ((tmplinezone->cell.type == Xq_VALUE) && 
		  (region->y_min != region->y_max) &&
		  !lastwasval)
		{
		  int	i;
		  c_line *tmplinetmp;
		  c_line *tmplinelast;

		  real_d = 0;
		  real_m = 0;
		  tmplinetmp = tmplinezone;
		  real_s = (real)gtodouble(tmplinetmp->cell.value2);
		  while (tmplinetmp)
		    {
		      tmplinelast = tmplinetmp;

		      if (testsel == region->y_max+1)
			{
			  i = tmplinetmp->number+1;
			  if (i > region->y_max)
			    break;
			  tmplinetmp = tmplinetmp->Next;
			}
		      else
			{
			  i = tmplinetmp->number-1;
			  if (i < region->y_min)
			    break;
			  tmplinetmp = tmplinetmp->Prev;
			}
		      if (!tmplinetmp)
			break;
		      if (tmplinetmp->number != i)
			break;
		      if (tmplinetmp->cell.type != Xq_VALUE)
			break;
		    }
		  if (tmplinelast == tmplinezone)
		    {
		      real_d = 1;
		      genlast = tmplinelast->cell.value2;
		      gentmp = str2g("1");
		    }
		  else
		    {
		      int	delta;
		      real	test;
		      char	str[50];
		      
		      if (tmplinelast->number > tmplinezone->number)
			delta = (tmplinelast->number - tmplinezone->number);
		      else
			delta = (tmplinezone->number - tmplinelast->number);
		      real_e = (real)gtodouble(tmplinelast->cell.value2);
		      genlast = tmplinelast->cell.value2;

		      if (delta == 1)
			{
			  real_d = real_e - real_s;
			  sprintf (str, "%g", real_d);
			  gentmp = str2g(str);
			}
		      else
			{
			  real_m = pow (((double)real_e / real_s),
					1/(double)delta);
			  if (real_m == 1)
			    {
			      real_m = 0;
			      real_d = (real_e - real_s) / delta;
			      sprintf (str, "%g", real_d);
			      gentmp = str2g(str);
			    }
			  else
			    {
			      if (testsel == region->y_max+1)
				tmplinelast = tmplinezone->Next;
			      else
				tmplinelast = tmplinezone->Prev;
			      test = (real)gtodouble(tmplinelast->cell.value2);
			      Xc_TRACE(("mega comp %g %g %g %g", test,
					real_s * real_m, real_s, real_m));
			      if ((float)test != (float)(real_s * real_m))
				{
				  real_m = 0;
				  real_d = (real_e - real_s) / delta;
				  sprintf (str, "%g", real_d);
				  gentmp = str2g(str);
				}
			      else
				{
				  real_d = 0;
				  sprintf (str, "%g", real_m);
				  gentmp = str2g(str);
				}
			    }
			}
		    }
		}
	      else
		{
		  if (!lastwasval)
		    {
		      real_d = 0;
		      real_m = 0;
		    }
		}
	      /* find extend zone pos */
	      tmpline = &(tmpcol->line);
	      while (*tmpline && ((*tmpline)->number < numline))
		{
		  prevline = *tmpline;
		  tmpline = &((*tmpline)->Next);
		}

	      if (*tmpline)
		{
		  if (((*tmpline)->number) == numline)
		    {
		      /* line is there clean cell */
		      if ((*tmpline)->cell.formula)
			{
			  F(This->parser).parse(This->parser,
						(*tmpline)->cell.formula+1,
						(*tmpline)->number,
						tmpcol->number, 
						&((*tmpline)->cell), SUB_DEP, 
						GlobEditBar->sel_feuille);
			  Xc_free((*tmpline)->cell.formula);
			  (*tmpline)->cell.formula = NULL;
			}
		      if ((*tmpline)->cell.text)
			{
			  Xc_free((*tmpline)->cell.text);
			  (*tmpline)->cell.text = NULL;
			}
		      add_line = 0;
		    }
		  else
		    add_line = 1;
		}
	      else
		add_line = 1;

	      if (add_line)
		{
		  nextline = *tmpline;
		  (*tmpline) = (c_line *)Xc_malloc("addlinemove",
						   sizeof(c_line));
		  (*tmpline)->number = numline;
		  (*tmpline)->cell.text = NULL;
		  (*tmpline)->cell.formula = NULL;
		  (*tmpline)->cell.status = 0;
		  (*tmpline)->cell.value2 = NULL;
		  (*tmpline)->cell.depend = NULL;
		  (*tmpline)->cell.graph_list = NULL;
		  (*tmpline)->Next = nextline;
		  (*tmpline)->Prev = prevline;
		  if (nextline)
		    nextline->Prev = *tmpline;
		  attach_depend(This, &((*tmpline)->cell.depend),
				tmpcol->number, numline);
		}

	      (*tmpline)->cell.type = tmplinezone->cell.type;
	      if (tmplinezone->cell.text)
		{
		  (*tmpline)->cell.text = 
		    Xc_malloc("c_text_ex", 
			      strlen(tmplinezone->cell.text)+1);
		  strcpy((*tmpline)->cell.text, tmplinezone->cell.text);
		}
	      lastwasval = FALSE;
	      switch (tmplinezone->cell.type)
		{
		case Xq_FORMULA:
		case Xq_FORMERR:
		  translate(tmplinezone->cell.formula, &tmpptr,
			    0,
			    numline - tmplinezone->number);
		  This->force = 1;
		  F(This).set_string(This, 
				     tmpcol->number,
				     (*tmpline)->number,
				     &((*tmpline)->cell),
				     tmpptr,
				     GlobEditBar->sel_feuille);
		  Xc_free(tmpptr);
		  break;
		case Xq_VOID:
		case Xq_TEXT:
		  break;
		case Xq_VALUE:
		  lastwasval = TRUE;
		  if (tmplinezone->cell.value2)
		    {
		      if (real_d)
			{
			  gduplicate(gadd(gentmp, genlast),
				     &((*tmpline)->cell.value2));
			  genlast = (*tmpline)->cell.value2;
			}
		      else
			if (real_m)
			  {
			    gduplicate(gmul(gentmp, genlast),
				       &((*tmpline)->cell.value2));
			    genlast = (*tmpline)->cell.value2;
			  }
			else
			  gduplicate(tmplinezone->cell.value2,
				     &((*tmpline)->cell.value2));
		      (*tmpline)->cell.ret_type = tmplinezone->cell.ret_type;
		    }
		}
	      store_depend((*tmpline)->cell.depend, &all_dep,
			   (*tmpline)->cell.graph_list, &all_gdep);
	      numsel += inc;
	      numline += inc;
	    }
	  tmpcol = tmpcol->Next;
	}
    }
  else
    {
      int	line_num;
      int	numcol;
      c_col	*tmpcolstart;
      c_col	*tmpcolzone;
      c_col	**tmpcol;
      c_col	*tmpcoltmp;
      c_col	*prevcol;
      int	add_col;
      c_col	*nextcol;

      Xc_TRACE(("extend horizontally"));

      /* extend style horizontally */
      region_style.x_min = region->x_min;
      region_style.y_min = region->y_min;
      region_style.x_max = region->x_max;
      region_style.y_max = region->y_max;


      div_style = region->x_max - region->x_min + 1;
      if (region->x_min > regionend->x_min)
	{
	  div_style = -div_style;
	  pos_style = region->x_min;
	}
      else
	pos_style = region->x_max + 1;
	
      sortie = 0;
      while (!sortie)
	{
	  if (div_style > 0)
	    {
	      if (pos_style > regionend->x_max)
		{
		  sortie = 1;
		  continue;
		}
	      if (regionend->x_max - pos_style + 1< div_style)
		region_style.x_max = 
		  region->x_min+regionend->x_max-pos_style;
	      F(This).copy_style(This, &region_style, 
				 pos_style, region->y_min);
	      pos_style += div_style;
	    }
	  else
	    {
	      if (pos_style < regionend->x_min)
		{
		  sortie = 1;
		  continue;
		}

	      if (pos_style - regionend->x_min > -div_style)
		pos_style += div_style;
	      else
		{
		  region_style.x_min = 
		    region->x_max - (pos_style - regionend->x_min) + 1;
		  pos_style = regionend->x_min;
		  sortie = 1;
		}
	      F(This).copy_style(This, &region_style, 
				 pos_style, region->y_min);
	    }
	}

      line_num = region->y_min;
      /* treat each line independent */
      while (line_num <= region->y_max)
	{
	  lastwasval = FALSE;
	  tmpcol = &(This->tableCL);

	  /* find selected zone start_X */
	  if (regionend->x_min < region->x_min)
	    {
	      numcol = region->x_min-1;
	      test = regionend->x_min-1;
	      numsel = region->x_max;
	      testsel = region->x_min-1;
	      inc = -1;
	    }
	  else
	    {
	      numcol = region->x_max+1;
	      test = regionend->x_max+1;
	      numsel = region->x_min;
	      testsel = region->x_max+1;
	      inc = 1;
	    }
	  while (*tmpcol && ((*tmpcol)->number < numsel))
	    {
	      tmpcol = &(*tmpcol)->Next;
	    }
	  tmpcolstart = *tmpcol;
	  tmpcolzone = tmpcolstart;

	  /* extend to zone end_X */
	  while (numcol != test)
	    {
	      Xc_TRACE(("INFO: %d %d", numcol, numsel));
	      if (testsel == region->x_max+1)
		{
		  if (numsel == testsel)
		    {
		      numsel = region->x_min;
		      tmpcolzone = tmpcolstart;
		    }
		  while (tmpcolzone && (tmpcolzone->number < numsel))
		    tmpcolzone = tmpcolzone->Next;
		}
	      else
		{
		  if (numsel == testsel)
		    {
		      numsel = region->x_max;
		      tmpcolzone = tmpcolstart;
		    }
		  while (tmpcolzone && (tmpcolzone->number > numsel))
		    tmpcolzone = tmpcolzone->Prev;
		}

	      if (!tmpcolzone ||
		  (tmpcolzone->number != numsel))
		{
		  numsel += inc;
		  numcol += inc;
		  continue;
		}

	      /* now find line in column */
	      tmplinezone = tmpcolzone->line;
	      while (tmplinezone && tmplinezone->number < line_num)
		tmplinezone = tmplinezone->Next;

	      if (!tmplinezone ||
		  (tmplinezone->number != line_num) ||
		  (tmplinezone->cell.type == Xq_VOID))
		{
		  numsel += inc;
		  numcol += inc;
		  continue;
		}
	      if ((tmplinezone->cell.type == Xq_VALUE) && 
		  (region->x_min != region->x_max) &&
		  !lastwasval)
		{
		  int	i;
		  c_line *tmplinetmp;
		  c_line *tmplinelast;

		  real_d = 0;
		  real_m = 0;
		  tmpcoltmp = tmpcolzone;
		  tmplinetmp = tmplinezone;
		  real_s = (real)gtodouble(tmplinetmp->cell.value2);
		  while (tmpcoltmp)
		    {
		      tmplinelast = tmplinetmp;

		      if (testsel == region->x_max+1)
			{
			  i = tmpcoltmp->number+1;
			  if (i > region->x_max)
			    break;
			  tmpcoltmp = tmpcoltmp->Next;
			}
		      else
			{
			  i = tmpcoltmp->number-1;
			  if (i < region->x_min)
			    break;
			  tmpcoltmp = tmpcoltmp->Prev;
			}
		      if (!tmpcoltmp)
			break;
		      if (tmpcoltmp->number != i)
			break;
		      tmplinetmp = tmpcoltmp->line;
		      while (tmplinetmp->number < line_num)
			tmplinetmp = tmplinetmp->Next;
		      if (tmplinetmp->number != line_num)
			break;
		      if (tmplinetmp->cell.type != Xq_VALUE)
			break;
		    }
		  if (tmpcoltmp == tmpcolzone)
		    {
		      real_d = 1;
		      genlast = tmplinelast->cell.value2;
		      gentmp = str2g("1");
		    }
		  else
		    {
		      int	delta;
		      real	test;
		      char	str[50];

		      if (tmpcoltmp->number > tmpcolzone->number)
			delta = (tmpcoltmp->number - tmpcolzone->number);
		      else
			delta = (tmpcolzone->number - tmpcoltmp->number);
		      real_e = (real)gtodouble(tmplinelast->cell.value2);
		      genlast = tmplinelast->cell.value2;

		      if (delta == 1)
			{
			  real_d = real_e - real_s;
			  sprintf (str, "%g", real_d);
			  gentmp = str2g(str);
			}
		      else
			{
			  real_m = pow (((double)real_e / real_s),
					1/(double)delta);
			  if (real_m == 1)
			    {
			      real_m = 0;
			      real_d = (real_e - real_s) / delta;
			      sprintf (str, "%g", real_d);
			      gentmp = str2g(str);
			    }
			  else
			    {
			      if (testsel == region->x_max+1)
				tmpcoltmp = tmpcolzone->Next;
			      else
				tmpcoltmp = tmpcolzone->Prev;
			      
			      tmplinelast = tmpcoltmp->line;
			      while (tmplinelast->number < line_num)
				tmplinelast = tmplinelast->Next;

			      test = (real)gtodouble(tmplinelast->cell.value2);
			      Xc_TRACE(("mega comp %g %g %g %g", test,
					real_s * real_m, real_s, real_m));
			      if ((float)test != (float)(real_s * real_m))
				{
				  real_m = 0;
				  real_d = (real_e - real_s) / delta;
				  sprintf (str, "%g", real_d);
				  gentmp = str2g(str);
				}
			      else
				{
				  real_d = 0;
				  sprintf (str, "%g", real_m);
				  gentmp = str2g(str);
				}
			    }
			}
		    }
		}
	      else
		{
		  if (!lastwasval)
		    {
		      real_d = 0;
		      real_m = 0;
		    }
		}

	      /* find extend zone pos */
	      tmpcol = &(This->tableCL);
	      while (*tmpcol && ((*tmpcol)->number < numcol))
		{
		  prevcol = *tmpcol;
		  tmpcol = &((*tmpcol)->Next);
		}

	      if (*tmpcol)
		{
		  if (((*tmpcol)->number) == numcol)
		    {

		      /* search line */
		      tmpline = &((*tmpcol)->line);
		      while (*tmpline && ((*tmpline)->number < line_num))
			{
			  prevline = *tmpline;
			  tmpline = &((*tmpline)->Next);
			}

		      if (*tmpline)
			{
			  if (((*tmpline)->number) == line_num)
			    {
			      /* line is there clean cell*/
			      
			      if ((*tmpline)->cell.formula)
				{
				  F(This->parser).
				    parse(This->parser,
					  (*tmpline)->cell.formula+1,
					  (*tmpline)->number,
					  (*tmpcol)->number, 
					  &((*tmpline)->cell), SUB_DEP, 
					  GlobEditBar->sel_feuille);
				  Xc_free((*tmpline)->cell.formula);
				  (*tmpline)->cell.formula = NULL;
				}
			      if ((*tmpline)->cell.text)
				{
				  Xc_free((*tmpline)->cell.text);
				  (*tmpline)->cell.text = NULL;
				}
			      add_line = 0;
			    }
			  else
			    add_line = 1;
			}
		      else
			add_line = 1;

		      if (add_line)
			{
			  nextline = *tmpline;
			  (*tmpline) = (c_line *)Xc_malloc("addlinemove",
							   sizeof(c_line));
			  (*tmpline)->number = line_num;
			  (*tmpline)->cell.text = NULL;
			  (*tmpline)->cell.formula = NULL;
			  (*tmpline)->cell.status = 0;
			  (*tmpline)->cell.value2 = NULL;
			  (*tmpline)->cell.depend = NULL;
			  (*tmpline)->cell.graph_list = NULL;
			  (*tmpline)->Next = nextline;
			  (*tmpline)->Prev = prevline;
			  if (nextline)
			    nextline->Prev = *tmpline;
			  attach_depend(This, &((*tmpline)->cell.depend),
					(*tmpcol)->number, line_num);
			}
		      add_col = 0;
		    }
		  else
		    add_col = 1;
		}
	      else
		add_col = 1;

	      if (add_col)
		{
		  nextcol = *tmpcol;
		  (*tmpcol) = (c_col *)Xc_malloc("addcolext",
						 sizeof(c_col));
		  (*tmpcol)->number = numcol;
		  (*tmpcol)->Next = nextcol;
		  (*tmpcol)->Prev = prevcol;
		  (*tmpcol)->last_pointed = NULL;
		  (*tmpcol)->last_added = NULL;
		  if (nextcol)
		    nextcol->Prev = *tmpcol;

		  tmpline = &((*tmpcol)->line);
		  (*tmpline) = (c_line *)Xc_malloc("addlinemove",
						   sizeof(c_line));
		  (*tmpline)->number = line_num;
		  (*tmpline)->cell.text = NULL;
		  (*tmpline)->cell.formula = NULL;
		  (*tmpline)->cell.status = 0;
		  (*tmpline)->cell.value2 = NULL;
		  (*tmpline)->cell.depend = NULL;
		  (*tmpline)->cell.graph_list = NULL;
		  (*tmpline)->Next = NULL;
		  (*tmpline)->Prev = NULL;	
		  attach_depend(This, &((*tmpline)->cell.depend),
				(*tmpcol)->number, line_num);
		}

	      (*tmpline)->cell.type = tmplinezone->cell.type;
	      if (tmplinezone->cell.text)
		{
		  (*tmpline)->cell.text = 
		    Xc_malloc("c_text_ex", 
			      strlen(tmplinezone->cell.text)+1);
		  strcpy((*tmpline)->cell.text, tmplinezone->cell.text);
		}
	      lastwasval = FALSE;
	      switch (tmplinezone->cell.type)
		{
		case Xq_FORMULA:
		case Xq_FORMERR:
		  translate(tmplinezone->cell.formula, &tmpptr,
			    numcol - numsel,
			    0);
		  This->force = 1;
		  F(This).set_string(This, 
				     (*tmpcol)->number,
				     (*tmpline)->number,
				     &((*tmpline)->cell),
				     tmpptr,
				     GlobEditBar->sel_feuille);
		  Xc_free(tmpptr);
		  break;
		case Xq_VOID:
		case Xq_TEXT:
		  break;
		case Xq_VALUE:
		  lastwasval = TRUE;
		  if (tmplinezone->cell.value2)
		    {
		      if (real_d)
			{
			  gduplicate(gadd(gentmp, genlast),
				     &((*tmpline)->cell.value2));
			  genlast = (*tmpline)->cell.value2;
			}
		      else
			if (real_m)
			  {
			    gduplicate(gmul(gentmp, genlast),
				       &((*tmpline)->cell.value2));
			    genlast = (*tmpline)->cell.value2;
			  }
			else
			  gduplicate(tmplinezone->cell.value2,
				     &((*tmpline)->cell.value2));
		      (*tmpline)->cell.ret_type = tmplinezone->cell.ret_type;
		    }
		}
	      store_depend((*tmpline)->cell.depend, &all_dep,
			   (*tmpline)->cell.graph_list, &all_gdep);
	      numsel += inc;
	      numcol += inc;
	    }
	  line_num++;
	}
    }
  pop_depend(This, all_dep, &all_gdep);
}

/* ----------------------------------------------------------------- ** 
** Moteur_del_cell2 - delete cell                                    ** 
** ----------------------------------------------------------------- */
static void Moteur_del_cell2(This, line, col, dep, gdep)
c_Moteur	*This;
int		line;
int		col;
cell_depend	**dep;
HG_list		**gdep;
{
  c_col		*tmpcol;
  c_line	**tmplines;
  c_line	*tmpline;
  c_line	*tmpline2;
  c_line	*oldline;
  
  tmpcol = This->tableCL;
  while (tmpcol && (tmpcol->number < col))
    {
      tmpcol = tmpcol->Next;
    }
  if (!tmpcol)
    return;

  if (tmpcol->number == col)
    {
      tmplines = tmpcol->last_added;
      if (tmplines)
	{
	  if (*tmplines)
	    {
	      if ((*tmplines)->number > line)
		tmplines = &(tmpcol->line);
	    }
	  else
	    tmplines = &(tmpcol->line);
	}
      else
	tmplines = &(tmpcol->line);

      oldline = NULL;
      while (*tmplines && ((*tmplines)->number < line))
	{
	  oldline = *tmplines;
	  tmplines = &(*tmplines)->Next;
	}
      tmpline = *tmplines;

      if (tmpline && (tmpline->number == line))
	{
	  if ((tmpline->Prev) && (tmpline->Prev->Prev))
	    tmpcol->last_added = &(tmpline->Prev->Prev->Next);
	  else
	    tmpcol->last_added = NULL;
	  tmpcol->last_pointed = NULL;
	  tmpline2 = tmpline->Next;
	  if (tmpline->cell.formula)
	    {
	      F(This->parser).parse(This->parser, tmpline->cell.formula+1,
				    tmpcol->number, 
				    tmpline->number,
				    &(tmpline->cell), SUB_DEP, 
				    GlobEditBar->sel_feuille);
	      Xc_free(tmpline->cell.formula);
	      tmpline->cell.formula = NULL;
	    }
	  if (tmpline->cell.text)
	    {
	      Xc_free(tmpline->cell.text);
	      tmpline->cell.text = NULL;
	    }
	  if (tmpline->cell.value2)
	    {
	      Xc_free(tmpline->cell.value2);
	      tmpline->cell.value2 = NULL;
	    }

	  if (tmpline->cell.depend || tmpline->cell.graph_list)
	    {
	      tmpline->cell.type = Xq_VOID;
	      store_depend(tmpline->cell.depend, dep,
			   tmpline->cell.graph_list, gdep);
	      return;
	    }
	  Xc_free(tmpline);
	  tmpline = tmpline2;
	  if (tmpline)
	    tmpline->Prev = oldline;
	  *tmplines = tmpline;
	}
    }
}

/* ----------------------------------------------------------------- ** 
** Moteur_delkeep_cell - delete cell and keep cell struct            ** 
** ----------------------------------------------------------------- */
static void Moteur_delkeep_cell(This, col, line, cellp, dep, gdep)
c_Moteur	*This;
int		col;
int		line;
cell		*cellp;
cell_depend	**dep;
HG_list		**gdep;
{
  c_col		*tmpcol;
  c_line	**tmplines;
  c_line	*tmpline;
  c_line	*tmpline2;
  c_line	*oldline;
  cell_depend	*tmp_depend;
  HG_list	*tmp_graph_list;
  
  tmpcol = This->tableCL;
  while (tmpcol && (tmpcol->number < col))
    {
      tmpcol = tmpcol->Next;
    }
  if (!tmpcol)
    return;

  if (tmpcol->number == col)
    {
      tmplines = tmpcol->last_added;
      if (tmplines)
	{
	  if (*tmplines)
	    {
	      if (abs(line - (*tmplines)->number) >
		  abs(line - tmpcol->line->number))
		tmplines = &(tmpcol->line);
	    }
	  else
	    tmplines = &(tmpcol->line);
	}
      else
	tmplines = &(tmpcol->line);
      
      if (*tmplines)
	{
	  oldline = (*tmplines)->Prev;
	  
	  if ((*tmplines)->number > line)
	    {
	      while ((*tmplines)->Prev && ((*tmplines)->number > line))
		{
		  if ((*tmplines)->Prev->Prev)
		    tmplines = &((*tmplines)->Prev->Prev->Next);
		  else
		    tmplines = &(tmpcol->line);
		}
	      oldline = (*tmplines)->Prev;
	    }
	  else
	    {
	      while ((*tmplines) && ((*tmplines)->number < line))
		{
		  oldline = *tmplines;
		  tmplines = &((*tmplines)->Next);
		}
	    }

	  tmpline = *tmplines;
	  if (tmpline && (tmpline->number == line))
	    {
	      if (tmpline->Prev && tmpline->Prev->Prev)
		tmpcol->last_added = &(tmpline->Prev->Prev->Next);
	      else
		tmpcol->last_added = NULL;
	      tmpcol->last_pointed = NULL;
	      tmpline2 = tmpline->Next;
	      
	      tmp_depend = cellp->depend;
	      tmp_graph_list = cellp->graph_list;
	      memcpy(cellp, &(tmpline->cell), sizeof(cell));
	      cellp->depend = tmp_depend;
	      cellp->graph_list = tmp_graph_list;
	      store_depend(cellp->depend, dep,
			   cellp->graph_list, gdep);
	      
	      if (tmpline->cell.formula)
		{
		  F(This->parser).parse(This->parser,
					tmpline->cell.formula+1,
					tmpline->number,
					tmpcol->number, 
					&(tmpline->cell), SUB_DEP, 
					GlobEditBar->sel_feuille);
		}
	      if (tmpline->cell.depend || tmpline->cell.graph_list)
		{
		  tmpline->cell.type = Xq_VOID;
		  tmpline->cell.text = NULL;
		  tmpline->cell.value2 = NULL;
		  tmpline->cell.formula = NULL;
		  store_depend(tmpline->cell.depend, dep,
			       tmpline->cell.graph_list, gdep);
		  return;
		}
	      Xc_free(tmpline);
	      tmpline = tmpline2;
	      if (tmpline)
		{
		  tmpline->Prev = oldline;
		}
	      *tmplines = tmpline;
	    }
	}
    }
}

/* ----------------------------------------------------------------- ** 
** move_cell - move a cell across the sheet                          ** 
** ----------------------------------------------------------------- */
static void Moteur_move_cell(This, x_src, y_src, x_dst, y_dst, dep, gdep)
c_Moteur	*This;
int		x_src;
int		y_src;
int		x_dst;
int		y_dst;
cell_depend	**dep;
HG_list		**gdep;
{
  cell		*cellp;

  cellp = Moteur_add_cell(This, y_dst, x_dst);
  Moteur_delkeep_cell(This, x_src, y_src, cellp, dep, gdep);
  if ((cellp->type == Xq_FORMULA) ||
      (cellp->type == Xq_FORMERR))
    {
      char	*tmpptr;
      
      translate(cellp->formula, &tmpptr,
		x_dst-x_src,
		y_dst-y_src);
      This->force = 1;
      Moteur_deffered_set_string(This,
				 x_dst,
				 y_dst,
				 cellp,
				 tmpptr,
				 GlobEditBar->sel_feuille, 
				 dep,
				 gdep);

      Xc_free(tmpptr);
    }
}

/* ----------------------------------------------------------------- ** 
** insere_zone_col - insert a column zone                            ** 
** ----------------------------------------------------------------- */
static void Moteur_insere_zone_col(This, region)
c_Moteur	*This;
region_t	*region;
{
  int		num;
  region_t	region_style;
  int		col_style;
  c_col		*tmpcol;
  c_col		*lastcol;
  c_line	*tmpline;
  c_line	*lastline;
  cell_depend	*all_dep;
  HG_list	*all_gdep;

  Xc_TRACE(("insert zone column"));
  all_dep = NULL;
  all_gdep = NULL;
  num = (region->x_max - region->x_min) + 1;

  /* bouger big style */
  region_style.x_min = region->x_min;
  region_style.y_min = region->y_min;
  region_style.x_max = COLUMN_MAX;
  region_style.y_max = region->y_max;
  F(This).move_style(This, &region_style, region->x_max+1, region->y_min);
  

  /* recopie style de la derniere colonne */
  region_style.x_min = region->x_max+1;
  region_style.y_min = region->y_min;
  region_style.x_max = region->x_max+1;
  region_style.y_max = region->y_max;
  col_style = region->x_max;

  while (col_style >= region->x_min)
    {
      F(This).copy_style(This, &region_style, col_style, region->y_min);
      col_style--;
    }

  /* deplace les tailles des lignes si necessaire */
  if ((region->y_min == 0) &&
      (region->y_max == ROW_MAX))
    {
      F(This).insert_size_zone(This, Xc_COLUMN,
			       region->x_min, region->x_max);
    }

  /* find last col */
  tmpcol = This->tableCL;
  lastcol = NULL;
  while (tmpcol)
    {
      lastcol = tmpcol;
      tmpcol = tmpcol->Next;
    }

  if (!lastcol)
    {
      return;
    }

  lastline = NULL;
  while (lastcol && (lastcol->number >= region->x_min))
    {
      tmpline = lastcol->line;
      while (tmpline && (tmpline->number < region->y_min))
	tmpline = tmpline->Next;
      while (tmpline && (tmpline->number <= region->y_max))
	{
	  lastline = tmpline->Next;
	  Moteur_move_cell(This, lastcol->number, tmpline->number, 
			   (lastcol->number)+num, tmpline->number, 
			   &all_dep, &all_gdep);
	  tmpline = lastline;
	}
      lastcol = lastcol->Prev;
    }
  pop_depend(This, all_dep, &all_gdep);
}

/* ----------------------------------------------------------------- ** 
** insere_zone_row - insert a row zone                               ** 
** ----------------------------------------------------------------- */
static void Moteur_insere_zone_row(This, region)
c_Moteur	*This;
region_t	*region;
{
  int		num;
  region_t	region_style;
  int		line_style;
  c_col		*lastcol;
  c_line	*tmpline;
  c_line	*lastline;
  cell_depend	*all_dep;
  HG_list	*all_gdep;

  Xc_TRACE(("insert zone row"));
  all_dep = NULL;
  all_gdep = NULL;
  num = (region->y_max - region->y_min) + 1;
  
  /* bouger big style */
  region_style.x_min = region->x_min;
  region_style.y_min = region->y_min;
  region_style.x_max = region->x_max;
  region_style.y_max = ROW_MAX;
  F(This).move_style(This, &region_style, region->x_min, region->y_max+1);
  
  /* recopie style de la derniere ligne */
  region_style.x_min = region->x_min;
  region_style.y_min = region->y_max+1;
  region_style.x_max = region->x_max;
  region_style.y_max = region->y_max+1;
  line_style = region->y_max;

  while (line_style >= region->y_min)
    {
      F(This).copy_style(This, &region_style, region->x_min, line_style);
      line_style--;
    }

  /* deplace les tailles des lignes si necessaire */
  if ((region->x_min == 0) &&
      (region->x_max == COLUMN_MAX))
    {
      F(This).insert_size_zone(This, Xc_ROW,
			       region->y_min, region->y_max);
    }

  /* find first col */
  lastcol = This->tableCL;
  while (lastcol && (lastcol->number < region->x_min))
    {
      lastcol = lastcol->Next;
    }

  if (!lastcol)
    {
      return;
    }

  while (lastcol && (lastcol->number <= region->x_max))
    {
      lastline = lastcol->line;
      while (lastline)
	{
	  tmpline = lastline;
	  lastline = lastline->Next;
	}
      while (tmpline && (tmpline->number >= region->y_min))
	{
	  lastline = tmpline->Prev;
	  Moteur_move_cell(This, lastcol->number, tmpline->number, 
			   lastcol->number, (tmpline->number)+num, 
			   &all_dep, &all_gdep);
	  tmpline = lastline;
	}
      lastcol = lastcol->Next;
    }
  pop_depend(This, all_dep, &all_gdep);
}

/* ----------------------------------------------------------------- ** 
** delete_zone_col - delete a column zone                            ** 
** ----------------------------------------------------------------- */
static void Moteur_delete_zone_col(This, region)
c_Moteur	*This;
region_t	*region;
{
  int		num;
  region_t	region_style;
  c_col		*lastcol;
  c_line	*tmpline;
  c_line	*lastline;
  cell_depend	*all_dep;
  HG_list	*all_gdep;

  Xc_TRACE(("delete zone column"));
  all_dep = NULL;
  all_gdep = NULL;
  num = (region->x_max - region->x_min) + 1;

  /* bouger big style */
  region_style.x_min = region->x_max+1;
  region_style.y_min = region->y_min;
  region_style.x_max = COLUMN_MAX;
  region_style.y_max = region->y_max;
  F(This).move_style(This, &region_style, region->x_min, region->y_min);

  /* deplace les tailles des lignes si necessaire */
  if ((region->y_min == 0) &&
      (region->y_max == ROW_MAX))
    {
      F(This).delete_size_zone(This, Xc_COLUMN,
			       region->x_min, region->x_max);
    }

  /* find first col */
  lastcol = This->tableCL;
  while (lastcol && (lastcol->number < region->x_min))
    {
      lastcol = lastcol->Next;
    }

  if (!lastcol)
    {
      return;
    }

  /* delete all column in zone */
  while (lastcol && (lastcol->number <= region->x_max))
    {
      tmpline = lastcol->line;
      while (tmpline && (tmpline->number < region->y_min))
	tmpline = tmpline->Next;
      while (tmpline && (tmpline->number <= region->y_max))
	{
	  lastline = tmpline->Next;
	  Moteur_del_cell2(This, tmpline->number, lastcol->number, 
			   &all_dep, &all_gdep);
	  tmpline = lastline;
	}
      lastcol = lastcol->Next;
    }

  lastline = NULL;
  while (lastcol && (lastcol->number <= COLUMN_MAX))
    {
      tmpline = lastcol->line;
      while (tmpline && (tmpline->number < region->y_min))
	tmpline = tmpline->Next;
      while (tmpline && (tmpline->number <= region->y_max))
	{
	  lastline = tmpline->Next;
	  Moteur_move_cell(This, lastcol->number, tmpline->number, 
			   (lastcol->number)-num, tmpline->number, 
			   &all_dep, &all_gdep);
	  tmpline = lastline;
	}
      lastcol = lastcol->Next;
    }
  pop_depend(This, all_dep, &all_gdep);
}

/* ----------------------------------------------------------------- ** 
** delete_zone_row - delete a row zone                               ** 
** ----------------------------------------------------------------- */
static void Moteur_delete_zone_row(This, region)
c_Moteur	*This;
region_t	*region;
{
  int		num;
  region_t	region_style;
  c_col		*lastcol;
  c_line	*tmpline;
  c_line	*lastline;
  cell_depend	*all_dep;
  HG_list	*all_gdep;

  Xc_TRACE(("delete zone column"));
  all_dep = NULL;
  all_gdep = NULL;
  num = (region->y_max - region->y_min) + 1;

  /* bouger big style */
  region_style.x_min = region->x_min;
  region_style.y_min = region->y_max+1;
  region_style.x_max = region->x_max;
  region_style.y_max = ROW_MAX;
  F(This).move_style(This, &region_style, region->x_min, region->y_min);

  /* deplace les tailles des lignes si necessaire */
  if ((region->x_min == 0) &&
      (region->x_max == COLUMN_MAX))
    {
      F(This).delete_size_zone(This, Xc_ROW,
			       region->y_min, region->y_max);
    }

  /* find first col */
  lastcol = This->tableCL;
  while (lastcol && (lastcol->number < region->x_min))
    {
      lastcol = lastcol->Next;
    }

  if (!lastcol)
    {
      return;
    }

  while (lastcol && (lastcol->number <= region->x_max))
    {
      /* delete all line in zone */
      tmpline = lastcol->line;
      while (tmpline && (tmpline->number < region->y_min))
	tmpline = tmpline->Next;
      while (tmpline && (tmpline->number <= region->y_max))
	{
	  lastline = tmpline->Next;
	  Moteur_del_cell2(This, tmpline->number, lastcol->number, 
			   &all_dep, &all_gdep);
	  tmpline = lastline;
	}

      while (tmpline)
	{
	  lastline = tmpline->Next;
	  Moteur_move_cell(This, lastcol->number, tmpline->number, 
			   lastcol->number, (tmpline->number)-num, 
			   &all_dep, &all_gdep);
	  tmpline = lastline;
	}
      lastcol = lastcol->Next;
    }
  pop_depend(This, all_dep, &all_gdep);
}

/* ----------------------------------------------------------------- ** 
** calcul_page_origin - calcul page origin for column or row         ** 
** ----------------------------------------------------------------- */
static coord_t Moteur_calcul_page_origin(This, type, pos, page_size)
c_Moteur *This;
int type;
int pos; 
coord_t page_size;
{
  int		i, nb_pos, nb_pos_by_page;
  coord_t	tmp_size, max_size;
  cell_size_list	*lsize;

  if (type == Xc_COLUMN)
    lsize = This->col_size;
  else
    lsize = This->line_size;
  
  tmp_size = COORD_ZERO;
  if (!pos) return tmp_size;
  i=0;
  while(lsize)
    {
      if (pos > lsize->Amax)
      {
	if (!lsize->size) 
	{
	  lsize = lsize->Next;
	  continue;
	}
	nb_pos = lsize->Amax - lsize->Amin + 1;
	max_size = nb_pos * lsize->size;
	if (tmp_size + max_size < page_size)
	  tmp_size += max_size;
	else
	{
	  i = (page_size - tmp_size) / lsize->size;
	  nb_pos -= i;
	  nb_pos_by_page = (page_size / lsize->size); 
	  if (nb_pos_by_page)
	  {
	    while( (nb_pos+1) * lsize->size > page_size)
	    {
	      nb_pos -= nb_pos_by_page;
	    }
	    tmp_size = nb_pos * lsize->size;
	  }
	  else
	    tmp_size = COORD_ZERO;
	}
      }
      else
      {
	if (!lsize->size) break;
	nb_pos = pos - lsize->Amin;
	i = (page_size - tmp_size) / lsize->size;
	if (nb_pos >= i)
	{
	  nb_pos_by_page = (page_size / lsize->size); 
	  if (nb_pos_by_page)
	  {
	    nb_pos -= i;
	    while((nb_pos+1) * lsize->size > page_size)
	    {
	      nb_pos -= nb_pos_by_page;
	    }
	    tmp_size = nb_pos * lsize->size;
	  }
	  else
	    tmp_size = COORD_ZERO;
	}
	else
	  tmp_size += nb_pos * lsize->size;
	break;
      }
      lsize = lsize->Next;
    }
  return tmp_size;
}

/* ----------------------------------------------------------------- ** 
** get_ideal_column_width - compute the ideal size for content       ** 
** ----------------------------------------------------------------- */
static coord_t Moteur_get_ideal_column_width(This, column, factor)
c_Moteur	*This;
int		column;
scale_t		factor;					   
{
  c_col		*tmpccol;
  c_line	*tmpcline;
  coord_t	width, height;
  coord_t	max_width, old_width;
  CellStyle_t	*cstyle;
  c_CellStyle	*cellstyle;
  
  tmpccol = This->tableCL;
  while (tmpccol && (tmpccol->number < column))
  {
    tmpccol = tmpccol->Next;
  }
  
  if (!tmpccol || tmpccol->number != column)
    return (coord_t)(-1);
  
  old_width = Moteur_get_size(This, Xc_COLUMN, column);
  max_width = (coord_t)(-1);
  cellstyle = This->BaseStd->cell_style_base;
  
  tmpcline = tmpccol->line;
  while (tmpcline)
  {
    if (tmpcline->cell.type != Xq_VOID)
      {
	cstyle = Moteur_get_cell_style(This, column, tmpcline->number);
	height = Moteur_get_size(This, Xc_ROW, tmpcline->number) * factor;
	width = F(cellstyle).CalculIdealCellWidth
	  (cellstyle, &(tmpcline->cell), cstyle, height, factor);
	if (width > max_width)
	  max_width = width;
      }
    tmpcline = tmpcline->Next;
    }
  if (max_width == old_width)
      return (coord_t)(-1);
  return max_width;
}

/* ----------------------------------------------------------------- ** 
** get_ideal_row_height - compute the ideal size for content         ** 
** ----------------------------------------------------------------- */
static coord_t Moteur_get_ideal_row_height(This, row, factor)
c_Moteur	*This;
int		row;
scale_t		factor;					   
{
  c_col		*tmpccol;
  c_line	*tmpcline;
  coord_t	height, width;
  coord_t	max_height, old_height;
  CellStyle_t	*cstyle;
  c_CellStyle	*cellstyle;
  
  old_height = Moteur_get_size(This, Xc_ROW, row);
  max_height = (coord_t)(-1);
  cellstyle = This->BaseStd->cell_style_base;
  
  tmpccol = This->tableCL;
  while (tmpccol)
    {
      tmpcline = tmpccol->line;
      while (tmpcline && (tmpcline->number < row))
	{
	  tmpcline = tmpcline->Next;
	}
      if (tmpcline && tmpcline->number == row &&
	  tmpcline->cell.type != Xq_VOID)
	{
	  cstyle = Moteur_get_cell_style(This, tmpccol->number, row);
	  width = Moteur_get_size(This, Xc_COLUMN, tmpccol->number) * factor; 
	  height = F(cellstyle).CalculIdealCellHeight(cellstyle, 
						      &(tmpcline->cell), 
						      cstyle, width, factor);
	  if (height > max_height)
	    max_height = height;
	}
      tmpccol = tmpccol->Next;
    }
  if (max_height == old_height)
    return (coord_t)(-1);
  return max_height;
}

/* ----------------------------------------------------------------- ** 
** get_filled_region - get restriction of region with filled cells   ** 
** ----------------------------------------------------------------- */
static boolean get_filled_region(This, region)
c_Moteur *This;
region_t *region;
{
  c_col		*tmpccol;
  c_line	*tmpcline;
  int		x_min, x_max, y_min, y_max, y;
  region_t	region2;
  
  memcpy(&region2, region, sizeof(region_t));
  x_min = x_max = y_min = y_max = 0;
  if (region->x_min == 0 && region->x_max == COLUMN_MAX)
    { /* whole line region */
      tmpccol = This->tableCL;
      
      x_min = COLUMN_MAX; x_max = 0;
      while (tmpccol)
	{
	  tmpcline = tmpccol->line;
	  while (tmpcline && (tmpcline->number < region->y_min))
	    tmpcline = tmpcline->Next;
	  
	  while(tmpcline && (tmpcline->number <= region->y_max))
	    {
	      if (tmpcline->cell.type != Xq_VOID)
		{
		  if (x_min > tmpccol->number)
		    x_min = tmpccol->number;
		  if (x_max < tmpccol->number)
		    x_max = tmpccol->number;
		  break;
		}
	      tmpcline = tmpcline->Next;
	    }
	  tmpccol = tmpccol->Next;
	}
      if (x_min <= x_max)
	{
	  region2.x_min = x_min;
	  region2.x_max = x_max;
	}
    }

  if (region->y_min == 0 && region->y_max == ROW_MAX)
    { /* whole column region */
      tmpccol = This->tableCL;
      
      while(tmpccol && !tmpccol->line)
	tmpccol = tmpccol->Next;
      if (!tmpccol) return FALSE;
      while (tmpccol && (tmpccol->number < region->x_min))
	tmpccol = tmpccol->Next;
      y_min = ROW_MAX; y_max = 0;
      
      while (tmpccol && (tmpccol->number <= region->x_max))
	{
	  tmpcline = tmpccol->line;
	  while(tmpcline)
	    {
	      if (tmpcline->cell.type != Xq_VOID)
		{
		  y = tmpcline->number;
		  if (y_min > y)
		    y_min = y;
		  
		  tmpcline = tmpcline->Next;
		  while(tmpcline)
		    {
		      if (tmpcline->cell.type != Xq_VOID)
			y = tmpcline->number;
		      tmpcline = tmpcline->Next;
		    }
		  if (y_max < y)
		    y_max = y;
		  break;
		}
	      tmpcline = tmpcline->Next;
	    }
	  tmpccol = tmpccol->Next;
	}
      if (y_min <= y_max)
	{
	  region2.y_min = y_min;
	  region2.y_max = y_max;
	}
    }
  
  if (x_min > x_max && y_min > y_max)
    return FALSE;
  
  memcpy(region, &region2, sizeof(region_t));
  
  return TRUE;
}
