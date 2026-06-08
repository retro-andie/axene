/*
** Interface.c for AxeneOffice in Interface/
** Interface Part of AxeneOffice
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
** Started on  Mon Jul 17 15:05:00 1995 Antoine Buat
** Last update Sun Jan 19 09:29:16 2048 Emmanuel Paris
*/

#include "Interface.h"
#include "InterfaceP.h"
#include "DocIcon.h"

extern c_DocIcon *GlobDocIcon;

static void *cons_Interface();
static void dest_Interface();
static void *copy_Interface();
static c_Interface *get_Interface_by_widget ___PROTO((c_Interface *base,
						      Widget widget));
static c_Interface *get_Interface_by_window ___PROTO((c_Interface *base,
						      Window window));

static void create_interface ___PROTO((c_Interface *This));
static void destroy_interface ___PROTO((c_Interface *This));

extern void cb_quit();
extern void cb_override();
extern void cb_transient();
extern void cb_flip();
extern void cb_launch();

#if XmUPDATE_LEVEL > 2
extern void _XmRegisterPixmapConverters();
#endif

sf_Interface fc_Interface =
{
 cons_Interface,
 dest_Interface,
 copy_Interface,
 get_Interface_by_widget,
 get_Interface_by_window
};

static void *cons_Interface(MainInterface)
c_MainInterface *MainInterface;
{
 c_Interface	*This;
 Xc_HISTORY(("Constructor ..."));
  
 if ((This = (c_Interface *)Xc_malloc("Interface", sizeof(c_Interface)))
     == NULL)
 {
  Xc_FATAL(("Can't build Interface."));
 }
 memset(This, 0, sizeof(c_Interface));
 This->f = &fc_Interface;
  
 This->MainInterface = MainInterface;
 This->Manager = (c_ManageWidget *)NEW(c_ManageWidget)();
 F(This->MainInterface).create_TopLevel(This->MainInterface, This->Manager);
#if XmUPDATE_LEVEL > 2
 _XmRegisterPixmapConverters();
#endif
 F(GlobDocIcon).registerPixmapConverters(GlobDocIcon);
 create_interface(This);
  
 F(This->Manager).Manage_all(This->Manager, NULL);
 F(This->MainInterface).map_TopLevel(This->MainInterface);
  
 F(This->MainInterface).set_main_icon(This->MainInterface);
 This->AppTitle[0] = '\0';
 This->override = FALSE;
 This->transient = TRUE;
 This->flip = FALSE;

 This->Prev = This->Next = NULL;
  
 Xc_HISTORY(("Constructor ok..."));
 return This;
}

static void dest_Interface(This, all)
c_Interface *This;
boolean all;
{
 Xc_HISTORY(("Destructor ..."));
    
 if (all)
 {
  while(This->Prev)
   This = This->Prev;
      
  while(This)
  {
   c_Interface *NextInterface;
	
   NextInterface = This->Next;
   DELETE(c_Interface)(This, FALSE);
   This = NextInterface;
  }
  return;
 }
    
 if (This->Prev)
  This->Prev->Next = This->Next;
 if (This->Next)
  This->Next->Prev = This->Prev;
    
 F(This->MainInterface).unmap_TopLevel(This->MainInterface);
 destroy_interface(This);
 DELETE(c_MainInterface)(This->MainInterface);
    
 Xc_free(This);
    
 Xc_HISTORY(("Destructor ok ..."));
};

static void *copy_Interface(This)
c_Interface *This;
{
 c_Interface *New;
 c_MainInterface *MainInterface;
  
 Xc_HISTORY(("Add New ..."));
  
 MainInterface = (c_MainInterface *)
  COPY(c_MainInterface)(This->MainInterface);
  
 New = (c_Interface *)NEW(c_Interface)(MainInterface);
  
 New->Prev = This;
 New->Next = This->Next;
 This->Next = New;
  
 Xc_HISTORY(("Add New ok ..."));
 return New;
};

static c_Interface *get_Interface_by_widget(base, widget)
c_Interface *base;
Widget widget;
{
 while(!XtIsTopLevelShell(widget))
 {
  widget = XtParent(widget);
 }
  
 if (!base->Prev)
 {
  while(base->Prev)
   base = base->Prev;
 }
  
 while(base)
 {
  if (base->MainInterface->w_Main == widget)
   return base;
  base = base->Next;
 }
 return NULL;
}

static c_Interface *get_Interface_by_window(base, window)
c_Interface *base;
Window window;
{
 Window iwin;
 unsigned int	min;
 c_Interface *Interface;
  
 if (!base->Prev)
 {
  while(base->Prev)
   base = base->Prev;
 }
  
 min = (unsigned int)-1;
 Interface = NULL;
  
 while(base)
 {
  iwin = XtWindow(base->MainInterface->w_Main);
    
  if ((window >= iwin) && ((unsigned int)(window - iwin) < min))
  {
   min = (unsigned int)(window - iwin);
   Interface = base;
  }
  base = base->Next;
 }
 return Interface;
}

/* ------------------------------------------------------------------------ **
** Interface --                                                             ** 
** ------------------------------------------------------------------------ */

#include "../Pixmaps/small_xc_r.zpm"
#include "../Pixmaps/small_xq_r.zpm"
#include "../Pixmaps/small_xa_r.zpm"
#include "../Pixmaps/small_xm_r.zpm"

static void create_interface(This)
c_Interface *This;
{
 int i;

 This->rcMain = (c_RowColumn *)NEW(c_RowColumn)(WIDGET(This->MainInterface),
						This->Manager, "rcMain", 
						10, 10, XmHORIZONTAL);
 This->Icon[0] = (c_Icon *)
  NEW(c_Icon)(WIDGET(This->rcMain), This->Manager,
	      "Icon1", XtDisplay(WIDGET(This->rcMain)), 
	      small_xc_r_xpm , NULL, XcPush);

 This->Icon[1] = (c_Icon *)
  NEW(c_Icon)(WIDGET(This->rcMain), This->Manager,
	      "Icon2", XtDisplay(WIDGET(This->rcMain)), 
	      small_xq_r_xpm , NULL, XcPush);

 This->Icon[2] = (c_Icon *)
  NEW(c_Icon)(WIDGET(This->rcMain), This->Manager,
	      "Icon3", XtDisplay(WIDGET(This->rcMain)), 
	      small_xa_r_xpm , NULL, XcPush);

 This->Icon[3] = (c_Icon *)
  NEW(c_Icon)(WIDGET(This->rcMain), This->Manager,
	      "Icon4", XtDisplay(WIDGET(This->rcMain)), 
	      small_xm_r_xpm , NULL, XcPush);

 This->nbIcon = NB_SOFT;

 This->popup = (c_PopupMenu *)NEW(c_PopupMenu) 
  (This->MainInterface->w_Hidden, "Popup", 
   TOGGLEMODEON, RIEN,
   "mOverride", POPUP_OVERRIDE,
   "mTransient", POPUP_TRANSIENT,
   TOGGLEMODEOFF, RIEN,
   NOMANAGE, RIEN,
   "mFlipH", POPUP_FLIP_HORIZONTAL,
   "mFlipV", POPUP_FLIP_VERTICAL,
   MENUSEPARATOR, POPUP_SEP1,
   SUBMENUON, POPUP_SUBMENU1,
   "mLaunch", POPUP_LAUNCH,
   "Xclamation", POPUP_XCLAMATION,
   "XQuad", POPUP_XQUAD,
   "XAllWrite", POPUP_XALLWRITE,
   "XMayday", POPUP_XMAYDAY,
   SUBMENUOFF, RIEN,
   MENUSEPARATOR, POPUP_SEP2,
   "mQuit", POPUP_QUIT,
   NULL);
 
 { 
  Dimension iH=0, iW=0, mH=0, mW=0, S=0;
  Dimension width, height;

  XtVaGetValues(WIDGET(This->Icon[0]), XmNwidth, &iW,
		XmNheight,  &iH, NULL);
  XtVaGetValues(WIDGET(This->rcMain), 
		XmNmarginWidth, &mW,
		XmNmarginHeight,  &mH,
		XmNspacing, &S, NULL);
 
  height = iH + 2*mH;
  width = (This->nbIcon * iW) + ((This->nbIcon -1) * S) + 2*mW;
  
  XtVaSetValues(This->MainInterface->w_Main, 
		XmNwidth, width,
		XmNminWidth, width,
		XmNmaxWidth, width,
		XmNheight, height,
		XmNminHeight, height,
		XmNmaxHeight, height,
		NULL);

 }

 F(This->popup).ToggleButton(This->popup, POPUP_OVERRIDE, True);

 F(This->popup).AddCallback2(This->popup, This, POPUP_OVERRIDE,
			     cb_override, 
			     cb_transient, 
			     cb_flip, cb_flip, NULL);

 for(i = 0; i < This->nbIcon; i++)
 {
  F(This->popup).AddCallback(This->popup, POPUP_XCLAMATION + i,
			     cb_launch, i, NULL); 
  CALLBACK(WIDGET(This->Icon[i]), FUNC_ACTIVATE, cb_launch, (void *)i);
 }

 F(This->popup).AddCallback2(This->popup, This, POPUP_QUIT, cb_quit, NULL);
 
}


static void destroy_interface(This)
c_Interface *This;
{
 int i;

 DELETE(c_PopupMenu)(This->popup);
 for (i = 0; i < This->nbIcon; i++)
  DELETE(c_Icon)(This->Icon[i]);
 DELETE(c_RowColumn)(This->rcMain);
}


