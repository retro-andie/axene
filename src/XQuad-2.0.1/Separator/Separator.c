/*
** Separator.c for XQuad in Separator/
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
** Started on  Thu Dec 15 18:44:46 1994 Robin Castanier
** Last update Thu Apr 11 13:35:33 1996 Robin Castanier
*/

#include "Separator.h"

static void *constructor();
static void destructor();
static void *copy();

sf_Separator fc_Separator =
{
 constructor,
 destructor,
 copy,
};

/*----------------------------------------------------------------------**
**		Constructor for object Separator ...			**
**----------------------------------------------------------------------*/

static void *constructor(w_Parent, Manage_W, mode)
Widget 		w_Parent;
c_ManageWidget 	*Manage_W;
int		mode;
{
  c_Separator	*ObjTmp;
  Arg		argsw[4];
  int		nargs;

  Xc_HISTORY(("Constructor ..."));

  if (!(ObjTmp = (c_Separator *) Xc_malloc("Separator", sizeof (c_Separator))))
    Xc_FATAL(("Can't allocate object ... exit"));
  
  nargs=0;
  switch (mode)
    {
    case XcSEPARATOR_H :
      XtSetArg(argsw[nargs], XmCOrientation, XmHORIZONTAL); nargs++;
      break;
    case XcSEPARATOR_V :
      XtSetArg(argsw[nargs], XmCOrientation, XmVERTICAL); nargs++;
      break;
    }
  ObjTmp->f= &fc_Separator;
  ObjTmp->w_This = XmCreateSeparatorGadget(w_Parent, "___separator___",
					   argsw, nargs);
  
  if (Manage_W)
    F(Manage_W).Add_child(Manage_W, ObjTmp->w_This, w_Parent);
  
  Xc_HISTORY(("Constructor ok"));
  return ObjTmp;
}

/*----------------------------------------------------------------------**
**		Destructor for object Separator ...			**
**----------------------------------------------------------------------*/

static void destructor(ObjectToDelete)
c_Separator *ObjectToDelete;
{
  Xc_HISTORY(("Destructor ..."));

  XtDestroyWidget(ObjectToDelete->w_This);
  Xc_free(ObjectToDelete);

  Xc_HISTORY(("Destructor ok"));
}

/*----------------------------------------------------------------------**
**		Copy method for object Separator ...			**
**----------------------------------------------------------------------*/

static void *copy(ObjectToCopy)
c_Separator *ObjectToCopy;
{
 c_Separator *ObjTmp;

 Xc_HISTORY(("Copy ..."));

 if (!(ObjTmp = (c_Separator *) Xc_malloc("Separator", sizeof(c_Separator))))
   Xc_SYSERROR(("Not enough memory to allocate object"));

 memcpy(ObjTmp, ObjectToCopy, sizeof(c_Separator));

 Xc_HISTORY(("Copy ok"));
 return ObjTmp;
}
