/*
** iff.h for Xclamation in Images/
** Methods for the reading iff file
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
** Started on  Sun Jun 12 02:14:52 1994 Emmanuel Paris
** Last update Thu Oct 24 16:55:02 1996 Emmanuel Paris
*/

#ifndef _iff_h_
#define _iff_h_

#include "xcalibur.h"
#include "Image.h"
#include <sys/file.h>

#define Xc_IFF_NAME		"Interchange File Format IFF"
#define Xc_IFF_COPYRIGHT	"Copyright Xcalibur 1995"

#define Xc_IFF_EXTC		2
#define Xc_IFF_EXT1		"IFF"
#define Xc_IFF_EXT2		"LBM"

#define GET_BYTE (char)(fgetc(The_Data.File) & 0xff)
#define SKIP_BYTE fgetc(The_Data.File)

typedef struct 
{
  FILE          *File;
  int		header_pos;
  int		colormap_pos;
  int		image_pos;
  int		real_depth;
  boolean	compression;
} IFF_Import_t;
  
#endif
