/*
** ImportDocument.c for XQuad and XAllWrite in FileSelector/
** File selector for text ImportDocument
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
** Started on  Fri Dec 16 14:34:10 1994 Stéphane Boisson
** Last update Mon Aug 10 14:14:05 1998 Emmanuel Paris
*/

#include "SpecialFileSelector.h"
#include "Grid.h"
#include "Frame.h"
#include "Import.h"
#include <Xm/DrawingA.h>

typedef struct 
{
 char *name;
 int num;
} ids_format_t;

boolean CreateImportDocumentSelector();
void DestroyImportDocumentSelector ___PROTO((void));
void ManageImportDocumentSelector ___PROTO((boolean doc_available));

static void cb_format ___PROTO((c_ItemMenu *menu, c_FileSelector *selector,
				ids_format_t *format));

c_FileSelector *ImportDocumentSelector = NULL;

typedef struct 
{
 c_Import	*import;
 boolean	doc_available;
 ids_format_t	*format;
 c_Grid	*grid;
 c_Frame	*frame;
 c_ItemMenu	*menu;
 Widget	wSep;
} ids_data_t;

boolean CreateImportDocumentSelector(parent, import, doc_available)
Widget	 parent;
c_Import *import;
boolean	 doc_available;
{
 int		 i;
 ids_data_t	 *mdata;
 Arg		 argsw[4];
 XmString       xmsBaratin;
 c_ManageWidget *manager;
 
 ImportDocumentSelector = 
  NEW(c_FileSelector)(parent, "ImportDocument", 500, 500);
 if(ImportDocumentSelector != NULL)
 {
  xmsBaratin = 
   F(GlobResources).getXmString(GlobResources, XcR_importDocument);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(ImportDocumentSelector->w_Title, argsw, 1);
  XmStringFree(xmsBaratin);
      
  mdata = (ids_data_t *)Xc_malloc("ids data", sizeof(ids_data_t));
  ImportDocumentSelector->data = (void *)mdata;
  memset(mdata, 0, sizeof(ids_data_t));
  mdata->import = import;
  mdata->doc_available = doc_available;
      
  manager = MANAGER(ImportDocumentSelector->dialog);
	
  mdata->wSep = XtVaCreateWidget("", xmDrawingAreaWidgetClass,
				 WIDGET(ImportDocumentSelector), 
				 NULL);
  /*--- Grid ---*/
  mdata->grid = 
   NEW(c_Grid)(WIDGET(ImportDocumentSelector), manager, "grid", TRUE);
      
  /*--- Menu ---*/
  mdata->format = 
   (ids_format_t *)Xc_malloc("ids format", sizeof(ids_format_t) * 
			     import->count_import_format);
  for(i = 0; i < import->count_import_format; i++)
  {
   mdata->format[i].name = import->import_format[i]->id;
   mdata->format[i].num = i;
  }
  mdata->menu = 
   NEW(c_ItemMenu)(WIDGET(mdata->grid), manager, "Format", 0, 0);
      
  F(mdata->menu).set(mdata->menu, TRUE,
		     offsetof(ids_format_t, name), NULL, NULL);
  F(mdata->menu).setCallback(mdata->menu, (void *)cb_format, 
			     ImportDocumentSelector);
  F(mdata->menu).addItems(mdata->menu, XcIM_ARRAY,
			  import->count_import_format, mdata->format,
			  sizeof(ids_format_t));
  F(mdata->menu).setSelection(mdata->menu, 
			      &mdata->format[import->format_selected]);
      
  F(mdata->grid).addColumn(mdata->grid, XcG_BEST_SIZE,
			   WIDGET(mdata->menu), XcG_MAX_SIZE,
			   NULL);
      
  /*--- Frame ---*/
  mdata->frame = NEW(c_Frame)(WIDGET(mdata->grid), manager, 
			      "fFSITSetup", -1);
  F(mdata->grid).addColumn(mdata->grid, XcG_MAX_SIZE,
			   WIDGET(mdata->frame), XcG_MAX_SIZE,
			   NULL);
  IMPORT_CREATE_ATTRIBS_BOX
   (import->import_format[import->format_selected])
    (import->import_format[import->format_selected], 
     WIDGET(mdata->frame));
  *(import->import_format[import->format_selected]->box_created) = TRUE;
  IMPORT_MANAGE_ATTRIBS_BOX
   (import->import_format[import->format_selected])
    (import->import_format[import->format_selected], doc_available);
      
  F(ImportDocumentSelector->dialog).blockChild(ImportDocumentSelector->dialog, 
					       mdata->wSep, 10);
      
  return TRUE;
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** DestroyImportDocumentSelector - Destruction method                ** 
** ----------------------------------------------------------------- */
void DestroyImportDocumentSelector()
{
 if (ImportDocumentSelector != NULL)
 {
  int		i;
  ids_data_t	*mdata;
  c_Import		*import;
    
  mdata = (ids_data_t *)ImportDocumentSelector->data;
  import = mdata->import;
    
  for(i = 0; i < import->count_import_format; i++)
  {
   if (*(import->import_format[i]->box_created))
   {
    IMPORT_DESTROY_ATTRIBS_BOX
     (import->import_format[i])(import->import_format[i]);
    *(import->import_format[i]->box_created) = FALSE;
   }
  }
  DELETE(c_Frame)(mdata->frame);
  DELETE(c_ItemMenu)(mdata->menu);
    
  DELETE(c_Grid)(mdata->grid);
  XtDestroyWidget(mdata->wSep);
  Xc_free(mdata->format);
  Xc_free(mdata);
    
  DELETE(c_FileSelector)(ImportDocumentSelector);
  ImportDocumentSelector = NULL;
 }
}

/* ----------------------------------------------------------------- ** 
** ManageImportDocumentSelector - Manage method                      ** 
** ----------------------------------------------------------------- */

void ManageImportDocumentSelector(doc_available)
boolean doc_available;
{
 ids_data_t	*mdata;
 c_Import	*import;
  
 Xc_TRACE(("Manage Import Document Selector"));
 mdata = (ids_data_t *)ImportDocumentSelector->data;
 import = mdata->import;

 mdata->doc_available = doc_available;
 IMPORT_MANAGE_ATTRIBS_BOX
  (import->import_format[import->format_selected])
   (import->import_format[import->format_selected],
    mdata->doc_available);	/* this unmanage */
 IMPORT_MANAGE_ATTRIBS_BOX
  (import->import_format[import->format_selected])
   (import->import_format[import->format_selected],
    mdata->doc_available);	/* this manage */
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
 c_Import	*import;
 int		num;
  
 Xc_TRACE(("cb_format, selected: %s", format->name));
 mdata = (ids_data_t *)ImportDocumentSelector->data;
 import = mdata->import;
  
 IMPORT_MANAGE_ATTRIBS_BOX
  (import->import_format[import->format_selected])
   (import->import_format[import->format_selected],
    mdata->doc_available);
  
 mdata->import->format_selected = num = format->num;
 if (! *(import->import_format[num]->box_created))
 {
  IMPORT_CREATE_ATTRIBS_BOX
   (import->import_format[num])(import->import_format[num], 
				WIDGET(mdata->frame));
  *(import->import_format[num]->box_created) = TRUE;
 }
 IMPORT_MANAGE_ATTRIBS_BOX
  (import->import_format[num])(import->import_format[num], 
			       mdata->doc_available);
}
