/*
** ExportText.c for Xclamation in FileSelector/
** File selector for text ExportText
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
** Started on  Mon Jan  1 14:34:10 1996 Emmanuel Paris
** Last update Sun Jan 16 15:59:15 2000 Emmanuel Paris
*/

#include "FileSelector.h"
#include "ExportTextFileSelector.h"
#include <Xm/DrawingA.h>
#include "Resource.h"

extern c_Resource *GlobResources;

boolean CreateExportTextSelector();
void DestroyExportTextSelector ___PROTO((void));
void ManageExportTextSelector ___PROTO((void));

static void ExportTextCreateItem ___PROTO((c_FileSelector *selector,
					   ids_format_t *format));
static void ExportTextManageItem ___PROTO((c_FileSelector *selector,
					   ids_format_t *format));
static void ExportTextDestroyItem ___PROTO((c_FileSelector *selector,
					    ids_format_t *format));

static void cb_format ___PROTO((c_ItemMenu *menu, c_FileSelector *selector,
				ids_format_t *format));

static void toggle_rulers ___PROTO((Widget w, ids_data_t *mdata));


static void certify_frame_break ___PROTO((c_WTextField *this,
					  ids_data_t *mdata));
static void certify_carriage_return ___PROTO((c_WTextField *this,
					      ids_data_t *mdata));

static void cb_style ___PROTO((c_ItemMenu *menu, ids_data_t *mdata,
			       char **name));

static void certify_style_begin ___PROTO((c_WTextField *this,
					  ids_data_t *mdata));
static void certify_style_end ___PROTO((c_WTextField *this,
					ids_data_t *mdata));

c_FileSelector *ExportTextSelector = NULL;

static int resource[NB_IDS_ITEM] = 
{
 XcR_exportTextXcl, XcR_exportTextASCII, XcR_exportTextHTML
};

static char *filler = "Filler";
static char **pfiller = &filler;

boolean CreateExportTextSelector(parent)
Widget	 parent;
{
 int		 i;
 ids_data_t	 *mdata;
 Arg		 argsw[4];
 XmString       xmsBaratin;
 c_ManageWidget *manager;
 
 ExportTextSelector = 
  NEW(c_FileSelector)(parent, "ExportText", 600, 470);
 if (ExportTextSelector != NULL)
 {
  xmsBaratin = 
   F(GlobResources).getXmString(GlobResources, XcR_exportText);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(ExportTextSelector->w_Title, argsw, 1);
  XmStringFree(xmsBaratin);
      
  mdata = (ids_data_t *)Xc_malloc("ids data", sizeof(ids_data_t));
  ExportTextSelector->data = (void *)mdata;
  memset(mdata, 0, sizeof(ids_data_t));
      
  manager = MANAGER(ExportTextSelector->dialog);
	
  mdata->wSep = XtVaCreateWidget("", xmDrawingAreaWidgetClass,
				 WIDGET(ExportTextSelector),
				 NULL);
  F(ExportTextSelector->dialog).blockChild(ExportTextSelector->dialog, 
					   mdata->wSep, 10);
  /*--- Grid ---*/
  mdata->grid = 
   NEW(c_Grid)(WIDGET(ExportTextSelector), manager, "grid", TRUE);
      
  F(ExportTextSelector->dialog).blockChild(ExportTextSelector->dialog,
					   WIDGET(mdata->grid), 300);

  /*--- Menu ---*/
  mdata->format = mdata->selected_item =
   (ids_format_t *)Xc_malloc("ids format",
			     sizeof(ids_format_t) * NB_IDS_ITEM);
  for(i = 0; i < NB_IDS_ITEM; i++)
  {
   mdata->format[i].name = 
    F(GlobResources).getString(GlobResources, resource[i]);
   mdata->format[i].num = i;
   mdata->format[i].created = FALSE;
   mdata->format[i].managed = FALSE;
  }
  mdata->menu = 
   NEW(c_ItemMenu)(WIDGET(mdata->grid), manager, "Format", 0, 0);
      
  F(mdata->menu).set(mdata->menu, TRUE,
		     offsetof(ids_format_t, name), NULL, NULL);
  F(mdata->menu).setCallback(mdata->menu, (void *)cb_format, 
			     ExportTextSelector);
  F(mdata->menu).addItems(mdata->menu, XcIM_ARRAY,
			  NB_IDS_ITEM, mdata->format,
			  sizeof(ids_format_t));
  F(mdata->menu).setSelection(mdata->menu, mdata->format);
      
  F(mdata->grid).addColumn(mdata->grid, XcG_BEST_SIZE,
			   WIDGET(mdata->menu), XcG_MAX_SIZE,
			   NULL);
      
  /*--- Frame ---*/
  mdata->frame = NEW(c_Frame)(WIDGET(mdata->grid), manager, 
			      "fETSetup", -1);
      
  F(mdata->grid).addColumn(mdata->grid, XcG_MAX_SIZE,
			   WIDGET(mdata->frame), XcG_MAX_SIZE,
			   NULL);
  mdata->dim_calc = FALSE;
            
  ExportTextCreateItem(ExportTextSelector, mdata->format);
  ExportTextManageItem(ExportTextSelector, mdata->format);
      
  return TRUE;
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** DestroyExportTextSelector - Destruction method                ** 
** ----------------------------------------------------------------- */
void DestroyExportTextSelector()
{
 if(ExportTextSelector != NULL)
 {
  int		i;
  ids_data_t	*mdata;
  expt_HTML_t	*export;
      
  mdata = (ids_data_t *)ExportTextSelector->data;
      
  for(i = 0; i < NB_IDS_ITEM; i++)
  {
   if (mdata->format[i].created)
   {
    ExportTextDestroyItem(ExportTextSelector, &mdata->format[i]);
   }
  }
      
  export = &(export_text.expt_HTML);
  Xc_free(export->frame_break);
  Xc_free(export->carriage_return);
  for(i = 0; i < export->nb_style; i++)
  {
   Xc_free(export->style_name[i]);
   Xc_free(export->style_begin[i]);
   Xc_free(export->style_end[i]);
  }
  if (export->nb_style)
  {
   Xc_free(export->style_name);
   Xc_free(export->style_begin);
   Xc_free(export->style_end);
  }
      
  DELETE(c_Frame)(mdata->frame);
  DELETE(c_ItemMenu)(mdata->menu);
      
  DELETE(c_Grid)(mdata->grid);
  XtDestroyWidget(mdata->wSep);
  
  Xc_free(mdata->format);
  Xc_free(mdata);
      
  DELETE(c_FileSelector)(ExportTextSelector);
  ExportTextSelector = NULL;
 }
}

/* ----------------------------------------------------------------- ** 
** ManageExportTextSelector - Manage method                          ** 
** ----------------------------------------------------------------- */

void ManageExportTextSelector()
{
 ids_data_t	*mdata;
  
 Xc_TRACE(("Manage Export Text Selector"));
 mdata = (ids_data_t *)ExportTextSelector->data;

 /* this unmanage */
 ExportTextManageItem(ExportTextSelector, mdata->selected_item);
 /* this manage */
 ExportTextManageItem(ExportTextSelector, mdata->selected_item);
}

/* ----------------------------------------------------------------- ** 
** ExportTextCreateItem - Create item method                         ** 
** ----------------------------------------------------------------- */
static void ExportTextCreateItem(selector, format)
c_FileSelector *selector;
ids_format_t *format;
{
 ids_data_t	*mdata;
  
 Xc_TRACE(("Create Export Text Item: %d", format->num));
 mdata = (ids_data_t *)ExportTextSelector->data;
  
 switch(format->num)
 {
 case IDS_EXPORT_XCL:
  format->created = TRUE;
  mdata->formXCL = NEW(c_Form)(WIDGET(mdata->frame), NULL,
			       "foETXCL");
  break;
 case IDS_EXPORT_ASCII:
  format->created = TRUE;
  break;
 case IDS_EXPORT_HTML:
  format->created = TRUE;
  /*TOTO*/
   
  mdata->gridHTML = (c_Grid *)NEW(c_Grid)(WIDGET(mdata->frame), NULL,
					  "gETHTML", TRUE);
    
  mdata->tbHTMLRulers = 
   (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(mdata->gridHTML),
					 NULL, "tbETHTMLRulers", NULL);
  F(mdata->tbHTMLRulers).Add_Callback(mdata->tbHTMLRulers, 
				      XmNvalueChangedCallback,
				      toggle_rulers, mdata);
    
  mdata->lHTMLFrameBreak = (c_Label *)NEW(c_Label)
   (WIDGET(mdata->gridHTML), NULL, "lETHTMLFrameBreak");
    
  mdata->tfHTMLFrameBreak = 
   (c_WTextField *)NEW(c_WTextField)(WIDGET(mdata->gridHTML),
				     NULL,
				     "tfETHTMLFrameBreak",
				     TF_STRING,
				     0.0, 199.0,
				     "", NULL, 5);
    
  F(mdata->tfHTMLFrameBreak).add_callback
   (mdata->tfHTMLFrameBreak, (void *)certify_frame_break, mdata);
    
  mdata->lHTMLCarrierReturn = (c_Label *)NEW(c_Label)
   (WIDGET(mdata->gridHTML), NULL, "lETHTMLCarrierReturn");
  mdata->tfHTMLCarrierReturn = 
   (c_WTextField *)NEW(c_WTextField)(WIDGET(mdata->gridHTML),
				     NULL,
				     "tfETHTMLCarrierReturn",
				     TF_STRING,
				     0.0, 199.0,
				     "", NULL, 5);
    
  F(mdata->tfHTMLCarrierReturn).add_callback
   (mdata->tfHTMLCarrierReturn, (void *)certify_carriage_return, mdata);
    
  mdata->sepHTML1 = XmCreateSeparatorGadget(WIDGET(mdata->gridHTML),
					    "___sepHTML1___", NULL, 0);
    

  mdata->lHTMLStyles = (c_Label *)NEW(c_Label)
   (WIDGET(mdata->gridHTML), NULL, "lETHTMLStyles");

  mdata->imHTMLStyles = 
   (c_ItemMenu *)NEW(c_ItemMenu)(WIDGET(mdata->gridHTML),
				 NULL,
				 "imHTMLStyles", 0, 0);
  F(mdata->imHTMLStyles).setCallback(mdata->imHTMLStyles,
				     (void *)cb_style, mdata);
  F(mdata->imHTMLStyles).set(mdata->imHTMLStyles, TRUE, 0, NULL, NULL);
    
  F(mdata->imHTMLStyles).addItems(mdata->imHTMLStyles, XcIM_ARRAY,
				  1, pfiller, sizeof(char *));
  mdata->lHTMLStyleBegin = (c_Label *)NEW(c_Label)
   (WIDGET(mdata->gridHTML), NULL, "lETHTMLStyleBegin");
  mdata->tfHTMLStyleBegin = 
   (c_WTextField *)NEW(c_WTextField)(WIDGET(mdata->gridHTML),
				     NULL,
				     "tfETHTMLStyleBegin",
				     TF_STRING,
				     0.0, 199.0,
				     "", NULL, 5);
  F(mdata->tfHTMLStyleBegin).add_callback
   (mdata->tfHTMLStyleBegin, (void *)certify_style_begin, mdata);
  mdata->lHTMLStyleEnd = (c_Label *)NEW(c_Label)
   (WIDGET(mdata->gridHTML), NULL, "lETHTMLStyleEnd");
  mdata->tfHTMLStyleEnd = 
   (c_WTextField *)NEW(c_WTextField)(WIDGET(mdata->gridHTML),
				     NULL,
				     "tfETHTMLStyleEnd",
				     TF_STRING,
				     0.0, 199.0,
				     "", NULL, 5);
  F(mdata->tfHTMLStyleEnd).add_callback
   (mdata->tfHTMLStyleEnd, (void *)certify_style_end, mdata);
        
  F(mdata->gridHTML).addColumn(mdata->gridHTML, XcG_MAX_SIZE,
			       WIDGET(mdata->tbHTMLRulers), XcG_MAX_SIZE,
			       NULL);
  F(mdata->gridHTML).addColumn(mdata->gridHTML, XcG_BEST_SIZE,
			       WIDGET(mdata->lHTMLFrameBreak), 120,
			       WIDGET(mdata->tfHTMLFrameBreak), XcG_MAX_SIZE,
			       NULL);
  F(mdata->gridHTML).addColumn(mdata->gridHTML, XcG_BEST_SIZE,
			       WIDGET(mdata->lHTMLCarrierReturn), 120,
			       WIDGET(mdata->tfHTMLCarrierReturn),
			       XcG_MAX_SIZE,
			       NULL);
  F(mdata->gridHTML).addColumn(mdata->gridHTML, XcG_MAX_SIZE,
			       mdata->sepHTML1, XcG_MAX_SIZE,
			       NULL);
  F(mdata->gridHTML).addColumn(mdata->gridHTML, XcG_MAX_SIZE,
			       WIDGET(mdata->lHTMLStyles), XcG_MAX_SIZE,
			       NULL);
  F(mdata->gridHTML).addColumn(mdata->gridHTML, 50,
			       WIDGET(mdata->imHTMLStyles), XcG_MAX_SIZE,
			       NULL);
  F(mdata->gridHTML).addColumn(mdata->gridHTML, XcG_BEST_SIZE,
			       WIDGET(mdata->lHTMLStyleBegin), 120,
			       WIDGET(mdata->tfHTMLStyleBegin), XcG_MAX_SIZE,
			       NULL);
  F(mdata->gridHTML).addColumn(mdata->gridHTML, XcG_BEST_SIZE,
			       WIDGET(mdata->lHTMLStyleEnd), 120,
			       WIDGET(mdata->tfHTMLStyleEnd), XcG_MAX_SIZE,
			       NULL);
    
  if (mdata->dim_calc)
  {
   XtVaSetValues(WIDGET(mdata->gridHTML), XmNwidth, mdata->width,
		 XmNheight, mdata->height, NULL);
  }
    
  XtManageChild(WIDGET(mdata->tbHTMLRulers));
  XtManageChild(WIDGET(mdata->lHTMLFrameBreak));
  XtManageChild(WIDGET(mdata->tfHTMLFrameBreak));
  XtManageChild(WIDGET(mdata->lHTMLCarrierReturn));
  XtManageChild(WIDGET(mdata->tfHTMLCarrierReturn));
  XtManageChild(mdata->sepHTML1);
  XtManageChild(WIDGET(mdata->lHTMLStyles));
  XtManageChild(WIDGET(mdata->imHTMLStyles));
  XtManageChild(WIDGET(mdata->lHTMLStyleBegin));
  XtManageChild(WIDGET(mdata->tfHTMLStyleBegin));
  XtManageChild(WIDGET(mdata->lHTMLStyleEnd));
  XtManageChild(WIDGET(mdata->tfHTMLStyleEnd));

  break;
 }
}

/* ----------------------------------------------------------------- ** 
** ExportTextManageItem - Manage item method                         ** 
** ----------------------------------------------------------------- */
static void ExportTextManageItem(selector, format)
c_FileSelector *selector;
ids_format_t *format;
{
 ids_data_t	*mdata;
 expt_HTML_t	*export;

 Xc_TRACE(("Manage Export Text Item: %d", format->num));
 mdata = (ids_data_t *)ExportTextSelector->data;
  
 if (format->managed && !mdata->dim_calc)
 {
  XtVaGetValues(WIDGET(mdata->formXCL), XmNwidth, &mdata->width,
		XmNheight, &mdata->height, NULL);
    
  Xc_TRACE(("Form size: %d x %d", mdata->width, mdata->height));
  mdata->dim_calc = TRUE;
  
  DELETE(c_Form)(mdata->formXCL);
  mdata->formXCL = NULL;
 }

 mdata->selected_item = format;
 switch(format->num)
 {
 case IDS_EXPORT_XCL:
  F(ExportTextSelector).addPattern(ExportTextSelector, TRUE, "*.xct");
  if (mdata->formXCL)
  {
   if (format->managed)
    XtUnmanageChild(WIDGET(mdata->formXCL));
   else
    XtManageChild(WIDGET(mdata->formXCL));
  }
  break;
 case IDS_EXPORT_ASCII:
  F(ExportTextSelector).addPattern(ExportTextSelector, TRUE, NULL);
  break;
 case IDS_EXPORT_HTML:
  F(ExportTextSelector).addPattern(ExportTextSelector, TRUE, "*.html");
  F(ExportTextSelector).addPattern(ExportTextSelector, FALSE, "*.htm");
  export = &(export_text.expt_HTML);
    
  if (format->managed)
  {
   XtUnmanageChild(WIDGET(mdata->gridHTML));
  }
  else
  {
   F(mdata->tbHTMLRulers).Set(mdata->tbHTMLRulers, 
			      export->rulers? True : False, False);
   F(mdata->tfHTMLFrameBreak).set(mdata->tfHTMLFrameBreak, 
				  export->frame_break);
   F(mdata->tfHTMLCarrierReturn).set(mdata->tfHTMLCarrierReturn, 
				     export->carriage_return);
	
   if (mdata->imHTMLStyles->item_base->item != export->style_name)
   {
    F(mdata->imHTMLStyles).deleteAllItems(mdata->imHTMLStyles);
    F(mdata->imHTMLStyles).addItems(mdata->imHTMLStyles, XcIM_ARRAY,
				    export->nb_style, export->style_name,
				    sizeof(char *));
    cb_style(mdata->imHTMLStyles, mdata, export->style_name);
   }
   XtManageChild(WIDGET(mdata->gridHTML));
   F(mdata->gridHTML).callResize(mdata->gridHTML);
  }
    
  break;
 }
 format->managed ^= TRUE;
}

/* ----------------------------------------------------------------- ** 
** ExportTextDestroyItem - Destroy item method                       ** 
** ----------------------------------------------------------------- */
static void ExportTextDestroyItem(selector, format)
c_FileSelector *selector;
ids_format_t *format;
{
 ids_data_t	*mdata;

 Xc_TRACE(("Destroy Export Text Item: %d", format->num));
 mdata = (ids_data_t *)ExportTextSelector->data;
  
 switch(format->num)
 {
 case IDS_EXPORT_XCL:
  if (mdata->formXCL)
   DELETE(c_Form)(mdata->formXCL);
  break;
 case IDS_EXPORT_ASCII:
  break;
 case IDS_EXPORT_HTML:    
  DELETE(c_Label)(mdata->lHTMLFrameBreak);
  DELETE(c_Label)(mdata->lHTMLCarrierReturn);
  DELETE(c_Label)(mdata->lHTMLStyles);
  DELETE(c_Label)(mdata->lHTMLStyleBegin);
  DELETE(c_Label)(mdata->lHTMLStyleEnd);
  DELETE(c_ToggleButton)(mdata->tbHTMLRulers);
  DELETE(c_WTextField)(mdata->tfHTMLFrameBreak);
  DELETE(c_WTextField)(mdata->tfHTMLCarrierReturn);
  DELETE(c_WTextField)(mdata->tfHTMLStyleBegin);
  DELETE(c_WTextField)(mdata->tfHTMLStyleEnd);
  XtDestroyWidget(mdata->sepHTML1);
  DELETE(c_ItemMenu)(mdata->imHTMLStyles);
  DELETE(c_Grid)(mdata->gridHTML);

  break;
 }
}

/* ----------------------------------------------------------------- ** 
** cb_format - Format menu callback                                  ** 
** ----------------------------------------------------------------- */
static void cb_format(menu, selector, format)
c_ItemMenu *menu;
c_FileSelector *selector;
ids_format_t *format;
{
 ids_data_t	*mdata;
  
 Xc_TRACE(("cb_format, selected: %s", format->name));
 mdata = (ids_data_t *)ExportTextSelector->data;
  
 ExportTextManageItem(ExportTextSelector, mdata->selected_item);
  
 if (!format->created)
 {
  ExportTextCreateItem(ExportTextSelector, format);
 }
 ExportTextManageItem(ExportTextSelector, format);
}

/* ----------------------------------------------------------------- ** 
** toggle_ruler - ruler toggle callback                              ** 
** ----------------------------------------------------------------- */
static void toggle_rulers(w, mdata)
Widget w;
ids_data_t *mdata;
{
 Xc_TRACE(("toggle rulers"));
 export_text.expt_HTML.rulers ^= TRUE;
}

/* ----------------------------------------------------------------- ** 
** certify_frame_break - Style certify_frame_break callback          ** 
** ----------------------------------------------------------------- */
static void certify_frame_break(this, mdata)
c_WTextField *this;
ids_data_t *mdata;
{
 char	*str;
  
 Xc_TRACE(("certify_frame_break"));
 if((str = F(this).get(this)) != NULL)
 {
  Xc_free(export_text.expt_HTML.frame_break);
  export_text.expt_HTML.frame_break = str;
 }
}

/* ----------------------------------------------------------------- ** 
** certify_carriage_return - Style certify_carriage_return callback  ** 
** ----------------------------------------------------------------- */
static void certify_carriage_return(this, mdata)
c_WTextField *this;
ids_data_t *mdata;
{
 char	*str;
  
 Xc_TRACE(("certify_carriage_return"));
 if((str = F(this).get(this)) != NULL)
 {
  Xc_free(export_text.expt_HTML.carriage_return);
  export_text.expt_HTML.carriage_return = str;
 }
}

/* ----------------------------------------------------------------- ** 
** cb_style - Style callback                                         ** 
** ----------------------------------------------------------------- */
static void cb_style(menu, mdata, name)
c_ItemMenu *menu;
ids_data_t *mdata;
char **name;
{
 int		num_style;
 expt_HTML_t	*export;
  
 Xc_TRACE(("style name: %s", *name));
 Xc_TRACE(("style number: %d", name - export_text.expt_HTML.style_name));

 export = &(export_text.expt_HTML);
 num_style = name - export_text.expt_HTML.style_name;
  
 F(mdata->tfHTMLStyleBegin).set(mdata->tfHTMLStyleBegin, 
				export->style_begin[num_style]);
 F(mdata->tfHTMLStyleEnd).set(mdata->tfHTMLStyleEnd, 
			      export->style_end[num_style]);
 export->selected_style = num_style;
}

/* ----------------------------------------------------------------- ** 
** certify_style_begin - Style certify_style_begin callback          ** 
** ----------------------------------------------------------------- */
static void certify_style_begin(this, mdata)
c_WTextField *this;
ids_data_t *mdata;
{
 char	*str;
  
 Xc_TRACE(("certify_style_begin"));
 if((str = F(this).get(this)) != NULL)
 {
  Xc_free(export_text.expt_HTML.style_begin
	  [export_text.expt_HTML.selected_style]);
  export_text.expt_HTML.style_begin
   [export_text.expt_HTML.selected_style] = str;
 }
}

/* ----------------------------------------------------------------- ** 
** certify_style_end - Style certify_style_end callback              ** 
** ----------------------------------------------------------------- */
static void certify_style_end(this, mdata)
c_WTextField *this;
ids_data_t *mdata;
{
 char	*str;
  
 Xc_TRACE(("certify_style_end"));
 if((str = F(this).get(this)) != NULL)
 {
  Xc_free(export_text.expt_HTML.style_end
	  [export_text.expt_HTML.selected_style]);
  export_text.expt_HTML.style_end
   [export_text.expt_HTML.selected_style] = str;
 }
}




