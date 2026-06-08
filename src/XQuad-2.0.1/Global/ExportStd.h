/*
** ExportStd.h for XQuad in Global/
** Common Interface of Export Objects
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
** Started on  Sun Dec 15 17:41:20 1996 Robin Castanier
** Last update Wed Dec 18 14:41:19 1996 Emmanuel Paris
*/

#ifndef _export_std_h_
#define _export_std_h_

#include "xcalibur.h"
#include <X11/Intrinsic.h>
#include "Tableau.h"
#include <sys/file.h>

typedef struct {
  boolean	dim_calc;
  Dimension	width;
  Dimension	height;
} ab_size_t;

#define F_EXPORT_STD \
void	(*create_attribs_box)(); \
void	(*manage_attribs_box)(); \
void	(*destroy_attribs_box)(); \
error	(*save)()     
     
#if 0
     void	(*create_attribs_box) ___PROTO((void *This, Widget Parent, 
						ab_size_t *size));
     /* Parent Widget will be a frame widget (in fileselector) */
     /* This function must create and manage her attribs box */
     void	(*manage_attribs_box) ___PROTO((void *This, 
						boolean doc_available,
						ab_size_t *size));
     /* can be called severall time, must act in flip flop */
     void	(*destroy_attribs_box) ___PROTO((void *This));
     /* called at the destruction of the fileselector */
     error	(*save) ___PROTO((void *This, c_Tableau *sheet));
     /* save sheet */
#endif
     
typedef struct 
{
  F_STD;
  F_EXPORT_STD;
} sf_Export_Std;
     
typedef struct {
  sf_Export_Std *f;
  
/* constant field, must be set in the constructor */  
  char *name;			/* format full name */
  char *copyright;		/* format copyright */
  char *id;			/* general id */
  int extension_count;		/* nb usual extensions */
  char **extensions;		/* extensions */
/* variable field, can be use in attribs_box, test and load methods */
  char *filename;		/* will be set by the c_Export object */
  boolean *box_created;		/* (*) will be set by the c_Export object */
} c_Export_Std;

#define EXPORT_FIELD \
  char *name; \
  char *copyright; \
  char *id; \
  int extension_count; \
  char **extensions; \
  char *filename; \
  boolean *box_created

#define EXPORT_NEW(obj) \
((c_Export_Std *)(obj))->f->cons
#define EXPORT_DELETE(obj) \
((c_Export_Std *)(obj))->f->dest
#define EXPORT_CREATE_ATTRIBS_BOX(obj) \
((c_Export_Std *)(obj))->f->create_attribs_box
#define EXPORT_MANAGE_ATTRIBS_BOX(obj) \
((c_Export_Std *)(obj))->f->manage_attribs_box
#define EXPORT_DESTROY_ATTRIBS_BOX(obj) \
((c_Export_Std *)(obj))->f->destroy_attribs_box
#define EXPORT_SAVE(obj) \
((c_Export_Std *)(obj))->f->save

#endif /* _export_std_h_ */






