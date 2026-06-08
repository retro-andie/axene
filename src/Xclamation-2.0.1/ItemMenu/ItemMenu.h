/*
** ItemMenu.h for Xclamation, XQuad, XAllWrite, XMayday and XInstall in 
** 	ItemMenu/
** Declaration for the ItemMenu class
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
** Started on  Mon Oct 17 14:22:21 1994 Stéphane Boisson
** Last update Sat Aug 14 16:59:29 1999 Emmanuel Paris
*/

#ifndef __XC_ITEM_MENU_H__
#define __XC_ITEM_MENU_H__

typedef struct sc_ItemMenu c_ItemMenu;

#include "xcalibur.h"
#include "ManageWidget.h"
#include "WInterface.h"

#include <Xm/List.h>

/*--- Definitions ---*/
#define XcIM_POINTER(ptr, offset) ((void *)((char *) (ptr) + (offset)))

#define XcIM_ITEM_NAME(this, ptr) \
(((this)->name_deference_flag == FALSE)? \
 ((char *) XcIM_POINTER((ptr), (this)->name_offset)) : \
   *((char **) XcIM_POINTER((ptr), (this)->name_offset)))

#define XcIM_ARRAY 0
#define XcIM_POINTER_ARRAY 1
#define XcIM_LINKED_LIST 2
#define XcIM_VA_LIST 3

#define XcIM_SINGLE_MODE XmSINGLE_SELECT
#define XcIM_BROWSE_MODE XmBROWSE_SELECT
#define XcIM_MULTIPLE_MODE XmMULTIPLE_SELECT
#define XcIM_EXTENDED_MODE XmEXTENDED_SELECT

#define MENUPANE(p) ((p)->w_Pane)
#define CASCADEBUTTON(p) ((p)->w_Cascade)
#define TITLELABEL(p) ((p)->w_Label)
#define CBUTTON_SIZE 50

typedef struct
{
 void *item;
 Widget w;
} item_menu_button_t;


/*--- Declaration of the methods ---*/
typedef struct
{
 F_STD;

 /*___Mapping___*/
 void (*set) ___PROTO((c_ItemMenu *this,
		       boolean name_flag, long name_offset,
		       int (*sort) ___PROTO((void *, void *)),
		       boolean (*filter) ___PROTO((void *))));
 /*___Callback___*/
 void (*setCallback) ___PROTO((c_ItemMenu *this,
			       void (*cb_func) ___PROTO((c_ItemMenu *this,
							 void *data,
							 void *item)),
			       void *cb_data));
 void *(*getSelection) ___PROTO((c_ItemMenu *this));
 void (*setSelection) ___PROTO((c_ItemMenu *this, void *item));

 /*___Item managing___*/
 boolean (*addItems) ___PROTO((c_ItemMenu *this,
			       int mode, int count, ...));
 void (*deleteItems) ___PROTO((c_ItemMenu *this,
			       int mode, int count, ...));
 boolean (*updateItems) ___PROTO((c_ItemMenu *this,
				  int mode, int count, ...));

 void (*deleteAllItems) ___PROTO((c_ItemMenu *this));
 void (*calculSize) ___PROTO((c_ItemMenu *this, int maxsize));
} sf_ItemMenu;


/*--- Declaration of the class ---*/
struct sc_ItemMenu
{
 sf_ItemMenu *f;

 /*____Motif infos___*/
 Widget w_This;
 Widget w_Pane;
 Widget w_Cascade;
 Widget w_Label;

 /*___Callback___*/
 void (*cb_function) ___PROTO((c_ItemMenu *this, void *data, void *item));
 void *cb_data;
  
 /*___Infos to get the item name___*/
 long name_offset;
 boolean name_deference_flag;

 /*___Item database___*/
 item_menu_button_t *selection;
 item_menu_button_t *item_base;
 int buffer_len;
 int item_count;
 boolean staticSize;

 /*___User functions___*/
 int (*sort_func) ___PROTO((void *, void *));
 boolean (*filter_func) ___PROTO((void *));
};

extern sf_ItemMenu fc_ItemMenu;

#endif /* !__XC_ITEM_MENU_H__ */
