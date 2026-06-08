/*
** Image.c for XMayday in Images/
** Methods of the Image class
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
** Started on  Wed Oct 26 16:10:00 1994 Stéphane Boisson
** Last update Sun Apr 25 15:28:31 1999 Emmanuel Paris
*/

#include "Image.h"
#include "ImagePrint.h"
#include "ImageCache.h"
#include "ImageManager.h"

/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();
static char *CalculImage();
static Pixmap CalculImagePixmap();
static void ps_print();

extern c_Colormap	*GlobColormap;
extern c_ImageManager	*GlobImgMng;
extern c_ImageCache	*GlobImgCache;

extern image_load_t Load_Targa, Load_Jpeg, Load_Gif, Load_Bmp, Load_Png;

image_load_t *image_load_list[] = 
{ 
 &Load_Jpeg,
 &Load_Targa,
 &Load_Gif,
 &Load_Bmp,
 &Load_Png,
 NULL
};

/*--- Methods ---*/
sf_Image fc_Image =
{
 /*___Standard methods___*/
 constructor,
 destructor,
 copy,
 CalculImage,
 CalculImagePixmap,
 ps_print
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(filename, load_module)
char *filename;
image_load_t *load_module;
{
 c_Image *Image;

 Xc_HISTORY(("constructor(%s)", filename));
  
  
 if((Image = Xc_malloc("Image", sizeof(c_Image))) == NULL) return NULL;
 Image->f = &fc_Image;
  
 Image->handle		= NULL;
 Image->flip_H		= FALSE;
 Image->flip_V		= FALSE;
  
 Image->cache_id = F(GlobImgCache).load(GlobImgCache, filename, Image,
					load_module, IMGLOAD_SCREEN_DEPTH);
 if (Image->cache_id == NULL)
 {
  Xc_free(Image);
  return NULL;
 }
  
 return Image;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(This)
c_Image *This;
{
 Xc_HISTORY(("destructor(%s)", This->filename));
  
 F(GlobImgCache).delete_id(GlobImgCache, This->cache_id);
  
 if(This->handle)
 {
  F(GlobImgMng).free(GlobImgMng, This->handle);
  This->handle = NULL;
 }
  
 Xc_free(This);
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(This)
c_Image		*This;
{
 c_Image	*Image;
  
 Xc_HISTORY(("copy"));

 if (!(Image=(c_Image *)Xc_malloc("copy img",sizeof(c_Image))))
 {
  return NULL;
 }
 memcpy(Image,This,sizeof(c_Image));

 Image->cache_id	= This->cache_id;
  
 F(GlobImgCache).add_reference_to_id(GlobImgCache, This->cache_id);
 F(GlobImgCache).get_header(GlobImgCache, This->cache_id, Image);
  
 return Image;
}

/* -------------------------------------------------------------------- **
**			Calcul Image ...				**
** -------------------------------------------------------------------- */
static char *CalculImage(This, width, height)
c_Image	*This;
int	width;
int	height;
{
 char		*data;
 char		*data_calc;
 double	x_zoom;
 double	y_zoom;
 double	x;
 double	y;
 boolean	inverse;
 int		oct;
 int		i, j;
 int		svg;
 unsigned long	filler, fil1, fil2, fil3;
 int		indexy;
 int		index,offset;
 int		bindex, nx;
 static char   bit_test[8]={128,64,32,16,8,4,2,1};
 static char	bit_setLM[8]={1,2,4,8,16,32,64,128};
 static char	bit_setML[8]={128,64,32,16,8,4,2,1};
 char		*bit_set;
 int		byte_per_line;
  
 Xc_HISTORY(("CalculImage"));
  
 switch(This->type)
 {
 case BITMAP:
  byte_per_line = (width + 7) >> 3;
  break;
 case PIXMAP4:
  byte_per_line = ((width + 7) >> 3 ) << 2;
  break;
 case PIXMAP8:
  byte_per_line = width;
  break;
 case PIXMAP16:
  byte_per_line = width << 1;
  break;
 case PIXMAP24:
  if (GlobImgMng->real_depth == 24)
   byte_per_line = width * 3;
  else
   byte_per_line = width << 2;
  break;
 case PIXMAP32:
  byte_per_line = width << 2;
  break;
 default:
  Xc_WARNING(("Unknow Pixmap type"));
  return (Pixmap)0;
  break;
 }    
    
 data = F(GlobImgCache).get_data(GlobImgCache, This->cache_id);
 if (data == NULL)
  return NULL;
  
 x_zoom = (real)(This->width ) / (real)width;
 y_zoom = (real)(This->height) / (real)height;

 if ( (height * byte_per_line == 0) || 
     ((data_calc = (char *) Xc_malloc
       ("datac", sizeof(char) * height * byte_per_line)) == NULL))
  return NULL;
  
 switch(This->type)
 {
  /*--- Bitmap type ---*/
 case BITMAP :
  Xc_TRACE(("bitmap type"));
    
  if (GlobImgMng->bit_order_LSBFirst)
   bit_set = bit_setLM;
  else
   bit_set = bit_setML;
  if (GlobColormap->Cwhite == 1)
   inverse = TRUE;
  else
   inverse = FALSE;

  filler = (char)GlobColormap->Cwhite;

  index = oct = svg = offset = 0;
  y = 0.0;
  for(i = 0; i < height; i++)
  {
   x = 0.0;
   indexy = (This->flip_V?This->height-(int)y:(int)y) 
    * This->scrn_byte_per_line;
   for(j = 0; j < width; j++)
   {
    nx = (This->flip_H?This->width-(int)x:(int)x);
    bindex =(nx & 7);
    index = indexy + (nx >> 3);
    if (data[index] & bit_test[bindex])
     svg |= bit_set[oct];
    oct++;
    if (oct == 8)
    {
     oct = 0;
     if (inverse)
      data_calc[offset] = ~svg;
     else
      data_calc[offset] = svg;
     svg = 0;
     offset++;
    }
    x += x_zoom;
   }
   if (oct != 0)
   {
    if (inverse)
     data_calc[offset] = ~svg;
    else
     data_calc[offset] = svg;
    oct = 0;
    offset++;
   }
   y += y_zoom;
  }
  break;
    
  /*--- Pixmap 8 planes ---*/
 case PIXMAP8:
  Xc_TRACE(("Pixmap 8 planes type"));
  filler = GlobColormap->Cwhite;
    
  y = 0.0; 
  offset = 0;
  for( i = 0; i < height; i++)
  {
   x = 0.0;
   indexy = ((This->flip_V?This->height-1-(int)y:(int)y)
	     * This->scrn_byte_per_line);
   for(j = 0; j < width; j++)
   {
    data_calc[offset++] = ((unsigned char *)data)
     [indexy + (This->flip_H?This->width-1-(int)x:(int)x)];
    x += x_zoom;
   }
   y += y_zoom;
  }
  break;
    
  /*--- True color display ---*/
 case PIXMAP16:
  Xc_TRACE(("Pixmap 16 planes type"));
  filler = GlobColormap->Cwhite;
  fil1 = (filler & 0xff00) >> 8;
  fil2 = filler & 0xff;
    
  y = 0.0; 
  offset = 0;
    
  if (GlobImgMng->byte_order_LSBFirst)
  {
   for( i = 0; i < height; i++)
   {
    x = 0.0;
    indexy = ((This->flip_V ? 
	       This->height - 1 - (int)y : (int)y)
	      * This->scrn_byte_per_line);
    for(j = 0; j < width; j++)
    {
     nx = indexy + (This->flip_H ? 
		    This->width - 1 - (int)x : (int)x)*2;
     data_calc[offset++] = ((unsigned char *)data)[nx+1];
     data_calc[offset++] = ((unsigned char *)data)[nx];
     x += x_zoom;
    }
    y += y_zoom;
   }
  }
  else
  {
   for( i = 0; i < height; i++)
   {
    x = 0.0;
    indexy = ((This->flip_V ? 
	       This->height - 1 - (int)y : (int)y)
	      * This->scrn_byte_per_line);
    for(j = 0; j < width; j++)
    {
     nx = indexy + (This->flip_H ? 
		    This->width - 1 - (int)x : (int)x)*2;
     data_calc[offset++] = ((unsigned char *)data)[nx];
     data_calc[offset++] = ((unsigned char *)data)[nx+1];
     x += x_zoom;
    }
    y += y_zoom;
   }
  }
  break;
 case PIXMAP24:
  Xc_TRACE(("Pixmap 24 planes type"));
  filler = GlobColormap->Cwhite;
  fil1 = (filler & 0xff0000) >> 16;
  fil2 = (filler & 0xff00) >> 8;
  fil3 = filler & 0xff;
    
  if (GlobImgMng->real_depth == 24)
  {
   y = 0.0; 
   offset = 0;
   if (GlobImgMng->byte_order_LSBFirst)
   {
    for( i = 0; i < height; i++)
    {
     x = 0.0;
     indexy = ((This->flip_V?This->height-1-(int)y:(int)y)
	       * This->scrn_byte_per_line);
     for(j = 0; j < width; j++)
     {
      nx= indexy+(This->flip_H ? This->width - 1 - (int)x : (int)x) * 3;
      data_calc[offset++] = ((unsigned char *)data)[nx+2];
      data_calc[offset++] = ((unsigned char *)data)[nx+1];
      data_calc[offset++] = ((unsigned char *)data)[nx];
      x += x_zoom;
     }
     y += y_zoom;
    }
   } 
   else
   {
    for( i = 0; i < height; i++)
    {
     x = 0.0;
     indexy = ((This->flip_V?This->height-1-(int)y:(int)y)
	       * This->scrn_byte_per_line);
     for(j = 0; j < width; j++)
     {
      nx= indexy+(This->flip_H ? This->width - 1 - (int)x : (int)x) * 3;
      data_calc[offset++] = ((unsigned char *)data)[nx];
      data_calc[offset++] = ((unsigned char *)data)[nx+1];
      data_calc[offset++] = ((unsigned char *)data)[nx+2];
      x += x_zoom;
     }
     y += y_zoom;
    }
   }
  }
  else				/* 32 bits display */
  {
   y = 0.0; 
   offset = 0;
   if (GlobImgMng->byte_order_LSBFirst)
   {
    for( i = 0; i < height; i++)
    {
     x = 0.0;
     indexy = ((This->flip_V?This->height-1-(int)y:(int)y)
	       * This->scrn_byte_per_line);
     for(j = 0; j < width; j++)
     {
      nx=indexy+(This->flip_H ? This->width - 1 - (int)x : (int)x) * 3;
      data_calc[offset++] = ((unsigned char *)data)[nx+2];
      data_calc[offset++] = ((unsigned char *)data)[nx+1];
      data_calc[offset++] = ((unsigned char *)data)[nx];
      offset++;
      x += x_zoom;
     }
     y += y_zoom;
    }
   } 
   else
   {
    for( i = 0; i < height; i++)
    {
     x = 0.0;
     indexy = ((This->flip_V?This->height-1-(int)y:(int)y)
	       * This->scrn_byte_per_line);
     for(j = 0; j < width; j++)
     {
      nx= indexy+(This->flip_H ? This->width - 1 - (int)x : (int)x) * 3;
      offset++;
      data_calc[offset++] = ((unsigned char *)data)[nx];
      data_calc[offset++] = ((unsigned char *)data)[nx+1];
      data_calc[offset++] = ((unsigned char *)data)[nx+2];
      x += x_zoom;
     }
     y += y_zoom;
    }
   }
  }
  break;
 case PIXMAP32:
  Xc_WARNING(("Compute 32 bits image : non implemented yet"));
  return (Pixmap)0;
  break;
 }
  
 Xc_TRACE(("Compute Image ok"));
 return data_calc;
}


static Pixmap CalculImagePixmap(This, width, height)
c_Image	*This;
int	width;
int	height;
{
 char	*data_calc;
    
 data_calc = F(This).CalculImage(This, width, height);
 if (data_calc == NULL)
  return (Pixmap)0;
  
 switch(This->type)
 {
  /*--- Bitmap type ---*/
 case BITMAP :
  Xc_TRACE(("bitmap type"));
    
  This->handle = F(GlobImgMng).allocate
   (GlobImgMng, data_calc, width, height, 1, TRUE);
  break;
    
  /*--- Pixmap 8 planes ---*/
 case PIXMAP8:
  Xc_TRACE(("Pixmap 8 planes type"));
  This->handle = F(GlobImgMng).allocate
   (GlobImgMng, data_calc, width, height, 8, TRUE);
  break;
    
  /*--- True color display ---*/
 case PIXMAP16:
  Xc_TRACE(("Pixmap 16 planes type"));
  This->handle = F(GlobImgMng).allocate
   (GlobImgMng, data_calc, width, height, GlobColormap->depth, TRUE);   
  break;
  
 case PIXMAP24:
  Xc_TRACE(("Pixmap 24 planes type"));
  This->handle = F(GlobImgMng).allocate
   (GlobImgMng, data_calc, width, height, 24, TRUE);
  break;
  
 case PIXMAP32:
  Xc_WARNING(("Compute 32 bits image : non implemented yet"));
  return (Pixmap)0;
  break;
 }
  
 Xc_TRACE(("Compute Image ok"));
 return F(GlobImgMng).getPixmap(GlobImgMng, This->handle);
}

static void ps_print(this, ps_printf, color)
c_Image *this;
int (*ps_printf)();
int color;
{
 ps_image_t *handle;
 error rc;
  
 Xc_TRACE(("ps print image: %s", this->filename));
  
 handle = ps_beginImage(ps_printf, color,
			this->width, this->height, this->depth,
			this->pal_r, this->pal_v, this->pal_b); 
 if (handle == NULL)
  return;
  
 rc = this->load_module->load(this, handle->line_buffer, 
			      (void *)handle->cb_func, handle);
 ps_endImage(ps_printf, handle);
}





