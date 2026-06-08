/*
** SYLKDocFile.h for XQuad in SYLKDocFile/
** Declarations of the SYLKDocFile class
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
** Started on  Tue Nov 10 17:35:06 1995 Antoine Buat
** Last update Fri Oct 13 14:30:46 1995 Antoine Buat
*/

#ifndef __XC_SYLK_DOCUMENT_FILE_H__
#define __XC_SYLK_DOCUMENT_FILE_H__

/*--- Forward declarations ---*/
typedef struct sc_SYLKDocFile c_SYLKDocFile;

#include "xcalibur.h"
#include "ImportStd.h"

/*--- Macros ---*/
#define XcSYLKDF_SAVE 0
#define XcSYLKDF_LOAD 1
#define XcSYLKDF_BUFFER_SIZE 10240

/*--- Methods ---*/
typedef struct {
  F_STD;
  F_IMPORT_STD;

  /*___Reading___*/
  char (*readWord) ___PROTO((c_SYLKDocFile *doc,
			     unsigned char **ptr, long *len));
  boolean (*readInteger) ___PROTO((c_SYLKDocFile *doc, long *integer));
  boolean (*readReal) ___PROTO((c_SYLKDocFile *doc, real *realp));
  boolean (*readBoolean) ___PROTO((c_SYLKDocFile *doc, boolean *booleanp));
  boolean (*readString) ___PROTO((c_SYLKDocFile *doc, 
				  unsigned char **stringp));
  void (*skipSpace) ___PROTO((c_SYLKDocFile *doc));  
} sf_SYLKDocFile;


/*--- Declaration of the DocFile class ---*/
struct sc_SYLKDocFile {
  sf_SYLKDocFile *f;

  IMPORT_FIELD;
  
  /*___File info___*/
  FILE	*fildes;
  int	filelen;
  int handle;
  int mode;
  
  /*___Error flags___*/
  boolean error_flag;

};

extern sf_SYLKDocFile fc_SYLKDocFile;

#endif /* !__XC_SYLK_DOCUMENT_FILE_H__ */
