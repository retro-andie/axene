/*
** ScrolledList.c for XQuad in ScrolledList/
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
** Started on  Tue Jul 18 12:23:49 1995 Marc Blet
** Last update Tue Jul 18 12:23:49 1995 Marc Blet
*/

#include "ScrolledList.h"
#include "RegisterHelp.h"


static void *constructor();
static void destructor();
static void *copy();
static void Initialize_Data();

sf_ScrolledList fc_ScrolledList =
{
  constructor,
  destructor,
  copy,
  Initialize_Data
};

int iValue1;

static void Initialize_Data(ObjTmp, Str_List, Count, Nbr_Visible)
c_ScrolledList	*ObjTmp;
XmStringTable	Str_List;
int		Count;
int		Nbr_Visible;
{
  Arg 		args[6];
  int		nargs;

  Xc_HISTORY(("Initializing ..."));

  nargs = 0;
  XtSetArg(args[nargs], XmNitemCount, Count); nargs++;
  XtSetArg(args[nargs], XmNvisibleItemCount, Nbr_Visible); nargs++;
  XtSetArg(args[nargs], XmNitems, Str_List); nargs++;
  XtSetValues(WIDGET(ObjTmp), args, nargs);

  ObjTmp->Item_Count = Count;

  Xc_HISTORY(("Initialize ok"));
}

/* --------------------------------------------------------------------	**
** ------	Concstructor of object ScrolledList 		  ----- **
** -------------------------------------------------------------------- */

static void *constructor(w_Parent, ManageW, Width, Height)
Widget 		w_Parent;
c_ManageWidget 	*ManageW;
int		Width;
int		Height;
{
  c_ScrolledList	*ObjTmp;
  Arg			args[4];
  int			n;
  
  Xc_HISTORY(("Concstructor ..."));

  if ((ObjTmp = (c_ScrolledList *) Xc_malloc("Selection Box",
					     sizeof(c_ScrolledList))) == NULL)
    {
      Xc_WARNING(("memory error ..."));
      return (ObjTmp);
    }

  ObjTmp->f = &fc_ScrolledList; 

  n=0;
  XtSetArg(args[n], XmNwidth, Width); n++;
  XtSetArg(args[n], XmNheight, Height); n++;

  ObjTmp->w_ScrolledList = XmCreateScrolledList(w_Parent, "ScrolledList", 
						args, n);

  XtVaSetValues(ObjTmp->w_ScrolledList,
                XmNselectionPolicy,     XmEXTENDED_SELECT,
                XmNlistSizePolicy,      XmVARIABLE,
                NULL); 

  F(ManageW).Add_child(ManageW, ObjTmp->w_ScrolledList, 
		       XtParent(ObjTmp->w_ScrolledList));

  ObjTmp->w_This = ObjTmp->w_ScrolledList;
  Xc_REGISTER_HELP(ObjTmp->w_This);

  Xc_HISTORY(("Constructor ok"));
  return (ObjTmp);
}


/* --------------------------------------------------------------------	**
** ------	Destructor of object ScrolledList 		  ----- **
** -------------------------------------------------------------------- */

static void destructor(This)
c_ScrolledList *This;
{
  Xc_HISTORY(("Destructor ..."));

  XtDestroyWidget(This->w_This);
  Xc_free(This);

  Xc_HISTORY(("Destructor ok"));
}


/* --------------------------------------------------------------------	**
** ------	Copy method of object ScrolledList 		  ----- **
** -------------------------------------------------------------------- */

static void *copy(This)
c_ScrolledList *This;
{
  c_ScrolledList *ObjTmp;
  
  Xc_HISTORY(("Copy object ..."));
  if ((ObjTmp = (c_ScrolledList *)Xc_malloc("CScrolledList",
					    sizeof(c_ScrolledList))) == NULL)
    {
      Xc_FATAL(("Can't copy this object: memory allocation error"));
    }
  memcpy(ObjTmp, This, sizeof(c_ScrolledList));

  Xc_HISTORY(("Copy object ok"));
  return ObjTmp; 
}


















