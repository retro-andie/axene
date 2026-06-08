/*
** BNP_Std_Prec.c for XQuad in Box_nombres/
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
** Last update Thu Feb 22 11:48:35 1996 Emmanuel Paris
*/

#include "BNP_Std_Prec.h"
#include "Moteur.h"

static void CreateBNP_Std_Prec ___PROTO((c_Box_nombres *This,
					 bnp_std_prec_t *bnp));
static void ManageBNP_Std_Prec ___PROTO((c_Box_nombres *This,
					 bnp_std_prec_t *bnp));
static void RefreshBNP_Std_Prec ___PROTO((c_Box_nombres *This,
					  bnp_std_prec_t *bnp));
static void DestroyBNP_Std_Prec ___PROTO((c_Box_nombres *This,
					  bnp_std_prec_t *bnp));

static void change_truncate();
static void certify_precmax();
static void certify_precmin();
static void certify_space_each();
static void certify_coma_sep();
static void certify_mill_sep();

bnp_std_t bnp_std_prec = 
{
  FALSE, FALSE,
  CreateBNP_Std_Prec,
  ManageBNP_Std_Prec,
  RefreshBNP_Std_Prec,
  DestroyBNP_Std_Prec,
  NULL
};

/* -------------------------------------------------------------------- ** 
** CreateBNP_Std_Prec - BNP Std_Prec Creator				** 
** -------------------------------------------------------------------- */
static void CreateBNP_Std_Prec(This, bnp)
c_Box_nombres *This;
bnp_std_prec_t *bnp;
{
  Xc_HISTORY(("Create BNP Std_Prec"));
  bnp->data = 
    (data_std_prec_t *)Xc_malloc("data", sizeof(data_std_prec_t));
  memset(bnp->data, 0, sizeof(data_std_prec_t));
  
  bnp->data->Grid = 
    (c_Grid *)NEW(c_Grid)(WIDGET(This->fSetup), NULL, "gBNStdPrec", TRUE);
  
  bnp->data->TBtruncate = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(bnp->data->Grid),
					  NULL, "tbBNTruncate", NULL);
  F(bnp->data->TBtruncate).Add_Callback(bnp->data->TBtruncate, 
					XmNvalueChangedCallback,
					(void *)change_truncate, This);
  
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
					 (void *)certify_coma_sep,
					 This);
  
  bnp->data->LaSepMString = (c_Label *)NEW(c_Label)
    (WIDGET(bnp->data->Grid), NULL, "lBNSeparatorMString");
  bnp->data->WTsepMString = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNSeparatorMString",
				      TF_STRING, 0.0, 1.0, " ", NULL, 5);
  F(bnp->data->WTsepMString).add_callback(bnp->data->WTsepMString,
					  (void *)certify_mill_sep,
					  This);
  
  bnp->data->LaSepPos = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid), NULL,
						"lBNSeparatorPosition");
  bnp->data->WTsepPos = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(bnp->data->Grid), NULL,
				      "tfBNSeparatorPosition",
				      TF_INTEGER, 0.0, 20.0, "3", NULL, 5);
  F(bnp->data->WTsepPos).add_callback(bnp->data->WTsepPos,
				      (void *)certify_space_each,
				      This);
  
  F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			       WIDGET(bnp->data->TBtruncate), XcG_MAX_SIZE,
			       NULL);
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
			       WIDGET(bnp->data->LaSepMString), XcG_MAX_SIZE,
			       WIDGET(bnp->data->WTsepMString), XcG_BEST_SIZE,
			       NULL);
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->LaSepPos), XcG_MAX_SIZE,
			       WIDGET(bnp->data->WTsepPos), XcG_BEST_SIZE,
			       NULL);
  
  if (This->dim_calc)
  {
    XtVaSetValues(WIDGET(bnp->data->Grid), XmNwidth, This->width,
		  XmNheight, This->height, NULL);
    F(bnp->data->Grid).callResize(bnp->data->Grid);
  }
  
  XtManageChild(WIDGET(bnp->data->TBtruncate));
  XtManageChild(WIDGET(bnp->data->LaPrecMax));
  XtManageChild(WIDGET(bnp->data->WTprecMax));
  XtManageChild(WIDGET(bnp->data->LaPrecMin));
  XtManageChild(WIDGET(bnp->data->WTprecMin));
  XtManageChild(bnp->data->separator);
  XtManageChild(WIDGET(bnp->data->LaSepString));
  XtManageChild(WIDGET(bnp->data->WTsepString));
  XtManageChild(WIDGET(bnp->data->LaSepMString));
  XtManageChild(WIDGET(bnp->data->WTsepMString));
  XtManageChild(WIDGET(bnp->data->LaSepPos));
  XtManageChild(WIDGET(bnp->data->WTsepPos));
  
  bnp->created = TRUE;
  bnp->managed = FALSE;
  bnp->data->enable_trunc = -1;
}

/* -------------------------------------------------------------------- ** 
** ManageBNP_Std_Prec - BNP Std_Prec Manager				** 
** -------------------------------------------------------------------- */
static void ManageBNP_Std_Prec(This, bnp)
c_Box_nombres *This;
bnp_std_prec_t *bnp;
{
  boolean	enable_trunc;
  
  if (bnp->managed)
  {
    Xc_HISTORY(("Unmanage BNP Std_Prec"));
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
    Xc_HISTORY(("Manage BNP Std_Prec"));
    if (This->selected_type == Xq_VALUE)
      enable_trunc = TRUE;
    else
      enable_trunc = FALSE;
    
    if (bnp->data->enable_trunc != enable_trunc)
    {
      bnp->data->enable_trunc = enable_trunc;
      if (enable_trunc)
      {
	XtSetSensitive(WIDGET(bnp->data->TBtruncate), True);
      }
      else
      {
	XtSetSensitive(WIDGET(bnp->data->TBtruncate), False);
      }
    }
    
    RefreshBNP_Std_Prec(This, bnp);
    XtManageChild(WIDGET(bnp->data->Grid));
  }
  bnp->managed ^= TRUE;
}

/* -------------------------------------------------------------------- ** 
** RefreshBNP_Std_Prec - BNP Std_Prec Refresh value			** 
** -------------------------------------------------------------------- */
static void RefreshBNP_Std_Prec(This, bnp)
c_Box_nombres *This;
bnp_std_prec_t *bnp;
{
  char		str_W[4];
  unsigned int	tmaxprec, tminprec, tspace;
  char		tcomasep, tcharsep;
  boolean	truncate;
  
  Xc_HISTORY(("Refresh BNP Std_Prec"));
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 7, &truncate) == -1)
    truncate = This->edited_format->truncate;
  XmToggleButtonSetState(WIDGET(bnp->data->TBtruncate), truncate, False);
  
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
			     This->edited_format, 2, &tspace) == -1)
    tspace = This->edited_format->space_each;
  sprintf(str_W, "%d", tspace);
  F(bnp->data->WTsepPos).set(bnp->data->WTsepPos, str_W);
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 3, &tcomasep) == -1)
    tcomasep = This->edited_format->comma_sep;
  sprintf(str_W, "%c", tcomasep);
  F(bnp->data->WTsepString).set(bnp->data->WTsepString, str_W);
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 6, &tcharsep) == -1)
    tcharsep = This->edited_format->char_sep;
  sprintf(str_W, "%c", tcharsep);
  F(bnp->data->WTsepMString).set(bnp->data->WTsepMString, str_W);
  
}

/* -------------------------------------------------------------------- ** 
** DestroyBNP_Std_Prec - BNP Std_Prec Destructor			** 
** -------------------------------------------------------------------- */
static void DestroyBNP_Std_Prec(This, bnp)
c_Box_nombres *This;
bnp_std_prec_t *bnp;
{  
  DELETE(c_ToggleButton)(bnp->data->TBtruncate);
  DELETE(c_Label)(bnp->data->LaPrecMin);
  DELETE(c_WTextField)(bnp->data->WTprecMin);
  DELETE(c_Label)(bnp->data->LaPrecMax);
  DELETE(c_WTextField)(bnp->data->WTprecMax);
  
  XtDestroyWidget(bnp->data->separator);
  DELETE(c_Label)(bnp->data->LaSepString);
  DELETE(c_WTextField)(bnp->data->WTsepString);
  DELETE(c_Label)(bnp->data->LaSepMString);
  DELETE(c_WTextField)(bnp->data->WTsepMString);
  DELETE(c_Label)(bnp->data->LaSepPos);
  DELETE(c_WTextField)(bnp->data->WTsepPos);
  
  DELETE(c_Grid)(bnp->data->Grid);
  
  Xc_free(bnp->data);
  bnp->created = FALSE;
}

static void change_truncate(This, Obj, cbs)
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
	F(Obj->list_save).set(Obj->list_save, format[i], 7, &bset);
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
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

static void certify_space_each(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  c_FormatNbr		**format;
  unsigned int		numf;
  int			num2, i;

  if((vstr = F(this).get(this)) != NULL)
    {
      F(this).set(this, vstr);
      numf = atoi(vstr);
      
      if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					     (void ***)&format, &num2) && num2)
      {
	if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
	{
	  F(Obj->list_save).set(Obj->list_save,
				(c_FormatNbr *)Obj->PBox->edited_item,
				2, &numf);
	}
	else
	{
	  for(i = 0; i < num2; i++)
	    F(Obj->list_save).set(Obj->list_save, format[i], 2, &numf);
	}
	if (num2)
	  Xc_free(format);
	F(Obj).refresh_preview(Obj);
      }
      Xc_free(vstr);
      F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
    }
}
			       
static void certify_coma_sep(this, Obj)
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

static void certify_mill_sep(this, Obj)
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
    
    if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					    (void ***)&format, &num2))
    {
      if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
      {
	F(Obj->list_save).set(Obj->list_save,
			      (c_FormatNbr *)Obj->PBox->edited_item,
			      6, &numf);
      }
      else
      {
	for(i = 0; i < num2; i++)
	  F(Obj->list_save).set(Obj->list_save,	format[i], 6, &numf);
      }
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}

