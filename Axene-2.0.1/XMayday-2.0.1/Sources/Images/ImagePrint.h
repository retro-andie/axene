/*
** ImagePrint.h for XMayday in Images/
** Methods for bitmap images printing
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Wed Feb 22 18:05:27 1995 Stéphane Boisson
** Last update Sat Aug 31 18:03:02 1996 Emmanuel Paris
*/

#ifndef _ImagePrint_h_
#define _ImagePrint_h_

#include "xcalibur.h"

/*--- Macros ---*/
#define XcPS_BYTES_PER_LINE 39
#define XcPS_GRAYSCALE(r,g,b) (((long)(r)*11 + (long)(g)*16 + (long)(b)*5)>>5)

/*--- Structures ---*/
typedef struct ps_image_s ps_image_t;
struct ps_image_s{
  boolean (*cb_func) ___PROTO((unsigned char **pbuffer, ps_image_t *cb_data));
  boolean (*filter) ___PROTO((ps_image_t *handle, int len));
  int (*ps_printf)();
  int width;
  int height;
  int depth;
  int byte_depth;
  unsigned long line_len;
  unsigned char *line_buffer;
  unsigned char output_buffer[(XcPS_BYTES_PER_LINE * 2) + 1];
  int output_index;
  unsigned short *red;
  unsigned short *green;
  unsigned short *blue;
  unsigned char gray[256];
};

/*--- Image methods ---*/
extern ps_image_t *ps_beginImage ___PROTO((int (*ps_printf)(), int color,
					   int width, int height, int depth,
					   unsigned short *red,
					   unsigned short *green,
					   unsigned short *blue));
extern boolean ps_endImage ___PROTO((int (*ps_printf)(), ps_image_t *handle));

#endif /* _ImagePrint_h_ */








