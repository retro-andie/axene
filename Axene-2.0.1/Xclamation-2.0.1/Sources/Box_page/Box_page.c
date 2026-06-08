/*
** Box_page.c for Xclamation in Box_page/
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
** Started on  Mon Jul 17 14:03:57 1995 Robin Castanier
** Last update Sun Feb  8 16:15:41 1998 Emmanuel Paris
*/

#include "Box_page.h"
#include "../Pixmaps/document.portrait.zpm"
#include "../Pixmaps/document.paysage.zpm"
#include "../Pixmaps/previous.zpm"
#include "../Pixmaps/next.zpm"

void *cons_Box_page();
void dest_Box_page();
void *copy_Box_page();

void changeformat();
static void page_plusplus();
static void page_moinsmoins();
static void Change_page ___PROTO((c_Box_page *Obj, int new_page));
static void certify_pagenum ___PROTO((c_WTextField *this,
				      c_Box_page *Obj));
static void certify_size ___PROTO((c_WTextField *this,
				   c_Box_page *Obj));
static void certify_marge ___PROTO((c_WTextField *this,
				    c_Box_page *Obj));
static void certify_decoupage ___PROTO((c_WTextField *this,
					c_Box_page *Obj));
static void certify_gouttiere ___PROTO((c_WTextField *this,
					c_Box_page *Obj));
static void cb_format ___PROTO((c_ItemMenu *popup, c_Box_page *this,
				page_format_t *format));
static void Set_param_rv();
static void Set_param_do();

sf_Box_page fc_Box_page =
{
 cons_Box_page,
 dest_Box_page,
 copy_Box_page
};

void *cons_Box_page(w_Parent, pszTitle, param, nb_page, selpg)
Widget		w_Parent;
char		*pszTitle;
new_doc_s	*param;
int		nb_page;
int		selpg;
{
 c_Box_page	*ObjTmp;
 char			str[10];
 listbutton		*buttonptr;
 Display		*display;
  
 if((ObjTmp = (c_Box_page *)Xc_malloc("Box_page",
				      sizeof(c_Box_page))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
  
 ObjTmp->f = &fc_Box_page;
 ObjTmp->all_page = param;
 ObjTmp->nb_page = nb_page;
 ObjTmp->selected_page = selpg;
 ObjTmp->param = (new_doc_s *)Xc_malloc("param", sizeof(new_doc_s));
 memcpy(ObjTmp->param, &(param[selpg]), sizeof(new_doc_s));
 param = ObjTmp->param;
  
 ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, 620, 360,
							XcCD_HORIZONTAL
							| XcCD_HRESIZE
							| XcCD_VRESIZE);
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);
 display = XtDisplay(WIDGET(ObjTmp->Dialog));

 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gBoxPage", FALSE);

 ObjTmp->Ltitre = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
			  MANAGER(ObjTmp->Dialog),
			  "lBoxPageTitle");

  
 ObjTmp->FramenumPage = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
						MANAGER(ObjTmp->Dialog), 
						NULL, 0);

 ObjTmp->GridnumPage = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->FramenumPage),
					     MANAGER(ObjTmp->Dialog),
					     "gPageNumber", TRUE);
 ObjTmp->LnumPage = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridnumPage),
			  MANAGER(ObjTmp->Dialog),
			  "lPageNumber");

 ObjTmp->Icon1 = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->GridnumPage),
				       MANAGER(ObjTmp->Dialog),
				       "iBP1", display,
				       previous_xpm, NULL, XcPush);
 XtAddCallback(WIDGET(ObjTmp->Icon1), XmNactivateCallback, 
	       page_moinsmoins, ObjTmp);

 sprintf(str, "%d", param->page_nbr);
 ObjTmp->NumPage = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridnumPage),
				    MANAGER(ObjTmp->Dialog),
				    "tfPageNumber",
				    TF_INTEGER,
				    1.0, (float)nb_page,
				    str, NULL, 4);
 F(ObjTmp->NumPage).add_callback(ObjTmp->NumPage,
				 (WTextFieldProc)certify_pagenum, ObjTmp);


 ObjTmp->Icon2 = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->GridnumPage),
				       MANAGER(ObjTmp->Dialog),
				       "iBP2", display,
				       next_xpm, NULL, XcPush);
 XtAddCallback(WIDGET(ObjTmp->Icon2), XmNactivateCallback, 
	       page_plusplus, ObjTmp);
  
 F(ObjTmp->GridnumPage).addColumn(ObjTmp->GridnumPage, XcG_BEST_SIZE,
				  WIDGET(ObjTmp->LnumPage), XcG_MAX_SIZE,
				  WIDGET(ObjTmp->Icon1), XcG_BEST_SIZE,
				  WIDGET(ObjTmp->NumPage), XcG_BEST_SIZE,
				  WIDGET(ObjTmp->Icon2), XcG_BEST_SIZE,
				  NULL);
  


 ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fDirection", 0);
 ObjTmp->RadioImage = 
  (c_RadioImage *)NEW(c_RadioImage)(WIDGET(ObjTmp->Frame1),
				    MANAGER(ObjTmp->Dialog),
				    &(param->p_o),
				    "riPortrait",
				    document_portrait_xpm,
				    "riLandscape",
				    document_paysage_xpm,
				    NULL);

 ObjTmp->Frame4 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  "fFormat", 90);

 ObjTmp->GridFormat = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame4),
					    MANAGER(ObjTmp->Dialog),
					    "gFormat", TRUE);

 ObjTmp->Option = NEW(c_ItemMenu)(WIDGET(ObjTmp->GridFormat),
				  MANAGER(ObjTmp->Dialog), 
				  "imPageFormat", 0, 0);
 Xc_Table_Page_Format[Xc_CUSTOM_FORMAT].width = param->p_f->width;
 Xc_Table_Page_Format[Xc_CUSTOM_FORMAT].height = param->p_f->height;
 F(ObjTmp->Option).setCallback(ObjTmp->Option, (void *)cb_format,
			       ObjTmp);
 F(ObjTmp->Option).set(ObjTmp->Option, TRUE,
		       offsetof(page_format_t, name), NULL, NULL);
 F(ObjTmp->Option).addItems(ObjTmp->Option, XcIM_ARRAY, Xc_Count_Page_Format,
			    Xc_Table_Page_Format, sizeof(page_format_t));

 ObjTmp->Lsize = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridFormat),
			  MANAGER(ObjTmp->Dialog),
			  "lSize");
 sprintf(str, "%g", SCALE_TO_CENTIMETERS(param->p_f->width));
 ObjTmp->TFdocWidth = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridFormat),
				    MANAGER(ObjTmp->Dialog),
				    "tfDocWidth",
				    TF_REAL,
				    1.0, 300.0,
				    str, NULL, 7);
 F(ObjTmp->TFdocWidth).set_unit(ObjTmp->TFdocWidth,
				XcUB_LENGTH_CENTIMETER, NULL, TRUE, FALSE);
 F(ObjTmp->TFdocWidth).add_callback(ObjTmp->TFdocWidth,
				    (WTextFieldProc)certify_size, ObjTmp);
  
 ObjTmp->Lcroix = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridFormat),
			  MANAGER(ObjTmp->Dialog),
			  " x ");
 sprintf(str, "%g", SCALE_TO_CENTIMETERS(param->p_f->height));
 ObjTmp->TFdocHeight = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridFormat),
				    MANAGER(ObjTmp->Dialog),
				    "tfDocHeight",
				    TF_REAL,
				    1.0, 300.0,
				    str, NULL, 7);
 F(ObjTmp->TFdocHeight).set_unit(ObjTmp->TFdocHeight,
				 XcUB_LENGTH_CENTIMETER, NULL, TRUE, FALSE);
 F(ObjTmp->TFdocHeight).add_callback(ObjTmp->TFdocHeight,
				     (WTextFieldProc)certify_size, ObjTmp);

 F(ObjTmp->GridFormat).addColumn(ObjTmp->GridFormat, XcG_BEST_SIZE,
				 WIDGET(ObjTmp->Option), XcG_MAX_SIZE,
				 NULL);
 F(ObjTmp->GridFormat).addColumn(ObjTmp->GridFormat, XcG_MAX_SIZE, NULL);
 F(ObjTmp->GridFormat).addColumn(ObjTmp->GridFormat, XcG_BEST_SIZE,
				 WIDGET(ObjTmp->Lsize), XcG_MAX_SIZE,
				 WIDGET(ObjTmp->TFdocWidth), XcG_BEST_SIZE,
				 WIDGET(ObjTmp->Lcroix), XcG_BEST_SIZE,
				 WIDGET(ObjTmp->TFdocHeight), XcG_BEST_SIZE,
				 NULL);
 F(ObjTmp->GridFormat).addColumn(ObjTmp->GridFormat, XcG_MAX_SIZE, NULL);

 ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fPageStyle", 50);

 ObjTmp->Pages = (c_RadioBox *)NEW(c_RadioBox)(WIDGET(ObjTmp->Frame2),
					       MANAGER(ObjTmp->Dialog),
					       RADIO_NORMAL,
					       "rbPages",
					       "rbRecto Verso", 
					       "rbDouble Page", NULL);
  
 ObjTmp->Frame5 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fMargin", 50);

 ObjTmp->GridMarges = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame5),
					    MANAGER(ObjTmp->Dialog),
					    "gMargin", FALSE);

 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(param->up_m));
 ObjTmp->TFMargeH = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridMarges),
				    MANAGER(ObjTmp->Dialog),
				    "tfMarginT",
				    TF_REAL,
				    0.0, 150.0,
				    str, NULL, 6);
 F(ObjTmp->TFMargeH).set_unit(ObjTmp->TFMargeH,
			      XcUB_LENGTH_CENTIMETER, "%.2f", TRUE, FALSE);
 F(ObjTmp->TFMargeH).add_callback(ObjTmp->TFMargeH, 
				  (WTextFieldProc)certify_marge, ObjTmp);
 ObjTmp->LMargeH = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridMarges),
			  MANAGER(ObjTmp->Dialog),
			  "lMarginT");

 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(param->lo_m));
 ObjTmp->TFMargeB = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridMarges),
				    MANAGER(ObjTmp->Dialog),
				    "tfMarginB",
				    TF_REAL,
				    0.0, 150.0,
				    str, NULL, 6);
 F(ObjTmp->TFMargeB).set_unit(ObjTmp->TFMargeB,
			      XcUB_LENGTH_CENTIMETER, "%.2f", TRUE, FALSE);
 F(ObjTmp->TFMargeB).add_callback(ObjTmp->TFMargeB, 
				  (WTextFieldProc)certify_marge, ObjTmp);
 ObjTmp->LMargeB = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridMarges),
			  MANAGER(ObjTmp->Dialog),
			  "lMarginB");
 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(param->le_m));
 ObjTmp->TFMargeI = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridMarges),
				    MANAGER(ObjTmp->Dialog),
				    "tfMarginL",
				    TF_REAL,
				    0.0, 150.0,
				    str, NULL, 6);
 F(ObjTmp->TFMargeI).set_unit(ObjTmp->TFMargeI,
			      XcUB_LENGTH_CENTIMETER, "%.2f", TRUE, FALSE);
 F(ObjTmp->TFMargeI).add_callback(ObjTmp->TFMargeI, 
				  (WTextFieldProc)certify_marge, ObjTmp);
 ObjTmp->LMargeI = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridMarges),
			  MANAGER(ObjTmp->Dialog),
			  "lMarginL");
 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(param->ri_m));
 ObjTmp->TFMargeE = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridMarges),
				    MANAGER(ObjTmp->Dialog),
				    "tfMarginR",
				    TF_REAL,
				    0.0, 150.0,
				    str, NULL, 6);
 F(ObjTmp->TFMargeE).set_unit(ObjTmp->TFMargeE,
			      XcUB_LENGTH_CENTIMETER, "%.2f", TRUE, FALSE);
 F(ObjTmp->TFMargeE).add_callback(ObjTmp->TFMargeE, 
				  (WTextFieldProc)certify_marge, ObjTmp);
 ObjTmp->LMargeE = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridMarges),
			  MANAGER(ObjTmp->Dialog),
			  "lMarginR");


 F(ObjTmp->GridMarges).addColumn(ObjTmp->GridMarges, XcG_MAX_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 WIDGET(ObjTmp->LMargeH), XcG_BEST_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 WIDGET(ObjTmp->LMargeB), XcG_BEST_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 NULL);
 F(ObjTmp->GridMarges).addColumn(ObjTmp->GridMarges, XcG_BEST_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 WIDGET(ObjTmp->TFMargeH), XcG_BEST_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 WIDGET(ObjTmp->TFMargeB), XcG_BEST_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 NULL);
 F(ObjTmp->GridMarges).addColumn(ObjTmp->GridMarges, XcG_MAX_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 WIDGET(ObjTmp->LMargeI), XcG_BEST_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 WIDGET(ObjTmp->LMargeE), XcG_BEST_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 NULL);
 F(ObjTmp->GridMarges).addColumn(ObjTmp->GridMarges, XcG_BEST_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 WIDGET(ObjTmp->TFMargeI), XcG_BEST_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 WIDGET(ObjTmp->TFMargeE), XcG_BEST_SIZE,
				 XcG_WIDGET_NULL, XcG_MAX_SIZE,
				 NULL);

 ObjTmp->Frame3 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fSections", 50);

 ObjTmp->GridDecoupage = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame3),
					       MANAGER(ObjTmp->Dialog),
					       "gSections", FALSE);

 sprintf(str, "%d", param->n_c);
 ObjTmp->TFDecoupageV = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridDecoupage),
				    MANAGER(ObjTmp->Dialog),
				    "tfColumns",
				    TF_INTEGER,
				    1.0, 100.0,
				    str, NULL, 3);
 F(ObjTmp->TFDecoupageV).add_callback(ObjTmp->TFDecoupageV,
				      (WTextFieldProc)certify_decoupage,
				      ObjTmp);

 ObjTmp->LDecoupageV = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridDecoupage),
			  MANAGER(ObjTmp->Dialog),
			  "lColumns");

 sprintf(str, "%d", param->n_r);
 ObjTmp->TFDecoupageH = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridDecoupage),
				    MANAGER(ObjTmp->Dialog),
				    "tfRows",
				    TF_INTEGER,
				    1.0, 100.0,
				    str, NULL, 3);
 F(ObjTmp->TFDecoupageH).add_callback(ObjTmp->TFDecoupageH,
				      (WTextFieldProc)certify_decoupage,
				      ObjTmp);
 ObjTmp->LDecoupageH = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridDecoupage),
			  MANAGER(ObjTmp->Dialog),
			  "lRows");
 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(param->c_s));
 ObjTmp->TFGouttiereV = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridDecoupage),
				    MANAGER(ObjTmp->Dialog),
				    "tfVSpacing",
				    TF_REAL,
				    0.0, 20.0,
				    str, NULL, 6);
 F(ObjTmp->TFGouttiereV).set_unit(ObjTmp->TFGouttiereV,
				  XcUB_LENGTH_CENTIMETER,
				  "%.2f", TRUE, FALSE);
 F(ObjTmp->TFGouttiereV).add_callback(ObjTmp->TFGouttiereV,
				      (WTextFieldProc)certify_gouttiere,
				      ObjTmp);
 ObjTmp->LGouttiereV = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridDecoupage),
			  MANAGER(ObjTmp->Dialog),
			  "lVSpacing");
 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(param->r_s));
 ObjTmp->TFGouttiereH = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridDecoupage),
				    MANAGER(ObjTmp->Dialog),
				    "tfHSpacing",
				    TF_REAL,
				    0.0, 20.0,
				    str, NULL, 6);
 F(ObjTmp->TFGouttiereH).set_unit(ObjTmp->TFGouttiereH,
				  XcUB_LENGTH_CENTIMETER,
				  "%.2f", TRUE, FALSE);
 F(ObjTmp->TFGouttiereH).add_callback(ObjTmp->TFGouttiereH,
				      (WTextFieldProc)certify_gouttiere,
				      ObjTmp);
 ObjTmp->LGouttiereH = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridDecoupage),
			  MANAGER(ObjTmp->Dialog),
			  "lHSpacing");

 F(ObjTmp->GridDecoupage).addColumn(ObjTmp->GridDecoupage, XcG_MAX_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    WIDGET(ObjTmp->LDecoupageV), XcG_BEST_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    WIDGET(ObjTmp->LDecoupageH), XcG_BEST_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    NULL);
 F(ObjTmp->GridDecoupage).addColumn(ObjTmp->GridDecoupage, XcG_BEST_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    WIDGET(ObjTmp->TFDecoupageV), 
				    XcG_BEST_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    WIDGET(ObjTmp->TFDecoupageH), 
				    XcG_BEST_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    NULL);
 F(ObjTmp->GridDecoupage).addColumn(ObjTmp->GridDecoupage, XcG_MAX_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    WIDGET(ObjTmp->LGouttiereV), XcG_BEST_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    WIDGET(ObjTmp->LGouttiereH), XcG_BEST_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    NULL);
 F(ObjTmp->GridDecoupage).addColumn(ObjTmp->GridDecoupage, XcG_BEST_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    WIDGET(ObjTmp->TFGouttiereV), 
				    XcG_BEST_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    WIDGET(ObjTmp->TFGouttiereH), 
				    XcG_BEST_SIZE,
				    XcG_WIDGET_NULL, XcG_MAX_SIZE,
				    NULL);

 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Ltitre), 50,
			   WIDGET(ObjTmp->FramenumPage), 45,
			   WIDGET(ObjTmp->Frame1), 75,
			   WIDGET(ObjTmp->Frame4), XcG_MAX_SIZE,
			   NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 5, NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Frame2), 75,
			   WIDGET(ObjTmp->Frame5), XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Frame3), XcG_MAX_SIZE,
			   NULL);

 F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				    XcCD_OK | XcCD_CANCEL,
				    XcCD_OK,
				    TRUE, NULL, NULL);
  
 F(ObjTmp->Option).setSelection(ObjTmp->Option, param->p_f);

 F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);
 /* must be set function of the param passed */
 /* buttonptr = ObjTmp->RadioImage->RadioBox->Button;
    i = 0;
    while(param->p_o > i)
    {
    buttonptr = buttonptr->Next;
    i++;
    }
    XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
    */
 buttonptr = ObjTmp->Pages->Button;
 if (param->p_rv)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
 buttonptr = buttonptr->Next;
 if (param->p_do)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
 F(ObjTmp->Pages).AddCallback(ObjTmp->Pages, 0,
			      XmNvalueChangedCallback,
			      Set_param_rv, ObjTmp);
 F(ObjTmp->Pages).AddCallback(ObjTmp->Pages, 1, 
			      XmNvalueChangedCallback,
			      Set_param_do, ObjTmp);
 Xc_TRACE(("Object Box_page build"));
 return ObjTmp;
}

void dest_Box_page(This)
c_Box_page *This;
{
 Change_page(This, This->selected_page);/* for OK button */
  
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_Label)(This->Lsize);
 DELETE(c_Label)(This->Lcroix);
 DELETE(c_Label)(This->LDecoupageV);
 DELETE(c_Label)(This->LGouttiereV);
 DELETE(c_Label)(This->LDecoupageH);
 DELETE(c_Label)(This->LGouttiereH);
 DELETE(c_Label)(This->LMargeH);
 DELETE(c_Label)(This->LMargeB);
 DELETE(c_Label)(This->LMargeI);
 DELETE(c_Label)(This->LMargeE);
 DELETE(c_Label)(This->LnumPage);
 DELETE(c_Icon)(This->Icon1);
 DELETE(c_Icon)(This->Icon2);
 DELETE(c_WTextField)(This->TFdocWidth);
 DELETE(c_WTextField)(This->TFdocHeight);
 DELETE(c_WTextField)(This->TFDecoupageV);
 DELETE(c_WTextField)(This->TFGouttiereV);
 DELETE(c_WTextField)(This->TFDecoupageH);
 DELETE(c_WTextField)(This->TFGouttiereH);
 DELETE(c_WTextField)(This->NumPage);
 DELETE(c_WTextField)(This->TFMargeH);
 DELETE(c_WTextField)(This->TFMargeB);
 DELETE(c_WTextField)(This->TFMargeI);
 DELETE(c_WTextField)(This->TFMargeE);
 DELETE(c_RadioBox)(This->Pages);
 DELETE(c_RadioImage)(This->RadioImage);
 DELETE(c_ItemMenu)(This->Option);
 DELETE(c_Grid)(This->GridMarges);
 DELETE(c_Grid)(This->GridDecoupage);
 DELETE(c_Grid)(This->GridFormat);
 DELETE(c_Grid)(This->GridnumPage);
 DELETE(c_Frame)(This->Frame1);
 DELETE(c_Frame)(This->Frame2);
 DELETE(c_Frame)(This->Frame3);
 DELETE(c_Frame)(This->Frame4);
 DELETE(c_Frame)(This->Frame5);
 DELETE(c_Frame)(This->FramenumPage);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This->param);
 Xc_free(This);
 Xc_TRACE(("Object Box_page destroyed"));
}

void *copy_Box_page(This)
c_Box_page *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}


static void page_plusplus(w, Obj)
Widget		w;
c_Box_page	*Obj;
{
 if (Obj->selected_page < Obj->nb_page-1)
  Change_page(Obj, Obj->selected_page+1);
}

static void page_moinsmoins(w, Obj)
Widget		w;
c_Box_page	*Obj;
{
 if (Obj->selected_page > 0)
  Change_page(Obj, Obj->selected_page-1);
}

static void Change_page(Obj, new_page)
c_Box_page *Obj;
int new_page;
{
 char	str[10];
 new_doc_s	*oparam;
 new_doc_s	*param;
 listbutton	*buttonptr;
  
 param = Obj->param;
 param->doc_name = (char *)((long)param->doc_name & 254);
 if (param->p_o != Obj->all_page[Obj->selected_page].p_o)
  param->doc_name = (char *)((long)param->doc_name | 2);
  
 memcpy(&(Obj->all_page[Obj->selected_page]), 
	param, sizeof(new_doc_s));
 oparam = &(Obj->all_page[Obj->selected_page]);
 Obj->selected_page = new_page;
 memcpy(param, &(Obj->all_page[Obj->selected_page]),
	sizeof(new_doc_s));
 param->doc_name = (char *)((long)param->doc_name | 1);
  
 sprintf(str, "%d", param->page_nbr);
 F(Obj->NumPage).set(Obj->NumPage, str);
 Xc_TRACE(("change to page: %s", str));
  
 if (param->p_o != oparam->p_o)
  F(Obj->RadioImage->Toggles[param->p_o])
   .Set(Obj->RadioImage->Toggles[param->p_o], True, True);

 if (param->p_rv != oparam->p_rv)  
 {
  buttonptr = Obj->Pages->Button;
  if (param->p_rv)
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
  else
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
 }
 if (param->p_do != oparam->p_do)
 {
  buttonptr = Obj->Pages->Button->Next;
  if (param->p_do)
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
  else
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
 }
 if (param->p_f->width != oparam->p_f->width)
 {
  F(Obj->TFdocWidth).set_value_in_unit
   (Obj->TFdocWidth, (double)SCALE_TO_CENTIMETERS(param->p_f->width));
 }
 if (param->p_f->height != oparam->p_f->height)
 {
  F(Obj->TFdocHeight).set_value_in_unit
   (Obj->TFdocHeight, (double)SCALE_TO_CENTIMETERS(param->p_f->height));
 }
 if (param->p_f != oparam->p_f)
  F(Obj->Option).setSelection(Obj->Option, param->p_f);
  
 if (param->up_m != oparam->up_m)
 {
  F(Obj->TFMargeH).set_value_in_unit
   (Obj->TFMargeH, (double)SCALE_TO_CENTIMETERS(param->up_m));
 }
 if (param->lo_m != oparam->lo_m)
 {
  F(Obj->TFMargeB).set_value_in_unit
   (Obj->TFMargeB, (double)SCALE_TO_CENTIMETERS(param->lo_m));
 }
 if (param->le_m != oparam->le_m)
 {
  F(Obj->TFMargeI).set_value_in_unit
   (Obj->TFMargeI, (double)SCALE_TO_CENTIMETERS(param->le_m));
 }
 if (param->ri_m != oparam->ri_m)
 {
  F(Obj->TFMargeE).set_value_in_unit
   (Obj->TFMargeE, (double)SCALE_TO_CENTIMETERS(param->ri_m));
 }
 if (param->n_c != oparam->n_c)
 {
  sprintf(str, "%d", param->n_c);
  F(Obj->TFDecoupageV).set(Obj->TFDecoupageV, str);
 }
 if (param->n_r != oparam->n_r)
 {
  sprintf(str, "%d", param->n_r);
  F(Obj->TFDecoupageH).set(Obj->TFDecoupageH, str);
 }
 if (param->c_s != oparam->c_s)
 {
  F(Obj->TFGouttiereV).set_value_in_unit
   (Obj->TFGouttiereV, (double)SCALE_TO_CENTIMETERS(param->c_s));;
 }
 if (param->r_s != oparam->r_s)
 {
  F(Obj->TFGouttiereH).set_value_in_unit
   (Obj->TFGouttiereH, (double)SCALE_TO_CENTIMETERS(param->r_s));
 }
}

static void certify_pagenum(this, Obj)
c_WTextField	*this;
c_Box_page	*Obj;
{
 char	*vstr;
 unsigned int	num;
 int		i,num_page;

 if((vstr = F(this).get(this)) != NULL)
 {
  num = atoi(vstr);
  Xc_free(vstr);
  num_page = 0;
  i = Obj->all_page[num_page].page_nbr;
      
  while(i < (int)num)
  {
   num_page ++;
   i = Obj->all_page[num_page].page_nbr;
  }
  if (num_page!=Obj->selected_page)
   Change_page(Obj, num_page);
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
 }
}

static void certify_size(this, Obj)
c_WTextField	*this;
c_Box_page	*Obj;
{
 coord_t	num;

 num = F(this).get_value_in_unit(this);
 if (this == Obj->TFdocWidth)
  Xc_Table_Page_Format[Xc_CUSTOM_FORMAT].width = num;
 if (this == Obj->TFdocHeight)
  Xc_Table_Page_Format[Xc_CUSTOM_FORMAT].height = num;
 F(Obj->Option).setSelection(Obj->Option,
			     &Xc_Table_Page_Format[Xc_CUSTOM_FORMAT]);
 cb_format(Obj->Option, Obj, &Xc_Table_Page_Format[Xc_CUSTOM_FORMAT]);
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void certify_marge(this, Obj)
c_WTextField	*this;
c_Box_page	*Obj;
{
 coord_t	num;

 Xc_HISTORY(("margin"));

 num = F(this).get_value_in_unit(this);
 if (this == Obj->TFMargeH)
  Obj->param->up_m = num;
 if (this == Obj->TFMargeB)
  Obj->param->lo_m = num;
 if (this == Obj->TFMargeI)
  Obj->param->le_m = num;
 if (this == Obj->TFMargeE)
  Obj->param->ri_m = num;
 Obj->param->doc_name = (char *)((long)Obj->param->doc_name | 2);
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}


static void certify_decoupage(this, Obj)
c_WTextField	*this;
c_Box_page	*Obj;
{
 char		*vstr;
 coord_t	num;

 if((vstr = F(this).get(this)) != NULL)
 {
  num = atoi(vstr);
  Xc_free(vstr);
  if (this == Obj->TFDecoupageH)
   Obj->param->n_r = num;
  if (this == Obj->TFDecoupageV)
   Obj->param->n_c = num;
  Obj->param->doc_name = (char *)((long)Obj->param->doc_name | 2);
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
 }
}


static void certify_gouttiere(this, Obj)
c_WTextField	*this;
c_Box_page	*Obj;
{
 coord_t	num;
  
 num = (coord_t)F(this).get_value_in_unit(this);
 if (this == Obj->TFGouttiereH)
  Obj->param->r_s = num;
 if (this == Obj->TFGouttiereV)
  Obj->param->c_s = num;
 Obj->param->doc_name = (char *)((long)Obj->param->doc_name | 2);
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}


/* ----------------------------------------------------------------- ** 
** cb_format - ItemMenu callback for page format                     ** 
** ----------------------------------------------------------------- */
static void cb_format(popup, this, format)
c_ItemMenu *popup;
c_Box_page *this;
page_format_t *format;
{
 F(this->TFdocWidth).set_value_in_unit
  (this->TFdocWidth, (double)SCALE_TO_CENTIMETERS(format->width));
  
 F(this->TFdocHeight).set_value_in_unit
  (this->TFdocHeight, (double)SCALE_TO_CENTIMETERS(format->height));
  
 this->param->p_f = format;
 this->param->doc_name = (char *)((long)this->param->doc_name | 2);
 F(this->Dialog).resetTraversal(this->Dialog);
}

static void Set_param_rv(This, Obj, appel)
Widget	This;
c_Box_page *Obj;
caddr_t	appel;
{
 Obj->param->p_rv = !Obj->param->p_rv;
 Obj->param->doc_name = (char *)((long)Obj->param->doc_name | 2);
}

static void Set_param_do(This, Obj, appel)
Widget	This;
c_Box_page *Obj;
caddr_t	appel;
{
 Obj->param->p_do = !Obj->param->p_do;
 Obj->param->doc_name = (char *)((long)Obj->param->doc_name | 2);
}













