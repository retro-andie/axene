/*
** Tongues.h for XInstall in Tongues/
** Set Application resources for text messages with tongue dependencies.
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
** Started on  Sat Jul  5 16:05:57 1997 Emmanuel Paris
** Last update Wed Feb  4 19:46:04 1998 Emmanuel Paris
*/

#ifndef _Tongues_h_
#define _Tongues_h_

typedef struct sc_Tongue c_Tongue;

#include "xcalibur.h"

#define Xi_USENGLISH	1
#define Xi_USA		Xi_USENGLISH
#define Xi_ENGLISH	2
#define Xi_FRENCH	3
#define Xi_GERMAN	4
#define Xi_SPANISH	5


/*--- Methods definition ---*/
typedef struct 
{
  F_STD;
} sf_Tongue;


/*--- Class definition ---*/
struct sc_Tongue 
{
 sf_Tongue *f;
};

extern sf_Tongue fc_Tongue;

#endif /* _Tongues_h_ */
