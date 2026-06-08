/*
** Xpm.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and AxeneOffice 
** 	in Xpm/
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
** Started on  Tue Jul 18 13:07:20 1995 Emmanuel Paris
** Last update Tue Feb  1 10:48:00 2000 Emmanuel Paris
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "xcalibur.h"
#include "Colormap.h"
#include "fsdither.h"
#include "ImageManager.h"

extern c_Colormap	*GlobColormap;
extern c_ImageManager	*GlobImgMng;

void    XPMload();
void	XPMGetAttrib();
void	XPMGetColor();
void	XPMColorAlloc();
void	XPMCreateImage();
void	XPMCreateImageMono();
void	XPMCreateImage16();
void	XPMCreateMask();
#ifdef WITH_ZPM
char **ZPMtoXPM();
#endif

void XPMload(display, Image, ximage, xmask, mono)
Display	*display;
char	**Image;
XImage	**ximage;
XImage	**xmask;
boolean	mono;
{
 int		width;
 int		height;
 int 		color;
 int		bit;
 int		i;
 int		x_hot;
 int		y_hot;
 int		depth;
 int		xim_depth;
 int		screen;
 long		foreground;
 char		*convert;
 Visual	*visual;
 color_cells_t	*PalColors;
 boolean	bit_order;
 boolean	byte_order;
 char		*ImgBuffer;
 char		*MaskBuffer = NULL;
 char		*ImgBufferMono;
 char		mask;
 char		rmap[256], gmap[256], bmap[256];
#ifdef WITH_ZPM
 boolean	zpm = FALSE;
  
 if (**Image == 'z')
 {
  Image = ZPMtoXPM(Image);
  zpm = TRUE;
 }
#endif
  
 XPMGetAttrib(Image, &width, &height, &color, &bit, &x_hot, &y_hot);
 convert = (char *)Xc_malloc("Iconconvert", color+1);
 PalColors = (color_cells_t *)Xc_malloc("IconPalColors",
					(color+1)*sizeof(color_cells_t));
 XPMColorAlloc(display, Image, color, bit, PalColors, convert, &mask);
  
 if (mask)
 {
  MaskBuffer = (char *)Xc_malloc("IconMaskBuffer",
				 (width/8+1*(width%8!=0))*height);
  memset(MaskBuffer,'\0',(width/8+1*(width%8!=0))*height);
 }
  
 screen = DefaultScreen(display);
 visual = DefaultVisual(display, screen);
 xim_depth = DefaultDepth(display, screen); 
 depth = GlobImgMng->real_depth;
  
 *ximage= XCreateImage(display,  visual, 1, XYBitmap, 0, NULL,
		       width, height, 8, 0);
 bit_order = (*ximage)->bitmap_bit_order == LSBFirst;
 byte_order = (*ximage)->byte_order == LSBFirst;
  
 XPMCreateImage(Image, color, bit, &ImgBuffer, width, height, convert,
		PalColors, byte_order, depth);
  
 if ((color==2 && mask==0) || (color==3 && mask))
 {
  ImgBufferMono = (char *)Xc_malloc("IconMaskBuffer", 
				    ((width + 7) >> 3) * height);
  memset(ImgBufferMono, 0, ((width + 7) >> 3) * height);
  if (convert[0] == mask)
   foreground = PalColors[1].pixel;
  else 
   foreground = PalColors[0].pixel;
  XPMCreateImageMono(ImgBuffer, ImgBufferMono, width, height,
		     foreground, bit_order, byte_order, depth);
  (*ximage)->data = ImgBufferMono;
  Xc_free(ImgBuffer); 
 }      
 else
 {
  switch(depth)
  {
  case 1:
  case 4:
  case 8:
   if (depth == 1 || mono)
   {
    memset(rmap, 0, 256); memset(gmap, 0, 256); memset(bmap, 0, 256);
	      
    for(i=0;i<color;i++)
    {
     rmap[PalColors[i].pixel]=PalColors[i].red;
     gmap[PalColors[i].pixel]=PalColors[i].green;
     bmap[PalColors[i].pixel]=PalColors[i].blue;
    }
    ImgBufferMono = (char *)FSDither(ImgBuffer, PIC8, width, height,
				     rmap , gmap , bmap,
				     BlackPixel(display, screen),
				     WhitePixel(display, screen),
				     bit_order);
    (*ximage)->data=ImgBufferMono;
    Xc_free(ImgBuffer);
   }
   else
   {
    if (depth == 4)
    {
     char	*ImgBuffer16;
		  
     XDestroyImage(*ximage);
     *ximage = XCreateImage(display, visual, xim_depth, XYPixmap,
			    0, ImgBuffer, width, height, 4, 0);
     XPMCreateImage16(ImgBuffer, &ImgBuffer16, width, height,
		      bit_order);
     (*ximage)->data = ImgBuffer16;
     Xc_free(ImgBuffer);
    }
    else
    {
     XDestroyImage(*ximage);
     *ximage = XCreateImage(display, visual, xim_depth, ZPixmap,
			    0, ImgBuffer, width, height, 8, 0);
    }
   }
   break;
  case 16:  
   if (mono)
   {
    ImgBufferMono = (char *)Xc_malloc("IconMaskBuffer", 
				      ((width + 7) >> 3) * height);
    memset(ImgBufferMono, 0, ((width + 7) >> 3) * height);
    if (convert[0] == mask)
     foreground = PalColors[1].pixel;
    else 
     foreground = PalColors[0].pixel;
    XPMCreateImageMono(ImgBuffer, ImgBufferMono, width, height,
		       foreground, bit_order, byte_order, depth);
	      
    (*ximage)->data = ImgBufferMono;
    Xc_free(ImgBuffer);
   }
   else
   {
    XDestroyImage(*ximage);
    *ximage =	XCreateImage(display, visual, xim_depth, ZPixmap,
			     0, ImgBuffer, width, height, 8, 0);
   }
   break;
  case 24:
   if (mono)
   {
    ImgBufferMono = (char *)FSDither(ImgBuffer, PIC24, width, height,
				     NULL, NULL, NULL,
				     BlackPixel(display, screen),
				     WhitePixel(display, screen),
				     bit_order);
    (*ximage)->data = ImgBufferMono;
    Xc_free(ImgBuffer);
   }
   else
   {
    XDestroyImage(*ximage);
    *ximage =	XCreateImage(display, visual, xim_depth, ZPixmap,
			     0, ImgBuffer, width, height, 8, 0);
   }
   break;
  case 32:
   if (mono)
   {
    ImgBufferMono = (char *)FSDither(ImgBuffer, PIC24, width, height,
				     NULL, NULL, NULL,
				     BlackPixel(display, screen),
				     WhitePixel(display, screen),
				     bit_order);
    (*ximage)->data = ImgBufferMono;
    Xc_free(ImgBuffer);
   }
   else
   {
    XDestroyImage(*ximage);
    *ximage =	XCreateImage(display, visual, xim_depth, ZPixmap,
			     0, ImgBuffer, width, height, 8, 0);
   }
   break;
  default:
   Xc_FATAL(("Screen depth: %d not managed", depth));
   break;
  }
 }
  
 if (mask)
 {
  *xmask = XCreateImage(display, visual, 1, XYBitmap, 0, NULL,
			width, height, 8, 0);
  XPMCreateMask(Image,color, MaskBuffer, mask, width, height, bit_order);
  (*xmask)->data=MaskBuffer;
 }
 else
  *xmask = (XImage *)NULL;

 Xc_free(PalColors);
 Xc_free(convert);

#ifdef WITH_ZPM
 if (zpm)
 {
  Xc_free(*Image);
  Xc_free(Image);
 }
#endif
}

void XPMGetAttrib(Image, width, height, color, bit, x_hot, y_hot)
char	**Image;
int	*width;
int	*height;
int	*color;
int	*bit;
int	*x_hot;
int	*y_hot;
{
 int	num;
 char	buffer[10];
 char	*PtrImg;

 PtrImg = Image[0];
 num = 0;
 while (PtrImg[num] != ' ')
 {
  buffer[num] = PtrImg[num];
  num++;
 }
 buffer[num++] = 0;
 /*  Xc_TRACE(("%s\n", buffer)); */
 *width = atoi(buffer);
 PtrImg = PtrImg+num;
 /*  Xc_TRACE(("%s\n", PtrImg)); */
 num = 0;
 while (PtrImg[num] != ' ')
 {
  buffer[num] = PtrImg[num];
  num++;
 }
 buffer[num++] = 0;
 *height = atoi(buffer);
 PtrImg = PtrImg+num;
 num = 0;
 while (PtrImg[num] != ' ')
 {
  buffer[num] = PtrImg[num];
  num++;
 }
 buffer[num++] = 0;
 *color = atoi(buffer);
 PtrImg = PtrImg+num;
 num = 0;
 while (PtrImg[num] != ' ' && PtrImg[num]!='\0')
 {
  buffer[num] = PtrImg[num];
  num++;
 }
 buffer[num++] = 0;
 *bit = atoi(buffer);
 PtrImg = PtrImg+num;
 if (!PtrImg[-1])
 {
  *x_hot=0;
  *y_hot=0;
 }
 else
 {
  num = 0;
  while (PtrImg[num] != ' ')
  {
   buffer[num] = PtrImg[num];
   num++;
  }
  buffer[num++] = 0;
  *x_hot = atoi(buffer);
  PtrImg = PtrImg+num;
  *y_hot = atoi(PtrImg);
 }
} 

void XPMGetColor(display, Image, ncolors, colors)
Display *display;
char	**Image;
int	ncolors;
XColor	*colors;
{
 char		buffer[30];
 char 		*ptr;
 int		i,j;
 XColor	realcolor;
 Colormap      c_map;
 int		colornum;
  
 c_map = GlobColormap->cmap;
 j=0;
 for (colornum = 0; colornum<ncolors; colornum++)
 {
  ptr = Image[colornum+1];
  while (*(ptr++) != 'c');
  while (*(ptr++) == ' ');
  i = 0;
  ptr--;
  while ((*ptr != ' ') && (*ptr != 0))
  {
   buffer[i++] = *(ptr++);
  }
  buffer[i] = 0;
  /*      Xc_TRACE(("%s \n", buffer); */
  if (strcmp(buffer,"None"))
  {
   XParseColor(display, c_map, buffer, &realcolor);
   memcpy( (char *)&(colors[j++]), (char *)&realcolor, sizeof(XColor));
  }
 }
}

void XPMColorAlloc(display, Image, ncolors, bit, colors, convert, mask)
Display *display;
char	**Image;
int	ncolors;
int	bit;
color_cells_t *colors;
char	*convert;
char	*mask;
{
 char		buffer[30];
 char 		*ptr;
 int		i;
 color_cells_t	realcolor;
 int		colornum;
  
 *mask=0;
 for (colornum = 0; colornum<ncolors; colornum++)
 {
  ptr = Image[colornum+1];
  convert[colornum] = *(ptr++);
  while (*(ptr++) != 'c');
  while (*(ptr++) == ' ');
  i = 0;
  ptr--;
  while ((*ptr != ' ') && (*ptr != 0))
  {
   buffer[i++] = *(ptr++);
  }
  buffer[i] = 0;
  /*      Xc_TRACE(("%s \n", buffer); */
  if (!strcmp(buffer,"None"))
  {
   *mask=convert[colornum];
   memset( (char *)&colors[colornum], 0, sizeof(color_cells_t));
  }
  else
  {
   F(GlobColormap).allocate_named_color(GlobColormap, buffer, &realcolor);
   memcpy( (char *)&colors[colornum], (char *)&realcolor,
	  sizeof(color_cells_t));
  }
 }
}

void XPMCreateImage(Image, ncolors, bit, buf, width, height,
		    convert, colors, byte_order, depth)
char	**Image;
int	ncolors;
int	bit;
char	**buf;
int	width;
int	height;
char	*convert;
color_cells_t *colors;
boolean	byte_order;
int	depth;
{
 char	*ptr, *buffer;
 int	x;
 int	y;
 int	i;
  
 switch(depth)
 {
 case 1:
 case 4:
 case 8:
  *buf = buffer = (char *)Xc_malloc("IconImgBuffer", width * height);  
      
  for (y = 0; y<height; y++)
  {
   ptr = Image[ncolors+1+y];
   for (x = 0; x<width; x++)
   {
    i = 0;
    while ( (*(ptr + x) != convert[i]) && i < ncolors)
     i++;
	      
    *buffer++ = colors[i].pixel;
   }
  }
  break;
 case 16:
  *buf = buffer = (char *)Xc_malloc("IconImgBuffer", width * height * 2);  
      
  for (y = 0; y<height; y++)
  {
   ptr = Image[ncolors+1+y];
   for (x = 0; x<width; x++)
   {
    i = 0;
    while ( (*(ptr + x) != convert[i]) && i < ncolors)
     i++;
	      
    if (byte_order)
    {
     *buffer++ = (colors[i].pixel & 0xff);
     *buffer++ = (colors[i].pixel & 0xff00) >> 8;
    }
    else
    {
     *buffer++ = (colors[i].pixel & 0xff00) >> 8;
     *buffer++ = (colors[i].pixel & 0xff);
    }
   }
  }
  break;
 case 24:
  *buf = buffer = (char *)Xc_malloc("IconImgBuffer", height * width * 3); 
      
  for (y = 0; y<height; y++)
  {
   ptr = Image[ncolors+1+y];
   for (x = 0; x<width; x++)
   {
    i = 0;
    while ( (*(ptr + x) != convert[i]) && i < ncolors)
     i++;

    if (byte_order)
    {
     *buffer++ = (colors[i].pixel & 0xff);
     *buffer++ = (colors[i].pixel & 0xff00) >> 8;
     *buffer++ = (colors[i].pixel & 0xff0000) >> 16;
    }
    else
    {
     *buffer++ = (colors[i].pixel & 0xff0000) >> 16;
     *buffer++ = (colors[i].pixel & 0xff00) >> 8;
     *buffer++ = (colors[i].pixel & 0xff);
    }
   }
  }
  break;
 case 32:
  *buf = buffer = (char *)Xc_malloc("IconImgBuffer", width * height * 4);  
  for (y = 0; y < height; y++)
  {
   ptr = Image[ncolors+1+y];
   for (x = 0; x<width; x++)
   {
    i = 0;
    while ( (*(ptr + x) != convert[i]) && i < ncolors)
     i++;

    if (byte_order)
    {
     *buffer++ = (colors[i].pixel & 0xff);
     *buffer++ = (colors[i].pixel & 0xff00) >> 8;
     *buffer++ = (colors[i].pixel & 0xff0000) >> 16;
     *buffer++ = (colors[i].pixel & 0xff000000) >> 24;
    }
    else
    {
     *buffer++ = (colors[i].pixel & 0xff000000) >> 24;
     *buffer++ = (colors[i].pixel & 0xff0000) >> 16;
     *buffer++ = (colors[i].pixel & 0xff00) >> 8;
     *buffer++ = (colors[i].pixel & 0xff);
    }
   }
  }
  break;
 default:
  Xc_FATAL(("Screen depth: %d not managed", depth));
  break;
 } 
 Xc_mcheck();
}

void XPMCreateImageMono(imagebuffer, imagemonobuffer, width, height,
			foreground, bit_order, byte_order, depth)
char	*imagebuffer;
char    *imagemonobuffer;
int	width;
int	height;
long	foreground;
boolean	bit_order;
boolean byte_order;
int	depth;
{
 char	*ptr;  
 char	*ptr_buf;
 int	x,y;
 int	k, val = 0;
 char	c1, c2, c3, c4;
 int	byte_per_line = 0;
  
 c1 = c2 = c3 = c4 = 0;
 switch(depth)
 {
 case 1:
 case 4:
 case 8:
  byte_per_line = width;
  c1 = (foreground & 0xff);
  break;
 case 16:
  byte_per_line = width * 2;
  if (byte_order)
  {
   c1 = ((foreground & 0xff00) >> 8);
   c2 = (foreground & 0xff);
  }
  else
  {
   c1 = (foreground & 0xff);
   c2 = ((foreground & 0xff00) >> 8);
  }
  break;
 case 24:
  byte_per_line = width * 3;
  if (byte_order)
  {
   c1 = ((foreground & 0xff0000) >> 16);
   c2 = ((foreground & 0xff00) >> 8);
   c3 = (foreground & 0xff);
  }
  else
  {
   c1 = (foreground & 0xff);
   c2 = ((foreground & 0xff00) >> 8);
   c3 = ((foreground & 0xff0000) >> 16);
  }
  break;
 case 32:
  byte_per_line = width * 4;
  if (byte_order)
  {
   c1 = ((foreground & 0xff000000) >> 24);
   c2 = ((foreground & 0xff0000) >> 16);
   c3 = ((foreground & 0xff00) >> 8);
   c4 = (foreground & 0xff);
  }
  else
  {
   c1 = (foreground & 0xff);
   c2 = ((foreground & 0xff00) >> 8);
   c3 = ((foreground & 0xff0000) >> 16);
   c4 = ((foreground & 0xff000000) >> 24);
  }
  break;
 }
  
 for (y = 0; y<height; y++)
 {
  ptr = imagebuffer + y * byte_per_line;
  ptr_buf = imagemonobuffer + ((width + 7) >> 3) * y;
  k = 0;
  for (x = 0; x < width; x++)
  {
   switch(depth)
   {
   case 1:
   case 4:
   case 8:
    val = (*(ptr++) == c1);
    break;
   case 16:
    val = *(ptr++) == c2;
    val &= *(ptr++) == c1;
    break;
   case 24:
    val = *(ptr++) == c3;
    val &= *(ptr++) == c2;
    val &= *(ptr++) == c1;
    break;
   case 32:
    val = *(ptr++) == c4;
    val &= *(ptr++) == c3;
    val &= *(ptr++) == c2;
    val &= *(ptr++) == c1;
    break;
   }
   if (k & 8)
   {
    k &= 7;
    ptr_buf++;
   }
   if (val)
   {
    if (bit_order)
     *ptr_buf |= (1 << k);
    else
     *ptr_buf |= (1 << (7 - k));
   }
   k++;
  }
 }
}


void XPMCreateImage16(imagebuffer, image16buffer, width, height, bit_order)
char	*imagebuffer;
char    **image16buffer;
int	width;
int	height;
boolean	bit_order;
{
 char	*imgp1, *imgp2, *imgp3, *imgp4, *ptr;
 int	plan_size, line_size;
 int	x, y;
 int	k, n, val;

 line_size = ((width + 7) >> 3);
 plan_size = line_size * height;
 *image16buffer = imgp1 = (char *)Xc_malloc("IconMaskBuffer", plan_size * 4);
 imgp2 = imgp1 + plan_size;
 imgp3 = imgp2 + plan_size;
 imgp4 = imgp3 + plan_size;
 ptr = imagebuffer;
  
 for (y = 0; y < height; y++)
 {
  k = n = 0;
  for (x = 0; x < width; x++)
  {
   val = *ptr++;
   if (k & 8)
   {
    k&=7;
    n++;
   }

   if (val & 8)
   {
    if (bit_order)
     imgp1[n] |= (1 << k);
    else
     imgp1[n] |= (1 << (7 - k));
   }
   if (val & 4)
   {
    if (bit_order)
     imgp2[n] |= (1 << k);
    else
     imgp2[n] |= (1 << (7 - k));
   }
   if (val & 2)
   {
    if (bit_order)
     imgp3[n] |= (1 << k);
    else
     imgp3[n] |= (1 << (7 - k));
   }
   if (val & 1)
   {
    if (bit_order)
     imgp4[n] |= (1 << k);
    else
     imgp4[n] |= (1 << (7 - k));
   }

   k++;
  }
  imgp1 += line_size;
  imgp2 += line_size;
  imgp3 += line_size;
  imgp4 += line_size;
 }
}


void XPMCreateMask(Image, ncolors, maskbuffer, mask,
		    width, height, bit_order)
char	**Image;
int	ncolors;
char	*maskbuffer;
char	mask;
int	width;
int	height;
boolean	bit_order;
{
 char	*ptr;
 char	*ptr_mask;
 int	x,y;
 int	i,k;
 int	val;
  
 for (y = 0; y < height; y++)
 {
  ptr = Image[ncolors+1+y];
  ptr_mask = maskbuffer + ((width + 7) >> 3) * y;
  k=0;
  for (x = 0; x<width; x++)
  {
   i = 0;
   val = (*(ptr+x) == mask) ? 1 : 0;
   if (k&8)
   {
    k&=7;
    ptr_mask++;
   }
   if (bit_order)
    *ptr_mask |= (1 << k) * val;
   else
    *ptr_mask |= (1 << (7-k)) * val;
   k++;
  }
 }
}

#ifdef WITH_ZPM
char **ZPMtoXPM(Image)
char **Image;
{
 unsigned int rsize;
 unsigned int linecnt;
 char *info;
 char *data, *ptr;
 char *line_buf, *lptr;
 char **img_buf;
 char c;
 int i, j, k;
  
 info = Image[0];
 data = Image[1];
  
 rsize = ((info[1] & 0xff) << 8) + (unsigned int)(info[2] & 0xff);
 k = 3;
 linecnt = (unsigned char)info[k++];
 if (linecnt < 5)
 {
  linecnt <<= 8;
  linecnt |= (unsigned char)info[k++];
 }

 img_buf = (char **)Xc_malloc("img_buf", sizeof(char *) * linecnt);
 line_buf = (char *)Xc_malloc("row_buf", rsize);
  
 ptr = line_buf; 
 memset(line_buf, 0xff, rsize);

 j = 0;
 for(i = 0; i < (int)linecnt; i++)
 {
  img_buf[i] = ptr;    
  if (info[k] != '\0')
   j = (unsigned char)info[k++];
  ptr += j;
  *(ptr - 1) = '\0';
 }
  
 ptr = data;
 lptr = line_buf;
 for(i = 0; i < (int)rsize; i++)
 {
  c = *ptr++;
  if ( (c & 128) == 128)
  {
   k = (c & 127);
   c = *ptr++;
   for(j = 0; j < k; j++)
   {
    *lptr++ = c;
    if (!*lptr)
     {
      lptr++; i++;
     }
   }
   i += k - 1;
  }
  else
  {
   *lptr++ = c;
   if (!*lptr)
    {
     lptr++; i++;
    }
  }
 }
  
/* for(i=0; i<linecnt;i++)
  printf("%s\n", img_buf[i]); */

 Xc_mcheck();
 return img_buf;
}
#endif









