/*
** encodings.c for Xclamation, XQuad, XAllWrite and XMayday in Global/
** Header for encodings convertion
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
** Started on  Wed Feb 22 16:59:27 1995 Emmanuel Paris
** Last update Sun May 11 21:34:19 1997 Antoine Buat
*/

#include "xcalibur.h"
#include "encodings.h"
#include "isolat1_encoding.h"
#include "dos_encoding.h"
#include "mac_encoding.h"
#include "html_encoding.h"

/* convert from encoding to iso latin 1 */
/* !! Result directly in input string. Original string is overwritted !! */

void convert_encoding(string, encoding)
char		*string;
encoding_t	encoding;
{
 int	ptr = 0;
  
 switch(encoding)
 {
 case XcE_LATIN1:
  while(string[ptr])
  {
   string[ptr] = isolat1_encoding[(unsigned char)string[ptr]];
   ptr++ ;
  }
  break;
 case XcE_DOS:
  while(string[ptr])
  {
   string[ptr] = dos_encoding[(unsigned char)string[ptr]];
   ptr++ ;
  }
  break;
 case XcE_MAC:
  while(string[ptr])
  {
   string[ptr] = mac_encoding[(unsigned char)string[ptr]];
   ptr++ ;
  }
 case XcE_HTML:
  break;
 }
}

/* convert from iso latin 1 to encoding */
/* !! For encoding == XcE_LATIN1, XcE_DOS and XcE_MAC : */
/* !! result directly in input string. Original string is overwritted !! */
/* !! For encoding == XcE_HTML : */
/* !! Result in return string. Original string is not modified. */
/* !! Return string must be freed !! */

char *convert_to_encoding(string, encoding)
char		*string;
encoding_t	encoding;
{
 int	i, j;
 char	*str, *ptr;
 int	lng, olng, plng;
  
 switch(encoding)
 {
 case XcE_LATIN1:
  str = string;
  break;
 case XcE_DOS:
  i = 0;
  while(string[i])
  {
   string[i] = to_dos_encoding[(unsigned char)string[i]];
   i++ ;
  }
  str = string;
  break;
 case XcE_MAC:
  i = 0;
  while(string[i])
  {
   string[i] = to_mac_encoding[(unsigned char)string[i]];
   i++ ;
  }
  str = string;
  break;
 case XcE_HTML:
  str = Xc_strdup("html", string);
  olng = strlen(string);
  lng = olng + 1;
  j = i = 0;
    
  while(i < olng)
  {
   ptr = to_html_encoding[(unsigned char)string[i]];
      
   if ((plng = strlen(ptr)) != 0)
   {
    if (j + plng >= lng - 1)
    {
     lng += 4000;
     str = Xc_realloc("str", str, lng);
    }
    memcpy(str + j, ptr, plng);
    j += plng;
   }
   i++;
  }
  str[j] = '\0';
    
  plng = strlen(str) + 1;
  if (plng != lng)
   str = Xc_realloc("str", str, plng);
  break;
 default:
  return NULL;    
 }
 return str;
}





