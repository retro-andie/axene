/*
** BNP_Boolean.c for XQuad in Box_nombres/
** Box Nombres Part that manage Boolean
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
** Last update Sun Dec  1 15:00:00 1996 Emmanuel Paris
*/

#include "BNP_Boolean.h"

static void CreateBNP_Boolean ___PROTO((c_Box_nombres *This,
				     bnp_boolean_t *bnp));
static void ManageBNP_Boolean ___PROTO((c_Box_nombres *This,
				     bnp_boolean_t *bnp));
static void RefreshBNP_Boolean ___PROTO((c_Box_nombres *This,
				      bnp_boolean_t *bnp));
static void DestroyBNP_Boolean ___PROTO((c_Box_nombres *This,
				      bnp_boolean_t *bnp));

static void certify_true_name();
static void certify_false_name();

bnp_std_t bnp_boolean_name = 
{
  FALSE, FALSE,
  CreateBNP_Boolean,
  ManageBNP_Boolean,
  RefreshBNP_Boolean,
  DestroyBNP_Boolean,
  NULL
};

/* -------------------------------------------------------------------- ** 
** CreateBNP_Boolean - BNP Boolean Creator					** 
** -------------------------------------------------------------------- */
static void CreateBNP_Boolean(This, bnp)
c_Box_nombres *This;
bnp_boolean_t *bnp;
{
  Xc_HISTORY(("Create BNP Boolean"));
  bnp->data = 
    (data_boolean_t *)Xc_malloc("data", sizeof(data_boolean_t));
  memset(bnp->data, 0, sizeof(data_boolean_t)); 
  
  bnp->data->Grid = 
    (c_Grid *)NEW(c_Grid)(WIDGET(This->fSetup), NULL, "gBNBoolean", TRUE);
    
  bnp->data->LaTrueString = 
    (c_Label *) NEW(c_Label)(WIDGET(bnp->data->Grid), NULL,
			     "lBNTrueString");
  
  bnp->data->WTTrueString = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNTrueString", TF_STRING,
				      0.0, (double)(XcMAX_NAME_LENGTH-1), "",
				      NULL, 10);
  F(bnp->data->WTTrueString).add_callback(bnp->data->WTTrueString,
					  (void *)certify_true_name, This);
  
  bnp->data->LaFalseString = 
    (c_Label *) NEW(c_Label)(WIDGET(bnp->data->Grid), NULL,
			     "lBNFalseString");
  
  bnp->data->WTFalseString = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNFalseString", TF_STRING,
				      0.0, (double)(XcMAX_NAME_LENGTH-1), "",
				      NULL, 10);
  F(bnp->data->WTFalseString).add_callback(bnp->data->WTFalseString,
					   (void *)certify_false_name, This);
  
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->LaTrueString), 200,
			       WIDGET(bnp->data->WTTrueString), XcG_MAX_SIZE,
			       NULL);
  F(bnp->data->Grid).addColumn(bnp->data->Grid, 5,
			       NULL);
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->LaFalseString), 200,
			       WIDGET(bnp->data->WTFalseString), XcG_MAX_SIZE,
			       NULL);
  
  if (This->dim_calc)
    {
      XtVaSetValues(WIDGET(bnp->data->Grid), XmNwidth, This->width,
		    XmNheight, This->height, NULL);
      F(bnp->data->Grid).callResize(bnp->data->Grid);
    }
  
  XtManageChild(WIDGET(bnp->data->LaTrueString));
  XtManageChild(WIDGET(bnp->data->WTTrueString));
  XtManageChild(WIDGET(bnp->data->LaFalseString));
  XtManageChild(WIDGET(bnp->data->WTFalseString));
  
  bnp->created = TRUE;
  bnp->managed = FALSE;
}

/* -------------------------------------------------------------------- ** 
** ManageBNP_Boolean - BNP Boolean Manager					** 
** -------------------------------------------------------------------- */
static void ManageBNP_Boolean(This, bnp)
c_Box_nombres *This;
bnp_boolean_t *bnp;
{
  if (bnp->managed)
  {
    Xc_HISTORY(("Unmanage BNP Boolean"));
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
    Xc_HISTORY(("Manage BNP Boolean"));
    
    RefreshBNP_Boolean(This, bnp);
    XtManageChild(WIDGET(bnp->data->Grid));
  }
  bnp->managed ^= TRUE;
}

/* -------------------------------------------------------------------- ** 
** RefreshBNP_Boolean - BNP Boolean Refresh value				** 
** -------------------------------------------------------------------- */
static void RefreshBNP_Boolean(This, bnp)
c_Box_nombres *This;
bnp_boolean_t *bnp;
{
  char		ttrue[XcMAX_NAME_LENGTH];
  char		tfalse[XcMAX_NAME_LENGTH];
  
  Xc_HISTORY(("Refresh BNP Boolean"));
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 16, ttrue) == -1)
    strcpy(ttrue, This->edited_format->true_name);
  F(bnp->data->WTTrueString).set(bnp->data->WTTrueString, ttrue);
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 17, tfalse) == -1)
    strcpy(tfalse, This->edited_format->false_name);
  F(bnp->data->WTFalseString).set(bnp->data->WTFalseString, tfalse);
}

/* -------------------------------------------------------------------- ** 
** DestroyBNP_Boolean - BNP Boolean Destructor				** 
** -------------------------------------------------------------------- */
static void DestroyBNP_Boolean(This, bnp)
c_Box_nombres *This;
bnp_boolean_t *bnp;
{
  Xc_HISTORY(("Destroy BNP Boolean"));
  
  DELETE(c_Label)(bnp->data->LaTrueString);
  DELETE(c_WTextField)(bnp->data->WTTrueString);
  DELETE(c_Label)(bnp->data->LaFalseString);
  DELETE(c_WTextField)(bnp->data->WTFalseString);
  DELETE(c_Grid)(bnp->data->Grid);
  
  Xc_free(bnp->data);
  bnp->created = FALSE;
}

static void certify_true_name(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr, ttrue[XcMAX_NAME_LENGTH];
  c_FormatNbr		**format;
  int			num2, i;

  if ((vstr = F(this).get(this)) != NULL)
  {
    F(this).set(this, vstr);
    strcpy(ttrue, vstr);
    
    if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					    (void ***)&format, &num2))
    {
      if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
      {
	F(Obj->list_save).set(Obj->list_save,
			      (c_FormatNbr *)Obj->PBox->edited_item,
			      16, ttrue);
      }
      else
      {
	for(i = 0; i < num2; i++)
	  F(Obj->list_save).set(Obj->list_save, format[i], 16, ttrue);
      }
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}

static void certify_false_name(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr, tfalse[XcMAX_NAME_LENGTH];
  c_FormatNbr		**format;
  int			num2, i;

  if ((vstr = F(this).get(this)) != NULL)
  {
    F(this).set(this, vstr);
    strcpy(tfalse, vstr);
    
    if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					    (void ***)&format, &num2))
    {
      if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
      {
	F(Obj->list_save).set(Obj->list_save,
			      (c_FormatNbr *)Obj->PBox->edited_item,
			      17, tfalse);
      }
      else
      {
	for(i = 0; i < num2; i++)
	  F(Obj->list_save).set(Obj->list_save, format[i], 17, tfalse);
      }
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}
























