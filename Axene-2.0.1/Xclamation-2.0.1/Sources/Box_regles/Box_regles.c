/*
** Box_regles.c for Xclamation in Box_regles/
** Object and method for Box_regles Class
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
** Started on  Mon Dec 26 15:17:18 1994 Robin Castanier
** Last update Sat Jan 22 15:24:18 2000 Emmanuel Paris
*/

#include "../Pixmaps/texte.mode.align.zpm"
#include "../Pixmaps/texte.mode.center.zpm"
#include "../Pixmaps/texte.mode.droite.zpm"
#include "../Pixmaps/texte.mode.gauche.zpm"
#include "Box_regles.h"
#include "TextRuler.h"

void *cons_Box_regles();
void dest_Box_regles();
void *copy_Box_regles();

static void Add_Item_R();
static void Del_SItem();
static void Change_Sel();

static void certify_alinea ___PROTO((c_WTextField *this,
				     c_Box_regles *Obj));
static void certify_margeG ___PROTO((c_WTextField *this,
				     c_Box_regles *Obj));
static void certify_margeD ___PROTO((c_WTextField *this,
				    c_Box_regles *Obj));
static void certify_interlig ___PROTO((c_WTextField *this,
				    c_Box_regles *Obj));
static void certify_interpar ___PROTO((c_WTextField *this,
				    c_Box_regles *Obj));
static void certify_distri ___PROTO((c_WTextField *this,
				    c_Box_regles *Obj));
static void certify_ratioMin ___PROTO((c_WTextField *this,
				    c_Box_regles *Obj));
static void certify_ratioMax ___PROTO((c_WTextField *this,
				    c_Box_regles *Obj));
static void Set_param();
static void arm_left();
static void arm_right();
static void arm_center();
static void arm_justify();
static void cb_action ___PROTO((c_CustomDialog	*cb_dialog,
				c_Box_regles	*box,
				int		cb_reason));
static void cb_change ___PROTO((c_Box_regles	*box,
				c_TextRuler	*ruler,
				int		type,
				int		num,
				void 		*data));

sf_Box_regles fc_Box_regles =
{
  cons_Box_regles,
  dest_Box_regles,
  copy_Box_regles
};

void *cons_Box_regles(w_Parent, pszTitle, ruler_base)
Widget	    w_Parent;
char	    *pszTitle;
c_TextRuler **ruler_base;
{
  c_Box_regles	*ObjTmp;
  Arg	        args[5]; 
  XmString      strTitre;
  Display	*display;
 
  if ((ObjTmp = (c_Box_regles *)Xc_malloc("Box_rulers",
					  sizeof(c_Box_regles))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_Box_regles;


  ObjTmp->ruler_base = ruler_base;
  ObjTmp->v_ruler_base = NULL;
  ObjTmp->list_save = (c_ModifList *)
    NEW(c_ModifList)(ObjTmp, cb_change,
		     11,
		     (int)(offsetof(c_TextRuler, left_margin)),
		     (int)sizeof(coord_t),
		     (int)(offsetof(c_TextRuler, right_margin)),
		     (int)sizeof(coord_t),
		     (int)(offsetof(c_TextRuler, paragraph_indent)),
		     (int)sizeof(coord_t),
		     (int)(offsetof(c_TextRuler, inter_mode)),
		     (int)sizeof(boolean),
		     (int)(offsetof(c_TextRuler, inter_line)),
		     (int)sizeof(coord_t),
		     (int)(offsetof(c_TextRuler, inter_paragraph)),
		     (int)sizeof(coord_t),
		     (int)(offsetof(c_TextRuler, justification)),
		     (int)sizeof(textruler_just_t),
		     (int)(offsetof(c_TextRuler, word_distribution)),
		     (int)sizeof(scale_t),
		     (int)(offsetof(c_TextRuler, min_ratio)),
		     (int)sizeof(scale_t),
		     (int)(offsetof(c_TextRuler, max_ratio)),
		     (int)sizeof(scale_t),
		     (int)(offsetof(c_TextRuler, name)),
		     (int)XcMAX_NAME_LENGTH);
  
  ObjTmp->PBox = (c_ASLBox *)NEW(c_ASLBox)(w_Parent,
					   pszTitle,
					   675, 450,
					   FALSE, 
					   offsetof(c_TextRuler, name));

  display = XtDisplay(WIDGET(ObjTmp->PBox));

  strTitre = F(GlobResources).getXmString(GlobResources, XcR_editRulers);
  XtSetArg(args[0], XmNlabelString, strTitre);
  XtSetValues(WIDGET(ObjTmp->PBox->Label), args, 1);
  XmStringFree(strTitre);
  
  F(ObjTmp->PBox).setCallback(ObjTmp->PBox, Change_Sel,
			      ObjTmp);

  F(ObjTmp->PBox).Add_Buttons(ObjTmp->PBox, "lParagraphAdd", Add_Item_R,
			      ObjTmp,
			      NULL);
  F(ObjTmp->PBox).Add_Buttons(ObjTmp->PBox, "lParagraphDel", Del_SItem,
			      ObjTmp,
			      NULL);

  if (!ruler_base[0])
    NEW(c_TextRuler)(ruler_base, "New_ruler");
  
  WIDGET(ObjTmp) = WIDGET(ObjTmp->PBox);
  ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				       MANAGER(ObjTmp->PBox->Dialog),
				       "gBoxRuler", FALSE);
  ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->PBox->Dialog), 
					   "fBRMargin", 0);
  ObjTmp->Grid1 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame1),
					MANAGER(ObjTmp->PBox->Dialog),
					"gBFMargin", TRUE);
  ObjTmp->Lalinea = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
			    MANAGER(ObjTmp->PBox->Dialog),
			    "lBRAlinea");
  ObjTmp->LmargeG = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
			    MANAGER(ObjTmp->PBox->Dialog),
			    "lBRMarginL");
  ObjTmp->LmargeD = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
			    MANAGER(ObjTmp->PBox->Dialog),
			    "lBRMarginR");
  ObjTmp->TFalinea = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid1),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfBRAlinea",
				      TF_REAL,
				      -500.0, 500.0,
				      "0", NULL, 5);
  F(ObjTmp->TFalinea).set_unit(ObjTmp->TFalinea, XcUB_LENGTH_MILLIMETER,
			       "%.2f", TRUE, FALSE);
  F(ObjTmp->TFalinea).add_callback(ObjTmp->TFalinea,
				   (WTextFieldProc)certify_alinea, ObjTmp);

  ObjTmp->TFmargeG = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid1),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfBRMarginL",
				      TF_REAL,
				      0.0, 500.0,
				      "0", NULL, 5);
  F(ObjTmp->TFmargeG).set_unit(ObjTmp->TFmargeG, XcUB_LENGTH_MILLIMETER,
			       "%.2f", TRUE, FALSE);
  F(ObjTmp->TFmargeG).add_callback(ObjTmp->TFmargeG,
				   (WTextFieldProc)certify_margeG, ObjTmp);
  
  ObjTmp->TFmargeD = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid1),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfBRMarginR",
				      TF_REAL,
				      0.0, 500.0,
				      "0", NULL, 5);
  F(ObjTmp->TFmargeD).set_unit(ObjTmp->TFmargeD, XcUB_LENGTH_MILLIMETER,
			       "%.2f", TRUE, FALSE);
  F(ObjTmp->TFmargeD).add_callback(ObjTmp->TFmargeD,
				   (WTextFieldProc)certify_margeD, ObjTmp);
  
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_BEST_SIZE,
			     WIDGET(ObjTmp->LmargeG), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TFmargeG), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_BEST_SIZE,
			     WIDGET(ObjTmp->Lalinea), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TFalinea), XcG_BEST_SIZE,
			     WIDGET(ObjTmp->LmargeD), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TFmargeD), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);

  
  ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->PBox->Dialog), 
					   "fBRSpacing", 0);
  ObjTmp->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame2),
					MANAGER(ObjTmp->PBox->Dialog),
					"gBRSpacing", TRUE);
  ObjTmp->Tinterligne = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid2),
				  MANAGER(ObjTmp->PBox->Dialog),
				  "tbRelativInterline", NULL);

  F(ObjTmp->Tinterligne).Add_Callback(ObjTmp->Tinterligne,
				      XmNvalueChangedCallback,
				      Set_param,
				      ObjTmp);

  ObjTmp->Linterlig = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
			    MANAGER(ObjTmp->PBox->Dialog),
			    "lInterLine");
  ObjTmp->Linterpar = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
			    MANAGER(ObjTmp->PBox->Dialog),
			    "lInterParagraph");
  ObjTmp->TFinterlig = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid2),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfInterLine",
				      TF_INTEGER,
				      0.0, 1000.0,
				      "5", NULL, 5);
  F(ObjTmp->TFinterlig).set_unit(ObjTmp->TFinterlig, XcUB_LENGTH_POINT,
				NULL, TRUE, FALSE);
  F(ObjTmp->TFinterlig).add_callback(ObjTmp->TFinterlig,
				     (WTextFieldProc)certify_interlig, ObjTmp);
  ObjTmp->TFinterpar = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid2),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfInterParagraph",
				      TF_INTEGER,
				      0.0, 1000.0,
				      "5", NULL, 5);
  F(ObjTmp->TFinterpar).set_unit(ObjTmp->TFinterpar, XcUB_LENGTH_POINT,
				NULL, TRUE, FALSE);
  F(ObjTmp->TFinterpar).add_callback(ObjTmp->TFinterpar,
				     (WTextFieldProc)certify_interpar, ObjTmp);
  
  F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_MAX_SIZE, NULL);
  F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_BEST_SIZE,
			     WIDGET(ObjTmp->Tinterligne), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_MAX_SIZE, NULL);
  F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_BEST_SIZE,
			     WIDGET(ObjTmp->Linterpar), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TFinterpar), XcG_BEST_SIZE,
			     WIDGET(ObjTmp->Linterlig), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TFinterlig), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_MAX_SIZE, NULL);

  ObjTmp->Frame3 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->PBox->Dialog), 
					   "fJustify", 0);

  ObjTmp->Row1 = (c_RowColumn *)NEW(c_RowColumn)(WIDGET(ObjTmp->Frame3),
						 MANAGER(ObjTmp->PBox->Dialog),
						 "gJustify", 10, 10,
						 XmHORIZONTAL);
  
  XtSetArg(args[0], XmNradioBehavior, True);
  XtSetArg(args[1], XmNradioAlwaysOne, True);
  XtSetValues(WIDGET(ObjTmp->Row1), args, 2);

  ObjTmp->Icon1 = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Row1),
					MANAGER(ObjTmp->PBox->Dialog),
					"BRIcon1", display,
					texte_mode_gauche_xpm, NULL, XcToggle);
  XtAddCallback(WIDGET(ObjTmp->Icon1),
	        XmNarmCallback, arm_left, ObjTmp);
  ObjTmp->Icon2 = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Row1),
					MANAGER(ObjTmp->PBox->Dialog),
					"BRIcon2", display,
					texte_mode_center_xpm, NULL, XcToggle);
  XtAddCallback(WIDGET(ObjTmp->Icon2),
	        XmNarmCallback, arm_center, ObjTmp);
  ObjTmp->Icon3 = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Row1),
					MANAGER(ObjTmp->PBox->Dialog),
					"BRIcon3", display,
					texte_mode_droite_xpm, NULL, XcToggle);
  XtAddCallback(WIDGET(ObjTmp->Icon3),
	        XmNarmCallback, arm_right, ObjTmp);
  ObjTmp->Icon4 = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Row1),
					MANAGER(ObjTmp->PBox->Dialog),
					"BRIcon4", display,
					texte_mode_align_xpm, NULL, XcToggle);
  XtAddCallback(WIDGET(ObjTmp->Icon4),
	        XmNarmCallback, arm_justify, ObjTmp);
  ObjTmp->Frame4 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->PBox->Dialog), 
					   "fDistribution", 0);
  ObjTmp->Grid4 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame4),
					MANAGER(ObjTmp->PBox->Dialog),
					"gDistribution", TRUE);
  ObjTmp->Ldistri = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid4),
			    MANAGER(ObjTmp->PBox->Dialog),
			    "lWordDistribution");
  ObjTmp->LratioMin = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid4),
			    MANAGER(ObjTmp->PBox->Dialog),
			    "lRatioMin");
  ObjTmp->LratioMax = 
    (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid4),
			    MANAGER(ObjTmp->PBox->Dialog),
			    "lRatioMax");
  ObjTmp->TFdistri = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfWordDistribution",
				      TF_REAL,
				      0.0, 100.0,
				      "80", NULL, 5);
  F(ObjTmp->TFdistri).set_unit(ObjTmp->TFdistri, XcUB_RATIO_PERCENTAGE,
			      NULL, TRUE, FALSE);
  F(ObjTmp->TFdistri).add_callback(ObjTmp->TFdistri,
				   (WTextFieldProc)certify_distri, ObjTmp);
  ObjTmp->TFratioMin =
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfRatioMin",
				      TF_REAL,
				      1.0, 100.0,
				      "75", NULL, 5);
  F(ObjTmp->TFratioMin).set_unit(ObjTmp->TFratioMin, XcUB_RATIO_PERCENTAGE,
				NULL, TRUE, FALSE);
  F(ObjTmp->TFratioMin).add_callback(ObjTmp->TFratioMin,
				     (WTextFieldProc)certify_ratioMin,
				     ObjTmp);
  ObjTmp->TFratioMax = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfRatioMax",
				      TF_REAL,
				      100.0, 1000.0,
				      "100", NULL, 5);
  F(ObjTmp->TFratioMax).set_unit(ObjTmp->TFratioMax, XcUB_RATIO_PERCENTAGE,
				NULL, TRUE, FALSE);
  F(ObjTmp->TFratioMax).add_callback(ObjTmp->TFratioMax,
				     (WTextFieldProc)certify_ratioMax,
				     ObjTmp);
  
  F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE, NULL);
  F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_BEST_SIZE,
			     WIDGET(ObjTmp->LratioMin), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TFratioMin), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE, NULL);
  F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_BEST_SIZE,
			     WIDGET(ObjTmp->Ldistri), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TFdistri), XcG_BEST_SIZE,
			     WIDGET(ObjTmp->LratioMax), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TFratioMax), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE, NULL);
  
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Frame1), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Frame2), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Frame3), 64,
			    WIDGET(ObjTmp->Frame4), XcG_MAX_SIZE,
			    NULL);
  


  F(ObjTmp->PBox->ItemList).addItems(ObjTmp->PBox->ItemList, FALSE,
				     XcIL_LINKED_LIST, -1,
				     ruler_base[0],
				     offsetof(c_TextRuler, next));
  
  F(ObjTmp->PBox->Dialog).createActionArea(ObjTmp->PBox->Dialog,
					   XcCD_OK | XcCD_CANCEL, XcCD_OK,
					   TRUE, (void (*)())cb_action, ObjTmp);

  F(ObjTmp->PBox).Map(ObjTmp->PBox, TRUE, ruler_base[0]);
  Xc_TRACE(("Object Box_rulers build"));
  return ObjTmp;
}

void dest_Box_regles(This)
c_Box_regles *This;
{
  while (This->v_ruler_base != NULL)
    DELETE(c_TextRuler)(This->v_ruler_base);
  DELETE(c_Label)(This->Lalinea);
  DELETE(c_Label)(This->LmargeG);
  DELETE(c_Label)(This->LmargeD);
  DELETE(c_Label)(This->Ldistri);
  DELETE(c_Label)(This->LratioMin);
  DELETE(c_Label)(This->LratioMax);
  DELETE(c_Label)(This->Linterlig);
  DELETE(c_Label)(This->Linterpar);
  DELETE(c_Icon)(This->Icon1);
  DELETE(c_Icon)(This->Icon2);
  DELETE(c_Icon)(This->Icon3);
  DELETE(c_Icon)(This->Icon4);
  DELETE(c_WTextField)(This->TFalinea);
  DELETE(c_WTextField)(This->TFmargeG);
  DELETE(c_WTextField)(This->TFmargeD);
  DELETE(c_WTextField)(This->TFdistri);
  DELETE(c_WTextField)(This->TFratioMin);
  DELETE(c_WTextField)(This->TFratioMax);
  DELETE(c_WTextField)(This->TFinterlig);
  DELETE(c_WTextField)(This->TFinterpar);
  DELETE(c_ToggleButton)(This->Tinterligne);
  DELETE(c_RowColumn)(This->Row1);
  DELETE(c_Grid)(This->Grid1);
  DELETE(c_Grid)(This->Grid2);
  DELETE(c_Grid)(This->Grid4);
  DELETE(c_Frame)(This->Frame1);
  DELETE(c_Frame)(This->Frame2);
  DELETE(c_Frame)(This->Frame3);
  DELETE(c_Frame)(This->Frame4);
  DELETE(c_Grid)(This->Grid);
  DELETE(c_ASLBox)(This->PBox);
  DELETE(c_ModifList)(This->list_save);
  Xc_free(This);
  Xc_TRACE(("Object Box_rulers destroyed"));
}

void *copy_Box_regles(This)
c_Box_regles *This;
{
  Xc_TRACE(("Copy not implemented"));
  return NULL;
}

/* ----------------------------------------------------------------- **
** Add_Item - Create a new ruler item                                **
** ----------------------------------------------------------------- */
static void Add_Item_R(w, This)
Widget		w;
c_Box_regles	*This;
{
  c_TextRuler	*tmp_ruler;
  char		*name;
  c_TextRuler	*ruler;
  coord_t	tleft_margin;
  coord_t	tright_margin;
  coord_t	tparagraph_indent;
  boolean	tinter_mode;
  coord_t	tinter_line;
  coord_t	tinter_paragraph;
  textruler_just_t tjustification;
  scale_t	tword_distribution;
  scale_t	tmin_ratio;
  scale_t	tmax_ratio;
  
  name = F(This->PBox).getNewItemName(This->PBox, (void **)&ruler);
  if (!name) return;
  
  if (F(This->list_save).get(This->list_save, ruler, 0, &tleft_margin) == -1)
    tleft_margin = ruler->left_margin;
  if (F(This->list_save).get(This->list_save, ruler, 1, &tright_margin) == -1)
    tright_margin = ruler->right_margin;
  if (F(This->list_save).get(This->list_save, ruler, 2, &tparagraph_indent)
      == -1) tparagraph_indent = ruler->paragraph_indent;
  if (F(This->list_save).get(This->list_save, ruler, 3, &tinter_mode) == -1)
    tinter_mode = ruler->inter_mode;
  if (F(This->list_save).get(This->list_save, ruler, 4, &tinter_line) == -1)
    tinter_line = ruler->inter_line;
  if (F(This->list_save).get(This->list_save, ruler, 5, &tinter_paragraph)
      == -1) tinter_paragraph = ruler->inter_paragraph;
  if (F(This->list_save).get(This->list_save, ruler, 6, &tjustification) == -1)
    tjustification = ruler->justification;
  if (F(This->list_save).get(This->list_save, ruler, 7, &tword_distribution) 
      == -1) tword_distribution = ruler->word_distribution;
  if (F(This->list_save).get(This->list_save, ruler, 8, &tmin_ratio) == -1)
    tmin_ratio = ruler->min_ratio;
  if (F(This->list_save).get(This->list_save, ruler, 9, &tmax_ratio) == -1)
    tmax_ratio = ruler->max_ratio;
  
  tmp_ruler = NEW(c_TextRuler)(&(This->v_ruler_base), name);
  
  F(tmp_ruler).set(tmp_ruler,
	       XcTR_LEFT_MARGIN, tleft_margin,
	       XcTR_RIGHT_MARGIN, tright_margin,
	       XcTR_INDENT, tparagraph_indent,
	       XcTR_INTERLINE_MODE, tinter_mode,
	       XcTR_INTERLINE, tinter_line,
	       XcTR_INTER_PARAGRAPH, tinter_paragraph,
	       XcTR_JUSTIFICATION, tjustification,
	       XcTR_WORD_DISTRIBUTION, tword_distribution,
	       XcTR_MIN_RATIO, tmin_ratio,
	       XcTR_MAX_RATIO, tmax_ratio,
	       XcTR_END);
  
  F(This->list_save).add(This->list_save, tmp_ruler);
  F(This->PBox->ItemList).setState(This->PBox->ItemList, FALSE,
				   XcIL_POINTER_ARRAY, -1);
  F(This->PBox->ItemList).addItems(This->PBox->ItemList, TRUE,
				   XcIL_VA_LIST, 1, tmp_ruler, NULL);
  F(This->PBox).selectItem(This->PBox, tmp_ruler, TRUE);
  XmProcessTraversal(WIDGET(This->PBox->text_field), XmTRAVERSE_CURRENT);
  XmTextFieldSetInsertionPosition(WIDGET(This->PBox->text_field), 
				  (XmTextPosition)strlen(name));
  Xc_free(name);
}

/* ----------------------------------------------------------------- ** 
** Del_SItem - Delete the Ruler Selection                            ** 
** ----------------------------------------------------------------- */
static void Del_SItem(w, This)
Widget		w;
c_Box_regles	*This;
{
  c_TextRuler	**ruler;
  int		num, i;
  c_TextRuler	*item;
  
  if(F(This->PBox->ItemList).getSelection(This->PBox->ItemList,
					  (void ***)&ruler, &num))
    {
      item = 
	F(This->PBox->ItemList).getFirstUnselectItem(This->PBox->ItemList);
      for(i = 0; i < num; i++)
	{
	  if (XcH_USAGE(ruler[i]) == 0 &&
	      This->PBox->ItemList->item_count > 1)
	    {
	      F(This->PBox->ItemList).deleteItems(This->PBox->ItemList,
						  XcIL_POINTER_ARRAY, 1,
						  (void **)&(ruler[i]));  
	      F(This->list_save).sub(This->list_save, ruler[i]);
	    }
	}
      if (num)
	Xc_free(ruler);
      if (item)
	{
	  F(This->PBox).selectItem(This->PBox, item, TRUE);
	  F(This->PBox->ItemList).selectItem(This->PBox->ItemList, item);
	}
    }
}


/* ----------------------------------------------------------------- ** 
** Change_Sel - Callback for reason Selection had change             ** 
** ----------------------------------------------------------------- */
static void Change_Sel(this, box_regles, ruler)
c_ASLBox	*this;
c_Box_regles	*box_regles;
c_TextRuler	*ruler;
{
  coord_t	tleft_margin;
  coord_t	tright_margin;
  coord_t	tparagraph_indent;
  boolean	tinter_mode;
  coord_t	tinter_line;
  coord_t	tinter_paragraph;
  textruler_just_t tjustification;
  scale_t	tword_distribution;
  scale_t	tmin_ratio;
  scale_t	tmax_ratio;

  if (F(box_regles->list_save).get(box_regles->list_save,
				   ruler, 3, &tinter_mode) == -1)
    tinter_mode = ruler->inter_mode;
  if (tinter_mode == TRUE)
    F(box_regles->Tinterligne).Set(box_regles->Tinterligne, False, False);
  else
    F(box_regles->Tinterligne).Set(box_regles->Tinterligne, True, False);

  if (F(box_regles->list_save).get(box_regles->list_save,
				   ruler, 0, &tleft_margin) == -1)
    tleft_margin = ruler->left_margin;
  F(box_regles->TFmargeG).set_value_in_unit
    (box_regles->TFmargeG, SCALE_TO_MILLIMETERS(tleft_margin));

  if (F(box_regles->list_save).get(box_regles->list_save,
				   ruler, 1, &tright_margin) == -1)
    tright_margin = ruler->right_margin;
  F(box_regles->TFmargeD).set_value_in_unit
    (box_regles->TFmargeD, SCALE_TO_MILLIMETERS(tright_margin));

  if (F(box_regles->list_save).get(box_regles->list_save,
				   ruler, 2, &tparagraph_indent) == -1)
    tparagraph_indent = ruler->paragraph_indent;
  F(box_regles->TFalinea).set_value_in_unit
    (box_regles->TFalinea, SCALE_TO_MILLIMETERS(tparagraph_indent));
  
  if (F(box_regles->list_save).get(box_regles->list_save,
				   ruler, 7, &tword_distribution) == -1)
    tword_distribution = ruler->word_distribution;
  F(box_regles->TFdistri).set_value_in_unit
    (box_regles->TFdistri, SCALE_TO_PERCENT(tword_distribution));
  
  if (F(box_regles->list_save).get(box_regles->list_save,
				   ruler, 8, &tmin_ratio) == -1)
    tmin_ratio = ruler->min_ratio;
  F(box_regles->TFratioMin).set_value_in_unit
    (box_regles->TFratioMin, SCALE_TO_PERCENT(tmin_ratio));

  if (F(box_regles->list_save).get(box_regles->list_save,
				   ruler, 9, &tmax_ratio) == -1)
    tmax_ratio = ruler->max_ratio;
  F(box_regles->TFratioMax).set_value_in_unit
    (box_regles->TFratioMax, SCALE_TO_PERCENT(tmax_ratio));
  
  if (F(box_regles->list_save).get(box_regles->list_save,
				   ruler, 4, &tinter_line) == -1)
    tinter_line = ruler->inter_line;
  F(box_regles->TFinterlig).set_value_in_unit
    (box_regles->TFinterlig, SCALE_TO_POINTS(tinter_line));
  
  if (F(box_regles->list_save).get(box_regles->list_save,
				   ruler, 5, &tinter_paragraph) == -1)
    tinter_paragraph = ruler->inter_paragraph;
  F(box_regles->TFinterpar).set_value_in_unit
    (box_regles->TFinterpar, SCALE_TO_POINTS(tinter_paragraph));
  
  if (F(box_regles->list_save).get(box_regles->list_save,
				   ruler, 6, &tjustification) == -1)
    tjustification = ruler->justification;
 
  switch (tjustification)
    {
    case XcTR_LEFT:
      XmToggleButtonSetState(WIDGET(box_regles->Icon1), True, True);
      break;
    case XcTR_CENTER:
      XmToggleButtonSetState(WIDGET(box_regles->Icon2), True, True);
      break;
    case XcTR_RIGHT:
      XmToggleButtonSetState(WIDGET(box_regles->Icon3), True, True);
      break;
    case XcTR_JUSTIFIED:
      XmToggleButtonSetState(WIDGET(box_regles->Icon4), True, True);
      break;
    }
}

static void certify_alinea(this, Obj)
c_WTextField *this;
c_Box_regles *Obj;
{
  c_TextRuler	**ruler;
  int		num2, i;
  coord_t	numf;
/*coord_t	tleft_margin;*/

  Xc_TRACE(("indent"));
  
  numf = (coord_t)F(this).get_value_in_unit(this);

  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
  {
    if (num2 == 1 && ((void *)*ruler) != Obj->PBox->edited_item)
    {
      /*	      if (F(Obj->list_save).get(Obj->list_save, 
		      (c_TextRuler *)Obj->PBox->edited_item,
		      0, &tleft_margin) == -1)
		      tleft_margin = ((c_TextRuler *)
		      Obj->PBox->edited_item)
		      ->left_margin;
		      if (-numf <= tleft_margin)
		      {
		      */		  
      F(Obj->list_save).set(Obj->list_save,
			    (c_TextRuler *)Obj->PBox->edited_item,
			    2, &numf);
      /*		}
			else
			{
			tleft_margin = -tleft_margin;
			F(Obj->list_save).set(Obj->list_save,
			(c_TextRuler *)Obj->PBox->edited_item,
			2, &tleft_margin);
			}
			*/
    }
    else
    {
      for (i = 0; i < num2; i++)
      {
	/*		  if (F(Obj->list_save).get(Obj->list_save, 
			  ruler[i], 0, &tleft_margin) == -1)
			  tleft_margin = (ruler[i])->left_margin;
			  if ((-numf) <= tleft_margin)
			  {
			  */
	F(Obj->list_save).set(Obj->list_save, ruler[i], 2, 
			      &numf);
	/*
	   }	    
	   else
	   {
	   tleft_margin = -tleft_margin;
	   F(Obj->list_save).set(Obj->list_save, ruler[i], 2,
	   &tleft_margin);
	   sprintf(string, "%g", SCALE_TO_POINTS(tleft_margin));
	   F(Obj->TFalinea).set(Obj->TFalinea, string);
	   }
	   */
      }
      
    }
    if (num2)
      Xc_free(ruler);
  }
  F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}


static void certify_margeG(this, Obj)
c_WTextField *this;
c_Box_regles *Obj;
{
  c_TextRuler	**ruler;
  int		num2, i;
  coord_t	numf;

  Xc_TRACE(("left margin"));

  numf = (coord_t)F(this).get_value_in_unit(this);

  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
  {
    if (num2 == 1 && ((void *)*ruler) != Obj->PBox->edited_item)
    {
      F(Obj->list_save).set(Obj->list_save,
			    (c_TextRuler *)Obj->PBox->edited_item,
			    0,
			    &numf);
    }
    else
    {
      for(i=0; i<num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      0,
			      &numf);
    }
    if (num2)
      Xc_free(ruler);
  }
  F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}

static void certify_margeD(this, Obj)
c_WTextField *this;
c_Box_regles *Obj;
{
  c_TextRuler	**ruler;
  int		num2, i;
  coord_t	numf;
  
  Xc_TRACE(("right margin"));
  
  numf = (coord_t)F(this).get_value_in_unit(this);
  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
  {
    if (num2 == 1 && ((void *)*ruler) != Obj->PBox->edited_item)
    {
      F(Obj->list_save).set(Obj->list_save,
			    (c_TextRuler *)Obj->PBox->edited_item,
			    1,
			    &numf);
    }
    else
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      1,
			      &numf);
    }
    if (num2)
      Xc_free(ruler);
  }
  F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}


static void certify_interlig(this, Obj)
c_WTextField *this;
c_Box_regles *Obj;
{
  c_TextRuler	**ruler;
  int		num2, i;
  coord_t	numf;

  numf = (coord_t)F(this).get_value_in_unit(this);
  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
  {
    if (num2 == 1 && ((void *)*ruler) != Obj->PBox->edited_item)
    {
      F(Obj->list_save).set(Obj->list_save,
			    (c_TextRuler *)Obj->PBox->edited_item,
			    4,
			    &numf);
    }
    else
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      4,
			      &numf);
    }
    if (num2)
      Xc_free(ruler);
  }
  F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}


static void certify_interpar(this, Obj)
c_WTextField *this;
c_Box_regles *Obj;
{
  c_TextRuler	**ruler;
  int		num2, i;
  coord_t	numf;

  Xc_TRACE(("interpar"));
  
  numf = (coord_t)F(this).get_value_in_unit(this);
  
  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
  {
    if (num2 == 1 && ((void *)*ruler) != Obj->PBox->edited_item)
    {
      F(Obj->list_save).set(Obj->list_save,
			    (c_TextRuler *)Obj->PBox->edited_item,
			    5,
			    &numf);
    }
    else
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      5,
			      &numf);
    }
    if (num2)
      Xc_free(ruler);
  }
  F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}

static void certify_distri(this, Obj)
c_WTextField *this;
c_Box_regles *Obj;
{
  c_TextRuler	**ruler;
  int		num2, i;
  scale_t	numf;

  Xc_TRACE(("word distribution"));

  numf = (scale_t)F(this).get_value_in_unit(this);
  F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
  {
    if (num2 == 1 && ((void *)*ruler) != Obj->PBox->edited_item)
    {
      F(Obj->list_save).set(Obj->list_save,
			    (c_TextRuler *)Obj->PBox->edited_item,
			    7,
			    &numf);
    }
    else
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      7,
			      &numf);
    }
    if (num2)
      Xc_free(ruler);
  }
  F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}


static void certify_ratioMin(this, Obj)
c_WTextField *this;
c_Box_regles *Obj;
{
  c_TextRuler	**ruler;
  int		num2, i;
  scale_t	numf;

  Xc_TRACE(("ratio min"));
  
  numf = (scale_t)F(this).get_value_in_unit(this);
  F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
  {
    if (num2 == 1 && ((void *)*ruler) != Obj->PBox->edited_item)
    {
      F(Obj->list_save).set(Obj->list_save,
			    (c_TextRuler *)Obj->PBox->edited_item,
			    8,
			    &numf);
    }
    else
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      8,
			      &numf);
    }
    if (num2)
      Xc_free(ruler);
  }
  F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}


static void certify_ratioMax(this, Obj)
c_WTextField *this;
c_Box_regles *Obj;
{
  c_TextRuler	**ruler;
  int		num2, i;
  scale_t	numf;

  Xc_TRACE(("ratio max"));
  
  numf = (scale_t)F(this).get_value_in_unit(this);
  F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
  {
    if (num2 == 1 && ((void *)*ruler) != Obj->PBox->edited_item)
    {
      F(Obj->list_save).set(Obj->list_save,
			    (c_TextRuler *)Obj->PBox->edited_item,
			    9,
			    &numf);
    }
    else
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      9,
			      &numf);
    }
    if (num2)
      Xc_free(ruler);
  }
  F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}


static void Set_param(This, Obj, cbs)
Widget	This;
c_Box_regles *Obj;
XmToggleButtonCallbackStruct *cbs;
{
  c_TextRuler	**ruler;
  int		num2, i;
  boolean	numf;

  Xc_HISTORY(("abs / rel switch"));

  if (cbs->set)
    numf = FALSE;
  else
    numf = TRUE;
  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      3,
			      &numf);
      if (num2)
	Xc_free(ruler);
    }
}

static void arm_left(This, Obj)
Widget	This;
c_Box_regles *Obj;
{
  c_TextRuler		**ruler;
  int			num2, i;
  textruler_just_t	numf;

  Xc_HISTORY(("arm left"));

  numf = XcTR_LEFT;
  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      6,
			      &numf);
      if (num2)
	Xc_free(ruler);
    }
}

static void arm_right(This, Obj)
Widget	This;
c_Box_regles *Obj;
{
  c_TextRuler	**ruler;
  int		num2, i;
  textruler_just_t	numf;

  Xc_HISTORY(("arm right"));

  numf = XcTR_RIGHT;
  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      6,
			      &numf);
      if (num2)
	Xc_free(ruler);
    }
}

static void arm_center(This, Obj)
Widget	This;
c_Box_regles *Obj;
{
  c_TextRuler	**ruler;
  int		num2, i;
  textruler_just_t	numf;

  Xc_HISTORY(("arm center"));

  numf = XcTR_CENTER;
  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      6,
			      &numf);
      if (num2)
	Xc_free(ruler);
    }
}

static void arm_justify(This, Obj)
Widget	This;
c_Box_regles *Obj;
{
  c_TextRuler	**ruler;
  int		num2, i;
  textruler_just_t	numf;

  Xc_HISTORY(("arm justify"));
  
  numf = XcTR_JUSTIFIED;
  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&ruler, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      ruler[i],
			      6,
			      &numf);
      if (num2)
	Xc_free(ruler);
    }
}


static void cb_action(cb_dialog, box, cb_reason)
c_CustomDialog	*cb_dialog;
c_Box_regles	*box;
int		cb_reason;
{
  F(cb_dialog).unmap(cb_dialog);
  if (cb_reason == XcCD_OK)
    {
      Xc_TRACE(("Try to go into test mode!"));
      F(box->list_save).change_all2(box->list_save);
    }
  else
    {
      Xc_TRACE(("canceled"));
    }
}

static void cb_change(box, ruler, type, num, data)
c_Box_regles	*box;
c_TextRuler	*ruler;
int		type;
int		num;
void		*data;
{
  switch (type)
    {
    case NEWSET:
      ruler = box->modif.ruler;
    case TOSET:
      Xc_TRACE(("TOSET!"));
      switch(num)
	{
	case 0:
	  box->modif.l_margin = ((coord_t *)data)[0];
	  break;
	case 1:
	  box->modif.r_margin = ((coord_t *)data)[0];
	  break;
	case 2:
	  box->modif.p_indent = ((coord_t *)data)[0];
	  break;
	case 3:
	  box->modif.il_mode = (int)((boolean *)data)[0];
	  break;
	case 4:
	  box->modif.il_line = ((coord_t *)data)[0];
	  break;
	case 5:
	  box->modif.il_para = ((coord_t *)data)[0];
	  break;
	case 6:
	  box->modif.justif = ((textruler_just_t *)data)[0];
	  break;
	case 7:
	  box->modif.word_distri = ((scale_t *)data)[0];
	  break;
	case 8:
	  box->modif.min_ratio = ((scale_t *)data)[0];
	  break;
	case 9:
	  box->modif.max_ratio = ((scale_t *)data)[0];
	  F(ruler).set(ruler,
		       XcTR_LEFT_MARGIN, box->modif.l_margin,
		       XcTR_RIGHT_MARGIN, box->modif.r_margin,
		       XcTR_INDENT, box->modif.p_indent,
		       XcTR_INTERLINE_MODE, box->modif.il_mode,
		       XcTR_INTERLINE, box->modif.il_line,
		       XcTR_INTER_PARAGRAPH, box->modif.il_para,
		       XcTR_JUSTIFICATION, box->modif.justif,
		       XcTR_WORD_DISTRIBUTION, box->modif.word_distri,
		       XcTR_MIN_RATIO, box->modif.min_ratio,
		       XcTR_MAX_RATIO, box->modif.max_ratio,
		       XcTR_END);
	  break;
	}
      break;
    case TOADD:
      Xc_TRACE(("TOADD!"));
      box->modif.ruler = (c_TextRuler *)NEW(c_TextRuler)(box->ruler_base, 
							 ruler->name);
      break;
    case TOSUB:
      Xc_TRACE(("TOSUB!"));
      DELETE(c_TextRuler)(ruler);
      break;
    }
}






