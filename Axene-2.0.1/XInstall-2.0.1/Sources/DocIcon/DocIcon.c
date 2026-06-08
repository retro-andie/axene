/*
** DocIcon.c for XInstall in DocIcon/
** Object Loading Document Icons
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Wed Feb 22 16:58:39 1995 Emmanuel Paris
** Last update Sat Jan 10 20:42:52 1998 Emmanuel Paris
*/

#define DOC_ICON_PRIVATE
#include "DocIcon.h"
#include "Image.h"
#include "ImageManager.h"

extern c_ImageManager	*GlobImgMng;
extern image_load_t	Load_Gif;

#ifndef NO_INTRINSIC
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/XmStrDefs.h>
#endif

static void *cons_DocIcon();
static void dest_DocIcon();
static void *copy_DocIcon();
static Pixmap get_pixmap();

static void create_pixmap();

#ifndef NO_INTRINSIC
static void registerPixmapConverters();
static Boolean xt_convert_string_to_pixmap();

static char *pixmap_converters[] =
{
 XmRBitmap, XmRPixmap, XmRBackgroundPixmap, XmRXmBackgroundPixmap,
 XmRPrimForegroundPixmap, XmRPrimHighlightPixmap, XmRPrimTopShadowPixmap,
 XmRPrimBottomShadowPixmap, XmRManForegroundPixmap, XmRManHighlightPixmap,
 XmRManTopShadowPixmap, XmRManBottomShadowPixmap, XmRGadgetPixmap,
 XmRAnimationPixmap,  XmRAnimationPixmap, XmRAnimationMask, NULL
};
#endif

sf_DocIcon fc_DocIcon =
{
 cons_DocIcon,
 dest_DocIcon,
 copy_DocIcon,
 get_pixmap,
#ifndef NO_INTRINSIC
 registerPixmapConverters,
#endif
};

static void *cons_DocIcon(display)
Display	*display;
{
 c_DocIcon	*This;

 if ((This=(c_DocIcon *)Xc_malloc("DocIcon",sizeof(c_DocIcon)))==NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 memset(This, 0, sizeof(c_DocIcon));
 This->f= &fc_DocIcon;  
  
 This->display = display;
 memset(This->icon_Doc, 0, sizeof(Pixmap) * NB_PIXMAP);
  
#ifndef NO_INTRINSIC
 F(This).registerPixmapConverters(This);
#endif
 return This;
}

static void dest_DocIcon(This)
c_DocIcon   *This;
{
 int i;
  
 for(i=0; i < NB_PIXMAP; i++)
 {
  if (This->icon_Doc[i] != (Pixmap)0)
   XFreePixmap(This->display, This->icon_Doc[i]);
 }
 Xc_free(This);
}

static void *copy_DocIcon(This)
c_DocIcon   This;
{
 Xc_TRACE(("copy DocIcon function not done.normally not usefull."));
 return NULL;
}

static void create_pixmap(This, pixm)
c_DocIcon	*This;
doc_icon_t	pixm;
{
 if (pixm < NB_PIXMAP_XPM)
 {
  XImage	*xsource;
  XImage	*xmask;
  Pixmap	psource;
  char		**image;
  GC		gc;

  image = PixmapXPM[pixm];
  
  XPMload(This->display, image, &xsource, &xmask, FALSE);
  psource = XCreatePixmap(This->display, 
			  DefaultRootWindow(This->display),
			  xsource->width,xsource->height,
			  DefaultDepth(This->display, 
				       DefaultScreen(This->display)));
  gc = XCreateGC(This->display, psource, 0, 0);
  XPutImage(This->display, psource, gc, xsource, 0, 0, 0, 0,
	    xsource->width, xsource->height);
  
  Xc_free(xsource->data);
  XFree((char*)xsource);
  if (xmask)
  {
   Xc_free(xmask->data);
   XFree((char *)xmask);
  }
  This->icon_Doc[pixm] = psource;
 }
 else
 {
  c_Image		*Image;
  MemStream		*ImageStream;
  Pixmap		pixmap;

  ImageStream = Xc_NewStream(PixmapGIF[pixm - NB_PIXMAP_XPM].gif_data,
			     PixmapGIF[pixm - NB_PIXMAP_XPM].gif_size);
  Image = (c_Image *)NEW(c_Image)((char *)ImageStream, &Load_Gif);
  
  F(Image).CalculImage(Image, 0, 0);
  
  pixmap = F(GlobImgMng).getPixmap(GlobImgMng, Image->handle);
  
  Image->handle->pixmap = (Pixmap)0;
  DELETE(c_Image)(Image);
  Xc_DeleteStream(ImageStream);
  
  This->icon_Doc[pixm] = pixmap;
 }
}

static Pixmap get_pixmap(This, pixm)
c_DocIcon	*This;
doc_icon_t	pixm;
{
  
 if (pixm >= NB_PIXMAP)
  return (Pixmap)0;
  
 if (This->icon_Doc[pixm] == (Pixmap)0)
  create_pixmap(This, pixm);
  
 return This->icon_Doc[pixm];
}

/* -------------------------------------------------------------------- **
** register all String to Pixmap Convertions				**
** -------------------------------------------------------------------- */
#ifndef NO_INTRINSIC
static void registerPixmapConverters(This)
c_DocIcon *This;
{
 int	i;
 XtConvertArgRec	converter_arg;
 
 converter_arg.address_mode = XtAddress;
 converter_arg.address_id = (void *)This;
 converter_arg.size = sizeof(void *);
 
 i = 0;
 while(pixmap_converters[i])
 {
  XtSetTypeConverter(XtRString, pixmap_converters[i],
		     xt_convert_string_to_pixmap,
		     &converter_arg, 1, 
		     XtCacheAll, NULL);
  i++;
 }
}
#endif

/* -------------------------------------------------------------------- **
** callbacks and internal private functions				**
** -------------------------------------------------------------------- */

#ifndef NO_INTRINSIC
static Boolean xt_convert_string_to_pixmap(display, args, nargs, 
					   source, target, ptr)
Display *display;
XrmValue *args;
int *nargs;
XrmValue *source;
XrmValue *target;
XtPointer *ptr;
{ 
 c_DocIcon	*This;
 doc_icon_t	resource_icon;
 int		i;
     
 if (*nargs == 1)
  This = (c_DocIcon *)args->addr;
 else
  return False;
  
 if (!source->addr)
 {
  XtStringConversionWarning("Null", "Pixmap");
  return False;
 } 
 
 if (This->display != display)
  This->display = display;
  
 if (target->addr)					
 {						
  if (target->size < sizeof(Pixmap))		
  {						
   target->size = sizeof(Pixmap);		
   return FALSE;				
  }						
 }
  
 i = 0;
 while(IconResource[i].resource_name)
 {
  if (Xstrcmp(IconResource[i].resource_name, (char *)source->addr))
   break;
  i++;
 }
  
 if (!IconResource[i].resource_name)
  return False;
  
 {
  Pixmap	pixm;
  static Pixmap buf ;					
    
  resource_icon = IconResource[i].resource_icon;
    
  if (This->icon_Doc[resource_icon] == (Pixmap)0)
   create_pixmap(This, resource_icon);
    
  pixm = This->icon_Doc[resource_icon];
            
  if (target->addr)					
  {
   *((Pixmap *) (target->addr)) = pixm;	
  }
  else
  {						
   buf = pixm;					
   target->addr = (XPointer) &buf;		
  }						
  target->size = sizeof(Pixmap);			
  return True;	
 }
}

#endif










