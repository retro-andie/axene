/*
** MenuBar.c for Xclamation, XQuad, XAllWrite and XMayday in MenuBar/
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
** Started on  Mon Jul 17 19:44:00 1995 Robin Castanier
** Last update Thu Sep 17 15:24:13 1998 Emmanuel Paris
*/

#include "MenuBar.h"
#include "RegisterHelp.h"

void *cons_MenuBar();
void dest_MenuBar();
void *copy_MenuBar();

sf_MenuBar fc_MenuBar =
{
 cons_MenuBar,
 dest_MenuBar,
 copy_MenuBar,
};

void *cons_MenuBar(w_Parent, ManageW, Title)
Widget		w_Parent;
c_ManageWidget	*ManageW;
char		*Title;
{
 c_MenuBar	*ObjTmp;
 Arg		argsw[4];
 int		nargs;

 if ((ObjTmp=(c_MenuBar *)Xc_malloc("MenuBar",
				    sizeof(c_MenuBar))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_MenuBar;
 nargs = 0;
 XtSetArg(argsw[nargs], XmNscrollingPolicy, XmAUTOMATIC);
 nargs++;
 ObjTmp->w_This = XmCreateMenuBar(w_Parent, Title, argsw, nargs);
 Xc_REGISTER_HELP(ObjTmp->w_This);
 if (ManageW)
 {
  F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);
 }
 Xc_TRACE(("Object MenuBar build"));
 return ObjTmp;
}

void dest_MenuBar(This)
c_MenuBar *This;
{
 XtDestroyWidget(This->w_This);
 Xc_free(This);
 Xc_TRACE(("Object MenuBar destroyed"));
}

void *copy_MenuBar(This)
c_MenuBar *This;
{
 c_MenuBar *ObjTmp;

 if ((ObjTmp = (c_MenuBar *)Xc_malloc("CMenuBar",
				      sizeof (c_MenuBar))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_MenuBar));
 Xc_TRACE(("Object MenuBar copied"));
 return ObjTmp;
}





