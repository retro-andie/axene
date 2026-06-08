/*
** minimain.c for XAllWrite in Grid/
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
** Started on  Tue Dec  6 14:32:02 1994 Stéphane Boisson
** Last update Wed Dec  7 11:32:16 1994 Stéphane Boisson
*/

#include "Grid.h"
#include <Xm/Label.h>

#define BUTTON(name, parent) \
XtVaCreateManagedWidget((name), xmLabelWidgetClass, (parent), \
			XmNbackground, color++, NULL)

void main(argc, argv)
int argc;
char **argv;
{
  c_ManageWidget *manager;
  Widget wTopLevel, w;
  XtAppContext app;
  c_Grid *grid;
  int color = 2;
  
  manager = NEW(c_ManageWidget)();
  wTopLevel = XtVaAppInitialize(&app, "Xclamation", NULL, 0, &argc, argv,
                                NULL, NULL);
  XtVaSetValues(wTopLevel, XmNheight, 300, XmNwidth, 400, NULL);

  grid = NEW(c_Grid)(wTopLevel, manager, "grid", TRUE);
  F(grid).addColumn(grid, 0,
		    BUTTON("Bouton 1", WIDGET(grid)), -1,
		    BUTTON("Bouton 2", WIDGET(grid)), -1,
		    BUTTON("Bouton 3", WIDGET(grid)), -1,
		    NULL);
  F(grid).addColumn(grid, -1,
		    BUTTON("Bouton 4", WIDGET(grid)), -1,
		    BUTTON("Bouton 4", WIDGET(grid)), -1,
		    BUTTON("Bouton 5", WIDGET(grid)), -1,
		    NULL);
  F(grid).addColumn(grid, 0,
		    BUTTON("Bouton 4", WIDGET(grid)), -1,
		    BUTTON("Bouton 4", WIDGET(grid)), -1,
		    BUTTON("Bouton 5", WIDGET(grid)), -1,
		    NULL);

  F(manager).Manage_all(manager, NULL);
  XtPopup(wTopLevel, XtGrabNone);
  XtAppMainLoop(app);
}



