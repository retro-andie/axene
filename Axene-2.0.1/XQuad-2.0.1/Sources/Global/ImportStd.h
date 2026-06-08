/*
** ImportStd.h for XQuad in Global/
** Common Interface of Import Objects
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
** Started on  Wed Oct 11 23:10:52 1995 Emmanuel Paris
** Last update Fri Oct 13 11:11:04 1995 Emmanuel Paris
*/

#ifndef _import_std_h_
#define _import_std_h_

#include "xcalibur.h"
#include <X11/Intrinsic.h>
#include "Tableau.h"
#include <sys/file.h>

#define F_IMPORT_STD \
void	(*create_attribs_box)(); \
void	(*manage_attribs_box)(); \
void	(*destroy_attribs_box)(); \
boolean	(*test)(); \
boolean (*create_new_sheet)(); \
error	(*load)()     
     
#if 0
     void	(*create_attribs_box) ___PROTO((void *This, Widget Parent));
     /* Parent Widget will be a frame widget (in fileselector) */
     /* This function must create and manage her attribs box */
     void	(*manage_attribs_box) ___PROTO((void *This, 
						boolean doc_available));
     /* can be called severall time, must act in flip flop */
     void	(*destroy_attribs_box) ___PROTO((void *This));
     /* called at the destruction of the fileselector */
     boolean	(*test) ___PROTO((void *This));
     /* TRUE if good format, FALSE if wrong one */
     boolean	(*create_new_sheet) ___PROTO((void *This));
     /* TRUE: a new sheet is created for load, FALSE: the current sheet 
	is given, column and row are filled with current active cell */    
     error	(*load) ___PROTO((void *This, c_Tableau *sheet));
     /* load sheet, sheet is already created (feuille and moteur) */
#endif
     
typedef struct 
{
  F_STD;
  F_IMPORT_STD;
} sf_Import_Std;
     
typedef struct {
  sf_Import_Std *f;
  
/* constant field, must be set in the constructor */  
  char *name;			/* format full name */
  char *copyright;		/* format copyright */
  char *id;			/* general id */
  int extension_count;		/* nb usual extensions */
  char **extensions;		/* extensions */
/* variable field, can be use in attribs_box, test and load methods */
  char *filename;		/* will be set by the c_Import object */
  boolean *box_created;		/* (*) will be set by the c_Import object */
} c_Import_Std;

#define IMPORT_FIELD \
  char *name; \
  char *copyright; \
  char *id; \
  int extension_count; \
  char **extensions; \
  char *filename; \
  boolean *box_created

#define IMPORT_NEW(obj) \
((c_Import_Std *)(obj))->f->cons
#define IMPORT_DELETE(obj) \
((c_Import_Std *)(obj))->f->dest
#define IMPORT_CREATE_ATTRIBS_BOX(obj) \
((c_Import_Std *)(obj))->f->create_attribs_box
#define IMPORT_MANAGE_ATTRIBS_BOX(obj) \
((c_Import_Std *)(obj))->f->manage_attribs_box
#define IMPORT_DESTROY_ATTRIBS_BOX(obj) \
((c_Import_Std *)(obj))->f->destroy_attribs_box
#define IMPORT_TEST(obj) \
((c_Import_Std *)(obj))->f->test
#define IMPORT_CREATE_NEW_SHEET(obj) \
((c_Import_Std *)(obj))->f->create_new_sheet
#define IMPORT_LOAD(obj) \
((c_Import_Std *)(obj))->f->load

#endif /* _import_std_h_ */
