/*
** Box_paths.c for Xclamation in Box_paths/
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
** Started on  Tue Mar 19 11:36:15 1996 Robin Castanier
** Last update Wed Jun  4 17:36:36 1997 Emmanuel Paris
*/

#include "../Pixmaps/item.del.zpm"
#include "../Pixmaps/item.new.zpm"
#include "Box_paths.h"

void *cons_Box_paths();
void dest_Box_paths();
void *copy_Box_paths();

static void cb_action ___PROTO((c_CustomDialog	*cb_dialog,
				c_Box_paths	*box,
				int		cb_reason));

sf_Box_paths fc_Box_paths =
{
  cons_Box_paths,
  dest_Box_paths,
  copy_Box_paths
};

void *cons_Box_paths(w_Parent, pszTitle)
Widget		w_Parent;
char		*pszTitle;
{
  c_Box_paths	*ObjTmp;
  Arg		args[5]; 
  XmString	strTitre;
  Display	*display;
 
  if ((ObjTmp = (c_Box_paths *)Xc_malloc("Box_paths",
					  sizeof(c_Box_paths))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_Box_paths;

  ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							 pszTitle, 500, 400,
							 XcCD_HORIZONTAL
							 | XcCD_HRESIZE
							 | XcCD_VRESIZE);

  display = XtDisplay(WIDGET(ObjTmp->Dialog));

  F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog,
				     XcCD_OK | XcCD_CANCEL, XcCD_OK,
				     TRUE, (void (*)())cb_action, ObjTmp);

  F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);
  Xc_TRACE(("Object Box_paths build"));
  return ObjTmp;
}

void dest_Box_paths(This)
c_Box_paths *This;
{
  Xc_free(This);
  Xc_TRACE(("Object Box_paths destroyed"));
}

void *copy_Box_paths(This)
c_Box_paths *This;
{
  Xc_TRACE(("Copy not implemented"));
  return NULL;
}


static void cb_action(cb_dialog, box, cb_reason)
c_CustomDialog	*cb_dialog;
c_Box_paths	*box;
int		cb_reason;
{
/*
  if (cb_reason == XcCD_OK)
    F(box->list_save).change_all2(box->list_save);
  else
    Xc_TRACE(("canceled"));
*/
  F(cb_dialog).unmap(cb_dialog);

}

