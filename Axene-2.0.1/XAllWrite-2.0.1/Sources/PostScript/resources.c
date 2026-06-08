/*
** resources.c for Xclamation, XQuad and XAllWrite in PostScript/
** Resource methods for PostScript calss
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
** Started on  Wed Mar 29 16:07:22 1995 Stéphane Boisson
** Last update Fri Jan  7 15:01:54 2000 Emmanuel Paris
*/

#define NTRACE

#include <varargs.h>
#include "PostScriptP.h"

#define BUFFER_SIZE 4096

/*--- Prototypes ---*/
static boolean includeFont ___PROTO((c_PostScript *this, c_VectorFont *font));


/* ----------------------------------------------------------------- ** 
** includeResource - Include resource in output file                 ** 
** ----------------------------------------------------------------- */
boolean ps_includeResource(this, type, va_alist)
c_PostScript *this;
ps_resource_t type;
va_dcl
{
 boolean rc = FALSE;
 va_list ap;

 Xc_HISTORY(("includeResource"));

 va_start(ap);
 switch(type)
 {
  /*--- Font dictionary ---*/
 case XcPS_FONT_RESOURCE:
  rc = includeFont(this, va_arg(ap, c_VectorFont *));
  break;

  /*--- Encoding vector (array) ---*/
 case XcPS_ENCODING_RESOURCE:
  rc = FALSE;
  break;

  /*--- Form definition (dictionary) ---*/
 case XcPS_FORM_RESOURCE:
  rc = FALSE;
  break;

  /*--- Pattern definition (dictionary) ---*/
 case XcPS_PATTERN_RESOURCE:
  rc = FALSE;
  break;

  /*--- Procedure set (dictionary) ---*/
 case XcPS_PROC_SET_RESOURCE:
  rc = FALSE;
  break;

  /*--- Parameterized color space (array) ---*/
 case XcPS_COLOR_SPACE_RESOURCE:
  rc = FALSE;
  break;

  /*--- Halftone dictionary ---*/
 case XcPS_HALFTONE_RESOURCE:
  rc = FALSE;
  break;

  /*--- Color rendering dictionary ---*/
 case XcPS_COLOR_RENDERING_RESOURCE:
  rc = FALSE;
  break;
 }

 va_end(ap);

 Xc_TRACE(("done (%s)", rc? "OK":"Error"));
 return rc;
}


/* ----------------------------------------------------------------- ** 
** includeFont - Include Postscript font                             ** 
** ----------------------------------------------------------------- */
static boolean includeFont(this, font)
c_PostScript *this;
c_VectorFont *font;
{
 unsigned char buffer[BUFFER_SIZE];
 boolean flag;
 FILE *fontfd;
 size_t rc;

 Xc_HISTORY(("includeFont(%s)", font->font_name));

 F(this).putLine(this, "%%BeginFont: $s", font->font_name); 

 if((fontfd = fopen(font->filename, "rb")) == NULL)
 {
  Xc_SYSERROR(("Font file %s", font->filename));
  this->error_flag = TRUE;
  return FALSE;
 }

 if((rc = fread(buffer, 1, 6, fontfd)) < 2)
 {
  this->error_flag = TRUE;
  return FALSE;
 }

 if((*buffer == '%') && (buffer[1] == '!'))
 {
  /*--- Assume ascii format ---*/
  Xc_TRACE(("Assume PFA format"));
  fclose(fontfd);
  return(F(this).includeFile(this, font->filename) &&
	 F(this).putLine(this, "%%EndFont")); 
 }

 /*--- Assume PFB format ---*/
 Xc_TRACE(("Assume PFB format"));
 flag = FALSE;
 while(42)
 {
  unsigned char type;
  size_t len;

  if(*buffer != 128) break;
  if((type = buffer[1]) != 3)
  {
   if(rc != 6) break;
   len = buffer[5];
   len <<= 8;
   len |= buffer[4]; 
   len <<= 8;
   len |= buffer[3]; 
   len <<= 8;
   len |= buffer[2]; 
  }
  else
   len = 0;

  Xc_TRACE(("Chunk type %d (size=%ld)", type, len));

  if((type == 1) || (type == 2))
  {
   size_t nread, i;
   ps_image_t filter;

   /*--- Setup filter ---*/
   filter.output_index = 0;
   filter.line_buffer = buffer;
   filter.this = this;

   for(nread = 0; nread < len; )
   {
    rc = MIN(BUFFER_SIZE, len - nread);
    if((rc = fread(buffer, 1, rc, fontfd)) <= 0) break;
    nread += rc;
    if(type == 1)
    {
     register char *ptr;

     for(i = rc, ptr = (char *)buffer; i--; ptr++)
      if(*ptr == '\r') *ptr = '\n';
     if(!F(this).write(this, buffer, rc)) break;
    }
    else
    {
     ps_encode_ASCIIHex(&filter, rc);
     if(nread == len)
     {
      *(filter.output_buffer + filter.output_index) = 0;
      if(!F(this).putLine(this, "$s", filter.output_buffer))
       break;
     }
    }
   }
  }
  else
  {
   flag = (type == 3)? TRUE : FALSE;
   break;
  }

  if((rc = fread(buffer, 1, 6, fontfd)) < 2) break;
 }
      
 fclose(fontfd);
 if(flag == FALSE)
 {
  Xc_TRACE(("error"));
  this->error_flag = TRUE;
 }
 return(flag && F(this).putLine(this, "%%EndFont"));
}
