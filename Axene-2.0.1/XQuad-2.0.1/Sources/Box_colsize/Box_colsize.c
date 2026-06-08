/*
** Box_colsize.c for XQuad in Box_colsize/
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
** Started on  Mon Sep 18 17:12:50 1995 Robin Castanier
** Last update Sun Mar  9 21:45:05 1997 Robin Castanier
*/

#include "Box_colsize.h"

void *cons_Box_colsize();
void dest_Box_colsize();
void *copy_Box_colsize();

static void certify_col_size();
static void certify_row_size();
static void toggle_boolean();

sf_Box_colsize fc_Box_colsize =
{
  cons_Box_colsize,
  dest_Box_colsize,
  copy_Box_colsize
};

void *cons_Box_colsize(w_Parent, pszTitle,colrow)
Widget		w_Parent;
char		*pszTitle;
colrow_size_t	*colrow;
{
  Display	*display; 
  c_Box_colsize	*ObjTmp;
  char		str[10];

  if((ObjTmp = (c_Box_colsize *)Xc_malloc("Box_colsize",
					    sizeof(c_Box_colsize))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }

  ObjTmp->f = &fc_Box_colsize;
  ObjTmp->colrow = colrow;

  ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							 pszTitle, 280, 300,
							 XcCD_HORIZONTAL);
  ObjTmp->w_This = WIDGET(ObjTmp->Dialog);

  display = XtDisplay(WIDGET(ObjTmp->Dialog));
  ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				       MANAGER(ObjTmp->Dialog),
				       "gBoxColSize", FALSE);
  ObjTmp->Ltitre = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
			    MANAGER(ObjTmp->Dialog),
			    "lBoxColSizeTitle");
  ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->Dialog), 
					   "fCSColSize", 0);

  ObjTmp->Grid1 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame1),
				       MANAGER(ObjTmp->Dialog),
				       "gCSColSize", TRUE);
  ObjTmp->Lcolsize = (c_Label *)
    NEW(c_Label)(WIDGET(ObjTmp->Grid1),
		 MANAGER(ObjTmp->Dialog), "lCSColSize");
  sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(colrow->col_size));
  ObjTmp->WTcolsize = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid1),
				      MANAGER(ObjTmp->Dialog),
				      "tfCSColSize",
				      TF_REAL,
				      0.0, 100.0,
				      str, NULL, 6);
  F(ObjTmp->WTcolsize).set_unit(ObjTmp->WTcolsize, XcUB_LENGTH_CENTIMETER,
				"%.2f", TRUE, FALSE);
  F(ObjTmp->WTcolsize).add_callback(ObjTmp->WTcolsize, 
				    (WTextFieldProc)certify_col_size, ObjTmp);
  
  ObjTmp->TBcolsize = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid1),
					  MANAGER(ObjTmp->Dialog),
					  "tbCSColSize", NULL);
  F(ObjTmp->TBcolsize).Add_Callback(ObjTmp->TBcolsize,
				    XmNvalueChangedCallback,
				    toggle_boolean,
				    &(ObjTmp->colrow->change_col_size));

  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->Lcolsize), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->WTcolsize), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TBcolsize), XcG_MAX_SIZE,
			     NULL);

  ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->Dialog), 
					   "fCSRowSize", 0);

  ObjTmp->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame2),
				       MANAGER(ObjTmp->Dialog),
				       "gCSRowSize", TRUE);

  ObjTmp->Lrowsize = (c_Label *)
    NEW(c_Label)(WIDGET(ObjTmp->Grid2),
		 MANAGER(ObjTmp->Dialog), "lCSRowSize");
  sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(colrow->row_size));
  ObjTmp->WTrowsize = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid2),
				      MANAGER(ObjTmp->Dialog),
				      "tfCSRowSize",
				      TF_REAL,
				      0.0, 100.0,
				      str, NULL, 6);
  F(ObjTmp->WTrowsize).set_unit(ObjTmp->WTrowsize, XcUB_LENGTH_CENTIMETER,
				"%.2f", TRUE, FALSE); 
  F(ObjTmp->WTrowsize).add_callback(ObjTmp->WTrowsize, 
				    (WTextFieldProc)certify_row_size, ObjTmp);
  ObjTmp->TBrowsize = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid2),
					  MANAGER(ObjTmp->Dialog),
					  "tbCSRowSize", NULL);
  F(ObjTmp->TBrowsize).Add_Callback(ObjTmp->TBrowsize,
				    XmNvalueChangedCallback,
				    toggle_boolean,
				    &(ObjTmp->colrow->change_row_size));
 
  F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->Lrowsize), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->WTrowsize), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TBrowsize), XcG_MAX_SIZE,
			     NULL);


  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Ltitre), 30,
			    WIDGET(ObjTmp->Frame1), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Frame2), XcG_MAX_SIZE,
			    NULL);

  F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				     XcCD_OK | XcCD_CANCEL, 
				     XcCD_OK,
				     TRUE, NULL, NULL);
  
  F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);

  Xc_TRACE(("Object Box_colsize build"));
  return ObjTmp;
}

void dest_Box_colsize(This)
c_Box_colsize *This;
{
  DELETE(c_Label)(This->Ltitre);
  DELETE(c_Label)(This->Lcolsize);
  DELETE(c_Label)(This->Lrowsize);
  DELETE(c_WTextField)(This->WTcolsize);
  DELETE(c_WTextField)(This->WTrowsize);
  DELETE(c_ToggleButton)(This->TBcolsize);
  DELETE(c_ToggleButton)(This->TBrowsize);
  DELETE(c_Grid)(This->Grid1);
  DELETE(c_Grid)(This->Grid2);
  DELETE(c_Frame)(This->Frame1);
  DELETE(c_Frame)(This->Frame2);
  DELETE(c_Grid)(This->Grid);
  DELETE(c_CustomDialog)(This->Dialog);

  Xc_free(This);
  Xc_TRACE(("Object Box_colsize destroyed"));
}

void *copy_Box_colsize(This)
c_Box_colsize *This;
{
  Xc_TRACE(("Copy not implemented"));
  return NULL;
}

static void certify_col_size(this, Obj)
c_WTextField	*this;
c_Box_colsize	*Obj;
{
  coord_t	num;

  Xc_HISTORY(("column size"));
  
  num = F(this).get_value_in_unit(this);
  if (Obj->colrow->col_size != num)
  {
    Obj->colrow->col_size = num;
    if (!Obj->colrow->change_col_size)
      XmToggleButtonGadgetSetState(WIDGET(Obj->TBcolsize), True, True);
  }
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void certify_row_size(this, Obj)
c_WTextField	*this;
c_Box_colsize	*Obj;
{
  coord_t	num;

  Xc_HISTORY(("row size"));
  
  num = F(this).get_value_in_unit(this);
  if (Obj->colrow->row_size != num)
  {
    Obj->colrow->row_size = num;
    if (!Obj->colrow->change_row_size)
      XmToggleButtonGadgetSetState(WIDGET(Obj->TBrowsize), True, True);
  }
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void toggle_boolean(This, value, appel)
Widget	This;
boolean	*value;
caddr_t	appel;
{
  *value = !*value;
}




