/*
** Box_gridSetup.c for Xclamation in Box_gridSetup/
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
** Started on  Fri Oct  2 19:13:14 1998 Robin Castanier
** Last update Mon Oct  5 14:33:03 1998 Robin Castanier
*/

#include "Box_gridSetup.h"
#include "Box_document.h"

extern new_doc_s GlobNewDocInfo;

void *cons_Box_gridSetup();
void dest_Box_gridSetup();
void callback_dest_GridSetup();
static void certify_orig_x();
static void certify_orig_y();
static void certify_spacing_x();
static void certify_spacing_y();

sf_Box_gridSetup fc_Box_gridSetup =
{
 cons_Box_gridSetup,
 dest_Box_gridSetup,
};

void *cons_Box_gridSetup(w_Parent, pszTitle)
Widget w_Parent;
char   *pszTitle;
{
 Display          *display;
 c_Box_gridSetup  *ObjTmp;
 char             str[10];

 if ((ObjTmp = (c_Box_gridSetup *)Xc_malloc("Box_gridSetup",
					    sizeof(c_Box_gridSetup))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Box_gridSetup;
 
 ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, 320, 320,
							XcCD_HORIZONTAL
							| XcCD_HRESIZE
							| XcCD_VRESIZE);
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);
 display = XtDisplay(w_Parent);

 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gGridSetup", FALSE);
  
 ObjTmp->lTitle = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
			  MANAGER(ObjTmp->Dialog),
			  "lBoxGridSetupTitle");

 ObjTmp->FrameH = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fGridSetupH", 0);

 ObjTmp->GridH = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->FrameH),
				       MANAGER(ObjTmp->Dialog),
				       "gGridSetupH", FALSE);
  
 ObjTmp->FrameV = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fGridSetupV", 0);

 ObjTmp->GridV = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->FrameV),
				       MANAGER(ObjTmp->Dialog),
				       "gGridSetupH", FALSE);
  
 ObjTmp->lOrigX = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridH),
			  MANAGER(ObjTmp->Dialog),
			  "lOrigX");

 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(GlobNewDocInfo.g_orx));
 ObjTmp->tfOrigX = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridH),
				    MANAGER(ObjTmp->Dialog),
				    "tfOrigX",
				    TF_REAL,
				    -10.0, 10.0,
				    str, NULL, 7);
 F(ObjTmp->tfOrigX).set_unit(ObjTmp->tfOrigX,
				XcUB_LENGTH_CENTIMETER, "%.2f", TRUE, FALSE);
 F(ObjTmp->tfOrigX).add_callback(ObjTmp->tfOrigX,
				 (WTextFieldProc)certify_orig_x, ObjTmp);

 ObjTmp->lOrigY = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridV),
			  MANAGER(ObjTmp->Dialog),
			  "lOrigY");

 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(GlobNewDocInfo.g_ory));
 ObjTmp->tfOrigY = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridV),
				    MANAGER(ObjTmp->Dialog),
				    "tfOrigY",
				    TF_REAL,
				    -10.0, 10.0,
				    str, NULL, 7);
 F(ObjTmp->tfOrigY).set_unit(ObjTmp->tfOrigY,
				XcUB_LENGTH_CENTIMETER, "%.2f", TRUE, FALSE);
 F(ObjTmp->tfOrigY).add_callback(ObjTmp->tfOrigY,
				 (WTextFieldProc)certify_orig_y, ObjTmp);

 ObjTmp->lSpacingX = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridH),
			  MANAGER(ObjTmp->Dialog),
			  "lSpacingX");

 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(GlobNewDocInfo.g_dtx));
 ObjTmp->tfSpacingX = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridH),
				    MANAGER(ObjTmp->Dialog),
				    "tfSpacingX",
				    TF_REAL,
				    0.1, 10.0,
				    str, NULL, 7);
 F(ObjTmp->tfSpacingX).set_unit(ObjTmp->tfSpacingX,
				XcUB_LENGTH_CENTIMETER, "%.2f", TRUE, FALSE);
 F(ObjTmp->tfSpacingX).add_callback(ObjTmp->tfSpacingX,
				    (WTextFieldProc)certify_spacing_x, ObjTmp);
 ObjTmp->lSpacingY = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridV),
			  MANAGER(ObjTmp->Dialog),
			  "lSpacingY");

 sprintf(str, "%.2f", SCALE_TO_CENTIMETERS(GlobNewDocInfo.g_dty));
 ObjTmp->tfSpacingY = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->GridV),
				    MANAGER(ObjTmp->Dialog),
				    "tfSpacingY",
				    TF_INTEGER,
				    0.1, 10.0,
				    str, NULL, 7);
 F(ObjTmp->tfSpacingY).set_unit(ObjTmp->tfSpacingY,
				XcUB_LENGTH_CENTIMETER, "%.2f", TRUE, FALSE);
 F(ObjTmp->tfSpacingY).add_callback(ObjTmp->tfSpacingY,
				    (WTextFieldProc)certify_spacing_y, ObjTmp);

 F(ObjTmp->GridH).addColumn(ObjTmp->GridH, XcG_MAX_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lOrigX), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lSpacingX), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->GridH).addColumn(ObjTmp->GridH, XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->tfOrigX), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->tfSpacingX), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);

 F(ObjTmp->GridV).addColumn(ObjTmp->GridV, XcG_MAX_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lOrigY), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lSpacingY), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->GridV).addColumn(ObjTmp->GridV, XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->tfOrigY), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->tfSpacingY), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);

 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->lTitle), 30,
			   WIDGET(ObjTmp->FrameH), XcG_MAX_SIZE,
			   WIDGET(ObjTmp->FrameV), XcG_MAX_SIZE,
			   NULL);

 F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				    XcCD_OK | XcCD_CANCEL,
				    XcCD_OK,
				    TRUE, NULL, NULL);
  
 F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);
  
 Xc_TRACE(("Object Box_gridSetup build"));
 return ObjTmp;
}

void dest_Box_gridSetup(This)
c_Box_gridSetup *This;
{
 DELETE(c_Label)(This->lOrigX);
 DELETE(c_Label)(This->lOrigY);
 DELETE(c_Label)(This->lSpacingX);
 DELETE(c_Label)(This->lSpacingY);
 DELETE(c_WTextField)(This->tfOrigX);
 DELETE(c_WTextField)(This->tfOrigY);
 DELETE(c_WTextField)(This->tfSpacingX);
 DELETE(c_WTextField)(This->tfSpacingY);
 DELETE(c_Label)(This->lTitle);
 DELETE(c_Grid)(This->GridH);
 DELETE(c_Grid)(This->GridV);
 DELETE(c_Frame)(This->FrameH);
 DELETE(c_Frame)(This->FrameV);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_gridSetup destroyed"));
}

void callback_dest_GridSetup(wid, This)
Widget	    wid;
c_Box_gridSetup *This;
{
 DELETE(c_Box_gridSetup)(This);
}


static void certify_orig_x(this, Obj)
c_WTextField	*this;
c_Box_gridSetup	*Obj;
{
 coord_t	num;

 num = F(this).get_value_in_unit(this);
 GlobNewDocInfo.g_orx = num;
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void certify_orig_y(this, Obj)
c_WTextField	*this;
c_Box_gridSetup	*Obj;
{
 coord_t	num;

 num = F(this).get_value_in_unit(this);
 GlobNewDocInfo.g_ory = num;
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void certify_spacing_x(this, Obj)
c_WTextField	*this;
c_Box_gridSetup	*Obj;
{
 coord_t	num;

 num = F(this).get_value_in_unit(this);
 GlobNewDocInfo.g_dtx = num;
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void certify_spacing_y(this, Obj)
c_WTextField	*this;
c_Box_gridSetup	*Obj;
{
 coord_t	num;

 num = F(this).get_value_in_unit(this);
 GlobNewDocInfo.g_dty = num;
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}





