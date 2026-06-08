/*
** ClipBuffer.c for XAllWrite in ClipBuffer/
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
** Started on  Mon Jul 17 14:12:01 1995 Emmanuel Paris
** Last update Mon Apr 20 19:49:48 1998 Emmanuel Paris
*/

#include "ClipBuffer.h"

extern c_Color *GlobColorBase;
extern c_VectorFont *GlobFontBase;

static void *cons_ClipBuffer();
static void dest_ClipBuffer();
static void *copy_ClipBuffer();

static void new_level ___PROTO((c_ClipBuffer *This));
static void add_object ___PROTO((c_ClipBuffer *This, void *object,
				 clipb_e type, void (*destructor)()));
static void del_object ___PROTO((c_ClipBuffer *This, void *object));
static void del_level ___PROTO((c_ClipBuffer *This, int level));
static void empty_list ___PROTO((c_ClipBuffer *This));

static void start_request_object ___PROTO((c_ClipBuffer *This, int level));
static void *request_next_object ___PROTO((c_ClipBuffer *This));

static void purge_item ___PROTO((c_ClipBuffer *This, clipb_t *item, 
				 clipb_t *pitem));

sf_ClipBuffer fc_ClipBuffer =
{
 cons_ClipBuffer,
 dest_ClipBuffer,
 copy_ClipBuffer,
 
 new_level,
 add_object,
 del_object,
 del_level,
 empty_list,
 start_request_object,
 request_next_object,
};

static void *cons_ClipBuffer(store_level)
int store_level;
{
 c_ClipBuffer	*This;
 c_Color *color;
  
 This=(c_ClipBuffer *)Xc_malloc("clipbuffer",sizeof(c_ClipBuffer));
 This->f= &fc_ClipBuffer;
  
 /*--- Copy global color base ---*/
 This->BaseStd.color_base = NULL;
 for(color = GlobColorBase; color != NULL; color = color->next)
  COPY(c_Color)(color, &This->BaseStd.color_base);

 This->BaseStd.text_ruler_base = NULL;
 This->BaseStd.text_style_base = NULL;
 This->BaseStd.text_base = NULL;

#if 0 
 NEW(c_TextRuler)(&This->BaseStd.text_ruler_base, "clip_ruler");
 NEW(c_TextStyle)(&This->BaseStd.text_style_base,
		  "clip_style",
		  F(GlobFontBase).getFontByName(GlobFontBase, "Times-Roman"),
		  SCALE_FROM_POINTS(12), This->BaseStd.color_base);
#endif

 This->base = NULL;
 This->level = This->nb_level = 0;
 This->low_level = 1;
 This->store_level = store_level;

 return This;
}

static void dest_ClipBuffer(This)
c_ClipBuffer	*This;
{
 F(This).empty_list(This);
  
 /*--- Destroy the clip_buffer text database ---*/
#if 0
 while(This->BaseStd.text_base != NULL)
  DELETE(c_Text)(This->BaseStd.text_base);
#endif

 /*--- Destroy the clip_buffer text ruler list ---*/
 while(This->BaseStd.text_ruler_base != NULL)
  DELETE(c_TextRuler)(This->BaseStd.text_ruler_base);
  
 /*--- Destroy the clip_buffer text style list ---*/
 while(This->BaseStd.text_style_base != NULL)
  DELETE(c_TextStyle)(This->BaseStd.text_style_base);
  
 /*--- Destroy the clip_buffer color database ---*/
 while(This->BaseStd.color_base != NULL)
  DELETE(c_Color)(This->BaseStd.color_base);
  
 Xc_free(This);
}

static void *copy_ClipBuffer(This)
c_ClipBuffer	*This;
{
 Xc_TRACE(("Not usefull"));
 return NULL;
}

static void new_level(This)
c_ClipBuffer *This;
{
 This->level++;
 This->nb_level++;
 
 if (This->store_level != XcCB_INFINITE_LEVEL && 
     This->nb_level > This->store_level)
  del_level(This, This->low_level);
}

static void add_object(This, object, type, destructor)
c_ClipBuffer *This;
void *object;
clipb_e type;
void (*destructor)();
{
 clipb_t *item;
 
 item = (clipb_t *)Xc_malloc("cbitem", sizeof(clipb_t));
 item->object = object;
 item->destructor = destructor;
 item->type = type;
 item->level = This->level;
 item->next = This->base;
 This->base = item;
}

static void del_object(This, object)
c_ClipBuffer	*This;
void		*object;
{
 clipb_t *item, *pitem;
 
 pitem = NULL;
 item = This->base;
 while(item && item->object != object)
 {
  pitem = item;
  item = item->next;
 }

 purge_item(This, item, pitem);
}

static void del_level(This, level)
c_ClipBuffer	*This;
int level;
{
 clipb_t *item, *pitem;
 
 pitem = NULL;
 item = This->base;
 
 while(item)
 {
  if (item->level == level)
  {
   purge_item(This, item, pitem);
   if (pitem)
    item = pitem->next;
   else
    item = This->base;
  }
  else
  {
   pitem = item;
   item = item->next;
  }
 }
}

static void empty_list(This)
c_ClipBuffer	*This;
{
 while(This->base)
  purge_item(This, This->base, NULL);
 
 This->base = NULL;
 This->level = This->nb_level = 0;
 This->low_level = 1;
}

static void start_request_object(This, level)
c_ClipBuffer	*This;
int level;
{
 This->req_item = This->base;
 This->req_level = level;
}

static void *request_next_object(This)
c_ClipBuffer	*This;
{
 clipb_t *item;
 
 item = This->req_item;
 while(item && item->level != This->req_level)
  item = item->next;
 
 if (item)
 {
  This->req_item = item->next;
  return item->object;
 }
 else
  This->req_item = NULL;
 return NULL;
}

static void purge_item(This, item, pitem)
c_ClipBuffer *This;
clipb_t *item;
clipb_t *pitem;
{
#ifndef NTRACE
 if (!item)
  Xc_TRACE(("Warning cannot delete object: not found in list"));
 else
#else
 if (item)
#endif
 {
  if (!pitem)
   This->base = item->next;
  else
   pitem->next = item->next;

  if (item->destructor)
   item->destructor(item->object);

  Xc_free(item);
 }
}
