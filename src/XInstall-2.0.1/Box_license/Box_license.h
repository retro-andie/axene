/*
** Box_license.h for XInstall in Box_license/
** Display the license file.
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
** Started on  Mon Oct 20 19:30:25 1997 Emmanuel Paris
** Last update Tue Oct 21 12:58:55 1997 Robin Castanier
*/

#ifndef _Box_license_h
#define _Box_license_h

/*--- includes ---*/
#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Grid.h"
#include "Label.h"
#include "Frame.h"
#include "ScrolledWindow.h"

/*--- define the method for Class Box_license ---*/
typedef struct
{
  F_STD;
  int (*waitForUnmap)();
} sf_Box_license;

/*--- define the Box_license Class ---*/
typedef struct
{
  sf_Box_license	*f;
  Widget		w_This;

  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Label		*Ltitre;
  c_Frame		*FText;
  c_ScrolledWindow	*SText;
  c_Label		*LText;
}
 c_Box_license;

extern sf_Box_license fc_Box_license;

#endif /* _Box_license_h */
