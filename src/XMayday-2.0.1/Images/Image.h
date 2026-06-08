/*
** Image.h for XMayday in Images/
** Declaration for the Image class
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
** Started on  Wed Oct 26 15:46:19 1994 Emmanuel Paris
** Last update Wed Sep  4 16:55:21 1996 Emmanuel Paris
*/

#ifndef __XC_IMAGE_H__
#define __XC_IMAGE_H__

typedef struct sc_Image c_Image;
typedef struct image_load_s image_load_t;

#include "xcalibur.h"
#include "Colormap.h"
#include "ImageManager.h"
#include <X11/Xlib.h>

#define XcDF_IMAGE_KEYWORD "IMAGE"

enum { ROUGE,VERT,BLEU };

#define	BITMAP		1
#define PIXMAP4		2
#define	PIXMAP8		3
#define PIXMAP16	4
#define	PIXMAP24	5
#define	PIXMAP32	6

#define NO_MODE			0
#define ZOOM_NORMAL_MODE	1
#define ZOOM_AUTO_RESIZE_MODE	2
#define ZOOM_BEST_ASPECT_MODE	3	
#define ZOOM_MAX_ASPECT_MODE	4	
#define MAPPING1_MODE		5
#define MAPPING2_MODE		6

#define Xc_IMAGE_MIRROR_H	1
#define Xc_IMAGE_MIRROR_V	2

/*--- miscellaneous constant definitions ---*/
typedef void (*XcLoadImageProc) ___PROTO((unsigned char **pbuffer,
					  void *cb_data));

struct image_load_s {
  char *name;
  char *copyright;
  char *id;
  int extension_count;
  char **extensions;
  boolean (*test) ___PROTO((char *filename));
  error (*getHeader) ___PROTO((c_Image *image));
  error (*load) ___PROTO((c_Image *image, unsigned char *line_buffer,
			  XcLoadImageProc cb_func, void *cb_data));
};

extern image_load_t	*image_load_list[];

/*--- Methods for the Image class ---*/
typedef struct
{
  F_STD;
  char	*(*CalculImage) ___PROTO((c_Image *This, int width, int height));
  Pixmap (*CalculImagePixmap) ___PROTO((c_Image *This, int width, int height));
  void   (*ps_print) ___PROTO((c_Image *This, int (*ps_printf)(), int color));
} sf_Image;

/*--- Declaration of the Image class ---*/
struct sc_Image
{
  sf_Image	*f;
  
  int		type;        /* type du buffer data: BITMAP, PIXMAP8 etc... */
  
  int		depth;       /* profondeur: 1,8,24,32 */
  int		width;	       /* largeur */ 	
  int		height;        /* hauteur */
  int		byte_per_line; /* nombre d'octets par lignes */
  int		nb_color;      
  
  unsigned short        *pal_r;      /* palette composante rouge */
  unsigned short        *pal_v;      /* palette composante verte */
  unsigned short        *pal_b;      /* palette composante bleue */
  
  int		scrn_depth;
  int		scrn_byte_per_line;
  int		scrn_nb_color;
  
  boolean		scrn_pal_alloc;	  /* if scrn_pal has his own alloc */
  unsigned short        *scrn_pal_r;      /* palette composante rouge */
  unsigned short        *scrn_pal_v;      /* palette composante verte */
  unsigned short        *scrn_pal_b;      /* palette composante bleue */
  
  image_item_t	*handle;	/* ImageManager handle */
  void		*cache_id;	/* ImageCache id (ImageCache_t *) */
  char		*filename;
  image_load_t	*load_module;
  
  boolean	flip_H;
  boolean	flip_V;
};

extern sf_Image fc_Image;

#endif /* __XC_IMAGE_H__ */












