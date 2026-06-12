/*
** Export_FText_Interface.c for XQuad in Export/
** Export_FText Object Interface
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
** Started on  Sun Dec 15 17:31:21 1996 Robin Castanier
** Last update Wed Mar 12 14:15:48 1997 Robin Castanier
*/

#include "Export_FText.h"

static void toggle_current_selection();
static void toggle_current_grid();
static void toggle_current_headers();
static void toggle_code_latin1();
static void toggle_code_dos();
static void toggle_code_mac();

static c_Export_FText	*cb_object;

void export_ftxt_create_attribs_box(This, Parent, size)
c_Export_FText *This;
Widget Parent;
ab_size_t *size;
{
 Arg		 argsw[2];
  
 cb_object = This;
 This->box->Grid = (c_Grid *)NEW(c_Grid)(Parent, NULL,
					 "gFSExportFText", FALSE);
  
  
 This->box->tbFSETselection = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(This->box->Grid),
					NULL, "tbFSETselection", NULL);
 F(This->box->tbFSETselection).Add_Callback(This->box->tbFSETselection,
					    XmNvalueChangedCallback,
					    toggle_current_selection,
					    &cb_object);
 This->box->tbFSETgrid = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(This->box->Grid),
					NULL, "tbFSETgrid", NULL);
 F(This->box->tbFSETgrid).Add_Callback(This->box->tbFSETgrid,
				       XmNvalueChangedCallback,
				       toggle_current_grid,
				       &cb_object);
 This->box->tbFSETheaders = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(This->box->Grid),
					NULL, "tbFSETheaders", NULL);
 F(This->box->tbFSETheaders).Add_Callback(This->box->tbFSETheaders,
					  XmNvalueChangedCallback,
					  toggle_current_headers,
					  &cb_object);
  
 This->box->sep1 = XmCreateSeparatorGadget(WIDGET(This->box->Grid),
					   "___sep1___", NULL, 0);

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
			      WIDGET(This->box->tbFSETselection),XcG_BEST_SIZE,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(This->box->tbFSETgrid), XcG_BEST_SIZE,
			      WIDGET(This->box->tbFSETheaders), XcG_BEST_SIZE,
			      This->box->sep1, XcG_MAX_SIZE,
			      WIDGET(This->box->lFSETcharCode), XcG_BEST_SIZE,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(This->box->rbFSETcharCode), XcG_BEST_SIZE,
			      XcG_WIDGET_NULL, XcG_MAX_SIZE, 
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
 XtManageChild(WIDGET(This->box->tbFSETgrid));
 XtManageChild(WIDGET(This->box->tbFSETheaders));
 XtManageChild(This->box->sep1);
 XtManageChild(WIDGET(This->box->lFSETcharCode));
 XtManageChild(WIDGET(This->box->rbFSETcharCode));
}

void export_ftxt_manage_attribs_box(This, doc_available, size)
c_Export_FText *This;
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
   
  F(This->box->tbFSETselection).Set(This->box->tbFSETselection, 
				    This->selection, False);
  F(This->box->tbFSETgrid).Set(This->box->tbFSETgrid,
			       This->grid, False);
  F(This->box->tbFSETheaders).Set(This->box->tbFSETheaders, 
				  This->headers, False);
   
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

void export_ftxt_destroy_attribs_box(This)
c_Export_FText *This;
{
 DELETE(c_ToggleButton)(This->box->tbFSETselection);
 DELETE(c_ToggleButton)(This->box->tbFSETgrid);
 DELETE(c_ToggleButton)(This->box->tbFSETheaders);
 XtDestroyWidget(This->box->sep1);
 DELETE(c_Label)(This->box->lFSETcharCode);
 DELETE(c_RadioBox)(This->box->rbFSETcharCode);
 DELETE(c_Grid)(This->box->Grid);
}

static void toggle_current_selection(this, Obj)
Widget		this;
c_Export_FText	**Obj;
{
 (*Obj)->selection ^= TRUE;
}

static void toggle_current_grid(this, Obj)
Widget		this;
c_Export_FText	**Obj;
{
 (*Obj)->grid ^= TRUE;
}

static void toggle_current_headers(this, Obj)
Widget		this;
c_Export_FText	**Obj;
{
 (*Obj)->headers ^= TRUE;
}

static void toggle_code_latin1(this, Obj)
Widget		this;
c_Export_FText	**Obj;
{
 (*Obj)->encoding = XcE_LATIN1;
}

static void toggle_code_dos(this, Obj)
Widget		this;
c_Export_FText	**Obj;
{
 (*Obj)->encoding = XcE_DOS;
}

static void toggle_code_mac(this, Obj)
Widget		this;
c_Export_FText	**Obj;
{
 (*Obj)->encoding = XcE_MAC;
}

