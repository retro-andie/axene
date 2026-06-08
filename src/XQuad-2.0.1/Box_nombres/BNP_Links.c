/*
** BNP_Links.c for XQuad in Box_nombres/
** Box Nombres Part that manage links to other format
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
** Started on  Wed Jan  3 22:16:04 1996 Emmanuel Paris
** Last update Sun Dec  1 14:56:58 1996 Emmanuel Paris
*/

#include "BNP_Links.h"
#include "Moteur.h"

static void CreateBNP_Links ___PROTO((c_Box_nombres *This,
				      bnp_links_t *bnp));
static void ManageBNP_Links ___PROTO((c_Box_nombres *This,
				      bnp_links_t *bnp));
static void RefreshBNP_Links ___PROTO((c_Box_nombres *This,
				       bnp_links_t *bnp));
static void DestroyBNP_Links ___PROTO((c_Box_nombres *This,
				       bnp_links_t *bnp));

bnp_std_t bnp_links = 
{
  FALSE, FALSE,
  CreateBNP_Links,
  ManageBNP_Links,
  RefreshBNP_Links,
  DestroyBNP_Links,
  NULL
};

typedef struct 
{
  char		*label_name;
  char		*itemMenu_name;
  boolean	(*filter)();
  void		(*callback)();
  int		type;
} bnl_info_t;

#ifdef ___GNUC_COMPOSE_MACRO

#define BNL_FILTER_MACRO(mtype, utype) \
static boolean format_##mtype##_filter(this) \
c_FormatNbr *this; \
{ \
    return ((this->type == utype && !this->hide_flag) \
	    || this->special == XcFt_GENERIC); \
} \
\
static void change_##mtype##_other_format(This, Obj, format) \
c_ItemMenu	*This; \
c_Box_nombres	*Obj; \
c_FormatNbr	*format; \
{ \
  c_FormatNbr	*tother_format[Xq_NB_TYPE]; \
  c_FormatNbr	**formatl; \
  int		num2, i; \
\
  Xc_TRACE(("change format: %s", format->name)); \
  if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList, \
					  (void ***)&formatl, &num2)) \
    { \
      for(i = 0; i < num2; i++) \
	{ \
	  if (F(Obj->list_save).get(Obj->list_save, \
				     formatl[i], 23, tother_format) == -1) \
	    memcpy(tother_format, formatl[i]->other_format, \
		   sizeof(c_FormatNbr *) * Xq_NB_TYPE); \
\
	  tother_format[utype] = format; \
	  F(Obj->list_save).set(Obj->list_save, formatl[i], \
				23, tother_format); \
	} \
      if (num2) \
	Xc_free(formatl); \
      F(Obj).refresh_preview(Obj); \
    } \
}

#else

#define BNL_FILTER_MACRO(mtype, utype) \
static boolean format_/**/mtype/**/_filter(this) \
c_FormatNbr *this; \
{ \
    return ((this->type == utype && !this->hide_flag) \
	    || this->special == XcFt_GENERIC); \
} \
\
static void change_/**/mtype/**/_other_format(This, Obj, format) \
c_ItemMenu	*This; \
c_Box_nombres	*Obj; \
c_FormatNbr	*format; \
{ \
  c_FormatNbr	*tother_format[Xq_NB_TYPE]; \
  c_FormatNbr	**formatl; \
  int		num2, i; \
\
  Xc_TRACE(("change format: %s", format->name)); \
  if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList, \
					  (void ***)&formatl, &num2)) \
    { \
      for(i = 0; i < num2; i++) \
	{ \
	  if (F(Obj->list_save).get(Obj->list_save, \
				     formatl[i], 23, tother_format) == -1) \
	    memcpy(tother_format, formatl[i]->other_format, \
		   sizeof(c_FormatNbr *) * Xq_NB_TYPE); \
\
	  tother_format[utype] = format; \
	  F(Obj->list_save).set(Obj->list_save, formatl[i], \
				23, tother_format); \
	} \
      if (num2) \
	Xc_free(formatl); \
      F(Obj).refresh_preview(Obj); \
    } \
}

#endif

BNL_FILTER_MACRO(normal, Xq_VALUE) 
BNL_FILTER_MACRO(unit, Xq_UNIT)
BNL_FILTER_MACRO(exponantial, Xq_EXPONANTIAL)
BNL_FILTER_MACRO(percent, Xq_PERCENT)
BNL_FILTER_MACRO(fraction, Xq_FRACTION)
BNL_FILTER_MACRO(boolean, Xq_BOOLEAN)
BNL_FILTER_MACRO(day, Xq_DAY_IN_WEEK)
BNL_FILTER_MACRO(month, Xq_MONTH)
BNL_FILTER_MACRO(time, Xq_TIME)
BNL_FILTER_MACRO(date, Xq_DATE)
BNL_FILTER_MACRO(date_time, Xq_DATE_TIME)

static bnl_info_t bnl_info[BNL_NB_FORMAT] =
{
  { "lBNLnormal", "imBNLnormal", format_normal_filter,
      change_normal_other_format, Xq_VALUE}, 
  { "lBNLunit", "imBNLunit", format_unit_filter,
      change_unit_other_format, Xq_UNIT},
  { "lBNLexponantial", "imBNLexponantial", format_exponantial_filter,
      change_exponantial_other_format, Xq_EXPONANTIAL},
  { "lBNLpercent", "imBNLpercent", format_percent_filter,
      change_percent_other_format, Xq_PERCENT},
  { "lBNLfraction", "imBNLfraction", format_fraction_filter,
      change_fraction_other_format, Xq_FRACTION},
  { "lBNLboolean", "imBNLboolean", format_boolean_filter,
      change_boolean_other_format, Xq_BOOLEAN},
  { "lBNLday", "imBNLday", format_day_filter,
      change_day_other_format, Xq_DAY_IN_WEEK},
  { "lBNLmonth", "imBNLmonth", format_month_filter,
      change_month_other_format, Xq_MONTH},
  { "lBNLtime", "imBNLtime", format_time_filter,
      change_time_other_format, Xq_TIME},
  { "lBNLdate", "imBNLdate", format_date_filter,
      change_date_other_format, Xq_DATE},
  { "lBNLdate_time", "imBNLdate_time", format_date_time_filter,
      change_date_time_other_format, Xq_DATE_TIME},
};

/* -------------------------------------------------------------------- ** 
** CreateBNP_Links - BNP Links Creator					** 
** -------------------------------------------------------------------- */
static void CreateBNP_Links(This, bnp)
c_Box_nombres *This;
bnp_links_t *bnp;
{
  int	i;
  
  Xc_HISTORY(("Create BNP Links"));
  bnp->data = 
    (data_links_t *)Xc_malloc("data", sizeof(data_links_t));
  memset(bnp->data, 0, sizeof(data_links_t)); 
  
  bnp->data->Grid = 
    (c_Grid *)NEW(c_Grid)(WIDGET(This->fSetup), NULL, "gBNLinks", TRUE);
  
  for(i = 0; i < BNL_NB_FORMAT; i++)
    {
      bnp->data->Labels[i] = 
	(c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid), NULL,
				bnl_info[i].label_name);
      
      bnp->data->ItemMenus[i] = 
	(c_ItemMenu *)NEW(c_ItemMenu)(WIDGET(bnp->data->Grid), NULL,
				      bnl_info[i].itemMenu_name, 0, 0);
      F(bnp->data->ItemMenus[i]).setCallback(bnp->data->ItemMenus[i],
					     (void *)(bnl_info[i].callback),
					     This);
      F(bnp->data->ItemMenus[i]).set(bnp->data->ItemMenus[i],
				     FALSE, offsetof(c_FormatNbr, name),
				     NULL, (void *)(bnl_info[i].filter));
      F(bnp->data->ItemMenus[i]).addItems(bnp->data->ItemMenus[i],
					  XcIM_LINKED_LIST, -1,
					  *This->format_base,
					  offsetof(c_FormatNbr, next));
      F(bnp->data->ItemMenus[i]).addItems(bnp->data->ItemMenus[i],
					  XcIM_LINKED_LIST, -1,
					  This->v_format_base,
					  offsetof(c_FormatNbr, next));
      F(bnp->data->ItemMenus[i]).deleteItems(bnp->data->ItemMenus[i],
					     XcIL_POINTER_ARRAY, 
					     This->d_nb_format,
					     This->d_format_base);
    }
  
  for(i = 0; i < BNL_NB_FORMAT; i++)
    {
      F(bnp->data->Grid).addColumn
	(bnp->data->Grid, XcG_BEST_SIZE,
	 WIDGET(bnp->data->Labels[i]), 120,
	 WIDGET(bnp->data->ItemMenus[i]), XcG_MAX_SIZE,
	 NULL);
    }
  
  for(i = 0; i < BNL_NB_FORMAT; i++)
    {
      XtManageChild(WIDGET(bnp->data->Labels[i]));
      XtManageChild(WIDGET(bnp->data->ItemMenus[i]));
    }
  
  if (This->dim_calc)
    {
      XtVaSetValues(WIDGET(bnp->data->Grid), XmNwidth, This->width,
		    XmNheight, This->height, NULL);
      F(bnp->data->Grid).callResize(bnp->data->Grid);
    }
  
  bnp->created = TRUE;
  bnp->managed = FALSE;
  bnp->data->type_unmanaged = -1;
}

/* -------------------------------------------------------------------- ** 
** ManageBNP_Links - BNP Links Manager					** 
** -------------------------------------------------------------------- */
static void ManageBNP_Links(This, bnp)
c_Box_nombres *This;
bnp_links_t *bnp;
{
  int	i;

  if (bnp->managed)
  {
    Xc_HISTORY(("Unmanage BNP Links"));
    if (!This->dim_calc)
      {
	XtVaGetValues(WIDGET(bnp->data->Grid), XmNwidth, &This->width,
		      XmNheight, &This->height, NULL);
	Xc_TRACE(("Grid size: %d x %d", This->width, This->height));
	This->dim_calc = TRUE;
      } 
    XtUnmanageChild(WIDGET(bnp->data->Grid));
  }
  else
  {
    Xc_HISTORY(("Manage BNP Links"));

    if (This->selected_type != bnp->data->type_unmanaged)
      {
	if (bnp->data->type_unmanaged != -1)
	  {
	    i = 0;
	    while( i < BNL_NB_FORMAT && 
		  bnl_info[i].type != bnp->data->type_unmanaged)
	      i++;

	    if (i < BNL_NB_FORMAT)
	      {
		XtSetSensitive(WIDGET(bnp->data->Labels[i]), True);
		XtSetSensitive(WIDGET(bnp->data->ItemMenus[i]), True);
	      }
	  }
	bnp->data->type_unmanaged = This->selected_type;
	 i = 0;
	while( i < BNL_NB_FORMAT && 
	      bnl_info[i].type != bnp->data->type_unmanaged)
	  i++;
	
	if (i < BNL_NB_FORMAT)
	  {
	    XtSetSensitive(WIDGET(bnp->data->Labels[i]), False);
	    XtSetSensitive(WIDGET(bnp->data->ItemMenus[i]), False);
	  }
      }
    RefreshBNP_Links(This, bnp);
    F(bnp->data->Grid).callResize(bnp->data->Grid);
    XtManageChild(WIDGET(bnp->data->Grid)); 
  }
  bnp->managed ^= TRUE;
}

/* -------------------------------------------------------------------- ** 
** RefreshBNP_Links - BNP Links Refresh value				** 
** -------------------------------------------------------------------- */
static void RefreshBNP_Links(This, bnp)
c_Box_nombres *This;
bnp_links_t *bnp;
{
  int	i;
  c_FormatNbr	*sel_format;
  c_FormatNbr	*tother_format[Xq_NB_TYPE];
  Xc_HISTORY(("Refresh BNP Links"));
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 23, tother_format) == -1)
    memcpy(tother_format, This->edited_format->other_format,
	   sizeof(c_FormatNbr *) * Xq_NB_TYPE);
  
  for(i = 0; i < BNL_NB_FORMAT; i++)
    {
      if ( bnl_info[i].type != bnp->data->type_unmanaged)
	{
	  sel_format = tother_format[bnl_info[i].type];
	  if (sel_format == NULL)
	    sel_format = This->generic_format;
	}
      else
	sel_format = This->edited_format;
      F(bnp->data->ItemMenus[i]).setSelection(bnp->data->ItemMenus[i],
					      sel_format);
    }
}

/* -------------------------------------------------------------------- ** 
** DestroyBNP_Links - BNP Links Destructor				** 
** -------------------------------------------------------------------- */
static void DestroyBNP_Links(This, bnp)
c_Box_nombres *This;
bnp_links_t *bnp;
{
  int	i;
  
  Xc_HISTORY(("Destroy BNP Links"));
  
  for(i = 0; i < BNL_NB_FORMAT; i++)
    {
      DELETE(c_Label)(bnp->data->Labels[i]);
      DELETE(c_ItemMenu)(bnp->data->ItemMenus[i]);
    }
  
  DELETE(c_Grid)(bnp->data->Grid);
  
  Xc_free(bnp->data);
  bnp->created = FALSE;
}


