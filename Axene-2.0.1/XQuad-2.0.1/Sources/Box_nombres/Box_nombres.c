/*
** Box_nombres.c for XQuad in Box_nombres/
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
** Last update Tue Jan 18 16:54:35 2000 Emmanuel Paris
*/

#include "Box_nombres.h"
#include "Resource.h"
#include "Colormap.h"
#include "Moteur.h"
extern c_Resource *GlobResources;
extern c_Colormap *GlobColormap;

static void *cons_Box_nombres();
static void dest_Box_nombres();
static void *copy_Box_nombres();
static void refresh_preview();

static void Add_Item();
static void Del_SItem();
static void Change_Sel();

static void cb_action ___PROTO((c_CustomDialog	*cb_dialog,
				c_Box_nombres	*box,
				int		cb_reason));

static void cb_change ___PROTO((c_Box_nombres	*box,
				c_FormatNbr	*format,
				int		type,
				int		num,
				void 		*data));

static boolean format_filter ___PROTO((c_FormatNbr *this));

static void cb_format_type ___PROTO((Widget w, c_Box_nombres *box,
				     bn_format_t *bn_format));

static void cb_bnp_bar ___PROTO((c_Box_nombres *box, int bar));
static void toggle_button ___PROTO((Widget w, c_Box_nombres *box));

static void copy_saved_format ___PROTO((c_Box_nombres *box,
					c_FormatNbr *dst, c_FormatNbr *src));

static int resource[NB_BN_FMT] =
{
 XcR_BnFmtAll,
 XcR_BnFmtNormal,
 XcR_BnFmtUnit,
 XcR_BnFmtExponantial,
 XcR_BnFmtPercent,
 XcR_BnFmtFrac,
 XcR_BnFmtBoolean,
 XcR_BnFmtDay,
 XcR_BnFmtMonth,
 XcR_BnFmtTime,
 XcR_BnFmtDate,
 XcR_BnFmtDateTime
};

static int filter_type;

sf_Box_nombres fc_Box_nombres =
{
 cons_Box_nombres,
 dest_Box_nombres,
 copy_Box_nombres,
 refresh_preview,
};

static int	convert_fmt_type[] =
{
 0, 0, 0, 0, BN_FMT_PERCENT, BN_FMT_NORMAL, BN_FMT_UNIT, BN_FMT_FRAC,
 BN_FMT_BOOLEAN, BN_FMT_DAY, BN_FMT_MONTH, BN_FMT_TIME, BN_FMT_DATE,
 BN_FMT_DATE_TIME, BN_FMT_EXPONANTIAL
};

static boolean extended_select[] =
{
 FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
 FALSE, FALSE, FALSE, TRUE
};

static void *cons_Box_nombres(w_Parent, pszTitle, BaseStd, format, preview)
Widget		w_Parent;
char		*pszTitle;
BaseStd_t 	*BaseStd;
c_FormatNbr	**format;
char		*preview;
{
 c_Box_nombres	*ObjTmp;
 Arg	        argsw[5]; 
 XmString      strTitre;
 int		i, j;
 xc_date_time_t  date_time; 

 if ((ObjTmp = (c_Box_nombres *)Xc_malloc("Box_numbers",
					  sizeof(c_Box_nombres))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Box_nombres;

 ObjTmp->format_base = &(BaseStd->format_base);
 ObjTmp->generic_format = 
  F(BaseStd->format_base).get(NULL, XcFt_GENERIC, BaseStd);
 ObjTmp->v_format_base = NULL;
 ObjTmp->d_format_base = NULL;
 ObjTmp->d_nb_format = 0;
 ObjTmp->preview_color = GlobColormap->Cblack;
 ObjTmp->preview_format = NULL;
 ObjTmp->preview_format =
  NEW(c_FormatNbr)(&ObjTmp->preview_format, "preview");
 ObjTmp->preview_string = preview;
 ObjTmp->dt_seq_base = NULL;
 ObjTmp->selected_bnp_bar = -1;
 ObjTmp->selected_bnp_std = -1;
 ObjTmp->selected_fmt = BN_FMT_ALL;
 ObjTmp->bnp_std_sel = NULL;
 ObjTmp->dim_calc = ObjTmp->modify_flag = FALSE;
 ObjTmp->width = ObjTmp->height = (Dimension)0;
  
 filter_type = Xq_ALL;

 ObjTmp->list_save = (c_ModifList *)
  NEW(c_ModifList)(ObjTmp, cb_change,
		   26,
		   (int)(offsetof(c_FormatNbr, max_prec)),
		   (int)sizeof(unsigned int),
		   (int)(offsetof(c_FormatNbr, min_prec)),
		   (int)sizeof(unsigned int),
		   (int)(offsetof(c_FormatNbr, space_each)),
		   (int)sizeof(unsigned int),
		   (int)(offsetof(c_FormatNbr, comma_sep)),
		   (int)sizeof(char),
		   (int)(offsetof(c_FormatNbr, color1)),
		   (int)sizeof(c_Color *),
		   (int)(offsetof(c_FormatNbr, color2)),
		   (int)sizeof(c_Color *),
		   (int)(offsetof(c_FormatNbr, char_sep)),
		   (int)sizeof(char),
		   (int)(offsetof(c_FormatNbr, truncate)),
		   (int)sizeof(boolean),
		     
		   (int)(offsetof(c_FormatNbr, unit)),
		   (int)XcMAX_NAME_LENGTH,
		   (int)(offsetof(c_FormatNbr, unitafter)),
		   (int)sizeof(boolean),
		     
		   (int)(offsetof(c_FormatNbr, exp_digit)),
		   (int)sizeof(unsigned int),
		   (int)(offsetof(c_FormatNbr, exp_multiple)),
		   (int)sizeof(unsigned int),
		   (int)(offsetof(c_FormatNbr, exp_sign)),
		   (int)sizeof(boolean),
		   (int)(offsetof(c_FormatNbr, exp_string)),
		   (int)XcMAX_NAME_LENGTH,
		     
		   (int)(offsetof(c_FormatNbr, frac_prec)),
		   (int)sizeof(unsigned int),
		   (int)(offsetof(c_FormatNbr, frac_dec)),
		   (int)sizeof(boolean),
		     
		   (int)(offsetof(c_FormatNbr, true_name)),
		   (int)XcMAX_NAME_LENGTH,
		   (int)(offsetof(c_FormatNbr, false_name)),
		   (int)XcMAX_NAME_LENGTH,
		     
		   (int)(offsetof(c_FormatNbr, day_name_size)),
		   (int)sizeof(name_size_id_t),
		   (int)(offsetof(c_FormatNbr, month_name_size)),
		   (int)sizeof(name_size_id_t),
		     
		   (int)(offsetof(c_FormatNbr, date_seq)),
		   (int)(sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1)),
		   (int)(offsetof(c_FormatNbr, time_seq)),
		   (int)(sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1)),
		   (int)(offsetof(c_FormatNbr, date_time_seq)),
		   (int)(sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1)),
		     
		   (int)(offsetof(c_FormatNbr, other_format)),
		   (int)(sizeof(c_FormatNbr *) * Xq_NB_TYPE),
		     
		   (int)(offsetof(c_FormatNbr, type)),
		   (int)(sizeof(int)),
		     
		   (int)(offsetof(c_FormatNbr, name)),
		   (int)XcMAX_NAME_LENGTH);
  
 ObjTmp->color_base = BaseStd->color_base;

 /*--- Menu ---*/
 ObjTmp->bn_format =
  (bn_format_t *)Xc_malloc("bn format",
			   sizeof(bn_format_t) * NB_BN_FMT);
 for(i = 1; i < NB_BN_FMT; i++)
 {
  ObjTmp->bn_format[i].name = 
   F(GlobResources).getString(GlobResources, resource[i]);
  ObjTmp->bn_format[i].num = i - 1;
 }
 ObjTmp->bn_format[0].name = 
  F(GlobResources).getString(GlobResources, resource[0]);
 ObjTmp->bn_format[0].num = BN_FMT_ALL;
  
 ObjTmp->bnp_bar = bnp_bar;
 fill_xc_date_and_time(&date_time);
  
 sprintf(ObjTmp->bnp_bar[BN_FMT_DAY]->preview_string,
	 "%d", date_time.date.day_in_week);
 sprintf(ObjTmp->bnp_bar[BN_FMT_MONTH]->preview_string,
	 "%d", date_time.date.month);
 sprintf(ObjTmp->bnp_bar[BN_FMT_TIME]->preview_string,
	 "%f", convert_to_xc_time(&date_time.time, NULL));
 sprintf(ObjTmp->bnp_bar[BN_FMT_DATE]->preview_string,
	 "%f", convert_to_xc_date(&date_time.date, NULL));
 sprintf(ObjTmp->bnp_bar[BN_FMT_DATE_TIME]->preview_string,
	 "%f", convert_to_xc_date_and_time(&date_time, NULL));

 /* --------------- Interface Construction Beginning -------------- */
  
 ObjTmp->PBox = (c_ASLBox *)NEW(c_ASLBox)(w_Parent,
					  pszTitle,
					  690,	560,
					  FALSE, 
					  offsetof(c_FormatNbr, name));
  
 strTitre = F(GlobResources).getXmString(GlobResources, XcR_styleNumbers);
 XtSetArg(argsw[0], XmNlabelString, strTitre);
 XtSetValues(WIDGET(ObjTmp->PBox->Label), argsw, 1);
 XmStringFree(strTitre);
 XtVaSetValues(LIST(ObjTmp->PBox->ItemList),
	       XmNselectionPolicy, XmBROWSE_SELECT,
	       NULL);
  
 ObjTmp->imBNFormatType = (c_ItemMenu *)
  NEW(c_ItemMenu)(WIDGET(ObjTmp->PBox->Form),
		  MANAGER(ObjTmp->PBox->Dialog),
		  "imBNFormatType",
		  0, 0);

 F(ObjTmp->imBNFormatType).set(ObjTmp->imBNFormatType, TRUE,
			       offsetof(bn_format_t, name), NULL, NULL);
 F(ObjTmp->imBNFormatType).setCallback(ObjTmp->imBNFormatType, 
				       (void *)cb_format_type,
				       ObjTmp);

 F(ObjTmp->imBNFormatType).addItems(ObjTmp->imBNFormatType, XcIM_ARRAY,
				    NB_BN_FMT, ObjTmp->bn_format,
				    sizeof(bn_format_t));
 F(ObjTmp->imBNFormatType).setSelection(ObjTmp->imBNFormatType, 
					ObjTmp->bn_format);
  

 XtVaSetValues(WIDGET(ObjTmp->imBNFormatType),
	       XmNtopAttachment, XmATTACH_WIDGET,
	       XmNtopWidget, WIDGET(ObjTmp->PBox->Label),
	       XmNbottomAttachment, XmATTACH_NONE,
	       XmNleftAttachment,  XmATTACH_FORM,
	       XmNrightAttachment,  XmATTACH_FORM,
	       NULL);

 XtVaSetValues(WIDGET(ObjTmp->PBox->ItemList),
	       XmNtopAttachment, XmATTACH_WIDGET,
	       XmNtopWidget, WIDGET(ObjTmp->imBNFormatType),
	       NULL);


 F(ObjTmp->PBox).setCallback(ObjTmp->PBox, Change_Sel, ObjTmp);
  
 F(ObjTmp->PBox).Add_Buttons(ObjTmp->PBox, "lBNumAdd", Add_Item,
			     ObjTmp,
			     NULL);
 F(ObjTmp->PBox).Add_Buttons(ObjTmp->PBox, "lBNumDel", Del_SItem,
			     ObjTmp,
			     NULL);

 WIDGET(ObjTmp) = WIDGET(ObjTmp->PBox);
 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "gBoxNumbers", FALSE);
 ObjTmp->fOnglet = NEW(c_Frame)(WIDGET(ObjTmp->Grid),
				MANAGER(ObjTmp->PBox->Dialog), 
				NULL, -1);
 XtSetArg(argsw[0], XmNshadowType, XmSHADOW_IN);
 XtSetArg(argsw[1], XmNshadowThickness, 1);
 XtSetValues(WIDGET(ObjTmp->fOnglet), argsw, 2);

#ifndef LESSTIF_VERSION
 ObjTmp->rcOnglet = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(ObjTmp->fOnglet), MANAGER(ObjTmp->PBox->Dialog),
   "rcBNBar", 0, 0, XmVERTICAL);


 XtVaSetValues(WIDGET(ObjTmp->rcOnglet),
	       XmNpacking, XmPACK_COLUMN,
	       XmNresizeHeight, False,
	       XmNresizeWidth, False,
	       XmNmarginHeight, 0,
	       XmNmarginWidth, 0,
	       XmNspacing, 0,
	       NULL);
#endif

 for( i = 0; i < NB_BNP_BAR; i++)
 {
#ifndef LESSTIF_VERSION
  ObjTmp->bnp_bar[i]->bnp_grid = (c_Grid *)NEW(c_Grid)
   (WIDGET(ObjTmp->rcOnglet), NULL, "gBNBar", FALSE);
#else  /* for LESSTIF_VERSION */
  ObjTmp->bnp_bar[i]->bnp_grid = (c_Grid *)NEW(c_Grid)
   (WIDGET(ObjTmp->fOnglet), NULL, "gBNBar", FALSE);
#endif

  XtVaSetValues(WIDGET(ObjTmp->bnp_bar[i]->bnp_grid),
		XmNmarginHeight, 0,
		XmNmarginWidth, 0,
		XmNspacing, 0,
		NULL);

  for(j = 0; j < ObjTmp->bnp_bar[i]->nb_bnp; j++)
  {
   ObjTmp->bnp_bar[i]->bnp[j]->created = FALSE;
   ObjTmp->bnp_bar[i]->bnp_tbutton[j] = 
    (c_ToggleButton *)NEW(c_ToggleButton)
     (WIDGET(ObjTmp->bnp_bar[i]->bnp_grid),
      NULL, ObjTmp->bnp_bar[i]->bnp_name[j], NULL);
	  
   XtVaSetValues(WIDGET(ObjTmp->bnp_bar[i]->bnp_tbutton[j]),
		 XmNindicatorOn, False,
		 XmNindicatorSize, 1,
		 XmNheight, 22,
		 XmNspacing, 0,
		 XmNhighlightThickness, 1,
		 XmNborderWidth, 0,
		 NULL);
	  
   F(ObjTmp->bnp_bar[i]->bnp_tbutton[j]).Add_Callback
    (ObjTmp->bnp_bar[i]->bnp_tbutton[j],
     XmNvalueChangedCallback, toggle_button, ObjTmp);
  }
  for(j = 0; j < ObjTmp->bnp_bar[i]->nb_bnp; j++)
  {
   F(ObjTmp->bnp_bar[i]->bnp_grid).addColumn
    (ObjTmp->bnp_bar[i]->bnp_grid, XcG_MAX_SIZE,
     WIDGET(ObjTmp->bnp_bar[i]->bnp_tbutton[j]), XcG_MAX_SIZE,
     NULL);
  }
      
  ObjTmp->bnp_bar[i]->bnp_sel = 0;
  F(ObjTmp->bnp_bar[i]->bnp_tbutton[0]).Set
   (ObjTmp->bnp_bar[i]->bnp_tbutton[0], TRUE, FALSE);
  for(j = 0; j < ObjTmp->bnp_bar[i]->nb_bnp; j++)
   XtManageChild(WIDGET(ObjTmp->bnp_bar[i]->bnp_tbutton[j]));
 }
  
 ObjTmp->fSetup = NEW(c_Frame)(WIDGET(ObjTmp->Grid),
			       MANAGER(ObjTmp->PBox->Dialog), 
			       "fBNSetup", -1);
  
 
 ObjTmp->fPreview = NEW(c_Frame)(WIDGET(ObjTmp->Grid),
				 MANAGER(ObjTmp->PBox->Dialog), 
				 "fBNPreview", -1);
  
 ObjTmp->tfPreview = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->fPreview),
				    MANAGER(ObjTmp->PBox->Dialog),
				    "tfBNPreview",
				    TF_STRING,
				    0.0, 300.0,
				    "test", NULL, 7);

 XtVaSetValues(WIDGET(ObjTmp->tfPreview),
	       XmNeditable, False,
	       /*XmNcursorPositionVisible, False,*/
	       XmNtraversalOn, False,
	       XmNselectionArrayCount, 0,
	       NULL);
     
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->fOnglet), 24,
			   WIDGET(ObjTmp->fSetup), XcG_MAX_SIZE,
			   WIDGET(ObjTmp->fPreview), 70,
			   NULL);
    
 F(ObjTmp->PBox->ItemList).set(ObjTmp->PBox->ItemList, XcIL_BROWSE_MODE,
			       FALSE, -1L, -1, (void *)-1, 
			       (void *)format_filter);
 F(ObjTmp->PBox->ItemList).addItems(ObjTmp->PBox->ItemList, FALSE,
				    XcIL_LINKED_LIST, -1,
				    BaseStd->format_base,
				    offsetof(c_FormatNbr, next));
  
 F(ObjTmp->PBox->Dialog).createActionArea(ObjTmp->PBox->Dialog,
					  XcCD_APPLY | XcCD_OK | 
					  XcCD_CANCEL, XcCD_APPLY,
					  TRUE, (void (*)())cb_action, ObjTmp);
  
 if ((*format)->hide_flag)
 {
  *format = *(ObjTmp->format_base);
  while(*format && (*format)->hide_flag)
   *format = (*format)->next;
 }
 ObjTmp->selected_type = (*format)->type;
 ObjTmp->edited_format = *format;
 cb_bnp_bar(ObjTmp, convert_fmt_type[(*format)->type]);
  
 F(ObjTmp->PBox).Map(ObjTmp->PBox, TRUE, *format);
 ObjTmp->retptr = format;

 Xc_TRACE(("Object Box_numbers build"));
 return ObjTmp;
}

static void dest_Box_nombres(This)
c_Box_nombres *This;
{
 int	i, j;
  
 while (This->v_format_base != NULL)
  DELETE(c_FormatNbr)(This->v_format_base);
  
 for(i = 0; i < NB_BNP_BAR; i++)
 {
  for( j = 0; j < This->bnp_bar[i]->nb_bnp; j++)
  {
   if (This->bnp_bar[i]->bnp[j]->created)
    BNP_DESTROY(This->bnp_bar[i]->bnp[j])
     (This, This->bnp_bar[i]->bnp[j]);
   DELETE(c_ToggleButton)(This->bnp_bar[i]->bnp_tbutton[j]);
  }
  DELETE(c_RowColumn)(This->bnp_bar[i]->bnp_grid);
 }
#ifndef LESSTIF_VERSION
 DELETE(c_RowColumn)(This->rcOnglet);
#endif
 DELETE(c_Frame)(This->fOnglet);
 DELETE(c_WTextField)(This->tfPreview);
 DELETE(c_Frame)(This->fPreview);

 DELETE(c_Frame)(This->fSetup);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_ItemMenu)(This->imBNFormatType);
 DELETE(c_ASLBox)(This->PBox);
 DELETE(c_ModifList)(This->list_save);
  
 DELETE(c_FormatNbr)(This->preview_format);
  
 if (This->d_format_base)
  Xc_free(This->d_format_base);
 Xc_free(This->bn_format);
  
 free_dt_seq_base(This);
 Xc_free(This);
 Xc_TRACE(("Object Box_numbers destroyed"));
}

static void *copy_Box_nombres(This)
c_Box_nombres *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}

static void refresh_preview(This)
c_Box_nombres *This;
{
 char	value[Xq_FORMULA_LENGTH];
 conv_info_t	conv_info;
  
 if (This->preview_string)
 {
  strcpy(value, This->preview_string);
  Xc_TRACE(("refresh preview: user: %s", value));
 }
 else
 {
  strcpy(value, This->bnp_bar[This->selected_bnp_bar]->preview_string);
  Xc_TRACE(("refresh preview: const: %s", value));
 }
  
 copy_saved_format(This, This->preview_format, This->edited_format);
 F(This->preview_format).convert_string(This->preview_format, value,
					(This->preview_format)->type, 
					&conv_info);
 Xc_TRACE(("format: %s string after convertion: %s",
	   (This->preview_format)->name, value));
 This->modify_flag = TRUE;
 F(This->tfPreview).set(This->tfPreview, value);
 This->modify_flag = FALSE;
 if ( (conv_info.color &&
       This->preview_color != conv_info.color->cell.pixel) ||
     (!conv_info.color && This->preview_color != GlobColormap->Cblack))
 {
  if (conv_info.color)
   This->preview_color = conv_info.color->cell.pixel;
  else
   This->preview_color = GlobColormap->Cblack;
  XtVaSetValues(WIDGET(This->tfPreview),
		XmNforeground, This->preview_color,
		NULL);
 }
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
  
 name = F(This->PBox).getNewItemName(This->PBox, (void **)&format); 
 if (!name) return;
  
 tmp_format = NEW(c_FormatNbr)(&(This->v_format_base), name);
 copy_saved_format(This, tmp_format, format);
  
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
 int		num, i, j, nb_type, type;
 c_FormatNbr	*item, *mformat;
 c_FormatNbr	*tother_format[Xq_NB_TYPE];
  
 if (F(This->PBox->ItemList).getSelection(This->PBox->ItemList,
					  (void ***)&format, &num))
 {
  item = F(This->PBox->ItemList).
   getFirstUnselectItem(This->PBox->ItemList);
  for(i = 0; i < num; i++)
  {
   if (XcH_USAGE(format[i]) == 0 && format[i]->lock_flag == FALSE &&
       This->PBox->ItemList->item_count > 1)
   {
    type = format[i]->type;
    nb_type = 0;
    mformat = *(This->format_base);
    while(mformat)
    {
     if (mformat->type == type)
      nb_type++;
     mformat = mformat->next;
    }
    mformat = This->v_format_base;
    while(mformat)
    {
     if (mformat->type == type)
      nb_type++;
     mformat = mformat->next;
    }
    for(j = 0; j < This->d_nb_format; j++)
    {
     if (This->d_format_base[j]->type == type)
      nb_type --;
    }

    if (nb_type < 2)
     continue;
	      
    F(This->PBox->ItemList).
     deleteItems(This->PBox->ItemList,
		 XcIL_POINTER_ARRAY, 1, (void **)&(format[i]));  
    F(This->list_save).sub(This->list_save, format[i]);
	      
    This->d_nb_format++;
    This->d_format_base = 
     (c_FormatNbr **)Xc_realloc("delt base", This->d_format_base,
				This->d_nb_format *
				sizeof(c_FormatNbr *));
    This->d_format_base[This->d_nb_format-1] = format[i];


    mformat = *(This->format_base);
    while(mformat)
    {
     if (F(This->list_save).get
	 (This->list_save, mformat, 19, tother_format) == -1)
      memcpy(tother_format, mformat->other_format,
	     sizeof(c_FormatNbr *) * Xq_NB_TYPE);
     if (mformat->type != type && 
	 tother_format[type] == format[i])
     {
      tother_format[type] = This->generic_format;
		  
      F(This->list_save).set
       (This->list_save, mformat, 19, tother_format);
     }
     mformat = mformat->next;
    }
    mformat = This->v_format_base;
    while(mformat)
    {
     if (F(This->list_save).get
	 (This->list_save, mformat, 19, tother_format) == -1)
      memcpy(tother_format, mformat->other_format,
	     sizeof(c_FormatNbr *) * Xq_NB_TYPE);
     if (mformat->type != type && 
	 tother_format[type] == format[i])
     {
      tother_format[type] = This->generic_format;
		    
      F(This->list_save).set
       (This->list_save, mformat, 19, tother_format);
     }
     mformat = mformat->next;
    }
   }
  }
  /*      if (bnp_links.created)
	  {
	  BNP_DESTROY(&bnp_links)(This, &bnp_links);
	  if (This->bnp_std_sel == &bnp_links)
	  {
	  BNP_CREATE(This->bnp_std_sel)(This, This->bnp_std_sel);
	  BNP_MANAGE(This->bnp_std_sel)(This, This->bnp_std_sel);
	  }
	  } */
      
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
static void Change_Sel(this, box, format)
c_ItemList	*this;
c_Box_nombres	*box;
c_FormatNbr	*format;
{
  
 box->edited_format = format;
 if (box->selected_fmt == BN_FMT_ALL && 
     box->selected_type != format->type)
 {
  Xc_TRACE(("format name: %s", format->name));
  Xc_TRACE(("formatype : %d", convert_fmt_type[format->type]));
  box->selected_type = format->type;
  cb_bnp_bar(box, convert_fmt_type[format->type]);    
 }
 else
 {
  BNP_REFRESH(box->bnp_std_sel)(box, box->bnp_std_sel);
 }
 F(box).refresh_preview(box);
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
   copy_saved_format(box, format, format);
	  
   format2 = *box->format_base;
   while(format2 && !F(format).compare(format, format2, CMP_WITH_NAME))
    format2 = format2->next;
   if (format2)
    format = format2;
   else
    format = NULL;
  }
  *(box->retptr) = format;
  break;
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
 c_FormatNbr	*mformat;
 int		i;
  
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
   box->modif.coma_sep = ((char *)data)[0];
   break;
  case 4:
   box->modif.color1 = ((c_Color **)data)[0];
   break;
  case 5:
   box->modif.color2 = ((c_Color **)data)[0];
   break;
  case 6:
   box->modif.char_sep = ((char *)data)[0];
   break;
  case 7:
   box->modif.truncate = ((boolean *)data)[0];
   break;
  case 8:
   memcpy(box->modif.unit, (char *)data, XcMAX_NAME_LENGTH);
   break;
  case 9:
   box->modif.unitafter = ((boolean *)data)[0];
   break;
  case 10:
   box->modif.exp_digit = ((unsigned int *)data)[0];
   break;
  case 11:
   box->modif.exp_multiple = ((unsigned int *)data)[0];
   break;
  case 12:
   box->modif.exp_sign = ((boolean *)data)[0];
   break;
  case 13:
   memcpy(box->modif.exp_string, (char *)data, XcMAX_NAME_LENGTH);
   break;
  case 14:
   box->modif.frac_prec = ((unsigned int *)data)[0];
   break;
  case 15:
   box->modif.frac_dec = ((boolean *)data)[0];
   break;
  case 16:
   memcpy(box->modif.true_name, (char *)data, XcMAX_NAME_LENGTH);
   break;
  case 17:
   memcpy(box->modif.false_name, (char *)data, XcMAX_NAME_LENGTH);
   break;
  case 18:
   box->modif.day_name_size = ((name_size_id_t *)data)[0];
   break;
  case 19:
   box->modif.month_name_size = ((name_size_id_t *)data)[0];
   break;
  case 20:
   memcpy(box->modif.date_seq, (dt_seq_t *)data, 
	  sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1));
   break;
  case 21:
   memcpy(box->modif.time_seq, (dt_seq_t **)data, 
	  sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1));
   break;
  case 22:
   memcpy(box->modif.date_time_seq, (dt_seq_t **)data, 
	  sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1));
   break;
  case 23:
   memcpy(box->modif.other_format, (c_FormatNbr **)data,
	  sizeof(c_FormatNbr *) * Xq_NB_TYPE);
   break;
  case 24:
   box->modif.type = ((int *)data)[0];
   break;
  case 25:
   F(format).set(format,
		 XcF_TYPE, box->modif.type,
		 XcF_MAXPREC, box->modif.max_prec,
		 XcF_MINPREC, box->modif.min_prec,
		 XcF_SPACE, box->modif.space_each,
		 XcF_UNIT_NAME, box->modif.unit,
		 XcF_COMMA, box->modif.coma_sep,
		 XcF_COLORP, box->modif.color1,
		 XcF_COLORN, box->modif.color2,
		 XcF_UNITAFTER, box->modif.unitafter,
		 XcF_CHARSEP, box->modif.char_sep,
		 XcF_TRUNCATE, box->modif.truncate,
		 XcF_EXPDIGIT, box->modif.exp_digit,
		 XcF_EXPMULTIPLE, box->modif.exp_multiple,
		 XcF_EXPSTRING, box->modif.exp_string,
		 XcF_EXPSIGN, box->modif.exp_sign,
		 XcF_FRAC_PREC, box->modif.frac_prec,
		 XcF_FRAC_DEC, box->modif.frac_dec,
		 XcF_TRUE_NAME, box->modif.true_name,
		 XcF_FALSE_NAME, box->modif.false_name,
		 XcF_DAY_NAME_SIZE, box->modif.day_name_size,
		 XcF_MONTH_NAME_SIZE, box->modif.month_name_size,
		 XcF_SET_DATE_SEQS, box->modif.date_seq,
		 XcF_SET_TIME_SEQS, box->modif.time_seq,
		 XcF_SET_DATE_TIME_SEQS, box->modif.date_time_seq,
		 XcF_ALL_OTHER_TYPES, box->modif.other_format,
		 XcF_END);
	  
   Xc_TRACE(("setez: %s",  box->modif.unit));
   break;
  }
  break;
 case TOADD:
  Xc_TRACE(("TOADD!"));
  box->modif.format = (c_FormatNbr *)NEW(c_FormatNbr)(box->format_base, 
						      format->name);
      
  mformat = *box->format_base;
  while(mformat)
  {
   for(i = 0; i < Xq_NB_TYPE; i++)
   {
    if (mformat->other_format[i] == format)
     mformat->other_format[i] = box->modif.format;
   }
   mformat = mformat->next;
  }
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
  
 if (filter_type == Xq_ALL)
  return !this->hide_flag;
 else
  return (!this->hide_flag) && (this->type == filter_type);
}

/* ----------------------------------------------------------------- ** 
** callback format type                                              ** 
** ----------------------------------------------------------------- */
static void cb_format_type(w, box, bn_format)
Widget w;
c_Box_nombres	*box;
bn_format_t	*bn_format;
{
 int		ofilter;
 c_FormatNbr	*selected_format;
 int		new_bar;
  
 Xc_TRACE(("Format type selected : %d", bn_format->num));
 ofilter = filter_type;
 box->selected_fmt = bn_format->num;
  
 switch(bn_format->num)
 {
 case BN_FMT_ALL:
  filter_type = Xq_ALL;
  break;
 case BN_FMT_NORMAL:
  filter_type = Xq_VALUE;
  break;
 case BN_FMT_UNIT:
  filter_type = Xq_UNIT;
  break;
 case BN_FMT_EXPONANTIAL:
  filter_type = Xq_EXPONANTIAL;
  break;
 case BN_FMT_PERCENT:
  filter_type = Xq_PERCENT;
  break;
 case BN_FMT_FRAC:
  filter_type = Xq_FRACTION;
  break;
 case BN_FMT_BOOLEAN:
  filter_type = Xq_BOOLEAN;
  break;
 case BN_FMT_DAY:
  filter_type = Xq_DAY_IN_WEEK;
  break;
 case BN_FMT_MONTH:
  filter_type = Xq_MONTH;
  break;
 case BN_FMT_TIME:
  filter_type = Xq_TIME;
  break;
 case BN_FMT_DATE:
  filter_type = Xq_DATE;
  break;
 case BN_FMT_DATE_TIME:
  filter_type = Xq_DATE_TIME;
  break;
 }
  
 if (extended_select[ofilter] != extended_select[filter_type])
 {
  XtVaSetValues(LIST(box->PBox->ItemList),
		XmNselectionPolicy, 
		extended_select[filter_type] ? 
		XmEXTENDED_SELECT : XmBROWSE_SELECT,
		NULL);
 }
  
 F(box->PBox->ItemList).deleteItems(box->PBox->ItemList,
				    XcIL_LINKED_LIST, -1,
				    *box->format_base,
				    offsetof(c_FormatNbr, next));

 F(box->PBox->ItemList).deleteItems(box->PBox->ItemList,
				    XcIL_LINKED_LIST, -1,
				    box->v_format_base,
				    offsetof(c_FormatNbr, next));
  
 F(box->PBox->ItemList).addItems(box->PBox->ItemList, FALSE,
				 XcIL_LINKED_LIST, -1,
				 *box->format_base,
				 offsetof(c_FormatNbr, next));
  
 F(box->PBox->ItemList).addItems(box->PBox->ItemList, FALSE,
				 XcIL_LINKED_LIST, -1,
				 box->v_format_base,
				 offsetof(c_FormatNbr, next));
  
 F(box->PBox->ItemList).deleteItems(box->PBox->ItemList,
				    XcIL_POINTER_ARRAY, box->d_nb_format,
				    box->d_format_base);
  
 F(box->PBox).selectItem(box->PBox, *box->PBox->ItemList->item_base, TRUE); 
 F(box->PBox->ItemList).selectItem(box->PBox->ItemList, 
				   *box->PBox->ItemList->item_base);
 box->edited_format = selected_format = 
  (c_FormatNbr *)(*box->PBox->ItemList->item_base);
 box->PBox->edited_item = (*box->PBox->ItemList->item_base);
  
 if (bn_format->num == BN_FMT_ALL)
 {
  box->selected_type = selected_format->type;
  new_bar = convert_fmt_type[selected_format->type];
 }
 else
 {
  box->selected_type = filter_type;
  new_bar = bn_format->num;
 }
 cb_bnp_bar(box, new_bar);
 F(box).refresh_preview(box);
}

static void cb_bnp_bar(box, bar)
c_Box_nombres *box;
int bar;
{
 Xc_HISTORY(("selected new bar: %d", bar));
  
 if (box->selected_bnp_bar != -1)
 {
  XtUnmanageChild(WIDGET(box->bnp_bar[box->selected_bnp_bar]->bnp_grid));
 }

 box->selected_bnp_bar = bar;
  
 box->selected_bnp_std = -1;
 toggle_button(WIDGET(box->bnp_bar[box->selected_bnp_bar]->bnp_tbutton
		      [box->bnp_bar[box->selected_bnp_bar]->bnp_sel]), box);

 XtManageChild(WIDGET(box->bnp_bar[box->selected_bnp_bar]->bnp_grid));
 F(box->bnp_bar[box->selected_bnp_bar]->bnp_grid).callResize
  (box->bnp_bar[box->selected_bnp_bar]->bnp_grid);
}

static void toggle_button(w, box)
Widget w;
c_Box_nombres *box;
{
 int	i, j;
  
 i = box->selected_bnp_bar;
 j = 0;
 while(j < box->bnp_bar[i]->nb_bnp && 
       w != WIDGET(box->bnp_bar[i]->bnp_tbutton[j]))
  j++;
  
 if (j == box->bnp_bar[i]->nb_bnp)
 {
  Xc_ERROR(("Toggle Button not found"));
  return;
 }
  
 if (box->selected_bnp_std == j)
 {
  F(box->bnp_bar[i]->bnp_tbutton[j]).Set(box->bnp_bar[i]->bnp_tbutton[j],
					 True, False);
  return;
 }
  
 if (box->bnp_std_sel)
  BNP_MANAGE(box->bnp_std_sel)(box, box->bnp_std_sel);
  
 if (box->selected_bnp_std != -1)
 {
  F(box->bnp_bar[i]->bnp_tbutton[box->selected_bnp_std]).Set
   (box->bnp_bar[i]->bnp_tbutton[box->selected_bnp_std], False, False);
 }
 box->bnp_bar[i]->bnp_sel = j;
 box->bnp_std_sel = box->bnp_bar[i]->bnp[j];
 box->selected_bnp_std = j;

 Xc_TRACE(("select tbutton: %d bar: %d", j, i));

 if (!box->bnp_std_sel->created)
 {
  BNP_CREATE(box->bnp_std_sel)(box, box->bnp_std_sel);
  BNP_MANAGE(box->bnp_std_sel)(box, box->bnp_std_sel);
 }
 else
 {
  BNP_MANAGE(box->bnp_std_sel)(box, box->bnp_std_sel);
 }
 F(box->bnp_bar[box->selected_bnp_bar]->bnp_grid).callResize
  (box->bnp_bar[box->selected_bnp_bar]->bnp_grid);
}

static void copy_saved_format(box, dest, src)
c_Box_nombres	*box;
c_FormatNbr	*dest;
c_FormatNbr	*src;
{
 c_Color	*tposcolor, *tnegcolor;
 char		tunit[XcMAX_NAME_LENGTH];
 char		texpstring[XcMAX_NAME_LENGTH];
 char		ttrue[XcMAX_NAME_LENGTH];
 char		tfalse[XcMAX_NAME_LENGTH];
 dt_seq_t	*tdate_seq[XqMAX_DT_SEQ+1];
 dt_seq_t	*ttime_seq[XqMAX_DT_SEQ+1];
 dt_seq_t	*tdate_time_seq[XqMAX_DT_SEQ+1];
 name_size_id_t	tday_name_size, tmonth_name_size;
 int		ttype;
 unsigned int	tmaxprec, tminprec, tspace;
 unsigned int	texpdigit, texpmultiple, tfrac_prec;
 char		tcomasep, tcharsep;
 c_FormatNbr	*tother_format[Xq_NB_TYPE];
 boolean	tunitafter, truncate, texpsign, tfrac_dec;

 if (F(box->list_save).get(box->list_save,
			   src, 0, &tmaxprec) == -1)
  tmaxprec = src->max_prec;
 if (F(box->list_save).get(box->list_save,
			   src, 1, &tminprec) == -1)
  tminprec = src->min_prec;
 if (F(box->list_save).get(box->list_save,
			   src, 2, &tspace) == -1)
  tspace = src->space_each;
 if (F(box->list_save).get(box->list_save,
			   src, 3, &tcomasep) == -1)
  tcomasep = src->comma_sep;
 if (F(box->list_save).get(box->list_save,
			   src, 4, &tposcolor) == -1)
  tposcolor = src->color1;
 if (F(box->list_save).get(box->list_save,
			   src, 5, &tnegcolor) == -1)
  tnegcolor = src->color2;
 if (F(box->list_save).get(box->list_save,
			   src, 6, &tcharsep) == -1)
  tcharsep = src->char_sep;
 if (F(box->list_save).get(box->list_save,
			   src, 7, &truncate) == -1)
  truncate = src->truncate;

 if (F(box->list_save).get(box->list_save,
			   src, 8, tunit) == -1)
  strcpy(tunit, src->unit);
 Xc_TRACE(("Unit to set: %s", tunit));
 
 if (F(box->list_save).get(box->list_save,
			   src, 9, &tunitafter) == -1)
  tunitafter = src->unitafter;
 Xc_TRACE(("Unit pos: %s", tunitafter?"AFTER":"BEFORE"));
 
 if (F(box->list_save).get(box->list_save,
			   src, 10, &texpdigit) == -1)
  texpdigit = src->exp_digit;
 
 if (F(box->list_save).get(box->list_save,
			   src, 11, &texpmultiple) == -1)
  texpmultiple = src->exp_multiple;
 
 if (F(box->list_save).get(box->list_save,
			   src, 12, &texpsign) == -1)
  texpsign = src->exp_sign;
 
 if (F(box->list_save).get(box->list_save,
			   src, 13, texpstring) == -1)
  strcpy(texpstring, src->exp_string);
 
 if (F(box->list_save).get(box->list_save,
			   src, 14, &tfrac_prec) == -1)
  tfrac_prec = src->frac_prec;
 if (F(box->list_save).get(box->list_save,
			   src, 15, &tfrac_dec) == -1)
  tfrac_dec = src->frac_dec;
 
 if (F(box->list_save).get(box->list_save,
			   src, 16, ttrue) == -1)
  strcpy(ttrue, src->true_name);
 if (F(box->list_save).get(box->list_save,
			   src, 17, tfalse) == -1)
  strcpy(tfalse, src->false_name);
 
 if (F(box->list_save).get(box->list_save,
			   src, 18, &tday_name_size) == -1)
  tday_name_size = src->day_name_size;
 if (F(box->list_save).get(box->list_save,
			   src, 19, &tmonth_name_size) == -1)
  tmonth_name_size = src->month_name_size;
 
 if (F(box->list_save).get(box->list_save,
			   src, 20, tdate_seq) == -1)
  memcpy(tdate_seq, src->date_seq, sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1));
 if (F(box->list_save).get(box->list_save,
			   src, 21, ttime_seq) == -1)
  memcpy(ttime_seq, src->time_seq, sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1));
 if (F(box->list_save).get(box->list_save,
			   src, 22, tdate_time_seq) == -1)
  memcpy(tdate_time_seq, src->date_time_seq,
	 sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1));
 
 if (F(box->list_save).get(box->list_save,
			   src, 23, tother_format) == -1)
  memcpy(tother_format, src->other_format,
	 sizeof(c_FormatNbr *) * Xq_NB_TYPE);

 if (F(box->list_save).get(box->list_save,
			   src, 24, &ttype) == -1)
  ttype = src->type;
 
 F(dest).set(dest,
	     XcF_TYPE, ttype,
	     XcF_MAXPREC, tmaxprec,
	     XcF_MINPREC, tminprec,
	     XcF_SPACE, tspace,
	     XcF_COMMA, tcomasep,
	     XcF_COLORP, tposcolor,
	     XcF_COLORN, tnegcolor,
	     XcF_UNIT_NAME, tunit,
	     XcF_UNITAFTER, tunitafter,
	     XcF_CHARSEP, tcharsep,
	     XcF_TRUNCATE, truncate,
	     XcF_EXPDIGIT, texpdigit,
	     XcF_EXPMULTIPLE, texpmultiple,
	     XcF_EXPSTRING, texpstring,
	     XcF_EXPSIGN, texpsign,
	     XcF_FRAC_PREC, tfrac_prec,
	     XcF_FRAC_DEC, tfrac_dec,
	     XcF_TRUE_NAME, ttrue,
	     XcF_FALSE_NAME, tfalse,
	     XcF_DAY_NAME_SIZE, tday_name_size,
	     XcF_MONTH_NAME_SIZE, tmonth_name_size,
	     XcF_SET_DATE_SEQS, tdate_seq,
	     XcF_SET_TIME_SEQS, ttime_seq,
	     XcF_SET_DATE_TIME_SEQS, tdate_time_seq,
	     XcF_ALL_OTHER_TYPES, tother_format,
	     XcF_END);
}
