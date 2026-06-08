/*
** Box_paths.h for Xclamation in Box_paths/
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
** Started on  Tue Mar 19 11:36:53 1996 Robin Castanier
** Last update Tue Mar 19 11:43:21 1996 Robin Castanier
*/

#ifndef _Box_paths_h
#define _Box_paths_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Grid.h"
#include "Frame.h"
#include "WTextField.h"
#include "ToggleButton.h"
#include "Resource.h"
#include "TextStyle.h"
#include <X11/Intrinsic.h>

/*--- define the method for Class Box_paths ---*/
typedef struct
{
  F_STD;
} sf_Box_paths;

/*--- define the Box_paths Class ---*/
typedef struct
{
  sf_Box_paths *f;
  Widget	w_This;

  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Frame		*Frame;

/*style_modif_t	modif;*/
} c_Box_paths;

extern sf_Box_paths fc_Box_paths;
extern c_Resource *GlobResources;

#endif







