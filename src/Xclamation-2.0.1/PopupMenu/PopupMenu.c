/*
** PopupMenu.c for Xclamation, XAllWrite and AxeneOffice in PopupMenu/
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
** Started on  Tue Jul 18 11:46:39 1995 Robin Castanier
** Last update Sat Aug 14 18:51:40 1999 Emmanuel Paris
*/

#include "PopupMenu.h"

#ifndef NO_HELPLINE
#include "HelpLine.h"
extern c_HelpLine *GlobHelpLine;
#endif

static void *cons_PopupMenu();
static void dest_PopupMenu();
static void *copy_PopupMenu();
static void PopupMenu_ButtonFlag();
static void PopupMenu_ToggleManage();
static void PopupMenu_ToggleButton();
static void PopupMenu_AddCallback();
static void PopupMenu_AddCallback2();
static void mapAndWaitForMap();
static void waitForUserAction();

extern void XcHandleEvent ___PROTO((XEvent *event));
extern void XcFlushEvents ___PROTO((Widget w,
				    void (*func) ___PROTO((XEvent *))));

sf_PopupMenu fc_PopupMenu =
{
 cons_PopupMenu,
 dest_PopupMenu,
 copy_PopupMenu,
 PopupMenu_ButtonFlag,
 PopupMenu_ToggleManage,
 PopupMenu_ToggleButton,
 PopupMenu_AddCallback,
 PopupMenu_AddCallback2,
 mapAndWaitForMap,
 waitForUserAction
};

static void *cons_PopupMenu(w_Parent, Title, va_alist)
Widget		w_Parent; 
char 		*Title;
va_dcl
{
 va_list 		other_arg;
 Display		*display;
 c_PopupMenu		*ObjTmp;
 Arg			argsw[4];
 int			nargs;
 int			names_number;
 int			num = 0;
 char			*temp;
 int			iSubMenu;
 int			iIconmode;
 int			iTogglemode;
 char			icon_title[30];
#ifndef NO_HELPLINE
 int			helpline;
#endif
  
 Xc_HISTORY(("contructor(`%s')", Title));
  
 if ((ObjTmp = (c_PopupMenu *)Xc_malloc("PopupMenu",
					sizeof(c_PopupMenu))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_PopupMenu;
 ObjTmp->Title = Title;
 ObjTmp->mapped_flag = FALSE;

 display = XtDisplay(w_Parent);
 nargs = 0;
 ObjTmp->w_This = XmCreatePopupMenu(w_Parent, Title, NULL, 0);
  
 va_start(other_arg);
 names_number = 0;
 while ((temp = va_arg(other_arg, char *)))
 {
  num = va_arg(other_arg, int);
  names_number += 2;
 }
 va_end(other_arg);

 ObjTmp->numberw = names_number;
 if (names_number)
 {
  va_start(other_arg);

  if ((ObjTmp->Buttons = 
       (strPopupContent *)Xc_malloc("WidgetId",
				    ObjTmp->numberw /2 
				    * sizeof(strPopupContent))) == NULL)
  {
   Xc_FATAL(("Can't create WidgetId table: memory error"));
  }
      
  memset(ObjTmp->Buttons, 0, ObjTmp->numberw /2 * sizeof(strPopupContent));
  names_number = 0;
  iSubMenu = -1;
  iIconmode = 0;
  iTogglemode = 0;
  ObjTmp->first_button = (Widget)0;
      
  while (names_number < ObjTmp->numberw)
  {
   temp = va_arg(other_arg, char *);
   switch((long)temp)
   {
   case LABELTYPE:
    names_number += 3;
    num = va_arg(other_arg, int);
    temp = va_arg(other_arg, char *);
    num = va_arg(other_arg, int) -1;
    ObjTmp->Buttons[num].w_content = 
     (Widget)XmCreateLabelGadget(ObjTmp->w_This, temp, NULL, 0);
    XtManageChild(ObjTmp->Buttons[num].w_content);
    break;
   case MENUSEPARATOR:
    names_number++;
    num = va_arg(other_arg, int) -1;
    if ( iSubMenu == -1 )
     ObjTmp->Buttons[num].w_content =
      XmCreateSeparatorGadget(ObjTmp->w_This, "__sep_me__", NULL, 0);
    else
     ObjTmp->Buttons[num].w_content =
      XmCreateSeparatorGadget(ObjTmp->Buttons[iSubMenu].w_content,
			      "__sep_me__", NULL, 0);
    XtManageChild(ObjTmp->Buttons[num].w_content);
    break;
   case SUBMENUON:
    names_number += 3 ;
    iSubMenu = va_arg(other_arg, int) -1;
    ObjTmp->Buttons[iSubMenu].w_content =
     XmCreatePulldownMenu(ObjTmp->w_This, "__sub_menu__", NULL, 0);
	    
    temp = va_arg(other_arg, char *);
    num = va_arg(other_arg, int) -1;
	    
    nargs = 0;
    XtSetArg(argsw[nargs], XmNsubMenuId,
	     ObjTmp->Buttons[iSubMenu].w_content);
    nargs++;
    ObjTmp->Buttons[num].w_content = 
     XmCreateCascadeButtonGadget(ObjTmp->w_This, 
				 temp, argsw, nargs);
    if (ObjTmp->first_button == (Widget)0)
     ObjTmp->first_button = ObjTmp->Buttons[num].w_content;
    XtManageChild(ObjTmp->Buttons[num].w_content);
    break;
   case SUBMENUOFF:
    names_number++;
    iSubMenu = -1;
    num = va_arg(other_arg, int);
    break;
   case NOMANAGE:
    names_number += 3;
    num = va_arg(other_arg, int);
    temp = va_arg(other_arg, char *);
    num = va_arg(other_arg, int) -1;
    ObjTmp->Buttons[num].w_content = 
     XmCreatePushButtonGadget(ObjTmp->w_This, temp, NULL, 0);
    break;
   case ICONMODEON:
    iIconmode = 1;
    num = va_arg(other_arg, int);
    names_number++;
    break;
   case ICONMODEOFF:
    iIconmode = 0;
    num = va_arg(other_arg, int);
    names_number++;
    break;		    
   case TOGGLEMODEON:
    iTogglemode = 1;
    num = va_arg(other_arg, int);
    names_number++;
    break;
   case TOGGLEMODEOFF:
    iTogglemode = 0;
    num = va_arg(other_arg, int);
    names_number++;
    break;
   case HELPLINE:
#ifndef NO_HELPLINE
    helpline = va_arg(other_arg, int);
    names_number++;
    Xc_TRACE(("enable helpline"));
    if (iIconmode)
    {
     F(GlobHelpLine).enable_helpline
      (GlobHelpLine, WIDGET(((c_Icon *)ObjTmp->Buttons[num].objet)), 
       helpline, XcR_NONE);
    }
#endif
    break;
   default:
    names_number++;
    num = va_arg(other_arg, int) -1;
    if ( iSubMenu == -1 )
    {
     if (iIconmode == 0)
     {
      if (iTogglemode == 0)
      {
       ObjTmp->Buttons[num].w_content = 
	XmCreatePushButtonGadget(ObjTmp->w_This, temp, NULL, 0);
       if (ObjTmp->first_button == (Widget)0)
	ObjTmp->first_button = ObjTmp->Buttons[num].w_content;
      }
      else
      {
       ObjTmp->Buttons[num].w_content = 
	XmCreateToggleButtonGadget(ObjTmp->w_This, temp, NULL, 0);
       if (ObjTmp->first_button == (Widget)0)
	ObjTmp->first_button = ObjTmp->Buttons[num].w_content;
      }
     }
     else
     {
      sprintf(icon_title, "Icon %d", num);
      ObjTmp->Buttons[num].objet =
       (c_Icon *)NEW(c_Icon)(ObjTmp->w_This, NULL,
			     icon_title, display,
			     (char **)temp, NULL, XcPush);
      ObjTmp->Buttons[num].destructor = DELETE(c_Icon);
      ObjTmp->Buttons[num].w_content = 
       WIDGET(((c_Icon *)(ObjTmp->Buttons[num].objet)));
      if (ObjTmp->first_button == (Widget)0)
       ObjTmp->first_button = ObjTmp->Buttons[num].w_content;
     }
    }	
    else
    {
     ObjTmp->Buttons[num].w_content = 
      XmCreatePushButtonGadget(ObjTmp->Buttons[iSubMenu].w_content,
			       temp, NULL, 0);
    }
    XtManageChild(ObjTmp->Buttons[num].w_content);
    break;
   }
   names_number++;
  } 
  va_end(other_arg);
 } 
  
 return ObjTmp;
}

static void dest_PopupMenu(This)
c_PopupMenu *This;
{
 int	i;
 int	num;
 Display *display;
  
 Xc_HISTORY(("Object PopupMenu destructor"));
 num = This->numberw / 2;
 display = XtDisplay(WIDGET(This));
 XtUnmanageChild(This->w_This);
 for(i=0; i<num; i++)
 {
  if (This->Buttons[i].objet)
   This->Buttons[i].destructor(This->Buttons[i].objet);
  else
   if (This->Buttons[i].w_content)
    XtDestroyWidget(This->Buttons[i].w_content);
 }
#ifdef LESSTIF_VERSION
  XtDestroyWidget(XtParent(This->w_This));
#else
 XtDestroyWidget(This->w_This);
#endif

 XUngrabPointer(display, CurrentTime);
 XUngrabServer(display);
  
 Xc_free(This->Buttons);
 Xc_free(This);
 Xc_TRACE(("Object PopupMenu destroyed"));
}

static void *copy_PopupMenu(This)
c_PopupMenu *This;
{
 c_PopupMenu *ObjTmp;
  
 if ((ObjTmp = (c_PopupMenu *)Xc_malloc("CPopupMenu",
					sizeof(c_PopupMenu))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_PopupMenu));
 Xc_TRACE(("Object PopupMenu copied"));
 return ObjTmp;
}

static void PopupMenu_ButtonFlag(This, buttonnum, flag)
c_PopupMenu	*This;
int		buttonnum;
int 		flag;
{
 XtSetSensitive(This->Buttons[buttonnum-1].w_content, flag);
}

static void PopupMenu_ToggleManage(This, button1, button2)
c_PopupMenu	*This;
int		button1;
int 		button2;
{
 XtUnmanageChild(This->Buttons[button1-1].w_content);
 XtManageChild(This->Buttons[button2-1].w_content);
}

static void PopupMenu_ToggleButton(This, button, state)
c_PopupMenu *This;
int	button;
Boolean	state;
{
 XmToggleButtonSetState(This->Buttons[button-1].w_content, state, False);
}

static void PopupMenu_AddCallback(This, buttonnum, function, 
				  argument, argument2)
c_PopupMenu	*This;
int		buttonnum;
void		(*function)();
void 		*argument;
void 		*argument2;
{
 if (argument2 != NULL)
 {
  XtAddCallback(This->Buttons[buttonnum-1].w_content, XmNarmCallback,
		function, argument);
  XtAddCallback(This->Buttons[buttonnum-1].w_content, XmNdisarmCallback,
		function, argument2);
 }
 else
 {
  WidgetClass wclass;

  wclass = XtClass(This->Buttons[buttonnum-1].w_content);
  if (wclass == xmPushButtonGadgetClass || 
      wclass == xmPushButtonWidgetClass)
   XtAddCallback(This->Buttons[buttonnum-1].w_content,
		 XmNactivateCallback, function, argument); 
  else if (wclass == xmToggleButtonGadgetClass || 
	   wclass == xmToggleButtonWidgetClass)
  {
   XtAddCallback(This->Buttons[buttonnum-1].w_content,
		 XmNvalueChangedCallback, function, argument); 
  }
 }
}

static void PopupMenu_AddCallback2(This, arg, buttonnum, va_alist)
c_PopupMenu	*This;
void		*arg;
int		buttonnum;
va_dcl
{
 va_list	other_arg;
 void		(*temp)();
 WidgetClass	wclass;

 va_start(other_arg);
 while ((temp = va_arg(other_arg, void *)))
 {
  wclass = XtClass(This->Buttons[buttonnum-1].w_content);
  if (wclass == xmPushButtonGadgetClass || 
      wclass == xmPushButtonWidgetClass)
   XtAddCallback(This->Buttons[buttonnum-1].w_content,
		 XmNactivateCallback, temp, arg); 
  else if (wclass == xmToggleButtonGadgetClass || 
	   wclass == xmToggleButtonWidgetClass)
  {
   XtAddCallback(This->Buttons[buttonnum-1].w_content,
		 XmNvalueChangedCallback, temp, arg); 
  }
  buttonnum++;
 }
 va_end(other_arg);
}

static void mapAndWaitForMap(This, Oevent)
c_PopupMenu *This;
XEvent *Oevent;
{
 XtAppContext	app;
 XEvent		event;
 boolean	release, cont;
 Window		wnone;
 int		cnone, x, y;
 XmPushButtonCallbackStruct cbs;

 Xc_HISTORY(("map popup menu"));

 XmMenuPosition(WIDGET(This), (XButtonPressedEvent *)Oevent);

 /*#if (XmVERSION == 1)*/		/* MOTIF 1.X */
 XUngrabPointer(XtDisplay(WIDGET(This)), CurrentTime);
 /*#endif*/

 XtManageChild(WIDGET(This));

 app = XtWidgetToApplicationContext(WIDGET(This));
 release = FALSE; cont = TRUE;
  
 while(cont)
 {
  XtAppNextEvent(app, &event);
  if(event.xany.type == ButtonPress || event.xany.type == KeyPress)
  {
   release = TRUE;
   break;
  }
  if (event.type == ButtonRelease)
   release = TRUE;
  if((event.xany.type == MapNotify) &&
     (event.xany.window == XtWindow(XtParent(WIDGET(This)))))
  {      
   XQueryPointer(XtDisplay(WIDGET(This)), XtWindow(WIDGET(This)),
		 &wnone, &wnone, &cnone, &cnone, &x, &y, &cnone);
   Xc_TRACE(("coord_pos= %d %d", x, y));
   if (x>0 && y>0 && This->first_button != (Widget)0)
    if ( XtHasCallbacks(This->first_button, 
			XmNarmCallback) == XtCallbackHasSome)
    {
     cbs.reason = XmCR_ARM;
     XtCallCallbacks(This->first_button, XmNarmCallback, &cbs);
    }
   cont = FALSE;
  }
  XcHandleEvent(&event);
 }
 if (!release)
  This->mapped_flag = TRUE;

 Xc_TRACE(("popup mapped"));
}

static void waitForUserAction(This)
c_PopupMenu *This;
{
 XtAppContext app;
 XEvent event;

 Xc_HISTORY(("waitForUserAction"));
  
 XFlush(XtDisplay(WIDGET(This)));
 if(This->mapped_flag == TRUE)
 {
  app = XtWidgetToApplicationContext(WIDGET(This));
  XSync(XtDisplay(WIDGET(This)), False);
      
  while(This->mapped_flag)
  {
   XtAppNextEvent(app, &event);
   /*XtDispatchEvent(&event);*/
   XcHandleEvent(&event);
   if (event.type == ButtonRelease)
   {
    This->mapped_flag = FALSE;
    break;
   }
  }
 }
 Xc_TRACE(("done"));
}





