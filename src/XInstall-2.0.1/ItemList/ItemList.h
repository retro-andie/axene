/*
** ItemList.h for Xclamation, XQuad, XAllWrite, XMayday and XInstall in 
** 	ItemList/
** Declaration for the ItemList class
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
** Last update Fri Aug 11 17:35:38 1995 Emmanuel Paris
*/

#ifndef __XC_ITEM_LIST_H__
#define __XC_ITEM_LIST_H__

typedef struct sc_ItemList c_ItemList;

#include "xcalibur.h"
#include "ManageWidget.h"
#include "WInterface.h"

#include <Xm/List.h>

/*--- Definitions ---*/
#define XcIL_POINTER(ptr, offset) ((void *)((char *) (ptr) + (offset)))

#define XcIL_ITEM_NAME(this, ptr) \
  (((this)->name_deference_flag == FALSE)? \
   ((char *) XcIL_POINTER((ptr), (this)->name_offset)) : \
   *((char **) XcIL_POINTER((ptr), (this)->name_offset)))

#define XcIL_ARRAY 0
#define XcIL_POINTER_ARRAY 1
#define XcIL_LINKED_LIST 2
#define XcIL_VA_LIST 3

#define XcIL_SINGLE_MODE XmSINGLE_SELECT
#define XcIL_BROWSE_MODE XmBROWSE_SELECT
#define XcIL_MULTIPLE_MODE XmMULTIPLE_SELECT
#define XcIL_EXTENDED_MODE XmEXTENDED_SELECT

#define XcIL_SIMPLE_CLICK 0
#define XcIL_DOUBLE_CLICK 1
 
#define LIST(this) (this->w_List)

typedef void (*XcItemListCallbackProc) ___PROTO((c_ItemList *this,
						 void *data,
						 int reason, ...));

/*--- Declaration of the methods ---*/
typedef struct
{
  F_STD;

  /*___Paramaters___*/
  void (*set) ___PROTO((c_ItemList *this, int mode,
			boolean name_flag, long name_offset,
			int visible_count,
			int (*sort) ___PROTO((void *, void *)),
			boolean (*filter) ___PROTO((void *))));
  /*___Callback___*/
  void (*setCallback) ___PROTO((c_ItemList *this,
				XcItemListCallbackProc cb_func,
				void *cb_data));
  void (*removeCallback) ___PROTO((c_ItemList *this));

  /*___Item managing___*/
  boolean (*addItems) ___PROTO((c_ItemList *this, boolean state,
				int mode, int count, ...));
  void (*deleteItems) ___PROTO((c_ItemList *this,
				int mode, int count, ...));
  boolean (*updateItems) ___PROTO((c_ItemList *this,
				int mode, int count, ...));

  void (*showItem) ___PROTO((c_ItemList *this, void *item));

  /*___Item state___*/
  boolean (*getSelection) ___PROTO((c_ItemList *this,
				    void ***arrayp, int *countp));
  boolean (*getState) ___PROTO((c_ItemList *this, void *item));
  void (*setState) ___PROTO((c_ItemList *this, boolean state,
			     int mode, int count, ...));
  void (*selectItem) ___PROTO((c_ItemList *this, void *item));
  void *(*getFirstUnselectItem) ___PROTO((c_ItemList *this));
} sf_ItemList;


/*--- Declaration of the class ---*/
struct sc_ItemList
{
  sf_ItemList *f;

  /*____Motif infos___*/
  Widget w_This;
  Widget w_List;
  int mode;
  
  /*___Callback___*/
  XcItemListCallbackProc cb_function;
  void *cb_data;
  
  /*___Infos to get the item name___*/
  long name_offset;
  boolean name_deference_flag;

  /*___Item database___*/
  void **item_base;
  int buffer_len;
  int item_count;

  /*___User functions___*/
  int (*sort_func) ___PROTO((void *, void *));
  boolean (*filter_func) ___PROTO((void *));
};

extern sf_ItemList fc_ItemList;

#endif /* !__XC_ITEM_LIST_H__ */
