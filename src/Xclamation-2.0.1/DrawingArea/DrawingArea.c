/*
** DrawingArea.c for Xclamation, XQuad and XAllWrite in DrawingArea/
** Object and method for DrawingArea
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
** Started on  Mon Jul 17 15:05:00 1994 Antoine Buat
** Last update Sat Jan  8 16:01:53 2000 Emmanuel Paris
*/

#include "DrawingArea.h"

void *cons_DrawingArea();
void dest_DrawingArea();
void *copy_DrawingArea();

sf_DrawingArea fc_DrawingArea =
{
  cons_DrawingArea,
  dest_DrawingArea,
  copy_DrawingArea
};

void *cons_DrawingArea(w_Parent, ManageW, Title, Width, Height)
Widget		w_Parent;
c_ManageWidget	*ManageW;
char		*Title;
int		Width;
int		Height;
{
  c_DrawingArea		*ObjTmp;
  Arg			argsw[4];
  int			nargs;

  if ((ObjTmp = (c_DrawingArea *)Xc_malloc("DrawingArea", 
					   sizeof(c_DrawingArea))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_DrawingArea;
  nargs = 0;
  XtSetArg(argsw[nargs], XmNwidth, Width);
  nargs++;
  XtSetArg(argsw[nargs], XmNheight, Height);
  nargs++;
  ObjTmp->w_This = XmCreateDrawingArea(w_Parent, Title, argsw, nargs);
  if (ManageW)
    {
      F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);
    }
  Xc_TRACE(("Object DrawingArea build"));
  return ObjTmp;
}

void dest_DrawingArea(This)
c_DrawingArea *This;
{
  XtDestroyWidget(This->w_This);
  Xc_free(This);
  Xc_TRACE(("Object DrawingArea destroyed"));
}

void *copy_DrawingArea(This)
c_DrawingArea *This;
{
  c_DrawingArea *ObjTmp;

  if ((ObjTmp = (c_DrawingArea *)Xc_malloc("CDrawingArea",
					   sizeof (c_DrawingArea))) == NULL)
    {
      Xc_FATAL(("Can't copy this object: memory allocation error"));
    }
  memcpy(ObjTmp, This, sizeof(c_DrawingArea));
  Xc_TRACE(("Object DrawingArea copied"));
  return ObjTmp;
}
