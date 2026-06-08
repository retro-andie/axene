/*
** Box_imprimer.c for XQuad in Box_imprimer/
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
** Started on  Mon Sep 18 17:21:09 1995 Robin Castanier
** Last update Fri Oct  2 17:07:44 1998 Robin Castanier
*/

#include "Box_imprimer.h"

void *cons_Box_imprimer();
void dest_Box_imprimer();
void *copy_Box_imprimer();


/*--- Local function used for callbacks ---*/
static void changeprinter ___PROTO((c_ItemMenu *This, c_Box_imprimer *Obj,
				    printer_t *printer));
static void toggle_all_pages ___PROTO((Widget this, c_Box_imprimer *Obj));
static void toggle_selection ___PROTO((Widget this, c_Box_imprimer *Obj));
static void certify_startpage ___PROTO((c_WTextField *this,
					c_Box_imprimer *Obj));
static void certify_endpage ___PROTO((c_WTextField *this,
				      c_Box_imprimer *Obj));
static void certify_nbrcopies ___PROTO((c_WTextField *this,
					c_Box_imprimer *Obj));

/*--- Global Printer DataBase ---*/
extern printer_t *GlobPrinterBase;

sf_Box_imprimer fc_Box_imprimer =
{
 cons_Box_imprimer,
 dest_Box_imprimer,
 copy_Box_imprimer
};


void *cons_Box_imprimer(w_Parent, pszTitle, print_param)
Widget		w_Parent;
char		*pszTitle;
print_param_t	*print_param;
{
 c_Box_imprimer	*ObjTmp;
 char			pag_str[10];
  
 if((ObjTmp = (c_Box_imprimer *)Xc_malloc("Box_print",
					  sizeof(c_Box_imprimer))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }

 ObjTmp->f = &fc_Box_imprimer;
 ObjTmp->print_param = print_param;
  
 Xc_TRACE(("MAX page: %d Start page: %d End page: %d", print_param->NbPage,
	   print_param->StartPage, print_param->EndPage));
  
 ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, 280, 320,
							XcCD_HORIZONTAL
							| XcCD_HRESIZE
							| XcCD_VRESIZE);
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);
  
 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gBoxPrint", FALSE);

 ObjTmp->Ltitre = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  "lBoxPrintTitle");
 ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fBPPrinter", 0);
 ObjTmp->IMprinter = NEW(c_ItemMenu)(WIDGET(ObjTmp->Frame1),
				     MANAGER(ObjTmp->Dialog),
				     "imBPPrinter",
				     0, 0);
 F(ObjTmp->IMprinter).setCallback(ObjTmp->IMprinter,
				  (void *)changeprinter, ObjTmp);

 F(ObjTmp->IMprinter).set(ObjTmp->IMprinter,
			  TRUE, offsetof(printer_t, name),
			  NULL, NULL);
 F(ObjTmp->IMprinter).addItems(ObjTmp->IMprinter,
			       XcIM_LINKED_LIST, -1, GlobPrinterBase,
			       offsetof(printer_t, next));
  
 F(ObjTmp->IMprinter).setSelection(ObjTmp->IMprinter, print_param->printer);

  

 ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fBPPrintPage", 0);
  
 ObjTmp->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame2),
				       MANAGER(ObjTmp->Dialog),
				       "gBPPrintPage", TRUE);
  
 ObjTmp->TBselection = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid2),
					MANAGER(ObjTmp->Dialog),
					"tbBPPrintArea", NULL);
 ObjTmp->TBtoutes = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid2),
					MANAGER(ObjTmp->Dialog),
					"tbBPPrintAllPages", NULL);
  
 ObjTmp->LPrintDe = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
					    MANAGER(ObjTmp->Dialog),
					    "lBPPrintFrom");
  
 sprintf(pag_str, "%d", print_param->StartPage);
 ObjTmp->WTPrintDe = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid2),
				    MANAGER(ObjTmp->Dialog),
				    "tfBPPrintFrom",
				    TF_INTEGER,
				    1.0, (float)print_param->NbPage,
				    pag_str, NULL, 5);
 F(ObjTmp->WTPrintDe).add_callback(ObjTmp->WTPrintDe,
				   (void *)certify_startpage, ObjTmp);

 ObjTmp->LPrintA = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
					   MANAGER(ObjTmp->Dialog),
					   "lBPPrintTo");
 sprintf(pag_str, "%d", print_param->EndPage);
 ObjTmp->WTPrintA = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid2),
				    MANAGER(ObjTmp->Dialog),
				    "tfBPPrintTo",
				    TF_INTEGER,
				    1.0, (float)print_param->NbPage,
				    pag_str, NULL, 5);
 F(ObjTmp->WTPrintA).add_callback(ObjTmp->WTPrintA,
				  (void *)certify_endpage, ObjTmp);
 
 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->TBselection), XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->TBtoutes), XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->LPrintDe), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->WTPrintDe), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->LPrintA), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->WTPrintA), XcG_BEST_SIZE,
			    NULL);

 ObjTmp->Frame3 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  NULL, 0);
  
 ObjTmp->Grid3 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame3),
				       MANAGER(ObjTmp->Dialog),
				       "gBPNbCopy", TRUE);
 ObjTmp->Lnbcopies = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid3),
			  MANAGER(ObjTmp->Dialog),
			  "lBPNbCopy");
  
 sprintf(pag_str, "%d", print_param->NbCopies);
 ObjTmp->NbDeCopies = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid3),
				    MANAGER(ObjTmp->Dialog),
				    "tfBPNbCopy",
				    TF_INTEGER,
				    1.0, 1000.0,
				    pag_str, NULL, 5);
 F(ObjTmp->NbDeCopies).add_callback(ObjTmp->NbDeCopies,
				    (void *)certify_nbrcopies, ObjTmp);

 F(ObjTmp->Grid3).addColumn(ObjTmp->Grid3, 30,
			    WIDGET(ObjTmp->Lnbcopies), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->NbDeCopies), XcG_BEST_SIZE,
			    NULL);
  
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Ltitre), 30,
			   WIDGET(ObjTmp->Frame1), 60,
			   WIDGET(ObjTmp->Frame2), XcG_MAX_SIZE,
			   XcG_WIDGET_NULL, 9,
			   WIDGET(ObjTmp->Frame3), 45,
			   NULL);
  
 F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				    XcCD_OK | XcCD_CANCEL, 
				    XcCD_OK,
				    TRUE, NULL, NULL);
  
 F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);


 if (print_param->AllPages)
  F(ObjTmp->TBtoutes).Set(ObjTmp->TBtoutes, True, False);
 F(ObjTmp->TBtoutes).Add_Callback(ObjTmp->TBtoutes, 
				  XmNvalueChangedCallback,
				  toggle_all_pages, ObjTmp);

 if (print_param->selection_flag)
 {
  F(ObjTmp->TBtoutes).Set(ObjTmp->TBselection, True, False);
  XtSetSensitive(WIDGET(ObjTmp->LPrintDe), FALSE);
  XtSetSensitive(WIDGET(ObjTmp->WTPrintDe), FALSE);
  XtSetSensitive(WIDGET(ObjTmp->LPrintA), FALSE);
  XtSetSensitive(WIDGET(ObjTmp->WTPrintA), FALSE);
 }
 F(ObjTmp->TBselection).Add_Callback(ObjTmp->TBselection, 
				     XmNvalueChangedCallback,
				     toggle_selection, ObjTmp);
  
 Xc_TRACE(("Object Box_print build"));
 return ObjTmp;
}

void dest_Box_imprimer(This)
c_Box_imprimer *This;
{
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_Label)(This->Lnbcopies);
 DELETE(c_Label)(This->LPrintA);
 DELETE(c_Label)(This->LPrintDe);
 DELETE(c_ToggleButton)(This->TBselection);
 DELETE(c_ToggleButton)(This->TBtoutes);
 DELETE(c_WTextField)(This->WTPrintA);
 DELETE(c_WTextField)(This->WTPrintDe);
 DELETE(c_WTextField)(This->NbDeCopies);
 DELETE(c_ItemMenu)(This->IMprinter);
 DELETE(c_Grid)(This->Grid2);
 DELETE(c_Grid)(This->Grid3);
 DELETE(c_Frame)(This->Frame1);
 DELETE(c_Frame)(This->Frame2);
 DELETE(c_Frame)(This->Frame3);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_print destroyed"));
}

void *copy_Box_imprimer(This)
c_Box_imprimer *This;
{
 Xc_TRACE(("Copy not impelmented"));
 return NULL;
}

static void changeprinter(This, Obj, printer)
c_ItemMenu	*This;
c_Box_imprimer	*Obj;
printer_t	*printer;
{
 Xc_TRACE(("printer %s", printer->name));
 Obj->print_param->printer = printer;
}

static void toggle_all_pages(this, Obj)
Widget		 this;
c_Box_imprimer	*Obj;
{
 char	str[6];
  
 if (Obj->print_param->AllPages)
 {
  XmToggleButtonGadgetSetState(Obj->TBtoutes->w_This, True, False);
 }
 else
 {
  Obj->print_param->AllPages = TRUE;
  if (Obj->print_param->selection_flag)
  {
   Obj->print_param->selection_flag = FALSE;
   XmToggleButtonGadgetSetState(Obj->TBselection->w_This, False, False);
   XtSetSensitive(WIDGET(Obj->LPrintDe), TRUE);
   XtSetSensitive(WIDGET(Obj->WTPrintDe), TRUE);
   XtSetSensitive(WIDGET(Obj->LPrintA), TRUE);
   XtSetSensitive(WIDGET(Obj->WTPrintA), TRUE);
  }

  if (Obj->print_param->StartPage != 1)
  {
   Obj->print_param->StartPage = 1;
   sprintf(str, "%d", Obj->print_param->StartPage);
   F(Obj->WTPrintDe).set(Obj->WTPrintDe, str);
  }
  if (Obj->print_param->EndPage != Obj->print_param->NbPage)
  {
   Obj->print_param->EndPage = Obj->print_param->NbPage;
   sprintf(str, "%d", Obj->print_param->EndPage);
   F(Obj->WTPrintA).set(Obj->WTPrintA, str);
  }
 }
}

static void toggle_selection(this, Obj)
Widget		 this;
c_Box_imprimer	*Obj;
{
 Obj->print_param->selection_flag = !Obj->print_param->selection_flag;
 if (Obj->print_param->selection_flag)
 {
  Obj->print_param->AllPages = FALSE;
  XmToggleButtonGadgetSetState(Obj->TBtoutes->w_This, False, False);
 }
 XtSetSensitive(WIDGET(Obj->LPrintDe), !Obj->print_param->selection_flag);
 XtSetSensitive(WIDGET(Obj->WTPrintDe), !Obj->print_param->selection_flag);
 XtSetSensitive(WIDGET(Obj->LPrintA), !Obj->print_param->selection_flag);
 XtSetSensitive(WIDGET(Obj->WTPrintA), !Obj->print_param->selection_flag);
}

static void certify_startpage(this, Obj)
c_WTextField	*this;
c_Box_imprimer	*Obj;
{
 char	*vstr;
 char	str[6];
 unsigned int	num;
 boolean	all_pages;
  
 if((vstr = F(this).get(this)) != NULL)
 {
  num = atoi(vstr);
  Xc_free(vstr);
  Obj->print_param->StartPage = num;
  if ((int)num > Obj->print_param->EndPage)
  {
   Obj->print_param->EndPage = num;
   sprintf(str, "%d", Obj->print_param->EndPage);
   F(Obj->WTPrintA).set(Obj->WTPrintA, str);
  }
      
  all_pages = ((Obj->print_param->StartPage == 1) &&
	       Obj->print_param->EndPage == Obj->print_param->NbPage);
  if (all_pages != Obj->print_param->AllPages)
  {
   Obj->print_param->AllPages = all_pages;
   XmToggleButtonGadgetSetState(Obj->TBtoutes->w_This, 
				(all_pages ? True : False), False);
  }
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
 }
}

static void certify_endpage(this, Obj)
c_WTextField	*this;
c_Box_imprimer	*Obj;
{
 char	*vstr;
 char	str[6];
 unsigned int	num;
 boolean	all_pages;
  
 if((vstr = F(this).get(this)) != NULL)
 {
  num = atoi(vstr);
  Xc_free(vstr);
  Obj->print_param->EndPage = num;
  if ((int)num < Obj->print_param->StartPage)
  {
   Obj->print_param->StartPage = num;
   sprintf(str, "%d", Obj->print_param->StartPage);
   F(Obj->WTPrintDe).set(Obj->WTPrintDe, str);
  }
      
  all_pages = ((Obj->print_param->StartPage == 1) &&
	       Obj->print_param->EndPage == Obj->print_param->NbPage);
  if (all_pages != Obj->print_param->AllPages)
  {
   Obj->print_param->AllPages = all_pages;
   XmToggleButtonGadgetSetState(Obj->TBtoutes->w_This, 
				(all_pages ? True : False), False);
  }
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
 }
}

static void certify_nbrcopies(this, Obj)
c_WTextField	*this;
c_Box_imprimer	*Obj;
{
 char	*vstr;
 unsigned int	num;
  
 if((vstr = F(this).get(this)) != NULL)
 {
  num = atoi(vstr);
  Xc_free(vstr);
  Obj->print_param->NbCopies = num;
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
 }
}

