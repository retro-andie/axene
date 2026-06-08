/*
** BG_ManageBase.h for XQuad in Basic_Graph/
** structure de base standard
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
** Started on  Mon Dec 12 16:17:18 1994 Emmanuel Paris
** Last update Wed Jul  5 16:12:26 1995 Emmanuel Paris
*/

#ifndef _BG_ManageBase_h_
#define _BG_ManageBase_h_

#include "BaseStd.h"

#define BG_NB_COLOR_STEP	10
#define BG_NB_COLOR		6*BG_NB_COLOR_STEP

#define BGC_BLACK	0
#define BGC_GRAY_DARK	1
#define BGC_GRAY_MEDIUM	2
#define BGC_GRAY_LIGHT	3
#define BGC_WHITE	4
#define BGC_TRANSPARENT	5
#define BGC_NB_DEFINED	6

#define BGS_TITLE	0
#define BGS_LEGEND	1
#define BGS_GRADUATE	2
#define BGS_NORMAL	3

extern void Create_BG_Color_Base ___PROTO((BaseStd_t *basestd));
extern void Destroy_BG_Color_Base ___PROTO((BaseStd_t *basestd));
extern c_Color *Get_BG_Color ___PROTO((BaseStd_t *basestd, int color));
extern c_Color *Get_BG_Color_Step ___PROTO((BaseStd_t *basestd, int step));

extern void Create_BG_TextStyle_Base ___PROTO((BaseStd_t *basestd));
extern void Destroy_BG_TextStyle_Base ___PROTO((BaseStd_t *basestd));
extern c_TextStyle *Get_BG_TextStyle ___PROTO((BaseStd_t *basestd,int tstyle));

#endif

