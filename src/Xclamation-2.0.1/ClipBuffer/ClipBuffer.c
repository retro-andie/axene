/*
** ClipBuffer.c for Xclamation in ClipBuffer/
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
** Started on  Mon Jul 17 14:12:01 1995 Emmanuel Paris
** Last update Mon Apr 20 15:27:38 1998 Emmanuel Paris
*/

#include "ClipBuffer.h"

extern c_Color *GlobColorBase;
extern c_VectorFont *GlobFontBase;

static void *cons_ClipBuffer();
static void dest_ClipBuffer();
static void *copy_ClipBuffer();

static void add_object_in_list();
static void del_object_in_list();
static void empty_list();

static void start_request_object();
static XcLM_Object *request_next_object();

sf_ClipBuffer fc_ClipBuffer =
{
 cons_ClipBuffer,
 dest_ClipBuffer,
 copy_ClipBuffer,
 add_object_in_list,
 del_object_in_list,
 empty_list,
 start_request_object,
 request_next_object,
};

static void *cons_ClipBuffer()
{
 c_ClipBuffer	*This;
 c_Color *color;
  
 This=(c_ClipBuffer *)Xc_malloc("clipbuffer",sizeof(c_ClipBuffer));
 This->f= &fc_ClipBuffer;
 This->LMobject= NULL;
  
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

 return This;
}

static void dest_ClipBuffer(This)
c_ClipBuffer	*This;
{
 F(This).empty_list(This);
  
 /*--- Destroy the clip_buffer text database ---*/
 while(This->BaseStd.text_base != NULL)
  DELETE(c_Text)(This->BaseStd.text_base);

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

static void add_object_in_list(This,object)
c_ClipBuffer	*This;
XcLM_Object	*object;
{
 l_XcLM_Object	*lmobject;
  
 if (This->LMobject)
 {
  lmobject=This->LMobject;
  while(lmobject->NextLMobject)
   lmobject=lmobject->NextLMobject;
  lmobject->NextLMobject=(l_XcLM_Object *)Xc_malloc("lc XcLMobj",
						    sizeof(l_XcLM_Object));
  lmobject=lmobject->NextLMobject;
  lmobject->object=object;
  lmobject->NextLMobject=NULL;
 }
 else
 {
  This->LMobject=(l_XcLM_Object *)Xc_malloc("lc XcLMobj",
					    sizeof(l_XcLM_Object));
  This->LMobject->object=object;
  This->LMobject->NextLMobject=NULL;
 }
}

static void del_object_in_list(This,object)
c_ClipBuffer	*This;
XcLM_Object	*object;
{
 Xc_TRACE(("Not done"));
}

static void empty_list(This)
c_ClipBuffer	*This;
{
 l_XcLM_Object	*lmobject;
 l_XcLM_Object	*lmobject2;
  
 lmobject=This->LMobject;
 while(lmobject)
 {
  lmobject2=lmobject->NextLMobject;
  Xc_free(lmobject);
  lmobject=lmobject2;
 }
 This->LMobject=NULL;
}

static void start_request_object(This)
c_ClipBuffer	*This;
{
 This->Request_LMobject = This->LMobject;
}

static XcLM_Object *request_next_object(This)
c_ClipBuffer	*This;
{
 XcLM_Object	*object;

 if (This->Request_LMobject)
 {
  object = This->Request_LMobject->object;
  This->Request_LMobject = This->Request_LMobject->NextLMobject;
  return object;
 }
 else
  return NULL;
}
