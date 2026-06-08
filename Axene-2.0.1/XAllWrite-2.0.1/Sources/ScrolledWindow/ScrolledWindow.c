/*
** ScrolledWindow.c for XAllWrite in ScrolledWindow/
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
** Started on  Tue Jul 18 12:25:21 1995 Robin Castanier
** Last update Tue Jul 18 12:25:21 1995 Robin Castanier
*/


#include "ScrolledWindow.h"

void *cons_ScrolledWindow();
void dest_ScrolledWindow();
void *copy_ScrolledWindow();

sf_ScrolledWindow fc_ScrolledWindow =
{
  cons_ScrolledWindow,
  dest_ScrolledWindow,
  copy_ScrolledWindow,
};

void *cons_ScrolledWindow(w_Parent, ManageW, Title)
Widget		w_Parent;
c_ManageWidget	*ManageW;
char 		*Title;
{
  c_ScrolledWindow	*ObjTmp;
  Arg			argsw[4];
  int			nargs;

  if ((ObjTmp = (c_ScrolledWindow *)Xc_malloc("ScrolledWindow",
					      sizeof(c_ScrolledWindow)))
      == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_ScrolledWindow;
  nargs = 0;
  XtSetArg(argsw[nargs], XmNscrollingPolicy, XmAUTOMATIC);
  nargs++;
  ObjTmp->w_This = XmCreateScrolledWindow(w_Parent, Title, argsw, nargs);
  if (ManageW)
    {
      F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);
    }
  Xc_TRACE(("Object ScrolledWindow build"));
  return ObjTmp;
}

void dest_ScrolledWindow(This)
c_ScrolledWindow *This;
{
  XtDestroyWidget(This->w_This);
  Xc_free(This);
  Xc_TRACE(("Object ScrolledWindow destroyed"));
}

void *copy_ScrolledWindow(This)
c_ScrolledWindow *This;
{
  c_ScrolledWindow *ObjTmp;

  if ((ObjTmp = (c_ScrolledWindow *)Xc_malloc("CScrolledWindow",
					      sizeof(c_ScrolledWindow)))
      == NULL)
    {
      Xc_FATAL(("Can't copy this object: memory allocation error"));
    }
  memcpy(ObjTmp, This, sizeof(c_ScrolledWindow));
  Xc_TRACE(("Object ScrolledWindow copied"));
  return ObjTmp;
}
