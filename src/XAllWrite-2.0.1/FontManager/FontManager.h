/*
** FontManager.h for Xclamation, XQuad and XAllWrite in FontManager/
** Definition of the class FontManager
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
** Started on  Thu Oct 13 17:10:06 1994 Antoine Buat
** Last update Wed Jul  5 13:49:55 1995 Emmanuel Paris
*/

#ifndef _FontManager_h
#define _FontManager_h

typedef struct sc_FontManager c_FontManager;

#include "xcalibur.h"
#include "RasterFont.h"
#include "VectorFont.h"
#include "CharMetrics.h"

#define FM_NB_FONT_KEEPED    50

/* ----------------------------------------------------------------- ** 
** define the FontManager Struct	                             ** 
** ----------------------------------------------------------------- */

typedef struct FontManager_s
{
  c_VectorFont *vfont;
  c_CharMetrics metrics;
  c_RasterFont	*rfont;
  int	used_time;
  
  struct FontManager_s	*Next;
} FontManager_t;

/* ----------------------------------------------------------------- ** 
** define the method for FontManager Class                           ** 
** ----------------------------------------------------------------- */
typedef struct
{
  F_STD;
  c_RasterFont *(*new_font) ___PROTO((c_FontManager *This,
				    c_VectorFont *vfont,
				    c_CharMetrics *metrics));
  void (*delete_font) ___PROTO((c_FontManager *This,
				c_RasterFont *rfont));
} sf_FontManager;

/* ----------------------------------------------------------------- ** 
** define the FontManager Class                                      ** 
** ----------------------------------------------------------------- */
struct sc_FontManager
{
 sf_FontManager	*f;
 Display	*display;
 
 int	nb_font_keep;
 FontManager_t	*FntMng;
};

extern sf_FontManager fc_FontManager;

#endif
