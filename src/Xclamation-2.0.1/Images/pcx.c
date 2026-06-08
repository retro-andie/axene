/*
** pcx.c for Xclamation and XAllWrite in Images/
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
** Started on  Mon Jul 17 17:58:46 1995 Stéphane Boisson
** Last update Sat May 31 19:40:49 1997 Emmanuel Paris
*/

#include "pcx.h"

static boolean	PCXDecodeLine();
static short	PCXReadShort();

static boolean 	Test();
static error	Get_Header();
static error 	Load_Image();

static 	char	*Extensions_Tab[] = { Xc_PCX_EXT1 };

image_load_t	Load_Pcx =
{
  Xc_PCX_NAME,
  Xc_PCX_COPYRIGHT,
  "PCX",
  Xc_PCX_EXTC,
  Extensions_Tab,
  Test,
  Get_Header,
  Load_Image
};

/*------------------------------------------------------------------------*/
static boolean	Test(name)
char	*name;
{
  FILE	*file;
  long  taille;
  int	test;

  if (!(file=fopen(name, "rb")))
    {
      Xc_WARNING(("file %s doesn't exist or unreadable",name));
      return FALSE;
    }
  fseek(file, 0, SEEK_END);
  if ((taille = ftell(file)) < 128)
    {
      Xc_WARNING(("file %s is truncated",name));
      fclose(file);
      return FALSE;
    }
  fseek(file, 0, SEEK_SET);
  
  test = fgetc(file) & 0xff;
  test <<= 16;
  fgetc(file);
  test += fgetc(file) & 0xff;
  if (test != 0x0a0001)
    {
      fclose(file);
      return FALSE;
    }
  return TRUE;
}
/*------------------------------------------------------------------------*/
static error Load_Image(Image, line_buffer , Callback, CallbackData)
c_Image		*Image;
unsigned char	*line_buffer;
boolean		(*Callback)  ___PROTO((unsigned char **pbuf, void *data));
void		*CallbackData;
{
  FILE	*file;
  long  taille;
  int	byte_per_pixel;
  int	byte_per_line_per_plane;
  int	nb_col,nb_lig;
  int	byte_per_line;
  int	depth;
  char	*data;
  char	*plan1,*plan2,*plan3,*plan4;
  int	count,decount;
  int	index,indexl;
  int	nplane;
  int	i,j,oct;
  unsigned short	xmin,ymin,xmax,ymax;
  static char	bit_test[8]={128,64,32,16,8,4,2,1};
  
  Xc_HISTORY(("Load Image ..."));
  if (!(file = fopen(Image->filename, "rb")))
  {
    Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
    return XC_FILE_NOT_EXIST;
  }
  fseek(file, 0, SEEK_END);
  taille = ftell(file);
  fseek(file, 3, SEEK_SET);
  byte_per_pixel = fgetc(file) & 0xff;
  xmin = PCXReadShort(file);
  ymin = PCXReadShort(file);
  xmax = PCXReadShort(file);
  ymax = PCXReadShort(file);
  nb_col = xmax-xmin+1;
  nb_lig = ymax-ymin+1;
  fseek(file, 53, SEEK_CUR);
  nplane = fgetc(file) & 0xff;
  byte_per_line_per_plane = PCXReadShort(file);
  fseek(file, 128, SEEK_SET);
  
  if ((nplane == 1) && (byte_per_pixel == 1))
  {
    depth = 1;
    byte_per_line = (nb_col >>3 ) + ((nb_col & 7) != 0);
  }
  else
  {
    if ((nplane == 4) && (byte_per_pixel == 1))
    {
      depth = 4;
      byte_per_line = nb_col;
    }
    else
    {
      if ((nplane == 1) && (byte_per_pixel == 8))
      {
	depth = 8;
	byte_per_line = nb_col;
      }
      else
      {
	Xc_WARNING(("pcx file format not supported."));
	fclose(file);
	return XC_FILE_UNKNOW_FORMAT;
      }
    }
  }
  
  count = 0;
  switch(depth)
  {
  case 1:
    data = (char *)
      Xc_malloc("data", sizeof(char) * byte_per_line_per_plane);
    for (i = 0; i < Image->height; i++)
    {
      if (!PCXDecodeLine(file, data, byte_per_line_per_plane, &count))
      {
	Xc_free(data);
	Xc_WARNING(("file %s is truncated", Image->filename));
	fclose(file);
	return XC_FILE_TRUNCATED;
      }
      memcpy((void *) line_buffer, data, Image->byte_per_line);
      (Callback)(&line_buffer, CallbackData);
    }
    Xc_free(data);
    break;
  case 4:
    decount = byte_per_line_per_plane * 4;
    data = (char *)
      Xc_malloc("data", sizeof(char) * decount);
    plan1 = data;
    plan2 = plan1 + byte_per_line_per_plane;
    plan3 = plan2 + byte_per_line_per_plane;
    plan4 = plan3 + byte_per_line_per_plane;
    for (i = 0; i < Image->height; i++)
    {
      if (!PCXDecodeLine(file, data, decount, &count))
      {
	Xc_free(data);
	Xc_WARNING(("file %s is truncated", Image->filename));
	fclose(file);
	return XC_FILE_TRUNCATED;
      }
      index = indexl = oct = 0;
      for(j = 0; j < Image->width; j++)
      {
	if (oct == 8)
	{
	  indexl++;
	  oct = 0;
	}
	line_buffer[index++] = (((plan1[indexl] & bit_test[oct]) != 0)<< 3) +
	  (((plan2[indexl] & bit_test[oct]) != 0)<< 2) +
	    (((plan3[indexl] & bit_test[oct]) != 0)<< 1) +
	      ((plan4[indexl] & bit_test[oct]) != 0);
	oct++;
      }
      (Callback)(&line_buffer, CallbackData);
    } 
    Xc_free(data);
    break;
  case 8:
    if (byte_per_line_per_plane == Image->width)
    {
      for (i = 0; i < Image->height; i++)
      {
	if (!PCXDecodeLine(file, line_buffer, Image->width, &count))
	{
	  Xc_WARNING(("file %s is truncated", Image->filename));
	  fclose(file);
	  return XC_FILE_TRUNCATED;
	}
	(Callback)(&line_buffer, CallbackData);
      }      
    }
    else
    {
      data = (char *)
	Xc_malloc("data",sizeof(char)*(byte_per_line_per_plane));
      for (i = 0; i < Image->height; i++)
      {
	if (!PCXDecodeLine(file, data, byte_per_line_per_plane, &count))
	{
	  Xc_free(data);
	  Xc_WARNING(("file %s is truncated", Image->filename));
	  fclose(file);
	  return XC_FILE_TRUNCATED;
	}
	memcpy((void *)line_buffer, data, sizeof(Image->width));
	(Callback)(&line_buffer, CallbackData);
      }      
      Xc_free(data);
    }
    break;
  }
  
  fclose(file);
  return XC_NO_ERROR;
}
/*------------------------------------------------------------------------*/
static error	Get_Header(Image)
c_Image		*Image;
{
  FILE	*file;
  long  taille;
  int	byte_per_pixel;
  int	byte_per_line_per_plane;
  int	nb_col,nb_lig;
  int	nplane, nb_color;
  int	i, gray;
  boolean       grayscale;
  double     	gscale;
  unsigned short xmin,ymin,xmax,ymax;
  
  Xc_HISTORY(("Get Header ..."));

  if (!(file = fopen(Image->filename, "rb")))
  {
    Xc_WARNING(("file %s doesn't exist or unreadable",Image->filename));
    return XC_FILE_NOT_EXIST;
  }
  grayscale = 0;

  fseek(file, 0, SEEK_END);
  taille = ftell(file);
  fseek(file, 3, SEEK_SET);
  byte_per_pixel = fgetc(file) & 0xff;
  xmin = PCXReadShort(file);
  ymin = PCXReadShort(file);
  xmax = PCXReadShort(file);
  ymax = PCXReadShort(file);
  nb_col = xmax - xmin + 1;
  nb_lig = ymax - ymin + 1;
  Image->width = nb_col;
  Image->height = nb_lig;

  fseek(file, 4+16*3+1, SEEK_CUR);

  nplane = fgetc(file) & 0xff;
  nb_color= 1 << (nplane * byte_per_pixel) ;
  if ((nplane == 1) && (byte_per_pixel == 1))
    Image->depth = 1;
  else
  {
    if (((nplane == 4) && (byte_per_pixel == 1)) ||
	((nplane == 1) && (byte_per_pixel == 8)))
      Image->depth = 8;
    else
    {
      Xc_WARNING(("pcx file format not supported."));
      fclose(file);
      return XC_FILE_UNKNOW_FORMAT;
    }
  }
  byte_per_line_per_plane = PCXReadShort(file);
  grayscale = (PCXReadShort(file) == 2);

  Image->pal_r = NULL;
  Image->pal_v = NULL;
  Image->pal_b = NULL;
  
  if (Image->depth == 8)
    {
      Image->pal_r = (unsigned short *)
	Xc_malloc("rouge", sizeof(unsigned short)*256);
      Image->pal_v = (unsigned short *)
	Xc_malloc("vert", sizeof(unsigned short)*256);
      Image->pal_b = (unsigned short *)
	Xc_malloc("bleu", sizeof(unsigned short)*256);
    }
  else
    if (Image->depth == 1)
      {
	Image->pal_r = (unsigned short *)
	  Xc_malloc("rouge", sizeof(unsigned short)*2);
	Image->pal_v = (unsigned short *)
	  Xc_malloc("vert", sizeof(unsigned short)*2);
	Image->pal_b = (unsigned short *)
	  Xc_malloc("bleu", sizeof(unsigned short)*2);
      }
  if (grayscale)
    {
      gray = 0;
      gscale = 0xffff / nb_color;
      for( i = 0; i < nb_color; i++)
	{
	  gray += gscale;
	  Image->pal_r[i] = Image->pal_v[i] = Image->pal_b[i] =  gray;
	}
    }
  else
    switch(nb_color)
      {
      case 2: 
	fseek(file, 16, SEEK_SET);
	Image->pal_r[0] = (fgetc(file) & 0xff) << 8;
	Image->pal_v[0] = (fgetc(file) & 0xff) << 8;
	Image->pal_b[0] = (fgetc(file) & 0xff) << 8;
	Image->pal_r[1] = (fgetc(file) & 0xff) << 8;
	Image->pal_v[1] = (fgetc(file) & 0xff) << 8;
	Image->pal_b[1] = (fgetc(file) & 0xff) << 8;
	break;
      case 16:
	memset(Image->pal_r, 0, sizeof(unsigned short)*256);
	memset(Image->pal_v, 0, sizeof(unsigned short)*256);
	memset(Image->pal_b, 0, sizeof(unsigned short)*256);
	fseek(file, 16, SEEK_SET);
	for( i = 0; i < 16; i++)
	{
	  Image->pal_r[i] = (fgetc(file) & 0xff) << 8;
	  Image->pal_v[i] = (fgetc(file) & 0xff) << 8;
	  Image->pal_b[i] = (fgetc(file) & 0xff) << 8;
	}
	break;
      case 256:
	Xc_TRACE(("256 colors colormap"));
	if (fseek(file, taille - 768, SEEK_SET))
	  {
	    Xc_WARNING(("Truncated file ..."));
	    fclose(file);
	    return (XC_FILE_TRUNCATED);
	  }
	else
	  for( i = 0; i < 256; i++)
	    {
	      Image->pal_r[i]= fgetc(file) << 8;
	      Image->pal_v[i]= fgetc(file) << 8;
	      Image->pal_b[i]= fgetc(file) << 8;
	    }
	break;
      }
  fclose(file);
  Xc_HISTORY(("Get Header ok"));
  return XC_NO_ERROR;
}

static boolean	PCXDecodeLine(file, data, decount, count)
FILE	*file;
char	*data;
int	decount;
int	*count;
{
  int	 index;
  boolean readb , prems;
  char	c = '\0';
  
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
      if ((c & 0xc0) == 0xc0)
      {
	*count = c & 0x3f;
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
static short	PCXReadShort(file)
FILE	*file;
{
  register int c0,c1;
  
  c0 = fgetc(file) & 0xff;
  c1 = fgetc(file) & 0xff;
  return ((short)c1<<8)+(short)c0;
}






