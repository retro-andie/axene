/*
** ruler.h for XQuad in Global/
** Definitions of misc. ruler structures
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
** Started on  Mon Jun 13 00:08:10 1994 Stéphane Boisson
** Last update Fri Oct 27 15:49:08 1995 Robin Castanier
*/

#ifndef __RULERS_H__
#define __RULERS_H__

/* rules: alignment & justification */
typedef enum {
  XqR_DEFAULT = 0,	/* left align for text and right for number, Vcenter */
  XqR_H_ALIGN_MASK = 7,
  XqR_H_ALIGN_LEFT = 1,
  XqR_H_ALIGN_RIGHT = 2,
  XqR_H_ALIGN_CENTER = 3,
  XqR_H_JUSTIFIED = 4,	
  XqR_MULTILINE = 8,
  XqR_V_ALIGN_MASK = 48,
  XqR_V_ALIGN_CENTER = 0,
  XqR_V_ALIGN_TOP = 16,
  XqR_V_ALIGN_BOTTOM = 32,
  XqR_V_JUSTIFIED = 48,	
  XqR_A_MASK = 192,
  XqR_A_DEFAULT = 0,
  XqR_A_90 = 64,	
  XqR_A_270 = 128,
  XqR_A_180 = 192 /* not managed yiet */
} ruler_t;

#endif /* !__RULERS_H__ */
