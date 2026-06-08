/*
** ImageCache.h for Xclamation, XAllWrite and XMayday in ImageCache/
** Definition of the cache of images
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
** Started on  Wed Oct 25 22:10:06 1995 Emmanuel Paris
** Last update Sat Jan 31 18:00:10 1998 Emmanuel Paris
*/

#ifndef _ImageCache_h
#define _ImageCache_h

typedef struct sc_ImageCache c_ImageCache;
typedef struct ImageCache_s ImageCache_t;

#include "xcalibur.h"
#include "Image.h"
#include "Colormap.h"
#include "DocFile.h"
#include "types.h"

#define XcDF_CACHE_CONFIG_KEYWORD "CACHE_CONFIG"
#define XcDF_MEMORY_CACHE_SIZE_KEYWORD "MEMORY_CACHE_SIZE"
#define XcDF_DISK_CACHE_SIZE_KEYWORD "DISK_CACHE_SIZE"
#define XcDF_KEEP_DISK_CACHE_KEYWORD "KEEP_DISK_CACHE"
#define XcDF_CACHE_DIR_KEYWORD "CACHE_DIR"

#define XcIC_BLOCKSIZE		64000

#define IMGDATA_NOT_LOADED	0
#define IMGDATA_IN_MEMORY	1
#define IMGDATA_ON_DISK		2

#define IMGLOAD_SCREEN_DEPTH	FALSE
#define IMGLOAD_IMAGE_DEPTH	TRUE

/* ----------------------------------------------------------------- ** 
** define the ImageCache Struct		                             ** 
** ----------------------------------------------------------------- */

struct ImageCache_s
{
 char	 *filename;
 ino_t	 inode;
 time_t mtime;
  
 boolean  load_mode;
 int	count;
 int	data_kept;
 long	data_size;		/* colormap not counted */
  
 time_t	data_time;
 char	*data;
 char	*cache_filename;
  
 c_Image	img_header;
  
 struct ImageCache_s	*Next;
};

/* ----------------------------------------------------------------- ** 
** define the method for ImageCache Class                           ** 
** ----------------------------------------------------------------- */
typedef struct
{
 F_STD;
 void	(*init) ___PROTO((c_ImageCache *This));
 ImageCache_t *(*load)();	/* ___PROTO((c_ImageCache *This, char *filename, 
				   c_Image *Image, image_load_t *load_module,
				   boolean mode)); */
 /* Image can be set to null, call get_header afterward */
 void	(*get_header) ___PROTO((c_ImageCache *This, ImageCache_t *id, 
				c_Image *Image));
 char	*(*get_data) ___PROTO((c_ImageCache *This, ImageCache_t *id));
 void	(*cache_data_on_disk) ___PROTO((c_ImageCache *This, ImageCache_t *id));
 void	(*add_reference_to_id) ___PROTO((c_ImageCache *This, 
					 ImageCache_t *id));
 void	(*delete_id) ___PROTO((c_ImageCache *This, ImageCache_t *id));
 void	(*empty_memory_cache) ___PROTO((c_ImageCache *This));
 void	(*empty_disk_cache) ___PROTO((c_ImageCache *This));
 void  (*set_memory_cache_size) ___PROTO((c_ImageCache *This, long new_size));
 void  (*set_disk_cache_size) ___PROTO((c_ImageCache *This, long new_size));
 boolean (*read_config) ___PROTO((c_DocFile *doc, char *keyword, long param,
				  c_ImageCache *This));
 boolean (*write_config) ___PROTO((c_ImageCache *This, c_DocFile *doc));
} sf_ImageCache;

/* ----------------------------------------------------------------- ** 
** define the ImageCache Class                                       ** 
** ----------------------------------------------------------------- */
struct sc_ImageCache
{
 sf_ImageCache	*f;
 
 int	nb_image_kept;
 ImageCache_t	*ImgCacheList;
 
 char	*cache_directory;	/* all these data are initialized in the */
 long	max_size_in_memory;	/* constructor and should be set by resource */
 long	max_size_on_disk;	/* definitions */
 boolean  keep_cache_on_disk;
 
 c_Colormap	*colormap;
 boolean	disk_cache_available;
 long		size_in_memory;
 long		size_on_disk;
 int32_t	file_id;
 int		byte_order;
};

extern sf_ImageCache fc_ImageCache;

#endif /* _ImageCache_h */











