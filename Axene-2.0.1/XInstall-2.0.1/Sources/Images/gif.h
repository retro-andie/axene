/*
** gif.h for XInstall in Images/
** GIF loading functions. LZW decoder.
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
** Started on  Tue Nov  4 19:47:22 1994 Emmanuel Paris
** Last update Sat Jul  5 13:59:40 1997 Emmanuel Paris
*/

#ifndef _gif_h_
#define _gif_h_

#include "xcalibur.h"
#include "Image.h"

#define Xc_GIF_NAME		"Compuserve GIF"
#define Xc_GIF_COPYRIGHT	"Copyright Xcalibur 1994"

#define Xc_GIF_EXTC		1
#define Xc_GIF_EXT1		"gif"

#define Xc_GIF_HEADER_LENGTH	2048

#define MAX_LZW_BITS    12      /* maximum LZW code size */
#define LZW_TABLE_SIZE (1L << MAX_LZW_BITS)
#define LZW_DEPACK_WORKSIZE (LZW_TABLE_SIZE + 1)

#define EXTENSION	0x21
#define ASPECT_EXT	0x52	/* info du bloc EXTENSION: ne sert pas */
#define COMMENT_EXT	0xfe	/* info du bloc EXTENSION: ne sert pas */
#define TEXT_EXT	0x01	/* info du bloc EXTENSION: ne sert pas */
#define GRAPH_EXT	0xf9	/* info du bloc EXTENSION: ne sert pas */
#define APP_EXT		0xff	/* info du bloc EXTENSION: ne sert pas */
#define IMAGE_SEP	0x2c
#define TRAILER		0x3b
#define	DEPTH_MASK	0x07
#define COLORMAP_MASK	0x80
#define INTERLACE_MASK	0x40

typedef struct
{
  long pos;
  long end;
  int size;
  unsigned char data[260];
  boolean error;
  Xc_FILE *file;
} bitfield_t;

#define bitfieldGetCode(bitfield, rc, code_size, end_code) \
  do{ \
    unsigned char *ptr; \
\
    /*--- Refill test ---*/ \
    if((bitfield.pos + (code_size)) > bitfield.end) \
      { \
	while((bitfieldRefill(&bitfield) != FALSE) && \
	      ((bitfield.pos + (code_size)) > bitfield.end)); \
	if(bitfield.error) \
	  { \
	    rc = (end_code); \
	    break; \
	  } \
      } \
\
    /*--- Build 24bits accumulator ---*/ \
    ptr = bitfield.data + (bitfield.pos >> 3) + 2; \
    rc = *ptr; \
    rc <<= 8; \
    rc |= *--ptr; \
    rc <<= 8; \
    rc |= *--ptr; \
    /* fprintf(stderr, "  GetCode: size=%d acc=%08lx pos=%ld\n", code_size, rc, bitfield.pos); */ \
\
    /*--- Align right ---*/ \
    rc >>= (bitfield.pos & 7); \
    bitfield.pos += (code_size); \
  } while(0)

#endif


