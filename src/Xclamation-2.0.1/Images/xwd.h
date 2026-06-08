/*
** xwd.h for Xclamation and XAllWrite in Images/
** Methods for the reading xwd file
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

#ifndef _xwd_h_
#define _xwd_h_

#if defined(___alpha) && !defined(__alpha)
#define __alpha
#endif

#include "xcalibur.h"
#include "Image.h"
#include "XWDFile.h"
#include <sys/file.h>

#define Xc_XWD_NAME		"XWindow Dump XWD"
#define Xc_XWD_COPYRIGHT	"Copyright Xcalibur 1995 & XConsortium"

#define Xc_XWD_EXTC		1
#define Xc_XWD_EXT1		"XWD"

#define Xc_XWD_HEADER_LENGTH	2048

#endif


