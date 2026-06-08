/*
** Box_color.c for Xclamation, XQuad and XAllWrite in Box_color/
** Object and method for Box_color Class
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
** Started on  Mon Dec  5 16:33:17 1994 Robin Castanier
** Last update Tue Jan 18 11:19:26 2000 Emmanuel Paris
*/

/*--- include ---*/
#include "Box_color.h"
#include "Color.h"
#ifdef Xc_XALLWRITE
#include "Undo.h"
#endif

#undef NTRACE

/*--- position for modif list ---*/
#define SAVE_MODE	0
#define SAVE_CMYK	1
#define SAVE_RGB	2
#define SAVE_HSL	3
#define SAVE_YCBCR	4
#define SAVE_GRAY	5
#define SAVE_LAST	6

/*--- prototypes ---*/
void *cons_Box_color();
void dest_Box_color();
void *copy_Box_color();

static void Add_Item();
static void Del_SItem();
static void Change_Sel();
static int convertCMYK ___PROTO((int value));
static int invert_convertCMYK ___PROTO((int value));
static int convertH ___PROTO((int value));
static int invert_convertH ___PROTO((int value));
static void update_text_cmyk ___PROTO((Widget w, c_Box_color *Obj,
				       XmScaleCallbackStruct	*cbs));
static void update_text_rgb ___PROTO((Widget w, c_Box_color *Obj,
				      XmScaleCallbackStruct	*cbs));
static void update_text_hsl ___PROTO((Widget w, c_Box_color *Obj,
				      XmScaleCallbackStruct	*cbs));
static void update_text_gray ___PROTO((Widget w, c_Box_color *Obj,
				       XmScaleCallbackStruct	*cbs));
static void certify_text_cmyk ___PROTO((c_WTextField *this, c_Box_color *Obj));
static void certify_text_rgb ___PROTO((c_WTextField *this, c_Box_color *Obj));
static void certify_text_hsl ___PROTO((c_WTextField *this, c_Box_color *Obj));
static void certify_text_gray ___PROTO((c_WTextField *this, c_Box_color *Obj));
static void cb_action ___PROTO((c_CustomDialog	*cb_dialog,
				c_Box_color *box,
				int		cb_reason));
static void cb_change_C ___PROTO((c_Box_color	*box,
				  c_Color		*color,
				  int		type));
static boolean color_filter ___PROTO((c_Color *this));

static void change_part();
static void create_part_CMYK();
static void set_CMYK_part();
static void create_part_RGB();
static void set_RGB_part();
static void create_part_HSL();
static void set_HSL_part();
static void create_part_GRAY();
static void set_GRAY_part();
#ifdef __YCBCR__
static void create_part_YCBCR();
static void set_YCBCR_part();
#endif
static void HSLtoRGB();
static void RGBtoHSL();

/*--- fill class method ---*/
sf_Box_color fc_Box_color =
{
 cons_Box_color,
 dest_Box_color,
 copy_Box_color
};

/*--- external ---*/
extern c_Colormap *GlobColormap;

/* ----------------------------------------------------------------- ** 
** constructeur                                                      ** 
** ----------------------------------------------------------------- */
void *cons_Box_color(w_Parent, pszTitle, color_base)
Widget	w_Parent;
char	*pszTitle;
c_Color	**color_base;
{
 c_Box_color	*ObjTmp;
 Arg	        args[5]; 
 XmString      strTitre;
 c_Color	*color;
  
 if ((ObjTmp = (c_Box_color *)Xc_malloc("Box_color",
					sizeof(c_Box_color))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Box_color;

#ifdef Xc_XALLWRITE
 ObjTmp->close_reason = XcCD_CANCEL;
#endif
 ObjTmp->color_base = color_base;
 ObjTmp->v_color_base = NULL;
 ObjTmp->list_save = (c_ModifList *)
  NEW(c_ModifList)(ObjTmp, cb_change_C,
		   SAVE_LAST,
		   (int)(offsetof(c_Color, mode)),
		   (int)sizeof(color_set_code_t),
		   (int)(offsetof(c_Color, cmyk)),
		   (int)sizeof(cmyk_t),
		   (int)(offsetof(c_Color, rgb)),
		   (int)sizeof(rgb_t),
		   (int)(offsetof(c_Color, hsl)),
		   (int)sizeof(hsl_t),
		   (int)(offsetof(c_Color, ycbcr.y)),
		   (int)sizeof(int),
		   (int)(offsetof(c_Color, ycbcr)),
		   (int)sizeof(ycbcr_t));
 /*		   (int)(offsetof(c_Color, name)),
		   (int)XcMAX_NAME_LENGTH); */
 ObjTmp->PBox = (c_ASLBox *)NEW(c_ASLBox)(w_Parent,
					  pszTitle,
					  600, 402,
					  FALSE, 
					  offsetof(c_Color, name));

 strTitre = F(GlobResources).getXmString(GlobResources, XcR_editColors);
 XtSetArg(args[0], XmNlabelString, strTitre);
 XtSetValues(WIDGET(ObjTmp->PBox->Label), args, 1);
 XmStringFree(strTitre);
  
 F(ObjTmp->PBox).setCallback(ObjTmp->PBox, Change_Sel, ObjTmp);

 F(ObjTmp->PBox).Add_Buttons(ObjTmp->PBox, "lColorAdd", Add_Item,
			     ObjTmp,
			     NULL);
 F(ObjTmp->PBox).Add_Buttons(ObjTmp->PBox, "lColorDel", Del_SItem,
			     ObjTmp,
			     NULL);

 if (!color_base[0])
  NEW(c_Color)(color_base, "New_color");

 WIDGET(ObjTmp) = WIDGET(ObjTmp->PBox);
 ObjTmp->Gmain = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				       MANAGER(ObjTmp->PBox->Dialog),
				       "gBoxColors", FALSE);

 ObjTmp->Fonglet = NEW(c_Frame)(WIDGET(ObjTmp->Gmain),
				MANAGER(ObjTmp->PBox->Dialog), 
				NULL);
 XtSetArg(args[0], XmNshadowType, XmSHADOW_IN);
 XtSetArg(args[1], XmNshadowThickness, 1);
 XtSetValues(WIDGET(ObjTmp->Fonglet), args, 2);

 ObjTmp->Gonglet = (c_Grid *)NEW(c_Grid)
  (WIDGET(ObjTmp->Fonglet), MANAGER(ObjTmp->PBox->Dialog), "gBCBar", TRUE);
 XtVaSetValues(WIDGET(ObjTmp->Gonglet),
	       XmNmarginHeight, 0,
	       XmNmarginWidth, 0,
	       XmNspacing, 0,
	       NULL);
  
 ObjTmp->TBonglet[XcG_CMYK_PART] = (c_ToggleButton *)NEW(c_ToggleButton)
  (WIDGET(ObjTmp->Gonglet), MANAGER(ObjTmp->PBox->Dialog),
   "tbBC_CMYK");
 F(ObjTmp->TBonglet[XcG_CMYK_PART]).Add_Callback
  (ObjTmp->TBonglet[XcG_CMYK_PART], XmNvalueChangedCallback, 
   change_part, ObjTmp);
  
 XtVaSetValues(WIDGET(ObjTmp->TBonglet[XcG_CMYK_PART]),
	       XmNindicatorOn, False,
	       XmNindicatorSize, 1,
	       /*	       XmNheight, 20, */
	       XmNspacing, 0,
	       XmNhighlightThickness, 1,
	       XmNborderWidth, 0,
	       NULL);

 ObjTmp->TBonglet[XcG_RGB_PART] = (c_ToggleButton *)NEW(c_ToggleButton)
  (WIDGET(ObjTmp->Gonglet), MANAGER(ObjTmp->PBox->Dialog),
   "tbBC_RGB");

 F(ObjTmp->TBonglet[XcG_RGB_PART]).Add_Callback
  (ObjTmp->TBonglet[XcG_RGB_PART], XmNvalueChangedCallback, 
   change_part, ObjTmp);

  
 XtVaSetValues(WIDGET(ObjTmp->TBonglet[XcG_RGB_PART]),
	       XmNindicatorOn, False,
	       XmNindicatorSize, 1,
	       /*       XmNheight, 20, */
	       XmNspacing, 0,
	       XmNhighlightThickness, 1,
	       XmNborderWidth, 0,
	       NULL);

 ObjTmp->TBonglet[XcG_HSL_PART] = (c_ToggleButton *)NEW(c_ToggleButton)
  (WIDGET(ObjTmp->Gonglet), MANAGER(ObjTmp->PBox->Dialog),
   "tbBC_HSL");

 F(ObjTmp->TBonglet[XcG_HSL_PART]).Add_Callback
  (ObjTmp->TBonglet[XcG_HSL_PART], XmNvalueChangedCallback, 
   change_part, ObjTmp);

  
 XtVaSetValues(WIDGET(ObjTmp->TBonglet[XcG_HSL_PART]),
	       XmNindicatorOn, False,
	       XmNindicatorSize, 1,
	       /*       XmNheight, 20, */
	       XmNspacing, 0,
	       XmNhighlightThickness, 1,
	       XmNborderWidth, 0,
	       NULL);

 ObjTmp->TBonglet[XcG_GRAY_PART] = (c_ToggleButton *)NEW(c_ToggleButton)
  (WIDGET(ObjTmp->Gonglet), MANAGER(ObjTmp->PBox->Dialog),
   "tbBC_GRAY");

 F(ObjTmp->TBonglet[XcG_GRAY_PART]).Add_Callback
  (ObjTmp->TBonglet[XcG_GRAY_PART], XmNvalueChangedCallback, 
   change_part, ObjTmp);

  
 XtVaSetValues(WIDGET(ObjTmp->TBonglet[XcG_GRAY_PART]),
	       XmNindicatorOn, False,
	       XmNindicatorSize, 1,
	       /*       XmNheight, 20, */
	       XmNspacing, 0,
	       XmNhighlightThickness, 1,
	       XmNborderWidth, 0,
	       NULL);

#ifdef __YCBCR__
 ObjTmp->TBonglet[XcG_YCBCR_PART] = (c_ToggleButton *)NEW(c_ToggleButton)
  (WIDGET(ObjTmp->Gonglet), MANAGER(ObjTmp->PBox->Dialog),
   "tbBC_GRAY");

 F(ObjTmp->TBonglet[XcG_YCBCR_PART]).Add_Callback
  (ObjTmp->TBonglet[XcG_YCBCR_PART], XmNvalueChangedCallback, 
   change_part, ObjTmp);

 XtVaSetValues(WIDGET(ObjTmp->TBonglet[XcG_YCBCR_PART]),
	       XmNindicatorOn, False,
	       XmNindicatorSize, 1,
	       /*       XmNheight, 20, */
	       XmNspacing, 0,
	       XmNhighlightThickness, 1,
	       XmNborderWidth, 0,
	       NULL);
#endif

 F(ObjTmp->Gonglet).addColumn(ObjTmp->Gonglet, XcG_MAX_SIZE,
			      WIDGET(ObjTmp->TBonglet[XcG_CMYK_PART]),
			      XcG_MAX_SIZE,
			      WIDGET(ObjTmp->TBonglet[XcG_RGB_PART]), 
			      XcG_MAX_SIZE,
			      WIDGET(ObjTmp->TBonglet[XcG_HSL_PART]),
			      XcG_MAX_SIZE,
			      WIDGET(ObjTmp->TBonglet[XcG_GRAY_PART]),
			      XcG_MAX_SIZE,
#ifdef __YCBCR__
			      WIDGET(ObjTmp->TBonglet[XcG_YCBCR_PART]),
			      XcG_MAX_SIZE,
#endif
			      NULL);
 ObjTmp->Fmain = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Gmain),
					 MANAGER(ObjTmp->PBox->Dialog), 
					 "fBCmain");
 
 ObjTmp->F_color = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Gmain),
					   MANAGER(ObjTmp->PBox->Dialog), 
					   NULL);
 ObjTmp->L_color = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->F_color),
			  MANAGER(ObjTmp->PBox->Dialog),
			  "lColor");

 ObjTmp->G_void = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Fmain),
					MANAGER(ObjTmp->PBox->Dialog), 
					"gBC_void", TRUE);

 ObjTmp->current_part = XcG_NONE_PART;
 ObjTmp->part_created[XcG_CMYK_PART] = FALSE;
 ObjTmp->part_created[XcG_RGB_PART] = FALSE; 
 ObjTmp->part_created[XcG_HSL_PART] = FALSE;
 ObjTmp->part_created[XcG_GRAY_PART] = FALSE;
#ifdef YBCBCR
 ObjTmp->part_created[XcG_YCBCR_PART] = FALSE;
#endif

 F(ObjTmp->Gmain).addColumn(ObjTmp->Gmain, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Fonglet), 24,
			    WIDGET(ObjTmp->Fmain), XcG_MAX_SIZE,
			    XcG_WIDGET_NULL, 10,
			    WIDGET(ObjTmp->F_color), 50,
			    NULL);
  
 F(ObjTmp->PBox->ItemList).set(ObjTmp->PBox->ItemList, -1,
			       FALSE, -1L, -1, (void *)-1, 
			       (void *)color_filter);
 F(ObjTmp->PBox->ItemList).addItems(ObjTmp->PBox->ItemList, FALSE,
				    XcIL_LINKED_LIST, -1,
				    color_base[0], offsetof(c_Color, next));

 F(ObjTmp->PBox->Dialog).createActionArea(ObjTmp->PBox->Dialog,
					  XcCD_OK | XcCD_CANCEL, XcCD_OK,
					  TRUE, 
					  (void (*)())cb_action, ObjTmp);

 color = *color_base;
 while(color && (color->hide_flag || color->transparency))
  color = color->next;
 if (!color)
  color = *color_base;
  
 memcpy(&(ObjTmp->cmyk), &(color->cmyk), sizeof(cmyk_t));
 memcpy(&(ObjTmp->rgb), &(color->rgb), sizeof(rgb_t));
 memcpy(&(ObjTmp->hsl), &(color->hsl), sizeof(hsl_t));
 memcpy(&(ObjTmp->ycbcr), &(color->ycbcr), sizeof(ycbcr_t));

 F(ObjTmp->PBox).Map(ObjTmp->PBox, TRUE, NULL); 
 F(ObjTmp->PBox).selectItem(ObjTmp->PBox, color, TRUE);
 F(ObjTmp->PBox->ItemList).selectItem(ObjTmp->PBox->ItemList, color);

 Xc_TRACE(("Object Box_color build"));
 return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** destructeur                                                       ** 
** ----------------------------------------------------------------- */
void dest_Box_color(This)
c_Box_color *This;
{
 while (This->v_color_base != NULL)
  DELETE(c_Color)(This->v_color_base);

 DELETE(c_Label)(This->L_color);
 DELETE(c_Frame)(This->F_color);

 if (This->part_created[XcG_CMYK_PART])
 {
  DELETE(c_Label)(This->L_C);
  DELETE(c_Label)(This->L_M);
  DELETE(c_Label)(This->L_Y);
  DELETE(c_Label)(This->L_K);
  DELETE(c_WTextField)(This->V_C);
  DELETE(c_WTextField)(This->V_M);
  DELETE(c_WTextField)(This->V_Y);
  DELETE(c_WTextField)(This->V_K);
  DELETE(c_Scale)(This->S_C);
  DELETE(c_Scale)(This->S_M);
  DELETE(c_Scale)(This->S_Y);
  DELETE(c_Scale)(This->S_K);
  DELETE(c_Grid)(This->G_type[XcG_CMYK_PART]);
 }

 if (This->part_created[XcG_RGB_PART])
 {
  DELETE(c_Label)(This->L_R);
  DELETE(c_Label)(This->L_G);
  DELETE(c_Label)(This->L_B);
  DELETE(c_WTextField)(This->V_R);
  DELETE(c_WTextField)(This->V_G);
  DELETE(c_WTextField)(This->V_B);
  DELETE(c_Scale)(This->S_R);
  DELETE(c_Scale)(This->S_G);
  DELETE(c_Scale)(This->S_B);
  DELETE(c_Grid)(This->G_type[XcG_RGB_PART]);
 }

 if (This->part_created[XcG_HSL_PART])
 {
  DELETE(c_Label)(This->L_H);
  DELETE(c_Label)(This->L_S);
  DELETE(c_Label)(This->L_L);
  DELETE(c_WTextField)(This->V_H);
  DELETE(c_WTextField)(This->V_S);
  DELETE(c_WTextField)(This->V_L);
  DELETE(c_Scale)(This->S_H);
  DELETE(c_Scale)(This->S_S);
  DELETE(c_Scale)(This->S_L);
  DELETE(c_Grid)(This->G_type[XcG_HSL_PART]);
 }

 if (This->part_created[XcG_GRAY_PART])
 {
  DELETE(c_Label)(This->L_G1);
  DELETE(c_WTextField)(This->V_G1);
  DELETE(c_Scale)(This->S_G1);
  DELETE(c_Grid)(This->G_type[XcG_GRAY_PART]);
 }

#ifdef __YCBCR__
 if (This->part_created[XcG_YCBCR_PART])
 {
  DELETE(c_Label)(This->L_Y1);
  DELETE(c_Label)(This->L_Cb);
  DELETE(c_Label)(This->L_Cr);
  DELETE(c_WTextField)(This->V_Y1);
  DELETE(c_WTextField)(This->V_Cb);
  DELETE(c_WTextField)(This->V_Cr);
  DELETE(c_Scale)(This->S_Y1);
  DELETE(c_Scale)(This->S_Cb);
  DELETE(c_Scale)(This->S_Cr);
  DELETE(c_Grid)(This->G_type[XcG_YCBCR_PART]);
 }
#endif

 DELETE(c_Grid)(This->G_void);
 DELETE(c_Frame)(This->Fmain);

 DELETE(c_ToggleButton)(This->TBonglet[XcG_GRAY_PART]);
 DELETE(c_ToggleButton)(This->TBonglet[XcG_HSL_PART]);
 DELETE(c_ToggleButton)(This->TBonglet[XcG_RGB_PART]);
 DELETE(c_ToggleButton)(This->TBonglet[XcG_CMYK_PART]);
 DELETE(c_Grid)(This->Gonglet);
 DELETE(c_Frame)(This->Fonglet);

 DELETE(c_Grid)(This->Gmain);
 DELETE(c_ASLBox)(This->PBox);
 
#ifdef Xc_XALLWRITE
 if (This->close_reason == XcCD_OK)
 {
  if (F(This->list_save).something_to_change(This->list_save))
  {
   if (SET_UNDO(XcR_UCList)) goto no_change;
   F(This->list_save).change_all(This->list_save);
  }
 }
 no_change:
#endif

 DELETE(c_ModifList)(This->list_save);
 Xc_free(This);
 Xc_TRACE(("Object Box_color destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copieur                                                           ** 
** ----------------------------------------------------------------- */
void *copy_Box_color(This)
c_Box_color *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}

/* ----------------------------------------------------------------- **
** Add_Item - Create a new color item                                **
** ----------------------------------------------------------------- */
static void Add_Item(w, This)
Widget		w;
c_Box_color	*This;
{
 c_Color		*tmp_color;
 char			*name;
 c_Color		*color;
 cmyk_t			cmyk;
 hsl_t			hsl;
 rgb_t			rgb;
 int			gray;
 color_set_code_t	mode;
  
 name = F(This->PBox).getNewItemName(This->PBox, (void **)&color);
 if (!name) return;
 tmp_color = COPY(c_Color)(color, &(This->v_color_base));

 if (F(This->list_save).get(This->list_save,
			    color, SAVE_MODE, &mode) == -1)
  mode = color->mode;

 switch (mode)
 {
 case XcC_CMYK:
  if (F(This->list_save).get(This->list_save,
			     color, SAVE_CMYK, &cmyk) == -1)
   memcpy(&cmyk, &(color->cmyk), sizeof(cmyk_t));
  F(tmp_color).set(tmp_color, 
		   XcC_NAME, name, 
		   XcC_CMYK, cmyk.c, cmyk.m, cmyk.y, cmyk.k,
		   XcC_END);
  break;
 case XcC_HSL:
  if (F(This->list_save).get(This->list_save,
			     color, SAVE_HSL, &hsl) == -1)
   memcpy(&hsl, &(color->hsl), sizeof(hsl_t));
  F(tmp_color).set(tmp_color, 
		   XcC_NAME, name, 
		   XcC_HSL, hsl.h, hsl.s, hsl.l,
		   XcC_END);
  break;

 case XcC_GRAY:
  if (F(This->list_save).get(This->list_save,
			     color, SAVE_GRAY, &gray) == -1)
   gray = color->ycbcr.y;
  F(tmp_color).set(tmp_color, 
		   XcC_NAME, name, 
		   XcC_GRAY, gray,
		   XcC_END);
  break;
 case XcC_RGB:
 default:
  if (F(This->list_save).get(This->list_save,
			     color, SAVE_RGB, &rgb) == -1)
   memcpy(&rgb, &(color->rgb), sizeof(rgb_t));
  F(tmp_color).set(tmp_color,
		   XcC_NAME, name,
		   XcC_RGB, rgb.r, rgb.g, rgb.b,
		   XcC_END);
  break;
 }

 F(This->list_save).add(This->list_save, tmp_color);
 F(This->PBox->ItemList).setState(This->PBox->ItemList, FALSE,
				  XcIL_POINTER_ARRAY, -1);
 F(This->PBox->ItemList).addItems(This->PBox->ItemList, TRUE,
				  XcIL_VA_LIST, 1, tmp_color, NULL);
 F(This->PBox).selectItem(This->PBox, tmp_color, TRUE);
 XmProcessTraversal(WIDGET(This->PBox->text_field), XmTRAVERSE_CURRENT);
 XmTextFieldSetInsertionPosition(WIDGET(This->PBox->text_field), 
				 (XmTextPosition)strlen(name));
 Xc_free(name);
}

/* ----------------------------------------------------------------- ** 
** Del_SItem - Delete the Color Selection                            ** 
** ----------------------------------------------------------------- */
static void Del_SItem(w, This)
Widget		w;
c_Box_color	*This;
{
 c_Color	**color;
 int		num, i;
 c_Color	*item;
  
 if (F(This->PBox->ItemList).getSelection(This->PBox->ItemList,
					  (void ***)&color, &num))
 {
  item =
   F(This->PBox->ItemList).getFirstUnselectItem(This->PBox->ItemList);
  for (i = 0; i < num; i++)
  {
   if (XcH_USAGE(color[i]) == 0 && color[i]->lock_flag == FALSE && 
       This->PBox->ItemList->item_count > 1)
   {
    F(This->PBox->ItemList).deleteItems(This->PBox->ItemList,
					XcIL_POINTER_ARRAY, 1,
					(void **)&(color[i]));
    F(This->list_save).sub(This->list_save, color[i]);
   }
  }
  if (num)
   Xc_free(color);
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
static void Change_Sel(this, box, color)
c_ASLBox	*this;
c_Box_color	*box;
c_Color		*color;
{
 Arg			args[2]; 
 color_set_code_t	mode;
 real		y;

 Xc_TRACE(("Change Selection. color: %s", color->name));

 if (F(box->list_save).get(box->list_save,
			   color, SAVE_MODE, &mode) == -1)
  mode = color->mode;

 /*printf("color %s mode: %d - mode: %d \n", color->name, color->mode, mode);*/
 switch (mode)
 {
 case XcC_RGB:
  if (box->current_part != XcG_RGB_PART)
   change_part(WIDGET(box->TBonglet[XcG_RGB_PART]), box);
  if (F(box->list_save).get(box->list_save,
			    color, SAVE_RGB, &(box->rgb)) == -1)
   memcpy(&(box->rgb), &(color->rgb), sizeof(rgb_t));
  memcpy(&(box->rgb_r), &(box->rgb), sizeof(rgb_t));
  break;
 case XcC_HSL:
  if (box->current_part != XcG_HSL_PART)
   change_part(WIDGET(box->TBonglet[XcG_HSL_PART]), box);
  if (F(box->list_save).get(box->list_save,
			    color, SAVE_HSL, &(box->hsl)) == -1)
   memcpy(&(box->hsl), &(color->hsl), sizeof(rgb_t));
  HSLtoRGB(&(box->hsl), &(box->rgb_r));
  break;
#ifdef __YCBCR__
 case XcC_YCbCr:
  if (box->current_part != XcG_YCBCR_PART)
   change_part(WIDGET(box->TBonglet[XcG_YCBCR_PART]), box);
  break;
#endif
 case XcC_GRAY:
  if (box->current_part != XcG_GRAY_PART)
   change_part(WIDGET(box->TBonglet[XcG_GRAY_PART]), box);
  if (F(box->list_save).get(box->list_save,
			    color, SAVE_GRAY, &box->ycbcr.y) == -1)
   box->ycbcr.y = color->ycbcr.y;
  box->rgb_r.r = box->ycbcr.y;
  box->rgb_r.g = box->ycbcr.y;
  box->rgb_r.b = box->ycbcr.y;
  break;
 case XcC_CMYK:
 default:
  if (box->current_part != XcG_CMYK_PART)
   change_part(WIDGET(box->TBonglet[XcG_CMYK_PART]), box);
  if (F(box->list_save).get(box->list_save,
			    color, SAVE_CMYK, &(box->cmyk)) == -1)
   memcpy(&(box->cmyk), &(color->cmyk), sizeof(cmyk_t));
  
  box->rgb_r.r = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					   box->cmyk.c + box->cmyk.k);
  box->rgb_r.g = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					   box->cmyk.m + box->cmyk.k);
  box->rgb_r.b = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					   box->cmyk.y + box->cmyk.k);
  break;
 }

 switch (box->current_part)
 {
 case XcG_CMYK_PART:
  set_CMYK_part(box);
  break;
 case XcG_RGB_PART:
  set_RGB_part(box);
  break;
 case XcG_HSL_PART:
  set_HSL_part(box);
  break;
 case XcG_GRAY_PART:
  set_GRAY_part(box);
  break;
#ifdef __YCBCR__
 case XcG_YCBCR_PART:
  set_YCBCR_part(box);
  break;
#endif
 }

 F(GlobColormap).get_closest_color(GlobColormap, &(box->cell),
				   XcC_SHORTVALUE_COLOR(box->rgb_r.r),
				   XcC_SHORTVALUE_COLOR(box->rgb_r.g),
				   XcC_SHORTVALUE_COLOR(box->rgb_r.b));

 XtSetArg(args[0], XmNbackground, box->cell.pixel);

 y = XcC_DESCALE_COLOR(double, (int)((0.2990 * (float)box->rgb_r.r)
				     + (0.5870 * (float)box->rgb_r.g)
				     + ( 0.1140 * (float)box->rgb_r.b) + .5));
 if( y > 0.5)
  XtSetArg(args[1], XmNforeground, GlobColormap->Cblack);
 else
  XtSetArg(args[1], XmNforeground, GlobColormap->Cwhite);
 
 XtSetValues(WIDGET(box->L_color), args, 2);

}

/* ----------------------------------------------------------------- ** 
** convert to percent                                                ** 
** ----------------------------------------------------------------- */
static int convertCMYK(value)
int	value;
{
 return (int) XcC_PERCENT_COLOR(value);
}

/* ----------------------------------------------------------------- ** 
** convert from percent                                              ** 
** ----------------------------------------------------------------- */
static int invert_convertCMYK(value)
int	value;
{
 return (int)XcC_COLOR_PERCENT(value);
}

/* ----------------------------------------------------------------- ** 
** convert to percent                                                ** 
** ----------------------------------------------------------------- */
static int convertH(value)
int	value;
{
 return (int)rint(XcC_PERCENT_COLOR(value) * 3.6);
}

/* ----------------------------------------------------------------- ** 
** convert from percent                                              ** 
** ----------------------------------------------------------------- */
static int invert_convertH(value)
int	value;
{
 return (int)rint(XcC_COLOR_PERCENT(value / 3.6));
}

/* ----------------------------------------------------------------- ** 
** update text cmyk                                                  ** 
** ----------------------------------------------------------------- */
static void update_text_cmyk(w, Obj, cbs)
Widget			w;
c_Box_color		*Obj;
XmScaleCallbackStruct	*cbs;
{
 Arg		argsw[2];
 real		value;
 c_Color	**color;
 int		num2, i, compo = 0;
 cmyk_t		cmyk;
 real		y;
  
 value = (real)cbs->value;
 if (w == WIDGET(Obj->S_C))
 {
  F(Obj->V_C).set_value_in_unit(Obj->V_C, value);
  Obj->cmyk.c = (int)XcC_COLOR_PERCENT(cbs->value);
  compo = 0;
 }
 if (w == WIDGET(Obj->S_M))
 {
  F(Obj->V_M).set_value_in_unit(Obj->V_M, value);
  Obj->cmyk.m = (int)XcC_COLOR_PERCENT(cbs->value);
  compo = 1;
 }
 if (w == WIDGET(Obj->S_Y))
 {
  F(Obj->V_Y).set_value_in_unit(Obj->V_Y, value);
  Obj->cmyk.y = (int)XcC_COLOR_PERCENT(cbs->value);
  compo = 2;
 }
 if (w == WIDGET(Obj->S_K))
 {
  F(Obj->V_K).set_value_in_unit(Obj->V_K, value);
  Obj->cmyk.k = (int)XcC_COLOR_PERCENT(cbs->value);
  compo = 3;
 }
 if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&color, &num2))
 {
  Obj->rgb.r = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					 Obj->cmyk.c + Obj->cmyk.k);
  Obj->rgb.g = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					 Obj->cmyk.m + Obj->cmyk.k);
  Obj->rgb.b = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					 Obj->cmyk.y + Obj->cmyk.k);
  F(GlobColormap).get_closest_color(GlobColormap, &(Obj->cell),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.r),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.g),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.b));
  XtSetArg(argsw[0], XmNbackground, Obj->cell.pixel);

  y = XcC_DESCALE_COLOR(double, (int)((0.2990 * (float)Obj->rgb.r)
				      + (0.5870 * (float)Obj->rgb.g)
				      + ( 0.1140 * (float)Obj->rgb.b) + .5));
  if( y > 0.5)
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cblack);
  else
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cwhite);
  XtSetValues(WIDGET(Obj->L_color), argsw, 2);

  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     color[i], SAVE_CMYK, &cmyk) == -1)
    memcpy(&cmyk, &(color[i]->cmyk), sizeof(cmyk_t));
   switch(compo)
   {
   case 0:
    cmyk.c = Obj->cmyk.c;
    break;
   case 1:
    cmyk.m = Obj->cmyk.m;	      
    break;
   case 2:
    cmyk.y = Obj->cmyk.y;	      
    break;
   case 3:
    cmyk.k = Obj->cmyk.k;	    
    break;
   }
   F(Obj->list_save).set(Obj->list_save, color[i], SAVE_CMYK, &cmyk);
  }

  if (num2)
   Xc_free(color);
 }
}

/* ----------------------------------------------------------------- ** 
** update text rgb                                                   ** 
** ----------------------------------------------------------------- */
static void update_text_rgb(w, Obj, cbs)
Widget			w;
c_Box_color		*Obj;
XmScaleCallbackStruct	*cbs;
{
 Arg		argsw[2];
 real		value;
 c_Color	**color;
 int		num2, i, compo = 0;
 rgb_t		rgb;
 real		y;
  
 value = (real)cbs->value;
 if (w == WIDGET(Obj->S_R))
 {
  F(Obj->V_R).set_value_in_unit(Obj->V_R, value);
  Obj->rgb.r = (int)XcC_COLOR_PERCENT(cbs->value);
  compo = 0;
 }
 if (w == WIDGET(Obj->S_G))
 {
  F(Obj->V_G).set_value_in_unit(Obj->V_G, value);
  Obj->rgb.g = (int)XcC_COLOR_PERCENT(cbs->value);
  compo = 1;
 }
 if (w == WIDGET(Obj->S_B))
 {
  F(Obj->V_B).set_value_in_unit(Obj->V_B, value);
  Obj->rgb.b = (int)XcC_COLOR_PERCENT(cbs->value);
  compo = 2;
 }
 if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&color, &num2))
 {
  F(GlobColormap).get_closest_color(GlobColormap, &(Obj->cell),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.r),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.g),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.b));
  XtSetArg(argsw[0], XmNbackground, Obj->cell.pixel);
  y = XcC_DESCALE_COLOR(double, (int)((0.2990 * (float)Obj->rgb.r)
				      + (0.5870 * (float)Obj->rgb.g)
				      + ( 0.1140 * (float)Obj->rgb.b) + .5));
  if( y > 0.5)
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cblack);
  else
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cwhite);
  XtSetValues(WIDGET(Obj->L_color), argsw, 2);
  

  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     color[i], SAVE_RGB, &rgb) == -1)
    memcpy(&rgb, &(color[i]->rgb), sizeof(rgb_t));
   switch(compo)
   {
   case 0:
    rgb.r = Obj->rgb.r;
    break;
   case 1:
    rgb.g = Obj->rgb.g;	      
    break;
   case 2:
    rgb.b = Obj->rgb.b;	      
    break;
   }
   F(Obj->list_save).set(Obj->list_save, color[i], SAVE_RGB, &rgb);
  }

  if (num2)
   Xc_free(color);
 }
}

/* ----------------------------------------------------------------- ** 
** update text hsl                                                   ** 
** ----------------------------------------------------------------- */
static void update_text_hsl(w, Obj, cbs)
Widget			w;
c_Box_color		*Obj;
XmScaleCallbackStruct	*cbs;
{
 Arg		argsw[2];
 real		value;
 c_Color	**color;
 int		num2, i, compo = 0;
 hsl_t		hsl;
 real		y;
  
 value = (real)cbs->value;
 if (w == WIDGET(Obj->S_H))
 {
  F(Obj->V_H).set_value_in_unit(Obj->V_H, value);
  Obj->hsl.h = (int)XcC_COLOR_PERCENT(value / 3.6);
  compo = 0;
 }
 if (w == WIDGET(Obj->S_S))
 {
  F(Obj->V_S).set_value_in_unit(Obj->V_S, value);
  Obj->hsl.s = (int)XcC_COLOR_PERCENT(cbs->value);
  compo = 1;
 }
 if (w == WIDGET(Obj->S_L))
 {
  F(Obj->V_L).set_value_in_unit(Obj->V_L, value);
  Obj->hsl.l = (int)XcC_COLOR_PERCENT(cbs->value);
  compo = 2;
 }
 if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&color, &num2))
 {
  HSLtoRGB(&(Obj->hsl), &(Obj->rgb));
  F(GlobColormap).get_closest_color(GlobColormap, &(Obj->cell),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.r),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.g),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.b));
  XtSetArg(argsw[0], XmNbackground, Obj->cell.pixel);
  y = XcC_DESCALE_COLOR(double, (int)((0.2990 * (float)Obj->rgb.r)
				      + (0.5870 * (float)Obj->rgb.g)
				      + ( 0.1140 * (float)Obj->rgb.b) + .5));
  if( y > 0.5)
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cblack);
  else
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cwhite);
  XtSetValues(WIDGET(Obj->L_color), argsw, 2);

  for(i = 0; i < num2; i++)
  {
   if (F(Obj->list_save).get(Obj->list_save,
			     color[i], SAVE_HSL, &hsl) == -1)
    memcpy(&hsl, &(color[i]->hsl), sizeof(hsl_t));
   switch(compo)
   {
   case 0:
    hsl.h = Obj->hsl.h;
    break;
   case 1:
    hsl.s = Obj->hsl.s;
    break;
   case 2:
    hsl.l = Obj->hsl.l;
    break;
   }
   F(Obj->list_save).set(Obj->list_save, color[i], SAVE_HSL, &hsl);
  }
  if (num2)
   Xc_free(color);
 }
}

/* ----------------------------------------------------------------- ** 
** update text gray                                                  ** 
** ----------------------------------------------------------------- */
static void update_text_gray(w, Obj, cbs)
Widget			w;
c_Box_color		*Obj;
XmScaleCallbackStruct	*cbs;
{
 Arg		argsw[2];
 real		value;
 c_Color	**color;
 int		num2, i, compo = 0;
 real		y;

 value = (real)cbs->value;
 if (w == WIDGET(Obj->S_G1))
 {
  F(Obj->V_G1).set_value_in_unit(Obj->V_G1, value);
  Obj->ycbcr.y = (int)XcC_COLOR_PERCENT(cbs->value);
  compo = 0;
 }
 if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&color, &num2))
 {
  F(GlobColormap).get_closest_color(GlobColormap, &(Obj->cell),
				    XcC_SHORTVALUE_COLOR(Obj->ycbcr.y),
				    XcC_SHORTVALUE_COLOR(Obj->ycbcr.y),
				    XcC_SHORTVALUE_COLOR(Obj->ycbcr.y));
  XtSetArg(argsw[0], XmNbackground, Obj->cell.pixel);
  y = XcC_DESCALE_COLOR(double, Obj->ycbcr.y);
  if( y > 0.5)
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cblack);
  else
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cwhite);
  XtSetValues(WIDGET(Obj->L_color), argsw, 2);
  for(i = 0; i < num2; i++)
  {
   F(Obj->list_save).set(Obj->list_save, color[i], SAVE_GRAY, &(Obj->ycbcr.y));
  }

  if (num2)
   Xc_free(color);
 }
}

/* ----------------------------------------------------------------- ** 
** certify text cmyk                                                 ** 
** ----------------------------------------------------------------- */
static void certify_text_cmyk(this, Obj)
c_WTextField	*this;
c_Box_color	*Obj;
{
 Arg		argsw[2];
 real		rnum;
 c_Color	**color;
 int		num2, i, compo = 0;
 cmyk_t		cmyk;
 real		y;

 rnum =  rint(F(this).get_value_in_unit(this) * 1000.0) / 10.0;
 F(this).set_value_in_unit(this, rnum);
  
 if(this == Obj->V_C)
 {
  F(Obj->S_C).change(Obj->S_C, (int)rint(rnum));
  Obj->cmyk.c = (int)XcC_COLOR_PERCENT(rnum);
  compo = 0;
 }
 if(this == Obj->V_M)
 {
  F(Obj->S_M).change(Obj->S_M, (int)rint(rnum));
  Obj->cmyk.m = (int)XcC_COLOR_PERCENT(rnum);
  compo = 1;
 }
 if(this == Obj->V_Y)
 {
  F(Obj->S_Y).change(Obj->S_Y, (int)rint(rnum));
  Obj->cmyk.y = (int)XcC_COLOR_PERCENT(rnum);
  compo = 2;
 }
 if(this == Obj->V_K)
 {
  F(Obj->S_K).change(Obj->S_K, (int)rint(rnum));
  Obj->cmyk.k = (int)XcC_COLOR_PERCENT(rnum);
  compo = 3;
 }

 if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&color, &num2))
 {
  Obj->rgb.r = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					 Obj->cmyk.c + Obj->cmyk.k);
  Obj->rgb.g = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					 Obj->cmyk.m + Obj->cmyk.k);
  Obj->rgb.b = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					 Obj->cmyk.y + Obj->cmyk.k);
  F(GlobColormap).get_closest_color(GlobColormap, &(Obj->cell),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.r),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.g),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.b));
  XtSetArg(argsw[0], XmNbackground, Obj->cell.pixel);
  y = XcC_DESCALE_COLOR(double, (int)((0.2990 * (float)Obj->rgb.r)
				      + (0.5870 * (float)Obj->rgb.g)
				      + ( 0.1140 * (float)Obj->rgb.b) + .5));
  if( y > 0.5)
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cblack);
  else
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cwhite);
  XtSetValues(WIDGET(Obj->L_color), argsw, 2);

  if (num2 == 1 && ((void *)*color) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_Color *)Obj->PBox->edited_item,
			 SAVE_CMYK,
			 &(Obj->cmyk));
  }
  else
  {
   for(i=0; i<num2; i++)
   {
    if (F(Obj->list_save).get(Obj->list_save,
			      color[i], SAVE_CMYK, &cmyk) == -1)
     memcpy(&cmyk, &(color[i]->cmyk), sizeof(cmyk_t));
    switch(compo)
    {
    case 0:
     cmyk.c = Obj->cmyk.c;
     break;
    case 1:
     cmyk.m = Obj->cmyk.m;
     break;
    case 2:
     cmyk.y = Obj->cmyk.y;
     break;
    case 3:
     cmyk.k = Obj->cmyk.k;
     break;
    }
    F(Obj->list_save).set(Obj->list_save, color[i], SAVE_CMYK, &cmyk);
   }
  }
  if (num2)
   Xc_free(color);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}

/* ----------------------------------------------------------------- ** 
** certify text rgb                                                  ** 
** ----------------------------------------------------------------- */
static void certify_text_rgb(this, Obj)
c_WTextField	*this;
c_Box_color	*Obj;
{
 Arg		argsw[2];
 real		rnum;
 c_Color	**color;
 int		num2, i, compo = 0;
 rgb_t		rgb;
 real		y;
  
 rnum = rint(F(this).get_value_in_unit(this) * 1000.0) / 10.0;
 F(this).set_value_in_unit(this, rnum);
  
 if(this == Obj->V_R)
 {
  F(Obj->S_R).change(Obj->S_R, (int)rint(rnum));
  Obj->rgb.r = (int)XcC_COLOR_PERCENT(rnum);
  compo = 0;
 }
 if(this == Obj->V_G)
 {
  F(Obj->S_G).change(Obj->S_G, (int)rint(rnum));
  Obj->rgb.g = (int)XcC_COLOR_PERCENT(rnum);
  compo = 1;
 }
 if(this == Obj->V_B)
 {
  F(Obj->S_B).change(Obj->S_B, (int)rint(rnum));
  Obj->rgb.b = (int)XcC_COLOR_PERCENT(rnum);
  compo = 2;
 }

 if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&color, &num2))
 {
  F(GlobColormap).get_closest_color(GlobColormap, &(Obj->cell),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.r),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.g),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.b));
  XtSetArg(argsw[0], XmNbackground, Obj->cell.pixel);
  y = XcC_DESCALE_COLOR(double, (int)((0.2990 * (float)Obj->rgb.r)
				      + (0.5870 * (float)Obj->rgb.g)
				      + ( 0.1140 * (float)Obj->rgb.b) + .5));
  if( y > 0.5)
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cblack);
  else
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cwhite);
  XtSetValues(WIDGET(Obj->L_color), argsw, 2);

  if (num2 == 1 && ((void *)*color) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_Color *)Obj->PBox->edited_item,
			 SAVE_RGB,
			 &(Obj->rgb));
  }
  else
  {
   for(i=0; i<num2; i++)
   {
    if (F(Obj->list_save).get(Obj->list_save,
			      color[i], SAVE_RGB, &rgb) == -1)
     memcpy(&rgb, &(color[i]->rgb), sizeof(rgb_t));
    switch(compo)
    {
    case 0:
     rgb.r = Obj->rgb.r;
     break;
    case 1:
     rgb.g = Obj->rgb.g;
     break;
    case 2:
     rgb.b = Obj->rgb.b;
     break;
    }
    F(Obj->list_save).set(Obj->list_save, color[i], SAVE_RGB, &rgb);
   }
  }
  if (num2)
   Xc_free(color);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}

/* ----------------------------------------------------------------- ** 
** certify text hsl                                                  ** 
** ----------------------------------------------------------------- */
static void certify_text_hsl(this, Obj)
c_WTextField	*this;
c_Box_color	*Obj;
{
 Arg		argsw[2];
 real		rnum;
 c_Color	**color;
 int		num2, i, compo = 0;
 hsl_t		hsl;
 real		y;
  
 if(this == Obj->V_H)
 {
  rnum = rint(F(this).get_value_in_unit(this) * 10.0) / 10.0;
  F(this).set_value_in_unit(this, rnum);
  
  F(Obj->S_H).change(Obj->S_H, (int)rint(rnum));
  Obj->hsl.h = (int)XcC_COLOR_PERCENT(rnum / 3.6);
  compo = 0;
 }
 else
 {
  rnum = rint(F(this).get_value_in_unit(this) * 1000.0) / 10.0;
  F(this).set_value_in_unit(this, rnum);

  if(this == Obj->V_S)
  {
   F(Obj->S_S).change(Obj->S_S, (int)rint(rnum));
   Obj->hsl.s = (int)XcC_COLOR_PERCENT(rnum);
   compo = 1;
  }
  if(this == Obj->V_L)
  {
   F(Obj->S_L).change(Obj->S_L, (int)rint(rnum));
   Obj->hsl.l = (int)XcC_COLOR_PERCENT(rnum);
   compo = 2;
  }
 }

 if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&color, &num2))
 {
  HSLtoRGB(&(Obj->hsl), &(Obj->rgb));
  F(GlobColormap).get_closest_color(GlobColormap, &(Obj->cell),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.r),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.g),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.b));
  XtSetArg(argsw[0], XmNbackground, Obj->cell.pixel);
  y = XcC_DESCALE_COLOR(double, (int)((0.2990 * (float)Obj->rgb.r)
				      + (0.5870 * (float)Obj->rgb.g)
				      + ( 0.1140 * (float)Obj->rgb.b) + .5));
  if( y > 0.5)
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cblack);
  else
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cwhite);
  XtSetValues(WIDGET(Obj->L_color), argsw, 2);

  if (num2 == 1 && ((void *)*color) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_Color *)Obj->PBox->edited_item,
			 SAVE_HSL,
			 &(Obj->hsl));
  }
  else
  {
   for(i=0; i<num2; i++)
   {
    if (F(Obj->list_save).get(Obj->list_save,
			      color[i], SAVE_HSL, &hsl) == -1)
     memcpy(&hsl, &(color[i]->hsl), sizeof(hsl_t));
    switch(compo)
    {
    case 0:
     hsl.h = Obj->hsl.h;
     break;
    case 1:
     hsl.s = Obj->hsl.s;
     break;
    case 2:
     hsl.l = Obj->hsl.l;
     break;
    }
    F(Obj->list_save).set(Obj->list_save, color[i], SAVE_HSL, &hsl);
   }
  }
  if (num2)
   Xc_free(color);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}

/* ----------------------------------------------------------------- ** 
** certify text gray                                                 ** 
** ----------------------------------------------------------------- */
static void certify_text_gray(this, Obj)
c_WTextField	*this;
c_Box_color	*Obj;
{
 Arg		argsw[2];
 real		rnum, y;
 c_Color	**color;
 int		num2, i;
  
 rnum = rint(F(this).get_value_in_unit(this) * 1000.0) / 10.0;
 F(this).set_value_in_unit(this, rnum);
  
 if(this == Obj->V_G1)
 {
  F(Obj->S_G1).change(Obj->S_G1, (int)rint(rnum));
  Obj->ycbcr.y = (int)XcC_COLOR_PERCENT(rnum);
 }

 if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&color, &num2))
 {
  F(GlobColormap).get_closest_color(GlobColormap, &(Obj->cell),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.r),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.g),
				    XcC_SHORTVALUE_COLOR(Obj->rgb.b));
  XtSetArg(argsw[0], XmNbackground, Obj->cell.pixel);
  y = XcC_DESCALE_COLOR(double, (int)((0.2990 * (float)Obj->rgb.r)
				      + (0.5870 * (float)Obj->rgb.g)
				      + (0.1140 * (float)Obj->rgb.b) + .5));
  if( y > 0.5)
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cblack);
  else
   XtSetArg(argsw[1], XmNforeground, GlobColormap->Cwhite);
  XtSetValues(WIDGET(Obj->L_color), argsw, 2);

  if (num2 == 1 && ((void *)*color) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 (c_Color *)Obj->PBox->edited_item,
			 SAVE_GRAY,
			 &(Obj->ycbcr.y));
  }
  else
  {
   for(i=0; i<num2; i++)
   {
    F(Obj->list_save).set(Obj->list_save, color[i], SAVE_GRAY, 
			  &(Obj->ycbcr.y));
   }
  }
  if (num2)
   Xc_free(color);
 }
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}


/* ----------------------------------------------------------------- ** 
** box action                                                        ** 
** ----------------------------------------------------------------- */
static void cb_action(cb_dialog, box, cb_reason)
c_CustomDialog	*cb_dialog;
c_Box_color	*box;
int		cb_reason;
{
#ifdef Xc_XALLWRITE
 box->close_reason = cb_reason; 
#ifndef NTRACE
 if (cb_reason == XcCD_OK)
  Xc_TRACE(("OK"));
 else
  Xc_TRACE(("canceled"));
#endif
 F(cb_dialog).unmap(cb_dialog);
#else
 F(cb_dialog).unmap(cb_dialog);
 if (cb_reason == XcCD_OK)
 {
  F(box->list_save).change_all(box->list_save);
 }
 else
 {
  Xc_TRACE(("canceled"));
 }
#endif
}

/* ----------------------------------------------------------------- ** 
** change color                                                      ** 
** ----------------------------------------------------------------- */
static void cb_change_C(box, color, type)
c_Box_color	*box;
c_Color		*color;
int		type;
{
 c_Color	*tmp_color;

 switch (type)
 {
 case TOSET:
  Xc_TRACE(("TOSET!"));
  switch(color->mode)
  {
  case XcC_CMYK:
   F(color).set(color, XcC_CMYK,
		color->cmyk.c,
		color->cmyk.m,
		color->cmyk.y,
		color->cmyk.k,
		XcC_END);
   break;
  case XcC_HSL:
   F(color).set(color, XcC_HSL,
		color->hsl.h,
		color->hsl.s,
		color->hsl.l,
		XcC_END);
   break;
  case XcC_GRAY:
   F(color).set(color, XcC_GRAY,
		color->ycbcr.y,
		XcC_END);
   break;
  case XcC_RGB:
  default:
   F(color).set(color, XcC_RGB,
		color->rgb.r,
		color->rgb.g,
		color->rgb.b,
		XcC_END);
  }
  break;
 case TOADD:
  Xc_TRACE(("TOADD!"));
  Xc_TRACE(("%s", color->name));
  tmp_color = NEW(c_Color)(box->color_base, color->name);
  switch(color->mode)
  {
  case XcC_CMYK:
   F(tmp_color).set(tmp_color, XcC_CMYK,
		    color->cmyk.c,
		    color->cmyk.m,
		    color->cmyk.y,
		    color->cmyk.k,
		    XcC_END);
   break;
  case XcC_HSL:
   F(tmp_color).set(tmp_color, XcC_HSL,
		    color->hsl.h,
		    color->hsl.s,
		    color->hsl.l,
		    XcC_END);
   break;
  case XcC_GRAY:
   F(tmp_color).set(tmp_color, XcC_GRAY,
		    color->ycbcr.y,
		    XcC_END);
   break;
  case XcC_RGB:
  default:
   F(tmp_color).set(tmp_color, XcC_RGB,
		    color->rgb.r,
		    color->rgb.g,
		    color->rgb.b,
		    XcC_END);
  }
  break;
 case TOSUB:
  Xc_TRACE(("TOSUB!"));
  DELETE(c_Color)(color);
  break;
 }
}


/* ----------------------------------------------------------------- ** 
** color_filter - Filter colors                                      ** 
** ----------------------------------------------------------------- */
static boolean color_filter(this)
c_Color *this;
{
 return !this->hide_flag && !this->transparency;
}

/* ----------------------------------------------------------------- ** 
** change interface part                                             ** 
** ----------------------------------------------------------------- */
static void change_part(w, Obj)
Widget w;
c_Box_color	*Obj;
{
 Dimension		width;
 Dimension		height;
 int			i;
 color_set_code_t	mode;
 color_set_code_t	next_mode;
 Arg			args[2]; 
 real y;

 i = 0;
 while ((w != WIDGET(Obj->TBonglet[i])) && (i < XcG_COUNT_PART)) 
  i++;
 if (i >= XcG_COUNT_PART)
  return;

 if (Obj->current_part != XcG_NONE_PART)
  F(Obj->TBonglet[Obj->current_part]).Set(Obj->TBonglet[Obj->current_part], 
					  False, False);
 F(Obj->TBonglet[i]).Set(Obj->TBonglet[i], True, False);

 if (Obj->current_part != i)
 {
  if (!Obj->part_created[i])
  {
   if (Obj->current_part != XcG_NONE_PART)
    XtVaGetValues(WIDGET(Obj->G_type[Obj->current_part]),
		  XmNwidth, &width,
		  XmNheight, &height, NULL);
   else
   {
    XtVaGetValues(WIDGET(Obj->G_void),
		  XmNwidth, &width,
		  XmNheight, &height, NULL);
   }
   switch(i)
   {
   case XcG_CMYK_PART:
    create_part_CMYK(Obj, width, height);
    break;
   case XcG_RGB_PART:
    create_part_RGB(Obj, width, height);
    break;
   case XcG_HSL_PART:
    create_part_HSL(Obj, width, height);
    break;
   case XcG_GRAY_PART:
    create_part_GRAY(Obj, width, height);
    break;
#ifdef __YCBCR__
   case XcG_YCBCR_PART:
    create_part_YCBCR(Obj, width, height);
    break;
#endif
   default:
    Xc_WARNING(("Unbelievable Error !"));
   }
  }

  switch (i)
  {
  case XcG_CMYK_PART:
   next_mode = XcC_CMYK;
   break;
  case XcG_RGB_PART:
   next_mode = XcC_RGB;
   break;
  case XcG_HSL_PART:
   next_mode = XcC_HSL;
   break;
  case XcG_GRAY_PART:
   next_mode = XcC_GRAY;
   break;
#ifdef __YCBCR__
  case XcG_YCBCR_PART:
   next_mode = XcC_YCbCr;
   break;
#endif
  default:
   Xc_WARNING(("Unbelievable Error !"));
  }

  if (F(Obj->list_save).get(Obj->list_save,
			    (c_Color*)Obj->PBox->edited_item,
			    SAVE_MODE, &mode) == -1)
   mode = ((c_Color*)Obj->PBox->edited_item)->mode;

  if (mode != next_mode)
   F(Obj->list_save).set(Obj->list_save, 
			 (c_Color*)Obj->PBox->edited_item,
			 SAVE_MODE, &next_mode);

  switch(mode)
  {
  case XcC_CMYK:
   if (F(Obj->list_save).get(Obj->list_save, 
			     (c_Color*)Obj->PBox->edited_item,
			     SAVE_CMYK, &(Obj->cmyk)) == -1)
    memcpy(&(Obj->cmyk), 
	   &(((c_Color*)Obj->PBox->edited_item)->cmyk), sizeof(cmyk_t));
   Obj->rgb.r = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					  Obj->cmyk.c + Obj->cmyk.k);
   Obj->rgb.g = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					  Obj->cmyk.m + Obj->cmyk.k);
   Obj->rgb.b = XcC_MAX_COLOR_VALUE - MIN(XcC_MAX_COLOR_VALUE,
					  Obj->cmyk.y + Obj->cmyk.k);
   break;
  case XcC_HSL:
   if (F(Obj->list_save).get(Obj->list_save, 
			     (c_Color*)Obj->PBox->edited_item,
			     SAVE_HSL, &(Obj->hsl)) == -1)
    memcpy(&(Obj->hsl), 
	   &(((c_Color*)Obj->PBox->edited_item)->hsl), sizeof(hsl_t));
   HSLtoRGB(&(Obj->hsl), &(Obj->rgb));
   break;
  case XcC_GRAY:
   if (F(Obj->list_save).get(Obj->list_save, 
			     (c_Color*)Obj->PBox->edited_item,
			     SAVE_GRAY, &(Obj->ycbcr.y)) == -1)
    Obj->ycbcr.y = ((c_Color*)Obj->PBox->edited_item)->ycbcr.y;
   Obj->rgb.r = Obj->ycbcr.y;
   Obj->rgb.g = Obj->ycbcr.y;
   Obj->rgb.b = Obj->ycbcr.y;
   break;
  case XcC_RGB:   
   if (F(Obj->list_save).get(Obj->list_save, 
			     (c_Color*)Obj->PBox->edited_item,
			     SAVE_RGB, &(Obj->rgb)) == -1)
    memcpy(&(Obj->rgb), 
	   &(((c_Color*)Obj->PBox->edited_item)->rgb), sizeof(rgb_t));
  default:
   break;
  }

  switch(i)
  {
  case XcG_CMYK_PART:
   Obj->cmyk.k = MIN(XcC_MAX_COLOR_VALUE - Obj->rgb.r,
		     MIN(XcC_MAX_COLOR_VALUE - Obj->rgb.g,
			 XcC_MAX_COLOR_VALUE - Obj->rgb.b));
   Obj->cmyk.c = XcC_MAX_COLOR_VALUE - Obj->rgb.r - Obj->cmyk.k;
   Obj->cmyk.m = XcC_MAX_COLOR_VALUE - Obj->rgb.g - Obj->cmyk.k;
   Obj->cmyk.y = XcC_MAX_COLOR_VALUE - Obj->rgb.b - Obj->cmyk.k;
   F(Obj->list_save).set(Obj->list_save,
			 ((c_Color*)Obj->PBox->edited_item),
			 SAVE_CMYK, &(Obj->cmyk));
   set_CMYK_part(Obj);
   break;
  case XcG_RGB_PART:
   F(Obj->list_save).set(Obj->list_save,
			 ((c_Color*)Obj->PBox->edited_item),
			 SAVE_RGB, &(Obj->rgb));
   set_RGB_part(Obj);
   break;
  case XcG_HSL_PART:
   RGBtoHSL(&(Obj->rgb), &(Obj->hsl));
   F(Obj->list_save).set(Obj->list_save,
			 ((c_Color*)Obj->PBox->edited_item),
			 SAVE_HSL, &(Obj->hsl));
   set_HSL_part(Obj);
   break;
  case XcG_GRAY_PART:
   Obj->ycbcr.y = 
    (int)((0.2990 * (float)Obj->rgb.r) 
	  + (0.5870 * (float)Obj->rgb.g) 
	  + (0.1140 * (float)Obj->rgb.b) + .5);
   F(GlobColormap).get_closest_color(GlobColormap, &(Obj->cell),
				     XcC_SHORTVALUE_COLOR(Obj->ycbcr.y),
				     XcC_SHORTVALUE_COLOR(Obj->ycbcr.y),
				     XcC_SHORTVALUE_COLOR(Obj->ycbcr.y));
   XtSetArg(args[0], XmNbackground, Obj->cell.pixel);

   y = XcC_DESCALE_COLOR(double, Obj->ycbcr.y);
   if( y > 0.5)
    XtSetArg(args[1], XmNforeground, GlobColormap->Cblack);
   else
    XtSetArg(args[1], XmNforeground, GlobColormap->Cwhite);
   XtSetValues(WIDGET(Obj->L_color), args, 2);
   F(Obj->list_save).set(Obj->list_save,
			 ((c_Color*)Obj->PBox->edited_item),
			 SAVE_GRAY, &(Obj->ycbcr.y));
   set_GRAY_part(Obj);
   break;
#ifdef __YCBCR__
  case XcG_YCBCR_PART:
   set_YCBCR_part(Obj);
   break;
#endif
  default:
   Xc_WARNING(("Unbelievable Error !"));
  }
  if (Obj->current_part != XcG_NONE_PART)
   XtUnmanageChild(WIDGET(Obj->G_type[Obj->current_part]));
  else
   XtUnmanageChild(WIDGET(Obj->G_void));
  Obj->current_part = i;
  /*Change_Sel(Obj->PBox, Obj, Obj->PBox->edited_item);*/
  XtManageChild(WIDGET(Obj->G_type[i]));
  F(Obj->G_type[i]).callResize(Obj->G_type[i]);
 }
}

/* ----------------------------------------------------------------- ** 
** create interface part CMYK                                        ** 
** ----------------------------------------------------------------- */
static void create_part_CMYK(ObjTmp, width, height)
c_Box_color	*ObjTmp;
Dimension	width;
Dimension	height;
{
 c_ManageWidget		*man_w;
 char		str_C[8];
 char		str_M[8];
 char		str_Y[8];
 char		str_K[8];

 sprintf(str_C, "%.1f", XcC_PERCENT_COLOR(ObjTmp->cmyk.c));
 sprintf(str_M, "%.1f", XcC_PERCENT_COLOR(ObjTmp->cmyk.m));
 sprintf(str_Y, "%.1f", XcC_PERCENT_COLOR(ObjTmp->cmyk.y));
 sprintf(str_K, "%.1f", XcC_PERCENT_COLOR(ObjTmp->cmyk.k));

 man_w = (c_ManageWidget *)NEW(c_ManageWidget)();
  
 ObjTmp->G_type[XcG_CMYK_PART] = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Fmain),
						       NULL, 
						       "gBC_CMYK", TRUE);
 
 XtVaSetValues(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
	       XmNwidth, width, XmNheight, height, NULL);

 ObjTmp->L_C = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
			  man_w,
			  "l_C");
 ObjTmp->L_M = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
			  man_w,
			  "l_M");
 ObjTmp->L_Y = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
			  man_w,
			  "l_Y");
 ObjTmp->L_K = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
			  man_w,
			  "l_K");

 ObjTmp->V_C = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
				    man_w,
				    "tf_C",
				    TF_REAL,
				    0.0, 100.0,
				    str_C, NULL, 5);
 F(ObjTmp->V_C).set_unit(ObjTmp->V_C, XcUB_RATIO_PERCENTAGE,
			 "%.1f", TRUE, FALSE);
 ObjTmp->V_M = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
				    man_w,
				    "tf_M",
				    TF_REAL,
				    0.0, 100.0,
				    str_M, NULL, 5);
 F(ObjTmp->V_M).set_unit(ObjTmp->V_M, XcUB_RATIO_PERCENTAGE,
			 "%.1f", TRUE, FALSE);
 ObjTmp->V_Y = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
				    man_w,
				    "tf_Y",
				    TF_REAL,
				    0.0, 100.0,
				    str_Y, NULL, 5);
 F(ObjTmp->V_Y).set_unit(ObjTmp->V_Y, XcUB_RATIO_PERCENTAGE,
			 "%.1f", TRUE, FALSE);
 ObjTmp->V_K = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
				    man_w,
				    "tf_K",
				    TF_REAL,
				    0.0, 100.0,
				    str_K, NULL, 5);
 F(ObjTmp->V_K).set_unit(ObjTmp->V_K, XcUB_RATIO_PERCENTAGE,
			 "%.1f", TRUE, FALSE);
  
 F(ObjTmp->V_C).add_callback(ObjTmp->V_C, (WTextFieldProc)certify_text_cmyk,
			     ObjTmp);
 F(ObjTmp->V_M).add_callback(ObjTmp->V_M, (WTextFieldProc)certify_text_cmyk,
			     ObjTmp);
 F(ObjTmp->V_Y).add_callback(ObjTmp->V_Y, (WTextFieldProc)certify_text_cmyk,
			     ObjTmp);
 F(ObjTmp->V_K).add_callback(ObjTmp->V_K, (WTextFieldProc)certify_text_cmyk,
			     ObjTmp);

 ObjTmp->S_C = 
  (c_Scale *)NEW(c_Scale)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
			  man_w,
			  "s_C",
			  0, &(ObjTmp->cmyk.c), 100,
			  convertCMYK, invert_convertCMYK);
 ObjTmp->S_M = 
  (c_Scale *)NEW(c_Scale)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
			  man_w,
			  "s_M",
			  0, &(ObjTmp->cmyk.m), 100,
			  convertCMYK, invert_convertCMYK);

 ObjTmp->S_Y = 
  (c_Scale *)NEW(c_Scale)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
			  man_w,
			  "s_Y",
			  0, &(ObjTmp->cmyk.y), 100,
			  convertCMYK, invert_convertCMYK);
 ObjTmp->S_K = 
  (c_Scale *)NEW(c_Scale)(WIDGET(ObjTmp->G_type[XcG_CMYK_PART]),
			  man_w,
			  "s_K",
			  0, &(ObjTmp->cmyk.k), 100,
			  convertCMYK, invert_convertCMYK);

 F(ObjTmp->S_C).set_drag_callback(ObjTmp->S_C, update_text_cmyk,
				  ObjTmp);
 F(ObjTmp->S_C).set_valuechange_callback(ObjTmp->S_C, update_text_cmyk,
					 ObjTmp);
 F(ObjTmp->S_M).set_drag_callback(ObjTmp->S_M, update_text_cmyk,
				  ObjTmp);
 F(ObjTmp->S_M).set_valuechange_callback(ObjTmp->S_M, update_text_cmyk,
					 ObjTmp);
 F(ObjTmp->S_Y).set_drag_callback(ObjTmp->S_Y, update_text_cmyk,
				  ObjTmp);
 F(ObjTmp->S_Y).set_valuechange_callback(ObjTmp->S_Y, update_text_cmyk,
					 ObjTmp);
 F(ObjTmp->S_K).set_drag_callback(ObjTmp->S_K, update_text_cmyk,
				  ObjTmp);
 F(ObjTmp->S_K).set_valuechange_callback(ObjTmp->S_K, update_text_cmyk,
					 ObjTmp);

 F(ObjTmp->G_type[XcG_CMYK_PART]).addColumn(ObjTmp->G_type[XcG_CMYK_PART], 
					    XcG_MAX_SIZE,
					    NULL);
 F(ObjTmp->G_type[XcG_CMYK_PART]).addColumn(ObjTmp->G_type[XcG_CMYK_PART], 
					    XcG_BEST_SIZE,
					    WIDGET(ObjTmp->L_C), 90,
					    WIDGET(ObjTmp->V_C), XcG_BEST_SIZE,
					    XcG_WIDGET_NULL, 7,
					    WIDGET(ObjTmp->S_C), XcG_MAX_SIZE,
					    NULL);
 F(ObjTmp->G_type[XcG_CMYK_PART]).addColumn(ObjTmp->G_type[XcG_CMYK_PART], 
					    XcG_MAX_SIZE,
					    NULL);
 F(ObjTmp->G_type[XcG_CMYK_PART]).addColumn(ObjTmp->G_type[XcG_CMYK_PART], 
					    XcG_BEST_SIZE,
					    WIDGET(ObjTmp->L_M), 90,
					    WIDGET(ObjTmp->V_M), XcG_BEST_SIZE,
					    XcG_WIDGET_NULL, 7,
					    WIDGET(ObjTmp->S_M), XcG_MAX_SIZE,
					    NULL);
 F(ObjTmp->G_type[XcG_CMYK_PART]).addColumn(ObjTmp->G_type[XcG_CMYK_PART], 
					    XcG_MAX_SIZE,
					    NULL);
 F(ObjTmp->G_type[XcG_CMYK_PART]).addColumn(ObjTmp->G_type[XcG_CMYK_PART], 
					    XcG_BEST_SIZE,
					    WIDGET(ObjTmp->L_Y), 90,
					    WIDGET(ObjTmp->V_Y), XcG_BEST_SIZE,
					    XcG_WIDGET_NULL, 7,
					    WIDGET(ObjTmp->S_Y), XcG_MAX_SIZE,
					    NULL);
 F(ObjTmp->G_type[XcG_CMYK_PART]).addColumn(ObjTmp->G_type[XcG_CMYK_PART], 
					    XcG_MAX_SIZE,
					    NULL);
 F(ObjTmp->G_type[XcG_CMYK_PART]).addColumn(ObjTmp->G_type[XcG_CMYK_PART], 
					    XcG_BEST_SIZE,
					    WIDGET(ObjTmp->L_K), 90,
					    WIDGET(ObjTmp->V_K), XcG_BEST_SIZE,
					    XcG_WIDGET_NULL, 7,
					    WIDGET(ObjTmp->S_K), XcG_MAX_SIZE,
					    NULL);
 F(ObjTmp->G_type[XcG_CMYK_PART]).addColumn(ObjTmp->G_type[XcG_CMYK_PART], 
					    XcG_MAX_SIZE,
					    NULL);

 ObjTmp->part_created[XcG_CMYK_PART] = TRUE;
 F(man_w).Manage_all(man_w, (Widget)0);
 F(ObjTmp->G_type[XcG_CMYK_PART]).callResize(ObjTmp->G_type[XcG_CMYK_PART]);

}

/* ----------------------------------------------------------------- ** 
** set interface part CMYK                                           ** 
** ----------------------------------------------------------------- */
static void set_CMYK_part(Obj)
c_Box_color *Obj;
{
 F(Obj->V_C).set_value_in_unit(Obj->V_C, XcC_PERCENT_COLOR(Obj->cmyk.c));
 F(Obj->S_C).change(Obj->S_C, (int)rint(XcC_PERCENT_COLOR(Obj->cmyk.c)));
 
 F(Obj->V_M).set_value_in_unit(Obj->V_M, XcC_PERCENT_COLOR(Obj->cmyk.m));
 F(Obj->S_M).change(Obj->S_M, (int)rint(XcC_PERCENT_COLOR(Obj->cmyk.m)));
  
 F(Obj->V_Y).set_value_in_unit(Obj->V_Y, XcC_PERCENT_COLOR(Obj->cmyk.y));
 F(Obj->S_Y).change(Obj->S_Y, (int)rint(XcC_PERCENT_COLOR(Obj->cmyk.y)));
  
 F(Obj->V_K).set_value_in_unit(Obj->V_K, XcC_PERCENT_COLOR(Obj->cmyk.k));
 F(Obj->S_K).change(Obj->S_K, (int)rint(XcC_PERCENT_COLOR(Obj->cmyk.k)));
}

/* ----------------------------------------------------------------- ** 
** create interface part RGB                                         ** 
** ----------------------------------------------------------------- */
static void create_part_RGB(ObjTmp, width, height)
c_Box_color	*ObjTmp;
Dimension	width;
Dimension	height;
{
 c_ManageWidget		*man_w;
 char		str_R[8];
 char		str_G[8];
 char		str_B[8];

 sprintf(str_R, "%.1f", XcC_PERCENT_COLOR(ObjTmp->rgb.r));
 sprintf(str_G, "%.1f", XcC_PERCENT_COLOR(ObjTmp->rgb.g));
 sprintf(str_B, "%.1f", XcC_PERCENT_COLOR(ObjTmp->rgb.b));

 man_w = (c_ManageWidget *)NEW(c_ManageWidget)();

 ObjTmp->G_type[XcG_RGB_PART] = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Fmain),
						      NULL,
						      "gBC_RGB", TRUE);
 XtVaSetValues(WIDGET(ObjTmp->G_type[XcG_RGB_PART]),
	       XmNwidth, width,
	       XmNheight, height, NULL);

 ObjTmp->L_R = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->G_type[XcG_RGB_PART]),
			  man_w,
			  "l_R");
 ObjTmp->L_G = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->G_type[XcG_RGB_PART]),
			  man_w,
			  "l_G");
 ObjTmp->L_B = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->G_type[XcG_RGB_PART]),
			  man_w,
			  "l_B");

 ObjTmp->V_R = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->G_type[XcG_RGB_PART]),
				    man_w,
				    "tf_R",
				    TF_REAL,
				    0.0, 100.0,
				    str_R, NULL, 5);
 F(ObjTmp->V_R).set_unit(ObjTmp->V_R, XcUB_RATIO_PERCENTAGE,
			 "%.1f", TRUE, FALSE);
 ObjTmp->V_G = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->G_type[XcG_RGB_PART]),
				    man_w,
				    "tf_G",
				    TF_REAL,
				    0.0, 100.0,
				    str_G, NULL, 5);
 F(ObjTmp->V_G).set_unit(ObjTmp->V_G, XcUB_RATIO_PERCENTAGE,
			 "%.1f", TRUE, FALSE);
 ObjTmp->V_B = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->G_type[XcG_RGB_PART]),
				    man_w,
				    "tf_B",
				    TF_REAL,
				    0.0, 100.0,
				    str_B, NULL, 5);
 F(ObjTmp->V_B).set_unit(ObjTmp->V_B, XcUB_RATIO_PERCENTAGE,
			 "%.1f", TRUE, FALSE);
  
 F(ObjTmp->V_R).add_callback(ObjTmp->V_R, (WTextFieldProc)certify_text_rgb,
			     ObjTmp);
 F(ObjTmp->V_G).add_callback(ObjTmp->V_G, (WTextFieldProc)certify_text_rgb,
			     ObjTmp);
 F(ObjTmp->V_B).add_callback(ObjTmp->V_B, (WTextFieldProc)certify_text_rgb,
			     ObjTmp);

 ObjTmp->S_R = 
  (c_Scale *)NEW(c_Scale)(WIDGET(ObjTmp->G_type[XcG_RGB_PART]),
			  man_w,
			  "s_R",
			  0, &(ObjTmp->rgb.r), 100,
			  convertCMYK, invert_convertCMYK);
 ObjTmp->S_G = 
  (c_Scale *)NEW(c_Scale)(WIDGET(ObjTmp->G_type[XcG_RGB_PART]),
			  man_w,
			  "s_G",
			  0, &(ObjTmp->rgb.g), 100,
			  convertCMYK, invert_convertCMYK);
 
 ObjTmp->S_B = 
  (c_Scale *)NEW(c_Scale)(WIDGET(ObjTmp->G_type[XcG_RGB_PART]),
			  man_w,
			  "s_B",
			  0, &(ObjTmp->rgb.b), 100,
			  convertCMYK, invert_convertCMYK);
 
 F(ObjTmp->S_R).set_drag_callback(ObjTmp->S_R, update_text_rgb,
				  ObjTmp);
 F(ObjTmp->S_R).set_valuechange_callback(ObjTmp->S_R, update_text_rgb,
					 ObjTmp);
 F(ObjTmp->S_G).set_drag_callback(ObjTmp->S_G, update_text_rgb,
				  ObjTmp);
 F(ObjTmp->S_G).set_valuechange_callback(ObjTmp->S_G, update_text_rgb,
					 ObjTmp);
 F(ObjTmp->S_B).set_drag_callback(ObjTmp->S_B, update_text_rgb,
				  ObjTmp);
 F(ObjTmp->S_B).set_valuechange_callback(ObjTmp->S_B, update_text_rgb,
					 ObjTmp);

 F(ObjTmp->G_type[XcG_RGB_PART]).addColumn(ObjTmp->G_type[XcG_RGB_PART], 
					   XcG_MAX_SIZE,
					   NULL);
 F(ObjTmp->G_type[XcG_RGB_PART]).addColumn(ObjTmp->G_type[XcG_RGB_PART], 
					   XcG_BEST_SIZE,
					   WIDGET(ObjTmp->L_R), 90,
					   WIDGET(ObjTmp->V_R), XcG_BEST_SIZE,
					   XcG_WIDGET_NULL, 7,
					   WIDGET(ObjTmp->S_R), XcG_MAX_SIZE,
					   NULL);
 F(ObjTmp->G_type[XcG_RGB_PART]).addColumn(ObjTmp->G_type[XcG_RGB_PART], 
					   XcG_MAX_SIZE,
					   NULL);
 F(ObjTmp->G_type[XcG_RGB_PART]).addColumn(ObjTmp->G_type[XcG_RGB_PART], 
					   XcG_BEST_SIZE,
					   WIDGET(ObjTmp->L_G), 90,
					   WIDGET(ObjTmp->V_G), XcG_BEST_SIZE,
					   XcG_WIDGET_NULL, 7,
					   WIDGET(ObjTmp->S_G), XcG_MAX_SIZE,
					   NULL);
 F(ObjTmp->G_type[XcG_RGB_PART]).addColumn(ObjTmp->G_type[XcG_RGB_PART], 
					   XcG_MAX_SIZE,
					   NULL);
 F(ObjTmp->G_type[XcG_RGB_PART]).addColumn(ObjTmp->G_type[XcG_RGB_PART], 
					   XcG_BEST_SIZE,
					   WIDGET(ObjTmp->L_B), 90,
					   WIDGET(ObjTmp->V_B), XcG_BEST_SIZE,
					   XcG_WIDGET_NULL, 7,
					   WIDGET(ObjTmp->S_B), XcG_MAX_SIZE,
					   NULL);
 F(ObjTmp->G_type[XcG_RGB_PART]).addColumn(ObjTmp->G_type[XcG_RGB_PART], 
					   XcG_MAX_SIZE,
					   NULL);

 ObjTmp->part_created[XcG_RGB_PART] = TRUE;
 F(man_w).Manage_all(man_w, (Widget)0);
 F(ObjTmp->G_type[XcG_RGB_PART]).callResize(ObjTmp->G_type[XcG_RGB_PART]);
}

/* ----------------------------------------------------------------- ** 
** set interface part RGB                                            ** 
** ----------------------------------------------------------------- */
static void set_RGB_part(Obj)
c_Box_color *Obj;
{
 
 F(Obj->V_R).set_value_in_unit(Obj->V_R, XcC_PERCENT_COLOR(Obj->rgb.r));
 F(Obj->S_R).change(Obj->S_R, (int)rint(XcC_PERCENT_COLOR(Obj->rgb.r)));
 F(Obj->V_G).set_value_in_unit(Obj->V_G, XcC_PERCENT_COLOR(Obj->rgb.g));
 F(Obj->S_G).change(Obj->S_G, (int)rint(XcC_PERCENT_COLOR(Obj->rgb.g)));
 F(Obj->V_B).set_value_in_unit(Obj->V_B, XcC_PERCENT_COLOR(Obj->rgb.b));
 F(Obj->S_B).change(Obj->S_B, (int)rint(XcC_PERCENT_COLOR(Obj->rgb.b)));

}

/* ----------------------------------------------------------------- ** 
** create interface part HSL                                         ** 
** ----------------------------------------------------------------- */
static void create_part_HSL(This, width, height)
c_Box_color	*This;
Dimension	width;
Dimension	height;
{
 c_ManageWidget		*man_w;
 char		str_H[8];
 char		str_S[8];
 char		str_L[8];

 sprintf(str_H, "%.1f", XcC_PERCENT_COLOR(This->hsl.h) * 3.6);
 sprintf(str_S, "%.1f", XcC_PERCENT_COLOR(This->hsl.s));
 sprintf(str_L, "%.1f", XcC_PERCENT_COLOR(This->hsl.l));

 man_w = (c_ManageWidget *)NEW(c_ManageWidget)();

 This->G_type[XcG_HSL_PART] = (c_Grid *)NEW(c_Grid)(WIDGET(This->Fmain),
						    NULL,
						    "gBC_HSL", TRUE);
 XtVaSetValues(WIDGET(This->G_type[XcG_HSL_PART]),
	       XmNwidth, width,
	       XmNheight, height, NULL);

 This->L_H = 
  (c_Label *)NEW(c_Label)(WIDGET(This->G_type[XcG_HSL_PART]),
			  man_w,
			  "l_H");
 This->L_S = 
  (c_Label *)NEW(c_Label)(WIDGET(This->G_type[XcG_HSL_PART]),
			  man_w,
			  "l_S");
 This->L_L = 
  (c_Label *)NEW(c_Label)(WIDGET(This->G_type[XcG_HSL_PART]),
			  man_w,
			  "l_L");

 This->V_H = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->G_type[XcG_HSL_PART]),
				    man_w,
				    "tf_H",
				    TF_REAL,
				    0.0, 360.0,
				    str_H, NULL, 5);
 F(This->V_H).set_unit(This->V_H, XcUB_ANGLE_DEGREE,
		       "%.1f", TRUE, FALSE);

 This->V_S = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->G_type[XcG_HSL_PART]),
				    man_w,
				    "tf_S",
				    TF_REAL,
				    0.0, 100.0,
				    str_S, NULL, 5);
 F(This->V_S).set_unit(This->V_S, XcUB_RATIO_PERCENTAGE,
		       "%.1f", TRUE, FALSE);
 This->V_L = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->G_type[XcG_HSL_PART]),
				    man_w,
				    "tf_L",
				    TF_INTEGER,
				    0.0, 100.0,
				    str_L, NULL, 5);
 F(This->V_L).set_unit(This->V_L, XcUB_RATIO_PERCENTAGE,
		       "%.1f", TRUE, FALSE);
 F(This->V_H).add_callback(This->V_H, (WTextFieldProc)certify_text_hsl,
			   This);
 F(This->V_S).add_callback(This->V_S, (WTextFieldProc)certify_text_hsl,
			   This);
 F(This->V_L).add_callback(This->V_L, (WTextFieldProc)certify_text_hsl,
			   This);
 
 This->S_H = 
  (c_Scale *)NEW(c_Scale)(WIDGET(This->G_type[XcG_HSL_PART]),
			  man_w,
			  "s_H",
			  0, &(This->hsl.h), 360,
			  convertH, invert_convertH);
 This->S_S = 
  (c_Scale *)NEW(c_Scale)(WIDGET(This->G_type[XcG_HSL_PART]),
			  man_w,
			  "s_S",
			  0, &(This->hsl.s), 100,
			  convertCMYK, invert_convertCMYK);
 
 This->S_L = 
  (c_Scale *)NEW(c_Scale)(WIDGET(This->G_type[XcG_HSL_PART]),
			  man_w,
			  "s_L",
			  0, &(This->hsl.l), 100,
			  convertCMYK, invert_convertCMYK);
 
 F(This->S_H).set_drag_callback(This->S_H, update_text_hsl,
				This);
 F(This->S_H).set_valuechange_callback(This->S_H, update_text_hsl,
				       This);
 F(This->S_S).set_drag_callback(This->S_S, update_text_hsl,
				This);
 F(This->S_S).set_valuechange_callback(This->S_S, update_text_hsl,
				       This);
 F(This->S_L).set_drag_callback(This->S_L, update_text_hsl,
				This);
 F(This->S_L).set_valuechange_callback(This->S_L, update_text_hsl,
				       This);

 F(This->G_type[XcG_HSL_PART]).addColumn(This->G_type[XcG_HSL_PART], 
					 XcG_MAX_SIZE,
					 NULL);
 F(This->G_type[XcG_HSL_PART]).addColumn(This->G_type[XcG_HSL_PART], 
					 XcG_BEST_SIZE,
					 WIDGET(This->L_H), 90,
					 WIDGET(This->V_H), XcG_BEST_SIZE,
					 XcG_WIDGET_NULL, 7,
					 WIDGET(This->S_H), XcG_MAX_SIZE,
					 NULL);
 F(This->G_type[XcG_HSL_PART]).addColumn(This->G_type[XcG_HSL_PART], 
					 XcG_MAX_SIZE,
					 NULL);
 F(This->G_type[XcG_HSL_PART]).addColumn(This->G_type[XcG_HSL_PART], 
					 XcG_BEST_SIZE,
					 WIDGET(This->L_S), 90,
					 WIDGET(This->V_S), XcG_BEST_SIZE,
					 XcG_WIDGET_NULL, 7,
					 WIDGET(This->S_S), XcG_MAX_SIZE,
					 NULL);
 F(This->G_type[XcG_HSL_PART]).addColumn(This->G_type[XcG_HSL_PART], 
					 XcG_MAX_SIZE,
					 NULL);
 F(This->G_type[XcG_HSL_PART]).addColumn(This->G_type[XcG_HSL_PART], 
					 XcG_BEST_SIZE,
					 WIDGET(This->L_L), 90,
					 WIDGET(This->V_L), XcG_BEST_SIZE,
					 XcG_WIDGET_NULL, 7,
					 WIDGET(This->S_L), XcG_MAX_SIZE,
					 NULL);
 F(This->G_type[XcG_HSL_PART]).addColumn(This->G_type[XcG_HSL_PART], 
					 XcG_MAX_SIZE,
					 NULL);

 This->part_created[XcG_HSL_PART] = TRUE;
 F(man_w).Manage_all(man_w, (Widget)0);
 F(This->G_type[XcG_HSL_PART]).callResize(This->G_type[XcG_HSL_PART]);
}

/* ----------------------------------------------------------------- ** 
** set interface part HSL                                            ** 
** ----------------------------------------------------------------- */
static void set_HSL_part(Obj)
c_Box_color *Obj;
{
 F(Obj->V_H).set_value_in_unit(Obj->V_H, XcC_PERCENT_COLOR(Obj->hsl.h) * 3.6);
 F(Obj->S_H).change(Obj->S_H, (int)rint(XcC_PERCENT_COLOR(Obj->hsl.h) * 3.6));
 F(Obj->V_S).set_value_in_unit(Obj->V_S, XcC_PERCENT_COLOR(Obj->hsl.s));
 F(Obj->S_S).change(Obj->S_S, (int)rint(XcC_PERCENT_COLOR(Obj->hsl.s)));
 F(Obj->V_L).set_value_in_unit(Obj->V_L, XcC_PERCENT_COLOR(Obj->hsl.l));
 F(Obj->S_L).change(Obj->S_L, (int)rint(XcC_PERCENT_COLOR(Obj->hsl.l)));
}

/* ----------------------------------------------------------------- ** 
** create interface part GRAY                                        ** 
** ----------------------------------------------------------------- */
static void create_part_GRAY(This, width, height)
c_Box_color	*This;
Dimension	width;
Dimension	height;
{
 c_ManageWidget		*man_w;
 char		str_G1[8];

 sprintf(str_G1, "%.1f", XcC_PERCENT_COLOR(This->ycbcr.y));

 man_w = (c_ManageWidget *)NEW(c_ManageWidget)();

 This->G_type[XcG_GRAY_PART] = (c_Grid *)NEW(c_Grid)(WIDGET(This->Fmain),
						     NULL,
						     "gBC_GRAY", TRUE);
 XtVaSetValues(WIDGET(This->G_type[XcG_GRAY_PART]),
	       XmNwidth, width,
	       XmNheight, height, NULL);

 This->L_G1 = 
  (c_Label *)NEW(c_Label)(WIDGET(This->G_type[XcG_GRAY_PART]),
			  man_w,
			  "l_G1");

 This->V_G1 = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->G_type[XcG_GRAY_PART]),
				    man_w,
				    "tf_G1",
				    TF_REAL,
				    0.0, 100.0,
				    str_G1, NULL, 5);
 F(This->V_G1).set_unit(This->V_G1, XcUB_RATIO_PERCENTAGE,
			"%.1f", TRUE, FALSE);
  
 F(This->V_G1).add_callback(This->V_G1, (WTextFieldProc)certify_text_gray,
			    This);

 This->S_G1 = 
  (c_Scale *)NEW(c_Scale)(WIDGET(This->G_type[XcG_GRAY_PART]),
			  man_w,
			  "s_G1",
			  0, &(This->ycbcr.y), 100,
			  convertCMYK, invert_convertCMYK);
 
 F(This->S_G1).set_drag_callback(This->S_G1, update_text_gray,
				 This);
 F(This->S_G1).set_valuechange_callback(This->S_G1, update_text_gray,
					This);

 F(This->G_type[XcG_GRAY_PART]).addColumn(This->G_type[XcG_GRAY_PART], 
					  XcG_MAX_SIZE,
					  NULL);
 F(This->G_type[XcG_GRAY_PART]).addColumn(This->G_type[XcG_GRAY_PART], 
					  XcG_BEST_SIZE,
					  WIDGET(This->L_G1), 90,
					  WIDGET(This->V_G1), 
					  XcG_BEST_SIZE,
					  XcG_WIDGET_NULL, 7,
					  WIDGET(This->S_G1), XcG_MAX_SIZE,
					  NULL);
 F(This->G_type[XcG_GRAY_PART]).addColumn(This->G_type[XcG_GRAY_PART], 
					  XcG_MAX_SIZE,
					  NULL);


 This->part_created[XcG_GRAY_PART] = TRUE;
 F(man_w).Manage_all(man_w, (Widget)0);
 F(This->G_type[XcG_GRAY_PART]).callResize(This->G_type[XcG_GRAY_PART]);
}

/* ----------------------------------------------------------------- ** 
** set interface part GRAY                                           ** 
** ----------------------------------------------------------------- */
static void set_GRAY_part(Obj)
c_Box_color *Obj;
{
 F(Obj->V_G1).set_value_in_unit(Obj->V_G1,
				XcC_PERCENT_COLOR(Obj->ycbcr.y));
 F(Obj->S_G1).change(Obj->S_G1, (int)rint(XcC_PERCENT_COLOR(Obj->ycbcr.y)));
}

#ifdef __YCBCR__
/* ----------------------------------------------------------------- ** 
** create interface part YCBCR                                       ** 
** ----------------------------------------------------------------- */
static void create_part_YCBCR(This, width, height)
c_Box_color	*This;
Dimension	width;
Dimension	height;
{
 c_ManageWidget		*man_w;

 man_w = (c_ManageWidget *)NEW(c_ManageWidget)();

 This->G_type[XcG_YCBCR_PART] = (c_Grid *)NEW(c_Grid)(WIDGET(This->Fmain),
						      NULL,
						      "gBC_YCBCR", TRUE);
 XtVaSetValues(WIDGET(This->G_type[XcG_YCBCR_PART]),
	       XmNwidth, width,
	       XmNheight, height, NULL);

 This->L_Y1 = 
  (c_Label *)NEW(c_Label)(WIDGET(This->G_type[XcG_YCBCR_PART]),
			  man_w,
			  "l_Y1");
 This->L_Cb = 
  (c_Label *)NEW(c_Label)(WIDGET(This->G_type[XcG_YCBCR_PART]),
			  man_w,
			  "l_Cb");
 This->L_Cr = 
  (c_Label *)NEW(c_Label)(WIDGET(This->G_type[XcG_YCBCR_PART]),
			  man_w,
			  "l_Cr");

 This->V_Y1 = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->G_type[XcG_YCBCR_PART]),
				    man_w,
				    "tf_Y1",
				    TF_REAL,
				    0.0, 100.0,
				    "", NULL, 5);
 F(This->V_Y1).set_unit(This->V_Y1, XcUB_RATIO_PERCENTAGE,
			"%.1f", TRUE, FALSE);
 This->V_Cb = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->G_type[XcG_YCBCR_PART]),
				    man_w,
				    "tf_Cb",
				    TF_REAL,
				    0.0, 100.0,
				    "", NULL, 5);
 F(This->V_Cb).set_unit(This->V_Cb, XcUB_RATIO_PERCENTAGE,
			"%.1f", TRUE, FALSE);
 This->V_Cr = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->G_type[XcG_YCBCR_PART]),
				    man_w,
				    "tf_Cr",
				    TF_REAL,
				    0.0, 100.0,
				    "", NULL, 5);
 F(This->V_Cr).set_unit(This->V_Cr, XcUB_RATIO_PERCENTAGE,
			"%.1f", TRUE, FALSE);
  
 F(This->V_Y1).add_callback(This->V_Y1, (WTextFieldProc)certify_text_YCbCr,
			    This);
 F(This->V_Cb).add_callback(This->V_Cb, (WTextFieldProc)certify_text_YCbCr,
			    This);
 F(This->V_Cr).add_callback(This->V_Cr, (WTextFieldProc)certify_text_YCbCr,
			    This);
 This->S_Y1 = 
  (c_Scale *)NEW(c_Scale)(WIDGET(This->G_type[XcG_YCBCR_PART]),
			  man_w,
			  "s_Y1",
			  0, &(This->ycbcr.y), 100,
			  NULL, NULL);
 This->S_Cb = 
  (c_Scale *)NEW(c_Scale)(WIDGET(This->G_type[XcG_YCBCR_PART]),
			  man_w,
			  "s_Cb",
			  0, &(This->ycbcr.cb), 100,
			  NULL, NULL);
 
 This->S_Cr = 
  (c_Scale *)NEW(c_Scale)(WIDGET(This->G_type[XcG_YCBCR_PART]),
			  man_w,
			  "s_Cr",
			  0, &(This->ycbcr.cr), 100,
			  NULL, NULL);
 
 F(This->S_H).set_drag_callback(This->S_H, update_text_ycbcr,
				This);
 F(This->S_H).set_valuechange_callback(This->S_H, update_text_ycbcr,
				       This);
 F(This->S_S).set_drag_callback(This->S_S, update_text_ycbcr,
				This);
 F(This->S_S).set_valuechange_callback(This->S_S, update_text_ycbcr,
				       This);
 F(This->S_L).set_drag_callback(This->S_L, update_text_ycbcr,
				This);
 F(This->S_L).set_valuechange_callback(This->S_L, update_text_ycbcr,
				       This);

 F(This->G_type[XcG_YCBCR_PART]).addColumn(This->G_type[XcG_YCBCR_PART], 
					   XcG_MAX_SIZE,
					   NULL);
 F(This->G_type[XcG_YCBCR_PART]).addColumn(This->G_type[XcG_YCBCR_PART], 
					   XcG_BEST_SIZE,
					   WIDGET(This->L_Y1), 90,
					   WIDGET(This->V_Y1), XcG_BEST_SIZE,
					   XcG_WIDGET_NULL, 7,
					   WIDGET(This->S_Y1), XcG_MAX_SIZE,
					   NULL);
 F(This->G_type[XcG_YCBCR_PART]).addColumn(This->G_type[XcG_YCBCR_PART], 
					   XcG_MAX_SIZE,
					   NULL);
 F(This->G_type[XcG_YCBCR_PART]).addColumn(This->G_type[XcG_YCBCR_PART], 
					   XcG_BEST_SIZE,
					   WIDGET(This->L_Cb), 90,
					   WIDGET(This->V_Cb), XcG_BEST_SIZE,
					   XcG_WIDGET_NULL, 7,
					   WIDGET(This->S_Cb), XcG_MAX_SIZE,
					   NULL);
 F(This->G_type[XcG_YCBCR_PART]).addColumn(This->G_type[XcG_YCBCR_PART], 
					   XcG_MAX_SIZE,
					   NULL);
 F(This->G_type[XcG_YCBCR_PART]).addColumn(This->G_type[XcG_YCBCR_PART], 
					   XcG_BEST_SIZE,
					   WIDGET(This->L_Cr), 90,
					   WIDGET(This->V_Cr), XcG_BEST_SIZE,
					   XcG_WIDGET_NULL, 7,
					   WIDGET(This->S_Cr), XcG_MAX_SIZE,
					   NULL);
 F(This->G_type[XcG_YCBCR_PART]).addColumn(This->G_type[XcG_YCBCR_PART], 
					   XcG_MAX_SIZE,
					   NULL);

 This->part_created[XcG_YCBCR_PART] = TRUE;
 F(man_w).Manage_all(man_w, (Widget)0);
 F(This->G_type[XcG_YCBCR_PART]).callResize(This->G_type[XcG_YCBCR_PART]);
}

/* ----------------------------------------------------------------- ** 
** set interface part YCBCR                                          ** 
** ----------------------------------------------------------------- */
static void set_YCBCR_part(Obj)
c_Box_color *Obj;
{
 
 /*
   
   attention, les noms ne sont pas bons...
   
   F(Obj->V_).set_value_in_unit(Obj->V_H, (int)XcC_PERCENT_COLOR(Obj->hsl.h));
   F(Obj->S_H).change(Obj->S_H, XcC_PERCENT_COLOR(Obj->hsl.h));
   F(Obj->V_S).set_value_in_unit(Obj->V_S, (int)XcC_PERCENT_COLOR(Obj->hsl.s));
   F(Obj->S_S).change(Obj->S_S, XcC_PERCENT_COLOR(Obj->hsl.s));
   F(Obj->V_L).set_value_in_unit(Obj->V_L, (int)XcC_PERCENT_COLOR(Obj->hsl.s));
   F(Obj->S_L).change(Obj->S_L, XcC_PERCENT_COLOR(Obj->hsl.l));
   */
}
#endif

/* ----------------------------------------------------------------- ** 
** convert HSL to RGB colorspace                                     ** 
** ----------------------------------------------------------------- */
static void HSLtoRGB(hsl, rgb)
hsl_t	*hsl;
rgb_t	*rgb;
{
 int     i;
 float	 h, s, l, r, g, b;
 float   f, p, q, t;
 

 h =  XcC_DESCALE_COLOR(float, hsl->h);
 s =  XcC_DESCALE_COLOR(float, hsl->s);
 l =  XcC_DESCALE_COLOR(float, hsl->l);
 
 if (h >= 1.0) h -= 1.0;

 if (s == 0.0 && h == 0.0)
 {
  rgb->r = rgb->g = rgb->b = XcC_SCALE_COLOR(l);
 } 
 else
 {
  h = h * 6.0;
  
  i = (int)h;
  f = h - i;
  p = l * (1.0 - s);
  q = l * (1.0 - (s * f));
  t = l * (1.0 - (s * (1.0 - f)));
  switch (i)
  {
  case 0: r = l; g = t; b = p; break;
  case 1: r = q; g = l; b = p; break;
  case 2: r = p; g = l; b = t; break;
  case 3: r = p; g = q; b = l; break;
  case 4: r = t; g = p; b = l; break;
  default:
  case 5: r = l; g = p; b = q; break;
  }
  rgb->r = XcC_SCALE_COLOR(r);
  rgb->g = XcC_SCALE_COLOR(g);
  rgb->b = XcC_SCALE_COLOR(b);
 }
}

/* ----------------------------------------------------------------- ** 
** Convert RGB to HSL colorspace                                     ** 
** ----------------------------------------------------------------- */
static void RGBtoHSL(rgb, hsl)
rgb_t	*rgb;
hsl_t	*hsl;
{
 float	  r = XcC_DESCALE_COLOR(float, rgb->r);
 float	  g = XcC_DESCALE_COLOR(float, rgb->g);
 float	  b = XcC_DESCALE_COLOR(float, rgb->b);
 float   max = MAX(r, MAX(g, b));
 float   min = MIN(r, MIN(g, b));
 float   h, delta;
  
 hsl->l = XcC_SCALE_COLOR(max);
 if (max != 0.0)
  hsl->s = XcC_SCALE_COLOR((max - min) / max);
 else
  hsl->s = XcC_SCALE_COLOR(0.0);
  
 if (hsl->s == 0)
 {
  hsl->h = 0.0;
 }
 else
 {
  delta = max - min;
  if (r == max) 
   h = (g - b) / delta;
  else
   if (g == max)
    h = 2 + (b - r) / delta;
   else
    h = 4 + (r - g) / delta;

  if (h > 6.0)
   h = h - 6.0;
  if (h < 0)
   h = h + 6.0;
  hsl->h = XcC_SCALE_COLOR(h / 6.0);
 }
}



