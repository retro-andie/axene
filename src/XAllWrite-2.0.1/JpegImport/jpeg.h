/*
 * jpeg.h		- loading jpeg routine declaration
 *
 * Original routine by Nexes converted by Xcalibur
 *
 * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
 */

#ifndef _jpeg_h_
#define _jpeg_h_

/*
   #include "Image.h"
   #include "Colormap.h"
*/

#include "xcalibur.h" 
#include "Image.h"
#include "Colormap.h"
#include "jinclude.h"
#include <sys/file.h>
#include <setjmp.h>
#include "JpegStd.h"

#define Xc_UNCOMPRESS_MODE	1
#define Xc_TEST_MODE		2
#define Xc_HEADER_MODE		3


typedef struct {
  c_Image	*The_Image;
  int		Position;
  int		Nbr_Color;
  int		Depth;
  unsigned char *Data;
  boolean 		(*Callback) ___PROTO((unsigned char **pbuf, void *data));
  void		*Callback_Data;
} Xc_JPEG_Internal_Data;


#endif






