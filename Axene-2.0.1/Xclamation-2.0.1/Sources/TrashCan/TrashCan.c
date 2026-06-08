/*
** TrashCan.c for Xclamation, XQuad and XAllWrite in TrashCan/
** Trash Manager
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
** Started on  Thu Nov 24 16:39:47 1994 Emmanuel Paris
** Last update Mon Apr 20 14:53:32 1998 Emmanuel Paris
*/

#include "TrashCan.h"

#ifdef Xc_XCLAMATION
#include "Page.h"     /* only for l_Cadre struct */
#endif

#if defined(Xc_XQUAD) || defined(Xc_XALLWRITE)
#include "FrameManager.h"
#endif

static void *cons_Trash();
static void dest_Trash();
static void *copy_Trash();
static void eventlook_trash();

static void trash_drop_cadre_move_done();
static void trash_drop_cadre_copy_done();

sf_Trash fc_Trash=
{
 cons_Trash,
 dest_Trash,
 copy_Trash,
 eventlook_trash,
};

static void *cons_Trash(display, window)
Display    *display;
Window     window;
{
 c_Trash *This;
  
 if ((This = (c_Trash *)Xc_malloc("Trash",sizeof(c_Trash)))==NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_Trash;  
 This->display = display;
 This->window = window;

 F(GlobDragAndDrop).select_input(GlobDragAndDrop,window, DD_DROP,
				 DD_LONG, eventlook_trash,
				 DD_LONG, This,
				 DD_INT, C_DROP_TRASH | (1 << 20),
				 DD_INT, getpid(), NULL);
 return This;
}

static void dest_Trash(This)
c_Trash   *This;
{
 Xc_free(This);
}

static void *copy_Trash(This)
c_Trash   This;
{
 Xc_TRACE(("copy trash function not done.normally not usefull."));
 return NULL;
}

static void eventlook_trash(This,event)
c_Trash	*This;
XEvent	*event;
{
 if (event->type == ClientMessage)
 {
  Xc_TRACE(("Drop on Trash"));
  if (DROP_ID(*event) == getpid())
  {
   if ((DROP_FLAGS(*event) & 0xf0) == DD_DROP_FOR_MOVE)
   {
    Xc_TRACE(("Trash: Drop cadre for move"));
    trash_drop_cadre_move_done(This, DROP_DATA(*event));
   }
   else
   {
    Xc_TRACE(("Trash: Drop cadre for copy"));
    trash_drop_cadre_copy_done(This, DROP_DATA(*event));
   }
  }
#ifndef NTRACE
  else
   Xc_TRACE(("Hum... Drop from another cession not allowed"));
#endif
 }
#ifndef NTRACE
 else
  Xc_TRACE(("event sendwithout object"));
#endif
}

static void trash_drop_cadre_move_done(This,data)
c_Trash	*This;
void	*data;
{
 l_Cadre	 *lcadre;
 l_Cadre	*svglcadre;
 /*  XcLM_Object	*object_i; */
  
 lcadre = svglcadre = (l_Cadre *)sort_asc_list_cadre((l_Cadre *)data);
 while(lcadre)
 {
  /*      object_i = NULL;
	  
	  F(High_Client).C_Build_Object(&object_i, High_Client, 
	  lcadre->name,
	  XcLM_USER_READ | XcLM_USER_WRITE |
	  XcLM_GROUP_READ | XcLM_ALL_READ,
	  XcLM_TEXT_CLASS | XcLM_TRASH |
	  XcLM_LOCAL_MANAGED,
	  -1,
	  lcadre->cadre,
	  XcLM_FRAME);
	  XcLM_Client_List_Object(High_Client->Trash_Object_List,
	  XcLM_ALL_CLASS); */
  DELETE(c_Cadre)(lcadre->cadre);
  lcadre = lcadre->NextCadre;
 }
 free_list_cadre(svglcadre, NO_DEL_CADRE);
 free_list_cadre((l_Cadre *)data, NO_DEL_CADRE);
}

static void trash_drop_cadre_copy_done(This, data)
c_Trash	*This;
void	*data;
{

}
