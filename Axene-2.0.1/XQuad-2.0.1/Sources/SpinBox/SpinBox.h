/*
** SpinBox.h for Xclamation, XQuad and XAllWrite in SpinBox/
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
** Started on  Sun Oct 25 17:21:55 1998 Emmanuel Paris
** Last update Tue Nov  3 17:31:12 1998 Emmanuel Paris
*/

#ifndef _SpinBox_h
#define _SpinBox_h

#include "xcalibur.h"
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include "WInterface.h"
#include "ManageWidget.h"
#include "Frame.h"
#include "RowColumn.h"
#include "WTextField.h"
#include "Icon.h"
#define SB_EXTERN_DEFINES
#include "SpinBoxP.h"
#undef SB_EXTERN_DEFINES

#define WTEXTFIELD(a) (a->textfield)

#define SB_TEXTFIELD_CALLBACK	0
#define SB_ARROW_UP_CALLBACK	1	
#define SB_ARROW_DOWN_CALLBACK	2

#define SB_VALUE_TABLE_END	(real)-456789.0

#define SB_NO_AUTO_REPEAT	0L
#define SB_DEFAULT_AUTO_REPEAT	300000L

typedef struct sc_SpinBox c_SpinBox;

/* define the method for Class SpinBox */
typedef struct
{
  F_STD;
  void (*addCallback) ___PROTO((c_SpinBox *This, void (*cb_func)(), 
				void *cb_data, int cb_type));
  void (*setUnit) ___PROTO((c_SpinBox *This, int unit, char *unit_prec,
			    boolean limit_in_unit, boolean text_in_unit));
  void (*setAutoRepeatDelay) ___PROTO((c_SpinBox *This, long delay));
						/* delay in micro second */
  void (*setIncrement) ___PROTO((c_SpinBox *This, real increment));
  void (*setValueTable) ___NPROTO((c_SpinBox *This, ...));
  void (*setStringTable) ___NPROTO((c_SpinBox *This, ...));
} sf_SpinBox;

/* define the SpinBox Class */
struct sc_SpinBox
{
  sf_SpinBox		*f;
  Widget		w_This;
  c_Frame		*frame;
  c_RowColumn		*rowcolumn;
  c_WTextField		*textfield;

  c_RowColumn		*rc_plusminus;
  c_Icon		*icon_plus;
  c_Icon		*icon_minus;
  Widget		arrow_plus;
  Widget		arrow_minus;
  
  int			sb_type;

  void			(*tf_cb_func)();
  void			*tf_cb_data;

  void			(*a1_cb_func)();
  void			*a1_cb_data;

  void			(*a2_cb_func)();
  void			*a2_cb_data;  

  long			auto_repeat;
  real			increment;
  real			*value_table;
  
  real			limit_inf;
  real			limit_sup;

  boolean		plus_sensitive;
  boolean		minus_sensitive;
};

extern sf_SpinBox fc_SpinBox;

#endif
