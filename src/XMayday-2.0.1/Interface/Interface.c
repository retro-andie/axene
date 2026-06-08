/*
** Interface.c for XMayday in Interface/
** Interface Part of XMayday
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
** Started on  Mon Jul 17 15:05:00 1995 Emmanuel Paris
** Last update Fri Feb  4 11:01:29 2000 Emmanuel Paris
*/

#include "Interface.h"
#include "InterfaceP.h"
#include "DocIcon.h"
#include "../Pixmaps/name_emboss.zpm"

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
static void create_bottom_bar ___PROTO((c_Interface *This));
static void destroy_bottom_bar ___PROTO((c_Interface *This));
extern void create_menu_bar ___PROTO((c_Interface *This));
extern void destroy_menu_bar ___PROTO((c_Interface *This));
extern void create_icon_bar ___PROTO((c_Interface *This));
extern void destroy_icon_bar ___PROTO((c_Interface *This));

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
 F(GlobDocIcon).registerPixmapConverters(GlobDocIcon);
 create_interface(This);
  
 F(This->Manager).Manage_all(This->Manager, NULL);
 F(This->MainInterface).map_TopLevel(This->MainInterface);
  
 F(This->MainInterface).set_main_icon(This->MainInterface);
 This->AppTitle[0] = '\0';
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

static void create_interface(This)
c_Interface *This;
{
 Arg		args[10];
  
 create_menu_bar(This);
  
 XtSetArg(args[0], XmNshadowType, XmSHADOW_OUT );
 This->fPaned = XmCreateFrame(WIDGET(This->MainInterface), "fPaned", args, 1);
  
 F(This->Manager).Add_child(This->Manager, This->fPaned,
			    WIDGET(This->MainInterface));
  
 This->Paned = (c_PanedWindow *)
  NEW(c_PanedWindow)(This->fPaned, This->Manager, "Paned");
  
 This->HelpWIndex = (c_HelpWidget *)
  NEW(c_HelpWidget)(WIDGET(This->Paned), NULL, "hwTOP");
  
 This->foBottom = (c_Form *)
  NEW(c_Form)(WIDGET(This->Paned), This->Manager, "foBottom", 32, 32);
  
 This->HelpWHelp = (c_HelpWidget *)
  NEW(c_HelpWidget)(WIDGET(This->foBottom), This->Manager, "hwBOTTOM"); 

 XtSetArg(args[0], XmNtopOffset, 3);
 XtSetValues(WIDGET(This->HelpWHelp), args, 1);
  
 This->PreviousAlreadyGrised = This->NextAlreadyGrised = 
  This->RefreshAlreadyGrised = This->IndexAlreadyGrised =
   This->ICreationAlreadyGrised = This->IndexExist = 
    This->CloseAlreadyGrised = FALSE;
  
 This->ButtonMaskIndex = FALSE;
 This->ButtonMaskBottomBar = TRUE;
  
 create_bottom_bar(This);
 create_icon_bar(This);
  
 This->HelpManager = (c_HelpManager *)
  NEW(c_HelpManager)(This->HelpWIndex, This->HelpWHelp,
		     This->Menu[MENU_NAVIGATE], MN_URL_LIST,
		     WIDGET(This->lHelpLine));
  
 This->OpenFileSelector = This->OpenIndexSelector = 
  This->PrintDocumentSelector = NULL;
  
 F(This->foBottom).attach_left(This->foBottom, This->fIconBar, NULL);
 F(This->foBottom).attach_right(This->foBottom, This->fIconBar, NULL);
 F(This->foBottom).attach_top(This->foBottom, This->fIconBar, NULL);

 F(This->foBottom).attach_left
  (This->foBottom, WIDGET(This->HelpWHelp), NULL);
 F(This->foBottom).attach_right
  (This->foBottom, WIDGET(This->HelpWHelp), NULL);
 F(This->foBottom).attach_bottom
  (This->foBottom, WIDGET(This->HelpWHelp), NULL);
 F(This->foBottom).attach_top
  (This->foBottom, WIDGET(This->HelpWHelp), This->fIconBar);

 F(This->MainInterface).attach_left
  (This->MainInterface, WIDGET(This->MenuBar), NULL);
 F(This->MainInterface).attach_right
  (This->MainInterface, WIDGET(This->MenuBar), NULL);
 F(This->MainInterface).attach_top
  (This->MainInterface, WIDGET(This->MenuBar), NULL);

 F(This->MainInterface).attach_left
  (This->MainInterface, WIDGET(This->BottomBar), NULL);
 F(This->MainInterface).attach_right
  (This->MainInterface, WIDGET(This->BottomBar), NULL);
 F(This->MainInterface).attach_bottom
  (This->MainInterface, WIDGET(This->BottomBar), NULL);
  
 F(This->MainInterface).attach_top
  (This->MainInterface, This->fPaned, WIDGET(This->MenuBar));
 F(This->MainInterface).attach_left
  (This->MainInterface, This->fPaned, NULL);
 F(This->MainInterface).attach_right
  (This->MainInterface, This->fPaned, NULL);
 F(This->MainInterface).attach_bottom
  (This->MainInterface, This->fPaned, WIDGET(This->BottomBar));

}

static void destroy_interface(This)
c_Interface *This;
{
 if(This->OpenIndexSelector != NULL)
  DELETE(c_FileSelector)(This->OpenIndexSelector);
 if(This->OpenFileSelector != NULL)
  DELETE(c_FileSelector)(This->OpenFileSelector);
 if(This->PrintDocumentSelector != NULL)
  DELETE(c_FileSelector)(This->PrintDocumentSelector);
  
 DELETE(c_HelpManager)(This->HelpManager);
  
 destroy_icon_bar(This);
 destroy_bottom_bar(This);
  
 DELETE(c_HelpWidget)(This->HelpWHelp); 
 DELETE(c_Form)(This->foBottom);
 DELETE(c_HelpWidget)(This->HelpWIndex);
  
 DELETE(c_PanedWindow)(This->Paned);
 XtDestroyWidget(This->fPaned);
    
 destroy_menu_bar(This);
}

/* ------------------------------------------------------------------------ **
** Bottom Bar --                                                            ** 
** ------------------------------------------------------------------------ */
static void create_bottom_bar(This)
c_Interface *This;
{
 This->BottomBar = (c_Form *)
  NEW(c_Form)(WIDGET(This->MainInterface), This->Manager,
	      "BottomBar", 32, 32);
  
 This->IconBottom = (c_Icon *)
  NEW(c_Icon)(WIDGET(This->BottomBar), This->Manager,
	      "IconBottom", XtDisplay(WIDGET(This->BottomBar)), 
	      name_emboss_xpm, NULL, XcLabel);
  
 This->lHelpLine = (c_Label *)
  NEW(c_Label)(WIDGET(This->BottomBar), This->Manager, "lHelpLine");
  
 F(This->BottomBar).attach_top
  (WIDGET(This->BottomBar), WIDGET(This->IconBottom), NULL);
 F(This->BottomBar).attach_bottom
  (WIDGET(This->BottomBar), WIDGET(This->IconBottom), NULL);
 F(This->BottomBar).attach_left
  (WIDGET(This->BottomBar), WIDGET(This->IconBottom), NULL);
  
 F(This->BottomBar).attach_top
  (WIDGET(This->BottomBar), WIDGET(This->lHelpLine), NULL);
 F(This->BottomBar).attach_bottom
  (WIDGET(This->BottomBar), WIDGET(This->lHelpLine), NULL);
 F(This->BottomBar).attach_left
  (WIDGET(This->BottomBar), WIDGET(This->lHelpLine), 
   WIDGET(This->IconBottom));
 F(This->BottomBar).attach_right
  (WIDGET(This->BottomBar), WIDGET(This->lHelpLine), NULL);
  
}

static void destroy_bottom_bar(This)
c_Interface *This;
{
 DELETE(c_Label)(This->lHelpLine);
 DELETE(c_Icon)(This->IconBottom);
 DELETE(c_Form)(This->BottomBar);
}







