/*
** Cut_Copy_Paste.c for Xclamation in Page/
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
** Started on  Mon Jul 17 20:26:12 1995 Emmanuel Paris
** Last update Fri Jan  7 15:59:59 2000 Emmanuel Paris
*/


#include "Page.h"

#ifdef d
# if defined(___rs6000) || \
    defined(__DECC)        /* __DECC : DEC C Compiler on Alpha platform */
#  undef d
# else
#  undef d(a)
# endif
#endif

#include "ClipIcon.h"
extern c_ClipIcon	*ClipIcon;

extern void cut_selected_cadre(cpage)
c_Page	*cpage;
{
 l_Cadre	*lcadre;
 l_Cadre	*svglcadre;
 c_Color	*color;
 c_Cadre	*frame;
 XcLM_Object	*object_i;
 int		type = 0;

 F(cpage).undisplay_selected_lcadre(cpage);
 lcadre=(l_Cadre *)sort_asc_list_cadre(cpage->SelectedCadre);
 F(GlobClipBuffer).start_request_object(GlobClipBuffer);
 while((object_i=F(GlobClipBuffer).request_next_object(GlobClipBuffer))!=NULL)
  F(ClipIcon).Delete_Object(ClipIcon, &object_i);
 F(GlobClipBuffer).empty_list(GlobClipBuffer);
 svglcadre=lcadre;
 while(lcadre)
 {
  object_i = NULL;

  Xc_TRACE(("sf cadre %p",lcadre->cadre->f));
  frame = lcadre->cadre;
      
  color = frame->border_color;
  if(((color =
       F(color).virtualCopy(color, &GlobClipBuffer->BaseStd)) == NULL) ||
     !F(frame).setOutlineColor(frame, color)) continue;

  color = frame->inside_color;
  if(((color = 
       F(color).virtualCopy(color, &GlobClipBuffer->BaseStd)) == NULL) ||
     !F(frame).setBackgroundColor(frame, color)) continue;
      
  switch(frame->object_type)
  {
  case CADRE_EMPTY:
   type = 0;
   break;
  case CADRE_IMAGE_BITMAP:
   type = XcLM_BITMAP_CLASS;
   break;
  case CADRE_TEXT:
   type = XcLM_TEXT_CLASS;
   break;
  case CADRE_VECTOR:
   type = XcLM_VECTOR_CLASS;
   break;
  }
  if (type)
  {
   void *ptr;
	 
   ptr = frame->object;
   if((CHANGE_BASE(ptr)(ptr, &GlobClipBuffer->BaseStd) != XC_NO_ERROR) ||
      (DISPLAY_INIT(ptr)(ptr, frame) != XC_NO_ERROR))
    F(frame).detachobject(frame);
  }
  F(ClipIcon).New_Object(ClipIcon, &object_i, 
			 lcadre->name,
			 XcLM_USER_READ | XcLM_USER_WRITE |
			 XcLM_GROUP_READ | XcLM_ALL_READ,
			 type | XcLM_PRIVATE |
			 XcLM_LOCAL_MANAGED,
			 -1,
			 frame,
			 XcLM_FRAME);
  F(GlobClipBuffer).add_object_in_list(GlobClipBuffer,object_i);
  XcLM_Client_List_Object(ClipIcon->High_Client->Private_Object_List,
			  XcLM_ALL_CLASS);
  F(cpage).destroy_cadre(cpage,lcadre, NO_DEL_CADRE);
  lcadre=lcadre->NextCadre;
      
 }
 free_list_cadre(svglcadre,NO_DEL_CADRE);
}

/*-------------------------------------------------------------------------*/

extern void copy_selected_cadre(cpage)
c_Page	*cpage;
{
 l_Cadre	*lcadre;
 l_Cadre	*svglcadre;
 c_Cadre	*dupcadre;
 XcLM_Object	*object_i;
 char		*name;
 int		type = 0;
  
 lcadre=(l_Cadre *)sort_asc_list_cadre(cpage->SelectedCadre);
 F(GlobClipBuffer).start_request_object(GlobClipBuffer);
 while((object_i=F(GlobClipBuffer).request_next_object(GlobClipBuffer))!=NULL)
  F(ClipIcon).Delete_Object(ClipIcon, &object_i);
 F(GlobClipBuffer).empty_list(GlobClipBuffer);
 svglcadre=lcadre;
 while(lcadre)
 {
  dupcadre=(c_Cadre *)COPY(c_Cadre)(lcadre->cadre,
				    &GlobClipBuffer->BaseStd);
  name=F(cpage).copy_cadre_name(cpage,lcadre->name);
  switch(lcadre->cadre->object_type)
  {
  case CADRE_EMPTY:
   type = 0;
   break;
  case CADRE_IMAGE_BITMAP:
   type = XcLM_BITMAP_CLASS;
   break;
  case CADRE_TEXT:
   type = XcLM_TEXT_CLASS;
   break;
  case CADRE_VECTOR:
   type = XcLM_VECTOR_CLASS;
   break;
  }
  F(ClipIcon).New_Object(ClipIcon, &object_i,
			 name,
			 XcLM_USER_READ | XcLM_USER_WRITE |
			 XcLM_GROUP_READ | XcLM_ALL_READ,
			 type | XcLM_PRIVATE |
			 XcLM_LOCAL_MANAGED,
			 -1,
			 dupcadre,
			 XcLM_FRAME);
  F(GlobClipBuffer).add_object_in_list(GlobClipBuffer,object_i);
  XcLM_Client_List_Object(ClipIcon->High_Client->Private_Object_List,
			  XcLM_ALL_CLASS);
  lcadre=lcadre->NextCadre;
 }
 free_list_cadre(svglcadre,NO_DEL_CADRE);
}


/*------------------------------------------------------------------------*/ 

extern void paste_selected_cadre(cpage)
c_Page	*cpage;
{
 l_XcLM_Object	*lobjet;
 c_Cadre	*cadre;
 l_Cadre	*lcadre;
 int		priority;
 char		*name;
 char		*name2;

 Xc_TRACE(("pasting a frame ..."));

 F(cpage).deselect_all_cadre(cpage,TRUE);
 priority=F(cpage).get_highest_priority(cpage);
 lobjet = GlobClipBuffer->LMobject;
 while(lobjet)
 {
  name = (char *) Xc_malloc("name",
			    strlen( lobjet->object->Descriptor->Name)+1);
      
  strcpy(name,lobjet->object->Descriptor->Name);
  cadre = (c_Cadre *)COPY(c_Cadre)((c_Cadre *)lobjet->object->Data,
				   cpage->BaseStd);
  /*      XcLM_Client_Destroy_Object(High_Client, &(lobjet->object));*/
  if (F(cpage).add_cadre(cpage,name,priority++,cadre)==
      XC_NAME_ALREADY_EXIST)
  {
   name2=F(cpage).copy_cadre_name(cpage,name);
   Xc_free(name);
   name=name2;
   F(cpage).add_cadre(cpage,name,priority++,cadre);
  }
  lcadre=F(cpage).get_cadre_by_name(cpage,name);

  /*    cadre->sOrig_X=cadre->Forme->Orig_X * cpage->sScale / cadre->sScale;
	cadre->sOrig_Y=cadre->Forme->Orig_Y * cpage->sScale / cadre->sScale;
	*/
  cadre->sScale=cpage->sScale;
  cadre->sFactor=cpage->sFactor;
  cadre->Forme->Orig_X = cadre->sOrig_X / cadre->sScale;
  cadre->Forme->Orig_Y = cadre->sOrig_Y / cadre->sScale;
  F(cadre).calculscaling(cadre);
  F(cadre).calculorigine(cadre);
  F(cadre).calculforme(cadre,CONTOUR);
  if (cadre->object_type!=CADRE_EMPTY)
  {
   F(cpage->cursor).push_and_set_cursor(cpage->cursor,C_WAIT);
   if (DISPLAY_INIT(cadre->object)(cadre->object,cadre)!=XC_NO_ERROR)
    F(cadre).detachobject(cadre);
   F(cpage->cursor).pop_cursor(cpage->cursor);
  }

  F(lcadre->cadre).reinit_X_info(lcadre->cadre,
				 cpage->X_info.display,
				 cpage->X_info.window);

  F(cpage).draw_cadre(cpage,lcadre);
  F(cpage).select_cadre(cpage,lcadre,FALSE);
  lobjet=lobjet->NextLMobject;
 }
 F(cpage).calcul_select_handle(cpage);
 F(cpage).display_selected_lcadre(cpage); 
 /*  F(GlobClipBuffer).empty_list(GlobClipBuffer);*/
}



