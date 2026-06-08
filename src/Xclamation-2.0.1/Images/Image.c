/*
** Image.c for Xclamation in Images/
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
** Last update Sun Apr 25 15:47:52 1999 Emmanuel Paris
*/

#include "Image.h"
#include "ImageCache.h"
#include "Cadre.h"

/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();
static error change_base();
static error display_init();
static void CalculImage();
static void display_redraw();
static void display_configure();
static boolean ps_print();
static boolean readImage();
static boolean writeImage();
static boolean cb_format();

extern c_Colormap	*GlobColormap;
extern c_ImageManager	*GlobImgMng;
extern c_ImageCache	*GlobImgCache;

extern image_load_t Load_Targa, Load_Jpeg, Load_Gif, Load_Pcx, Load_Tiff;
extern image_load_t Load_Xwd, Load_Bmp, Load_Mac, Load_Ras, Load_Iff;
extern image_load_t Load_Png, Load_Ppm;

image_load_t *image_load_list[] = 
{ 
 &Load_Pcx,
 &Load_Jpeg,
 &Load_Targa,
 &Load_Gif,
 &Load_Tiff,
 &Load_Xwd,
 &Load_Bmp,
 &Load_Mac,
 &Load_Ras,
 &Load_Iff,
 &Load_Png,
 &Load_Ppm,
 NULL
};

/*--- Methods ---*/
sf_Image fc_Image =
{
 /*___Standard methods___*/
 constructor, destructor, copy,

 /*___Standard frame methods___*/
 change_base, display_init, display_redraw, display_configure,
 ps_print, readImage, writeImage,

 /*___Image specific methods___*/
 Zoom_Normal_Mode,
 Zoom_Auto_Resize_Mode,
 Zoom_Best_Aspect_Mode,
 Zoom_Max_Aspect_Mode,
 Reinit_Zoom_Image,
 Recenter_Image,
 Flip_Horizontal_Image,
 Flip_Vertical_Image,
 Mapping1_Mode,
 Mapping2_Mode
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
  
 Image->cache_id = F(GlobImgCache).load(GlobImgCache, filename, Image,
					load_module, IMGLOAD_SCREEN_DEPTH);
  
 if (Image->cache_id == NULL)
 {
  Xc_free(Image);
  return NULL;
 }
  
 Image->image_calc 	= FALSE;
 Image->handle		= NULL; 	
 Image->zoom_type 	= NO_MODE;
 Image->Dx_zoom = (real)0;
 Image->Dy_zoom = (real)0;
 Image->Dangle = (angle_t)0;
  
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
  
 if(This->image_calc)
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
BaseStd_t	*base;
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
  
 Image->colormap	= GlobColormap;
 Image->image_calc	= FALSE;
 Image->Dx_zoom	= Image->Dy_zoom = 0;
 Image->Dangle		= (angle_t)0;
 return Image;
}

/* ----------------------------------------------------------------- ** 
** change base                                                       ** 
** ----------------------------------------------------------------- */

static error change_base(This, base)
c_Image		*This;
BaseStd_t	*base;
{
 return XC_NO_ERROR;
}


/* ----------------------------------------------------------------- ** 
** display_init                                                      ** 
** ----------------------------------------------------------------- */
static error display_init(This, cadre)
c_Image *This;
c_Cadre *cadre;
{
 coord_t	cwidth;
 coord_t	cheight;
 coord_t	scale;
 angle_t	angle;
 int		width,height;
 real		x_zoom = 1.0;
 real		y_zoom = 1.0;
 real		facteur;
 double	cs,ss;
 double	tmp;

 Xc_HISTORY(("display_init"));

 facteur = (real)(SCALE_FROM_PERCENT(100) / (real)cadre->sFactor);
 cwidth = cadre->saWidth;
 cheight = cadre->saHeight;
 scale = cadre->sScale;
 Xc_TRACE(("cwidth: %d cheight: %d scale: %d",cwidth,cheight,scale));

 Xc_TRACE(("Image size (%d, %d)",This->width, This->height));
 switch(This->zoom_type)
 {
 case NO_MODE:
  This->Dx_zoom	= This->Dy_zoom = 0;
  This->dwidth	= This->width * COORD_DEFAULT;
  This->dheight	= This->height * COORD_DEFAULT;
  This->x_zoom	= 1.0;
  This->y_zoom	= 1.0;
  This->x_orig	= (cwidth - This->width * COORD_DEFAULT)/2;
  This->y_orig	= (cheight- This->height * COORD_DEFAULT)/2;
  This->zoom_type	= ZOOM_NORMAL_MODE;
  This->angle	= (angle_t)0;

 case ZOOM_NORMAL_MODE:
  x_zoom		= This->x_zoom * facteur;
  y_zoom		= This->y_zoom * facteur;
  This->x_orig	-= cadre->sConfig_X;
  This->y_orig	-= cadre->sConfig_Y;
  This->Dx_orig	= This->x_orig / scale;
  This->Dy_orig	= This->y_orig / scale;
  This->Dwidth	= This->dwidth / scale;
  This->Dheight	= This->dheight / scale;
  break;

 case MAPPING1_MODE:
 case MAPPING2_MODE:
  cwidth = cadre->sWidth;
  cheight = cadre->sHeight;
 case ZOOM_AUTO_RESIZE_MODE:
  This->x_orig	=  This->y_orig	=  This->Dx_orig = This->Dy_orig = 0;
  This->dwidth	= cwidth ;
  This->dheight	= cheight ;
  This->Dwidth	= cwidth / scale;
  This->Dheight	= cheight / scale;
  This->x_zoom	= (real)(This->width*COORD_DEFAULT) / 
   (real)This->dwidth;
  This->y_zoom	= (real)(This->height*COORD_DEFAULT) / 
   (real)This->dheight;
  x_zoom		= This->x_zoom * facteur;
  y_zoom		= This->y_zoom * facteur;
  break;

 case ZOOM_BEST_ASPECT_MODE:
  This->dwidth	= This->width * COORD_DEFAULT;
  This->dheight	= This->height * COORD_DEFAULT;
  if (((real)This->dwidth / (real)cwidth ) >
      ((real)This->dheight / (real)cheight))
  {
   This->Dx_orig	= This->x_orig = 0;
   This->dheight = (coord_t)(((real)This->dheight
			      * (real)cwidth) / (real)This->dwidth);
   This->dwidth	= cwidth;
   This->Dheight = This->dheight / scale;
   This->Dwidth	= cwidth / scale;
   This->y_orig	= (cheight - This->dheight) / 2;
   This->Dy_orig	= This->y_orig / scale;
   Xc_TRACE(("1. dwidth %d  dheight %d",This->dwidth,This->dheight));
  }
  else
  {
   This->Dy_orig = This->y_orig = 0;
   This->dwidth	= (coord_t)(((real)This->dwidth
			     * (real)cheight) / (real)This->dheight);
   This->dheight = cheight;
   This->Dwidth  = This->dwidth / scale;
   This->Dheight = cheight / scale;
   This->x_orig  = (cwidth - This->dwidth) / 2;
   This->Dx_orig = This->x_orig / scale;
   Xc_TRACE(("2. dwidth %d  dheight %d",This->dwidth,This->dheight));
  }
  This->x_zoom	= (real)(This->width * COORD_DEFAULT) /
   (real) This->dwidth;
  This->y_zoom	= (real)(This->height * COORD_DEFAULT) /
   (real) This->dheight;
  x_zoom		= This->x_zoom * facteur;
  y_zoom		= This->y_zoom * facteur;
  break;

 case ZOOM_MAX_ASPECT_MODE:
  This->dwidth	= This->width * COORD_DEFAULT;
  This->dheight	= This->height * COORD_DEFAULT;
  if (((real)This->dwidth / (real)cwidth ) <
      ((real)This->dheight / (real)cheight))
  {
   This->Dx_orig	= This->x_orig = 0;
   This->dheight = (coord_t)(((real)This->dheight
			      * (real)cwidth) / (real)This->dwidth);
   This->dwidth	= cwidth;
   This->Dheight = This->dheight / scale;
   This->Dwidth	= cwidth / scale;
   This->y_orig	= (cheight - This->dheight) / 2;
   This->Dy_orig	= This->y_orig / scale;
   Xc_TRACE(("dwidth %d  dheight %d",This->dwidth,This->dheight));
  }
  else
  {
   This->Dy_orig = This->y_orig = 0;
   This->dwidth	= (coord_t)(((real)This->dwidth
			     * (real)cheight) / (real)This->dheight);
   This->dheight = cheight;
   This->Dwidth  = This->dwidth / scale;
   This->Dheight = cheight / scale;
   This->x_orig  = (cwidth - This->dwidth) / 2;
   This->Dx_orig = This->x_orig / scale;
   Xc_TRACE(("dwidth %d  dheight %d",This->dwidth,This->dheight));
  }
  This->x_zoom	= (real)(This->width * COORD_DEFAULT) /
   (real) This->dwidth;
  This->y_zoom	= (real)(This->height * COORD_DEFAULT) /
   (real) This->dheight;
  x_zoom		= This->x_zoom * facteur;
  y_zoom		= This->y_zoom * facteur;
  break;
 }
  
 if (This->zoom_type != MAPPING1_MODE &&
     This->zoom_type != MAPPING2_MODE )
 {
  angle = cadre->sAngle + This->angle;
    
  if (angle)
  {
   cs = cos( SCALE_TO_RADIANS(angle));
   ss = sin( SCALE_TO_RADIANS(angle));
   width  = (int)(ABS(cs * This->Dwidth) + ABS(ss * This->Dheight));
   height = (int)(ABS(ss * This->Dwidth) + ABS(cs * This->Dheight));
      
   Xc_TRACE(("angle/90: %d",
	     ((int)(SCALE_TO_DEGREES(angle+SCALE_TO_DEGREES(360))/90.0))));
   switch(((int)(SCALE_TO_DEGREES(angle+SCALE_TO_DEGREES(360))/90.0))%4)
   {
   case 0:
    This->Dx_offset = (real) 0.0;
    This->Dy_offset = (real)(ss * This->Dwidth);
    break;
   case 1:
    This->Dx_offset = (real)(-cs * This->Dwidth);
    This->Dy_offset = (real) height;
    break;
   case 2:
    This->Dx_offset = (real) width;
    This->Dy_offset = (real) (-cs * This->Dheight);
    break;
   default:
    This->Dx_offset = (real)(-ss * This->Dheight);
    This->Dy_offset = (real) 0.0;
    break;
   }
      
   This->Dwidth  = width;
   This->Dheight = height;

   tmp = (double)This->Dx_orig;
   This->Dx_orig = (int)((real)cadre->saOrig_X/cadre->sScale
			 + (cs * This->Dx_orig)
			 + (ss * This->Dy_orig)
			 - This->Dx_offset);
   This->Dy_orig = (int)((real)cadre->saOrig_Y/cadre->sScale
			 + (cs * This->Dy_orig)
			 - (ss * tmp)
			 - This->Dy_offset);
  }      
  else
  {
   This->Dx_offset = This->Dy_offset = 0.0;
  }
  if((This->Dx_zoom == x_zoom) && 
     (This->Dy_zoom == y_zoom) && 
     (This->Dangle == angle))
   return XC_NO_ERROR;
 }
 else
 {
  angle = (angle_t)0;
 }
 This->Dangle = angle;
 This->Dx_zoom = x_zoom;
 This->Dy_zoom = y_zoom;
  
 Xc_TRACE(("Dx_orig %d  Dy_orig %d  Dwidth %d  Deight %d  Dx_zoom %f  Dy_zoom %f",This->Dx_orig,This->Dy_orig,This->Dwidth,This->Dheight,
	   This->Dx_zoom,This->Dy_zoom));
  
 switch(This->type)
 {
 case BITMAP:
  This->Dbyte_per_line = (This->Dwidth + 7) >> 3;
  break;
 case PIXMAP4:
  This->Dbyte_per_line = ((This->Dwidth + 7) >> 3 ) << 2;
  break;
 case PIXMAP8:
  This->Dbyte_per_line = This->Dwidth;
  break;
 case PIXMAP16:
  This->Dbyte_per_line = This->Dwidth << 1;
  break;
 case PIXMAP24:
  if (GlobImgMng->real_depth == 24)
   This->Dbyte_per_line = This->Dwidth * 3;
  else
   This->Dbyte_per_line = This->Dwidth << 2;      
  break;
 case PIXMAP32:
  This->Dbyte_per_line = This->Dwidth << 2;
  break;
 }    

 if (This->image_calc)
 {
  F(GlobImgMng).free(GlobImgMng, This->handle);
  This->handle = NULL;
  This->image_calc = FALSE;
 }
 /*CalculImage(This, cadre);*/

 Xc_TRACE(("display init done"));
 return XC_NO_ERROR;
}


/* -------------------------------------------------------------------- **
**			Calcul Image ...				**
** -------------------------------------------------------------------- */
static void CalculImage(This,cadre)
c_Image	*This;
c_Cadre	*cadre;
{
 char		*data;
 char		*data_calc;
 double	x_zoom;
 double	y_zoom;
 double	x;
 double	y;
 double	orig_x = 0.0;
 double	orig_y = 0.0;
 boolean	inverse;
 int		oct;
 int		i,j,k;
 int		svg;
 unsigned long	filler, fil1, fil2, fil3;
 int		indexy;
 int		index,offset;
 int		bindex, nx;
 int		xmin,xmax,count;
 static char   bit_test[8]={128,64,32,16,8,4,2,1};
 static char	bit_setLM[8]={1,2,4,8,16,32,64,128};
 static char	bit_setML[8]={128,64,32,16,8,4,2,1};
 char		*bit_set;
 real		cs,ss;
 real		sx,sy;
 real		add_x_x = 0.0, add_x_y = 0.0;
 real		add_y_x = 0.0, add_y_y = 0.0;
 Ligne_h	*ligne;

 Xc_HISTORY(("CalculImage"));

 This->image_calc = TRUE;
 This->display = cadre->X_info.display;
  
 data = F(GlobImgCache).get_data(GlobImgCache, This->cache_id);
 if (data == NULL)
 {
  This->can_display = FALSE;
  return;
 }
  
 x_zoom = This->Dx_zoom;
 y_zoom = This->Dy_zoom;
 if (This->Dangle)
 {
  cs = cos( SCALE_TO_RADIANS(This->Dangle));
  ss = sin( SCALE_TO_RADIANS(This->Dangle));
  add_x_x = cs * x_zoom;
  add_x_y = ss * y_zoom;
  add_y_x = - ss * x_zoom;
  add_y_y = cs * y_zoom; 
  orig_x = -(This->Dy_offset * add_y_x + This->Dx_offset * add_x_x);
  orig_y = -(This->Dx_offset * add_x_y + This->Dy_offset * add_y_y);
 }


 if ( (This->Dheight * This->Dbyte_per_line ==0) ||
     ((data_calc = (char *) Xc_malloc
       ("datac", sizeof(char) *This->Dheight*This->Dbyte_per_line)) == NULL))
 {
  This->can_display = FALSE;
  return;
 }
 else
  This->can_display = TRUE;
  
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

  filler = (char)cadre->X_info.inside_color;

  switch(This->zoom_type)
  {
  case MAPPING1_MODE:
   Xc_TRACE(("mapping 1 in mono not done"));
   break;
  case MAPPING2_MODE:
   Xc_TRACE(("mapping 2 in mono not done"));
   break;
  default:
   if (cadre->sAngle)
   {
    index = oct = svg = offset=0;
    x = orig_x;
    y = orig_y;
    for( i = 0; i < This->Dheight; i++)
    {
     sx = x;
     sy = y;
     for(j = 0; j < This->Dwidth; j++)
     {
      nx = (This->flip_H?This->width-(int)x:(int)x);
      bindex =(nx & 7);
      index =  (This->flip_V?This->height-(int)y:(int)y) 
       * This->scrn_byte_per_line + (nx >> 3);
      if (x>=0 && x<This->width && y>=0 && y<This->height)
      {
       if (data[index] & bit_test[bindex])
	svg |= bit_set[oct];
      }
      else
       if (filler)
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
      x += add_x_x;
      y += add_x_y;
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
     x = sx + add_y_x;
     y = sy + add_y_y;
    }
   }
   else
   {
    index = oct = svg = offset = 0;
    y = 0.0;
    for(i = 0; i < This->Dheight; i++)
    {
     x = 0.0;
     indexy = (This->flip_V?This->height-(int)y:(int)y) 
      * This->scrn_byte_per_line;
     for(j = 0; j < This->Dwidth; j++)
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
   }
   break;
  }
  This->handle = F(GlobImgMng).allocate(GlobImgMng, data_calc,
					This->Dwidth, This->Dheight, 1,
					FALSE);
  break;
    
  /*--- Pixmap 8 planes ---*/
 case PIXMAP8:
  Xc_TRACE(("Pixmap 8 planes type"));
  filler = cadre->X_info.inside_color;
    
  switch(This->zoom_type)
  {
  case MAPPING1_MODE:
   Xc_TRACE(("mapping 1"));
   ligne = cadre->Forme->ligne;
   y = 0.0;
   offset = 0;
   for( i = 0; i < This->Dheight; i++)
   {
    x = 0.0;
    if (ligne->nb_seg)
    {
     xmin = ligne->segment[0].x_min;
     xmax = ligne->segment[ligne->nb_seg-1].x_max;
     x_zoom = (double)This->width / (double)(xmax-xmin+1);
     indexy = ((This->flip_V?This->height-(int)y:(int)y)
	       * This->scrn_byte_per_line);
     for(j = 0; j < xmin; j++)
      data_calc[offset++] = filler;
		
     for( ; j < MIN(xmax+1,This->Dwidth); j++)
     {
      data_calc[offset++] = ((unsigned char *)data)
       [indexy + (This->flip_H?This->width-(int)x:(int)x)];
      x += x_zoom;
     }
     for( ; j < This->Dwidth; j++)
      data_calc[offset++] = filler;
    }
    else
    {
     for(j = 0; j < This->Dwidth; j++)
      data_calc[offset++] = filler;
    }
    y += y_zoom;
    ligne++;
   }
   break;
	
  case MAPPING2_MODE:
   Xc_TRACE(("mapping 2"));
   ligne = cadre->Forme->ligne;
   y = 0.0;
   offset = 0;
   for( i = 0; i < This->Dheight; i++)
   {
    count = 0;
    for(k=0; k < ligne->nb_seg; k++)
     count += ligne->segment[k].x_max-ligne->segment[k].x_min+1;
    if (count)
    {
     xmin = ligne->segment[0].x_min;
     xmax = ligne->segment[ligne->nb_seg-1].x_max;
     x_zoom = (double)This->width / (double)(count);
     x = 0.0;
     indexy = ((This->flip_V?This->height-(int)y:(int)y)
	       * This->scrn_byte_per_line);
     for(j = 0; j < xmin; j++)
      data_calc[offset++] = filler;
		
     for(k=0; k < ligne->nb_seg-1; k++)
     {
      for( ; j<=ligne->segment[k].x_max; j++)
      {
       data_calc[offset++] = ((unsigned char *)data)
	[indexy + (This->flip_H?This->width-(int)x:(int)x)];
       x += x_zoom;
      }
      for( ; j < ligne->segment[k+1].x_min; j++)
       data_calc[offset++] = filler;
     }
     for( ; j < MIN(ligne->segment[k].x_max+1,This->Dwidth); j++)
     {
      data_calc[offset++] = ((unsigned char *)data)
       [indexy + (This->flip_H?This->width-(int)x:(int)x)];
      x += x_zoom;
     }
     for( ; j < This->Dwidth; j++)
      data_calc[offset++] = filler;
    }
    else
    {
     for(j=0 ; j < This->Dwidth; j++)
      data_calc[offset++] = filler;
    }
    y += y_zoom;
    ligne++;
   }
   break;
	
  default:
   if (cadre->sAngle)
   {
    offset=0;
    x = orig_x;
    y = orig_y;
    for( i = 0; i < This->Dheight; i++)
    {
     sx = x;
     sy = y;
     for(j = 0; j < This->Dwidth; j++)
     {
      if (x>=0 && x<This->width && y>=0 && y<This->height)
       data_calc[offset++] = ((unsigned char *)data)
	[(This->flip_V?This->height-(int)y:(int)y)
	 * This->scrn_byte_per_line
	 + (This->flip_H?This->width-(int)x:(int)x)];
      else
       data_calc[offset++] = filler;
      x += add_x_x;
      y += add_x_y;
     }
     x = sx + add_y_x;
     y = sy + add_y_y;
    }
   }
   else
   {
    y = 0.0; 
    offset = 0;
    for( i = 0; i < This->Dheight; i++)
    {
     x = 0.0;
     indexy = ((This->flip_V?This->height-1-(int)y:(int)y)
	       * This->scrn_byte_per_line);
     for(j = 0; j < This->Dwidth; j++)
     {
      data_calc[offset++] = ((unsigned char *)data)
       [indexy + (This->flip_H?This->width-1-(int)x:(int)x)];
      x += x_zoom;
     }
     y += y_zoom;
    }
   } 
   break;
  }  
  This->handle = F(GlobImgMng).allocate(GlobImgMng, data_calc,
					This->Dwidth, This->Dheight, 8,
					FALSE);
  break;
    
  /*--- True color display ---*/
 case PIXMAP16:
  Xc_TRACE(("Pixmap 16 planes type"));
  filler = cadre->X_info.inside_color;
  fil1 = (filler & 0xff00) >> 8;
  fil2 = filler & 0xff;
    
  switch(This->zoom_type)
  {
  case MAPPING1_MODE:
   Xc_TRACE(("mapping 1"));
   ligne = cadre->Forme->ligne;
   y = 0.0;
   offset = 0;
   for( i = 0; i < This->Dheight; i++)
   {
    x = 0.0;
    if (ligne->nb_seg)
    {
     xmin = ligne->segment[0].x_min;
     xmax = ligne->segment[ligne->nb_seg-1].x_max;
     x_zoom = (double)This->width / (double)(xmax-xmin+1);
     indexy = ((This->flip_V?This->height-(int)y:(int)y)
	       * This->scrn_byte_per_line);
     for(j = 0; j < xmin; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
     }
		
     for( ; j < MIN(xmax+1,This->Dwidth); j++)
     {
      nx = indexy + (This->flip_H?This->width-(int)x:(int)x) * 2;
      data_calc[offset++] = ((unsigned char *)data)[nx];
      data_calc[offset++] = ((unsigned char *)data)[nx+1];
      x += x_zoom;
     }
     for( ; j < This->Dwidth; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
     }
    }
    else
    {
     for(j = 0; j < This->Dwidth; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
     }
    }
    y += y_zoom;
    ligne++;
   }
   break;
	
  case MAPPING2_MODE:
   Xc_TRACE(("mapping 2"));
   ligne = cadre->Forme->ligne;
   y = 0.0;
   offset = 0;
   for( i = 0; i < This->Dheight; i++)
   {
    count = 0;
    for(k=0; k < ligne->nb_seg; k++)
     count += ligne->segment[k].x_max-ligne->segment[k].x_min+1;
    if (count)
    {
     xmin = ligne->segment[0].x_min;
     xmax = ligne->segment[ligne->nb_seg-1].x_max;
     x_zoom = (double)This->width / (double)(count);
     x = 0.0;
     indexy = ((This->flip_V?This->height-(int)y:(int)y)
	       * This->scrn_byte_per_line);
     for(j = 0; j < xmin; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
     }
		
     for(k=0; k < ligne->nb_seg-1; k++)
     {
      for( ; j<=ligne->segment[k].x_max; j++)
      {
       nx = indexy +
	(This->flip_H ? This->width-(int)x : (int)x) * 2;
       data_calc[offset++] = ((unsigned char *)data)[nx];
       data_calc[offset++] = ((unsigned char *)data)[nx+1];
       x += x_zoom;
      }
      for( ; j < ligne->segment[k+1].x_min; j++)
      {
       data_calc[offset++] = fil1;
       data_calc[offset++] = fil2;
      }
     }
     for( ; j < MIN(ligne->segment[k].x_max+1,This->Dwidth); j++)
     {
      nx = indexy +
       (This->flip_H ? This->width-(int)x : (int)x) * 3;
      data_calc[offset++] = ((unsigned char *)data)[nx];
      data_calc[offset++] = ((unsigned char *)data)[nx+1];
      x += x_zoom;
     }
     for( ; j < This->Dwidth; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
     }
    }
    else
    {
     for(j=0 ; j < This->Dwidth; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
     }
    }
    y += y_zoom;
    ligne++;
   }
   break;
	
  default:
   if (cadre->sAngle)
   {
    offset=0;
    x = orig_x;
    y = orig_y;

    if (GlobImgMng->byte_order_LSBFirst)
    {
     for( i = 0; i < This->Dheight; i++)
     {
      sx = x;
      sy = y;
      for(j = 0; j < This->Dwidth; j++)
      {
       if (x>=0 && x<This->width && y>=0 && y<This->height)
       {
	nx = (This->flip_V?This->height-(int)y:(int)y)
	 * This->scrn_byte_per_line
	  + (This->flip_H ? 
	     This->width - (int)x : (int)x) * 2;
	data_calc[offset++]= ((unsigned char *)data)[nx+1];
	data_calc[offset++]= ((unsigned char *)data)[nx];
       }
       else
       {
	data_calc[offset++] = fil2;
	data_calc[offset++] = fil1;
       }
       x += add_x_x;
       y += add_x_y;
      }
      x = sx + add_y_x;
      y = sy + add_y_y;
     }
    }
    else
    {
     for( i = 0; i < This->Dheight; i++)
     {
      sx = x;
      sy = y;
      for(j = 0; j < This->Dwidth; j++)
      {
       if (x>=0 && x<This->width && y>=0 && y<This->height)
       {
	nx = (This->flip_V?This->height-(int)y:(int)y)
	 * This->scrn_byte_per_line
	  + (This->flip_H ? 
	     This->width - (int)x : (int)x) * 2;
	data_calc[offset++]= ((unsigned char *)data)[nx];
	data_calc[offset++]= ((unsigned char *)data)[nx+1];
       }
       else
       {
	data_calc[offset++] = fil1;
	data_calc[offset++] = fil2;
       }
       x += add_x_x;
       y += add_x_y;
      }
      x = sx + add_y_x;
      y = sy + add_y_y;
     }
    }
   }
   else
   {
    y = 0.0; 
    offset = 0;

    if (GlobImgMng->byte_order_LSBFirst)
    {
     for( i = 0; i < This->Dheight; i++)
     {
      x = 0.0;
      indexy = ((This->flip_V ? 
		 This->height - 1 - (int)y : (int)y)
		* This->scrn_byte_per_line);
      for(j = 0; j < This->Dwidth; j++)
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
     for( i = 0; i < This->Dheight; i++)
     {
      x = 0.0;
      indexy = ((This->flip_V ? 
		 This->height - 1 - (int)y : (int)y)
		* This->scrn_byte_per_line);
      for(j = 0; j < This->Dwidth; j++)
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
   }
   break;
  }  
  This->handle = F(GlobImgMng).allocate(GlobImgMng, data_calc,
					This->Dwidth, This->Dheight, 
					GlobColormap->depth,
					FALSE);    
  break;
 case PIXMAP24:
  Xc_TRACE(("Pixmap 24 planes type"));
  filler = cadre->X_info.inside_color;
  fil1 = (filler & 0xff0000) >> 16;
  fil2 = (filler & 0xff00) >> 8;
  fil3 = filler & 0xff;
    
  switch(This->zoom_type)
  {
  case MAPPING1_MODE:
   Xc_TRACE(("mapping 1"));
   ligne = cadre->Forme->ligne;
   y = 0.0;
   offset = 0;
   for( i = 0; i < This->Dheight; i++)
   {
    x = 0.0;
    if (ligne->nb_seg)
    {
     xmin = ligne->segment[0].x_min;
     xmax = ligne->segment[ligne->nb_seg-1].x_max;
     x_zoom = (double)This->width / (double)(xmax-xmin+1);
     indexy = ((This->flip_V?This->height-(int)y:(int)y)
	       * This->scrn_byte_per_line);
     for(j = 0; j < xmin; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
      data_calc[offset++] = fil3;
     }
		
     for( ; j < MIN(xmax+1,This->Dwidth); j++)
     {
      nx = indexy + (This->flip_H?This->width-(int)x:(int)x) * 3;
      data_calc[offset++] = ((unsigned char *)data)[nx];
      data_calc[offset++] = ((unsigned char *)data)[nx+1];
      data_calc[offset++] = ((unsigned char *)data)[nx+2];
      x += x_zoom;
     }
     for( ; j < This->Dwidth; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
      data_calc[offset++] = fil3;
     }
    }
    else
    {
     for(j = 0; j < This->Dwidth; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
      data_calc[offset++] = fil3;
     }
    }
    y += y_zoom;
    ligne++;
   }
   break;
	
  case MAPPING2_MODE:
   Xc_TRACE(("mapping 2"));
   ligne = cadre->Forme->ligne;
   y = 0.0;
   offset = 0;
   for( i = 0; i < This->Dheight; i++)
   {
    count = 0;
    for(k=0; k < ligne->nb_seg; k++)
     count += ligne->segment[k].x_max-ligne->segment[k].x_min+1;
    if (count)
    {
     xmin = ligne->segment[0].x_min;
     xmax = ligne->segment[ligne->nb_seg-1].x_max;
     x_zoom = (double)This->width / (double)(count);
     x = 0.0;
     indexy = ((This->flip_V?This->height-(int)y:(int)y)
	       * This->scrn_byte_per_line);
     for(j = 0; j < xmin; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
      data_calc[offset++] = fil3;
     }
		
     for(k=0; k < ligne->nb_seg-1; k++)
     {
      for( ; j<=ligne->segment[k].x_max; j++)
      {
       nx = indexy +
	(This->flip_H ? This->width-(int)x : (int)x) * 3;
       data_calc[offset++] = ((unsigned char *)data)[nx];
       data_calc[offset++] = ((unsigned char *)data)[nx+1];
       data_calc[offset++] = ((unsigned char *)data)[nx+2];
       x += x_zoom;
      }
      for( ; j < ligne->segment[k+1].x_min; j++)
      {
       data_calc[offset++] = fil1;
       data_calc[offset++] = fil2;
       data_calc[offset++] = fil3;
      }
     }
     for( ; j < MIN(ligne->segment[k].x_max+1,This->Dwidth); j++)
     {
      nx = indexy +
       (This->flip_H ? This->width-(int)x : (int)x) * 3;
      data_calc[offset++] = ((unsigned char *)data)[nx];
      data_calc[offset++] = ((unsigned char *)data)[nx+1];
      data_calc[offset++] = ((unsigned char *)data)[nx+2];
      x += x_zoom;
     }
     for( ; j < This->Dwidth; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
      data_calc[offset++] = fil3;
     }
    }
    else
    {
     for(j=0 ; j < This->Dwidth; j++)
     {
      data_calc[offset++] = fil1;
      data_calc[offset++] = fil2;
      data_calc[offset++] = fil3;
     }
    }
    y += y_zoom;
    ligne++;
   }
   break;
	
  default:
   if (GlobImgMng->real_depth == 24)
   {
    if (cadre->sAngle)
    {
     offset=0;
     x = orig_x;
     y = orig_y;
	    
     if (GlobImgMng->byte_order_LSBFirst)
     {
      for( i = 0; i < This->Dheight; i++)
      {
       sx = x;
       sy = y;
       for(j = 0; j < This->Dwidth; j++)
       {
	if (x>=0 && x<This->width && y>=0 && y<This->height)
	{
	 nx = (This->flip_V?This->height-(int)y:(int)y)
	  * This->scrn_byte_per_line
	   + (This->flip_H?This->width-(int)x:(int)x) * 3;
	 data_calc[offset++] = ((unsigned char *)data)[nx+2];
	 data_calc[offset++] = ((unsigned char *)data)[nx+1];
	 data_calc[offset++] = ((unsigned char *)data)[nx];
	}
	else
	{
	 data_calc[offset++] = fil3;
	 data_calc[offset++] = fil2;
	 data_calc[offset++] = fil1;
	}
	x += add_x_x;
	y += add_x_y;
       }
       x = sx + add_y_x;
       y = sy + add_y_y;
      }
     }
     else
     {
      for( i = 0; i < This->Dheight; i++)
      {
       sx = x;
       sy = y;
       for(j = 0; j < This->Dwidth; j++)
       {
	if (x>=0 && x<This->width && y>=0 && y<This->height)
	{
	 nx = (This->flip_V?This->height-(int)y:(int)y)
	  * This->scrn_byte_per_line
	   + (This->flip_H?This->width-(int)x:(int)x) * 3;
	 data_calc[offset++] = ((unsigned char *)data)[nx];
	 data_calc[offset++] = ((unsigned char *)data)[nx+1];
	 data_calc[offset++] = ((unsigned char *)data)[nx+2];
	}
	else
	{
	 data_calc[offset++] = fil1;
	 data_calc[offset++] = fil2;
	 data_calc[offset++] = fil3;
	}
	x += add_x_x;
	y += add_x_y;
       }
       x = sx + add_y_x;
       y = sy + add_y_y;
      }
     }
    }
    else
    {
     y = 0.0; 
     offset = 0;
     if (GlobImgMng->byte_order_LSBFirst)
     {
      for( i = 0; i < This->Dheight; i++)
      {
       x = 0.0;
       indexy = ((This->flip_V?This->height-1-(int)y:(int)y)
		 * This->scrn_byte_per_line);
       for(j = 0; j < This->Dwidth; j++)
       {
	nx= indexy+(This->flip_H?This->width-1-(int)x:(int)x)*3;
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
      for( i = 0; i < This->Dheight; i++)
      {
       x = 0.0;
       indexy = ((This->flip_V?This->height-1-(int)y:(int)y)
		 * This->scrn_byte_per_line);
       for(j = 0; j < This->Dwidth; j++)
       {
	nx= indexy+(This->flip_H?This->width-1-(int)x:(int)x)*3;
	data_calc[offset++] = ((unsigned char *)data)[nx];
	data_calc[offset++] = ((unsigned char *)data)[nx+1];
	data_calc[offset++] = ((unsigned char *)data)[nx+2];
	x += x_zoom;
       }
       y += y_zoom;
      }
     }
    }
   }
   else				/* 32 bits display */
   {
    if (cadre->sAngle)
    {
     offset=0;
     x = orig_x;
     y = orig_y;
	    
     if (GlobImgMng->byte_order_LSBFirst)
     {
      for( i = 0; i < This->Dheight; i++)
      {
       sx = x;
       sy = y;
       for(j = 0; j < This->Dwidth; j++)
       {
	if (x>=0 && x<This->width && y>=0 && y<This->height)
	{
	 nx = (This->flip_V?This->height-(int)y:(int)y)
	  * This->scrn_byte_per_line
	   + (This->flip_H?This->width-(int)x:(int)x) * 3;
	 data_calc[offset++] = ((unsigned char *)data)[nx+2];
	 data_calc[offset++] = ((unsigned char *)data)[nx+1];
	 data_calc[offset++] = ((unsigned char *)data)[nx];
	 offset++;
	}
	else
	{
	 data_calc[offset++] = fil3;
	 data_calc[offset++] = fil2;
	 data_calc[offset++] = fil1;
	 offset++;
	}
	x += add_x_x;
	y += add_x_y;
       }
       x = sx + add_y_x;
       y = sy + add_y_y;
      }
     }
     else
     {
      for( i = 0; i < This->Dheight; i++)
      {
       sx = x;
       sy = y;
       for(j = 0; j < This->Dwidth; j++)
       {
	if (x>=0 && x<This->width && y>=0 && y<This->height)
	{
	 nx = (This->flip_V?This->height-(int)y:(int)y)
	  * This->scrn_byte_per_line
	   + (This->flip_H?This->width-(int)x:(int)x) * 3;
	 data_calc[offset++] = ((unsigned char *)data)[nx];
	 data_calc[offset++] = ((unsigned char *)data)[nx+1];
	 data_calc[offset++] = ((unsigned char *)data)[nx+2];
	 offset++;
	}
	else
	{
	 data_calc[offset++] = fil1;
	 data_calc[offset++] = fil2;
	 data_calc[offset++] = fil3;
	 offset++;
	}
	x += add_x_x;
	y += add_x_y;
       }
       x = sx + add_y_x;
       y = sy + add_y_y;
      }
     }
    }
    else
    {
     y = 0.0; 
     offset = 0;
     if (GlobImgMng->byte_order_LSBFirst)
     {
      for( i = 0; i < This->Dheight; i++)
      {
       x = 0.0;
       indexy = ((This->flip_V?This->height-1-(int)y:(int)y)
		 * This->scrn_byte_per_line);
       for(j = 0; j < This->Dwidth; j++)
       {
	nx= indexy+(This->flip_H?This->width-1-(int)x:(int)x)*3;
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
      for( i = 0; i < This->Dheight; i++)
      {
       x = 0.0;
       indexy = ((This->flip_V?This->height-1-(int)y:(int)y)
		 * This->scrn_byte_per_line);
       for(j = 0; j < This->Dwidth; j++)
       {
	nx= indexy+(This->flip_H?This->width-1-(int)x:(int)x)*3;
	data_calc[offset++] = ((unsigned char *)data)[nx];
	data_calc[offset++] = ((unsigned char *)data)[nx+1];
	data_calc[offset++] = ((unsigned char *)data)[nx+2];
	offset++;
	x += x_zoom;
       }
       y += y_zoom;
      }
     }
    }
   }
   break;
  }  
  This->handle = F(GlobImgMng).allocate(GlobImgMng, data_calc,
					This->Dwidth, This->Dheight, 24,
					FALSE);    
  break;
 case PIXMAP32:
  Xc_WARNING(("Compute 32 bits image : non implemented yet"));
  break;
 }
 Xc_TRACE(("Compute Image ok"));
}


/* -------------------------------------------------------------------- **
**				Draw Image 				**
** -------------------------------------------------------------------- */
static void display_redraw(This, cadre, polyg)
c_Image		*This;
c_Cadre		*cadre;
Polygone	*polyg;     
{
 int	Cx, Cy, Px, Py, Ix, Iy, Dx, Dy;
 int	Pwidth, Pheight, Iwidth, Iheight, Dwidth, Dheight;
  
 Xc_HISTORY(("display redraw"));

 if (!This->image_calc)
  CalculImage(This, cadre);

 Xc_TRACE(("will display"));
  
 Cx 		= cadre->Forme->Orig_X + cadre->Forme->x_min;
 Cy 		= cadre->Forme->Orig_Y + cadre->Forme->y_min;
 Px 		= polyg->Orig_X + polyg->x_min;
 Py 		= polyg->Orig_Y + polyg->y_min;
 Pwidth 	= polyg->x_max - polyg->x_min + 1;
 Pheight 	= polyg->y_max - polyg->y_min + 1;
 Ix 		= This->Dx_orig + Cx;
 Iy 		= This->Dy_orig + Cy;
 Iwidth 	= This->Dwidth;
 Iheight 	= This->Dheight;
 Dx		= MAX(Px, Ix);
 Dy		= MAX(Py, Iy);
 Dwidth	= MIN(Px + Pwidth, Ix + Iwidth) - Dx;
 Dheight	= MIN(Py + Pheight, Iy + Iheight) - Dy;
  
 Xc_TRACE(("calcul zone done"));

 if(This->can_display)
 {
  Xc_TRACE(("x %d, y %d, w %d, h %d, x_dest %d, y_dest %d",
	    Dx - Ix, Dy - Iy, Dwidth, Dheight, Dx, Dy));
      
  if((Dwidth > 0) && (Dheight > 0))
   F(GlobImgMng).draw(GlobImgMng, This->handle,
		      cadre->X_info.window, cadre->X_info.gc_inside,
		      Dx - Ix, Dy - Iy, Dwidth, Dheight, Dx, Dy);
 }
 else
 {
  XDrawLine(cadre->X_info.display,cadre->X_info.window,
	    cadre->X_info.gc_inside,Cx,Cy,
	    Cx+cadre->Forme->x_max,Cy+cadre->Forme->y_max);
  XDrawLine(cadre->X_info.display,cadre->X_info.window,
	    cadre->X_info.gc_inside,Cx+cadre->Forme->x_max,Cy,
	    Cx,Cy+cadre->Forme->y_max);
 }
 Xc_TRACE(("done"));
}


/* ----------------------------------------------------------------- ** 
** display_configure                                                 ** 
** ----------------------------------------------------------------- */
static void display_configure(this, frame, polygon)
c_Image *this;
c_Cadre *frame;
Polygone *polygon;
{
 Xc_HISTORY(("display_configure"));
  
 display_init(this, frame);
}

/* ----------------------------------------------------------------- ** 
** ps_print - Print to a ps file                                     ** 
** ----------------------------------------------------------------- */
static boolean ps_print(this, post)
c_Image		*this;
c_PostScript	*post;
{
 real x_coef, y_coef;
 ps_image_t *handle;
 error rc;
  
 Xc_HISTORY(("ps_print"));

 /*--- Set matrix ---*/
 Xc_TRACE(("dwidth = %g   dheight = %g",
	   SCALE_TO_POINTS(this->dwidth), SCALE_TO_POINTS(this->dheight)));
 x_coef = SCALE_FROM_COEF(SCALE_TO_POINTS(this->dwidth));
 y_coef = SCALE_FROM_COEF(SCALE_TO_POINTS(this->dheight));
 if(this->flip_H) x_coef = -x_coef;
 if(this->flip_V) y_coef = -y_coef;

 Xc_TRACE(("x_orig: %d y_orig: %d",this->x_orig,this->y_orig));

 F(post->matrix).translate(post->matrix, FALSE, this->x_orig, this->y_orig);
 F(post->matrix).scale(post->matrix, FALSE,
		       SCALE_FROM_COEF(x_coef),
		       SCALE_FROM_COEF(y_coef));

 if(this->flip_H || this->flip_V)
 {
  F(post->matrix).translate(post->matrix, FALSE,
			    this->flip_H? -COORD_ONE : COORD_ZERO,
			    this->flip_V? -COORD_ONE : COORD_ZERO);
 }
 F(post).putLine(post, "$m cm", F(post->matrix).get(post->matrix, 0));
  
 if (!post->print_param->Draft)
 {
  /*--- Print image ---*/
  handle = F(post).beginImage(post,
			      this->width, this->height, this->depth,
			      this->pal_r, this->pal_v, this->pal_b); 
  if(handle == NULL) return FALSE;
      
  rc = this->load_module->load(this, handle->line_buffer, 
			       (void *)handle->cb_func, handle);
  F(post).endImage(post, handle);
 }
 return TRUE;
}


/* ----------------------------------------------------------------- **
** readImage - Set an Image from a saved file                        **
** ----------------------------------------------------------------- */
static boolean readImage(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
 image_load_t **ptr;
 c_Image *image;

 Xc_TRACE(("read"));

 image = NULL;

 for(ptr = image_load_list; *ptr != NULL; ptr++)
  if(!F(doc).addCallbacks(doc, (*ptr)->id, cb_format, &image, NULL))
   return FALSE;

 while(42)
 {
  real x_orig, y_orig, dwidth, dheight, x_zoom, y_zoom, angle;
  boolean flip_H, flip_V;
  long zoom_type;
      
  if(!F(doc).readReal(doc, &x_orig) ||
     !F(doc).readReal(doc, &y_orig) ||
     !F(doc).readReal(doc, &dwidth) ||
     !F(doc).readReal(doc, &dheight) ||
     !F(doc).readInteger(doc, &zoom_type) ||
     !F(doc).readReal(doc, &x_zoom) ||
     !F(doc).readReal(doc, &y_zoom) ||
     !F(doc).readReal(doc, &angle) ||
     !F(doc).readBoolean(doc, &flip_H) ||
     !F(doc).readBoolean(doc, &flip_V) ||
     !F(doc).expectKeyword(doc, NULL, FALSE)) break;
  if(image == NULL) break;
      
  image->x_orig = SCALE_FROM_POINTS(x_orig);
  image->y_orig = SCALE_FROM_POINTS(y_orig);
  image->dwidth = SCALE_FROM_POINTS(dwidth);
  image->dheight = SCALE_FROM_POINTS(dheight);
  image->zoom_type = (int)zoom_type;
  image->x_zoom = SCALE_FROM_COEF(x_zoom);
  image->y_zoom = SCALE_FROM_COEF(y_zoom);
  image->angle = SCALE_FROM_DEGREES(angle);
  image->flip_H = flip_H;
  image->flip_V = flip_V;
      
  frame->object_type = CADRE_IMAGE_BITMAP;
  frame->object = image;
  F(frame).configure_request(frame, FALSE);
  /*F(frame).attachobject(frame, image, CADRE_IMAGE_BITMAP);*/
  Xc_TRACE(("load done"));
  return TRUE;
 }
  
 Xc_TRACE(("loading error"));
 if(image != NULL) DELETE(c_Image)(image);

 /*--- return TRUE-> Keep on loading ---*/
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** writeImage - Image saving                                         ** 
** ----------------------------------------------------------------- */
static boolean writeImage(this, doc)
c_Image *this;
c_DocFile *doc;
{
 Xc_HISTORY(("write"));

 return(F(doc).writeKeyword(doc, XcDF_IMAGE_KEYWORD, -1L) &&
	F(doc).startSequence(doc) &&
	F(doc).writeReal(doc, SCALE_TO_POINTS(this->x_orig)) &&
	F(doc).writeReal(doc, SCALE_TO_POINTS(this->y_orig)) &&
	F(doc).writeReal(doc, SCALE_TO_POINTS(this->dwidth)) &&
	F(doc).writeReal(doc, SCALE_TO_POINTS(this->dheight)) &&
	F(doc).writeInteger(doc, this->zoom_type) &&
	F(doc).writeReal(doc, SCALE_TO_COEF(this->x_zoom)) &&
	F(doc).writeReal(doc, SCALE_TO_COEF(this->y_zoom)) &&
	F(doc).writeReal(doc, SCALE_TO_DEGREES(this->angle)) &&
	F(doc).writeBoolean(doc, this->flip_H) &&
	F(doc).writeBoolean(doc, this->flip_V) &&
	F(doc).writeKeyword(doc, this->load_module->id, -1L) &&
	F(doc).startSequence(doc) &&
	F(doc).writeFilename(doc, this->filename) &&
	F(doc).endSequence(doc) &&
	F(doc).endSequence(doc));
}

/* ----------------------------------------------------------------- ** 
** cb_format - Loading callback for image format                     ** 
** ----------------------------------------------------------------- */
static boolean cb_format(doc, keyword, param, image)
c_DocFile *doc;
char *keyword;
long param;
c_Image **image;
{
 image_load_t **ptr;
 unsigned char *filename;

 Xc_TRACE(("cb_format"));

 for(ptr = image_load_list; *ptr != NULL; ptr++)
  if(!strcmp((*ptr)->id, keyword))
  {
   Xc_TRACE(("image format: `%s'", (*ptr)->name));
	
   if(!F(doc).readFilename(doc, &filename) || !filename) return FALSE;
   *image = NEW(c_Image)(filename, *ptr);
   Xc_free(filename);
   if(*image == NULL) return FALSE;
	
   Xc_TRACE(("cb_format done"));
   return TRUE;
  }
 return FALSE;
}




