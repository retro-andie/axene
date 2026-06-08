/*
** Colormap.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
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
** Started on  Mon Jul 17 14:14:37 1995 Emmanuel Paris
** Last update Wed Feb 16 23:59:18 2000 Emmanuel Paris
*/


#include <X11/Xlib.h>
#include "xcalibur.h"
#include "Colormap.h"
#ifndef NO_INTRINSIC
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/XmP.h>
#endif

#if defined(Xc_XCLAMATION) || defined(Xc_XMAYDAY) || defined(Xc_XINSTALL) ||\
defined(Xc_XALLWRITE)
#define HASH_TABLE
#endif

static void *cons_Colormap();
static void dest_Colormap();
static void *copy_Colormap();
static void generate_Colormap();
static void get_closest_color_static();
static void get_closest_color_pseudo();
static void get_closest_color_truecolor();
static void get_closest_color();
static unsigned long get_spec_color();
static void use_color();
#ifdef HASH_TABLE
static boolean Generate_Hash_Table();
#endif
static void Get_In_Hash_Table();
static boolean Allocate_Named_Color();
static boolean Get_RGB_Values();
#ifndef NO_INTRINSIC
static Boolean xt_convert_string_to_color();
#endif

sf_Colormap fc_Colormap=
{
 cons_Colormap,
 dest_Colormap,
 copy_Colormap,
 generate_Colormap,
 get_closest_color,
 get_spec_color,
 use_color,
 Get_In_Hash_Table,
 Allocate_Named_Color,
 Get_RGB_Values
};

#if defined(Xc_XCLAMATION) || defined(Xc_XALLWRITE)
#define XOR_COLORS
#define	MAX_SPEC_COLOR		6
unsigned short default_spec_color[MAX_SPEC_COLOR][3] =
{
 {0xf700, 0xff00, 0xff00}, {0xff00, 0xf700, 0xff00},
 {0xff00, 0xff00, 0xf700}, {0xf700, 0xf700, 0xff00},
 {0xf700, 0xff00, 0xf700}, {0xff00, 0xf700, 0xf700}
};
#endif

#ifdef Xc_XQUAD
#define	MAX_SPEC_COLOR		2
unsigned short default_spec_color[MAX_SPEC_COLOR][3] = 
{
 {0xcf5b, 0xcf5b, 0xcf5b}, {0x7fff, 0x7fff, 0x7fff} 
};
#endif

#ifndef MAX_SPEC_COLOR
#define NO_SPEC_COLOR
#define	MAX_SPEC_COLOR		0
unsigned short **default_spec_color;
#endif

/* -------------------------------------------------------------------- **
** --------		Constructor of Colormap		       --------	**
** -------------------------------------------------------------------- */

#ifndef NO_INTRINSIC
static void *cons_Colormap(display, wid, owncmap, free_color, force)
Display	*display;
Widget	wid;			/* only used if owncmap is True */
boolean owncmap;		/* if True, create a new private colormap */
int	free_color;
int	force;
#else
static void *cons_Colormap(display, window, owncmap, free_color, force)
Display	*display;
Window	window;			/* only used if owncmap is True */
boolean owncmap;		/* if True, create a new private colormap */
int	free_color;
int	force;
#endif
{
 c_Colormap	*This;
 Colormap	cmap;
 Visual	*visual;
 XColor	*colors;
 color_cells_t	tmpcolorc;
 unsigned long	*pixels;
 unsigned long	*tr, *tg, *tb;
 int		screen;
 int		count;
 int		i,j,k;
 int		nb_color;
 int		count_tmp;
 int		inc_r,inc_g,inc_b;
 int		for_r,for_g,for_b;
 int		val_r,val_g,val_b;
 int		type;
  
 Xc_HISTORY(("Constructor ..."));
  
 if ((This=(c_Colormap *)Xc_malloc("Colormap",sizeof(c_Colormap)))==NULL)
 {
  Xc_FATAL(("Can't build Colormap."));
 }
 memset(This, 0, sizeof(c_Colormap));
 This->f = &fc_Colormap;
 This->display = display;
 This->screen = screen = DefaultScreen(display);
 This->cmap = cmap = DefaultColormap(display, screen);
 This->depth = DefaultDepth(display, screen);
 This->nb_color = nb_color = DisplayCells(display, screen);
 This->max_color = This->nb_color;
 This->allocated = 0;
 This->pixels = NULL;
 This->usable_color = NULL;
 This->owncmap = owncmap;
  
 visual = DefaultVisual(display, screen);
 type = visual->class;
 if (type == PseudoColor)
 { 
  switch(force)
  {
  case FORCE_MONOCHROME:
   type = StaticGray;
   break;
  case FORCE_GRAYSCALE:
   type = GrayScale;
   break;
  case FORCE_COLOR:
  default:
   type = PseudoColor;
   break;
  }
  if (owncmap)
  {
   int i;
   unsigned long black, white;
   unsigned long pixels[48];
   XColor defcol[48];
#ifndef NO_INTRINSIC
   Window window = XtWindow(wid);
#endif

   black = BlackPixel(display, screen);
   white = WhitePixel(display, screen);
   
   This->cmap = cmap = 
    XCreateColormap(display, window, visual, AllocNone);

    
   for(i=0; i < 48; i++)
   {
    defcol[i].pixel = (unsigned long)i;
    pixels[i] = i;
   }
   
   XQueryColors(display, DefaultColormap(display, screen), defcol, 48);
   XAllocColorCells(display, cmap, FALSE, NULL, 0, pixels, 48);
   XStoreColors(display, cmap, defcol, 48);

   XInstallColormap(display, This->cmap);
#ifndef NO_INTRINSIC
   XtVaSetValues(wid, XtNcolormap, This->cmap, NULL);
#endif
   XSetWindowColormap(display, window, This->cmap);
   free_color = 0;
  }
 }

 switch(type)
 {
 case StaticGray:
  This->type = CMAP_STATICGRAY;
 case StaticColor: 
  if (visual->class == StaticColor)
   This->type = CMAP_STATICCOLOR;
  This->red =
   (unsigned short *)Xc_malloc("red",
			       sizeof(unsigned short) * nb_color);
  This->green =
   (unsigned short *)Xc_malloc("green",
			       sizeof(unsigned short) * nb_color);
  This->blue =
   (unsigned short *)Xc_malloc("blue",
			       sizeof(unsigned short) * nb_color);
  colors = (XColor *)Xc_malloc("colors", sizeof(XColor) * nb_color);
  for(i = 0; i < nb_color; i++)
   colors[i].pixel = i;
    
  XQueryColors(display, cmap, colors, nb_color);
  for(i = 0; i < nb_color; i++)
  {
   This->red[i] = colors[i].red;
   This->green[i] = colors[i].green;
   This->blue[i] = colors[i].blue;
  }
  Xc_free(colors);
  This->f->get_closest_color = get_closest_color_static;
  break;
        
 case GrayScale:
  This->type = CMAP_GRAYSCALE;
  This->pixels = (unsigned long *)Xc_malloc("pixels",
					    sizeof(unsigned long)*nb_color);
  This->red = (unsigned short *)Xc_malloc("red",
					  sizeof(unsigned short)*nb_color);
  This->green = (unsigned short *)Xc_malloc("green",
					    sizeof(unsigned short)*nb_color);
  This->blue = (unsigned short *)Xc_malloc("blue",
					   sizeof(unsigned short)*nb_color);
  This->usable_color = (boolean *)Xc_malloc("usable col",
					    sizeof(boolean)*nb_color);
  memset(This->pixels, 0, sizeof(unsigned long)*nb_color);
  memset(This->usable_color, 0, sizeof(boolean)*nb_color);

  pixels = (unsigned long *)Xc_malloc("pixels",
				      sizeof(unsigned long) * nb_color);
  count = nb_color;
  while(count && 
	!XAllocColorCells(display, cmap, FALSE, NULL, 0, pixels, count))
   count--;
  Xc_TRACE(("Color allocated: %d  Color free: %d", nb_color-count, count));
  if (free_color >= count)
   This->max_color = 0;
  else
   This->max_color = count - free_color;
    
  colors = (XColor *)Xc_malloc("colors",
			       sizeof(XColor) * (nb_color - count));
  k=0;
  for(i=0; i < (nb_color - count); i++)
  {
   while(k < nb_color)
   {
    j=0;
    while(j < count && pixels[j] != (unsigned long)k)
     j++;
    if (j == count)
     break;
    k++;
   }
   colors[i].pixel = k++;
  }
  XQueryColors(display, cmap, colors, nb_color-count);
  for(i = 0; i < nb_color - count; i++)
  {
   This->usable_color[colors[i].pixel] = TRUE;
   This->red[colors[i].pixel] = colors[i].red;
   This->green[colors[i].pixel] = colors[i].green;
   This->blue[colors[i].pixel] = colors[i].blue;
  }
  XFreeColors(display, cmap, pixels, count, 0);
  Xc_free(colors);
  Xc_free(pixels);
  This->f->get_closest_color = get_closest_color_pseudo;
  break;
    
 case PseudoColor:
  This->type = CMAP_PSEUDOCOLOR;
  This->pixels = (unsigned long *)Xc_malloc("pixels",
					    sizeof(unsigned long)*nb_color);
  This->red = (unsigned short *)Xc_malloc("red",
					  sizeof(unsigned short)*nb_color);
  This->green = (unsigned short *)Xc_malloc("green",
					    sizeof(unsigned short)*nb_color);
  This->blue = (unsigned short *)Xc_malloc("blue",
					   sizeof(unsigned short)*nb_color);
  This->usable_color = (boolean *)Xc_malloc("usable col",
					    sizeof(boolean)*nb_color);
  memset(This->pixels, 0, sizeof(unsigned long)*nb_color);
  memset(This->usable_color, 0, sizeof(boolean)*nb_color);
    
  pixels = (unsigned long *)Xc_malloc("pixels",
				      sizeof(unsigned long) * nb_color);
  count = nb_color;
  while(count && !XAllocColorCells(display, cmap, FALSE,
				   NULL, 0, pixels, count))
   count--;
  Xc_TRACE(("Color allocated: %d Color free: %d", nb_color - count, count));
  if (free_color >= count)
   This->max_color = 0;
  else
   This->max_color = count - free_color;
  colors = (XColor *)Xc_malloc("cols", sizeof(XColor) * (nb_color - count));
  k = 0;
  for(i = 0; i < (nb_color - count); i++)
  {
   while(k < nb_color)
   {
    j = 0;
    while(j < count && pixels[j] != (unsigned long)k)
     j++;
    if (j == count)
     break;
    k++;
   }
   colors[i].pixel = k++;
  }
  XQueryColors(display, cmap, colors, nb_color - count);
  for(i = 0; i < nb_color - count; i++)
  {
   This->usable_color[colors[i].pixel] = TRUE;
   This->red[colors[i].pixel] = colors[i].red;
   This->green[colors[i].pixel] = colors[i].green;
   This->blue[colors[i].pixel] = colors[i].blue;
  }
  XFreeColors(display, cmap, pixels, count, 0);
  Xc_free(colors);
  Xc_free(pixels);
  This->f->get_closest_color = get_closest_color_pseudo;
  break;
    
 case DirectColor:
  This->type=CMAP_DIRECTCOLOR;
 case TrueColor:
  if (type == TrueColor)
   This->type = CMAP_TRUECOLOR;
  Xc_TRACE(("type: %d nbc: %d", type, nb_color));
  This->allocated = visual->bits_per_rgb;
  This->red   = Xc_malloc("truecolr", sizeof(long)*(This->allocated+1));
  This->green = Xc_malloc("truecolg", sizeof(long)*(This->allocated+1));
  This->blue  = Xc_malloc("truecolb", sizeof(long)*(This->allocated+1));
  tr = (unsigned long *)This->red;
  tg = (unsigned long *)This->green;
  tb = (unsigned long *)This->blue;
  inc_r = visual->red_mask;
  inc_g = visual->green_mask;
  inc_b = visual->blue_mask;
  for_r = for_b = for_g = 0;
  val_r = val_b = val_g = 1 << (This->allocated * 3 - 1);
  count = (1 << (This->allocated * 3) ) - 1;
  count_tmp = val_r;
  for(i=0; i<This->allocated; i++)
  {
   while((inc_r&=count) && ((inc_r&count_tmp)==0))
   {
    inc_r <<= 1;
    val_r >>= 1;
   }
   if (inc_r)
   {
    for_r ++;
    tr[i+1] = val_r;
    inc_r <<= 1;
    val_r >>= 1;
   }
   else
    tr[i+1] = 0;
      
   while((inc_g&=count) && ((inc_g&count_tmp)==0))
   {
    inc_g <<= 1;
    val_g >>= 1;
   }
   if (inc_g)
   {
    for_g ++;
    tg[i+1] = val_g;
    inc_g <<= 1;
    val_g >>= 1;
   }
   else
    tg[i+1] = 0;
      
   while((inc_b&=count) && ((inc_b&count_tmp)==0))
   {
    inc_b <<= 1;
    val_b >>= 1;
   }
   if (inc_b)
   {
    for_b ++;
    tb[i+1] = val_b;
    inc_b <<= 1;
    val_b >>= 1;
   }
   else
    tb[i+1] = 0;
  }
  tr[0] = for_r; tg[0] = for_g; tb[0] = for_b;
  This->f->get_closest_color = get_closest_color_truecolor;
    
  break;
 }
 get_closest_color(This, &tmpcolorc, 0xffff, 0xffff, 0xffff);
 This->Cwhite = tmpcolorc.pixel;
 get_closest_color(This, &tmpcolorc, 0x0000, 0x0000, 0x0000);
 This->Cblack = tmpcolorc.pixel;
  
#ifndef NO_INTRINSIC
 {
  XtConvertArgRec	converter_arg;
    
  converter_arg.address_mode = XtAddress;
  converter_arg.address_id = (void *)This;
  converter_arg.size = sizeof(void *);
    
  XtSetTypeConverter(XtRString, XtRPixel, xt_convert_string_to_color,
		     &converter_arg, 1, XtCacheByDisplay,
		     NULL);
  /*
    XtSetTypeConverter(XtRString, XmRSelectColor, xt_convert_string_to_color,
    &converter_arg, 1, XtCacheByDisplay,
    NULL); */
 }
#endif
  
 Xc_HISTORY(("Constructor ok"));
 return This;
}

/* -------------------------------------------------------------------- **
** --------		Destructor of Colormap		       --------	**
** -------------------------------------------------------------------- */
static void dest_Colormap(This)
c_Colormap	*This;
{
  
 Xc_HISTORY(("Destructor ..."));
  
 if (This->pixels)
 {
  if (This->allocated)
   XFreeColors(This->display, This->cmap, This->pixels, This->allocated,
	       0);
  Xc_free(This->pixels);
 }
 if (This->owncmap)
 {
  XUninstallColormap(This->display, This->cmap);
  XFreeColormap(This->display, This->cmap);
 }

 if (This->hashtable) Xc_free(This->hashtable);
 if (This->red) Xc_free(This->red);
 if (This->green) Xc_free(This->green);
 if (This->blue) Xc_free(This->blue);
 if (This->spec_color) Xc_free(This->spec_color);
 if (This->usable_color) Xc_free(This->usable_color);
 Xc_free(This);

 Xc_HISTORY(("Destructor ok"));
}

/* -------------------------------------------------------------------- **
** --------		Copy method of Colormap		       --------	**
** -------------------------------------------------------------------- */

static void *copy_Colormap(This)
c_Colormap	*This;
{
 Xc_TRACE(("Copying Object Colormap"));
 return (void *)NULL;
}

/* -------------------------------------------------------------------- **
** --------   'Generate colormap' method of Colormap	       --------	**
** -------------------------------------------------------------------- */

static void generate_Colormap(This)
c_Colormap	*This;
{
 Display	*display;
 Colormap	cmap;
 XColor	*colors;
 color_cells_t	tmpcolorc;
 unsigned long	*pixels;
 double	linear,ln;
 int		count;
 int		i;
#ifndef NO_SPEC_COLOR
 int		j, k;
#endif
 int		nb_color;
 int		compo;
 int		count_tmp;
 int		inc_r,inc_g,inc_b;
 int		for_r,for_g,for_b;
 int		add_compo;
 boolean	get_white=FALSE;
  
 Xc_HISTORY(("Constructor ..."));
  
 display = This->display;
 cmap = This->cmap;
 nb_color = This->nb_color;

 switch(This->type)
 {
 case CMAP_STATICGRAY:
 case CMAP_STATICCOLOR:    
  This->spec_color =
   (unsigned long *)Xc_malloc("spec col", sizeof(unsigned long));
  get_white = TRUE;
  This->nb_spec_color = 1;
  if (nb_color == 2)
  {
   get_closest_color(This, &tmpcolorc, 0x0000, 0x0000, 0x0000);
   This->Cred = This->Cgreen = This->Cblue = This->Ccyan =
    This->Cyellow = This->Cmagenta = tmpcolorc.pixel;
  }
  else
  {
   get_closest_color(This,&tmpcolorc,0xffff,0x0000,0x0000);
   This->Cred=tmpcolorc.pixel;
   get_closest_color(This,&tmpcolorc,0x0000,0xffff,0x0000);
   This->Cgreen=tmpcolorc.pixel;
   get_closest_color(This,&tmpcolorc,0x0000,0x0000,0xffff);
   This->Cblue=tmpcolorc.pixel;
   get_closest_color(This,&tmpcolorc,0x0000,0xffff,0xffff);
   This->Ccyan=tmpcolorc.pixel;
   get_closest_color(This,&tmpcolorc,0xffff,0xffff,0x0000);
   This->Cyellow=tmpcolorc.pixel;
   get_closest_color(This,&tmpcolorc,0xffff,0x0000,0xffff);
   This->Cmagenta=tmpcolorc.pixel;
  }
  break;
    
    
 case CMAP_GRAYSCALE:
  pixels = (unsigned long *)Xc_malloc("pixels",
				      sizeof(unsigned long) * nb_color);
  count = This->max_color;
  while(count && !XAllocColorCells(display,cmap,FALSE,NULL,0,pixels,count))
   count--;
  Xc_TRACE(("Color allocated: %d   Color free: %d",nb_color-count,count));
    
  if (count>MAX_SPEC_COLOR)
  {
   count_tmp = MAX_SPEC_COLOR;
   This->spec_color=(unsigned long *)
    Xc_malloc("spec col",  sizeof(unsigned long)*count_tmp);
   for(i = 0 ; i < MAX_SPEC_COLOR; i++)
   {
    This->red[pixels[i]] = default_spec_color[i][0];
    This->green[pixels[i]] = default_spec_color[i][1];
    This->blue[pixels[i]] = default_spec_color[i][2];
    This->spec_color[i] = pixels[i];
    This->usable_color[pixels[i]] = TRUE;
   }
   This->nb_spec_color = MAX_SPEC_COLOR;
  }
  else
  {
   count_tmp = 0;
   This->spec_color = (unsigned long *)Xc_malloc("spec col",
						 sizeof(unsigned long));
   get_white=TRUE;
   This->nb_spec_color=1;
  }
  linear = 65535 / (count - count_tmp);
  ln = 0;
  for(i = count_tmp; i < count; i++)
  {
   This->red[pixels[i]] = (unsigned short)ln;
   This->green[pixels[i]] = (unsigned short)ln;
   This->blue[pixels[i]] = (unsigned short)ln;
   This->usable_color[pixels[i]] = TRUE;
   ln+=linear;
  }
  if (count)
  {
   colors = (XColor *)Xc_malloc("colors", sizeof(XColor) * count);
   for(i = 0; i < count; i++)
   {
    colors[i].pixel = pixels[i];
    colors[i].red = This->red[pixels[i]];
    colors[i].green = This->green[pixels[i]];
    colors[i].blue = This->blue[pixels[i]];
    colors[i].flags = DoRed|DoGreen|DoBlue;
   }
   XStoreColors(display, cmap, colors, count);
   memcpy(This->pixels + This->allocated, pixels,
	  sizeof(unsigned long) * count);
   This->allocated += count;
   Xc_free(colors);
  }
  Xc_free(pixels);
    
  get_closest_color(This,&tmpcolorc,0xffff,0x0000,0x0000);
  This->Cred=tmpcolorc.pixel;
  get_closest_color(This,&tmpcolorc,0x0000,0xffff,0x0000);
  This->Cgreen=tmpcolorc.pixel;
  get_closest_color(This,&tmpcolorc,0x0000,0x0000,0xffff);
  This->Cblue=tmpcolorc.pixel;
  get_closest_color(This,&tmpcolorc,0x0000,0xffff,0xffff);
  This->Ccyan=tmpcolorc.pixel;
  get_closest_color(This,&tmpcolorc,0xffff,0xffff,0x0000);
  This->Cyellow=tmpcolorc.pixel;
  get_closest_color(This,&tmpcolorc,0xffff,0x0000,0xffff);
  This->Cmagenta=tmpcolorc.pixel;
  break;
    
 case CMAP_PSEUDOCOLOR:
  pixels = (unsigned long *)Xc_malloc("pixels",
				      sizeof(unsigned long) * nb_color);
  count = This->max_color - This->allocated;
  while(count && !XAllocColorCells(display, cmap, FALSE,
				   NULL, 0, pixels, count))
   count--;
  Xc_TRACE(("Color allocated: %d Color free: %d", nb_color - count, count));
  Xc_TRACE(("allocated: %d max_color: %d count: %d", This->allocated, 
	    This->max_color, count));
    
#ifndef NO_SPEC_COLOR    
#ifdef XOR_COLORS
  if (count > MAX_SPEC_COLOR * 2 && 
      BlackPixel(display, This->screen) == 1)
#else
   if (count > MAX_SPEC_COLOR)
#endif
   {
      
#ifdef XOR_COLORS
    count_tmp = MAX_SPEC_COLOR * 2;
#else
    count_tmp = MAX_SPEC_COLOR;   
#endif
    This->spec_color =
     (unsigned long *)Xc_malloc("spec col",
				sizeof(unsigned long) * MAX_SPEC_COLOR);
    j = k = 0;
    if (pixels[0] & 1)
    {
     This->red[pixels[0]] = 0xf000;
     This->green[pixels[0]] = 0xa000;
     This->blue[pixels[0]] = 0xa000;
     This->usable_color[pixels[0]] = TRUE;
     k = 1;
     count_tmp++;
    }
#ifdef XOR_COLOR
    for(i = k; i < MAX_SPEC_COLOR * 2 + k; i++)
    {
     if (pixels[i] & 1)
     {
      This->red[pixels[i]] = default_spec_color[j][0];
      This->green[pixels[i]] = default_spec_color[j][1];
      This->blue[pixels[i]] = default_spec_color[j][2];
      This->usable_color[pixels[i]] = TRUE;
      This->spec_color[j++] = pixels[i];
     }
     else
     {
      This->red[pixels[i]] = This->green[pixels[i]] =
       This->blue[pixels[i]] = 0;
      This->usable_color[pixels[i]] = TRUE;
     }
    }
#else  /* for ! XOR_COLORS */
    for(i = k; i < MAX_SPEC_COLOR + k; i += 2)
    {
     This->red[pixels[i]] = default_spec_color[j][0];
     This->green[pixels[i]] = default_spec_color[j][1];
     This->blue[pixels[i]] = default_spec_color[j][2];
     This->usable_color[pixels[i]] = TRUE;
     This->spec_color[j++] = pixels[i];
     This->red[pixels[i+1]] = default_spec_color[j][0];
     This->green[pixels[i+1]] = default_spec_color[j][1];
     This->blue[pixels[i+1]] = default_spec_color[j][2];
     This->usable_color[pixels[i+1]] = TRUE;
     This->spec_color[j++] = pixels[i];
    }
#endif
    This->nb_spec_color = MAX_SPEC_COLOR;
   }
   else
   {
    count_tmp = 0;
	
#ifdef XOR_COLORS
    This->spec_color = (unsigned long *)
     Xc_malloc("spec col", sizeof(unsigned long));
    get_white = TRUE;
    This->nb_spec_color = 1;
#else /* ! XOR_COLORS */
    This->spec_color = (unsigned long *)
     Xc_malloc("spec col", sizeof(unsigned long) * MAX_SPEC_COLOR);
    for(i = 0; i < MAX_SPEC_COLOR; i++)
    {
     get_closest_color(This, &tmpcolorc,
		       default_spec_color[i][0],
		       default_spec_color[i][1],
		       default_spec_color[i][2]);
     This->spec_color[i] = tmpcolorc.pixel;
    }
    This->nb_spec_color = MAX_SPEC_COLOR;
#endif
   }
#else  /* NO_SPEC_COLOR */ 
  count_tmp = 0;
  This->spec_color = NULL;
  This->nb_spec_color = 0;
#endif
    
  i = count_tmp;
  count_tmp = count - count_tmp;
  while(count_tmp)
  {
   compo=(int)pow((double)count_tmp,(double)1/3);
   add_compo=((compo==1)?32000:64000/(compo-1));
   inc_r=1535;
   for(for_r=0;for_r<compo;for_r++)
   {
    inc_g=1535;
    for(for_g=0;for_g<compo;for_g++)
    {
     inc_b=1535;
     for(for_b=0;for_b<compo;for_b++)
     {
      This->usable_color[pixels[i]] = TRUE;
      This->red[pixels[i]]=inc_r;
      This->green[pixels[i]]=inc_g;
      This->blue[pixels[i++]]=inc_b;
      inc_b += add_compo;
     }
     inc_g += add_compo;
    } 
    inc_r += add_compo;
   }
   count_tmp -= compo*compo*compo;
  }

  if ( BlackPixel(display, This->screen) == 0)
  {
   get_closest_color(This,&tmpcolorc,0xffff,0xffff,0xffff);
   for(i = 0; i < count; i++)
   {
    if ( pixels[i] == (unsigned long)255 - tmpcolorc.pixel)
    {
     This->red[pixels[i]] = 0;
     This->green[pixels[i]] = 0;
     This->blue[pixels[i]] = 0;
     This->usable_color[pixels[i]] = TRUE;
     break;
    }
   }
  }
    
  if (count)
  {
   colors = (XColor *)Xc_malloc("colors", sizeof(XColor) * count);
   for(i = 0; i < count; i++)
   {
    colors[i].pixel = pixels[i];
    colors[i].red = This->red[pixels[i]];
    colors[i].green = This->green[pixels[i]];
    colors[i].blue = This->blue[pixels[i]];
    colors[i].flags = DoRed | DoGreen | DoBlue;
   }
   XStoreColors(display, cmap, colors, count);
   memcpy(This->pixels + This->allocated, pixels,
	  sizeof(unsigned long) * count);
   This->allocated += count;
   Xc_free(colors);
  }
  Xc_free(pixels);
    
  get_closest_color(This,&tmpcolorc,0xffff,0x0000,0x0000);
  This->Cred=tmpcolorc.pixel;
  get_closest_color(This,&tmpcolorc,0x0000,0xffff,0x0000);
  This->Cgreen=tmpcolorc.pixel;
  get_closest_color(This,&tmpcolorc,0x0000,0x0000,0xffff);
  This->Cblue=tmpcolorc.pixel;
  get_closest_color(This,&tmpcolorc,0x0000,0xffff,0xffff);
  This->Ccyan=tmpcolorc.pixel;
  get_closest_color(This,&tmpcolorc,0xffff,0xffff,0x0000);
  This->Cyellow=tmpcolorc.pixel;
  get_closest_color(This,&tmpcolorc,0xffff,0x0000,0xffff);
  This->Cmagenta=tmpcolorc.pixel;
  break;
    
 case CMAP_DIRECTCOLOR:
 case CMAP_TRUECOLOR:

#ifndef NO_SPEC_COLOR
  This->spec_color = (unsigned long *)Xc_malloc
   ("spec col", sizeof(unsigned long) * MAX_SPEC_COLOR);
  for(i = 0; i < MAX_SPEC_COLOR; i++)
  {
   get_closest_color(This, &tmpcolorc,
		     default_spec_color[i][0],
		     default_spec_color[i][1],
		     default_spec_color[i][2]);
   This->spec_color[i] = tmpcolorc.pixel;
  }
  This->nb_spec_color = MAX_SPEC_COLOR;
#else  /* FOR NO_SPEC_COLOR */
  This->spec_color = NULL;
  This->nb_spec_color = 0;
#endif
    
  get_closest_color(This, &tmpcolorc, 0xffff, 0x0000, 0x0000);
  This->Cred=tmpcolorc.pixel;
  get_closest_color(This, &tmpcolorc, 0x0000, 0xffff, 0x0000);
  This->Cgreen=tmpcolorc.pixel;
  get_closest_color(This, &tmpcolorc, 0x0000, 0x0000, 0xffff);
  This->Cblue=tmpcolorc.pixel;
  get_closest_color(This, &tmpcolorc, 0x0000, 0xffff, 0xffff);
  This->Ccyan=tmpcolorc.pixel;
  get_closest_color(This, &tmpcolorc, 0xffff, 0xffff, 0x0000);
  This->Cyellow=tmpcolorc.pixel;
  get_closest_color(This, &tmpcolorc, 0xffff, 0x0000, 0xffff);
  This->Cmagenta=tmpcolorc.pixel;
  break;
 }
  
 Xc_TRACE(("Black: %d White: %d R: %d G: %d B: %d", This->Cblack,
	   This->Cwhite, This->Cred, This->Cgreen, This->Cblue));
 if (get_white)
 {
  This->spec_color[0] = This->Cwhite;
 }
  
#ifdef HASH_TABLE
 if(!Generate_Hash_Table(This))
 {
  DELETE(c_Colormap)(This);
 }
#endif
  
}

#if 0
/* -------------------------------------------------------------------- **
** --------   'Apply colormap' method of Colormap	       --------	**
** -------------------------------------------------------------------- */

static void apply_Colormap(This, window)
c_Colormap	*This;
Window		window;
{
 XSetWindowColormap(This->display, window, This->cmap);
}
#endif

/* -------------------------------------------------------------------- **
** --------   'Get closest color' method  of Colormap	       --------	**
** -------------------------------------------------------------------- */

static void get_closest_color_static(This,cell,ulR,ulV,ulB)
c_Colormap	*This;
color_cells_t	*cell;
unsigned short	ulR;
unsigned short	ulV;
unsigned short	ulB;
{
 unsigned long	i;
 unsigned long	best_choice;
 unsigned long	closest_color;
 unsigned long	average;
 register unsigned short	usR;
 register unsigned short	usV;
 register unsigned short	usB;
 unsigned short	*R, *G, *B;

 usR = ulR; usV = ulV; usB = ulB;
 R = This->red; G = This->green; B = This->blue;
 best_choice = 0;
 closest_color = 3*0xffff;
 for(i = 0; i < (unsigned long)This->nb_color; i++)
 {
  average = ABS((long)(R[i] - usR)) + ABS((long)(G[i] - usV)) +
   ABS((long)(B[i] - usB));
  if (average < closest_color)
  {
   closest_color = average;
   best_choice = i;
  }
 }
 cell->pixel = best_choice;
 cell->red   = R[best_choice];
 cell->green = G[best_choice];
 cell->blue  = B[best_choice];
}

static void get_closest_color_pseudo(This,cell,ulR,ulV,ulB)
c_Colormap	*This;
color_cells_t	*cell;
unsigned short	ulR;
unsigned short	ulV;
unsigned short	ulB;
{
 unsigned long	i;
 unsigned long	best_choice;
 unsigned long	closest_color;
 unsigned long	average;
 register unsigned short	usR;
 register unsigned short	usV;
 register unsigned short	usB;
 unsigned short	*R, *G, *B;

 usR = ulR; usV = ulV; usB = ulB;
 R = This->red; G = This->green; B = This->blue;
 best_choice = 0;
 closest_color = 3*0xffff;
 for(i = 0; i < (unsigned long)This->nb_color; i++)
 {
  if (This->usable_color[i])
  {
   average = ABS((long)(R[i] - usR)) + ABS((long)(G[i] - usV)) +
    ABS((long)(B[i] - usB));
   if (average < closest_color)
   {
    closest_color = average;
    best_choice = i;
   }
  }
 }
 cell->pixel = best_choice;
 cell->red   = R[best_choice];
 cell->green = G[best_choice];
 cell->blue  = B[best_choice];
}

static void get_closest_color(This, cell, ulR, ulV, ulB)
c_Colormap	*This;
color_cells_t	*cell;
unsigned short	ulR;
unsigned short	ulV;
unsigned short	ulB;
{
 unsigned long	i;
 unsigned long	best_choice;
 unsigned long	closest_color;
 unsigned long	average;
 register unsigned short	usR;
 register unsigned short	usV;
 register unsigned short	usB;
 unsigned short	*R, *G, *B;
 unsigned long		*lR, *lG, *lB;
 unsigned short 	old_col;

 switch(This->type)
 {
 case CMAP_STATICGRAY:
 case CMAP_STATICCOLOR:
  usR = ulR; usV = ulV; usB = ulB;
  R = This->red; G = This->green; B = This->blue;
  best_choice = 0;
  closest_color = 3*0xffff;
  for(i = 0; i < (unsigned long)This->nb_color; i++)
  {
   average = ABS((long)(R[i] - usR)) + ABS((long)(G[i] - usV)) +
    ABS((long)(B[i] - usB));
   if (average < closest_color)
   {
    closest_color = average;
    best_choice = i;
   }
  }
  cell->pixel = best_choice;
  cell->red   = R[best_choice];
  cell->green = G[best_choice];
  cell->blue  = B[best_choice];
  break;
 case CMAP_GRAYSCALE:
 case CMAP_PSEUDOCOLOR:
  usR = ulR; usV = ulV; usB = ulB;
  R = This->red; G = This->green; B = This->blue;
  best_choice = 0;
  closest_color = 3*0xffff;
  for(i = 0; i < (unsigned long)This->nb_color; i++)
  {
   if (This->usable_color[i])
   {
    average = ABS((long)(R[i] - usR)) + ABS((long)(G[i] - usV)) +
     ABS((long)(B[i] - usB));
    if (average < closest_color)
    {
     closest_color = average;
     best_choice = i;
    }
   }
  }
  cell->pixel = best_choice;
  cell->red   = R[best_choice];
  cell->green = G[best_choice];
  cell->blue  = B[best_choice];
  break;
 case CMAP_DIRECTCOLOR:
 case CMAP_TRUECOLOR:
  lR = (unsigned long *)This->red; 
  lG = (unsigned long *)This->green;
  lB = (unsigned long *)This->blue;
  closest_color = 0;
  old_col = ulR;
  for(usR=0; usR < lR[0]; usR++)
  {
   if (ulR & (1<<(sizeof(short)*8-1)) )
    closest_color |= (long)lR[usR+1];
   ulR <<= 1;
  }
  cell->red = old_col - (ulR>>lR[0]);
  old_col = ulV;
  for(usV=0; usV < lG[0]; usV++)
  {
   if (ulV & (1<<(sizeof(short)*8-1)) )
    closest_color |= (long)lG[usV+1];
   ulV <<= 1;
  }
  cell->green = old_col - (ulV>>lG[0]);
  old_col = ulB;
  for(usB=0; usB < lB[0]; usB++)
  {
   if (ulB & (1<<(sizeof(short)*8-1)) )
    closest_color |= (long)lB[usB+1];
   ulB <<= 1;
  }
  cell->blue = old_col - (ulB>>lB[0]);
  cell->pixel = closest_color;
  break;
 }
}

static void get_closest_color_truecolor(This,cell,ulR,ulV,ulB)
c_Colormap	*This;
color_cells_t	*cell;
unsigned short	ulR;
unsigned short	ulV;
unsigned short	ulB;
{
 unsigned long	closest_color;
 register unsigned short	usR;
 register unsigned short	usV;
 register unsigned short	usB;
 unsigned long		*lR, *lG, *lB;
 unsigned short 	old_col;

 lR = (unsigned long *)This->red; 
 lG = (unsigned long *)This->green;
 lB = (unsigned long *)This->blue;
 closest_color = 0;
 old_col = ulR;
 for(usR=0; usR < lR[0]; usR++)
 {
  if (ulR & (1<<(sizeof(short)*8-1)) )
   closest_color |= (long)lR[usR+1];
  ulR <<= 1;
 }
 cell->red = old_col - (ulR>>lR[0]);
 old_col = ulV;
 for(usV=0; usV < lG[0]; usV++)
 {
  if (ulV & (1<<(sizeof(short)*8-1)) )
   closest_color |= (long)lG[usV+1];
  ulV <<= 1;
 }
 cell->green = old_col - (ulV>>lG[0]);
 old_col = ulB;
 for(usB=0; usB < lB[0]; usB++)
 {
  if (ulB & (1<<(sizeof(short)*8-1)) )
   closest_color |= (long)lB[usB+1];
  ulB <<= 1;
 }
 cell->blue = old_col - (ulB>>lB[0]);
 cell->pixel = closest_color;
}

/* -------------------------------------------------------------------- **
** --------    'Get spec color' method of Colormap	       --------	**
** -------------------------------------------------------------------- */

static unsigned long get_spec_color(This, nb_spec)
c_Colormap	*This;
int		nb_spec;
{
 return This->spec_color[nb_spec % This->nb_spec_color];
}

/* -------------------------------------------------------------------- **
** --------	 'use color' method of Colormap		       --------	**
** -------------------------------------------------------------------- */

static void use_color(This, gc, color)
c_Colormap	*This;
GC		gc;
int	color;
{
 unsigned long	real_color;
  
 switch(color)
 {
 case BLACK:
  real_color = This->Cblack;
  break;
 case RED:
  real_color = This->Cred;
  break;
 case GREEN:
  real_color = This->Cgreen;
  break;
 case BLUE:
  real_color = This->Cblue;
  break;
 case CYAN:
  real_color = This->Ccyan;
  break;
 case YELLOW:
  real_color = This->Cyellow;
  break;
 case MAGENTA:
  real_color = This->Cmagenta;
  break;
 case WHITE:
  real_color = This->Cwhite;
  break;
 default:
  real_color = This->Cwhite;
 }
 XSetForeground(This->display, gc, real_color);
}

/* -------------------------------------------------------------------- **
** --------  'Generate Hash Table' method of Colormap	       --------	**
** -------------------------------------------------------------------- */

#ifdef HASH_TABLE
static boolean Generate_Hash_Table(This)
c_Colormap	*This;
{
 int			RedC, GreenC, BlueC, MaxC;/* counters ... */
 unsigned short	CompR, CompB, CompG;
 unsigned char 	*Tab;
 color_cells_t		Cell;

 Xc_HISTORY(("Generate Hash Table ..."));

 if (This->type == CMAP_TRUECOLOR ||
     This->type == CMAP_DIRECTCOLOR)
 {
  Xc_TRACE(("Doesn't need Hash Table"));
  This->hashtable = NULL;
  return TRUE;
 }
  
 This->hashtable = Tab = Xc_malloc("hashtable",
				   (1 << CMAP_HASH_SIGN_BITS) * sizeof(char));
 if(Tab == NULL) return FALSE;
  
 MaxC = 1 << CMAP_HASH_BITS;
 for (RedC = 1; RedC <= MaxC; RedC++) 
 {
  CompR = (RedC << (sizeof(short)*8 - CMAP_HASH_BITS)) - 1;
  for (GreenC = 1; GreenC <= MaxC; GreenC++)
  {
   CompG = (GreenC << (sizeof(short)*8 - CMAP_HASH_BITS)) - 1;
   for (BlueC = 1; BlueC <= MaxC; BlueC++)
   {
    CompB = (BlueC << (sizeof(short)*8 - CMAP_HASH_BITS)) - 1;
    get_closest_color(This, &Cell, CompR, CompG, CompB);
    *Tab++ = (unsigned char) Cell.pixel;
   }
  }
 }

 Xc_HISTORY(("Generate Hash Table ok"));
 return TRUE;
}
#endif

/* ----------------------------------------------------------------- ** 
** Get_In_Hash_Table -- get quickly nearest color                    ** 
** ----------------------------------------------------------------- */
static void Get_In_Hash_Table(colormap, cell, red, green, blue)
c_Colormap	*colormap;
color_cells_t	*cell;
unsigned short 	red;
unsigned short 	green;
unsigned short 	blue;
{
 unsigned char		Num_Color;
 int		 	Indice;

 if (colormap->type == CMAP_TRUECOLOR ||
     colormap->type == CMAP_DIRECTCOLOR)
 {
  get_closest_color(colormap, cell, red, green, blue);
  return;
 }

 Indice = (blue & CMAP_HASH_MASK) >> (sizeof(short)*8 - CMAP_HASH_BITS);
 Indice += (green & CMAP_HASH_MASK) >> (sizeof(short)*8 - 2*CMAP_HASH_BITS);
 Indice += (red & CMAP_HASH_MASK) >> (sizeof(short)*8 - 3*CMAP_HASH_BITS);

 Num_Color = colormap->hashtable[Indice];

 cell->red = colormap->red[Num_Color];
 cell->green = colormap->green[Num_Color];
 cell->blue = colormap->blue[Num_Color];
 cell->pixel = (long) Num_Color;
}

/* ----------------------------------------------------------------- ** 
** Allocated_Named_Color -- allocate or return the closest color     ** 
** ----------------------------------------------------------------- */

static boolean Allocate_Named_Color(This, color_name, cell)
c_Colormap	*This;
char		*color_name;
color_cells_t	*cell;
{
 Display	*display;
 Colormap	cmap;
 XColor	xcolor, xrcolor;
 color_cells_t	rcell;

 display = This->display;
 cmap = This->cmap;
  
 if (!XParseColor(display, cmap, color_name, &xcolor))
 {
  char	*ptr;
  int	len;
  boolean result;

  len = strlen(color_name);
  if (len == 6 || len == 12)
  {
   ptr = (char *)Xc_malloc("cdp", len + 2);
   *ptr = '#';
   memcpy(ptr + 1, color_name, len + 1);

   result = Allocate_Named_Color(This, ptr, cell);
   Xc_free(ptr);
   return result;
  }

  Xc_WARNING(("Color %s not found in RGB database.", color_name));
  if (cell)
  {
   cell->red = cell->green = cell->blue = 0;
   cell->pixel = This->Cblack;
  }
  return FALSE;
 }
  
 memcpy(&xrcolor, &xcolor, sizeof(XColor));
 if (This->max_color <= This->allocated ||  
     !XAllocColor(display, cmap, &xcolor))
 {
  F(This).get_closest_color(This, &rcell, xrcolor.red, xrcolor.green,
			    xrcolor.blue);
  if (cell)
  {
   memcpy(cell, &rcell, sizeof(color_cells_t));
  }
  return TRUE;
 }
 if (This->usable_color && !This->usable_color[xcolor.pixel])
 {
  This->pixels[This->allocated] = xcolor.pixel;
  This->red[xcolor.pixel] = xcolor.red;
  This->green[xcolor.pixel] = xcolor.green;
  This->blue[xcolor.pixel] = xcolor.blue;
  This->usable_color[xcolor.pixel] = TRUE;
  This->allocated++;
 }
  
 cell->pixel = xcolor.pixel;
 cell->red = xcolor.red;
 cell->green = xcolor.green;
 cell->blue = xcolor.blue;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** Get_RGB_Value -- retreive the RGB component of a pixel value      ** 
** ----------------------------------------------------------------- */

static boolean Get_RGB_Values(This, cell)
c_Colormap	*This;
color_cells_t	*cell;
{
 boolean	ret_status = TRUE;
 register unsigned short	usR;
 register unsigned short	usG;
 register unsigned short	usB;
 register unsigned short	Val;
 unsigned long			*lR, *lG, *lB;
 register unsigned long	pixel;
 register int	i;

 switch(This->type)
 {
 case CMAP_STATICGRAY:
 case CMAP_STATICCOLOR:
  if (cell->pixel >= (unsigned long)This->nb_color)
   ret_status = FALSE;
  else
  {
   cell->red = This->red[cell->pixel];
   cell->green = This->green[cell->pixel];
   cell->blue = This->blue[cell->pixel];
  }
  break;
 case CMAP_GRAYSCALE:
 case CMAP_PSEUDOCOLOR:
  if (cell->pixel >= (unsigned long)This->nb_color || 
      !This->usable_color[cell->pixel])
   ret_status = FALSE;
  else
  {
   cell->red = This->red[cell->pixel];
   cell->green = This->green[cell->pixel];
   cell->blue = This->blue[cell->pixel];
  }
  break;
 case CMAP_DIRECTCOLOR:
 case CMAP_TRUECOLOR:
  lR = (unsigned long *)This->red; 
  lG = (unsigned long *)This->green;
  lB = (unsigned long *)This->blue;
  pixel = cell->pixel;

  i = (1<<(sizeof(short)*8-1));
  for(usR = Val = 0; usR < lR[0]; usR++)
  {
   if (pixel & (long)lR[usR+1])
    Val |= i;
   i >>= 1;
  }
  cell->red = Val;
      
  i = (1<<(sizeof(short)*8-1));
  for(usG = Val = 0; usG < lG[0]; usG++)
  {
   if (pixel & (long)lG[usG+1])
    Val |= i;
   i >>= 1;
  }
  cell->green = Val;
      
  i = (1<<(sizeof(short)*8-1));
  for(usB = Val = 0; usB < lB[0]; usB++)
  {
   if (pixel & (long)lB[usB+1])
    Val |= i;
   i >>= 1;
  }
  cell->blue = Val;
  break;
 }
 return ret_status;
}

/* -------------------------------------------------------------------- **
** callbacks and internal private functions				**
** -------------------------------------------------------------------- */

#ifndef NO_INTRINSIC
static Boolean xt_convert_string_to_color(display, args, nargs,
					  source, target, ptr)
Display	*display;
XrmValue *args;
int *nargs;
XrmValue *source;
XrmValue *target;
XtPointer *ptr;
{ 
 c_Colormap	*This;
 color_cells_t realcolor;
  
 if (*nargs == 1)
  This = (c_Colormap *)args->addr;
 else
  return False;
  
 if (source->addr)
 {
  
  if (target->size < sizeof(Pixel))
  {
   target->size = sizeof(Pixel);
   return False;
  }
  
  if (Xstrcmp(source->addr, XtDefaultBackground))
  {
   *(Pixel*)target->addr = This->Cwhite;
    /*WhitePixel(display, DefaultScreen(display));*/
   return True;
  }
    
  if (Xstrcmp(source->addr, XtDefaultForeground))
  {
   *(Pixel*)target->addr = This->Cblack;
   /*BlackPixel(display, DefaultScreen(display));*/
   return True;
  }
    
  if (This->display != display)
   This->display = display;

  F(This).allocate_named_color(This, source->addr, &realcolor);
    
  if (target->size < sizeof(Pixel))
  {
   target->size = sizeof(Pixel);
   return False;
  }
  *(Pixel*)target->addr = realcolor.pixel;
 }
 else
 {
  XtStringConversionWarning("Null", "Pixel");
  return False;
 }
 return True;
}

#endif
