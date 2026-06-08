/*
** Export_Text_Interface.c for XQuad in Export/
** Export_Text Object Interface
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
** Started on  Sun Dec 15 17:31:21 1996 Robin Castanier
** Last update Wed Mar 12 15:21:48 1997 Robin Castanier
*/

#include "Export_Text.h"

static void toggle_current_formula();
static void toggle_current_selection();
static void toggle_code_latin1();
static void toggle_code_dos();
static void toggle_code_mac();
static void certify_cmd_line();

c_Export_Text	*cb_object;

void export_txt_create_attribs_box(This, Parent, size)
c_Export_Text *This;
Widget Parent;
ab_size_t *size;
{
 Arg		 argsw[2];
  
 cb_object = This;
 This->box->Grid = (c_Grid *)NEW(c_Grid)(Parent, NULL,
					 "gFSExportText", FALSE);


 This->box->tbFSETselection = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(This->box->Grid),
					NULL, "tbFSETselection", NULL);
 F(This->box->tbFSETselection).Add_Callback(This->box->tbFSETselection,
					    XmNvalueChangedCallback,
					    toggle_current_selection,
					    &cb_object);

 This->box->tbFSETformula = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(This->box->Grid),
					NULL, "tbFSETformula", NULL);
 F(This->box->tbFSETformula).Add_Callback(This->box->tbFSETformula, 
					  XmNvalueChangedCallback,
					  toggle_current_formula, &cb_object);
  
 This->box->sep1 = XmCreateSeparatorGadget(WIDGET(This->box->Grid),
					   "___sep1___", NULL, 0);

 This->box->lFSETseparators = (c_Label *)NEW(c_Label)(WIDGET(This->box->Grid),
						      NULL, 
						      "lFSETseparators");
 This->box->tfFSETseparators = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->box->Grid),
				    NULL,
				    "tfFSETseparators",
				    TF_STRING,
				    0.0, (real)(SEP_SIZE-1),
				    "", NULL, 5);
  
 F(This->box->tfFSETseparators).add_callback(This->box->tfFSETseparators,
					     certify_cmd_line, &cb_object);
  
 This->box->sep2 = XmCreateSeparatorGadget(WIDGET(This->box->Grid),
					   "___sep2___", NULL, 0);


 This->box->lFSETcharCode = (c_Label *)NEW(c_Label)(WIDGET(This->box->Grid),
						    NULL, "lFSETcharCode");


 This->box->rbFSETcharCode = 
  (c_RadioBox *)NEW(c_RadioBox)(WIDGET(This->box->Grid),
				NULL,
				RADIO_NORMAL,
				"rbFSETcharCode",
				"rbCodeWindows",
				"rbCodeDos",
				"rbCodeMac",
				NULL);
  
 XtSetArg(argsw[0], XmNradioBehavior, True);
 XtSetArg(argsw[1], XmNradioAlwaysOne, True);
 XtSetValues(WIDGET(This->box->rbFSETcharCode), argsw, 2);
  
 F(This->box->rbFSETcharCode).AddCallback(This->box->rbFSETcharCode, 0, 
					  XmNvalueChangedCallback,
					  toggle_code_latin1, &cb_object);
 F(This->box->rbFSETcharCode).AddCallback(This->box->rbFSETcharCode, 1, 
					  XmNvalueChangedCallback,
					  toggle_code_dos, &cb_object);
 F(This->box->rbFSETcharCode).AddCallback(This->box->rbFSETcharCode, 2, 
					  XmNvalueChangedCallback,
					  toggle_code_mac, &cb_object);
  
 F(This->box->Grid).addColumn(This->box->Grid, XcG_MAX_SIZE,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(This->box->tbFSETselection),
			      XcG_BEST_SIZE,
			      WIDGET(This->box->tbFSETformula), XcG_BEST_SIZE,
			      This->box->sep1, XcG_MAX_SIZE,
			      WIDGET(This->box->lFSETseparators),XcG_BEST_SIZE,
			      WIDGET(This->box->tfFSETseparators),
			      XcG_BEST_SIZE,
			      This->box->sep2, XcG_MAX_SIZE,
			      WIDGET(This->box->lFSETcharCode), XcG_BEST_SIZE,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(This->box->rbFSETcharCode), XcG_BEST_SIZE,
			      XcG_WIDGET_NULL, XcG_MAX_SIZE,
			      XcG_WIDGET_NULL, XcG_MAX_SIZE,
			      NULL);


 if (size->dim_calc)
 {
  XtVaSetValues(WIDGET(This->box->Grid), XmNwidth, size->width,
		XmNheight, size->height, NULL);
  F(This->box->Grid).callResize(This->box->Grid);
 }
  
 XtManageChild(WIDGET(This->box->tbFSETselection));
 XtManageChild(WIDGET(This->box->tbFSETformula));
 XtManageChild(This->box->sep1);
 XtManageChild(WIDGET(This->box->lFSETcharCode));
 XtManageChild(WIDGET(This->box->rbFSETcharCode));
 XtManageChild(This->box->sep2);
 XtManageChild(WIDGET(This->box->lFSETseparators));
 XtManageChild(WIDGET(This->box->tfFSETseparators));
}

void export_txt_manage_attribs_box(This, doc_available, size)
c_Export_Text *This;
boolean	doc_available;
ab_size_t *size;
{
 listbutton	*buttonptr;  
  
 if (This->box->managed)
 {
  if (!size->dim_calc)
  {
   XtVaGetValues(WIDGET(This->box->Grid), XmNwidth, &(size->width),
		 XmNheight, &(size->height), NULL);
   size->dim_calc = TRUE;
  }
  XtUnmanageChild(WIDGET(This->box->Grid));
      
  This->box->managed = FALSE;
 }
 else
 {
  cb_object = This;
    
  F(This->box->tbFSETformula).Set(This->box->tbFSETformula, 
				  This->formula, False);
  F(This->box->tbFSETselection).Set(This->box->tbFSETselection, 
				    This->selection, False);
  F(This->box->tfFSETseparators).set(This->box->tfFSETseparators, 
				     This->sep_cmd);
    
  buttonptr = This->box->rbFSETcharCode->Button;
  switch(This->encoding)
  {
  case XcE_MAC:
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
   buttonptr = buttonptr->Next;
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
   buttonptr = buttonptr->Next;
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
   break;
  case XcE_DOS:
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
   buttonptr = buttonptr->Next;
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
   buttonptr = buttonptr->Next;
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
   break;
  case XcE_LATIN1:
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
   buttonptr = buttonptr->Next;
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
   buttonptr = buttonptr->Next;
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
  case XcE_HTML:
   break;
  }
    
  XtManageChild(WIDGET(This->box->Grid));
    
  This->box->managed = TRUE;
 }
}

void export_txt_destroy_attribs_box(This)
c_Export_Text *This;
{
 DELETE(c_ToggleButton)(This->box->tbFSETselection);
 DELETE(c_ToggleButton)(This->box->tbFSETformula);
 XtDestroyWidget(This->box->sep1);
 DELETE(c_Label)(This->box->lFSETseparators);
 DELETE(c_WTextField)(This->box->tfFSETseparators);
 XtDestroyWidget(This->box->sep2);
 DELETE(c_Label)(This->box->lFSETcharCode);
 DELETE(c_RadioBox)(This->box->rbFSETcharCode);
 DELETE(c_Grid)(This->box->Grid);
}

static void toggle_current_formula(this, Obj)
Widget		this;
c_Export_Text	**Obj;
{
 (*Obj)->formula ^= TRUE;
}

static void toggle_current_selection(this, Obj)
Widget		this;
c_Export_Text	**Obj;
{
 (*Obj)->selection ^= TRUE;
}

static void toggle_code_latin1(this, Obj)
Widget		this;
c_Export_Text	**Obj;
{
 (*Obj)->encoding = XcE_LATIN1;
}

static void toggle_code_dos(this, Obj)
Widget		this;
c_Export_Text	**Obj;
{
 (*Obj)->encoding = XcE_DOS;
}

static void toggle_code_mac(this, Obj)
Widget		this;
c_Export_Text	**Obj;
{
 (*Obj)->encoding = XcE_MAC;
}

static void certify_cmd_line(this, Obj)
c_WTextField	*this;
c_Export_Text	**Obj;
{
 char  *cmd_line;
  
 if((cmd_line = F(this).get(this)) != NULL)
 {
  memcpy((*Obj)->sep_cmd, cmd_line, strlen(cmd_line) + 1);
  Xc_free(cmd_line);
 }
}
