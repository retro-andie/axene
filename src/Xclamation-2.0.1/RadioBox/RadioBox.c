/*
** RadioBox.c for Xclamation, XQuad, XAllWrite and XMayday in RadioBox/
** Method for Class RadioBox
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
** Started on  Wed Oct 19 11:20:42 1994 Antoine Buat
** Last update Wed Mar 12 11:40:33 1997 Robin Castanier
*/

#include "RadioBox.h"
#include "RegisterHelp.h"

void *cons_RadioBox();
void dest_RadioBox();
void *copy_RadioBox();
void RadioBox_AddCallback();
void RadioBox_AddCallbackByName();
void RadioBox_AddSetCondition();
void RadioBox_AddUnsetCondition();
void RadioBox_AddSetfCondition();
void RadioBox_AddUnsetfCondition();
void Callback_Test();

/* ----------------------------------------------------------------- ** 
** seting the method                                                 ** 
** ----------------------------------------------------------------- */
sf_RadioBox fc_RadioBox =
{
  cons_RadioBox,
  dest_RadioBox,
  copy_RadioBox,
  RadioBox_AddCallback,
  RadioBox_AddCallbackByName,
  RadioBox_AddSetCondition,
  RadioBox_AddUnsetCondition,
  RadioBox_AddSetfCondition,
  RadioBox_AddUnsetfCondition
  };

/* ----------------------------------------------------------------- ** 
** constructor                                                       ** 
** ----------------------------------------------------------------- */
void *cons_RadioBox(w_Parent, ManageW, Norme, Title, va_alist)
Widget		w_Parent; 
c_ManageWidget	*ManageW;
int		Norme;
char 		*Title;
va_dcl
{
  va_list 	other_arg;
  c_RadioBox	*ObjTmp;
  int		names_number;
  char		*titletmp;
  listbutton	**PtrButton;
  Arg		argsw[3];
  
  if ((ObjTmp = (c_RadioBox *)Xc_malloc("RadioBox", 
					sizeof(c_RadioBox))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_RadioBox;
  ObjTmp->Title = Title;
  XtSetArg(argsw[0], XmNradioBehavior, FALSE);
  XtSetArg(argsw[1], XmNmarginWidth, 0);
  ObjTmp->w_This = XmCreateRadioBox(w_Parent, Title, argsw, 2);
  XtManageChild(ObjTmp->w_This);
  Xc_REGISTER_HELP(ObjTmp->w_This);
  ObjTmp->Button = NULL;
  ObjTmp->state = 0;
  PtrButton = &ObjTmp->Button;
 
  va_start(other_arg);
  names_number = 0;
  while ((titletmp = va_arg(other_arg, char *)))
    {
      names_number++;
    }
  va_end(other_arg);

  ObjTmp->number = names_number;
  if (names_number)
    {
      va_start(other_arg);
      names_number = 0;
      while (names_number < ObjTmp->number)
	{
	  titletmp = va_arg(other_arg, char *);
	  *PtrButton = (listbutton *)Xc_malloc("Tbutton",
					       sizeof(listbutton));
	  (*PtrButton)->setcondition = NULL;
	  (*PtrButton)->unsetcondition = NULL;
	  (*PtrButton)->setfcondition = NULL;
	  (*PtrButton)->unsetfcondition = NULL;
	  XtSetArg(argsw[0], XmNshadowThickness, 0);
	  (*PtrButton)->w_Buttons =
	    XmCreateToggleButtonGadget(ObjTmp->w_This, titletmp, argsw, 1);
	  (*PtrButton)->Name = titletmp;
	  if (Norme == RADIO_TEST)
	    XtAddCallback((*PtrButton)->w_Buttons, 
			  XmNvalueChangedCallback,
			  Callback_Test, ObjTmp);
	  XtManageChild((*PtrButton)->w_Buttons);
	  PtrButton = &((*PtrButton)->Next);
	  names_number++;
	}
      *PtrButton = NULL;
      va_end(other_arg);
    }
  if(ManageW) F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);
  Xc_TRACE(("Object RadioBox build"));
  return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
void dest_RadioBox(This)
c_RadioBox *This;
{
  listbutton		*but;
  listbutton		*buttmp;
  listsetcondition	*cond;
  listsetcondition	*condtmp;

  XtDestroyWidget(This->w_This);
  Xc_TRACE(("Object RadioBox destroyed"));
  but = This->Button;
  while (but)
    {
      cond = but->setcondition;
      while (cond)
	{
	  condtmp = cond->Next;
	  Xc_free(cond);
	  cond = condtmp;
	}
      cond = but->unsetcondition;
      while (cond)
	{
	  condtmp = cond->Next;
	  Xc_free(cond);
	  cond = condtmp;
	}
      cond = but->setfcondition;
      while (cond)
	{
	  condtmp = cond->Next;
	  Xc_free(cond);
	  cond = condtmp;
	}
      cond = but->unsetfcondition;
      while (cond)
	{
	  condtmp = cond->Next;
	  Xc_free(cond);
	  cond = condtmp;
	}
      buttmp = but->Next;
      Xc_free(but);
      but = buttmp;
    }
  Xc_free(This);
}

/* ----------------------------------------------------------------- ** 
** copy method                                                       ** 
** ----------------------------------------------------------------- */
void *copy_RadioBox(This)
c_RadioBox *This;
{
  c_RadioBox *ObjTmp;

  if ((ObjTmp = (c_RadioBox *)Xc_malloc("CRadioBox",
					sizeof(c_RadioBox))) == NULL)
    {
      Xc_FATAL(("Can't copy this object: memory allocation error"));
    }
  memcpy(ObjTmp, This, sizeof(c_RadioBox));
  Xc_TRACE(("Object RadioBox copied"));
  return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** Add Callbacks to buttons                                          ** 
** ----------------------------------------------------------------- */
void RadioBox_AddCallback(This, buttonnum, reason, func, data)
c_RadioBox	*This;
int		buttonnum;
String		reason;
void		(*func)();
char		*data;
{
  int 		but;
  listbutton	*tmplist;

  if (buttonnum >= This->number)
    {
      Xc_WARNING(("Button %d does not exist", buttonnum));
      return;
    }
  but = 0;
  tmplist = This->Button;
  while (but != buttonnum)
    {
      tmplist = tmplist->Next;
      but++;
    }
  XtAddCallback(tmplist->w_Buttons,
	        reason, func, data);
}

/* ----------------------------------------------------------------- ** 
** Add Callbacks to buttons by name                                  ** 
** ----------------------------------------------------------------- */
void RadioBox_AddCallbackByName(This, name, reason, func, data)
c_RadioBox	*This;
char		*name;
String		reason;
void		(*func)();
char		*data;
{
  int 		but;
  listbutton	*tmplist;

  but = 0;
  tmplist = This->Button;
  while (strcmp(tmplist->Name, name) && (but<This->number))
    {
      tmplist = tmplist->Next;
      but++;
    }
  if (but >= This->number)
    {
      Xc_WARNING(("Button %s does not exist", name));
      return;      
    }
  XtAddCallback(tmplist->w_Buttons,
	        reason, func, data);
}

/* ----------------------------------------------------------------- ** 
** Add condition for a button to be set                              ** 
** ----------------------------------------------------------------- */
void RadioBox_AddSetCondition(This, buttonnum, va_alist)
c_RadioBox	*This;
int		buttonnum;
va_dcl
{
  va_list	 	other_arg;
  listbutton		*ptrbutton;
  listsetcondition	**ptrcondition;
  int			num;
  int			buttonplace;
  int			flag;

  ptrbutton = This->Button;
  num = 0;
  while (num < buttonnum)
    {
      ptrbutton = ptrbutton->Next;
      num++;
    }
  ptrcondition = &(ptrbutton->setcondition);
  while ((*ptrcondition) != NULL)
    {
      ptrcondition = &((*ptrcondition)->Next);
    }
  if ((*ptrcondition = (listsetcondition *)
       Xc_malloc("Set condition",
		 sizeof(listsetcondition))) == NULL)
    Xc_FATAL(("Can't build this object: memory allocation error"));

  (*ptrcondition)->Next = NULL;
  (*ptrcondition)->scondition = 0;
  (*ptrcondition)->ucondition = 0;

  va_start(other_arg);
  while ((buttonplace = va_arg(other_arg, int)) != COND_END)
    {
      flag =  va_arg(other_arg, int);
      switch (flag)
	{
	case B_IS_SET:
	  Xc_TRACE(("button %d must be set to enable %d\n", 
		 buttonplace, buttonnum));
	  (*ptrcondition)->scondition |= 1 << buttonplace;
	  break;
	case B_IS_UNSET:
	  Xc_TRACE(("button %d must be uset to enable %d\n", 
		 buttonplace, buttonnum));
	  (*ptrcondition)->ucondition |= 1 << buttonplace;
	  break;
	}
    }
  Xc_TRACE(("||\n"));
  va_end(other_arg);
}

/* ----------------------------------------------------------------- ** 
** Add condition for a button to be unset                            ** 
** ----------------------------------------------------------------- */
void RadioBox_AddUnsetCondition(This, buttonnum, va_alist)
c_RadioBox	*This;
int		buttonnum;
va_dcl
{
  va_list	 	other_arg;
  listbutton		*ptrbutton;
  listsetcondition	**ptrcondition;
  int			num;
  int			buttonplace;
  int			flag;

  ptrbutton = This->Button;
  num = 0;
  while (num < buttonnum)
    {
      ptrbutton = ptrbutton->Next;
      num++;
    }
  ptrcondition = &(ptrbutton->unsetcondition);
  while ((*ptrcondition) != NULL)
    {
      ptrcondition = &((*ptrcondition)->Next);
    }
  if ((*ptrcondition = (listsetcondition *)
       Xc_malloc("UnSet condition",
		 sizeof(listsetcondition))) == NULL)
    Xc_FATAL(("Can't build this object: memory allocation error"));

  (*ptrcondition)->Next = NULL;
  (*ptrcondition)->scondition = 0;
  (*ptrcondition)->ucondition = 0;

  va_start(other_arg);
  while ((buttonplace = va_arg(other_arg, int)) != COND_END)
    {
      flag =  va_arg(other_arg, int);
      switch (flag)
	{
	case B_IS_SET:
	  Xc_TRACE(("button %d must be set to disable %d\n", 
		 buttonplace, buttonnum));
	  (*ptrcondition)->scondition |= 1 << buttonplace;
	  break;
	case B_IS_UNSET:
	  Xc_TRACE(("button %d must be unset to disable %d\n", 
		 buttonplace, buttonnum));
	  (*ptrcondition)->ucondition |= 1 << buttonplace;
	  break;
	}
    }
  Xc_TRACE(("||\n"));
  va_end(other_arg);
}

/* ----------------------------------------------------------------- ** 
** Add condition for a button to be set                              ** 
** ----------------------------------------------------------------- */
void RadioBox_AddSetfCondition(This, buttonnum, va_alist)
c_RadioBox	*This;
int		buttonnum;
va_dcl
{
  va_list	 	other_arg;
  listbutton		*ptrbutton;
  listsetcondition	**ptrcondition;
  int			num;
  int			buttonplace;
  int			flag;

  ptrbutton = This->Button;
  num = 0;
  while (num < buttonnum)
    {
      ptrbutton = ptrbutton->Next;
      num++;
    }
  ptrcondition = &(ptrbutton->setfcondition);
  while ((*ptrcondition) != NULL)
    {
      ptrcondition = &((*ptrcondition)->Next);
    }
  if ((*ptrcondition = (listsetcondition *)
       Xc_malloc("SetF condition",
		 sizeof(listsetcondition))) == NULL)
    Xc_FATAL(("Can't build this object: memory allocation error"));

  (*ptrcondition)->Next = NULL;
  (*ptrcondition)->scondition = 0;
  (*ptrcondition)->ucondition = 0;

  va_start(other_arg);
  while ((buttonplace = va_arg(other_arg, int)) != COND_END)
    {
      flag =  va_arg(other_arg, int);
      switch (flag)
	{
	case B_IS_SET:
	  Xc_TRACE(("button %d must be set to force enable %d\n", 
		 buttonplace, buttonnum));
	  (*ptrcondition)->scondition |= 1 << buttonplace;
	  break;
	case B_IS_UNSET:
	  Xc_TRACE(("button %d must be uset to force enable %d\n", 
		 buttonplace, buttonnum));
	  (*ptrcondition)->ucondition |= 1 << buttonplace;
	  break;
	}
    }
  Xc_TRACE(("||\n"));
  va_end(other_arg);
}

void RadioBox_AddUnsetfCondition(This, buttonnum, va_alist)
c_RadioBox	*This;
int		buttonnum;
va_dcl
{
  va_list	 	other_arg;
  listbutton		*ptrbutton;
  listsetcondition	**ptrcondition;
  int			num;
  int			buttonplace;
  int			flag;

  ptrbutton = This->Button;
  num = 0;
  while (num < buttonnum)
    {
      ptrbutton = ptrbutton->Next;
      num++;
    }
  ptrcondition = &(ptrbutton->unsetfcondition);
  while ((*ptrcondition) != NULL)
    {
      ptrcondition = &((*ptrcondition)->Next);
    }
  if ((*ptrcondition = (listsetcondition *)
       Xc_malloc("UnSetF condition",
		 sizeof(listsetcondition))) == NULL)
    Xc_FATAL(("Can't build this object: memory allocation error"));

  (*ptrcondition)->Next = NULL;
  (*ptrcondition)->scondition = 0;
  (*ptrcondition)->ucondition = 0;

  va_start(other_arg);
  while ((buttonplace = va_arg(other_arg, int)) != COND_END)
    {
      flag =  va_arg(other_arg, int);
      switch (flag)
	{
	case B_IS_SET:
	  Xc_TRACE(("button %d must be set to force disable %d\n", 
		 buttonplace, buttonnum));
	  (*ptrcondition)->scondition |= 1 << buttonplace;
	  break;
	case B_IS_UNSET:
	  Xc_TRACE(("button %d must be uset to force disable %d\n", 
		 buttonplace, buttonnum));
	  (*ptrcondition)->ucondition |= 1 << buttonplace;
	  break;
	}
    }
  Xc_TRACE(("||\n"));
  va_end(other_arg);
}


void Callback_Test(w_This, This)
Widget		w_This;
c_RadioBox	*This;
{
  listbutton		*buttonptr;
  listsetcondition	*conditionptr;
  int			num;
  int			num2;
  int			new_state;

  new_state = 0;
  This->state_c = 0;
  This->state_cf = 0;
  This->state_u = 0;
  This->state_uf = 0;
  num = 0;
  buttonptr = This->Button;
  while (buttonptr->w_Buttons != w_This)
    {
      num++;
      buttonptr = buttonptr->Next;
    }
  This->state ^= (1 << num);
  Xc_TRACE(("state %d\n", This->state));

  num2 = 0;
  buttonptr = This->Button;
  while (num2 < This->number)
    {
/* ----------------------------------------------------------------- ** 
** test conditons to set                                             ** 
** ----------------------------------------------------------------- */
      conditionptr = buttonptr->setcondition;
      while (conditionptr)
	{
	  Xc_TRACE(("S to be set: %d\n", conditionptr->scondition));
	  Xc_TRACE(("S to be unset: %d\n", conditionptr->ucondition));
	  if (((This->state & conditionptr->scondition) == 
	       conditionptr->scondition)
	      && 
	      ((This->state & conditionptr->ucondition) == 0))
	    {
	      This->state_c |= (1 << num2);
	      break;
	    }
	  conditionptr = conditionptr->Next;
	}
/* ----------------------------------------------------------------- ** 
** test conditons to unset                                           ** 
** ----------------------------------------------------------------- */	  
      conditionptr = buttonptr->unsetcondition;
      while (conditionptr)
	{
	  Xc_TRACE(("U to be set: %d\n", conditionptr->scondition));
	  Xc_TRACE(("U to be unset: %d\n", conditionptr->ucondition));
	  if (((This->state & conditionptr->scondition) == 
	       conditionptr->scondition)
	      && 
	      ((This->state & conditionptr->ucondition) == 0))
	    {
	      This->state_u |= (1 << num2);
	      break;
	    }
	  conditionptr = conditionptr->Next;
	}
/* ----------------------------------------------------------------- ** 
** test conditons to force set                                       ** 
** ----------------------------------------------------------------- */	  
      conditionptr = buttonptr->setfcondition;
      while (conditionptr)
	{
	  Xc_TRACE(("FS to be set: %d\n", conditionptr->scondition));
	  Xc_TRACE(("FS to be unset: %d\n", conditionptr->ucondition));
	  if (((This->state & conditionptr->scondition) == 
	       conditionptr->scondition)
	      && 
	      ((This->state & conditionptr->ucondition) == 0))
	    {
	      This->state_c |= (1 << num2);
	      This->state_cf |= (1 << num2);
	      break;
	    }
	  conditionptr = conditionptr->Next;
	}
/* ----------------------------------------------------------------- ** 
** test conditons to force unset                                     ** 
** ----------------------------------------------------------------- */	  
      conditionptr = buttonptr->unsetfcondition;
      while (conditionptr)
	{
	  Xc_TRACE(("FU to be set: %d\n", conditionptr->scondition));
	  Xc_TRACE(("FU to be unset: %d\n", conditionptr->ucondition));
	  if (((This->state & conditionptr->scondition) == 
	       conditionptr->scondition)
	      && 
	      ((This->state & conditionptr->ucondition) == 0))
	    {
	      This->state_u |= (1 << num2);
	      This->state_uf |= (1 << num2);
	      break;
	    }
	  conditionptr = conditionptr->Next;
	}
      buttonptr = buttonptr->Next;
      num2++;
    }
  
  Xc_TRACE(("state change set: %d\n", This->state_c));
  Xc_TRACE(("state change unset: %d\n", This->state_u));
  Xc_TRACE(("state change set force: %d\n", This->state_cf));
  Xc_TRACE(("state change unset force: %d\n", This->state_uf));

  num2 = 0;
  buttonptr = This->Button;
  while (num2 < This->number)
    {
      if (num2 != num)
	{
	  if (This->state_cf & (1 << num2))
	    {
	      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
	      XtSetSensitive(buttonptr->w_Buttons, False);
	      new_state |= 1 << num2;
	      num2++;
	      buttonptr = buttonptr->Next;
	      continue;
	    }
	  if (This->state_uf & (1 << num2))
	    {
	      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, True);
	      XtSetSensitive(buttonptr->w_Buttons, False);
	      num2++;
	      buttonptr = buttonptr->Next;
	      continue;
	    }
	  if (This->state_c & (1 << num2))
	    {
	      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
	      XtSetSensitive(buttonptr->w_Buttons, True);
	      new_state |= 1 << num2;
	      num2++;
	      buttonptr = buttonptr->Next;
	      continue;
	    }
	  if (This->state_u & (1 << num2))
	    {
	      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, True);
	      XtSetSensitive(buttonptr->w_Buttons, True);
	      num2++;
	      buttonptr = buttonptr->Next;
	      continue;
	    }
	  if (This->state & (1 << num2))
	    {
	      XmToggleButtonGadgetSetState(buttonptr->w_Buttons, True, True);
	      XtSetSensitive(buttonptr->w_Buttons, True);
	      new_state |= 1 << num2;
	      num2++;
	      buttonptr = buttonptr->Next;
	      continue;
	    }
	  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, True);
	  XtSetSensitive(buttonptr->w_Buttons, True);
	}
      num2++;
      buttonptr = buttonptr->Next;
    }
  new_state |= This->state & (1 << num);
  This->state = new_state;
}
