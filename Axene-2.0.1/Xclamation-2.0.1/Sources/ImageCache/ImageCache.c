/*
** ImageCache.c for Xclamation, XAllWrite and XMayday in ImageCache/
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
** Last update Sun Apr 25 15:34:00 1999 Emmanuel Paris
*/

#include "ImageCache.h"
#include "Dither.h"
#include "Resource.h"
#include "AlertBox.h"
#include "Cursor.h"
#include "file.h"
#include <time.h>

extern c_Resource	*GlobResources;

static void *cons_ImageCache();
static void dest_ImageCache();
static void *copy_ImageCache();
static void init_ImageCache();
static ImageCache_t *load();
static void get_header();
static char *get_data();
static void cache_data_on_disk();
static void add_reference_to_id();
static void delete_id();
static void empty_memory_cache();
static void empty_disk_cache();
static void set_memory_cache_size();
static void set_disk_cache_size();
static boolean read_config();
static boolean write_config();

static boolean simple_load_callback();
static boolean cb_memory_size();
static boolean cb_disk_size();
static boolean cb_keep_cache();
static boolean cb_cache_dir();

static void delete_id_forced();
static void read_index();
static void delete_index();
static void adjust_memory_cache();
static void adjust_disk_cache();
static int sort_id();

static boolean get_load_module();
static boolean read_image_header();
static boolean writeImage();
static boolean readHeader();
static boolean readData();
static char *get_new_filename();

static boolean ICwriteByteOrder();
static boolean ICwriteLoadModule();
static boolean ICwriteString();
static boolean ICwriteShort();
static boolean ICwriteByte();
static boolean ICwriteInteger();
static boolean ICwriteColormap();
static boolean ICwriteData();

static boolean ICreadByteOrder();
static boolean ICreadLoadModule();
static boolean ICreadString();
static boolean ICreadShort();
static boolean ICreadByte();
static boolean ICreadInteger();
static boolean ICreadColormap();
static boolean ICreadData();

sf_ImageCache fc_ImageCache =
{
 cons_ImageCache,
 dest_ImageCache,
 copy_ImageCache,
 init_ImageCache,
 load,
 get_header,
 get_data,
 cache_data_on_disk,
 add_reference_to_id,
 delete_id,
 empty_memory_cache,
 empty_disk_cache,
 set_memory_cache_size,
 set_disk_cache_size,
 read_config,
 write_config
};

/* ----------------------------------------------------------------- ** 
** construction method                                               ** 
** ----------------------------------------------------------------- */
static void *cons_ImageCache(colormap)
c_Colormap	*colormap;
{
 c_ImageCache *This;
  
 Xc_HISTORY(("Constructor ..."));
 if ((This = (c_ImageCache *)Xc_malloc("ImageCache",
				       sizeof(c_ImageCache))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation Signal"));
 }
 memset(This, 0, sizeof(c_ImageCache));
  
 This->f = &fc_ImageCache;
 This->colormap = colormap;
  
 This->nb_image_kept = 0;
 This->ImgCacheList = NULL;
  
#ifdef Xc_XCLAMATION
 This->cache_directory = Xc_strdup("cache dir", "/tmp/.xclamation-cache");
#endif

#ifdef Xc_XALLWRITE
 This->cache_directory = Xc_strdup("cache dir", "/tmp/.xallwrite-cache");
#endif
  
#ifdef Xc_XMAYDAY
 This->cache_directory = Xc_strdup("cache dir", "/tmp/.xmayday-cache");
#endif
  
 This->max_size_in_memory = 2000 * 1024;
 This->max_size_on_disk = 5000 * 1024;
 This->keep_cache_on_disk = FALSE;
 This->disk_cache_available = TRUE;
   
 This->file_id = 0;
 This->byte_order = Xc_BYTE_ORDER;
  
 Xc_HISTORY(("Constructor ok"));
 return (This);
}

/* ----------------------------------------------------------------- ** 
** destruction method                                                ** 
** ----------------------------------------------------------------- */
static void dest_ImageCache(This)
c_ImageCache *This;
{
 Xc_HISTORY(("Destructor ..."));
  
 while(This->nb_image_kept)
 {
  if (This->keep_cache_on_disk && This->disk_cache_available)
   F(This).cache_data_on_disk(This, This->ImgCacheList);
  delete_id_forced(This, This->ImgCacheList);
 }
  
 if (This->disk_cache_available && !This->keep_cache_on_disk)
  delete_index(This);
  
 Xc_free(This->cache_directory);
 Xc_free(This);
 Xc_HISTORY(("Destructor ok"));
}

/* -------------------------------------------------------------------- ** 
** 				copy method 				**
** -------------------------------------------------------------------- */
static void *copy_ImageCache(This)
c_ImageCache *This;
{
 c_ImageCache *ObjTmp;
  
 if ((ObjTmp = (c_ImageCache *)Xc_malloc("CImageCache", 
					 sizeof(c_ImageCache))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation Signal"));
 }
 memcpy(ObjTmp, This, sizeof(c_ImageCache));
 Xc_TRACE(("Object ImageCache copied but not working"));
 return ObjTmp;
}

/* -------------------------------------------------------------------- ** 
** 				init method 				**
** -------------------------------------------------------------------- */
static void init_ImageCache(This)
c_ImageCache *This;
{
 int	lng;
  
 lng = strlen(This->cache_directory);
 if (lng >= 1 && This->cache_directory[lng-1] == '/')
 {
  This->cache_directory[lng-1 ] = '\0';
 }
  
 if (!IsDirWritable(This->cache_directory))
 {
  Xc_TRACE(("cache directory: %s doesn't exist, try to create it",
	    This->cache_directory));
  if (mkdir(This->cache_directory, 0777) == -1)
  {
   Xc_TRACE(("Can not create dir: %s cache not available",
	     This->cache_directory));
   This->disk_cache_available = FALSE;
  }
 }
 if (This->disk_cache_available)
 {
  read_index(This);
 }
}

static ImageCache_t *load(This, filename, Image, load_module, mode)
c_ImageCache *This;
char *filename;
c_Image *Image;
image_load_t *load_module;
boolean mode;
{
 ImageCache_t	*id, *previd;
 ino_t	inode;
 time_t mtime;
 struct stat buf;


#ifndef NHISTORY
 if (load_module)
  Xc_HISTORY(("load %s picture (%s)",	load_module->name, filename));
 else
  Xc_HISTORY(("load picture (%s)", filename));
#endif
  
 if (lstat(filename, &buf) == -1)
  return NULL;
 inode = buf.st_ino;
 mtime = buf.st_mtime;
  
 id = previd = This->ImgCacheList;
 while(id)
 {
  if (Xstrcmp(id->filename, filename) &&
      id->inode == inode && id->mtime >= mtime && id->load_mode == mode)
   break;
  previd = id;
  id = id->Next;
 }
  
 if (id != NULL)
 {
  id->count += 1;
  if (Image)
   get_header(This, id, Image);
  return id;
 }
 id = (ImageCache_t	*)Xc_malloc("imgcache id", sizeof(ImageCache_t));
 memset(id, 0, sizeof(ImageCache_t));
 id->filename = id->img_header.filename = 
  Xc_strdup("id filename", filename);
 id->inode = inode;
 id->mtime = mtime;
 id->load_mode = mode;
 id->count = 1;
 id->data_kept = IMGDATA_NOT_LOADED;
 id->data_size = 0;
  
 if (load_module)
 {
  id->img_header.load_module = load_module;
 }
 else
 {
  if (!get_load_module(This, id))
  {
   Xc_free(id->filename);
   Xc_free(id);
   return NULL;
  }
 }
  
 if (!read_image_header(This, id))
 {
  Xc_free(id->filename);
  Xc_free(id);
  return NULL;
 }
 id->Next = NULL;
 if (previd)
  previd->Next = id;
 else
  This->ImgCacheList = id;
 This->nb_image_kept += 1;
  
 if (Image)
  get_header(This, id, Image);
 return id;
}

static void get_header(This, id, Image)
c_ImageCache *This;
ImageCache_t *id;
c_Image *Image;
{
 Image->type		= id->img_header.type;
  
 Image->depth		= id->img_header.depth;
 Image->width		= id->img_header.width;
 Image->height		= id->img_header.height;
 Image->byte_per_line	= id->img_header.byte_per_line;
 Image->nb_color	= id->img_header.nb_color;  
 Image->flip_H		= id->img_header.flip_H;  
 Image->flip_V		= id->img_header.flip_V;  

 Image->pal_r		= id->img_header.pal_r;
 Image->pal_v		= id->img_header.pal_v;
 Image->pal_b		= id->img_header.pal_b;
  
 Image->scrn_depth		= id->img_header.scrn_depth;
 Image->scrn_byte_per_line	= id->img_header.scrn_byte_per_line;
 Image->scrn_nb_color		= id->img_header.scrn_nb_color;  
  
 Image->scrn_pal_alloc		= id->img_header.scrn_pal_alloc;
 Image->scrn_pal_r		= id->img_header.scrn_pal_r;
 Image->scrn_pal_v		= id->img_header.scrn_pal_v;
 Image->scrn_pal_b		= id->img_header.scrn_pal_b;
  
 Image->filename	= id->img_header.filename;
 Image->load_module	= id->img_header.load_module;
}

static char *get_data(This, id)
c_ImageCache *This;
ImageCache_t *id;
{
 unsigned char *line_buffer;
 c_Dither *dither;
 char	*data = NULL;
 boolean need_dither = TRUE;
 int	dither_mode = 0;
 long	data_size;
 error	rc;
  
 switch(id->data_kept)
 {
 case IMGDATA_NOT_LOADED:
  Xc_TRACE(("IMGDATA_NOT_LOADED"));
  data_size = id->img_header.scrn_byte_per_line * id->img_header.height;
  id->data_size = 0;
  data = Xc_malloc("Image data", data_size);
  if (data == NULL)
  {
   char	erreur[100];
      
   sprintf(erreur, 
	   F(GlobResources).getString(GlobResources, XcR_memoryError),
	   data_size);
   Xc_ErrorAlert(erreur);
   break;
  }
    
  switch(id->img_header.type)
  {
  case BITMAP:
   switch(id->img_header.depth)
   {
   case 1:
    need_dither = FALSE;
    break;
   case 8:
    dither_mode = XcD_8_TO_2;
    break;
   case 24:
    dither_mode = XcD_24_TO_2;	
    break;
   }
   break;
  case PIXMAP8:
   switch(id->img_header.depth)
   {
   case 1:
    need_dither = FALSE;
    break;
   case 8:
    dither_mode = XcD_8_TO_8;
    break;
   case 24:
    dither_mode = XcD_24_TO_8;	
    break;
   }
   break;
  case PIXMAP16:
   switch(id->img_header.depth)
   {
   case 1:
    need_dither = FALSE;
    break;
   case 8:
    dither_mode = XcD_8_TO_16;
    break;
   case 24:
    dither_mode = XcD_24_TO_16;	
    break;
   }
   break;
  case PIXMAP24:
   switch(id->img_header.depth)
   {
   case 1:
    need_dither = FALSE;
    break;
   case 8:
    dither_mode = XcD_8_TO_24;
    break;
   case 24:
    dither_mode = XcD_24_TO_24;	
    break;
   }
   break;
  }
    
  if (need_dither)
  {
   line_buffer = Xc_malloc("line-buffer", 
			   id->img_header.byte_per_line);
   dither = NEW(c_Dither)(dither_mode, This->colormap, 
			  &(id->img_header), data);
   if (line_buffer == NULL || dither == NULL)
   {
    char	erreur[100];
	    
    sprintf(erreur, 
	    F(GlobResources).getString(GlobResources, XcR_memoryError),
	    id->img_header.byte_per_line);
    Xc_ErrorAlert(erreur);
    if (line_buffer) Xc_free(line_buffer);
    Xc_free(data); data = NULL;
    break;
   }
   SET_WAIT_MODE_ANIM;
   rc = id->img_header.load_module->load(&(id->img_header), line_buffer,
					 (void *)dither->callback,
					 (void *)dither);
   UNSET_WAIT_MODE_ANIM;
   DELETE(c_Dither)(dither);
      
   Xc_free(line_buffer);
   if (rc != XC_NO_ERROR)
   {
    Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					     XcR_loadError));
    Xc_free(data); data = NULL;
    break;
   }
  }
  else
  {
   SET_WAIT_MODE_ANIM;
   rc = id->img_header.load_module
    ->load(&(id->img_header), data,
	   (void *)simple_load_callback,
	   (void *)id->img_header.byte_per_line);
   UNSET_WAIT_MODE_ANIM;
   if (rc != XC_NO_ERROR)
   {
    Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					     XcR_loadError));
    Xc_free(data); data = NULL;
    break;
   }
  }
  id->data_time = time(NULL);
    
  id->data_kept = IMGDATA_IN_MEMORY;
  id->data = data;
  id->data_size = data_size;
    
  This->size_in_memory += id->data_size;
  if (This->size_in_memory >= This->max_size_in_memory)
   adjust_memory_cache(This, id);
  break;
 case IMGDATA_IN_MEMORY:
  Xc_TRACE(("IMGDATA_IN_MEMORY"));
  id->data_time = time(NULL);
  data = id->data;
  break;
 case IMGDATA_ON_DISK:
  Xc_TRACE(("IMGDATA_ON_DISK"));
  id->data_time = time(NULL);
    
  if (!readData(This, id))
  {
   Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					    XcR_loadError));
   id->data_kept = IMGDATA_NOT_LOADED;
   unlink(id->cache_filename);
   if (id->cache_filename)
    Xc_free(id->cache_filename);
   This->size_on_disk -= id->data_size;
   id->data_size = 0;
   if (!get_load_module(This, id) || !read_image_header(This, id))
    return NULL;
   data = F(This).get_data(This, id);
   break;
  }
  data = id->data;
    
  id->data_kept = IMGDATA_IN_MEMORY;
  unlink(id->cache_filename);
  Xc_free(id->cache_filename);
    
  This->size_in_memory += id->data_size;
  This->size_on_disk -= id->data_size;
  if (This->size_in_memory >= This->max_size_in_memory)
   adjust_memory_cache(This, id);
  break;
 }
 return data;
}

static char *get_new_filename(This)
c_ImageCache *This;
{
 char	*filename;
  
 filename = (char *)Xc_malloc("new file", strlen(This->cache_directory) + 16);
 do
 {
  sprintf(filename,"%s/cache%.8x", This->cache_directory,
	  (int)This->file_id);
  This->file_id += 1;
 }
 while(IsFileExist(filename));
  
 return filename;
}

static void cache_data_on_disk(This, id)
c_ImageCache *This;
ImageCache_t *id;
{
 if (id->data_kept != IMGDATA_IN_MEMORY || !This->disk_cache_available)
  return;
  
 if (This->size_on_disk + id->data_size < This->max_size_on_disk)
 {
  id->cache_filename = get_new_filename(This);
    
  if (!writeImage(This, id))
  {
   Xc_free(id->cache_filename);
   id->data_kept = IMGDATA_NOT_LOADED;
   Xc_free(id->data);
   This->size_in_memory -= id->data_size;
   id->data_size = 0;
  }
  else
  {
   id->data_kept = IMGDATA_ON_DISK;
   Xc_free(id->data);
   This->size_in_memory -= id->data_size;
   This->size_on_disk += id->data_size;
  }
 }
 else
 {
  id->data_kept = IMGDATA_NOT_LOADED;
  Xc_free(id->data);
  This->size_in_memory -= id->data_size;
  id->data_size = 0;
 }
}

static void add_reference_to_id(This, id)
c_ImageCache *This;
ImageCache_t *id;
{
 id->count += 1;
}

static void delete_id(This, id)
c_ImageCache *This;
ImageCache_t *id;
{
 if (--(id->count) < 0)
 {
  Xc_WARNING(("internal error, this cache id is not referenced"));
  delete_id_forced(This, id);
 } 
}

static void delete_id_forced(This, id)
c_ImageCache *This;
ImageCache_t *id;
{
 ImageCache_t *previd;
  
 previd = This->ImgCacheList;
 if (previd == id)
  This->ImgCacheList = id->Next;
 else
 {
  while(previd && previd->Next != id)
   previd = previd->Next;
      
  if (previd == NULL)
  {
   Xc_WARNING(("What? id not in list, internal error"));
  }
  else
   previd->Next = id->Next;
 }
 Xc_free(id->filename);
 if (id->img_header.nb_color > 0)
 {
  Xc_free(id->img_header.pal_r);
  Xc_free(id->img_header.pal_v);
  Xc_free(id->img_header.pal_b);
 }
 if (id->img_header.scrn_nb_color > 0 && id->img_header.scrn_pal_alloc)
 {
  Xc_free(id->img_header.scrn_pal_r);
  Xc_free(id->img_header.scrn_pal_v);
  Xc_free(id->img_header.scrn_pal_b);
 }    
 switch(id->data_kept)
 {
 case IMGDATA_NOT_LOADED:
  break;
 case IMGDATA_IN_MEMORY:
  Xc_free(id->data);
  break;
 case IMGDATA_ON_DISK:
  if (!This->keep_cache_on_disk)
   unlink(id->cache_filename);
  Xc_free(id->cache_filename);
  break;
 }
 Xc_free(id);
 This->nb_image_kept -= 1;
}

static void empty_memory_cache(This)
c_ImageCache *This;
{
 ImageCache_t *id;

 id = This->ImgCacheList;
 while(id)
 {
  if (id->data_kept == IMGDATA_IN_MEMORY)
  {
   id->data_kept = IMGDATA_NOT_LOADED;
   Xc_free(id->data);
   This->size_in_memory -= id->data_size;
   id->data_size = 0;
  }
  id = id->Next;
 }
}

static void empty_disk_cache(This)
c_ImageCache *This;
{
 ImageCache_t *id;

 id = This->ImgCacheList;
 while(id)
 {
  if (id->data_kept == IMGDATA_ON_DISK)
  {
   unlink(id->cache_filename);
   Xc_free(id->cache_filename);
   id->data_kept = IMGDATA_NOT_LOADED;
   This->size_on_disk -= id->data_size;
   id->data_size = 0;
  }
  id = id->Next;
 }
}

static void set_memory_cache_size(This, new_size)
c_ImageCache *This;
long new_size;
{
 This->max_size_in_memory = new_size;
 if (This->size_in_memory >= This->max_size_in_memory)
  adjust_memory_cache(This, NULL);
}

static void set_disk_cache_size(This, new_size)
c_ImageCache *This;
long new_size;
{
 This->max_size_on_disk = new_size;
 if (This->size_on_disk >= This->max_size_on_disk)
  adjust_disk_cache(This, NULL);
}

static boolean read_config(doc, keyword, param, This)
c_DocFile *doc;
char *keyword;
long param;
c_ImageCache *This;
{
 if( !F(doc).addCallbacks
    (doc,
     XcDF_MEMORY_CACHE_SIZE_KEYWORD, cb_memory_size, This,
     XcDF_DISK_CACHE_SIZE_KEYWORD, cb_disk_size, This,
     XcDF_KEEP_DISK_CACHE_KEYWORD, cb_keep_cache, This,
     XcDF_CACHE_DIR_KEYWORD, cb_cache_dir, This,
     NULL)) 
  return FALSE;
  
 if(!F(doc).expectKeyword(doc, NULL, TRUE))
  return FALSE;
  
 return TRUE;
}

static boolean write_config(This, doc)
c_ImageCache *This;
c_DocFile *doc;
{
 if(!F(doc).writeKeyword(doc, XcDF_MEMORY_CACHE_SIZE_KEYWORD,
			 This->max_size_in_memory))
  return FALSE;
  
 if(!F(doc).writeKeyword(doc, XcDF_DISK_CACHE_SIZE_KEYWORD,
			 This->max_size_on_disk))
  return FALSE;
  
 if (This->keep_cache_on_disk) 
  if(!F(doc).writeKeyword(doc, XcDF_KEEP_DISK_CACHE_KEYWORD, -1L))
   return FALSE;
  
 if (This->disk_cache_available && 
     (!F(doc).writeKeyword(doc, XcDF_CACHE_DIR_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !F(doc).write(doc, 
		    This->cache_directory, strlen(This->cache_directory)) ||
      !F(doc).endSequence(doc)))
  return FALSE;
  
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** Simple load callback                                              ** 
** ----------------------------------------------------------------- */
static boolean simple_load_callback(pbuffer, data)
unsigned char **pbuffer;
void *data;
{
 *pbuffer += (long)data;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_memory_size - memory size loading callback                     ** 
** ----------------------------------------------------------------- */
static boolean cb_memory_size(doc, keyword, param, This)
c_DocFile *doc;
char *keyword;
long param;
c_ImageCache *This;
{
 if (param > 0 && param <256000)
  This->max_size_in_memory = param * 1024;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_disk_size - disk size loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_disk_size(doc, keyword, param, This)
c_DocFile *doc;
char *keyword;
long param;
c_ImageCache *This;
{
 if (param > 0 && param < 1000000)
  This->max_size_on_disk = param * 1024;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_keep_cache - keep cache loading callback                       ** 
** ----------------------------------------------------------------- */
static boolean cb_keep_cache(doc, keyword, param, This)
c_DocFile *doc;
char *keyword;
long param;
c_ImageCache *This;
{
 This->keep_cache_on_disk = TRUE;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_cache_dir - cache dir loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_cache_dir(doc, keyword, param, This)
c_DocFile *doc;
char *keyword;
long param;
c_ImageCache *This;
{
 unsigned char *name;
 char	*home_dir, *new_name;
  
 if (!F(doc).readString(doc, &name)) 
  return FALSE;
 if (name != NULL)
 {
  Xc_free(This->cache_directory);
  if ( *name == '~')
  {
   home_dir = getenv("HOME");
   if (home_dir == NULL) 
    home_dir = "/";
   new_name = Xc_malloc("cache dir", strlen(name) + strlen(home_dir) + 1);
   sprintf(new_name, "%s%s", home_dir, name+1);
   Xc_free(name);
   name = (unsigned char *)new_name;
  }
  This->cache_directory = (char *)name;
 }
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** read_index - read index                                           ** 
** ----------------------------------------------------------------- */
static void read_index(This)
c_ImageCache	*This;
{
 DIR	*handle;
 struct dirent *entry;
 int32_t	file_id;
 char		*endptr;
 ImageCache_t	*id, *previd;
  
 Xc_TRACE(("opening dir: %s", This->cache_directory));
 if ((handle = opendir(This->cache_directory)) == NULL)
  return;
  
 while((entry = readdir(handle)) != 0)
 {
  if (REAL_DIR_ENTRY(entry))
   continue;
    
  Xc_TRACE(("dir entry: %s", entry->d_name));
  if ((strlen(entry->d_name) == 13) && 
      (Xstrncmp(entry->d_name, "cache", 5)))
  {
   file_id = strtol(entry->d_name+5, &endptr, 16);
   if (endptr == NULL || *endptr != '\0')
    continue;
   if (This->file_id < file_id)
    This->file_id = file_id;
   id = (ImageCache_t *)Xc_malloc("imgcache id", sizeof(ImageCache_t));
   memset(id, 0, sizeof(ImageCache_t));
   id->cache_filename = 
    (char *)Xc_malloc("id filename", strlen(This->cache_directory) + 15);
   sprintf(id->cache_filename, "%s/%s", This->cache_directory, 
	   entry->d_name); 
   id->count = 0;
   id->data_kept = IMGDATA_ON_DISK;
   if (!readHeader(This, id) || 
       (id->img_header.scrn_depth == 16 && This->colormap->depth != 16 &&
	This->colormap->depth != 15))
   {
    unlink(id->cache_filename);
    Xc_free(id->cache_filename);
    Xc_free(id);
    continue;
   }
      
   Xc_TRACE(("image: file: %s", id->filename));
   Xc_TRACE(("image: %dx%dx%d (colormap depth: %d)",
	     id->img_header.width, id->img_header.height, 
	     id->img_header.depth, This->colormap->depth));

   id->data_size = id->img_header.scrn_byte_per_line*id->img_header.height;
   id->Next = NULL;
   previd = This->ImgCacheList;
   while(previd && previd->Next)
    previd = previd->Next;
   if (previd)
    previd->Next = id;
   else
    This->ImgCacheList = id;
   This->size_on_disk += id->data_size;
   This->nb_image_kept += 1;
  }
 }
 closedir(handle);
}

/* ----------------------------------------------------------------- ** 
** delete_index - delete index                                       ** 
** ----------------------------------------------------------------- */

static void delete_index(This)
c_ImageCache	*This;
{
 DIR	*handle;
 struct dirent *entry;
 char		*filename;
  
 Xc_TRACE(("opening dir: %s", This->cache_directory));
 if ((handle = opendir(This->cache_directory)) == NULL)
  return;
  
 filename = 
  (char *)Xc_malloc("filename", strlen(This->cache_directory) + 15);
  
 while((entry = readdir(handle)) != 0)
 {
  if (REAL_DIR_ENTRY(entry))
   continue;
    
  Xc_TRACE(("dir entry: %s", entry->d_name));
  if ((strlen(entry->d_name) == 13) && 
      (Xstrncmp(entry->d_name, "cache", 5)))
  {
   sprintf(filename, "%s/%s", This->cache_directory, entry->d_name); 
   unlink(filename);
  }
 }
 Xc_free(filename);
  
 closedir(handle);
}
/* ----------------------------------------------------------------- ** 
** adjust_memory_cache - adjust memory cache                         ** 
** ----------------------------------------------------------------- */
static void adjust_memory_cache(This, id)
c_ImageCache	*This;
ImageCache_t *id;
{
 ImageCache_t	**tab_id, *mid;
 int		nb_id, i;
  
 Xc_TRACE(("adjust memory cache"));
 nb_id = sort_id(This, id, IMGDATA_IN_MEMORY, &tab_id);
 i = 0;
 while(nb_id && This->size_in_memory >= This->max_size_in_memory)
 {
  mid = tab_id[i++];
  if (This->disk_cache_available)
  {
   mid->cache_filename = get_new_filename(This);
      
   if (!writeImage(This, mid))
   {
    Xc_free(mid->cache_filename);
    mid->data_kept = IMGDATA_NOT_LOADED;
    Xc_free(mid->data);
    This->size_in_memory -= mid->data_size;
    mid->data_size = 0;
   }
   else
   {
    mid->data_kept = IMGDATA_ON_DISK;
    Xc_free(mid->data);
    This->size_in_memory -= mid->data_size;
    This->size_on_disk += mid->data_size;
    if (This->size_on_disk >= This->max_size_on_disk)
     adjust_disk_cache(This, mid);
   }
  }
  else
  {
   Xc_free(mid->data);
   mid->data_kept = IMGDATA_NOT_LOADED;
   This->size_in_memory -= mid->data_size;
   mid->data_size = 0;
  }
  nb_id--;
 }
 if (tab_id)
  Xc_free(tab_id);
 Xc_HISTORY(("adjust memory cache done"));
}

/* ----------------------------------------------------------------- ** 
** adjust_disk_cache - adjust disk cache                             ** 
** ----------------------------------------------------------------- */
static void adjust_disk_cache(This, id)
c_ImageCache	*This;
ImageCache_t *id;
{
 ImageCache_t	**tab_id, *mid;
 int		nb_id, i;
  
 nb_id = sort_id(This, id, IMGDATA_ON_DISK, &tab_id);
 i = 0;
 while(nb_id && This->size_on_disk >= This->max_size_on_disk)
 {
  mid = tab_id[i++];
  mid->data_kept = IMGDATA_NOT_LOADED;
  unlink(mid->cache_filename);
  Xc_free(mid->cache_filename);
  This->size_on_disk -= mid->data_size;
  mid->data_size = 0;
  nb_id--;
 }
 if (tab_id)
  Xc_free(tab_id);
}

/* ----------------------------------------------------------------- ** 
** get_load_module                                                   ** 
** ----------------------------------------------------------------- */
static boolean get_load_module(This, id)
c_ImageCache	*This;
ImageCache_t *id;
{
 int i = 0;
  
 while(image_load_list[i] != NULL )
 {
  Xc_TRACE(("testing `%s'", image_load_list[i]->name));
  if (image_load_list[i]->test(id->filename) == TRUE)
   break;
  i++;
 }
 if (image_load_list[i] == NULL)
 {
  Xc_TRACE(("`%s' image format unknow", id->filename));
  return FALSE;
 }
 else
 {
  id->img_header.load_module = image_load_list[i];
 }
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** read_image_header - load real image header                        ** 
** ----------------------------------------------------------------- */
static boolean read_image_header(This, id)
c_ImageCache	*This;
ImageCache_t *id;
{
 error	rc;
  
 rc = id->img_header.load_module->getHeader(&id->img_header);
 if (rc != XC_NO_ERROR)
 {
  Xc_WARNING(("`%s' image error %d", id->filename, rc));
  return FALSE;
 }
  
 Xc_TRACE(("image: file: %s", id->filename));
 Xc_TRACE(("image: %dx%dx%d (colormap depth: %d)",
	   id->img_header.width, id->img_header.height, 
	   id->img_header.depth, This->colormap->depth));
 id->img_header.scrn_pal_alloc = FALSE;
  
 switch(id->img_header.depth)
 {
  /*--- Monochrome image ---*/
 case 1:
  id->img_header.type = BITMAP;
  id->img_header.scrn_depth = id->img_header.depth;
  id->img_header.byte_per_line = id->img_header.scrn_byte_per_line = 
   (id->img_header.width + 7) >> 3;
  id->img_header.nb_color = id->img_header.scrn_nb_color = 2;
  id->img_header.scrn_pal_r = This->colormap->red;
  id->img_header.scrn_pal_v = This->colormap->green;
  id->img_header.scrn_pal_b = This->colormap->blue;
  break;
 case 8:
  id->img_header.byte_per_line = id->img_header.width;
  id->img_header.nb_color = 256;
    
  if (id->load_mode == IMGLOAD_SCREEN_DEPTH)
  {
   switch(This->colormap->depth)
   {
   case 1:
    id->img_header.type = BITMAP;
    id->img_header.scrn_depth = 1;
    id->img_header.scrn_byte_per_line = (id->img_header.width+7)>>3;
    id->img_header.scrn_nb_color = 2;
    id->img_header.scrn_pal_r = This->colormap->red;
    id->img_header.scrn_pal_v = This->colormap->green;
    id->img_header.scrn_pal_b = This->colormap->blue;
    break;
   case 8:
    id->img_header.type = PIXMAP8;
    id->img_header.scrn_depth = 8;
    id->img_header.scrn_byte_per_line = id->img_header.byte_per_line;
    id->img_header.scrn_nb_color = 256;
    id->img_header.scrn_pal_r = This->colormap->red;
    id->img_header.scrn_pal_v = This->colormap->green;
    id->img_header.scrn_pal_b = This->colormap->blue;
    break;
   case 15:
   case 16:
    id->img_header.type = PIXMAP16;
    id->img_header.scrn_depth = 16;
    id->img_header.scrn_byte_per_line = 
     id->img_header.byte_per_line << 1;
    id->img_header.scrn_nb_color = -1;
    id->img_header.scrn_pal_r = id->img_header.pal_r;
    id->img_header.scrn_pal_v = id->img_header.pal_v;
    id->img_header.scrn_pal_b = id->img_header.pal_b;
    break;
   case 24:
    id->img_header.type = PIXMAP24;
    id->img_header.scrn_depth = 24;
    id->img_header.scrn_byte_per_line =
     id->img_header.byte_per_line * 3;
    id->img_header.scrn_nb_color = -1;
    id->img_header.scrn_pal_r = id->img_header.pal_r;
    id->img_header.scrn_pal_v = id->img_header.pal_v;
    id->img_header.scrn_pal_b = id->img_header.pal_b;
    break;
   default:
    Xc_ERROR(("Image depth not implemented"));
    return FALSE;
   }
  }
  else				/* IMGLOAD_IMAGE_DEPTH */
  {
   id->img_header.type = PIXMAP8;
   id->img_header.scrn_depth = id->img_header.depth;
   id->img_header.scrn_byte_per_line = id->img_header.byte_per_line;
   id->img_header.scrn_nb_color = id->img_header.nb_color;
   id->img_header.scrn_pal_r = id->img_header.pal_r;
   id->img_header.scrn_pal_v = id->img_header.pal_v;
   id->img_header.scrn_pal_b = id->img_header.pal_b;
  }
  break;
 case 16:
  id->img_header.byte_per_line = id->img_header.width << 1;
  id->img_header.nb_color = -1;
  id->img_header.pal_r = NULL;
  id->img_header.pal_v = NULL;
  id->img_header.pal_b = NULL;
  if (id->load_mode == IMGLOAD_SCREEN_DEPTH)
  {
   switch(This->colormap->depth)
   {
   case 1:
    id->img_header.type = BITMAP;
    id->img_header.scrn_depth = 1;
    id->img_header.scrn_byte_per_line = (id->img_header.width+7)>>3;
    id->img_header.scrn_nb_color = 2;
    id->img_header.scrn_pal_r = This->colormap->red;
    id->img_header.scrn_pal_v = This->colormap->green;
    id->img_header.scrn_pal_b = This->colormap->blue;
    break;
   case 8:
    id->img_header.type = PIXMAP8;
    id->img_header.scrn_depth = 8;
    id->img_header.scrn_byte_per_line = id->img_header.width;
    id->img_header.scrn_nb_color = 256;
    id->img_header.scrn_pal_r = This->colormap->red;
    id->img_header.scrn_pal_v = This->colormap->green;
    id->img_header.scrn_pal_b = This->colormap->blue;
    break;
   case 15:
   case 16:
    id->img_header.type = PIXMAP16;
    id->img_header.scrn_depth = id->img_header.depth;
    id->img_header.scrn_byte_per_line = id->img_header.byte_per_line;
    id->img_header.scrn_nb_color = id->img_header.nb_color;
    id->img_header.scrn_pal_r = id->img_header.pal_r;
    id->img_header.scrn_pal_v = id->img_header.pal_v;
    id->img_header.scrn_pal_b = id->img_header.pal_b;
    break;
   case 24:
    id->img_header.type = PIXMAP24;
    id->img_header.scrn_depth = 24;
    id->img_header.scrn_byte_per_line = id->img_header.width * 3;
    id->img_header.scrn_nb_color = -1;
    id->img_header.scrn_pal_r = id->img_header.pal_r;
    id->img_header.scrn_pal_v = id->img_header.pal_v;
    id->img_header.scrn_pal_b = id->img_header.pal_b;
    break;
   default:
    Xc_ERROR(("Image depth not implemented"));
    return FALSE;
   }
  }
  else				/* IMGLOAD_IMAGE_DEPTH */
  {
   id->img_header.type = PIXMAP16;
   id->img_header.scrn_depth = id->img_header.depth;
   id->img_header.scrn_byte_per_line = id->img_header.byte_per_line;
   id->img_header.scrn_nb_color = id->img_header.nb_color;
   id->img_header.scrn_pal_r = id->img_header.pal_r;
   id->img_header.scrn_pal_v = id->img_header.pal_v;
   id->img_header.scrn_pal_b = id->img_header.pal_b;
  }
  break;
 case 24:
  id->img_header.byte_per_line = id->img_header.width * 3;
  id->img_header.nb_color = -1;
  id->img_header.pal_r = NULL;
  id->img_header.pal_v = NULL;
  id->img_header.pal_b = NULL;
  if (id->load_mode == IMGLOAD_SCREEN_DEPTH)
  {
   switch(This->colormap->depth)
   {
   case 1:
    id->img_header.type = BITMAP;
    id->img_header.scrn_depth = 1;
    id->img_header.scrn_byte_per_line = (id->img_header.width+7)>>3;
    id->img_header.scrn_nb_color = 2;
    id->img_header.scrn_pal_r = This->colormap->red;
    id->img_header.scrn_pal_v = This->colormap->green;
    id->img_header.scrn_pal_b = This->colormap->blue;
    break;
   case 8:
    id->img_header.type = PIXMAP8;
    id->img_header.scrn_depth = 8;
    id->img_header.scrn_byte_per_line = id->img_header.width;
    id->img_header.scrn_nb_color = 256;
    id->img_header.scrn_pal_r = This->colormap->red;
    id->img_header.scrn_pal_v = This->colormap->green;
    id->img_header.scrn_pal_b = This->colormap->blue;
    break;
   case 15:
   case 16:
    id->img_header.type = PIXMAP16;
    id->img_header.scrn_depth = 16;
    id->img_header.scrn_byte_per_line = id->img_header.width << 1;
    id->img_header.scrn_nb_color = -1;
    id->img_header.scrn_pal_r = id->img_header.pal_r;
    id->img_header.scrn_pal_v = id->img_header.pal_v;
    id->img_header.scrn_pal_b = id->img_header.pal_b;
    break;
   case 24:
    id->img_header.type = PIXMAP24;
    id->img_header.scrn_depth = id->img_header.depth;
    id->img_header.scrn_byte_per_line = id->img_header.byte_per_line;
    id->img_header.scrn_nb_color = id->img_header.nb_color;
    id->img_header.scrn_pal_r = id->img_header.pal_r;
    id->img_header.scrn_pal_v = id->img_header.pal_v;
    id->img_header.scrn_pal_b = id->img_header.pal_b;
    break;
   default:
    Xc_ERROR(("Image depth not implemented"));
    return FALSE;
   }
  }
  else				/* IMGLOAD_IMAGE_DEPTH */
  {
   id->img_header.type = PIXMAP24;
   id->img_header.scrn_depth = id->img_header.depth;
   id->img_header.scrn_byte_per_line = id->img_header.byte_per_line;
   id->img_header.scrn_nb_color = id->img_header.nb_color;
   id->img_header.scrn_pal_r = id->img_header.pal_r;
   id->img_header.scrn_pal_v = id->img_header.pal_v;
   id->img_header.scrn_pal_b = id->img_header.pal_b;
  }
  break;
 default:
  Xc_ERROR(("Image depth not implemented"));
  return FALSE;
 }
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** writeImage - write Image                                          ** 
** ----------------------------------------------------------------- */
static boolean writeImage(This, id)
c_ImageCache	*This;
ImageCache_t *id;
{
 FILE *file;
 char flip;
 
 Xc_TRACE(("writing cache image: %s", id->cache_filename));
 if (!(file = fopen(id->cache_filename, "wb")))
 {
  Xc_WARNING(("can not create file %s ", id->cache_filename));
  return FALSE;
 }
  
 flip = (char)(((id->img_header.flip_V & 1) << 1)+(id->img_header.flip_H & 1));
 
 if ((!ICwriteByteOrder(This, file, Xc_BYTE_ORDER)) ||
     (!ICwriteLoadModule(This, file, id->img_header.load_module)) ||
     (!ICwriteString(This, file, id->filename)) ||
     (!ICwriteInteger(This, file, (int)id->inode)) ||
     (!ICwriteInteger(This, file, (int)id->mtime)) ||
     (!ICwriteShort(This, file, (short)id->img_header.type)) ||
     (!ICwriteByte(This, file, flip)) || 
     (!ICwriteByte(This, file, (char)id->img_header.depth)) ||
     (!ICwriteByte(This, file, (char)id->img_header.scrn_depth)) ||
     (!ICwriteByte(This, file, (char)id->load_mode)) || 
     (!ICwriteInteger(This, file, id->img_header.width)) ||
     (!ICwriteInteger(This, file, id->img_header.height)) ||
     (!ICwriteInteger(This, file, id->img_header.byte_per_line)) ||
     (!ICwriteInteger(This, file, id->img_header.scrn_byte_per_line)) ||
     (!ICwriteColormap(This, file, &(id->img_header), 0)) ||
     (!ICwriteColormap(This, file, &(id->img_header), 1)) ||
     (!ICwriteData(This, file, id)))
 {
  fclose(file);
  unlink(id->cache_filename);
  Xc_HISTORY(("writing cache image failed"));
  return FALSE;
 }
 fclose(file);
  
 Xc_HISTORY(("writing cache image ok"));
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** readHeader - read Header                                          ** 
** ----------------------------------------------------------------- */
static boolean readHeader(This, id)
c_ImageCache	*This;
ImageCache_t *id;
{
 FILE *file;
 short type;
 char	depth, scrn_depth, load_mode, flip;
  
 if (!(file = fopen(id->cache_filename, "rb")))
 {
  Xc_WARNING(("can not open file %s ", id->cache_filename));
  return FALSE;
 }
 fseek(file, 0, SEEK_SET);
  
 id->filename = NULL;
 id->img_header.pal_r = id->img_header.pal_v = 
  id->img_header.pal_b = id->img_header.scrn_pal_r = 
   id->img_header.scrn_pal_v = id->img_header.scrn_pal_b = NULL;
  
 if ((!ICreadByteOrder(This, file, &This->byte_order)) ||
     (!ICreadLoadModule(This, file, &(id->img_header.load_module))) ||
     (!ICreadString(This, file, &(id->filename))) ||
     (!ICreadInteger(This, file, (int *)&(id->inode))) ||
     (!ICreadInteger(This, file, (int *)&(id->mtime))) || 
     (!ICreadShort(This, file, &type)) ||
     (!ICreadByte(This, file, &flip)) ||
     (!ICreadByte(This, file, &depth)) ||
     (!ICreadByte(This, file, &scrn_depth)) ||
     (!ICreadByte(This, file, &load_mode)) || 
     (!ICreadInteger(This, file, &(id->img_header.width))) ||
     (!ICreadInteger(This, file, &(id->img_header.height))) ||
     (!ICreadInteger(This, file, &(id->img_header.byte_per_line))) ||
     (!ICreadInteger(This, file, &(id->img_header.scrn_byte_per_line))) ||
     (!ICreadColormap(This, file, &(id->img_header), 0)) ||
     (!ICreadColormap(This, file, &(id->img_header), 1)))
 {
  if (id->filename) 
  { Xc_free(id->filename); id->filename = NULL; }
  if (id->img_header.pal_r)
  { Xc_free(id->img_header.pal_r); id->img_header.pal_r = NULL; }
  if (id->img_header.pal_v) 
  { Xc_free(id->img_header.pal_v); id->img_header.pal_v = NULL; }
  if (id->img_header.pal_b)
  { Xc_free(id->img_header.pal_b); id->img_header.pal_b = NULL; }
  if (id->img_header.scrn_pal_b)
  { Xc_free(id->img_header.scrn_pal_b); id->img_header.scrn_pal_b = NULL; }
  if (id->img_header.pal_b)
  { Xc_free(id->img_header.scrn_pal_b); id->img_header.scrn_pal_b = NULL; }
  if (id->img_header.pal_b)
  { Xc_free(id->img_header.scrn_pal_b); id->img_header.scrn_pal_b = NULL; }
  fclose(file);
  Xc_HISTORY(("read Header failed"));
  return FALSE;
 }
 id->img_header.type = (int)type;
 id->img_header.depth = (int)depth;
 id->img_header.scrn_depth = (int)scrn_depth;
 id->img_header.filename = id->filename;
 id->img_header.flip_V = ((flip & 2) != 0);
 id->img_header.flip_H = ((flip & 1) != 0);
 id->load_mode = (boolean)load_mode;
 fclose(file);
 Xc_HISTORY(("read Header ok"));
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** readData - read Image Data                                        ** 
** ----------------------------------------------------------------- */
static boolean readData(This, id)
c_ImageCache	*This;
ImageCache_t *id;
{
 FILE *file;
 short offset;
 short	nb_color;
 c_Dither *dither;
 unsigned char	*buffer;
 boolean need_dither = TRUE;
 int	dither_mode = 0;
 int	i;
 struct stat buf;
  
 if (lstat(id->filename, &buf) != -1 && 
     (buf.st_ino != id->inode || buf.st_mtime > id->mtime))
 {
  return FALSE;
 }
 if (!(file = fopen(id->cache_filename, "rb")))
 {
  Xc_WARNING(("can not open file %s ", id->cache_filename));
  return FALSE;
 }
 fseek(file, 0, SEEK_SET);
  
 if (!ICreadByteOrder(This, file, &This->byte_order))
 {
  fclose(file);
  return FALSE;
 }
  
 fseek(file, 6, SEEK_SET);
 if (!ICreadShort(This, file, &offset))
 {
  fclose(file);
  return FALSE;
 }
 fseek(file, offset + 30, SEEK_CUR);
 if (!ICreadShort(This, file, &nb_color))
 {				/* first colormap */
  fclose(file);
  return FALSE;
 }
 if (nb_color != -1)
 {
  fseek(file, nb_color * 6, SEEK_CUR);
 }
 if (!ICreadShort(This, file, &nb_color))
 {				/* second colormap */
  fclose(file);
  return FALSE;
 }
 if (nb_color != -1)
 {
  fseek(file, nb_color * 6, SEEK_CUR);
 }
  
 if (!ICreadData(This, file, id))
 {
  fclose(file);
  return FALSE;
 }
  
 switch(id->img_header.type)
 {
 case BITMAP:
  need_dither = FALSE;
  break;
 case PIXMAP8:
  switch(id->img_header.scrn_depth)
  {
  case 1:
   dither_mode = XcD_8_TO_2;
   break;
  case 8:
   if (id->img_header.scrn_pal_r != This->colormap->red &&
       (memcmp(id->img_header.scrn_pal_r, This->colormap->red, 
	       id->img_header.scrn_nb_color*2) ||
	memcmp(id->img_header.scrn_pal_v, This->colormap->green, 
	       id->img_header.scrn_nb_color*2) ||
	memcmp(id->img_header.scrn_pal_b, This->colormap->blue, 
	       id->img_header.scrn_nb_color*2)))
    dither_mode = XcD_8_TO_8;
   else
    need_dither = FALSE;
   break;
  case 16:
   dither_mode = XcD_8_TO_16;
   break;
  case 24:
   dither_mode = XcD_8_TO_24;
   break;
  }
  break;
 case PIXMAP16:
  switch(id->img_header.scrn_depth)
  {
   /*    case 1:
	 dither_mode = XcD_16_TO_2;
	 break;
	 case 8:
	 dither_mode = XcD_16_TO_8;	
	 break; */
  case 16:
   need_dither = FALSE;
   break;
   /*    case 24:
	 dither_mode = XcD_16_TO_24;	
	 break; */
  }
  break;
 case PIXMAP24:
  switch(id->img_header.scrn_depth)
  {
  case 1:
   dither_mode = XcD_24_TO_2;
   break;
  case 8:
   dither_mode = XcD_24_TO_8;	
   break;
  case 16:
   dither_mode = XcD_24_TO_16;	
   break;
  case 24:
   need_dither = FALSE;
   break;
  }
 }
  
 if (need_dither)
 {
  SWAP(int, id->img_header.depth, id->img_header.scrn_depth);
  SWAP(int, id->img_header.byte_per_line, id->img_header.scrn_byte_per_line);
  SWAP(int, id->img_header.nb_color, id->img_header.scrn_nb_color);
  SWAP(unsigned short *, id->img_header.pal_r, id->img_header.scrn_pal_r);
  SWAP(unsigned short *, id->img_header.pal_v, id->img_header.scrn_pal_v);
  SWAP(unsigned short *, id->img_header.pal_b, id->img_header.scrn_pal_b);
  dither = NEW(c_Dither)(dither_mode, This->colormap, 
			 &(id->img_header), id->data);
  buffer = (unsigned char *)id->data;
  for(i = 0; i < id->img_header.height; i++)
  {
   dither->callback(&buffer, dither);
   buffer += id->img_header.byte_per_line;
  }
  DELETE(c_Dither)(dither);
  SWAP(int, id->img_header.depth, id->img_header.scrn_depth);
  SWAP(int, id->img_header.byte_per_line, id->img_header.scrn_byte_per_line);
  SWAP(int, id->img_header.nb_color, id->img_header.scrn_nb_color);
  SWAP(unsigned short *, id->img_header.pal_r, id->img_header.scrn_pal_r);
  SWAP(unsigned short *, id->img_header.pal_v, id->img_header.scrn_pal_v);
  SWAP(unsigned short *, id->img_header.pal_b, id->img_header.scrn_pal_b);
  id->img_header.scrn_nb_color = This->colormap->nb_color;
 }
  
 if (id->img_header.scrn_nb_color > 0)
 {
  if (id->img_header.scrn_pal_alloc)
  {
   Xc_free(id->img_header.scrn_pal_r);
   Xc_free(id->img_header.scrn_pal_v);
   Xc_free(id->img_header.scrn_pal_b);
   id->img_header.scrn_pal_alloc = FALSE;
  }
  id->img_header.scrn_pal_r = This->colormap->red;
  id->img_header.scrn_pal_v = This->colormap->green;
  id->img_header.scrn_pal_b = This->colormap->blue;
 }
 fclose(file);
 return TRUE;
}

static boolean ICwriteByteOrder(This, file, byte_order)
c_ImageCache	*This;
FILE		*file;
int		byte_order;
{
 char	*order = NULL;
  
 switch(byte_order)
 {
 case Xc_BIG_ENDIAN:
  order = "Xcl!";
  break;
 case Xc_LITTLE_ENDIAN:
  order = "l!Xc";
  break;
 case Xc_PDP_ENDIAN:
  order = "cX!l";
  break;
 }
 if ( fwrite(order, 4, 1, file) != 1)
  return FALSE;
 return TRUE;
}

static boolean ICreadByteOrder(This, file, byte_order)
c_ImageCache	*This;
FILE		*file;
int		*byte_order;
{
 char	order[4];
  
 if ( fread(order, 4, 1, file) != 1)
  return FALSE;
  
 if (Xstrncmp("Xcl!", order, 4))
  *byte_order = Xc_BIG_ENDIAN;
 else
 {
  if (Xstrncmp("l!Xc", order, 4))
   *byte_order = Xc_LITTLE_ENDIAN;
  else
  {
   if (Xstrncmp("cX!l", order, 4))
    *byte_order = Xc_PDP_ENDIAN;
   else
    return FALSE;
  } 
 }  
 return TRUE;
}

static boolean ICwriteLoadModule(This, file, load_module)
c_ImageCache	*This;
FILE		*file;
image_load_t	*load_module;
{
 int i;
  
 i = 0;
 while(image_load_list[i] && image_load_list[i] != load_module)
  i++;
  
 if (image_load_list[i] == NULL)
  return FALSE;
 return ICwriteShort(This, file, i);
}

static boolean ICreadLoadModule(This, file, load_module)
c_ImageCache	*This;
FILE		*file;
image_load_t	**load_module;
{
 short i, j;
  
 if (!ICreadShort(This, file, &i))
  return FALSE;
  
 j = 0;
 while(image_load_list[j])
  j++;
  
 if (i > j) return FALSE;
  
 *load_module = image_load_list[i];
 return TRUE;
}


static boolean ICwriteString(This, file, string)
c_ImageCache	*This;
FILE		*file;
char		*string;
{
 int length;
  
 length = strlen(string) + 1;
  
 if (!ICwriteShort(This, file, length))
  return FALSE;
  
 if ( fwrite(string, length, 1, file) != 1)
  return FALSE;
 return TRUE;
}

static boolean ICreadString(This, file, string)
c_ImageCache	*This;
FILE		*file;
char		**string;
{
 char	*ptr;
 short	length;
  
 if (!ICreadShort(This, file, &length))
  return FALSE;
  
 ptr = (char *)Xc_malloc("read string", length);
 if (ptr == NULL) return FALSE;
  
 if ( fread(ptr, length, 1, file) != 1)
 {
  Xc_free(ptr); ptr = NULL;
  return FALSE;
 }
 *string = ptr;
 return TRUE;
}

static boolean ICwriteShort(This, file, value)
c_ImageCache	*This;
FILE		*file;
short		value;
{
 if ( fwrite((char *)&value, 2, 1, file) != 1)
  return FALSE;
 return TRUE;
}

static boolean ICreadShort(This, file, value)
c_ImageCache	*This;
FILE		*file;
short		*value;
{
 short	nbr;
  
 if ( fread((char *)&nbr, 2, 1, file) != 1)
  return FALSE;
 if (This->byte_order != Xc_BYTE_ORDER)
  nbr = ((nbr & 0xff) << 8) + ((nbr & 0xff00) >> 8);
  
 *value = nbr;
 return TRUE;
}

static boolean ICwriteByte(This, file, mbyte)
c_ImageCache	*This;
FILE		*file;
char		mbyte;
{
 if ( fwrite( &mbyte, 1, 1, file) != 1)
  return FALSE;
 return TRUE;
}

static boolean ICreadByte(This, file, mbyte)
c_ImageCache	*This;
FILE		*file;
char		*mbyte;
{
 char	c;
  
 c = fgetc(file) & 0xff;
 if (feof(file))
  return FALSE;
 *mbyte = c;
 return TRUE;
}

static boolean ICwriteInteger(This, file, value)
c_ImageCache	*This;
FILE		*file;
int		value;
{
 if ( fwrite((char *)&value, 4, 1, file) != 1)
  return FALSE;
 return TRUE;
}

static boolean ICreadInteger(This, file, value)
c_ImageCache	*This;
FILE		*file;
int		*value;
{
 unsigned int	nbr;
  
 if ( fread((char *)&nbr, 4, 1, file) != 1)
  return FALSE;
 if (This->byte_order != Xc_BYTE_ORDER)
 {
  nbr = ((nbr & 0xff) << 24) + ((nbr & 0xff00) << 8) + 
   ((nbr & 0xff0000) >> 8) + ((nbr & 0xff000000) >> 24);
 }
 *value = (int)nbr;
 return TRUE;
}

static boolean ICwriteColormap(This, file, image, witch)
c_ImageCache	*This;
FILE		*file;
c_Image		*image;
int		witch;
{  
 if (witch == 0)
 {
  if (!ICwriteShort(This, file, image->nb_color))
   return FALSE;
    
  if (image->nb_color == -1)
   return TRUE;
    
  if (fwrite((char *)image->pal_r, image->nb_color*2, 1, file) != 1)
   return FALSE;
    
  if (fwrite((char *)image->pal_v, image->nb_color*2, 1, file) != 1)
   return FALSE;
    
  if (fwrite((char *)image->pal_b, image->nb_color*2, 1, file) != 1)
   return FALSE;
 }
 else
 {
  if (!ICwriteShort(This, file, image->scrn_nb_color))
   return FALSE;
    
  if (image->scrn_nb_color == -1)
   return TRUE;
    
  if (fwrite((char *)image->scrn_pal_r, image->scrn_nb_color*2, 1, file) !=1)
   return FALSE;
    
  if (fwrite((char *)image->scrn_pal_v, image->scrn_nb_color*2, 1, file) !=1)
   return FALSE;
    
  if (fwrite((char *)image->scrn_pal_b, image->scrn_nb_color*2, 1, file) !=1)
   return FALSE;
 }    
 return TRUE;
}

static boolean ICreadColormap(This, file, image, witch)
c_ImageCache	*This;
FILE		*file;
c_Image		*image;
int		witch;
{
 short	nb_color, i, value;
  
 if (witch == 0)
 {
  if (!ICreadShort(This, file, &nb_color))
   return FALSE;
    
  image->nb_color = (int)nb_color;
  if (nb_color == -1)
   return TRUE;

  if (nb_color != 2 && nb_color != 256)
   return FALSE;

  image->pal_r = (unsigned short *)Xc_malloc("pal_r", 2 * nb_color);
  image->pal_v = (unsigned short *)Xc_malloc("pal_r", 2 * nb_color);
  image->pal_b = (unsigned short *)Xc_malloc("pal_r", 2 * nb_color);
  if (!image->pal_r || !image->pal_v || !image->pal_b)
  {
   if (image->pal_r) { Xc_free(image->pal_r); image->pal_r = NULL; } 
   if (image->pal_v) { Xc_free(image->pal_v); image->pal_v = NULL; } 
   if (image->pal_b) { Xc_free(image->pal_b); image->pal_b = NULL; } 
   return FALSE;
  }
    
  if (fread((char *)image->pal_r, nb_color*2, 1, file) != 1)
  {
   Xc_free(image->pal_r); image->pal_r = NULL; 
   Xc_free(image->pal_v); image->pal_v = NULL; 
   Xc_free(image->pal_b); image->pal_b = NULL; 
   return FALSE;
  }
    
  if (fread((char *)image->pal_v, nb_color*2, 1, file) != 1)
  {
   Xc_free(image->pal_r); image->pal_r = NULL; 
   Xc_free(image->pal_v); image->pal_v = NULL; 
   Xc_free(image->pal_b); image->pal_b = NULL; 
   return FALSE;
  }
    
  if (fread((char *)image->pal_b, nb_color*2, 1, file) != 1)
  {
   Xc_free(image->pal_r); image->pal_r = NULL; 
   Xc_free(image->pal_v); image->pal_v = NULL; 
   Xc_free(image->pal_b); image->pal_b = NULL; 
   return FALSE;
  }
    
  if (This->byte_order != Xc_BYTE_ORDER)
  {
   for(i = 0; i < nb_color; i++)
   {
    value = image->pal_r[i];
    image->pal_r[i] = ((value & 0xff) << 8) + ((value & 0xff00) >> 8);
    value = image->pal_v[i];
    image->pal_v[i] = ((value & 0xff) << 8) + ((value & 0xff00) >> 8);
    value = image->pal_b[i];
    image->pal_b[i] = ((value & 0xff) << 8) + ((value & 0xff00) >> 8);
   }
  }
 }
 else
 {        
  if (!ICreadShort(This, file, &nb_color))
   return FALSE;
    
  image->scrn_nb_color = (int)nb_color;
  if (nb_color == -1)
   return TRUE;
    
  image->scrn_pal_alloc = TRUE;
  image->scrn_pal_r = (unsigned short *)Xc_malloc("pal_r", 2 * nb_color);
  image->scrn_pal_v = (unsigned short *)Xc_malloc("pal_r", 2 * nb_color);
  image->scrn_pal_b = (unsigned short *)Xc_malloc("pal_r", 2 * nb_color);
  if (!image->scrn_pal_r || !image->scrn_pal_v || !image->scrn_pal_b)
  {
   if (image->scrn_pal_r) 
   { Xc_free(image->scrn_pal_r); image->scrn_pal_r = NULL; } 
   if (image->scrn_pal_v) 
   { Xc_free(image->scrn_pal_v); image->scrn_pal_v = NULL; } 
   if (image->scrn_pal_b) 
   { Xc_free(image->scrn_pal_b); image->scrn_pal_b = NULL; } 
   return FALSE;
  }
    
  if (fread((char *)image->scrn_pal_r, nb_color*2, 1, file) != 1)
  {
   Xc_free(image->scrn_pal_r); image->scrn_pal_r = NULL; 
   Xc_free(image->scrn_pal_v); image->scrn_pal_v = NULL; 
   Xc_free(image->scrn_pal_b); image->scrn_pal_b = NULL; 
   return FALSE;
  }
    
  if (fread((char *)image->scrn_pal_v, nb_color*2, 1, file) != 1)
  {
   Xc_free(image->scrn_pal_r); image->scrn_pal_r = NULL; 
   Xc_free(image->scrn_pal_v); image->scrn_pal_v = NULL; 
   Xc_free(image->scrn_pal_b); image->scrn_pal_b = NULL; 
   return FALSE;
  }
    
  if (fread((char *)image->scrn_pal_b, nb_color*2, 1, file) != 1)
  {
   Xc_free(image->scrn_pal_r); image->scrn_pal_r = NULL; 
   Xc_free(image->scrn_pal_v); image->scrn_pal_v = NULL; 
   Xc_free(image->scrn_pal_b); image->scrn_pal_b = NULL; 
   return FALSE;
  }
    
  if (This->byte_order != Xc_BYTE_ORDER)
  {
   for(i = 0; i < nb_color; i++)
   {
    value = image->scrn_pal_r[i];
    image->scrn_pal_r[i] = ((value & 0xff) << 8) + ((value & 0xff00) >> 8);
    value = image->scrn_pal_v[i];
    image->scrn_pal_v[i] = ((value & 0xff) << 8) + ((value & 0xff00) >> 8);
    value = image->scrn_pal_b[i];
    image->scrn_pal_b[i] = ((value & 0xff) << 8) + ((value & 0xff00) >> 8);
   }
  }
 }
 return TRUE;
}

static boolean ICwriteData(This, file, id)
c_ImageCache	*This;
FILE		*file;
ImageCache_t	*id;
{
 int	length, i;
 int	nb_block, remain_last_block;
 char	*data;
  
 length = id->img_header.height * id->img_header.scrn_byte_per_line;
 if (!ICwriteInteger(This, file, length))
  return FALSE;
  
 data = id->data;
 nb_block = length / XcIC_BLOCKSIZE;
 remain_last_block = length % XcIC_BLOCKSIZE;
 for(i = 0; i < nb_block; i++)
 {
  if (fwrite(data, XcIC_BLOCKSIZE, 1, file) != 1)
   return FALSE;
  data += XcIC_BLOCKSIZE;
 }
 if (remain_last_block)
 {
  if (fwrite(data, remain_last_block, 1, file) != 1)
   return FALSE;
 }
  
 return TRUE;
}

static boolean ICreadData(This, file, id)
c_ImageCache	*This;
FILE		*file;
ImageCache_t	*id;
{
 int	length, length2, i;
 int	nb_block, remain_last_block;
 char	*data;
  
 length = id->img_header.height * id->img_header.scrn_byte_per_line;
 if (!ICreadInteger(This, file, &length2))
  return FALSE;
  
 if (length != length2)
  return FALSE;

  
 id->data = (char *)Xc_malloc("data", length);
 if (id->data == NULL)
 {
  Xc_WARNING(("memory exhausted"));
  return FALSE;
 }
  
 data = id->data;
 nb_block = length / XcIC_BLOCKSIZE;
 remain_last_block = length % XcIC_BLOCKSIZE;
 for(i = 0; i < nb_block; i++)
 {
  if (fread(data, XcIC_BLOCKSIZE, 1, file) != 1)
  {
   Xc_free(id->data); id->data = NULL;
   return FALSE;
  }
  data += XcIC_BLOCKSIZE;
 }
 if (remain_last_block)
 {
  if (fread(data, remain_last_block, 1, file) != 1)
  {
   Xc_free(id->data); id->data = NULL;
   return FALSE;
  }
 }
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** sort_id - sort ImageCache id: the first key is count,             ** 
** if count == 0 then the second key is data_size		     **
**               else the second key is data_time		     **
** ----------------------------------------------------------------- */
static int sort_id(This, exclude_id, data_kept, sorted_id)
c_ImageCache	*This;
ImageCache_t	*exclude_id;
int		data_kept;
ImageCache_t	***sorted_id;
{
 int		nb_id, nb_key, old_key, key, remain_id, i, j;
 ImageCache_t	*id, **tab_id;
  
 nb_id = 0; 
 id = This->ImgCacheList;
 while(id)
 {
  if (id->data_kept == data_kept && id != exclude_id)
  {
   nb_id ++;
  }
  id = id->Next;
 }
  
 if (nb_id == 0)
 {
  *sorted_id = NULL;
  return 0;
 }

 tab_id = (ImageCache_t **)Xc_malloc("tab id", 
				     sizeof(ImageCache_t *) * nb_id);
 i = 0;
 id = This->ImgCacheList;
 while(id && i < nb_id)
 {
  if (id->data_kept == data_kept && id != exclude_id)
  {
   tab_id[i++] = id;
  }
  id = id->Next;
 }
  
 for(i = 0; i < nb_id; i++)
 {
  for(j = i+1; j < nb_id; j++)
  {
   if (tab_id[j]->count < tab_id[i]->count)
   {
    SWAP(ImageCache_t *, tab_id[i], tab_id[j]);
   }
  }
 }

 remain_id = nb_id;
 key = 0;
  
 nb_key = 0; 
 while(nb_key < nb_id && tab_id[nb_key]->count == key)
  nb_key ++;

 for(i = 0; i < nb_key; i++)
 {
  for(j = i+1; j < nb_key; j++)
  {
   if (tab_id[j]->data_size < tab_id[i]->data_size)
   {
    SWAP(ImageCache_t *, tab_id[i], tab_id[j]);
   }
  }
 }
 remain_id -= nb_key;
 key++;
  
 while(remain_id > 0)
 {
  old_key = nb_key;
  while(nb_key < nb_id && tab_id[nb_key]->count == key)
   nb_key ++;
    
  for(i = old_key; i < nb_key; i++)
  {
   for(j = i+1; j < nb_key; j++)
   {
    if (tab_id[j]->data_time < tab_id[i]->data_time)
    {
     SWAP(ImageCache_t *, tab_id[i], tab_id[j]);
    }
   }
  }
  remain_id -= nb_key - old_key;
  key++;
 }
  
 *sorted_id = tab_id;
 return nb_id;
}





