/*
** Text.c for XAllWrite in Text/
** Text Object. Manage Document's text blocks.
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Wed Mar 26 14:14:12 1997 Emmanuel Paris
** Last update Wed Nov 11 19:10:09 1998 Emmanuel Paris
*/

#include "Text.h"
#include "TextP.h"
#include "char_translate.h"

static void *cons_Text();
static void dest_Text();
static void *copy_Text();
static void insert_Text ___PROTO((c_Text *This, int item_type, void *data));
static void delete_Text ___PROTO((c_Text *This));
static void goto_Text ___PROTO((c_Text *This, int type, int direction, 
				int count, xa_item_t *item, int pos, 
				boolean select));
static xa_item_t *cut_line_Text ___PROTO((c_Text *This, xa_item_t *item,
					  int pos));
static void pack_Text ___PROTO((c_Text *This, xa_item_t **item));
static void or_tstyle_Text ___PROTO((c_Text *This, c_TextStyle *(*cb_func)(),
				     void *page, void *cb_data));
static void or_truler_Text ___PROTO((c_Text *This, c_TextRuler *(*cb_func)(),
				     void *page, void *cb_data));
static c_TextRuler *get_ruler_Text ___PROTO((c_Text *This));
static void clip_purge_Text ___PROTO((c_Text *This));
static void clip_copy_Text ___PROTO((c_Text *This, c_Text *clipboard));
static void clip_paste_Text ___PROTO((c_Text *This, c_Text *clipboard));
static void select_all_Text ___PROTO((c_Text *This));

static void delete_item ___PROTO((c_Text *This, xa_item_t *item));
static void simple_delete_item ___PROTO((c_Text *This, xa_item_t *item));
xa_item_t *create_item ___PROTO((c_Text *This, int type, void *data,
				 xa_item_t *item));
static xa_item_t *break_Text ___PROTO((c_Text *This, xa_item_t *item,
				       int pos));
static xa_item_t *duplicate_item ___PROTO((c_Text *This, xa_item_t *item));
static xa_item_t *push_in_changed_stack ___PROTO((c_Text *This,
						  xa_item_t *item,
						  boolean dup));
static void purge_changed_items ___PROTO((c_Text *This));
static boolean need_undo_Text ___PROTO((c_Text *This, int nb_char));
static boolean check_diff_text_item ___PROTO((c_Text *This, xa_item_t *item1,
					      xa_item_t *item2, int *pos));
static void check_begin_style ___PROTO((c_Text *This));
/* debug function */
static void console();

static BaseStd_t clipboard_base =
{
 NULL, NULL, NULL, NULL
};

static char *xa_text_name[] = 
{
 "Body", "Clipboard", "Header", "Footer", "Footnotes"
};

sf_Text fc_Text =
{
 cons_Text,
 dest_Text,
 copy_Text,
 insert_Text,
 delete_Text,
 goto_Text,
 cut_line_Text,
 pack_Text,
 or_tstyle_Text,
 or_truler_Text,
 clip_copy_Text,
 clip_paste_Text,
 select_all_Text,
 check_diff_text_item,
 purge_changed_items,
 need_undo_Text,
 readText, writeText, 
 readTextList, writeTextList,
};

#define BE_BEFORE(item, pos) \
if (!pos) \
{ \
 item = item->any.prev; \
 if (item->type == XA_TEXT) \
  pos = item->text.text_len; \
 else \
  pos = 1; \
}

#define BE_AFTER(item, pos) \
if (pos) \
{ \
 if (item->type != XA_TEXT || pos == item->text.text_len) \
 { \
   pos = 0; \
   item = item->any.next; \
  } \
}

#define IS_AFTER(item, pos) \
((item->type == XA_TEXT && pos == item->text.text_len) || \
 (item->type != XA_TEXT && pos == 1)) 

#define BE_IN_RIGHT_PLACE(item, pos) \
switch(item->type) \
{ \
 case XA_BEGIN: \
 if (!pos) pos = 1; \
 break; \
 case XA_LINEFEED: \
 case XA_PAGEBREAK: \
 if (pos) \
 { \
  item = item->any.next; \
  pos = 0; \
 } \
 break; \
 case XA_END: \
 if (pos) pos = 0; \
 break; \
}


#define GET_PREVIOUS_ITEM(item, pos) \
item = item->any.prev; \
switch(item->type) \
{ \
 case XA_TEXT: \
 pos = item->text.text_len; \
 break; \
 default: \
 pos = 1; \
 break; \
}

#define GET_SELECTION_STYLE \
if (This->select_start != This->select_end) \
{ \
  if (This->start_end) \
  { \
    if ((This->select_start->type == XA_TEXT && \
	 This->select_start_pos == This->select_start->text.text_len) || \
	(This->select_start->type != XA_TEXT && This->select_start_pos == 1)) \
      This->style = This->select_start->any.next->any.style; \
    else \
      This->style = This->select_start->any.style; \
  } \
  else \
   This->style = This->select_end->any.style; \
} \
else \
{ \
   This->style = This->select_start->any.style; \
}

static void *cons_Text(BaseStd, type)
BaseStd_t       *BaseStd;
int		type;
{
 xa_item_t *item;
 c_Text *This;

 Xc_HISTORY(("Constructor Text..."));

 if((This = (c_Text *)Xc_malloc("Text", sizeof (c_Text))) == NULL)
  return NULL;
 memset(This, 0, sizeof(c_Text));
 This->f= &fc_Text;

 if (type == Xa_CLIPBOARD)
 {
  This->BaseStd = BaseStd = &clipboard_base;
  
  This->BaseStd->color_base = (c_Color *)NEW(c_Color)
   (&This->BaseStd->color_base, "Unknown");
  This->BaseStd->text_style_base = (c_TextStyle *)NEW(c_TextStyle)
   (&This->BaseStd->text_style_base, "Unknown", NULL, SCALE_ZERO, 
    This->BaseStd->color_base);
  This->BaseStd->text_ruler_base = (c_TextRuler *)NEW(c_TextRuler)
   (&This->BaseStd->text_ruler_base, "Unknown");
 }
 else
  This->BaseStd = BaseStd;

 This->base = &BaseStd->text_base;
 This->next = BaseStd->text_base;
 BaseStd->text_base = This;

 This->type = type;
 strcpy(This->name, xa_text_name[type]);

 This->i_handle = Xc_createItem("xa_item", 100, sizeof(xa_item_t));

 This->style	= This->BaseStd->text_style_base;

/* if (This->BaseStd->text_ruler_base)
  This->ruler	= This->BaseStd->text_ruler_base;
 else */
 This->ruler	= This->BaseStd->text_ruler_base;
 
 item = (xa_item_t *)Xc_new(This->i_handle);
 item->any.type = XA_BEGIN;
 item->any.prev = NULL;
 item->any.style = This->style;
 item->linefeed.ruler = This->ruler;
 F(This->style).markUsed(This->style);
 F(This->ruler).markUsed(This->ruler);

 This->item_base = item;
 
 This->select_start = This->select_end = item;
 This->select_start_pos = This->select_end_pos = 1;
 This->start_end = TRUE;
 This->changed_start = This->changed_end = This->changed_stack = NULL;

 item = (xa_item_t *)Xc_new(This->i_handle);
 item->any.type = XA_END;
 item->any.next = NULL;
 item->any.style = This->style;
 F(This->style).markUsed(This->style);
 
 item->any.prev = This->item_base;
 This->item_base->any.next = item;

 Xc_TRACE(("Constructor Text done"));
 return This;
}

static void dest_Text(This)
c_Text *This;
{
 xa_item_t	*item, *item2;
 
 Xc_HISTORY(("Destructor Text..."));
 
 item = This->item_base->any.next;
 while(item->type != XA_END)
 {
  item2 = item->any.next;
  delete_item(This, item);
  item = item2;
 }
 purge_changed_items(This);
 
 Xc_destroyItem(This->i_handle);

 if (This->BaseStd == &clipboard_base)
 {
  Xc_TRACE(("Destroy clipboard BaseStd"));
  
  /*--- Destroy the document text ruler list ---*/
  while(clipboard_base.text_ruler_base != NULL)
   DELETE(c_TextRuler)(clipboard_base.text_ruler_base);
  
  /*--- Destroy the document text style list ---*/
  while(clipboard_base.text_style_base != NULL)
   DELETE(c_TextStyle)(clipboard_base.text_style_base);
  
 /*--- Destroy the document color database ---*/
  while(clipboard_base.color_base != NULL)
   DELETE(c_Color)(clipboard_base.color_base);
 }

 /*--- Remove from linked list ---*/
 if (This->base != NULL)
 {
  c_Text **ptr;

  for(ptr = This->base; *ptr != This; ptr = &((*ptr)->next))
   Xc_ASSERT(*ptr != NULL);
  *ptr = This->next;
 }
 
 Xc_free(This);
 Xc_TRACE(("Text Destroyed"));
}

static void *copy_Text(Orig)
c_Text *Orig;
{
 Xc_TRACE(("Copy Text not done."));
 return NULL;
}

static void insert_Text(This, item_type, data)
c_Text *This;
int item_type;
void *data;
{
 xa_item_t	*nitem = NULL;
 xa_item_t	*item;
 int		pos;
 
 if (This->select_start != This->select_end ||
     This->select_start_pos != This->select_end_pos)
 {
  delete_Text(This);
 }
 else
 {
  purge_changed_items(This);
 }
 
 item = This->select_start;
 pos = This->select_start_pos;
 BE_BEFORE(item, pos);

 switch(item->type)
 {
 case XA_TEXT:
  if (pos != item->text.text_len)
  {
   if (!This->changed_start)
    This->changed_start = This->changed_end = item;
   item = break_Text(This, item, pos);
   if (!pos) item = item->any.prev;
  }
  nitem = create_item(This, item_type, data, item);
  break;
 default:
  nitem = create_item(This, item_type, data, item);
  break;
 }
 
 if (nitem != NULL)
  switch(item_type)
  {
  case XA_TEXT:
   This->select_start = nitem;
   This->select_start_pos = nitem->text.text_len;
   break;
  default:
   This->select_start = nitem;
   This->select_start_pos = 1;
   break;
  }
 
 BE_IN_RIGHT_PLACE(This->select_start, This->select_start_pos);
 This->select_end = This->select_start;
 This->select_end_pos = This->select_start_pos;

 console(This);
}


static void delete_Text(This)
c_Text *This;
{
 xa_item_t *item, *item2;
 int pos;

 purge_changed_items(This);
 
 BE_BEFORE(This->select_start, This->select_start_pos);
 BE_BEFORE(This->select_end, This->select_end_pos);
   
 if (This->select_start == This->select_end &&
     This->select_start_pos == This->select_end_pos)
 {				/* only delete one character */

  item = This->select_start;
  pos = This->select_start_pos;

  switch(item->type)
  {
  case XA_TEXT:
   if (pos == item->text.text_len)
   {
    item = item->any.next;
    if (item->type == XA_TEXT)
     pos = 1;
   }
   else
    pos += 1;
   break;
  case XA_END:
   Xc_ERROR(("End block overtaken !!"));
   break;
  default:
   item = item->any.next;
   if (item->type == XA_TEXT)
    pos = 1;
   break;
  }
  switch(item->type)
  {
  case XA_END:
   break;
  case XA_TEXT:
   if (item->text.text_len > 1)
   {
    This->changed_start = This->changed_end = item;
    item = duplicate_item(This, item);

    item->any.next->any.prev = item;
    item->any.prev->any.next = item;

    memmove(item->text.text + pos - 1, item->text.text + pos,
	    item->text.text_len - pos + 1);
    item->text.text = (char *)Xc_realloc("dt", item->text.text, 
					 --item->text.text_len + 1);

    if (pos > 1)
    {
     This->select_start = item;
     This->select_start_pos = pos - 1;
    }
    else
    { 
     This->select_start = item->any.prev;
     if (This->select_start->type == XA_TEXT)
      This->select_start_pos = This->select_start->text.text_len;
     else
      This->select_start_pos = 1;
    }
    break;
   }
  default:		/* delete the item block */
   This->changed_start = This->changed_end = item;
   if (This->select_start == item)
   {
    This->select_start = item->any.prev;
    if (item->any.prev->type == XA_TEXT)
     This->select_start_pos = item->any.prev->text.text_len; 
    else
     This->select_start_pos = 1; 
   }
   item->any.prev->any.next = item->any.next;
   item->any.next->any.prev = item->any.prev;
   break;
  }
 }
 else
 {				/* delete the selection */
  if (This->select_start == This->select_end && 
      This->select_start->type == XA_TEXT)
  {				/* deletion in the same TEXT block */
   This->changed_start = This->changed_end = This->select_start;
   item = duplicate_item(This, This->select_start);
   item->any.prev->any.next = item;
   item->any.next->any.prev = item;
   
   memmove(item->text.text + This->select_start_pos,  
	   item->text.text + This->select_end_pos, 
	   item->text.text_len - This->select_end_pos + 1);
   
   item->text.text_len -= This->select_end_pos - This->select_start_pos;
   item->text.text = (char *)Xc_realloc("dt", item->text.text,
					item->text.text_len + 1);

   This->select_start = item;
  }
  else
  {
   This->changed_start = This->select_start; 
   This->changed_end = This->select_end;

   item = This->select_start;
   if (item->type == XA_TEXT && This->select_start_pos != item->text.text_len)
   {
    This->select_start = item = duplicate_item(This, item);
    item->any.prev->any.next = item;

    item->text.text[This->select_start_pos] = '\0';
    item->text.text_len = This->select_start_pos;
    item->text.text = (char *)Xc_realloc("dt", item->text.text, 
					 item->text.text_len + 1);
   }
   else
    This->changed_start = item->any.next;
   
   item2 = item;
   item = item->any.next;
   while(item != This->select_end)
   {
    item = item->any.next;
   }
   
   if (item->type == XA_TEXT && This->select_end_pos != item->text.text_len)
   {
    item = duplicate_item(This, item);
    item->any.next->any.prev = item;
    item->any.prev = item2;
    item2->any.next = item;

    pos = This->select_end_pos;
    item->text.text_len -= pos;
    memmove(item->text.text, item->text.text + pos, item->text.text_len + 1);
    item->text.text = (char *)Xc_realloc("dt", item->text.text,
					 item->text.text_len + 1);
   }
   else
   {
    item = item->any.next;
    item->any.prev = item2;
    item2->any.next = item;
   }
   
  }
 }
 
 BE_IN_RIGHT_PLACE(This->select_start, This->select_start_pos);
 This->select_end = This->select_start;
 This->select_end_pos = This->select_start_pos;
 
 check_begin_style(This);
 
 This->style = This->select_start->any.style;
 This->ruler = get_ruler_Text(This);
 console(This);
}

static void goto_Text(This, type, direction, count, item, pos, select)
c_Text *This;
int direction;
int type;
int count;
xa_item_t *item;
int pos;
boolean select;
{
 switch(type)
 {
 case XA_WORD:
  goto_Text(This, XA_CHAR, direction, 1, NULL, 0, select);
  
 case XA_CHAR:

  if (This->select_start == This->select_end &&
      This->select_start_pos == This->select_end_pos)
  {
   if (direction == XA_LEFT)
   {
    item = This->select_start;
    pos = This->select_start_pos;
   }
   else
   {
    item = This->select_end;
    pos = This->select_end_pos;
   }
  }
  else
  {
   if (This->start_end)
   {
    item = This->select_start;
    pos = This->select_start_pos;
   }
   else
   {
    item = This->select_end;
    pos = This->select_end_pos;
   }
  }

  if (direction == XA_LEFT)
  {
   BE_AFTER(item, pos);
  }
  else
  {
   BE_BEFORE(item, pos);
  }

  /*--- XA_CHAR ---*/
  if (type == XA_CHAR)
  {
   if (direction == XA_LEFT)	/* go to left */
   {
    switch(item->type)
    {
    case XA_BEGIN:
     pos = 1;
     break;
    case XA_TEXT:
     if (pos == 0)
     {
      GET_PREVIOUS_ITEM(item, pos);
     }
     pos--;
     break;
    default:
     GET_PREVIOUS_ITEM(item, pos);
     pos--;
     break;
    }
   }
   else				/* go to right */
   {
    switch(item->type)
    {
    case XA_TEXT:
     if (pos == item->text.text_len)
     {
      item = item->any.next;
      if (item->type == XA_END) 
       pos = 0;
      else
       pos = 1;
     }
     else
      pos += 1;
     
     break;
    case XA_END:		/* ARGH!!!! Should never happen */
     pos = 0;
     break;
    default:
     item = item->any.next;
     if (item->type == XA_END)
      pos = 0;
     else
      pos = 1;
     break;
    }
   }

   count --;
   if (count)
   {
    goto_Text(This, type, direction, count, NULL, 0, select);
    return;
   }

  }
  /*--- XA_WORD ---*/
  else if (type == XA_WORD)
  {
   if (direction == XA_LEFT)	/* go to left */
   {
    
    if (item->type == XA_TEXT && pos != item->text.text_len)
    {
     do
     {
      pos = pos - 1;
      if (pos == -1)
      {
       item = item->any.prev;
       if (item->type == XA_TEXT)
	pos = item->text.text_len - 1;
       else
       {
	pos = 1;
	break;
       }
      }
     }
     while(!Xc_IS_PUNCTUATION(item->text.text[pos]));
     
     if (item->type == XA_TEXT && pos != item->text.text_len)
      pos++;
    }

   }
   else				/* go to right */
   {

    if (item->type == XA_TEXT)
    {
     if (pos == item->text.text_len)
     {
      item = item->any.next; pos = 0;
     }
    }
    else
    {
     break;
    }
    
    if (item->type == XA_TEXT)
    {
     
     while(!Xc_IS_PUNCTUATION(item->text.text[pos]))
     {
      pos = pos + 1;
      if (pos == item->text.text_len)
      {
       item = item->any.next;
       if (item->type == XA_TEXT)
	pos = 0;
       else
       {
	pos = 0;
	break;
       }
      }
     }
     
    }
   }
   
   count --;
   if (count)
   {
    goto_Text(This, type, direction, count, NULL, 0, select);
    return;
   }

  }
  break;

 case XA_PARAGRAPH:
  /*--- XA_PARAGRAPH ---*/

  if (direction == XA_LEFT)
  {
   item = This->select_start;
   pos = This->select_start_pos;
  }
  else
  {
   item = This->select_end;
   pos = This->select_end_pos;
  }
  BE_BEFORE(item, pos);
  
  if (direction == XA_LEFT)	/* go to left */
  {
   while(item->type != XA_LINEFEED && item->type != XA_BEGIN)
    item = item->any.prev;
   pos = 1;
  }
  else				/* go to right */
  {
   if (item->type != XA_END) item = item->any.next;
   while(item->type != XA_LINEFEED && item->type != XA_END)
     item = item->any.next;
    pos = 1;
  }
  
  count --;
  if (count)
  {
   goto_Text(This, type, direction, count, NULL, 0, select);
   return;
  }
  
  break;
 case XA_LINE:
 case XA_GLOBAL:
 default:
  break;
 }
 
 if (!select)
 {
  BE_IN_RIGHT_PLACE(item, pos);
  This->select_start = This->select_end = item;
  This->select_start_pos = This->select_end_pos = pos;
 }
 else
 {
  if (item->type == XA_BEGIN)
   pos = 1;
  if (item->type == XA_END)
   pos = 0;

  if (This->select_start == This->select_end &&
      This->select_start_pos == This->select_end_pos)
  {
   if (direction == XA_LEFT)
   {
    This->select_start = item;
    This->select_start_pos = pos;
    This->start_end = TRUE;
   }
   else
   {
    This->select_end = item;
    This->select_end_pos = pos;
    This->start_end = FALSE;
   }
  }
  else
  {
   if (This->start_end)
   {
    This->select_start = item;
    This->select_start_pos = pos;
   }
   else
   {
    This->select_end = item;
    This->select_end_pos = pos;
   }
  }
  
  if (This->select_start == This->select_end &&
      This->select_start_pos > This->select_end_pos)
  {
   SWAP(xa_item_t *, This->select_start, This->select_end);
   SWAP(int, This->select_start_pos, This->select_end_pos);
   This->start_end ^= TRUE;
  }

  if (This->select_start != This->select_end ||
      This->select_start_pos != This->select_end_pos)
   {
    if (This->select_start->type == XA_LINEFEED && This->select_start_pos == 1)
    {
     This->select_start = This->select_start->any.next;
     This->select_start_pos = 0;
    }
    if (This->select_end_pos == 0 &&
	This->select_end->any.prev->type == XA_LINEFEED)
    {
     GET_PREVIOUS_ITEM(This->select_end, This->select_end_pos);
    }
   }
   else if (This->select_start->type == XA_LINEFEED && 
	    This->select_start_pos == 1)
   {
    This->select_start = This->select_end = This->select_start->any.next;
    This->select_start_pos = This->select_end_pos = 0;
   }
  

  if ((IS_AFTER(This->select_start, This->select_start_pos) &&
      This->select_end_pos == 0 && 
      This->select_start->any.next == This->select_end) ||
      (IS_AFTER(This->select_end, This->select_end_pos) &&
       This->select_start_pos == 0 && 
       This->select_end->any.next == This->select_start))
  {
   if (direction == XA_LEFT)
   {
    BE_IN_RIGHT_PLACE(This->select_start, This->select_start_pos);
    This->select_end = This->select_start;
    This->select_end_pos = This->select_start_pos;
    This->start_end = TRUE;
   }
   else
   {
    BE_IN_RIGHT_PLACE(This->select_end, This->select_end_pos);
    This->select_start = This->select_end;
    This->select_start_pos = This->select_end_pos;
    This->start_end = FALSE;
   }
  }

 }

 if (This->select_start != This->select_end)
 {
  if (This->start_end)
   This->style = This->select_end->any.style;
  else
   This->style = This->select_start->any.next->any.style;
 }
 else
 {
  This->style = This->select_start->any.style;
 }
 This->ruler = get_ruler_Text(This);
 console(This);
}

static xa_item_t *cut_line_Text(This, item, pos)
c_Text *This;
xa_item_t *item;
int pos;
{
 xa_item_t *nitem;
 c_TextStyle *style;

 switch(item->type)
 {
 case XA_TEXT:
  if (pos > 0 && pos < item->text.text_len)
  {
   style = This->style;
   This->style = item->text.style;

   nitem = create_item(This, XA_TEXT, (void *)(item->text.text + pos), item);
   
   item->text.text[pos] = '\0';
   item->text.text_len = pos;
   item->text.text = (char *)Xc_realloc("ot", item->text.text, 
					item->text.text_len + 1);
   This->style = style;

   if (This->select_start == item && This->select_start_pos > pos)
   {
    This->select_start = nitem;
    This->select_start_pos -= pos;
   }
   if (This->select_end == item && This->select_end_pos > pos)
   {
    This->select_end = nitem;
    This->select_end_pos -= pos;
   }
  }
  break;
 default:
  break;
 }
 console(This);

 return (xa_item_t *)NULL;
}

static void pack_Text(This, ritem)
c_Text *This;
xa_item_t **ritem;
{
 xa_item_t *item, *nitem, *new_item;
 int len;
 
 item = *ritem;
 nitem = item->any.next;
 if (nitem->type == XA_TEXT && item->type == XA_TEXT && 
     nitem->text.style == item->text.style)
 {
  len = nitem->text.text_len;
  
  new_item = push_in_changed_stack(This, item, TRUE);

  new_item->text.text = (char *)Xc_realloc("nt", new_item->text.text, 
					   new_item->text.text_len + len + 1);
  memcpy(new_item->text.text + new_item->text.text_len, 
	 nitem->text.text, len + 1);
  new_item->text.text_len += len;
  
  if ( This->select_start == item)
   This->select_start = new_item;
   
  if (This->select_start == nitem)
  {
   This->select_start = new_item;
   This->select_start_pos += new_item->text.text_len - len;
  }
  
  if ( This->select_end == item)
   This->select_end = new_item;

  if (This->select_end == nitem)
  {
   This->select_end = new_item;
   This->select_end_pos += new_item->text.text_len - len;
  }
  nitem = push_in_changed_stack(This, nitem, FALSE);

  *ritem = item = new_item;
  nitem = item->any.next;
 }
}

static void or_tstyle_Text(This, cb_func, page, cb_data)
c_Text *This;
c_TextStyle *(*cb_func)();
void *page;
void *cb_data;
{
 xa_item_t	*item, *nitem;
 int		pos;
 c_TextStyle	*tstyle;

 purge_changed_items(This);
 
 item = This->select_start;
 pos = This->select_start_pos;
 
 if (item == This->select_end && 
     pos == This->select_end_pos)
 {
  This->style = cb_func(page, This->style, cb_data);
  return;
 } 
 
 BE_BEFORE(This->select_start, This->select_start_pos);
 BE_BEFORE(This->select_end, This->select_end_pos);

 item = This->select_start;
 pos = This->select_start_pos;

 if (item == This->select_end && item->type == XA_TEXT)
 {				/* selection within a text block */
  tstyle = cb_func(page, item->any.style, cb_data);
  if (tstyle != item->any.style)
  {
   This->changed_start = This->changed_end = item;
   item = break_Text(This, item, pos);
   if (pos)
    item = item->any.next;

   if (This->select_end_pos != This->changed_start->text.text_len)
   {
    nitem = item;
    item = break_Text(This, item, This->select_end_pos - 
		      This->select_start_pos);
    simple_delete_item(This, nitem);
   }
   F(item->any.style).markUnused(item->any.style);
   item->any.style = This->style = tstyle;
   F(tstyle).markUsed(tstyle);
   
   This->select_start = This->select_end = item;
   This->select_start_pos = 0;
   This->select_end_pos = item->text.text_len;
  }
 }
 else
 {
  This->changed_end = This->select_end;

  tstyle = cb_func(page, item->any.style, cb_data);
  
  if (tstyle != item->any.style)
  {		
   switch(item->type)
   {
   case XA_TEXT:
    if (pos != item->text.text_len)/* must cut the text block */
    {
     This->changed_start = item;
     
     item = break_Text(This, item, pos);
     
     if (pos)
      item = item->any.next;
     
     This->select_start = item;
     This->select_start_pos = 0;
     
     F(item->any.style).markUnused(item->any.style);
     item->any.style = tstyle;
     F(tstyle).markUsed(tstyle);
    }
    break;
   default:
    break;
   }
  }
  
  if (!This->changed_start)
   This->changed_start = item->any.next;

  item = item->any.next;
  while(item != This->select_end)
  {
   tstyle = cb_func(page, item->any.style, cb_data);
   
   item = duplicate_item(This, item);
   
   item->any.next->any.prev = item;
   item->any.prev->any.next = item;
   
   if (tstyle != item->any.style)
   {		
    F(item->any.style).markUnused(item->any.style);
    item->any.style = tstyle;
    F(tstyle).markUsed(tstyle);
   }

   item = item->any.next;
  }
   
  tstyle = cb_func(page, item->any.style, cb_data);
  if (tstyle != item->any.style)
  {		
   if (item->type == XA_TEXT && This->select_end_pos != item->text.text_len)
   {
    pos = This->select_end_pos;
    
    item = break_Text(This, item, pos);
    if (pos)
    {
     F(item->any.style).markUnused(item->any.style);
     item->any.style = tstyle;
     F(tstyle).markUsed(tstyle);
     
     This->select_end = item;
     This->select_end_pos = item->text.text_len;
    }
    else
    {
     This->select_end = item;
     This->select_end_pos = 1;
    }
   }
   else
   {
    item = duplicate_item(This, item);
    
    item->any.next->any.prev = item;
    item->any.prev->any.next = item;
    F(item->any.style).markUnused(item->any.style);
    item->any.style = tstyle;
    F(tstyle).markUsed(tstyle);
    This->select_end = item;
   }
  }
  else
  {
   item = duplicate_item(This, item);
   
   item->any.next->any.prev = item;
   item->any.prev->any.next = item;
   This->select_end = item;
  }
 }
 
 BE_IN_RIGHT_PLACE(This->select_start, This->select_start_pos);
 /* BE_IN_RIGHT_PLACE(This->select_end, This->select_end_pos); */

 check_begin_style(This);

 GET_SELECTION_STYLE
 
 console(This);
}

static void or_truler_Text(This, cb_func, page, cb_data)
c_Text *This;
c_TextRuler *(*cb_func)();
void *page;
void *cb_data;
{
 xa_item_t	*item, *nitem, *ritem, *titem;
 c_TextRuler	*truler;

 purge_changed_items(This);
 
 item = This->select_start;
 if (item->type == XA_LINEFEED) item = item->any.prev;
 while(item->type != XA_LINEFEED && item->type != XA_BEGIN)
  item = item->any.prev;

 nitem = This->select_end;
 while(nitem->type != XA_LINEFEED && nitem->type != XA_END)
  nitem = nitem->any.next;

 if (item == nitem) nitem = nitem->any.next;
 
 This->changed_start = item;
 This->changed_end = nitem->any.prev;
 
 if (This->select_start) 
  ritem = This->select_start;
 else
  ritem = This->select_end;
 
 while(item != nitem)
 {
  truler = cb_func(page, item->linefeed.ruler, cb_data);
  
  titem = duplicate_item(This, item);

  if (item == This->select_start) This->select_start = titem;
  if (item == This->select_end) This->select_end = titem;

  titem->any.next->any.prev = titem;
  if (titem->any.prev)		/* in case where item->type == XA_BEGIN */
   titem->any.prev->any.next = titem;
  else
  {
   This->item_base = titem;
   titem->any.prev = NULL;
  }
  F(titem->linefeed.ruler).markUnused(titem->linefeed.ruler);
  titem->linefeed.ruler = truler;
  F(titem->linefeed.ruler).markUsed(titem->linefeed.ruler);

  if (item == ritem)
   This->ruler = truler;

  item = item->any.next;

  if (item == ritem)
   This->ruler = truler;

  while(item != nitem && item->type != XA_LINEFEED)
  {
   if (item == ritem)
    This->ruler = truler;

   titem = duplicate_item(This, item);

   titem->any.next->any.prev = titem;
   titem->any.prev->any.next = titem;

   if (item == This->select_start) This->select_start = titem;
   if (item == This->select_end) This->select_end = titem;
   
   item = item->any.next;
  }
 }

 GET_SELECTION_STYLE
}

/* ----------------------------------------------------------------- ** 
** get ruler - look backward for a LINEFEED item                     ** 
**             and get its ruler.                                    ** 
** ----------------------------------------------------------------- */
static c_TextRuler *get_ruler_Text(This)
c_Text *This;
{
 xa_item_t	*item = This->select_start;
 
 if ((item->type == XA_LINEFEED || item->type == XA_END) &&
     This->select_start_pos == 0)
  item = item->any.prev;
 
 while(item->type != XA_LINEFEED && item->type != XA_BEGIN)
  item = item->any.prev;
 
 return item->linefeed.ruler;
}

/* ----------------------------------------------------------------- ** 
** Clipboard functions                                               ** 
** ----------------------------------------------------------------- */
static void clip_purge_Text(This)
c_Text	*This;
{
 xa_item_t	*item;
 
 item = This->item_base->any.next;
 while(item->type != XA_END)
 {
  item = item->any.next;
  delete_item(This, item->any.prev);
 }
 This->select_start = This->item_base;
}

static void clip_copy_Text(This, clipboard)
c_Text	*This;
c_Text	*clipboard;
{
 xa_item_t	*item, *citem, *nitem;
 char		*ptr;
 int		len;

 clip_purge_Text(clipboard);
 citem = clipboard->item_base;

 if (This->select_start == This->select_end)
 {
  if (This->select_start_pos == This->select_end_pos)
   return;
  
  item = This->select_start;
  len = This->select_end_pos - This->select_start_pos;
  ptr = (char *)Xc_malloc("np", len + 1);
  memcpy(ptr, item->text.text + This->select_start_pos, len);
  ptr[len] = '\0';

  nitem = create_item(clipboard, XA_TEXT, (void *)ptr, citem);
  F(nitem->any.style).markUnused(nitem->any.style);
  nitem->any.style = F(item->any.style).virtualCopy
   (item->any.style, clipboard->BaseStd);
  F(nitem->any.style).markUsed(nitem->any.style);
  
  Xc_free(ptr);
 }
 else
 {
  item = This->select_start;
  if (item->type == XA_BEGIN) item = item->any.next;

  while(item != This->select_end->any.next && item->type != XA_END)
  {
   switch(item->type)
   {
   case XA_TEXT:
    if (item == This->select_start && This->select_start_pos > 0)
    {
     nitem = create_item(clipboard, XA_TEXT, 
			 (void *)(item->text.text + This->select_start_pos),
			 citem);
    }
    else if (item == This->select_end && 
	     This->select_end_pos != item->text.text_len)
    {
     len = This->select_end_pos;
     ptr = (char *)Xc_malloc("np", len + 1);
     memcpy(ptr, item->text.text, len);
     ptr[len] = '\0';
     
     nitem = create_item(clipboard, XA_TEXT, (void *)ptr, citem);
     Xc_free(ptr);
    }
    else
     nitem = create_item(clipboard, XA_TEXT, (void *)item->text.text, citem);
    break;
   case XA_LINEFEED:
    nitem = create_item(clipboard, item->type, NULL, citem);
    F(nitem->linefeed.ruler).markUnused(nitem->linefeed.ruler);
    nitem->linefeed.ruler = F(item->linefeed.ruler).virtualCopy
     (item->linefeed.ruler, clipboard->BaseStd);    
    F(nitem->linefeed.ruler).markUsed(nitem->linefeed.ruler);
    break;
   default:
    nitem = create_item(clipboard, item->type, NULL, citem);
    break;
   }
   F(nitem->any.style).markUnused(nitem->any.style);
   nitem->any.style = F(item->any.style).virtualCopy
    (item->any.style, clipboard->BaseStd);
   F(nitem->any.style).markUsed(nitem->any.style);
   citem = nitem;
   item = item->any.next;
  }
 }

 console(clipboard);
}

static void clip_paste_Text(This, clipboard)
c_Text *This;
c_Text *clipboard;
{
 xa_item_t *citem;
 xa_item_t *item;
 int	    pos;

 if (This->select_start != This->select_end ||
     This->select_start_pos != This->select_end_pos)
 {
  delete_Text(This);
 }
 else
 {
  purge_changed_items(This);
 }

 citem = clipboard->item_base->any.next;
 item = This->select_start;
 pos = This->select_start_pos;

 BE_BEFORE(item, pos);

 if (item->type == XA_TEXT && This->select_start_pos != item->text.text_len)
 {
  if (!This->changed_start)
   This->changed_start = This->changed_end = item;
  item = break_Text(This, item, pos);
  if (!pos) item = item->any.prev;
  
  /* This->select_start = item;
     if (item->type == XA_TEXT)
     This->select_start_pos = item->text.text_len;
     else
     This->select_start_pos = 0;  
     This->style =  This->select_start->any.style; */
 }
 
 while(citem->type != XA_END)
 {
  This->style = F(citem->any.style).virtualCopy(citem->any.style,
						This->BaseStd);
  switch(citem->type)
  {
  case XA_TEXT:
   item = create_item(This, XA_TEXT, (void *)citem->text.text, item);
   break;
  case XA_LINEFEED:
   item = create_item(This, citem->type, NULL, item);
   F(item->linefeed.ruler).markUnused(item->linefeed.ruler);
   item->linefeed.ruler = F(citem->linefeed.ruler).virtualCopy
    (citem->linefeed.ruler, This->BaseStd);
   F(item->linefeed.ruler).markUsed(item->linefeed.ruler);
   break;
  default:
   item = create_item(This, citem->type, NULL, item);
   break;
  }
  citem = citem->any.next;
 }
 
 This->select_start = item;
 if (item->type == XA_TEXT)
  This->select_start_pos = item->text.text_len;
 else
  This->select_start_pos = 1;

 BE_IN_RIGHT_PLACE(This->select_start, This->select_start_pos);
 This->select_end = This->select_start;
 This->select_end_pos = This->select_start_pos;
 
 This->style =  This->select_start->any.style;

 console(This);
}

static void select_all_Text(This)
c_Text *This;
{
 xa_item_t	*item;

 This->select_start = item = This->item_base;
 This->select_start_pos = 1;
 
 while(item->type != XA_END)
  item = item->any.next;

 This->select_end = item;
 This->select_end_pos = 0;
 
 console(This);
}

static boolean check_diff_text_item(This, item1, item2, pos)
c_Text *This;
xa_item_t *item1;
xa_item_t *item2;
int *pos;
{
 char *str1, *str2;
 int i;

 if (pos) *pos = -1;
 if (item1->type != item2->type) return TRUE;
 if (item1->any.style != item2->any.style) return TRUE;
 switch(item1->type)
 {
 case XA_TEXT:
  str1 = item1->text.text;
  str2 = item2->text.text;
  if (item1->text.text_len != item2->text.text_len || !Xstrcmp(str1, str2))
  {
   if (pos)
   {
    i = 0;
    while(str1[i] && str2[i] && str1[i] == str2[i])
     i++;
    *pos = i;
   }
   return TRUE;
  }
  break;
 default:
  if (item1 != item2) return TRUE;
  break;
 }
 return FALSE;
}

static void console(This)
c_Text *This;
{
 xa_item_t	*nitem;
 FILE *debug_file;
 char *ptr, nc;
 int  i;

 return;

 debug_file = fopen("/dev/console", "w");
 
 if (This->BaseStd == &clipboard_base)
  fprintf(debug_file, "CLIPBOARD  ");

 nitem = This->item_base;
 fprintf(debug_file, "----------// style: %s //----------\n",
	 This->style->name);
 
 if (This->select_start == This->select_end && 
     This->select_start_pos == This->select_end_pos)
 {
  while(nitem)
  {
   if (nitem == This->select_start)
    nc = '|'; else nc = ' ';
   
   switch(nitem->type)
   {
   case XA_TEXT:
    if (nitem != This->select_start)
     fprintf(debug_file, "(%30s) XA_TEXT: %s\n", nitem->any.style->name,
	     nitem->text.text);
    else
    {
     fprintf(debug_file, "(%30s) XA_TEXT: ", nitem->any.style->name);
     ptr = nitem->text.text;
     for(i = 0; i < This->select_start_pos; i++)
      fputc(*ptr++, debug_file);
     fputc('|', debug_file);
     if (*ptr)
      fputs(ptr, debug_file);
     fputc('\n', debug_file);
    }
    break;
   case XA_LINEFEED:
    fprintf(debug_file, "(%30s) XA_LINEFEED%c\n", nitem->any.style->name, nc);
    break;
   case XA_TAB:
    fprintf(debug_file, "(%30s) XA_TAB%c\n", nitem->any.style->name, nc);
    break;
   case XA_BEGIN:
    fprintf(debug_file, "(%30s) XA_BEGIN%c\n", nitem->any.style->name, nc);
    break;
   case XA_END:
    fprintf(debug_file, "(%30s) XA_END%c\n", nitem->any.style->name, nc);
    break;
   }
   nitem = nitem->any.next;
  }
 }
 else
 {
  while(nitem)
  {
   if (nitem == This->select_start)
    nc = '[';
   else if (nitem == This->select_end)
    nc = ']';
   else
    nc = ' ';
   
   switch(nitem->type)
   {
   case XA_TEXT:
    if (nitem != This->select_start && nitem != This->select_end)
     fprintf(debug_file, "(%30s) XA_TEXT: %s \n", nitem->any.style->name,
	     nitem->text.text);
    else
    {
     if (nitem != This->select_end)
     {
      fprintf(debug_file, "(%30s) XA_TEXT: ", nitem->any.style->name);
      ptr = nitem->text.text;
      for(i = 0; i < This->select_start_pos; i++)
       fputc(*ptr++, debug_file);
      fputc('[', debug_file);
      if (*ptr)
       fputs(ptr, debug_file);
      fputc('\n', debug_file);
     }
     else if (nitem != This->select_start)
     {
      fprintf(debug_file, "(%30s) XA_TEXT: ", nitem->any.style->name);
      ptr = nitem->text.text;
      for(i = 0; i < This->select_end_pos; i++)
       fputc(*ptr++, debug_file);
      fputc(']', debug_file);
      if (*ptr)
       fputs(ptr, debug_file);
      fputc('\n', debug_file);
     }
     else
     {
      fprintf(debug_file, "(%30s) XA_TEXT: ", nitem->any.style->name);
      ptr = nitem->text.text;
      for(i = 0; i < This->select_start_pos; i++)
       fputc(*ptr++, debug_file);
      fputc('[', debug_file);
      for(; i < This->select_end_pos; i++)
       fputc(*ptr++, debug_file);
      fputc(']', debug_file);
      if (*ptr)
       fputs(ptr, debug_file);
      fputc('\n', debug_file);
     }
    }
    break;
   case XA_LINEFEED:
    fprintf(debug_file, "(%30s) XA_LINEFEED%c\n", nitem->any.style->name, nc);
    break;
   case XA_TAB:
    fprintf(debug_file, "(%30s) XA_TAB%c\n", nitem->any.style->name, nc);
    break;
   case XA_BEGIN:
    fprintf(debug_file, "(%30s) XA_BEGIN%c\n", nitem->any.style->name, nc);
    break;
   case XA_END:
    fprintf(debug_file, "(%30s) XA_END%c\n", nitem->any.style->name, nc);
    break;
   }
   nitem = nitem->any.next;
  }
 }
 
 if (This->changed_start)
 {
  xa_item_t *item;
  
  fprintf(debug_file, "Original Chain: ");
  item = This->changed_start->any.prev;
  if (!item) fprintf(debug_file, "[TOP]->|");
  else fprintf(debug_file, "[%p]->|", item);
  
  item = This->changed_start;
  while(item != This->changed_end)
  {
   fprintf(debug_file, "[%p]->", item);
   item = item->any.next;
  }
  fprintf(debug_file, "[%p]|->[%p]\n", item, item->any.next);
  
  fprintf(debug_file, "New Chain:      ");
  item = This->changed_start->any.prev;
  if (!item) 
  {
   fprintf(debug_file, "[TOP]->|");
   item = This->item_base;
  }
  else 
  {
   fprintf(debug_file, "[%p]->|", item);
   item = item->any.next;
  }
  if (item == This->changed_end->any.next)
   fprintf(debug_file, "|->[%p]\n", item);
  else
  {
   while(item != This->changed_end->any.next->any.prev)
   {
    fprintf(debug_file, "[%p]->", item);
    item = item->any.next;
   }
   fprintf(debug_file, "[%p]|->[%p]\n", item, item->any.next);
  }
 }

 fclose(debug_file);
}

/* ----------------------------------------------------------------- ** 
** item_functions - Text object internal functions                   ** 
** ----------------------------------------------------------------- */
static void delete_item(This, item)
c_Text	*This;
xa_item_t *item;
{
 item->any.next->any.prev = item->any.prev;
 item->any.prev->any.next = item->any.next;

 F(item->any.style).markUnused(item->any.style);
 switch(item->type)
 {
 case XA_TEXT:
  Xc_free(item->text.text);
  break;
 case XA_BEGIN:
 case XA_LINEFEED:
  F(item->linefeed.ruler).markUnused(item->linefeed.ruler);
  break;
 default:
  break;
 }
 Xc_delete(This->i_handle, item);
}

static void simple_delete_item(This, item)
c_Text	*This;
xa_item_t *item;
{

 F(item->any.style).markUnused(item->any.style);
 switch(item->type)
 {
 case XA_TEXT:
  Xc_free(item->text.text);
  break;
 case XA_BEGIN:
 case XA_LINEFEED:
  F(item->linefeed.ruler).markUnused(item->linefeed.ruler);
  break;
 default:
  break;
 }
 Xc_delete(This->i_handle, item);
}

/*--- Insert AFTER item ---*/
xa_item_t *create_item(This, type, data, item)
c_Text	*This;
int	type;
void	*data;
xa_item_t *item;
{
 xa_item_t *nitem;

 nitem = (xa_item_t *)Xc_new(This->i_handle);
 nitem->type = type;
 nitem->any.prev = item;
 nitem->any.next = item->any.next;
 nitem->any.next->any.prev = nitem;
 item->any.next = nitem;
 
 nitem->any.style = This->style;
 F(This->style).markUsed(This->style);
 
 switch(type)
 {
 case XA_TEXT:
  nitem->text.text = (char *)Xc_strdup("nt", data);
  nitem->text.text_len = strlen((char *)data);
  break;
 case XA_BEGIN:
 case XA_LINEFEED:
  nitem->linefeed.ruler = This->ruler;
  F(This->ruler).markUsed(This->ruler);
  break;
 case XA_TAB:
  nitem->tab.filler = ' ';
  break;
 default:
  break;
 }
 return nitem;
}

/*--- Duplicate an item ---*/
static xa_item_t *duplicate_item(This, item)
c_Text *This;
xa_item_t *item;
{
 xa_item_t *new_item;

 new_item = (xa_item_t *)Xc_new(This->i_handle);
 memcpy(new_item, item, sizeof(xa_item_t));
 F(item->any.style).markUsed(item->any.style);

 switch(item->type)
 {
 case XA_TEXT:
  new_item->text.text = (char *)Xc_strdup("dt", item->text.text); 
  break;
 case XA_BEGIN:
 case XA_LINEFEED:
  F(item->linefeed.ruler).markUsed(item->linefeed.ruler);
  break;
 default:
  break;
 }

 return new_item;
}

static xa_item_t *push_in_changed_stack(This, item, dup)
c_Text *This;
xa_item_t *item;
boolean dup;
{
 xa_item_t *new_item;

 if (dup)
 {
  new_item = duplicate_item(This, item);
  new_item->any.prev->any.next = new_item;
  new_item->any.next->any.prev = new_item;
 }
 else
 {
  item->any.next->any.prev = item->any.prev;
  item->any.prev->any.next = item->any.next;
  new_item = item->any.prev;
 }
 item->any.next = This->changed_stack;
 This->changed_stack = item;

 return new_item;
}

static void purge_changed_items(This)
c_Text *This;
{
 xa_item_t *item, *next, *end;
 
 item = This->changed_start;
 end = This->changed_end;
 if (item && end)
 {
  while(item != end->any.next)
  {
   next = item->any.next;
   simple_delete_item(This, item);
   item = next;
  }
  This->changed_start = This->changed_end = NULL;
 }

 item = This->changed_stack;
 while(item)
 {
  next = item->any.next;
  simple_delete_item(This, item);
  item = item->any.next;
 }
 This->changed_stack = NULL;
}

static boolean need_undo_Text(This, nb_char)
c_Text *This;
int nb_char;
{
 xa_item_t *item;
 int pos, nbc;

 if (This->select_start == This->select_end && 
     This->select_start_pos == This->select_end_pos)
  return FALSE;

 item = This->select_start;
 pos = This->select_start_pos;
 BE_AFTER(item, pos)

 if (item == This->select_end && pos == This->select_end_pos)
  return FALSE;
 
 if (item == This->select_end)
 {
  switch(item->type)
  {
  case XA_TEXT:
   return (This->select_end_pos >= pos + nb_char);
   break;
  default:
   return (nb_char <= 1);
  }
 }
 
 nbc = 0;
 if (pos != 0)
 {
  switch(item->type)
  {
  case XA_TEXT:
   nbc = item->text.text_len - pos;
   if (nbc >= nb_char) return TRUE;
   item = item->any.next;
   break;
  default:
   break;
  }
 }
 
 while(item != This->select_end)
 {
  switch(item->type)
  {
  case XA_TEXT:
   nbc = item->text.text_len;
   break;
  default:
   nbc++;
   break;
  }
  if (nbc >= nb_char) return TRUE;
  item = item->any.next;
 }
 
 return (This->select_end_pos + nbc >= nb_char); 
}

static void check_begin_style(This)
c_Text *This;
{
 xa_item_t *item;
 
 item = This->item_base;
 if (item->any.style != item->any.next->any.style)
 {
  F(item->any.style).markUnused(item->any.style);
  item->any.style = item->any.next->any.style;
  F(item->any.style).markUsed(item->any.style);
 }
}

static xa_item_t *break_Text(This, item, pos)
c_Text *This;
xa_item_t *item;
int pos;
{
 xa_item_t *nitem, *sitem;

 if (pos == 0)
 {
  nitem = (xa_item_t *)Xc_new(This->i_handle);
  nitem->type = XA_TEXT;
  nitem->any.next = item->any.next;
  nitem->any.prev = item->any.prev;
  nitem->any.style = item->any.style;
  F(nitem->any.style).markUsed(nitem->any.style);
  nitem->text.text_len = item->text.text_len;
  nitem->text.text = (char *)Xc_malloc("s1", nitem->text.text_len + 1);
  strcpy(nitem->text.text, item->text.text);
  
  item->any.prev->any.next = nitem;
  item->any.next->any.prev = nitem;

  return nitem;
 }
 
 nitem = (xa_item_t *)Xc_new(This->i_handle);
 nitem->type = XA_TEXT;
 nitem->any.prev = item->any.prev;
 nitem->any.style = item->any.style;
 F(nitem->any.style).markUsed(nitem->any.style);
 nitem->text.text_len = pos;
 nitem->text.text = (char *)Xc_malloc("s1", pos + 1);
 memcpy(nitem->text.text, item->text.text, pos);
 nitem->text.text[pos] = '\0';
 
 sitem = (xa_item_t *)Xc_new(This->i_handle);
 sitem->type = XA_TEXT;
 sitem->any.next = item->any.next;
 sitem->any.style = item->any.style;
 F(sitem->any.style).markUsed(sitem->any.style);
 sitem->text.text_len = item->text.text_len - pos;
 sitem->text.text = (char *)Xc_malloc("s1", sitem->text.text_len + 1);
 strcpy(sitem->text.text, item->text.text + pos);

 nitem->any.next = sitem;
 sitem->any.prev = nitem;

 item->any.prev->any.next = nitem;
 item->any.next->any.prev = sitem;

 return nitem;
}
