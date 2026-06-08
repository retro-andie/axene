/*
** SYLKDocFileP.h for XQuad in SYLKDocFile/
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
** Started on  Tue Oct 10 17:32:36 1995 Antoine Buat
** Last update Tue Oct 10 17:55:50 1995 Antoine Buat
*/

#ifndef __XC_SYLK_DOCUMENT_FILE_P_H__
#define __XC_SYLK_DOCUMENT_FILE_P_H__

#include "SYLKDocFile.h"

/*--- Definitions ---*/
#define XcDF_EOF_CODE 0
#define XcDF_CONTROL_CODE 1
#define XcDF_SEQ_START_CODE 2
#define XcDF_SEQ_END_CODE 3
#define XcDF_NEWLINE_CODE 4
#define XcDF_SPACE_CODE 5

#define XcDF_ConvertPrime 37
#define XcDF_HashPrime 1048583UL

#define XcDF_DATA_CHUNK_SIZE 4096

#define XcDF_KEYWORD 1
#define XcDF_NUMBER 2
#define XcDF_TEXT 3
#define XcDF_SEQUENCE_START 4
#define XcDF_SEQUENCE_END 5

#endif /* !__XC_SYLK_DOCUMENT_FILE_P_H__ */
