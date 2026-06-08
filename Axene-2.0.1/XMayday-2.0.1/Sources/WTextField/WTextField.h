/*
** WTextField.h for Xclamation, XQuad, XAllWrite, XMayday and XInstall in 
** 	WTextField/
** Object and method for WTextField
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
** Started on  Thu Nov 24 15:51:53 1994 Antoine Buat
** Last update Wed Nov  4 15:10:14 1998 Emmanuel Paris
*/

#ifndef _WTextField_h
#define _WTextField_h

/*--- Forward declarations ---*/
typedef struct sc_WTextField c_WTextField;

#include "xcalibur.h"
#include "ManageWidget.h"
#include "WInterface.h"
#include "Cursor.h"
#include <X11/keysym.h>
#include <X11/Intrinsic.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>

#if !defined(Xc_NO_UNIT) && !defined(WTF_UNIT)
#define WTF_UNIT
#endif

#ifdef WTF_UNIT
#include "UnitConvertion.h"
#endif

/*--- type ---*/
#define TF_INTEGER	0
#define TF_REAL		1
#define TF_STRING	2
#define TF_UNIT		3
#define TF_NUMSTRING	4

#define TEXTFIELD(obj) ((obj)->TextF)

typedef void (*WTextFieldProc)___PROTO((c_WTextField *this, void *data,
					XmAnyCallbackStruct *cbs));

/*--- define the method for Class WTextField ---*/
typedef struct
{
 F_STD;
 void (*set) ___PROTO((c_WTextField *this, char *string_val));
 char *(*get) ___PROTO((c_WTextField *this));
 void (*add_callback) ___PROTO((c_WTextField *this, 
				WTextFieldProc func, void *data));
 void (*remove_lostfocus_callback) ___PROTO((c_WTextField *this));
 void (*add_escape_func) ___PROTO((c_WTextField *this, 
				   void (*func)(), void *param));
#ifdef WTF_UNIT
 void (*set_unit) ___PROTO((c_WTextField *this, int unitb, char *unit_prec,
			    boolean limit_in_unit, boolean text_in_unit));
 void (*set_value_in_unit) ___PROTO((c_WTextField *this, real value));
 real (*get_value_in_unit) ___PROTO((c_WTextField *this));
 void (*set_value_no_unit) ___PROTO((c_WTextField *this, real value));
#endif
 void (*add_control_callback) ___PROTO((c_WTextField *this, 
					WTextFieldProc func, void *data));
} sf_WTextField;

/*--- define the WTextField Class ---*/
struct sc_WTextField
{
 sf_WTextField	*f;
 Widget		w_This;
 Widget		Label;
 Widget		TextF;
 int		Type;
 int		SetT;		/* to force no analys */
 int		neg;		/* negative number */
 real		LimitInf;
 real		LimitSup;
 c_Cursor	*cursor;

 /*___Callback___*/
 WTextFieldProc cb_func;
 void *cb_data;

 int		cleanSetT;	/* SetT was cleaned this callback */
 int		flag_edited;
 char		*old_string;
 void		(*esc_func)();
 void		*esc_param;
 
 int		unitb;		/* unit base: XcUB_... see UnitConvertion.h */
 char		*unit_prec;
 real		old_value;
 
 /*___Control Callback___*/
 WTextFieldProc cb_ctrl_func;
 void		*cb_ctrl_data;
 char		*ctrl_text;
};

extern sf_WTextField fc_WTextField;

#endif




