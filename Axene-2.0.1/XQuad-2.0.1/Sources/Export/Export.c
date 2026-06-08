/*
** Export.c for XQuad in Export/
** Object to export various spreadsheet file format
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
** Started on  Sun Dec 15 17:18:33 1996 Robin Castanier
** Last update Mon Mar 23 14:51:16 1998 Emmanuel Paris
*/

#include "Export.h"
#include "ExportP.h"
#include "SpecialFileSelector.h"
#include "DocumentList.h"
#include "WinManager.h"
#include "DocIcon.h"
#include "Resource.h"
#include "RowColumn.h"
#include "AlertBox.h"
#include <X11/Xlib.h>

extern Widget wMainWindow;
extern c_DocumentList	*DocList;
extern c_WinManager	*Gwm;
extern c_RowColumn      *MainDocument;
extern c_Resource	*GlobResources;
extern c_DocIcon	*GlobDocIcon;

static void *cons_Export();
static void dest_Export();
static void *copy_Export();
static void export ___PROTO((c_Export *This, c_Tableau *current_sheet));

sf_Export fc_Export=
{
 cons_Export,
 dest_Export,
 copy_Export,
 export
};

static void *cons_Export()
{
 int		i;
 c_Export	*This;

 if ((This=(c_Export *)Xc_malloc("Export",sizeof(c_Export)))==NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f= &fc_Export;  
  
 i=0;
 while(export_init[i].export_methods != NULL)
  i++;
  
 This->count_export_format = i; 
 This->format_selected = 0;
 This->export_format = 
  (c_Export_Std **)Xc_malloc("c_ExpStd **", sizeof(c_Export_Std *)*i);
  
 for(i = 0; i < This->count_export_format; i++)
 {
  This->export_format[i] = 
   (c_Export_Std *)((*export_init[i].export_methods).
		    cons(export_init[i].export_init_parameter));
 }
 return This;
}

static void dest_Export(This)
c_Export	*This;
{
 int	i;
  
 for(i = 0; i < This->count_export_format; i++)
 {
  EXPORT_DELETE(This->export_format[i])(This->export_format[i]);
 }
 Xc_free(This->export_format);
 Xc_free(This);
}

static void *copy_Export(This)
c_Export	*This;
{
 Xc_TRACE(("COPY Export Object not done: normally not usefull."));
 return NULL;
}


static void export(This, current_sheet)
c_Export	*This;
c_Tableau	*current_sheet;
{
 error		save_error;
 char		*filename;
    
 This->current_sheet = current_sheet;
    
 if( ExportTextSelector == NULL)
 {
  if (CreateExportTextSelector(wMainWindow, This, current_sheet!=NULL) 
      == FALSE)
   return;
 }
 else
 {
  ManageExportTextSelector(current_sheet!=NULL);
 }
  
 if((filename = F(ExportTextSelector).
     getFile(ExportTextSelector)) == NULL)
  return;
  
 Xc_TRACE(("filename: %s format selected: %d", filename, 
	   This->format_selected));

 This->export_format[This->format_selected]->filename = filename;

  
 save_error = EXPORT_SAVE(This->export_format[This->format_selected])
  (This->export_format[This->format_selected], current_sheet);

 switch(save_error)
 {
 case XC_NO_ERROR:
  break;
 case XC_MEMORY_ALLOCATION:
  Xc_ErrorAlert("memory exhauted");
  break;
 case XC_NAME_ALREADY_EXIST:
  Xc_ErrorAlert("file already exist");
  break;
 default:
  Xc_ErrorAlert("unknow error");
  break;
 }
  
  
 Xc_free(filename);  
}



