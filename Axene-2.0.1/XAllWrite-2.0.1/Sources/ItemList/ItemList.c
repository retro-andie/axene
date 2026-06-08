/*
** ItemList.c for Xclamation, XQuad, XAllWrite, XMayday and XInstall in 
** 	ItemList/
** Methods for the ItemList class
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
** Started on  Mon Oct 17 14:27:44 1994 Stéphane Boisson
** Last update Sat Aug 14 15:10:08 1999 Emmanuel Paris
*/

#ifndef NHISTORY
#define NHISTORY
#endif
#ifndef NTRACE
#define NTRACE
#endif

#include <varargs.h>
#include "ItemList.h"
#include "RegisterHelp.h"


/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();

static void removeCallback ___PROTO((c_ItemList *this));
static void setCallback ___PROTO((c_ItemList *this,
				  XcItemListCallbackProc cb_func,
				  void *cb_data));

static boolean add_items();
static void delete_items();
static boolean updateItems();

/*static void unmap ___PROTO((c_ItemList *this));*/
static void map ___PROTO((c_ItemList *this, int mode,
			  boolean name_flag, long name_offset,
			  int visible_count,
			  int (*sort) ___PROTO((void *, void *)),
			  boolean (*filter) ___PROTO((void *))));

static void show_item ___PROTO((c_ItemList *this, void *item));
static boolean getSelection ___PROTO((c_ItemList *this,
				      void ***arrayp, int *countp));
static boolean getState ___PROTO((c_ItemList *this, void *item));
static void setState();
static void select_item ___PROTO((c_ItemList *this, void *item));
static void *getFirstUnselectItem ___PROTO((c_ItemList *this));

static void cb_list ___PROTO((Widget w, c_ItemList *this,
			      XmListCallbackStruct *cbs));

sf_ItemList fc_ItemList =
{
 constructor, destructor, copy,
 map, 
 setCallback, removeCallback,
 (boolean (*) ___PROTO((c_ItemList *, boolean, int, int, ...))) add_items,
 (void (*) ___PROTO((c_ItemList *, int, int, ...))) delete_items,
 (boolean (*) ___PROTO((c_ItemList *, int, int, ...))) updateItems,
 show_item, 
 getSelection, getState,
 (void (*) ___PROTO((c_ItemList *, boolean, int, int, ...))) setState,
 select_item,
 getFirstUnselectItem,
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(parent, manager, title, w, h)
Widget parent;
c_ManageWidget *manager;
char *title;
int w;
int h;
{
 c_ItemList *p;
 Arg xtarg[4];

 Xc_HISTORY(("constructor"));

 if((p = Xc_malloc("ItemList", sizeof(c_ItemList))) == NULL)
  return NULL;
 p->f = &fc_ItemList; 

 p->item_base = NULL;
 p->buffer_len = p->item_count = 0;
 p->sort_func = NULL;
 p->filter_func = NULL;
 p->cb_function = NULL;
 p->cb_data = NULL;

 XtSetArg(xtarg[0], XmNvisualPolicy, XmCONSTANT);
 /*XtSetArg(xtarg[1], XmNlistSizePolicy, XmVARIABLE);*/
 XtSetArg(xtarg[1], XmNlistSizePolicy, XmCONSTANT);
 LIST(p) = XmCreateScrolledList(parent, title, xtarg, 2);
 WIDGET(p) = XtParent(LIST(p));

 Xc_REGISTER_HELP(LIST(p));

 if(w)
  XtVaSetValues(WIDGET(p), XmNwidth, w, NULL);
 if(h)
  XtVaSetValues(WIDGET(p), XmNheight, h, NULL);

#ifdef LESSTIF_VERSION
 {
  Widget HSB, VSB;
  
  XtVaGetValues(WIDGET(p), XmNverticalScrollBar, &HSB, 
		XmNhorizontalScrollBar, &VSB, NULL);
  XtVaSetValues(HSB, XmNhighlightThickness, 0, NULL);
  XtVaSetValues(VSB, XmNhighlightThickness, 0, NULL);
 }
#endif

 F(manager).Add_child(manager, LIST(p), WIDGET(p));

 return (p);
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_ItemList *this;
{
 Xc_HISTORY(("destructor"));

 if(this->item_base) Xc_free(this->item_base);
 XtDestroyWidget(WIDGET(this));
 Xc_free(this);
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_ItemList *this;
{
 c_ItemList *p;

 Xc_HISTORY(("copy"));

 if((p = Xc_malloc("CItemList", sizeof(c_ItemList))) == NULL)
  return NULL;

 memcpy(p, this, sizeof(c_ItemList));
 return p; 
}


/* ----------------------------------------------------------------- ** 
** map - Remanage ItemList                                       ** 
** ----------------------------------------------------------------- */
static void map(this, mode, name_flag, name_offset, view_count, sort, filter)
c_ItemList *this;
int mode;
boolean name_flag;
long name_offset;
int view_count;
int (*sort) ___PROTO((void *, void *));
boolean (*filter) ___PROTO((void *));
{
 Xc_HISTORY(("set"));

 if(view_count > 0)
  XtVaSetValues(LIST(this),
		XmNvisibleItemCount, view_count,
		XmNselectionPolicy, mode,
		NULL);
 else if(mode != -1)
  XtVaSetValues(LIST(this), XmNselectionPolicy, mode, NULL);
    
 if(mode != -1) this->mode = mode;
 if(sort != (int (*)())-1L) this->sort_func = sort;
 if(filter != (boolean (*)())-1L) this->filter_func = filter;
 if(name_offset != -1L)
 {
  this->name_deference_flag = name_flag;
  this->name_offset = name_offset;
 }
}


/* ----------------------------------------------------------------- ** 
** addItems - Add new items in the ItemList                      ** 
** ----------------------------------------------------------------- */
static boolean add_items(this, state, mode, count, va_alist)
c_ItemList *this;
boolean state;
int mode;
int count;
va_dcl
{
 boolean speed_flag;
 long next_offset = 0L;
 char *item_list = NULL;
 int new_count;
 XmString *array = NULL;
 va_list ap;

 Xc_HISTORY(("addItems(%d)", count));

 /*--- Initialize item list data ---*/
 va_start(ap);
 if((mode == XcIL_LINKED_LIST) || (mode == XcIL_ARRAY))
 {
  item_list = va_arg(ap, char *);
  next_offset = va_arg(ap, long);
 }
 else if(mode == XcIL_POINTER_ARRAY)
  item_list = va_arg(ap, char *);

 if((mode == XcIL_LINKED_LIST) && (count <= 0))
 {
  void *ptr;

  ptr = item_list;
  count = 0;
  while(ptr != NULL)
  {
   count++;
   ptr = *((void **)XcIL_POINTER(ptr, next_offset));
  }
  Xc_TRACE(("real count: %d", count));
 }

 if(count == 0)
 {
  va_end(ap);
  return TRUE;
 }

 /*--- Check if we've got enough memory ---*/
 new_count = this->item_count + count;
 if(new_count > this->buffer_len)
 {
  void *new;

  new = Xc_realloc("ItemList items",
		   this->item_base, new_count * sizeof(void *));
  if(new == NULL)
  {
   va_end(ap);
   return FALSE;
  }
  this->buffer_len = new_count;
  this->item_base = new;
 }

 if(this->item_count == 0)
 {
  speed_flag = TRUE;
  array = (XmString *)XtMalloc(sizeof(XmString) * count);
 }
 else
  speed_flag = FALSE;
  
 if((this->mode == XcIL_EXTENDED_MODE) && (state == TRUE))
  XtVaSetValues(LIST(this), XmNselectionPolicy, XcIL_MULTIPLE_MODE, NULL);
	
 /*--- Main loop ---*/
 while(count--)
 {
  char *new_name;
  XmString str;
  void *ptr = NULL;

  switch(mode)
  {
  case XcIL_ARRAY:
   ptr = (void *)item_list;
   item_list += next_offset;
   break;

  case XcIL_POINTER_ARRAY:
   ptr = *((void **)item_list);
   item_list += sizeof(void *); 
   break;

  case XcIL_LINKED_LIST:
   if((ptr = item_list) == NULL) break;;
   item_list = *((void **)XcIL_POINTER(item_list, next_offset));
   break;

  case XcIL_VA_LIST:
   ptr = va_arg(ap, void *);
   break;
  }

  if(ptr == NULL) break;
  if((this->filter_func != NULL) &&
     (this->filter_func(ptr) == FALSE)) continue;

  new_name = XcIL_ITEM_NAME(this, ptr);
  Xc_TRACE(("name `%s'", new_name));

  str = XmStringCreateSimple(new_name);
  if(str == NULL)
  {
   va_end(ap);
   if(speed_flag == TRUE) XtFree((XtPointer)array);
   if((this->mode == XcIL_EXTENDED_MODE) && (state == TRUE))
    XtVaSetValues(LIST(this),
		  XmNselectionPolicy, XcIL_EXTENDED_MODE, NULL);
   return FALSE;
  }

  if(this->sort_func != NULL)
  {
   int u_bound, l_bound, i;
	  
   l_bound = 0;
   u_bound = this->item_count - 1;

   /*--- Binary search ---*/
   while(u_bound >= l_bound)
   {
    char *text;
    int i;
	  
    i = l_bound + ((u_bound - l_bound) >> 1);
    text = XcIL_ITEM_NAME(this, this->item_base[i]);
    if(this->sort_func(this->item_base[i], ptr) > 0)
     u_bound = i - 1;
    else
     l_bound = i + 1;
   }
   if((i = this->item_count - l_bound) != 0)
    memmove(this->item_base + l_bound + 1,
	    this->item_base + l_bound,
	    i * sizeof(void *));
   this->item_base[l_bound] = ptr;

   if(speed_flag == TRUE)
   {
    if(i != 0)
     memmove(array + l_bound + 1, array + l_bound,
	     i * sizeof(XmString));
    array[l_bound] = str;
   }
   else
   {
    XmListAddItemUnselected(LIST(this), str, l_bound + 1);
    if(state != FALSE)
     XmListSelectPos(LIST(this), l_bound + 1, False);
   }
  }
  else
  {
   /*--- Item are added at the end of the list ---*/
   this->item_base[this->item_count] = ptr;
   if(speed_flag == FALSE)
   {
    XmListAddItemUnselected(LIST(this), str, 0);
    if(state != FALSE)
     XmListSelectPos(LIST(this), this->item_count + 1, False);
   }
   else
    array[this->item_count] = str;
  }

  this->item_count++;
  if(speed_flag == FALSE) XmStringFree(str);
 }
 va_end(ap);

 if(speed_flag == TRUE)
 {
  int i;

  XtVaSetValues(LIST(this),
		XmNitems, array,
		XmNitemCount, this->item_count,
		NULL);
  if(state == TRUE)
   XtVaSetValues(LIST(this),
		 XmNselectedItems, array,
		 XmNselectedItemCount, this->item_count,
		 NULL);
 
  for(i = this->item_count; i--; XmStringFree(array[i]));
  XtFree((XtPointer)array);
 }
 if((this->mode == XcIL_EXTENDED_MODE) && (state == TRUE))
  XtVaSetValues(LIST(this), XmNselectionPolicy, XcIL_EXTENDED_MODE, NULL);

 Xc_TRACE(("add done"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** deleteItems - Delete some item in the ItemList                ** 
** ----------------------------------------------------------------- */
static void delete_items(this, mode, count, va_alist)
c_ItemList *this;
int mode;
int count;
va_dcl
{
 long next_offset = 0L;
 char *item_list = NULL;
 va_list ap;

 Xc_HISTORY(("deleteItems(%d)", count));
  
 /*--- Delete all items ---*/
 if(count == -1)
 {
  Xc_TRACE(("delete all"));
  XmListDeleteAllItems(LIST(this));
  this->item_count = 0;
  return;
 }      

 /*--- Initialize ---*/
 va_start(ap);
 if((mode == XcIL_LINKED_LIST) || (mode == XcIL_ARRAY))
 {
  item_list = va_arg(ap, char *);
  next_offset = va_arg(ap, long);
 }
 else if(mode == XcIL_POINTER_ARRAY)
  item_list = va_arg(ap, char *);
	
 /*--- Main loop ---*/
 while(count--)
 {
  void *ptr = NULL;
  void **list;
  int i;

  switch(mode)
  {
  case XcIL_ARRAY:
   ptr = (void *)item_list;
   item_list += next_offset;
   break;

  case XcIL_POINTER_ARRAY:
   ptr = *((void **)item_list);
   item_list += sizeof(void *); 
   break;

  case XcIL_LINKED_LIST:
   if((ptr = item_list) == NULL) break;
   item_list = *((void **)XcIL_POINTER(item_list, next_offset));
   break;

  case XcIL_VA_LIST:
   ptr = va_arg(ap, void *);
   break;
  }

  if(ptr == NULL) break;

  for(list = this->item_base, i = 1; i <= this->item_count; i++)
   if(*list++ == ptr)
   {
    Xc_TRACE(("delete `%s'", XcIL_ITEM_NAME(this, ptr)));
    if(i != this->item_count)
     memmove(list - 1, list, (this->item_count - i) * sizeof(void *));
    XmListDeletePos(LIST(this), i);
    this->item_count--;
    break;
   }
 }
 va_end(ap);
}


/* ----------------------------------------------------------------- ** 
** show_item - Make sure the given item is visible                   ** 
** ----------------------------------------------------------------- */
static void show_item(this, item)
c_ItemList *this;
void *item;
{
 void **list;
 int i;

 Xc_HISTORY(("showItem(`%s')", XcIL_ITEM_NAME(this, item)));

 for(list = this->item_base, i = 1; i <= this->item_count; i++)
  if(*list++ == item)
  {
   int top, visible;

   XtVaGetValues(LIST(this),
		 XmNtopItemPosition, &top,
		 XmNvisibleItemCount, &visible,
		 NULL);
   if(i < top)
    XmListSetPos(LIST(this), i);
   else if(i >= (top + visible))
    XmListSetBottomPos(LIST(this), i);
   return;
  }
 Xc_TRACE(("item `%s' not found", XcIL_ITEM_NAME(this, item)));
} 

/* ----------------------------------------------------------------- ** 
** select_item - select specified item by calling XmListSelectPos    ** 
** ----------------------------------------------------------------- */
static void select_item(this, item)
c_ItemList *this;
void *item;
{
 void **list;
 int i;
  
 Xc_HISTORY(("selectItem(`%s')", XcIL_ITEM_NAME(this, item)));
 for(list = this->item_base, i = 1; i <= this->item_count; i++)
  if(*list++ == item)
  {
   XmListSelectPos(LIST(this), i, FALSE);
   return;
  }
 Xc_TRACE(("item `%s' not found", XcIL_ITEM_NAME(this, item))); 
} 

/* ----------------------------------------------------------------- ** 
** getFirstUnselectItem - get the first unselected item		     ** 
** ----------------------------------------------------------------- */
static void *getFirstUnselectItem(this)
c_ItemList *this;
{
 int *list, count, i, max_item, min_item;
 Xc_HISTORY(("getFirstUnselectItem"));

 if(XmListGetSelectedPos(LIST(this), &list, &count))
 {
  max_item = 0; 
  min_item = this->item_count;
  for(i=0; i<count; i++)
  {
   if (max_item < list[i])
    max_item = list[i];
   if (min_item > list[i])
    min_item = list[i];
  }
  XtFree((char *)list);
  if (max_item >= this->item_count)
  {
   if (--min_item <= 0)
    return NULL;
   else
    return this->item_base[min_item-1];
  }
  else
  {
   return this->item_base[max_item];
  }
 }
 return NULL;
} 

/* ----------------------------------------------------------------- ** 
** getSelection - Get the selected items                             ** 
** ----------------------------------------------------------------- */
static boolean getSelection(this, arrayp, countp)
c_ItemList *this;
void ***arrayp;
int *countp;
{
 int *list, count;

 Xc_HISTORY(("getSelection"));

 *arrayp = NULL;
 if(XmListGetSelectedPos(LIST(this), &list, &count))
 {
  void **buffer;
  int *ptr;

  if((*countp = count) == 0) return FALSE;
  if((buffer = Xc_malloc("selection", count * sizeof(void *))) == NULL)
  {
   XtFree((char *)list);
   return FALSE;
  }
  ptr = list;
  *arrayp = buffer;
  while(count--) *buffer++ = this->item_base[(*ptr++) - 1];
  XtFree((char *)list);
  return TRUE;
 }
 *countp = 0;
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** getState - Get the state of an item                               ** 
** ----------------------------------------------------------------- */
static boolean getState(this, item)
c_ItemList *this;
void *item;
{
 int *list, count;

 Xc_HISTORY(("getState(`%s')", XcIL_ITEM_NAME(this, item)));

 if(XmListGetSelectedPos(LIST(this), &list, &count))
 {
  int *ptr;

  ptr = list;
  while(count--)
   if(this->item_base[(*ptr++) - 1] == item)
   {
    XtFree((char *)list);
    return TRUE;
   }
  XtFree((char *)list);
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** setState - Select or Deslect items                                ** 
** ----------------------------------------------------------------- */
static void setState(this, state, mode, count, va_alist)
c_ItemList *this;
boolean state;
int mode;
int count;
va_dcl
{
 long next_offset = 0L;
 char *item_list = NULL;
 va_list ap;

 Xc_HISTORY(("setState(%d)", count));
  
 va_start(ap);

 if(this->mode == XcIL_EXTENDED_MODE)
  XtVaSetValues(LIST(this), XmNselectionPolicy, XcIL_MULTIPLE_MODE, NULL);

 /*--- Delete all items ---*/
 if(count == -1)
 {
  Xc_TRACE(("set all"));
  count = this->item_count;
  item_list = (char *)this->item_base;
  mode = XcIL_POINTER_ARRAY;
 }
 else
 {
  if((mode == XcIL_LINKED_LIST) || (mode == XcIL_ARRAY))
  {
   item_list = va_arg(ap, char *);
   next_offset = va_arg(ap, long);
  }
  else if(mode == XcIL_POINTER_ARRAY)
   item_list = va_arg(ap, char *);
 }

 /*--- Main loop ---*/
 while(count--)
 {
  void *ptr = NULL;
  void **list;
  int i;

  switch(mode)
  {
  case XcIL_ARRAY:
   ptr = (void *)item_list;
   item_list += next_offset;
   break;

  case XcIL_POINTER_ARRAY:
   ptr = *((void **)item_list);
   item_list += sizeof(void *); 
   break;

  case XcIL_LINKED_LIST:
   if((ptr = item_list) == NULL) break;
   item_list = *((void **)XcIL_POINTER(item_list, next_offset));
   break;

  case XcIL_VA_LIST:
   ptr = va_arg(ap, void *);
   break;
  }

  if(ptr == NULL) break;

  for(list = this->item_base, i = 1; i <= this->item_count; i++)
   if(*list++ == ptr)
   {
    Xc_TRACE(("setState `%s'", XcIL_ITEM_NAME(this, ptr)));
    if(state)
     XmListSelectPos(LIST(this), i, False);
    else
     XmListDeselectPos(LIST(this), i);
    break;
   }
 }
 va_end(ap);

 if(this->mode == XcIL_EXTENDED_MODE)
  XtVaSetValues(LIST(this), XmNselectionPolicy, XcIL_EXTENDED_MODE, NULL);
}


/* ----------------------------------------------------------------- ** 
** updateItems - Update items name in list                           ** 
** ----------------------------------------------------------------- */
static boolean updateItems(this, mode, count, va_alist)
c_ItemList *this;
int mode;
int count;
va_dcl
{
 long next_offset = 0L;
 char *item_list = NULL;
 int *sellist, selcount;
 va_list ap;

 Xc_HISTORY(("updateItems(%d)", count));
  
 va_start(ap);

 /*--- Delete all items ---*/
 if(count == -1)
 {
  Xc_TRACE(("update all"));
  count = this->item_count;
  item_list = (char *)this->item_base;
  mode = XcIL_POINTER_ARRAY;
 }
 else
 {
  if((mode == XcIL_LINKED_LIST) || (mode == XcIL_ARRAY))
  {
   item_list = va_arg(ap, char *);
   next_offset = va_arg(ap, long);
  }
  else if(mode == XcIL_POINTER_ARRAY)
   item_list = va_arg(ap, char *);
 }

 if(this->sort_func == NULL)
  if(!XmListGetSelectedPos(LIST(this), &sellist, &selcount)) selcount = 0;
  
 /*--- Main loop ---*/
 while(count--)
 {
  void *ptr = NULL;
  void **list;
  int i;

  switch(mode)
  {
  case XcIL_ARRAY:
   ptr = (void *)item_list;
   item_list += next_offset;
   break;

  case XcIL_POINTER_ARRAY:
   ptr = *((void **)item_list);
   item_list += sizeof(void *); 
   break;

  case XcIL_LINKED_LIST:
   if((ptr = item_list) == NULL) break;
   item_list = *((void **)XcIL_POINTER(item_list, next_offset));
   break;

  case XcIL_VA_LIST:
   ptr = va_arg(ap, void *);
   break;
  }

  if(ptr == NULL) break;

  for(list = this->item_base, i = 1; i <= this->item_count; i++)
   if(*list++ == ptr)
   {
    Xc_TRACE(("update `%s'", XcIL_ITEM_NAME(this, ptr)));

    if(this->sort_func == NULL)
    {
     XmString str;

     str = XmStringCreateSimple(XcIL_ITEM_NAME(this, ptr));
     if(str == NULL)
     {
      va_end(ap);
      if(selcount) XtFree((char *)sellist);
      return FALSE;
     }
     XmListReplaceItemsPos(LIST(this), &str, 1, i);
     XmStringFree(str);
    }
    else
    {
     boolean state;

     state = F(this).getState(this, ptr);
     if(i != this->item_count)
      memmove(list - 1, list,
	      (this->item_count - i) * sizeof(void *));
     XmListDeletePos(LIST(this), i);
     this->item_count--;
     if(!F(this).addItems(this, state, XcIL_VA_LIST, 1, ptr))
     {
      va_end(ap);
      return FALSE;
     }
    }
    break;
   }
 }

 if((this->sort_func == NULL) && selcount)
 {
  int *ptr;
      
  if(this->mode == XcIL_EXTENDED_MODE)
   XtVaSetValues(LIST(this),
		 XmNselectionPolicy, XcIL_MULTIPLE_MODE, NULL);
  for(ptr = sellist; selcount--; )
   XmListSelectPos(LIST(this), *ptr++, False);
  if(this->mode == XcIL_EXTENDED_MODE)
   XtVaSetValues(LIST(this),
		 XmNselectionPolicy, XcIL_EXTENDED_MODE, NULL);
  XtFree((char *)sellist);
 }
 va_end(ap);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setCallback - Set callback function                               ** 
** ----------------------------------------------------------------- */
static void setCallback(this, cb_function, cb_data)
c_ItemList *this;
XcItemListCallbackProc cb_function;
void *cb_data;
{
 Xc_HISTORY(("addCallback"));

 this->cb_function = cb_function;
 this->cb_data = cb_data;
 XtAddCallback(LIST(this), XmNdefaultActionCallback,
	       (XtCallbackProc)cb_list, this);
 XtAddCallback(LIST(this), XmNbrowseSelectionCallback,
	       (XtCallbackProc)cb_list, this);
 XtAddCallback(LIST(this), XmNsingleSelectionCallback,
	       (XtCallbackProc)cb_list, this);
 XtAddCallback(LIST(this), XmNmultipleSelectionCallback,
	       (XtCallbackProc)cb_list, this);
 XtAddCallback(LIST(this), XmNextendedSelectionCallback,
	       (XtCallbackProc)cb_list, this);
}


/* ----------------------------------------------------------------- ** 
** removeCallback - Remove callcallback                              ** 
** ----------------------------------------------------------------- */
static void removeCallback(this)
c_ItemList *this;
{
 Xc_HISTORY(("removeCallback"));

 this->cb_function = NULL;;
 this->cb_data = NULL;
 XtRemoveCallback(LIST(this), XmNdefaultActionCallback,
		  (XtCallbackProc)cb_list, this);
 XtRemoveCallback(LIST(this), XmNbrowseSelectionCallback,
		  (XtCallbackProc)cb_list, this);
 XtRemoveCallback(LIST(this), XmNsingleSelectionCallback,
		  (XtCallbackProc)cb_list, this);
 XtRemoveCallback(LIST(this), XmNmultipleSelectionCallback,
		  (XtCallbackProc)cb_list, this);
 XtRemoveCallback(LIST(this), XmNextendedSelectionCallback,
		  (XtCallbackProc)cb_list, this);
}


/* ----------------------------------------------------------------- ** 
** cb_list - Internal ScrolledList callback                          ** 
** ----------------------------------------------------------------- */
static void cb_list(w, this, cbs)
Widget w;
c_ItemList *this;
XmListCallbackStruct *cbs;
{
 Xc_TRACE(("callback %d", cbs->reason));
 if(this->cb_function == NULL) return;

 if(cbs->reason == XmCR_DEFAULT_ACTION)
  this->cb_function(this, this->cb_data, XcIL_DOUBLE_CLICK, NULL);
 else
  this->cb_function(this, this->cb_data, XcIL_SIMPLE_CLICK);
}





