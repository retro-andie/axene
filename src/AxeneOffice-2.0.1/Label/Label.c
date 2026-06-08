/*
** Label.c for AxeneOffice in Label/
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
** Started on  Mon Jul 17 19:01:33 1995 Robin Castanier
** Last update Fri Feb  6 03:00:34 1998 Emmanuel Paris
*/

#include "Label.h"
#include "RegisterHelp.h"

void *cons_Label();
void dest_Label();
void *copy_Label();

sf_Label fc_Label =
{
 cons_Label,
 dest_Label,
 copy_Label
};

void *cons_Label(w_Parent, ManageW, Title)
Widget w_Parent;
c_ManageWidget *ManageW;
char   *Title;
{
 c_Label	*ObjTmp;
 Arg		argsw[4];
 int		nargs;
  
 if ((ObjTmp = (c_Label *)Xc_malloc("Label", sizeof(c_Label))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Label;
  
 nargs = 0;
 ObjTmp->w_This = XmCreateLabel(w_Parent, Title, argsw, nargs);
 Xc_REGISTER_HELP(ObjTmp->w_This);
 if (ManageW)
  F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);  
 Xc_TRACE(("Object Label build"));
 return ObjTmp;
}

void dest_Label(This)
c_Label *This;
{
 XtDestroyWidget(This->w_This);
 Xc_TRACE(("Object Label destroyed"));
 Xc_free(This);
}

void *copy_Label(This)
c_Label *This;
{
 c_Label *ObjTmp;
  
 if ((ObjTmp = (c_Label *)Xc_malloc("CLabel", sizeof(c_Label))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }

 memcpy(ObjTmp, This, sizeof(c_Label));
 Xc_TRACE(("Object Label copied"));
 return ObjTmp;
}




