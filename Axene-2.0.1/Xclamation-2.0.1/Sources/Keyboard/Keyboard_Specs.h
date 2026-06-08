/*
** Keyboard_Specs.h for Xclamation and XAllWrite in Keyboard/
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
** Started on  Sat Aug  5 18:26:36 1995 Emmanuel Paris
** Last update Wed Oct 14 17:56:28 1998 Emmanuel Paris
*/

#ifndef _Keyboard_Specs_h_
#define	_Keyboard_Specs_h_

#include <X11/keysym.h>

#define KB_MODIF_SHIFT		0
#define KB_MODIF_LOCK		1
#define KB_MODIF_CONTROL	2
#define KB_MODIF_MOD1		3
#define KB_MODIF_MOD2		4
#define KB_MODIF_MOD3		5
#define KB_MODIF_MOD4		6
#define KB_MODIF_MOD5		7
#define KB_MODIF_END		8

typedef struct 
{
 int		modifier;
 KeySym		keysym;
 boolean	(*convertion) ___PROTO((c_Keyboard *This));
} keyboard_specs_t;

extern keyboard_specs_t keyboard_specs[];

#endif




