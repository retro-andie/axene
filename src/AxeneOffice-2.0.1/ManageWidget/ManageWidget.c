/*
** ManageWidget.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in ManageWidget/
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
** Started on  Mon Jul 17 19:38:47 1995 Antoine Buat
** Last update Tue Jan 20 18:33:05 1998 Robin Castanier
*/

#ifndef NTRACE
#define NTRACE
#endif

#include "ManageWidget.h"

void *cons_ManageWidget();
void dest_ManageWidget();
void *copy_ManageWidget();
void Manage_all();
void Manage_Add_child();

sf_ManageWidget fc_ManageWidget =
{
 cons_ManageWidget,
 dest_ManageWidget,
 copy_ManageWidget,
 Manage_all,
 Manage_Add_child
};

void *cons_ManageWidget()
{
 c_ManageWidget *ObjTmp;

 Xc_HISTORY(("constructor"));
 if ((ObjTmp = (c_ManageWidget *)Xc_malloc("ManageWidget",
					   sizeof(c_ManageWidget))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_ManageWidget;
 ObjTmp->number = 0;
 ObjTmp->w_childlist = NULL;
 return ObjTmp;
}

void dest_ManageWidget(This)
c_ManageWidget *This;
{
 listwidget	*ListTmp;
 listwidget	*ListTmp2;

 Xc_HISTORY(("destructor"));
 ListTmp = This->w_childlist;
 while (ListTmp)
 {
  ListTmp2 = ListTmp->NextWidget;
  Xc_free(ListTmp);
  ListTmp = ListTmp2;
 }
 Xc_TRACE(("gato"));
 Xc_free(This);
}

void *copy_ManageWidget(This)
c_ManageWidget *This;
{
 c_ManageWidget *ObjTmp;

 Xc_HISTORY(("copy"));
 if ((ObjTmp = (c_ManageWidget *)Xc_malloc("CManageWidget",
					   sizeof(c_ManageWidget))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_ManageWidget));
 return ObjTmp;
}

void Manage_all(This, w_root)
c_ManageWidget *This;
Widget		w_root;
{
 Widget	*listtomanage;
 listwidget	*temp;
 listwidget    **temp2;
 int		num;
 int		nbr;
 Widget	w_same_parent;

 Xc_HISTORY(("manage all"));
 if(This->number != 0)
 {
  if ((listtomanage = (Widget *)Xc_malloc("widgetlisttomanage",
					  This->number*sizeof(Widget))) 
      == NULL)
  {
   Xc_FATAL(("Can't create widget list to manage: memory error!"));
  }
  while (This->number > 0)
  {
   w_same_parent = This->w_childlist->w_parent;
   num = 0;
   nbr = 0;
   temp2 = &(This->w_childlist);
   Xc_TRACE(("parent = %lx", w_same_parent));
   while (num < This->number)
   {
    temp = *temp2;
    if (temp->w_parent == w_same_parent)
    {
     listtomanage[nbr] = temp->w_current;
     *temp2 = temp->NextWidget;
     This->number--;
     nbr++;
     Xc_free(temp);
    }
    else
    {
     temp2 = &(temp->NextWidget);
     num++;
    }
   }
   Xc_TRACE(("start Manage_children %d", nbr));
   XtManageChildren(listtomanage, nbr);
   Xc_TRACE(("end Manage_children"));
  }
  Xc_free(listtomanage);
 }
 This->w_childlist = NULL;
 if (w_root)
 {
  Xc_TRACE(("realize"));
  XtRealizeWidget(w_root);
  XFlush(XtDisplay(w_root));
  XSync(XtDisplay(w_root), False);
 }
 DELETE(c_ManageWidget)(This);
}

void Manage_Add_child(This, child, parent)
c_ManageWidget	*This;
Widget		child;
Widget		parent;
{
 listwidget	**WidgetTmp;
 listwidget	*WidgetNew;

 if ((WidgetNew = (listwidget *)Xc_malloc("addwidget",
					  sizeof (listwidget))) == NULL)
 {
  Xc_FATAL(("Can't add new Widget to the Manager: memory error"));
 }

 WidgetNew->w_current = child;
 WidgetNew->w_parent = parent;
 WidgetNew->NextWidget = NULL;

 WidgetTmp = &D(This).w_childlist;
 while (*WidgetTmp != NULL)
 {
  WidgetTmp = &((*WidgetTmp)->NextWidget);
 }
 *WidgetTmp = WidgetNew;
 This->number++;
}

