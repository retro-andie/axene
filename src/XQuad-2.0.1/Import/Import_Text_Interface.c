/*
** Import_Text_Interface.c for XQuad in Import/
** Import_Text Object Interface
**
** Copyright (C) 1995-2000 Axene.
** Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
** Started on  Wed Feb 22 16:59:27 1995 Emmanuel Paris
** Last update Mon May 29 00:33:15 2000 Emmanuel Paris
*/

#include "Import_Text.h"

extern void repair_cmd_line();

static void toggle_current_doc();
static void toggle_trim_before();
static void toggle_trim_after();
static void toggle_code_latin1();
static void toggle_code_dos();
static void toggle_code_mac();
static void certify_cmd_line();

static c_Import_Text	*cb_object;

void txt_create_attribs_box(This, Parent)
c_Import_Text *This;
Widget Parent;
{
 Arg		 argsw[2];
  
 cb_object = This;
 This->box->Grid = (c_Grid *)NEW(c_Grid)(Parent, NULL,
					 "gFSImportText", FALSE);

 This->box->tbFSITdoc = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(This->box->Grid),
					NULL, "tbFSITdoc", NULL);
	
 F(This->box->tbFSITdoc).Add_Callback(This->box->tbFSITdoc, 
				      XmNvalueChangedCallback,
				      toggle_current_doc, &cb_object);
  
 This->box->sep1 = XmCreateSeparatorGadget(WIDGET(This->box->Grid),
					   "___sep1___", NULL, 0);

 This->box->rbFSITtrim = 
  (c_RadioBox *)NEW(c_RadioBox)(WIDGET(This->box->Grid),
				NULL,
				RADIO_NORMAL,
				"rbFSITtrim",
				"rbTrimBefore",
				"rbTrimAfter",
				NULL);
  
 F(This->box->rbFSITtrim).AddCallback(This->box->rbFSITtrim, 0, 
				      XmNvalueChangedCallback,
				      toggle_trim_before, &cb_object);
 F(This->box->rbFSITtrim).AddCallback(This->box->rbFSITtrim, 1, 
				      XmNvalueChangedCallback,
				      toggle_trim_after, &cb_object);
  
 This->box->sep2 = XmCreateSeparatorGadget(WIDGET(This->box->Grid),
					   "___sep2___", NULL, 0);


 This->box->lFSITseparators = (c_Label *)NEW(c_Label)(WIDGET(This->box->Grid),
						      NULL, 
						      "lFSITseparators");
 This->box->tfFSITseparators = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->box->Grid),
				    NULL,
				    "tfFSITseparators",
				    TF_STRING,
				    0.0, 199.0,
				    "", NULL, 5);
  
 F(This->box->tfFSITseparators).add_callback(This->box->tfFSITseparators,
					     certify_cmd_line, &cb_object);
  
 This->box->sep3 = XmCreateSeparatorGadget(WIDGET(This->box->Grid),
					   "___sep3___", NULL, 0);

 This->box->lFSITcharCode = (c_Label *)NEW(c_Label)(WIDGET(This->box->Grid),
						    NULL, "lFSITcharCode");


 This->box->rbFSITcharCode = 
  (c_RadioBox *)NEW(c_RadioBox)(WIDGET(This->box->Grid),
				NULL,
				RADIO_NORMAL,
				"rbFSITcharCode",
				"rbCodeWindows",
				"rbCodeDos",
				"rbCodeMac",
				NULL);
  
 XtSetArg(argsw[0], XmNradioBehavior, True);
 XtSetArg(argsw[1], XmNradioAlwaysOne, True); 
 XtSetValues(WIDGET(This->box->rbFSITcharCode), argsw, 2);
  
 F(This->box->rbFSITcharCode).AddCallback(This->box->rbFSITcharCode, 0, 
					  XmNvalueChangedCallback,
					  toggle_code_latin1, &cb_object);
 F(This->box->rbFSITcharCode).AddCallback(This->box->rbFSITcharCode, 1, 
					  XmNvalueChangedCallback,
					  toggle_code_dos, &cb_object);
 F(This->box->rbFSITcharCode).AddCallback(This->box->rbFSITcharCode, 2, 
					  XmNvalueChangedCallback,
					  toggle_code_mac, &cb_object);
  
 F(This->box->Grid).addColumn(This->box->Grid, XcG_MAX_SIZE,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(This->box->tbFSITdoc), XcG_BEST_SIZE,
			      This->box->sep1, XcG_MAX_SIZE,
			      WIDGET(This->box->rbFSITtrim), XcG_BEST_SIZE,
			      This->box->sep2, XcG_MAX_SIZE,
			      WIDGET(This->box->lFSITseparators),
			      XcG_BEST_SIZE,
			      WIDGET(This->box->tfFSITseparators),
			      XcG_BEST_SIZE,
			      This->box->sep3, XcG_MAX_SIZE,
			      WIDGET(This->box->lFSITcharCode), XcG_BEST_SIZE,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(This->box->rbFSITcharCode), XcG_BEST_SIZE,
			      XcG_WIDGET_NULL, XcG_MAX_SIZE,
			      NULL);

 XtManageChild(WIDGET(This->box->tbFSITdoc));
 XtManageChild(This->box->sep1);
 XtManageChild(WIDGET(This->box->rbFSITtrim));
 XtManageChild(This->box->sep2);
 XtManageChild(WIDGET(This->box->lFSITcharCode));
 XtManageChild(WIDGET(This->box->rbFSITcharCode));
 XtManageChild(This->box->sep3);
 XtManageChild(WIDGET(This->box->lFSITseparators));
 XtManageChild(WIDGET(This->box->tfFSITseparators));
}

void txt_manage_attribs_box(This, doc_available)
c_Import_Text *This;
boolean	doc_available;
{
 listbutton	*buttonptr;  
  
 if (This->box->managed)
 {
  XtUnmanageChild(WIDGET(This->box->Grid));
      
  This->box->managed = FALSE;
 }
 else
 {
  cb_object = This;
    
  F(This->box->tbFSITdoc).Set(This->box->tbFSITdoc, 
			      doc_available? !This->new_doc : False, False);
  XtSetSensitive(WIDGET(This->box->tbFSITdoc), doc_available ? True : False);
    
  buttonptr = This->box->rbFSITtrim->Button;
  if (This->trim_before)
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
  else
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
  buttonptr = buttonptr->Next;
  if (This->trim_after)
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, False);
  else
   XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
    
  F(This->box->tfFSITseparators).set(This->box->tfFSITseparators, 
				     This->sep_cmd);
    
  buttonptr = This->box->rbFSITcharCode->Button;
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

void txt_destroy_attribs_box(This)
c_Import_Text *This;
{
 DELETE(c_ToggleButton)(This->box->tbFSITdoc);
 XtDestroyWidget(This->box->sep1);
 DELETE(c_RadioBox)(This->box->rbFSITtrim);
 XtDestroyWidget(This->box->sep2);
 DELETE(c_Label)(This->box->lFSITseparators);
 DELETE(c_WTextField)(This->box->tfFSITseparators);
 XtDestroyWidget(This->box->sep3);
 DELETE(c_Label)(This->box->lFSITcharCode);
 DELETE(c_RadioBox)(This->box->rbFSITcharCode);
 DELETE(c_Grid)(This->box->Grid);
}

static void toggle_current_doc(this, Obj)
Widget		this;
c_Import_Text	**Obj;
{
 (*Obj)->new_doc ^= TRUE;
}

static void toggle_trim_before(this, Obj)
Widget		this;
c_Import_Text	**Obj;
{
 (*Obj)->trim_before ^= TRUE;
}

static void toggle_trim_after(this, Obj)
Widget		this;
c_Import_Text	**Obj;
{
 (*Obj)->trim_after ^= TRUE;
}

static void toggle_code_latin1(this, Obj)
Widget		this;
c_Import_Text	**Obj;
{
 (*Obj)->encoding = XcE_LATIN1;
}

static void toggle_code_dos(this, Obj)
Widget		this;
c_Import_Text	**Obj;
{
 (*Obj)->encoding = XcE_DOS;
}

static void toggle_code_mac(this, Obj)
Widget		this;
c_Import_Text	**Obj;
{
 (*Obj)->encoding = XcE_MAC;
}

static void certify_cmd_line(this, Obj)
c_WTextField	*this;
c_Import_Text	**Obj;
{    
 char  *cmd_line;
 char	cmd_line2[200];
  
 if((cmd_line = F(this).get(this)) != NULL)
 {
  memcpy(cmd_line2, cmd_line, strlen(cmd_line)+1);
  Xc_TRACE(("cmd_line before: %s", cmd_line2));
  repair_cmd_line(cmd_line2);
  Xc_TRACE(("cmd_line after: %s", cmd_line2));
  if (Xstrcmp(cmd_line, cmd_line2) == FALSE)
   F(this).set(this, cmd_line2);
  memcpy((*Obj)->sep_cmd, cmd_line2, strlen(cmd_line2) + 1);
  Xc_free(cmd_line);
 }
}
