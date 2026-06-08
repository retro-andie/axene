/*
** FontEncoding.h for Xclamation, XQuad and XAllWrite in FontEncoding/
** Functions to get char code from an encoding name
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
** Started on  Tue Jun 21 15:01:48 1994 Stéphane Boisson
** Last update Fri Jun  4 17:04:30 1999 Emmanuel Paris
*/

#ifndef __FontEncoding_h__
#define __FontEncoding_h__

#include "xcalibur.h"

typedef enum {
  XcFE_ADOBE_STD = 0, XcFE_LATIN1, XcFE_DINGBATS, XcFE_SYMBOL, XcFE_UNKNOW
} font_encoding_t;

#define XcFE_ADOBE_STD_SCHEME "AdobeStandardEncoding"
#define XcFE_FONT_SPECIFIC_SCHEME "FontSpecific"
#define XcFE_DINGBATS_NAME "ZapfDingbats"
#define XcFE_SYMBOL_NAME "Symbol"

/*--- Struture for encoding vectors ---*/
typedef struct {
  char *name;    /* encoding name */
  char_t code;   /* char code     */
} encoding_name_t;


/*--- AdobeStandardEncoding ---*/
encoding_name_t *is_AdobeStd_char ___NPROTO((char *str,
					    unsigned int len));

/*--- Latin1 (iso8859-1) encoding ---*/
encoding_name_t *is_Latin1_char ___NPROTO((char *str,
					  unsigned int len));

encoding_name_t *is_Dingbats_char ___NPROTO((char *str,
					    unsigned int len));

encoding_name_t *is_Symbol_char ___NPROTO((char *str,
					  unsigned int len));

/*--- ASCII encoding ---*/
encoding_name_t *is_ASCII_char ___NPROTO((char *str,
					 unsigned int len));

typedef encoding_name_t *(*XcIsEncodedCharProc) ___PROTO((char *str,
							  unsigned int len));

#endif /* !__FontEncoding_h__ */
