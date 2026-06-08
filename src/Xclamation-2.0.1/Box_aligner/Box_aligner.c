/*
** Box_aligner.c for Xclamation in Box_aligner/
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
** Started on  Mon Jul 17 13:55:49 1995 Robin Castanier
** Last update Mon Jun 23 10:20:41 1997 Robin Castanier
*/

#include "Box_aligner.h"
#include "../Pixmaps/aligner.vertical.gauche.zpm"
#include "../Pixmaps/aligner.vertical.centre.zpm"
#include "../Pixmaps/aligner.vertical.droite.zpm"
#include "../Pixmaps/aligner.vertical.block.zpm"
#include "../Pixmaps/aligner.horizontal.haut.zpm"
#include "../Pixmaps/aligner.horizontal.centre.zpm"
#include "../Pixmaps/aligner.horizontal.bas.zpm"
#include "../Pixmaps/aligner.horizontal.block.zpm"

void *cons_Box_aligner();
void dest_Box_aligner();
void *copy_Box_aligner();

static void certify_offH ___PROTO((c_WTextField *this,
				   c_Box_aligner *Obj));
static void certify_offV ___PROTO((c_WTextField *this,
				   c_Box_aligner *Obj));
/*static void distriH ___PROTO((Widget *wthis,
			      c_Box_aligner *Obj));
static void distriV ___PROTO((Widget *wthis,
			      c_Box_aligner *Obj)); 
 PROTO NE CORRESPONDANT PAS A LA FONCTION */
 

static void distriH();
static void distriV();
static void change1();

sf_Box_aligner fc_Box_aligner =
{
  cons_Box_aligner,
  dest_Box_aligner,
  copy_Box_aligner
};

void *cons_Box_aligner(w_Parent, pszTitle, param, nbr)
Widget		w_Parent;
char		*pszTitle;
align_param_t	*param;
int		nbr;
{
  Arg	        args[2]; 
  Display	*display; 
  c_Box_aligner	*ObjTmp;
  
  if((ObjTmp = (c_Box_aligner *)Xc_malloc("Box_align",
					    sizeof(c_Box_aligner))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }

  ObjTmp->f = &fc_Box_aligner;
  ObjTmp->param = param;
  ObjTmp->param->horizontal = FALSE;
  ObjTmp->param->vertical = FALSE;
  ObjTmp->param->type_h = ALIGN_END_NUM;
  ObjTmp->param->type_v = ALIGN_END_NUM;
  ObjTmp->param->distribution_h = FALSE;
  ObjTmp->param->distribution_v = FALSE;
  ObjTmp->param->offset_h = 0;
  ObjTmp->param->offset_v = 0;
  Xc_TRACE(("%d", nbr));
  if (nbr > 2)
    ObjTmp->TrueBool = True;
  else
    ObjTmp->TrueBool = False;

  ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							 pszTitle, 400, 300,
							 XcCD_HORIZONTAL );
  ObjTmp->w_This = WIDGET(ObjTmp->Dialog);

  display = XtDisplay(WIDGET(ObjTmp->Dialog));
  ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				       MANAGER(ObjTmp->Dialog),
				       "gBoxAlign", FALSE);
  ObjTmp->Ltitre = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
			    MANAGER(ObjTmp->Dialog),
			    "lBoxAlignTitle");
  ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->Dialog), 
					   "fAlignH", 0);

  ObjTmp->Grid1 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame1),
				       MANAGER(ObjTmp->Dialog),
				       "gBAAlignH", TRUE);

  ObjTmp->RowH = (c_RowColumn *)NEW(c_RowColumn)(WIDGET(ObjTmp->Grid1),
						 MANAGER(ObjTmp->Dialog),
						 "rcAlignH",
						 32, 32, XmHORIZONTAL);
  XtSetArg(args[0], XmNradioAlwaysOne, False);
  XtSetArg(args[1], XmNradioBehavior, True);
  XtSetValues(WIDGET(ObjTmp->RowH), args, 2);
  
  ObjTmp->Icon1H = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->RowH),
					 MANAGER(ObjTmp->Dialog),
					 "BALIcon1H", display,
					 aligner_vertical_gauche_xpm,
					 NULL, XcToggle);
  CALLBACK(WIDGET(ObjTmp->Icon1H), FUNC_VALUE, change1, ObjTmp);
  ObjTmp->Icon2H = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->RowH),
					 MANAGER(ObjTmp->Dialog),
					 "BALIcon2H", display,
					 aligner_vertical_centre_xpm, 
					 NULL, XcToggle);
  CALLBACK(WIDGET(ObjTmp->Icon2H), FUNC_VALUE, change1, ObjTmp);
  ObjTmp->Icon3H = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->RowH),
					 MANAGER(ObjTmp->Dialog),
					 "BALIcon3H", display,
					 aligner_vertical_droite_xpm,
					 NULL, XcToggle);
  CALLBACK(WIDGET(ObjTmp->Icon3H), FUNC_VALUE, change1, ObjTmp);
  ObjTmp->Icon4H = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->RowH),
					 MANAGER(ObjTmp->Dialog),
					 "BALIcon4H", display,
					 aligner_vertical_block_xpm,
					 NULL, XcToggle);
  CALLBACK(WIDGET(ObjTmp->Icon4H), FUNC_VALUE, change1, ObjTmp);
  ObjTmp->LespacementH = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
			    MANAGER(ObjTmp->Dialog),
			    "lBASpacingH");
  
  ObjTmp->WToffsetH = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid1),
				      MANAGER(ObjTmp->Dialog),
				      "tfBASpacingH",
				      TF_REAL,
				      -500.0, 500.0,
				      "0", NULL, 6);
  F(ObjTmp->WToffsetH).set_unit(ObjTmp->WToffsetH, XcUB_LENGTH_MILLIMETER,
				"%.2f", TRUE, FALSE);
  F(ObjTmp->WToffsetH).add_callback(ObjTmp->WToffsetH,
				    (WTextFieldProc)certify_offH, ObjTmp);

  ObjTmp->TBdistriH = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid1),
					  MANAGER(ObjTmp->Dialog),
					  "tbDistributionH", NULL);

  XtVaSetValues(WIDGET(ObjTmp->TBdistriH),
		XmNrecomputeSize, False,
		NULL);
 
  F(ObjTmp->TBdistriH).Add_Callback(ObjTmp->TBdistriH,
				    XmNvalueChangedCallback,
				    distriH,
				    ObjTmp);

  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, 36,
			     WIDGET(ObjTmp->RowH), 38*4,
			     WIDGET(ObjTmp->LespacementH), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->WToffsetH), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TBdistriH), XcG_MAX_SIZE,
			     NULL);

  ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->Dialog), 
					   "fAlignV", 0);

  ObjTmp->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame2),
				       MANAGER(ObjTmp->Dialog),
				       "gBAAlignV", TRUE);

  ObjTmp->RowV = (c_RowColumn *)NEW(c_RowColumn)(WIDGET(ObjTmp->Grid2),
						 MANAGER(ObjTmp->Dialog),
						 "rcAlignV",
						 32, 32, XmHORIZONTAL);
  XtSetArg(args[0], XmNradioAlwaysOne, False);
  XtSetArg(args[1], XmNradioBehavior, True);
  XtSetValues(WIDGET(ObjTmp->RowV), args, 2);

  ObjTmp->Icon1V = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->RowV),
					 MANAGER(ObjTmp->Dialog),
					 "BALIcon1V", display,
					 aligner_horizontal_haut_xpm,
					 NULL, XcToggle);
  CALLBACK(WIDGET(ObjTmp->Icon1V), FUNC_VALUE, change1, ObjTmp);
  ObjTmp->Icon2V = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->RowV),
					 MANAGER(ObjTmp->Dialog),
					 "BALIcon2V", display,
					 aligner_horizontal_centre_xpm, 
					 NULL, XcToggle);
  CALLBACK(WIDGET(ObjTmp->Icon2V), FUNC_VALUE, change1, ObjTmp);
  ObjTmp->Icon3V = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->RowV),
					 MANAGER(ObjTmp->Dialog),
					 "BALIcon3V", display,
					 aligner_horizontal_bas_xpm,
					 NULL, XcToggle);
  CALLBACK(WIDGET(ObjTmp->Icon3V), FUNC_VALUE, change1, ObjTmp);
  ObjTmp->Icon4V = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->RowV),
					 MANAGER(ObjTmp->Dialog),
					 "BALIcon4V", display,
					 aligner_horizontal_block_xpm,
					 NULL, XcToggle);
  CALLBACK(WIDGET(ObjTmp->Icon4V), FUNC_VALUE, change1, ObjTmp);
  
  ObjTmp->LespacementV = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
			    MANAGER(ObjTmp->Dialog),
			    "lBASpacingV");

  ObjTmp->WToffsetV = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid2),
				      MANAGER(ObjTmp->Dialog),
				      "tfBASpacingV",
				      TF_REAL,
				      -500.0, 500.0,
				      "0", NULL, 6);
  F(ObjTmp->WToffsetV).set_unit(ObjTmp->WToffsetV, XcUB_LENGTH_MILLIMETER,
				"%.2f", TRUE, FALSE);
  F(ObjTmp->WToffsetV).add_callback(ObjTmp->WToffsetV,
				    (WTextFieldProc)certify_offV, ObjTmp);

  ObjTmp->TBdistriV = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid2),
					  MANAGER(ObjTmp->Dialog),
					  "tbDistributionV", NULL);
 
  XtVaSetValues(WIDGET(ObjTmp->TBdistriV),
		XmNrecomputeSize, False,
		NULL);

  F(ObjTmp->TBdistriV).Add_Callback(ObjTmp->TBdistriV,
				    XmNvalueChangedCallback,
				    distriV,
				    ObjTmp);

  F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, 36,
			     WIDGET(ObjTmp->RowV), 38*4,
			     WIDGET(ObjTmp->LespacementV), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->WToffsetV), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TBdistriV), XcG_MAX_SIZE,
			     NULL);

  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Ltitre), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->Frame1), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Frame2), XcG_MAX_SIZE,
			    NULL);

  F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				     XcCD_OK | XcCD_CANCEL, 
				     XcCD_OK,
				     TRUE, NULL, NULL);
  
  XtSetSensitive(WIDGET(ObjTmp->TBdistriH), False);
  XtSetSensitive(WIDGET(ObjTmp->TBdistriV), False);
  F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);

  Xc_TRACE(("Object Box_align build"));
  return ObjTmp;
}

void dest_Box_aligner(This)
c_Box_aligner *This;
{
  DELETE(c_ToggleButton)(This->TBdistriV);
  DELETE(c_WTextField)(This->WToffsetV);
  DELETE(c_Label)(This->LespacementV);
  DELETE(c_Icon)(This->Icon1V);
  DELETE(c_Icon)(This->Icon2V);
  DELETE(c_Icon)(This->Icon3V);
  DELETE(c_Icon)(This->Icon4V);
  DELETE(c_RowColumn)(This->RowV);
  DELETE(c_Grid)(This->Grid2);
  DELETE(c_Frame)(This->Frame2);
  
  DELETE(c_ToggleButton)(This->TBdistriH);
  DELETE(c_WTextField)(This->WToffsetH);
  DELETE(c_Label)(This->LespacementH);
  DELETE(c_Icon)(This->Icon1H);
  DELETE(c_Icon)(This->Icon2H);
  DELETE(c_Icon)(This->Icon3H);
  DELETE(c_Icon)(This->Icon4H);
  DELETE(c_RowColumn)(This->RowH);
  DELETE(c_Grid)(This->Grid1);
  DELETE(c_Frame)(This->Frame1);
  
  DELETE(c_Label)(This->Ltitre);
  DELETE(c_Grid)(This->Grid);
  DELETE(c_CustomDialog)(This->Dialog);

  Xc_free(This);
  Xc_TRACE(("Object Box_align destroyed"));
}

void *copy_Box_aligner(This)
c_Box_aligner *This;
{
  Xc_TRACE(("Copy not implemented"));
  return NULL;
}

static void certify_offH(this, Obj)
c_WTextField	*this;
c_Box_aligner	*Obj;
{
  coord_t	num;

  Xc_TRACE(("offset_H"));

  num = F(this).get_value_in_unit(this);
  Obj->param->offset_h = num;
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void certify_offV(this, Obj)
c_WTextField	*this;
c_Box_aligner	*Obj;
{
  coord_t	num;

  Xc_TRACE(("offset_V"));

  num = F(this).get_value_in_unit(this);
  Obj->param->offset_v = num;
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void distriH(wthis, Obj, cbs)
Widget				wthis;
c_Box_aligner			*Obj;
XmToggleButtonCallbackStruct	*cbs;
{
  Xc_TRACE(("distriH"));

  if (cbs->set == True)
    {
      XtSetSensitive(WIDGET(Obj->WToffsetH), False);
      XtSetSensitive(WIDGET(Obj->LespacementH), False);
      Obj->param->distribution_h = TRUE;
    }
  else
    {
      XtSetSensitive(WIDGET(Obj->WToffsetH), True);
      XtSetSensitive(WIDGET(Obj->LespacementH), True);
      Obj->param->distribution_h = FALSE;
    }
}

static void distriV(wthis, Obj, cbs)
Widget				wthis;
c_Box_aligner			*Obj;
XmToggleButtonCallbackStruct	*cbs;
{
  Xc_TRACE(("distriV"));

  if (cbs->set == True)
    {
      XtSetSensitive(WIDGET(Obj->WToffsetV), False);
      XtSetSensitive(WIDGET(Obj->LespacementV), False);
      Obj->param->distribution_v = TRUE;
    }
  else
    {
      XtSetSensitive(WIDGET(Obj->WToffsetV), True);
      XtSetSensitive(WIDGET(Obj->LespacementV), True);
      Obj->param->distribution_v = FALSE;
    }
}

static void change1(w, Obj)
Widget		w;
c_Box_aligner	*Obj;
{
  Xc_HISTORY(("change"));
  
  if (w == WIDGET(Obj->Icon1H))
    {
      Xc_HISTORY(("change_hl"));
      if (Obj->param->type_h == ALIGN_LEFT)
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriH), False);
	  Obj->param->type_h = ALIGN_END_NUM;
	  Obj->param->horizontal = FALSE;
	}
      else
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriH), Obj->TrueBool);
	  Obj->param->type_h = ALIGN_LEFT;
	  Obj->param->horizontal = TRUE;
	}
      return;
    }
  if (w == WIDGET(Obj->Icon2H))
    {
      Xc_HISTORY(("change_hc"));
      if (Obj->param->type_h == ALIGN_CENTER)
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriH), False);
	  Obj->param->type_h = ALIGN_END_NUM;
	  Obj->param->horizontal = FALSE;
	}
      else
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriH), Obj->TrueBool);
	  Obj->param->type_h = ALIGN_CENTER;
	  Obj->param->horizontal = TRUE;
	}
      return;
    }
  if (w == WIDGET(Obj->Icon3H))
    {
      Xc_HISTORY(("change_hr"));
      if (Obj->param->type_h == ALIGN_RIGHT)
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriH), False);
	  Obj->param->type_h = ALIGN_END_NUM;
	  Obj->param->horizontal = FALSE;
	}
      else
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriH), Obj->TrueBool);
	  Obj->param->type_h = ALIGN_RIGHT;
	  Obj->param->horizontal = TRUE;
	}
      return;
    }
  if (w == WIDGET(Obj->Icon4H))
    {
      Xc_HISTORY(("change_hb"));
      if (Obj->param->type_h == ALIGN_BLOCK)
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriH), False);
	  Obj->param->type_h = ALIGN_END_NUM;
	  Obj->param->horizontal = FALSE;
	}
      else
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriH), Obj->TrueBool);
	  Obj->param->type_h = ALIGN_BLOCK;
	  Obj->param->horizontal = TRUE;
	}
      return;
    }
  if (w == WIDGET(Obj->Icon1V))
    {
      Xc_HISTORY(("change_vt"));
      if (Obj->param->type_v == ALIGN_TOP)
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriV), False);
	  Obj->param->type_v = ALIGN_END_NUM;
	  Obj->param->vertical = FALSE;
	}
      else
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriV), Obj->TrueBool);
	  Obj->param->type_v = ALIGN_TOP;
	  Obj->param->vertical = TRUE;
	}
      return;
    }
  if (w == WIDGET(Obj->Icon2V))
    {
      Xc_HISTORY(("change_vc"));
      if (Obj->param->type_v == ALIGN_CENTER)
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriV), False);
	  Obj->param->type_v = ALIGN_END_NUM;
	  Obj->param->vertical = FALSE;
	}
      else
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriV), Obj->TrueBool);
	  Obj->param->type_v = ALIGN_CENTER;
	  Obj->param->vertical = TRUE;
	}
      return;
    }
  if(w == WIDGET(Obj->Icon3V))
    {
      Xc_HISTORY(("change_vb"));
      if (Obj->param->type_v == ALIGN_BOTTOM)
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriV), False);
	  Obj->param->type_v = ALIGN_END_NUM;
	  Obj->param->vertical = FALSE;
	}
      else
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriV), Obj->TrueBool);
	  Obj->param->type_v = ALIGN_BOTTOM;
	  Obj->param->vertical = TRUE;
	}
      return;
    }  
  if(w == WIDGET(Obj->Icon4V))
    {
      Xc_HISTORY(("change_vbl"));
      if (Obj->param->type_v == ALIGN_BLOCK)
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriV), False);
	  Obj->param->type_v = ALIGN_END_NUM;
	  Obj->param->vertical = FALSE;
	}
      else
	{
	  XtSetSensitive(WIDGET(Obj->TBdistriV), Obj->TrueBool);
	  Obj->param->type_v = ALIGN_BLOCK;
	  Obj->param->vertical = TRUE;
	}
      return;
    }  
}
