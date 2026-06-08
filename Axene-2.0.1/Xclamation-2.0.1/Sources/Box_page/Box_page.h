/*
** Box_page.h for Xclamation in Box_page/
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
** Started on  Mon Jul 17 14:04:25 1995 Robin Castanier
** Last update Fri Oct  2 19:18:23 1998 Robin Castanier
*/

#ifndef _Box_page_h
#define _Box_page_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Form.h"
#include "Icon.h"
#include "Frame.h"
#include "RadioImage.h"
#include "RadioBox.h"
#include "ManageWidget.h"
#include "ItemMenu.h"
#include "WTextField.h"
#include "Label.h"
#include "Grid.h"
#include "DocumentList.h"
#include "Document.h"
#include "PageFormat.h"
#include "Box_document.h"
#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>

/* define the method for Class Box_page */
typedef struct
{
  F_STD;
} sf_Box_page;

/* define the Box_page Class */
typedef struct
{
  sf_Box_page	*f;
  Widget		w_This;
  new_doc_s		*param;
  new_doc_s		*all_page;
  int			nb_page;
  int			selected_page;
  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Label		*Ltitre;
  c_RadioImage		*RadioImage;
  c_Frame		*Frame1;
  c_Frame		*Frame2;
  c_Frame		*Frame3;
  c_Frame		*Frame4;
  c_Frame		*Frame5;

  c_Grid		*GridFormat;
  c_Label		*Lsize;
  c_Icon		*Icon1;
  c_Icon		*Icon2;
  c_WTextField		*TFdocWidth;
  c_Label		*Lcroix;
  c_WTextField		*TFdocHeight;
  c_ItemMenu		*Option;

  c_Grid		*GridDecoupage;
  c_WTextField		*TFDecoupageV;
  c_WTextField		*TFGouttiereV;
  c_WTextField		*TFDecoupageH;
  c_WTextField		*TFGouttiereH;
  c_Label		*LDecoupageV;
  c_Label		*LGouttiereV;
  c_Label		*LDecoupageH;
  c_Label		*LGouttiereH;

  c_Frame		*FramenumPage;
  c_Grid		*GridnumPage;
  c_Label		*LnumPage;
  c_WTextField		*NumPage;
  c_Icon		*IPrevious;
  c_Icon		*INext;

  c_Grid		*GridMarges;
  c_WTextField		*TFMargeH;
  c_WTextField		*TFMargeB;
  c_WTextField		*TFMargeI;
  c_WTextField		*TFMargeE;
  c_Label		*LMargeH;
  c_Label		*LMargeB;
  c_Label		*LMargeI;
  c_Label		*LMargeE;
  c_RadioBox		*Pages;

  c_DocumentList	*Listdoc;

} c_Box_page;

extern sf_Box_page fc_Box_page;

#endif

