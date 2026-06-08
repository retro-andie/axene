/*
** Cut_Copy_Paste.c for XAllWrite in FrameManager/
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
** Started on  Mon Jul 17 20:26:12 1995 Emmanuel Paris
** Last update Tue Jun 23 16:41:16 1998 Emmanuel Paris
*/


#include "FrameManager.h"
#include "ClipBuffer.h"

extern c_ClipBuffer *GlobClipBuffer;

/*
#include "ClipIcon.h"
extern c_ClipIcon	*ClipIcon;
*/

extern void cut_selected_cadre(cfrmg)
c_FrMg	*cfrmg;
{
 l_Cadre	*lcadre;
 l_Cadre	*svglcadre;
 c_Color	*color;
 c_Cadre	*frame;
 int		type = 0;

 F(cfrmg).undisplay_selected_lcadre(cfrmg);
 lcadre = (l_Cadre *)sort_asc_list_cadre(cfrmg->SelectedCadre);
 F(GlobClipBuffer).start_request_object(GlobClipBuffer, 
					XcCB_CURRENT_LEVEL(GlobClipBuffer));
 
/* while((frame = (c_Cadre *)
	F(GlobClipBuffer).request_next_object(GlobClipBuffer)) != NULL)
  F(ClipIcon).Delete_Object(ClipIcon, &cadre); */

 F(GlobClipBuffer).empty_list(GlobClipBuffer);

 svglcadre = lcadre;
 while(lcadre)
 {
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
   type = XcCB_FRAME_EMPTY;
   break;
  case CADRE_IMAGE_BITMAP:
   type = XcCB_FRAME_IMAGE;
   break;
  case CADRE_TEXT:
   type = XcCB_FRAME_TEXT;
   break;
  case CADRE_VECTOR:
   type = XcCB_FRAME_VECTOR;
   break;
  }
  if (type != XcCB_FRAME_EMPTY)
  {
   void *ptr;
	 
   ptr = frame->object;
   if((CHANGE_BASE(ptr)(ptr, &GlobClipBuffer->BaseStd) != XC_NO_ERROR) ||
      (DISPLAY_INIT(ptr)(ptr, frame) != XC_NO_ERROR))
    F(frame).detachobject(frame);
  }
  F(GlobClipBuffer).add_object(GlobClipBuffer, (void *)frame, type, 
			       DELETE(c_Cadre));

  F(cfrmg).destroy_cadre(cfrmg,lcadre, NO_DEL_CADRE);
  lcadre = lcadre->NextCadre;
      
 }
 free_list_cadre(svglcadre,NO_DEL_CADRE);
}

/*-------------------------------------------------------------------------*/

extern void copy_selected_cadre(cfrmg)
c_FrMg	*cfrmg;
{
 l_Cadre	*lcadre;
 l_Cadre	*svglcadre;
 c_Cadre	*dupcadre;
 int		type = 0;
  
 lcadre=(l_Cadre *)sort_asc_list_cadre(cfrmg->SelectedCadre);
 F(GlobClipBuffer).start_request_object(GlobClipBuffer, 
					XcCB_CURRENT_LEVEL(GlobClipBuffer));
/* while((frame = 
	F(GlobClipBuffer).request_next_object(GlobClipBuffer)) != NULL)
  F(ClipIcon).Delete_Object(ClipIcon, &frame); */

 F(GlobClipBuffer).empty_list(GlobClipBuffer);
 svglcadre = lcadre;
 while(lcadre)
 {
  dupcadre = (c_Cadre *)COPY(c_Cadre)(lcadre->cadre,
				      &GlobClipBuffer->BaseStd);
  switch(lcadre->cadre->object_type)
  {
  case CADRE_EMPTY:
   type = XcCB_FRAME_EMPTY;
   break;
  case CADRE_IMAGE_BITMAP:
   type = XcCB_FRAME_IMAGE;
   break;
  case CADRE_TEXT:
   type = XcCB_FRAME_TEXT;
   break;
  case CADRE_VECTOR:
   type = XcCB_FRAME_VECTOR;
   break;
  }
  F(GlobClipBuffer).add_object(GlobClipBuffer, (void *)dupcadre, type, 
			       DELETE(c_Cadre));
  lcadre = lcadre->NextCadre;
 }
 free_list_cadre(svglcadre, NO_DEL_CADRE);
}


/*------------------------------------------------------------------------*/ 

extern void paste_selected_cadre(cfrmg)
c_FrMg	*cfrmg;
{
 c_Cadre	*cadre;
 c_Cadre	*frame;
 l_Cadre	*lcadre;
 int		priority;
 char		*name;
 char		*name2;
 boolean	recalibrate = FALSE;

 Xc_TRACE(("pasting a frame ..."));

 F(cfrmg).deselect_all_cadre(cfrmg,TRUE);
 priority = F(cfrmg).get_highest_priority(cfrmg);
 
 F(GlobClipBuffer).start_request_object(GlobClipBuffer, 
					XcCB_CURRENT_LEVEL(GlobClipBuffer));
 
 while((frame = 
	F(GlobClipBuffer).request_next_object(GlobClipBuffer)) != NULL)
 {
  name = (char *)Xc_malloc("name", strlen(frame->name) + 1);
  
  strcpy(name, frame->name);
  cadre = (c_Cadre *)COPY(c_Cadre)((c_Cadre *)frame, cfrmg->BaseStd);

  if (F(cfrmg).add_cadre(cfrmg, name, priority++, cadre) ==
      XC_NAME_ALREADY_EXIST)
  {
   name2 = F(cfrmg).copy_cadre_name(cfrmg, name);
   Xc_free(name);
   name = name2;
   F(cfrmg).add_cadre(cfrmg, name, priority++, cadre);
  }
  lcadre = F(cfrmg).get_cadre_by_name(cfrmg, name);
  Xc_free(name);

  cadre->sScale = cfrmg->sScale;
  cadre->sFactor = cfrmg->sFactor;
  cadre->Forme->Orig_X = cadre->sOrig_X / cadre->sScale;
  cadre->Forme->Orig_Y = cadre->sOrig_Y / cadre->sScale;
  F(cadre).calculscaling(cadre);
  F(cadre).calculorigine(cadre);
  F(cadre).calculforme(cadre, CONTOUR);
  if (cadre->object_type != CADRE_EMPTY)
  {
   F(cfrmg->cursor).push_and_set_cursor(cfrmg->cursor, C_WAIT);
   if (DISPLAY_INIT(cadre->object)(cadre->object, cadre) != XC_NO_ERROR)
    F(cadre).detachobject(cadre);
   F(cfrmg->cursor).pop_cursor(cfrmg->cursor);
  }

  F(lcadre->cadre).reinit_X_info(lcadre->cadre,
				 cfrmg->X_info.display,
				 cfrmg->X_info.window);

  F(cfrmg).draw_cadre(cfrmg, lcadre);
  F(cfrmg).select_cadre(cfrmg, lcadre, FALSE);

  if ((lcadre->cadre->textflow_mode & XcTFM_HOLE_MASK) > XcTFM_OVER)
   recalibrate = TRUE;
 }
 
 cfrmg->modify_cadre_callback(cfrmg->callback_data, recalibrate);
  
 F(cfrmg).calcul_select_handle(cfrmg);
 F(cfrmg).display_selected_lcadre(cfrmg); 
}









