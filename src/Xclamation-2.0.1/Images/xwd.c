/*
** xwd.c for Xclamation and XAllWrite in Images/
** Methods for the reading xwd file
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
** Started on  Sun Jun 12 02:14:52 1994 Stéphane Boisson
** Last update Wed Jan 28 11:46:25 1998 Robin Castanier
*/

#include "xwd.h"

static short ReadShort();
static int  ReadInt();
static long ReadHeader();
static long ReadColor();

static boolean	Test();
static error Load_Image();
static error Get_Header();

static 	char *Extensions_Tab[] = { Xc_XWD_EXT1 };

image_load_t Load_Xwd =
{
 Xc_XWD_NAME,
 Xc_XWD_COPYRIGHT,
 "XWD",
 Xc_XWD_EXTC,
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
 long	norm_taille;
 XWDFileHeader header;
  
 if (!(file=fopen(name,"rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable",name));
  return FALSE;
 }
 fseek(file, 0, SEEK_END);
 taille = ftell(file);
  
 if (taille<sz_XWDheader)
 {
  Xc_WARNING(("file %s is truncated",name));
  fclose(file);
  return FALSE;
 }
 fseek(file,0,SEEK_SET);
 if (ReadHeader(&header, file) != sz_XWDheader)
 {
  Xc_WARNING(("file %s: read error",name));
  fclose(file);
  return FALSE;
 }
 norm_taille = header.header_size;
 norm_taille += header.ncolors * sz_XWDColor;
 norm_taille += header.pixmap_height*header.bytes_per_line;
 if (norm_taille != taille)
 {
  Xc_TRACE(("file %s has not the good size",name));
  fclose(file);
  return FALSE;
 }
 if (header.file_version>XWD_FILE_VERSION)
 {
  Xc_WARNING(("file %s version: %d not supported (%d max)",name,
	      header.file_version, XWD_FILE_VERSION));
  fclose(file);
  return FALSE;
 }
  
 switch(header.pixmap_depth)
 {
 case 1:
  if (header.pixmap_format != XYPixmap && 
      header.pixmap_format != XYBitmap &&
      header.pixmap_format != ZPixmap )
  {
   Xc_WARNING(("file %s wrong bitmap format",name));
   fclose(file);
   return FALSE;
  }
  break;
 case 8:
 case 24:
  if (header.pixmap_format != ZPixmap)
  {
   Xc_WARNING(("file %s: pixmap depth %d format not supported",
	       name, header.pixmap_depth));
   fclose(file);
   return FALSE;
  }
  break;
 default:
  Xc_WARNING(("file %s: pixmap depth %d not supported",
	      name, header.pixmap_depth));
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
 long	pos;
 int	i,j;
 int	filler;
 int	line_length;
 boolean is_filler;
 char	oct, bit, svg;
 XWDFileHeader header;
  
 Xc_HISTORY(("Load Image ..."));
 if (!(file = fopen(Image->filename,"rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
  return XC_FILE_NOT_EXIST;
 }
  
 if (ReadHeader(&header, file) != sz_XWDheader)
 {
  Xc_WARNING(("file %s: read error", Image->filename));
  fclose(file);
  return XC_FILE_TRUNCATED;
 }  
  
 pos = header.header_size + header.ncolors * sz_XWDColor;
 fseek(file, pos, SEEK_SET);
  
 filler = ABS((int)header.bytes_per_line - (int)Image->byte_per_line);
 line_length = MIN((int)header.bytes_per_line, (int)Image->byte_per_line);
 is_filler = (int)header.bytes_per_line < (int)Image->byte_per_line;
  
 for(i = 0; i < (int)header.pixmap_height; i++)
 {
  fread( (void *)line_buffer, sizeof(char), line_length, file);
  if (filler)
  {
   if (is_filler)
   {
    for(j=0; j<filler; j++)
     ((char *)line_buffer)[line_length+j] = '\0';
   }
   else
   {
    for(j=0; j<filler; j++)
     fgetc(file);
   }
  }
  if ((Image->depth==1) && (header.bitmap_bit_order==LSBFirst))
   for(j=0; j<Image->byte_per_line; j++)
   {
    svg = ((char *)line_buffer)[j];
    for( oct=bit=0; bit<8; bit++)
    {
     oct <<=1;
     oct |= ((svg & 128)?1:0);
     svg <<=1;
    }
    ((char *)line_buffer)[j] = oct;
   }
  (Callback)(&line_buffer, CallbackData);
 }
 fclose(file);
 return XC_NO_ERROR;
}

static error	Get_Header(Image)
c_Image		*Image;
{
 FILE	*file;
 int	i;
 int	max_color;
 XWDColor	color;
 XWDFileHeader header;
  
 Xc_HISTORY(("Get Header ..."));
  
 if (!(file = fopen(Image->filename,"rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
  return XC_FILE_NOT_EXIST;
 }
 fseek(file, 0, SEEK_SET);
 if (ReadHeader(&header, file) != sz_XWDheader)
 {
  Xc_WARNING(("file %s: read error",Image->filename));
  fclose(file);
  return XC_FILE_TRUNCATED;
 }  
  
 Image->width = header.pixmap_width;
 Image->height = header.pixmap_height;
 Image->depth = header.pixmap_depth;
  
 if (Image->depth == 8)
 {
  max_color = 256;
  Image->pal_r = (unsigned short *)
   Xc_malloc("rouge", sizeof(unsigned short)*256);
  Image->pal_v = (unsigned short *)
   Xc_malloc("vert", sizeof(unsigned short)*256);
  Image->pal_b = (unsigned short *)
   Xc_malloc("bleu", sizeof(unsigned short)*256);
  memset(Image->pal_r, 0, sizeof(unsigned short)*256);
  memset(Image->pal_v, 0, sizeof(unsigned short)*256);
  memset(Image->pal_b, 0, sizeof(unsigned short)*256);
 }
 else
  if (Image->depth == 1)
  {
   max_color = 2;
   Image->pal_r = (unsigned short *)
    Xc_malloc("rouge", sizeof(unsigned short)*2);
   Image->pal_v = (unsigned short *)
    Xc_malloc("vert", sizeof(unsigned short)*2);
   Image->pal_b = (unsigned short *)
    Xc_malloc("bleu", sizeof(unsigned short)*2);
   Image->pal_r[0] = Image->pal_v[0] = Image->pal_b[0] = 0;
   Image->pal_r[1] = Image->pal_v[1] = Image->pal_b[1] = 0xffff;
  }
  else
  {
   fclose(file);
   return XC_NO_ERROR;
  }
  
 fseek(file, header.header_size ,SEEK_SET);
  
 for (i = 0; i < (int)header.ncolors; i++)
 {
  if ( ReadColor(&color, file) == 0)
  {
   Xc_WARNING(("file %s: read error",Image->filename));
   fclose(file);
   return XC_FILE_TRUNCATED;
  }  
  if ((int)color.pixel > max_color)
  {
   Xc_WARNING(("file %s: color definition not in range [0-%d[",
	       Image->filename, max_color));
   fclose(file);
   return XC_FILE_UNKNOW_FORMAT;
  }
  Image->pal_r[color.pixel] = color.red;
  Image->pal_v[color.pixel] = color.green;
  Image->pal_b[color.pixel] = color.blue;
 }
 fclose(file);
 return XC_NO_ERROR;
}

static short  ReadShort(file)
FILE	*file;
{
 register int c0,c1;
  
 c0 = fgetc(file) & 0xff;
 c1 = fgetc(file) & 0xff;
 return ((short)c0<<8) + (short)c1;
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

static long ReadHeader(header, file)
XWDFileHeader	*header;
FILE		*file;
{
 int	i;
  
 for(i=0; i<(sz_XWDheader>>2); i++)
 {
  ((int *)(header))[i] = ReadInt(file);
 } 
 return ftell(file);
}

static long ReadColor(color, file)
XWDColor	*color;
FILE		*file;
{
 color->pixel = ReadInt(file);
 color->red = ReadShort(file);
 color->green = ReadShort(file);
 color->blue = ReadShort(file);
 color->flags = (char)(fgetc(file) & 0xff);
 color->pad = (char)(fgetc(file) & 0xff);

 return ftell(file);
}

