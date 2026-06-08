/*
** Box_imprimer.c for XAllWrite in Box_imprimer/
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
** Started on  Mon Jul 17 14:01:35 1995 Robin Castanier
** Last update Wed May 27 14:30:14 1998 Robin Castanier
*/

#include "Box_imprimer.h"

void *cons_Box_imprimer();
void dest_Box_imprimer();
void *copy_Box_imprimer();
static void changeprinter();
static void toggle_mode_draft();
static void toggle_mode_bw();
static void toggle_mode_color();
static void toggle_left_page();
static void toggle_right_page();
static void toggle_all_pages();
static void certify_nbrcopies();
static void certify_startpage();
static void certify_endpage();

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
 listbutton		*buttonptr;
  
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
							pszTitle, 280, 480,
							XcCD_HORIZONTAL 
							|XcCD_HRESIZE
							|XcCD_VRESIZE);
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);

 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gBoxPrint", FALSE);

 ObjTmp->Ltitre = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  "lBoxPrintTitle");
 ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fPrinter", 0);
 ObjTmp->IMprinter = NEW(c_ItemMenu)(WIDGET(ObjTmp->Frame1),
				     MANAGER(ObjTmp->Dialog),
				     "imPrinter",
				     0, 0);

 F(ObjTmp->IMprinter).setCallback(ObjTmp->IMprinter,
				  changeprinter, ObjTmp);

 F(ObjTmp->IMprinter).set(ObjTmp->IMprinter,
			  TRUE, offsetof(printer_t, name),
			  NULL, NULL);
 F(ObjTmp->IMprinter).addItems(ObjTmp->IMprinter,
			       XcIM_LINKED_LIST, -1, GlobPrinterBase,
			       offsetof(printer_t, next));
  
 ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  NULL, 0);
  
 ObjTmp->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame2),
				       MANAGER(ObjTmp->Dialog),
				       "gNbcopy", TRUE);
 ObjTmp->Lnbcopies = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
			  MANAGER(ObjTmp->Dialog),
			  "lNbCopy");
  
 sprintf(pag_str, "%d", print_param->NbCopies);
 ObjTmp->NbDeCopies = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid2),
				    MANAGER(ObjTmp->Dialog),
				    "tfNbCopy",
				    TF_INTEGER,
				    1.0, 1000.0,
				    pag_str, NULL, 5);
 F(ObjTmp->NbDeCopies).add_callback(ObjTmp->NbDeCopies,
				    certify_nbrcopies, ObjTmp);

 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->Lnbcopies), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->NbDeCopies), XcG_BEST_SIZE,
			    NULL);
  
 ObjTmp->Frame5 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fHowPrint", 0);
 ObjTmp->RBhowPrint = (c_RadioBox *)NEW(c_RadioBox)(WIDGET(ObjTmp->Frame5),
						    MANAGER(ObjTmp->Dialog),
						    RADIO_NORMAL,
						    "rbHowPrint",
						    "rbDraftMode",
						    "rbBWMode",
						    "rbColorMode",
						    NULL);
  
 ObjTmp->Frame3 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fWhatPrint", 0);
 ObjTmp->RBwhatPrint = (c_RadioBox *)NEW(c_RadioBox)(WIDGET(ObjTmp->Frame3),
						     MANAGER(ObjTmp->Dialog),
						     RADIO_NORMAL,
						     "rbWhatPrint",
						     "rbLeftPages",
						     "rbRightPages",
						     NULL);
  
 ObjTmp->Frame4 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fPrintPage", 0);
 ObjTmp->Grid4 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame4),
				       MANAGER(ObjTmp->Dialog),
				       "gPrintPage", TRUE);
  
 ObjTmp->TBtoutes = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid4),
					MANAGER(ObjTmp->Dialog),
					"tbAllPages", NULL);
  
 ObjTmp->LPrintDe = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid4),
					    MANAGER(ObjTmp->Dialog),
					    "lFrom");
  
 sprintf(pag_str, "%d", print_param->StartPage);
 ObjTmp->WTPrintDe = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				    MANAGER(ObjTmp->Dialog),
				    "tfFrom",
				    TF_INTEGER,
				    1.0, (float)print_param->NbPage,
				    pag_str, NULL, 5);
 F(ObjTmp->WTPrintDe).add_callback(ObjTmp->WTPrintDe,
				   certify_startpage, ObjTmp);
 ObjTmp->LPrintA = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid4),
					   MANAGER(ObjTmp->Dialog),
					   "lTo");
 sprintf(pag_str, "%d", print_param->EndPage);
 ObjTmp->WTPrintA = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				    MANAGER(ObjTmp->Dialog),
				    "tfTo",
				    TF_INTEGER,
				    1.0, (float)print_param->NbPage,
				    pag_str, NULL, 5);
 F(ObjTmp->WTPrintA).add_callback(ObjTmp->WTPrintA,
				  certify_endpage, ObjTmp);
  
 F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->TBtoutes), XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->LPrintDe), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->WTPrintDe), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->LPrintA), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->WTPrintA), XcG_BEST_SIZE,
			    NULL);
 F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE, NULL);
  

 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Ltitre), 30,
			   WIDGET(ObjTmp->Frame1), 60,
			   XcG_WIDGET_NULL, 7,
			   WIDGET(ObjTmp->Frame2), 45,
			   WIDGET(ObjTmp->Frame5), 95,
			   WIDGET(ObjTmp->Frame3), 70,
			   WIDGET(ObjTmp->Frame4), XcG_MAX_SIZE,
			   NULL);

 F(ObjTmp->IMprinter).setSelection(ObjTmp->IMprinter, print_param->printer);
  
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
  
 buttonptr = ObjTmp->RBhowPrint->Button;
 if (print_param->Draft)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
 buttonptr = buttonptr->Next;
 if (!print_param->Color)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
 else
 {
  buttonptr = buttonptr->Next;
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
 }    
  
 buttonptr = ObjTmp->RBwhatPrint->Button;
 if (print_param->Left)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
 buttonptr = buttonptr->Next;
 if (print_param->Right)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
  
 F(ObjTmp->RBhowPrint).AddCallback(ObjTmp->RBhowPrint, 0, 
				   XmNvalueChangedCallback,
				   toggle_mode_draft, ObjTmp);
 F(ObjTmp->RBhowPrint).AddCallback(ObjTmp->RBhowPrint, 1, 
				   XmNvalueChangedCallback,
				   toggle_mode_bw, ObjTmp);
 F(ObjTmp->RBhowPrint).AddCallback(ObjTmp->RBhowPrint, 2, 
				   XmNvalueChangedCallback,
				   toggle_mode_color, ObjTmp);
  
 F(ObjTmp->RBwhatPrint).AddCallback(ObjTmp->RBwhatPrint, 0,
				    XmNvalueChangedCallback,
				    toggle_left_page, ObjTmp);
 F(ObjTmp->RBwhatPrint).AddCallback(ObjTmp->RBwhatPrint, 1, 
				    XmNvalueChangedCallback,
				    toggle_right_page, ObjTmp);
  
  
 Xc_TRACE(("Object Box_print build"));
 return ObjTmp;
}

void dest_Box_imprimer(This)
c_Box_imprimer *This;
{
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_Label)(This->LPrintDe);
 DELETE(c_Label)(This->LPrintA);
 DELETE(c_Label)(This->Lnbcopies);
 DELETE(c_WTextField)(This->NbDeCopies);
 DELETE(c_WTextField)(This->WTPrintDe);
 DELETE(c_WTextField)(This->WTPrintA);
 DELETE(c_ItemMenu)(This->IMprinter);
 DELETE(c_RadioBox)(This->RBhowPrint);
 DELETE(c_RadioBox)(This->RBwhatPrint);
 DELETE(c_Grid)(This->Grid2);
 DELETE(c_Grid)(This->Grid4);
 DELETE(c_Frame)(This->Frame1);
 DELETE(c_Frame)(This->Frame2);
 DELETE(c_Frame)(This->Frame3);
 DELETE(c_Frame)(This->Frame4);
 DELETE(c_Frame)(This->Frame5);
 DELETE(c_ToggleButton)(This->TBtoutes);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_print destroyed"));
}

void *copy_Box_imprimer(This)
c_Box_imprimer *This;
{
 Xc_TRACE(("Copy not implemented"));
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

static void toggle_left_page(this, Obj)
Widget		 this;
c_Box_imprimer	*Obj;
{
 Obj->print_param->Left = !Obj->print_param->Left;
}

static void toggle_right_page(this, Obj)
Widget		 this;
c_Box_imprimer	*Obj;
{
 Obj->print_param->Right = !Obj->print_param->Right;
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

static void toggle_mode_draft(this, Obj)
Widget		 this;
c_Box_imprimer	*Obj;
{
 Obj->print_param->Draft = !Obj->print_param->Draft;
}

static void toggle_mode_bw(this, Obj)
Widget		 this;
c_Box_imprimer	*Obj;
{
 listbutton	*buttonptr;
  
 if (Obj->print_param->Color)
 {
  buttonptr = Obj->RBhowPrint->Button->Next->Next;
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
  Obj->print_param->Color = FALSE;
 }
 else
 {
  buttonptr = Obj->RBhowPrint->Button->Next->Next;
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
  Obj->print_param->Color = TRUE;
 }
}

static void toggle_mode_color(this, Obj)
Widget		 this;
c_Box_imprimer	*Obj;
{
 listbutton	*buttonptr;
  
 if (Obj->print_param->Color)
 {
  buttonptr = Obj->RBhowPrint->Button->Next;
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
  Obj->print_param->Color = FALSE;
 }
 else
 {
  buttonptr = Obj->RBhowPrint->Button->Next;
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
  Obj->print_param->Color = TRUE;
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

