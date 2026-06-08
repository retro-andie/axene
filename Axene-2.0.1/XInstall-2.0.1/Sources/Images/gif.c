/*
** gif.c for XInstall in Images/
** GIF loading functions. LZW decoder.
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
** Started on  Tue Nov  4 19:47:22 1994 Emmanuel Paris
** Last update Thu Jan  6 18:03:27 2000 Emmanuel Paris
*/

#include "gif.h"

static boolean 	Test();
static error	Get_Header();
static error 	Load_Image();

static unsigned char *getSubBlocks ___PROTO((Xc_FILE *file));
static boolean skipSubBlocks ___PROTO((Xc_FILE *file));
static char *depackLZW ___PROTO((Xc_FILE *file, int width, int height));
static void bitfieldInit ___PROTO((Xc_FILE *file, bitfield_t *bitfield));
static boolean bitfieldRefill ___PROTO((bitfield_t *bitfield));

static 	char	*Extensions_Tab[] = { Xc_GIF_EXT1 };

image_load_t	Load_Gif =
{
 Xc_GIF_NAME,
 Xc_GIF_COPYRIGHT,
 "GIF",
 Xc_GIF_EXTC,
 Extensions_Tab,
 Test,
 Get_Header,
 Load_Image
};

static char	*GIF_ID87 = "GIF87a";
static char	*GIF_ID89 = "GIF89a";
static int	interlace_inc[8] = { 0, 2, 1, 2, 0, 2, 1, 2};

/*-------------------------------------------------------------------------*/
static boolean	Test(name)
char	*name;
{
 Xc_FILE		*file;
 char		id[7];
  
 Xc_HISTORY(("Testing Gif file : '%s'",name));

 if (!(file = Xc_fopen(name, "rb")))
 {
  Xc_WARNING(("Can't open file %s", name));
  return FALSE;
 }
  
 if (Xc_fread(id, sizeof(char), 6, file) != 6)
 {
  Xc_WARNING(("Error while reading file %s",name));
  Xc_fclose(file);
  return FALSE;
 }
  
 Xc_TRACE(("testing gif87a or gif89a format"));
 if (!Xstrncmp(id, GIF_ID87, 6) && !Xstrncmp(id, GIF_ID89, 6))
 {
  Xc_TRACE(("file %s is not a gif file", name));
  Xc_fclose(file);
  return FALSE;
 }
 Xc_fclose(file);
 return TRUE;
}

/*-----------------------------------------------------------------------*/
static error Load_Image(image, line_buffer , callback_func, callback_data)
c_Image		*image;
unsigned char	*line_buffer;
boolean		(*callback_func)  ___PROTO((unsigned char **pbuf, void *data));
void		*callback_data;
{
 Xc_FILE	*file;
 unsigned char buffer[1024], *ptr;
 unsigned int width, height;
 int filesize, depth, colors;
 boolean gotimage = FALSE;
 boolean interlaced = FALSE;
 char	*picture = NULL;
  
 if (!(file = Xc_fopen(image->filename, "rb")))
 {
  Xc_WARNING(("Can't open file %s", image->filename));
  return XC_FILE_NOT_EXIST;
 }
 Xc_fseek(file, 0L, SEEK_END);
 filesize = Xc_ftell(file);
 Xc_fseek(file, 0L, SEEK_SET);
  
 if (Xc_fread( buffer, 13, 1, file) != 1)
 {
  Xc_WARNING(("Error while reading file %s", image->filename));
  Xc_fclose( file);
  return XC_FILE_TRUNCATED;
 }

 while(1)
 {
  /*--- Logical Screen Descriptot Block ---*/
  width = buffer[6] | ((int)buffer[7] << 8);
  height = buffer[8] | ((int)buffer[9] << 8);
  depth = (buffer[10] & 7) + 1;
  colors = 1 << depth;
    
  if(buffer[10] & 0x80)
  {
   /*--- Global Color Map Block ---*/
   Xc_TRACE(("Global Color Map"));
   Xc_fseek(file, colors * 3, SEEK_CUR);
  }
    
  /*--- Extensions ---*/
  *buffer = 0;
    
  while(Xc_fread(buffer, 1, 1, file) == 1)
   if(*buffer == ';')
   {
    Xc_TRACE(("GIF terminator"));
	
   gif_end:
    if (interlaced)
    {
     unsigned char	*buffer[8], *gbuffer;
     int	i, j, width;
	  
	  
     width = image->byte_per_line; 
	  
     gbuffer = (unsigned char *)picture;
     buffer[0] = gbuffer;
     for(j = 0; j < image->height; j += 8)
      gbuffer += width;
     buffer[4] = gbuffer;
     for(j = 4; j < image->height; j += 8)
      gbuffer += width;
     buffer[2] = gbuffer;
     buffer[6] = buffer[2] + width;
     for(j = 2; j < image->height; j += 4)
      gbuffer += width;
     buffer[1] = gbuffer;
     buffer[3] = buffer[1] + width;
     buffer[5] = buffer[3] + width;
     buffer[7] = buffer[5] + width;
	  
     for(i = 0; i < image->height; i++)
     {
      j = i & 7;
      memcpy((void *)line_buffer, buffer[j], image->byte_per_line);
      if (!(callback_func)(&line_buffer, callback_data))
      {
       Xc_WARNING(("Error while dithering"));
       Xc_free(picture);
       Xc_fclose(file);
       return XC_FILE_FORMAT_ERROR;
      }
      buffer[j] += width << interlace_inc[j];
     }
    }
    else
    {
     unsigned char	*buffer;
     int	i, width;
	  
     buffer = (unsigned char *)picture;
     width = image->byte_per_line;
     for(i = 0; i < image->height; i++)
     {
      memcpy((void *)line_buffer, buffer, image->byte_per_line);
      if (!(callback_func)(&line_buffer, callback_data))
      {
       Xc_WARNING(("Error while dithering"));
       Xc_free(picture);
       Xc_fclose(file);
       return XC_FILE_FORMAT_ERROR;
      }
      buffer += width;
     }
    }
    Xc_free(picture);
    Xc_fclose(file);
	
    Xc_mcheck();
    if (gotimage)
     return XC_NO_ERROR;
    else
     return XC_FILE_FORMAT_ERROR;
   }
   else if(*buffer == '!')
   {
    unsigned char type;
	
    /*--- Extension Block ---*/
    if((Xc_fread(&type, 1, 1, file) != 1) ||
       ((ptr = getSubBlocks(file)) == NULL)) break;
	
    switch(type)
    {
     /*--- Graphic Control Extension Block ---*/
    case 0xF9:
     Xc_TRACE(("Graphic Control Extension Block"));
     break;
	  
     /*--- Comment Extension Block ---*/
    case 0xFE:
     Xc_TRACE(("Comment Extension Block: `%s'", ptr));
     break;
	  
     /*--- Plain Text Extension Block ---*/
    case 0x01:
     Xc_TRACE(("Plain Text Extension Block"));
     break;
	  
     /*--- Application Extension Block ---*/
    case 0xFF:
     ptr[8] = 0;
     Xc_TRACE(("Application Extension Block (%s)", ptr));
     break;
	  
    default:
     Xc_TRACE(("Unknown extension type 0x%2x", type));
     break;
    }
    Xc_free(ptr);
   }
   else if(*buffer == ',')
   {
    unsigned int lwidth, lheight, lx, ly;
    int ldepth, lcolors;
    char	*picture_tmp;
	
    /*--- Image Descriptor Block ---*/
    if (Xc_fread(buffer, 9, 1, file) != 1) break;
    lx = buffer[0] | ((int)buffer[1] << 8);
    ly = buffer[2] | ((int)buffer[3] << 8);
    lwidth = buffer[4] | ((int)buffer[5] << 8);
    lheight = buffer[6] | ((int)buffer[7] << 8);
    ldepth = (buffer[8] & 0x80)? (buffer[8] & 7) + 1 : depth;
    lcolors = 1 << ldepth;
	
    if(buffer[8] & 0x80)
    {
     /*--- Local Color Map Block ---*/
     Xc_TRACE(("Local Color Map"));
     Xc_fseek(file, lcolors * 3, SEEK_CUR);
     if (Xc_feof(file))
      break;
    }
	
    /*--- LZW data ---*/
    if (gotimage)
    {
#ifndef NTRACE
     if(buffer[8] & 0x40) 
      Xc_TRACE((" Interlaced"));
#endif
     picture_tmp = depackLZW(file, lwidth, lheight);
    }
    else
    {
     if(buffer[8] & 0x40) 
     {
      Xc_TRACE((" Interlaced"));
      interlaced = TRUE;
     }
     picture = depackLZW(file, lwidth, lheight);
     gotimage = TRUE;
    }
   }
   else
   {
    Xc_TRACE(("Unexpected char 0x%2x", *buffer));
    break;
   }
  break;
 }
 if (gotimage)
 {
  Xc_TRACE(("Gif Terminator is missing."));
  goto gif_end;
 }
 
 Xc_fclose(file);
 return XC_FILE_FORMAT_ERROR;
}


/*----------------------------------------------------------------------*/
static error	Get_Header(Image)
c_Image		*Image;
{
 Xc_FILE	*file;
 int		filesize, hascolormap, 	colormapsize, bitmask;
 boolean	end;
 int		i;
 int		gray;
 unsigned char	*datainit;
 unsigned char	*data;
 int		nb_color;
 double	gscale;

 Xc_HISTORY(("Get Gif Image Header..."));

 if (!(file = Xc_fopen(Image->filename, "rb")))
 {
  Xc_WARNING(("Can't open file %s", Image->filename));
  return XC_FILE_NOT_EXIST;
 }
 Xc_fseek(file, 0L, SEEK_END);
 filesize = Xc_ftell(file);
 Xc_fseek(file, 0L, SEEK_SET);
  
 if (!(data = datainit = (unsigned char *)
       Xc_malloc("gifdat", 
		 sizeof(char) * MIN(filesize, Xc_GIF_HEADER_LENGTH))))
 {
  Xc_fclose(file);
  return XC_MEMORY_ALLOCATION;
 }

 Xc_TRACE(("GIF file size = %d",filesize));

 if ( Xc_fread(data, sizeof(char) * MIN(filesize, Xc_GIF_HEADER_LENGTH),
	    1, file) != 1)
 {
  Xc_WARNING(("Error while reading file %s",Image->filename));
  Xc_fclose(file);
  Xc_free( datainit);
  return XC_FILE_TRUNCATED;
 }
 Xc_fclose(file);
 Image->pal_r = (unsigned short *)
  Xc_malloc("pal_r", sizeof(unsigned short) * 256);
 Image->pal_v = (unsigned short *)
  Xc_malloc("pal_v", sizeof(unsigned short) * 256);
 Image->pal_b = (unsigned short *)
  Xc_malloc("pal_b", sizeof(unsigned short) * 256);

 /* lecture de l'en-tete global */
 data += 10;			/* caracteristiques (w,h) de l'image virtuelle: ne sert pas */
 hascolormap = ( (*data & COLORMAP_MASK) ? TRUE : FALSE);
 colormapsize = 1 << ((*data & DEPTH_MASK) + 1);
 bitmask = colormapsize - 1;
 data += 3;	
 /* couleur de fond et "aspect ratio" de l'image virtuelle: ne sert pas */
  
 /* Lecture de la table de couleurs si celle-ci existe */
 nb_color = colormapsize;
 if ( hascolormap)
 {
  Xc_TRACE(("Get colormap ..."));
      
  for ( i = 0; i < colormapsize; i++)
  {
   Image->pal_r[i] = *(data++) << 8;
   Image->pal_v[i] = *(data++) << 8;
   Image->pal_b[i] = *(data++) << 8;
  }
 }
 else
 {
  gray = 0;
  gscale = 0xffff / nb_color;
  for(i = 0; i < nb_color; i++)
  {
   gray += gscale;
   Image->pal_r[i] = Image->pal_v[i] = Image->pal_b[i] = gray;
  }
 }

 /* Lecture des donnees */
 end = FALSE;
 while (!end)
 {
  switch (*data)
  {
  case EXTENSION:		/* bloc EXTENSION: ignorer */
   {
    int	size;
	    
    Xc_TRACE(("Extension bloc in Gif file"));
    data +=2;			/* type d'info du bloc EXTENSION */
    size = *(data++);
    while ( size)
    {
     data += size;
     if ( data - datainit < filesize)
      size = *(data++);
     else
     {
      size = 0;
      end = TRUE;
     }
    }
   }
   break;
  case IMAGE_SEP:		/* bloc donnees de l'image */
   {
    data += 5;
	    
    Image->width =  *(data++);
    Image->width  |= *(data++) << 8;
    Image->height =  *(data++);
    Image->height |= *(data++) << 8;
    Image->depth  = 8;
    if (*data & COLORMAP_MASK)
    {
     colormapsize = 1 << ((*data & DEPTH_MASK) + 1);
     bitmask = colormapsize - 1;
     data++;
     nb_color = colormapsize;
     Xc_TRACE(("Get colormap..."));
     for (i = 0; i < colormapsize; i++)
     {
      Image->pal_r[i] = *(data++) << 8;
      Image->pal_v[i] = *(data++) << 8;
      Image->pal_b[i] = *(data++) << 8;
     }
    }
    else 
    {
     data++;
     if ( !hascolormap)
     {
      nb_color = 256;
      Xc_TRACE(("Get colormap..."));
      for (i = 0; i < 256; i++)
      {
       Image->pal_r[i] = i << 8;
       Image->pal_v[i] = i << 8;
       Image->pal_b[i] = i << 8;
      }
     }
    }
    end = TRUE;
   }
   end = TRUE;
   break;
  case TRAILER:			/* fin du fichier: l'image n'est pas lue */
  default:			/* bloc inconnu */
   end = TRUE;
   break;
  }
  if (end != TRUE)
   if ( data - datainit >= filesize)
    end = TRUE;
 }
 Xc_free( datainit);
  
 return XC_NO_ERROR;
}


/* ----------------------------------------------------------------- ** 
** getSubBlocks - retreive data blocks                               ** 
** ----------------------------------------------------------------- */
static unsigned char *getSubBlocks(file)
Xc_FILE *file;
{
 unsigned char *old, *new, len;
 long total;

 /*fprintf(stderr, "  getSubBlocks\n");*/

 old = NULL;
 total = 0;

 while(Xc_fread(&len, 1, 1, file) == 1)
 {
  if(!len)
   return old;
  if((new = Xc_realloc("old", old, total + len)) == NULL)
   break;
  old = new;
  if(Xc_fread(old + total, (long)len, 1, file) != 1)
   break;
  total += len;
 }
  
 if(old != NULL)
  Xc_free(old);
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** skipSubBlocks - Skip data blocks                                  ** 
** ----------------------------------------------------------------- */
static boolean skipSubBlocks(file)
Xc_FILE *file;
{
 unsigned char len;
 long pos;
  
 pos = Xc_fseek(file, 0, SEEK_CUR);
 while(Xc_fread(&len, 1, 1, file) == 1)
 {
  if(!len)
   return TRUE;
    
  pos += len;
  if(Xc_fseek(file, (long)len, SEEK_CUR) != pos)
   break;
 }
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** depackLZW - Depack LZW data                                       ** 
** ----------------------------------------------------------------- */
char *depackLZW(file, width, height)
Xc_FILE *file;
int width;
int height;
{
 unsigned char input_codesize, **lzw_ptr, **lzw_newptr, *picture;
 register int codesize, codemask, count, len = 0;
 int clearcode, *lzw_len, *lzw_newlen;
 register unsigned char *output;
 register unsigned long code;
 bitfield_t bitfield;

 Xc_HISTORY(("depackLZW"));

 /*--- Get initial code size ---*/
 if (Xc_fread(&input_codesize, 1, 1, file) != 1)
  return 0;
  
 if (input_codesize < 2 || input_codesize >= MAX_LZW_BITS)
 {
  Xc_WARNING(("Invalid initial code size"));
  return NULL;
 }

 /*--- Alloc output ---*/
 if((picture = Xc_malloc("pGif", (width * height + 4096) 
			 * sizeof(unsigned char))) == NULL)
 {
  skipSubBlocks(file);
  return NULL;
 }
 output = picture;
  
 /*--- Init LZW ---*/
 clearcode = 1 << input_codesize++;
  
 if ((lzw_ptr = Xc_malloc("lzw", LZW_DEPACK_WORKSIZE * 
			  (sizeof(void *) + sizeof(int)))) == NULL)
 {
  skipSubBlocks(file);
  Xc_free(picture);
  return NULL;
 }
  
 lzw_len = (int *)(lzw_ptr + LZW_DEPACK_WORKSIZE);
 memset(lzw_ptr, 0, LZW_DEPACK_WORKSIZE * sizeof(void *));
 memset(lzw_len, 0, LZW_DEPACK_WORKSIZE * sizeof(int));
 memset(lzw_len, -1, clearcode * sizeof(int));
  
 /*for(count = clearcode; count--; lzw_len[count] = count - 0x100);*/
 lzw_len[clearcode] = -0x300;
 lzw_len[clearcode + 1] = -0x200;

 bitfieldInit(file, &bitfield);

 /*--- Depack loop ---*/
 do
 {
  /*--- Clear code ---*/
  codesize = input_codesize;
  count = clearcode - 1;
  lzw_newlen = lzw_len + clearcode + 2;
  lzw_newptr = lzw_ptr + clearcode + 2;
    
  /*--- Build dict ---*/
  do
  {
   codemask = (1 << codesize) - 1;
      
   while(count--)
   {
    bitfieldGetCode(bitfield, code, codesize, clearcode + 1);
    code &= codemask;
    if((len = lzw_len[code]) >= 0)
    {
     register unsigned char *ptr;
	  
     /*--- Add one-byte entry ---*/
     *lzw_newlen++ = ++len;
     *lzw_newptr++ = output;
	  
     for(ptr = lzw_ptr[code]; len--; *output++ = *ptr++);
    }
    else
    {
     /*--- Add one-byte entry ---*/
     *lzw_newlen++ = 1;
     *lzw_newptr++ = output;
     *output++ = (unsigned char)code;
     if(len <= -0x200)
     {
      codesize = MAX_LZW_BITS;
      break;
     }  
    }
   }
      
   count = 1 << codesize;
  } 
  while(codesize++ < MAX_LZW_BITS);
    
  codesize = MAX_LZW_BITS;
    
  /*--- Dict full ---*/
  while(len > -0x200)
  {
   bitfieldGetCode(bitfield, code, codesize, clearcode + 1);
   code &= codemask;
   if((len = lzw_len[code]) > 0)
   {
    register unsigned char *ptr;
	
    for(ptr = lzw_ptr[code]; len--; *output++ = *ptr++);
    continue;
   }
      
   *output++ = (unsigned char)code;
  }
    
  --output;
 }
 while(len < -0x200);
  
 Xc_free(lzw_ptr);

 if(bitfield.error == FALSE)
  skipSubBlocks(file);
 else
 {
  Xc_free(picture);
  return NULL;
 }
  
 Xc_TRACE(("  End LZW (expected=%ld done=%ld)",
	   (long)width * height, (long)(output - picture)));
 return (char *)picture;
}


/* ----------------------------------------------------------------- ** 
** bitfieldInit - Initialize bitfield                                ** 
** ----------------------------------------------------------------- */
static void bitfieldInit(file, bitfield)
Xc_FILE *file;
bitfield_t *bitfield;
{
 bitfield->size = 2;
 bitfield->end = 0;
 bitfield->pos = 0;
 bitfield->error = FALSE;
 bitfield->file = file;
 memset(bitfield->data, 0, 260);
} 

/* ----------------------------------------------------------------- ** 
** bitfieldRefill - Read next block in bitfield buffer               ** 
** ----------------------------------------------------------------- */
static boolean bitfieldRefill(bitfield)
bitfield_t *bitfield;
{
 unsigned char block_len;
  
 /*--- Reload the buffer ---*/
 if(bitfield->error)
 {
  Xc_WARNING(("Unexpected end of LZW data"));
  return FALSE;
 }
  
 /*--- Preserve last two bytes ---*/
 bitfield->data[0] = bitfield->data[bitfield->size - 2];
 bitfield->data[1] = bitfield->data[bitfield->size - 1];
  
 if (Xc_fread(&block_len, 1, 1, bitfield->file) != 1 || (block_len == 0) ||
     Xc_fread(bitfield->data + 2, (int)block_len, 1, bitfield->file) != 1)
 {	 
  bitfield->error = TRUE;
  Xc_WARNING(("Unexpected end of LZW data"));
  return FALSE;
 }
     
 /*--- Reset counters ---*/
 bitfield->pos = (bitfield->pos - bitfield->end) + 16;
 bitfield->size = 2 + block_len;
 bitfield->end = bitfield->size * 8;
  
 return TRUE;
}










