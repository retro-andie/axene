/*
** PanedWindow.c for Xclamation, XQuad and XMayday in PanedWindow/
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
** Started on  Tue Jul 18 11:44:26 1995 Antoine Buat
** Last update Sat May 25 18:12:11 1996 Emmanuel Paris
*/

#include "PanedWindow.h"
#include "RegisterHelp.h"

void *cons_PanedWindow();
void dest_PanedWindow();
void *copy_PanedWindow();

sf_PanedWindow fc_PanedWindow =
{
  cons_PanedWindow,
  dest_PanedWindow,
  copy_PanedWindow,
};

void *cons_PanedWindow(w_Parent, ManageW, Title)
Widget		w_Parent;
c_ManageWidget	*ManageW;
char		*Title;
{
  c_PanedWindow	*ObjTmp;
  Arg		argsw[4];
  int	 	nargs;

  if ((ObjTmp = (c_PanedWindow *)Xc_malloc("PanedWindow",
					sizeof(c_PanedWindow))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_PanedWindow;
  nargs = 0;
  XtSetArg(argsw[nargs], XmNscrollingPolicy, XmAUTOMATIC);
  nargs++;
  ObjTmp->w_This = XmCreatePanedWindow(w_Parent, Title, argsw, nargs);
  Xc_REGISTER_HELP(ObjTmp->w_This);
  if (ManageW)
    {
      F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);
    }
  Xc_TRACE(("Object PanedWindow build"));
  return ObjTmp;
}

void dest_PanedWindow(This)
c_PanedWindow *This;
{
  XtDestroyWidget(This->w_This);
  Xc_free(This);
  Xc_TRACE(("Object PanedWindow destroyed"));
}

void *copy_PanedWindow(This)
c_PanedWindow *This;
{
  c_PanedWindow *ObjTmp;

  if ((ObjTmp = (c_PanedWindow *)Xc_malloc("CPanedWindow",
					   sizeof(c_PanedWindow))) == NULL)
    {
      Xc_FATAL(("Can't copy this object: memory allocation error"));
    }
  memcpy(ObjTmp, This, sizeof(c_PanedWindow));
  Xc_TRACE(("Object PanedWindow copied"));
  return ObjTmp;
}

