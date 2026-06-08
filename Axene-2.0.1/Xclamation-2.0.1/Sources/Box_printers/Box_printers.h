/*
** Box_printers.h for Xclamation in Box_printers/
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
** Started on  Mon Mar  4 16:58:06 1996 Robin Castanier
** Last update Mon Mar  4 17:41:46 1996 Robin Castanier
*/

#ifndef _Box_printers_h
#define _Box_printers_h

#include "xcalibur.h"
#include "WInterface.h"
#include "ASLBox.h"
#include "Grid.h"
#include "Frame.h"
#include "WTextField.h"
#include "ToggleButton.h"
#include "Resource.h"
#include "TextStyle.h"
#include <X11/Intrinsic.h>

/*--- define the method for Class Box_printers ---*/
typedef struct
{
  F_STD;
} sf_Box_printers;

/*--- define the Box_printers Class ---*/
typedef struct
{
  sf_Box_printers *f;
  Widget	w_This;

/*c_ModifList	*list_save;*/
/*c_TextStyle	**Style_base;*/
/*c_TextStyle	*v_Style_base;*/
/*c_Color	*colorbase;*/

  c_ASLBox	*PBox;
  c_Grid	*Grid;
  c_Frame	*Frame;
  c_WTextField	*tfBPCommandPrinter;
  c_ToggleButton *tbBPDefaultPrinter;

/*style_modif_t	modif;*/
} c_Box_printers;

extern sf_Box_printers fc_Box_printers;
extern c_Resource *GlobResources;

#endif
