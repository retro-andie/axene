/*
** filters.c for Xclamation, XQuad and XAllWrite in PostScript/
** Filter methods for PostScript class
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
** Started on  Wed Feb 22 18:08:09 1995 Stéphane Boisson
** Last update Wed May 31 14:08:04 1995 Stéphane Boisson
*/

#define NTRACE

#include "PostScriptP.h"


/* ----------------------------------------------------------------- ** 
** encode_ASCIIHex - ASCII Hex PDF encoding method                   ** 
** ----------------------------------------------------------------- */
boolean ps_encode_ASCIIHex(handle, len)
ps_image_t *handle;
int len;
{
  register unsigned char *source, *target, *end;
  static char *hex_table = "0123456789ABCDEF";
  register boolean rc;

  Xc_TRACE(("encode_ASCIIHex"));

  source = handle->line_buffer;
  target = handle->output_buffer + handle->output_index;
  end = handle->output_buffer + (XcPS_BYTES_PER_LINE * 2);
  rc = TRUE;

  while(len > 0)
    {
      while((target < end) && (len-- > 0) && (rc = TRUE))
	{
	  register unsigned char c;
	  
	  c = *source++;
	  *target++ = hex_table[c >> 4];
	  *target++ = hex_table[c & 0xf];
	}

      if(len > 0)
	{
	  *target = 0;
	  target = handle->output_buffer;
	  rc = F(handle->this).putLine(handle->this, "$s",
				       handle->output_buffer); 
	}
    }
  handle->output_index = target - handle->output_buffer;

  return rc;
}
