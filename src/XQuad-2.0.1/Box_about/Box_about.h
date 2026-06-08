/*
** Box_about.h for Xclamation, XQuad, XAllWrite and XMayday in Box_about/
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
** Started on  Mon Jul 17 13:54:56 1995 Robin Castanier
** Last update Sun Jan 16 17:35:29 2000 Emmanuel Paris
*/

#ifndef _Box_about_h
#define _Box_about_h

#include "xcalibur.h"
#include "WInterface.h"
#include "Resource.h"
#include "Grid.h"
#include "Frame.h"
#include "Form.h"
#include "Icon.h"
#include "CustomDialog.h"
#include "Label.h"
#include "ManageWidget.h"
#include "version.h"
#include <X11/Intrinsic.h>
#include <Xm/DialogS.h>

/* defint the method for Class Box_about */
typedef struct
{
  F_STD;
} sf_Box_about;

/* define the Box_about Class */
typedef struct
{
  sf_Box_about	*f;
  Widget	w_This;
  c_CustomDialog *Dialog;
  c_Grid	*Grid;
  c_Frame	*Frame2;
  c_Grid	*Grid2;
  c_Form	*FormIcon;
  c_Icon        *IconSoft;
  c_Icon        *IconEditor;
  c_Label       *Label1;
  c_Label       *Label2;
  c_Label       *Label3;
  c_Label       *Label4;
  c_Label       *VersionLabel;
  c_Label       *LicenseLabel;
  XtIntervalId	id;
  XtAppContext	app;
  boolean	bPixmap;
} c_Box_about;

extern sf_Box_about fc_Box_about;
extern c_Resource *GlobResources;

#endif










