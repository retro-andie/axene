/*
** BNP_Frac_Prec.c for XQuad in Box_nombres/
** Box Nombres Part that manage Fraction precision
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
** Last update Sun Dec  1 14:59:00 1996 Emmanuel Paris
*/

#include "BNP_Frac_Prec.h"

static void CreateBNP_Frac_Prec ___PROTO((c_Box_nombres *This,
					 bnp_frac_prec_t *bnp));
static void ManageBNP_Frac_Prec ___PROTO((c_Box_nombres *This,
					 bnp_frac_prec_t *bnp));
static void RefreshBNP_Frac_Prec ___PROTO((c_Box_nombres *This,
					   bnp_frac_prec_t *bnp));
static void DestroyBNP_Frac_Prec ___PROTO((c_Box_nombres *This,
					   bnp_frac_prec_t *bnp));

static void change_fracdec();
static void certify_fracprec();

bnp_std_t bnp_frac_prec = 
{
  FALSE, FALSE,
  CreateBNP_Frac_Prec,
  ManageBNP_Frac_Prec,
  RefreshBNP_Frac_Prec,
  DestroyBNP_Frac_Prec,
  NULL
};

/* -------------------------------------------------------------------- ** 
** CreateBNP_Frac_Prec - BNP Frac_Prec Creator				** 
** -------------------------------------------------------------------- */
static void CreateBNP_Frac_Prec(This, bnp)
c_Box_nombres *This;
bnp_frac_prec_t *bnp;
{
  Xc_HISTORY(("Create BNP Frac_Prec"));
  bnp->data = 
    (data_frac_prec_t *)Xc_malloc("data", sizeof(data_frac_prec_t));
  memset(bnp->data, 0, sizeof(data_frac_prec_t)); 
  
  bnp->data->Grid = 
    (c_Grid *)NEW(c_Grid)(WIDGET(This->fSetup), NULL, "gBNFracPrec", TRUE);
  
  bnp->data->TBfracDec = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(bnp->data->Grid),
					  NULL, "tbBNfracDecimal", NULL);
  F(bnp->data->TBfracDec).Add_Callback(bnp->data->TBfracDec, 
				       XmNvalueChangedCallback,
				       (void *)change_fracdec, This);
  
  bnp->data->LafracPrec = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
						  NULL, "lBNFracPrecision");
  
  bnp->data->WTfracPrec = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNfracPrecision",
				      TF_INTEGER, 1.0, 20.0, "2", NULL, 5);
  
  F(bnp->data->WTfracPrec).add_callback(bnp->data->WTfracPrec,
				       (void *)certify_fracprec,
					This);
  
  F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			       WIDGET(bnp->data->TBfracDec), XcG_MAX_SIZE,
			       NULL);
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->LafracPrec), XcG_MAX_SIZE,
			       WIDGET(bnp->data->WTfracPrec), XcG_BEST_SIZE,
			       NULL);
  
  XtManageChild(WIDGET(bnp->data->TBfracDec));
  XtManageChild(WIDGET(bnp->data->LafracPrec));
  XtManageChild(WIDGET(bnp->data->WTfracPrec));
  
  if (This->dim_calc)
  {
    XtVaSetValues(WIDGET(bnp->data->Grid), XmNwidth, This->width,
		  XmNheight, This->height, NULL);
    F(bnp->data->Grid).callResize(bnp->data->Grid);
  }

  bnp->created = TRUE;
  bnp->managed = FALSE;
}

/* -------------------------------------------------------------------- ** 
** ManageBNP_Frac_Prec - BNP Frac_Prec Manager				** 
** -------------------------------------------------------------------- */
static void ManageBNP_Frac_Prec(This, bnp)
c_Box_nombres *This;
bnp_frac_prec_t *bnp;
{
  if (bnp->managed)
  {
    Xc_HISTORY(("Unmanage BNP Frac_Prec"));
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
    Xc_HISTORY(("Manage BNP Frac_Prec"));
    RefreshBNP_Frac_Prec(This, bnp);
    XtManageChild(WIDGET(bnp->data->Grid));
  }
  bnp->managed ^= TRUE;
}

/* -------------------------------------------------------------------- ** 
** RefreshBNP_Frac_Prec - BNP Frac_Prec Refresh value			** 
** -------------------------------------------------------------------- */
static void RefreshBNP_Frac_Prec(This, bnp)
c_Box_nombres *This;
bnp_frac_prec_t *bnp;
{
  char		str_W[4];
  boolean	fracDec;
  unsigned int	fracPrec;
  Xc_HISTORY(("Refresh BNP Frac_Prec"));
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 15, &fracDec) == -1)
    fracDec = This->edited_format->frac_dec;
  XmToggleButtonSetState(WIDGET(bnp->data->TBfracDec), fracDec, False);
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 14, &fracPrec) == -1)
    fracPrec = This->edited_format->frac_prec;
  sprintf(str_W, "%d", fracPrec);
  F(bnp->data->WTfracPrec).set(bnp->data->WTfracPrec, str_W);
}

/* -------------------------------------------------------------------- ** 
** DestroyBNP_Frac_Prec - BNP Frac_Prec Destructor			** 
** -------------------------------------------------------------------- */
static void DestroyBNP_Frac_Prec(This, bnp)
c_Box_nombres *This;
bnp_frac_prec_t *bnp;
{
  Xc_HISTORY(("Destroy BNP Frac_Prec"));
  
  DELETE(c_ToggleButton)(bnp->data->TBfracDec);
  DELETE(c_Label)(bnp->data->LafracPrec);
  DELETE(c_WTextField)(bnp->data->WTfracPrec);
  
  DELETE(c_Grid)(bnp->data->Grid);
  
  Xc_free(bnp->data);
  bnp->created = FALSE;
}

static void change_fracdec(This, Obj, cbs)
Widget				This;
c_Box_nombres			*Obj;
XmToggleButtonCallbackStruct	*cbs;
{
  c_FormatNbr	**format;
  int		num2, i;
  boolean	bset;

  bset = (cbs->set ? TRUE : FALSE);
  if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					  (void ***)&format, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save, format[i], 15, &bset);
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
}

static void certify_fracprec(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  c_FormatNbr		**format;
  unsigned int		numf;
  int			num2, i;
  
  if((vstr = F(this).get(this)) != NULL)
  {
    numf = atoi(vstr);
    
    if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					    (void ***)&format, &num2))
    {
      if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
      {
	F(Obj->list_save).set(Obj->list_save,
			      (c_FormatNbr *)Obj->PBox->edited_item,
			      14, &numf);
      }
      else
      {
	for(i = 0; i < num2; i++)
	  F(Obj->list_save).set(Obj->list_save, format[i], 14, &numf);
      }
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}











