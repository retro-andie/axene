/*
** Image.c for XInstall in Images/
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
** Last update Tue Jan  6 18:53:16 1998 Emmanuel Paris
*/

#include "Image.h"
#include "Dither.h"
#include "Resource.h"
#include "AlertBox.h"

#define IMGLOAD_SCREEN_DEPTH 1

/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();
static void CalculImage();
static void draw();
static char *get_data();
static boolean read_image_header();
static boolean simple_load_callback();
static void drawputPart();
static void savePart();
static void restorePart();

extern c_Colormap	*GlobColormap;
extern c_ImageManager	*GlobImgMng;
extern c_Resource	*GlobResources;

extern image_load_t Load_Gif;

image_load_t *image_load_list[] = 
{ 
 &Load_Gif,
 NULL
};

/*--- Methods ---*/
sf_Image fc_Image =
{
 /*___Standard methods___*/
 constructor, destructor, copy,
 CalculImage,
 draw,
 drawputPart,
 savePart,
 restorePart
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(filename, load_module)
char		*filename;
image_load_t	*load_module;
{
 c_Image *Image;

 Xc_HISTORY(("constructor(%s)", filename));
  
 if (!load_module->test(filename))
  return NULL;
 if((Image = Xc_malloc("Image", sizeof(c_Image))) == NULL)
  return NULL;
 Image->f = &fc_Image;

 if (!Xstrncmp(filename, "*id", 3))
 {
  if (!(Image->filename = Xc_strdup("dup_s", filename)))
  {
   Xc_free(Image);
   return NULL;
  }
 }
 else
 {
  Image->filename = filename;
 }

 Image->load_module = load_module;
 Image->colormap = GlobColormap;
 Image->flip_H		= FALSE;
 Image->flip_V		= FALSE;

 Image->handle		= NULL;

 if (!read_image_header(Image, IMGLOAD_SCREEN_DEPTH))
 {
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
  
 if (!Xstrncmp(This->filename, "*id", 3))
  Xc_free(This->filename);

 if (This->nb_color > 0)
 {
  Xc_free(This->pal_r);
  Xc_free(This->pal_v);
  Xc_free(This->pal_b);
 }

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
static void *copy(This, base)
c_Image		*This;
{
 Xc_HISTORY(("copy"));
 return NULL;
}

/* -------------------------------------------------------------------- **
**			Calcul Image ...				**
** -------------------------------------------------------------------- */
static void CalculImage(This, width, height)
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

 if (width <= 0) width = This->width;
 if (height <= 0) height = This->height;

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
  return;
  break;
 }    
    
 data = get_data(This);
 if (!data)
  return;
  
 x_zoom = (real)(This->width ) / (real)width;
 y_zoom = (real)(This->height) / (real)height;

 This->Dwidth = width;
 This->Dheight = height;

 if ( (height * byte_per_line == 0) || 
     ((data_calc = (char *) Xc_malloc
       ("datac", sizeof(char) * height * byte_per_line)) == NULL))
  return;
  
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

  This->handle = F(GlobImgMng).allocate
   (GlobImgMng, data_calc, width, height, 1, TRUE);

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

  This->handle = F(GlobImgMng).allocate
   (GlobImgMng, data_calc, width, height, 8, TRUE);
 
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

  This->handle = F(GlobImgMng).allocate
   (GlobImgMng, data_calc, width, height, 16, TRUE);   
 
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

  This->handle = F(GlobImgMng).allocate
   (GlobImgMng, data_calc, width, height, 24, TRUE);

  break;
 case PIXMAP32:
  Xc_WARNING(("Compute 32 bits image : non implemented yet"));
  break;
 }

 Xc_free(data);
  
 Xc_TRACE(("Compute Image ok"));
}

/* -------------------------------------------------------------------- **
**				Draw Image 				**
** -------------------------------------------------------------------- */
static void draw(This, window, gc, w, h)
c_Image		*This;
Window		window;
GC		gc;
int		w;
int		h;
{
 int	x;
 int	y;

 x = (w - This->Dwidth)/2;
 y = (h - This->Dheight)/2;
 Xc_HISTORY(("display redraw"));
 if(This->handle)
 {
  F(GlobImgMng).draw(GlobImgMng, This->handle,
		     window, gc,
		     0, 0, This->Dwidth, This->Dheight, x, y);
 }
 Xc_TRACE(("done"));
}

/* -------------------------------------------------------------------- **
**				Draw Image 				**
** -------------------------------------------------------------------- */
static void drawputPart(This, Src, window, gc, x, y, w, h, x_dest, y_dest,
			dec_x, dec_y)
c_Image		*This; /* dest */
c_Image		*Src;
GC		gc;
int		w;
int		h;
int		x;
int		y;
int		x_dest;
int		y_dest;
int		dec_x;
int		dec_y;
{
 Xc_HISTORY(("display redraw"));
 F(GlobImgMng).putPart(GlobImgMng, Src->handle, This->handle, gc, x, y,
		       w, h, x_dest, y_dest);

 if(This->handle)
 {
  F(GlobImgMng).draw(GlobImgMng, This->handle,
		     window, gc,
		     x, y, w, h, x_dest+dec_x, y_dest+dec_y);
 }
 Xc_TRACE(("done"));
}

/* -------------------------------------------------------------------- **
**				Draw Image 				**
** -------------------------------------------------------------------- */
static void savePart(This, window, gc, x, y, w, h)
c_Image		*This;
GC		gc;
int		x;
int		y;
int		w;
int		h;
{
 Xc_HISTORY(("display redraw"));
 F(GlobImgMng).savePart(GlobImgMng, This->handle, gc, x, y, w, h);
 Xc_TRACE(("done"));
}

/* -------------------------------------------------------------------- **
**				Draw Image 				**
** -------------------------------------------------------------------- */
static void restorePart(This, window, gc, dec_x, dec_y)
c_Image		*This;
GC		gc;
int		dec_x;
int		dec_y;
{
 int	x;
 int	y;
 int	w;
 int	h;

 Xc_HISTORY(("display redraw"));
 x = This->handle->save.x;
 y = This->handle->save.y;
 w = This->handle->save.w;
 h = This->handle->save.h;
 F(GlobImgMng).restorePart(GlobImgMng, This->handle, gc);
 if(This->handle)
  F(GlobImgMng).draw(GlobImgMng, This->handle,
		     window, gc,
		     x, y, w, h, x+dec_x, y+dec_y);

 Xc_TRACE(("done"));
}


static char *get_data(This)
c_Image *This;
{
 unsigned char *line_buffer;
 c_Dither *dither;
 char	*data = NULL;
 boolean need_dither = TRUE;
 int	dither_mode = 0;
 long	data_size;
 error	rc;
  
 Xc_TRACE(("IMGDATA_NOT_LOADED"));
 data_size = This->scrn_byte_per_line * This->height;
 data = Xc_malloc("Image data", data_size);
 if (data == NULL)
 {
  char	erreur[100];
      
  sprintf(erreur, 
	  F(GlobResources).getString(GlobResources, XcR_memoryError),
	  data_size);
  Xc_ErrorAlert(erreur);
 }
    
 switch(This->type)
 {
 case BITMAP:
  switch(This->depth)
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
  switch(This->depth)
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
  switch(This->depth)
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
  switch(This->depth)
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
			  This->byte_per_line);
  dither = NEW(c_Dither)(dither_mode, This->colormap, 
			 This, data);
  if (line_buffer == NULL || dither == NULL)
  {
   char	erreur[100];
	    
   sprintf(erreur, 
	   F(GlobResources).getString(GlobResources, XcR_memoryError),
	   This->byte_per_line);
   Xc_ErrorAlert(erreur);
   if (line_buffer) Xc_free(line_buffer);
   Xc_free(data); 
   data = NULL;
   return NULL;
  }
  rc = This->load_module->load(This, line_buffer,
			       (void *)dither->callback,
			       (void *)dither);
  DELETE(c_Dither)(dither);
      
  Xc_free(line_buffer);
  if (rc != XC_NO_ERROR)
  {
   Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					    XcR_loadError));
   Xc_free(data); data = NULL;
   return NULL;
  }
 }
 else
 {
  rc = This->load_module->load(This, data,
			       (void *)simple_load_callback,
			       (void *)This->byte_per_line);
  if (rc != XC_NO_ERROR)
  {
   Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					    XcR_loadError));
   Xc_free(data); data = NULL;
   return NULL;
  }
 }
 return data;
}

/* ----------------------------------------------------------------- ** 
** read_image_header - load real image header                        ** 
** ----------------------------------------------------------------- */
static boolean read_image_header(This, load_mode)
c_Image	*This;
boolean	load_mode;
{
 error	rc;
  
 rc = This->load_module->getHeader(This);
 if (rc != XC_NO_ERROR)
 {
  Xc_WARNING(("`%s' image error %d", This->filename, rc));
  return FALSE;
 }
  
 Xc_TRACE(("image: file: %s", This->filename));
 Xc_TRACE(("image: %dx%dx%d (colormap depth: %d)",
	   This->width, This->height, 
	   This->depth, This->colormap->depth));
 This->scrn_pal_alloc = FALSE;
  
 switch(This->depth)
 {
  /*--- Monochrome image ---*/
 case 1:
  This->type = BITMAP;
  This->scrn_depth = This->depth;
  This->byte_per_line = This->scrn_byte_per_line = 
   (This->width + 7) >> 3;
  This->nb_color = This->scrn_nb_color = 2;
  This->scrn_pal_r = This->colormap->red;
  This->scrn_pal_v = This->colormap->green;
  This->scrn_pal_b = This->colormap->blue;
  break;
 case 8:
  This->byte_per_line = This->width;
  This->nb_color = 256;
    
  if (load_mode == IMGLOAD_SCREEN_DEPTH)
  {
   switch(This->colormap->depth)
   {
   case 1:
    This->type = BITMAP;
    This->scrn_depth = 1;
    This->scrn_byte_per_line = (This->width+7)>>3;
    This->scrn_nb_color = 2;
    This->scrn_pal_r = This->colormap->red;
    This->scrn_pal_v = This->colormap->green;
    This->scrn_pal_b = This->colormap->blue;
    break;
   case 8:
    This->type = PIXMAP8;
    This->scrn_depth = 8;
    This->scrn_byte_per_line = This->byte_per_line;
    This->scrn_nb_color = 256;
    This->scrn_pal_r = This->colormap->red;
    This->scrn_pal_v = This->colormap->green;
    This->scrn_pal_b = This->colormap->blue;
    break;
   case 16:
    This->type = PIXMAP16;
    This->scrn_depth = 16;
    This->scrn_byte_per_line = 
     This->byte_per_line << 1;
    This->scrn_nb_color = -1;
    This->scrn_pal_r = This->pal_r;
    This->scrn_pal_v = This->pal_v;
    This->scrn_pal_b = This->pal_b;
    break;
   case 24:
    This->type = PIXMAP24;
    This->scrn_depth = 24;
    This->scrn_byte_per_line =
     This->byte_per_line * 3;
    This->scrn_nb_color = -1;
    This->scrn_pal_r = This->pal_r;
    This->scrn_pal_v = This->pal_v;
    This->scrn_pal_b = This->pal_b;
    break;
   default:
    Xc_ERROR(("Image depth not implemented"));
    return FALSE;
   }
  }
  else				/* IMGLOAD_IMAGE_DEPTH */
  {
   This->type = PIXMAP8;
   This->scrn_depth = This->depth;
   This->scrn_byte_per_line = This->byte_per_line;
   This->scrn_nb_color = This->nb_color;
   This->scrn_pal_r = This->pal_r;
   This->scrn_pal_v = This->pal_v;
   This->scrn_pal_b = This->pal_b;
  }
  break;
 case 16:
  This->byte_per_line = This->width << 1;
  This->nb_color = -1;
  This->pal_r = NULL;
  This->pal_v = NULL;
  This->pal_b = NULL;
  if (load_mode == IMGLOAD_SCREEN_DEPTH)
  {
   switch(This->colormap->depth)
   {
   case 1:
    This->type = BITMAP;
    This->scrn_depth = 1;
    This->scrn_byte_per_line = (This->width+7)>>3;
    This->scrn_nb_color = 2;
    This->scrn_pal_r = This->colormap->red;
    This->scrn_pal_v = This->colormap->green;
    This->scrn_pal_b = This->colormap->blue;
    break;
   case 8:
    This->type = PIXMAP8;
    This->scrn_depth = 8;
    This->scrn_byte_per_line = This->width;
    This->scrn_nb_color = 256;
    This->scrn_pal_r = This->colormap->red;
    This->scrn_pal_v = This->colormap->green;
    This->scrn_pal_b = This->colormap->blue;
    break;
   case 16:
    This->type = PIXMAP16;
    This->scrn_depth = This->depth;
    This->scrn_byte_per_line = This->byte_per_line;
    This->scrn_nb_color = This->nb_color;
    This->scrn_pal_r = This->pal_r;
    This->scrn_pal_v = This->pal_v;
    This->scrn_pal_b = This->pal_b;
    break;
   case 24:
    This->type = PIXMAP24;
    This->scrn_depth = 24;
    This->scrn_byte_per_line = This->width * 3;
    This->scrn_nb_color = -1;
    This->scrn_pal_r = This->pal_r;
    This->scrn_pal_v = This->pal_v;
    This->scrn_pal_b = This->pal_b;
    break;
   default:
    Xc_ERROR(("Image depth not implemented"));
    return FALSE;
   }
  }
  else				/* IMGLOAD_IMAGE_DEPTH */
  {
   This->type = PIXMAP16;
   This->scrn_depth = This->depth;
   This->scrn_byte_per_line = This->byte_per_line;
   This->scrn_nb_color = This->nb_color;
   This->scrn_pal_r = This->pal_r;
   This->scrn_pal_v = This->pal_v;
   This->scrn_pal_b = This->pal_b;
  }
  break;
 case 24:
  This->byte_per_line = This->width * 3;
  This->nb_color = -1;
  This->pal_r = NULL;
  This->pal_v = NULL;
  This->pal_b = NULL;
  if (load_mode == IMGLOAD_SCREEN_DEPTH)
  {
   switch(This->colormap->depth)
   {
   case 1:
    This->type = BITMAP;
    This->scrn_depth = 1;
    This->scrn_byte_per_line = (This->width+7)>>3;
    This->scrn_nb_color = 2;
    This->scrn_pal_r = This->colormap->red;
    This->scrn_pal_v = This->colormap->green;
    This->scrn_pal_b = This->colormap->blue;
    break;
   case 8:
    This->type = PIXMAP8;
    This->scrn_depth = 8;
    This->scrn_byte_per_line = This->width;
    This->scrn_nb_color = 256;
    This->scrn_pal_r = This->colormap->red;
    This->scrn_pal_v = This->colormap->green;
    This->scrn_pal_b = This->colormap->blue;
    break;
   case 16:
    This->type = PIXMAP16;
    This->scrn_depth = 16;
    This->scrn_byte_per_line = This->width << 1;
    This->scrn_nb_color = -1;
    This->scrn_pal_r = This->pal_r;
    This->scrn_pal_v = This->pal_v;
    This->scrn_pal_b = This->pal_b;
    break;
   case 24:
    This->type = PIXMAP24;
    This->scrn_depth = This->depth;
    This->scrn_byte_per_line = This->byte_per_line;
    This->scrn_nb_color = This->nb_color;
    This->scrn_pal_r = This->pal_r;
    This->scrn_pal_v = This->pal_v;
    This->scrn_pal_b = This->pal_b;
    break;
   default:
    Xc_ERROR(("Image depth not implemented"));
    return FALSE;
   }
  }
  else				/* IMGLOAD_IMAGE_DEPTH */
  {
   This->type = PIXMAP24;
   This->scrn_depth = This->depth;
   This->scrn_byte_per_line = This->byte_per_line;
   This->scrn_nb_color = This->nb_color;
   This->scrn_pal_r = This->pal_r;
   This->scrn_pal_v = This->pal_v;
   This->scrn_pal_b = This->pal_b;
  }
  break;
 default:
  Xc_ERROR(("Image depth not implemented"));
  return FALSE;
 }
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
