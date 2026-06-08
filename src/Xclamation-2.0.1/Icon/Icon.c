/*
** Icon.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Icon/
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
** Started on  Mon Jul 17 17:51:37 1995 Emmanuel Paris
** Last update Sun May 28 16:39:52 2000 Emmanuel Paris
*/

#ifndef NTRACE
#define NTRACE
#endif

#include "Icon.h"
#include "Xpm.h"
#include "Colormap.h"
#include "ImageManager.h"
#include "RegisterHelp.h"

extern c_ImageManager	*GlobImgMng;
extern c_Colormap	*GlobColormap;

static void	*cons_Icon();
static void	dest_Icon();
static void	*copy_Icon();
static void	load_Icon();
static void	create_sensitive_pixmap();

sf_Icon fc_Icon =
{
 cons_Icon,
 dest_Icon,
 copy_Icon,
 load_Icon,
};

static void *cons_Icon(w_Parent, ManageW, Title, display, image, image2, Btype)
Widget		w_Parent;
c_ManageWidget	*ManageW;
char		*Title;
Display		*display;
char		**image;
char		**image2;
char		Btype;
{
 Window	window;
 c_Icon	*This;
 Arg		argsw[7];
 int		nargs;
 GC		gc;
 long		forec;
 long		backc;
 boolean	sel_pix = FALSE;
  
 if ((This = (c_Icon *)Xc_malloc("Icon",
				 sizeof (c_Icon))) == NULL)
  Xc_FATAL(("Can't build this object: memory allocation error"));
  
 memset(This, 0, sizeof(c_Icon));
 This->f = &fc_Icon;
  
 nargs = 0;
 switch (Btype)
 {
 case XcPush:
  This->w_This = XmCreatePushButton(w_Parent, Title, NULL, 0);
  Xc_REGISTER_HELP(This->w_This);
  XtSetArg(argsw[nargs], XmNbackground, &forec);
  nargs++;
  XtSetArg(argsw[nargs], XmNarmColor, &backc);
  nargs++;
  XtGetValues(This->w_This, argsw, nargs);
  break;
 case XcToggle:
  This->w_This = XmCreateToggleButton(w_Parent, Title, NULL, 0);
  Xc_REGISTER_HELP(This->w_This);
  XtSetArg(argsw[nargs], XmNbackground, &forec);
  nargs++;
  XtSetArg(argsw[nargs], XmNselectColor, &backc);
  nargs++;
  XtGetValues(This->w_This, argsw, nargs);
  /*XtDestroyWidget(This->w_This);*/
  break;
 case XcSeparator:
  This->w_This = XmCreateSeparatorGadget(w_Parent, Title, NULL, 0);
  break;
 case XcLabel:
  This->w_This = XmCreateLabel(w_Parent, Title, NULL, 0);
  Xc_REGISTER_HELP(This->w_This);
  XtSetArg(argsw[nargs], XmNbackground, &forec);
  nargs++;
  XtGetValues(This->w_This, argsw, nargs);
  backc = 0;
  break;
 }
  
 if (image)
 {
  window = DefaultRootWindow(display);
    
  F(This).Load(This, XtDisplay(w_Parent), image, forec, backc);
  This->PixImg = XCreatePixmap(display, window,
			       This->Image->width,
			       This->Image->height,
			       This->Image->depth);
    
  gc = XCreateGC(display, This->PixImg, 0, NULL);
  XPutImage(display, This->PixImg, gc, This->Image, 0, 0, 0, 0, 
	    This->Image->width, This->Image->height);
    
  if (Btype != XcLabel && Btype != XcSeparator && !image2)
  {
   Xc_TRACE(("Create Second Image"));
   if (This->Image2)
   {
    This->PixImg2 = XCreatePixmap(display, window,
				  This->Image2->width,
				  This->Image2->height,
				  This->Image2->depth);
	
    XPutImage(display, This->PixImg2, gc, This->Image2, 0, 0,
	      0, 0, This->Image2->width, This->Image2->height);
    sel_pix = TRUE;
   }
   Xc_TRACE(("call create_sensitive_pixmap"));
   create_sensitive_pixmap(This, display, window, gc, forec);
  }
    
  if (Btype == XcLabel)
  {
   Xc_TRACE(("call create_sensitive_pixmap"));
   create_sensitive_pixmap(This, display, window, gc, forec);
  }
    
  if (This->Image2)
  {
   Xc_free(This->Image2->data);
   XFree((char *)This->Image2);
   This->Image2 = NULL;
  }
  Xc_free(This->Image->data);
  XFree((char *)This->Image);
  XFreeGC(display, gc);
  This->Image = NULL;
 }
  
 if (image2)
 {
  window = DefaultRootWindow(display);
    
  F(This).Load(This, XtDisplay(w_Parent), image2, forec, backc);
  This->PixImg2 = XCreatePixmap(display, window,
				This->Image->width,
				This->Image->height,
				This->Image->depth);
    
  gc = XCreateGC(display, This->PixImg2, 0, NULL);
  XPutImage(display, This->PixImg2, gc, This->Image, 0, 0, 0, 0, 
	    This->Image->width, This->Image->height);
    
  Xc_TRACE(("call create_sensitive_pixmap"));
#ifdef Xc_XINSTALL
  if (Btype != XcToggle)
   create_sensitive_pixmap(This, display, window, gc, forec);
  else
  {
   This->DisPixImg = XCreatePixmap(display, window,
				   This->Image->width,
				   This->Image->height,
				   This->Image->depth);    
   gc = XCreateGC(display, This->DisPixImg, 0, NULL);
   XPutImage(display, This->DisPixImg, gc, This->Image, 0, 0, 0, 0, 
	     This->Image->width, This->Image->height);
  }
#else
  create_sensitive_pixmap(This, display, window, gc, forec);
#endif

  if (This->Image2)
  {
   Xc_free(This->Image2->data);
   XFree((char *)This->Image2);
   This->Image2 = NULL;
  }
  Xc_free(This->Image->data);
  XFree((char *)This->Image);
  XFreeGC(display, gc);
  This->Image = NULL;
  sel_pix = TRUE;
 }
  
 nargs = 0;
 This->Type = Btype;
 switch (Btype)
 {
 case XcPush:
  XtSetArg(argsw[nargs], XmNlabelType, XmPIXMAP);
  nargs++;
  if (sel_pix)
  {
   XtSetArg(argsw[nargs], XmNarmPixmap, This->PixImg2);
   nargs++;
   XtSetArg(argsw[nargs], XmNlabelPixmap, This->PixImg);
   nargs++;
  }
  else
  {
   XtSetArg(argsw[nargs], XmNarmPixmap, This->PixImg);
   nargs++;
  }
  XtSetArg(argsw[nargs], XmNlabelInsensitivePixmap, This->DisPixImg);
  nargs++;
  XtSetValues(This->w_This, argsw, nargs);
  break;
 case XcToggle:
  XtSetArg(argsw[nargs], XmNindicatorOn, False);
  nargs++;
  XtSetArg(argsw[nargs], XmNindicatorType, XmN_OF_MANY);
  nargs++;
  XtSetArg(argsw[nargs], XmNlabelType, XmPIXMAP);
  nargs++;
  if (sel_pix)
  {
   XtSetArg(argsw[nargs], XmNselectPixmap, This->PixImg2);
   nargs++;
   XtSetArg(argsw[nargs], XmNlabelPixmap, This->PixImg);
   nargs++;
  }
  else
  {
   XtSetArg(argsw[nargs], XmNselectPixmap, This->PixImg);
   nargs++;
  }
  XtSetArg(argsw[nargs], XmNselectInsensitivePixmap, This->DisPixImg);
  nargs++;
  XtSetValues(This->w_This, argsw, nargs);
  /*This->w_This = XmCreateToggleButton(w_Parent, Title, argsw, nargs);*/
  break;
 case XcSeparator:
  XtSetArg(argsw[nargs], XmNorientation, XmVERTICAL);
  nargs++;
  XtSetValues(This->w_This, argsw, nargs);
  break;
 case XcLabel:
  XtSetArg(argsw[nargs], XmNlabelType, XmPIXMAP);
  nargs++;
  XtSetArg(argsw[nargs], XmNlabelPixmap, This->PixImg);
  nargs++;
  XtSetArg(argsw[nargs], XmNlabelInsensitivePixmap, This->DisPixImg);
  nargs++;
  XtSetValues(This->w_This, argsw, nargs);
  break;
 }
 if (ManageW)
 {
  F(ManageW).Add_child(ManageW, This->w_This, w_Parent);
 }
 Xc_TRACE(("Object Icon build"));
 return This;
}

static void dest_Icon(This)
c_Icon *This;
{
 if (This->Type != XcSeparator)
 {
  XFreePixmap( XtDisplay(This->w_This), This->PixImg );
  if (This->Type != XcLabel)
  {
   if (This->PixImg2)
   {
    XFreePixmap( XtDisplay(This->w_This), This->PixImg2 );
   }  
   XFreePixmap( XtDisplay(This->w_This), This->DisPixImg );
  }
 }
 XtDestroyWidget(This->w_This);
 Xc_free(This);
 Xc_TRACE(("Object Icon destroyed"));
}

static void *copy_Icon(This)
c_Icon *This;
{
 c_Icon *ObjTmp;

 if ((ObjTmp = (c_Icon *)Xc_malloc("CIcon",
				   sizeof(c_Icon))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_Icon));
 Xc_TRACE(("Object Icon copied"));
 return ObjTmp;
}

static void load_Icon(This, display, Image, forec, backc)
c_Icon	*This;
Display	*display;
char 	**Image;
long	forec;
long	backc;
{
 XImage	*ximage;
 XImage	*xmask;
 char		*datai;
 char		*datai2;
 char		*datam;
 int		depth;
 int		x,y,ki,km;
 int		ptri;
 char		*ptrm;
 boolean	bit_order;
  
 xmask = NULL;
 XPMload(display, Image, &ximage, &xmask,FALSE);/* force mono=FALSE */
 if (xmask)
 {
  datai = ximage->data;
  datam = xmask->data;
  depth = GlobImgMng->real_depth;
  bit_order = (xmask->bitmap_bit_order==LSBFirst?TRUE:FALSE);
    
  datai2 = (char *)Xc_malloc("dup img2",
			     ximage->height * ximage->bytes_per_line);
  memcpy(datai2, datai, ximage->height*ximage->bytes_per_line);
    
  for(y = 0; y < ximage->height; y++)
  {
   ptrm = datam + y * xmask->bytes_per_line;
   ptri = y * ximage->bytes_per_line;
   ki = km = 0;
   for(x = 0; x<ximage->width; x++)
   {
    if (*ptrm & (1 << (bit_order ? km : 7-km)))
    {
     switch(depth)
     {
     case 1:
      if (forec)
       datai[ptri] &= ~(1 << (bit_order ? ki : 7-ki));
      else
       datai[ptri] |= (1 << (bit_order ? ki : 7-ki));
		  
      if (backc)
       datai2[ptri] &= ~(1 << (bit_order ? ki : 7-ki));
      else
       datai2[ptri] |= (1 << (bit_order ? ki : 7-ki));
		  
      if (++ki&8)
      {
       ki &= 7;
       ptri++;
      }
      break;
     case 32:
      if (GlobImgMng->byte_order_LSBFirst)
      {
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
       datai[ptri] = (char)(forec >> 16);
       datai2[ptri++] = (char)(backc >> 16);
       datai[ptri] = (char)(forec >> 24);
       datai2[ptri++] = (char)(backc >> 24);
      }
      else
      {
       datai[ptri] = (char)(forec >> 24);
       datai2[ptri++] = (char)(backc >> 24);
       datai[ptri] = (char)(forec >> 16);
       datai2[ptri++] = (char)(backc >> 16);
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
      }
      break;
     case 24:
      if (GlobImgMng->byte_order_LSBFirst)
      {
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
       datai[ptri] = (char)(forec >> 16);
       datai2[ptri++] = (char)(backc >> 16);
      }
      else
      {		      
       datai[ptri] = (char)(forec >> 16);
       datai2[ptri++] = (char)(backc >> 16);
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
      }
      break;
     case 16:
      if (GlobImgMng->byte_order_LSBFirst)
      {
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
      }
      else
      {		      
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
      }		  
      break;
     case 8:
      datai[ptri] = (char)(forec);
      datai2[ptri++] = (char)(backc);
      break;
     }
    }
    else
    {
     switch(depth)
     {
     case 1:
      if (++ki&8)
      {
       ki &= 7;
       ptri ++;
      }
      break;
     case 8:
      ptri++;
      break;
     case 15:
     case 16:
      ptri += 2;
      break;
     case 24:
      ptri += 3;
      break;
     case 32:
      ptri += 4;
      break;
     }
    }
    if (++km&8)
    {
     km &= 7;
     ptrm ++;
    }
   }
  }  
  Xc_free(xmask->data);
  XFree((char *)xmask); 
    
  switch(depth)
  {
  case 1:
   xmask = XCreateImage(display, 
			DefaultVisual(display, DefaultScreen(display)),
			1, XYBitmap, 0, datai2, ximage->width,
			ximage->height, 8, 0);
   break;
  case 8:
  case 16:
  case 24:
  case 32:
   xmask = XCreateImage(display, 
			DefaultVisual(display, DefaultScreen(display)),
			ximage->depth, ZPixmap, 0, datai2, ximage->width,
			ximage->height, 8, 0);
   break;
  default:
   Xc_WARNING(("Icon image unknow depth: %d",depth));
   break;
  }
 }
 This->Image = ximage; 
 This->Image2 = xmask;
 Xc_TRACE(("Icon loaded"));
}

/* ----------------------------------------------------------------- ** 
** static data for sensitive pixmap calculation                      ** 
** ----------------------------------------------------------------- */
static boolean		BWSensitive = FALSE;
static unsigned char	pattern_grid[4] = { 0x55, 0xaa };
static Pixmap		SensitivePattern;
static unsigned long	SensitiveColor;

static boolean		ColorSensitive = FALSE;
static char 		*SensitiveColormapName[16] = 
{
 "Black", "Black", "Gray10", "Gray20", "Gray30", "Gray35", "Gray40",
 "Gray45", "Gray50", "Gray55", "Gray60", "Gray65", "Gray70", "Gray80",
 "Gray90", "White"
};
static unsigned long	SensitiveColormapPixel[16];
static XColor		SensitiveColormapConv[50];
static char		SensitiveGrayScaleConv[256];
static long		Lx = -117L, Ly = 116L;
static long		Nz2 = 585225L, NzLz = 149940L;

static void create_sensitive_pixmap(This, display, window, gc, forec)
c_Icon	*This;
Display	*display;
Window	window;
GC	gc;
long	forec;
{
 Colormap 	c_map;
 color_cells_t	realcolor;
 XGCValues	gcv;
 XImage	*ximage, *oximage;
 char		*data, *datag, *odata;
 unsigned char c, oc;
 int		screen;
 int		i, j, k, w, h, size, sizeg;
 int		Nx, Ny;
 long		NdotL;
 register	unsigned char *s1, *s2, *s3, *bump, *dst;
 register	unsigned short x, y;
 unsigned short rgb;
 unsigned long  pixel;

 screen = DefaultScreen(display);
 This->DisPixImg = XCreatePixmap(display, window,
				 This->Image->width,
				 This->Image->height,
				 This->Image->depth);

 Xc_TRACE(("create_sensitive_pixmap"));

 switch(GlobImgMng->real_depth)
 {
 case 1:
  if (!BWSensitive)
  {
   SensitivePattern = XCreateBitmapFromData(display, window,
					    pattern_grid, 8, 2);
	  
   F(GlobColormap).allocate_named_color(GlobColormap, "gray80",
					&realcolor);
   SensitiveColor = realcolor.pixel;
   BWSensitive = TRUE;
  }
      
  XCopyArea(display, This->PixImg, This->DisPixImg, gc,
	    0, 0, This->Image->width, This->Image->height, 0, 0);
      
  gcv.function = GXcopy;
  gcv.foreground = SensitiveColor;
  gcv.fill_style = FillStippled;
  gcv.stipple = SensitivePattern;
  XChangeGC(display, gc, GCFunction | GCForeground | GCFillStyle | 
	    GCStipple, &gcv);
      
  XFillRectangle(display, This->DisPixImg, gc, 0, 0,
		 This->Image->width, This->Image->height);
  break;
 case 8:
  c_map = GlobColormap->cmap;
  if (!ColorSensitive)
  {
   for(i = 0; i < 16; i++)
   {
    F(GlobColormap).allocate_named_color(GlobColormap,
					 SensitiveColormapName[i],
					 &realcolor);
    SensitiveColormapPixel[i] = realcolor.pixel;
   }
   ColorSensitive = TRUE;
  }
  oximage = This->Image;
  w = oximage->width;
  h = oximage->height;
  size = h *  oximage->bytes_per_line;
      
  data = (char *)Xc_malloc("sens data", size);
  datag = (char *)Xc_malloc("sens data", size);
      
  Xc_TRACE(("collect all different color"));

  odata = oximage->data; j = 0;
  oc = odata[0];
  SensitiveColormapConv[j++].pixel = oc;
  for(i = 1; i < size; i++)
  {
   c = odata[i];
   if (c != oc)
   {
    k = 0;
    while(k < j && SensitiveColormapConv[k].pixel != c)
     k++;
    if (k == j)
     SensitiveColormapConv[j++].pixel = c;
   }
  }
  XQueryColors(display, c_map, SensitiveColormapConv, j);
      
  Xc_TRACE(("Create GrayScale Convertion Colormap"));
  for(i = 0; i < j; i++)
  {
   SensitiveGrayScaleConv[SensitiveColormapConv[i].pixel] = 
    (((long)(SensitiveColormapConv[i].red) * 11 + 
      (long)(SensitiveColormapConv[i].green) * 16 +
      (long)(SensitiveColormapConv[i].blue) * 5) >> 13);
  }
      
  Xc_TRACE(("GrayScalize image"));
  for(i = 0; i < size; i++)
  {
   datag[i] = SensitiveGrayScaleConv[(int)odata[i]];
  }
      
  bump = (unsigned char *)datag;
  dst = (unsigned char *)data;

  Xc_TRACE(("begin emboss..."));
  memset(dst, (char)forec, w);
  dst += w;
  for( y = 1; y < h - 1; y++, bump += w)
  {
   s1 = bump + 1;
   s2 = s1 + w;
   s3 = s2 + w;
   *dst ++ = (char)forec;
   for(x = 1; x < w - 1; x++, s1++, s2++, s3++)
   {
    Nx = (int)(s1[-1] + s2[-1] + s3[-1] - s1[1] - s2[1] - s3[1]);
    Ny = (int)(s3[-1] + (*s3) + s3[1] - s1[-1] - (*s1) - s1[1]);
	      
    if (Nx == 0 && Ny == 0)
     *dst++ = (char)forec;
    else
    {
     if ((NdotL = Nx * Lx + Ny * Ly + NzLz) < 0)
      *dst++ = SensitiveColormapPixel[0];
     else
      *dst++ = SensitiveColormapPixel
       [ ((int)((NdotL / 
		 sqrt((double)(Nx * Nx + Ny * Ny + Nz2))))
	  >> 4) & 0xf];
    }
   }
   *dst ++ = (char)forec;
  }
  memset(dst, (char)forec, w);
      
  ximage = XCreateImage(display, DefaultVisual(display, screen),
			8, ZPixmap, 0, data, w, h, 8, 0);
  Xc_TRACE(("end emboss generate pixmap"));
  XPutImage(display, This->DisPixImg, gc, ximage, 0, 0, 0, 0, w, h);
      
  Xc_free(data);
  Xc_free(datag);
  XFree((char *)ximage);
  break;
 case 16:
  oximage = This->Image;
  w = oximage->width;
  h = oximage->height;
  odata = oximage->data;
  size = h *  oximage->bytes_per_line;
  sizeg = size >> 1;
 
  data = (char *)Xc_malloc("sens data", size);
  datag = (char *)Xc_malloc("sens data", sizeg);
      
  Xc_TRACE(("GrayScalize image"));
  for(i = j = 0; i < sizeg; i++, j+=2)
  {
   if (GlobImgMng->byte_order_LSBFirst)
    realcolor.pixel = ( odata[j + 1] << 8 ) + odata[j];
   else
    realcolor.pixel = ( odata[j] << 8 ) + odata[j + 1];
   F(GlobColormap).get_RGB_values(GlobColormap, &realcolor);
   datag[i] = ((long)realcolor.red * 11 + 
	       (long)realcolor.green * 16 +
	       (long)realcolor.blue * 5) >> 13;
  }
      
  bump = (unsigned char *)datag;
  dst = (unsigned char *)data;
      
  Xc_TRACE(("begin emboss..."));
      
  if (GlobImgMng->byte_order_LSBFirst)
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
   }
  }
  else
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
  }
  
      
  for( y = 1; y < h - 1; y++, bump += w)
  {
   s1 = bump + 1;
   s2 = s1 + w;
   s3 = s2 + w;
   if (GlobImgMng->byte_order_LSBFirst)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
   }
   else
   {
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
   for(x = 1; x < w - 1; x++, s1++, s2++, s3++)
   {
    Nx = (int)(s1[-1] + s2[-1] + s3[-1] - s1[1] - s2[1] - s3[1]);
    Ny = (int)(s3[-1] + (*s3) + s3[1] - s1[-1] - (*s1) - s1[1]);
	      
    if (Nx == 0 && Ny == 0)
    {
     if (GlobImgMng->byte_order_LSBFirst)
     {
      *dst ++ = (char)forec;
      *dst ++ = (char)(forec >> 8);
     }
     else
     {
      *dst ++ = (char)(forec >> 8);
      *dst ++ = (char)forec;
     }
    }
    else
    {
     if ((NdotL = Nx * Lx + Ny * Ly + NzLz) < 0)
      pixel = GlobColormap->Cblack;
     else
     {
      rgb = ((short)((NdotL / 
		      sqrt((double)(Nx * Nx + Ny * Ny + Nz2))))
	     << 8);
      F(GlobColormap).get_closest_color(GlobColormap,
					&realcolor,
					rgb, rgb, rgb);
      pixel = realcolor.pixel;
     }
     if (GlobImgMng->byte_order_LSBFirst)
     {
      *dst ++ = (char)pixel;
      *dst ++ = (char)(pixel >> 8);
     }
     else
     {
      *dst ++ = (char)(pixel >> 8);
      *dst ++ = (char)pixel;
     }
    }
   }
   if (GlobImgMng->byte_order_LSBFirst)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
   }
   else
   {
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
  }
  if (GlobImgMng->byte_order_LSBFirst)
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
   }
  }
  else
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
  }
  ximage = XCreateImage(display, DefaultVisual(display, screen),
			oximage->depth, ZPixmap, 0, data, w, h, 8, 0);
  Xc_TRACE(("end emboss generate pixmap"));
  XPutImage(display, This->DisPixImg, gc, ximage, 0, 0, 0, 0, w, h);
      
  Xc_free(data);
  Xc_free(datag);
  XFree((char *)ximage);
  break;
 case 24:
  oximage = This->Image;
  w = oximage->width;
  h = oximage->height;
  odata = oximage->data;
  size = h *  oximage->bytes_per_line;
  sizeg = size / 3;
 
  data = (char *)Xc_malloc("sens data", size);
  datag = (char *)Xc_malloc("sens data", sizeg);
      
  Xc_TRACE(("GrayScalize image"));
  for(i = j = 0; i < sizeg; i++, j += 3)
  {
   if (GlobImgMng->byte_order_LSBFirst)
    realcolor.pixel = (odata[j + 2] << 16) + (odata[j + 1] << 8) +
     odata[j];
   else
    realcolor.pixel = (odata[j] << 16) + (odata[j + 1] << 8) +
     odata[j + 2];
   F(GlobColormap).get_RGB_values(GlobColormap, &realcolor);
   datag[i] = ((long)realcolor.red * 11 + 
	       (long)realcolor.green * 16 +
	       (long)realcolor.blue * 5) >> 13;
  }
      
  bump = (unsigned char *)datag;
  dst = (unsigned char *)data;
      
  Xc_TRACE(("begin emboss..."));
      
  if (GlobImgMng->byte_order_LSBFirst)
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)(forec >> 16);
   }
  }
  else
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
  }
  
      
  for( y = 1; y < h - 1; y++, bump += w)
  {
   s1 = bump + 1;
   s2 = s1 + w;
   s3 = s2 + w;
   if (GlobImgMng->byte_order_LSBFirst)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)(forec >> 16);
   }
   else
   {
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
   for(x = 1; x < w - 1; x++, s1++, s2++, s3++)
   {
    Nx = (int)(s1[-1] + s2[-1] + s3[-1] - s1[1] - s2[1] - s3[1]);
    Ny = (int)(s3[-1] + (*s3) + s3[1] - s1[-1] - (*s1) - s1[1]);
	      
    if (Nx == 0 && Ny == 0)
    {
     if (GlobImgMng->byte_order_LSBFirst)
     {
      *dst ++ = (char)forec;
      *dst ++ = (char)(forec >> 8);
      *dst ++ = (char)(forec >> 16);
     }
     else
     {
      *dst ++ = (char)(forec >> 16);
      *dst ++ = (char)(forec >> 8);
      *dst ++ = (char)forec;
     }
    }
    else
    {
     if ((NdotL = Nx * Lx + Ny * Ly + NzLz) < 0)
      pixel = GlobColormap->Cblack;
     else
     {
      rgb = ((short)((NdotL / 
		      sqrt((double)(Nx * Nx + Ny * Ny + Nz2))))
	     << 8);
      F(GlobColormap).get_closest_color(GlobColormap,
					&realcolor,
					rgb, rgb, rgb);
      pixel = realcolor.pixel;
     }
     if (GlobImgMng->byte_order_LSBFirst)
     {
      *dst ++ = (char)pixel;
      *dst ++ = (char)(pixel >> 8);
      *dst ++ = (char)(pixel >> 16);
     }
     else
     {
      *dst ++ = (char)(pixel >> 16);
      *dst ++ = (char)(pixel >> 8);
      *dst ++ = (char)pixel;
     }
    }
   }
   if (GlobImgMng->byte_order_LSBFirst)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)(forec >> 16);
   }
   else
   {
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
  }
  if (GlobImgMng->byte_order_LSBFirst)
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)(forec >> 16);
   }
  }
  else
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
  }
  ximage = XCreateImage(display, DefaultVisual(display, screen),
			oximage->depth, ZPixmap, 0, data, w, h, 8, 0);
  Xc_TRACE(("end emboss generate pixmap"));
  XPutImage(display, This->DisPixImg, gc, ximage, 0, 0, 0, 0, w, h);
      
  Xc_free(data);
  Xc_free(datag);
  XFree((char *)ximage);
  break;
 case 32:
  oximage = This->Image;
  w = oximage->width;
  h = oximage->height;
  odata = oximage->data;
  size = h *  oximage->bytes_per_line;
  sizeg = size >> 2;
 
  data = (char *)Xc_malloc("sens data", size);
  datag = (char *)Xc_malloc("sens data", sizeg);
      
  Xc_TRACE(("GrayScalize image"));
  for(i = j = 0; i < sizeg; i++, j+=4)
  {
   if (GlobImgMng->byte_order_LSBFirst)
    realcolor.pixel = (odata[j + 3] << 24) + (odata[j + 2] << 16) + 
     (odata[j + 1] << 8) + odata[j];
   else
    realcolor.pixel = (odata[j] << 24) + (odata[j + 1] << 16) + 
     (odata[j + 2] << 8) + odata[j + 3];
   F(GlobColormap).get_RGB_values(GlobColormap, &realcolor);
   datag[i] = ((long)realcolor.red * 11 + 
	       (long)realcolor.green * 16 +
	       (long)realcolor.blue * 5) >> 13;
  }
      
  bump = (unsigned char *)datag;
  dst = (unsigned char *)data;
      
  Xc_TRACE(("begin emboss..."));
      
  if (GlobImgMng->byte_order_LSBFirst)
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 24);
   }
  }
  else
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)(forec >> 24);
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
  }
  
      
  for( y = 1; y < h - 1; y++, bump += w)
  {
   s1 = bump + 1;
   s2 = s1 + w;
   s3 = s2 + w;
   if (GlobImgMng->byte_order_LSBFirst)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 24);
   }
   else
   {
    *dst ++ = (char)(forec >> 24);
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
   for(x = 1; x < w - 1; x++, s1++, s2++, s3++)
   {
    Nx = (int)(s1[-1] + s2[-1] + s3[-1] - s1[1] - s2[1] - s3[1]);
    Ny = (int)(s3[-1] + (*s3) + s3[1] - s1[-1] - (*s1) - s1[1]);
	      
    if (Nx == 0 && Ny == 0)
    {
     if (GlobImgMng->byte_order_LSBFirst)
     {
      *dst ++ = (char)forec;
      *dst ++ = (char)(forec >> 8);
      *dst ++ = (char)(forec >> 16);
      *dst ++ = (char)(forec >> 24);
     }
     else
     {
      *dst ++ = (char)(forec >> 24);
      *dst ++ = (char)(forec >> 16);
      *dst ++ = (char)(forec >> 8);
      *dst ++ = (char)forec;
     }
    }
    else
    {
     if ((NdotL = Nx * Lx + Ny * Ly + NzLz) < 0)
      pixel = GlobColormap->Cblack;
     else
     {
      rgb = ((short)((NdotL / 
		      sqrt((double)(Nx * Nx + Ny * Ny + Nz2))))
	     << 8);
      F(GlobColormap).get_closest_color(GlobColormap,
					&realcolor,
					rgb, rgb, rgb);
      pixel = realcolor.pixel;
     }
     if (GlobImgMng->byte_order_LSBFirst)
     {
      *dst ++ = (char)pixel;
      *dst ++ = (char)(pixel >> 8);
      *dst ++ = (char)(pixel >> 16);
      *dst ++ = (char)(pixel >> 24);
     }
     else
     {
      *dst ++ = (char)(pixel >> 24);
      *dst ++ = (char)(pixel >> 16);
      *dst ++ = (char)(pixel >> 8);
      *dst ++ = (char)pixel;
     }
    }
   }
   if (GlobImgMng->byte_order_LSBFirst)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 24);
   }
   else
   {
    *dst ++ = (char)(forec >> 24);
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
  }
  if (GlobImgMng->byte_order_LSBFirst)
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)forec;
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 24);
   }
  }
  else
  {
   for(i = 0; i < w; i++)
   {
    *dst ++ = (char)(forec >> 24);
    *dst ++ = (char)(forec >> 16);
    *dst ++ = (char)(forec >> 8);
    *dst ++ = (char)forec;
   }
  }
  ximage = XCreateImage(display, DefaultVisual(display, screen),
			oximage->depth, ZPixmap, 0, data, w, h, 8, 0);
  Xc_TRACE(("end emboss generate pixmap"));
  XPutImage(display, This->DisPixImg, gc, ximage, 0, 0, 0, 0, w, h);
      
  Xc_free(data);
  Xc_free(datag);
  XFree((char *)ximage);
  break;
 }
}


