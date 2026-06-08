/*
** Translations.c for Xclamation, XQuad and XAllWrite in MainInterface/
** intrinsics add translation for Text and/or TextField widgets
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
** Started on  Tue Sep  5 11:12:17 1995 Emmanuel Paris
** Last update Sun Sep 13 17:19:05 1998 Emmanuel Paris
*/

#define NTRACE

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "Translations.h"

#define TSL_BEGIN	"#override\n"

static char *create_tsl ___PROTO((char *header));

void add_translations(display, type)
Display *display;
int type;
{
 char		*tsl;
 XrmDatabase	db;
 char		*vtype;
 XrmValue	value, set_value;
 char		*header;

 db = XrmGetDatabase(display);

     
 if ( (type & TSL4_TEXT) == TSL4_TEXT)
 {
  if (XrmGetResource(db, "*XmText.translations", 
		     "*XmText.translations", &vtype, &value) 
      == True)
  {
   header = value.addr;
  }
  else
   header = TSL_BEGIN;

  if ( (tsl = create_tsl(header)) != NULL)
  {
   set_value.size = strlen(tsl) + 1;
   set_value.addr = (caddr_t)tsl;
   XrmPutResource(&db, "*XmText.translations", "String", &set_value); 
   Xc_free(tsl);
  }
 }
 
 if ( (type & TSL4_TEXTFIELD) == TSL4_TEXTFIELD)
 {
  if (XrmGetResource(db, "*XmTextField.translations", 
		     "*XmTextField.translations", &vtype, &value) 
      == True)
  {
   header = value.addr;
  }
  else
   header = TSL_BEGIN;
  
  if ( (tsl = create_tsl(header)) != NULL)
  {
   set_value.size = strlen(tsl) + 1;
   set_value.addr = (caddr_t)tsl;
   XrmPutResource(&db, "*XmTextField.translations", "String", &set_value); 
   Xc_free(tsl);
  }
 } 
}

static char *create_tsl(header)
char *header;
{
 char *str, *ptr, tmp[100];
#if 0
 char  tmp2[100];
#endif
 int   i, len;
 char  c;

 len = strlen(header);
 str = ptr = (char *)Xc_malloc("tsl", NB_KEY_TRANSLATION * 55 * 3 + 1 + len); 
 strcpy(str, header); ptr += len - 1;
 while(*(ptr-1) == '\n')  ptr--;
 *(++ptr) = '\0';
 
 for(i = FIRST_KEY_TRANSLATION; i <= LAST_KEY_TRANSLATION; i++)
 {
  /* translation for decimal to ascii */
  sprintf(tmp, "m<Key>space,\"%d\":\t\t\t\txc_insert_hexchar(0x%2x)\n",i,i);
  len = strlen(tmp);
  memcpy(ptr, tmp, len);
  ptr += len;
  
  /* translation for octal to ascii */
  sprintf(tmp, "m<Key>space,\"0%o\":\t\t\t\txc_insert_hexchar(0x%2x)\n",i,i);
  len = strlen(tmp);
  memcpy(ptr, tmp, len);
  ptr += len;

#if 0		/* do not work with capslock :-( */ 
  /* translation for hexa to ascii */
  sprintf(tmp, "m<Key>space,:<Key>x,");
  if ( (( i & 0xf0) < 0xa0) && ((i & 0xf) < 0xa))
   sprintf(tmp2, "\"%x\":\t\t\t\txc_insert_hexchar(0x%2x)\n",i,i);
  else
  {
   if ( (i & 0xf0) < 0xa0)
    sprintf(tmp2, "\"%c\",:<Key>%c:\t\txc_insert_hexchar(0x%2x)\n",
	    ((i & 0xf0) >> 4) + '0', (i & 0xf) - 10 + 'a', i);
   else if ((i & 0xf) < 0xa)
    sprintf(tmp2, ":<Key>%c,\"%c\":\t\txc_insert_hexchar(0x%2x)\n",
	    ((i & 0xf0) >> 4) - 10 + 'a', (i & 0xf) + '0', i);
   else
    sprintf(tmp2, ":<Key>%c,:<Key>%c:\txc_insert_hexchar(0x%2x)\n",
	    ((i & 0xf0) >> 4) - 10 + 'a', (i & 0xf) - 10 + 'a', i);
  }
  strcat(tmp, tmp2);
  len = strlen(tmp);
  memcpy(ptr, tmp, len);
  ptr += len;
#endif
 }
 for(c = '0'; c <= '9'; c++)
 {
  sprintf(tmp, "\"%c\":\t\t\t\txc_insert_string(\"%c\")\n", c, c);
  len = strlen(tmp);
  memcpy(ptr, tmp, len);
  ptr += len;
 }
#if 0
 for(c = 'a'; c <= 'f'; c++)
 {
  sprintf(tmp, ":<Key>%c:\t\t\txc_insert_string(\"%c\")\n", c, c);
  len = strlen(tmp);
  memcpy(ptr, tmp, len);
  ptr += len;
 }
 sprintf(tmp, ":<Key>x:\t\t\txc_insert_string(\"x\")\n");
 len = strlen(tmp);
 memcpy(ptr, tmp, len);
 ptr += len;
#endif
 *ptr = '\0';

 return str;
}
