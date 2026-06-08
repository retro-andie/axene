/*
** BNP_Exp_Prec.c for XQuad in Box_nombres/
** Box Nombres Part that manage Standard precision
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
** Last update Sun Jan 16 18:24:36 2000 Emmanuel Paris
*/

#include "BNP_Exp_Prec.h"
#include "Moteur.h"

static void CreateBNP_Exp_Prec ___PROTO((c_Box_nombres *This,
					 bnp_exp_prec_t *bnp));
static void ManageBNP_Exp_Prec ___PROTO((c_Box_nombres *This,
					 bnp_exp_prec_t *bnp));
static void RefreshBNP_Exp_Prec ___PROTO((c_Box_nombres *This,
					  bnp_exp_prec_t *bnp));
static void DestroyBNP_Exp_Prec ___PROTO((c_Box_nombres *This,
					  bnp_exp_prec_t *bnp));

static void certify_precmax();
static void certify_precmin();
static void certify_comma_sep();
static void certify_exp_str();

static void certify_exp_digit();	/* nb digit de l'exposant    tf */
static void certify_exp_multiple();	/* multiple de l'exposant    tf */
static void change_expsign();		/* signe de l'exposant       tb */


bnp_std_t bnp_exp_prec = 
{
  FALSE, FALSE,
  CreateBNP_Exp_Prec,
  ManageBNP_Exp_Prec,
  RefreshBNP_Exp_Prec,
  DestroyBNP_Exp_Prec,
  NULL
};

/* -------------------------------------------------------------------- ** 
** CreateBNP_Exp_Prec - BNP Exp_Prec Creator				** 
** -------------------------------------------------------------------- */
static void CreateBNP_Exp_Prec(This, bnp)
c_Box_nombres *This;
bnp_exp_prec_t *bnp;
{
  Xc_HISTORY(("Create BNP Exp_Prec"));
  bnp->data = 
    (data_exp_prec_t *)Xc_malloc("data", sizeof(data_exp_prec_t));
  memset(bnp->data, 0, sizeof(data_exp_prec_t));
  
  bnp->data->Grid = 
    (c_Grid *)NEW(c_Grid)(WIDGET(This->fSetup), NULL, "gBNExpPrec", TRUE);
  
  bnp->data->LaPrecMax = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
						 NULL, "lBNMaxPrecision");
  bnp->data->WTprecMax = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNMaxPrecision",
				      TF_INTEGER, 0.0, 20.0, "20", NULL, 5);
  F(bnp->data->WTprecMax).add_callback(bnp->data->WTprecMax,
				       (void *)certify_precmax,
				       This);
  
  bnp->data->LaPrecMin = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
						 NULL, "lBNMinPrecision");
  bnp->data->WTprecMin = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNMinPrecision",
				      TF_INTEGER, 0.0, 20.0, "0", NULL, 5);
  F(bnp->data->WTprecMin).add_callback(bnp->data->WTprecMin,
				       (void *)certify_precmin,
				       This);
  
  bnp->data->separator = XmCreateSeparatorGadget(WIDGET(bnp->data->Grid),
						 "___separator___", NULL, 0);
  
  bnp->data->LaSepString = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
						   NULL, "lBNSeparatorString");
  bnp->data->WTsepString = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNSeparatorString",
				      TF_STRING, 0.0, 1.0, " ", NULL, 5);
  F(bnp->data->WTsepString).add_callback(bnp->data->WTsepString,
					 (void *)certify_comma_sep,
					 This);
  
  bnp->data->LaSepEString = (c_Label *)NEW(c_Label)
    (WIDGET(bnp->data->Grid), NULL, "lBNStringEString");
  bnp->data->WTsepEString = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNStringEString",
				      TF_STRING, 0.0, 
				      (double)(XcMAX_NAME_LENGTH-1),
				      " ", NULL, 5);
  F(bnp->data->WTsepEString).add_callback(bnp->data->WTsepEString,
					  (void *)certify_exp_str,
					  This);
  
  bnp->data->LaExpMinDigit = (c_Label *)NEW(c_Label)
    (WIDGET(bnp->data->Grid), NULL, "lBNExpMinDigit");
  bnp->data->WTExpMinDigit =
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNExpMinDigit",
				      TF_INTEGER, 
				      0.0, 10.0,
				      "0", NULL, 5);
  F(bnp->data->WTExpMinDigit).add_callback(bnp->data->WTExpMinDigit,
					   (void *)certify_exp_digit,
					   This);
  
  bnp->data->LaExpMul = (c_Label *)NEW(c_Label)
    (WIDGET(bnp->data->Grid), NULL, "lBNExpMul");
  bnp->data->WTExpMul = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNExpMul",
				      TF_INTEGER, 
				      1.0, 20.0,
				      "1", NULL, 5);
  F(bnp->data->WTExpMul).add_callback(bnp->data->WTExpMul,
					   (void *)certify_exp_multiple,
					   This);
  
  bnp->data->tbExpSign = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(bnp->data->Grid),
					  NULL, "tbBNExpSign", NULL);
  F(bnp->data->tbExpSign).Add_Callback(bnp->data->tbExpSign, 
				       XmNvalueChangedCallback,
				       (void *)change_expsign, This);
  
  
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->LaPrecMax), XcG_MAX_SIZE,
			       WIDGET(bnp->data->WTprecMax), XcG_BEST_SIZE,
			       NULL);
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->LaPrecMin), XcG_MAX_SIZE,
			       WIDGET(bnp->data->WTprecMin), XcG_BEST_SIZE,
			       NULL);
  
  F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			       bnp->data->separator, XcG_MAX_SIZE,
			       NULL);
  
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->LaSepString), XcG_MAX_SIZE,
			       WIDGET(bnp->data->WTsepString), XcG_BEST_SIZE,
			       NULL);
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->LaSepEString), XcG_MAX_SIZE,
			       WIDGET(bnp->data->WTsepEString), XcG_BEST_SIZE,
			       NULL);
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->LaExpMinDigit), XcG_MAX_SIZE,
			       WIDGET(bnp->data->WTExpMinDigit), XcG_BEST_SIZE,
			       NULL);
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->LaExpMul), XcG_MAX_SIZE,
			       WIDGET(bnp->data->WTExpMul), XcG_BEST_SIZE,
			       NULL);
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->tbExpSign), XcG_MAX_SIZE,
			       NULL);
  
  
  if (This->dim_calc)
  {
    XtVaSetValues(WIDGET(bnp->data->Grid), XmNwidth, This->width,
		  XmNheight, This->height, NULL);
    F(bnp->data->Grid).callResize(bnp->data->Grid);
  }
  
  XtManageChild(WIDGET(bnp->data->LaPrecMax));
  XtManageChild(WIDGET(bnp->data->WTprecMax));
  XtManageChild(WIDGET(bnp->data->LaPrecMin));
  XtManageChild(WIDGET(bnp->data->WTprecMin));
  XtManageChild(bnp->data->separator);
  XtManageChild(WIDGET(bnp->data->LaSepString));
  XtManageChild(WIDGET(bnp->data->WTsepString));
  XtManageChild(WIDGET(bnp->data->LaSepEString));
  XtManageChild(WIDGET(bnp->data->WTsepEString));
  XtManageChild(WIDGET(bnp->data->LaExpMinDigit));
  XtManageChild(WIDGET(bnp->data->WTExpMinDigit));
  XtManageChild(WIDGET(bnp->data->LaExpMul));
  XtManageChild(WIDGET(bnp->data->WTExpMul));
  XtManageChild(WIDGET(bnp->data->tbExpSign));
  
  bnp->created = TRUE;
  bnp->managed = FALSE;
  bnp->data->enable_trunc = -1;
}

/* -------------------------------------------------------------------- ** 
** ManageBNP_Exp_Prec - BNP Exp_Prec Manager				** 
** -------------------------------------------------------------------- */
static void ManageBNP_Exp_Prec(This, bnp)
c_Box_nombres *This;
bnp_exp_prec_t *bnp;
{
  if (bnp->managed)
  {
    Xc_HISTORY(("Unmanage BNP Exp_Prec"));
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
    Xc_HISTORY(("Manage BNP Exp_Prec"));
    
    RefreshBNP_Exp_Prec(This, bnp);
    XtManageChild(WIDGET(bnp->data->Grid));
  }
  bnp->managed ^= TRUE;
}

/* -------------------------------------------------------------------- ** 
** RefreshBNP_Exp_Prec - BNP Exp_Prec Refresh value			** 
** -------------------------------------------------------------------- */
static void RefreshBNP_Exp_Prec(This, bnp)
c_Box_nombres *This;
bnp_exp_prec_t *bnp;
{
  char		str_W[4];
  boolean	texpsign;
  unsigned int	tmaxprec, tminprec, texpdigit, texpmultiple;
  char		tcomasep, texpstring[XcMAX_NAME_LENGTH];
  
  Xc_HISTORY(("Refresh BNP Exp_Prec"));
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 0, &tmaxprec) == -1)
    tmaxprec = This->edited_format->max_prec;
  sprintf(str_W, "%d", tmaxprec);
  F(bnp->data->WTprecMax).set(bnp->data->WTprecMax, str_W);
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 1, &tminprec) == -1)
    tminprec = This->edited_format->min_prec;
  sprintf(str_W, "%d", tminprec);
  F(bnp->data->WTprecMin).set(bnp->data->WTprecMin, str_W);
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 3, &tcomasep) == -1)
    tcomasep = This->edited_format->comma_sep;
  sprintf(str_W, "%c", tcomasep);
  F(bnp->data->WTsepString).set(bnp->data->WTsepString, str_W);
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 13, &texpstring) == -1)
     strcpy(texpstring, This->edited_format->exp_string);
  F(bnp->data->WTsepEString).set(bnp->data->WTsepEString, texpstring);
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 10, &texpdigit) == -1)
    texpdigit = This->edited_format->exp_digit;
  sprintf(str_W, "%d", texpdigit);
  F(bnp->data->WTExpMinDigit).set(bnp->data->WTExpMinDigit, str_W);
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 11, &texpmultiple) == -1)
    texpmultiple = This->edited_format->exp_multiple;
  sprintf(str_W, "%d", texpmultiple);
  F(bnp->data->WTExpMul).set(bnp->data->WTExpMul, str_W);
    
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 12, &texpsign) == -1)
    texpsign = This->edited_format->exp_sign;
  XmToggleButtonSetState(WIDGET(bnp->data->tbExpSign), texpsign, False);
}

/* -------------------------------------------------------------------- ** 
** DestroyBNP_Exp_Prec - BNP Exp_Prec Destructor			** 
** -------------------------------------------------------------------- */
static void DestroyBNP_Exp_Prec(This, bnp)
c_Box_nombres *This;
bnp_exp_prec_t *bnp;
{  
  DELETE(c_Label)(bnp->data->LaPrecMin);
  DELETE(c_WTextField)(bnp->data->WTprecMin);
  DELETE(c_Label)(bnp->data->LaPrecMax);
  DELETE(c_WTextField)(bnp->data->WTprecMax);
  
  XtDestroyWidget(bnp->data->separator);
  DELETE(c_Label)(bnp->data->LaSepString);
  DELETE(c_WTextField)(bnp->data->WTsepString);
  DELETE(c_Label)(bnp->data->LaSepEString);
  DELETE(c_WTextField)(bnp->data->WTsepEString);
  DELETE(c_Label)(bnp->data->LaExpMinDigit);
  DELETE(c_WTextField)(bnp->data->WTExpMinDigit);
  DELETE(c_Label)(bnp->data->LaExpMul);
  DELETE(c_WTextField)(bnp->data->WTExpMul);
  DELETE(c_ToggleButton)(bnp->data->tbExpSign);
  
  DELETE(c_Grid)(bnp->data->Grid);
  
  Xc_free(bnp->data);
  bnp->created = FALSE;
}

static void certify_precmax(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  char			tstr[20];
  c_FormatNbr		**format;
  unsigned int		numf;
  int			num2, i;
  unsigned int		tminprec, tset;
  
  if((vstr = F(this).get(this)) != NULL)
  {
    numf = atoi(vstr);
    
    if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					    (void ***)&format, &num2))
    {
      if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
      {
	if (F(Obj->list_save).get(Obj->list_save,
				  (c_FormatNbr *)Obj->PBox->edited_item, 
				  1, &tminprec) == -1)
	  tminprec = ((c_FormatNbr *)Obj->PBox->edited_item)->min_prec;
	if (numf < tminprec)
	  tset = tminprec;
	else
	  tset = numf;
	
	F(Obj->list_save).set(Obj->list_save,
			      (c_FormatNbr *)Obj->PBox->edited_item,
			      0, &tset);
	
	sprintf(tstr, "%d", tset);
	F(this).set(this, tstr);
      }
      else
      {
	for(i = 0; i < num2; i++)
	{
	  if (F(Obj->list_save).get(Obj->list_save, format[i], 1,
				    &tminprec) == -1)
	    tminprec = format[i]->min_prec;
	  if (numf < tminprec)
	    tset = tminprec;
	  else
	    tset = numf;
	  F(Obj->list_save).set(Obj->list_save, format[i], 0, &tset);
	  if (!i)
	  {
	    if (tset != numf)
	    {
	      sprintf(tstr, "%d", tset);
	      F(this).set(this, tstr);
	    }
	  }
	}
      }
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}

static void certify_precmin(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  char			tstr[20];
  c_FormatNbr		**format;
  unsigned int		numf;
  int			num2, i;
  unsigned int		tmaxprec, tset;

  if((vstr = F(this).get(this)) != NULL)
  {
    numf = atoi(vstr);
    
    if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					   (void ***)&format, &num2) && num2)
    {
      if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
      {
	if (F(Obj->list_save).get(Obj->list_save,
				  (c_FormatNbr *)Obj->PBox->edited_item, 
				  0, &tmaxprec) == -1)
	  tmaxprec = ((c_FormatNbr *)Obj->PBox->edited_item)->max_prec;
	if (numf > tmaxprec)
	  tset = tmaxprec;
	else
	  tset = numf;
	
	F(Obj->list_save).set(Obj->list_save,
			      (c_FormatNbr *)Obj->PBox->edited_item,
			      1, &tset);
	sprintf(tstr, "%d", tset);
	F(this).set(this, tstr);
      }
      else
      {
	for(i = 0; i < num2; i++)
	{
	  if (F(Obj->list_save).get(Obj->list_save, format[i], 
				    0, &tmaxprec) == -1)
	    tmaxprec = format[i]->max_prec;
	  if (numf > tmaxprec)
	    tset = tmaxprec;
	  else
	    tset = numf;
	  
	  F(Obj->list_save).set(Obj->list_save,	format[i], 1, &tset);
	  if (!i)
	  {
	    if (tset != numf)
	    {
	      sprintf(tstr, "%d", tset);
	      F(this).set(this, tstr);
	    }
	  }
	}
      }
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}

static void certify_comma_sep(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  c_FormatNbr		**format;
  char			numf;
  int			num2, i;
  
  if((vstr = F(this).get(this)) != NULL)
  {
    F(this).set(this, vstr);
    numf = vstr[0];
    
    if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					   (void ***)&format, &num2) && num2)
    {
      if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
      {
	F(Obj->list_save).set(Obj->list_save,
			      (c_FormatNbr *)Obj->PBox->edited_item,
			      3, &numf);
      }
      else
      {
	for(i = 0; i < num2; i++)
	  F(Obj->list_save).set(Obj->list_save,	format[i], 3, &numf);
      }
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}

static void certify_exp_digit(this, Obj)
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
			      10, &numf);
      }
      else
      {
	for(i = 0; i < num2; i++)
	  F(Obj->list_save).set(Obj->list_save,	format[i], 10, &numf);
      }
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}

static void certify_exp_multiple(this, Obj)
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
			      11, &numf);
      }
      else
      {
	for(i = 0; i < num2; i++)
	  F(Obj->list_save).set(Obj->list_save,	format[i], 11, &numf);
      }
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}

static void change_expsign(This, Obj, cbs)
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
	F(Obj->list_save).set(Obj->list_save, format[i], 12, &bset);
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
}

static void certify_exp_str(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  c_FormatNbr		**format;
  int			num2, i;

  if((vstr = F(this).get(this)) != NULL)
  {
    F(this).set(this, vstr);
    
    if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					    (void ***)&format, &num2))
    {
      if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
      {
	F(Obj->list_save).set(Obj->list_save,
			      (c_FormatNbr *)Obj->PBox->edited_item,
			      13, vstr);
      }
      else
      {
	for(i = 0; i < num2; i++)
	  F(Obj->list_save).set(Obj->list_save,	format[i], 13, vstr);
      }
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}











