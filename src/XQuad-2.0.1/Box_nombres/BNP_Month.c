/*
** BNP_Month.c for XQuad in Box_nombres/
** Box Nombres Part that manage Color and Month
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
** Last update Sun Dec  1 15:01:49 1996 Emmanuel Paris
*/

#include "BNP_Month.h"

static void CreateBNP_Month ___PROTO((c_Box_nombres *This,
				     bnp_month_t *bnp));
static void ManageBNP_Month ___PROTO((c_Box_nombres *This,
				     bnp_month_t *bnp));
static void RefreshBNP_Month ___PROTO((c_Box_nombres *This,
				      bnp_month_t *bnp));
static void DestroyBNP_Month ___PROTO((c_Box_nombres *This,
				      bnp_month_t *bnp));

static void change_very_short();
static void change_short();
static void change_long();

bnp_std_t bnp_month_name = 
{
  FALSE, FALSE,
  CreateBNP_Month,
  ManageBNP_Month,
  RefreshBNP_Month,
  DestroyBNP_Month,
  NULL
};

/* -------------------------------------------------------------------- ** 
** CreateBNP_Month - BNP Month Creator					** 
** -------------------------------------------------------------------- */
static void CreateBNP_Month(This, bnp)
c_Box_nombres *This;
bnp_month_t *bnp;
{
  Arg	argsw[3];
  
  Xc_HISTORY(("Create BNP Month"));
  bnp->data = 
    (data_month_t *)Xc_malloc("data", sizeof(data_month_t));
  memset(bnp->data, 0, sizeof(data_month_t)); 
  
  bnp->data->Grid = 
    (c_Grid *)NEW(c_Grid)(WIDGET(This->fSetup), NULL, "gBNMonth", TRUE);
  
  bnp->data->RBmonthNameSize = 
    (c_RadioBox *)NEW(c_RadioBox)(WIDGET(bnp->data->Grid),
				  NULL, RADIO_NORMAL,
				  "rbMonthNameSize",
				  "rbMonthNameVeryShort",
				  "rbMonthNameShort",
				  "rbMonthNameLong",
				  NULL);
  
  XtSetArg(argsw[0], XmNradioBehavior, True);
  XtSetArg(argsw[1], XmNradioAlwaysOne, True);
  XtSetValues(WIDGET(bnp->data->RBmonthNameSize), argsw, 2);
  
  F(bnp->data->RBmonthNameSize).AddCallback(bnp->data->RBmonthNameSize, 0, 
					  XmNvalueChangedCallback,
					  change_very_short, This);
  F(bnp->data->RBmonthNameSize).AddCallback(bnp->data->RBmonthNameSize, 1, 
					  XmNvalueChangedCallback,
					  change_short, This);
  F(bnp->data->RBmonthNameSize).AddCallback(bnp->data->RBmonthNameSize, 2, 
					  XmNvalueChangedCallback,
					  change_long, This);
  
  F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_BEST_SIZE,
			       WIDGET(bnp->data->RBmonthNameSize),
			       XcG_BEST_SIZE, NULL);
  
  if (This->dim_calc)
  {
    XtVaSetValues(WIDGET(bnp->data->Grid), XmNwidth, This->width,
		  XmNheight, This->height, NULL);
    F(bnp->data->Grid).callResize(bnp->data->Grid);
  }
  
  XtManageChild(WIDGET(bnp->data->RBmonthNameSize));
  
  bnp->created = TRUE;
  bnp->managed = FALSE;
}

/* -------------------------------------------------------------------- ** 
** ManageBNP_Month - BNP Month Manager					** 
** -------------------------------------------------------------------- */
static void ManageBNP_Month(This, bnp)
c_Box_nombres *This;
bnp_month_t *bnp;
{
  if (bnp->managed)
  {
    Xc_HISTORY(("Unmanage BNP Month"));
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
    Xc_HISTORY(("Manage BNP Month"));
    
    RefreshBNP_Month(This, bnp);
    XtManageChild(WIDGET(bnp->data->Grid));
  }
  bnp->managed ^= TRUE;
}

/* -------------------------------------------------------------------- ** 
** RefreshBNP_Month - BNP Month Refresh value				** 
** -------------------------------------------------------------------- */
static void RefreshBNP_Month(This, bnp)
c_Box_nombres *This;
bnp_month_t *bnp;
{
  name_size_id_t	name_size;
  listbutton	*buttonptr;
  
  Xc_HISTORY(("Refresh BNP Month"));
  
  if (F(This->list_save).get(This->list_save,
			     This->edited_format, 19, &name_size) == -1)
    name_size = This->edited_format->month_name_size;
  
  buttonptr = bnp->data->RBmonthNameSize->Button;
  switch(name_size)
    {
    case Xq_NAME_VERY_SHORT:
      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
      buttonptr = buttonptr->Next;
      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
      buttonptr = buttonptr->Next;
      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
      break;
    case Xq_NAME_SHORT:
      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
      buttonptr = buttonptr->Next;
      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
      buttonptr = buttonptr->Next;
      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
      break;
    case Xq_NAME_LONG:
      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
      buttonptr = buttonptr->Next;
      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
      buttonptr = buttonptr->Next;
      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
      break;
    }
}

/* -------------------------------------------------------------------- ** 
** DestroyBNP_Month - BNP Month Destructor				** 
** -------------------------------------------------------------------- */
static void DestroyBNP_Month(This, bnp)
c_Box_nombres *This;
bnp_month_t *bnp;
{
  Xc_HISTORY(("Destroy BNP Month"));
  
  DELETE(c_RadioBox)(bnp->data->RBmonthNameSize);
  DELETE(c_Grid)(bnp->data->Grid);
  
  Xc_free(bnp->data);
  bnp->created = FALSE;
}

static void change_very_short(This, Obj)
Widget		This;
c_Box_nombres	*Obj;
{
  c_FormatNbr	**format;
  int		num2, i;
  name_size_id_t	name_size = Xq_NAME_VERY_SHORT;

  if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					  (void ***)&format, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save, format[i], 19,
			      &name_size);
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
}

static void change_short(This, Obj)
Widget		This;
c_Box_nombres	*Obj;
{
  c_FormatNbr	**format;
  int		num2, i;
  name_size_id_t	name_size = Xq_NAME_SHORT;

  if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					  (void ***)&format, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save, format[i], 19,
			      &name_size);
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
}

static void change_long(This, Obj)
Widget		This;
c_Box_nombres	*Obj;
{
  c_FormatNbr	**format;
  int		num2, i;
  name_size_id_t	name_size = Xq_NAME_LONG;
  
  if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					  (void ***)&format, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save, format[i], 19,
			      &name_size);
      if (num2)
	Xc_free(format);
      F(Obj).refresh_preview(Obj);
    }
}

