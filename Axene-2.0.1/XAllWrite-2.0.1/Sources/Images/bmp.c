/*
** bmp.c for Xclamation, XAllWrite and XMayday in Images/
** Methods for the reading bmp file (also rle)
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
** Last update Wed Jan 28 12:00:57 1998 Robin Castanier
*/

#include "bmp.h"

static short ReadShort();
static int  ReadInt();
static boolean BMPDecodeRLE8Line ___PROTO((FILE *file,char *data,int decount,
					   int *mode, unsigned int *count, 
					   unsigned int *fill));
static boolean BMPDecodeRLE4Line ___PROTO((FILE *file,char *data,int decount,
					   int *mode, unsigned int *count, 
					   unsigned int *fill));

static boolean	Test();
static error Load_Image();
static error Get_Header();

static 	char *Extensions_Tab[] = { Xc_BMP_EXT1, Xc_BMP_EXT2};

image_load_t Load_Bmp =
{
 Xc_BMP_NAME,
 Xc_BMP_COPYRIGHT,
 "BMP",
 Xc_BMP_EXTC,
 Extensions_Tab,
 Test,
 Get_Header,
 Load_Image
};

static boolean	Test(name)
char	*name;
{
 FILE	*file;
 long  taille;
 long	norm_taille, compression_mode, header_size;
 short filler1, filler2;
 char	c1, c2;
  
 if (!(file = fopen(name, "rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable",name));
  return FALSE;
 }
 fseek(file, 0, SEEK_END);
 taille = ftell(file);
  
 if (taille <= 44)
 {
  Xc_WARNING(("file %s is truncated", name));
  fclose(file);
  return FALSE;
 }
 fseek(file, 0, SEEK_SET);
 c1 = GET_BYTE(file);
 c2 = GET_BYTE(file);
 if (c1 != 'B' || c2 != 'M')
 {
  Xc_TRACE(("file %s wrong bitmap format", name));
  fclose(file);
  return FALSE;
 }
  
 norm_taille = ReadInt(file);
 if (norm_taille > taille)
 {
  fseek(file, 30, SEEK_SET);
  compression_mode = ReadInt(file);
  if (compression_mode != Xc_BI_RLE4 && compression_mode != Xc_BI_RLE8)
  {
   Xc_WARNING(("file %s is truncated", name));
   fclose(file);
   return FALSE;
  }
  fseek(file, 6, SEEK_SET);
 }
 filler1 = ReadShort(file);
 filler2 = ReadShort(file);
 if (filler1 || filler2)
 {
  Xc_WARNING(("file %s wrong bitmap format", name));
  fclose(file);
  return FALSE;
 }
 header_size = ReadInt(file);
 if (taille < header_size)
 {
  Xc_WARNING(("file %s is truncated", name));
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
 long	taille, header_size;
 int	byte_per_line;
 int	i, j, k;
 char	*data;
 int	mode; 
 unsigned int count, fill;
 short real_depth;
 int compression_mode;
  
 Xc_HISTORY(("Load Image ..."));
 if (!(file = fopen(Image->filename,"rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
  return XC_FILE_NOT_EXIST;
 }
 fseek(file, 0, SEEK_END);
 taille = ftell(file);
  
 fseek(file, 10, SEEK_SET);
 header_size = ReadInt(file);
 fseek(file, 28, SEEK_SET);
 real_depth = ReadShort(file);
 compression_mode = ReadInt(file);
 fseek(file, header_size, SEEK_SET);
 
 Xc_TRACE(("Image size: %dx%dx%d compression: %s", Image->width, 
	   Image->height, real_depth, (compression_mode == 0? "NO":"YES") ));
  
 switch(real_depth)
 {
 case 1:
  byte_per_line = ((Image->width >> 3) + 3) & (-4);
  if (taille < header_size + byte_per_line * Image->height)
  {
   Xc_WARNING(("file %s is truncated", Image->filename));
   fclose(file);
   return XC_FILE_TRUNCATED;
  }
  data = (char *)
   Xc_malloc("data", sizeof(char) * byte_per_line);
  for(i = 0; i < Image->height; i++)
  {
   fread( (void *)data, sizeof(char), byte_per_line, file);
   memcpy((void *) line_buffer, data, Image->byte_per_line);
   (Callback)(&line_buffer, CallbackData);
  }
  Xc_free(data);
  break;
 case 4:
  if (compression_mode == Xc_BI_RGB)
  {
   byte_per_line = ((Image->width + 7) >> 3) << 2;
   if (taille < header_size + byte_per_line * Image->height)
   {
    Xc_WARNING(("file %s is truncated", Image->filename));
    fclose(file);
    return XC_FILE_TRUNCATED;
   }
   data = (char *)
    Xc_malloc("data", sizeof(char) * byte_per_line);
   for(i = 0; i < Image->height; i++)
   {
    fread( (void *)data, sizeof(char), byte_per_line, file);
    for (j = 0, k = 0; j < Image->width; j++)
    {
     if ( j & 1)
     {
      line_buffer[j] = data[k] & 0x0f;
      k++;
     }
     else
      line_buffer[j] = (data[k] & 0xf0) >> 4;
    }
    (Callback)(&line_buffer, CallbackData);
   }
   Xc_free(data);
  }
  else
  {
   mode = count = fill = 0;
   byte_per_line = ((Image->width + 7) >> 3) << 3;
   data = (char *)
    Xc_malloc("data", sizeof(char) * byte_per_line);
   for(i = 0; i < Image->height; i++)
   {
    if (!BMPDecodeRLE4Line(file, data, byte_per_line, 
			   &mode, &count, &fill) )
    {
     Xc_WARNING(("file %s is truncated", Image->filename));
     fclose(file);
     return XC_FILE_TRUNCATED;
    }
    memcpy((void *) line_buffer, data, Image->width);
    (Callback)(&line_buffer, CallbackData);
   }
   Xc_free(data);
  }
  break;
 case 8:
  if (compression_mode == Xc_BI_RGB)
  {
   byte_per_line = ((Image->width + 3) >> 2) << 2;
   if (taille < header_size + byte_per_line * Image->height)
   {
    Xc_WARNING(("file %s is truncated", Image->filename));
    fclose(file);
    return XC_FILE_TRUNCATED;
   }
   for(i = 0; i < Image->height; i++)
   {
    fread( (void *)line_buffer, sizeof(char), byte_per_line, file);
    (Callback)(&line_buffer, CallbackData);
   }
  }
  else
  {
   mode = count = fill = 0;
   byte_per_line = ((Image->width + 3) >> 2) << 2;
   for(i = 0; i < Image->height; i++)
   {
    if (!BMPDecodeRLE8Line(file, line_buffer, byte_per_line, 
			   &mode, &count, &fill) )
    {
     Xc_WARNING(("file %s is truncated", Image->filename));
     fclose(file);
     return XC_FILE_TRUNCATED;
    }
    (Callback)(&line_buffer, CallbackData);
   }
  }
  break;
 case 16:
  byte_per_line = ((Image->width * 2 + 3 ) >> 2 ) << 2;
  if (taille < header_size + byte_per_line * Image->height)
  {
   Xc_WARNING(("file %s is truncated", Image->filename));
   fclose(file);
   return XC_FILE_TRUNCATED;
  }
  data = (char *)
   Xc_malloc("data", sizeof(char) * byte_per_line);
  for(i = 0; i < Image->height; i++)
  {
   fread( (void *)data, sizeof(char), byte_per_line, file);
   for (j = k = 0; j < Image->width * 2; j += 2, k += 3)
   {
    fill = (data[j + 1] << 8) + (data[j] & 0x00ff);
    line_buffer[k] = ((fill >> 10) & 0x3f) << 2;	/* blue 5 bits */
    line_buffer[k + 1] = ((fill >> 5) & 0x1f) << 3;	/* green 6 bits */
    line_buffer[k + 2] = (fill & 0x1f) << 3;		/* red 5 bits */
   }
   (Callback)(&line_buffer, CallbackData);
  }
  Xc_free(data);
  break;
 case 24:
  byte_per_line = ((Image->width * 3 + 3 ) >> 2 ) << 2;
  if (taille < header_size + byte_per_line * Image->height)
  {
   Xc_WARNING(("file %s is truncated", Image->filename));
   fclose(file);
   return XC_FILE_TRUNCATED;
  }
  data = (char *)
   Xc_malloc("data", sizeof(char) * byte_per_line);
  for(i = 0; i < Image->height; i++)
  {
   fread( (void *)data, sizeof(char), byte_per_line, file);
   for (j = 0; j < Image->width * 3; j += 3)
   {
    line_buffer[j] = data[j + 2];
    line_buffer[j + 1] = data[j + 1];
    line_buffer[j + 2] = data[j];
   }
   (Callback)(&line_buffer, CallbackData);
  }
  Xc_free(data);
  break;
 }
  
 fclose(file);
 return XC_NO_ERROR;
}

static error Get_Header(Image)
c_Image	*Image;
{
 FILE	*file;
 int	pos, nb_color, i;
 short real_depth;
 int compression_mode;
  
 Xc_HISTORY(("Get Header ..."));
  
 if (!(file = fopen(Image->filename,"rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
  return XC_FILE_NOT_EXIST;
 }
 pos = 14;
 fseek(file, pos, SEEK_SET);
 pos += ReadInt(file);
 Image->width = ReadInt(file);
 Image->height = ReadInt(file);
 if (ReadShort(file) != 1)
 {
  Xc_WARNING(("file %s: error in file format", Image->filename));
  fclose(file);
  return XC_FILE_UNKNOW_FORMAT;
 }
 real_depth = Image->depth = ReadShort(file);
 switch(Image->depth)
 {
 case 1:
  if (ReadInt(file) != 0)
  {
   Xc_WARNING(("file %s: wrong compression mode", Image->filename));
   fclose(file);
   return XC_FILE_UNKNOW_FORMAT;
  }
  Image->pal_r = (unsigned short *)
   Xc_malloc("rouge", sizeof(unsigned short)*2);
  Image->pal_v = (unsigned short *)
   Xc_malloc("vert", sizeof(unsigned short)*2);
  Image->pal_b = (unsigned short *)
   Xc_malloc("bleu", sizeof(unsigned short)*2);
  fseek(file, pos, SEEK_SET);
  Image->pal_b[0] = GET_BYTE(file) << 8;
  Image->pal_v[0] = GET_BYTE(file) << 8;
  Image->pal_r[0] = GET_BYTE(file) << 8;	
  SKIP_BYTE(file);
  Image->pal_b[1] = GET_BYTE(file) << 8;
  Image->pal_v[1] = GET_BYTE(file) << 8;
  Image->pal_r[1] = GET_BYTE(file) << 8;
  break;
 case 4:
  Image->depth = 8;
 case 8:
  compression_mode = ReadInt(file);
  switch(compression_mode)
  {
  case Xc_BI_RGB:
   break;
  case Xc_BI_RLE8:
   if (real_depth != 8)
   {
    Xc_WARNING(("file %s: wrong compression mode", Image->filename));
    fclose(file);
    return XC_FILE_UNKNOW_FORMAT;
   }
   break;
  case Xc_BI_RLE4:
   if (real_depth != 4)
   {
    Xc_WARNING(("file %s: wrong compression mode", Image->filename));
    fclose(file);
    return XC_FILE_UNKNOW_FORMAT;
   }
   break;
  default:
   Xc_WARNING(("file %s: wrong compression mode", Image->filename));
   fclose(file);
   return XC_FILE_UNKNOW_FORMAT;
   break;
  }
  Image->pal_r = (unsigned short *)
   Xc_malloc("rouge", sizeof(unsigned short)*256);
  Image->pal_v = (unsigned short *)
   Xc_malloc("vert", sizeof(unsigned short)*256);
  Image->pal_b = (unsigned short *)
   Xc_malloc("bleu", sizeof(unsigned short)*256);
  memset(Image->pal_r, 0, sizeof(unsigned short)*256);
  memset(Image->pal_v, 0, sizeof(unsigned short)*256);
  memset(Image->pal_b, 0, sizeof(unsigned short)*256);
  nb_color = (1<<real_depth);
  fseek(file, pos, SEEK_SET);
  for( i = 0; i < nb_color; i++)
  {
   Image->pal_b[i] = GET_BYTE(file) << 8;
   Image->pal_v[i] = GET_BYTE(file) << 8;
   Image->pal_r[i] = GET_BYTE(file) << 8;
   SKIP_BYTE(file);
  }
  break;
 case 16:
  Image->depth = 24;
 case 24:
  if (ReadInt(file) != 0)
  {
   Xc_WARNING(("file %s: wrong compression mode", Image->filename));
   fclose(file);
   return XC_FILE_UNKNOW_FORMAT;
  }
  break;
 default:
  Xc_WARNING(("file %s: unknow depth %d", Image->filename, Image->depth));
  fclose(file);
  return XC_FILE_UNKNOW_FORMAT;
  break;
 }
  
 Image->flip_V = TRUE;
 fclose(file);
 return XC_NO_ERROR;
}

static short  ReadShort(file)
FILE	*file;
{
 register int c0,c1;
  
 c0 = fgetc(file) & 0xff;
 c1 = fgetc(file) & 0xff;
 return ((short)c1<<8) + (short)c0;
}

static int  ReadInt(file)
FILE	*file;
{
 register int c0,c1,c2,c3;
  
 c0 = fgetc(file) & 0xff;
 c1 = fgetc(file) & 0xff;
 c2 = fgetc(file) & 0xff;
 c3 = fgetc(file) & 0xff;
 return ((int)c3<<24) + ((int)c2<<16) + ((int)c1<<8) + (int)c0;
}

static boolean BMPDecodeRLE8Line(file, data, decount, mode, count, fill)
FILE	*file;
char	*data;
int	decount;
int	*mode;
unsigned int	*count;
unsigned int	*fill;
{
 char		c;
 int		i, index;
  
 index = 0;
 while(decount)
 {
  switch(*mode)
  {
  case END_OF_LINE:
   memset(data + index, 0, decount);
   decount = 0;
   break;
  case END_OF_BITMAP:
   memset(data + index, 0, decount);
   decount = 0;
   continue;
   break;
  case DELTA:
   if (*fill)
   {
    memset(data + index, 0, decount);
    decount = 0;
    (*count) += index;
    (*fill)--;
    continue;
   }
   if (*count)
   {
    if (decount < (int)*count)
     return FALSE;
    memset(data + index, 0, *count);
    index += *count;
    decount -= *count;
    *count = 0;
    if (!decount)
     continue;
   }
   break;
  case ABSOLUTE_MODE:
   for(i = MIN((int)*count, decount); i ; i--)
   {
    c = GET_BYTE(file);
    if (feof(file))
     return FALSE;
    data[index] = c;
    index++;
    decount--;
    (*count)--;
   }
   if (!*count)
   {
    if (*fill) SKIP_BYTE(file);
   }
   else
   {
    if (!decount)
     continue;
   }
   break;
  case ENCODED_MODE:
   if (decount < (int)*count)
   {
    memset(data + index, *fill, decount);
    (*count) -= decount;
    decount = 0;
    continue;
   }
   else
   {
    memset(data + index, *fill, *count);
    decount -= *count;
    index += *count;
   }
   break;
  default:
   break;
  }
  c = GET_BYTE(file);
  if (feof(file))
   return FALSE;
    
  if (c == 0)
  {
   c = GET_BYTE(file);
   if (feof(file))
    return FALSE;
   switch((int)c)
   {
   case 0:
    if (!decount)
     *mode = XNO_MODE;
    else
     *mode = END_OF_LINE;
    break;
   case 1:
    *mode = END_OF_BITMAP;
    break;
   case 2:
    c = GET_BYTE(file);
    if (feof(file))
     return FALSE;
    *count = (unsigned int)c;
    c = GET_BYTE(file);
    if (feof(file))
     return FALSE;
    *fill = (unsigned int)c;
    *mode = DELTA;
    break;
   default:
    *count = (unsigned int)c;
    *mode = ABSOLUTE_MODE;
    *fill =  (*count & 1);
    break;
   }
  }
  else
  {
   *count = (unsigned int)c;
   *mode = ENCODED_MODE;
   c = GET_BYTE(file);
   if (feof(file))
    return FALSE;
   *fill = (unsigned int)c;
  }
 }
 return TRUE;
}

static boolean BMPDecodeRLE4Line(file, data, decount, mode, count, fill)
FILE	*file;
char	*data;
int	decount;
int	*mode;
unsigned int	*count;
unsigned int	*fill;
{
 char		c = 0;
 int		i, j, index;
  
 index = 0;
 while(decount)
 {
  switch(*mode)
  {
  case END_OF_LINE:
   memset(data + index, 0, decount);
   decount = 0;
   break;
  case END_OF_BITMAP:
   memset(data + index, 0, decount);
   decount = 0;
   continue;
   break;
  case DELTA:
   if (*fill)
   {
    memset(data + index, 0, decount);
    decount = 0;
    (*count) += index;
    (*fill)--;
    continue;
   }
   if (*count)
   {
    if (decount < (int)*count)
     return FALSE;
    memset(data + index, 0, *count);
    index += *count;
    decount -= *count;
    *count = 0;
    if (!decount)
     continue;
   }
   break;
  case ABSOLUTE_MODE:
   j = MIN((int)*count, decount);
   for(i = 0; i < j; i++)
   {
    if ((i & 1) == 0)
    {
     c = GET_BYTE(file);
     if (feof(file))
      return FALSE;
     data[index] = (c & 0xf0) >> 4;
    }
    else
     data[index] = c & 0x0f;
    index++;
    decount--;
    (*count)--;
   }
   if (!*count)
   {
    if (*fill) SKIP_BYTE(file);
   }
   else
   {
    if (!decount)
     continue;
   }
   break;
  case ENCODED_MODE:
   j = MIN(decount, (int)*count);
   for(i = 0; i < j; i++)
   {
    if ((i & 1) == 0)
     data[index] = (*fill & 0xf0) >> 4;
    else
     data[index] = *fill & 0x0f;
    index++;
   }
   if (decount < (int)*count)
   {
    (*count) -= decount;
    decount = 0;
    continue;
   }
   else
   {
    decount -= *count;
   }
   break;
  default:
   break;
  }
  c = GET_BYTE(file);
  if (feof(file))
   return FALSE;
    
  if (c == 0)
  {
   c = GET_BYTE(file);
   if (feof(file))
    return FALSE;
   switch((int)c)
   {
   case 0:
    if (!decount)
     *mode = NO_MODE;
    else
     *mode = END_OF_LINE;
    break;
   case 1:
    *mode = END_OF_BITMAP;
    break;
   case 2:
    c = GET_BYTE(file);
    if (feof(file))
     return FALSE;
    *count = (unsigned char)c;
    c = GET_BYTE(file);
    if (feof(file))
     return FALSE;
    *fill = (unsigned char)c;
    *mode = DELTA;
    break;
   default:
    *count = (unsigned char)c;
    *mode = ABSOLUTE_MODE;
    *fill = ((*count + 1) & 3) >> 1;
    break;
   }
  }
  else
  {
   *count = (unsigned char)c;
   *mode = ENCODED_MODE;
   c = GET_BYTE(file);
   if (feof(file))
    return FALSE;
   *fill = (unsigned int)c;
  }
 }
 return TRUE;
}





