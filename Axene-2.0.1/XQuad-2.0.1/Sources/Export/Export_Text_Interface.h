/*
** Export_Text_Interface.h for XQuad in Export/
** Header for Export_Text_Interface Object
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
** Started on  Sun Dec 15 17:31:54 1996 Robin Castanier
** Last update Wed Dec 18 15:52:41 1996 Robin Castanier
*/

#ifndef _Export_Text_Interface_h_
#define _Export_Text_Interface_h_

#include "xcalibur.h"
#include "WInterface.h"
#include "Grid.h"
#include "ToggleButton.h"
#include "Label.h"
#include "WTextField.h"
#include "RadioBox.h"
#include <Xm/SeparatoG.h>
#include <X11/Intrinsic.h>

extern void export_txt_create_attribs_box ___PROTO((c_Export_Text *This,
						    Widget Parent,
						    ab_size_t *size));
extern void export_txt_manage_attribs_box ___NPROTO((c_Export_Text *This, 
						     boolean doc_available,
						     ab_size_t *size));
extern void export_txt_destroy_attribs_box ___PROTO((c_Export_Text *This));
extern Widget export_txt_get_widget_attribs_box ___PROTO((c_Export_Text 
							  *This));

typedef struct {
  boolean	managed;
  c_Grid	*Grid;
  c_ToggleButton *tbFSETselection;
  c_ToggleButton *tbFSETformula;
  Widget	sep1;
  c_Label	*lFSETseparators;
  c_WTextField	*tfFSETseparators;
  Widget	sep2;
  c_Label	*lFSETcharCode;
  c_RadioBox	*rbFSETcharCode;
} export_text_box_t;

#endif /* _Export_Text_Interface_h_ */
