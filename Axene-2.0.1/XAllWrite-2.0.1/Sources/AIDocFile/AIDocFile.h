/*
** AIDocFile.h for Xclamation and XAllWrite in AIDocFile/
** Declarations of the AIDocFile class
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
** Last update Tue Jan 30 15:14:01 1996 Stéphane Boisson
*/

#ifndef __XC_AI_DOCUMENT_FILE_H__
#define __XC_AI_DOCUMENT_FILE_H__

/*--- Forward declarations ---*/
typedef struct sc_AIDocFile c_AIDocFile;
typedef struct ai_font_record_s ai_font_record_t;

#include "xcalibur.h"
#include "VectorGraph.h"
#include "VectorPolygon.h"

/*--- Macros ---*/
#define XcAIDF_BUFFER_SIZE	10240
#define XcAIDF_LOAD		1
#define XcAIDF_WRITE		2

#define XcAIDF_ERROR 0
#define XcAIDF_STRING 1
#define XcAIDF_COMMENT 2
#define XcAIDF_OPEN_MARK 3
#define XcAIDF_CLOSE_MARK 4
#define XcAIDF_WORD 5
#define XcAIDF_NAME 6


/*--- Methods ---*/
typedef struct {
  F_STD;
  /*___Reading___*/
  boolean (*readMain) ___PROTO((c_AIDocFile *this));
  unsigned char (*AI_getchar) ___PROTO((c_AIDocFile *this));
  int (*read) ___PROTO((c_AIDocFile *doc,
			unsigned char *ptr, long len));
  int (*seek) ___PROTO((c_AIDocFile *doc,
			int mode, int len));
  int (*readWord) ___PROTO((c_AIDocFile *doc,
				unsigned char **ptr, long *len));
  boolean (*readInteger) ___PROTO((c_AIDocFile *doc, long *integer));
  boolean (*readReal) ___PROTO((c_AIDocFile *doc, real *realp));
  boolean (*readBoolean) ___PROTO((c_AIDocFile *doc, boolean *booleanp));
  void (*skipSpace) ___PROTO((c_AIDocFile *doc));
  boolean (*searchData) ___PROTO((c_AIDocFile *doc, char *data));
} sf_AIDocFile;


/*--- Declaration of the AIDocFile class ---*/
struct sc_AIDocFile {
  sf_AIDocFile *f;
  
  /*___File info___*/
  char *filename;
  int handle;
  int next_char;
  int last_char;
  boolean close_flag;

  /*___File buffer___*/
  unsigned char buffer_file[XcAIDF_BUFFER_SIZE];
  long		buffer_index;
  long		end_index;
  unsigned char line[256];
  unsigned char *line_ptr;

  /*___Memory struct___*/
  c_VectorGraph	*vector;

  /*___Color___*/
  c_Color	**color_base;

  /*--- Internal ---*/
  int string_level;
  char_t native_encoding[256];
  char_t *encoding;
  ai_font_record_t *font_base;
};

extern sf_AIDocFile fc_AIDocFile;

#endif /* !__XC_DOCUMENT_FILE_H__ */
