/*
** BNP_Color.c for XQuad in Box_nombres/
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
** Last update Wed Mar 12 21:07:01 1997 Emmanuel Paris
*/

#include "BNP_Color.h"
#include "Resource.h"
#include "Moteur.h"

static void CreateBNP_Color ___PROTO((c_Box_nombres *This,
				      bnp_color_t *bnp));
static void ManageBNP_Color ___PROTO((c_Box_nombres *This,
				      bnp_color_t *bnp));
static void RefreshBNP_Color ___PROTO((c_Box_nombres *This,
				       bnp_color_t *bnp));
static void DestroyBNP_Color ___PROTO((c_Box_nombres *This,
				       bnp_color_t *bnp));

static void change_color1();
static void change_color2();

bnp_std_t bnp_color = 
{
 FALSE, FALSE,
 CreateBNP_Color,
 ManageBNP_Color,
 RefreshBNP_Color,
 DestroyBNP_Color,
 NULL
};

extern c_Color	*GlobColorAuto;
extern c_Resource *GlobResources;

/* -------------------------------------------------------------------- ** 
** CreateBNP_Color - BNP Color Creator					** 
** -------------------------------------------------------------------- */
static void CreateBNP_Color(This, bnp)
c_Box_nombres *This;
bnp_color_t *bnp;
{
 Xc_HISTORY(("Create BNP Color"));
 bnp->data = 
  (data_color_t *)Xc_malloc("data", sizeof(data_color_t));
 memset(bnp->data, 0, sizeof(data_color_t)); 
  
 bnp->data->Grid = 
  (c_Grid *)NEW(c_Grid)(WIDGET(This->fSetup), NULL, "gBNColors", TRUE);
  
 bnp->data->LaColor1 = (c_Label *)
  NEW(c_Label)(WIDGET(bnp->data->Grid), NULL, "lBNColor1");
 bnp->data->ClColor1 = (c_ColorList *)
  NEW(c_ColorList)(WIDGET(bnp->data->Grid), NULL, "clBNColor1", XcCL_NORMAL);
 F(bnp->data->ClColor1).setCallback(bnp->data->ClColor1,
				    change_color1, This);
 F(bnp->data->ClColor1).setColorBase(bnp->data->ClColor1, This->color_base,
				     CL_SKIP_TRANSPARENCY | CL_ADD_AUTOMATIC);
 
 bnp->data->LaColor2 = (c_Label *)
  NEW(c_Label)(WIDGET(bnp->data->Grid), NULL, "lBNColor2");
 bnp->data->ClColor2 = (c_ColorList *)
  NEW(c_ColorList)(WIDGET(bnp->data->Grid), NULL, "clBNColor2", XcCL_NORMAL);
 F(bnp->data->ClColor2).setCallback(bnp->data->ClColor2,
				    change_color2, This);
 F(bnp->data->ClColor2).setColorBase(bnp->data->ClColor2, This->color_base,
				     CL_SKIP_TRANSPARENCY | CL_ADD_AUTOMATIC);
 
 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->LaColor1), XcG_MAX_SIZE,
			      WIDGET(bnp->data->ClColor1), 122,
			      NULL);
 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->LaColor2), XcG_MAX_SIZE,
			      WIDGET(bnp->data->ClColor2), 122,
			      NULL);
 
 if (This->dim_calc)
 {
  XtVaSetValues(WIDGET(bnp->data->Grid), XmNwidth, This->width,
		XmNheight, This->height, NULL);
  F(bnp->data->Grid).callResize(bnp->data->Grid);
 }
  
 XtManageChild(WIDGET(bnp->data->LaColor1));
 XtManageChild(WIDGET(bnp->data->ClColor1));
 XtManageChild(WIDGET(bnp->data->LaColor2));
 XtManageChild(WIDGET(bnp->data->ClColor2));
  
 bnp->created = TRUE;
 bnp->managed = FALSE;
 bnp->data->col4sign = -1;
}

/* -------------------------------------------------------------------- ** 
** ManageBNP_Color - BNP Color Manager					** 
** -------------------------------------------------------------------- */
static void ManageBNP_Color(This, bnp)
c_Box_nombres *This;
bnp_color_t *bnp;
{
 Arg           args[1];
 XmString      xms1, xms2;
 boolean	col4sign;
  
 if (bnp->managed)
 {
  Xc_HISTORY(("Unmanage BNP Color"));
      
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
  Xc_HISTORY(("Manage BNP Color"));
      
  if (This->selected_type == Xq_BOOLEAN)
   col4sign = FALSE;
  else
   col4sign = TRUE;

  if (col4sign != bnp->data->col4sign)
  {
   bnp->data->col4sign = col4sign;
   if (col4sign)
   {
    xms1 = 
     F(GlobResources).getXmString(GlobResources, XcR_BnColPositive);
    xms2 = 
     F(GlobResources).getXmString(GlobResources, XcR_BnColNegative);
   }
   else
   {
    xms1 = 
     F(GlobResources).getXmString(GlobResources, XcR_BnColTrue);
    xms2 = 
     F(GlobResources).getXmString(GlobResources, XcR_BnColFalse);
   }
   XtSetArg(args[0], XmNlabelString, xms1);
   XtSetValues(WIDGET(bnp->data->LaColor1), args, 1);
   XtSetArg(args[0], XmNlabelString, xms2);
   XtSetValues(WIDGET(bnp->data->LaColor2), args, 1);
  }
  RefreshBNP_Color(This, bnp);
  XtManageChild(WIDGET(bnp->data->Grid));
 }
 bnp->managed ^= TRUE;
}

/* -------------------------------------------------------------------- ** 
** RefreshBNP_Color - BNP Color Refresh value				** 
** -------------------------------------------------------------------- */
static void RefreshBNP_Color(This, bnp)
c_Box_nombres *This;
bnp_color_t *bnp;
{
 c_Color	*tcolor;
  
 Xc_HISTORY(("Refresh BNP Color"));
 if (F(This->list_save).get(This->list_save,
			    This->edited_format, 4, &tcolor) == -1)
  tcolor = This->edited_format->color1;
 F(bnp->data->ClColor1).setColor(bnp->data->ClColor1, tcolor);
  
 if (F(This->list_save).get(This->list_save,
			    This->edited_format, 5, &tcolor) == -1)
  tcolor = This->edited_format->color2;
 F(bnp->data->ClColor2).setColor(bnp->data->ClColor2, tcolor);
}

/* -------------------------------------------------------------------- ** 
** DestroyBNP_Color - BNP Color Destructor		       		** 
** -------------------------------------------------------------------- */
static void DestroyBNP_Color(This, bnp)
c_Box_nombres *This;
bnp_color_t *bnp;
{
 Xc_HISTORY(("Destroy BNP Color"));
  
 DELETE(c_Label)(bnp->data->LaColor2);
 DELETE(c_ColorList)(bnp->data->ClColor2);
 DELETE(c_Label)(bnp->data->LaColor1);
 DELETE(c_ColorList)(bnp->data->ClColor1);
 DELETE(c_Grid)(bnp->data->Grid);
  
 Xc_free(bnp->data);
 bnp->created = FALSE;
}


static void change_color1(This, Obj, color)
c_ColorList	*This;
c_Box_nombres	*Obj;
c_Color		*color;
{
 c_FormatNbr	**format;
 int		num2, i;
  
 Xc_TRACE(("change color1: %s", color->name));
 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&format, &num2))
 {
  for(i = 0; i < num2; i++)
   F(Obj->list_save).set(Obj->list_save, format[i], 4, &color);
  if (num2)
   Xc_free(format);
  F(Obj).refresh_preview(Obj);
 }
} 

static void change_color2(This, Obj, color)
c_ColorList	*This;
c_Box_nombres	*Obj;
c_Color		*color;
{
 c_FormatNbr	**format;
 int		num2, i;
  
 Xc_TRACE(("change color2: %s", color->name));
 if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&format, &num2))
 {
  for(i = 0; i < num2; i++)
   F(Obj->list_save).set(Obj->list_save, format[i], 5, &color);
  if (num2)
   Xc_free(format);
  F(Obj).refresh_preview(Obj);
 }
}
