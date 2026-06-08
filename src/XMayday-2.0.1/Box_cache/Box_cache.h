/*
** Box_cache.h for Xclamation, XAllWrite and XMayday in Box_cache/
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
** Last update Mon Mar  4 18:51:28 1996 Robin Castanier
*/

#ifndef _Box_cache_h
#define _Box_cache_h

#include "xcalibur.h"
#include "WInterface.h"
#include "Grid.h"
#include "Frame.h"
#include "Icon.h"
#include "WTextField.h"
#include "CustomDialog.h"
#include "Label.h"
#include "ToggleButton.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include <Xm/PushB.h>
#include <Xm/DialogS.h>

/* defint the method for Class Box_cache */
typedef struct
{
  F_STD;
} sf_Box_cache;

/* define the Box_cache Class */
typedef struct
{
  sf_Box_cache	*f;
  Widget	w_This;
  c_CustomDialog *Dialog;
  c_Grid	*Grid;
  c_Label       *lTitle;
  c_Frame	*Frame1;
  c_Grid	*Grid1;
  c_Label       *lMemTotal1;
  c_WTextField	*tfMemTotal;
  c_Label       *lMemTotal2;
  c_Label       *lMemUsed1;
  c_WTextField	*tfMemUsed;
  c_Label       *lMemUsed2;
  c_Label       *lMemFree1;
  c_WTextField	*tfMemFree;
  c_Label       *lMemFree2;
  Widget        wPush1;

  c_Frame	*Frame2;
  c_Grid	*Grid2;
  c_Label       *lDiskTotal1;
  c_WTextField	*tfDiskTotal;
  c_Label       *lDiskTotal2;
  c_Label       *lDiskUsed1;
  c_WTextField	*tfDiskUsed;
  c_Label       *lDiskUsed2;
  c_Label       *lDiskFree1;
  c_WTextField	*tfDiskFree;
  c_Label       *lDiskFree2;
  Widget        wPush2;

  c_Frame	*Frame3;
  c_Grid	*Grid3;
  c_Icon        *IconBrowse;
  c_WTextField	*tfDiskCachePath;
  
  c_ToggleButton *tbBCKeepCacheOnDisk;

} c_Box_cache;

extern sf_Box_cache fc_Box_cache;

#endif










