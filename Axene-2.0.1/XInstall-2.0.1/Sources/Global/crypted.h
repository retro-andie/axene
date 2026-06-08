/*
** crypted.h for XInstall in Global/
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Mon Jan 12 16:53:52 1998 Emmanuel Paris
** Last update Mon Jan 12 16:56:31 1998 Emmanuel Paris
*/

#ifndef __crypted
#define __crypted

#ifdef CRYPT_STATIC
unsigned char crypt_str0[] = 
{249,36,35,220,41,213,24,29,22,35,28,26,213,37,39,36,28,0};
unsigned char crypt_str1[] = 
{14,36,42,213,22,39,26,213,22,213,23,22,25,213,24,39,22,24,32,26,39,0};
#else
extern unsigned char crypt_str0[];
extern unsigned char crypt_str1[];
#endif

#endif

