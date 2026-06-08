/*
** tga.c for Xclamation, XAllWrite and XMayday in Images/
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
** Started on  Tue Apr 23 19:45:24 1996 Emmanuel Paris
** Last update Wed Jan 28 14:26:03 1998 Robin Castanier
*/

#include "tga.h"

static boolean	TGADecodeLine();
static short	TGAReadShort();

static boolean 	Test();
static error	Get_Header();
static error 	Load_Image();

static 	char	*Extensions_Tab[] = { Xc_TARGA_EXT1 };

image_load_t	Load_Targa =
{
 Xc_TARGA_NAME,
 Xc_TARGA_COPYRIGHT,
 "TARGA",
 Xc_TARGA_EXTC,
 Extensions_Tab,
 Test,
 Get_Header,
 Load_Image
};

/*---------------------------------------------------------------------------*/
static boolean	Test(name)
char	*name;
{
 FILE	*file;
 long  taille;
 int test;
 unsigned int	id_size;
  
 if (!(file = fopen(name, "rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable",name));
  return FALSE;
 }
  
 fseek(file, 0L, SEEK_END);
 if ((taille = ftell(file)) <  Xc_TARGA_HEADER_LENGTH)
 {
  Xc_WARNING(("file %s is truncated", name));
  fclose(file);
  return FALSE;
 }
 fseek(file, 0L, SEEK_SET);
  
 id_size = (unsigned int)fgetc(file);
  
 test = fgetc(file);
 if (test !=0 && test != 1)
 {
  fclose(file);
  return FALSE;
 }
  
 test = fgetc(file);
 if (test != 1 && test !=2 && test != 3 && 
     test != 9 && test != 10 && test != 11)
 {
  fclose(file);
  return FALSE;
 }
  
 fseek(file, 9L, SEEK_CUR);
 test = fgetc(file) + (fgetc(file) << 8);
 if (test > 30000)
 {
  fclose(file);
  return FALSE;
 }
 test = fgetc(file) + (fgetc(file) << 8);
 if (test > 30000)
 {
  fclose(file);
  return FALSE;
 }
 test = fgetc(file);
 if (test != 1 && test != 8 && test != 24)
 {
  fclose(file);
  return FALSE;
 }
  
 if (taille <= (int)(Xc_TARGA_HEADER_LENGTH + id_size))
 {
  fclose(file);
  return FALSE;
 }
  
 fclose(file);
 return TRUE;
}

/*---------------------------------------------------------------------------*/
static error Get_Header(Image)
c_Image		*Image;
{
 FILE	*file;
 long  taille;
 boolean colormap;
 unsigned short colormap_origin, colormap_length, colorcell_size, value;
 unsigned int id_size;
 int	type, tmp, i;

 Xc_HISTORY(("Get Header ..."));
  
 if (!(file = fopen(Image->filename,"rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable", Image->filename));
  return XC_FILE_NOT_EXIST;
 }
  
 fseek(file, 0L, SEEK_END);
 taille = ftell(file);
 fseek(file, 0L, SEEK_SET);
  
 id_size = (unsigned int)fgetc(file);
  
 colormap = ((fgetc(file) & 0xff) == 1);
 type = fgetc(file);
  
 colormap_origin = TGAReadShort(file);
 colormap_length = TGAReadShort(file);
 colorcell_size = fgetc(file) & 0xff;
  
 fseek(file, 0xc, SEEK_SET);
 Image->width = TGAReadShort(file);
 Image->height = TGAReadShort(file);
 Image->depth = (int)(fgetc(file) & 0xff);
 tmp = fgetc(file);
 if (tmp & 16)
  Image->flip_H = TRUE;
 if (tmp & 32)
  Image->flip_V = TRUE;
  
 fseek(file, Xc_TARGA_HEADER_LENGTH + id_size, SEEK_SET);

 switch(Image->depth)
 {
 case 1:
  Image->pal_r = (unsigned short *)
   Xc_malloc("rouge", sizeof(unsigned short)*2);
  Image->pal_v = (unsigned short *)
   Xc_malloc("vert", sizeof(unsigned short)*2);
  Image->pal_b = (unsigned short *)
   Xc_malloc("bleu", sizeof(unsigned short)*2);
      
  memset(Image->pal_r, 0, sizeof(unsigned short)*2);
  memset(Image->pal_v, 0, sizeof(unsigned short)*2);
  memset(Image->pal_b, 0, sizeof(unsigned short)*2);
      
  if(!colormap)
  {
   Image->pal_r[1] = Image->pal_v[1] = Image->pal_b[1] = 0xffff;
  }
  else
  {
   for(i = colormap_origin;
       i < colormap_origin + colormap_length && i < 2; i++)
   {
    switch(colorcell_size)
    {
    case 16:
     value = TGAReadShort(file);
     Image->pal_r[i] = (value & 0x7C00) << 1;
     Image->pal_v[i] = (value & 0x3E0) << 6;
     Image->pal_b[i] = (value & 0x1F) << 11;
     break;
    case 24:
     Image->pal_b[i] = (fgetc(file) & 0xff) << 8;
     Image->pal_v[i] = (fgetc(file) & 0xff) << 8;
     Image->pal_r[i] = (fgetc(file) & 0xff) << 8;
     break;
    case 32:
     Image->pal_b[i] = (fgetc(file) & 0xff) << 8;
     Image->pal_v[i] = (fgetc(file) & 0xff) << 8;
     Image->pal_r[i] = (fgetc(file) & 0xff) << 8;
     fgetc(file);
     break;
    default:
     fclose(file);
     return XC_FILE_FORMAT_ERROR;
     break;
    }
   }
  }
  break;
 case 8:
  Image->pal_r = (unsigned short *)
   Xc_malloc("rouge", sizeof(unsigned short)*256);
  Image->pal_v = (unsigned short *)
   Xc_malloc("vert", sizeof(unsigned short)*256);
  Image->pal_b = (unsigned short *)
   Xc_malloc("bleu", sizeof(unsigned short)*256);

  memset(Image->pal_r, 0, sizeof(unsigned short)*256);
  memset(Image->pal_v, 0, sizeof(unsigned short)*256);
  memset(Image->pal_b, 0, sizeof(unsigned short)*256);

  if(!colormap)
  {
   for(i=0; i < 256; i++)
   {
    Image->pal_r[i] = Image->pal_v[i] = Image->pal_b[i] = (i << 8);
   }
  }
  else
  {
   for(i = colormap_origin;
       i < colormap_origin + colormap_length && i < 256; i++)
   {
    switch(colorcell_size)
    {
    case 16:
     value = TGAReadShort(file);
     Image->pal_r[i] = (value & 0x7C00) << 1;
     Image->pal_v[i] = (value & 0x3E0) << 6;
     Image->pal_b[i] = (value & 0x1F) << 11;
     break;
    case 24:
     Image->pal_b[i] = (fgetc(file) & 0xff) << 8;
     Image->pal_v[i] = (fgetc(file) & 0xff) << 8;
     Image->pal_r[i] = (fgetc(file) & 0xff) << 8;
     break;
    case 32:
     Image->pal_b[i] = (fgetc(file) & 0xff) << 8;
     Image->pal_v[i] = (fgetc(file) & 0xff) << 8;
     Image->pal_r[i] = (fgetc(file) & 0xff) << 8;
     fgetc(file);
     break;
    default:
     fclose(file);
     return XC_FILE_FORMAT_ERROR;
     break;
    }
   }
  }
  break;
 case 24:
  if (colormap)
  {
   fclose(file);
   return XC_FILE_FORMAT_ERROR;
  }
  Image->pal_r = Image->pal_v = Image->pal_b = NULL;
  break;
 default:
  fclose(file);
  return XC_FILE_FORMAT_ERROR;
  break;
 }
      
 fclose(file);
 return XC_NO_ERROR;
}

/*---------------------------------------------------------------------------*/
static error Load_Image(Image, line_buffer , Callback, CallbackData)
c_Image		*Image;
unsigned char	*line_buffer;
boolean		(*Callback)  ___PROTO((unsigned char **pbuf, void *data));
void		*CallbackData;
{
 FILE	*file;
 long  taille;
 char	*data_buffer;
 unsigned int id_size;
 boolean colormap;
 unsigned short colormap_length; 
 unsigned short colorcell_size, value;
 int	type, i, j;
 int	count;
  
 Xc_HISTORY(("Load Image ..."));
 if (!(file = fopen(Image->filename,"rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
  return XC_FILE_NOT_EXIST;
 }
 fseek(file, 0, SEEK_END);
 taille = ftell(file);
 fseek(file, 0L, SEEK_SET);
  
 id_size = (unsigned int)fgetc(file);
 colormap = ((fgetc(file) & 0xff) == 1);
 type = fgetc(file);
  
 fseek(file, 2L, SEEK_CUR);
 colormap_length = TGAReadShort(file);
 colorcell_size = fgetc(file) & 0xff;
 fseek(file, 10 + id_size + colormap * (colormap_length * 
					(colorcell_size >> 3)), SEEK_CUR);
  
 switch(type)
 {
 case 1:
 case 3:
  for(i = 0; i < Image->height; i++)
  {
   fread(line_buffer, Image->byte_per_line, 1, file);
   (Callback)(&line_buffer, CallbackData);
  }
  break;
 case 2:
  switch(colorcell_size)
  {
  case 16:
   data_buffer = (char *)Xc_malloc("TGA16", 2 * Image->width);
   for(i = 0; i < Image->height; i++)
   {
    fread(data_buffer, Image->width * 2, 1, file);
    for(j = 0; j < Image->width; j++)
    {
     value = ((unsigned char)data_buffer[j*2]) + 
      (((unsigned char)data_buffer[j*2]) << 8);
     line_buffer[j*3] = (value & 0x7C00) << 1;
     line_buffer[j*3+1] = (value & 0x3E0) << 6;
     line_buffer[j*3+2] = (value & 0x1F) << 11;
    }
    (Callback)(&line_buffer, CallbackData);
   }
   Xc_free(data_buffer);
   break;
  case 24:
   for(i = 0; i < Image->height; i++)
   {
    fread(line_buffer, Image->width * 3, 1, file);
    for(j = 0; j < Image->width * 3; j+= 3)
     SWAP(char, line_buffer[j], line_buffer[j + 2]);
	      
    (Callback)(&line_buffer, CallbackData);
   }
   break;
  case 32:
   data_buffer = (char *)Xc_malloc("TGA32", 4 * Image->width);
   for(i = 0; i < Image->height; i++)
   {
    fread(data_buffer, Image->width * 4, 1, file);
    for(j = 0; j < Image->width; j++)
    {
     line_buffer[j * 3 + 2] = data_buffer[j * 4];
     line_buffer[j * 3 + 1] = data_buffer[j * 4 + 1];
     line_buffer[j * 3] = data_buffer[j * 4 + 2];
    }
    (Callback)(&line_buffer, CallbackData);
   }
   Xc_free(data_buffer);
   break;
  }
  break;
 case 9:
 case 11:
  count = 0;
  for(i = 0; i < Image->height; i++)
  {
   if (!TGADecodeLine(file, line_buffer, Image->byte_per_line, &count))
   {
    Xc_WARNING(("file %s is truncated", Image->filename));
    fclose(file);
    return XC_FILE_TRUNCATED;
   }
   (Callback)(&line_buffer, CallbackData);
  }
  break;
 case 10:
  switch(colorcell_size)
  {
  case 16:
   data_buffer = (char *)Xc_malloc("TGA16", 2 * Image->width);
   for(i = 0; i < Image->height; i++)
   {
    if (!TGADecodeLine(file, data_buffer, Image->width * 2, &count))
    {
     Xc_WARNING(("file %s is truncated", Image->filename));
     fclose(file);
     return XC_FILE_TRUNCATED;
    }
    for(j = 0; j < Image->width; j++)
    {
     value = ((unsigned char)data_buffer[j*2]) + 
      (((unsigned char)data_buffer[j*2]) << 8);
     line_buffer[j*3] = (value & 0x7C00) << 1;
     line_buffer[j*3+1] = (value & 0x3E0) << 6;
     line_buffer[j*3+2] = (value & 0x1F) << 11;
    }
    (Callback)(&line_buffer, CallbackData);
   }
   Xc_free(data_buffer);
   break;
  case 24:
   for(i = 0; i < Image->height; i++)
   {
    if (!TGADecodeLine(file, line_buffer, Image->width * 3, &count))
    {
     Xc_WARNING(("file %s is truncated", Image->filename));
     fclose(file);
     return XC_FILE_TRUNCATED;
    }
    for(j = 0; j < Image->width * 3; j+= 3)
     SWAP(char, line_buffer[j], line_buffer[j + 2]);
	      
    (Callback)(&line_buffer, CallbackData);
   }
   break;
  case 32:
   data_buffer = (char *)Xc_malloc("TGA32", 4 * Image->width);
   for(i = 0; i < Image->height; i++)
   {
    if (!TGADecodeLine(file, data_buffer, Image->width * 4, &count))
    {
     Xc_WARNING(("file %s is truncated", Image->filename));
     fclose(file);
     return XC_FILE_TRUNCATED;
    }
    for(j = 0; j < Image->width; j++)
    {
     line_buffer[j * 3 + 2] = data_buffer[j * 4];
     line_buffer[j * 3 + 1] = data_buffer[j * 4 + 1];
     line_buffer[j * 3] = data_buffer[j * 4 + 2];
    }
    (Callback)(&line_buffer, CallbackData);
   }
   Xc_free(data_buffer);
   break;
  }
  break;
 }
 return XC_NO_ERROR;
}

/*------------------------------------------------------------------------*/
static boolean	TGADecodeLine(file, data, decount, count)
FILE	*file;
char	*data;
int	decount;
int	*count;
{
 int	 index;
 boolean readb , prems;
 char	c = 0;
  
 index = 0;
 if (!*count)
  readb = prems = TRUE;
 else
  readb = prems = FALSE;
  
 while(decount)
 {
  if (readb)
  {
   c = fgetc(file);
   if (feof(file))
    return FALSE;
  }
  else
  {
   while(*count)
   {
    data[index++] = c;
    decount--;
    (*count)--;
    if (!decount)
     return TRUE;
   }
   readb = TRUE;
   prems = TRUE;
   continue;
  }
  if (prems)
  {
   if ((c & 0x80) == 0x80)
   {
    *count = c & 0x7f;
    prems = FALSE;
   }
   else
   {
    *count = 1;
    readb = FALSE;
   }
  }
  else
   readb = FALSE;
 }
 return TRUE;
}

/*------------------------------------------------------------------------*/
static short	TGAReadShort(file)
FILE	*file;
{
 register int c0,c1;
  
 c0 = fgetc(file) & 0xff;
 c1 = fgetc(file) & 0xff;
 return ((short)c1<<8)+(short)c0;
}
