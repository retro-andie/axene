/*
** BNP_Unit.c for XQuad in Box_nombres/
** Box Nombres Part that manage Color and Unit
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
** Last update Thu Feb 15 17:59:01 1996 Emmanuel Paris
*/

#include "BNP_Unit.h"

static void CreateBNP_Unit ___PROTO((c_Box_nombres *This,
				     bnp_unit_t *bnp));
static void ManageBNP_Unit ___PROTO((c_Box_nombres *This,
				     bnp_unit_t *bnp));
static void RefreshBNP_Unit ___PROTO((c_Box_nombres *This,
				      bnp_unit_t *bnp));
static void DestroyBNP_Unit ___PROTO((c_Box_nombres *This,
				      bnp_unit_t *bnp));

static void change_unitpos();
static void certify_unit();

bnp_std_t bnp_unit_name = 
{
  FALSE, FALSE,
  CreateBNP_Unit,
  ManageBNP_Unit,
  RefreshBNP_Unit,
  DestroyBNP_Unit,
  NULL
};

/* -------------------------------------------------------------------- ** 
** CreateBNP_Unit - BNP Unit Creator					** 
** -------------------------------------------------------------------- */
static void CreateBNP_Unit(This, bnp)
c_Box_nombres *This;
bnp_unit_t *bnp;
{
  Xc_HISTORY(("Create BNP Unit"));
  bnp->data = 
    (data_unit_t *)Xc_malloc("data", sizeof(data_unit_t));
  memset(bnp->data, 0, sizeof(data_unit_t)); 
  
  bnp->data->Grid = 
    (c_Grid *)NEW(c_Grid)(WIDGET(This->fSetup), NULL, "gBNUnit", TRUE);
  
  bnp->data->TBunitPos = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(bnp->data->Grid),
					  NULL, "tbBNUnitAfter", NULL);
   F(bnp->data->TBunitPos).Add_Callback(bnp->data->TBunitPos, 
					XmNvalueChangedCallback,
					(void *)change_unitpos,	This);
  
  bnp->data->LaUnitString = 
    (c_Label *) NEW(c_Label)(WIDGET(bnp->data->Grid), NULL,
			     "lBNUnitString");
  
  bnp->data->WTunitString = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNUnitString", TF_STRING,
				      0.0, (double)(XcMAX_NAME_LENGTH-1), "",
				      NULL, 10);
  F(bnp->data->WTunitString).add_callback(bnp->data->WTunitString,
					  (void *)certify_unit, This);
  
  F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			       WIDGET(bnp->data->TBunitPos), XcG_MAX_SIZE,
			       NULL);
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->LaUnitString), 230,
			       WIDGET(bnp->data->WTunitString), XcG_MAX_SIZE,
			       NULL);
  
  if (This->dim_calc)
  {
    XtVaSetValues(WIDGET(bnp->data->Grid), XmNwidth, This->width,
		  XmNheight, This->height, NULL);
    F(bnp->data->Grid).callResize(bnp->data->Grid);
  }
  
  XtManageChild(WIDGET(bnp->data->TBunitPos));
  XtManageChild(WIDGET(bnp->data->LaUnitString));
  XtManageChild(WIDGET(bnp->data->WTunitString));
  
  bnp->created = TRUE;
  bnp->managed = FALSE;
}

/* -------------------------------------------------------------------- ** 
** ManageBNP_Unit - BNP Unit Manager					** 
** -------------------------------------------------------------------- */
static void ManageBNP_Unit(This, bnp)
c_Box_nombres *This;
bnp_unit_t *bnp;
{
  if (bnp->managed)
  {
    Xc_HISTORY(("Unmanage BNP Unit"));
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
    Xc_HISTORY(("Manage BNP Unit"));
    
    RefreshBNP_Unit(This, bnp);
    XtManageChild(WIDGET(bnp->data->Grid));
  }
  bnp->managed ^= TRUE;
}

/* -------------------------------------------------------------------- ** 
** RefreshBNP_Unit - BNP Unit Refresh value				** 
** -------------------------------------------------------------------- */
static void RefreshBNP_Unit(This, bnp)
c_Box_nombres *This;
bnp_unit_t *bnp;
{
  boolean	tunitafter;
  char		tunit[XcMAX_NAME_LENGTH];
  
  Xc_HISTORY(("Refresh BNP Unit"));
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 9, &tunitafter) == -1)
    tunitafter = This->edited_format->unitafter;
  XmToggleButtonSetState(WIDGET(bnp->data->TBunitPos), tunitafter, False);
  
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 8, tunit) == -1)
    strcpy(tunit, This->edited_format->unit);
  F(bnp->data->WTunitString).set(bnp->data->WTunitString, tunit);
}

/* -------------------------------------------------------------------- ** 
** DestroyBNP_Unit - BNP Unit Destructor				** 
** -------------------------------------------------------------------- */
static void DestroyBNP_Unit(This, bnp)
c_Box_nombres *This;
bnp_unit_t *bnp;
{
  Xc_HISTORY(("Destroy BNP Unit"));
  
  DELETE(c_ToggleButton)(bnp->data->TBunitPos);
  DELETE(c_Label)(bnp->data->LaUnitString);
  DELETE(c_WTextField)(bnp->data->WTunitString);
  DELETE(c_Grid)(bnp->data->Grid);
  
  Xc_free(bnp->data);
  bnp->created = FALSE;
}

static void change_unitpos(This, Obj, cbs)
Widget		This;
c_Box_nombres	*Obj;
XmToggleButtonCallbackStruct *cbs;
{
  c_FormatNbr	**format;
  int		num2, i;
  boolean	bset;

  bset = (cbs->set ? TRUE : FALSE);
  if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					  (void ***)&format, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save, format[i], 9, &bset);
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
}

static void certify_unit(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr, tunit[XcMAX_NAME_LENGTH];
  c_FormatNbr		**format;
  int			num2, i;

  if ((vstr = F(this).get(this)) != NULL)
  {
    F(this).set(this, vstr);
    strcpy(tunit, vstr);
    
    if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					    (void ***)&format, &num2))
    {
      if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
      {
	F(Obj->list_save).set(Obj->list_save,
			      (c_FormatNbr *)Obj->PBox->edited_item,
			      8, tunit);
      }
      else
      {
	for(i = 0; i < num2; i++)
	  F(Obj->list_save).set(Obj->list_save, format[i], 8, tunit);
      }
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}

