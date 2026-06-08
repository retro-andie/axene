/*
** ToggleButton.c for Xclamation, XQuad, XAllWrite, XMayday and XInstall in 
** 	ToggleButton/
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
** Started on  Tue Jul 18 12:44:00 1995 Robin Castanier
** Last update Wed Mar 12 16:38:55 1997 Robin Castanier
*/

#include "ToggleButton.h"
#include "RegisterHelp.h"

void *cons_ToggleButton();
void dest_ToggleButton();
void *copy_ToggleButton();
void ToggleButton_Set();
void ToggleButton_AddCallback();

sf_ToggleButton fc_ToggleButton =
{
 cons_ToggleButton,
 dest_ToggleButton,
 copy_ToggleButton,
 ToggleButton_AddCallback,
 ToggleButton_Set
};

/* ----------------------------------------------------------------- ** 
** constructor                                                       ** 
** ----------------------------------------------------------------- */
void *cons_ToggleButton(w_Parent, ManageW, Title)
Widget w_Parent;
c_ManageWidget *ManageW;
char   *Title;
{
  c_ToggleButton	*ObjTmp;
  Arg		argsw[4];
  int		nargs;
  
  if ((ObjTmp = (c_ToggleButton *)Xc_malloc("ToggleButton",
					    sizeof(c_ToggleButton))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_ToggleButton;
  
  nargs = 0;
  ObjTmp->w_This = XmCreateToggleButtonGadget(w_Parent, Title, argsw, nargs);
  Xc_REGISTER_HELP(ObjTmp->w_This);

  if (ManageW)
    F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);  
  Xc_TRACE(("Object ToggleButton build"));
  return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
void dest_ToggleButton(This)
c_ToggleButton *This;
{
  XtDestroyWidget(This->w_This);
  Xc_TRACE(("Object ToggleButton destroyed"));
  Xc_free(This);
}

/* ----------------------------------------------------------------- ** 
** copy function                                                     ** 
** ----------------------------------------------------------------- */
void *copy_ToggleButton(This)
c_ToggleButton *This;
{
  c_ToggleButton *ObjTmp;
  
  if ((ObjTmp = (c_ToggleButton *)Xc_malloc("CToggleButton", sizeof(c_ToggleButton))) == NULL)
    {
      Xc_FATAL(("Can't copy this object: memory allocation error"));
    }

 memcpy(ObjTmp, This, sizeof(c_ToggleButton));
 Xc_TRACE(("Object ToggleButton copied"));
 return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** Add Callbacks                                                     ** 
** ----------------------------------------------------------------- */
void ToggleButton_AddCallback(This, reason, func, data)
c_ToggleButton	*This;
String		reason;
void		(*func)();
char		*data;
{
  XtAddCallback(WIDGET(This),
	        reason, func, data);
}

/* ----------------------------------------------------------------- ** 
** set button                                                        ** 
** ----------------------------------------------------------------- */
void ToggleButton_Set(This, setto, callit)
c_ToggleButton	*This;
Boolean		setto;
Boolean		callit;
{
  XmToggleButtonGadgetSetState(WIDGET(This), setto, callit);
}

