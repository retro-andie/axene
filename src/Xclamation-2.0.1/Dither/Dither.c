/*
** Dither.c for Xclamation, XAllWrite, XMayday and XInstall in Dither/
** Methods for the Dither class
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
** Started on  Fri Oct 21 17:08:31 1994 Stéphane Boisson
** Last update Thu Jan  6 18:09:12 2000 Emmanuel Paris
*/

#define NTRACE

#include "Dither.h"

#define XcD_GRAYSCALE(r,g,b) (((long)(r)*11 + (long)(g)*16 + (long)(b)*5) >> 5)

/*--- Methods prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();
static boolean dither_24_to_8 ___PROTO((unsigned char **line_buffer,
					c_Dither *this));
static boolean dither_24_to_2 ___PROTO((unsigned char **line_buffer,
					c_Dither *this));
static boolean dither_8_to_2 ___PROTO((unsigned char **line_buffer,
				       c_Dither *this));
static boolean dither_8_to_8 ___PROTO((unsigned char **line_buffer,
				       c_Dither *this));
static boolean dither_8_to_24 ___PROTO((unsigned char **line_buffer,
					c_Dither *this));
static boolean dither_24_to_24 ___PROTO((unsigned char **line_buffer,
					 c_Dither *this));
static boolean dither_8_to_16 ___PROTO((unsigned char **line_buffer,
					c_Dither *this));
static boolean dither_24_to_16 ___PROTO((unsigned char **line_buffer,
					 c_Dither *this));

sf_Dither fc_Dither = {constructor, destructor, copy};


/* ----------------------------------------------------------------- **
** Constructor                                                       **
** ----------------------------------------------------------------- */
static void *constructor(mode, colormap, image, data)
enum dither_mode_e mode;
c_Colormap *colormap;
c_Image *image;
char	*data;
{
 unsigned long size = 0L;
 boolean (*callback)() = NULL;
 unsigned short *red, *green, *blue;
 unsigned int width, height;
 unsigned char *dest;
 c_Dither *p;

 Xc_HISTORY(("constructor mode: %d", mode));

 red = image->pal_r;
 green = image->pal_v;
 blue = image->pal_b;
 width = image->width;
 height = image->height;
 dest = (unsigned char *)data;

 switch(mode)
 {
 case XcD_24_TO_8:
  callback = dither_24_to_8;
  size = sizeof(c_Dither) + ((width + 2) * 3 * sizeof(long));
  break;

 case XcD_24_TO_2:
  callback = dither_24_to_2;
  size = sizeof(c_Dither) + ((width + 2) * sizeof(short));
  break;

 case XcD_8_TO_2:
  callback = dither_8_to_2;
  size = sizeof(c_Dither) + ((width + 258) * sizeof(short));
  break;
    
 case XcD_8_TO_8:
  callback = dither_8_to_8;
  size = sizeof(c_Dither) + (((width + 2) * 3 * sizeof(long)) +
			     (256 * sizeof(color_cells_t)));
  break;

 case XcD_8_TO_24:
  callback = dither_8_to_24;
  size = sizeof(c_Dither) + 256 * sizeof(color_cells_t);
  break;

 case XcD_24_TO_24:
  callback = dither_24_to_24;
  size = sizeof(c_Dither);
  break;

 case XcD_8_TO_16:
  callback = dither_8_to_16;
  size = sizeof(c_Dither) + 256 * sizeof(color_cells_t);
  break;

 case XcD_24_TO_16:
  callback = dither_24_to_16;
  size = sizeof(c_Dither);
  break;

 }

 if((p = Xc_malloc("Dither class", size)) == NULL) return NULL;
 memset(p, 0, size);
 p->f = &fc_Dither;
 p->callback = callback;
 p->colormap = colormap;
 p->size = size;
 p->dest = dest;
 p->width = width;
 p->height = height;

 switch(mode)
 {
  int i;

 case XcD_8_TO_8:
 case XcD_8_TO_16:
 case XcD_8_TO_24:
  {
   color_cells_t *ptr;
	
   ptr = (color_cells_t *)(p + 1);
   for(i = 256; i > 0; i--, ptr++)
    ptr->red = *red++, ptr->green = *green++, ptr->blue = *blue++;
   break;
  }

 case XcD_8_TO_2:
  {
   short *ptr;

   ptr = (short *)(p + 1);
   for(i = 256; i > 0; i--)
    *ptr++ = XcD_GRAYSCALE(*red++, *green++, *blue++) >> 8;
  }
 default:
  break;
 }

 return p;
}


/* ----------------------------------------------------------------- **
** Destructor                                                        **
** ----------------------------------------------------------------- */
static void destructor(this)
c_Dither *this;
{
 Xc_HISTORY(("destructor"));
  
 Xc_free(this);
}


/* ----------------------------------------------------------------- **
** Copy                                                              **
** ----------------------------------------------------------------- */
static void *copy(this)
c_Dither *this;
{
 Xc_HISTORY(("copy (not implemented)"));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** dither_24_to_8 - Dithering 24 bits -> 8 bits                      ** 
** ----------------------------------------------------------------- */
static boolean dither_24_to_8(line_buffer, dither)
unsigned char **line_buffer;
c_Dither *dither;
{
 long *red_error_ptr, *green_error_ptr, *blue_error_ptr;
 long red_error, green_error, blue_error;
 unsigned char *source, *dest;
 c_Colormap *colormap;
 int x;

 Xc_TRACE(("dither_24_to_8"));
    
 source = *line_buffer;
 dest = dither->dest;
 dither->dest += dither->width;
 colormap = dither->colormap;

 /*--- Init error array for current line ---*/
 red_error_ptr = (long *)(dither + 1) + 1;
 red_error = *red_error_ptr;

 green_error_ptr = red_error_ptr + 2 + dither->width;
 green_error = *green_error_ptr;

 blue_error_ptr = green_error_ptr + 2 + dither->width;
 blue_error = *blue_error_ptr;

 *green_error_ptr++ = 0;
 *red_error_ptr++ = 0;
 *blue_error_ptr++ = 0;

 for(x = dither->width; x > 0; x--)
 {
  int index;
  long red, green, blue, tmp;

  red = (((long) *source++ << 12) + red_error + 2048) >> 4;
  if(red > 0xffff) red = 0xffff;
  else if(red < 0) red = 0;

  green = (((long) *source++ << 12) + green_error + 2048) >> 4;
  if(green > 0xffff) green = 0xffff;
  else if(green < 0) green = 0;

  blue = (((long) *source++ << 12) + blue_error + 2048) >> 4;
  if(blue > 0xffff) blue = 0xffff;
  else if(blue < 0) blue = 0;
      
  XcC_INDEX_LOOKUP(colormap, index, red, green, blue);  
      
  /*--- Red propagation ---*/
  red -= XcC_GET_RED(colormap, index);
  red_error = *red_error_ptr;
  tmp = red + red;
  *red_error_ptr++ = red;	/*   1/16   */
  red += tmp;
  red_error_ptr[-2] += red;	/*   3/16   */
  red += tmp;
  red_error_ptr[-3] += red;	/*   5/16   */
  red_error += red + tmp;	/*   7/16   */

  /*--- Green propagation ---*/
  green -= XcC_GET_GREEN(colormap, index);
  green_error = *green_error_ptr;
  tmp = green + green;
  *green_error_ptr++ = green;	/*   1/16   */
  green += tmp;
  green_error_ptr[-2] += green;	/*   3/16   */
  green += tmp;
  green_error_ptr[-3] += green;	/*   5/16   */
  green_error += green + tmp;	/*   7/16   */

  /*--- Blue propagation ---*/
  blue -= XcC_GET_BLUE(colormap, index);
  blue_error = *blue_error_ptr;
  tmp = blue + blue;
  *blue_error_ptr++ = blue;	/*   1/16   */
  blue += tmp;
  blue_error_ptr[-2] += blue;	/*   3/16   */
  blue += tmp;
  blue_error_ptr[-3] += blue;	/*   5/16   */
  blue_error += blue + tmp;	/*   7/16   */

  *dest++ = (unsigned char)index;
 }
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** dither_24_to_2 - Dithering 24 bits -> 2 bits                      ** 
** ----------------------------------------------------------------- */
static boolean dither_24_to_2(line_buffer, dither)
unsigned char **line_buffer;
c_Dither *dither;
{
 short errorv, *error_ptr;
 unsigned char *source, *dest, byte = 0;
 int x, c, i;

 Xc_TRACE(("dither_24_to_2"));

 source = *line_buffer;
 dest = dither->dest;
 dither->dest += (dither->width + 7) >> 3;

 /*--- Init error array for current line ---*/
 error_ptr = (short *)(dither + 1) + 1;
 errorv = *error_ptr;
 *error_ptr++ = 0;

 for(x = dither->width, c = (x + 7) >> 3; c > 0; c--, x -=8 )
 {
  for(i = MIN(8, x); i > 0; i--)
  {
   short value, tmp;
	  
   byte <<= 1;
   value = ((XcD_GRAYSCALE(*source, *(source+1), *(source+2)) << 4)
	    + errorv + 8) >> 4;
   source += 3;
   if(value >= 128)
   {
    value = (value >= 255)? 0 : (value - 255);
    byte |= 1;
   }
   else
    value = MAX(0, value);

   /*--- Error propagation ---*/
   errorv = *error_ptr;
   tmp = value + value;
   *error_ptr++ = value;	/*   1/16   */
   value += tmp;
   error_ptr[-2] += value;	/*   3/16   */
   value += tmp;
   error_ptr[-3] += value;	/*   5/16   */
   errorv += value + tmp;	/*   7/16   */
  }
  *dest++ = byte;
 }
 if(x < 0) 
  dest[-1] = byte << -x;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** dither_8_to_2 - Dithering 8 bits -> 2 bits                        ** 
** ----------------------------------------------------------------- */
static boolean dither_8_to_2(line_buffer, dither)
unsigned char **line_buffer;
c_Dither *dither;
{
 short errorv, *error_ptr, *lum_array;
 unsigned char *source, *dest, byte = 0;
 int x, c, i;

 Xc_TRACE(("dither_8_to_2"));
    
 source = *line_buffer;
 dest = dither->dest;
 dither->dest += (dither->width + 7) >> 3;

 /*--- Init error array for current line ---*/
 lum_array = (short *)(dither + 1);
 error_ptr = lum_array + 257; 
 errorv = *error_ptr;
 *error_ptr++ = 0;

 for(x = dither->width, c = (x + 7) >> 3; c > 0; c--, x -=8 )
 {
  for(i = MIN(8, x); i > 0; i--)
  {
   short value, tmp;
	  
   byte <<= 1;
   value = ((lum_array[*source++] << 4) + errorv + 8) >> 4;
   if(value >= 128)
   {
    value = (value >= 255)? 0 : (value - 255);
    byte |= 1;
   }
   else
    value = MAX(0, value);

   /*--- Error propagation ---*/
   errorv = *error_ptr;
   tmp = value + value;
   *error_ptr++ = value;	/*   1/16   */
   value += tmp;
   error_ptr[-2] += value;	/*   3/16   */
   value += tmp;
   error_ptr[-3] += value;	/*   5/16   */
   errorv += value + tmp;	/*   7/16   */
  }
  *dest++ = byte;
 }
 if(x < 0) dest[-1] = byte << -x;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** dither_8_to_8 - Dithering 8 bits -> 8 bits                        ** 
** ----------------------------------------------------------------- */
static boolean dither_8_to_8(line_buffer, dither)
unsigned char **line_buffer;
c_Dither *dither;
{
 long red_error, green_error, blue_error;
 long *red_error_ptr, *green_error_ptr, *blue_error_ptr;
 unsigned char *source, *dest;
 color_cells_t *cell_base;
 c_Colormap *colormap;
 int x;

 Xc_TRACE(("dither_8_to_8"));
    
 source = *line_buffer;
 dest = dither->dest;
 dither->dest += dither->width;
 cell_base = (color_cells_t *)(dither + 1);
 colormap = dither->colormap;

 /*--- Init error array for current line ---*/
 red_error_ptr = (long *)(cell_base + 256) + 1;
 red_error = *red_error_ptr;

 green_error_ptr = red_error_ptr + 2 + dither->width;
 green_error = *green_error_ptr;

 blue_error_ptr = green_error_ptr + 2 + dither->width;
 blue_error = *blue_error_ptr;

 *green_error_ptr++ = 0;
 *red_error_ptr++ = 0;
 *blue_error_ptr++ = 0;

 for(x = dither->width; x > 0; x--)
 {
  color_cells_t *source_cell;
  long red, green, blue, tmp;
  int index;

  source_cell = cell_base + *source++;
  red = (((long) source_cell->red << 4) + red_error + 2048) >> 4;
  if(red > 0xffff) red = 0xffff;
  else if(red < 0) red = 0;

  green = (((long) source_cell->green << 4) + green_error + 2048) >> 4;
  if(green > 0xffff) green = 0xffff;
  else if(green < 0) green = 0;

  blue = (((long) source_cell->blue << 4) + blue_error + 2048) >> 4;
  if(blue > 0xffff) blue = 0xffff;
  else if(blue < 0) blue = 0;

  XcC_INDEX_LOOKUP(colormap, index, red, green, blue);  
	  
  /*--- Red propagation ---*/
  red -= XcC_GET_RED(colormap, index);
  red_error = *red_error_ptr;
  tmp = red + red;
  *red_error_ptr++ = red;	/*   1/16   */
  red += tmp;
  red_error_ptr[-2] += red;	/*   3/16   */
  red += tmp;
  red_error_ptr[-3] += red;	/*   5/16   */
  red_error += red + tmp;	/*   7/16   */

  /*--- Green propagation ---*/
  green -= XcC_GET_GREEN(colormap, index);
  green_error = *green_error_ptr;
  tmp = green + green;
  *green_error_ptr++ = green;	/*   1/16   */
  green += tmp;
  green_error_ptr[-2] += green;	/*   3/16   */
  green += tmp;
  green_error_ptr[-3] += green;	/*   5/16   */
  green_error += green + tmp;	/*   7/16   */

  /*--- Blue propagation ---*/
  blue -= XcC_GET_BLUE(colormap, index);
  blue_error = *blue_error_ptr;
  tmp = blue + blue;
  *blue_error_ptr++ = blue;	/*   1/16   */
  blue += tmp;
  blue_error_ptr[-2] += blue;	/*   3/16   */
  blue += tmp;
  blue_error_ptr[-3] += blue;	/*   5/16   */
  blue_error += blue + tmp;	/*   7/16   */

  *dest++ = (unsigned char)index;
 }
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** dither_8_to_24 - Dithering 8 bits -> 24 bits                      ** 
** ----------------------------------------------------------------- */
static boolean dither_8_to_24(line_buffer, dither)
unsigned char **line_buffer;
c_Dither *dither;
{
 color_cells_t	cell;
 unsigned char *source, *dest;
 c_Colormap *colormap;
 color_cells_t *cell_base;
 color_cells_t *source_cell;
 unsigned short red, green, blue;
 unsigned long pixel;
 int	x;
  
 Xc_TRACE(("dither_8_to_24"));

 source = *line_buffer;
 dest = dither->dest;
 dither->dest += dither->width * 3;
 colormap = dither->colormap;
 cell_base = (color_cells_t *)(dither + 1);
  
 for(x = dither->width; x > 0; x--)
 {
  source_cell = cell_base + *source++;
  red =  source_cell->red;
  green = source_cell->green;
  blue = source_cell->blue;
      
  F(colormap).get_closest_color(colormap, &cell, red, green, blue);
  pixel = cell.pixel;
      
  *dest++ = (pixel & 0xff0000) >> 16;
  *dest++ = (pixel & 0xff00) >> 8;
  *dest++ = pixel & 0xff;
 }  
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** dither_24_to_24 - Dithering 24 bits -> 24 bits                    ** 
** ----------------------------------------------------------------- */
static boolean dither_24_to_24(line_buffer, dither)
unsigned char **line_buffer;
c_Dither *dither;
{
 color_cells_t	cell;
 unsigned char *source, *dest;
 c_Colormap *colormap;
 unsigned short red, green, blue;
 unsigned long pixel;
 int	x;

 Xc_TRACE(("dither_24_to_24"));
  
 source = *line_buffer;
 dest = dither->dest;
 dither->dest += dither->width * 3;
 colormap = dither->colormap;
  
 for(x = dither->width; x > 0; x--)
 {
  red = (*source++) << 8;
  green = (*source++) << 8;
  blue = (*source++) << 8;

  F(colormap).get_closest_color(colormap, &cell, red, green, blue);
  pixel = cell.pixel;
      
  *dest++ = (pixel & 0xff0000) >> 16;
  *dest++ = (pixel & 0xff00) >> 8;
  *dest++ = pixel & 0xff;
 }  
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** dither_8_to_16 - Dithering 8 bits -> 16 bits                      ** 
** ----------------------------------------------------------------- */
static boolean dither_8_to_16(line_buffer, dither)
unsigned char **line_buffer;
c_Dither *dither;
{
 color_cells_t	cell;
 unsigned char *source, *dest;
 c_Colormap *colormap;
 color_cells_t *cell_base;
 color_cells_t *source_cell;
 unsigned short red, green, blue;
 unsigned long pixel;
 int	x;
  
 Xc_TRACE(("dither_8_to_16"));

 source = *line_buffer;
 dest = dither->dest;
 dither->dest += dither->width * 2;
 colormap = dither->colormap;
 cell_base = (color_cells_t *)(dither + 1);
  
 for(x = dither->width; x > 0; x--)
 {
  source_cell = cell_base + *source++;
  red =  source_cell->red;
  green = source_cell->green;
  blue = source_cell->blue;
      
  F(colormap).get_closest_color(colormap, &cell, red, green, blue);
  pixel = cell.pixel;
      
  *dest++ = (pixel & 0xff00) >> 8;
  *dest++ = pixel & 0xff;
 }  
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** dither_24_to_16 - Dithering 24 bits -> 16 bits                    ** 
** ----------------------------------------------------------------- */
static boolean dither_24_to_16(line_buffer, dither)
unsigned char **line_buffer;
c_Dither *dither;
{
 color_cells_t	cell;
 unsigned char *source, *dest;
 c_Colormap *colormap;
 unsigned short red, green, blue;
 unsigned long pixel;
 int	x;

 Xc_TRACE(("dither_24_to_16"));
  
 source = *line_buffer;
 dest = dither->dest;
 dither->dest += dither->width * 2;
 colormap = dither->colormap;
  
 for(x = dither->width; x > 0; x--)
 {
  red = (*source++) << 8;
  green = (*source++) << 8;
  blue = (*source++) << 8;
      
  F(colormap).get_closest_color(colormap, &cell, red, green, blue);
  pixel = cell.pixel;
      
  *dest++ = (pixel & 0xff00) >> 8;
  *dest++ = pixel & 0xff;
 }  
 return TRUE;
}

