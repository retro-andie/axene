/*
** ImagePrint.c for XMayday in Images/
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
** Last update Thu Jan  6 14:41:45 2000 eparis
*/

#define NTRACE

#include "ImagePrint.h"

/*--- Image callbacks ---*/
static boolean image_raw ___PROTO((unsigned char **pbuffer,
				   ps_image_t *cb_data));
static boolean image_bitmap2gray ___PROTO((unsigned char **pbuffer,
					   ps_image_t *cb_data));
static boolean image_truecolor2gray ___PROTO((unsigned char **pbuffer,
					      ps_image_t *cb_data));
static boolean image_bitmap ___PROTO((unsigned char **pbuffer,
				      ps_image_t *cb_data));
/*--- Filtering methods ---*/
static boolean ps_encode_ASCIIHex ___PROTO((ps_image_t *handle, int len));

/* ----------------------------------------------------------------- ** 
** beginImage - Start an inlined image                               ** 
** ----------------------------------------------------------------- */
ps_image_t *ps_beginImage(ps_printf, color, width, height, depth,
			  red, green, blue)
int (*ps_printf)();
int color;
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

  if((handle = Xc_malloc("image", sizeof(ps_image_t))) == NULL)
      return NULL;

  if(color == 1)
    out_depth = (depth > 1)? 24 : 1;
  else
    out_depth = (depth > 1)? 8 : 1;

  /*--- Line buffer stuff ---*/
  handle->line_len = (((long)width * MAX(out_depth, depth)) + 7) >> 3;
  handle->line_buffer = Xc_malloc("line buffer", handle->line_len);
  if(handle->line_buffer == NULL)
  {
    Xc_free(handle);
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
  handle->ps_printf = ps_printf;
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
  
  if(!ps_printf("save mark\n") ||
     !ps_printf("/Width %d /Height %d /BitsPerComponent %d\n",
		(long)width, (long)height, (long)((depth == 1)? 1:8)) ||
     !ps_printf("/ColorSpace /Device%s /Filter /ASCIIHexDecode\n",
		(out_depth == 24)? "RGB" : "Gray") ||
     !ps_printf("%%BeginData %d Hex Bytes\n",
		(long)height * handle->line_len) ||
     !ps_printf("ID\n"))
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
boolean ps_endImage(ps_printf, handle)
int (*ps_printf)();
ps_image_t *handle;
{
  boolean rc;
  
  Xc_HISTORY(("endImage"));

  if(handle->height != 0)
    {
      Xc_WARNING(("Missing %d lines", handle->height));
    }

  *(handle->output_buffer + handle->output_index) = 0;
  rc = (ps_printf("%s>\n", handle->output_buffer) && 
	ps_printf("%%%%%%%%EndData\n") &&
	ps_printf("restore\n"));

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

/* ----------------------------------------------------------------- ** 
** encode_ASCIIHex - ASCII Hex PDF encoding method                   ** 
** ----------------------------------------------------------------- */
static boolean ps_encode_ASCIIHex(handle, len)
ps_image_t *handle;
int len;
{
  register unsigned char *source, *target, *end;
  static char *hex_table = "0123456789ABCDEF";
  register boolean rc;

  Xc_TRACE(("encode_ASCIIHex"));

  source = handle->line_buffer;
  target = handle->output_buffer + handle->output_index;
  end = handle->output_buffer + (XcPS_BYTES_PER_LINE * 2);
  rc = TRUE;

  while(len > 0)
    {
      while((target < end) && (len-- > 0) && (rc = TRUE))
	{
	  register unsigned char c;
	  
	  c = *source++;
	  *target++ = hex_table[c >> 4];
	  *target++ = hex_table[c & 0xf];
	}

      if(len > 0)
	{
	  *target = 0;
	  target = handle->output_buffer;
	  rc = handle->ps_printf("%s\n", handle->output_buffer); 
	}
    }
  handle->output_index = target - handle->output_buffer;
  
  return rc;
}


