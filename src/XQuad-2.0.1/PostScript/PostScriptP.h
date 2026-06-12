/*
** PostScriptP.h for Xclamation, XQuad and XAllWrite in PostScript/
** Private header for PostScript class
**
** Copyright (C) 1994-2000 Axene.
** Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
** Started on  Wed Feb 22 17:59:58 1995 St�phane Boisson
** Last update Fri Jan 12 17:45:08 1996 St�phane Boisson
*/

#ifndef __XC_POSTSCRIPT_P_H__
#define __XC_POSTSCRIPT_P_H__

#include "PostScript.h"

#define XcPS_DIR NULL
#define XcPS_PROCSET_FILENAME "xcalibur.prc"
#define XcPS_PROCSET_NAME "XCaliburDict"

#define XcPS_GRAYSCALE(r,g,b) (((long)(r)*11 + (long)(g)*16 + (long)(b)*5)>>5)


/*--- Image methods ---*/
ps_image_t *ps_beginImage ___PROTO((c_PostScript *this,
					int width, int height, int depth,
					unsigned short *red,
					unsigned short *green,
					unsigned short *blue));
boolean ps_endImage ___PROTO((c_PostScript *this, ps_image_t *handle));

/*--- Filtering methods ---*/
boolean ps_encode_ASCIIHex ___PROTO((ps_image_t *handle, int len));

/*--- Resources ---*/
boolean ps_includeResource ___PROTO((c_PostScript *this,
				      ps_resource_t type, ...));

#endif /* !__XC_POSTSCRIPT_P_H__ */
