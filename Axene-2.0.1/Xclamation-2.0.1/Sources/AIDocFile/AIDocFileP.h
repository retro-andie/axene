/*
** AIDocFileP.h for Xclamation in AIDocFile/
** Private header for the AIDocFile class
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
** Last update Tue Jan 30 15:27:34 1996 Stéphane Boisson
*/

#ifndef __XC_AI_DOCUMENT_FILE_P_H__
#define __XC_AI_DOCUMENT_FILE_P_H__

#include "AIDocFile.h"
#include "VectorFont.h"

/*--- Definitions ---*/
#define XcAIDF_STRING_START_CODE 1
#define XcAIDF_STRING_END_CODE 2
#define XcAIDF_LAST_CODE 2


/*--- Structure ---*/
struct ai_font_record_s {
  ai_font_record_t *next;
  char name[128];
  c_VectorFont *font;
  char_t encoding[256];
};

/*--- Prototypes ---*/
ai_font_record_t *aiEncodeFont ___PROTO((c_AIDocFile *this, char *old,
					 char *new, int native_flag));
c_VectorFont *aiFindFont ___PROTO((c_AIDocFile *this, char *name));
void aiTranslateText ___PROTO((c_AIDocFile *this, char_t *string));


#endif /* !__XC_AI_DOCUMENT_FILE_P_H__ */
