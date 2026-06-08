/*
** Dither.h for Xclamation, XAllWrite, XMayday and XInstall in Dither/
** Declaration of the Dither class
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
** Started on  Fri Oct 21 16:58:19 1994 Stéphane Boisson
** Last update Thu Jan  6 18:08:59 2000 Emmanuel Paris
*/

#ifndef __XC_DITHER_H__
#define __XC_DITHER_H__

typedef struct sc_Dither c_Dither;

#include "xcalibur.h"
#include "Image.h"
#include "Colormap.h"

/*--- Internal structures ---*/
enum dither_mode_e { XcD_24_TO_8, XcD_24_TO_2, XcD_8_TO_2, XcD_8_TO_8, 
		   XcD_8_TO_24, XcD_24_TO_24, XcD_8_TO_16, XcD_24_TO_16};

/*--- Forward declarations ---*/
typedef struct sfs_Dither sf_Dither;


/*--- Declaration ---*/
struct sc_Dither {
  sf_Dither *f;

  unsigned int width;
  unsigned int height;
  unsigned long size;
  unsigned char *dest;
  c_Colormap *colormap;
  boolean (*callback) ___PROTO((unsigned char **line_buffer_p,
				c_Dither *this));
};

/*--- Methods ---*/
struct sfs_Dither {
  F_STD;
};

extern sf_Dither fc_Dither;

#endif /* !__XC_DITHER_H__ */


