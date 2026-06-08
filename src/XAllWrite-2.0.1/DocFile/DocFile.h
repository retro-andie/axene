/*
** DocFile.h for Xclamation, XQuad, XAllWrite and XMayday in DocFile/
** Declarations of the DocFile class
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
** Started on  Tue Nov  1 16:28:06 1994 Stéphane Boisson
** Last update Mon Nov 24 17:58:14 1997 Emmanuel Paris
*/

#ifndef __XC_DOCUMENT_FILE_H__
#define __XC_DOCUMENT_FILE_H__

/*--- Forward declarations ---*/
typedef struct sc_DocFile c_DocFile;

#include "xcalibur.h"

#define XcDF_DOCUMENT_KEYWORD "DOCUMENT"
#define XcDF_NAME_KEYWORD "NAME"
#define XcDF_FULLNAME_KEYWORD "FULLNAME"
#define XcDF_CREATOR_KEYWORD "CREATOR"
#define XcDF_STARTED_ON_KEYWORD "STARTED_ON"
#define XcDF_LAST_UPDATE_KEYWORD "LAST_UPDATE"
#define XcDF_VERSION_KEYWORD "VERSION"
#define XcDF_OSNAME_KEYWORD "OSNAME"
#define XcDF_COMMENT_KEYWORD "COMMENT"
#define XcDF_FILENAME_KEYWORD "FILENAME"
#define XcDF_COMMAND_KEYWORD "COMMAND"
#define XcDF_BITFIELD_KEYWORD "BITFIELD"


/*--- Macros ---*/
#define XcDF_SAVE 0
#define XcDF_LOAD 1
#define XcDF_KEYWORD_MAX_LEN 64
#define XcDF_BUFFER_SIZE 10240

/*--- Internal structures ---*/
typedef struct docfile_context_s docfile_context_t;
typedef struct docfile_keyword_s docfile_keyword_t;

enum docfile_data_type_e
{
 XcDF_NONE,
 XcDF_DOCUMENT_TYPE,
 XcDF_PAGE_TYPE,
 XcDF_CADRE_TYPE,
 XcDF_IMAGE_TYPE,
 XcDF_COLOR_TYPE,
 XcDF_TEXT_TYPE,
 XcDF_CELLSTYLE_TYPE,
 XcDF_STYLE_TYPE,
 XcDF_RULER_TYPE
};


/*--- Methods ---*/
typedef struct
{
 F_STD;
  
 /*___Reading___*/
 boolean (*expectKeyword) ___PROTO((c_DocFile *doc, char *keyword, 
				    boolean persist_flag));
 boolean (*addCallbacks) ___PROTO((c_DocFile *doc,
				   /* char *keyword,
				      int (*function)(c_DocFile *doc,
				      char *keyword,
				      long param,
				      void *data),
				      void *data, ... NULL*/
				   ...));
 boolean (*removeCallbacks) ___PROTO((c_DocFile *doc,
				      /* char *keyword, ... NULL */
				      ...));
 boolean (*read) ___PROTO((c_DocFile *doc,
			   unsigned char **ptr, long *len));
 boolean (*readWord) ___PROTO((c_DocFile *doc,
			       unsigned char **ptr, long *len));
 boolean (*readInteger) ___PROTO((c_DocFile *doc, long *integer));
 boolean (*readReal) ___PROTO((c_DocFile *doc, real *realp));
 boolean (*readBoolean) ___PROTO((c_DocFile *doc, boolean *booleanp));
 boolean (*readString) ___PROTO((c_DocFile *doc, unsigned char **stringp));
 boolean (*readFilename) ___PROTO((c_DocFile *doc, unsigned char **strp));

 /*___Writing___*/
 boolean (*write) ___PROTO((c_DocFile *doc,
			    unsigned char *buffer, long len));
 boolean (*writeInteger) ___PROTO((c_DocFile *doc, long integer));
 boolean (*writeReal) ___PROTO((c_DocFile *doc, real r));
 boolean (*writeBoolean) ___PROTO((c_DocFile *doc, boolean flag));
 boolean (*writeKeyword) ___PROTO((c_DocFile *doc,
				   char *keyword, long param));
 boolean (*writeFilename) ___PROTO((c_DocFile *doc, unsigned char *str));
 boolean (*startSequence) ___PROTO((c_DocFile *doc));
 boolean (*endSequence) ___PROTO((c_DocFile *doc));
  
} sf_DocFile;


/*--- Declaration of the DocFile class ---*/
struct sc_DocFile
{
 sf_DocFile *f;
  
 /*___File info___*/
 char *filename;
 char *basename;
 int handle;
 long depth;
 int mode;
 int next_char, last_char;
 boolean close_flag;

 /*___File buffer___*/
 unsigned char *ptr;
 unsigned char *endptr;
 unsigned char buffer[XcDF_BUFFER_SIZE];
  
 /*___Context___*/
 boolean lock_flag;
 ItemHandle *item;
 long context_len;
 long context_pos;
 docfile_keyword_t *base;
 docfile_context_t *context;

 /*___user data___*/
 enum docfile_data_type_e data_type;
 void *data_ptr;

 /*___Error flags___*/
 boolean error_flag;
};

extern sf_DocFile fc_DocFile;

#endif /* !__XC_DOCUMENT_FILE_H__ */
