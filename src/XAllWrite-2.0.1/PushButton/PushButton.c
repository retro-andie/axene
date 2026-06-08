/*
** PushButton.c for XAllWrite in PushButton/
** encapsulation of Motif's ToggleButton Widget
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Wed Jan  3 22:16:04 1996 Emmanuel Paris
** Last update Mon Apr  1 16:05:37 1996 Robin Castanier
*/

#include "PushButton.h"
#include "RegisterHelp.h"

void *cons_PushButton();
void dest_PushButton();
void *copy_PushButton();
void PushButton_AddCallback();

sf_PushButton fc_PushButton =
{
  cons_PushButton,
  dest_PushButton,
  copy_PushButton,
  PushButton_AddCallback,
};

/* ----------------------------------------------------------------- ** 
** constructor                                                       ** 
** ----------------------------------------------------------------- */
void *cons_PushButton(w_Parent, ManageW, Title)
Widget w_Parent;
c_ManageWidget *ManageW;
char   *Title;
{
  c_PushButton	*ObjTmp;
  Arg		argsw[4];
  int		nargs;
  
  if ((ObjTmp = (c_PushButton *)Xc_malloc("PushButton",
					  sizeof(c_PushButton))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_PushButton;
  
  nargs = 0;
  ObjTmp->w_This = XmCreatePushButton(w_Parent, Title, argsw, nargs);
  Xc_REGISTER_HELP(ObjTmp->w_This);
  if (ManageW)
    F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);  
  Xc_TRACE(("Object PushButton build"));
  return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
void dest_PushButton(This)
c_PushButton *This;
{
  XtDestroyWidget(This->w_This);
  Xc_TRACE(("Object PushButton destroyed"));
  Xc_free(This);
}

/* ----------------------------------------------------------------- ** 
** copy function                                                     ** 
** ----------------------------------------------------------------- */
void *copy_PushButton(This)
c_PushButton *This;
{
  Xc_TRACE(("Object PushButton not copied, not usefull"));
  return NULL;
}

/* ----------------------------------------------------------------- ** 
** Add Callbacks                                                     ** 
** ----------------------------------------------------------------- */
void PushButton_AddCallback(This, reason, func, data)
c_PushButton	*This;
String		reason;
void		(*func)();
char		*data;
{
  XtAddCallback(WIDGET(This),
	        reason, func, data);
}
