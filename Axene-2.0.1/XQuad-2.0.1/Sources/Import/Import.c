/*
** Import.c for XQuad in Import/
** Object to import various spreadsheet file format
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
** Started on  Wed Feb 22 16:58:39 1995 Emmanuel Paris
** Last update Thu Dec 19 12:49:45 1996 Emmanuel Paris
*/

#include "Import.h"
#include "ImportP.h"
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

static void *cons_Import();
static void dest_Import();
static void *copy_Import();
static void import ___PROTO((c_Import *This, c_Tableau *current_sheet));

sf_Import fc_Import=
 {
  cons_Import,
  dest_Import,
  copy_Import,
  import
};

static void *cons_Import()
{
  int		i;
  c_Import	*This;

  if ((This=(c_Import *)Xc_malloc("Import",sizeof(c_Import)))==NULL)
  {
    Xc_FATAL(("Can't build this object: memory allocation error"));
  }
  This->f= &fc_Import;  
  
  i=0;
  while(import_init[i].import_methods != NULL)
    i++;
  
  This->count_import_format = i; 
  This->format_selected = 0;
  This->import_format = 
    (c_Import_Std **)Xc_malloc("c_ImpStd **", sizeof(c_Import_Std *)*i);
  
  for(i = 0; i < This->count_import_format; i++)
  {
    This->import_format[i] = 
      (c_Import_Std *)((*import_init[i].import_methods).
		       cons(import_init[i].import_init_parameter));
  }
  return This;
}

static void dest_Import(This)
c_Import	*This;
{
  int	i;
  
  for(i = 0; i < This->count_import_format; i++)
  {
    IMPORT_DELETE(This->import_format[i])(This->import_format[i]);
  }
  Xc_free(This->import_format);
  Xc_free(This);
}

static void *copy_Import(This)
c_Import	*This;
{
  Xc_TRACE(("COPY Import Object not done: normally not usefull."));
  return NULL;
}


static void import(This, current_sheet)
c_Import	*This;
c_Tableau	*current_sheet;
{
  int		pos;
  boolean	new_sheet;
  c_Tableau	*sheet;
  error		load_error;
  char		*filename, *doc_name, *base_name;
    
  This->current_sheet = current_sheet;
    
  if( ImportDocumentSelector == NULL)
    {
      if (CreateImportDocumentSelector(wMainWindow, This, current_sheet!=NULL) 
	  == FALSE)
	return;
    }
  else
    {
      ManageImportDocumentSelector(current_sheet!=NULL);
    }
  
  if((filename = F(ImportDocumentSelector).
      getFile(ImportDocumentSelector)) == NULL)
    return;
  
  Xc_TRACE(("filename: %s format selected: %d", filename, 
	    This->format_selected));
  if (filename[strlen(filename)-1] == '/' || !IsFileExist(filename))
  {
    Xc_ErrorAlert2(F(GlobResources).getXmString(GlobResources, XcR_loadError));
    Xc_free(filename);
    return;
  }
  
  This->import_format[This->format_selected]->filename = filename;
  if ( IMPORT_TEST(This->import_format[This->format_selected])
      (This->import_format[This->format_selected]) == FALSE)
    {
      Xc_ErrorAlert("wrong file format");
      Xc_free(filename);
      return;
    }
  
  if (!current_sheet || 
      IMPORT_CREATE_NEW_SHEET(This->import_format[This->format_selected])
      (This->import_format[This->format_selected]) == TRUE)
    {
      new_sheet = TRUE;
      sheet = (c_Tableau *)NEW(c_Tableau)();
    }
  else
    {
      new_sheet = FALSE;
      sheet = current_sheet;
    }
  
  load_error = IMPORT_LOAD(This->import_format[This->format_selected])
    (This->import_format[This->format_selected], sheet);

  switch(load_error)
    {
    case XC_NO_ERROR:
      break;
    case XC_MEMORY_ALLOCATION:
      Xc_ErrorAlert("memory exhauted");
      break;
    case XC_FILE_NOT_EXIST:
      Xc_ErrorAlert("file doesn't exist");
      break;
    case XC_FILE_TRUNCATED:
      Xc_ErrorAlert("file truncated");
      break;
    case XC_FILE_FORMAT_ERROR:
      Xc_ErrorAlert("file format error");
      break;
    case XC_FILE_UNKNOW_FORMAT:
      Xc_ErrorAlert("unknow file format");
      break;
    default:
      Xc_ErrorAlert("unknow error");
      break;
    }
  
  if (load_error != XC_NO_ERROR)
    {
      if (new_sheet)
	DELETE(c_Tableau)(sheet);
      
      Xc_free(filename);
      return;
    }
  
  if (new_sheet)
  {
    doc_name = (char *)Xc_malloc("doc name", XcMAX_NAME_LENGTH);
    base_name = GetBasename(filename, FALSE);
    pos = Xstrnchr(base_name, '.');
    if (pos == -1)
      pos = strlen(base_name);
    
    memcpy(doc_name, base_name, MIN(pos, XcMAX_NAME_LENGTH-1));
    doc_name[MIN(pos, XcMAX_NAME_LENGTH-1)] = '\0';
    
    F(sheet).Init_Tableau(sheet,
			  XtDisplay(WIDGET(MainDocument)),
			  XtWindow(WIDGET(MainDocument)),
			  TRUE, doc_name);
    
/*    {
      c_Moteur *moteur;
      CellStyle_t *ptr;
      cell_style_list	*list_tmp;
      
      ptr = sheet->BaseStd.cell_style_base->cell_style;
      while(ptr)
      {
	printf("cs: %p count: %d\n", ptr, ptr->count);
	ptr = ptr->NextCellStyle;
      }
      
      moteur = sheet->moteur;
      list_tmp = moteur->style_list;
      while (list_tmp)
      {
	printf("xmin: %d, xmax:%d, ymin:%d, ymax:%d cs: %p\n", 
	       list_tmp->region.x_min, list_tmp->region.x_max,
	       list_tmp->region.y_min, list_tmp->region.y_max, 
	       list_tmp->cstyle);
	list_tmp = list_tmp->Next;
      }
    } */
    
    sheet->modified = FALSE;
    XStoreName(XtDisplay(WIDGET(MainDocument)), sheet->window, doc_name);
    F(DocList).Add(DocList, sheet, DOCUMENT_TABLEUR, doc_name);
    Xc_free(doc_name);
    Xc_TRACE(("manage"));
    F(Gwm).ManageWin(Gwm, sheet->display, sheet->window,
		     F(GlobDocIcon).get_pixmap(GlobDocIcon, Icon_TABLEUR));
  }
  else
  {
    redraw_all_feuille(sheet->Feuille);
    sheet->modified = TRUE;
  }
  
  Xc_free(filename);  
}



