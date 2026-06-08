/*
** text.c for Xclamation in AIDocFile/
** Text operators for Adobe Illustrator loading
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
** Started on  Tue Jan 30 13:49:24 1996 Stéphane Boisson
** Last update Thu Oct 24 18:06:25 1996 Emmanuel Paris
*/

#include "AIDocFileP.h"

extern c_VectorFont *GlobFontBase;

#if 0
static char_t StandardEncoding[256] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  32, 33, 34, 35, 36, 37, 38, 39,
  40, 41, 42, 43, 44, 45, 46, 47,
  48, 49, 50, 51, 52, 53, 54, 55,
  56, 57, 58, 59, 60, 61, 62, 63,
  64, 65, 66, 67, 68, 69, 70, 71,
  72, 73, 74, 75, 76, 77, 78, 79,
  80, 81, 82, 83, 84, 85, 86, 87,
  88, 89, 90, 91, 92, 93, 94, 95,
  96, 97, 98, 99, 100, 101, 102, 103,
  104, 105, 106, 107, 108, 109, 110, 111,
  112, 113, 114, 115, 116, 117, 118, 119,
  120, 121, 122, 123, 124, 125, 126, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 161, 162, 163, 164, 165, 166, 167,
  168, 169, 170, 171, 172, 173, 174, 175,
  0, 177, 178, 179, 180, 0, 182, 183,
  184, 185, 186, 187, 188, 189, 0, 191,
  0, 193, 194, 195, 196, 197, 198, 199,
  200, 0, 202, 203, 0, 205, 206, 207,
  208, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 225, 0, 227, 0, 0, 0, 0,
  232, 233, 234, 235, 0, 0, 0, 0,
  0, 241, 0, 0, 0, 245, 0, 0,
  248, 249, 250, 251, 0, 0, 0, 0
};
#endif

/* ----------------------------------------------------------------- ** 
** aiEncodeFont - Add a font record                                  ** 
** ----------------------------------------------------------------- */
ai_font_record_t *aiEncodeFont(this, old, new, native_flag)
c_AIDocFile *this;
char *old;
char *new;
int native_flag;
{
  ai_font_record_t *ptr;
  c_VectorFont *font;

  Xc_TRACE(("aiEncodeFont(`%s'->`%s' %snative)", old, new,
	    native_flag? "":"non "));

  /*--- Find font ---*/
  if(GlobFontBase == NULL) return NULL;
  if(((font = F(GlobFontBase).getFontByName(GlobFontBase, old)) == NULL) &&
     ((font = F(GlobFontBase).selectDialog(GlobFontBase, old)) == NULL))
    return NULL;
  
  /*--- Allocate ---*/
  if((ptr = Xc_malloc("AI font record", sizeof(ai_font_record_t))) == NULL)
    return NULL;

  ptr->next = this->font_base;
  this->font_base = ptr;
  
  /*--- Font encoding ---*/
  if(!native_flag)
    {
      int i;

      for(i = 0; i < 256; i++) ptr->encoding[i] = i;
    }
  else
    memcpy(ptr->encoding, this->encoding, 256 * sizeof(char_t));

  ptr->font = font;
  strncpy(ptr->name, new, 127);
  ptr->name[127] = 0;

  return ptr;
}


/* ----------------------------------------------------------------- ** 
** aiFindFont - Find VectorFont from name                            ** 
** ----------------------------------------------------------------- */
c_VectorFont *aiFindFont(this, name)
c_AIDocFile *this;
char *name;
{
  ai_font_record_t *ptr;
  
  Xc_TRACE(("aiFindFont(`%s')", name));

  for(ptr = this->font_base; ptr != NULL; ptr = ptr->next)
    if(!strcmp(name, ptr->name))
       return ptr->font;
  
  if(GlobFontBase != NULL)
    {
      c_VectorFont *font;
      
      font = F(GlobFontBase).getFontByName(GlobFontBase, name);
      return((font == NULL)?
	     F(GlobFontBase).selectDialog(GlobFontBase, name) : font);
    }
  else
    return NULL;
}


/* ----------------------------------------------------------------- ** 
** aiTranslateText - Translate text to ISO-Latin-1                   ** 
** ----------------------------------------------------------------- */
void aiTranslateText(this, string)
c_AIDocFile *this;
char_t *string;
{
  char_t *src;

  Xc_TRACE(("aiTranslateText(`%s')", string));

  for(src = string; *src; src++)
    if(this->encoding[*src])
      *string++ = this->encoding[*src];
  *string = 0;
}
