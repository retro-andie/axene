/*
** Box_miseenpage.c for XQuad in Box_miseenpage/
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
** Started on  Mon Sep 18 16:28:10 1995 Robin Castanier
** Last update Sat Feb  7 16:47:09 1998 Emmanuel Paris
*/

#include "Box_miseenpage.h"
#include "../Pixmaps/document.portrait.zpm"
#include "../Pixmaps/document.paysage.zpm"
#include "../Pixmaps/impression.n.zpm"
#include "../Pixmaps/impression.z.zpm"

void *cons_Box_miseenpage();
void dest_Box_miseenpage();
void *copy_Box_miseenpage();
static void arm_reduct ___PROTO((Widget w,
				 c_Box_miseenpage *Obj,
				 XmToggleButtonCallbackStruct *cbs));
static void arm_tenirsur ___PROTO((Widget w,
				   c_Box_miseenpage *Obj,
				   XmToggleButtonCallbackStruct *cbs));
static void certify_size ___PROTO((c_WTextField *this,
				      c_Box_miseenpage *Obj));
static void certify_reduct ___PROTO((c_WTextField *this,
				      c_Box_miseenpage *Obj));
static void certify_tenir ___PROTO((c_WTextField *this,
				      c_Box_miseenpage *Obj));
static void cb_format ___PROTO((c_ItemMenu *popup, c_Box_miseenpage *this,
				page_format_t *format));
static void certify_marge ___PROTO((c_WTextField *this,
				    c_Box_miseenpage *Obj));
static void Set_param();
static void cb_action ___PROTO((c_CustomDialog	*cb_dialog,
				c_Box_miseenpage *This,
				int		cb_reason));

sf_Box_miseenpage fc_Box_miseenpage =
{
 cons_Box_miseenpage,
 dest_Box_miseenpage,
 copy_Box_miseenpage
};


void *cons_Box_miseenpage(w_Parent, pszTitle, setup_param)
Widget		w_Parent;
char		*pszTitle;
setup_print_t	*setup_param;
{
 c_Box_miseenpage	*ObjTmp;
 char			str[10];
 listbutton		*buttonptr;
  
 if((ObjTmp = (c_Box_miseenpage *)Xc_malloc("Box_printsetup",
					    sizeof(c_Box_miseenpage))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }

 ObjTmp->f = &fc_Box_miseenpage;
 ObjTmp->setup_save = setup_param;
 ObjTmp->setup_param = (setup_print_t *)Xc_malloc("param save",
						  sizeof(setup_print_t));
 memcpy(ObjTmp->setup_param, ObjTmp->setup_save, sizeof(setup_print_t));
  
 ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, 740, 360,
							XcCD_HORIZONTAL
							| XcCD_VRESIZE
							| XcCD_HRESIZE);
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);

 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gBoxPrintSetup", FALSE);

 ObjTmp->Ltitre = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  "lBoxPrintSetupTitle");
 ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fPSPrintDirection", 0);
 ObjTmp->fbool_sens = ObjTmp->setup_save->direction_f;
 ObjTmp->RIsens = 
  (c_RadioImage *)NEW(c_RadioImage)(WIDGET(ObjTmp->Frame1),
				    MANAGER(ObjTmp->Dialog),
				    &(ObjTmp->fbool_sens),
				    "riZ",
				    impression_z_xpm,
				    "riN",
				    impression_n_xpm, 
				    NULL);

  
 ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fPSWhatPrint", 0);
  
 ObjTmp->RBwhatPrint = (c_RadioBox *)NEW(c_RadioBox)(WIDGET(ObjTmp->Frame2),
						     MANAGER(ObjTmp->Dialog),
						     RADIO_NORMAL,
						     "riWhatPrint",
						     "riGrid",
						     "riRowColumn",
						     NULL);

 buttonptr = ObjTmp->RBwhatPrint->Button;
 if (setup_param->grid_f)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
 buttonptr = buttonptr->Next;
 if (setup_param->rowcol_f)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
 F(ObjTmp->RBwhatPrint).AddCallback(ObjTmp->RBwhatPrint, 0,
				    XmNvalueChangedCallback,
				    Set_param,
				    &(ObjTmp->setup_param->grid_f));
 F(ObjTmp->RBwhatPrint).AddCallback(ObjTmp->RBwhatPrint, 1, 
				    XmNvalueChangedCallback,
				    Set_param,
				    &(ObjTmp->setup_param->rowcol_f));
  
 ObjTmp->Frame3 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fPSOrientation", 0);

 ObjTmp->fbool_PP = ObjTmp->setup_save->orientation;
 ObjTmp->RIPortraitPaysage = 
  (c_RadioImage *)NEW(c_RadioImage)(WIDGET(ObjTmp->Frame3),
				    MANAGER(ObjTmp->Dialog),
				    &(ObjTmp->fbool_PP),
				    "riPortrait",
				    document_portrait_xpm,
				    "riLandscape",
				    document_paysage_xpm,
				    NULL);

 ObjTmp->Frame4 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fPSMargin", 0);
 ObjTmp->Grid4 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame4),
				       MANAGER(ObjTmp->Dialog),
				       "gPSMargin", TRUE);

 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(setup_param->top_margin));
 ObjTmp->TFMargeH = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				    MANAGER(ObjTmp->Dialog),
				    "tfPSMarginT",
				    TF_REAL,
				    0.0, 150.0,
				    str, NULL, 6);
 F(ObjTmp->TFMargeH).set_unit(ObjTmp->TFMargeH, XcUB_LENGTH_CENTIMETER,
			      "%.2f", TRUE, FALSE);
 F(ObjTmp->TFMargeH).add_callback(ObjTmp->TFMargeH,
				  (WTextFieldProc)certify_marge, ObjTmp);
 ObjTmp->LMargeH = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid4),
			  MANAGER(ObjTmp->Dialog),
			  "lPSMarginT");

 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(setup_param->bottom_margin));
 ObjTmp->TFMargeB = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				    MANAGER(ObjTmp->Dialog),
				    "tfPSMarginB",
				    TF_REAL,
				    0.0, 150.0,
				    str, NULL, 6);
 F(ObjTmp->TFMargeB).set_unit(ObjTmp->TFMargeB, XcUB_LENGTH_CENTIMETER,
			      "%.2f", TRUE, FALSE);
 F(ObjTmp->TFMargeB).add_callback(ObjTmp->TFMargeB, 
				  (WTextFieldProc)certify_marge, ObjTmp);
 ObjTmp->LMargeB = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid4),
			  MANAGER(ObjTmp->Dialog),
			  "lPSMarginB");

 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(setup_param->left_margin)); 
 ObjTmp->TFMargeG = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				    MANAGER(ObjTmp->Dialog),
				    "tfPSMarginL",
				    TF_REAL,
				    0.0, 150.0,
				    str, NULL, 6);
 F(ObjTmp->TFMargeG).set_unit(ObjTmp->TFMargeG, XcUB_LENGTH_CENTIMETER,
			      "%.2f", TRUE, FALSE);
 F(ObjTmp->TFMargeG).add_callback(ObjTmp->TFMargeG,
				  (WTextFieldProc)certify_marge, ObjTmp);
 ObjTmp->LMargeG = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid4),
			  MANAGER(ObjTmp->Dialog),
			  "lPSMarginL");

 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(setup_param->right_margin));
 ObjTmp->TFMargeD = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				    MANAGER(ObjTmp->Dialog),
				    "tfPSMarginR",
				    TF_REAL,
				    0.0, 150.0,
				    str, NULL, 6);
 F(ObjTmp->TFMargeD).set_unit(ObjTmp->TFMargeD, XcUB_LENGTH_CENTIMETER,
			      "%.2f", TRUE, FALSE);
 F(ObjTmp->TFMargeD).add_callback(ObjTmp->TFMargeD, 
				  (WTextFieldProc)certify_marge, ObjTmp);
 ObjTmp->LMargeD = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid4),
			  MANAGER(ObjTmp->Dialog),
			  "lPSMarginR");

 ObjTmp->RBcentrage = (c_RadioBox *)NEW(c_RadioBox)(WIDGET(ObjTmp->Grid4),
						    MANAGER(ObjTmp->Dialog),
						    RADIO_NORMAL,
						    "rbPSCenter",
						    "rbPSCenterV",
						    "rbPSCenterH",
						    NULL);
 buttonptr = ObjTmp->RBcentrage->Button;
 if (setup_param->vcenter_f)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
 buttonptr = buttonptr->Next;
 if (setup_param->hcenter_f)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
 F(ObjTmp->RBcentrage).AddCallback(ObjTmp->RBcentrage, 0,
				   XmNvalueChangedCallback,
				   Set_param,
				   &(ObjTmp->setup_param->vcenter_f));
 F(ObjTmp->RBcentrage).AddCallback(ObjTmp->RBcentrage, 1, 
				   XmNvalueChangedCallback,
				   Set_param,
				   &(ObjTmp->setup_param->hcenter_f));

 F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->LMargeH), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->TFMargeH), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->LMargeG), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->TFMargeG), XcG_BEST_SIZE,
			    NULL);
 F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->LMargeB), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->TFMargeB), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->LMargeD), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->TFMargeD), XcG_BEST_SIZE,
			    NULL);
 F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->RBcentrage), XcG_BEST_SIZE,
			    NULL);
  
 ObjTmp->Frame5 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fPSFormat", 0);
 ObjTmp->Grid5 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame5),
				       MANAGER(ObjTmp->Dialog),
				       "gPSFormat", TRUE);
  
 ObjTmp->OFormat = NEW(c_ItemMenu)(WIDGET(ObjTmp->Grid5),
				   MANAGER(ObjTmp->Dialog), "imPageFormat", 0, 0);
 F(ObjTmp->OFormat).setCallback(ObjTmp->OFormat, (void *)cb_format, ObjTmp);
 F(ObjTmp->OFormat).set(ObjTmp->OFormat, TRUE,
			offsetof(page_format_t, name), NULL, NULL);
 F(ObjTmp->OFormat).addItems(ObjTmp->OFormat, XcIM_ARRAY, 
			     Xc_Count_Page_Format,
			     Xc_Table_Page_Format, sizeof(page_format_t));

 ObjTmp->Lsize = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid5),
			  MANAGER(ObjTmp->Dialog),
			  "lPSSize");
  
 sprintf(str, "%g", SCALE_TO_CENTIMETERS(setup_param->page_format->width));
 ObjTmp->TFdocWidth = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid5),
				    MANAGER(ObjTmp->Dialog),
				    "tfPSDocWidth",
				    TF_REAL,
				    1.0, 300.0,
				    str, NULL, 7);
  
 F(ObjTmp->TFdocWidth).set_unit(ObjTmp->TFdocWidth,
				XcUB_LENGTH_CENTIMETER, NULL, TRUE, FALSE);
 F(ObjTmp->TFdocWidth).add_callback(ObjTmp->TFdocWidth,
				    (WTextFieldProc)certify_size, ObjTmp);

 ObjTmp->Lcroix = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid5),
			  MANAGER(ObjTmp->Dialog),
			  " x ");
 sprintf(str, "%g", SCALE_TO_CENTIMETERS(setup_param->page_format->height));
 ObjTmp->TFdocHeight = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid5),
				    MANAGER(ObjTmp->Dialog),
				    "tfPSDocHeight",
				    TF_REAL,
				    1.0, 300.0,
				    str, NULL, 7);
 F(ObjTmp->TFdocHeight).set_unit(ObjTmp->TFdocHeight,
				 XcUB_LENGTH_CENTIMETER, NULL, TRUE, FALSE);
 F(ObjTmp->TFdocHeight).add_callback(ObjTmp->TFdocHeight,
				     (WTextFieldProc)certify_size, ObjTmp);

 F(ObjTmp->Grid5).addColumn(ObjTmp->Grid5, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->OFormat), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->Lsize), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->TFdocWidth), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->Lcroix), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->TFdocHeight), XcG_BEST_SIZE,
			    NULL);

 ObjTmp->Frame6 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fPSScale", 0);
 ObjTmp->Grid6 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame6),
				       MANAGER(ObjTmp->Dialog),
				       "gPSScale", TRUE);

 ObjTmp->TBreduire = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid6),
					MANAGER(ObjTmp->Dialog),
					"tbPSReduce", NULL);
 ObjTmp->TBtenirsur = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid6),
					MANAGER(ObjTmp->Dialog),
					"tbPSPrintOnPage", NULL);


 sprintf(str, "%g", SCALE_TO_PERCENT(setup_param->scale));
 ObjTmp->TFreduire = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid6),
				    MANAGER(ObjTmp->Dialog),
				    "tfPSReduceOffset",
				    TF_REAL,
				    0.1, 1000.0,
				    str, NULL, 5);
 F(ObjTmp->TFreduire).set_unit(ObjTmp->TFreduire, XcUB_RATIO_PERCENTAGE,
			       NULL, TRUE, FALSE);
 F(ObjTmp->TFreduire).add_callback(ObjTmp->TFreduire,
				   (WTextFieldProc)certify_reduct, ObjTmp);

 sprintf(str, "%d", setup_param->horizontal_pages);
 ObjTmp->TFtenirsur1 = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid6),
				    MANAGER(ObjTmp->Dialog),
				    "tfPSPrintOnPage1",
				    TF_INTEGER,
				    1.0, 100.0,
				    str, NULL, 5);
 F(ObjTmp->TFtenirsur1).add_callback(ObjTmp->TFtenirsur1,
				     (WTextFieldProc)certify_tenir, ObjTmp);
 ObjTmp->Lx = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid6),
			  MANAGER(ObjTmp->Dialog),
			  " x ");
 sprintf(str, "%d", setup_param->vertical_pages);
 ObjTmp->TFtenirsur2 = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid6),
				    MANAGER(ObjTmp->Dialog),
				    "tfPSPrintOnPage2",
				    TF_INTEGER,
				    1.0, 100.0,
				    str, NULL, 5);
 F(ObjTmp->TFtenirsur2).add_callback(ObjTmp->TFtenirsur2,
				     (WTextFieldProc)certify_tenir, ObjTmp);

 F(ObjTmp->TBreduire).Add_Callback(ObjTmp->TBreduire, FUNC_VALUE,
				   arm_reduct, ObjTmp);
 F(ObjTmp->TBtenirsur).Add_Callback(ObjTmp->TBtenirsur, FUNC_VALUE,
				    arm_tenirsur, ObjTmp);

 switch (setup_param->scale_mode)
 {
 case 0:
  XtSetSensitive(WIDGET(ObjTmp->TFtenirsur1), False);
  XtSetSensitive(WIDGET(ObjTmp->Lx), False);
  XtSetSensitive(WIDGET(ObjTmp->TFtenirsur2), False);
  XtSetSensitive(WIDGET(ObjTmp->TFreduire), False);
  break;
 case 1:
  XmToggleButtonSetState(WIDGET(ObjTmp->TBreduire), True, False);
  XtSetSensitive(WIDGET(ObjTmp->TFtenirsur1), False);
  XtSetSensitive(WIDGET(ObjTmp->Lx), False);
  XtSetSensitive(WIDGET(ObjTmp->TFtenirsur2), False);
  break;
 case 2:
  XmToggleButtonSetState(WIDGET(ObjTmp->TBtenirsur), True, False);
  XtSetSensitive(WIDGET(ObjTmp->TFreduire), False);
 }


 F(ObjTmp->Grid6).addColumn(ObjTmp->Grid6, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid6).addColumn(ObjTmp->Grid6, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->TBreduire), 120,
			    WIDGET(ObjTmp->TFreduire), XcG_BEST_SIZE,
			    NULL);
 F(ObjTmp->Grid6).addColumn(ObjTmp->Grid6, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid6).addColumn(ObjTmp->Grid6, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->TBtenirsur), 120,
			    WIDGET(ObjTmp->TFtenirsur1), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->Lx), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->TFtenirsur2), XcG_BEST_SIZE,
			    NULL);
 F(ObjTmp->Grid6).addColumn(ObjTmp->Grid6, XcG_MAX_SIZE, NULL);
  
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Ltitre), 30,
			   WIDGET(ObjTmp->Frame1), 75,
			   WIDGET(ObjTmp->Frame3), 75,
			   WIDGET(ObjTmp->Frame6), XcG_MAX_SIZE,
			   NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 10, NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Frame5), 65,
			   WIDGET(ObjTmp->Frame4), XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Frame2), 75,
			   NULL);

 F(ObjTmp->OFormat).setSelection(ObjTmp->OFormat, setup_param->page_format);

  
 F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				    XcCD_OK | XcCD_CANCEL, 
				    XcCD_OK,
				    TRUE, (void (*)())cb_action, ObjTmp);
  
 F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);
  
 Xc_TRACE(("Object Box_printsetup build"));
 return ObjTmp;
}

void dest_Box_miseenpage(This)
c_Box_miseenpage *This;
{
 DELETE(c_RadioImage)(This->RIsens);
 DELETE(c_RadioBox)(This->RBwhatPrint);
 DELETE(c_RadioImage)(This->RIPortraitPaysage);
 DELETE(c_WTextField)(This->TFMargeH);
 DELETE(c_Label)(This->LMargeH);
 DELETE(c_Label)(This->LMargeB);
 DELETE(c_Label)(This->LMargeG);
 DELETE(c_Label)(This->LMargeD);
 DELETE(c_WTextField)(This->TFMargeB);
 DELETE(c_WTextField)(This->TFMargeG);
 DELETE(c_WTextField)(This->TFMargeD);
 DELETE(c_RadioBox)(This->RBcentrage);
 DELETE(c_ItemMenu)(This->OFormat);
 DELETE(c_Label)(This->Lsize);
 DELETE(c_WTextField)(This->TFdocHeight);
 DELETE(c_Label)(This->Lcroix);
 DELETE(c_WTextField)(This->TFdocWidth);
 DELETE(c_ToggleButton)(This->TBreduire);
 DELETE(c_ToggleButton)(This->TBtenirsur);
 DELETE(c_WTextField)(This->TFreduire);
 DELETE(c_WTextField)(This->TFtenirsur1);
 DELETE(c_Label)(This->Lx);
 DELETE(c_WTextField)(This->TFtenirsur2);
 DELETE(c_Grid)(This->Grid4);
 DELETE(c_Grid)(This->Grid5);
 DELETE(c_Grid)(This->Grid6);
 DELETE(c_Frame)(This->Frame1);
 DELETE(c_Frame)(This->Frame2);
 DELETE(c_Frame)(This->Frame3);
 DELETE(c_Frame)(This->Frame4);
 DELETE(c_Frame)(This->Frame5);
 DELETE(c_Frame)(This->Frame6);
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This->setup_param);
 Xc_free(This);
 Xc_TRACE(("Object Box_printsetup destroyed"));
}

void *copy_Box_miseenpage(This)
c_Box_miseenpage *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}

static void arm_reduct(w, Obj, cbs)
Widget 			w;
c_Box_miseenpage	*Obj;
XmToggleButtonCallbackStruct *cbs;
{
 if (cbs->set)
 {
  XmToggleButtonSetState(WIDGET(Obj->TBtenirsur), False, False);
  XtSetSensitive(WIDGET(Obj->TFtenirsur1), False);
  XtSetSensitive(WIDGET(Obj->Lx), False);
  XtSetSensitive(WIDGET(Obj->TFtenirsur2), False);
  XtSetSensitive(WIDGET(Obj->TFreduire), True);
  Obj->setup_param->scale_mode = 1;
 }
 else
 {
  XtSetSensitive(WIDGET(Obj->TFreduire), False);
  Obj->setup_param->scale_mode = 0;
 }
}

static void arm_tenirsur(w, Obj, cbs)
Widget 			w;
c_Box_miseenpage	*Obj;
XmToggleButtonCallbackStruct *cbs;
{
 if (cbs->set)
 {
  XmToggleButtonSetState(WIDGET(Obj->TBreduire), False, False);
  XtSetSensitive(WIDGET(Obj->TFreduire), False);
  XtSetSensitive(WIDGET(Obj->TFtenirsur1), True);
  XtSetSensitive(WIDGET(Obj->Lx), True);
  XtSetSensitive(WIDGET(Obj->TFtenirsur2), True);
  Obj->setup_param->scale_mode = 2;
 }
 else
 {
  XtSetSensitive(WIDGET(Obj->TFtenirsur1), False);
  XtSetSensitive(WIDGET(Obj->Lx), False);
  XtSetSensitive(WIDGET(Obj->TFtenirsur2), False);
  Obj->setup_param->scale_mode = 0;
 }
}

static void certify_size(this, Obj)
c_WTextField		*this;
c_Box_miseenpage	*Obj;
{
 coord_t	num;
  
 Xc_TRACE(("size"));
  
 num = F(this).get_value_in_unit(this);
  
 if(this == Obj->TFdocWidth)
  Xc_Table_Page_Format[Xc_CUSTOM_FORMAT].width = num;
 else
  if(this == Obj->TFdocHeight)
   Xc_Table_Page_Format[Xc_CUSTOM_FORMAT].height = num;
  
 F(Obj->OFormat).setSelection(Obj->OFormat,
			      &Xc_Table_Page_Format[Xc_CUSTOM_FORMAT]);
 cb_format(Obj->OFormat, Obj, &Xc_Table_Page_Format[Xc_CUSTOM_FORMAT]);
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void certify_reduct(this, Obj)
c_WTextField		*this;
c_Box_miseenpage	*Obj;
{
 scale_t	num;

 Xc_TRACE(("size"));

 num = F(this).get_value_in_unit(this);

 Obj->setup_param->scale = num;
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void certify_tenir(this, Obj)
c_WTextField		*this;
c_Box_miseenpage	*Obj;
{
 char		*vstr;
 unsigned int	num;

 Xc_TRACE(("size"));

 if((vstr = F(this).get(this)) != NULL)
 {
  num = atoi(vstr);
  Xc_free(vstr);
  if (this == Obj->TFtenirsur1)
   Obj->setup_param->horizontal_pages = num;
  if (this == Obj->TFtenirsur2)
   Obj->setup_param->vertical_pages = num;
      
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
 }
}


/* ----------------------------------------------------------------- ** 
** cb_format - ItemMenu callback for page format                     ** 
** ----------------------------------------------------------------- */
static void cb_format(popup, this, format)
c_ItemMenu *popup;
c_Box_miseenpage *this;
page_format_t *format;
{
 Xc_TRACE(("cb_format"));
  
 F(this->TFdocWidth).set_value_in_unit
  (this->TFdocWidth, (double)SCALE_TO_CENTIMETERS(format->width));
  
 F(this->TFdocHeight).set_value_in_unit
  (this->TFdocHeight, (double)SCALE_TO_CENTIMETERS(format->height));

 this->setup_param->page_format = format;
 this->setup_param->width = format->width;
 this->setup_param->height = format->height;
 F(this->Dialog).resetTraversal(this->Dialog);
}


static void certify_marge(this, Obj)
c_WTextField	*this;
c_Box_miseenpage	*Obj;
{
 coord_t	num;

 Xc_HISTORY(("margin"));
  
 num = F(this).get_value_in_unit(this);
  
 if (this == Obj->TFMargeH)
  Obj->setup_param->top_margin = num;
 if (this == Obj->TFMargeB)
  Obj->setup_param->bottom_margin = num;
 if (this == Obj->TFMargeD)
  Obj->setup_param->right_margin = num;
 if (this == Obj->TFMargeG)
  Obj->setup_param->left_margin = num;
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void Set_param(This, value, appel)
Widget	This;
boolean	*value;
caddr_t	appel;
{
 *value = !*value;
}

/* ----------------------------------------------------------------- ** 
** cb_action - CallBack on exit                                      ** 
** ----------------------------------------------------------------- */
static void cb_action(cb_dialog, This, cb_reason)
c_CustomDialog	*cb_dialog;
c_Box_miseenpage *This;
int		cb_reason;
{
 switch(cb_reason)
 {
 case XcCD_OK:
  memcpy(This->setup_save, This->setup_param, sizeof(setup_print_t));
  This->setup_save->direction_f = (boolean)This->fbool_sens;
  This->setup_save->orientation = (boolean)This->fbool_PP;
  break;
 default:
  Xc_TRACE(("Cancel Box_printsetup"));
 }
 F(cb_dialog).unmap(cb_dialog);
}
