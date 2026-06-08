/*
** iff.c for Xclamation in Images/
** Methods for the reading iff file (also rle)
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
** Last update Thu Oct 24 16:56:26 1996 Emmanuel Paris
*/

#include "iff.h"

static short ReadShort();
static int  ReadInt();
static boolean IFFDecodeLine ___PROTO((FILE *file,char *data,int decount,
				       boolean *mode, unsigned int *count, 
				       unsigned char *fill));
static boolean	Test();
static error Load_Image();
static error Get_Header();

static  IFF_Import_t	The_Data;
static 	char *Extensions_Tab[] = { Xc_IFF_EXT1, Xc_IFF_EXT2};

image_load_t Load_Iff =
{
  Xc_IFF_NAME,
  Xc_IFF_COPYRIGHT,
  "IFF",
  Xc_IFF_EXTC,
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
  long	norm_taille, offset;
  char	signature[4];
  boolean	end_of_file;
  
  if (!(file = fopen(name, "rb")))
    {
      Xc_WARNING(("file %s doesn't exist or unreadable",name));
      return FALSE;
    }
  The_Data.File = file;
  fseek(file, 0, SEEK_END);
  taille = ftell(file);
  
  if (taille <= 48)
  {
    Xc_WARNING(("file %s is truncated", name));
    fclose(file);
    return FALSE;
  }
  fseek(file, 0, SEEK_SET);
  
  fread(signature, sizeof(char), 4, file);
  if (!Xstrncmp(signature, "FORM", 4))
  {
    Xc_TRACE(("file %s wrong bitmap format", name));
    fclose(file);
    return FALSE;
  }
  
  norm_taille = ReadInt() + 8;
  if (norm_taille != taille)
  {
    Xc_WARNING(("file %s is truncated", name));
    fclose(file);
    return FALSE;
  }
  
  fread(signature, sizeof(char), 4, file);
  if (!Xstrncmp(signature, "ILBM", 4) && !Xstrncmp(signature, "PBM", 3))
  {
    Xc_WARNING(("file %s wrong bitmap format", name));
    fclose(file);
    return FALSE;
  }
  
  The_Data.header_pos = The_Data.colormap_pos = The_Data.image_pos = 0;
  end_of_file = FALSE;
  
  while(!end_of_file)
  {
    fread(signature, sizeof(char), 4, file);
    if (feof(file))
    {
      Xc_WARNING(("file %s is truncated", name));
      fclose(file);
      return FALSE;
    }
    if (Xstrncmp(signature, "BMHD", 4))
    {
      if (The_Data.header_pos != 0)
      {
	Xc_WARNING(("file %s wrong bitmap format", name));
	fclose(file);
	return FALSE;
      }
      The_Data.header_pos = ftell(file);
    }
    if (Xstrncmp(signature, "CMAP", 4))
    {
      if (The_Data.colormap_pos != 0)
      {
	Xc_WARNING(("file %s wrong bitmap format", name));
	fclose(file);
	return FALSE;
      }
      The_Data.colormap_pos = ftell(file);
    }
    if (Xstrncmp(signature, "BODY", 4))
    {
      if (The_Data.image_pos != 0)
      {
	Xc_WARNING(("file %s wrong bitmap format", name));
	fclose(file);
	return FALSE;
      }
      The_Data.image_pos = ftell(file);
    }
    offset = ReadInt();
    fseek(file, offset, SEEK_CUR);
    if ( (ftell(file) == taille) || (offset >= taille) ) 
      end_of_file = TRUE;
  }
  
  if (!The_Data.header_pos || !The_Data.image_pos)
  {
    Xc_WARNING(("file %s wrong bitmap format", name));
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
  long	taille, chunk_size;
  int	byte_per_line;
  int	i, j, k;
  char	*data;
  boolean  mode; 
  unsigned int count;
  unsigned char fill;
  
  Xc_HISTORY(("Load Image ..."));
  if (!(file = fopen(Image->filename,"rb")))
  {
    Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
    return XC_FILE_NOT_EXIST;
  }
  The_Data.File = file;
  fseek(file, 0, SEEK_END);
  taille = ftell(file);
  fseek(file, The_Data.image_pos, SEEK_SET);
  chunk_size = ReadInt();
  
  Xc_TRACE(("Image size: %dx%dx%d compression: %s", Image->width, 
	    Image->height, The_Data.real_depth, 
	    (The_Data.compression? "YES":"NO") ));
  
  switch(The_Data.real_depth)
  {
  case 1:
    if (!The_Data.compression)
    {
      byte_per_line = ((Image->width >> 3) + 3) & (-4);
      if (taille < The_Data.header_pos + byte_per_line * Image->height)
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
    }
    else
    {
      Xc_WARNING(("file %s is truncated", Image->filename));
      fclose(file);
      return XC_FILE_TRUNCATED; 
    }
   break;
  case 4:
    if (!The_Data.compression)
    {
      byte_per_line = ((Image->width + 7) >> 3) << 2;
      if (taille < The_Data.header_pos + byte_per_line * Image->height)
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
      mode = FALSE; count = fill = 0;
      byte_per_line = ((Image->width + 7) >> 3) << 3;
      data = (char *)
	Xc_malloc("data", sizeof(char) * byte_per_line);
      for(i = 0; i < Image->height; i++)
      {
	if (!IFFDecodeLine(file, data, byte_per_line, 
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
    if (!The_Data.compression)
    {
      if (taille < The_Data.header_pos + Image->width * Image->height)
      {
	Xc_WARNING(("file %s is truncated", Image->filename));
	fclose(file);
	return XC_FILE_TRUNCATED;
      }
      for(i = 0; i < Image->height; i++)
      {
	fread( (void *)line_buffer, sizeof(char), Image->width, file);
	(Callback)(&line_buffer, CallbackData);
      }
    }
    else
    {
      mode = FALSE; count = fill = 0;
      for(i = 0; i < Image->height; i++)
      {
	if (!IFFDecodeLine(file, line_buffer, Image->width, 
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
  case 24:
    if (!The_Data.compression)
    {
      byte_per_line = (Image->width + (Image->width & 1)) * 3;
      if (taille < The_Data.header_pos + byte_per_line * Image->height)
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
  int	comp, nb_color, i;
  
  Xc_HISTORY(("Get Header ..."));
  
  if (!(file = fopen(Image->filename,"rb")))
  {
    Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
    return XC_FILE_NOT_EXIST;
  }
  The_Data.File = file;
  
  fseek(file, The_Data.header_pos + 4, SEEK_SET);
  
  Image->width = ReadShort();
  Image->height = ReadShort();
  
  fseek(file, 4, SEEK_CUR);
  
  The_Data.real_depth = Image->depth = GET_BYTE;
  SKIP_BYTE;
  comp = GET_BYTE; 
  if (comp == 1)  
    The_Data.compression = TRUE;
  else
  {
    if (comp == 0)
      The_Data.compression = FALSE;
    else
    {
      Xc_WARNING(("file %s: wrong compression mode", Image->filename));
      fclose(file);
      return XC_FILE_UNKNOW_FORMAT;
    }
  }
  
  if (The_Data.colormap_pos)
  {
    fseek(file, The_Data.colormap_pos, SEEK_SET);
    nb_color = ReadInt() / 3;
  }
  else
    nb_color = 0;
  
  switch(Image->depth)
  {
  case 1:
    Image->pal_r = (unsigned short *)
      Xc_malloc("rouge", sizeof(unsigned short)*2);
    Image->pal_v = (unsigned short *)
      Xc_malloc("vert", sizeof(unsigned short)*2);
    Image->pal_b = (unsigned short *)
      Xc_malloc("bleu", sizeof(unsigned short)*2);
    if (nb_color == 2)
    {
      Image->pal_b[0] = GET_BYTE << 8;
      Image->pal_v[0] = GET_BYTE << 8;
      Image->pal_r[0] = GET_BYTE << 8;	
      SKIP_BYTE;
      Image->pal_b[1] = GET_BYTE << 8;
      Image->pal_v[1] = GET_BYTE << 8;
      Image->pal_r[1] = GET_BYTE << 8;
    }
    else
    {
      Image->pal_b[0] = Image->pal_v[0] =  Image->pal_r[0] = 0;
      Image->pal_b[1] = Image->pal_v[1] =  Image->pal_r[1] = 0xffff;
    }
    break;
  case 4:
    Image->depth = 8;
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
    if (nb_color)
    {
      for( i = 0; i < nb_color; i++)
      {
	Image->pal_b[i] = GET_BYTE << 8;
	Image->pal_v[i] = GET_BYTE << 8;
	Image->pal_r[i] = GET_BYTE << 8;
	SKIP_BYTE;
      }
    }
    else
    {
      for( i = 0; i < (1<<The_Data.real_depth); i++)
      {
	Image->pal_b[i] = Image->pal_v[i] = Image->pal_r[i] = 
	  (i << (16-The_Data.real_depth));
      }
    }
    break;
  case 24:
    if (nb_color)
    {
      Xc_WARNING(("file %s: 24 bitplanes pictures should have no colormap", 
		  Image->filename));
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
  
  fclose(file);
  return XC_NO_ERROR;
}

static short  ReadShort()
{
  register int c0,c1;
  
  c0 = fgetc(The_Data.File) & 0xff;
  c1 = fgetc(The_Data.File) & 0xff;
  return ((short)c0<<8) + (short)c1;
}

static int  ReadInt()
{
  register int c0,c1,c2,c3;
  
  c0 = fgetc(The_Data.File) & 0xff;
  c1 = fgetc(The_Data.File) & 0xff;
  c2 = fgetc(The_Data.File) & 0xff;
  c3 = fgetc(The_Data.File) & 0xff;
  return ((int)c0<<24) + ((int)c1<<16) + ((int)c2<<8) + (int)c3;
}

static boolean IFFDecodeLine(file, data, decount, mode, count, fill)
FILE	*file;
char	*data;
int	decount;
boolean		*mode;
unsigned int	*count;
unsigned char	*fill;
{
  int	 index;
  char	c;
  
  index = 0;
  
  while(decount)
  {
    if (*mode) 
    {		/* fill data with *count times fill */
      while(*count)
      {
	data[index++] = *fill;
	decount--;
	(*count)--;
	if (!decount)
	  return TRUE;
      }
    }
    else
    {		/* copy *count byte from file to data */
      while(*count)
      {
	c = GET_BYTE;
	if (feof(file))
	  return FALSE;
	data[index++] = c;
	decount--;
	(*count)--;
	if (!decount)
	  return TRUE;
      }
    }
    
    c = GET_BYTE;
    if (feof(file))
      return FALSE;
    if ((unsigned char)c == 0x80)
      continue;
    else
    {
      if ((unsigned char)c < 0x80)
      {
	*count = c + 1;
	*mode = FALSE;
      }
      else
      {
	*count = 0x100 - (unsigned char)c;
	c = GET_BYTE;
	if (feof(file))
	  return FALSE;
	*fill = c;
	*mode = TRUE;
      }
    }
  }
  return TRUE;
}



