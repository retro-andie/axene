/*
** OptionMenu.c for Xclamation, XQuad and XAllWrite in OptionMenu/
**
** Copyright (C) 1994-2000 Axene.
** Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
** Started on  Mon Jul 17 19:45:58 1995 Robin Castanier
** Last update Sun Mar 15 17:36:53 1998 Emmanuel Paris
*/

#include "OptionMenu.h"
#include "HelpLine.h"

extern c_HelpLine *GlobHelpLine;

static void *cons_OptionMenu(Widget w_Parent, c_ManageWidget *ManageW, char *Title, ...);
static void dest_OptionMenu();
static void *copy_OptionMenu();
static void OptionMenu_ButtonFlag();
static void OptionMenu_setSelection();
static void OptionMenu_AddCalback();
static void OptionMenu_AddCalback2(c_OptionMenu *This, int buttonnum, ...);
static void OptionMenu_SetGlobalCallback();
static void waitForUserAction();

extern void XcHandleEvent ___PROTO((XEvent *event));
extern void XcFlushEvents ___PROTO((Widget w,
				    void (*func) ___PROTO((XEvent *))));

sf_OptionMenu fc_OptionMenu =
{
 cons_OptionMenu,
 dest_OptionMenu,
 copy_OptionMenu,
 OptionMenu_ButtonFlag,
 OptionMenu_AddCalback,
 OptionMenu_AddCalback2,
 waitForUserAction,
 OptionMenu_setSelection,
 OptionMenu_SetGlobalCallback
};

static void *cons_OptionMenu(Widget w_Parent, c_ManageWidget *ManageW, char *Title, ...)
{
 va_list 		other_arg;
 Display               *display;
 c_OptionMenu		*ObjTmp;
 Arg			argsw[4];
 int			nargs;
 int			names_number;
 int                   num = 0;
 int			helpline;
 char			*temp;
 int                   iSubMenu;
 int                   iIconmode;
 XmString		xstr; 
 char			icon_title[30];

 Xc_HISTORY(("contructor(`%s')", Title));
  
 if ((ObjTmp = (c_OptionMenu *)Xc_malloc("OptionMenu",
					 sizeof(c_OptionMenu))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_OptionMenu;
 ObjTmp->Title = Title;

 display = XtDisplay(w_Parent);

 nargs = 0;
 ObjTmp->w_Pulldown = XmCreatePulldownMenu(w_Parent, Title, NULL, 0);

 va_start(other_arg, Title);
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
  va_start(other_arg, Title);

  if ((ObjTmp->Buttons = 
       (strPopupContent *)Xc_malloc("WidgetId",
				    ObjTmp->numberw /2 
				    * sizeof(strPopupContent))) == NULL)
  {
   Xc_FATAL(("Can't create WidgetId table: memory error"));
  }
      
  memset(ObjTmp->Buttons, 0, ObjTmp->numberw /2 * sizeof(strPopupContent));
  names_number = 0;
  iSubMenu = 0;
  iIconmode = 0;
      
  while (names_number < ObjTmp->numberw)
  {
   temp = va_arg(other_arg, char *);
   switch((long)temp)
   {
   case  LABELTYPE:
    names_number += 3;
    num = va_arg(other_arg, int);
    temp = va_arg(other_arg, char *);
    num = va_arg(other_arg, int) -1;
    ObjTmp->Buttons[num].w_content = 
     XmCreateLabelGadget(ObjTmp->w_Pulldown, temp, NULL, 0);
    XtManageChild(ObjTmp->Buttons[num].w_content);
    break;
   case MENUSEPARATOR:
    names_number++;
    num = va_arg(other_arg, int) -1;
    ObjTmp->Buttons[num].w_content =
     XmCreateSeparatorGadget(ObjTmp->w_Pulldown, 
			     "__sep_me__", NULL, 0);
    XtManageChild(ObjTmp->Buttons[num].w_content);
    break;
   case SUBMENUON:
    names_number += 3 ;
    iSubMenu = va_arg(other_arg, int) -1;
    ObjTmp->Buttons[iSubMenu].w_content =
     XmCreatePulldownMenu(ObjTmp->w_Pulldown, 
			  "__sub_menu__", NULL, 0);
	      
    temp = va_arg(other_arg, char *);
    num = va_arg(other_arg, int) -1;
	      
    nargs = 0;
    XtSetArg(argsw[nargs], XmNsubMenuId,
	     ObjTmp->Buttons[iSubMenu].w_content);
    nargs++;
    ObjTmp->Buttons[num].w_content = 
     XmCreateCascadeButtonGadget(ObjTmp->w_Pulldown, 
				 temp, argsw, nargs);
    XtManageChild(ObjTmp->Buttons[num].w_content);
    break;
   case SUBMENUOFF:
    names_number++;
    iSubMenu = 0;
    num = va_arg(other_arg, int);
    break;
   case NOMANAGE:
    names_number += 3;
    num = va_arg(other_arg, int);
    temp = va_arg(other_arg, char *);
    num = va_arg(other_arg, int) -1;
    ObjTmp->Buttons[num].w_content = 
     XmCreatePushButtonGadget(ObjTmp->w_Pulldown, temp, NULL, 0);
    break;
   case OICONMODEON:
    iIconmode = ICONMODE_EXTERNE;
    num = va_arg(other_arg, int);
    names_number++;
    break;
   case ICONMODEON:
    iIconmode = ICONMODE_INTERNE;
    num = va_arg(other_arg, int);
    names_number++;
    break;
   case ICONMODEOFF:
    iIconmode = ICONMODE_INACTIF;
    num = va_arg(other_arg, int);
    names_number++;
    break;		    
   case HELPLINE:
    helpline = va_arg(other_arg, int);
    names_number++;
    if (iIconmode != ICONMODE_INACTIF)
    {
     F(GlobHelpLine).enable_helpline
      (GlobHelpLine, WIDGET(((c_Icon *)ObjTmp->Buttons[num].objet)), 
       helpline, XcR_NONE);
    }
    break;
   default:
    names_number++;
    num = va_arg(other_arg, int) -1;
    if (iSubMenu == 0)
    {
     switch (iIconmode)
     {
     case ICONMODE_INACTIF:
		      
      ObjTmp->Buttons[num].w_content = 
       XmCreatePushButtonGadget(ObjTmp->w_Pulldown, 
				temp, NULL, 0);
      break;
     case ICONMODE_INTERNE:
      sprintf(icon_title, "Icon %d", num);
      ObjTmp->Buttons[num].objet =
       (c_Icon *)NEW(c_Icon)(ObjTmp->w_Pulldown, NULL,
			     icon_title, display,
			     (char **)temp, NULL, XcPush);
      ObjTmp->Buttons[num].destructor = DELETE(c_Icon);
      ObjTmp->Buttons[num].w_content = 
       WIDGET(((c_Icon *)(ObjTmp->Buttons[num].objet)));
      break;
     case ICONMODE_EXTERNE:
      break;
     }
    }
    else
    {
     ObjTmp->Buttons[num].w_content = 
      XmCreatePushButtonGadget(ObjTmp->
			       Buttons[iSubMenu].w_content,
			       temp, NULL, 0);
    }
    XtManageChild(ObjTmp->Buttons[num].w_content);
    break;
   }
   names_number++;
  } 
  va_end(other_arg);
 } 
  
 nargs = 0;
 XtSetArg(argsw[nargs], XmNsubMenuId, ObjTmp->w_Pulldown);
 nargs++;

 ObjTmp->w_This = XmCreateOptionMenu(w_Parent, Title,
				     argsw, nargs);
 xstr = XmStringCreateSimple("");
 XtVaSetValues(XmOptionLabelGadget(ObjTmp->w_This),
	       XmNlabelString, xstr,
	       XmNwidth, 0,
	       XmNmarginHeight, 0,
	       XmNmarginWidth, 0,
	       XmNhighlightThickness, 0,
	       NULL);
 XmStringFree(xstr);

 XtVaSetValues(ObjTmp->w_This,
	       XmNmarginHeight, 0,
	       XmNmarginWidth, 0,
	       XmNspacing, 0,
	       NULL);

 F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);

 /*XmMenuPosition(WIDGET(ObjTmp), (XButtonPressedEvent *)event);*/
  
 XtManageChild(WIDGET(ObjTmp));
  
 ObjTmp->mapped_flag = TRUE;
  
 Xc_TRACE(("constructor end"));
 return ObjTmp;
}

static void dest_OptionMenu(This)
c_OptionMenu *This;
{
 int	i;
 int	num;
 Display *display;
  
 Xc_HISTORY(("Object OptionMenu destructor"));
 num = This->numberw / 2;
 display = XtDisplay(WIDGET(This));
 XtUnmanageChild(This->w_This);
 for(i=0; i<num; i++)
 {
  if (This->Buttons[i].objet)
  {
   This->Buttons[i].destructor(This->Buttons[i].objet);
   XFlush(display);
  }
  else
  {
   if (This->Buttons[i].w_content)
    XtDestroyWidget(This->Buttons[i].w_content);
  }
 }
#if defined(OLD_MOTIF)
 XtDestroyWidget(This->w_Pulldown);
#endif
 XtDestroyWidget(This->w_This);
 XFlush(display);

 Xc_free(This->Buttons);
 Xc_free(This);
 Xc_TRACE(("Object OptionMenu destroyed"));
}

static void *copy_OptionMenu(This)
c_OptionMenu *This;
{
 c_OptionMenu *ObjTmp;
  
 if ((ObjTmp = (c_OptionMenu *)Xc_malloc("COptionMenu",
					 sizeof(c_OptionMenu))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_OptionMenu));
 Xc_TRACE(("Object OptionMenu copied"));
 return ObjTmp;
}

static void OptionMenu_ButtonFlag(This, buttonnum, flag)
c_OptionMenu	*This;
int		buttonnum;
int 		flag;
{
 XtSetSensitive(This->Buttons[buttonnum-1].w_content, flag);
}

static void OptionMenu_setSelection(This, buttonnum)
c_OptionMenu	*This;
int		buttonnum;
{
 XtVaSetValues(WIDGET(This), XmNmenuHistory, 
	       This->Buttons[buttonnum-1].w_content, NULL);
}

static void OptionMenu_AddCalback(This, buttonnum, function, argument,argument2)
c_OptionMenu	*This;
int		buttonnum;
void		(*function)();
void 		*argument;
void 		*argument2;
{
 XtAddCallback(This->Buttons[buttonnum-1].w_content, XmNarmCallback,
	       function, argument);
 XtAddCallback(This->Buttons[buttonnum-1].w_content, XmNdisarmCallback,
	       function, argument2);
}

static void OptionMenu_AddCalback2(c_OptionMenu *This, int buttonnum, ...)
{
 va_list	other_arg;
 void		(*temp)();

 va_start(other_arg, buttonnum);
 while ((temp = (void (*)())va_arg(other_arg, void *)))
 {
  XtAddCallback(This->Buttons[buttonnum-1].w_content,
		XmNactivateCallback, temp, This); 
  buttonnum++;
 }
 va_end(other_arg);
}

static void menu_global_callback(w, This)
Widget		w;
c_OptionMenu	*This;
{
 int	i;

 i=0;
 while (i < This->numberw)
 {
  if (w == This->Buttons[i].w_content)
   break;
  i++;
 }
 if (i < This->numberw)
  This->glob_func(This->glob_arg, i);
}

static void OptionMenu_SetGlobalCallback(This, function, argument)
c_OptionMenu	*This;
void		(*function)();
void 		*argument;
{
 int	i;

 This->glob_func = function;
 This->glob_arg = argument;
 for (i=0; i<This->numberw/2; i++)
 {
  if (This->Buttons[i].w_content)
   XtAddCallback(This->Buttons[i].w_content, XmNarmCallback,
		 menu_global_callback, This);
 }
}

static void waitForUserAction(This)
c_OptionMenu *This;
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
   if (event.type == ButtonRelease)
   {
    This->mapped_flag = FALSE;
    break;
   }
   XcHandleEvent(&event);
  }
 }
 Xc_TRACE(("done"));
}
