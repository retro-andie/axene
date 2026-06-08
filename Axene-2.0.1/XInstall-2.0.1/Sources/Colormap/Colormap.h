/*
** Colormap.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Colormap/
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
** Started on  Mon Jul 17 14:15:33 1995 Emmanuel Paris
** Last update Tue Feb  1 11:15:48 2000 Emmanuel Paris
*/

#ifndef _Colormap_h_
#define _Colormap_h_

#include "xcalibur.h"
#include <X11/Xlib.h>

/* colormap type */

#define CMAP_STATICGRAY		0
#define CMAP_GRAYSCALE		1
#define CMAP_PSEUDOCOLOR	2
#define CMAP_STATICCOLOR	3
#define CMAP_DIRECTCOLOR	4
#define CMAP_TRUECOLOR		5

#define FORCE_MONOCHROME	0
#define FORCE_GRAYSCALE		1
#define FORCE_COLOR		2
#define	NO_FORCE		3

#define CMAP_HASH_SIGN_BITS	12
#define CMAP_HASH_BITS		4
#define CMAP_HASH_MASK		0xf000

#define	BLACK			0
#define	RED			1
#define	GREEN			2
#define	BLUE			3
#define CYAN			4
#define YELLOW			5
#define MAGENTA			6
#define WHITE			7


/*--- lookup macro ---*/
#define XcC_INDEX_LOOKUP(colormap, index, Red, Green, Blue) \
{\
  if (colormap->hashtable) \
  { \
   unsigned int hashkey; \
    hashkey = (unsigned int)(Blue & CMAP_HASH_MASK) >> (sizeof(short)*8 -CMAP_HASH_BITS); \
     hashkey += (unsigned int)(Green & CMAP_HASH_MASK) >> (sizeof(short)*8 -2*CMAP_HASH_BITS); \
      hashkey += (unsigned int)(Red & CMAP_HASH_MASK) >>(sizeof(short)*8 -3*CMAP_HASH_BITS); \
       index = colormap->hashtable[hashkey]; \
} \
 else \
 { \
  color_cells_t	cell; \
   F(colormap).get_closest_color(colormap, &cell, (unsigned short)Red, (unsigned short)Green, (unsigned short)Blue); \
    index = cell.pixel; \
     Red -= cell.red; \
      Green -= cell.green; \
       Blue -= cell.blue; \
} \
}

#define XcC_GET_RED(colormap, index) \
(colormap->hashtable!=NULL?colormap->red[index]:0)
#define XcC_GET_GREEN(colormap, index) \
(colormap->hashtable!=NULL?colormap->green[index]:0)
#define XcC_GET_BLUE(colormap, index) \
(colormap->hashtable!=NULL?colormap->blue[index]:0)


/* define color_cells struct */
typedef struct 
{
 unsigned long	pixel;
 unsigned short	red;
 unsigned short	green;
 unsigned short	blue;
} color_cells_t;

/* define the methods for Class Colormap */
typedef struct
{
 F_STD;
 void		(*generate_colormap)();
 void		(*get_closest_color)();
 unsigned long  (*get_spec_color)();
 void		(*use_color)();
 void 		(*get_in_hash)();
 boolean	(*allocate_named_color)();
 boolean	(*get_RGB_values)();
} sf_Colormap;

/* define the Colormap class */
typedef struct
{
 sf_Colormap	*f;

 Display	*display;
 Colormap	cmap;
 int		screen;
 int		depth;		/* nb de plan */
 int		type;
 int		nb_color;	/* nb de couleur totale */
 int		max_color;	/* nb de couleur maximum à allouée */
 int		allocated;	/* nb de couleurs allouées */
 unsigned long	*pixels;	/* valeur de pixels des couleurs allouées */
 unsigned short	*red;
 unsigned short	*green;
 unsigned short	*blue;
 boolean	*usable_color;/* if TRUE: allocated color */
  
 int	       nb_spec_color;
 unsigned long *spec_color;
  
 unsigned long Cwhite;
 unsigned long Cblack;
 unsigned long Cred;
 unsigned long Cgreen;
 unsigned long Cblue;
 unsigned long Ccyan;
 unsigned long Cyellow;
 unsigned long Cmagenta;

 unsigned char	*hashtable;
 boolean	owncmap;
} c_Colormap;

extern sf_Colormap fc_Colormap;

#endif
