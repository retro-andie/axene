/*
** sun_raster.c for Xclamation and XAllWrite in Images/
** Methods for the reading sun raster file
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
** Started on  Sun Jun 12 02:14:52 1994 Emmanuel Paris
** Last update Thu Oct 24 17:00:13 1996 Emmanuel Paris
*/

#include "sun_raster.h"

static int  ReadInt();
#if 0
static boolean RASDecodeLine ___PROTO((FILE *file,char *data, int decount));
#endif
static boolean Test();
static error Load_Image();
static error Get_Header();

static 	char *Extensions_Tab[] = { Xc_RAS_EXT1 };

image_load_t Load_Ras =
{
  Xc_RAS_NAME,
  Xc_RAS_COPYRIGHT,
  "RAS",
  Xc_RAS_EXTC,
  Extensions_Tab,
  Test,
  Get_Header,
  Load_Image
};

static boolean	Test(name)
char	*name;
{
  FILE	*file;
  long  taille, signature, test;
  
  if (!(file = fopen(name, "rb")))
    {
      Xc_WARNING(("file %s doesn't exist or unreadable",name));
      return FALSE;
    }
  fseek(file, 0, SEEK_END);
  taille = ftell(file);
  
  if (taille <= 32)
  {
    Xc_WARNING(("file %s is truncated", name));
    fclose(file);
    return FALSE;
  }
  fseek(file, 0, SEEK_SET);
  signature = ReadInt(file);
  if (signature != 0x59a66a95)
  {
    Xc_TRACE(("file %s is not a sun raster file", name));
    fclose(file);
    return FALSE;
  }
  test = ReadInt(file);
  if (test < 0 || test > 30000)
  {
    Xc_WARNING(("file %s is not a sun raster file", name));
    fclose(file);
    return FALSE;
  }
  test = ReadInt(file);
  if (test < 0 || test > 30000)
  {
    Xc_WARNING(("file %s is not a sun raster file", name));
    fclose(file);
    return FALSE;
  }
  test = ReadInt(file);
  if ((test != 1) && (test != 8) && (test != 24) && (test != 32))
  {
    Xc_WARNING(("file %s is not a sun raster file", name));
    fclose(file);
    return FALSE;
  }
  fseek(file, 4, SEEK_CUR);
  test = ReadInt(file);
  if ((test != SR_OLD_STYLE) && (test != SR_STANDARD_STYLE) && 
      (test != SR_BYTE_ENCODED) && (test != SR_RGB_FORMAT) && 
      (test != SR_TIFF_FORMAT) && (test != SR_IFF_FORMAT))
  {
    Xc_WARNING(("file %s is not a sun raster file", name));
    fclose(file);
    return FALSE;
  } 
  test = ReadInt(file);
  if ((test != SR_NO_COLORMAP) && (test != SR_COLORMAP) && 
      (test != SR_RAW_COLORMAP))
  {
    Xc_WARNING(("file %s is not a sun raster file", name));
    fclose(file);
    return FALSE;
  }
  fclose(file);
  return TRUE;
}

     
static error Load_Image(Image, line_buffer , Callback, CallbackData)
c_Image		*Image;
unsigned char	*line_buffer;
boolean		(*Callback)  ___PROTO((unsigned char **pbuf, void *data));
void		*CallbackData;
{
  FILE	*file;
  long	taille;
  char	*data;
  int	i, j, byte_per_line;
  int	header_size, real_depth, compression_mode;
  
  Xc_HISTORY(("Load Image ..."));
  if (!(file = fopen(Image->filename,"rb")))
  {
    Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
    return XC_FILE_NOT_EXIST;
  }
  fseek(file, 0, SEEK_END);
  taille = ftell(file);
  
  header_size = 32;
  fseek(file, 12, SEEK_SET);
  real_depth = ReadInt(file);
  fseek(file, 20, SEEK_SET);
  compression_mode = ReadInt(file);
  if (ReadInt(file) == SR_COLORMAP)
    header_size += ReadInt(file);
  
  fseek(file, header_size, SEEK_SET);
  
  Xc_TRACE(("Image size: %dx%dx%d compression: %s" , 
	    Image->width, Image->height, Image->depth, 
	    (compression_mode == SR_BYTE_ENCODED ? "YES" : "NO")));
  
  switch(real_depth)
  {
  case 1:
   if (compression_mode != SR_BYTE_ENCODED)
   {
     byte_per_line = ((Image->width >> 3) + 3) & (-4);
     if (taille < header_size + byte_per_line * Image->height)
     {
       Xc_WARNING(("file %s is truncated", Image->filename));
       fclose(file);
       return XC_FILE_TRUNCATED;
     }
     data = (char *)Xc_malloc("data", sizeof(char) * byte_per_line);
     for(i = 0; i < Image->height; i++)
     {
       fread( (void *)data, sizeof(char), byte_per_line, file);
       memcpy((void *) line_buffer, data, Image->byte_per_line);
       (Callback)(&line_buffer, CallbackData);
     }
     Xc_free(data);
   }
   else
   {
     Xc_WARNING(("file %s is truncated", Image->filename));
     fclose(file);
     return XC_FILE_TRUNCATED;
   }    
   break;
 case 8:
   if (compression_mode != SR_BYTE_ENCODED)
   {
     byte_per_line = Image->width + (Image->width & 1);
     if (taille < header_size + byte_per_line * Image->height)
     {
       Xc_WARNING(("file %s is truncated", Image->filename));
       fclose(file);
       return XC_FILE_TRUNCATED;
     }
     data = (char *)Xc_malloc("data", sizeof(char) * byte_per_line);
     for(i = 0; i < Image->height; i++)
     {
       fread( (void *)data, sizeof(char), byte_per_line, file);
       memcpy((void *) line_buffer, data, Image->byte_per_line);
       (Callback)(&line_buffer, CallbackData);
     }
     Xc_free(data);
   }
   else
   {
     Xc_WARNING(("file %s is truncated", Image->filename));
     fclose(file);
     return XC_FILE_TRUNCATED;
   }
   break;
  case 24:
    if (compression_mode != SR_BYTE_ENCODED)
    {
      byte_per_line = Image->width * 3 + ((Image->width * 3) & 1);
      if (taille < header_size + byte_per_line * Image->height)
      {
	Xc_WARNING(("file %s is truncated", Image->filename));
	fclose(file);
	return XC_FILE_TRUNCATED;
      }
      data = (char *)Xc_malloc("data", sizeof(char) * byte_per_line);
      for(i = 0; i < Image->height; i++)
      {
	fread( (void *)data, sizeof(char), byte_per_line, file);
	for(j = 0; j < Image->width; j++)
	{
	  line_buffer[j*3] = data[j*3+2];
	  line_buffer[j*3+1] = data[j*3+1];
	  line_buffer[j*3+2] = data[j*3];
	}
	(Callback)(&line_buffer, CallbackData);
      }
      Xc_free(data);
    }
    else
    {
      Xc_WARNING(("file %s is truncated", Image->filename));
      fclose(file);
      return XC_FILE_TRUNCATED;
    }
    break;
  case 32:
    if (compression_mode != SR_BYTE_ENCODED)
    {
      byte_per_line = Image->width * 4;
      if (taille < header_size + byte_per_line * Image->height)
      {
	Xc_WARNING(("file %s is truncated", Image->filename));
	fclose(file);
	return XC_FILE_TRUNCATED;
      }
      data = (char *)Xc_malloc("data", sizeof(char) * byte_per_line);
      for(i = 0; i < Image->height; i++)
      {
	fread( (void *)data, sizeof(char), byte_per_line, file);
	for(j = 0; j < Image->width; j++)
	{
	  line_buffer[j*3] = data[j*4+2];
	  line_buffer[j*3+1] = data[j*4+1];
	  line_buffer[j*3+2] = data[j*4];
	}
	(Callback)(&line_buffer, CallbackData);
      }
      Xc_free(data);
    }
    else
    {
      Xc_WARNING(("file %s is truncated", Image->filename));
      fclose(file);
      return XC_FILE_TRUNCATED;
    }
    break;
  }
  
  
  fclose(file);
  return XC_NO_ERROR;
}

static error Get_Header(Image)
c_Image	*Image;
{
  FILE	*file;
  int	nb_color, read_color, i;
  int	header_size, real_depth, colormap_mode;
  
  Xc_HISTORY(("Get Header ..."));
  if (!(file = fopen(Image->filename,"rb")))
  {
    Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
    return XC_FILE_NOT_EXIST;
  }
  
  header_size = 32;
  fseek(file, 4, SEEK_SET);
  
  Image->width = ReadInt(file);
  Image->height = ReadInt(file);
  Image->depth = real_depth = ReadInt(file);
  if (Image->depth == 32) 
    Image->depth = 24;
  
  nb_color = ((Image->depth<=8) ? (1 << Image->depth) : -1);
  
  if (nb_color > 0)
  {
    Image->pal_r = (unsigned short *)
      Xc_malloc("rouge", sizeof(unsigned short) * nb_color);
    Image->pal_v = (unsigned short *)
      Xc_malloc("vert", sizeof(unsigned short) * nb_color);
    Image->pal_b = (unsigned short *)
      Xc_malloc("bleu", sizeof(unsigned short) * nb_color);
  }
  fseek(file, 24, SEEK_SET);
  colormap_mode = ReadInt(file);
  
  switch(colormap_mode)
  {
  case SR_NO_COLORMAP:
  case SR_RAW_COLORMAP:
    for(i=0; i<nb_color; i++)
    {
      Image->pal_r[i] = Image->pal_v[i] = 
	Image->pal_b[i] = (0xffff * i) / (nb_color - 1);
    }
    break;
  case SR_COLORMAP:
    memset(Image->pal_r, 0, sizeof(unsigned short) * nb_color);
    memset(Image->pal_v, 0, sizeof(unsigned short) * nb_color);
    memset(Image->pal_b, 0, sizeof(unsigned short) * nb_color);
    read_color = ReadInt(file) / 3;
    if (read_color > nb_color)
    {
      Xc_WARNING(("file %s has too many color definitions", Image->filename));
      fclose(file);
      return XC_FILE_FORMAT_ERROR; 
    }
    for(i = 0; i < read_color; i++)
      Image->pal_r[i] = GET_BYTE(file) << 8;
    for(i = 0; i < read_color; i++)
      Image->pal_v[i] = GET_BYTE(file) << 8;
    for(i = 0; i < read_color; i++)
      Image->pal_b[i] = GET_BYTE(file) << 8;
    header_size += read_color * 3;
    break;
  }
  return XC_NO_ERROR;
}

static int  ReadInt(file)
FILE	*file;
{
  register int c0,c1,c2,c3;
  
  c0 = fgetc(file) & 0xff;
  c1 = fgetc(file) & 0xff;
  c2 = fgetc(file) & 0xff;
  c3 = fgetc(file) & 0xff;
  return ((int)c0<<24) + ((int)c1<<16) + ((int)c2<<8) + (int)c3;
}

#if 0
static boolean RASDecodeLine(file, data, decount)
FILE	*file;
char	*data;
int	decount;
{
  char	c;
  int	index;
  
  index = 0;
  while(decount)
  {
    c = GET_BYTE(file);
    if (feof(file))
      return FALSE;
    
  }
  return TRUE;
}

#endif
