/*
** encodings.h for Xclamation, XQuad, XAllWrite and XMayday in Global/
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
** Last update Wed Jan  3 11:42:59 1996 Emmanuel Paris
*/

#ifndef _encodings_h_
#define _encodings_h_

typedef enum 
{
  XcE_LATIN1,
  XcE_DOS,
  XcE_MAC,
  XcE_HTML
} encoding_t;

/* convert encoding to iso-latin 1 */
extern void convert_encoding ___PROTO((char *string, encoding_t encoding));
/* convert iso-latin 1 to encoding */
extern char *convert_to_encoding ___PROTO((char *string, encoding_t encoding));

#endif /* _encodings_h_ */

