/*
** mac_paint.c for Xclamation in Images/
** Methods for the reading mac_paint file
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
** Last update Wed Jan 28 14:36:12 1998 Robin Castanier
*/

#include "mac_paint.h"

static int  ReadInt();
static boolean MACDecodeLine ___PROTO((FILE *file,char *data, int decount));
static boolean	Test();
static error Load_Image();
static error Get_Header();

static 	char *Extensions_Tab[] = { Xc_MAC_EXT1 };

image_load_t Load_Mac =
{
 Xc_MAC_NAME,
 Xc_MAC_COPYRIGHT,
 "MAC",
 Xc_MAC_EXTC,
 Extensions_Tab,
 Test,
 Get_Header,
 Load_Image
};

static boolean	Test(name)
char	*name;
{
 FILE	*file;
 long  taille, fork_size, ident, header_size;
 char	c, signature[5];
  
 if (!(file = fopen(name, "rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable",name));
  return FALSE;
 }
 fseek(file, 0, SEEK_END);
 taille = ftell(file);
  
 if (taille <= 512)
 {
  Xc_WARNING(("file %s is truncated", name));
  fclose(file);
  return FALSE;
 }
 fseek(file, 0, SEEK_SET);
  
 if (GET_BYTE(file) != '\0')
 {
  Xc_TRACE(("file %s not a MAC-Paint file", name));
  fclose(file);
  return FALSE;
 }
 c = GET_BYTE(file);
 if ((unsigned char)c > 63)
 {
  Xc_WARNING(("file %s not a MAC-Paint file", name));
  fclose(file);
  return FALSE;
 }
 if (c == 0)
 {
  header_size = 0;
 }
 else
 {
  fseek(file, 0x41, SEEK_SET);
  fread( signature, sizeof(char), 4, file);
  signature[4] = '\0';
  if (!Xstrcmp(signature, "PNTG"))
  {
   Xc_WARNING(("file %s not a MAC-Paint file", name));
   fclose(file);
   return FALSE;
  }
  fseek(file, 0x53, SEEK_SET);
  fork_size = ReadInt(file);
  if (fork_size + 0x80 != taille)
  {
   Xc_WARNING(("file %s is truncated", name));
   fclose(file);
   return FALSE;
  }
  fork_size = ReadInt(file);
  if (fork_size != 0)
  {
   Xc_WARNING(("file %s not a MAC-Paint file", name));
   fclose(file);
   return FALSE;
  }
  header_size = 128;
 }
 fseek(file, header_size, SEEK_SET);
 ident = ReadInt(file);
 if (ident != 2)
 {
  Xc_WARNING(("file %s not a MAC-Paint file", name));
  fclose(file);
  return FALSE;
 }  
 taille -= header_size;
 if (taille < 512)
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
 char	*data, c;
 int	i;
  
 Xc_HISTORY(("Load Image ..."));
 if (!(file = fopen(Image->filename,"rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
  return XC_FILE_NOT_EXIST;
 }
 fseek(file, 0, SEEK_END);
 taille = ftell(file);
  
 fseek(file, 1, SEEK_SET);
 c = GET_BYTE(file);
 if (c == 0)
  header_size = 0;
 else
  header_size = 128;
 header_size += 512;
 fseek(file, header_size, SEEK_SET);
  
 Xc_TRACE(("Image size: %dx%dx%d" , Image->width, Image->height, 1));
  
 data = (char *)Xc_malloc("data", sizeof(char) * 72);
 for(i = 0; i < Image->height; i++)
 {
  if (!MACDecodeLine(file, data, 72))
  {
   Xc_free(data);
   Xc_WARNING(("file %s is truncated", Image->filename));
   fclose(file);
   return XC_FILE_TRUNCATED;
  }
  memcpy((void *) line_buffer, data, MIN(Image->byte_per_line, 72));
  (Callback)(&line_buffer, CallbackData);
 }
 Xc_free(data);
 fclose(file);
 return XC_NO_ERROR;
}

static error Get_Header(Image)
c_Image	*Image;
{
 Xc_HISTORY(("Get Header ..."));
  
 Image->width = 576;
 Image->height = 720;
 Image->depth = 1;
 Image->pal_r = (unsigned short *)
  Xc_malloc("rouge", sizeof(unsigned short)*2);
 Image->pal_v = (unsigned short *)
  Xc_malloc("vert", sizeof(unsigned short)*2);
 Image->pal_b = (unsigned short *)
  Xc_malloc("bleu", sizeof(unsigned short)*2);
 Image->pal_r[0] = Image->pal_v[0] = Image->pal_b[0] = 0xffff;
 Image->pal_r[1] = Image->pal_v[1] = Image->pal_b[1] = 0;
  
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

static boolean MACDecodeLine(file, data, decount)
FILE	*file;
char	*data;
int	decount;
{
 char	c;
 int	index;
 unsigned int	count;
  
 index = 0;
 while(decount)
 {
  c = GET_BYTE(file);
  if (feof(file))
   return FALSE;
    
  if (c & 0x80)
  {
   count = (unsigned char)(1-(unsigned char)c);
   c = GET_BYTE(file);
   if (feof(file))
    return FALSE;
   if (decount < (int)count)
    return FALSE;
   memset(data + index, c, count);
  }
  else
  {
   count = (unsigned char)c + 1;
   if (decount < (int)count)
    return FALSE;
   if ((fread(data + index, sizeof(char), count, file) < count)
       && feof(file))
    return FALSE;
  }
  index += count;
  decount -= count;
 }
 return TRUE;
}
