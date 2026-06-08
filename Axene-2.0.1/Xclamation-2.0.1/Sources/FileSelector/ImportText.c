/*
** ImportText.c for Xclamation in FileSelector/
** File selector for text import
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
** Last update Wed Aug 11 00:20:02 1999 Emmanuel Paris
*/

#include "FileSelector.h"
#include "Grid.h"
#include "Frame.h"
/*#include <Xm/SeparatoG.h>*/
#include <Xm/LabelG.h>
#include <Xm/DrawingA.h>
#include "Resource.h"
extern c_Resource *GlobResources;

typedef struct 
{
 char *name;
 Widget w;
 void *data;
} import_format_t;

static import_format_t import_XClamation = {"Xclamation", NULL, NULL};
static import_format_t import_ASCII = {"ASCII", NULL, (void *)1};
/*static import_format_t import_Xmw = {"Xmegawriter", NULL, (void *)2};*/

static import_format_t *formats[] =
{
 &import_XClamation,
 &import_ASCII  
  /*&import_Xmw,*/
};

static void cb_format ___PROTO((c_ItemMenu *menu, c_FileSelector *selector,
				import_format_t *format));

c_FileSelector *ImportTextSelector = NULL;
static c_Grid *grid;
static c_Frame *frame;
static c_ItemMenu *menu;
static Widget	wSep;

boolean CreateImportTextSelector(parent)
Widget parent;
{
 Arg		 argsw[4];
 XmString       xmsBaratin;
 
 ImportTextSelector = NEW(c_FileSelector)(parent, "textImport", 500, 500);
 xmsBaratin = F(GlobResources).getXmString(GlobResources,
					   XcR_loadText);
 XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
 XtSetValues(ImportTextSelector->w_Title, argsw, 1);
 XmStringFree(xmsBaratin);

 if(ImportTextSelector != NULL)
 {
  c_ManageWidget *manager;

  ImportTextSelector->data = NULL;
  manager = MANAGER(ImportTextSelector->dialog);
  menu = NULL;
  grid = NULL;
  frame = NULL;
  do 
  {
   Widget w;
      
   /*	w = XtVaCreateWidget("sep", xmSeparatorGadgetClass,
	WIDGET(ImportTextSelector),
	XmNorientation, XmVERTICAL, NULL);
	F(ImportTextSelector->dialog).blockChild(ImportTextSelector->dialog,
	w, -1);
	F(manager).Add_child(manager, w, WIDGET(ImportTextSelector));
	*/	
   /*--- Grid ---*/
   wSep = XtVaCreateWidget("", xmDrawingAreaWidgetClass,
			   WIDGET(ImportTextSelector), 
			   NULL);

   grid = NEW(c_Grid)(WIDGET(ImportTextSelector), manager, "grid", TRUE);
   if(grid == NULL) break;

   /*--- Label ---*/
   w = XtVaCreateWidget("Format", xmLabelGadgetClass, WIDGET(grid), NULL);
   F(manager).Add_child(manager, w, WIDGET(grid));

   /*--- Menu ---*/
   menu = NEW(c_ItemMenu)(WIDGET(grid), manager, "format", 0, 0);
   if(menu == NULL) break;
   F(menu).set(menu, TRUE, offsetof(import_format_t, name), NULL, NULL);
   F(menu).setCallback(menu, (void *)cb_format, ImportTextSelector);
   F(menu).addItems(menu, XcIM_POINTER_ARRAY,
		    sizeof(formats) / sizeof(import_format_t *),
		    formats);

   F(grid).addColumn(grid, XcG_BEST_SIZE,
		     w, XcG_BEST_SIZE,
		     WIDGET(menu), XcG_MAX_SIZE,
		     NULL);

   /*--- Frame ---*/
   frame = NEW(c_Frame)(WIDGET(grid), manager, "Configuration", -1);
   if(frame == NULL) break;
   F(grid).addColumn(grid, XcG_MAX_SIZE,
		     WIDGET(frame), XcG_MAX_SIZE,
		     NULL);
   F(ImportTextSelector->dialog).blockChild(ImportTextSelector->dialog, 
					    wSep, 10);
   return TRUE;
  } while(0);

  if(grid == NULL)
  {
   if(frame != NULL) DELETE(c_Frame)(frame);
   if(menu != NULL) DELETE(c_ItemMenu)(menu);
   DELETE(c_Grid)(grid);
  }
  DELETE(c_FileSelector)(ImportTextSelector);
  ImportTextSelector = NULL;
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** DestroyImportTextSelector - Destruction method                    ** 
** ----------------------------------------------------------------- */
void DestroyImportTextSelector()
{
 if(ImportTextSelector != NULL)
 {
  if(grid != NULL)
  {
   if(frame != NULL) DELETE(c_Frame)(frame);
   if(wSep != NULL) XtDestroyWidget(wSep);
   if(menu != NULL) DELETE(c_ItemMenu)(menu);
   DELETE(c_Grid)(grid);
  }
  DELETE(c_FileSelector)(ImportTextSelector);
  ImportTextSelector = NULL;
 }
}


/* ----------------------------------------------------------------- ** 
** cb_format - Format menu callback                                  ** 
** ----------------------------------------------------------------- */
static void cb_format(menu, selector, format)
c_ItemMenu *menu;
c_FileSelector *selector;
import_format_t *format;
{
 Xc_TRACE(("cb_format"));

 selector->data = format->data;
}

