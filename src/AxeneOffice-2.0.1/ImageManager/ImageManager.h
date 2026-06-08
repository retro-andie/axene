/*
** ImageManager.h for Xclamation, XQuad, XAllWrite, XMayday and AxeneOffice 
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
** Last update Tue Dec  2 16:09:06 1997 Emmanuel Paris
*/

#ifndef _ImageManager_h
#define _ImageManager_h

typedef struct sc_ImageManager c_ImageManager;
typedef struct image_item_s image_item_t;

#include "xcalibur.h"
#include "XErrorHandler.h"
#include <X11/Xlib.h>
#ifdef HAVE_XSHM
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#endif


/*--- Macros ---*/
#define XcPM_X_BAD_ALLOCATION_ERROR_CODE	11
#define	XcPM_UNKOWN_PIXMAP			-1

#define XcPM_EMPTY		0
#define XcPM_PIXMAP		1
#define XcPM_XIMAGE		2
#define XcPM_XSHM_XIMAGE	3


/*--- Internal types ---*/
struct image_item_s
{
 image_item_t *next;

 int type;
 XImage *ximage;
 Pixmap pixmap;

 int width;
 int height;
 int depth;

 int expose_count;

#ifdef HAVE_XSHM
 XShmSegmentInfo shminfo;
#endif
};


/*--- Methods ---*/
typedef struct
{
 F_STD;

 image_item_t *(*allocate) ___PROTO((c_ImageManager *this, char *data,
				     int width, int height, int depth,
				     boolean Force));
 void (*free) ___PROTO((c_ImageManager *this, image_item_t *item));
 boolean (*draw) ___PROTO((c_ImageManager *this, image_item_t *item,
			   Drawable drawable, GC gc,
			   int XSrc, int YSrc, int width, int height,
			   int XDes, int YDes));
  
 boolean (*force) ___PROTO((c_ImageManager *this, image_item_t *item));
 boolean 	(*bit_order_LSBFirst)  ___PROTO((c_ImageManager *this));
 boolean	(*byte_order_LSBFirst) ___PROTO((c_ImageManager *this));
 unsigned long (*getBytesPerLine) ___PROTO((c_ImageManager *this,
					    unsigned long width,
					    unsigned int depth));
 boolean (*drawMasked) ___PROTO((c_ImageManager *this, image_item_t *item,
				 Drawable drawable, GC gc1, GC gc2,
				 int XSrc, int YSrc, int width, int height,
				 int XDes, int YDes));
 Pixmap (*getPixmap) ___PROTO((c_ImageManager *this, image_item_t *item));
} sf_ImageManager;


/*--- Class definition ---*/
struct sc_ImageManager
{
 sf_ImageManager *f;

 boolean	bit_order_LSBFirst;	/* used for Bitmap XImage */
 boolean	byte_order_LSBFirst;	/* used for > 8 bits ZImage */
  
 Display	*display; 
 GC		gc;
 GC		mono_gc;

 image_item_t	*base;

 int		(*func_def)();

 boolean	error_flag;	
 boolean	local_flag;		/* TRUE -> local station */

 ItemHandle	*handle;	

#ifdef HAVE_XSHM
 boolean	xshm_flag;		/* TRUE -> XShm supported */
#endif
  
 int		bytes_per_pixel;
 int		real_depth;
 boolean	draw_points_bug;
 boolean	draw_linesclip_bug;
};

extern sf_ImageManager fc_ImageManager;

#ifdef HAVE_XSHM
extern boolean xshm_flag;
#endif

#endif
