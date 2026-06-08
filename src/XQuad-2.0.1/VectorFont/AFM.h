/*
** AFM.h for Xclamation, XQuad and XAllWrite in VectorFont/
** Identifiers for AFM keywords
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
** Started on  Fri Jun 10 19:55:39 1994 Stéphane Boisson
** Last update Fri Jun  4 17:03:14 1999 Emmanuel Paris
*/

#ifndef __AFM_H__
#define __AFM_H__

#include "xcalibur.h"
#include "VectorFont.h"


enum keyword_e {
  AK_UNKNOW, AK_COMMENT, AK_SEPARATOR,

  /*___AFM Specific___*/
  AK_START_FONT_METRICS, AK_END_FONT_METRICS,
  
  /*___Control Information___*/
  AK_METRICS_SETS,
  
  /*___Global Font Information___*/
  AK_FONT_NAME, AK_FULL_NAME, AK_FAMILY_NAME, AK_WEIGHT, AK_FONT_BBOX,
  AK_VERSION, AK_NOTICE, AK_ENCODING_SCHEME, AK_MAPPING_SCHEME,
  AK_ESC_CHAR, AK_CHARACTER_SET, AK_CHARACTERS, AK_IS_BASE_FONT,
  AK_V_VECTOR, AK_IS_FIXED_V, AK_CAP_HEIGHT, AK_X_HEIGHT,
  AK_ASCENDER, AK_DESCENDER,
  
  /*___Writing Direction Information___*/
  AK_START_DIRECTION, AK_END_DIRECTION, AK_UNDERLINE_POSITION,
  AK_UNDERLINE_THICKNESS, AK_ITALIC_ANGLE, AK_CHAR_WIDTH, AK_IS_FIXED_PITCH,
  
  /*___Individual Character Metrics___*/
  AK_START_CHAR_METRICS, AK_END_CHAR_METRICS,
  AK_C, AK_CH,
  AK_WX, AK_W0X, AK_W1X, AK_WY, AK_W0Y, AK_W1Y, AK_W, AK_W0, AK_W1,
  AK_VV, AK_N, AK_B, AK_L,

  /*___Kerning Data___*/
  AK_START_KERN_DATA, AK_END_KERN_DATA,
  AK_START_TRACK_KERN, AK_END_TRACK_KERN, AK_TRACK_KERN,
  AK_START_KERN_PAIRS, AK_END_KERN_PAIRS, AK_START_KERN_PAIRS0,
  AK_START_KERN_PAIRS1, AK_KP, AK_KPH, AK_KPX, AK_KPY,

  /*___Composite Character___*/
  AK_START_COMPOSITES, AK_END_COMPOSITES,
  AK_CC, AK_PCC
};

typedef struct {
  char *name;
  enum keyword_e type;
} AFM_keyword_t;

extern AFM_keyword_t unknow_AFM_keyword;
___INLINE AFM_keyword_t *is_AFM_keyword ___NPROTO((char *, unsigned int));

boolean AFM_parse ___PROTO((char *, c_VectorFont *));

#endif /* !__AFM_H__ */
