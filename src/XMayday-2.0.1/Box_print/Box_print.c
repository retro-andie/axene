/*
** Box_print.c for XMayday in Box_print/
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Sun Sep  1 07:39:47 1996 Robin Castanier
** Last update Sun Feb  8 16:18:58 1998 Emmanuel Paris
*/

#define _PRIVATE_PAGEFORMAT_
#include "Box_printP.h"
#include "Box_print.h"
#include <X11/IntrinsicP.h>

extern c_Resource *GlobResources;

void *cons_Box_print();
void dest_Box_print();
void *copy_Box_print();
static void change_printer();
static void toggle_mode_banners();
static void toggle_mode_footnotes();
static void toggle_mode_draft();
static void toggle_mode_color();


static void certify_size ___PROTO((c_WTextField *this,
				      c_Box_print *Obj));
static void cb_format ___PROTO((c_ItemMenu *popup, c_Box_print *this,
				page_format_t *format));
static void cb_exporttype ___PROTO((c_ItemMenu *popup, c_Box_print *this,
				    export_type_t *exporttype));
static void cb_fonttype ___PROTO((c_ItemMenu *popup, c_Box_print *this,
				    font_type_t *fonttype));

sf_Box_print fc_Box_print =
{
 cons_Box_print,
 dest_Box_print,
 copy_Box_print
};


void *cons_Box_print(w_Parent, pszTitle, param)
Widget		w_Parent;
char		*pszTitle;
print_param_t	*param;
{
 c_Box_print	*ObjTmp;
 listbutton	*buttonptr;
 char		str[10];
 int		i;
  
 if((ObjTmp = (c_Box_print *)Xc_malloc("Box_print",
				       sizeof(c_Box_print))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 
 ObjTmp->f = &fc_Box_print;
 ObjTmp->param = param;
  
 ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, 400, 370,
							XcCD_HORIZONTAL
							| XcCD_HRESIZE
							| XcCD_VRESIZE);
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);

 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gBoxPrint", TRUE);

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
				  change_printer, ObjTmp); 

 F(ObjTmp->IMprinter).set(ObjTmp->IMprinter,
			  TRUE, offsetof(printer_t, name),
			  NULL, NULL);
 F(ObjTmp->IMprinter).addItems(ObjTmp->IMprinter,
			       XcIM_LINKED_LIST, -1, GlobPrinterBase,
			       offsetof(printer_t, next));
  
 ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fExportType", 50);
  
 if (!Xc_Table_Export_Type[0].name)
 {
  for(i = 0; i < Xc_Count_Export_Type; i++)
  {
   Xc_Table_Export_Type[i].name = F(GlobResources).getString
    (GlobResources, Xc_Table_Export_Type[i].resource);
  }
 }
  
 ObjTmp->IMexportType = NEW(c_ItemMenu)(WIDGET(ObjTmp->Frame2),
					MANAGER(ObjTmp->Dialog),
					"imExportType",
					0, 0);
  
 F(ObjTmp->IMexportType).setCallback(ObjTmp->IMexportType, 
				     (void (*)())cb_exporttype, ObjTmp);
 F(ObjTmp->IMexportType).set(ObjTmp->IMexportType, TRUE,
			     offsetof(export_type_t, name), NULL, NULL);
 F(ObjTmp->IMexportType).addItems(ObjTmp->IMexportType, XcIM_ARRAY, 
				  Xc_Count_Export_Type,
				  Xc_Table_Export_Type, 
				  sizeof(export_type_t));
  
  
 ObjTmp->Frame6 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fFontType", 50);
  
 ObjTmp->IMfontType = NEW(c_ItemMenu)(WIDGET(ObjTmp->Frame6),
				      MANAGER(ObjTmp->Dialog),
				      "imFontType",
				      0, 0);
  
 F(ObjTmp->IMfontType).setCallback(ObjTmp->IMfontType, 
				   (void (*)())cb_fonttype, ObjTmp);
 F(ObjTmp->IMfontType).set(ObjTmp->IMfontType, TRUE,
			   offsetof(font_type_t, name), NULL, NULL);
 F(ObjTmp->IMfontType).addItems(ObjTmp->IMfontType, XcIM_ARRAY, 
				Xc_Count_Font_Type,
				Xc_Table_Font_Type, 
				sizeof(font_type_t));
  

  
 ObjTmp->Frame4 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  "fFormat", 90);

 ObjTmp->GridFormat = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame4),
					    MANAGER(ObjTmp->Dialog),
					    "gBPFormat", TRUE);

 XtVaSetValues(WIDGET(ObjTmp->GridFormat), 
	       XmNmarginHeight, 0,
	       XmNmarginWidth, 0,
	       NULL);


 ObjTmp->IMpageFormat = NEW(c_ItemMenu)(WIDGET(ObjTmp->GridFormat),
					MANAGER(ObjTmp->Dialog), 
					"imPageFormat", 0, 0);
  
 F(ObjTmp->IMpageFormat).setCallback(ObjTmp->IMpageFormat, 
				     (void (*)())cb_format, ObjTmp);
  
 F(ObjTmp->IMpageFormat).set(ObjTmp->IMpageFormat, TRUE,
			     offsetof(page_format_t, name), NULL, NULL);
  
 F(ObjTmp->IMpageFormat).addItems(ObjTmp->IMpageFormat, XcIM_ARRAY,
				  Xc_Count_Page_Format,
				  Xc_Table_Page_Format,
				  sizeof(page_format_t));
  
 ObjTmp->Lsize = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridFormat),
			  MANAGER(ObjTmp->Dialog),
			  "lBPsize");
 sprintf(str, "%g", SCALE_TO_CENTIMETERS(param->page_format->width));
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
 sprintf(str, "%g", SCALE_TO_CENTIMETERS(param->page_format->height));
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
				 WIDGET(ObjTmp->IMpageFormat), XcG_BEST_SIZE,
				 WIDGET(ObjTmp->Lsize), XcG_MAX_SIZE,
				 WIDGET(ObjTmp->TFdocWidth), XcG_BEST_SIZE,
				 WIDGET(ObjTmp->Lcroix), XcG_BEST_SIZE,
				 WIDGET(ObjTmp->TFdocHeight), XcG_BEST_SIZE,
				 XcG_WIDGET_NULL, 7,
				 NULL);
  
 ObjTmp->Frame5 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fParams", 0);
 ObjTmp->Grid5 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame5),
				       MANAGER(ObjTmp->Dialog),
				       "gBPparams", TRUE);

 ObjTmp->RBparam1 = (c_RadioBox *)NEW(c_RadioBox)(WIDGET(ObjTmp->Grid5),
						  MANAGER(ObjTmp->Dialog),
						  RADIO_NORMAL,
						  "rbPParam1",
						  "rbBanners",
						  "rbFootnotes",
						  NULL);
 ObjTmp->RBparam2 = (c_RadioBox *)NEW(c_RadioBox)(WIDGET(ObjTmp->Grid5),
						  MANAGER(ObjTmp->Dialog),
						  RADIO_NORMAL,
						  "rbPParam2",
						  "rbDraftMode",
						  "rbColorMode",
						  NULL);
    
 F(ObjTmp->Grid5).addColumn(ObjTmp->Grid5, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->RBparam1), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->RBparam2), XcG_MAX_SIZE,
			    NULL);
  
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 30,
			   WIDGET(ObjTmp->Ltitre), XcG_MAX_SIZE,
			   NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 60,
			   WIDGET(ObjTmp->Frame1), XcG_MAX_SIZE,
			   NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 60,
			   WIDGET(ObjTmp->Frame2), XcG_MAX_SIZE,
			   XcG_WIDGET_NULL, 5,
			   WIDGET(ObjTmp->Frame6), XcG_MAX_SIZE,
			   NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 60,
			   WIDGET(ObjTmp->Frame4), XcG_MAX_SIZE,
			   NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Frame5), XcG_MAX_SIZE,
			   NULL);
  
 F(ObjTmp->IMprinter).setSelection(ObjTmp->IMprinter, param->printer);
  
 F(ObjTmp->IMpageFormat).setSelection(ObjTmp->IMpageFormat,
				      param->page_format);
  
 F(ObjTmp->IMexportType).setSelection
  (ObjTmp->IMexportType, &Xc_Table_Export_Type[param->exporttype]);
  
 F(ObjTmp->IMfontType).setSelection(ObjTmp->IMfontType,
				    &Xc_Table_Font_Type[param->fonttype]);
  
 F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				    XcCD_OK | XcCD_CANCEL, 
				    XcCD_OK,
				    TRUE, NULL, NULL);
  
 F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);
  
 ObjTmp->nopostscript = FALSE;
 XtQueryGeometry(WIDGET(ObjTmp->Frame5), NULL, &(ObjTmp->geometry));
 cb_exporttype(ObjTmp->IMexportType, ObjTmp,
	       &Xc_Table_Export_Type[param->exporttype]);
  
 buttonptr = ObjTmp->RBparam1->Button;
 if (param->bBanners)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
  
 buttonptr = buttonptr->Next;
 if (param->bFootnotes)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
  
 buttonptr = ObjTmp->RBparam2->Button;
 if (param->bDraft)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
  
 buttonptr = buttonptr->Next;
 if (param->bColor)
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
  
  
 F(ObjTmp->RBparam1).AddCallback(ObjTmp->RBparam1, 0, 
				 XmNvalueChangedCallback,
				 toggle_mode_banners, ObjTmp);
 F(ObjTmp->RBparam1).AddCallback(ObjTmp->RBparam1, 1, 
				 XmNvalueChangedCallback,
				 toggle_mode_footnotes, ObjTmp);
 F(ObjTmp->RBparam2).AddCallback(ObjTmp->RBparam2, 0, 
				 XmNvalueChangedCallback,
				 toggle_mode_draft, ObjTmp);
 F(ObjTmp->RBparam2).AddCallback(ObjTmp->RBparam2, 1, 
				 XmNvalueChangedCallback,
				 toggle_mode_color, ObjTmp);
  
 Xc_TRACE(("Object Box_print build"));
 return ObjTmp;
}

void dest_Box_print(This)
c_Box_print *This;
{
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_ItemMenu)(This->IMprinter);
 DELETE(c_RadioBox)(This->RBparam1);
 DELETE(c_RadioBox)(This->RBparam2);
  
 DELETE(c_ItemMenu)(This->IMexportType);
 DELETE(c_ItemMenu)(This->IMfontType);
  
 DELETE(c_Label)(This->Lsize);
 DELETE(c_WTextField)(This->TFdocWidth);
 DELETE(c_Label)(This->Lcroix);
 DELETE(c_WTextField)(This->TFdocHeight);
 DELETE(c_ItemMenu)(This->IMpageFormat);
	 
 DELETE(c_Grid)(This->GridFormat);
 DELETE(c_Grid)(This->Grid5);

 DELETE(c_Frame)(This->Frame1);
 DELETE(c_Frame)(This->Frame2);
 DELETE(c_Frame)(This->Frame4);
 DELETE(c_Frame)(This->Frame5);
 DELETE(c_Frame)(This->Frame6);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_print destroyed"));
}

void *copy_Box_print(This)
c_Box_print *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}

static void change_printer(This, Obj, printer)
c_ItemMenu	*This;
c_Box_print	*Obj;
printer_t	*printer;
{
 Xc_TRACE(("printer %s", printer->name));
 Obj->param->printer = printer;
}


static void toggle_mode_banners(this, Obj)
Widget		 this;
c_Box_print	*Obj;
{
 Obj->param->bBanners = !Obj->param->bBanners;
}

static void toggle_mode_footnotes(this, Obj)
Widget		 this;
c_Box_print	*Obj;
{
 Obj->param->bFootnotes = !Obj->param->bFootnotes;
}

static void toggle_mode_draft(this, Obj)
Widget		 this;
c_Box_print	*Obj;
{
 Obj->param->bDraft = !Obj->param->bDraft;
}

static void toggle_mode_color(this, Obj)
Widget		 this;
c_Box_print	*Obj;
{
 Obj->param->bColor = !Obj->param->bColor;
}


/* ----------------------------------------------------------------- ** 
** cb_format - ItemMenu callback for page format                     ** 
** ----------------------------------------------------------------- */
static void cb_format(popup, this, format)
c_ItemMenu *popup;
c_Box_print *this;
page_format_t *format;
{
 Xc_TRACE(("cb_format"));
  
 F(this->TFdocWidth).set_value_in_unit
  (this->TFdocWidth, (double)SCALE_TO_CENTIMETERS(format->width));
  
 F(this->TFdocHeight).set_value_in_unit
  (this->TFdocHeight, (double)SCALE_TO_CENTIMETERS(format->height));
  
 this->param->page_format = format;
 F(this->Dialog).resetTraversal(this->Dialog);
}

/* ----------------------------------------------------------------- ** 
** cb_exporttype - ItemMenu callback for page format                     ** 
** ----------------------------------------------------------------- */
static void cb_exporttype(popup, this, exporttype)
c_ItemMenu *popup;
c_Box_print *this;
export_type_t *exporttype;
{
 Xc_TRACE(("cb_exporttype"));
  
 this->param->exporttype = exporttype->param;
  
 if (exporttype->param != 2)
 {
  if (!this->nopostscript)
  {
   XtQueryGeometry(WIDGET(this->Frame5), NULL, &(this->geometry));
   /* unsensitivize */
   XtSetSensitive(WIDGET(this->Frame6), False);
   XtSetSensitive(WIDGET(this->Frame4), False);
   XtSetSensitive(WIDGET(this->Frame5), False);
   this->nopostscript = TRUE;
  }
 }
 else
 {
  if (this->nopostscript)
  {
   /* sensitivize */
   XtSetSensitive(WIDGET(this->Frame6), True);
   XtSetSensitive(WIDGET(this->Frame4), True);
   XtSetSensitive(WIDGET(this->Frame5), True);
   this->nopostscript = FALSE;
      
   XtConfigureWidget(WIDGET(this->Frame5), 
		     this->geometry.x, this->geometry.y,
		     this->geometry.width, this->geometry.height,
		     this->geometry.border_width);
  }
 } 
  
 F(this->Dialog).resetTraversal(this->Dialog);
}

/* ----------------------------------------------------------------- ** 
** cb_exporttype - ItemMenu callback for page format                     ** 
** ----------------------------------------------------------------- */
static void cb_fonttype(popup, this, fonttype)
c_ItemMenu *popup;
c_Box_print *this;
font_type_t *fonttype;
{
 Xc_TRACE(("cb_fonttype"));
  
 this->param->fonttype = fonttype->param;
  
 F(this->Dialog).resetTraversal(this->Dialog);
}

static void certify_size(this, Obj)
c_WTextField	*this;
c_Box_print	*Obj;
{
 coord_t	num;

 Xc_TRACE(("size"));
  
 num = F(this).get_value_in_unit(this);
 if (this == Obj->TFdocWidth)
  Xc_Table_Page_Format[Xc_CUSTOM_FORMAT].width = num;
 if (this == Obj->TFdocHeight)
  Xc_Table_Page_Format[Xc_CUSTOM_FORMAT].height = num;
 F(Obj->IMpageFormat).setSelection(Obj->IMpageFormat,
				   &Xc_Table_Page_Format[Xc_CUSTOM_FORMAT]);
 cb_format(Obj->IMpageFormat, Obj, &Xc_Table_Page_Format[Xc_CUSTOM_FORMAT]);

 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}












