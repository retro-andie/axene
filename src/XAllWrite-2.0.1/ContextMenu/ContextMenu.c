/*
** ContextMenu.c for XAllWrite in ContextMenu/
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
** Started on  Mon Jul 17 14:18:34 1998 Emmanuel Paris
** Last update Sat Aug 14 18:55:56 1999 Emmanuel Paris
*/


#define _CONTEXT_MENU_PRIVATE_

#include "ContextMenu.h"
#include "../Pixmaps/import.image.zpm"
#include "../Pixmaps/import.vector.zpm"

#include "../Pixmaps/creer.zpm"
#include "../Pixmaps/creer.ovale.zpm"
#include "../Pixmaps/creer.lignes.zpm"

#include "Resource.h"

static void *cons_ContextMenu();
static void dest_ContextMenu();
static void *copy_ContextMenu();
static void wait_for_unmap_ContextMenu();
static void callback();

sf_ContextMenu fc_ContextMenu =
{
 cons_ContextMenu,
 dest_ContextMenu,
 copy_ContextMenu,
 wait_for_unmap_ContextMenu
};

static void *cons_ContextMenu(display, num_menu, 
			      ret_button, delay,  event, param)
Display		*display;
num_CM_t	num_menu;
int		*ret_button;
boolean		delay;
XEvent		*event;
void		*param;
{
 XEvent	event2, none;
 int		i;
 int		nb_event;
 int		xo, yo, xm, ym;
 but_CM_s	*cont_menu;
 int		nb_menu;
 c_ContextMenu	*This;
  
 if ((This = (c_ContextMenu *)Xc_malloc("ContextMenu", 
					sizeof(c_ContextMenu))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_ContextMenu;
  
 xo = event->xbutton.x;
 yo = event->xbutton.y;
  
 for(i=0; i< (delay ? 100 : 15); i++)
 {
  Zusleep(CM_PAUSE/100);
  if ((nb_event = XEventsQueued(display, QueuedAfterReading)) != 0)
  {
   if (XCheckTypedEvent(display, ButtonRelease, &none))
   {
    *ret_button = 0;
    Xc_free(This);
    return NULL;
   }
   if (XCheckTypedEvent(display, ButtonPress, &none))
   {
    *ret_button = 0;
    Xc_free(This);
    return NULL;
   }
   while(nb_event)
   {
    XPeekEvent(display, &event2);
    switch(event2.type)
    {
    case GraphicsExpose:
    case NoExpose:
    case Expose:
     XNextEvent(display, &event2);
     break;
    case MotionNotify:
     XNextEvent(display, &event2);
     if (delay)
     {
      xm = event2.xmotion.x;
      ym = event2.xmotion.y;
      if (xm>xo+5 || xm<xo-5 || ym>yo+5 || ym<yo-5)
      {
       *ret_button = 0;
       Xc_free(This);
       return NULL;
       break;
      }
     }
     break;
    case ButtonRelease:
    case ButtonPress:
    default:
     *ret_button = 0;
     Xc_free(This);
     return NULL;
     break;
    }
    nb_event--;
   }
  }
 }
  
 *ret_button = 0;

#if (XmVERSION == 2)			/* MOTIF 2.X */
 if (num_menu != CM_KEYBOARD)
  XtDispatchEvent(event);
#endif
  
 switch(num_menu)
 {
 case CM_KEYBOARD:
  This->popup = (c_PopupMenu *)NEW(c_PopupMenu)
   (MainInterface1->w_This, "Copy Buffer Context Menu", 
    *((char (*)[45])param  ), CM_Kbd_BUFFER_0,
    *((char (*)[45])param+1), CM_Kbd_BUFFER_1,
    *((char (*)[45])param+2), CM_Kbd_BUFFER_2,
    *((char (*)[45])param+3), CM_Kbd_BUFFER_3,
    *((char (*)[45])param+4), CM_Kbd_BUFFER_4,
    *((char (*)[45])param+5), CM_Kbd_BUFFER_5,
    *((char (*)[45])param+6), CM_Kbd_BUFFER_6,
    *((char (*)[45])param+7), CM_Kbd_BUFFER_7,
    NULL);
      
  nb_menu = CM_Kbd_callback_num;
  cont_menu = CM_Kbd_callback;
  break;

 case CM_XAREGLE:
  This->popup = (c_PopupMenu *)NEW(c_PopupMenu)
   (MainInterface1->w_Hidden, "Ruler Context Menu",
    SUBMENUON, CM_Xar_SUBMENU1,
    "mAlignment", CM_Xar_TAB_ALIGNMENT,
    "mLeft", CM_Xar_TAB_LEFT,
    "mRight", CM_Xar_TAB_RIGHT,
    "mCenter", CM_Xar_TAB_CENTER,
    "mCperiod" ,CM_Xar_TAB_CPERIOD,
    SUBMENUOFF, CM_Xar_SUBMENU1,
    SUBMENUON, CM_Xar_SUBMENU2,
    "mFiller", CM_Xar_FILLER,
    "mNone", CM_Xar_FILLER_NONE,
    "mPoint", CM_Xar_FILLER_POINT,
    "mDot", CM_Xar_FILLER_DOT,
    "mHyphen", CM_Xar_FILLER_HYPHEN,
    MENUSEPARATOR, CM_Xar_SEP1,
    "mUserdef", CM_Xar_FILLER_USERDEF,  
    SUBMENUOFF, CM_Xar_SUBMENU2,
    MENUSEPARATOR, CM_Xar_SEP2,
    "mDelete", CM_Xar_TAB_DELETE,
    NULL);

  nb_menu = CM_Xar_callback_num;
  cont_menu = CM_Xar_callback;
  break;

 case CM_TEXT:
  This->popup = (c_PopupMenu *)NEW(c_PopupMenu)
   (MainInterface1->w_Hidden, "Text Context Menu",
    "mCut", CM_Tex_CUT,
    "mCopy", CM_Tex_COPY,
    "mPaste", CM_Tex_PASTE,
    NULL);
  
  nb_menu = CM_Tex_callback_num;
  cont_menu = CM_Tex_callback;
  break;

 case CM_CADRE_OBJECT:
  This->popup = (c_PopupMenu *)NEW(c_PopupMenu)
   (MainInterface1->w_Hidden, "Frame Object Context Menu", 
    ICONMODEON, 1,
    (char *)import_image_xpm, CM_Cde_IMPORT_IMAGE,
    HELPLINE, XcR_helpCMCdeImpImage,
    MENUSEPARATOR, CM_Cde_SEPARATOR_1,
    (char *)import_vector_xpm, CM_Cde_IMPORT_VECTOR,
    HELPLINE, XcR_helpCMCdeImpVector,
    ICONMODEOFF, 1,
    NULL);
      
  nb_menu = CM_Cde_callback_num;
  cont_menu = CM_Cde_callback;
  break;

 case CM_CREATE_CADRE:
  This->popup = (c_PopupMenu *)NEW(c_PopupMenu)
   (MainInterface1->w_Hidden, "Create Frame Context Menu", 
    ICONMODEON, 1,
    (char *)creer_xpm, CM_Pge_CREATE_SQUARE,
    HELPLINE, XcR_helpFrameSquare,
    MENUSEPARATOR, CM_Pge_SEPARATOR_1,
    (char *)creer_ovale_xpm, CM_Pge_CREATE_ELIPSE,
    HELPLINE, XcR_helpFrameCircle,
    MENUSEPARATOR, CM_Pge_SEPARATOR_2,
    (char *)creer_lignes_xpm, CM_Pge_CREATE_POLYGON,
    HELPLINE, XcR_helpFramePolygon,
    ICONMODEOFF, 1,
    NULL);
      
  nb_menu = CM_Pge_callback_num;
  cont_menu = CM_Pge_callback;
  break;

 default:
  Xc_WARNING(("Context Menu Not Defined !!!!"));
  Xc_free(This);
  return NULL;
  break;
 } 
 for( i=0; i<nb_menu; i++)
 {
  if (!cont_menu[i].button) continue;

  cont_menu[i].ret_button = ret_button;
  F(This->popup).AddCallback(This->popup, cont_menu[i].button,
			     (void *)callback,
			     &cont_menu[i], NULL);
 }
  
 F(This->popup).mapAndWaitForMap(This->popup, event);
 if (!This->popup->mapped_flag)
 {	
  DELETE(c_ContextMenu)(This);
  return NULL;
 }    
 Xc_TRACE(("Object ContextMenu build"));
 return This;
}

static void dest_ContextMenu(This)
     c_ContextMenu *This;
{
 DELETE(c_PopupMenu)(This->popup); 
 Xc_TRACE(("Object ContextMenu destroyed"));
 Xc_free(This);
}

static void *copy_ContextMenu(This)
c_ContextMenu *This;
{
 Xc_WARNING(("Copy object ContextMenu useless"));
 return NULL;
}


static void wait_for_unmap_ContextMenu(This)
c_ContextMenu	*This;
{
 F(This->popup).waitForUnmap(This->popup);
}

static void callback(w, button)
Widget		w;
but_CM_s	*button;
{
 *(button->ret_button) = button->button;
}

