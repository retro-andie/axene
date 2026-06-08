/*
** ImageManager.c for Xclamation, XQuad, XAllWrite, XMayday and AxeneOffice 
** 	in ImageManager/
** Methods for the ImageManager class
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
** Started on  Sat Jan 14 23:06:43 1995 Stéphane Boisson
** Last update Sun Apr 25 15:04:50 1999 Emmanuel Paris
*/

#ifndef NHISTORY
#define NHISTORY
#endif
#ifndef NTRACE
#define NTRACE
#endif

#include "ImageManager.h"
#include <X11/Xutil.h>

#define FREE_XIMAGE(xim) XFree(xim)

static void 		*constructor();
static Pixmap		create_clip_mask();
static void 		destructor();
static void 		*copy();

static boolean 		bit_order_LSBFirst();
static boolean 		byte_order_LSBFirst();
static int 		X_Memory_Error_Handler();

#if 0
static image_item_t *getWorstRatioItem ___PROTO((c_ImageManager *this));
#endif
static boolean		isLocalServer();

static image_item_t *allocateItem ___PROTO((c_ImageManager *this, char *data,
					    int width, int height, int depth,
					    boolean Force));
static void freeItem ___PROTO((c_ImageManager *this, image_item_t *item));
static boolean drawItem ___PROTO((c_ImageManager *this, image_item_t *item,
				  Drawable drawable, GC gc,
				  int XSrc, int YSrc, int width, int height,
				  int XDes, int YDes));
static boolean drawMasked ___PROTO((c_ImageManager *this, image_item_t *item,
				    Drawable drawable, GC gc1, GC gc2,
				    int XSrc, int YSrc, int width, int height,
				    int XDes, int YDes));
static boolean forceItem ___PROTO((c_ImageManager *this, image_item_t *item));
static unsigned long getBytesPerLine ___PROTO((c_ImageManager *this,
					       unsigned long width,
					       unsigned int depth));
static Pixmap getPixmap ___PROTO((c_ImageManager *this, image_item_t *item));

boolean	want_xbug_line = FUZZY;
boolean	want_xbug_point = FUZZY;

sf_ImageManager fc_ImageManager =
{
 constructor,
 destructor,
 copy,
 allocateItem, freeItem, drawItem,
 forceItem,
 bit_order_LSBFirst,
 byte_order_LSBFirst,
 getBytesPerLine,
 drawMasked,
 getPixmap
};


/* ----------------------------------------------------------------- ** 
** construction method                                               ** 
** ----------------------------------------------------------------- */
static void *constructor(The_Handler, display, xshm_flag)
c_XErrorHandler	*The_Handler;
Display		*display;
boolean		xshm_flag;
{
 c_ImageManager *this;
 XImage *ximage;
 XGCValues values;

 Xc_HISTORY(("constructor"));

 if((this = Xc_malloc("ImageManager", sizeof(c_ImageManager))) == NULL)
  return (NULL);
 this->f = &fc_ImageManager;

 this->handle = Xc_createItem("images", 16, sizeof(image_item_t));
 if(this->handle == NULL)
 {
  Xc_free(this);
  return NULL;
 }

 F(The_Handler).Add_Callback(XcPM_X_BAD_ALLOCATION_ERROR_CODE,
			     X_Memory_Error_Handler, this); 

 this->display = display;

 /*--- Create GCs ---*/
 values.subwindow_mode = IncludeInferiors;
 values.foreground = 1;
 values.background = 0;
 values.graphics_exposures = False;
     
 this->gc = XCreateGC(display, DefaultRootWindow(display),
		      GCSubwindowMode|GCForeground|GCBackground, &values);
 if(DisplayPlanes(display, DefaultScreen(display)) > 1)
 {
  Pixmap bitmap;
      
  bitmap = XCreatePixmap(display, DefaultRootWindow(display), 1, 1, 1);
  this->mono_gc = XCreateGC(display, bitmap,
			    GCForeground|GCBackground, &values);
  XFreePixmap(display, bitmap);
 }
 else
  this->mono_gc = this->gc;


 this->draw_points_bug = FALSE;
 this->draw_linesclip_bug = FALSE;
 this->local_flag = isLocalServer(display);
 this->base = NULL;

 /*--- Get byte order ---*/
 this->real_depth = DefaultDepth(display, DefaultScreen(display));
 ximage = XCreateImage(display, None, 1, XYBitmap, 0, NULL, 16, 16, 8, 2);
 if(ximage != NULL)
 {
  this->bit_order_LSBFirst = 
   (ximage->bitmap_bit_order == LSBFirst)? TRUE : FALSE;
  this->byte_order_LSBFirst = 
   (ximage->byte_order == LSBFirst)? TRUE : FALSE;
  this->bytes_per_pixel = ximage->bits_per_pixel >> 3;
  FREE_XIMAGE((char *)ximage);
 }
  
 {
  Pixmap	pixmap = (Pixmap)0;
  GC		gc = (GC)0;
  XPoint	point[2];
  int		pix_ok;
    
  pix_ok = 0;
  if (want_xbug_point != FUZZY)
  {
   if (want_xbug_point == FALSE)
    this->draw_points_bug = FALSE;
   else
    this->draw_points_bug = TRUE;
  }
  else
  {
   /*---  tryto autodetect ---*/
   pix_ok = 1;
   pixmap = XCreatePixmap(display, DefaultRootWindow(display),
			  16, 16, this->real_depth);
   values.foreground = 0L;
   gc = XCreateGC(display, pixmap, GCForeground, &values);
   XFillRectangle(display, pixmap, gc, 0, 0, 16, 16);
   point[0].x = point[0].y = 8;
   point[1].x = -1;  point[1].y = 1;
   XSetForeground(display, gc, (unsigned long)-1);
   XDrawPoints(display, pixmap, gc, point, 2, CoordModePrevious);
    
   ximage = XGetImage
    (display, pixmap, 0, 0, 16, 16, (unsigned long)-1, 
     (this->real_depth < 8 ? XYPixmap : ZPixmap));
   if (XGetPixel(ximage, 7, 9) == 0)
    this->draw_points_bug = TRUE;
   FREE_XIMAGE((char *)ximage);
  }

  if (want_xbug_line != FUZZY)
  {
   if (want_xbug_line == FALSE)
    this->draw_linesclip_bug = FALSE;
   else
    this->draw_linesclip_bug = TRUE;
  }
  else
  {
   if (!pix_ok)
   {
    pixmap = XCreatePixmap(display, DefaultRootWindow(display),
			   16, 16, this->real_depth);
    values.foreground = 0L;
    gc = XCreateGC(display, pixmap, GCForeground, &values);
   }
   else
    XSetForeground(display, gc, (unsigned long)0);
   XFillRectangle(display, pixmap, gc, 0, 0, 16, 16);
   point[0].x = point[0].y = 8;
   point[1].x = -1;  point[1].y = -1;
   values.clip_x_origin = 1;
   values.clip_y_origin = 1;
   values.clip_mask = create_clip_mask(display, pixmap, 14, 14);
   XChangeGC(display, gc, GCClipMask|GCClipXOrigin|GCClipYOrigin, &values);
   XSetForeground(display, gc, (unsigned long)-1);
   XDrawLines(display, pixmap, gc, point, 2, CoordModePrevious);
   ximage = XGetImage
    (display, pixmap, 0, 0, 16, 16, (unsigned long)-1, 
     (this->real_depth < 8 ? XYPixmap : ZPixmap));
   if (XGetPixel(ximage, point[0].x+point[1].x, point[0].y+point[1].y) == 0)
    this->draw_linesclip_bug = TRUE;   
   FREE_XIMAGE((char *)ximage);
  }
 }
  
 if (this->real_depth > 8)
 {
  ximage = XCreateImage(display, None, this->real_depth, ZPixmap,
			0, NULL, 16, 16, 8, 2);
  if(ximage != NULL)
  {
   this->real_depth = ximage->bits_per_pixel;
   FREE_XIMAGE((char *)ximage);
  }
 }
  
#ifdef HAVE_XSHM
 if(this->local_flag == TRUE)
 {
  Xc_TRACE(("Setting Xshm flag ..."));
  this->xshm_flag = xshm_flag;
  /*this->xshm_flag = FALSE;*/
 }
 else
  this->xshm_flag = FALSE;

#endif

 /*--- if not; fill cut buffer ---*/
 {
  int	i;
  int	len;
  char	*buf;

  i = 0;
  while (i < 8)
  {
   buf = XFetchBuffer(display, &len, i);
   if (!buf)
    XStoreBuffer(display, "", 0, i);
   i++;
  }
 }
  
 Xc_TRACE(("Constructor"));
 return (this);
}

/* ----------------------------------------------------------------- ** 
** create clip mask                                                  ** 
** ----------------------------------------------------------------- */
Pixmap create_clip_mask(display, window, sx, sy)
Display	*display;
Window	window;
int	sx;
int	sy;
{
 Pixmap	pixmap;
 GC	gc;
 
 pixmap = XCreatePixmap(display, window,
			sx, sy, 1);
 gc = XCreateGC(display, pixmap, 0, NULL);
 XSetFunction(display, gc, GXcopy);
 XSetForeground(display, gc, -1);
 XFillRectangle(display, pixmap, gc, 0, 0,
		sx, sy);
#ifdef B_CLIP
 XSetForeground(display, gc, 0);
 XDrawPoint(display, pixmap, gc, 0, 0);
#endif
 return pixmap;
}


/* ----------------------------------------------------------------- ** 
** destruction method                                                ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_ImageManager *this;
{
 Xc_HISTORY(("destructor"));

 while(this->base != NULL) F(this).free(this, this->base);
 Xc_destroyItem(this->handle);

 XFreeGC(this->display, this->gc);
 if(DisplayPlanes(this->display, DefaultScreen(this->display)) > 1)
  XFreeGC(this->display, this->mono_gc);

 Xc_free(this);
  
 Xc_TRACE(("destroyed"));
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_ImageManager *this;
{
 Xc_HISTORY(("copy: non implemented"));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** isLocalServer - Test if XServer is on local machine               ** 
** ----------------------------------------------------------------- */
static boolean isLocalServer(display)
Display	*display;
{
 char	*dstring;
 char  *n;
 int	Ci;
 boolean Return_Value;

 dstring = (char *) Xc_malloc("temp", 80);
 n = DisplayString(display);
 for (Ci = 0; (*n != ':') && (*n != '\0'); n++)
  dstring[Ci++] = *n;
 dstring[Ci] = '\0';

 if ((strcmp(dstring, "unix") == 0) || (strcmp(dstring, "localhost") == 0) ||
     (dstring[0] == '\0'))
 {
  Xc_TRACE(("On a working station"));
  Return_Value = TRUE;
 }
 else
 {
  Xc_TRACE(("You're working on a terminal ..."));
  Return_Value = FALSE;
 }
 Xc_free(dstring);
 return (Return_Value);
}

#if 0
/* ----------------------------------------------------------------- ** 
** getWorstRatioItem - Get the oldest exposed item                   ** 
** ----------------------------------------------------------------- */
static image_item_t *getWorstRatioItem(this)
c_ImageManager *this;
{
 image_item_t *ptr, *last;
 long count;

 Xc_TRACE(("getWorstRatioItem"));

 ptr = this->base;
 if(this->base == NULL) return NULL;

 count = ptr->expose_count; 
 last = ptr;

 while((ptr = ptr->next) != NULL)
  if(ptr->expose_count < count)
  {
   count = ptr->expose_count;
   last = ptr;
  }
 return last;
}
#endif

/* ----------------------------------------------------------------- ** 
** X_Memory_Error_Handler - Called when server is out of memory      ** 
** ----------------------------------------------------------------- */
static int X_Memory_Error_Handler(Error, this)
XErrorEvent	*Error;
c_ImageManager	*this;
{
 Xc_HISTORY(("X-Memory Error handler"));

 this->error_flag = TRUE;
 return 0;
}


/* ----------------------------------------------------------------- ** 
** allocateItem - Item allocation                                    ** 
** ----------------------------------------------------------------- */
static image_item_t *allocateItem(this, data, width, height, depth, force_flag)
c_ImageManager *this;
char *data;
int width;
int height;
int depth;
boolean force_flag;
{
 image_item_t *item;
 XImage *ximage;
 long bytes_per_line;
 int format;

 Xc_HISTORY(("allocate(%dx%dx%d)", width, height, depth));

 if(data == NULL) return NULL;

 ximage = NULL;
 format = (depth == 1)? XYBitmap : ZPixmap;

 bytes_per_line = F(this).getBytesPerLine(this, width, depth);

 if((item = Xc_new(this->handle)) == NULL) return NULL;

 item->type = XcPM_EMPTY;
 item->width = width;
 item->height = height;
 item->depth = depth;
 item->expose_count = 0;


#ifdef HAVE_XSHM
 if(this->xshm_flag == TRUE && !force_flag)
 {
  XShmSegmentInfo *shminfo_ptr;
  int state = 0;

  do 
  {
   unsigned long size;

   Xc_TRACE(("XShmCreateImage"));

   this->error_flag = FALSE;
   shminfo_ptr = &item->shminfo;
   ximage = XShmCreateImage(this->display, None, depth, format, NULL,
			    shminfo_ptr, width, height);
   if(ximage == NULL) break;
   state++;

   size = ximage->bytes_per_line * ximage->height;

   Xc_TRACE(("shmget"));
   shminfo_ptr->shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0777);
   if(shminfo_ptr->shmid < 0) break;
   state++;

   Xc_TRACE(("shmat"));
   shminfo_ptr->shmaddr = shmat(shminfo_ptr->shmid, NULL, 0);

   if(shminfo_ptr->shmaddr == ((char *) -1)) break;
   ximage->data = shminfo_ptr->shmaddr;
   state++;

   shminfo_ptr->readOnly = True;

   Xc_TRACE(("XShmAttach"));
   if(XShmAttach(this->display, shminfo_ptr) == 0) break;
   /*XSync(this->display, False);*/
   state++;

   /*Xc_TRACE(("shmctl"));*/
   /*shmctl(shminfo_ptr->shmid, IPC_RMID, 0);*/

   if(this->error_flag) break;

   /*--- Fill buffer ---*/
   if(bytes_per_line == ximage->bytes_per_line)
    memcpy(ximage->data, data, size);
   else
   {
    register char *source, *target;
    register unsigned long i, len;

    Xc_TRACE(("bytes_per_line != ximage->bytes_per_line"));
    source = data;
    target =  ximage->data;
    len = ximage->bytes_per_line;
    for(i = height; i--; source += bytes_per_line, target += len)
    {
     memcpy(target, source, bytes_per_line);
     /*memset(target + bytes_per_line, 0, len - bytes_per_line);*/
    }
   }

   Xc_TRACE(("--> XShmImage"));
   item->type = XcPM_XSHM_XIMAGE;
   item->ximage = ximage;
   Xc_free(data);

   /*--- Insert in linked list ---*/
   item->next = this->base;
   this->base = item;
	
   Xc_TRACE(("allocated (#%lx)", item));
   return item;
  } while (0);

  if(state > 3) XShmDetach(this->display, shminfo_ptr);
  if(state > 2) shmdt(shminfo_ptr->shmaddr);
  if(state > 1) shmctl(shminfo_ptr->shmid, IPC_RMID, 0);
  if(state > 0) XDestroyImage(ximage);
 }
  
 if(item->type == XcPM_EMPTY)
 {
#endif
  ximage = XCreateImage(this->display, None, depth, format, 0, data,
			width, height, 8, bytes_per_line);

#if 0
  /*--- Put image on screen ---*/
  XPutImage(this->display, DefaultRootWindow(this->display),
	    this->gc, ximage, 0, 0, 0, 0, width, height);
#endif

  if(ximage == NULL)
  {
   Xc_ERROR(("XImage creation failed"));
   Xc_delete(this->handle, item);
   return NULL;
  }

  if(this->local_flag == FALSE || force_flag)
  {
   Pixmap pixmap;

   this->error_flag = FALSE;

   Xc_TRACE(("XCreatePixmap"));
   pixmap = XCreatePixmap(this->display,
			  DefaultRootWindow(this->display),
			  width, height,
			  depth);

   if(pixmap != (Pixmap)0)
   {
    Xc_TRACE(("XPutImage"));
    XPutImage(this->display, pixmap,
	      (depth == 1)? this->mono_gc : this->gc,
	      ximage, 0, 0, 0, 0, width, height);
    /*XSync(this->display, False);*/
   }
   else
    this->error_flag = TRUE;

   if(this->error_flag == FALSE)
   {
    Xc_TRACE(("--> Pixmap"));
    item->type = XcPM_PIXMAP;
    item->pixmap = pixmap;
    Xc_free(data);
    FREE_XIMAGE((char *)ximage);
   }
   else
   {
    Xc_TRACE(("--> XImage"));
    item->type = XcPM_XIMAGE;
    item->ximage = ximage;
   }
  }
  else
  {
   Xc_TRACE(("--> XImage (local host)"));
   item->type = XcPM_XIMAGE;
   item->ximage = ximage;
  }
#ifdef HAVE_XSHM      
 }
#endif
 if (force_flag && item->type != XcPM_PIXMAP)
 {
  Xc_ERROR(("PIXMAP creation failed."));
  Xc_delete(this->handle, item);
  Xc_free(data);
  FREE_XIMAGE((char *)ximage);
  return NULL;
 }
  
 /*--- Insert in linked list ---*/
 item->next = this->base;
 this->base = item;

 Xc_TRACE(("allocated (#%lx)", item));
 return item;
}


/* -------------------------------------------------------------------- **
**		Change Image Mode (Pixmap <-> XImage)			**
** -------------------------------------------------------------------- */
static boolean forceItem(this, item)
c_ImageManager *this;
image_item_t *item;
{
 Xc_HISTORY(("Force Image mode"));
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** drawItem - Render item                                            ** 
** ----------------------------------------------------------------- */
static boolean drawItem(this, item, drawable, gc, x, y, w, h, x_dest, y_dest)
c_ImageManager *this;
image_item_t *item;
Drawable drawable;
GC gc;
int x;
int y;
int w;
int h;
int x_dest;
int y_dest;
{
 Xc_HISTORY(("draw(%lx)", item));

 if(item == NULL)
 {
  Xc_TRACE(("NULL item"));
  return TRUE;
 }

 item->expose_count++;

 switch(item->type)
 {
  /*--- XImage ---*/
 case XcPM_XIMAGE:
  XPutImage(this->display, drawable, gc,
	    item->ximage, x, y, x_dest, y_dest, w, h);
  return TRUE;
  break;

  /*--- Pixmap ---*/
 case XcPM_PIXMAP:
  if(item->depth == 1)
   XCopyPlane(this->display, item->pixmap, drawable,
	      gc, x, y, w, h, x_dest, y_dest, 1L);
  else
   XCopyArea(this->display, item->pixmap, drawable,
	     gc, x, y, w, h, x_dest, y_dest);
  return TRUE;
  break;

  /*--- Shared memory XImage ---*/
 case XcPM_XSHM_XIMAGE:
#ifdef HAVE_XSHM
  if(XShmPutImage(this->display, drawable, gc,
		  item->ximage, x, y, x_dest, y_dest, w, h, False)== False)
  {
   Xc_TRACE(("Error with XShmPutImage"));
   return FALSE;
  }
#else
  Xc_BREAK(("Shared memory XImage?"));
#endif

 default:
  return TRUE;
  break;
 }
}


/* ----------------------------------------------------------------- ** 
** drawMasked - Draw twice with 2 GC                                 ** 
** ----------------------------------------------------------------- */
static boolean drawMasked(this, item, drawable, gc1, gc2, x, y, w, h, xd, yd)
c_ImageManager *this;
image_item_t *item;
Drawable drawable;
GC gc1;
GC gc2;
int x;
int y;
int w;
int h;
int xd;
int yd;
{
 Xc_HISTORY(("drawMasked(%lx)", item));
  
 if(item == NULL)
 {
  Xc_TRACE(("NULL item"));
  return TRUE;
 }
  
 item->expose_count++;

 switch(item->type)
 {
  /*--- XImage ---*/
 case XcPM_XIMAGE:
  XPutImage(this->display, drawable, gc1, item->ximage,
	    x, y, xd, yd, w, h);
  XPutImage(this->display, drawable, gc2, item->ximage,
	    x, y, xd, yd, w, h);
  return TRUE;
  break;

  /*--- Pixmap ---*/
 case XcPM_PIXMAP:
  if(item->depth == 1)
  {
   XCopyPlane(this->display, item->pixmap, drawable,
	      gc1, x, y, w, h, xd, yd, 1L);
   XCopyPlane(this->display, item->pixmap, drawable,
	      gc2, x, y, w, h, xd, yd, 1L);
  }
  else
  {
   XCopyArea(this->display, item->pixmap, drawable,
	     gc1, x, y, w, h, xd, yd);
   XCopyArea(this->display, item->pixmap, drawable,
	     gc2, x, y, w, h, xd, yd);
  }
  return TRUE;
  break;

  /*--- Shared memory XImage ---*/
 case XcPM_XSHM_XIMAGE:
#ifdef HAVE_XSHM
  if(XShmPutImage(this->display, drawable, gc1,
		  item->ximage, x, y, xd, yd, w, h, False)== False)
  {
   Xc_TRACE(("Error with XShmPutImage"));
   return FALSE;
  }
  if(XShmPutImage(this->display, drawable, gc2,
		  item->ximage, x, y, xd, yd, w, h, False)== False)
  {
   Xc_TRACE(("Error with XShmPutImage"));
   return FALSE;
  }
#else
  Xc_BREAK(("Shared memory XImage?"));
#endif

 default:
  return TRUE;
  break;
 }
}


/* -------------------------------------------------------------------- **
**			Free Pixmap or XImage				**
** -------------------------------------------------------------------- */
static void freeItem(this, item)
c_ImageManager *this;
image_item_t *item;
{
 image_item_t **ptr;

 Xc_HISTORY(("free #%lx", item));

 if(item == NULL) return;
 switch(item->type)
 {
  /*--- XImage ---*/
 case XcPM_XIMAGE :
  Xc_free(item->ximage->data);
  item->ximage->data = NULL;
  FREE_XIMAGE((char *)item->ximage);
  break;
      
  /*--- Pixmap ---*/
 case XcPM_PIXMAP :
  XFreePixmap(this->display, item->pixmap);
  break;
      
  /*--- XShmImage ---*/
 case XcPM_XSHM_XIMAGE :
#ifdef HAVE_XSHM
  XShmDetach(this->display, &item->shminfo);
  XDestroyImage(item->ximage);
  shmdt(item->shminfo.shmaddr);
  shmctl(item->shminfo.shmid, IPC_RMID, 0);
#endif

 default:
  break;
 }

 /*--- Remove from linked list ---*/
 ptr = &this->base;
 while(*ptr && *ptr != item)
  ptr = &((*ptr)->next);
 if (!*ptr)
  Xc_WARNING(("try to remove an unknow image_item_t"));
 else
  *ptr = item->next;

 Xc_delete(this->handle, item);

 Xc_TRACE(("freed"));
}


/* ----------------------------------------------------------------- ** 
** getBytesPerLine - Compute bytes per line for a width and a depth  ** 
** ----------------------------------------------------------------- */
static unsigned long getBytesPerLine(this, width, depth)
c_ImageManager	*this;
unsigned long width;
unsigned int depth;
{
 unsigned long bytes_per_line;

 Xc_HISTORY(("getBytesPerLine(%ld, depth %d)", width, depth));

 switch(depth)
 {
 case 1:
  bytes_per_line = (width + 7) >> 3;
  break;
      
 case 4:
  bytes_per_line = ((width + 7) >> 3) << 2;
  break;
      
 case 8:
  bytes_per_line = width;
  break;
      
 case 15:
 case 16:
  bytes_per_line = width << 1;
  break;
      
 case 24:
  if (this->real_depth == 24)
   bytes_per_line = width * 3;
  else
   bytes_per_line = width << 2;
  break;
      
 case 32:
  bytes_per_line = width << 2;
  break;
      
 default :
  Xc_WARNING(("Non implemented depth %d", depth));
  return 0;
 }
  
 Xc_TRACE(("bytes_per_line = %ld", bytes_per_line));
 return bytes_per_line;
}


/* -------------------------------------------------------------------- **
**			return bit_order_LSBFirst			**
** -------------------------------------------------------------------- */
static boolean 	bit_order_LSBFirst(this)
c_ImageManager	*this;
{
 return (this->bit_order_LSBFirst);
}

/* -------------------------------------------------------------------- **
**			return byte_order_LSBFirst			**
** -------------------------------------------------------------------- */
static boolean 	byte_order_LSBFirst(this)
c_ImageManager	*this;
{
 return (this->byte_order_LSBFirst);
}

/* -------------------------------------------------------------------- **
**                          return id Pixmap                           **
** -------------------------------------------------------------------- */
static Pixmap getPixmap(this, item)
c_ImageManager *this;
image_item_t *item;
{
 if (item->type != XcPM_PIXMAP)
  return (Pixmap)0;
 return item->pixmap;
}





