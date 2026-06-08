/*
** tiff.c for Xclamation in Images/
** Tiff loading
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
** Started on  Tue May 16 13:30:11 1995 Antoine Buat
** Last update Sat Oct 26 16:45:29 1996 Emmanuel Paris
*/

#include "Image.h"
#include "tiffio.h"

static 	boolean 	IsTiff();
static  error 		GetHeader();
static 	error		LoadImageTiff();

static 	char	*Extensions_Tab[] =  { "tif", "tiff"};

image_load_t	Load_Tiff = 
{
  "Tiff - Tag Image File Format",
  "Copyright (c) 1988, 1989, 1990, 1991, 1992 Sam Leffler\nCopyright (c) 1991, 1992 Silicon Graphics, Inc.",
  "TIFF",
  2,
  Extensions_Tab,
  IsTiff,
  GetHeader,
  LoadImageTiff
};

/*---------------------------------------------------------------------------*/
static boolean	IsTiff(name)
char	*name;
{
  TIFF*	tif;

  Xc_TRACE(("Testing Tiff file ..."));
  tif = TIFFOpen(name, "r");
  if (tif == NULL)
    {
      Xc_TRACE(("Not a Tiff file ..."));
      return (FALSE);
    }
  TIFFClose(tif);
  return (TRUE);
}
/*---------------------------------------------------------------------------*/
static error 	GetHeader(Image)
c_Image 	*Image;
{
  TIFF*			tif;
  unsigned short	bps, spp, photomet;
  int			Ii;

  Xc_TRACE(("get info of image '%s'", Image->filename)); 
  tif = TIFFOpen(Image->filename, "r");
  if (tif == NULL)
    {
      Xc_TRACE(("Not a Tiff file ..."));
      return (FALSE);
    }
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &(Image->width));
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &(Image->height));
  if (!TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps))
    bps = 1;
  if (!TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp))
    spp = 1;
  if (!TIFFGetField( tif, TIFFTAG_PHOTOMETRIC, &photomet))
    return XC_FILE_FORMAT_ERROR;

  Image->depth = bps*spp;

  if ((Image->depth <= 8) && (Image->depth > 1))
    {
      /* allouer la memoire pour la palette dans l'objet image ... */

      switch (photomet)
	{
	case 1:
	  Image->pal_r = (unsigned short *)
	    Xc_malloc("pal_r",sizeof(unsigned short)*(2<<Image->depth));
	  Image->pal_v = (unsigned short *)
	    Xc_malloc("pal_v",sizeof(unsigned short)*(2<<Image->depth));
	  Image->pal_b = (unsigned short *)
	    Xc_malloc("pal_b",sizeof(unsigned short)*(2<<Image->depth));
	  for (Ii = 0; Ii < (2<<Image->depth); Ii++)
	    {
	      Image->pal_r[Ii] = Ii << 8;
	      Image->pal_v[Ii] = Ii << 8;
	      Image->pal_b[Ii] = Ii << 8;
	    }
	  break;
	case 0:
	  Image->pal_r = (unsigned short *)
	    Xc_malloc("pal_r",sizeof(unsigned short)*(2<<Image->depth));
	  Image->pal_v = (unsigned short *)
	    Xc_malloc("pal_v",sizeof(unsigned short)*(2<<Image->depth));
	  Image->pal_b = (unsigned short *)
	    Xc_malloc("pal_b",sizeof(unsigned short)*(2<<Image->depth));
	  for (Ii = 0; Ii < (2<<Image->depth); Ii++)
	    {
	      Image->pal_r[Ii] = ((2<<Image->depth)-(Ii+1)) << 8;
	      Image->pal_v[Ii] = ((2<<Image->depth)-(Ii+1)) << 8;
	      Image->pal_b[Ii] = ((2<<Image->depth)-(Ii+1)) << 8;
	    }
	  break;
	case 2:
	  return XC_FILE_FORMAT_ERROR;
	case 3:
	  /*color pal to load */
	  if (!TIFFGetField(tif, TIFFTAG_COLORMAP, 
			    &(Image->pal_r), 
			    &(Image->pal_v),
			    &(Image->pal_b)))
	    return XC_FILE_FORMAT_ERROR;
	}
    }
  TIFFClose(tif);
  return XC_NO_ERROR;
}
/*---------------------------------------------------------------------------*/

static error LoadImageTiff(Image, line_buf, call_func, call_data)
c_Image		*Image;
unsigned char 	*line_buf;
boolean		(*call_func)  ___PROTO((unsigned char **pbuf, void *data));
void		*call_data;

{
  TIFF*	tif;
  int	line;
  Xc_TRACE(("Try to load TIFF !!!!!!!"));
 
  tif = TIFFOpen(Image->filename, "r");
  line = 0;
  while (line < Image->height)
    {
#ifndef NTRACE
      if (TIFFReadScanline(tif, line_buf, line, 0 ) < 0)
	Xc_TRACE(("bad data read on line %d", line));
#else
      TIFFReadScanline(tif, line_buf, line, 0 );
#endif
      call_func(&line_buf, call_data);
      line++;
    }
  TIFFClose(tif);
  return XC_NO_ERROR;
}


