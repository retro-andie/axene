/*
** ExportText.c for XQuad in FileSelector/
** File selector for text ExportText
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
** Started on  Sun Dec 15 17:10:45 1996 Robin Castanier
** Last update Sat Feb  7 16:10:09 1998 Emmanuel Paris
*/

#include "SpecialFileSelector.h"
#include "Grid.h"
#include "Frame.h"
#include "Export.h"
#include <Xm/DrawingA.h>

typedef struct {
		char *name;
		int num;
	       } eds_format_t;

boolean CreateExportTextSelector();
void DestroyExportTextSelector ___PROTO((void));
void ManageExportTextSelector ___PROTO((boolean doc_available));

static void cb_format ___PROTO((c_ItemMenu *menu, c_FileSelector *selector,
				eds_format_t *format));

c_FileSelector *ExportTextSelector = NULL;

typedef struct 
{
 c_Export	*export;
 boolean	doc_available;
 eds_format_t	*format;
 c_Grid	*grid;
 c_Frame	*frame;
 c_ItemMenu	*menu;
 Widget	wSep;
} eds_data_t;

ab_size_t	GlobAbSize;

boolean CreateExportTextSelector(parent, export, doc_available)
Widget	 parent;
c_Export *export;
boolean	 doc_available;
{
 int		 i;
 eds_data_t	 *mdata;
 Arg		 argsw[4];
 XmString       xmsBaratin;
 c_ManageWidget *manager;
  
 ExportTextSelector = 
  NEW(c_FileSelector)(parent, "ExportText", 500, 500);
 if(ExportTextSelector != NULL)
 {
  xmsBaratin = 
   F(GlobResources).getXmString(GlobResources, XcR_exportDocument);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(ExportTextSelector->w_Title, argsw, 1);
  XmStringFree(xmsBaratin);
      
  mdata = (eds_data_t *)Xc_malloc("eds data", sizeof(eds_data_t));
  ExportTextSelector->data = (void *)mdata;
  memset(mdata, 0, sizeof(eds_data_t));
  mdata->export = export;
  mdata->doc_available = doc_available;
      
  manager = MANAGER(ExportTextSelector->dialog);
      
  mdata->wSep = XtVaCreateWidget("", xmDrawingAreaWidgetClass,
				 WIDGET(ExportTextSelector), 
				 NULL);
  /*--- Grid ---*/
  mdata->grid = 
   NEW(c_Grid)(WIDGET(ExportTextSelector), manager, "grid", TRUE);
      
  /*--- Menu ---*/
  mdata->format = 
   (eds_format_t *)Xc_malloc("eds format", sizeof(eds_format_t) * 
			     export->count_export_format);
  for(i = 0; i < export->count_export_format; i++)
  {
   mdata->format[i].name = export->export_format[i]->id;
   mdata->format[i].num = i;
  }
  mdata->menu = 
   NEW(c_ItemMenu)(WIDGET(mdata->grid), manager, "Format", 0, 0);
      
  F(mdata->menu).set(mdata->menu, TRUE,
		     offsetof(eds_format_t, name), NULL, NULL);
  F(mdata->menu).setCallback(mdata->menu, (void *)cb_format, 
			     ExportTextSelector);
  F(mdata->menu).addItems(mdata->menu, XcIM_ARRAY,
			  export->count_export_format, mdata->format,
			  sizeof(eds_format_t));
  F(mdata->menu).setSelection(mdata->menu, 
			      &mdata->format[export->format_selected]);
      
  F(mdata->grid).addColumn(mdata->grid, XcG_BEST_SIZE,
			   WIDGET(mdata->menu), XcG_MAX_SIZE,
			   NULL);
      
  /*--- Frame ---*/
  mdata->frame = NEW(c_Frame)(WIDGET(mdata->grid), manager, 
			      "fFSITSetup", -1);
  F(mdata->grid).addColumn(mdata->grid, XcG_MAX_SIZE,
			   WIDGET(mdata->frame), XcG_MAX_SIZE,
			   NULL);
      
  GlobAbSize.dim_calc = FALSE;
      
  EXPORT_CREATE_ATTRIBS_BOX
   (export->export_format[export->format_selected])
    (export->export_format[export->format_selected], 
     WIDGET(mdata->frame), &GlobAbSize);
  *(export->export_format[export->format_selected]->box_created) = TRUE;
      
  EXPORT_MANAGE_ATTRIBS_BOX
   (export->export_format[export->format_selected])
    (export->export_format[export->format_selected], 
     doc_available, &GlobAbSize);
      
  F(ExportTextSelector->dialog).blockChild(ExportTextSelector->dialog, 
					   mdata->wSep, 10);
  return TRUE;
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** DestroyExportTextSelector - Destruction method                    ** 
** ----------------------------------------------------------------- */
void DestroyExportTextSelector()
{
 if(ExportTextSelector != NULL)
 {
  int		i;
  eds_data_t	*mdata;
  c_Export		*export;
    
  mdata = (eds_data_t *)ExportTextSelector->data;
  export = mdata->export;
    
  for(i = 0; i < export->count_export_format; i++)
  {
   if (*(export->export_format[i]->box_created))
   {
    EXPORT_DESTROY_ATTRIBS_BOX
     (export->export_format[i])(export->export_format[i]);
    *(export->export_format[i]->box_created) = FALSE;
   }
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
** ManageExportTextSelector - Manage method                      ** 
** ----------------------------------------------------------------- */

void ManageExportTextSelector(doc_available)
boolean doc_available;
{
 eds_data_t	*mdata;
 c_Export	*export;
  
 Xc_TRACE(("Manage Export Document Selector"));
 mdata = (eds_data_t *)ExportTextSelector->data;
 export = mdata->export;

 mdata->doc_available = doc_available;
 EXPORT_MANAGE_ATTRIBS_BOX
  (export->export_format[export->format_selected])
   (export->export_format[export->format_selected],
    mdata->doc_available, &GlobAbSize);/* this unmanage */
 EXPORT_MANAGE_ATTRIBS_BOX
  (export->export_format[export->format_selected])
   (export->export_format[export->format_selected],
    mdata->doc_available, &GlobAbSize);/* this manage */
}

/* ----------------------------------------------------------------- ** 
** cb_format - Format menu callback                                  ** 
** ----------------------------------------------------------------- */
static void cb_format(menu, selector, format)
c_ItemMenu *menu;
c_FileSelector *selector;
eds_format_t *format;
{
 eds_data_t	*mdata;
 c_Export	*export;
 int		num;
  
 Xc_TRACE(("cb_format, selected: %s", format->name));
 mdata = (eds_data_t *)ExportTextSelector->data;
 export = mdata->export;
  
 EXPORT_MANAGE_ATTRIBS_BOX
  (export->export_format[export->format_selected])
   (export->export_format[export->format_selected],
    mdata->doc_available, &GlobAbSize);
  
 mdata->export->format_selected = num = format->num;
 if (! *(export->export_format[num]->box_created))
 {
  EXPORT_CREATE_ATTRIBS_BOX
   (export->export_format[num])(export->export_format[num], 
				WIDGET(mdata->frame), &GlobAbSize);
  *(export->export_format[num]->box_created) = TRUE;
 }
  
 EXPORT_MANAGE_ATTRIBS_BOX
  (export->export_format[num])(export->export_format[num], 
			       mdata->doc_available, &GlobAbSize);
}











