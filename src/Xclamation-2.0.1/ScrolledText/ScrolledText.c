/*
** ScrolledText.c for Xclamation in ScrolledText/
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
** Started on  Thu Nov 28 18:25:14 1996 Robin Castanier
** Last update Thu Nov 28 22:47:54 1996 Robin Castanier
*/


#include "ScrolledText.h"

void *cons_ScrolledText();
void dest_ScrolledText();
void *copy_ScrolledText();

sf_ScrolledText fc_ScrolledText =
{
  cons_ScrolledText,
  dest_ScrolledText,
  copy_ScrolledText,
};

void *cons_ScrolledText(w_Parent, ManageW, Title)
Widget		w_Parent;
c_ManageWidget	*ManageW;
char 		*Title;
{
  c_ScrolledText	*ObjTmp;
  Arg			argsw[10];
  int			nargs;

  if ((ObjTmp = (c_ScrolledText *)Xc_malloc("ScrolledText",
					      sizeof(c_ScrolledText)))
      == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_ScrolledText;
  nargs = 0;
  XtSetArg(argsw[nargs], XmNtraversalOn, False);
  nargs++;
  XtSetArg(argsw[nargs], XmNeditMode, XmMULTI_LINE_EDIT);
  nargs++;
  XtSetArg(argsw[nargs], XmNmarginWidth, 10);
  nargs++;
  XtSetArg(argsw[nargs], XmNwordWrap, True);
  nargs++;
  XtSetArg(argsw[nargs], XmNscrollHorizontal, False);
  nargs++;
  XtSetArg(argsw[nargs], XmNverifyBell, False);
  nargs++;
  XtSetArg(argsw[nargs], XmNcursorPositionVisible, False);
  nargs++;
  XtSetArg(argsw[nargs], XmNeditable, False);
  nargs++;
  XtSetArg(argsw[nargs], XmNscrollingPolicy, XmAUTOMATIC);
  nargs++;
  ObjTmp->w_This = XmCreateScrolledText(w_Parent, Title, argsw, nargs);
  if (ManageW)
    {
      F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);
    }
  Xc_TRACE(("Object ScrolledText build"));
  return ObjTmp;
}

void dest_ScrolledText(This)
c_ScrolledText *This;
{
  XtDestroyWidget(This->w_This);
  Xc_free(This);
  Xc_TRACE(("Object ScrolledText destroyed"));
}

void *copy_ScrolledText(This)
c_ScrolledText *This;
{
  c_ScrolledText *ObjTmp;

  if ((ObjTmp = (c_ScrolledText *)Xc_malloc("CScrolledText",
					      sizeof(c_ScrolledText)))
      == NULL)
    {
      Xc_FATAL(("Can't copy this object: memory allocation error"));
    }
  memcpy(ObjTmp, This, sizeof(c_ScrolledText));
  Xc_TRACE(("Object ScrolledText copied"));
  return ObjTmp;
}
