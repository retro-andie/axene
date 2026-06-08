/*
** Box_nombres.old.c for XQuad in Box_nombres/
** Object and method for Box_nombres Class
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
** Started on  Mon May 22 13:32:14 1995 Robin Castanier
** Last update Fri Dec 29 12:04:32 1995 Robin Castanier
*/

#include "../Pixmaps/item.del.xpm"
#include "../Pixmaps/item.new.xpm"
#include "Box_nombres.h"

void *cons_Box_nombres();
void dest_Box_nombres();
void *copy_Box_nombres();

static void Add_Item();
static void Del_SItem();
static void Change_Sel();
static void certify_precmax();
static void certify_precmin();
static void certify_space_each();
static void certify_unit();
static void certify_coma_sep();
static void certify_mill_sep();
static void changecpos();
static void changecneg();
static void changeunitpos();
static void changeafftrunc();

static void cb_action ___PROTO((c_CustomDialog	*cb_dialog,
				c_Box_nombres	*box,
				int		cb_reason));
static void cb_change ___PROTO((c_Box_nombres	*box,
				c_FormatNbr	*format,
				int		type,
				int		num,
				void 		*data));
static boolean format_filter ___PROTO((c_FormatNbr *this));

extern c_Color	*GlobColorAuto;

sf_Box_nombres fc_Box_nombres =
{
  cons_Box_nombres,
  dest_Box_nombres,
  copy_Box_nombres
};

void *cons_Box_nombres(w_Parent, pszTitle, BaseStd, format)
Widget		w_Parent;
char		*pszTitle;
BaseStd_t 	*BaseStd;
c_FormatNbr	**format;
{
  c_Box_nombres	*ObjTmp;
  Arg	        args[5]; 
  XmString      strTitre;
  
  if ((ObjTmp = (c_Box_nombres *)Xc_malloc("Box_numbers",
					   sizeof(c_Box_nombres))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_Box_nombres;

  ObjTmp->format_base = &(BaseStd->format_base);
  ObjTmp->v_format_base = NULL;
  ObjTmp->list_save = (c_ModifList *)
    NEW(c_ModifList)(ObjTmp, cb_change,
		     10,
		     (int)(offsetof(c_FormatNbr, max_prec)),
		     (int)sizeof(unsigned int),
		     (int)(offsetof(c_FormatNbr, min_prec)),
		     (int)sizeof(unsigned int),
		     (int)(offsetof(c_FormatNbr, space_each)),
		     (int)sizeof(unsigned int),
		     (int)(offsetof(c_FormatNbr, unit)),
		     (int)sizeof(char *),
		     (int)(offsetof(c_FormatNbr, comma_sep)),
		     (int)sizeof(char),
		     (int)(offsetof(c_FormatNbr, pos)),
		     (int)sizeof(c_Color *),
		     (int)(offsetof(c_FormatNbr, neg)),
		     (int)sizeof(c_Color *),
		     (int)(offsetof(c_FormatNbr, unitafter)),
		     (int)sizeof(int),
		     (int)(offsetof(c_FormatNbr, char_sep)),
		     (int)sizeof(char),
		     (int)(offsetof(c_FormatNbr, afftrunc)),
		     (int)sizeof(int),
		     (int)(offsetof(c_FormatNbr, name)),
		     (int)XcMAX_NAME_LENGTH);

  ObjTmp->color_base = BaseStd->color_base;

  ObjTmp->PBox = (c_ASLBox *)NEW(c_ASLBox)(w_Parent,
					   pszTitle,
					   680,	560,
					   FALSE, 
					   offsetof(c_FormatNbr, name));

  strTitre = F(GlobResources).getXmString(GlobResources, XcR_styleNumbers);
  XtSetArg(args[0], XmNlabelString, strTitre);
  XtSetValues(ObjTmp->PBox->wLabel, args, 1);
  XmStringFree(strTitre);

  F(ObjTmp->PBox).setCallback(ObjTmp->PBox, Change_Sel,
			      ObjTmp);
  
  F(ObjTmp->PBox).Add_Icons(ObjTmp->PBox, "add", item_new_xpm, Add_Item,
			    ObjTmp,
			    NULL);
  F(ObjTmp->PBox).Add_Icons(ObjTmp->PBox, "del", item_del_xpm, Del_SItem,
			    ObjTmp,
			    NULL);

  if (!BaseStd->format_base)
    NEW(c_FormatNbr)(&(BaseStd->format_base), "New_format");

  WIDGET(ObjTmp) = WIDGET(ObjTmp->PBox);
  ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				       MANAGER(ObjTmp->PBox->Dialog),
				       "gBoxNumbers", FALSE);
  
  ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->PBox->Dialog), 
					   "fBNColors", 0);
  ObjTmp->Grid1 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame1),
					MANAGER(ObjTmp->PBox->Dialog),
					"gBNColors", TRUE);
  ObjTmp->LaColorPos = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
					       MANAGER(ObjTmp->PBox->Dialog),
					       "lBNPositiveColor");
  ObjTmp->ItColorPos = (c_ItemMenu *)
    NEW(c_ItemMenu)(WIDGET(ObjTmp->Grid1),
		    MANAGER(ObjTmp->PBox->Dialog),
		    "imBNPositiveColor",
		    0, 0);

  F(ObjTmp->ItColorPos).setCallback(ObjTmp->ItColorPos,
				    changecpos, ObjTmp);
  F(ObjTmp->ItColorPos).set(ObjTmp->ItColorPos,
			    FALSE, offsetof(c_Color, name),
			    NULL, NULL);
  F(ObjTmp->ItColorPos).addItems(ObjTmp->ItColorPos,
				 XcIM_LINKED_LIST, -1,
				 BaseStd->color_base,
				 offsetof(c_Color, next));
  F(ObjTmp->ItColorPos).addItems(ObjTmp->ItColorPos,
				 XcIM_LINKED_LIST, -1,
				 GlobColorAuto,
				 offsetof(c_Color, next));

  ObjTmp->LaColorNeg = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
					       MANAGER(ObjTmp->PBox->Dialog),
					       "lBNNegativeColor");
  ObjTmp->ItColorNeg = (c_ItemMenu *)
    NEW(c_ItemMenu)(WIDGET(ObjTmp->Grid1),
		    MANAGER(ObjTmp->PBox->Dialog),
		    "imBNNegativeColor",
		    0, 0);
  F(ObjTmp->ItColorNeg).setCallback(ObjTmp->ItColorNeg,
				    changecneg, ObjTmp);
  F(ObjTmp->ItColorNeg).set(ObjTmp->ItColorNeg,
			    FALSE, offsetof(c_Color, name),
			    NULL, NULL);
  F(ObjTmp->ItColorNeg).addItems(ObjTmp->ItColorNeg,
				 XcIM_LINKED_LIST, -1,
				 BaseStd->color_base,
				 offsetof(c_Color, next)); 
  F(ObjTmp->ItColorNeg).addItems(ObjTmp->ItColorNeg,
				 XcIM_LINKED_LIST, -1,
				 GlobColorAuto,
				 offsetof(c_Color, next));

  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->LaColorPos), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->ItColorPos), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->LaColorNeg), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->ItColorNeg), XcG_BEST_SIZE,
			     NULL);
  	      
  ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->PBox->Dialog), 
					   "fBNUnits", 0);
  ObjTmp->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame2),
					MANAGER(ObjTmp->PBox->Dialog),
					"gBNUnits", TRUE);
  ObjTmp->LaUnitString = (c_Label *)
    NEW(c_Label)(WIDGET(ObjTmp->Grid2),
		 MANAGER(ObjTmp->PBox->Dialog),
		 "lBNUnitString");
  strcpy(ObjTmp->unit_string, "$");
  ObjTmp->WTunitString = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid2),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfBNUnitString",
				      TF_STRING,
				      0.0, 20.0,
				      ObjTmp->unit_string, NULL, 5);
  F(ObjTmp->WTunitString).add_callback(ObjTmp->WTunitString,
					 (void *)certify_unit,
					 ObjTmp);

  ObjTmp->TBunitPos = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid2),
					  MANAGER(ObjTmp->PBox->Dialog),
					  "tbBNUnitAfter", NULL);
  F(ObjTmp->TBunitPos).Add_Callback(ObjTmp->TBunitPos, 
				    XmNvalueChangedCallback,
				    (void *)changeunitpos,
				    ObjTmp);
  
  F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->LaUnitString), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->WTunitString), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TBunitPos), XcG_MAX_SIZE,
			     NULL);

  ObjTmp->Frame3 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->PBox->Dialog), 
					   "fBNPrecision", 0);
  ObjTmp->Grid3 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame3),
					MANAGER(ObjTmp->PBox->Dialog),
					"gBNPrecision", TRUE);
  ObjTmp->TBtruncate = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid3),
					  MANAGER(ObjTmp->PBox->Dialog),
					  "tbBNTruncate", NULL);
  F(ObjTmp->TBtruncate).Add_Callback(ObjTmp->TBtruncate, 
				     XmNvalueChangedCallback,
				     (void *)changeafftrunc,
				     ObjTmp);

  ObjTmp->LaPrecMax = (c_Label *)
    NEW(c_Label)(WIDGET(ObjTmp->Grid3),
		 MANAGER(ObjTmp->PBox->Dialog),
		 "lBNMaxPrecision");
  ObjTmp->WTprecMax = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid3),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfBNMaxPrecision",
				      TF_INTEGER,
				      0.0, 20.0,
				      "20", NULL, 5);
  F(ObjTmp->WTprecMax).add_callback(ObjTmp->WTprecMax,
				    (void *)certify_precmax,
				    ObjTmp);

  ObjTmp->LaPrecMin = (c_Label *)
    NEW(c_Label)(WIDGET(ObjTmp->Grid3),
		 MANAGER(ObjTmp->PBox->Dialog),
		 "lBNMinPrecision");
  ObjTmp->WTprecMin = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid3),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfBNMinPrecision",
				      TF_INTEGER,
				      0.0, 20.0,
				      "0", NULL, 5);
  F(ObjTmp->WTprecMin).add_callback(ObjTmp->WTprecMin,
				    (void *)certify_precmin,
				    ObjTmp);

  F(ObjTmp->Grid3).addColumn(ObjTmp->Grid3, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->TBtruncate), XcG_MAX_SIZE,
			     NULL);
  F(ObjTmp->Grid3).addColumn(ObjTmp->Grid3, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->LaPrecMax), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->WTprecMax), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid3).addColumn(ObjTmp->Grid3, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->LaPrecMin), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->WTprecMin), XcG_BEST_SIZE,
			     NULL);
  
  ObjTmp->Frame4 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->PBox->Dialog), 
					   "fBNSeparators", 0);
  ObjTmp->Grid4 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame4),
					MANAGER(ObjTmp->PBox->Dialog),
					"gBNSeparators", TRUE);
  ObjTmp->LaSepString = (c_Label *)
    NEW(c_Label)(WIDGET(ObjTmp->Grid4),
		 MANAGER(ObjTmp->PBox->Dialog),
		 "lBNSeparatorString");
  ObjTmp->WTsepString = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfBNSeparatorString",
				      TF_STRING,
				      0.0, 1.0,
				      " ", NULL, 5);
  F(ObjTmp->WTsepString).add_callback(ObjTmp->WTsepString,
				      (void *)certify_coma_sep,
				      ObjTmp);

  ObjTmp->LaSepMString = (c_Label *)
    NEW(c_Label)(WIDGET(ObjTmp->Grid4),
		 MANAGER(ObjTmp->PBox->Dialog),
		 "lBNSeparatorMString");
  ObjTmp->WTsepMString = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfBNSeparatorMString",
				      TF_STRING,
				      0.0, 1.0,
				      " ", NULL, 5);
  F(ObjTmp->WTsepMString).add_callback(ObjTmp->WTsepMString,
				       (void *)certify_mill_sep,
				       ObjTmp);


  ObjTmp->LaSepPos = (c_Label *)
    NEW(c_Label)(WIDGET(ObjTmp->Grid4),
		 MANAGER(ObjTmp->PBox->Dialog),
		 "lBNSeparatorPosition");
  ObjTmp->WTsepPos = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid4),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfBNSeparatorPosition",
				      TF_INTEGER,
				      0.0, 20.0,
				      "3", NULL, 5);
  F(ObjTmp->WTsepPos).add_callback(ObjTmp->WTsepPos,
				   (void *)certify_space_each,
				   ObjTmp);

  F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->LaSepString), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->WTsepString), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->LaSepMString), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->WTsepMString), XcG_BEST_SIZE,
			     NULL);
  F(ObjTmp->Grid4).addColumn(ObjTmp->Grid4, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->LaSepPos), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->WTsepPos), XcG_BEST_SIZE,
			     NULL);
  
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Frame1), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Frame2), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Frame3), 134,
			    WIDGET(ObjTmp->Frame4), 134,
			    NULL);

  F(ObjTmp->PBox->ItemList).set(ObjTmp->PBox->ItemList, -1,
				FALSE, -1L, -1, (void *)-1, 
				(void *)format_filter);
  F(ObjTmp->PBox->ItemList).addItems(ObjTmp->PBox->ItemList, FALSE,
				     XcIL_LINKED_LIST, -1,
				     BaseStd->format_base,
				     offsetof(c_FormatNbr, next));
  
  F(ObjTmp->PBox->Dialog).createActionArea(ObjTmp->PBox->Dialog,
					   XcCD_APPLY | XcCD_OK | 
					   XcCD_CANCEL, XcCD_APPLY,
					   (void (*)())cb_action, ObjTmp);
  
  F(ObjTmp->PBox).Map(ObjTmp->PBox, TRUE, *format);
  ObjTmp->retptr = format;
  Xc_TRACE(("Object Box_numbers build"));
  return ObjTmp;
}

void dest_Box_nombres(This)
c_Box_nombres *This;
{
  while (This->v_format_base != NULL)
    DELETE(c_FormatNbr)(This->v_format_base);
  DELETE(c_Label)(This->LaSepString);
  DELETE(c_WTextField)(This->WTsepString);
  DELETE(c_Label)(This->LaSepMString);
  DELETE(c_WTextField)(This->WTsepMString);
  DELETE(c_Label)(This->LaSepPos);
  DELETE(c_WTextField)(This->WTsepPos);

  DELETE(c_Label)(This->LaPrecMin);
  DELETE(c_WTextField)(This->WTprecMin);
  DELETE(c_Label)(This->LaPrecMax);
  DELETE(c_WTextField)(This->WTprecMax);

  DELETE(c_ToggleButton)(This->TBunitPos);
  DELETE(c_Label)(This->LaUnitString);
  DELETE(c_WTextField)(This->WTunitString);

  DELETE(c_ToggleButton)(This->TBtruncate);

  DELETE(c_Label)(This->LaColorPos);
  DELETE(c_ItemMenu)(This->ItColorPos);
  DELETE(c_Label)(This->LaColorNeg);
  DELETE(c_ItemMenu)(This->ItColorNeg);

  DELETE(c_Grid)(This->Grid1);
  DELETE(c_Grid)(This->Grid2);
  DELETE(c_Grid)(This->Grid3);
  DELETE(c_Grid)(This->Grid4);
  DELETE(c_Frame)(This->Frame1);
  DELETE(c_Frame)(This->Frame2);
  DELETE(c_Frame)(This->Frame3);
  DELETE(c_Frame)(This->Frame4);
  DELETE(c_Grid)(This->Grid);
  DELETE(c_ASLBox)(This->PBox);
  DELETE(c_ModifList)(This->list_save);
  Xc_free(This);
  Xc_TRACE(("Object Box_numbers destroyed"));
}

void *copy_Box_nombres(This)
c_Box_nombres *This;
{
  Xc_TRACE(("Copy not implemented"));
  return NULL;
}

/* ----------------------------------------------------------------- **
** Add_Item - Create a new style item                                **
** ----------------------------------------------------------------- */
static void Add_Item(w, This)
Widget		w;
c_Box_nombres	*This;
{
  c_FormatNbr	*tmp_format;
  char		*name;
  c_FormatNbr	*format;
  c_Color	*tposcolor, *tnegcolor;
  char		*tptr;
  unsigned int	tmaxprec, tminprec, tspace;
  char		tcomasep, tcharsep;
  int		tunitafter;
  int		tafftrunc;
  
  name = F(This->PBox).getNewItemName(This->PBox, (void **)&format);
  if (!name) return;
  
  if (F(This->list_save).get(This->list_save, format, 0, &tmaxprec) == -1)
    tmaxprec = format->max_prec;
  if (F(This->list_save).get(This->list_save, format, 1, &tminprec) == -1)
    tminprec = format->min_prec;
  if (F(This->list_save).get(This->list_save, format, 2, &tspace) == -1)
    tspace = format->space_each;
  if (F(This->list_save).get(This->list_save, format, 3, &tptr) == -1)
    tptr = format->unit;
  if (F(This->list_save).get(This->list_save, format, 4, &tcomasep) == -1)
    tcomasep = format->comma_sep;
  if (F(This->list_save).get(This->list_save, format, 5, &tposcolor) == -1)
    tposcolor = format->pos;
  if (F(This->list_save).get(This->list_save, format, 6, &tnegcolor) == -1)
    tnegcolor = format->neg;
  if (F(This->list_save).get(This->list_save, format, 7, &tunitafter) == -1)
    tunitafter = format->unitafter;
  if (F(This->list_save).get(This->list_save, format, 8, &tcharsep) == -1)
    tcharsep = format->char_sep;
  if (F(This->list_save).get(This->list_save, format, 9, &tafftrunc) == -1)
    tafftrunc = format->afftrunc;
 
  tmp_format = NEW(c_FormatNbr)(&(This->v_format_base), name);
  
  F(tmp_format).set(tmp_format,
		    XcF_MAXPREC, tmaxprec,
		    XcF_MINPREC, tminprec,
		    XcF_SPACE, tspace,
		    XcF_SUNIT, tptr,
		    XcF_COMMA, tcomasep,
		    XcF_COLORP, tposcolor,
		    XcF_COLORN, tnegcolor,
		    XcF_UNITAFTER, tunitafter,
		    XcF_CHARSEP, tcharsep,
		    XcF_AFFTRUNC, tafftrunc,
		    XcF_END);
  
  F(This->list_save).add(This->list_save, tmp_format);
  F(This->PBox->ItemList).setState(This->PBox->ItemList, FALSE,
				   XcIL_POINTER_ARRAY, -1);
  F(This->PBox->ItemList).addItems(This->PBox->ItemList, TRUE,
				   XcIL_VA_LIST, 1, tmp_format, NULL);
  F(This->PBox).selectItem(This->PBox, tmp_format, TRUE);
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
c_Box_nombres	*This;
{
  c_FormatNbr	**format;
  int		num, i;
  c_FormatNbr	*item;
  
  if (F(This->PBox->ItemList).getSelection(This->PBox->ItemList,
					  (void ***)&format, &num))
    {
      item =
	F(This->PBox->ItemList).getFirstUnselectItem(This->PBox->ItemList);
      for(i = 0; i < num; i++)
	{
	  if (XcH_USAGE(format[i]) == 0 && format[i]->lock_flag == FALSE &&
	      This->PBox->ItemList->item_count > 1)
	    {
	      F(This->PBox->ItemList).deleteItems(This->PBox->ItemList,
						  XcIL_POINTER_ARRAY, 1,
						  (void **)&(format[i]));  
	      F(This->list_save).sub(This->list_save, format[i]);
	    }
	}
      if (num)
	Xc_free(format);
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
static void Change_Sel(this, box_nombres, format)
c_ASLBox	*this;
c_Box_nombres	*box_nombres;
c_FormatNbr	*format;
{
  char		str_W[4];
  c_Color	*tcolor;
  char		*tptr;
  unsigned int	tprec;
  char		tchar;
  int		tunitafter;
  int		tafftrunc;

  if (F(box_nombres->list_save).get(box_nombres->list_save,
				    format, 0, &tprec) == -1)
    tprec = format->max_prec;
  sprintf(str_W, "%3d", tprec);
  F(box_nombres->WTprecMax).set(box_nombres->WTprecMax, str_W);

  if (F(box_nombres->list_save).get(box_nombres->list_save,
				    format, 1, &tprec) == -1)
    tprec = format->min_prec;
  sprintf(str_W, "%3d", tprec);
  F(box_nombres->WTprecMin).set(box_nombres->WTprecMin, str_W);

  if (F(box_nombres->list_save).get(box_nombres->list_save,
				    format, 2, &tprec) == -1)
    tprec = format->space_each;
  sprintf(str_W, "%3d", tprec);
  F(box_nombres->WTsepPos).set(box_nombres->WTsepPos, str_W);

  if (F(box_nombres->list_save).get(box_nombres->list_save,
				    format, 3, &tptr) == -1)
    tptr = format->unit;
  F(box_nombres->WTunitString).set(box_nombres->WTunitString, tptr);
  
  if (F(box_nombres->list_save).get(box_nombres->list_save,
				    format, 4, &tchar) == -1)
    tchar = format->comma_sep;
  sprintf(str_W, "%c", tchar);
  F(box_nombres->WTsepString).set(box_nombres->WTsepString, str_W);
  
  if (F(box_nombres->list_save).get(box_nombres->list_save,
				    format, 5, &tcolor) == -1)
    tcolor = format->pos;
  F(box_nombres->ItColorPos).setSelection
    (box_nombres->ItColorPos, tcolor);

  if (F(box_nombres->list_save).get(box_nombres->list_save,
				    format, 6, &tcolor) == -1)
    tcolor = format->neg;
  F(box_nombres->ItColorNeg).setSelection
    (box_nombres->ItColorNeg, tcolor);

  if (F(box_nombres->list_save).get(box_nombres->list_save,
				    format, 8, &tchar) == -1)
    tchar = format->char_sep;
  sprintf(str_W, "%c", tchar);
  F(box_nombres->WTsepMString).set(box_nombres->WTsepMString, str_W);
  
  if (F(box_nombres->list_save).get(box_nombres->list_save,
				    format, 7, &tunitafter) == -1)
    tunitafter = format->unitafter;
  Xc_TRACE(("Unit After %d", tunitafter));
  XmToggleButtonSetState(WIDGET(box_nombres->TBunitPos), tunitafter, False);
  
  if (F(box_nombres->list_save).get(box_nombres->list_save,
				    format, 9, &tafftrunc) == -1)
    tafftrunc = format->afftrunc;
  XmToggleButtonSetState(WIDGET(box_nombres->TBtruncate), tafftrunc, False);
}

char			tstr[100];

static void certify_precmax(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  c_FormatNbr		**format;
  unsigned int		numf;
  int			num2, i;
  unsigned int		tminprec, tset;

  if((vstr = F(this).get(this)) != NULL)
    {
      numf = atoi(vstr);

      if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					      (void ***)&format, &num2))
	{
	  if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
	    {
	      if (F(Obj->list_save).get(Obj->list_save,
					(c_FormatNbr *)Obj->PBox->edited_item, 
					1, &tminprec) == -1)
		tminprec = ((c_FormatNbr *)Obj->PBox->edited_item)->min_prec;
	      if (numf < tminprec)
		tset = tminprec;
	      else
		tset = numf;

	      F(Obj->list_save).set(Obj->list_save,
				    (c_FormatNbr *)Obj->PBox->edited_item,
				    0,
				    &tset);
	      
	      sprintf(tstr, "%d", tset);
	      F(Obj->WTprecMax).set(Obj->WTprecMax, tstr);
	    }
	  else
	    {
	      for(i = 0; i < num2; i++)
		{
		  if (F(Obj->list_save).get(Obj->list_save,
					    format[i], 
					    1, &tminprec) == -1)
		    tminprec = format[i]->min_prec;
		  if (numf < tminprec)
		    tset = tminprec;
		  else
		    tset = numf;
		  F(Obj->list_save).set(Obj->list_save,
					format[i],
					0,
					&tset);
		  if (!i)
		    {
		      if (tset != numf)
			{
			  sprintf(tstr, "%d", tset);
			  F(Obj->WTprecMax).set(Obj->WTprecMax, tstr);
			}
		    }
		}
	    }
	  if (num2)
	    Xc_free(format);
	}
      Xc_free(vstr);
      F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
    }
}

static void certify_precmin(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  c_FormatNbr		**format;
  unsigned int		numf;
  int			num2, i;
  unsigned int		tmaxprec, tset;

  if((vstr = F(this).get(this)) != NULL)
    {
      numf = atoi(vstr);
      
      if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					     (void ***)&format, &num2) && num2)
	{
	  if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
	    {
	      if (F(Obj->list_save).get(Obj->list_save,
					(c_FormatNbr *)Obj->PBox->edited_item, 
					0, &tmaxprec) == -1)
		tmaxprec = ((c_FormatNbr *)Obj->PBox->edited_item)->max_prec;
	      if (numf > tmaxprec)
		tset = tmaxprec;
	      else
		tset = numf;

	      F(Obj->list_save).set(Obj->list_save,
				    (c_FormatNbr *)Obj->PBox->edited_item,
				    1,
				    &tset);
	      sprintf(tstr, "%d", tset);
	      F(Obj->WTprecMax).set(Obj->WTprecMin, tstr);
	    }
	  else
	    {
	      for(i = 0; i < num2; i++)
		{
		  if (F(Obj->list_save).get(Obj->list_save,
					    format[i], 
					    0, &tmaxprec) == -1)
		    tmaxprec = format[i]->max_prec;
		  if (numf > tmaxprec)
		    tset = tmaxprec;
		  else
		    tset = numf;

		  F(Obj->list_save).set(Obj->list_save,
					format[i],
					1,
					&tset);
		  if (!i)
		    {
		      if (tset != numf)
			{
			  sprintf(tstr, "%d", tset);
			  F(Obj->WTprecMax).set(Obj->WTprecMin, tstr);
			}
		    }
		}
	    }
	  if (num2)
	    Xc_free(format);
	}
      Xc_free(vstr);
      F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
    }
}

static void certify_space_each(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  c_FormatNbr		**format;
  unsigned int		numf;
  int			num2, i;

  if((vstr = F(this).get(this)) != NULL)
    {
      F(Obj->WTsepPos).set(Obj->WTsepPos, vstr);
      numf = atoi(vstr);
      
      if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					     (void ***)&format, &num2) && num2)
	{
	  if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
	    {
	      F(Obj->list_save).set(Obj->list_save,
				    (c_FormatNbr *)Obj->PBox->edited_item,
				    2,
				    &numf);
	    }
	  else
	    {
	      for(i = 0; i < num2; i++)
		F(Obj->list_save).set(Obj->list_save,
				      format[i],
				      2,
				      &numf);
	    }
	  if (num2)
	    Xc_free(format);
	  
	}
      Xc_free(vstr);
      F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
    }
}


static void certify_unit(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  c_FormatNbr		**format;
  char			*numf;
  int			num2, i;

  if((vstr = F(this).get(this)) != NULL)
  {
    if (vstr[0]!=0)
      strcpy(Obj->unit_string, vstr);
    else
      Obj->unit_string[0] = '\0';
    F(Obj->WTunitString).set(Obj->WTunitString, vstr);
    
    if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					    (void ***)&format, &num2))
      {
	if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
	  {
	    if (F(Obj->list_save).get(Obj->list_save, 
				      (c_FormatNbr *)Obj->PBox->edited_item,
				      3, &numf) != -1)
	      {
		Xc_TRACE(("receveid: %s", numf));
		Xc_free(numf);
	      }
	    numf = Xc_strdup("unit str", Obj->unit_string);
	    Xc_TRACE(("new: %s", numf));
	    
	    F(Obj->list_save).set(Obj->list_save,
				  (c_FormatNbr *)Obj->PBox->edited_item,
				  3,
				  &numf);
	  }
	else
	  {
	    for(i = 0; i < num2; i++)
	      {
		if (F(Obj->list_save).get(Obj->list_save, 
					  format[i], 3, &numf) != -1)
		  {
		    Xc_TRACE(("received: %s", numf?numf:"NULL"));
		    Xc_free(numf);
		  }
		numf = Xc_strdup("unit str", Obj->unit_string);
		Xc_TRACE(("new: %s", numf?numf:"NULL"));
		
		F(Obj->list_save).set(Obj->list_save, format[i], 3, &numf);
	      }
	  }
	if (num2)
	  Xc_free(format);
      }
    Xc_free(vstr);
    F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
  }
}

static void certify_coma_sep(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  c_FormatNbr		**format;
  char			numf;
  int			num2, i;

  if((vstr = F(this).get(this)) != NULL)
    {
      F(Obj->WTsepString).set(Obj->WTsepString, vstr);
      numf = vstr[0];
      
      if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					     (void ***)&format, &num2) && num2)
	{
	  if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
	    {
	      F(Obj->list_save).set(Obj->list_save,
				    (c_FormatNbr *)Obj->PBox->edited_item,
				    4,
				    &numf);
	    }
	  else
	    {
	      for(i = 0; i < num2; i++)
		F(Obj->list_save).set(Obj->list_save,
				      format[i],
				      4,
				      &numf);
	    }
	  if (num2)
	    Xc_free(format);
	}
      Xc_free(vstr);
      F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
    }
}

static void certify_mill_sep(this, Obj)
c_WTextField	*this;
c_Box_nombres	*Obj;
{
  char			*vstr;
  c_FormatNbr		**format;
  char			numf;
  int			num2, i;

  if((vstr = F(this).get(this)) != NULL)
    {
      F(Obj->WTsepMString).set(Obj->WTsepMString, vstr);
      numf = vstr[0];
      
      if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					      (void ***)&format, &num2))
	{
	  if (num2 == 1 && ((void *)*format) != Obj->PBox->edited_item)
	    {
	      F(Obj->list_save).set(Obj->list_save,
				    (c_FormatNbr *)Obj->PBox->edited_item,
				    8,
				    &numf);
	    }
	  else
	    {
	      for(i = 0; i < num2; i++)
		F(Obj->list_save).set(Obj->list_save,
				      format[i],
				      8,
				      &numf);
	    }
	  if (num2)
	    Xc_free(format);
	}
      Xc_free(vstr);
      F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
    }
}

static void changecpos(This, Obj, color)
c_ItemMenu	*This;
c_Box_nombres	*Obj;
c_Color		*color;
{
  c_FormatNbr	**format;
  int		num2, i;

  if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					 (void ***)&format, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      format[i],
			      5,
			      &color);
      if (num2)
	Xc_free(format);
    }
}  

static void changecneg(This, Obj, color)
c_ItemMenu	*This;
c_Box_nombres	*Obj;
c_Color		*color;
{
  c_FormatNbr	**format;
  int		num2, i;

  if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					  (void ***)&format, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      format[i],
			      6,
			      &color);
      if (num2)
	Xc_free(format);
    }
}  

static void changeunitpos(This, Obj, cbs)
Widget				This;
c_Box_nombres			*Obj;
XmToggleButtonCallbackStruct	*cbs;
{
  c_FormatNbr	**format;
  int		num2, i;

  if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					  (void ***)&format, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      format[i],
			      7,
			      &(cbs->set));
      if (num2)
	Xc_free(format);
    }
}

static void changeafftrunc(This, Obj, cbs)
Widget				This;
c_Box_nombres			*Obj;
XmToggleButtonCallbackStruct	*cbs;
{
  c_FormatNbr	**format;
  int		num2, i;

  if (F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					  (void ***)&format, &num2))
    {
      for(i = 0; i < num2; i++)
	F(Obj->list_save).set(Obj->list_save,
			      format[i],
			      9,
			      &(cbs->set));
      if (num2)
	Xc_free(format);
    }
}

static void cb_action(cb_dialog, box, cb_reason)
c_CustomDialog	*cb_dialog;
c_Box_nombres	*box;
int		cb_reason;
{
  c_FormatNbr		*format, *format2;

  *(box->retptr) = NULL;
  switch (cb_reason)
    {
    case XcCD_APPLY:
      F(box->list_save).change_all2(box->list_save);
      format = (c_FormatNbr *)box->PBox->edited_item;
      format2 = *box->format_base;
      while(format2 && format2 != format)
	format2 = format2->next;
      if (format2)
	format = format2;
      else
	{
	  c_Color		*tposcolor, *tnegcolor;
	  char		*tptr;
	  unsigned int	tmaxprec, tminprec, tspace;
	  char		tcomasep, tcharsep;
	  int		tunitafter;
	  int		tafftrunc;
  
	  if (F(box->list_save).get(box->list_save,
				    format, 0, &tmaxprec) == -1)
	    tmaxprec = format->max_prec;
	  if (F(box->list_save).get(box->list_save,
				    format, 1, &tminprec) == -1)
	    tminprec = format->min_prec;
	  if (F(box->list_save).get(box->list_save,
				    format, 2, &tspace) == -1)
	    tspace = format->space_each;
	  if (F(box->list_save).get(box->list_save,
				    format, 3, &tptr) == -1)
	    tptr = format->unit;
	  if (F(box->list_save).get(box->list_save,
				    format, 4, &tcomasep) == -1)
	    tcomasep = format->comma_sep;
	  if (F(box->list_save).get(box->list_save,
				    format, 5, &tposcolor) == -1)
	    tposcolor = format->pos;
	  if (F(box->list_save).get(box->list_save,
				    format, 6, &tnegcolor) == -1)
	    tnegcolor = format->neg;
	  if (F(box->list_save).get(box->list_save,
				    format, 7, &tunitafter) ==-1)
	    tunitafter = format->unitafter;
	  if (F(box->list_save).get(box->list_save,
				    format, 8, &tcharsep) == -1)
	    tcharsep = format->char_sep;
	  if (F(box->list_save).get(box->list_save,
				    format, 9, &tafftrunc) ==-1)
	    tafftrunc = format->afftrunc;

	  F(format).set(format,
			XcF_MAXPREC, tmaxprec,
			XcF_MINPREC, tminprec,
			XcF_SPACE, tspace,
			XcF_SUNIT, tptr,
			XcF_COMMA, tcomasep,
			XcF_COLORP, tposcolor,
			XcF_COLORN, tnegcolor,
			XcF_UNITAFTER, tunitafter,
			XcF_CHARSEP, tcharsep,
			XcF_AFFTRUNC, tafftrunc,
			XcF_END);
	  
	  format2 = *box->format_base;
	  while(format2 && !F(format).compare(format, format2, CMP_WITH_NAME))
	    format2 = format2->next;
	  if (format2)
	    format = format2;
	  else
	    format = NULL;
	}
      *(box->retptr) = format;
    case XcCD_OK:
      F(box->list_save).change_all2(box->list_save);
      break;
    default:
      Xc_TRACE(("canceled"));
    }
  F(cb_dialog).unmap(cb_dialog);
}

static void cb_change(box, format, type, num, data)
c_Box_nombres	*box;
c_FormatNbr	*format;
int		type;
int		num;
void		*data;
{
  Xc_TRACE(("Test It NOW!"));
  switch (type)
    {
    case NEWSET:
      format = box->modif.format;
    case TOSET:
      Xc_TRACE(("TOSET!"));
      switch(num)
	{
	case 0:
	  box->modif.max_prec = ((unsigned int *)data)[0];
	  break;
	case 1:
	  box->modif.min_prec = ((unsigned int *)data)[0];
	  break;
	case 2:
	  box->modif.space_each = ((unsigned int *)data)[0];
	  break;
	case 3:
	  box->modif.unit = ((char **)data)[0];
	  break;
	case 4:
	  box->modif.coma_sep = ((char *)data)[0];
	  break;
	case 5:
	  box->modif.pos = ((c_Color **)data)[0];
	  break;
	case 6:
	  box->modif.neg = ((c_Color **)data)[0];
	  break;
	case 7:
	  box->modif.unitafter = ((int *)data)[0];
	  break;
	case 8:
	  box->modif.char_sep = ((char *)data)[0];
	  break;
	case 9:
	  box->modif.afftrunc = ((int *)data)[0];
	  
	  F(format).set(format,
			XcF_MAXPREC, box->modif.max_prec,
			XcF_MINPREC, box->modif.min_prec,
			XcF_SPACE, box->modif.space_each,
			XcF_SUNIT, box->modif.unit,
			XcF_COMMA, box->modif.coma_sep,
			XcF_COLORP, box->modif.pos,
			XcF_COLORN, box->modif.neg,
			XcF_UNITAFTER, box->modif.unitafter,
			XcF_CHARSEP, box->modif.char_sep,
			XcF_AFFTRUNC, box->modif.afftrunc,
			XcF_END);
	  Xc_TRACE(("setez: %s",  box->modif.unit));
	  break;
	}
      break;
    case TOADD:
      Xc_TRACE(("TOADD!"));
      box->modif.format = (c_FormatNbr *)NEW(c_FormatNbr)(box->format_base, 
							  format->name);
      break;
    case TOSUB:
      Xc_TRACE(("TOSUB!"));
      DELETE(c_FormatNbr)(format);
      break;
    }
}

/* ----------------------------------------------------------------- ** 
** format_filter - Filter formats                                    ** 
** ----------------------------------------------------------------- */
static boolean format_filter(this)
c_FormatNbr *this;
{
  return !this->hide_flag;
}
