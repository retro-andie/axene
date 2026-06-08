/*
** Box_styles.c for Xclamation, XQuad and XAllWrite in Box_styles/
** Object and method for Box_styles Class
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
** Last update Mon Jan 24 23:40:56 2000 Emmanuel Paris
*/

/*--- include ---*/
#include "Box_styles.h"
#ifdef Xc_XALLWRITE
#include "Undo.h"
#endif

#include "../Pixmaps/texte.style.gras.zpm"
#include "../Pixmaps/texte.style.italique.zpm"
#include "../Pixmaps/texte.style.shadow.zpm"
#include "../Pixmaps/texte.style.underline.zpm"
#include "../Pixmaps/texte.style.stroke.zpm"

#include "../Pixmaps/underline_simple.zpm"
#include "../Pixmaps/underline_double.zpm"
#include "../Pixmaps/underline_triple.zpm"
#include "../Pixmaps/underline_simple_unover.zpm"
#include "../Pixmaps/underline_double_unover.zpm"
#include "../Pixmaps/underline_triple_unover.zpm"
#include "../Pixmaps/strikeout_simple.zpm"
#include "../Pixmaps/strikeout_double.zpm"
#include "../Pixmaps/strikeout_triple.zpm"
#include "../Pixmaps/strikeout_simple_unover.zpm"
#include "../Pixmaps/strikeout_double_unover.zpm"
#include "../Pixmaps/strikeout_triple_unover.zpm"

/*--- Aspect size ---*/
#define PREVIEW_SIZE	75
#define ONGLET_SIZE	24
#define COLOR_SIZE	34

/*--- position for modif list ---*/
#define SAVE_COLOR	0
#define SAVE_FONT	1
#define SAVE_SIZE	2
#define SAVE_ATTRIB	3
#define SAVE_WIDTH	4
#define SAVE_UDY	5
#define SAVE_UTHICK	6
#define SAVE_UCOLOR	7
#define SAVE_UTYPE	8
#define SAVE_UOVER	9
#define SAVE_SDY	10
#define SAVE_STHICK	11
#define SAVE_SCOLOR	12
#define SAVE_STYPE	13
#define SAVE_SOVER	14
#define SAVE_SHDX	15
#define SAVE_SHDY	16
#define SAVE_SHCOLOR	17
#define SAVE_SUBDY	18
#define SAVE_SUBSCALE	19
#define SAVE_SUPDY	20
#define SAVE_SUPSCALE	21
#define SAVE_NAME	22
#define SAVE_LAST	23

/*--- prototypes ---*/
void *cons_Box_styles();
void dest_Box_styles();
void *copy_Box_styles();

static void Add_Item();
static void Del_SItem();
static void Change_Sel();
static void change_color();
static void change_fonte();
static void certify_width ___PROTO((c_WTextField *this, c_Box_styles *Obj));
static void certify_fontsize ___PROTO((c_WTextField *this, c_Box_styles *Obj));
static void cb_action ___PROTO((c_CustomDialog	*cb_dialog,
				c_Box_styles	*box,
				int		cb_reason));
static void cb_change ___PROTO((c_Box_styles	*box,
				c_TextStyle	*style,
				int		type,
				int		num,
				void 		*data));
static boolean style_filter ___PROTO((c_TextStyle *this));
static void change_width();
static void change_bold();
static void change_italic();
static void change_underline();
static void change_strikeout();
static void change_subscript();
static void change_supscript();
static void change_shadow();
static void change_small_caps();
static void change_big_caps();
static void change_underline_color();
static void change_strikeout_color();
static void change_shadow_color();
static void certify_un_dy ___PROTO((c_WTextField *this, c_Box_styles *Obj));
static void certify_un_thickness ___PROTO((c_WTextField *this, 
					   c_Box_styles *Obj));
static void certify_st_dy ___PROTO((c_WTextField *this, c_Box_styles *Obj));
static void certify_st_thickness ___PROTO((c_WTextField *this, 
					   c_Box_styles *Obj));
static void change_un_type();
static void change_st_type();
static void certify_sh_dx ___PROTO((c_WTextField *this, c_Box_styles *Obj));
static void certify_sh_dy ___PROTO((c_WTextField *this, c_Box_styles *Obj));
static void certify_sub_dy ___PROTO((c_WTextField *this, c_Box_styles *Obj));
static void certify_sub_scale ___PROTO((c_WTextField *this, 
					c_Box_styles *Obj));
static void certify_sup_dy ___PROTO((c_WTextField *this, c_Box_styles *Obj));
static void certify_sup_scale ___PROTO((c_WTextField *this, 
					c_Box_styles *Obj));
static void draw_preview();
static void change_part();
static void create_part2();
static void set_overview();

/*--- fill class method ---*/
sf_Box_styles fc_Box_styles =
{
 cons_Box_styles,
 dest_Box_styles,
 copy_Box_styles
};

/*--- external ---*/
extern c_VectorFont	*GlobFontBase;
extern c_FontManager	*GlobFntMng;

/* ----------------------------------------------------------------- ** 
** constructeur                                                      ** 
** ----------------------------------------------------------------- */
void *cons_Box_styles(w_Parent, pszTitle, BaseStd, style)
Widget		w_Parent;
char		*pszTitle;
BaseStd_t 	*BaseStd;
c_TextStyle	**style;
{
 c_Box_styles	*ObjTmp;
 Arg		args[5]; 
 XmString	strTitre;
 Display	*display;
 
 if ((ObjTmp = (c_Box_styles *)Xc_malloc("Box_styles",
					 sizeof(c_Box_styles))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Box_styles;

#ifdef Xc_XALLWRITE
 ObjTmp->close_reason = XcCD_CANCEL;
#endif
 ObjTmp->WDApreview = NULL;
 /*--- save bases ---*/
 ObjTmp->Style_base = &(BaseStd->text_style_base);
 ObjTmp->v_Style_base = NULL;
 ObjTmp->list_save = (c_ModifList *)
  NEW(c_ModifList)(ObjTmp, cb_change,
		   SAVE_LAST,
		   (int)(offsetof(c_TextStyle, color)),
		   (int)sizeof(c_Color *),
		   (int)(offsetof(c_TextStyle, font)),
		   (int)sizeof(c_VectorFont *),
		   (int)(offsetof(c_TextStyle, point_size)),
		   (int)sizeof(coord_t),
		   (int)(offsetof(c_TextStyle, attributes)),
		   (int)sizeof(unsigned int),
		   (int)(offsetof(c_TextStyle, width.scale)),
		   (int)sizeof(scale_t),
		   (int)(offsetof(c_TextStyle, underline.dy)),
		   (int)sizeof(scale_t),
		   (int)(offsetof(c_TextStyle, underline.thickness)),
		   (int)sizeof(scale_t),
		   (int)(offsetof(c_TextStyle, underline.color)),
		   (int)sizeof(c_Color *),
		   (int)(offsetof(c_TextStyle, underline.type)),
		   (int)sizeof(enum textstyle_underline_e),
		   (int)(offsetof(c_TextStyle, underline.overspace)),
		   (int)sizeof(boolean),
		   (int)(offsetof(c_TextStyle, strikeout.dy)),
		   (int)sizeof(scale_t),
		   (int)(offsetof(c_TextStyle, strikeout.thickness)),
		   (int)sizeof(scale_t),
		   (int)(offsetof(c_TextStyle, strikeout.color)),
		   (int)sizeof(c_Color *),
		   (int)(offsetof(c_TextStyle, strikeout.type)),
		   (int)sizeof(enum textstyle_underline_e),
		   (int)(offsetof(c_TextStyle, strikeout.overspace)),
		   (int)sizeof(boolean),
		   (int)(offsetof(c_TextStyle, shadow.dx)),
		   (int)sizeof(scale_t),
		   (int)(offsetof(c_TextStyle, shadow.dy)),
		   (int)sizeof(scale_t),
		   (int)(offsetof(c_TextStyle, shadow.color)),
		   (int)sizeof(c_Color *),
		   (int)(offsetof(c_TextStyle, subscript.dy)),
		   (int)sizeof(scale_t),
		   (int)(offsetof(c_TextStyle, subscript.scale)),
		   (int)sizeof(scale_t),
		   (int)(offsetof(c_TextStyle, supscript.dy)),
		   (int)sizeof(scale_t),
		   (int)(offsetof(c_TextStyle, supscript.scale)),
		   (int)sizeof(scale_t),
		   (int)(offsetof(c_TextStyle, name)),
		   (int)XcMAX_NAME_LENGTH);

 ObjTmp->colorbase = BaseStd->color_base;
  
 ObjTmp->PBox = (c_ASLBox *)NEW(c_ASLBox)(w_Parent,
					  pszTitle,
					  640, 520,
					  FALSE, 
					  offsetof(c_TextStyle, name));

 display = XtDisplay(WIDGET(ObjTmp->PBox));

 strTitre = F(GlobResources).getXmString(GlobResources, XcR_editStyles);
 XtSetArg(args[0], XmNlabelString, strTitre);
 XtSetValues(WIDGET(ObjTmp->PBox->Label), args, 1);
 XmStringFree(strTitre);
  
 F(ObjTmp->PBox).setCallback(ObjTmp->PBox, Change_Sel,
			     ObjTmp);

 F(ObjTmp->PBox).Add_Buttons(ObjTmp->PBox, "lAddFont", Add_Item,
			     ObjTmp,
			     NULL);
 F(ObjTmp->PBox).Add_Buttons(ObjTmp->PBox, "lDelFont",
			     Del_SItem,
			     ObjTmp,
			     NULL);
 
 if (!BaseStd->text_style_base)
  NEW(c_TextStyle)(&(BaseStd->text_style_base), "New_style");
 
 WIDGET(ObjTmp) = WIDGET(ObjTmp->PBox);
 ObjTmp->Gmain = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				       MANAGER(ObjTmp->PBox->Dialog),
				       "gBoxStyles", TRUE);
 
 ObjTmp->Fonglet = NEW(c_Frame)(WIDGET(ObjTmp->Gmain),
				MANAGER(ObjTmp->PBox->Dialog), 
				NULL, -1);
 XtSetArg(args[0], XmNshadowType, XmSHADOW_IN);
 XtSetArg(args[1], XmNshadowThickness, 1);
 XtSetValues(WIDGET(ObjTmp->Fonglet), args, 2);

 ObjTmp->Gonglet = (c_Grid *)NEW(c_Grid)
  (WIDGET(ObjTmp->Fonglet), MANAGER(ObjTmp->PBox->Dialog), "gBSBar", TRUE);
 XtVaSetValues(WIDGET(ObjTmp->Gonglet),
	       XmNmarginHeight, 0,
	       XmNmarginWidth, 0,
	       XmNspacing, 0,
	       NULL);

 ObjTmp->TBglobal = (c_ToggleButton *)NEW(c_ToggleButton)
  (WIDGET(ObjTmp->Gonglet), MANAGER(ObjTmp->PBox->Dialog),
   "tbBSglobal");

 F(ObjTmp->TBglobal).Add_Callback
  (ObjTmp->TBglobal, XmNvalueChangedCallback, change_part, ObjTmp);

  
 XtVaSetValues(WIDGET(ObjTmp->TBglobal),
	       XmNindicatorOn, False,
	       XmNspacing, 0,
	       XmNhighlightThickness, 1,
	       XmNborderWidth, 0,
	       NULL);

 ObjTmp->TBconfig = (c_ToggleButton *)NEW(c_ToggleButton)
  (WIDGET(ObjTmp->Gonglet), MANAGER(ObjTmp->PBox->Dialog),
   "tbBSconfig");
 F(ObjTmp->TBconfig).Add_Callback
  (ObjTmp->TBconfig, XmNvalueChangedCallback, change_part, ObjTmp);
  
 XtVaSetValues(WIDGET(ObjTmp->TBconfig),
	       XmNindicatorOn, False,
	       XmNspacing, 0,
	       XmNhighlightThickness, 1,
	       XmNborderWidth, 0,
	       NULL);

 F(ObjTmp->Gonglet).addColumn(ObjTmp->Gonglet, XcG_MAX_SIZE,
			      WIDGET(ObjTmp->TBglobal), XcG_MAX_SIZE,
			      WIDGET(ObjTmp->TBconfig), XcG_MAX_SIZE,
			      NULL);


 ObjTmp->Fmain = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Gmain),
					 MANAGER(ObjTmp->PBox->Dialog), 
					 "fBSmain", 0);

 ObjTmp->current_part = 0;
 ObjTmp->part2_created = FALSE;
 F(ObjTmp->TBglobal).Set(ObjTmp->TBglobal, True, False);
 ObjTmp->Gglobal = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Fmain),
					 MANAGER(ObjTmp->PBox->Dialog),
					 "gBSglobal", TRUE);
 ObjTmp->Lfamily = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Gglobal),
			  MANAGER(ObjTmp->PBox->Dialog),
			  "lBSfamily");
 ObjTmp->IMfont = NEW(c_ItemMenu)(WIDGET(ObjTmp->Gglobal),
				  MANAGER(ObjTmp->PBox->Dialog),
				  "imBSfont",
				  0, 0);
 F(ObjTmp->IMfont).set(ObjTmp->IMfont,
		       TRUE, offsetof(family_tree, name),
		       NULL, NULL);
 F(ObjTmp->IMfont).addItems(ObjTmp->IMfont,
			    XcIM_LINKED_LIST, -1,
			    GlobFontBase->family,
			    offsetof(family_tree, Next));
 F(ObjTmp->IMfont).setCallback(ObjTmp->IMfont,
			       change_fonte, ObjTmp);
 F(ObjTmp->IMfont).calculSize(ObjTmp->IMfont, 0);
  
 ObjTmp->CLfont = NEW(c_ColorList)(WIDGET(ObjTmp->Gglobal),
				   MANAGER(ObjTmp->PBox->Dialog),
				   "clBSfont", XcCL_NORMAL);
 F(ObjTmp->CLfont).setCallback(ObjTmp->CLfont,
			       (void *)change_color, ObjTmp);
 F(ObjTmp->CLfont).setColorBase(ObjTmp->CLfont,
				ObjTmp->colorbase, CL_SKIP_TRANSPARENCY);

 ObjTmp->Lfontsize = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Gglobal),
			  MANAGER(ObjTmp->PBox->Dialog),
			  "lBSfontsize");
 ObjTmp->SBfontsize = 
  (c_SpinBox *)NEW(c_SpinBox)(WIDGET(ObjTmp->Gglobal),
			      MANAGER(ObjTmp->PBox->Dialog),
			      "sbBSfontsize", XcSB_PLUS_MINUS,
			      TF_REAL, 1.0, 1000.0, "12", 5);

 F(ObjTmp->SBfontsize).setUnit(ObjTmp->SBfontsize, XcUB_LENGTH_POINT, 
			       NULL, TRUE, FALSE);
 
 F(ObjTmp->SBfontsize).setValueTable
  (ObjTmp->SBfontsize, 
   (real)SCALE_FROM_POINTS(4), (real)SCALE_FROM_POINTS(6), 
   (real)SCALE_FROM_POINTS(8), (real)SCALE_FROM_POINTS(9), 
   (real)SCALE_FROM_POINTS(10), (real)SCALE_FROM_POINTS(12),
   (real)SCALE_FROM_POINTS(14), (real)SCALE_FROM_POINTS(16), 
   (real)SCALE_FROM_POINTS(18), (real)SCALE_FROM_POINTS(24),
   (real)SCALE_FROM_POINTS(36), (real)SCALE_FROM_POINTS(64), 
   (real)SCALE_FROM_POINTS(100), (real)SCALE_FROM_POINTS(200), 
   (real)SCALE_FROM_POINTS(300), (real)SCALE_FROM_POINTS(400),
   (real)SCALE_FROM_POINTS(500), (real)SCALE_FROM_POINTS(600), 
   (real)SCALE_FROM_POINTS(700), (real)SCALE_FROM_POINTS(800), 
   (real)SCALE_FROM_POINTS(900), SB_VALUE_TABLE_END);
 F(ObjTmp->SBfontsize).setAutoRepeatDelay(ObjTmp->SBfontsize,
					  SB_DEFAULT_AUTO_REPEAT);
 F(ObjTmp->SBfontsize).addCallback(ObjTmp->SBfontsize,
				   (void *)certify_fontsize,
				   ObjTmp, SB_TEXTFIELD_CALLBACK);

 ObjTmp->Ibold = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Gglobal),
				       MANAGER(ObjTmp->PBox->Dialog),
				       "iBSbold", display,
				       texte_style_gras_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(ObjTmp->Ibold), FUNC_VALUE, 
	  change_bold, ObjTmp);

 ObjTmp->Iitalic = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Gglobal),
					 MANAGER(ObjTmp->PBox->Dialog),
					 "iBSitalic", display,
					 texte_style_italique_xpm, NULL, 
					 XcToggle);
 CALLBACK(WIDGET(ObjTmp->Iitalic), FUNC_VALUE, 
	  change_italic, ObjTmp);

 ObjTmp->Ishadow = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Gglobal),
					 MANAGER(ObjTmp->PBox->Dialog),
					 "iBSshadow", display,
					 texte_style_shadow_xpm, NULL, 
					 XcToggle);
 CALLBACK(WIDGET(ObjTmp->Ishadow), FUNC_VALUE, 
	  change_shadow, ObjTmp);

 ObjTmp->Iunderline = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Gglobal),
					    MANAGER(ObjTmp->PBox->Dialog),
					    "iBSunderline", display,
					    texte_style_underline_xpm,
					    NULL, XcToggle);
 CALLBACK(WIDGET(ObjTmp->Iunderline), FUNC_VALUE, 
	  change_underline, ObjTmp);

 ObjTmp->Istrikeout = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Gglobal),
					    MANAGER(ObjTmp->PBox->Dialog),
					    "iBSstrikeout", display,
					    texte_style_stroke_xpm, NULL,
					    XcToggle);
 CALLBACK(WIDGET(ObjTmp->Istrikeout), FUNC_VALUE, 
	  change_strikeout, ObjTmp);


 ObjTmp->Tsupscript = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Gglobal),
					MANAGER(ObjTmp->PBox->Dialog),
					"tbBSsupscript");

 F(ObjTmp->Tsupscript).Add_Callback
  (ObjTmp->Tsupscript, XmNvalueChangedCallback, change_supscript, ObjTmp);


 ObjTmp->Tsubscript = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Gglobal),
					MANAGER(ObjTmp->PBox->Dialog),
					"tbBSsubscript");

 F(ObjTmp->Tsubscript).Add_Callback
  (ObjTmp->Tsubscript, XmNvalueChangedCallback, change_subscript, ObjTmp);


 ObjTmp->Tsmallcaps = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Gglobal),
					MANAGER(ObjTmp->PBox->Dialog),
					"tbBSsmallcaps");
 F(ObjTmp->Tsmallcaps).Add_Callback
  (ObjTmp->Tsmallcaps, XmNvalueChangedCallback, change_small_caps, ObjTmp);

 ObjTmp->Tbigcaps = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Gglobal),
					MANAGER(ObjTmp->PBox->Dialog),
					"tbBSbigcaps");
 F(ObjTmp->Tbigcaps).Add_Callback
  (ObjTmp->Tbigcaps, XmNvalueChangedCallback, change_big_caps, ObjTmp);


 ObjTmp->Texpand = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Gglobal),
					MANAGER(ObjTmp->PBox->Dialog),
					"tbBSwidth");
 F(ObjTmp->Texpand).Add_Callback
  (ObjTmp->Texpand, XmNvalueChangedCallback, change_width, ObjTmp);


 ObjTmp->Foverview = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Gglobal),
					     MANAGER(ObjTmp->PBox->Dialog), 
					     "fBSoverview", 0);
 ObjTmp->w_TextOverview = XtVaCreateWidget("tBSoverview", xmTextWidgetClass,
					   WIDGET(ObjTmp->Foverview),
					   XmNshadowThickness, 0,
					   XmNverifyBell, False,
					   XmNcursorPositionVisible, False,
					   XmNeditMode, XmMULTI_LINE_EDIT,
					   XmNeditable, False,
					   XmNwordWrap, True,
					   XmNhighlightThickness, 0,
					   XmNtraversalOn, False,
					   XmNmarginHeight, 0,
					   XmNmarginWidth, 10,
					   NULL);
 F(MANAGER(ObjTmp->PBox->Dialog)).
  Add_child(MANAGER(ObjTmp->PBox->Dialog), ObjTmp->w_TextOverview,
	    WIDGET(ObjTmp->Foverview));

 F(ObjTmp->Gglobal).addColumn(ObjTmp->Gglobal, XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Lfamily), XcG_MAX_SIZE,
			      WIDGET(ObjTmp->IMfont), XcG_BEST_SIZE,
			      NULL);
 F(ObjTmp->Gglobal).addColumn(ObjTmp->Gglobal, XcG_BEST_SIZE,
			      WIDGET(ObjTmp->CLfont), XcG_MAX_SIZE,
			      WIDGET(ObjTmp->Lfontsize), XcG_BEST_SIZE,
			      WIDGET(ObjTmp->SBfontsize), XcG_BEST_SIZE,
			      NULL);
 F(ObjTmp->Gglobal).addColumn(ObjTmp->Gglobal, XcG_MAX_SIZE,
			      NULL);
 F(ObjTmp->Gglobal).addColumn(ObjTmp->Gglobal, XcG_BEST_SIZE,
			      XcG_WIDGET_NULL, XcG_MAX_SIZE,
			      WIDGET(ObjTmp->Ibold), XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Iitalic), XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Ishadow), XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Iunderline), XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Istrikeout), XcG_BEST_SIZE,
			      XcG_WIDGET_NULL, XcG_MAX_SIZE,
			      NULL);
 F(ObjTmp->Gglobal).addColumn(ObjTmp->Gglobal, XcG_MAX_SIZE,
			      NULL);
 F(ObjTmp->Gglobal).addColumn(ObjTmp->Gglobal, XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Tsupscript), XcG_MAX_SIZE,
			      NULL);
 F(ObjTmp->Gglobal).addColumn(ObjTmp->Gglobal, XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Tsubscript), XcG_MAX_SIZE,
			      NULL);
 F(ObjTmp->Gglobal).addColumn(ObjTmp->Gglobal, XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Tsmallcaps), XcG_MAX_SIZE,
			      NULL);
 F(ObjTmp->Gglobal).addColumn(ObjTmp->Gglobal, XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Tbigcaps), XcG_MAX_SIZE,
			      NULL);
 F(ObjTmp->Gglobal).addColumn(ObjTmp->Gglobal, XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Texpand), XcG_MAX_SIZE,
			      NULL);
 F(ObjTmp->Gglobal).addColumn(ObjTmp->Gglobal, 80,
			      WIDGET(ObjTmp->Foverview), XcG_MAX_SIZE,
			      NULL);


 ObjTmp->Fpreview = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Gmain),
					    MANAGER(ObjTmp->PBox->Dialog), 
					    NULL, 0);
 ObjTmp->WDApreview = XmCreateDrawingArea(WIDGET(ObjTmp->Fpreview),
					  "wdaBSpreview", args, 0);
 F(MANAGER(ObjTmp->PBox->Dialog)).Add_child(MANAGER(ObjTmp->PBox->Dialog),
					    ObjTmp->WDApreview,
					    WIDGET(ObjTmp->Fpreview));
 XtAddCallback(ObjTmp->WDApreview, XmNexposeCallback,
	       (XtCallbackProc)draw_preview, ObjTmp);

 F(ObjTmp->Gmain).addColumn(ObjTmp->Gmain, ONGLET_SIZE,
			    WIDGET(ObjTmp->Fonglet), XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Gmain).addColumn(ObjTmp->Gmain, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Fmain), XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Gmain).addColumn(ObjTmp->Gmain, 7, NULL);
 F(ObjTmp->Gmain).addColumn(ObjTmp->Gmain, PREVIEW_SIZE,
			    WIDGET(ObjTmp->Fpreview), XcG_MAX_SIZE,
			    NULL);
  
 F(ObjTmp->PBox->ItemList).set(ObjTmp->PBox->ItemList, -1,
			       FALSE, -1L, -1, (void *)-1, 
			       (void *)style_filter);
 F(ObjTmp->PBox->ItemList).addItems(ObjTmp->PBox->ItemList, FALSE,
				    XcIL_LINKED_LIST, -1,
				    BaseStd->text_style_base,
				    offsetof(c_TextStyle, next));
  
#if defined(Xc_XQUAD) || defined(Xc_XALLWRITE)
 F(ObjTmp->PBox->Dialog).createActionArea(ObjTmp->PBox->Dialog,
					  XcCD_APPLY
					  | XcCD_OK 		
					  | XcCD_CANCEL
					  , XcCD_APPLY,
					  TRUE,
					  (void (*)())cb_action, ObjTmp);
#else
 F(ObjTmp->PBox->Dialog).createActionArea(ObjTmp->PBox->Dialog,
					  XcCD_OK 
					  | XcCD_CANCEL, 
					  XcCD_OK,
					  TRUE,
					  (void (*)())cb_action, ObjTmp);
#endif
 if (style)
 {
  if ((*style)->hide_flag && (*style)->inherit_from)
   F(ObjTmp->PBox).Map(ObjTmp->PBox, TRUE, (*style)->inherit_from);
  else
   F(ObjTmp->PBox).Map(ObjTmp->PBox, TRUE, *style);
 }
 else
  F(ObjTmp->PBox).Map(ObjTmp->PBox, TRUE, BaseStd->text_style_base);
  
#if defined(Xc_XQUAD) || defined(Xc_XALLWRITE)
 ObjTmp->retptr = style;
#endif
 Xc_TRACE(("Object Box_styles build"));
 return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** destructeur                                                       ** 
** ----------------------------------------------------------------- */
void dest_Box_styles(This)
c_Box_styles *This;
{
#ifdef Xc_XALLWRITE
 c_TextStyle *style = (c_TextStyle *)This->PBox->edited_item, *style2;
#endif

 XtDestroyWidget(This->WDApreview);
 DELETE(c_Frame)(This->Fpreview);

 DELETE(c_ToggleButton)(This->Tsupscript);
 DELETE(c_ToggleButton)(This->Tsubscript);
 DELETE(c_ToggleButton)(This->Tsmallcaps);
 DELETE(c_ToggleButton)(This->Tbigcaps);
 DELETE(c_ToggleButton)(This->Texpand);

 DELETE(c_Icon)(This->Ibold);
 DELETE(c_Icon)(This->Iitalic);
 DELETE(c_Icon)(This->Ishadow);
 DELETE(c_Icon)(This->Iunderline);
 DELETE(c_Icon)(This->Istrikeout);

 DELETE(c_Label)(This->Lfontsize);
 DELETE(c_SpinBox)(This->SBfontsize);
 DELETE(c_ColorList)(This->CLfont);
 DELETE(c_ItemMenu)(This->IMfont);
 DELETE(c_Label)(This->Lfamily);
 XtDestroyWidget(This->w_TextOverview);
 DELETE(c_Frame)(This->Foverview);
 DELETE(c_Grid)(This->Gglobal);

 if (This->part2_created)
 {
  DELETE(c_Label)(This->Lun_dy);
  DELETE(c_WTextField)(This->TFun_dy);
  DELETE(c_Label)(This->Lun_thickness);
  DELETE(c_WTextField)(This->TFun_thickness);
  DELETE(c_ColorList)(This->CLunderline);
  DELETE(c_OptionMenu)(This->OMun_type);
  DELETE(c_Grid)(This->RCunderline);
  DELETE(c_Frame)(This->Funderline);

  DELETE(c_Label)(This->Lst_dy);
  DELETE(c_WTextField)(This->TFst_dy);
  DELETE(c_Label)(This->Lst_thickness);
  DELETE(c_WTextField)(This->TFst_thickness);
  DELETE(c_ColorList)(This->CLstrikeout);
  DELETE(c_OptionMenu)(This->OMst_type);
  DELETE(c_Grid)(This->RCstrikeout);
  DELETE(c_Frame)(This->Fstrikeout);

  DELETE(c_Label)(This->Lwidth);
  DELETE(c_WTextField)(This->TFwidth);
  DELETE(c_Grid)(This->RCwidth);
  DELETE(c_Frame)(This->Fwidth);

  DELETE(c_WTextField)(This->TFsh_dx);
  DELETE(c_WTextField)(This->TFsh_dy);
  DELETE(c_Label)(This->LshadowX);
  DELETE(c_Label)(This->LshadowY);
  DELETE(c_ColorList)(This->CLshadow);
  DELETE(c_Grid)(This->RCshadow);
  DELETE(c_Frame)(This->Fshadow);

  DELETE(c_Label)(This->Lsuby);
  DELETE(c_Label)(This->Lsubscale);
  DELETE(c_WTextField)(This->TFsub_dy);
  DELETE(c_WTextField)(This->TFsub_scale);
  DELETE(c_Grid)(This->RCsubscript);
  DELETE(c_Frame)(This->Fsubscript);

  DELETE(c_Label)(This->Lsupy);
  DELETE(c_Label)(This->Lsupscale);
  DELETE(c_WTextField)(This->TFsup_dy);
  DELETE(c_WTextField)(This->TFsup_scale);
  DELETE(c_Grid)(This->RCsupscript);
  DELETE(c_Frame)(This->Fsupscript);
  DELETE(c_Grid)(This->Gconfig);
 }

 DELETE(c_Frame)(This->Fmain);

 DELETE(c_ToggleButton)(This->TBglobal);
 DELETE(c_ToggleButton)(This->TBconfig);
 DELETE(c_Grid)(This->Gonglet);
 DELETE(c_Frame)(This->Fonglet);
 DELETE(c_Grid)(This->Gmain);
 DELETE(c_ASLBox)(This->PBox);

#ifdef Xc_XALLWRITE
 *(This->retptr) = NULL;
 switch(This->close_reason)
 {
 case XcCD_APPLY:
  if (F(This->list_save).something_to_change(This->list_save))
  {
   if (SET_UNDO(XcR_USApply))  goto no_change;
   F(This->list_save).change_all2(This->list_save);
  }
  style2 = *This->Style_base;	/* try to find style in Style_base */
  while(style2 && style2 != style)
   style2 = style2->next;

  if (style2)			/* ok found */
   style = style2;
  else				/* not found: style is a new created style */
  {
   c_VectorFont	*tfont1;
   coord_t	tpoint1;
   scale_t	tscale1;
   scale_t	tscale2;
   scale_t	tscale3;
   c_Color	*tcolor1;
   enum textstyle_underline_e ttype1;
   boolean       tboolean1;
   unsigned int	tattrib1;

   /*--- set color,font,size parameter ---*/
   if (F(This->list_save).get(This->list_save, style, SAVE_COLOR,
			      &tcolor1) == -1)
    tcolor1 = style->color;
   if (F(This->list_save).get(This->list_save, style, SAVE_FONT, 
			      &tfont1) == -1)
    tfont1 = style->font;
   if (F(This->list_save).get(This->list_save, style, SAVE_SIZE, 
			      &tpoint1) == -1)
    tpoint1 = style->point_size;
   F(style).set(style, XcTS_FONT, tfont1,
		XcTS_COLOR, tcolor1,
		XcTS_SIZE, tpoint1, XcTS_END);

   /*--- set width parameter ---*/
   if (F(This->list_save).get(This->list_save, style, SAVE_WIDTH,
			      &tscale1) == -1)
    tscale1 = style->width.scale;
   F(style).set(style, XcTS_WIDTH_CONF, tscale1, XcTS_END);

   /*--- set underline parameters ---*/
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_UDY, &tscale1) == -1)
    tscale1 = style->underline.dy;
   tscale2 = style->underline.overstroke;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_UTHICK, &tscale3) == -1)
    tscale3 = style->underline.thickness;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_UCOLOR, &tcolor1) == -1)
    tcolor1 = style->underline.color;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_UTYPE, &ttype1) == -1)
    ttype1 = style->underline.type;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_UOVER, &tboolean1) == -1)
    tboolean1 = style->underline.overspace;
   F(style).set(style, XcTS_UNDERLINE_CONF, tscale1, tscale2,
		tscale3, tboolean1, tcolor1, ttype1, XcTS_END);

   /*--- strikeout parameters ---*/ 
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_SDY, &tscale1) == -1)
    tscale1 = style->strikeout.dy;
   tscale2 = style->strikeout.overstroke;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_STHICK, &tscale3) == -1)
    tscale3 = style->strikeout.thickness;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_SCOLOR, &tcolor1) == -1)
    tcolor1 = style->strikeout.color;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_STYPE, &ttype1) == -1)
    ttype1 = style->strikeout.type;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_SOVER, &tboolean1) == -1)
    tboolean1 = style->strikeout.overspace;
   F(style).set(style, XcTS_STRIKEOUT_CONF, tscale1, tscale2,
		tscale3, tboolean1, tcolor1, ttype1, XcTS_END);

   /*--- shadow parameters ---*/
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_SHDX, &tscale1) == -1)
    tscale1 = style->shadow.dx;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_SHDY, &tscale2) == -1)
    tscale2 = style->shadow.dy;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_SHCOLOR, &tcolor1) == -1)
    tcolor1 = style->shadow.color;
   F(style).set(style, XcTS_SHADOW_CONF, tscale1, tscale2,
		tcolor1, XcTS_END);

   /*--- subscript parameters ---*/
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_SUBDY, &tscale1) == -1)
    tscale1 = style->subscript.dy;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_SUBSCALE, &tscale2) == -1)
    tscale2 = style->subscript.scale;
   F(style).set(style, XcTS_SUBSCRIPT_CONF, tscale1, tscale2,
		XcTS_END);

   /*--- superscript parameters ---*/
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_SUPDY, &tscale1) == -1)
    tscale1 = style->supscript.dy;
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_SUPSCALE, &tscale2) == -1)
    tscale2 = style->supscript.scale;
   F(style).set(style, XcTS_SUPSCRIPT_CONF, tscale1, tscale2,
		XcTS_END);

   /*--- set attributes ---*/
   if (F(This->list_save).get(This->list_save,
			      style, SAVE_ATTRIB, &tattrib1) == -1)
    tattrib1 = style->attributes;
   F(style).set(style, XcTS_ATTRIB, tattrib1, XcTS_END);

   style2 = *This->Style_base;
   while(style2 && !F(style).compare(style, style2, CMP_WITH_NAME))
   {
    /*printf("style2: %s\n", style2->name);*/
    style2 = style2->next;
   }
   if (style2)
    style = style2;
   else
    style = NULL;
  }
  *(This->retptr) = style;
  break;
 case XcCD_OK:
  if (F(This->list_save).something_to_change(This->list_save))
  {
   if (SET_UNDO(XcR_USList)) goto no_change;
   F(This->list_save).change_all2(This->list_save);
  }
  break;
 default:
  Xc_TRACE(("canceled"));
 }
 no_change:
#endif

#if 0		/* in case we include undo in Xclamation */
 if (This->close_reason == XcCD_OK)
 {
  if (F(This->list_save).something_to_change(This->list_save))
  {
   if (SET_UNDO(XcR_USList)) goto no_change;
   F(This->list_save).change_all2(This->list_save);
  }
 }
 no_change:
#endif
 
 while (This->v_Style_base != NULL)
  DELETE(c_TextStyle)(This->v_Style_base);

 DELETE(c_ModifList)(This->list_save);
 Xc_free(This);
 Xc_TRACE(("Object Box_styles destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copieur                                                           ** 
** ----------------------------------------------------------------- */
void *copy_Box_styles(This)
c_Box_styles *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}

/* ----------------------------------------------------------------- **
** Add_Item - Create a new style item                                **
** ----------------------------------------------------------------- */
static void Add_Item(w, This)
Widget		w;
c_Box_styles	*This;
{
 c_TextStyle	*tmp_style;
 char		*name;
 c_TextStyle	*style;
 c_Color	*tcolor;
 c_VectorFont	*tfont;
 coord_t	tpoint;

 scale_t	tscale1;
 scale_t	tscale2;
 scale_t	tscale3;
 c_Color	*tcolor1;
 enum textstyle_underline_e ttype1;
 boolean       tboolean1;
 unsigned int	tattrib1;
  
 name = F(This->PBox).getNewItemName(This->PBox, (void **)&style);
 if (!name) return;
  
 if (F(This->list_save).get(This->list_save, style, SAVE_COLOR, &tcolor) == -1)
  tcolor = style->color;
 if (F(This->list_save).get(This->list_save, style, SAVE_FONT, &tfont) == -1)
  tfont = style->font;
 if (F(This->list_save).get(This->list_save, style, SAVE_SIZE, &tpoint) == -1)
  tpoint = style->point_size;
 tmp_style = NEW(c_TextStyle)(&(This->v_Style_base), name,
			      tfont, tpoint, tcolor);

 /*--- set width parameter ---*/
 if (F(This->list_save).get(This->list_save, style, SAVE_WIDTH,
			    &tscale1) == -1)
  tscale1 = style->width.scale;
 F(tmp_style).set(tmp_style, XcTS_WIDTH_CONF, tscale1, XcTS_END);
 
 /*--- set underline parameters ---*/
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_UDY, &tscale1) == -1)
  tscale1 = style->underline.dy;
 tscale2 = style->underline.overstroke;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_UTHICK, &tscale3) == -1)
  tscale3 = style->underline.thickness;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_UCOLOR, &tcolor1) == -1)
  tcolor1 = style->underline.color;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_UTYPE, &ttype1) == -1)
  ttype1 = style->underline.type;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_UOVER, &tboolean1) == -1)
  tboolean1 = style->underline.overspace;
 F(tmp_style).set(tmp_style, XcTS_UNDERLINE_CONF, tscale1, tscale2,
		  tscale3, tboolean1, tcolor1, ttype1, XcTS_END);

 /*--- strikeout parameters ---*/ 
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_SDY, &tscale1) == -1)
  tscale1 = style->strikeout.dy;
 tscale2 = style->strikeout.overstroke;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_STHICK, &tscale3) == -1)
  tscale3 = style->strikeout.thickness;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_SCOLOR, &tcolor1) == -1)
  tcolor1 = style->strikeout.color;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_STYPE, &ttype1) == -1)
  ttype1 = style->strikeout.type;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_SOVER, &tboolean1) == -1)
  tboolean1 = style->strikeout.overspace;
 F(tmp_style).set(tmp_style, XcTS_STRIKEOUT_CONF, tscale1, tscale2,
		  tscale3, tboolean1, tcolor1, ttype1, XcTS_END);

 /*--- shadow parameters ---*/
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_SHDX, &tscale1) == -1)
  tscale1 = style->shadow.dx;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_SHDY, &tscale2) == -1)
  tscale2 = style->shadow.dy;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_SHCOLOR, &tcolor1) == -1)
  tcolor1 = style->shadow.color;
 F(tmp_style).set(tmp_style, XcTS_SHADOW_CONF, tscale1, tscale2,
		  tcolor1, XcTS_END);

 /*--- subscript parameters ---*/
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_SUBDY, &tscale1) == -1)
  tscale1 = style->subscript.dy;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_SUBSCALE, &tscale2) == -1)
  tscale2 = style->subscript.scale;
 F(tmp_style).set(tmp_style, XcTS_SUBSCRIPT_CONF, tscale1, tscale2,
		  XcTS_END);

 /*--- superscript parameters ---*/
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_SUPDY, &tscale1) == -1)
  tscale1 = style->supscript.dy;
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_SUPSCALE, &tscale2) == -1)
  tscale2 = style->supscript.scale;
 F(tmp_style).set(tmp_style, XcTS_SUPSCRIPT_CONF, tscale1, tscale2,
		  XcTS_END);

 /*--- set attributes ---*/
 if (F(This->list_save).get(This->list_save,
			    style, SAVE_ATTRIB, &tattrib1) == -1)
  tattrib1 = style->attributes;
 F(tmp_style).set(tmp_style, XcTS_ATTRIB, tattrib1, XcTS_END);


 F(This->list_save).add(This->list_save, tmp_style);
 F(This->PBox->ItemList).setState(This->PBox->ItemList, FALSE,
				  XcIL_POINTER_ARRAY, -1);
 F(This->PBox->ItemList).addItems(This->PBox->ItemList, TRUE,
				  XcIL_VA_LIST, 1, tmp_style, NULL);
 F(This->PBox).selectItem(This->PBox, tmp_style, TRUE);
 XmProcessTraversal(WIDGET(This->PBox->text_field), XmTRAVERSE_CURRENT);
 XmTextFieldSetInsertionPosition(WIDGET(This->PBox->text_field), 
				 (XmTextPosition)strlen(name));
 Xc_free(name);
}

/* ----------------------------------------------------------------- ** 
** Del_SItem - Delete the Style Selection                            ** 
** ----------------------------------------------------------------- */
static void Del_SItem(w, This)
Widget		w;
c_Box_styles	*This;
{
 c_TextStyle	**style;
 int		num, i;
 c_TextStyle	*item;
  
 if(F(This->PBox->ItemList).getSelection(This->PBox->ItemList,
					 (void ***)&style, &num))
 {
  item = 
   F(This->PBox->ItemList).getFirstUnselectItem(This->PBox->ItemList);
  for (i = 0; i < num; i++)
  {
   if (XcH_USAGE(style[i]) == 0 &&
       This->PBox->ItemList->item_count > 1)
   {
    F(This->PBox->ItemList).deleteItems(This->PBox->ItemList,
					XcIL_POINTER_ARRAY, 1,
					(void **)&(style[i]));  
    F(This->list_save).sub(This->list_save, style[i]);
   }
  }
  if (num)
   Xc_free(style);
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
static void Change_Sel(this, box_styles, style)
c_ASLBox	*this;
c_Box_styles	*box_styles;
c_TextStyle	*style;
{
 c_Color	*tcolor;
 c_VectorFont	*tfont;
 coord_t	tpoint;
 unsigned int	attributes;
 scale_t	tscale;
 boolean	tboolean;
 enum textstyle_underline_e ttype;


 if (box_styles->current_part == 0)
 {
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_FONT, &tfont) == -1)
   tfont = style->font;
  F(box_styles->IMfont).setSelection
   (box_styles->IMfont, tfont->spec_family);

  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_COLOR, &tcolor) == -1)
   tcolor = style->color;
  F(box_styles->CLfont).setColor(box_styles->CLfont, tcolor);
 
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_SIZE, &tpoint) == -1)
   tpoint = style->point_size;
  F(WTEXTFIELD(box_styles->SBfontsize)).set_value_in_unit
   (WTEXTFIELD(box_styles->SBfontsize), SCALE_TO_POINTS(tpoint));

  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_ATTRIB, &attributes) == -1)
   attributes = style->attributes;
  /*--- set all icone ---*/
  XmToggleButtonSetState(WIDGET(box_styles->Texpand),
			 (attributes & XcTS_B_WIDTH)==XcTS_B_WIDTH, False);
  XmToggleButtonSetState(WIDGET(box_styles->Ibold),
			 (attributes & XcTS_B_BOLD)==XcTS_B_BOLD, False);
  XmToggleButtonSetState(WIDGET(box_styles->Iitalic),
			 (attributes & XcTS_B_ITALIC)==XcTS_B_ITALIC, False);
  XmToggleButtonSetState(WIDGET(box_styles->Iunderline),
			 (attributes & XcTS_B_UNDERLINE)==XcTS_B_UNDERLINE,
			 False);
  XmToggleButtonSetState(WIDGET(box_styles->Istrikeout),
			 (attributes & XcTS_B_STRIKEOUT)==XcTS_B_STRIKEOUT,
			 False);
  XmToggleButtonSetState(WIDGET(box_styles->Tsubscript),
			 (attributes & XcTS_B_SUBSCRIPT)==XcTS_B_SUBSCRIPT,
			 False);
  XmToggleButtonSetState(WIDGET(box_styles->Tsupscript),
			 (attributes & XcTS_B_SUPSCRIPT)==XcTS_B_SUPSCRIPT,
			 False);
  XmToggleButtonSetState(WIDGET(box_styles->Ishadow),
			 (attributes & XcTS_B_SHADOW)==XcTS_B_SHADOW, False);
  XmToggleButtonSetState(WIDGET(box_styles->Tsmallcaps),
			 (attributes & XcTS_B_SMALL_CAPS)==XcTS_B_SMALL_CAPS,
			 False);
  XmToggleButtonSetState(WIDGET(box_styles->Tbigcaps),
			 (attributes & XcTS_B_BIG_CAPS)==XcTS_B_BIG_CAPS,
			 False);
 }
 else
 {
  /*--- attributes ---*/
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_ATTRIB, &attributes) == -1)
   attributes = style->attributes;
  /*--- width parameter ---*/ 
  F(box_styles->Fwidth).Toggle_Set(box_styles->Fwidth, 
				   (attributes & XcTS_B_WIDTH)==XcTS_B_WIDTH,
				   False);
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_WIDTH, &tscale) == -1)
   tscale = style->width.scale;
  F(box_styles->TFwidth).set_value_in_unit
   (box_styles->TFwidth, SCALE_TO_PERCENT(tscale));

  /*--- underline parameters ---*/ 
  F(box_styles->Funderline).Toggle_Set(box_styles->Funderline, 
				       (attributes & 
					XcTS_B_UNDERLINE)==XcTS_B_UNDERLINE,
				       False);
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_UDY, &tscale) == -1)
   tscale = style->underline.dy;
  F(box_styles->TFun_dy).set_value_in_unit
   (box_styles->TFun_dy, SCALE_TO_PERCENT(tscale));
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_UTHICK, &tscale) == -1)
   tscale = style->underline.thickness;
  F(box_styles->TFun_thickness).set_value_in_unit
   (box_styles->TFun_thickness, SCALE_TO_PERCENT(tscale));
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_UCOLOR, &tcolor) == -1)
   tcolor = style->underline.color;
  F(box_styles->CLunderline).setColor
   (box_styles->CLunderline, tcolor);
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_UTYPE, &ttype) == -1)
   ttype = style->underline.type;
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_UOVER, &tboolean) == -1)
   tboolean = style->underline.overspace;
  F(box_styles->OMun_type).setSelection(box_styles->OMun_type, 
					ttype*2+tboolean+1);

  /*--- strikeout parameters ---*/ 
  F(box_styles->Fstrikeout).Toggle_Set(box_styles->Fstrikeout, 
				       (attributes & 
					XcTS_B_STRIKEOUT)==XcTS_B_STRIKEOUT,
				       False);
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_SDY, &tscale) == -1)
   tscale = style->strikeout.dy;
  F(box_styles->TFst_dy).set_value_in_unit
   (box_styles->TFst_dy, SCALE_TO_PERCENT(-tscale));
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_STHICK, &tscale) == -1)
   tscale = style->strikeout.thickness;
  F(box_styles->TFst_thickness).set_value_in_unit
   (box_styles->TFst_thickness, SCALE_TO_PERCENT(tscale));
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_SCOLOR, &tcolor) == -1)
   tcolor = style->strikeout.color;
  F(box_styles->CLstrikeout).setColor
   (box_styles->CLstrikeout, tcolor);
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_STYPE, &ttype) == -1)
   ttype = style->strikeout.type;
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_SOVER, &tboolean) == -1)
   tboolean = style->strikeout.overspace;
  F(box_styles->OMst_type).setSelection(box_styles->OMst_type, 
					ttype*2+tboolean+1);
  /*--- shadow parameters ---*/
  F(box_styles->Fshadow).Toggle_Set(box_styles->Fshadow, 
				    (attributes & 
				     XcTS_B_SHADOW)==XcTS_B_SHADOW,
				    False);
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_SHDX, &tscale) == -1)
   tscale = style->shadow.dx;
  F(box_styles->TFsh_dx).set_value_in_unit
   (box_styles->TFsh_dx, SCALE_TO_PERCENT(tscale));
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_SHDY, &tscale) == -1)
   tscale = style->shadow.dy;
  F(box_styles->TFsh_dy).set_value_in_unit
   (box_styles->TFsh_dy, SCALE_TO_PERCENT(tscale));
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_SHCOLOR, &tcolor) == -1)
   tcolor = style->shadow.color;
  F(box_styles->CLshadow).setColor
   (box_styles->CLshadow, tcolor);
  /*--- subscript parameters ---*/
  F(box_styles->Fsubscript).Toggle_Set(box_styles->Fsubscript, 
				       (attributes & 
					XcTS_B_SUBSCRIPT)==XcTS_B_SUBSCRIPT,
				       False);
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_SUBDY, &tscale) == -1)
   tscale = style->subscript.dy;
  F(box_styles->TFsub_dy).set_value_in_unit
   (box_styles->TFsub_dy, SCALE_TO_PERCENT(-tscale));
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_SUBSCALE, &tscale) == -1)
   tscale = style->subscript.scale;
  F(box_styles->TFsub_scale).set_value_in_unit
   (box_styles->TFsub_scale, SCALE_TO_PERCENT(tscale));
  /*--- superscript parameters ---*/
  F(box_styles->Fsupscript).Toggle_Set(box_styles->Fsupscript, 
				       (attributes & 
					XcTS_B_SUPSCRIPT)==XcTS_B_SUPSCRIPT,
				       False);
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_SUPDY, &tscale) == -1)
   tscale = style->supscript.dy;
  F(box_styles->TFsup_dy).set_value_in_unit
   (box_styles->TFsup_dy, SCALE_TO_PERCENT(tscale));
  if (F(box_styles->list_save).get(box_styles->list_save,
				   style, SAVE_SUPSCALE, &tscale) == -1)
   tscale = style->supscript.scale;
  F(box_styles->TFsup_scale).set_value_in_unit
   (box_styles->TFsup_scale, SCALE_TO_PERCENT(tscale));
 }
 if (box_styles->WDApreview)
  draw_preview(box_styles->WDApreview, box_styles);


}

/* ----------------------------------------------------------------- ** 
** change color                                                      ** 
** ----------------------------------------------------------------- */
static void change_color(This, Obj, color)
c_ColorList	*This;
c_Box_styles	*Obj;
c_Color		*color;
{
 c_TextStyle	**style;
 int		num2, i;

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_COLOR,
			 &color);
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change font                                                       ** 
** ----------------------------------------------------------------- */
static void change_fonte(This, Obj, family)
c_ItemMenu	*This;
c_Box_styles	*Obj;
family_tree	*family;
{
 c_VectorFont	*font;
 font_list	*fontl;
 c_TextStyle	**style;
 int		num2, i;
 boolean	bold;
 boolean	italic; 
 
 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
  {
   fontl = style[i]->font->specific;
   
   bold = italic = FALSE;
   if (fontl->is_bold)
    bold = TRUE;
   if (fontl->is_italic)
    italic = TRUE;
   if (fontl->is_bold_italic)
    bold = italic = TRUE;
   
   fontl = family->list;
   if (family->can_bold_italic && bold && italic)
   {
    while(fontl && !fontl->is_bold_italic)
     fontl = fontl->Next;
   }
   else
   {
    if (family->can_bold && bold)
    {
     while(fontl && !fontl->is_bold)
      fontl = fontl->Next;
    }
    else
    {
     if (family->can_italic && italic)
     {
      while(fontl && !fontl->is_italic)
       fontl = fontl->Next;
     }
     else
     {
      while(fontl && !fontl->is_normal)
       fontl = fontl->Next;
     }
    }
   }
   if (fontl == NULL)
    font = family->list->font;
   else
    font = fontl->font;
   
   F(Obj->list_save).set(Obj->list_save, style[i], SAVE_FONT, &font);
  }
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change width                                                      ** 
** ----------------------------------------------------------------- */
static void certify_width(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle		**style;
 scale_t		numf, numo;
 int			num2, i;

 numf = (scale_t)F(this).get_value_in_unit(this);
 F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_WIDTH, &numo) == -1)
  numo = ((c_TextStyle *)Obj->PBox->edited_item)->width.scale;
 if (numf != numo)
  F(Obj->Fwidth).Toggle_Set(Obj->Fwidth, True, True);

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_WIDTH,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_WIDTH,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** change font size                                                  ** 
** ----------------------------------------------------------------- */
static void certify_fontsize(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 coord_t	numf;
 int		num2, i;

 numf = ((coord_t)F(this).get_value_in_unit(this)) & COORD_FONTSIZE_MASK;
 F(WTEXTFIELD(Obj->SBfontsize)).set_value_in_unit(WTEXTFIELD(Obj->SBfontsize), 
						  SCALE_TO_POINTS(numf));

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_SIZE,
			 &numf);
  }
  else
  {	
   for(i = 0; i < num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_SIZE,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** box action callback                                               ** 
** ----------------------------------------------------------------- */
static void cb_action(cb_dialog, box, cb_reason)
c_CustomDialog	*cb_dialog;
c_Box_styles	*box;
int		cb_reason;
{
#ifdef Xc_XQUAD
 c_TextStyle		*style, *style2;
  
 *(box->retptr) = NULL;
 switch(cb_reason)
 {
 case XcCD_APPLY:
  F(box->list_save).change_all2(box->list_save);
  style = (c_TextStyle *)box->PBox->edited_item;
  style2 = *box->Style_base;
  while(style2 && style2 != style)
   style2 = style2->next;
  if (style2)
   style = style2;
  else
  {
   c_VectorFont	*tfont1;
   coord_t	tpoint1;
   scale_t	tscale1;
   scale_t	tscale2;
   scale_t	tscale3;
   c_Color	*tcolor1;
   enum textstyle_underline_e ttype1;
   boolean       tboolean1;
   unsigned int	tattrib1;

   /*--- set color,font,size parameter ---*/
   if (F(box->list_save).get(box->list_save, style, SAVE_COLOR,
			     &tcolor1) == -1)
    tcolor1 = style->color;
   if (F(box->list_save).get(box->list_save, style, SAVE_FONT, 
			     &tfont1) == -1)
    tfont1 = style->font;
   if (F(box->list_save).get(box->list_save, style, SAVE_SIZE, 
			     &tpoint1) == -1)
    tpoint1 = style->point_size;
   F(style).set(style, XcTS_FONT, tfont1,
		XcTS_COLOR, tcolor1,
		XcTS_SIZE, tpoint1, XcTS_END);

   /*--- set width parameter ---*/
   if (F(box->list_save).get(box->list_save, style, SAVE_WIDTH,
			     &tscale1) == -1)
    tscale1 = style->width.scale;
   F(style).set(style, XcTS_WIDTH_CONF, tscale1, XcTS_END);

   /*--- set underline parameters ---*/
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_UDY, &tscale1) == -1)
    tscale1 = style->underline.dy;
   tscale2 = style->underline.overstroke;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_UTHICK, &tscale3) == -1)
    tscale3 = style->underline.thickness;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_UCOLOR, &tcolor1) == -1)
    tcolor1 = style->underline.color;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_UTYPE, &ttype1) == -1)
    ttype1 = style->underline.type;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_UOVER, &tboolean1) == -1)
    tboolean1 = style->underline.overspace;
   F(style).set(style, XcTS_UNDERLINE_CONF, tscale1, tscale2,
		tscale3, tboolean1, tcolor1, ttype1, XcTS_END);

   /*--- strikeout parameters ---*/ 
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_SDY, &tscale1) == -1)
    tscale1 = style->strikeout.dy;
   tscale2 = style->strikeout.overstroke;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_STHICK, &tscale3) == -1)
    tscale3 = style->strikeout.thickness;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_SCOLOR, &tcolor1) == -1)
    tcolor1 = style->strikeout.color;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_STYPE, &ttype1) == -1)
    ttype1 = style->strikeout.type;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_SOVER, &tboolean1) == -1)
    tboolean1 = style->strikeout.overspace;
   F(style).set(style, XcTS_STRIKEOUT_CONF, tscale1, tscale2,
		tscale3, tboolean1, tcolor1, ttype1, XcTS_END);

   /*--- shadow parameters ---*/
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_SHDX, &tscale1) == -1)
    tscale1 = style->shadow.dx;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_SHDY, &tscale2) == -1)
    tscale2 = style->shadow.dy;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_SHCOLOR, &tcolor1) == -1)
    tcolor1 = style->shadow.color;
   F(style).set(style, XcTS_SHADOW_CONF, tscale1, tscale2,
		tcolor1, XcTS_END);

   /*--- subscript parameters ---*/
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_SUBDY, &tscale1) == -1)
    tscale1 = style->subscript.dy;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_SUBSCALE, &tscale2) == -1)
    tscale2 = style->subscript.scale;
   F(style).set(style, XcTS_SUBSCRIPT_CONF, tscale1, tscale2,
		XcTS_END);

   /*--- superscript parameters ---*/
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_SUPDY, &tscale1) == -1)
    tscale1 = style->supscript.dy;
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_SUPSCALE, &tscale2) == -1)
    tscale2 = style->supscript.scale;
   F(style).set(style, XcTS_SUPSCRIPT_CONF, tscale1, tscale2,
		XcTS_END);

   /*--- set attributes ---*/
   if (F(box->list_save).get(box->list_save,
			     style, SAVE_ATTRIB, &tattrib1) == -1)
    tattrib1 = style->attributes;
   F(style).set(style, XcTS_ATTRIB, tattrib1, XcTS_END);

   style2 = *box->Style_base;
   while(style2 && !F(style).compare(style, style2, CMP_WITH_NAME))
   {
    /*printf("style2: %s\n", style2->name);*/
    style2 = style2->next;
   }
   if (style2)
    style = style2;
   else
    style = NULL;
  }
  *(box->retptr) = style;
  break;
 case XcCD_OK:
  F(box->list_save).change_all2(box->list_save);
  break;
 default:
  Xc_TRACE(("canceled"));
 }
#endif

#ifdef Xc_XCLAMATION
 if (cb_reason == XcCD_OK)
  F(box->list_save).change_all2(box->list_save);
#ifndef NTRACE
 else
  Xc_TRACE(("canceled"));
#endif
#endif

#ifdef Xc_XALLWRITE
 box->close_reason = cb_reason; 
#ifndef NTRACE
 if (cb_reason == XcCD_OK)
  Xc_TRACE(("OK"));
# if defined(Xc_XQUAD) || defined(Xc_XALLWRITE)
 else if (cb_reason == XcCD_APPLY)
  Xc_TRACE(("APPLY"));
# endif
 else
  Xc_TRACE(("canceled"));
#endif
#endif /* for XALLWRITE */

 F(cb_dialog).unmap(cb_dialog);
}

/* ----------------------------------------------------------------- ** 
** change all callback                                               ** 
** ----------------------------------------------------------------- */
static void cb_change(box, style, type, num, data)
c_Box_styles	*box;
c_TextStyle	*style;
int		type;
int		num;
void		*data;
{
 switch (type)
 {
 case NEWSET:
  style = box->modif.style;
 case TOSET:
  Xc_TRACE(("TOSET!"));
  switch(num)
  {
  case SAVE_COLOR:
   box->modif.color = ((c_Color **)data)[0];
   break;
  case SAVE_FONT:
   box->modif.font = ((c_VectorFont **)data)[0];
   break;
  case SAVE_SIZE:
   box->modif.size = ((coord_t *)data)[0];
   break;
  case SAVE_ATTRIB:
   box->modif.attributes = ((unsigned int *)data)[0];
   break;
  case SAVE_WIDTH:
   box->modif.width_scale = ((scale_t *)data)[0];
   break;
  case SAVE_UDY:
   box->modif.u_dy = ((scale_t *)data)[0];
   break;
  case SAVE_UTHICK:
   box->modif.u_thick = ((scale_t *)data)[0];
   break;
  case SAVE_UCOLOR:
   box->modif.u_color = ((c_Color **)data)[0];
   break;
  case SAVE_UTYPE:
   box->modif.u_type = ((enum textstyle_underline_e *)data)[0];
   break;
  case SAVE_UOVER:
   box->modif.u_over = ((boolean *)data)[0];
   break;
  case SAVE_SDY:
   box->modif.s_dy = ((scale_t *)data)[0];
   break;
  case SAVE_STHICK:
   box->modif.s_thick = ((scale_t *)data)[0];
   break;
  case SAVE_SCOLOR:
   box->modif.s_color = ((c_Color **)data)[0];
   break;
  case SAVE_STYPE:
   box->modif.s_type = ((enum textstyle_underline_e *)data)[0];
   break;
  case SAVE_SOVER:
   box->modif.s_over = ((boolean *)data)[0];
   break;
  case SAVE_SHDX:
   box->modif.sh_dx = ((scale_t *)data)[0];
   break;
  case SAVE_SHDY:
   box->modif.sh_dy = ((scale_t *)data)[0];
   break;
  case SAVE_SHCOLOR:
   box->modif.sh_color = ((c_Color **)data)[0];
   break;
  case SAVE_SUBDY:
   box->modif.sub_dy = ((scale_t *)data)[0];
   break;
  case SAVE_SUBSCALE:
   box->modif.sub_scale = ((scale_t *)data)[0];
   break;
  case SAVE_SUPDY:
   box->modif.sup_dy = ((scale_t *)data)[0];
   break;
  case SAVE_SUPSCALE:
   F(style).set(style,
		XcTS_COLOR, box->modif.color,
		XcTS_FONT, box->modif.font,
		XcTS_SIZE, box->modif.size,
		XcTS_ATTRIB, box->modif.attributes,
		XcTS_WIDTH_CONF, box->modif.width_scale,
		XcTS_UNDERLINE_CONF, box->modif.u_dy, 
		style->underline.overstroke, box->modif.u_thick,
		box->modif.u_over, box->modif.u_color, box->modif.u_type,
		XcTS_STRIKEOUT_CONF, box->modif.s_dy, 
		style->strikeout.overstroke, box->modif.s_thick,
		box->modif.s_over, box->modif.s_color, box->modif.s_type,
		XcTS_SHADOW_CONF, box->modif.sh_dx,
		box->modif.sh_dy, box->modif.sh_color,
		XcTS_SUBSCRIPT_CONF, box->modif.sub_dy, box->modif.sub_scale,
		XcTS_SUPSCRIPT_CONF, box->modif.sup_dy,	((scale_t *)data)[0], 
		XcTS_END);
   break;
  }
  break;
 case TOADD:
  Xc_TRACE(("TOADD!"));
  box->modif.style = (c_TextStyle *)NEW(c_TextStyle)(box->Style_base, 
						     style->name,
						     style->font,
						     style->point_size,
						     style->color);
  break;
 case TOSUB:
  Xc_TRACE(("TOSUB!"));
  DELETE(c_TextStyle)(style);
  break;
 }
}


/* ----------------------------------------------------------------- ** 
** style_filter - Filter styles                                      ** 
** ----------------------------------------------------------------- */
static boolean style_filter(this)
c_TextStyle *this;
{
 return !this->hide_flag;
}

/* ----------------------------------------------------------------- ** 
** change_width                                                      **
** ----------------------------------------------------------------- */
static void change_width(wThis, Obj)
Widget		wThis;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 int		num2, i;
 unsigned int	attributes;

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     style[i], SAVE_ATTRIB, &attributes) == -1)
    attributes = style[i]->attributes;
   attributes ^= XcTS_B_WIDTH;
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_ATTRIB,
			 &attributes);
  }
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change_bold                                                       **
** ----------------------------------------------------------------- */
static void change_bold(wThis, Obj)
Widget		wThis;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 int		num2, i;
 unsigned int	attributes;

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     style[i], SAVE_ATTRIB, &attributes) == -1)
    attributes = style[i]->attributes;
   attributes ^= XcTS_B_BOLD;
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_ATTRIB,
			 &attributes);
  }
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change_italic                                                     **
** ----------------------------------------------------------------- */
static void change_italic(wThis, Obj)
Widget		wThis;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 int		num2, i;
 unsigned int	attributes;

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     style[i], SAVE_ATTRIB, &attributes) == -1)
    attributes = style[i]->attributes;
   attributes ^= XcTS_B_ITALIC;
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_ATTRIB,
			 &attributes);
  }
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change_underline                                                  **
** ----------------------------------------------------------------- */
static void change_underline(wThis, Obj)
Widget		wThis;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 int		num2, i;
 unsigned int	attributes;

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     style[i], SAVE_ATTRIB, &attributes) == -1)
    attributes = style[i]->attributes;
   attributes ^= XcTS_B_UNDERLINE;
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_ATTRIB,
			 &attributes);
  }
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change_strikeout                                                  **
** ----------------------------------------------------------------- */
static void change_strikeout(wThis, Obj)
Widget		wThis;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 int		num2, i;
 unsigned int	attributes;

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     style[i], SAVE_ATTRIB, &attributes) == -1)
    attributes = style[i]->attributes;
   attributes ^= XcTS_B_STRIKEOUT;
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_ATTRIB,
			 &attributes);
  }
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change_subscript                                                  **
** ----------------------------------------------------------------- */
static void change_subscript(wThis, Obj)
Widget		wThis;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 int		num2, i;
 unsigned int	attributes;

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     style[i], SAVE_ATTRIB, &attributes) == -1)
    attributes = style[i]->attributes;
   if ((attributes & (XcTS_B_SUBSCRIPT|XcTS_B_SUPSCRIPT)) == XcTS_B_SUPSCRIPT)
   {
    attributes &= ~XcTS_B_SUPSCRIPT;
    if (style[i] == Obj->PBox->edited_item)
    {
     F(Obj->Tsupscript).Set(Obj->Tsupscript, False, False);
     if (Obj->part2_created)
      F(Obj->Fsupscript).Toggle_Set(Obj->Fsupscript, False, False);
    }
   }
   attributes ^= XcTS_B_SUBSCRIPT;
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_ATTRIB,
			 &attributes);
  }
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change_supscript                                                  **
** ----------------------------------------------------------------- */
static void change_supscript(wThis, Obj)
Widget		wThis;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 int		num2, i;
 unsigned int	attributes;

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     style[i], SAVE_ATTRIB, &attributes) == -1)
    attributes = style[i]->attributes;
   if ((attributes & (XcTS_B_SUBSCRIPT|XcTS_B_SUPSCRIPT)) == XcTS_B_SUBSCRIPT)
   {
    attributes &= ~XcTS_B_SUBSCRIPT;
    if (style[i] == Obj->PBox->edited_item)
    {
     F(Obj->Tsubscript).Set(Obj->Tsubscript, False, False);
     if (Obj->part2_created)
      F(Obj->Fsubscript).Toggle_Set(Obj->Fsubscript, False, False);
    }
   }
   attributes ^= XcTS_B_SUPSCRIPT;
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_ATTRIB,
			 &attributes);
  }
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change_shadow                                                     **
** ----------------------------------------------------------------- */
static void change_shadow(wThis, Obj)
Widget		wThis;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 int		num2, i;
 unsigned int	attributes;

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     style[i], SAVE_ATTRIB, &attributes) == -1)
    attributes = style[i]->attributes;
   attributes ^= XcTS_B_SHADOW;
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_ATTRIB,
			 &attributes);
  }
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change_small_caps                                                 **
** ----------------------------------------------------------------- */
static void change_small_caps(wThis, Obj)
Widget		wThis;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 int		num2, i;
 unsigned int	attributes;
 
 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     style[i], SAVE_ATTRIB, &attributes) == -1)
    attributes = style[i]->attributes;
   if ((attributes & (XcTS_B_BIG_CAPS | XcTS_B_SMALL_CAPS)) == XcTS_B_BIG_CAPS)
   {
    attributes &= ~XcTS_B_BIG_CAPS;
    if (style[i] == Obj->PBox->edited_item)
     F(Obj->Tbigcaps).Set(Obj->Tbigcaps, False, False);
   }
   
   attributes ^= XcTS_B_SMALL_CAPS;
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_ATTRIB,
			 &attributes);
  }
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change_big_caps                                                   **
** ----------------------------------------------------------------- */
static void change_big_caps(wThis, Obj)
Widget		wThis;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 int		num2, i;
 unsigned int	attributes;

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     style[i], SAVE_ATTRIB, &attributes) == -1)
    attributes = style[i]->attributes;
   if ((attributes & (XcTS_B_SMALL_CAPS|XcTS_B_BIG_CAPS)) == XcTS_B_SMALL_CAPS)
   {
    attributes &= ~XcTS_B_SMALL_CAPS;
    if (style[i] == Obj->PBox->edited_item)
     F(Obj->Tsmallcaps).Set(Obj->Tsmallcaps, False, False);
   }
   attributes ^= XcTS_B_BIG_CAPS;
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_ATTRIB,
			 &attributes);
  }
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change underline color                                            ** 
** ----------------------------------------------------------------- */
static void change_underline_color(This, Obj, color)
c_ColorList	*This;
c_Box_styles	*Obj;
c_Color		*color;
{
 c_TextStyle	**style;
 int		num2, i;
 c_Color	*coloro;

 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_UCOLOR, &coloro) == -1)
  coloro = ((c_TextStyle *)Obj->PBox->edited_item)->underline.color;
 if (color != coloro)
  F(Obj->Funderline).Toggle_Set(Obj->Funderline, True, True);
 
 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_UCOLOR,
			 &color);
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change strikeout color                                            ** 
** ----------------------------------------------------------------- */
static void change_strikeout_color(This, Obj, color)
c_ColorList	*This;
c_Box_styles	*Obj;
c_Color		*color;
{
 c_TextStyle	**style;
 int		num2, i;
 c_Color	*coloro;

 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_SCOLOR, &coloro) == -1)
  coloro = ((c_TextStyle *)Obj->PBox->edited_item)->strikeout.color;
 if (color != coloro)
  F(Obj->Fstrikeout).Toggle_Set(Obj->Fstrikeout, True, True);
 
 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_SCOLOR,
			 &color);
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change shadow color                                               ** 
** ----------------------------------------------------------------- */
static void change_shadow_color(This, Obj, color)
c_ColorList	*This;
c_Box_styles	*Obj;
c_Color		*color;
{
 c_TextStyle	**style;
 int		num2, i;
 c_Color	*coloro;

 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_SHCOLOR, &coloro) == -1)
  coloro = ((c_TextStyle *)Obj->PBox->edited_item)->shadow.color;
 if (color != coloro)
  F(Obj->Fshadow).Toggle_Set(Obj->Fshadow, True, True);
 
 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  for(i = 0; i < num2; i++)
   F(Obj->list_save).set(Obj->list_save,
			 style[i],
			 SAVE_SHCOLOR,
			 &color);
  if (num2)
   Xc_free(style);
 }
 draw_preview(Obj->WDApreview, Obj);
}  

/* ----------------------------------------------------------------- ** 
** change underline dy                                               ** 
** ----------------------------------------------------------------- */
static void certify_un_dy(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 scale_t	numf, numo;
 int		num2, i;

 numf = (scale_t)F(this).get_value_in_unit(this);
 F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
 
 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_UDY, &numo) == -1)
  numo = ((c_TextStyle *)Obj->PBox->edited_item)->underline.dy;
 if (numf != numo)
  F(Obj->Funderline).Toggle_Set(Obj->Funderline, True, True);

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_UDY,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_UDY,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** change underline thickness                                        ** 
** ----------------------------------------------------------------- */
static void certify_un_thickness(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 scale_t	numf, numo;
 int		num2, i;

 numf = (scale_t)F(this).get_value_in_unit(this);
 F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_UTHICK, &numo) == -1)
  numo = ((c_TextStyle *)Obj->PBox->edited_item)->underline.thickness;
 if (numf != numo)
  F(Obj->Funderline).Toggle_Set(Obj->Funderline, True, True);

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_UTHICK,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_UTHICK,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}


/* ----------------------------------------------------------------- ** 
** change strikeout dy                                               ** 
** ----------------------------------------------------------------- */
static void certify_st_dy(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 scale_t	numf, numo;
 int		num2, i;

 numf = (scale_t)F(this).get_value_in_unit(this);
 F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
 numf = -numf;
 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_SDY, &numo) == -1)
  numo = ((c_TextStyle *)Obj->PBox->edited_item)->strikeout.dy;
 if (numf != numo)
  F(Obj->Fstrikeout).Toggle_Set(Obj->Fstrikeout, True, True);

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_SDY,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_SDY,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** change strikeout thickness                                        ** 
** ----------------------------------------------------------------- */
static void certify_st_thickness(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle		**style;
 scale_t		numf, numo;
 int			num2, i;

 numf = (scale_t)F(this).get_value_in_unit(this);
 F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_STHICK, &numo) == -1)
  numo = ((c_TextStyle *)Obj->PBox->edited_item)->strikeout.thickness;
 if (numf != numo)
  F(Obj->Fstrikeout).Toggle_Set(Obj->Fstrikeout, True, True);

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_STHICK,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_STHICK,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** change underline type                                             ** 
** ----------------------------------------------------------------- */
static void change_un_type(Obj, num)
c_Box_styles	*Obj;
int		num;
{
 c_TextStyle			**style;
 int				num2, i;
 enum textstyle_underline_e	ttype;
 boolean			toverspace;

 ttype = num/2;
 toverspace = num%2;
 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_UTYPE,
			 &ttype);
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_UOVER,
			 &toverspace);
  }
  else
  {
   for(i=0; i<num2; i++)
   {
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_UTYPE,
			  &ttype);
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_UOVER,
			  &toverspace);
   }
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 F(Obj->Funderline).Toggle_Set(Obj->Funderline, True, True);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** change strikeout type                                             ** 
** ----------------------------------------------------------------- */
static void change_st_type(Obj, num)
c_Box_styles	*Obj;
int		num;
{
 c_TextStyle			**style;
 int				num2, i;
 enum textstyle_underline_e	ttype;
 boolean			toverspace;

 ttype = num/2;
 toverspace = num%2;
 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_STYPE,
			 &ttype);
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_SOVER,
			 &toverspace);
  }
  else
  {
   for(i=0; i<num2; i++)
   {
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_STYPE,
			  &ttype);
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_SOVER,
			  &toverspace);
   }
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 F(Obj->Fstrikeout).Toggle_Set(Obj->Fstrikeout, True, True);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** change shadow dx                                                  ** 
** ----------------------------------------------------------------- */
static void certify_sh_dx(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 scale_t	numf, numo;
 int		num2, i;

 numf = (scale_t)F(this).get_value_in_unit(this);
 F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_SHDX, &numo) == -1)
  numo = ((c_TextStyle *)Obj->PBox->edited_item)->shadow.dx;
 if (numf != numo)
  F(Obj->Fshadow).Toggle_Set(Obj->Fshadow, True, True);

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_SHDX,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_SHDX,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** change shadow dy                                                  ** 
** ----------------------------------------------------------------- */
static void certify_sh_dy(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 scale_t	numf, numo;
 int		num2, i;

 numf = (scale_t)F(this).get_value_in_unit(this);
 F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
 
 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_SHDY, &numo) == -1)
  numo = ((c_TextStyle *)Obj->PBox->edited_item)->shadow.dy;
 if (numf != numo)
  F(Obj->Fshadow).Toggle_Set(Obj->Fshadow, True, True);

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_SHDY,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_SHDY,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** change subscript dy                                               ** 
** ----------------------------------------------------------------- */
static void certify_sub_dy(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 scale_t	numf, numo;
 int		num2, i;

 numf = (scale_t)F(this).get_value_in_unit(this);
 F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
 numf = -numf;
 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_SUBDY, &numo) == -1)
  numo = ((c_TextStyle *)Obj->PBox->edited_item)->subscript.dy;
 if (numf != numo)
 {
  F(Obj->Fsubscript).Toggle_Set(Obj->Fsubscript, True, True);
 }

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_SUBDY,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_SUBDY,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** change subscript scale                                            ** 
** ----------------------------------------------------------------- */
static void certify_sub_scale(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 scale_t	numf, numo;
 int		num2, i;

 numf = (scale_t)F(this).get_value_in_unit(this);
 F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_SUBSCALE, &numo) == -1)
  numo = ((c_TextStyle *)Obj->PBox->edited_item)->subscript.scale;
 if (numf != numo)
  F(Obj->Fsubscript).Toggle_Set(Obj->Fsubscript, True, True);

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_SUBSCALE,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_SUBSCALE,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** change superscript dy                                             ** 
** ----------------------------------------------------------------- */
static void certify_sup_dy(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 scale_t	numf, numo;
 int		num2, i;

 numf = (scale_t)F(this).get_value_in_unit(this);
 F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_SUPDY, &numo) == -1)
  numo = ((c_TextStyle *)Obj->PBox->edited_item)->supscript.dy;
 if (numf != numo)
  F(Obj->Fsupscript).Toggle_Set(Obj->Fsupscript, True, True);

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_SUPDY,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_SUPDY,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** change superscript scale                                          ** 
** ----------------------------------------------------------------- */
static void certify_sup_scale(this, Obj)
c_WTextField	*this;
c_Box_styles	*Obj;
{
 c_TextStyle	**style;
 scale_t	numf, numo;
 int		num2, i;

 numf = (scale_t)F(this).get_value_in_unit(this);
 F(this).set_value_in_unit(this, SCALE_TO_PERCENT(numf));
  
 if (F(Obj->list_save).get(Obj->list_save,
			   (c_TextStyle *)Obj->PBox->edited_item,
			   SAVE_SUPSCALE, &numo) == -1)
  numo = ((c_TextStyle *)Obj->PBox->edited_item)->supscript.scale;
 if (numf != numo)
  F(Obj->Fsupscript).Toggle_Set(Obj->Fsupscript, True, True);

 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&style, &num2))
 {
  if (num2 == 1 && ((void *)*style) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_TextStyle *)Obj->PBox->edited_item,
			 SAVE_SUPSCALE,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  style[i],
			  SAVE_SUPSCALE,
			  &numf);
  }
  if (num2)
   Xc_free(style);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
 draw_preview(Obj->WDApreview, Obj);
}

/* ----------------------------------------------------------------- ** 
** draw preview of font                                              ** 
** ----------------------------------------------------------------- */
static void draw_preview(w, Obj)
Widget		w;
c_Box_styles	*Obj;
{
 Display	*display;
 Window		window;
 GC		gc_paint;
 GC		gc_mask;
 unsigned long	mask;
 XGCValues	xgcv;
 c_TextStyle	*style;
 c_RasterFont	*rfont;
 vector_t	pos;

 c_VectorFont	*tfont;
 c_Color	*tcolor;
 coord_t	tpoint;

 scale_t	tscale1;
 scale_t	tscale2;
 scale_t	tscale3;
 c_Color	*tcolor1;
 enum textstyle_underline_e ttype1;
 boolean       tboolean1;
 unsigned int	tattrib1;

 c_TextStyle	*LocalTextStyleBase = NULL;
 c_TextStyle	*prev_style;

 style = Obj->PBox->edited_item;

 display = XtDisplay(w);
 window = XtWindow(w);


#if 0
 XmToggleButtonSetState(WIDGET(box_styles->Ibold),
			(attributes & XcTS_B_BOLD)==XcTS_B_BOLD, False);
 XmToggleButtonSetState(WIDGET(box_styles->Iitalic),
			(attributes & XcTS_B_ITALIC)==XcTS_B_ITALIC, False);
#endif

 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_FONT, &tfont) == -1)
  tfont = style->font;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_COLOR, &tcolor) == -1)
  tcolor = style->color;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_SIZE, &tpoint) == -1)
  tpoint = style->point_size;

 prev_style = (c_TextStyle *)NEW(c_TextStyle)(&LocalTextStyleBase,
					      "Style_preview",
					      tfont,
					      tpoint,
					      tcolor);
 /*--- set width parameter ---*/
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_WIDTH, &tscale1) == -1)
  tscale1 = style->width.scale;
 F(prev_style).set(prev_style, XcTS_WIDTH_CONF, tscale1, XcTS_END);

 /*--- set underline parameters ---*/
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_UDY, &tscale1) == -1)
  tscale1 = style->underline.dy;
 tscale2 = style->underline.overstroke;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_UTHICK, &tscale3) == -1)
  tscale3 = style->underline.thickness;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_UCOLOR, &tcolor1) == -1)
  tcolor1 = style->underline.color;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_UTYPE, &ttype1) == -1)
  ttype1 = style->underline.type;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_UOVER, &tboolean1) == -1)
  tboolean1 = style->underline.overspace;
 F(prev_style).set(prev_style, XcTS_UNDERLINE_CONF, tscale1, tscale2,
		   tscale3, tboolean1, tcolor1, ttype1, XcTS_END);

 /*--- strikeout parameters ---*/ 
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_SDY, &tscale1) == -1)
  tscale1 = style->strikeout.dy;
 tscale2 = style->strikeout.overstroke;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_STHICK, &tscale3) == -1)
  tscale3 = style->strikeout.thickness;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_SCOLOR, &tcolor1) == -1)
  tcolor1 = style->strikeout.color;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_STYPE, &ttype1) == -1)
  ttype1 = style->strikeout.type;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_SOVER, &tboolean1) == -1)
  tboolean1 = style->strikeout.overspace;
 F(prev_style).set(prev_style, XcTS_STRIKEOUT_CONF, tscale1, tscale2,
		   tscale3, tboolean1, tcolor1, ttype1, XcTS_END);

 /*--- shadow parameters ---*/
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_SHDX, &tscale1) == -1)
  tscale1 = style->shadow.dx;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_SHDY, &tscale2) == -1)
  tscale2 = style->shadow.dy;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_SHCOLOR, &tcolor1) == -1)
  tcolor1 = style->shadow.color;
 F(prev_style).set(prev_style, XcTS_SHADOW_CONF, tscale1, tscale2,
		   tcolor1, XcTS_END);

 /*--- subscript parameters ---*/
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_SUBDY, &tscale1) == -1)
  tscale1 = style->subscript.dy;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_SUBSCALE, &tscale2) == -1)
  tscale2 = style->subscript.scale;
 F(prev_style).set(prev_style, XcTS_SUBSCRIPT_CONF, tscale1, tscale2,
		   XcTS_END);

 /*--- superscript parameters ---*/
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_SUPDY, &tscale1) == -1)
  tscale1 = style->supscript.dy;
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_SUPSCALE, &tscale2) == -1)
  tscale2 = style->supscript.scale;
 F(prev_style).set(prev_style, XcTS_SUPSCRIPT_CONF, tscale1, tscale2,
		   XcTS_END);

 /*--- set attributes ---*/
 if (F(Obj->list_save).get(Obj->list_save,
			   style, SAVE_ATTRIB, &tattrib1) == -1)
  tattrib1 = style->attributes;
 F(prev_style).set(prev_style, XcTS_ATTRIB, tattrib1, XcTS_END);

 if (XtIsRealized(w) == False) 
 {
  DELETE(c_TextStyle)(prev_style);
  return;
 }

 mask = GCForeground;
 xgcv.foreground = tcolor->cell.pixel;
 gc_paint = XCreateGC(display, window,
		      mask, &xgcv);

 rfont = F(GlobFntMng).new_font(GlobFntMng, 
				prev_style->font,
				prev_style->metrics);

#ifdef Xc_XALLWRITE
 gc_mask = XCreateGC(display, window,
		     mask, &xgcv);
 F(rfont).setGC(display, window, gc_mask, gc_paint);
#else
 gc_mask = (GC)0;
 F(rfont).setGC(display, window, &(gc_mask), gc_paint);
#endif
 
 {
  Dimension width, height;
  
  XtVaGetValues(w, XmNwidth, &width, XmNheight, &height, NULL);
  
  pos.dx = (((int)width << COORD_SCALEBITS) - 
	    F(prev_style->metrics).textWidth(prev_style->metrics, 
					     "Aaà BbCcDdEeFf 123!", 19)) / 2;
  if (pos.dx < COORD_ZERO) pos.dx = COORD_ZERO;
					      
  pos.dy = (((int)height << COORD_SCALEBITS) - tpoint) / 2 + 
   prev_style->metrics->ascender;
  
 }
 
 XClearWindow(display, window);
 F(rfont).drawString(rfont, display, window,
		     gc_mask, gc_paint,
		     &pos, COORD_ZERO, COORD_ZERO,
		     "Aaà BbCcDdEeFf 123!", 19, prev_style);
 
 F(GlobFntMng).delete_font(GlobFntMng, rfont);
 
 XFreeGC(display, gc_paint);
#ifdef Xc_XALLWRITE
 XFreeGC(display, gc_mask);
#else
 if (gc_mask != (GC)0)
  XFreeGC(display, gc_mask);
#endif

 if (Obj->current_part == 0)
  set_overview(Obj, prev_style);

 DELETE(c_TextStyle)(prev_style);
}

/* ----------------------------------------------------------------- ** 
** change interface part                                             ** 
** ----------------------------------------------------------------- */
static void change_part(w, Obj)
Widget w;
c_Box_styles	*Obj;
{
 Dimension	width;
 Dimension	height;

 if (w == WIDGET(Obj->TBglobal))
 {
  F(Obj->TBglobal).Set(Obj->TBglobal, True, False);
  F(Obj->TBconfig).Set(Obj->TBconfig, False, False);
  if (Obj->current_part == 1)
  {
   XtUnmanageChild(WIDGET(Obj->Gconfig));
   Obj->current_part = 0;
   Change_Sel(Obj->PBox, Obj, Obj->PBox->edited_item);
   XtManageChild(WIDGET(Obj->Gglobal));
  }
 }
 if (w == WIDGET(Obj->TBconfig))
 {
  F(Obj->TBconfig).Set(Obj->TBconfig, True, False);
  F(Obj->TBglobal).Set(Obj->TBglobal, False, False);
  if (Obj->current_part == 0)
  {
   if (!Obj->part2_created)
   {
    XtVaGetValues(WIDGET(Obj->Gglobal),
		  XmNwidth, &width,
		  XmNheight, &height, NULL);
    create_part2(Obj, width, height);
   }
   XtUnmanageChild(WIDGET(Obj->Gglobal));
   Obj->current_part = 1;
   Change_Sel(Obj->PBox, Obj, Obj->PBox->edited_item);
   XtManageChild(WIDGET(Obj->Gconfig));
  }
 }
}

/* ----------------------------------------------------------------- ** 
** create interface part 2                                           ** 
** ----------------------------------------------------------------- */
static void create_part2(ObjTmp, width, height)
c_Box_styles	*ObjTmp;
Dimension	width;
Dimension	height;
{
 c_ManageWidget		*man_w;

 man_w = (c_ManageWidget *)NEW(c_ManageWidget)();

 ObjTmp->Gconfig = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Fmain),
					 NULL,
					 "gBSconfig", TRUE);
 XtVaSetValues(WIDGET(ObjTmp->Gconfig),
	       XmNwidth, width,
	       XmNheight, height, NULL);

 ObjTmp->Fwidth = NEW(c_Frame)(WIDGET(ObjTmp->Gconfig),
			       man_w, 
			       NULL, -1);
 F(ObjTmp->Fwidth).Add_Toggle(ObjTmp->Fwidth, man_w, "fBSwidth");
 F(ObjTmp->Fwidth).Toggle_Add_Callback(ObjTmp->Fwidth,
				       change_width, ObjTmp);

 ObjTmp->RCwidth = 
  (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Fwidth),
			man_w, 
			"rcBSwidth", TRUE);
 ObjTmp->Lwidth = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->RCwidth),
			  man_w,
			  "lBSwidth");
 ObjTmp->TFwidth = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->RCwidth),
				    man_w,
				    "tfBSwidth",
				    TF_REAL,
				    1.0, 1000.0,
				    "100", NULL, 5);
 F(ObjTmp->TFwidth).set_unit(ObjTmp->TFwidth,
			     XcUB_RATIO_PERCENTAGE,
			     NULL, TRUE, FALSE);
 F(ObjTmp->TFwidth).add_callback(ObjTmp->TFwidth,
				 (void *)certify_width,
				 ObjTmp);


 ObjTmp->Funderline = NEW(c_Frame)(WIDGET(ObjTmp->Gconfig),
				   man_w, 
				   NULL, -1);
 F(ObjTmp->Funderline).Add_Toggle(ObjTmp->Funderline, man_w, "fBSunderline");
 F(ObjTmp->Funderline).Toggle_Add_Callback(ObjTmp->Funderline,
					   change_underline, ObjTmp);
 ObjTmp->RCunderline = 
  (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Funderline),
			man_w, 
			"rcBSunderline", TRUE);
 ObjTmp->Lun_dy = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->RCunderline),
			  man_w,
			  "lBSunderline_dy");
 ObjTmp->TFun_dy = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->RCunderline),
				    man_w,
				    "tfBSun_dy",
				    TF_REAL,
				    0.0, 100.0,
				    "30", NULL, 4);
 F(ObjTmp->TFun_dy).set_unit(ObjTmp->TFun_dy,
			     XcUB_RATIO_PERCENTAGE,
			     NULL, TRUE, FALSE);
 F(ObjTmp->TFun_dy).add_callback(ObjTmp->TFun_dy,
				 (void *)certify_un_dy,
				 ObjTmp);
 ObjTmp->Lun_thickness = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->RCunderline),
			  man_w,
			  "lBSthickness");
 ObjTmp->TFun_thickness = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->RCunderline),
				    man_w,
				    "tfBSun_thickness",
				    TF_REAL,
				    1.0, 30.0,
				    "10", NULL, 4);
 F(ObjTmp->TFun_thickness).set_unit(ObjTmp->TFun_thickness,
				    XcUB_RATIO_PERCENTAGE,
				    NULL, TRUE, FALSE);
 F(ObjTmp->TFun_thickness).add_callback(ObjTmp->TFun_thickness,
					(void *)certify_un_thickness,
					ObjTmp);

 ObjTmp->CLunderline = NEW(c_ColorList)(WIDGET(ObjTmp->RCunderline),
					man_w,
					"clBSunderline", XcCL_UNDERLINE);
 F(ObjTmp->CLunderline).setCallback(ObjTmp->CLunderline,
				    (void *)change_underline_color, ObjTmp);
 F(ObjTmp->CLunderline).setColorBase(ObjTmp->CLunderline,
				     ObjTmp->colorbase, CL_SKIP_TRANSPARENCY);
 ObjTmp->OMun_type = 
  (c_OptionMenu *)NEW(c_OptionMenu)(WIDGET(ObjTmp->RCunderline),
				    man_w,
				    "omBSun_type",
				    ICONMODEON, 1,
				    (char *)underline_simple_xpm, 1,
				    (char *)underline_simple_unover_xpm, 2,
				    (char *)underline_double_xpm, 3,
				    (char *)underline_double_unover_xpm, 4,
				    (char *)underline_triple_xpm, 5,
				    (char *)underline_triple_unover_xpm, 6,
				    ICONMODEOFF, 1,
				    NULL);
 F(ObjTmp->OMun_type).setGlobalCallback(ObjTmp->OMun_type, 
					change_un_type, ObjTmp);

 ObjTmp->Fstrikeout = NEW(c_Frame)(WIDGET(ObjTmp->Gconfig),
				   man_w, 
				   NULL, -1);
 F(ObjTmp->Fstrikeout).Add_Toggle(ObjTmp->Fstrikeout, man_w, "fBSstrikeout");
 F(ObjTmp->Fstrikeout).Toggle_Add_Callback(ObjTmp->Fstrikeout,
					   change_strikeout, ObjTmp);
 ObjTmp->RCstrikeout = 
  (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Fstrikeout),
			man_w, 
			"rcBSstrikeout", TRUE);
 ObjTmp->Lst_dy = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->RCstrikeout),
			  man_w,
			  "lBSstrikeout_dy");
 ObjTmp->TFst_dy = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->RCstrikeout),
				    man_w,
				    "tfBSst_dy",
				    TF_REAL,
				    0.0, 100.0,
				    "50", NULL, 4);
 F(ObjTmp->TFst_dy).set_unit(ObjTmp->TFst_dy,
			     XcUB_RATIO_PERCENTAGE,
			     NULL, TRUE, FALSE);
 F(ObjTmp->TFst_dy).add_callback(ObjTmp->TFst_dy,
				 (void *)certify_st_dy,
				 ObjTmp);
 ObjTmp->Lst_thickness = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->RCstrikeout),
			  man_w,
			  "lBSthickness");
 ObjTmp->TFst_thickness = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->RCstrikeout),
				    man_w,
				    "tfBSst_thickness",
				    TF_REAL,
				    1.0, 20.0,
				    "10", NULL, 4);
 F(ObjTmp->TFst_thickness).set_unit(ObjTmp->TFst_thickness,
				    XcUB_RATIO_PERCENTAGE,
				    NULL, TRUE, FALSE);
 F(ObjTmp->TFst_thickness).add_callback(ObjTmp->TFst_thickness,
					(void *)certify_st_thickness,
					ObjTmp);
 ObjTmp->CLstrikeout = NEW(c_ColorList)(WIDGET(ObjTmp->RCstrikeout),
					man_w,
					"clBSstrikeout", XcCL_STRIKEOUT);
 F(ObjTmp->CLstrikeout).setCallback(ObjTmp->CLstrikeout,
				    (void *)change_strikeout_color, ObjTmp);
 F(ObjTmp->CLstrikeout).setColorBase(ObjTmp->CLstrikeout,
				     ObjTmp->colorbase, CL_SKIP_TRANSPARENCY);
 ObjTmp->OMst_type = 
  (c_OptionMenu *)NEW(c_OptionMenu)(WIDGET(ObjTmp->RCstrikeout),
				    man_w,
				    "omBSst_type",
				    ICONMODEON, 1,
				    (char *)strikeout_simple_xpm, 1,
				    (char *)strikeout_simple_unover_xpm, 2,
				    (char *)strikeout_double_xpm, 3,
				    (char *)strikeout_double_unover_xpm, 4,
				    (char *)strikeout_triple_xpm, 5,
				    (char *)strikeout_triple_unover_xpm, 6,
				    ICONMODEOFF, 1,
				    NULL);
 F(ObjTmp->OMst_type).setGlobalCallback(ObjTmp->OMst_type, 
					change_st_type, ObjTmp);


 ObjTmp->Fshadow = NEW(c_Frame)(WIDGET(ObjTmp->Gconfig),
				man_w, 
				NULL, -1);
 F(ObjTmp->Fshadow).Add_Toggle(ObjTmp->Fshadow, man_w, "fBSshadow");
 F(ObjTmp->Fshadow).Toggle_Add_Callback(ObjTmp->Fshadow,
					change_shadow, ObjTmp);
 ObjTmp->RCshadow = 
  (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Fshadow),
			man_w, 
			"rcBSshadow", TRUE);

 ObjTmp->LshadowX = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->RCshadow),
			  man_w,
			  "lBSshadow_dx");
 ObjTmp->TFsh_dx = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->RCshadow),
				    man_w,
				    "tfBSsh_dx",
				    TF_REAL,
				    -50.0, 50.0,
				    "30", NULL, 4);
 F(ObjTmp->TFsh_dx).set_unit(ObjTmp->TFsh_dx,
			     XcUB_RATIO_PERCENTAGE,
			     NULL, TRUE, FALSE);
 F(ObjTmp->TFsh_dx).add_callback(ObjTmp->TFsh_dx,
				 (void *)certify_sh_dx,
				 ObjTmp);
 ObjTmp->LshadowY = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->RCshadow),
			  man_w,
			  "lBSshadow_dy");
 ObjTmp->TFsh_dy = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->RCshadow),
				    man_w,
				    "tfBSsh_dy",
				    TF_REAL,
				    -50.0, 50.0,
				    "30", NULL, 4);
 F(ObjTmp->TFsh_dy).set_unit(ObjTmp->TFsh_dy,
			     XcUB_RATIO_PERCENTAGE,
			     NULL, TRUE, FALSE);
 F(ObjTmp->TFsh_dy).add_callback(ObjTmp->TFsh_dy,
				 (void *)certify_sh_dy,
				 ObjTmp);
 ObjTmp->CLshadow = NEW(c_ColorList)(WIDGET(ObjTmp->RCshadow),
				     man_w,
				     "clBSshadow", XcCL_SHADOW);
 F(ObjTmp->CLshadow).setCallback(ObjTmp->CLshadow,
				 (void *)change_shadow_color, ObjTmp);
 F(ObjTmp->CLshadow).setColorBase(ObjTmp->CLshadow,
				  ObjTmp->colorbase, CL_SKIP_TRANSPARENCY);

 ObjTmp->Fsubscript = NEW(c_Frame)(WIDGET(ObjTmp->Gconfig),
				   man_w, 
				   NULL, -1);
 F(ObjTmp->Fsubscript).Add_Toggle(ObjTmp->Fsubscript, man_w, "fBSsubscript");
 F(ObjTmp->Fsubscript).Toggle_Add_Callback(ObjTmp->Fsubscript,
					   change_subscript, ObjTmp);

 ObjTmp->RCsubscript = 
  (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Fsubscript),
			man_w, 
			"rcBSsubscript", FALSE);

 ObjTmp->Lsuby = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->RCsubscript),
			  man_w,
			  "lBSypos");

 ObjTmp->TFsub_dy = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->RCsubscript),
				    man_w,
				    "tfBSsub_dy",
				    TF_REAL,
				    1.0, 100.0,
				    "100", NULL, 5);
 F(ObjTmp->TFsub_dy).set_unit(ObjTmp->TFsub_dy,
			      XcUB_RATIO_PERCENTAGE,
			      NULL, TRUE, FALSE);
 F(ObjTmp->TFsub_dy).add_callback(ObjTmp->TFsub_dy,
				  (void *)certify_sub_dy,
				  ObjTmp);

 ObjTmp->Lsubscale = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->RCsubscript),
			  man_w,
			  "lBSscale");

 ObjTmp->TFsub_scale = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->RCsubscript),
				    man_w,
				    "tfBSsub_scale",
				    TF_REAL,
				    1.0, 100.0,
				    "100", NULL, 5);
 F(ObjTmp->TFsub_scale).set_unit(ObjTmp->TFsub_scale,
				 XcUB_RATIO_PERCENTAGE,
				 NULL, TRUE, FALSE);
 F(ObjTmp->TFsub_scale).add_callback(ObjTmp->TFsub_scale,
				     (void *)certify_sub_scale,
				     ObjTmp);

 ObjTmp->Fsupscript = NEW(c_Frame)(WIDGET(ObjTmp->Gconfig),
				   man_w, 
				   NULL, -1);
 F(ObjTmp->Fsupscript).Add_Toggle(ObjTmp->Fsupscript, man_w, "fBSsupscript");
 F(ObjTmp->Fsupscript).Toggle_Add_Callback(ObjTmp->Fsupscript,
					   change_supscript, ObjTmp);
 ObjTmp->RCsupscript = 
  (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Fsupscript),
			man_w, 
			"rcBSsupscript", FALSE);


 ObjTmp->Lsupy = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->RCsupscript),
			  man_w,
			  "lBSypos");
 ObjTmp->TFsup_dy = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->RCsupscript),
				    man_w,
				    "tfBSsup_dy",
				    TF_REAL,
				    1.0, 100.0,
				    "100", NULL, 5);
 F(ObjTmp->TFsup_dy).set_unit(ObjTmp->TFsup_dy,
			      XcUB_RATIO_PERCENTAGE,
			      NULL, TRUE, FALSE);
 F(ObjTmp->TFsup_dy).add_callback(ObjTmp->TFsup_dy,
				  (void *)certify_sup_dy,
				  ObjTmp);
 ObjTmp->Lsupscale = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->RCsupscript),
			  man_w,
			  "lBSscale");
 ObjTmp->TFsup_scale = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->RCsupscript),
				    man_w,
				    "tfBSsup_scale",
				    TF_REAL,
				    1.0, 100.0,
				    "100", NULL, 5);
 F(ObjTmp->TFsup_scale).set_unit(ObjTmp->TFsup_scale,
				 XcUB_RATIO_PERCENTAGE,
				 NULL, TRUE, FALSE);
 F(ObjTmp->TFsup_scale).add_callback(ObjTmp->TFsup_scale,
				     (void *)certify_sup_scale,
				     ObjTmp);


 F(ObjTmp->RCunderline).addColumn(ObjTmp->RCunderline, XcG_MAX_SIZE, NULL);
 F(ObjTmp->RCunderline).addColumn(ObjTmp->RCunderline, XcG_BEST_SIZE,
				  WIDGET(ObjTmp->Lun_dy), 30,
				  WIDGET(ObjTmp->TFun_dy), XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, 5,
				  WIDGET(ObjTmp->CLunderline), XcG_MAX_SIZE,
				  NULL);
 F(ObjTmp->RCunderline).addColumn(ObjTmp->RCunderline, XcG_MAX_SIZE, NULL);
 F(ObjTmp->RCunderline).addColumn(ObjTmp->RCunderline, XcG_BEST_SIZE,
				  WIDGET(ObjTmp->Lun_thickness), 30,
				  WIDGET(ObjTmp->TFun_thickness),XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, 5,
				  WIDGET(ObjTmp->OMun_type), XcG_BEST_SIZE,
				  NULL);
 F(ObjTmp->RCunderline).addColumn(ObjTmp->RCunderline, XcG_MAX_SIZE, NULL);
 
 F(ObjTmp->RCstrikeout).addColumn(ObjTmp->RCstrikeout, XcG_MAX_SIZE, NULL);
 F(ObjTmp->RCstrikeout).addColumn(ObjTmp->RCstrikeout, XcG_BEST_SIZE,
				  WIDGET(ObjTmp->Lst_dy), 30,
				  WIDGET(ObjTmp->TFst_dy), XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, 5,
				  WIDGET(ObjTmp->CLstrikeout), XcG_MAX_SIZE,
				  NULL);
 F(ObjTmp->RCstrikeout).addColumn(ObjTmp->RCstrikeout, XcG_MAX_SIZE, NULL);
 F(ObjTmp->RCstrikeout).addColumn(ObjTmp->RCstrikeout, XcG_BEST_SIZE,
				  WIDGET(ObjTmp->Lst_thickness), 30,
				  WIDGET(ObjTmp->TFst_thickness),XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, 5,
				  WIDGET(ObjTmp->OMst_type), XcG_BEST_SIZE,
				  NULL);
 F(ObjTmp->RCstrikeout).addColumn(ObjTmp->RCstrikeout, XcG_MAX_SIZE, NULL);
 


 F(ObjTmp->RCwidth).addColumn(ObjTmp->RCwidth, XcG_MAX_SIZE, NULL);
 F(ObjTmp->RCwidth).addColumn(ObjTmp->RCwidth, XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Lwidth), XcG_MAX_SIZE,
			      WIDGET(ObjTmp->TFwidth), XcG_BEST_SIZE,
			      NULL);
 F(ObjTmp->RCwidth).addColumn(ObjTmp->RCwidth, XcG_MAX_SIZE, NULL);


 F(ObjTmp->RCshadow).addColumn(ObjTmp->RCshadow, XcG_MAX_SIZE, NULL);
 F(ObjTmp->RCshadow).addColumn(ObjTmp->RCshadow, XcG_BEST_SIZE,
			       WIDGET(ObjTmp->LshadowX), 30,
			       WIDGET(ObjTmp->TFsh_dx), XcG_BEST_SIZE,
			       XcG_WIDGET_NULL, 5,
			       WIDGET(ObjTmp->CLshadow), XcG_MAX_SIZE,
			       NULL);
 F(ObjTmp->RCshadow).addColumn(ObjTmp->RCshadow, XcG_MAX_SIZE, NULL);
 F(ObjTmp->RCshadow).addColumn(ObjTmp->RCshadow, XcG_BEST_SIZE,
			       WIDGET(ObjTmp->LshadowY), 30,
			       WIDGET(ObjTmp->TFsh_dy),XcG_BEST_SIZE,
			       XcG_WIDGET_NULL, 5,
			       XcG_WIDGET_NULL, XcG_MAX_SIZE,
			       NULL);
 F(ObjTmp->RCshadow).addColumn(ObjTmp->RCshadow, XcG_MAX_SIZE, NULL);
			       
 F(ObjTmp->RCsubscript).addColumn(ObjTmp->RCsubscript, XcG_MAX_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  WIDGET(ObjTmp->Lsuby), XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  WIDGET(ObjTmp->Lsubscale), XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  NULL);
 F(ObjTmp->RCsubscript).addColumn(ObjTmp->RCsubscript, XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  WIDGET(ObjTmp->TFsub_dy), XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  WIDGET(ObjTmp->TFsub_scale), XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  NULL);

 F(ObjTmp->RCsupscript).addColumn(ObjTmp->RCsupscript, XcG_MAX_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  WIDGET(ObjTmp->Lsupy), XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  WIDGET(ObjTmp->Lsupscale), XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  NULL);
 F(ObjTmp->RCsupscript).addColumn(ObjTmp->RCsupscript, XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  WIDGET(ObjTmp->TFsup_dy), XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  WIDGET(ObjTmp->TFsup_scale), XcG_BEST_SIZE,
				  XcG_WIDGET_NULL, XcG_MAX_SIZE,
				  NULL);
 

 F(ObjTmp->Gconfig).addColumn(ObjTmp->Gconfig, XcG_MAX_SIZE,
			      WIDGET(ObjTmp->Funderline), XcG_MAX_SIZE,
			      XcG_WIDGET_NULL, 7,
			      WIDGET(ObjTmp->Fstrikeout), XcG_MAX_SIZE,
			      NULL);
 F(ObjTmp->Gconfig).addColumn(ObjTmp->Gconfig, XcG_MAX_SIZE,
			      WIDGET(ObjTmp->Fshadow), XcG_MAX_SIZE,
			      XcG_WIDGET_NULL, 7,
			      WIDGET(ObjTmp->Fwidth), XcG_MAX_SIZE,
			      NULL);
 F(ObjTmp->Gconfig).addColumn(ObjTmp->Gconfig, XcG_MAX_SIZE,
			      WIDGET(ObjTmp->Fsubscript), XcG_MAX_SIZE,
			      XcG_WIDGET_NULL, 7,
			      WIDGET(ObjTmp->Fsupscript), XcG_MAX_SIZE,
			      NULL);

 ObjTmp->part2_created = TRUE;

 F(man_w).Manage_all(man_w, WIDGET(ObjTmp->Gconfig));

 F(ObjTmp->Gconfig).callResize(ObjTmp->Gconfig);
 F(ObjTmp->RCsubscript).callResize(ObjTmp->RCunderline);
 F(ObjTmp->RCsubscript).callResize(ObjTmp->RCstrikeout);
 F(ObjTmp->RCsubscript).callResize(ObjTmp->RCshadow);
 F(ObjTmp->RCsubscript).callResize(ObjTmp->RCwidth);
 F(ObjTmp->RCsubscript).callResize(ObjTmp->RCsubscript);
 F(ObjTmp->RCsubscript).callResize(ObjTmp->RCsupscript);

}

/* ----------------------------------------------------------------- ** 
** set font overview                                                 ** 
** ----------------------------------------------------------------- */
static void set_overview(box_styles, style)
c_Box_styles	*box_styles;
c_TextStyle	*style;
{
 char str[400];
 char tmp_str[100];
 unsigned int	attributes;


 sprintf(tmp_str, "%s | %dpt | %s",
	 style->font->family_name,
	 (int)SCALE_TO_POINTS(style->point_size),
	 style->color->name );
 strcpy(str, tmp_str);
  
 attributes = style->attributes;
 
 /* bold */
 if ((attributes & XcTS_B_BOLD)==XcTS_B_BOLD)
 {
  strcat(str, " | ");
  strcat(str, F(GlobResources).getString(GlobResources, XcR_styleBold));
 }
 
 /* italic */
 if ((attributes & XcTS_B_ITALIC)==XcTS_B_ITALIC)
 {
  strcat(str, " | ");
  strcat(str, F(GlobResources).getString(GlobResources, XcR_styleItalic));
 }

 /* underline */
 if ((attributes & XcTS_B_UNDERLINE)==XcTS_B_UNDERLINE)
 {
  sprintf(tmp_str, " | %s[dY:%d%%, %s:%d%%, %s, ",
	  F(GlobResources).getString(GlobResources, XcR_styleUnderline),
	  (int)SCALE_TO_PERCENT(style->underline.dy), 
	  F(GlobResources).getString(GlobResources, XcR_styleThickness),
	  (int)SCALE_TO_PERCENT(style->underline.thickness), 
	  style->underline.color->name);
  strcat(str, tmp_str);

  switch (style->underline.type)
  {
  case 0:
   strcat(str, F(GlobResources).getString(GlobResources, XcR_styleSimple));
   break;
  case 1:
   strcat(str, F(GlobResources).getString(GlobResources, XcR_styleDouble));
   break;
  case 2:
   strcat(str, F(GlobResources).getString(GlobResources, XcR_styleTriple));
   break;
  }
  if (style->underline.overspace)
  {
   strcat(str, ", ");
   strcat(str, F(GlobResources).getString(GlobResources, XcR_styleNoSpace));
   strcat(str, "]");
  }
  else
   strcat(str, "]");
 }

 /* strike out */
 if ((attributes & XcTS_B_STRIKEOUT)==XcTS_B_STRIKEOUT)
 {
  sprintf(tmp_str, " | %s[dY:%d%%, %s:%d%%, %s, ",
	  F(GlobResources).getString(GlobResources, XcR_styleStrikeout),
	  (int)SCALE_TO_PERCENT(-(style->strikeout.dy)), 
	  F(GlobResources).getString(GlobResources, XcR_styleThickness),
	  (int)SCALE_TO_PERCENT(style->strikeout.thickness), 
	  style->strikeout.color->name);
  strcat(str, tmp_str);

  switch (style->strikeout.type)
  {
  case 0:
   strcat(str, F(GlobResources).getString(GlobResources, XcR_styleSimple));
   break;
  case 1:
   strcat(str, F(GlobResources).getString(GlobResources, XcR_styleDouble));
   break;
  case 2:
   strcat(str, F(GlobResources).getString(GlobResources, XcR_styleTriple));
   break;
  }

  if (style->strikeout.overspace)
  {
   strcat(str, ", ");
   strcat(str, F(GlobResources).getString(GlobResources, XcR_styleNoSpace));
   strcat(str, "]");
  }
  else
   strcat(str, "]");
 }

 /* Shadow */
 if ((attributes & XcTS_B_SHADOW)==XcTS_B_SHADOW)
 {
  sprintf(tmp_str, " | %s[dX:%d%%, dY:%d%%, %s]",
	  F(GlobResources).getString(GlobResources, XcR_styleShadow),
	  (int)SCALE_TO_PERCENT(style->shadow.dx), 
	  (int)SCALE_TO_PERCENT(style->shadow.dy), 
	  style->shadow.color->name);
  strcat(str, tmp_str);
 }
 
 /* width */
 if ((attributes & XcTS_B_WIDTH)==XcTS_B_WIDTH)
 {
  sprintf(tmp_str, " | %s: %d%%",
	  F(GlobResources).getString(GlobResources, XcR_styleWidth),
	  (int)SCALE_TO_PERCENT(style->width.scale));
  strcat(str, tmp_str);
 }

 /* subscript */
 if ((attributes & XcTS_B_SUBSCRIPT)==XcTS_B_SUBSCRIPT)
 {
  sprintf(tmp_str, " | %s[dY:%d%%, %s:%d%%]",
	  F(GlobResources).getString(GlobResources, XcR_styleSubscript),
	  (int)SCALE_TO_PERCENT(-(style->subscript.dy)), 
	  F(GlobResources).getString(GlobResources, XcR_styleScale),
	  (int)SCALE_TO_PERCENT(style->subscript.scale)); 
  strcat(str, tmp_str);
 }

 /* supscript */
 if ((attributes & XcTS_B_SUPSCRIPT)==XcTS_B_SUPSCRIPT)
 {
  sprintf(tmp_str, " | %s[dY:%d%%, %s:%d%%]",
	  F(GlobResources).getString(GlobResources, XcR_styleSupscript),
	  (int)SCALE_TO_PERCENT(style->supscript.dy), 
	  F(GlobResources).getString(GlobResources, XcR_styleScale),
	  (int)SCALE_TO_PERCENT(style->supscript.scale)); 
  strcat(str, tmp_str);
 }

 /* small caps */
 if ((attributes & XcTS_B_SMALL_CAPS)==XcTS_B_SMALL_CAPS)
 {
  strcat(str, " | ");
  strcat(str, F(GlobResources).getString(GlobResources, XcR_styleSmallCaps));
 }

 /* big caps */
 if ((attributes & XcTS_B_BIG_CAPS)==XcTS_B_BIG_CAPS)
 {
  strcat(str, " | ");
  strcat(str, F(GlobResources).getString(GlobResources, XcR_styleBigCaps));
 }

 XmTextSetString(box_styles->w_TextOverview, str);
}
