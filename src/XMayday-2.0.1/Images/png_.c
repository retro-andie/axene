/*
** png_.c for Xclamation and XMayday in Images/
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
** Started on  Tue Nov  5 12:53:29 1996 Emmanuel Paris
** Last update Mon Dec 29 15:34:14 1997 Emmanuel Paris
*/

#include "png_.h"
#include "Image.h"

static 	boolean 	IsPNG();
static 	error		LoadImagePNG();
static  error 		GetHeader();

static 	char	*Extensions_Tab[] =  { Xc_PNG_EXT1 };

image_load_t	Load_Png = 
{
 Xc_PNG_NAME,
 Xc_PNG_COPYRIGHT,
 "PNG",
 Xc_PNG_EXTC,
 Extensions_Tab,
 IsPNG,
 GetHeader,
 LoadImagePNG
};

/*---------------------------------------------------------------------------*/
static boolean	IsPNG(name)
char	*name;
{
 FILE *file;
 char test_buffer[32];
  
 Xc_TRACE(("Testing PNG file ... wait ..."));

 if (!(file = fopen(name, "rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable", name));
  return FALSE;
 }
  
 if (fread(test_buffer, 1, 32, file) != 32)
 {
  Xc_WARNING(("file %s is truncated", name));
  fclose(file);
  return FALSE;
 }
 fclose(file);
  
 if (!png_check_sig(test_buffer, 32))
 {
  Xc_TRACE(("Not a PNG file ..."));
  return FALSE;
 }
 return (TRUE);
}

/*---------------------------------------------------------------------------*/
static error LoadImagePNG(Image, line_buffer, call_func, call_data)
c_Image		*Image;
unsigned char 	*line_buffer;
boolean		(*call_func)  ___PROTO((unsigned char **pbuf, void *data));
void		*call_data;
{
 FILE	*file;
 png_structp read_ptr;
 png_infop info_ptr;
 png_infop end_info;
 int x, x2, y, channels;
 char *row_buf, *row_buf2;
  
 Xc_TRACE(("Try to load PNG !!!!!!!"));
  
 if (!(file = fopen(Image->filename, "rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable", Image->filename));
  return XC_FILE_NOT_EXIST;
 }
  
 read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (void *)NULL,
				   (png_error_ptr)NULL,  (png_error_ptr)NULL);
 info_ptr = png_create_info_struct(read_ptr);
 end_info = png_create_info_struct(read_ptr);
 
 if (setjmp(read_ptr->jmpbuf))
 {
  Xc_WARNING(("libpng read error"));
  png_destroy_read_struct(&read_ptr, &info_ptr, &end_info);
  fclose(file);
  return XC_FILE_TRUNCATED;
 }
  
 png_init_io(read_ptr, file);
 png_read_info(read_ptr, info_ptr);
  
 info_ptr->num_trans = 0;
 if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE &&
     info_ptr->bit_depth != 1 && info_ptr->bit_depth < 8)
  png_set_expand(read_ptr);
  
  
 if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY &&
     info_ptr->bit_depth != 1 && info_ptr->bit_depth < 8)
  png_set_expand(read_ptr);
  
 if ((info_ptr->color_type == PNG_COLOR_TYPE_GRAY ||
      info_ptr->color_type == PNG_COLOR_TYPE_PALETTE) &&
     info_ptr->bit_depth == 1)
  png_set_invert_mono(read_ptr);
  
 if (info_ptr->bit_depth == 16)
  png_set_strip_16(read_ptr);
  
 if (info_ptr->valid & PNG_INFO_sBIT)
  png_set_shift(read_ptr, &(info_ptr->sig_bit));
  
 if ((info_ptr->color_type & PNG_COLOR_TYPE_PALETTE)==PNG_COLOR_TYPE_PALETTE)
  channels = 1;
 else
  channels = 3;
 if (info_ptr->color_type & PNG_COLOR_MASK_ALPHA)
  channels++;
  
 png_read_update_info(read_ptr, info_ptr);
  
 if (info_ptr->interlace_type)
 {
  char **row_pointers, *row_buf2;
  int	row, row2;
    
  row_pointers = (char **)Xc_malloc("rp**",
				    sizeof(char *) * info_ptr->height);
  for (row = 0; row < (int)info_ptr->height; row++)
  {
   if ( (row_pointers[row] = (char *)Xc_malloc("rp*", info_ptr->rowbytes))
       == NULL)
   {
    for(row2 = 0; row2 < row; row2 ++)
    {
     Xc_free(row_pointers[row2]);
    }
    Xc_free(row_pointers);
	
    png_destroy_read_struct(&read_ptr, &info_ptr, &end_info);
    fclose(file);
    return XC_FILE_FORMAT_ERROR;
   }
  }

  png_read_image(read_ptr, (png_bytep *)row_pointers);
    
  switch(info_ptr->color_type)
  {
  case PNG_COLOR_TYPE_GRAY:
  case PNG_COLOR_TYPE_PALETTE:
  case PNG_COLOR_TYPE_RGB:
   for (y = 0; y < (int)info_ptr->height; y++)
   {
    memcpy((void *) line_buffer, row_pointers[y], Image->byte_per_line);
    (call_func)(&line_buffer, call_data);
   }
   break;
  case PNG_COLOR_TYPE_RGB_ALPHA:
  case PNG_COLOR_TYPE_GRAY_ALPHA:
   row_buf2 = (char *)Xc_malloc
    ("rb2", (size_t)(info_ptr->width * info_ptr->bit_depth * 3 + 7) >> 3);
      
   for (y = 0; y < (int)info_ptr->height; y++)
   {
    for(x = x2 = 0; x < (int)(info_ptr->width << 2); x += 4, x2 +=3)
    {
     row_buf2[x2] = row_pointers[y][x];
     row_buf2[x2+1] = row_pointers[y][x+1];
     row_buf2[x2+2] = row_pointers[y][x+2];
    }
    memcpy((void *) line_buffer, row_buf2, Image->byte_per_line);
    (call_func)(&line_buffer, call_data);
   }      
      
   free(row_buf2);
   break;
  }
    
  for (row = 0; row < (int)info_ptr->height; row++)
  {
   Xc_free(row_pointers[row]);
  }
  Xc_free(row_pointers);
    
  png_read_end(read_ptr, end_info);
    
  png_destroy_read_struct(&read_ptr, &info_ptr, &end_info);
  fclose(file);
    
  return XC_NO_ERROR;
 }  
 else
 {
  int rowbytes;
  
  rowbytes = ((info_ptr->width * info_ptr->bit_depth * channels + 7) >> 3);
  row_buf = (char *)Xc_malloc("rb", (size_t)rowbytes);
    
  switch(info_ptr->color_type)
  {
  case PNG_COLOR_TYPE_GRAY:
  case PNG_COLOR_TYPE_PALETTE:
  case PNG_COLOR_TYPE_RGB:
   for (y = 0; y < (int)info_ptr->height; y++)
   {
    png_read_row(read_ptr, (png_bytep)row_buf, (png_bytep)0);
    memcpy((void *) line_buffer, row_buf, Image->byte_per_line);
    (call_func)(&line_buffer, call_data);
   }
   break;
  case PNG_COLOR_TYPE_RGB_ALPHA:
  case PNG_COLOR_TYPE_GRAY_ALPHA:
   row_buf2 = (char *)Xc_malloc
    ("rb2", (size_t)(info_ptr->width * info_ptr->bit_depth * 3 + 7) >> 3);
      
   for (y = 0; y < (int)info_ptr->height; y++)
   {
    png_read_row(read_ptr, (png_bytep)row_buf, (png_bytep)0);
	
    for(x = x2 = 0; x < (int)(info_ptr->width << 2); x += 4, x2 +=3)
    {
     row_buf2[x2] = row_buf[x];
     row_buf2[x2+1] = row_buf[x+1];
     row_buf2[x2+2] = row_buf[x+2];
    }
    memcpy((void *) line_buffer, row_buf2, Image->byte_per_line);
    (call_func)(&line_buffer, call_data);
   }      
   Xc_free(row_buf2);
   break;
  }
    
  Xc_free(row_buf);
 }
  
 png_read_end(read_ptr, end_info);
  
 png_destroy_read_struct(&read_ptr, &info_ptr, &end_info);
 fclose(file);
    
 return XC_NO_ERROR;
}
/*---------------------------------------------------------------------------*/

static error 	GetHeader(Image)
c_Image 	*Image;
{
 FILE	*file;
 png_structp read_ptr;
 png_infop info_ptr;
 png_infop end_info;
 int	i, nb_color;
  
 Xc_TRACE(("get info of image '%s'", Image->filename)); 
 if (!(file = fopen(Image->filename, "rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable", Image->filename));
  return XC_FILE_NOT_EXIST;
 }
  
 read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (void *)NULL,
				   (png_error_ptr)NULL,  (png_error_ptr)NULL);
 info_ptr = png_create_info_struct(read_ptr);
 end_info = png_create_info_struct(read_ptr);
 
 if (setjmp(read_ptr->jmpbuf))
 {
  Xc_WARNING(("libpng read error"));
  png_destroy_read_struct(&read_ptr, &info_ptr, &end_info);
  fclose(file);
  return XC_FILE_TRUNCATED;
 }
  
 png_init_io(read_ptr, file);
 png_read_info(read_ptr, info_ptr);
  
 Image->width  = (int)info_ptr->width;
 Image->height = (int)info_ptr->height;
  
 Xc_TRACE((" -------------------------------------------------------- "));
 Xc_TRACE(("Width: %d Height: %d Interlaced: %s",
	   Image->width, Image->height,
	   info_ptr->interlace_type?"True":"False"));
 Xc_TRACE(("Bit Depth: %d Pixel Depth: %d Nb Channels: %d",
	   (int)info_ptr->bit_depth, (int)info_ptr->pixel_depth,
	   (int)info_ptr->channels));
 Xc_TRACE(("Color Type: %d Palette: %p Palette Entries: %d",
	   (int)info_ptr->color_type, info_ptr->palette,
	   (int)info_ptr->num_palette));
 Xc_TRACE((" -------------------------------------------------------- "));
    
 switch(info_ptr->color_type)
 {
 case PNG_COLOR_TYPE_GRAY:
 case PNG_COLOR_TYPE_PALETTE:
  if (info_ptr->pixel_depth == 1)
  {
   Image->depth = 1;
   nb_color = 2;
  }
  else
  {
   Image->depth = 8;
   nb_color = 256;
  }
    
  Image->pal_r = (unsigned short *)
   Xc_malloc("rouge", sizeof(unsigned short) * nb_color);
  memset(Image->pal_r, 0, sizeof(unsigned short) * nb_color);
  Image->pal_v = (unsigned short *)
   Xc_malloc("vert", sizeof(unsigned short) * nb_color);
  memset(Image->pal_v, 0, sizeof(unsigned short) * nb_color);
  Image->pal_b = (unsigned short *)
   Xc_malloc("bleu", sizeof(unsigned short) * nb_color);
  memset(Image->pal_b, 0, sizeof(unsigned short) * nb_color);
    
  if (!info_ptr->palette || info_ptr->num_palette == 0)
  {
   int gray = 0;
   double gscale;
      
   gscale = 0xffff / nb_color;
   for( i = 0; i < nb_color; i++)
   {
    gray += gscale;
    Image->pal_r[i] = Image->pal_v[i] = Image->pal_b[i] =  gray;
   }
  }
  else
  {
   for(i = 0; i < MIN(nb_color, info_ptr->num_palette); i++)
   {
    Image->pal_r[i] = ((unsigned short)info_ptr->palette[i].red << 8);
    Image->pal_v[i] = ((unsigned short)info_ptr->palette[i].green << 8);
    Image->pal_b[i] = ((unsigned short)info_ptr->palette[i].blue << 8);
   }
  }
  break;
 case PNG_COLOR_TYPE_RGB:
 case PNG_COLOR_TYPE_RGB_ALPHA:
 case PNG_COLOR_TYPE_GRAY_ALPHA:
  Image->depth = 24;
  Image->pal_r = Image->pal_v = Image->pal_b = NULL;
  break;
 default:
  png_destroy_read_struct(&read_ptr, &info_ptr, &end_info);
  fclose(file);
  return XC_FILE_UNKNOW_FORMAT;
 }
  
 png_destroy_read_struct(&read_ptr, &info_ptr, &end_info);
 fclose(file);
 return XC_NO_ERROR;
}














