/*
** DocFileP.h for Xclamation, XQuad, XAllWrite and XMayday in DocFile/
** Private header for the DocFile class
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
** Started on  Tue Nov  1 16:56:36 1994 Stéphane Boisson
** Last update Fri Apr 17 15:06:18 1998 Emmanuel Paris
*/

#ifndef __XC_DOCUMENT_FILE_P_H__
#define __XC_DOCUMENT_FILE_P_H__

#include "DocFile.h"

/*--- Definitions ---*/
#define XcDF_EOF_CODE 0
#define XcDF_CONTROL_CODE 1
#define XcDF_SEQ_START_CODE 2
#define XcDF_SEQ_END_CODE 3
#define XcDF_NEWLINE_CODE 4
#define XcDF_SPACE_CODE 5
#define XcDF_LAST_CODE 5

#define XcDF_ConvertPrime 37
#define XcDF_HashPrime 1048583UL

#define XcDF_DATA_CHUNK_SIZE 4096

#define XcDF_KEYWORD 1
#define XcDF_NUMBER 2
#define XcDF_TEXT 3
#define XcDF_SEQUENCE_START 4
#define XcDF_SEQUENCE_END 5


/*--- Keyword structure ---*/
struct docfile_keyword_s 
{
 docfile_keyword_t *next;

 char name[XcDF_KEYWORD_MAX_LEN];
 unsigned long key;
 boolean (*callback) ___PROTO((c_DocFile *doc,
			       char *keyword,
			       long param,
			       void *callback_data));
 void *callback_data;
};

/*--- Context structure ---*/
struct docfile_context_s 
{
 docfile_keyword_t *current;
 docfile_keyword_t *base;
};

#endif /* !__XC_DOCUMENT_FILE_P_H__ */
