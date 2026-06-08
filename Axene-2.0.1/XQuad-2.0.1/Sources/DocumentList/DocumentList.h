/*
** DocumentList.h for Xclamation, XQuad and XAllWrite in DocumentList/
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
** Started on  Mon Jul 17 14:41:09 1995 Antoine Buat
** Last update Tue Nov  4 17:26:56 1997 Emmanuel Paris
*/

#ifndef _DocumentList_h
#define _DocumentList_h

#include "xcalibur.h"
#include "DocumentStd.h"
#include "PulldownMenu.h"
#include <X11/Intrinsic.h>


#define DL_DOC_LIST_LENGTH	15

typedef struct sc_DocumentList c_DocumentList;


/* defint the method for Class DocumentList */
typedef struct
{
 F_STD;
 void 		(*Add)();
 void 		(*Del)();
 void 		(*DelId)();
 void	 	*(*Find)();
 char		*(*Get_NextName)();
 void		(*SetDocSelection) ___PROTO((c_DocumentList *This,
					     Window window));
 void		(*ClearDocSelection) ___PROTO((c_DocumentList *This));
 void		(*ChangeDocName) ___PROTO((c_DocumentList *This, 
					   void *Document, char *name));
} sf_DocumentList;


/* define the listdocument type */
typedef struct listDOC
{
 char			*Name;
 int			Type;
 void			*Document;
 struct listDOC	*NextDocument;
} listdocument;

/* define the DocumentList Class */
struct sc_DocumentList
{
 sf_DocumentList	*f;
 char			*DefaultName;
 int			NbrDocument;
 listdocument		*ListDocument;
 c_PulldownMenu		*doclist_menu;
 int			doclist_button;
 char			*doclist[DL_DOC_LIST_LENGTH];
 void			(*doclist_callback)();
 
 Window			last_doc_selected;
};

extern sf_DocumentList fc_DocumentList;

#endif






