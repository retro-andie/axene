/*
** BNP_Date.c for XQuad in Box_nombres/
** Box Nombres Part that manage date to other format
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
** Started on  Wed Jan  3 22:16:04 1996 Emmanuel Paris
** Last update Mon May 29 20:34:08 2000 Emmanuel Paris
*/

#include "BNP_Date.h"
#include "TimeFuncs.h"

static void CreateBNP_Date ___PROTO((c_Box_nombres *This, bnp_date_t *bnp));
static void ManageBNP_Date ___PROTO((c_Box_nombres *This, bnp_date_t *bnp));
static void RefreshBNP_Date ___PROTO((c_Box_nombres *This, bnp_date_t *bnp));
static void DestroyBNP_Date ___PROTO((c_Box_nombres *This, bnp_date_t *bnp));
static void refresh_padding_field ___PROTO((c_Box_nombres *This,
					    bnp_date_t *bnp));
static void refresh_name_size_field ___PROTO((c_Box_nombres *This,
					      bnp_date_t *bnp));

static void change_to_no_pad();
static void change_to_pad_with_zero();
static void change_to_pad_with_space();

static void change_to_ns_very_short();
static void change_to_ns_short();
static void change_to_ns_long();
static void pb_callbacks();

bnp_std_t bnp_dates = 
{
 FALSE, FALSE,
 CreateBNP_Date,
 ManageBNP_Date,
 RefreshBNP_Date,
 DestroyBNP_Date,
 NULL
};

static int seq_id_date_conv[15] = 
{
 Xq_DT_YEAR, Xq_DT_YEAR_SHORT, Xq_DT_YEAR_JC, Xq_DT_MONTH,
 Xq_DT_MONTH_NAME, Xq_DT_WEEK, Xq_DT_DAY, Xq_DT_DAY_NAME,
 Xq_DT_DAY_IN_YEAR, Xq_DT_SEPARATOR, Xq_DT_SEPARATOR,
 Xq_DT_SEPARATOR, Xq_DT_SEPARATOR, Xq_DT_SEPARATOR,
 Xq_DT_SEPARATOR
};

static boolean seq_id_date_padding[15] =
{
 TRUE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE,
 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
};

static boolean seq_id_date_name_size[15] = 
{
 FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE,
 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
};

static boolean seq_id_date_separator[15] =
{
 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
 TRUE, TRUE, TRUE, TRUE, TRUE, TRUE
};

static char date_string_year[11], date_string_year_short[11];
static char date_string_year_jc[31], date_string_month_name[31];
static char date_string_day_name[31];

static char *date_string_filler[15] = 
{
 date_string_year, date_string_year_short, date_string_year_jc,
 "00", date_string_month_name, "00", "00", date_string_day_name, "000",
 "/", "-", ":", ".", " ", ","
};

static c_PushButton *pb_seq_date[15];

/* -------------------------------------------------------------------- ** 
** CreateBNP_Date - BNP Date Creator					** 
** -------------------------------------------------------------------- */
static void CreateBNP_Date(This, bnp)
c_Box_nombres *This;
bnp_date_t *bnp;
{
 int	i;
 double ddate;
 xc_date_time_t	xc_date_time;
 xc_date_t		xc_date, *mdate;
  
 Xc_HISTORY(("Create BNP Date"));
 bnp->data = 
  (data_date_t *)Xc_malloc("data", sizeof(data_date_t));
 memset(bnp->data, 0, sizeof(data_date_t)); 

 bnp->data->padding = Xq_PADDING_NONE;
 bnp->data->name_size = Xq_NAME_LONG;
 for(i = 0; i < 15; i++)
 {
  if (seq_id_date_padding[i])
   bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_date_conv[i]]
    .new(seq_id_date_conv[i], bnp->data->padding);
  else
  {
   if (seq_id_date_name_size[i])
    bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_date_conv[i]]
     .new(seq_id_date_conv[i], bnp->data->name_size);
   else
    if (seq_id_date_separator[i])
     bnp->data->dt_seq_pb[i] = 
      dt_seq_funcs[seq_id_date_conv[i]].new(seq_id_date_conv[i],
					    date_string_filler[i]);
    else
     bnp->data->dt_seq_pb[i] = 
      dt_seq_funcs[seq_id_date_conv[i]].new(seq_id_date_conv[i]);
  }
 }
  
 if (This->preview_string)
 {
  ddate = atof(This->preview_string);
  Xc_TRACE(("refresh preview: user: %g", ddate));
 }
 else
 {
  ddate = atof(This->bnp_bar[This->selected_bnp_bar]->preview_string);
  Xc_TRACE(("refresh preview: const: %g", ddate));
 }
  
 if (This->selected_bnp_bar == BN_FMT_DATE_TIME)
  mdate = &(convert_to_date_and_time(ddate, &xc_date_time)->date);
 else
  mdate = convert_to_date(ddate, &xc_date);
  
 bnp->data->Grid = 
  (c_Grid *)NEW(c_Grid)(WIDGET(This->fSetup), NULL, "gBNDate", TRUE);
  
  
 bnp->data->fPadding = (c_Frame *)NEW(c_Frame)(WIDGET(bnp->data->Grid),
					       NULL,
					       "fPadding", 0);
  
 bnp->data->rcPadding = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->fPadding), NULL, "rcPadding", 0, 0, XmVERTICAL);
  
 XtVaSetValues(WIDGET(bnp->data->rcPadding), 
	       XmNradioBehavior, True,
	       XmNradioAlwaysOne, True, 
	       NULL);
  
 bnp->data->tbNoPad = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(bnp->data->rcPadding),
					NULL, "tbNoPad");
 XtVaSetValues(WIDGET(bnp->data->tbNoPad), XmNset, True, NULL);
 F(bnp->data->tbNoPad).Add_Callback(bnp->data->tbNoPad, 
				    XmNvalueChangedCallback,
				    (void *)change_to_no_pad, This);
 bnp->data->tbPadWith0 = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(bnp->data->rcPadding),
					NULL, "tbPadWith0");

 F(bnp->data->tbPadWith0).Add_Callback(bnp->data->tbPadWith0, 
				       XmNvalueChangedCallback,
				       (void *)change_to_pad_with_zero, This);
  
 bnp->data->tbPadWithSpace = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(bnp->data->rcPadding),
					NULL, "tbPadWithSpace");

 F(bnp->data->tbPadWithSpace).Add_Callback(bnp->data->tbPadWithSpace, 
					   XmNvalueChangedCallback,
					   (void *)change_to_pad_with_space,
					   This);
  
 bnp->data->fNameSize = (c_Frame *)NEW(c_Frame)(WIDGET(bnp->data->Grid),
						NULL,
						"fNameSize", 0);
  
 bnp->data->rcNameSize = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->fNameSize), NULL, "rcNameSize", 0, 0, XmVERTICAL);
  
 XtVaSetValues(WIDGET(bnp->data->rcNameSize), XmNradioBehavior, True,
	       XmNradioAlwaysOne, True, NULL);
  
 bnp->data->tbNSVeryShort = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(bnp->data->rcNameSize),
					NULL, "tbNSVeryShort");

 F(bnp->data->tbNSVeryShort).Add_Callback(bnp->data->tbNSVeryShort, 
					  XmNvalueChangedCallback,
					  (void *)change_to_ns_very_short,
					  This);
 bnp->data->tbNSShort = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(bnp->data->rcNameSize),
					NULL, "tbNSShort");

 F(bnp->data->tbNSShort).Add_Callback(bnp->data->tbNSShort, 
				      XmNvalueChangedCallback,
				      (void *)change_to_ns_short, This);
  
 bnp->data->tbNSLong = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(bnp->data->rcNameSize),
					NULL, "tbNSLong");
 XtVaSetValues(WIDGET(bnp->data->tbNSLong), XmNset, True, NULL);
 F(bnp->data->tbNSLong).Add_Callback(bnp->data->tbNSLong, 
				     XmNvalueChangedCallback,
				     (void *)change_to_ns_long, This);
  
 bnp->data->sSeparator1 = (c_Separator *)NEW(c_Separator)
  (WIDGET(bnp->data->Grid), NULL, XcSEPARATOR_H);
  
 bnp->data->rcYear = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcYear", 0, 0, XmHORIZONTAL);
  
 bnp->data->lYear = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
					    NULL, "lYear");
  
 pb_seq_date[0] = bnp->data->pbYear = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcYear),
				    NULL, "pbYear");  
 *date_string_filler[0] = '\0';
 dt_seq_funcs[seq_id_date_conv[0]].concate(bnp->data->dt_seq_pb[0],
					   date_string_filler[0], mdate);
  
 pb_seq_date[1] = bnp->data->pbYearShort = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcYear),
				    NULL, "pbYearShort");
 *date_string_filler[1] = '\0';
 dt_seq_funcs[seq_id_date_conv[1]].concate(bnp->data->dt_seq_pb[1],
					   date_string_filler[1], mdate);
  
 pb_seq_date[2] = bnp->data->pbYearJC = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcYear),
				    NULL, "pbYearJC");
 *date_string_filler[2] = '\0';
 dt_seq_funcs[seq_id_date_conv[2]].concate(bnp->data->dt_seq_pb[2],
					   date_string_filler[2], mdate);
  
 bnp->data->rcMonth = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcMonth", 0, 0, XmHORIZONTAL);
  
 bnp->data->lMonth = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
					     NULL, "lMonth");
  
 pb_seq_date[3] = bnp->data->pbMonth = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcMonth),
				    NULL, "pbMonth");
 pb_seq_date[4] = bnp->data->pbMonthName = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcMonth),
				    NULL, "pbMonthName");
 *date_string_filler[4] = '\0';
 dt_seq_funcs[seq_id_date_conv[4]].concate(bnp->data->dt_seq_pb[4],
					   date_string_filler[4], mdate);
  
 bnp->data->rcWeek = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcWeek", 0, 0, XmHORIZONTAL);
  
 bnp->data->lWeek = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
					    NULL, "lWeek");

 pb_seq_date[5] = bnp->data->pbWeek = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcWeek),
				    NULL, "pbWeek");
  
 bnp->data->rcDay = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcDay", 0, 0, XmHORIZONTAL);
  
 bnp->data->lDay = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
					   NULL, "lDay");
  
 pb_seq_date[6] = bnp->data->pbDay = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcDay),
				    NULL, "pbDay");
 pb_seq_date[7] = bnp->data->pbDayName = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcDay),
				    NULL, "pbDayName");
 *date_string_filler[7] = '\0';
 dt_seq_funcs[seq_id_date_conv[7]].concate(bnp->data->dt_seq_pb[7],
					   date_string_filler[7], mdate);
  
 bnp->data->lDayInYear = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
						 NULL, "lDayInYear");
  
 bnp->data->rcDayInYear = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcDayInYear", 0, 0, XmHORIZONTAL);
  
 pb_seq_date[8] = bnp->data->pbDayInYear = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcDayInYear),
				    NULL, "pbDayInYear");
  
 bnp->data->sSeparator2 = (c_Separator *)NEW(c_Separator)
  (WIDGET(bnp->data->Grid), NULL, XcSEPARATOR_H);

 bnp->data->lSep = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
					   NULL, "lSep");
 bnp->data->rcSeparators = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcSeparators", 0, 0, XmHORIZONTAL);

 XtVaSetValues(WIDGET(bnp->data->rcSeparators), 
	       XmNpacking, XmPACK_COLUMN,
	       NULL);
 
 pb_seq_date[9] = bnp->data->pbSep1 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, date_string_filler[9]);
 pb_seq_date[10] = bnp->data->pbSep2 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, date_string_filler[10]);
 pb_seq_date[11] = bnp->data->pbSep3 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, date_string_filler[11]);
 pb_seq_date[12] = bnp->data->pbSep4 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, date_string_filler[12]);
 pb_seq_date[13] = bnp->data->pbSep5 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, date_string_filler[13]);
 pb_seq_date[14] = bnp->data->pbSep6 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, date_string_filler[14]);

 F(bnp->data->Grid).addColumn(bnp->data->Grid, 100,
			      WIDGET(bnp->data->fPadding), XcG_MAX_SIZE,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->fNameSize), XcG_MAX_SIZE,
			      NULL);
  
 F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_MAX_SIZE,
			      WIDGET(bnp->data->sSeparator1), XcG_MAX_SIZE,
			      NULL);
  
 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lYear), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcYear), XcG_MAX_SIZE,
			      NULL);
  
 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lMonth), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcMonth), XcG_MAX_SIZE,
			      NULL);
  
 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lWeek), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcWeek), XcG_MAX_SIZE,
			      NULL);
  
 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lDay), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcDay), XcG_MAX_SIZE,
			      NULL);

 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lDayInYear), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcDayInYear), XcG_MAX_SIZE,
			      NULL);

 F(bnp->data->Grid).addColumn(bnp->data->Grid, XcG_MAX_SIZE,
			      WIDGET(bnp->data->sSeparator2), XcG_MAX_SIZE,
			      NULL);

 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lSep), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcSeparators), XcG_MAX_SIZE,
			      NULL);

 for(i = 0; i < 15; i++)
 {
  XmString      xms;
      
  xms = XmStringCreate(date_string_filler[i], XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(WIDGET(pb_seq_date[i]), XmNlabelString, xms, NULL);
  XmStringFree(xms);
  F(pb_seq_date[i]).Add_Callback(pb_seq_date[i], FUNC_ACTIVATE,
				 pb_callbacks, This);
  XtVaSetValues(WIDGET(pb_seq_date[i]), XmNrecomputeSize, False);
 }

 if (This->dim_calc)
 {
  XtVaSetValues(WIDGET(bnp->data->Grid), XmNwidth, This->width,
		XmNheight, This->height, NULL);
  F(bnp->data->Grid).callResize(bnp->data->Grid);
 }
    
 /*XtManageChild(WIDGET(bnp->data->lPadding));*/
 XtManageChild(WIDGET(bnp->data->tbNoPad));
 XtManageChild(WIDGET(bnp->data->tbPadWith0));
 XtManageChild(WIDGET(bnp->data->tbPadWithSpace));
 XtManageChild(WIDGET(bnp->data->rcPadding));
 XtManageChild(WIDGET(bnp->data->fPadding));
  
 /*XtManageChild(WIDGET(bnp->data->lNameSize));*/
 XtManageChild(WIDGET(bnp->data->tbNSVeryShort));
 XtManageChild(WIDGET(bnp->data->tbNSShort));
 XtManageChild(WIDGET(bnp->data->tbNSLong));
 XtManageChild(WIDGET(bnp->data->rcNameSize));
 XtManageChild(WIDGET(bnp->data->fNameSize));

 XtManageChild(WIDGET(bnp->data->sSeparator1));
  
 XtManageChild(WIDGET(bnp->data->lYear));
 XtManageChild(WIDGET(bnp->data->pbYear));
 XtManageChild(WIDGET(bnp->data->pbYearShort));
 XtManageChild(WIDGET(bnp->data->pbYearJC));
 XtManageChild(WIDGET(bnp->data->rcYear));
  
 XtManageChild(WIDGET(bnp->data->lMonth));
 XtManageChild(WIDGET(bnp->data->pbMonth));
 XtManageChild(WIDGET(bnp->data->pbMonthName));
 XtManageChild(WIDGET(bnp->data->rcMonth));
  
 XtManageChild(WIDGET(bnp->data->lWeek));
 XtManageChild(WIDGET(bnp->data->pbWeek));
 XtManageChild(WIDGET(bnp->data->rcWeek));
  
 XtManageChild(WIDGET(bnp->data->lDay));
 XtManageChild(WIDGET(bnp->data->pbDay));
 XtManageChild(WIDGET(bnp->data->pbDayName));
 XtManageChild(WIDGET(bnp->data->rcDay));

 XtManageChild(WIDGET(bnp->data->lDayInYear));
 XtManageChild(WIDGET(bnp->data->pbDayInYear));
 XtManageChild(WIDGET(bnp->data->rcDayInYear));

 XtManageChild(WIDGET(bnp->data->sSeparator2));

 XtManageChild(WIDGET(bnp->data->pbSep1));
 XtManageChild(WIDGET(bnp->data->pbSep2));
 XtManageChild(WIDGET(bnp->data->pbSep3));
 XtManageChild(WIDGET(bnp->data->pbSep4));
 XtManageChild(WIDGET(bnp->data->pbSep5));
 XtManageChild(WIDGET(bnp->data->pbSep6));
 XtManageChild(WIDGET(bnp->data->rcSeparators));
 XtManageChild(WIDGET(bnp->data->lSep));

 bnp->created = TRUE;
 bnp->managed = FALSE;
}

/* -------------------------------------------------------------------- ** 
** ManageBNP_Date - BNP Date Manager					** 
** -------------------------------------------------------------------- */
static void ManageBNP_Date(This, bnp)
c_Box_nombres *This;
bnp_date_t *bnp;
{
 int	i;
 Pixel	background;
  
 if (bnp->managed)
 {
  Xc_HISTORY(("Unmanage BNP Date"));
  if (!This->dim_calc)
  {
   XtVaGetValues(WIDGET(bnp->data->Grid), XmNwidth, &This->width,
		 XmNheight, &This->height, NULL);
   Xc_TRACE(("Grid size: %d x %d", This->width, This->height));
   This->dim_calc = TRUE;
  }
  XtUnmanageChild(WIDGET(bnp->data->Grid));
  XtVaSetValues(WIDGET(This->tfPreview),
		XmNbackground, bnp->data->background,
		XmNeditable, False,
		/*XmNcursorPositionVisible, False,*/
		XmNtraversalOn, False,
		XmNselectionArrayCount, 0,
		NULL);
  XtRemoveCallback(WIDGET(This->tfPreview), XmNmodifyVerifyCallback,
		   (void *)bn_tf_modify, This);
 }
 else
 {
  Xc_HISTORY(("Manage BNP Date"));
  XtManageChild(WIDGET(bnp->data->Grid)); 
  RefreshBNP_Date(This, bnp);
  for(i = 0; i < 15; i++)
   if (seq_id_date_padding[i] || seq_id_date_name_size[i])
    XtVaSetValues(WIDGET(pb_seq_date[i]), XmNrecomputeSize, False, NULL);
      
  XtVaGetValues(WIDGET(This->tfPreview), XmNbackground, 
		&(bnp->data->background), NULL);
      
  XtVaGetValues(WIDGET(This->PBox->text_field), XmNbackground, 
		&background, NULL);
      
  XtVaSetValues(WIDGET(This->tfPreview),
		XmNbackground, background,
		XmNeditable, True,
		XmNcursorPositionVisible, True,
		XmNtraversalOn, True,
		XmNselectionArrayCount, 3,
		XmNverifyBell, False,
		NULL); 
      
  XtAddCallback(WIDGET(This->tfPreview), XmNmodifyVerifyCallback,
		(void *)bn_tf_modify, This);
      
  XmTextFieldSetInsertionPosition
   (WIDGET(This->tfPreview), XmTextFieldGetLastPosition
    (WIDGET(This->tfPreview)));

  XmProcessTraversal(WIDGET(This->tfPreview), XmTRAVERSE_CURRENT);
 }
 bnp->managed ^= TRUE;
}

/* -------------------------------------------------------------------- ** 
** RefreshBNP_Date - BNP Date Refresh value				** 
** -------------------------------------------------------------------- */
static void RefreshBNP_Date(This, bnp)
c_Box_nombres *This;
bnp_date_t *bnp;
{
 int	i;
 double ddate;
 char	field[Xq_FORMULA_LENGTH];
 xc_date_time_t	xc_date_time;
 xc_date_t		xc_date, *mdate;
 XmString      xms;
  
 Xc_HISTORY(("Refresh BNP Date"));
  
 if (This->preview_string)
 {
  ddate = atof(This->preview_string);
  Xc_TRACE(("refresh preview: user: %g", ddate));
 }
 else
 {
  ddate = atof(This->bnp_bar[This->selected_bnp_bar]->preview_string);
  Xc_TRACE(("refresh preview: const: %g", ddate));
 }
  
 if (This->selected_bnp_bar == BN_FMT_DATE_TIME)
  mdate = &(convert_to_date_and_time(ddate, &xc_date_time)->date);
 else
  mdate = convert_to_date(ddate, &xc_date);
  
 for(i = 0; i < 15; i++)
 {
  field[0] = '\0';
  dt_seq_funcs[seq_id_date_conv[i]].concate(bnp->data->dt_seq_pb[i],
					    field, mdate);
      
  xms = XmStringCreate(field, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(WIDGET(pb_seq_date[i]), XmNlabelString, xms, NULL);
  XmStringFree(xms);
 }
}

/* -------------------------------------------------------------------- ** 
** DestroyBNP_Date - BNP Date Destructor				** 
** -------------------------------------------------------------------- */
static void DestroyBNP_Date(This, bnp)
c_Box_nombres *This;
bnp_date_t *bnp;
{
 int	i;
 Xc_HISTORY(("Destroy BNP Date"));
  
 for(i = 0; i < 15; i++)
  dt_seq_funcs[seq_id_date_conv[i]].delete(bnp->data->dt_seq_pb[i]);
  
 DELETE(c_PushButton)(bnp->data->pbSep1);
 DELETE(c_PushButton)(bnp->data->pbSep2);
 DELETE(c_PushButton)(bnp->data->pbSep3);
 DELETE(c_PushButton)(bnp->data->pbSep4);
 DELETE(c_PushButton)(bnp->data->pbSep5);
 DELETE(c_PushButton)(bnp->data->pbSep6);
 DELETE(c_RowColumn)(bnp->data->rcSeparators);
 DELETE(c_Label)(bnp->data->lSep);

 DELETE(c_Separator)(bnp->data->sSeparator2);
  
 DELETE(c_PushButton)(bnp->data->pbDayInYear);
 DELETE(c_Label)(bnp->data->lDayInYear);
 DELETE(c_RowColumn)(bnp->data->rcDayInYear);

 DELETE(c_PushButton)(bnp->data->pbDayName);
 DELETE(c_PushButton)(bnp->data->pbDay);
 DELETE(c_Label)(bnp->data->lDay);
 DELETE(c_RowColumn)(bnp->data->rcDay);
  
 DELETE(c_PushButton)(bnp->data->pbWeek);
 DELETE(c_Label)(bnp->data->lWeek);
 DELETE(c_RowColumn)(bnp->data->rcWeek);
  
 DELETE(c_PushButton)(bnp->data->pbMonthName);
 DELETE(c_PushButton)(bnp->data->pbMonth);
 DELETE(c_Label)(bnp->data->lMonth);
 DELETE(c_RowColumn)(bnp->data->rcMonth);
  
 DELETE(c_PushButton)(bnp->data->pbYearJC);
 DELETE(c_PushButton)(bnp->data->pbYearShort);
 DELETE(c_PushButton)(bnp->data->pbYear);
 DELETE(c_Label)(bnp->data->lYear);
 DELETE(c_RowColumn)(bnp->data->rcYear);
  
 DELETE(c_Separator)(bnp->data->sSeparator1);
  
 DELETE(c_ToggleButton)(bnp->data->tbNSLong);
 DELETE(c_ToggleButton)(bnp->data->tbNSShort);
 DELETE(c_ToggleButton)(bnp->data->tbNSVeryShort);
 /*DELETE(c_Label)(bnp->data->lNameSize);*/
 DELETE(c_RowColumn)(bnp->data->rcNameSize);
 DELETE(c_Frame)(bnp->data->fNameSize);
  
 DELETE(c_ToggleButton)(bnp->data->tbPadWithSpace);
 DELETE(c_ToggleButton)(bnp->data->tbPadWith0);
 DELETE(c_ToggleButton)(bnp->data->tbNoPad);
 /*DELETE(c_Label)(bnp->data->lPadding);*/
 DELETE(c_RowColumn)(bnp->data->rcPadding);
 DELETE(c_Frame)(bnp->data->fPadding);
  
 DELETE(c_Grid)(bnp->data->Grid);
  
 Xc_free(bnp->data);
 bnp->created = FALSE;
}

static void refresh_padding_field(This, bnp)
c_Box_nombres *This;
bnp_date_t *bnp;
{
 int	i;
 double ddate;
 char	field[Xq_FORMULA_LENGTH];
 xc_date_time_t	xc_date_time;
 xc_date_t		xc_date, *mdate;
 XmString      xms;

 Xc_HISTORY(("Refresh BNP Date"));
  
 if (This->preview_string)
 {
  ddate = atof(This->preview_string);
  Xc_TRACE(("refresh preview: user: %g", ddate));
 }
 else
 {
  ddate = atof(This->bnp_bar[This->selected_bnp_bar]->preview_string);
  Xc_TRACE(("refresh preview: const: %g", ddate));
 }
 if (This->selected_bnp_bar == BN_FMT_DATE_TIME)
  mdate = &(convert_to_date_and_time(ddate, &xc_date_time)->date);
 else
  mdate = convert_to_date(ddate, &xc_date);
  
 for(i = 0; i < 15; i++)
 {
  if (seq_id_date_padding[i])
  {
   field[0] = '\0';
   dt_seq_funcs[seq_id_date_conv[i]].concate(bnp->data->dt_seq_pb[i],
					     field, mdate);
      
   xms = XmStringCreate(field, XmSTRING_DEFAULT_CHARSET);
   XtVaSetValues(WIDGET(pb_seq_date[i]), XmNlabelString, xms, NULL);
   XmStringFree(xms);
  }
 }
}

static void refresh_name_size_field(This, bnp)
c_Box_nombres *This;
bnp_date_t *bnp;
{
 int	i;
 double ddate;
 char	field[Xq_FORMULA_LENGTH];
 xc_date_time_t	xc_date_time;
 xc_date_t		xc_date, *mdate;
 XmString      xms;

 Xc_HISTORY(("Refresh BNP Date"));
  
 if (This->preview_string)
 {
  ddate = atof(This->preview_string);
  Xc_TRACE(("refresh preview: user: %g", ddate));
 }
 else
 {
  ddate = atof(This->bnp_bar[This->selected_bnp_bar]->preview_string);
  Xc_TRACE(("refresh preview: const: %g", ddate));
 }
 if (This->selected_bnp_bar == BN_FMT_DATE_TIME)
  mdate = &(convert_to_date_and_time(ddate, &xc_date_time)->date);
 else
  mdate = convert_to_date(ddate, &xc_date);
  
 for(i = 0; i < 15; i++)
 {
  if (seq_id_date_name_size[i])
  {
   field[0] = '\0';
   dt_seq_funcs[seq_id_date_conv[i]].concate(bnp->data->dt_seq_pb[i],
					     field, mdate);
      
   xms = XmStringCreate(field, XmSTRING_DEFAULT_CHARSET);
   XtVaSetValues(WIDGET(pb_seq_date[i]), XmNlabelString, xms, NULL);
   XmStringFree(xms);
  }
 }
}

static void change_to_no_pad(w, This, cbs)
Widget	w;
c_Box_nombres *This;
XmToggleButtonCallbackStruct *cbs;
{
 int	i;
 bnp_date_t *bnp;
  
 Xc_TRACE(("change to no pad callback"));
 bnp = (bnp_date_t *)This->bnp_std_sel;
  
 bnp->data->padding = Xq_PADDING_NONE;
 for(i = 0; i < 15; i++)
 {
  if (seq_id_date_padding[i])
  {
   dt_seq_funcs[seq_id_date_conv[i]].delete(bnp->data->dt_seq_pb[i]);
   bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_date_conv[i]]
    .new(seq_id_date_conv[i], bnp->data->padding);
  }
 }
 refresh_padding_field(This, bnp);
}

static void change_to_pad_with_zero(w, This, cbs)
Widget	w;
c_Box_nombres *This;
XmToggleButtonCallbackStruct *cbs;
{
 int	i;
 bnp_date_t *bnp;
  
 Xc_TRACE(("change to pad with zero callback"));
 bnp = (bnp_date_t *)This->bnp_std_sel;
  
 bnp->data->padding = Xq_PADDING_ZERO;
 for(i = 0; i < 15; i++)
 {
  if (seq_id_date_padding[i])
  {
   dt_seq_funcs[seq_id_date_conv[i]].delete(bnp->data->dt_seq_pb[i]);
   bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_date_conv[i]]
    .new(seq_id_date_conv[i], bnp->data->padding);
  }
 }
 refresh_padding_field(This, bnp);
}

static void change_to_pad_with_space(w, This, cbs)
Widget	w;
c_Box_nombres *This;
XmToggleButtonCallbackStruct *cbs;
{
 int	i;
 bnp_date_t *bnp;
  
 Xc_TRACE(("change to pad with space callback"));
 bnp = (bnp_date_t *)This->bnp_std_sel;
  
 bnp->data->padding = Xq_PADDING_SPACE;
 for(i = 0; i < 15; i++)
 {
  if (seq_id_date_padding[i])
  {
   dt_seq_funcs[seq_id_date_conv[i]].delete(bnp->data->dt_seq_pb[i]);
   bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_date_conv[i]]
    .new(seq_id_date_conv[i], bnp->data->padding);
  }
 }
 refresh_padding_field(This, bnp);
}

static void change_to_ns_very_short(w, This, cbs)
Widget	w;
c_Box_nombres *This;
XmToggleButtonCallbackStruct *cbs;
{
 int	i;
 bnp_date_t *bnp;
  
 Xc_TRACE(("change to name size very short callback"));
 bnp = (bnp_date_t *)This->bnp_std_sel;
  
 bnp->data->name_size = Xq_NAME_VERY_SHORT;
 for(i = 0; i < 15; i++)
 {
  if (seq_id_date_name_size[i])
  {
   dt_seq_funcs[seq_id_date_conv[i]].delete(bnp->data->dt_seq_pb[i]);
   bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_date_conv[i]]
    .new(seq_id_date_conv[i], bnp->data->name_size);
  }
 }
 refresh_name_size_field(This, bnp);
}

static void change_to_ns_short(w, This, cbs)
Widget	w;
c_Box_nombres *This;
XmToggleButtonCallbackStruct *cbs;
{
 int	i;
 bnp_date_t *bnp;
  
 Xc_TRACE(("change to name size short callback"));
 bnp = (bnp_date_t *)This->bnp_std_sel;
  
 bnp->data->name_size = Xq_NAME_SHORT;
 for(i = 0; i < 15; i++)
 {
  if (seq_id_date_name_size[i])
  {
   dt_seq_funcs[seq_id_date_conv[i]].delete(bnp->data->dt_seq_pb[i]);
   bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_date_conv[i]]
    .new(seq_id_date_conv[i], bnp->data->name_size);
  }
 }
 refresh_name_size_field(This, bnp);
}

static void change_to_ns_long(w, This, cbs)
Widget	w;
c_Box_nombres *This;
XmToggleButtonCallbackStruct *cbs;
{
 int	i;
 bnp_date_t *bnp;
  
 Xc_TRACE(("change to name size long callback"));
 bnp = (bnp_date_t *)This->bnp_std_sel;
  
 bnp->data->name_size = Xq_NAME_LONG;
 for(i = 0; i < 15; i++)
 {
  if (seq_id_date_name_size[i])
  {
   dt_seq_funcs[seq_id_date_conv[i]].delete(bnp->data->dt_seq_pb[i]);
   bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_date_conv[i]]
    .new(seq_id_date_conv[i], bnp->data->name_size);
  }
 }
 refresh_name_size_field(This, bnp);
}

static void pb_callbacks(w, This)
Widget	w;
c_Box_nombres *This;
{
 bnp_date_t *bnp;
 int	i = 0;
  
 while(i < 15 && WIDGET(pb_seq_date[i]) != w)
  i++;
 if (i == 15) return;
  
 bnp = (bnp_date_t *)This->bnp_std_sel;
 bn_insert_dt_seq(This, bnp->data->dt_seq_pb[i]);
 XmProcessTraversal(WIDGET(This->tfPreview), XmTRAVERSE_CURRENT);
}
