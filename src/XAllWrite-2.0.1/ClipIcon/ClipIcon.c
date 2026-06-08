/*
** ClipIcon.c for XAllWrite in ClipIcon/
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
** Started on  Thu Nov 24 16:42:30 1994 Emmanuel Paris
** Last update Wed Jun  4 17:38:32 1997 Emmanuel Paris
*/

#include "Page.h"
#undef d(a)
#include "ClipIcon.h"

#include "../Pixmaps/clipboard.cadre.zpm"
/*
  #include "../Pixmaps/clipboard.document.zpm" 
  #include "../Pixmaps/clipboard.page.zpm" 
  #include "../Pixmaps/clipboard.vectoriel.zpm" 
  */
#include "../Pixmaps/clipboard.image.zpm" 
#include "../Pixmaps/clipboard.texte.zpm" 
#include "../Pixmaps/clipboard.unknown.zpm" 

extern c_LM_Client	*High_Client;

static void *cons_ClipIcon();
static void dest_ClipIcon();
static void *copy_ClipIcon();
void eventlook_ClipIcon();
static int New_Object();
static void Delete_Object();

void ClipIcon_drop_cadre_move_done();
void ClipIcon_drop_cadre_copy_done();

sf_ClipIcon fc_ClipIcon =
{
 cons_ClipIcon,
 dest_ClipIcon,
 copy_ClipIcon,
 eventlook_ClipIcon,
 New_Object,
 Delete_Object,
};

static void *cons_ClipIcon(display, widget, Clipboard)
Display    	*display;
Widget     	widget;
c_LM_Client 	*Clipboard;
{
 c_ClipIcon *Objet;
  
 if ((Objet = 
      (c_ClipIcon *)Xc_malloc("ClipIcon", sizeof(c_ClipIcon))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 Objet->f = &fc_ClipIcon;  
 Objet->display = display;
 Objet->widget = widget;
 Objet->window = XtWindow(widget);
 Objet->High_Client = Clipboard;
 Objet->List = NULL;

 /*  F(GlobDragAndDrop).select_input(GlobDragAndDrop,window,DD_DROP,
     DD_LONG,eventlook_ClipIcon,
     DD_LONG,Objet,
     DD_INT,C_DROP_ClipIcon|(1<<20),
     DD_INT,getpid(),NULL);
     */
 return Objet;
}

static void dest_ClipIcon(This)
c_ClipIcon   *This;
{
 ClipIcon_t	*clip_icon;
 ClipIcon_t	*clip_icon2;
  
 clip_icon = This->List;
 while(clip_icon)
 {
  DELETE(c_Icon)(clip_icon->Icon);
  clip_icon2 = clip_icon->Next;
  Xc_free(clip_icon);
  clip_icon = clip_icon2;
 }
 Xc_free(This);
}

static void *copy_ClipIcon(This)
c_ClipIcon   This;
{
 Xc_TRACE(("copy ClipIcon function not done.normally not usefull."));
 return NULL;
}

void eventlook_ClipIcon(This,event)
c_ClipIcon	*This;
XEvent	event;
{
 /*
   if (event.type==ClientMessage)
   {
   Xc_TRACE(("Drop on ClipIcon"));
   if (DROP_ID(event)==getpid())
   {
   if ((DROP_FLAGS(event) & 0xf0)==DD_DROP_FOR_MOVE)
   {
   Xc_TRACE(("ClipIcon: Drop cadre for move"));
   ClipIcon_drop_cadre_move_done(This,DROP_DATA(event));
   }
   else
   {
   Xc_TRACE(("ClipIcon: Drop cadre for copy"));
   ClipIcon_drop_cadre_copy_done(This,DROP_DATA(event));
   }
   }
   else
   Xc_TRACE(("Hum... Drop from another cession not allowed"));
   }
   else
   Xc_TRACE(("event sendwithout object"));
   */
}
void ClipIcon_drop_cadre_move_done(This, data)
c_ClipIcon	*This;
void		*data;
{
 l_Cadre *lcadre;
 l_Cadre	*svglcadre;
 XcLM_Object	*object_i;

 lcadre = svglcadre = (l_Cadre *)sort_asc_list_cadre((l_Cadre *)data);
 while(lcadre)
 {
  object_i = NULL;
      
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
			  XcLM_ALL_CLASS);
  lcadre = lcadre->NextCadre;
 }
 free_list_cadre(svglcadre, NO_DEL_CADRE);
}

void ClipIcon_drop_cadre_copy_done(This, data)
c_ClipIcon	*This;
void		*data;
{

}

/* -------------------------------------------------------------------- **
** --------- Add an object to clipboard .... icon will be added ------- **
** -------------------------------------------------------------------- */
static int 	New_Object(ClipIcon, ObjTmp, Name, Attributes, Class, 
			   Length, Data, Type)
c_ClipIcon			*ClipIcon;
XcLM_Object			**ObjTmp;
char				*Name;
XcLM_Object_AttributesMask	Attributes;
XcLM_ClassMask			Class;
XcLM_Object_Length		Length;
char				*Data;
XcLM_Object_Type		Type;
{
 ClipIcon_t	**TmpList;

 XcLM_Client_Build_Object(ObjTmp, ClipIcon->High_Client, Name, Attributes, 
			  Class, Length, Data, Type);

 TmpList = &ClipIcon->List;
 while (*TmpList)
  TmpList = &(*TmpList)->Next;
 if ((*TmpList = (ClipIcon_t *)Xc_malloc("ClipIcon W",
					 sizeof(ClipIcon_t)))
     == NULL)
  return -1;
 (*TmpList)->Object = ObjTmp;
 (*TmpList)->Next = NULL;
 switch (Type)
 {
 case  XcLM_FRAME :
  switch(Class & (XcLM_TEXT_CLASS | XcLM_BITMAP_CLASS))
  {
  case XcLM_TEXT_CLASS:
   (*TmpList)->Icon = (c_Icon *)NEW(c_Icon)
    (ClipIcon->widget, NULL,
     "Icon_text",
     ClipIcon->display, clipboard_texte_xpm, NULL, XcLabel);
   break;
  case XcLM_BITMAP_CLASS:
   (*TmpList)->Icon = (c_Icon *)NEW(c_Icon)
    (ClipIcon->widget, NULL,
     "Icon_image",
     ClipIcon->display, clipboard_image_xpm, NULL, XcLabel);
   break;
  default:
   (*TmpList)->Icon = (c_Icon *)NEW(c_Icon)
    (ClipIcon->widget, NULL,
     "Icon_cadre",
     ClipIcon->display, clipboard_cadre_xpm, NULL, XcLabel);
   break;
  }
  XtManageChild(WIDGET((*TmpList)->Icon));
  break;
 default:
  (*TmpList)->Icon = (c_Icon *)NEW(c_Icon)
   (ClipIcon->widget, NULL,
    "Icon_unknown",
    ClipIcon->display, clipboard_unknown_xpm, NULL, XcLabel);
  XtManageChild(WIDGET((*TmpList)->Icon));
 }
 Xc_TRACE(("icone cree ..."));
 return (0);
}

static void Delete_Object(ClipIcon, ObjTmp)
c_ClipIcon			*ClipIcon;
XcLM_Object			**ObjTmp;
{
 ClipIcon_t	*TmpList;
 ClipIcon_t	*TmpList2;
  
 TmpList = TmpList2 = ClipIcon->List;  
 while(TmpList && TmpList->Object!=ObjTmp)
 {
  TmpList2 = TmpList;
  TmpList = TmpList->Next;
 }
 if (!TmpList)
 {
  Xc_WARNING(("Object not found in ClipBuffer: can't destroy it"));
  return;
 }

 if (TmpList == TmpList2)
  ClipIcon->List = TmpList->Next;
 else
  TmpList2->Next = TmpList->Next;
  
 XcLM_Client_Destroy_Object(ClipIcon->High_Client, TmpList->Object);
 DELETE(c_Icon)(TmpList->Icon);
 Xc_free(TmpList);
}










