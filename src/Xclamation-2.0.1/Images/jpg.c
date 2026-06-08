/*
** jpg.c for Xclamation, XAllWrite and XMayday in Images/
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
** Started on  Mon Jul 17 18:25:05 1995 Marc Blet
** Last update Fri May 10 19:03:24 1996 Emmanuel Paris
*/

#include "jpg.h"
#include "Image.h"

extern int	Test_Jpeg_Image();
extern int	Get_Jpeg_Header();
extern int	Load_Jpeg_File();

static 	boolean 	IsJPEG();
static 	error		LoadImageJPEG();
static  error 		GetHeader();

static 	char	*Extensions_Tab[] =  { Xc_JPEG_EXT1, Xc_JPEG_EXT2 };

image_load_t	Load_Jpeg = 
{
  Xc_JPEG_NAME,
  Xc_JPEG_COPYRIGHT,
  "JPEG",
  Xc_JPEG_EXTC,
  Extensions_Tab,
  IsJPEG,
  GetHeader,
  LoadImageJPEG
};

/*---------------------------------------------------------------------------*/
static boolean	IsJPEG(name)
char	*name;
{
  Xc_TRACE(("Testing JPEG file ... wait ..."));

  if (Test_Jpeg_Image(name) == FALSE)
    {
      Xc_TRACE(("Not a JPEG file ... NOt a JPEG file ..."));
      return (FALSE);
    }
  return (TRUE);
}
/*---------------------------------------------------------------------------*/
static error LoadImageJPEG(The_Image, line_buf, call_func, call_data)
c_Image		*The_Image;
unsigned char 	*line_buf;
boolean		(*call_func)  ___PROTO((unsigned char **pbuf, void *data));
void		*call_data;

{
  Xc_TRACE(("Try to load JPEG !!!!!!!"));

  return (Uncompress_Jpeg_Image(The_Image, line_buf, call_func, call_data));
}
/*---------------------------------------------------------------------------*/

static error 	GetHeader(Image)
c_Image 	*Image;
{
  Xc_TRACE(("get info of image '%s'",Image->filename)); 
  
  return (Get_Jpeg_Header(Image));
}


