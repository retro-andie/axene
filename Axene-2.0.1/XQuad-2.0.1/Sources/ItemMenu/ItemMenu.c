/*
** ItemMenu.c for Xclamation, XQuad, XAllWrite, XMayday and XInstall in 
** 	ItemMenu/
** Methods for the ItemMenu class
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
** Last update Tue Jan 25 00:13:02 2000 Emmanuel Paris
*/

/*#define NTRACE*/

#include <varargs.h>
#include "ItemMenu.h"
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/LabelGP.h>
#include "RegisterHelp.h"

#if XmVERSION == 1
extern Boolean _XmFontListGetDefaultFont ___PROTO((XmFontList fontlist,
						   XFontStruct **font_struct));
#endif

/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();

static void setCallback ___PROTO((c_ItemMenu *this,
				  void (*cb_func) ___PROTO((c_ItemMenu *this,
							    void *data,
							    void *item)),
				  void *cb_data));

static boolean add_items();
static void delete_items();
static boolean updateItems();
static void deleteAllItems();
static void calculSize ___PROTO((c_ItemMenu *this, int maxsize));

static void set ___PROTO((c_ItemMenu *this,
			  boolean name_flag, long name_offset,
			  int (*sort) ___PROTO((void *, void *)),
			  boolean (*filter) ___PROTO((void *))));
static void *getSelection ___PROTO((c_ItemMenu *this));
static void setSelection ___PROTO((c_ItemMenu *this, void *item));
static void cb_activate ___PROTO((Widget w, c_ItemMenu *this));
#if XmVERSION == 1
static void cb_resize_on_map ___PROTO((Widget w, c_ItemMenu *this, 
				       XEvent	*event));
#endif
static Cardinal order_proc ___PROTO((Widget w));
static Cardinal XcIM_insertPosition = 0;

sf_ItemMenu fc_ItemMenu =
{
 constructor, destructor, copy,
 set, 
 setCallback, getSelection, setSelection,
 (boolean (*) ___PROTO((c_ItemMenu *, int, int, ...))) add_items,
 (void (*) ___PROTO((c_ItemMenu *, int, int, ...))) delete_items,
 (boolean (*) ___PROTO((c_ItemMenu *, int, int, ...))) updateItems,
 (void (*) ___PROTO((c_ItemMenu *))) deleteAllItems,
 calculSize
};

/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(parent, manager, name, width, height)
Widget parent;
c_ManageWidget *manager;
char *name;
int width;
int height;
{
 c_ItemMenu *p;
 XmString xstr; 
 Arg args[3];
 int narg, erase_label;

 Xc_HISTORY(("constructor"));

 if((p = Xc_malloc("ItemMenu", sizeof(c_ItemMenu))) == NULL)
  return NULL;
 p->f = &fc_ItemMenu; 

 p->item_base = p->selection = NULL;
 p->buffer_len = p->item_count = 0;
 p->sort_func = NULL;
 p->filter_func = NULL;
 p->cb_function = NULL;
 p->cb_data = NULL;
 p->staticSize = FALSE;
 if (name && *name == '*')
  erase_label = 1;
 else
  erase_label = 0;
 MENUPANE(p) = XmCreatePulldownMenu(parent, "item_menu", NULL, 0);
 XtSetArg(args[0], XmNinsertPosition, order_proc);
 XtSetValues(MENUPANE(p), args, 1);
  
 XtSetArg(args[0], XmNsubMenuId, MENUPANE(p));
 narg = 1;
 if (width > 0)  { XtSetArg(args[narg], XmNwidth, width); narg++; }
 if (height > 0) { XtSetArg(args[narg], XmNheight, height); narg++; }
  
 WIDGET(p) = XmCreateOptionMenu(parent, name + erase_label, args, narg);

 TITLELABEL(p) = XmOptionLabelGadget(WIDGET(p));
 CASCADEBUTTON(p) = XmOptionButtonGadget(WIDGET(p));

#ifdef LESSTIF_VERSION
 XtVaSetValues(WIDGET(p), XmNshadowThickness, 0, NULL);
 /* shadowThickness is ignored in motif for this type of rowcolumn */
#endif

 Xc_REGISTER_HELP(WIDGET(p));
 Xc_REGISTER_HELP(TITLELABEL(p));
 Xc_REGISTER_HELP(CASCADEBUTTON(p));
 Xc_REGISTER_HELP(MENUPANE(p));
  
 if (erase_label == 0)
 {
  xstr = XmStringCreateSimple("");
  XtVaSetValues(TITLELABEL(p), XmNlabelString, xstr, NULL);
  XmStringFree(xstr);
 }
 
#if XmVERSION == 1
 XtAddEventHandler(WIDGET(p), StructureNotifyMask, False,
		   (void *)cb_resize_on_map, p);
#endif

 if (manager)
  F(manager).Add_child(manager, WIDGET(p), parent);

 return p;
}

/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_ItemMenu *this;
{
 item_menu_button_t *ptr;
 int i;

 Xc_HISTORY(("destructor"));

 for(i = this->item_count, ptr = this->item_base; i--; ptr++)
  XtDestroyWidget(ptr->w);

 if(this->item_base) Xc_free(this->item_base);
#if defined(OLD_MOTIF)
 XtDestroyWidget(MENUPANE(this)); 
 XtDestroyWidget(TITLELABEL(this));
 XtDestroyWidget(CASCADEBUTTON(this));
#endif
 XtDestroyWidget(WIDGET(this));
 Xc_free(this);
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_ItemMenu *this;
{
 c_ItemMenu *p;

 Xc_HISTORY(("copy"));

 if((p = Xc_malloc("CItemMenu", sizeof(c_ItemMenu))) == NULL)
  return NULL;

 memcpy(p, this, sizeof(c_ItemMenu));
 return p; 
}


/* ----------------------------------------------------------------- ** 
** set - Remanage ItemMenu                                           ** 
** ----------------------------------------------------------------- */
static void set(this, name_flag, name_offset, sort_func, filter_func)
c_ItemMenu *this;
boolean name_flag;
long name_offset;
int (*sort_func) ___PROTO((void *, void *));
boolean (*filter_func) ___PROTO((void *));
{
 Xc_HISTORY(("set"));

 if(sort_func != (int (*)())-1) this->sort_func = sort_func;
 if(filter_func != (boolean (*)())-1) this->filter_func = filter_func;
 if(name_offset != -1L)
 {
  this->name_deference_flag = name_flag;
  this->name_offset = name_offset;
 }
}


/* ----------------------------------------------------------------- ** 
** addItems - Add new items in the ItemMenu                          ** 
** ----------------------------------------------------------------- */
static boolean add_items(this, mode, count, va_alist)
c_ItemMenu *this;
int mode;
int count;
va_dcl
{
 long next_offset = 0L;
 char *item_list = NULL;
 int new_count;
 va_list ap;

 Xc_HISTORY(("addItems(%d)", count));
 Xc_HISTORY(("nb item before: %d", this->item_count));

 /*--- Initialize item list data ---*/
 va_start(ap);
 if((mode == XcIM_LINKED_LIST) || (mode == XcIM_ARRAY))
 {
  item_list = va_arg(ap, char *);
  next_offset = va_arg(ap, long);
 }
 else if(mode == XcIM_POINTER_ARRAY)
  item_list = va_arg(ap, char *);

 if((mode == XcIM_LINKED_LIST) && (count <= 0))
 {
  void *ptr;

  ptr = item_list;
  count = 0;
  while(ptr != NULL)
  {
   count++;
   ptr = *((void **)XcIM_POINTER(ptr, next_offset));
  }
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
  item_menu_button_t *new;

  new = Xc_realloc("ItemMenu items", this->item_base,
		   new_count * sizeof(item_menu_button_t));
  if(new == NULL)
  {
   va_end(ap);
   return FALSE;
  }
  this->buffer_len = new_count;
  this->item_base = new;
 }
	
 /*--- Main loop ---*/
 while(count--)
 {
  char *new_name;
  XmString str;
  void *ptr = NULL;
  Widget w;

  switch(mode)
  {
  case XcIM_ARRAY:
   ptr = (void *)item_list;
   item_list += next_offset;
   break;

  case XcIM_POINTER_ARRAY:
   ptr = *((void **)item_list);
   item_list += sizeof(void *); 
   break;

  case XcIM_LINKED_LIST:
   if((ptr = item_list) == NULL) break;;
   item_list = *((void **)XcIM_POINTER(item_list, next_offset));
   break;

  case XcIM_VA_LIST:
   ptr = va_arg(ap, void *);
   break;
  }

  if(ptr == NULL) break;
  if((this->filter_func != NULL) &&
     (this->filter_func(ptr) == FALSE)) continue;

  new_name = XcIM_ITEM_NAME(this, ptr);
  Xc_TRACE(("name `%s'", new_name));
  if((str = XmStringCreateSimple(new_name)) == NULL)
  {
   va_end(ap);
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
    text = XcIM_ITEM_NAME(this, this->item_base[i].item);
    if(this->sort_func(this->item_base[i].item, ptr) > 0)
     u_bound = i - 1;
    else
     l_bound = i + 1;
   }
   if((i = this->item_count - l_bound) != 0)
    memmove(this->item_base + l_bound + 1,
	    this->item_base + l_bound,
	    i * sizeof(item_menu_button_t));
   XcIM_insertPosition = l_bound;
  }
  else
   XcIM_insertPosition = this->item_count;

  Xc_TRACE(("insert %d/%d", (int)XcIM_insertPosition, (int)this->item_count));
  w = XtVaCreateManagedWidget("button", xmPushButtonWidgetClass,
			      MENUPANE(this),
			      XmNlabelType, XmSTRING,
			      XmNlabelString, str,
			      NULL);
  XtAddCallback(w, XmNactivateCallback, (void *)cb_activate, this);

  XmStringFree(str);
  if(w == NULL)
  {
   va_end(ap);
   return FALSE;
  }
      
  this->item_base[XcIM_insertPosition].item = ptr;
  this->item_base[XcIM_insertPosition].w = w;
	
  this->item_count++;
 }
 va_end(ap);

 if(this->selection == NULL)
 {
  if (this->item_count > 1)
  {
   this->selection = &this->item_base[1];
   F(this).setSelection(this, this->item_base[0].item);
  }
  else
  {
   this->selection = &this->item_base[0];
  }
 }
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** deleteItems - Delete some item in the ItemMenu                    ** 
** ----------------------------------------------------------------- */
static void delete_items(this, mode, count, va_alist)
c_ItemMenu *this;
int mode;
int count;
va_dcl
{
 Widget selected_w;
 long next_offset = 0L;
 char *item_list = NULL;
 va_list ap;

 Xc_HISTORY(("deleteItems(%d)", count));

 /*--- Delete all items ---*/
 if(count >= this->item_count)
 {
  Xc_TRACE(("delete all"));
  count = this->item_count;
 }      

 /*--- Initialize ---*/
 va_start(ap);
 if((mode == XcIM_LINKED_LIST) || (mode == XcIM_ARRAY))
 {
  item_list = va_arg(ap, char *);
  next_offset = va_arg(ap, long);
 }
 else if(mode == XcIM_POINTER_ARRAY)
  item_list = va_arg(ap, char *);
	
 XtVaGetValues(WIDGET(this), XmNmenuHistory, &selected_w, NULL);

 /*--- Main loop ---*/
 while(count--)
 {
  item_menu_button_t *list;
  void *ptr = NULL;
  int i;

  switch(mode)
  {
  case XcIM_ARRAY:
   ptr = (void *)item_list;
   item_list += next_offset;
   break;

  case XcIM_POINTER_ARRAY:
   ptr = *((void **)item_list);
   item_list += sizeof(void *); 
   break;

  case XcIM_LINKED_LIST:
   if((ptr = item_list) == NULL) break;
   item_list = *((void **)XcIM_POINTER(item_list, next_offset));
   break;

  case XcIM_VA_LIST:
   ptr = va_arg(ap, void *);
   break;
  }

  if(ptr == NULL) break;

  for(list = this->item_base, i = 1; i <= this->item_count; i++, list++)
   if(list->item == ptr)
   {
    Xc_TRACE(("delete `%s'", XcIM_ITEM_NAME(this, ptr)));
    if(list->w == selected_w)
    {
     Xc_TRACE(("delete selection"));
     if(i > 1)
      F(this).setSelection(this, list[-1].item);
     else if(i < this->item_count)
      F(this).setSelection(this, list[1].item);
     else
      XtVaSetValues(WIDGET(this),
		    XmNmenuHistory, NULL,
		    NULL);
    }
    XtDestroyWidget(list->w);
    if(i != this->item_count)
     memmove(list, list + 1,
	     (this->item_count - i) * sizeof(item_menu_button_t));
    this->item_count--;
    break;
   }
 }
 va_end(ap);
}

/* ----------------------------------------------------------------- ** 
** deleteAllItems - Delete all item in the ItemMenu                  ** 
** ----------------------------------------------------------------- */
static void deleteAllItems(this)
c_ItemMenu *this;
{
 int i;
 item_menu_button_t *list;
  
 Xc_HISTORY(("DeleteAllItems"));
#ifdef LESSTIF_VERSION
 XtVaSetValues(WIDGET(this), XmNresizeWidth, False,
	       XmNmenuHistory, TITLELABEL(this), NULL);
#else
 XtVaSetValues(WIDGET(this), XmNmenuHistory, NULL, NULL);
#endif
 list = this->item_base;
 for(i = 0; i < this->item_count; i++)
 {
  XtUnmanageChild(list->w);
  XtDestroyWidget(list->w);
  list++;
 }
 memset(this->item_base, 0, this->item_count * sizeof(item_menu_button_t));
 this->item_count = 0;
 this->selection = NULL;
}

/* ----------------------------------------------------------------- ** 
** updateItems - Update items name in list                           ** 
** ----------------------------------------------------------------- */
static boolean updateItems(this, mode, count, va_alist)
c_ItemMenu *this;
int mode;
int count;
va_dcl
{
#if 0
 long next_offset;
 char *item_list;
 int *sellist, selcount;
 va_list ap;
#endif
 Xc_HISTORY(("updateItems(%d)", count));
#if 0
 va_start(ap);

 /*--- Delete all items ---*/
 if(count == -1)
 {
  Xc_TRACE(("update all"));
  count = this->item_count;
  item_list = (char *)this->item_base;
  mode = XcIM_POINTER_ARRAY;
 }
 else
 {
  if((mode == XcIM_LINKED_LIST) || (mode == XcIM_ARRAY))
  {
   item_list = va_arg(ap, char *);
   next_offset = va_arg(ap, long);
  }
  else if(mode == XcIM_POINTER_ARRAY)
   item_list = va_arg(ap, char *);
 }

 /*--- Main loop ---*/
 while(count--)
 {
  void *ptr, **list;
  int i;

  switch(mode)
  {
  case XcIM_ARRAY:
   ptr = (void *)item_list;
   item_list += next_offset;
   break;

  case XcIM_POINTER_ARRAY:
   ptr = *((void **)item_list);
   item_list += sizeof(void *); 
   break;

  case XcIM_LINKED_LIST:
   if((ptr = item_list) == NULL) break;
   item_list = *((void **)XcIM_POINTER(item_list, next_offset));
   break;

  case XcIM_VA_LIST:
   ptr = va_arg(ap, void *);
   break;
  }

  if(ptr == NULL) break;

  for(list = this->item_base, i = 1; i <= this->item_count; i++)
   if(*list++ == ptr)
   {
    Xc_TRACE(("update `%s'", XcIM_ITEM_NAME(this, ptr)));

    if(this->sort_func == NULL)
    {
     XmString str;

     str = XmStringCreateSimple(XcIM_ITEM_NAME(this, ptr));
     if(str == NULL)
     {
      va_end(ap);
      return FALSE;
     }
     XmListReplaceItemsPos(LIST(this), &str, 1, i);
     XmStringFree(str);
    }
    else
    {
     boolean state;

     if(i != this->item_count)
      memmove(list - 1, list,
	      (this->item_count - i) * sizeof(void *));
     XmListDeletePos(LIST(this), i);
     this->item_count--;
     if(!F(this).addItems(this, state, XcIM_VA_LIST, 1, ptr))
     {
      va_end(ap);
      return FALSE;
     }
    }
    break;
   }
 }

 va_end(ap);
#endif
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setCallback - Set callback function                               ** 
** ----------------------------------------------------------------- */
static void setCallback(this, cb_function, cb_data)
c_ItemMenu *this;
void (*cb_function) ___PROTO((c_ItemMenu *this, void *data, void *item));
void *cb_data;
{
 Xc_HISTORY(("addCallback"));

 this->cb_function = cb_function;
 this->cb_data = cb_data;
}


/* ----------------------------------------------------------------- ** 
** getSelection - Get the selected item                              ** 
** ----------------------------------------------------------------- */
static void *getSelection(this)
c_ItemMenu *this;
{
 return (this->selection != NULL)? this->selection->item : NULL;
}


/* ----------------------------------------------------------------- ** 
** setSelection - Set the selected item                              ** 
** ----------------------------------------------------------------- */
static void setSelection(this, item)
c_ItemMenu *this;
void *item;
{
 item_menu_button_t *list;
 int i;

 Xc_HISTORY(("setSelection(`%s')", XcIM_ITEM_NAME(this, item)));

 if((this->selection != NULL) && (this->selection->item == item))
 {
  Xc_TRACE(("already selected"));
  /*return;*/
 }

 for(list = this->item_base, i = 0; i < this->item_count; i++, list++)
  if(list->item == item)
  {
   XmString str;

   Xc_TRACE(("set"));
   XtVaSetValues(WIDGET(this), XmNmenuHistory, list->w, NULL);
   if((str = XmStringCreateSimple(XcIM_ITEM_NAME(this, item))) != NULL)
   {
    XtVaSetValues(CASCADEBUTTON(this), XmNlabelString, str, NULL);
#if XmVERSION == 1
    if (!this->staticSize)
    {
     XmFontList fontlist;
     XFontStruct *fs = (XFontStruct *) NULL;
     Dimension height, borderwidth;
     int width;

     XtVaGetValues(CASCADEBUTTON(this),XmNfontList, &fontlist,
		   XmNheight, &height, XmNborderWidth, &borderwidth, NULL);
     _XmFontListGetDefaultFont(fontlist, &fs);
     if (fs)
     {
      width = XTextWidth(fs, XcIM_ITEM_NAME(this, item), 
			 strlen(XcIM_ITEM_NAME(this, item))) + CBUTTON_SIZE;
      XtResizeWidget(CASCADEBUTTON(this), (Dimension)width, height, 
		     borderwidth);
     }
    }
#endif
    XmStringFree(str);
   }
   this->selection = list;
   return;
  }
 Xc_WARNING(("item not found"));
}

/* ----------------------------------------------------------------- ** 
** calculSize - adjust ItemMenu size to its largest item             ** 
** ----------------------------------------------------------------- */
static void calculSize(this, maxsize)
c_ItemMenu *this;
int maxsize;
{
#if XmVERSION == 1
 item_menu_button_t *list;
 XmFontList fontlist;
 XFontStruct *fs = (XFontStruct *) NULL;
 Dimension cbheight, cbborderwidth, height, borderwidth, marginwidth, rcwidth;
 int i, width, maxwidth = 0;
 char *name;
 
 XtVaGetValues(CASCADEBUTTON(this), XmNfontList, &fontlist,
	       XmNheight, &cbheight, XmNborderWidth, &cbborderwidth, NULL);
 _XmFontListGetDefaultFont(fontlist, &fs);

 if (maxsize <= 0) maxsize = 1000000; else maxsize -= CBUTTON_SIZE;
 
 for(list = this->item_base, i = 0; i < this->item_count; i++, list++)
 {
  name = XcIM_ITEM_NAME(this, list->item);
 
  width = XTextWidth(fs, name, strlen(name));
  if (width > maxwidth)
  {
   maxwidth = width;
   if (maxwidth > maxsize) { maxwidth = maxsize; break; }
  }
 }
 
 if (maxwidth > 0)
 {
  maxwidth += CBUTTON_SIZE;
  XtVaGetValues(TITLELABEL(this), XmNwidth, &rcwidth, NULL);
  
  XtVaGetValues(WIDGET(this), XmNmarginWidth, &marginwidth,
		XmNheight, &height, XmNborderWidth, &borderwidth, NULL);
  
  rcwidth += (Dimension)maxwidth + 3 * marginwidth;
  XtResizeWidget(WIDGET(this), rcwidth, height, borderwidth);

  XtResizeWidget(CASCADEBUTTON(this), (Dimension)maxwidth, cbheight, 
		 cbborderwidth);
  
  
  XtVaGetValues(XtParent(WIDGET(this)), XmNwidth, &rcwidth,
		XmNheight, &height, XmNborderWidth, &borderwidth, NULL);
  
  XtResizeWidget(XtParent(WIDGET(this)), --rcwidth, height, borderwidth);
  XtResizeWidget(XtParent(WIDGET(this)), ++rcwidth, height, borderwidth);

  XtVaSetValues(MENUPANE(this), XmNresizeWidth, False, NULL);
  this->staticSize = TRUE;
 }
#endif
}
/* ----------------------------------------------------------------- ** 
** cb_activate - Internal callback                                   ** 
** ----------------------------------------------------------------- */
static void cb_activate(w, this)
Widget w;
c_ItemMenu *this;
{
 int i;

 Xc_TRACE(("callback"));
  
#ifndef NTRACE
 if(this->selection != NULL)
  Xc_TRACE(("(old item `%s')", XcIM_ITEM_NAME(this, this->selection->item)));
#endif
 for(i = 0; i < this->item_count; i++)
  if(this->item_base[i].w == w)
  {
   if(this->selection == this->item_base + i) return;
   this->selection = this->item_base + i;
   Xc_TRACE(("item `%s'", XcIM_ITEM_NAME(this, this->selection->item)));
   if(this->cb_function != NULL)
    this->cb_function(this, this->cb_data, this->selection->item);
   return;
  }
}

#if XmVERSION == 1
/* ----------------------------------------------------------------- ** 
** cb_resize_on_map - map callback                                   ** 
** ----------------------------------------------------------------- */
static void cb_resize_on_map(w, this, event)
Widget w;
c_ItemMenu *this;
XEvent	*event;
{
 
 if (event->type == MapNotify)
 {
  Xc_TRACE(("map callback"));
  if (this->staticSize)
  {
   calculSize(this, 0);
  }
 }
}
#endif

/* ----------------------------------------------------------------- ** 
** order_proc - Sorting widget proc                                  ** 
** ----------------------------------------------------------------- */
static Cardinal order_proc(w)
Widget w;
{
 return XcIM_insertPosition;
}
