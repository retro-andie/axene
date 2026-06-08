/*
** Box_gotopage.h for Xclamation and XAllWrite in Box_gotopage/
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
** Started on  Fri Oct 23 15:57:35 1998 Emmanuel Paris
** Last update Fri Oct 30 15:23:39 1998 Emmanuel Paris
*/

#ifndef _Box_gotopage_h
#define _Box_gotopage_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "ManageWidget.h"
#include "Grid.h"
#include "Frame.h"
#include "SpinBox.h"
#include "Label.h"
#include "Document.h"

/* define the method for Class Box_gotopage */
typedef struct
{
  F_STD;
} sf_Box_gotopage;

/* define the Box_gotopage Class */
typedef struct
{
  sf_Box_gotopage	*f;
  Widget		w_This;
  int			nb_page;
  int			selected_page;
  int			*ret_selpg;

  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Label		*Ltitre;

  c_Frame		*FramePage;
  c_Grid		*GridPage;
  c_Label		*LnumPage;
  c_SpinBox		*SpinBoxPage;
  c_Label		*LofPage;

} c_Box_gotopage;

extern sf_Box_gotopage fc_Box_gotopage;

#endif

