/*
** RowColumn.c for Xclamation, XQuad, XAllWrite, XMayday and AxeneOffice in 
** 	RowColumn/
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
** Started on  Tue Jul 18 12:16:56 1995 Robin Castanier
** Last update Tue Jan 20 16:31:51 1998 Robin Castanier
*/

#include "RowColumn.h"
#include "RegisterHelp.h"

void *cons_RowColumn();
void dest_RowColumn();
void *copy_RowColumn();

sf_RowColumn fc_RowColumn =
{
 cons_RowColumn,
 dest_RowColumn,
 copy_RowColumn,
};

void *cons_RowColumn(w_Parent, ManageW, Title, Width, Height, Orient)
Widget		w_Parent;
c_ManageWidget	*ManageW;
char		*Title;
int		Width;
int		Height;
int		Orient;
{
 c_RowColumn	*ObjTmp;
 Arg		argsw[4];
 int		nargs;

 if ((ObjTmp = (c_RowColumn *)Xc_malloc("RowColumn",
					sizeof(c_RowColumn))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_RowColumn;
 nargs = 0;
 if (Width > 0)
 {
  XtSetArg(argsw[nargs], XmNwidth, Width);
  nargs++;
 }
 if (Height > 0)
 {
  XtSetArg(argsw[nargs], XmNheight, Height);
  nargs++;
 }
 XtSetArg(argsw[nargs], XmNorientation, Orient);
 nargs++;
 ObjTmp->w_This = XmCreateRowColumn(w_Parent, Title, argsw, nargs);
 Xc_REGISTER_HELP(ObjTmp->w_This);
  
 if (ManageW)
 {
  F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);
 }
 Xc_TRACE(("Object RowColumn build"));
 return ObjTmp;
}

void dest_RowColumn(This)
c_RowColumn *This;
{
 XtDestroyWidget(This->w_This);
 Xc_free(This);
 Xc_TRACE(("Object RowColumn destroyed"));
}

void *copy_RowColumn(This)
c_RowColumn *This;
{
 c_RowColumn *ObjTmp;

 if ((ObjTmp=(c_RowColumn *)Xc_malloc("CRowColumn",
				      sizeof(c_RowColumn))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_RowColumn));
 Xc_TRACE(("Object RowColumn copied"));
 return ObjTmp;
}
