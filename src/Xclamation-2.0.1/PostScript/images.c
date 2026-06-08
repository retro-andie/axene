/*
** images.c for Xclamation, XQuad and XAllWrite in PostScript/
** Methods for bitmap images
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
** Started on  Wed Feb 22 18:05:27 1995 Stéphane Boisson
** Last update Fri Jan  7 15:01:28 2000 Emmanuel Paris
*/

#define NTRACE

#include "PostScriptP.h"


/*--- Image callbacks ---*/
static boolean image_raw ___PROTO((unsigned char **pbuffer,
				   ps_image_t *cb_data));
static boolean image_bitmap2gray ___PROTO((unsigned char **pbuffer,
					   ps_image_t *cb_data));
static boolean image_truecolor2gray ___PROTO((unsigned char **pbuffer,
					      ps_image_t *cb_data));
static boolean image_bitmap ___PROTO((unsigned char **pbuffer,
				      ps_image_t *cb_data));


/* ----------------------------------------------------------------- ** 
** beginImage - Start an inlined image                               ** 
** ----------------------------------------------------------------- */
ps_image_t *ps_beginImage(this, width, height, depth, red, green, blue)
c_PostScript *this;
int width;
int height;
int depth;
unsigned short *red;
unsigned short *green;
unsigned short *blue;
{
 ps_image_t *handle;
 int out_depth;

 Xc_HISTORY(("beginImage(%dx%dx%d)", width, height, depth));
 Xc_ASSERT(!this->ai_mode);

 if((handle = Xc_malloc("image", sizeof(ps_image_t))) == NULL)
 {
  this->error_flag = TRUE;
  return NULL;
 }

 if(this->print_param->Color == TRUE)
  out_depth = (depth > 1)? 24 : 1;
 else
  out_depth = (depth > 1)? 8 : 1;

 /*--- Line buffer stuff ---*/
 handle->line_len = (((long)width * MAX(out_depth, depth)) + 7) >> 3;
 handle->line_buffer = Xc_malloc("line buffer", handle->line_len);
 if(handle->line_buffer == NULL)
 {
  Xc_free(handle);
  this->error_flag = TRUE;
  return NULL;
 }

 /*--- Image info ---*/
 handle->width = width;
 handle->height = height;
 handle->depth = depth;
 handle->byte_depth = (depth + 7) >> 3;
 handle->red = red;
 handle->green = green;
 handle->blue = blue;
 handle->this = this;
 handle->output_index = 0;

 /*--- Loading callback ---*/
 if(depth == 1)
  handle->cb_func = image_raw;
 else if(depth <= 8)
 {
  unsigned char *ptr;
  int i;

  handle->cb_func = (out_depth == 24)? image_bitmap : image_bitmap2gray;
  for(ptr = handle->gray, i = 256; i--; )
   *ptr++ = XcPS_GRAYSCALE(*red++, *green++, *blue++) >> 8;
 }
 else
  handle->cb_func = (out_depth == 24)? image_raw : image_truecolor2gray;

 /*--- Encoding ---*/
 handle->filter = ps_encode_ASCIIHex;

 if(!F(this).putLine(this, "BI") ||
    !F(this).putLine(this, "/Width $d /Height $d /BitsPerComponent $d",
		     (long)width, (long)height, (long)((depth == 1)? 1:8)) ||
    !F(this).putLine(this, "/ColorSpace /Device$s /Filter /ASCIIHexDecode",
		     (out_depth == 24)? "RGB" : "Gray") ||
    !F(this).putLine(this, "%%BeginData $d Hex Bytes",
		     (long)height * handle->line_len) ||
    !F(this).putLine(this, "ID"))
 {
  Xc_free(handle->line_buffer);
  Xc_free(handle);
  return NULL;
 }

 return handle;
}


/* ----------------------------------------------------------------- ** 
** endImage - End an inlined image                                   ** 
** ----------------------------------------------------------------- */
boolean ps_endImage(this, handle)
c_PostScript *this;
ps_image_t *handle;
{
 boolean rc;

 Xc_HISTORY(("endImage"));

 if(handle->height != 0)
 {
  Xc_WARNING(("Missing %d lines", handle->height));
 }

 *(handle->output_buffer + handle->output_index) = 0;
 rc = (F(this).putLine(this, "$s>", handle->output_buffer) && 
       F(this).putLine(this, "%%EndData") &&
       F(this).putLine(this, "EI"));

 Xc_free(handle->line_buffer);
 Xc_free(handle);
 return rc;
}


/* ----------------------------------------------------------------- ** 
** image_raw - Image loading callback without conversion             ** 
** ----------------------------------------------------------------- */
static boolean image_raw(pbuffer, handle)
unsigned char **pbuffer;
ps_image_t *handle;
{
 Xc_TRACE(("image_raw"));

 handle->height--;
 return handle->filter(handle, handle->line_len);
}


/* ----------------------------------------------------------------- ** 
** image_bitmap2gray - Image loading callback for bitmap images      ** 
** ----------------------------------------------------------------- */
static boolean image_bitmap2gray(pbuffer, handle)
unsigned char **pbuffer;
ps_image_t *handle;
{
 register unsigned char *source, *target;
 register long len;

 Xc_TRACE(("image_bitmap2gray"));
  
 handle->height--;

 source = target = handle->line_buffer;
 for(len = handle->line_len; len--; *target++ = handle->gray[*source++]);

 return handle->filter(handle, handle->line_len);
}


/* ----------------------------------------------------------------- ** 
** image_truecolor2gray - Image loading callback for truecolor image ** 
** ----------------------------------------------------------------- */
static boolean image_truecolor2gray(pbuffer, handle)
unsigned char **pbuffer;
ps_image_t *handle;
{
 register unsigned char *source, *target;
 register long len;

 Xc_TRACE(("image_truecolor2gray"));

 handle->height--;
 source = target = handle->line_buffer;
 for(len = handle->line_len; len != 0; len -= handle->byte_depth)
 {
  *target++ = XcPS_GRAYSCALE(*source, *(source+1), *(source+2));
  source += 3;
 }
 return handle->filter(handle, target - handle->line_buffer);
}


/* ----------------------------------------------------------------- ** 
** image_bitmap - Image loading callback for bitmap images           ** 
** ----------------------------------------------------------------- */
static boolean image_bitmap(pbuffer, handle)
unsigned char **pbuffer;
ps_image_t *handle;
{
 register unsigned char *source, *target;
 register long len;

 Xc_TRACE(("image_bitmap"));
 Xc_ASSERT(handle->byte_depth == 1);

 handle->height--;

 source = handle->line_buffer + handle->width;
 target = handle->line_buffer + handle->line_len;
 for(len = handle->width; len != 0; len--)
 {
  *--target = handle->blue[*--source] >> 8;
  *--target = handle->green[*source] >> 8;
  *--target = handle->red[*source] >> 8;
 }

 return handle->filter(handle, handle->line_len);
}
