/*
** ContextMenu.c for Xclamation in ContextMenu/
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
** Started on  Mon Jul 17 14:18:34 1995 Emmanuel Paris
** Last update Sat Nov 14 16:57:07 1998 Emmanuel Paris
*/


#define _CONTEXT_MENU_PRIVATE_

#include "ContextMenu.h"
#include "../Pixmaps/import.texte.zpm"
#include "../Pixmaps/import.image.zpm"
#include "../Pixmaps/import.vector.zpm"
#include "../Pixmaps/popup.texte.edit.zpm"

#include "../Pixmaps/page.inserer.avant.zpm"
#include "../Pixmaps/page.inserer.apres.zpm"
#include "../Pixmaps/page.modifier.zpm"
#include "../Pixmaps/page.supprimer.zpm"

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

extern Widget arrow;

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
  
 for(i=0; i< (delay ? 70 : 15); i++)
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
 case CM_PAGER:
  This->popup = (c_PopupMenu *)NEW(c_PopupMenu)
   (MainInterface1->w_Hidden, "Pager Context Menu", 
    ICONMODEON, 1,
    (char *)page_inserer_avant_xpm, CM_Pgr_INSERT_BEFORE,
    HELPLINE, XcR_helpCMPgrInsBef,
    MENUSEPARATOR, CM_Pgr_SEPARATOR_1,
    (char *)page_inserer_apres_xpm, CM_Pgr_INSERT_AFTER,
    HELPLINE, XcR_helpCMPgrInsAft,
    MENUSEPARATOR, CM_Pgr_SEPARATOR_2,
    (char *)page_modifier_xpm, CM_Pgr_MODIFY,
    HELPLINE, XcR_helpCMPgrModify,
    MENUSEPARATOR, CM_Pgr_SEPARATOR_3,
    (char *)page_supprimer_xpm, CM_Pgr_DELETE,
    HELPLINE, XcR_helpCMPgrDelete,
    ICONMODEOFF, 1,
    NULL);
      
  nb_menu = CM_Pgr_callback_num;
  cont_menu = CM_Pgr_callback;
  break;

 case CM_CADRE_EMPTY:
  This->popup = (c_PopupMenu *)NEW(c_PopupMenu)
   (MainInterface1->w_Hidden, "Frame Empty Context Menu", 
    ICONMODEON, 1,
    (char *)import_texte_xpm, CM_Cde_IMPORT_TEXT,
    HELPLINE, XcR_helpCMCdeImpTexte,
    MENUSEPARATOR, CM_Cde_SEPARATOR_1,
    (char *)import_image_xpm, CM_Cde_IMPORT_IMAGE,
    HELPLINE, XcR_helpCMCdeImpImage,
    MENUSEPARATOR, CM_Cde_SEPARATOR_2,
    (char *)import_vector_xpm, CM_Cde_IMPORT_VECTOR,
    HELPLINE, XcR_helpCMCdeImpVector,
    MENUSEPARATOR, CM_Cde_SEPARATOR_3,
    (char *)popup_texte_edit_xpm, CM_Cde_EDIT_TEXTE,
    HELPLINE, XcR_helpTextEditor,
    ICONMODEOFF, 1,
    NULL);
      
  nb_menu = CM_Cde_callback_num;
  cont_menu = CM_Cde_callback;
  break;
      
 case CM_PAGE_EMPTY:
  This->popup = (c_PopupMenu *)NEW(c_PopupMenu)
   (MainInterface1->w_Hidden, "Page Empty Context Menu", 
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
   
 case CM_CADRE_TEXT:
  This->popup = (c_PopupMenu *)NEW(c_PopupMenu)
   (MainInterface1->w_Hidden, "Text Frame Context Menu", 
    ICONMODEON, 1,
    (char *)popup_texte_edit_xpm, CM_Cdt_EDIT_TEXTE,
    HELPLINE, XcR_helpTextEditor,
    ICONMODEOFF, 1,
    NULL);
      
  nb_menu = CM_Cdt_callback_num;
  cont_menu = CM_Cdt_callback;
  break;
    
 case CM_KEYBOARD:
  This->popup = (c_PopupMenu *)NEW(c_PopupMenu)
   (MainInterface1->w_Hidden, "Copy Buffer Context Menu", 
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

 default:
  Xc_WARNING(("Context Menu Not Defined !!!!"));
  Xc_free(This);
  return NULL;
  break;
 } 
 for( i=0; i<nb_menu; i++)
 {
  cont_menu[i].ret_button = ret_button;
  F(This->popup).AddCallback(This->popup, cont_menu[i].button,
			     (void *)callback, &cont_menu[i], NULL);
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

