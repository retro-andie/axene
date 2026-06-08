/*
** ASLBox.h for Xclamation, XQuad and XAllWrite in ASLBox/
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
** Started on  Mon Jul 17 13:49:22 1995 Marc Blet
** Last update Tue Jan 18 15:11:36 2000 Emmanuel Paris
*/

#ifndef _ASLBox_
#define _ASLBox_

typedef struct sc_ASLBox c_ASLBox;

#include "xcalibur.h"
#include "CustomDialog.h"
#include "Form.h"
#include "Label.h"
#include "Frame.h"
#include "Grid.h"
#include "Icon.h"
#include "PushButton.h"
#include "WTextField.h"
#include "ManageWidget.h"
#include "ItemList.h"


/* ----- Defines the maximum count of elements in bottom row-column ----- */

#define XcASL_MAX_COUNT		8	

/* ---- Defines the differents kind of elements in bottom row-column ---- */

#define XcASL_NOTHING		0
#define XcASL_ICON		1
#define XcASL_PUSHBUTTON	2

/* define the method for Class ASLBox */
typedef struct
{
 F_STD;
 void		(*Map)();
 void		(*Unmap)();
 int		(*Add_Buttons)();
 int		(*Add_Icons)();
 void		(*setCallback) ___PROTO((c_ASLBox *this,
					 void (*cb_func)
					 ___PROTO((c_ASLBox *box, void *data,
						   void *item)),
					 void *cb_data));
 void (*selectItem) ___PROTO((c_ASLBox *this, void *item, boolean cb_flag));
 char	*(*getNewItemName) ___PROTO((c_ASLBox *this, void **item));
} sf_ASLBox;


typedef struct
{
 union 
 {
  c_Icon	*The_Icon;
  c_PushButton	*PushB;
 } button;
 void (*Callback_Function)();
 void *Callback_Data;
} ASLButton;

/* define the ASLBox Class */
struct sc_ASLBox
{
 sf_ASLBox		*f;

 Widget		w_This;
 c_CustomDialog	*Dialog;
 c_ItemList	*ItemList;
 c_WTextField	*text_field;
 c_Label	*Label;
 c_Frame	*Frame;
 c_Form		*Form;
 c_Grid		*grid;
 ASLButton	Buttons[XcASL_MAX_COUNT];
 unsigned char	Last_Item;
 int		Type;

 /*___Callback___*/
 void (*cb_func) ___PROTO((c_ASLBox *box, void *data, void *item));
 void *cb_data;
 void *edited_item;
 Widget		wSep;
};



/*
* Dialog est l'objet CustomDialog
 *
 * ItemList contient la ItemList de la boite ... 
 * utile pour recuperer les ressources de la boite
 *
 * Editor est la TextWidget, utile pour editer le nom ;-)
 *
 * WDPArent is the warning dialog parent widget ...
 *
 * w_this est la widget row_column qui contient la Form et la widget de droite
 * ce n'est en aucun cas la CustomDialog ...
 *
 */


extern sf_ASLBox fc_ASLBox;

#endif










