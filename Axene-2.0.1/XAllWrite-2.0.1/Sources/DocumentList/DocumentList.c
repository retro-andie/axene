/*
** DocumentList.c for Xclamation, XQuad and XAllWrite in DocumentList/
** Manage Document List 
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
** Started on  Mon Jul 17 14:40:31 1995 Antoine Buat
** Last update Fri Oct 23 14:18:43 1998 Emmanuel Paris
*/

#include "DocumentList.h"

static void *cons_DocumentList();
static void dest_DocumentList();
static void *copy_DocumentList();
static char *get_NextName_Document();
static void add_Document();
static void del_Document();
static void delid_Document();
static void *find_Document();
static void SetDocSelection();
static void ClearDocSelection();
static void change_doc_name ___PROTO((c_DocumentList *This, 
				      void *Document, char *name));

static void generateDocList();
static void doclistMenuCallback();

sf_DocumentList fc_DocumentList =
{
 cons_DocumentList,
 dest_DocumentList,
 copy_DocumentList,
 add_Document,
 del_Document,
 delid_Document,
 find_Document,
 get_NextName_Document,
 SetDocSelection,
 ClearDocSelection,
 change_doc_name
};

static void *cons_DocumentList(doclist_menu, doclist_button, Name,
			       doclist_callback)
c_PulldownMenu	*doclist_menu;
int		doclist_button;
char		*Name;
void		(*doclist_callback)();
{
 c_DocumentList *This;
  
 Xc_HISTORY(("constructor"));
  
 if((This = Xc_malloc("DocumentList", sizeof(c_DocumentList))) == NULL)
  return NULL;
 memset(This, 0, sizeof(c_DocumentList));
  
 This->f = &fc_DocumentList;
 This->DefaultName = Name;
 This->NbrDocument = 0;
 This->ListDocument = NULL;
  
 This->doclist_menu = doclist_menu;
 This->doclist_button = doclist_button;
 This->last_doc_selected = (Window)0;
 This->doclist_callback = doclist_callback;
  
 F(doclist_menu).SetDynamicFlag(doclist_menu, doclist_button, TRUE, TRUE, 
				9, "Ctrl <Key>%1d", "Ctrl+%1d",
				PMt_CENTER, 20);
  
 F(doclist_menu).SetDynamicList(doclist_menu, doclist_button, This->doclist); 
 F(doclist_menu).AddDynamicCallback(doclist_menu, doclist_button, 
				    doclistMenuCallback, This);
  
 Xc_TRACE(("constructor ok"));
 return This;
}

static void dest_DocumentList(This)
c_DocumentList *This;
{
 int i;
 listdocument	*listdoc;
 listdocument	*listdoc2;
 Xc_HISTORY(("destructor"));
  
 for(i = 0; i < DL_DOC_LIST_LENGTH; i++)
 {
  if (This->doclist[i])
   Xc_free(This->doclist[i]);
 }
  
 listdoc=This->ListDocument;
 while(listdoc)
 {
  Xc_free(listdoc->Name);
  DELETE_DOCUMENT(listdoc->Document)(listdoc->Document);
  listdoc2=listdoc->NextDocument,
  Xc_free(listdoc);
  listdoc=listdoc2;
 }
 Xc_free(This);
 Xc_TRACE(("destructor ok"));
}


static void *copy_DocumentList(This)
c_DocumentList *This;
{
 c_DocumentList *ObjTmp;

 if ((ObjTmp = (c_DocumentList *)Xc_malloc("CDocumentList",
					   sizeof(c_DocumentList))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_DocumentList));
 Xc_TRACE(("Object DocumentList copied"));
 return ObjTmp;
}

static char *get_NextName_Document(This)
c_DocumentList	*This;
{
 int		long1;
 char		*name;
  
 long1 = strlen(This->DefaultName);
 if ((name = (char *)Xc_malloc("StringDoc", long1+10))
     == NULL)
 {
  Xc_FATAL(("Can't Name The Document: memory allocation error"));
 }
 sprintf(name, "%s %d", This->DefaultName, This->NbrDocument + 1);
 return name;
}

static void add_Document(This, Document, Type, name)
c_DocumentList	*This;
void		*Document;
int		Type;
char		*name;
{
 listdocument	**DocumentTmp;
 listdocument	*DocumentNew;

 Xc_HISTORY(("add_Document"));

 if ((DocumentNew = Xc_malloc("AddDocument", sizeof(listdocument))) == NULL)
 {
  Xc_FATAL(("Can't add new Document in list: memory allocation error"));
 }
 if (!name)
  DocumentNew->Name = F(This).Get_NextName(This);
 else
  DocumentNew->Name = Xc_strdup("doc name", name);

 DocumentNew->Type = Type;
 DocumentNew->Document = Document;
 DocumentNew->NextDocument = NULL;

 DocumentTmp = &D(This).ListDocument;
 D(This).NbrDocument++;
 while (*DocumentTmp != NULL)
 {
  DocumentTmp = &((*DocumentTmp)->NextDocument);
 }
 *DocumentTmp = DocumentNew;
 generateDocList(This);

 Xc_TRACE(("add ok"));
}

static void del_Document(This, pos)
c_DocumentList	*This;
int		pos;
{
 listdocument	**DocumentTmp;
 listdocument	*DocumentDel;
 int		num;

 Xc_HISTORY(("del"));

 DocumentTmp = &D(This).ListDocument;
 num = 0;
 while ((*DocumentTmp != NULL) && (num<pos))
 {
  DocumentTmp = &((*DocumentTmp)->NextDocument);
  num++;
 }
 if (num != pos)
 {
  Xc_WARNING(("Can't delete at this position"));
 }
 else
 {
  DocumentDel = *DocumentTmp;
  DELETE_DOCUMENT(DocumentDel->Document)(DocumentDel->Document);
  *DocumentTmp = ((*DocumentTmp)->NextDocument);
  D(This).NbrDocument--;
  Xc_free(DocumentDel);
  generateDocList(This);
 }
 Xc_TRACE(("del ok"));
}

static void delid_Document(This, Win)
c_DocumentList	*This;
Window		Win;
{
 listdocument	**DocumentTmp;
 listdocument	*DocumentDel;

 Xc_HISTORY(("delId"));

 DocumentTmp = &D(This).ListDocument;
 while ((*DocumentTmp != NULL) && 
	(DOCUMENT_WINDOW((*DocumentTmp)->Document) != Win))
 {
  DocumentTmp = &((*DocumentTmp)->NextDocument);
 }
 if (*DocumentTmp == NULL)
 {
  Xc_WARNING(("Can't delete this identity"));
 }
 else
 {
  DocumentDel = *DocumentTmp;
  Xc_free(DocumentDel->Name);
  DELETE_DOCUMENT(DocumentDel->Document)(DocumentDel->Document);
  *DocumentTmp = ((*DocumentTmp)->NextDocument);
  D(This).NbrDocument--;
  Xc_free(DocumentDel);
  generateDocList(This);
 }
 Xc_TRACE(("delId ok"));
}

static void *find_Document(This, Win, Type)
c_DocumentList	*This;
Window		Win;
int		*Type;
{
 listdocument	**DocumentTmp;

 DocumentTmp = &D(This).ListDocument;
 while ((*DocumentTmp != NULL) && 
	(DOCUMENT_WINDOW((*DocumentTmp)->Document) != Win))
 {
  DocumentTmp = &((*DocumentTmp)->NextDocument);
 }
 if (*DocumentTmp == NULL)
 {
  Xc_WARNING(("Can't find this identity"));
  return NULL;
 }
 if (Type)
  *Type = (*DocumentTmp)->Type;
 return (*DocumentTmp)->Document;
}

static void SetDocSelection(This, window)
c_DocumentList	*This;
Window	window;
{
 int	i;
 listdocument	*docl;
  
 if (window == This->last_doc_selected)
  return;
  
 i = 0;
 docl = This->ListDocument;
 while (docl && (DOCUMENT_WINDOW(docl->Document) != window))
 {
  docl = docl->NextDocument;
  i++;
 }
  
 if (docl && i < DL_DOC_LIST_LENGTH)
  F(This->doclist_menu).ToggleDynamicButton(This->doclist_menu,
					    i + This->doclist_button, True);
  
 This->last_doc_selected = window;
}

static void ClearDocSelection(This)
c_DocumentList	*This;
{
 int	i;
 listdocument	*docl;
  
 i = 0;
 docl = This->ListDocument;
 while (docl && (DOCUMENT_WINDOW(docl->Document) != This->last_doc_selected))
 {
  docl = docl->NextDocument;
  i++;
 }
  
 if (docl && i < DL_DOC_LIST_LENGTH)
  F(This->doclist_menu).ToggleDynamicButton(This->doclist_menu,
					    i + This->doclist_button, False);
  
 This->last_doc_selected = (Window)0;
}

static void change_doc_name(This, Document, name)
c_DocumentList *This;
void *Document;
char *name;
{
 listdocument	*docl;
  
 docl = This->ListDocument;
 while(docl)
 {
  if (docl->Document == Document)
  {
   if (docl->Name) Xc_free(docl->Name);
   docl->Name = Xc_strdup("dn", name);
   generateDocList(This);
   return;
  }
  docl = docl->NextDocument;
 }
}

static void generateDocList(This)
c_DocumentList *This;
{
 int	i;
 listdocument	*docl;
  
 Xc_TRACE(("id 1"));
  
 for(i = 0; i < DL_DOC_LIST_LENGTH; i++)
 {
  if (This->doclist[i])
  {
   Xc_free(This->doclist[i]);
   This->doclist[i] = NULL;
  }
 }
  
 Xc_TRACE(("id 2"));
 i = 0;
 docl = This->ListDocument;
 while(i < DL_DOC_LIST_LENGTH && docl)
 {
  This->doclist[i] = Xc_strdup("dup", docl->Name);
  docl = docl->NextDocument;
  i++;
 }
  
 Xc_TRACE(("id 3"));
 F(This->doclist_menu).SetDynamicList
  (This->doclist_menu, This->doclist_button, This->doclist);
 Xc_TRACE(("id 4"));
}


static void doclistMenuCallback(This, w, button)
c_DocumentList *This;
Widget w;
int button;
{
 int	i;
 listdocument *docl;
 Xc_TRACE(("Callback on button: %d", button));
 F(This->doclist_menu).ToggleDynamicButton(This->doclist_menu, 
					   button + This->doclist_button,
					   True);
  
 docl = This->ListDocument;
 i = 0;
 while(i < button && docl)
 {
  docl = docl->NextDocument;
  i++;
 }
  
 if (docl)
 {
  Xc_TRACE(("Select doc: %s", docl->Name));
  This->doclist_callback(DOCUMENT_WINDOW(docl->Document));
 }
}








