/*
** ppm.c for Xclamation and XAllWrite in Images/
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
** Last update Fri Jan  7 14:59:46 2000 Emmanuel Paris
*/

#include "ppm.h"
#include "Image.h"

static 	boolean 	IsPPM();
static 	error		LoadImagePPM();
static  error 		GetHeader();

static 	char	*Extensions_Tab[] =  { Xc_PPM_EXT1, Xc_PPM_EXT2, Xc_PPM_EXT3 };

image_load_t	Load_Ppm = 
{
 Xc_PPM_NAME,
 Xc_PPM_COPYRIGHT,
 "PPM",
 Xc_PPM_EXTC,
 Extensions_Tab,
 IsPPM,
 GetHeader,
 LoadImagePPM
};

/*---------------------------------------------------------------------------*/
static boolean	IsPPM(name)
char	*name;
{
 FILE *file;
 char test_buffer[200];
 char *str, *str2;
 int  fsize;
 boolean ascii;
 int	header_length, depth;
 int	width, height;
  
 Xc_TRACE(("Testing PPM file ... wait ..."));

 if (!(file = fopen(name, "rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable", name));
  return FALSE;
 }
  
 fseek(file, 0L, SEEK_END);
 fsize = ftell(file);
 fseek(file, 0L, SEEK_SET);
 
 if (fread(test_buffer, 1, 8, file) != 8)
 {
  Xc_WARNING(("file %s is truncated", name));
  fclose(file);
  return FALSE;
 }
  
 if (*test_buffer != 'P')
 {
  Xc_TRACE(("Not a PPM file ..."));
  fclose(file);
  return FALSE;
 }
  
 switch(test_buffer[1])
 {
 case '1':
  depth = 1; ascii = TRUE;
  break;
 case '2':
  depth = 8; ascii = TRUE;
  break;
 case '3':
  depth = 24; ascii = TRUE;
  break;
 case '4':
  depth = 1; ascii = FALSE;
  break;
 case '5':
  depth = 8; ascii = FALSE;
  break;
 case '6':
  depth = 24; ascii = FALSE;
  break;    
 default:
  Xc_TRACE(("Not a PPM file ..."));
  fclose(file);
  return FALSE;
  break;
 }
  
 fseek(file, 0L, SEEK_SET);
 fgets(test_buffer, 200, file);
 while( fgets(test_buffer, 200, file) && *test_buffer == '#') ;
  
 width = (int)strtol(test_buffer, &str, 10);
 if (str == test_buffer)
 {
  Xc_TRACE(("Not a PPM file ..."));
  fclose(file);
  return FALSE;
 }
 height = (int)strtol(++str, &str2, 10);
 if (str2 == str)
 {
  Xc_TRACE(("Not a PPM file ..."));
  fclose(file);
  return FALSE;
 }
  
 if (depth != 1)
  fgets(test_buffer, 200, file);
 header_length = ftell(file);
 fclose(file);
  
 if (!ascii)
 {
  switch(depth)
  {
  case 1:
   if (fsize == ((width + 7) >> 3) * height + header_length)
    break;
  case 8:
   if (fsize == width * height + header_length)
    break;
  case 24:
   if (fsize == width * 3 * height + header_length)
    break;
   Xc_WARNING(("files %s has a wrong size.", name));
   break;
  }
 }
 else
 {
  switch(depth)
  {
  case 1:
   if (fsize > width * height + header_length)
    break;
  case 8:
   if (fsize > width * 3 * height + header_length)
    break;
  case 24:
   if (fsize > width * 12 * height + header_length)
    break;
   Xc_WARNING(("files %s has a wrong size.", name));
   break;
  }    
 }
 return TRUE;
}


/*---------------------------------------------------------------------------*/
static error LoadImagePPM(Image, line_buffer, call_func, call_data)
c_Image		*Image;
unsigned char 	*line_buffer;
boolean		(*call_func)  ___PROTO((unsigned char **pbuf, void *data));
void		*call_data;
{
 FILE	*file;
 char test_buffer[200];
 boolean ascii;
 int x, y, byte_per_line;
  
 Xc_TRACE(("Try to load PPM !!!!!!!"));
  
 if (!(file = fopen(Image->filename, "rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable", Image->filename));
  return XC_FILE_NOT_EXIST;
 }
  
 fread(test_buffer, 1, 8, file);
 switch(test_buffer[1])
 {
 case '1':
 case '2':
 case '3':
  ascii = TRUE;
  break;
 case '4':
 case '5':
 case '6':
 default:
  ascii = FALSE;
  break;
 }
  
 fseek(file, 0L, SEEK_SET);
 fgets(test_buffer, 200, file);
 while( fgets(test_buffer, 200, file) && *test_buffer == '#') ;
 if (Image->depth != 1)
  fgets(test_buffer, 200, file);
  
 if (ascii)
 {
  switch(Image->depth)
  {
  case 1:
   {
    char c, bitm;
    char *ptr, *lbptr;
    int k;
	
    if (! fgets(test_buffer, 200, file))
    {
     fclose(file);
     return XC_FILE_TRUNCATED;
    }
    ptr = test_buffer;
    for(y = 0; y < Image->height; y++)
    {
     k = -1; bitm = 0; lbptr = (char *)line_buffer;
     for(x = 0; x < Image->width; x++)
     {
      do
      {
       c = *ptr++;
       if (c == '\n')
       {
	if (! fgets(test_buffer, 200, file))
	{
	 fclose(file);
	 return XC_FILE_TRUNCATED;
	}
	ptr = test_buffer;
	c = *ptr++;
       }
      }
      while( c != '0' && c != '1');
	    
      if (k == 7)
      {
       *lbptr ++ = bitm;
       k = 0;
       bitm = (c - '0');
      }
      else
      {
       k++;
       bitm <<= 1;
       bitm |= (c - '0');
      }
     }
     if (k != 0)
      *lbptr ++ = ( bitm << (8 - k));
	  
     (call_func)(&line_buffer, call_data);
    }
   }
   break;
  case 8:
   {
    char c;
    char *ptr, *lbptr;
    int k;
	
    if (! fgets(test_buffer, 200, file))
    {
     fclose(file);
     return XC_FILE_TRUNCATED;
    }
    ptr = test_buffer;
    for(y = 0; y < Image->height; y++)
    {
     lbptr = (char *)line_buffer;
     for(x = 0; x < Image->width; x++)
     {
      do
      {
       c = *ptr++;
       if (c == '\n')
       {
	if (! fgets(test_buffer, 200, file))
	{
	 fclose(file);
	 return XC_FILE_TRUNCATED;
	}
	ptr = test_buffer;
	c = *ptr++;
       }
      }
      while( c < '0' || c > '9');
      k = c - '0';
      c = *ptr++;
      while(c >= '0' && c <= '9')
      {
       k = k * 10 + c - '0';
       c = *ptr++; 
      }
      ptr--;
      *lbptr++ = (unsigned char)k;
     } 
     (call_func)(&line_buffer, call_data);
    }
   }
   break;
  case 24:
   {
    char c;
    char *ptr, *lbptr;
    int k;
	
    if (! fgets(test_buffer, 200, file))
    {
     fclose(file);
     return XC_FILE_TRUNCATED;
    }
    ptr = test_buffer;
    for(y = 0; y < Image->height; y++)
    {
     lbptr = (char *)line_buffer;
     for(x = 0; x < Image->width * 3; x++)
     {
      do
      {
       c = *ptr++;
       if (c == '\n')
       {
	if (! fgets(test_buffer, 200, file))
	{
	 fclose(file);
	 return XC_FILE_TRUNCATED;
	}
	ptr = test_buffer;
	c = *ptr++;
       }
      }
      while( c < '0' || c > '9');
      k = c - '0';
      c = *ptr++;
      while(c >= '0' && c <= '9')
      {
       k = k * 10 + c - '0';
       c = *ptr++; 
      }
      ptr--;
      *lbptr++ = (unsigned char)k;
     } 
     (call_func)(&line_buffer, call_data);
    }
   }
   break;
  }
 }
 else
 {
  switch(Image->depth)
  {
  case 1:
   byte_per_line = (Image->width + 7) >> 3;
   for(y = 0; y < Image->height; y++)
   {
    if ((int)fread(line_buffer, 1, byte_per_line, file) != byte_per_line)
    {
     fclose(file);
     return XC_FILE_TRUNCATED;
    }
    (call_func)(&line_buffer, call_data);
   }
   break;
  case 8:
   byte_per_line = Image->width;
   for(y = 0; y < Image->height; y++)
   {
    if ((int)fread(line_buffer, 1, byte_per_line, file) != byte_per_line)
    {
     fclose(file);
     return XC_FILE_TRUNCATED;
    }
    (call_func)(&line_buffer, call_data);
   }
   break;
  case 24:
   byte_per_line = Image->width * 3;
   for(y = 0; y < Image->height; y++)
   {
    if ((int)fread(line_buffer, 1, byte_per_line, file) != byte_per_line)
    {
     fclose(file);
     return XC_FILE_TRUNCATED;
    }
    (call_func)(&line_buffer, call_data);
   }
   break;
  }
 }
 
 fclose(file);
    
 return XC_NO_ERROR;
}

/*---------------------------------------------------------------------------*/
static error 	GetHeader(Image)
c_Image 	*Image;
{
 FILE	*file;
 char test_buffer[200];
 char *str, *str2;
 int	width, height;
 int	i, depth;
  
 Xc_TRACE(("get info of image '%s'", Image->filename)); 
 if (!(file = fopen(Image->filename, "rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable", Image->filename));
  return XC_FILE_NOT_EXIST;
 }
  
 fread(test_buffer, 1, 8, file);
  
 switch(test_buffer[1])
 {
 case '1':
 case '4':
  depth = 1;
  break;
 case '2':
 case '5':
  depth = 8;
  break;
 case '3':
 case '6':
 default:
  depth = 24;
  break;    
 }
  
 fseek(file, 0L, SEEK_SET);
 fgets(test_buffer, 200, file);
 while( fgets(test_buffer, 200, file) && *test_buffer == '#') ;
  
 fclose(file);
  
 width = (int)strtol(test_buffer, &str, 10);
 height = (int)strtol(++str, &str2, 10);
  
 Image->depth = depth;
 Image->width = width;
 Image->height = height;
  
 switch(depth)
 {
 case 1:
  Image->pal_r = (unsigned short *)
   Xc_malloc("r", sizeof(unsigned short) * 2);
  Image->pal_v = (unsigned short *)
   Xc_malloc("g", sizeof(unsigned short) * 2);
  Image->pal_b = (unsigned short *)
   Xc_malloc("b", sizeof(unsigned short) * 2);
  Image->pal_r[0] = Image->pal_v[0] = Image->pal_b[0] = 0x0000;
  Image->pal_r[1] = Image->pal_v[1] = Image->pal_b[1] = 0xffff;
  break;
 case 8:
  Image->pal_r = (unsigned short *)
   Xc_malloc("r", sizeof(unsigned short) * 256);
  Image->pal_v = (unsigned short *)
   Xc_malloc("g", sizeof(unsigned short) * 256);
  Image->pal_b = (unsigned short *)
   Xc_malloc("b", sizeof(unsigned short) * 256);
  for(i = 0; i < 256; i++)
  {
   Image->pal_r[i] = (unsigned short)(i << 8);
   Image->pal_v[i] = (unsigned short)(i << 8);
   Image->pal_b[i] = (unsigned short)(i << 8);
  }
  break;
 case 24:
  Image->pal_r = Image->pal_v = Image->pal_b = NULL;
  break;
 }
  
 return XC_NO_ERROR;
}







