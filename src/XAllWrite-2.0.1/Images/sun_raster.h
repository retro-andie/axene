/*
** sun_raster.h for Xclamation and XAllWrite in Images/
** Methods for the reading sun_raster file
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
** Started on  Sun Jun 12 02:14:52 1994 Stéphane Boisson
** Last update Fri May 19 11:33:38 1995 Emmanuel Paris
*/

#ifndef _sun_raster_h_
#define _sun_raster_h_

#include "xcalibur.h"
#include "Image.h"
#include <sys/file.h>

#define Xc_RAS_NAME		"SUN Raster format"
#define Xc_RAS_COPYRIGHT	"Copyright Xcalibur 1995"

#define Xc_RAS_EXTC		1
#define Xc_RAS_EXT1		"RAS"

#define GET_BYTE(file) (char)(fgetc(file) & 0xff)

#define SR_OLD_STYLE		0
#define SR_STANDARD_STYLE	1
#define SR_BYTE_ENCODED		2
#define SR_RGB_FORMAT		3
#define SR_TIFF_FORMAT		4
#define SR_IFF_FORMAT		5

#define SR_NO_COLORMAP		0
#define SR_COLORMAP		1
#define SR_RAW_COLORMAP		2
  
#endif
