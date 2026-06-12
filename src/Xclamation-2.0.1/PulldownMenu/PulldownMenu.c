/*
** PulldownMenu.c for Xclamation, XQuad, XAllWrite and XMayday in PulldownMenu/
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
** Started on  Tue Jul 18 11:51:56 1995 Robin Castanier
** Last update Sun Aug  8 17:45:11 1999 Emmanuel Paris
*/

#include "PulldownMenu.h"
#include "RegisterHelp.h"

static void *cons_PulldownMenu(Widget w_Parent, c_ManageWidget *ManageW, char *Title, ...);
static void dest_PulldownMenu();
static void *copy_PulldownMenu();
static void PulldownMenu_ButtonFlag();
static void PulldownMenu_ToggleManage();
static void PulldownMenu_ToggleButton();
static void PulldownMenu_AddCallback();
static void PulldownMenu_AddCallback2(c_PulldownMenu *This, int buttonnum, ...);
static void PulldownMenu_SetDynamicFlag();
static void PulldownMenu_SetDynamicList();
static void PulldownMenu_ToggleDynamicButton();
static void PulldownMenu_AddDynamicCallback();

static void PulldownMenu_CreateDynamic();
static void PulldownMenu_DynamicCallback();
static void Pulldown_CallDynamic();

sf_PulldownMenu fc_PulldownMenu =
{
 cons_PulldownMenu,
 dest_PulldownMenu,
 copy_PulldownMenu,
 PulldownMenu_ButtonFlag,
 PulldownMenu_ToggleManage,
 PulldownMenu_ToggleButton,
 PulldownMenu_AddCallback,
 PulldownMenu_AddCallback2,
 PulldownMenu_SetDynamicFlag,
 PulldownMenu_SetDynamicList,
 PulldownMenu_ToggleDynamicButton,
 PulldownMenu_AddDynamicCallback
};


static void *cons_PulldownMenu(Widget w_Parent, c_ManageWidget *ManageW, char *Title, ...)
{
 va_list 		other_arg;
 c_PulldownMenu	*This;
 Arg			argsw[4];
 Widget			widget;
 int			nargs, minw, maxw;
 int			nb_names, names_number;
 int                    num, nb_pmd, num_wid, num_pmd;
 char			*temp;
 int                   iSubMenu;
 int		       iTogglemode;
 boolean               bRadioBox, bManage;
 
 Xc_HISTORY(("contructor(`%s')", Title));

 if ((This = (c_PulldownMenu *)Xc_malloc("PulldownMenu",
					 sizeof(c_PulldownMenu))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_PulldownMenu;
 This->Title = Title;
 This->w_This = XmCreatePulldownMenu(w_Parent, Title, NULL, 0);
 Xc_REGISTER_HELP(This->w_This);

 va_start(other_arg, Title);
 names_number = nb_pmd = num_wid = 0;
 while ((temp = va_arg(other_arg, char *)))
 {
  switch((long)temp)
  {
  case DYNAMIC:
   nb_pmd ++;
   num =  va_arg(other_arg, int);
   minw = va_arg(other_arg, int);
   maxw = va_arg(other_arg, int);
   if (num_wid < num + maxw)
    num_wid = num + maxw;
   break;
  case SUBMENUON:
   num =  va_arg(other_arg, int);
   if (num_wid < num)
    num_wid = num;
   num = va_arg(other_arg, int);
   num = va_arg(other_arg, int);
   if (num_wid < num)
    num_wid = num;
   break;
  case NOMANAGE: 
  case SUBMENUOFF:
  case RADIOBOXON:
  case RADIOBOXOFF:
  case TOGGLEMODEON:
  case TOGGLEMODEOFF:
   num = va_arg(other_arg, int);
   break;
  case MENUSEPARATOR:
  default:
   num =  va_arg(other_arg, int);
   if (num_wid < num)
    num_wid = num;
   break;
  }
  names_number++;
 }
 va_end(other_arg);
  
 This->numberw = ++num_wid;
 This->nb_pm_dynamic = nb_pmd; 
    
 if (This->numberw)
 {
  This->w_Buttons = (Widget *)Xc_malloc
   ("WidgetId", This->numberw * sizeof(Widget));
  memset(This->w_Buttons, 0, This->numberw * sizeof(Widget));
 }
 if (This->nb_pm_dynamic)
 {
  This->pm_dynamic = (pm_dynamic_t *)Xc_malloc
   ("pmd", nb_pmd * sizeof(pm_dynamic_t));
  memset(This->pm_dynamic, 0, nb_pmd * sizeof(pm_dynamic_t));
 }
  
 nb_names = num_pmd = 0;
 iSubMenu = 0;
 iTogglemode = 0;
 bRadioBox = FALSE;
 bManage = TRUE;
  
 va_start(other_arg, Title);

 while (nb_names < names_number)
 {
  temp = va_arg(other_arg, char *); 
  switch((long)temp)
  {
  case MENUSEPARATOR:
   num = va_arg(other_arg, int);
   if (iSubMenu == 0)
   {
    This->w_Buttons[num] = XmCreateSeparatorGadget
     (This->w_This, "__sep_me__", NULL, 0);
   }
   else
   {
    This->w_Buttons[num] = XmCreateSeparatorGadget
     (This->w_Buttons[iSubMenu], "__sep_me__", NULL, 0);
   }
   XtManageChild(This->w_Buttons[num]);
   break;
  case SUBMENUON:
   iSubMenu = va_arg(other_arg, int);
   This->w_Buttons[iSubMenu] =
    XmCreatePulldownMenu(This->w_This, "__sub_menu__", NULL, 0);
      
   temp = va_arg(other_arg, char *);
   num = va_arg(other_arg, int);
      
   nargs = 0;
   XtSetArg(argsw[nargs], XmNsubMenuId, This->w_Buttons[iSubMenu]);
   nargs++;
   This->w_Buttons[num] = 
    XmCreateCascadeButtonGadget(This->w_This, temp, argsw, nargs);
   Xc_REGISTER_HELP(This->w_Buttons[num]);
   XtManageChild(This->w_Buttons[num]);
   break;
  case SUBMENUOFF:
   iSubMenu = 0;
   num = va_arg(other_arg, int);
   break;
  case NOMANAGE:
   num = va_arg(other_arg, int);
   bManage = FALSE;
   break;
  case RADIOBOXON:
   bRadioBox = TRUE;
   num = va_arg(other_arg, int);
   break;
  case RADIOBOXOFF:
   bRadioBox = FALSE;
   num = va_arg(other_arg, int);
   break;
  case TOGGLEMODEON:
   iTogglemode = 1;
   num = va_arg(other_arg, int);
   break;
  case TOGGLEMODEOFF:
   iTogglemode = 0;
   num = va_arg(other_arg, int);
   break;		    
   
  case DYNAMIC:
   num = va_arg(other_arg, int);
   minw = va_arg(other_arg, int);
   maxw = va_arg(other_arg, int);
      
   This->pm_dynamic[num_pmd].numberw = num;
   This->pm_dynamic[num_pmd].min = minw;
   This->pm_dynamic[num_pmd].max = maxw;
   This->pm_dynamic[num_pmd].toggle = bRadioBox;
      
   PulldownMenu_CreateDynamic(This, num_pmd);
      
   num_pmd ++;
      
   break;
  default:
   num = va_arg(other_arg, int);
   if (iSubMenu == 0)
    widget = This->w_This;
   else
    widget = This->w_Buttons[iSubMenu];

   if (!bRadioBox)
   {
    if (iTogglemode == 0)
    {
     This->w_Buttons[num] = 
      XmCreatePushButtonGadget(widget, temp, NULL, 0);
     Xc_REGISTER_HELP(This->w_Buttons[num]);
    }
    else
    {
     This->w_Buttons[num] = 
      XmCreateToggleButtonGadget(widget, temp, NULL, 0);
     Xc_REGISTER_HELP(This->w_Buttons[num]);
    }
   }
   else
   {
    This->w_Buttons[num] = 
     XmCreateToggleButtonGadget(widget, temp, NULL, 0);
    Xc_REGISTER_HELP(This->w_Buttons[num]);
   }
   
   if (bManage)
    XtManageChild(This->w_Buttons[num]);
   else
    bManage = TRUE;
   break;
  }
  nb_names++;
 } 
 va_end(other_arg);
  
 nargs = 0;
 XtSetArg(argsw[nargs], XmNsubMenuId, This->w_This);
 nargs++;
 This->w_Cascade = XmCreateCascadeButtonGadget
  (w_Parent, Title, argsw, nargs);
 Xc_REGISTER_HELP(This->w_Cascade);
  
 if (ManageW)
  F(ManageW).Add_child(ManageW, This->w_Cascade, w_Parent);
  
 return This;
}

static void dest_PulldownMenu(This)
c_PulldownMenu *This;
{
 int	num, i, j;
  
 Xc_HISTORY(("Object PulldownMenu destructor"));
 num = This->numberw / 2;
 XtUnmanageChild(This->w_This);
 for(i = 0; i < num; i++)
 {
  if (This->w_Buttons[i])
   XtDestroyWidget(This->w_Buttons[i]);
 }
 XtDestroyWidget(This->w_Cascade);
 XtDestroyWidget(This->w_This);
 if (This->numberw)
  Xc_free(This->w_Buttons);
 if (This->nb_pm_dynamic)
 {
  for(i = 0; i < This->nb_pm_dynamic; i++)
  {
   for(j = 0; j < This->pm_dynamic[i].max; j++)
   {
    if (This->pm_dynamic[i].text[j])
     Xc_free(This->pm_dynamic[i].text[j]);
   }
   Xc_free(This->pm_dynamic[i].text);
   Xc_free(This->pm_dynamic[i].accelerator);
   Xc_free(This->pm_dynamic[i].acceleratorText);
  }
  Xc_free(This->pm_dynamic);
 }
 Xc_free(This);
 Xc_TRACE(("Object PulldownMenu destroyed"));
}

static void *copy_PulldownMenu(This)
c_PulldownMenu *This;
{
 c_PulldownMenu *NThis;
  
 if ((NThis = (c_PulldownMenu *)Xc_malloc("CPulldownMenu",
					  sizeof(c_PulldownMenu))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(NThis, This, sizeof(c_PulldownMenu));

 Xc_TRACE(("Object PulldownMenu copied"));
 return NThis;
}

static void PulldownMenu_ButtonFlag(This, buttonnum, flag)
c_PulldownMenu	*This;
int		buttonnum;
int 		flag;
{
 XtSetSensitive(This->w_Buttons[buttonnum], flag);
}


static void PulldownMenu_ToggleManage(This, button1, button2)
c_PulldownMenu	*This;
int		button1;
int 		button2;
{
 XtUnmanageChild(This->w_Buttons[button1]);
 XtManageChild(This->w_Buttons[button2]);
}

static void PulldownMenu_ToggleButton(This, button, state)
c_PulldownMenu *This;
int	button;
Boolean	state;
{
 XmToggleButtonSetState(This->w_Buttons[button], state, False);
}

static void PulldownMenu_AddCallback2(c_PulldownMenu *This, int buttonnum, ...)
{
 va_list	other_arg;
 void		(*temp)();

 va_start(other_arg, buttonnum);
 while ((temp = (void (*)())va_arg(other_arg, void *)))
 {
  
  XtAddCallback(This->w_Buttons[buttonnum],
		XmNactivateCallback, temp, This); 
  
  buttonnum++;
 }
 va_end(other_arg);
}


static void PulldownMenu_AddCallback(This, buttonnum, function, argument)
c_PulldownMenu	*This;
int		buttonnum;
void		(*function)();
void 		*argument;
{
 WidgetClass wclass;

 wclass = XtClass(This->w_Buttons[buttonnum]);
 if (wclass == xmPushButtonGadgetClass || 
     wclass == xmPushButtonWidgetClass)
  XtAddCallback(This->w_Buttons[buttonnum], XmNactivateCallback,
		function, argument);
 else if (wclass == xmToggleButtonGadgetClass || 
	  wclass == xmToggleButtonWidgetClass)
 {
  XtAddCallback(This->w_Buttons[buttonnum], XmNvalueChangedCallback,
		function, argument);
 }
}


static void PulldownMenu_SetDynamicFlag
(This, button, autocount, radio, max_accelerator, accelerator, acceleratorText,
 truncate_type, truncate_count)
c_PulldownMenu *This;
int button;
boolean autocount;
boolean	radio;
int max_accelerator;
char *accelerator;
char *acceleratorText;
pm_truncate_t truncate_type;
int truncate_count;
{
 int	i;
 pm_dynamic_t *pmd;
  
 pmd = This->pm_dynamic;
  
 i = 0;
 while(i < This->nb_pm_dynamic && pmd[i].numberw != button)
  i++;
 Xc_ASSERT(i != This->nb_pm_dynamic);
  
 pmd[i].autocount = autocount;
 pmd[i].radio = radio;
 pmd[i].max_accelerator = max_accelerator;
 pmd[i].accelerator = Xc_strdup("acc", accelerator);
 pmd[i].acceleratorText = Xc_strdup("accT", acceleratorText);
 pmd[i].truncate_type = truncate_type;
 pmd[i].truncate_count = truncate_count;
}

static void PulldownMenu_SetDynamicList(This, button, list)
c_PulldownMenu	*This;
int		button;
char		**list;
{
 int i, j, len, count, narg;
 Arg	argw[4];
 pm_dynamic_t *pmd;
 XmString mstring, maccT = NULL;
 char	string[85], acc[40], accT[40], mnemo;
 char	trunc[80], format[10];
  
 pmd = This->pm_dynamic;
  
 i = 0;
 while(i < This->nb_pm_dynamic && pmd[i].numberw != button)
  i++;
 Xc_ASSERT(i != This->nb_pm_dynamic);
 pmd = &(pmd[i]);
  
 if (pmd->autocount)
  sprintf(format, "%%%dd %%s", ((int)log10((double)pmd->max)) + 1);
  
  
 for(i = 0; i < pmd->max; i++)
 {
  if (!Xstrcmp(pmd->text[i], list[i]))
  {
   if (list[i])
    strncpy(trunc, list[i], 79);
   else
    strcpy(trunc, "");
   len = strlen(trunc);
   count = pmd->truncate_count;
      
   if (len > count)
   {
    switch(pmd->truncate_type)
    {
    case PMt_NONE:
     break;
    case PMt_BEGIN:
     trunc[0] = trunc[1] = trunc[2] = '.';
     memmove(trunc + 3, trunc + len - count, count + 1);
     break;
    case PMt_END:
     trunc[count] = trunc[count + 1] = trunc[count + 2] = '.';
     trunc[count + 3] = '\0';
     break;
    case PMt_CENTER:
     j = count >> 1;
     memmove(trunc + j + 3, trunc + len - j, j + 1);
     trunc[j] = trunc[j + 1] = trunc[j + 2] = '.';
     break;
    case PMt_GUESS:
     if (trunc[0] == '/')
     {				/* if filename, trunc begin */ 
      trunc[0] = trunc[1] = trunc[2] = '.';
      memmove(trunc + 3, trunc + len - count, count + 1);
     }
     else
     {				/* if not filename, trunc end */
      trunc[count] = trunc[count + 1] = trunc[count + 2] = '.';
      trunc[count + 3] = '\0';
     }
     break;
    }
   }
      
   if (pmd->autocount)
   {
    sprintf(string, format, i + 1, trunc);
    mstring = XmStringCreateLtoR(string, XmSTRING_DEFAULT_CHARSET);
   }
   else
    mstring = XmStringCreateLtoR(trunc, XmSTRING_DEFAULT_CHARSET);
      
   narg = 0;
   XtSetArg(argw[narg], XmNlabelString, mstring); narg++;
   if (i < pmd->max_accelerator)
   {
    if (pmd->autocount)
    {
     mnemo = i + '1';
     XtSetArg(argw[narg], XmNmnemonic, mnemo); narg++;
    }
	
    if (pmd->accelerator)
    {
     sprintf(acc, pmd->accelerator, i + 1);
     XtSetArg(argw[narg], XmNaccelerator, acc); narg++;
    }
	
    if (pmd->acceleratorText)
    {
     sprintf(accT, pmd->acceleratorText, i + 1);
     maccT = XmStringCreateLtoR(accT, XmSTRING_DEFAULT_CHARSET);
     XtSetArg(argw[narg], XmNacceleratorText, maccT); narg++;
    }
   }
      
   XtSetValues(This->w_Buttons[button + i], argw, narg);
      
   XmStringFree(mstring);      
   if (i < pmd->max_accelerator)
   {
    if (pmd->acceleratorText)  
     XmStringFree(maccT);
   }
  }
 }
 for(i = pmd->min; i < pmd->max; i++)
 {
  if (!Xstrcmp(pmd->text[i], list[i]))
  {
   if (!pmd->text[i])
    XtManageChild(This->w_Buttons[button + i]);
      
   if (!list[i])
    XtUnmanageChild(This->w_Buttons[button + i]);
  }
 }

 for(i = 0; i < pmd->max; i++)
 {
  if (!Xstrcmp(pmd->text[i], list[i]))
  {
   if (pmd->text[i])
    Xc_free(pmd->text[i]);
   pmd->text[i] = list[i] ? Xc_strdup("txtd", list[i]) : NULL;
  }
 }
}

static void PulldownMenu_ToggleDynamicButton(This, button, state)
c_PulldownMenu *This;
int	button;
Boolean	state;
{
 int	i;
 pm_dynamic_t *pmd;
 boolean	set_old = FALSE;
  
 pmd = This->pm_dynamic;
 if (button < 0)
 {
  button = -button;
  set_old = TRUE;
 }
  
 i = 0;
 while(i < This->nb_pm_dynamic && 
       button < pmd[i].numberw && button > pmd[i].numberw+pmd[i].max)
  i++;
  
 if (i == This->nb_pm_dynamic)
  XmToggleButtonSetState(This->w_Buttons[button], state, False);
 else
 {
  pmd = &(pmd[i]);
  if (pmd->toggle)
  {
   if (set_old && pmd->button_push != -1)
   {
    XmToggleButtonSetState(This->w_Buttons[pmd->button_push],
			   state, False);
    pmd->button_push = -1;
   }
   else
   {
    if (pmd->radio && pmd->button_push != -1 && state == True)
    {
     XmToggleButtonSetState(This->w_Buttons[pmd->button_push],
			    False, False);
    }
    XmToggleButtonSetState(This->w_Buttons[button], state, False);
    pmd->button_push = button;
   }
  }
 }
}

static void PulldownMenu_AddDynamicCallback(This, button, callback,
					    callback_data)
c_PulldownMenu *This;
int	button;
void	(*callback)();
void	*callback_data;
{
 int	i;
 pm_dynamic_t *pmd;
  
 pmd = This->pm_dynamic;
  
 i = 0;
 while(i < This->nb_pm_dynamic && pmd[i].numberw != button)
  i++;
 Xc_ASSERT(i != This->nb_pm_dynamic);
 pmd = &(pmd[i]);
  
 pmd->callback = callback;
 pmd->callback_data = callback_data;
  
 for(i = button; i < button + pmd->max; i++)
 {
  if (pmd->toggle)
   XtAddCallback(This->w_Buttons[i], XmNvalueChangedCallback,
		 PulldownMenu_DynamicCallback, This);
  else
   XtAddCallback(This->w_Buttons[i], XmNactivateCallback,
		 PulldownMenu_DynamicCallback, This);
 }
}

static void PulldownMenu_CreateDynamic(This, num_pmd)
c_PulldownMenu *This;
int	num_pmd;
{
 int	i;
 Arg	argw[1];
 char	dname[20];
 pm_dynamic_t *pmd;
 XmString	string;
 XtAppContext	app;
 XtActionsRec	rec;
  
 pmd = &(This->pm_dynamic[num_pmd]);
  
 pmd->max_accelerator = 0;
 pmd->accelerator = NULL;
 pmd->acceleratorText = NULL;
  
 pmd->autocount = pmd->radio = FALSE;
 pmd->button_push = -1;
 pmd->text = Xc_malloc("text", pmd->max * sizeof(char *));
  
 string = XmStringCreateLtoR(" ", XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argw[0], XmNlabelString, string);
  
 rec.string = "xc_call_dynamic";
 rec.proc = (XtActionProc)Pulldown_CallDynamic;
 app = XtWidgetToApplicationContext(This->w_This);
 XtAppAddActions(app, &rec, 1);
  
 for(i = 0; i < pmd->max; i++)
 {
  pmd->text[i] = NULL;
    
  sprintf(dname, "Dynamic %d", i + 1);
  if (!pmd->toggle)
  {
   This->w_Buttons[pmd->numberw + i] = 
    XmCreatePushButtonGadget(This->w_This, dname, argw, 1);
      
      
   Xc_REGISTER_HELP(This->w_Buttons[pmd->numberw + i]);
  }
  else
  {
   This->w_Buttons[pmd->numberw + i] = 
    XmCreateToggleButtonGadget(This->w_This, dname, argw, 1);
   Xc_REGISTER_HELP(This->w_Buttons[pmd->numberw + i]);
  }
 }
 XmStringFree(string);
  
  
 for(i = 0; i < pmd->min; i++)
  XtManageChild(This->w_Buttons[pmd->numberw + i]);
}

void PulldownMenu_DynamicCallback(w, This)
Widget	w;
c_PulldownMenu	*This;
{
 int	i, button;
 pm_dynamic_t *pmd;
  
 i = 0;
 while(i < This->numberw)
 {
  if (This->w_Buttons[i] == w)
   break;
  i++;
 }
  
 if (i == This->numberw)
 {
  Xc_WARNING(("Callback on an unknow widget..."));
  return;
 }
  
 pmd = This->pm_dynamic;
  
 button = i; i = 0;
 while(i < This->nb_pm_dynamic && button < pmd[i].numberw && 
       button > pmd[i].numberw+pmd[i].max)
  i++;
 if (i == This->nb_pm_dynamic)
 {
  Xc_WARNING(("Callback not on a dynamic widget..."));
  return;    
 }
 pmd = &(pmd[i]);

 pmd->callback(pmd->callback_data, w, button - pmd[i].numberw);
}

static void Pulldown_CallDynamic(w, event, args, nargs)
Widget	w;
XEvent	*event;
char	*args[];
int	*nargs;
{
  
  
 if (XtClass(w) == xmPushButtonGadgetClass)
 {
  if ( XtHasCallbacks(w, XmNactivateCallback) == XtCallbackHasSome)
  {
   XmPushButtonCallbackStruct cbs;
      
   cbs.event = event;
   cbs.click_count = 1;
   cbs.reason = XmCR_ACTIVATE;
      
   XtCallCallbacks(w, XmNactivateCallback, &cbs);
  }
 }
  
 if (XtClass(w) == xmToggleButtonGadgetClass)
 {
  if ( XtHasCallbacks(w, XmNvalueChangedCallback) == XtCallbackHasSome)
  {
   XmToggleButtonCallbackStruct cbs;
      
   cbs.event = event;
   cbs.reason = XmCR_ACTIVATE;
   cbs.set = True;
   XtCallCallbacks(w, XmNvalueChangedCallback, &cbs);
  }
 }
}
