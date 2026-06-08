/*
** bmp.h for Xclamation, XAllWrite and XMayday in Images/
** Methods for the reading bmp file
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
** Last update Tue Sep 24 18:22:18 1996 Emmanuel Paris
*/

#ifndef _bmp_h_
#define _bmp_h_

#include "xcalibur.h"
#include "Image.h"
#include <sys/file.h>

#define Xc_BMP_NAME		"MS-Windows Image BMP"
#define Xc_BMP_COPYRIGHT	"Copyright Xcalibur 1995"

#define Xc_BMP_EXTC		2
#define Xc_BMP_EXT1		"BMP"
#define Xc_BMP_EXT2		"RLE"

#define Xc_BI_RGB	0
#define Xc_BI_RLE8	1
#define Xc_BI_RLE4	2

#define GET_BYTE(file) (char)(fgetc(file) & 0xff)
#define SKIP_BYTE(file) fgetc(file)

#define XNO_MODE	0
#define END_OF_LINE	1
#define END_OF_BITMAP	2
#define DELTA		3
#define ABSOLUTE_MODE	4
#define ENCODED_MODE	5
 
#endif

