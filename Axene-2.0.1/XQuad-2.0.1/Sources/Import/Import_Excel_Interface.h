/*
** Import_Excel_Interface.h for XQuad in Import/
** Header for Import_Excel_Interface Object
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Wed Feb 22 16:59:27 1995 Emmanuel Paris
** Last update Tue Nov 14 15:12:16 1995 Emmanuel Paris
*/

#ifndef _Import_Excel_Interface_h_
#define _Import_Excel_Interface_h_

#include "xcalibur.h"
#include "WInterface.h"
#include "Grid.h"
#include "Label.h"
#include "ToggleButton.h"
#include "WTextField.h"
#include "RadioBox.h"
#include <Xm/SeparatoG.h>
#include <X11/Intrinsic.h>

extern void xl_create_attribs_box ___PROTO((c_Import_Excel *This,
					    Widget Parent));
extern void xl_manage_attribs_box ___NPROTO((c_Import_Excel *This, 
					 boolean doc_available));
extern void xl_destroy_attribs_box ___PROTO((c_Import_Excel *This));

typedef struct {
  int	filler;
}import_excel_box_t;

#endif /* _Import_Excel_Interface_h_ */

