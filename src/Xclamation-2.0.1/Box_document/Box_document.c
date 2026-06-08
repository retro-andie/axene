/*
** Box_document.c for Xclamation in Box_document/
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
** Started on  Mon Jul 17 13:59:35 1995 Robin Castanier
** Last update Sun Feb  8 16:15:01 1998 Emmanuel Paris
*/

#include "Box_document.h"
#include "../Pixmaps/document.portrait.zpm"
#include "../Pixmaps/document.paysage.zpm"

void *cons_Box_document();
void dest_Box_document();
void *copy_Box_document();

void changeformat();
static void certify_docname ___PROTO((c_WTextField *this,
				      c_Box_document *Obj));
static void certify_pagenbr ___PROTO((c_WTextField *this,
				      c_Box_document *Obj));
static void certify_size ___PROTO((c_WTextField *this,
				      c_Box_document *Obj));
static void certify_marge ___PROTO((c_WTextField *this,
				    c_Box_document *Obj));
static void certify_decoupage ___PROTO((c_WTextField *this,
					c_Box_document *Obj));
static void certify_gouttiere ___PROTO((c_WTextField *this,
					c_Box_document *Obj));
static void cb_format ___PROTO((c_ItemMenu *popup, c_Box_document *this,
				page_format_t *format));
static void Set_param();


sf_Box_document fc_Box_document =
{
 cons_Box_document,
 dest_Box_document,
 copy_Box_document
};

void *cons_Box_document(w_Parent, pszTitle, ListDoc, param)
Widget		w_Parent;
char		*pszTitle;
c_DocumentList	*ListDoc;
new_doc_s	*param;
{
 c_Box_document	*ObjTmp;
 char			str[10];
 char			doc_name[20];
 listbutton		*buttonptr;

 if((ObjTmp = (c_Box_document *)Xc_malloc("Box_document",
					  sizeof(c_Box_document))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }

 ObjTmp->f = &fc_Box_document;
 ObjTmp->param = param;
 ObjTmp->Listdoc = ListDoc;
  
 ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, 620, 360,
							XcCD_HORIZONTAL
							| XcCD_HRESIZE
							| XcCD_VRESIZE);
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);

 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gBoxDocument", FALSE);

 ObjTmp->Ltitre = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
			  MANAGER(ObjTmp->Dialog),
			  "lBoxDocumentTitle");
 strcpy(doc_name, param->doc_name);
 ObjTmp->NomDuDoc = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid),
				    MANAGER(ObjTmp->Dialog),
				    "tfBDDocname",
				    TF_STRING,
				    1.0, (float)(XcMAX_NAME_LENGTH-1),
				    doc_name, 
				    NULL, 20);
 F(ObjTmp->NomDuDoc).add_callback(ObjTmp->NomDuDoc,
				  (WTextFieldProc)certify_docname, ObjTmp);


 ObjTmp->FramenbPages = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
						MANAGER(ObjTmp->Dialog), 
						NULL, 0);

 ObjTmp->GridNbPages = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->FramenbPages),
					     MANAGER(ObjTmp->Dialog),
					     "gNrPages", TRUE);
  
 ObjTmp->LNbDePages = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridNbPages),
			  MANAGER(ObjTmp->Dialog),
			  "lNumberOfPages");
  
 sprintf(str, "%d", param->page_nbr);
 ObjTmp->NbDePages = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridNbPages),
				    MANAGER(ObjTmp->Dialog),
				    "tfNumberOfPages",
				    TF_INTEGER,
				    1.0, 1000.0,
				    str, NULL, 5);

 F(ObjTmp->NbDePages).add_callback(ObjTmp->NbDePages,
				   (WTextFieldProc)certify_pagenbr, ObjTmp);

 F(ObjTmp->GridNbPages).addColumn(ObjTmp->GridNbPages, XcG_MAX_SIZE,
				  WIDGET(ObjTmp->LNbDePages), XcG_MAX_SIZE,
				  WIDGET(ObjTmp->NbDePages), XcG_BEST_SIZE,
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
					    "gBDFormat", TRUE);

 ObjTmp->Option = NEW(c_ItemMenu)(WIDGET(ObjTmp->GridFormat),
				  MANAGER(ObjTmp->Dialog), "imPageFormat",
				  0, 0);
 F(ObjTmp->Option).setCallback(ObjTmp->Option, (void (*)())cb_format, ObjTmp);
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
				    "tfMargeT",
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
				    "tfMargeB",
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
				    "tfMargeL",
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
				    "tfMargeR",
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
			   WIDGET(ObjTmp->Ltitre), 30,
			   WIDGET(ObjTmp->NomDuDoc), XcG_BEST_SIZE,
			   XcG_WIDGET_NULL, 7,
			   WIDGET(ObjTmp->FramenbPages), 45,
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
				    XcCD_OK | XcCD_CANCEL | XcCD_OPEN,
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
			      Set_param,
			      &(param->p_rv));
 F(ObjTmp->Pages).AddCallback(ObjTmp->Pages, 1, 
			      XmNvalueChangedCallback,
			      Set_param,
			      &(param->p_do));
 Xc_TRACE(("Object Box_document build"));
 return ObjTmp;
}

void dest_Box_document(This)
c_Box_document *This;
{
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_Label)(This->Lsize);
 DELETE(c_Label)(This->LNbDePages);
 DELETE(c_Label)(This->Lcroix);
 DELETE(c_Label)(This->LDecoupageV);
 DELETE(c_Label)(This->LGouttiereV);
 DELETE(c_Label)(This->LDecoupageH);
 DELETE(c_Label)(This->LGouttiereH);
 DELETE(c_Label)(This->LMargeH);
 DELETE(c_Label)(This->LMargeB);
 DELETE(c_Label)(This->LMargeI);
 DELETE(c_Label)(This->LMargeE);
 DELETE(c_WTextField)(This->TFdocWidth);
 DELETE(c_WTextField)(This->TFdocHeight);
 DELETE(c_WTextField)(This->TFDecoupageV);
 DELETE(c_WTextField)(This->TFGouttiereV);
 DELETE(c_WTextField)(This->TFDecoupageH);
 DELETE(c_WTextField)(This->TFGouttiereH);
 DELETE(c_WTextField)(This->NomDuDoc);
 DELETE(c_WTextField)(This->NbDePages);
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
 DELETE(c_Grid)(This->GridNbPages);
 DELETE(c_Frame)(This->Frame1);
 DELETE(c_Frame)(This->Frame2);
 DELETE(c_Frame)(This->Frame3);
 DELETE(c_Frame)(This->Frame4);
 DELETE(c_Frame)(This->Frame5);
 DELETE(c_Frame)(This->FramenbPages);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);
  
 Xc_free(This);
 Xc_TRACE(("Object Box_document destroyed"));
}

void *copy_Box_document(This)
c_Box_document *This;
{
 Xc_TRACE(("Copy not impemented"));
 return NULL;
}


static void certify_docname(this, Obj)
c_WTextField	*this;
c_Box_document	*Obj;
{
 char		*vstr;

 Xc_TRACE(("docname"));

 if((vstr = F(this).get(this)) != NULL)
 {
  Xc_TRACE(("%s", vstr));
  if (Obj->param->doc_name)
  {
   Xc_free(Obj->param->doc_name);
  }      
  Obj->param->doc_name = vstr;
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
 }
}

static void certify_pagenbr(this, Obj)
c_WTextField	*this;
c_Box_document	*Obj;
{
 char		*vstr;
 unsigned int	num;

 if((vstr = F(this).get(this)) != NULL)
 {
  num = atoi(vstr);
  Xc_free(vstr);
  Obj->param->page_nbr = num;
  Xc_TRACE(("page number %d", num));
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
 }
}

static void certify_size(this, Obj)
c_WTextField	*this;
c_Box_document	*Obj;
{
 coord_t	num;

 Xc_TRACE(("size"));
  
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
c_Box_document	*Obj;
{
 coord_t	num;

 Xc_HISTORY(("margin"));

 num = (coord_t)F(this).get_value_in_unit(this);
 if (this == Obj->TFMargeH)
  Obj->param->up_m = num;
 if (this == Obj->TFMargeB)
  Obj->param->lo_m = num;
 if (this == Obj->TFMargeI)
  Obj->param->le_m = num;
 if (this == Obj->TFMargeE)
  Obj->param->ri_m = num;
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}


static void certify_decoupage(this, Obj)
c_WTextField	*this;
c_Box_document	*Obj;
{
 char		*vstr;
 coord_t	num;

 Xc_TRACE(("row/columns"));

 if((vstr = F(this).get(this)) != NULL)
 {
  num = atoi(vstr);
  Xc_free(vstr);
  if (this == Obj->TFDecoupageH)
   Obj->param->n_r = num;
  if (this == Obj->TFDecoupageV)
   Obj->param->n_c = num;
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
 }
}


static void certify_gouttiere(this, Obj)
c_WTextField	*this;
c_Box_document	*Obj;
{
 coord_t	num;

 Xc_TRACE(("spacing"));

 num = (coord_t)F(this).get_value_in_unit(this);
  
 if (this == Obj->TFGouttiereH)
  Obj->param->r_s = num;
 if (this == Obj->TFGouttiereV)
  Obj->param->c_s = num;
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}


/* ----------------------------------------------------------------- ** 
** cb_format - ItemMenu callback for page format                     ** 
** ----------------------------------------------------------------- */
static void cb_format(popup, this, format)
c_ItemMenu *popup;
c_Box_document *this;
page_format_t *format;
{
 Xc_TRACE(("cb_format"));
  
 F(this->TFdocWidth).set_value_in_unit
  (this->TFdocWidth, (double)SCALE_TO_CENTIMETERS(format->width));
  
 F(this->TFdocHeight).set_value_in_unit
  (this->TFdocHeight, (double)SCALE_TO_CENTIMETERS(format->height));
  
 this->param->p_f = format;
 F(this->Dialog).resetTraversal(this->Dialog);
}

static void Set_param(This, value, appel)
Widget	This;
boolean	*value;
caddr_t	appel;
{
 *value = !*value;
}




