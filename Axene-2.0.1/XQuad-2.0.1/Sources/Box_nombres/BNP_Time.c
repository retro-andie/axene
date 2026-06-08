/*
** BNP_Time.c for XQuad in Box_nombres/
** Box Nombres Part that manage time to other format
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
** Last update Mon May 29 15:18:37 2000 Emmanuel Paris
*/

#include "BNP_Time.h"
#include "TimeFuncs.h"

static void CreateBNP_Time ___PROTO((c_Box_nombres *This, bnp_time_t *bnp));
static void ManageBNP_Time ___PROTO((c_Box_nombres *This, bnp_time_t *bnp));
static void RefreshBNP_Time ___PROTO((c_Box_nombres *This, bnp_time_t *bnp));
static void DestroyBNP_Time ___PROTO((c_Box_nombres *This, bnp_time_t *bnp));
static void refresh_padding_field ___PROTO((c_Box_nombres *This,
					    bnp_time_t *bnp));

static void change_to_no_pad();
static void change_to_pad_with_zero();
static void change_to_pad_with_space();
static void pb_callbacks();

bnp_std_t bnp_times = 
{
 FALSE, FALSE,
 CreateBNP_Time,
 ManageBNP_Time,
 RefreshBNP_Time,
 DestroyBNP_Time,
 NULL
};

static int seq_id_time_conv[14] = 
{
 Xq_DT_HOUR, Xq_DT_HOUR12, Xq_DT_HOUR_AM_PM, Xq_DT_MINUTE,
 Xq_DT_SECOND, Xq_DT_TENTH_OF_SECOND, Xq_DT_HUNDREDTH_OF_SECOND,
 Xq_DT_THOUSANDTH_OF_SECOND, Xq_DT_SEPARATOR, Xq_DT_SEPARATOR,
 Xq_DT_SEPARATOR, Xq_DT_SEPARATOR, Xq_DT_SEPARATOR,
 Xq_DT_SEPARATOR
};

static boolean seq_id_time_padding[14] =
{
 TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE,
 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
};

static boolean seq_id_time_separator[14] =
{
 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
 TRUE, TRUE, TRUE, TRUE, TRUE, TRUE
};

static char *time_string_filler[14] = 
{
 "00", "00", "PM", "00", "00", "0", "00", "000", "'", "\"", ":", ".", " ", ","
};

static c_PushButton *pb_seq_time[14];

/* -------------------------------------------------------------------- ** 
** CreateBNP_Time - BNP Time Creator					** 
** -------------------------------------------------------------------- */
static void CreateBNP_Time(This, bnp)
c_Box_nombres *This;
bnp_time_t *bnp;
{
 int	i;
  
 Xc_HISTORY(("Create BNP Time"));
 bnp->data = 
  (data_time_t *)Xc_malloc("data", sizeof(data_time_t));
 memset(bnp->data, 0, sizeof(data_time_t)); 

 bnp->data->padding = Xq_PADDING_NONE;
 for(i = 0; i < 14; i++)
 {
  if (seq_id_time_padding[i])
   bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_time_conv[i]]
    .new(seq_id_time_conv[i], bnp->data->padding);
  else
   if (seq_id_time_separator[i])
    bnp->data->dt_seq_pb[i] = 
     dt_seq_funcs[seq_id_time_conv[i]].new(seq_id_time_conv[i],
					   time_string_filler[i]);
   else
    bnp->data->dt_seq_pb[i] = 
     dt_seq_funcs[seq_id_time_conv[i]].new(seq_id_time_conv[i]);
      
 }
 bnp->data->Grid = 
  (c_Grid *)NEW(c_Grid)(WIDGET(This->fSetup), NULL, "gBNTime", TRUE);
  

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
  
 bnp->data->sSeparator1 = (c_Separator *)NEW(c_Separator)
  (WIDGET(bnp->data->Grid), NULL, XcSEPARATOR_H);
  
 bnp->data->rcHour = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcHour", 0, 0, XmHORIZONTAL);
  
 bnp->data->lHour = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
					    NULL, "lHour");
 pb_seq_time[1] = bnp->data->pbHour12 = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcHour),
				    NULL, "pbHour12");
 pb_seq_time[2] = bnp->data->pbHourAM_PM = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcHour),
				    NULL, "pbHourAM_PM");
  
 bnp->data->lHour24 = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->rcHour),
					      NULL, " (24)");
 pb_seq_time[0] = bnp->data->pbHour24 = 
  (c_PushButton *)NEW(c_PushButton)(WIDGET(bnp->data->rcHour),
				    NULL, "pbHour24");
  
 bnp->data->rcMinute = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcMinute", 0, 0, XmHORIZONTAL);
  
 bnp->data->lMinute = (c_Label *)NEW(c_Label)
  (WIDGET(bnp->data->Grid), NULL, "lMinute");
  
 pb_seq_time[3] = bnp->data->pbMinute = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcMinute), NULL, "pbMinute");
  
 bnp->data->lSecond = (c_Label *)NEW(c_Label)
  (WIDGET(bnp->data->Grid), NULL, "lSecond");
  
 bnp->data->rcSecond = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcSecond", 0, 0, XmHORIZONTAL);

 pb_seq_time[4] = bnp->data->pbSecond = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSecond), NULL, "pbSecond");
  
 bnp->data->rcTenthOfSecond = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcTenthOfSecond", 0, 0, XmHORIZONTAL);
  
 bnp->data->lTenthOfSecond = (c_Label *)NEW(c_Label)
  (WIDGET(bnp->data->Grid), NULL, "lTenthOfSecond");
  
 pb_seq_time[5] = bnp->data->pbTenthOfSecond = 
  (c_PushButton *)NEW(c_PushButton)
   (WIDGET(bnp->data->rcTenthOfSecond), NULL, "pbTenthOfSecond");
  
 bnp->data->rcHundredthOfSecond = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcHundredthOfSecond", 0, 0, XmHORIZONTAL);
  
 bnp->data->lHundredthOfSecond = (c_Label *)NEW(c_Label)
  (WIDGET(bnp->data->Grid), NULL, "lHundredthOfSecond");
  
 pb_seq_time[6] = bnp->data->pbHundredthOfSecond = 
  (c_PushButton *)NEW(c_PushButton)
   (WIDGET(bnp->data->rcHundredthOfSecond), NULL, "pbHundredthOfSecond");
  
 bnp->data->rcThousandthOfSecond = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid),NULL, "rcThousandthOfSecond", 0, 0, XmHORIZONTAL);
  
 bnp->data->lThousandthOfSecond = (c_Label *)NEW(c_Label)
  (WIDGET(bnp->data->Grid), NULL, "lThousandthOfSecond");
  
 pb_seq_time[7] = bnp->data->pbThousandthOfSecond = 
  (c_PushButton *)NEW(c_PushButton)
   (WIDGET(bnp->data->rcThousandthOfSecond), NULL, "pbThousandthOfSecond");
  
 bnp->data->sSeparator2 = (c_Separator *)NEW(c_Separator)
  (WIDGET(bnp->data->Grid), NULL, XcSEPARATOR_H);
  
 bnp->data->lSep = (c_Label *)NEW(c_Label)(WIDGET(bnp->data->Grid),
					   NULL, "lSep");
 bnp->data->rcSeparators = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(bnp->data->Grid), NULL, "rcSeparators", 0, 0, XmHORIZONTAL);
  
 XtVaSetValues(WIDGET(bnp->data->rcSeparators), 
	       XmNpacking, XmPACK_COLUMN,
	       NULL);
  
 pb_seq_time[8] = bnp->data->pbSep1 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, time_string_filler[8]);
 pb_seq_time[9] = bnp->data->pbSep2 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, time_string_filler[9]);
 pb_seq_time[10] = bnp->data->pbSep3 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, time_string_filler[10]);
 pb_seq_time[11] = bnp->data->pbSep4 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, time_string_filler[11]);
 pb_seq_time[12] = bnp->data->pbSep5 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, time_string_filler[12]);
 pb_seq_time[13] = bnp->data->pbSep6 = (c_PushButton *)NEW(c_PushButton)
  (WIDGET(bnp->data->rcSeparators), NULL, time_string_filler[13]);

 F(bnp->data->Grid).addColumn(bnp->data->Grid, 100,
			      WIDGET(bnp->data->fPadding), XcG_MAX_SIZE,
			      NULL);
  
 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->sSeparator1), XcG_MAX_SIZE,
			      NULL);

 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lHour), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcHour), XcG_MAX_SIZE,
			      NULL);
  
 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lMinute), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcMinute), XcG_MAX_SIZE,
			      NULL);

 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lSecond), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcSecond), XcG_MAX_SIZE,
			      NULL);
  
 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lTenthOfSecond), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcTenthOfSecond),
			      XcG_MAX_SIZE, NULL);
  
 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lHundredthOfSecond), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcHundredthOfSecond),
			      XcG_MAX_SIZE, NULL);

 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lThousandthOfSecond), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcThousandthOfSecond),
			      XcG_MAX_SIZE, NULL);
  
 F(bnp->data->Grid).addColumn(bnp->data->Grid, 25,
			      WIDGET(bnp->data->sSeparator2), XcG_MAX_SIZE,
			      NULL);

 F(bnp->data->Grid).addColumn(bnp->data->Grid, 30,
			      WIDGET(bnp->data->lSep), 190,
			      XcG_WIDGET_NULL, 10,
			      WIDGET(bnp->data->rcSeparators), XcG_MAX_SIZE,
			      NULL);

 for(i = 0; i < 14; i++)
 {
  XmString      xms;
      
  xms = XmStringCreate(time_string_filler[i], XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(WIDGET(pb_seq_time[i]), XmNlabelString, xms, NULL);
  XmStringFree(xms);
  F(pb_seq_time[i]).Add_Callback(pb_seq_time[i], FUNC_ACTIVATE,
				 pb_callbacks, This);
  XtVaSetValues(WIDGET(pb_seq_time[i]), XmNrecomputeSize, False);
 }

 if (This->dim_calc)
 {
  XtVaSetValues(WIDGET(bnp->data->Grid), XmNwidth, This->width,
		XmNheight, This->height, NULL);
  F(bnp->data->Grid).callResize(bnp->data->Grid);
 }
    
 XtManageChild(WIDGET(bnp->data->tbNoPad));
 XtManageChild(WIDGET(bnp->data->tbPadWith0));
 XtManageChild(WIDGET(bnp->data->tbPadWithSpace));
 XtManageChild(WIDGET(bnp->data->rcPadding));
 XtManageChild(WIDGET(bnp->data->fPadding));

 XtManageChild(WIDGET(bnp->data->sSeparator1));
  
 XtManageChild(WIDGET(bnp->data->lHour));
 XtManageChild(WIDGET(bnp->data->pbHour12));
 XtManageChild(WIDGET(bnp->data->pbHourAM_PM));
 XtManageChild(WIDGET(bnp->data->rcHour));
 XtManageChild(WIDGET(bnp->data->lHour24));
 XtManageChild(WIDGET(bnp->data->pbHour24));
  
 XtManageChild(WIDGET(bnp->data->lMinute));
 XtManageChild(WIDGET(bnp->data->pbMinute));
 XtManageChild(WIDGET(bnp->data->rcMinute));
 XtManageChild(WIDGET(bnp->data->lSecond));
 XtManageChild(WIDGET(bnp->data->pbSecond));
 XtManageChild(WIDGET(bnp->data->rcSecond));
  
 XtManageChild(WIDGET(bnp->data->lTenthOfSecond));
 XtManageChild(WIDGET(bnp->data->pbTenthOfSecond));
 XtManageChild(WIDGET(bnp->data->rcTenthOfSecond));
  
 XtManageChild(WIDGET(bnp->data->lHundredthOfSecond));
 XtManageChild(WIDGET(bnp->data->pbHundredthOfSecond));
 XtManageChild(WIDGET(bnp->data->rcHundredthOfSecond));
  
 XtManageChild(WIDGET(bnp->data->lThousandthOfSecond));
 XtManageChild(WIDGET(bnp->data->pbThousandthOfSecond));
 XtManageChild(WIDGET(bnp->data->rcThousandthOfSecond));

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
** ManageBNP_Time - BNP Time Manager					** 
** -------------------------------------------------------------------- */
static void ManageBNP_Time(This, bnp)
c_Box_nombres *This;
bnp_time_t *bnp;
{
 int	i;
 Pixel	background;
  
 if (bnp->managed)
 {
  Xc_HISTORY(("Unmanage BNP Time"));
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
  Xc_HISTORY(("Manage BNP Time"));      
  XtManageChild(WIDGET(bnp->data->Grid)); 
  for(i = 0; i < 14; i++)
   if (seq_id_time_padding[i])
    XtVaSetValues(WIDGET(pb_seq_time[i]), XmNrecomputeSize, False, NULL);
      
  RefreshBNP_Time(This, bnp);
      
  XtVaGetValues(WIDGET(This->tfPreview), XmNbackground, 
		&(bnp->data->background), NULL);
      
  XtVaGetValues(WIDGET(This->PBox->text_field), XmNbackground, 
		&background, NULL);
      
  XtVaSetValues(WIDGET(This->tfPreview),
		XmNbackground, background,
		XmNeditable, True,
		XmNtraversalOn, True,
		XmNselectionArrayCount, 3,
		XmNverifyBell, False,
		XmNcursorPositionVisible, True,
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
** RefreshBNP_Time - BNP Time Refresh value				** 
** -------------------------------------------------------------------- */
static void RefreshBNP_Time(This, bnp)
c_Box_nombres *This;
bnp_time_t *bnp;
{
 int	i;
 double dtime;
 char	field[Xq_FORMULA_LENGTH];
 xc_date_time_t	xc_date_time;
 xc_time_t		xc_time, *mtime;
 XmString      xms;

 Xc_HISTORY(("Refresh BNP Time"));
  
 if (This->preview_string)
 {
  dtime = atof(This->preview_string);
  Xc_TRACE(("refresh preview: user: %g", dtime));
 }
 else
 {
  dtime = atof(This->bnp_bar[This->selected_bnp_bar]->preview_string);
  Xc_TRACE(("refresh preview: const: %g", dtime));
 }
  
 if (This->selected_bnp_bar == BN_FMT_DATE_TIME)
  mtime = &(convert_to_date_and_time(dtime, &xc_date_time)->time);
 else
  mtime = convert_to_time(dtime, &xc_time);
  
 for(i = 0; i < 14; i++)
 {
  field[0] = '\0';
  dt_seq_funcs[seq_id_time_conv[i]].concate(bnp->data->dt_seq_pb[i],
					    field, mtime);
      
  xms = XmStringCreate(field, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(WIDGET(pb_seq_time[i]), XmNlabelString, xms, NULL);
  XmStringFree(xms);
 }
}

/* -------------------------------------------------------------------- ** 
** DestroyBNP_Time - BNP Time Destructor				** 
** -------------------------------------------------------------------- */
static void DestroyBNP_Time(This, bnp)
c_Box_nombres *This;
bnp_time_t *bnp;
{
 int	i;
 Xc_HISTORY(("Destroy BNP Time"));
  
 for(i = 0; i < 14; i++)
  dt_seq_funcs[seq_id_time_conv[i]].delete(bnp->data->dt_seq_pb[i]);
  
 DELETE(c_PushButton)(bnp->data->pbSep1);
 DELETE(c_PushButton)(bnp->data->pbSep2);
 DELETE(c_PushButton)(bnp->data->pbSep3);
 DELETE(c_PushButton)(bnp->data->pbSep4);
 DELETE(c_PushButton)(bnp->data->pbSep5);
 DELETE(c_PushButton)(bnp->data->pbSep6);
 DELETE(c_RowColumn)(bnp->data->rcSeparators);
 DELETE(c_Label)(bnp->data->lSep);

 DELETE(c_Separator)(bnp->data->sSeparator2);

 DELETE(c_PushButton)(bnp->data->pbThousandthOfSecond);
 DELETE(c_Label)(bnp->data->lThousandthOfSecond);
 DELETE(c_RowColumn)(bnp->data->rcThousandthOfSecond);
  
 DELETE(c_PushButton)(bnp->data->pbHundredthOfSecond);
 DELETE(c_Label)(bnp->data->lHundredthOfSecond);
 DELETE(c_RowColumn)(bnp->data->rcHundredthOfSecond);
  
 DELETE(c_PushButton)(bnp->data->pbTenthOfSecond);
 DELETE(c_Label)(bnp->data->lTenthOfSecond);
 DELETE(c_RowColumn)(bnp->data->rcTenthOfSecond);

 DELETE(c_PushButton)(bnp->data->pbMinute);
 DELETE(c_Label)(bnp->data->lMinute);
 DELETE(c_RowColumn)(bnp->data->rcMinute);
 DELETE(c_PushButton)(bnp->data->pbSecond);
 DELETE(c_Label)(bnp->data->lSecond);
 DELETE(c_RowColumn)(bnp->data->rcSecond);
  
 DELETE(c_PushButton)(bnp->data->pbHour24);
 DELETE(c_Label)(bnp->data->lHour24);
 DELETE(c_PushButton)(bnp->data->pbHour12);
 DELETE(c_PushButton)(bnp->data->pbHourAM_PM);
 DELETE(c_Label)(bnp->data->lHour);
 DELETE(c_RowColumn)(bnp->data->rcHour);
  
 DELETE(c_Separator)(bnp->data->sSeparator1);
  
 DELETE(c_ToggleButton)(bnp->data->tbPadWithSpace);
 DELETE(c_ToggleButton)(bnp->data->tbPadWith0);
 DELETE(c_ToggleButton)(bnp->data->tbNoPad);
 DELETE(c_RowColumn)(bnp->data->rcPadding);
 DELETE(c_Label)(bnp->data->fPadding);
  
 DELETE(c_Grid)(bnp->data->Grid);
  
 Xc_free(bnp->data);
 bnp->created = FALSE;
}

static void refresh_padding_field(This, bnp)
c_Box_nombres *This;
bnp_time_t *bnp;
{
 int	i;
 double dtime;
 char	field[Xq_FORMULA_LENGTH];
 xc_date_time_t	xc_date_time;
 xc_time_t		xc_time, *mtime;
 XmString      xms;

 Xc_HISTORY(("Refresh BNP Time"));
  
 if (This->preview_string)
 {
  dtime = atof(This->preview_string);
  Xc_TRACE(("refresh preview: user: %g", dtime));
 }
 else
 {
  dtime = atof(This->bnp_bar[This->selected_bnp_bar]->preview_string);
  Xc_TRACE(("refresh preview: const: %g", dtime));
 }
 if (This->selected_bnp_bar == BN_FMT_DATE_TIME)
  mtime = &(convert_to_date_and_time(dtime, &xc_date_time)->time);
 else
  mtime = convert_to_time(dtime, &xc_time);
  
 for(i = 0; i < 14; i++)
 {
  if (seq_id_time_padding[i])
  {
   field[0] = '\0';
   dt_seq_funcs[seq_id_time_conv[i]].concate(bnp->data->dt_seq_pb[i],
					     field, mtime);
	  
   xms = XmStringCreate(field, XmSTRING_DEFAULT_CHARSET);
   XtVaSetValues(WIDGET(pb_seq_time[i]), XmNlabelString, xms, NULL);
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
 bnp_time_t *bnp;
  
 Xc_TRACE(("change to no pad callback"));
 bnp = (bnp_time_t *)This->bnp_std_sel;
  
 bnp->data->padding = Xq_PADDING_NONE;
 for(i = 0; i < 14; i++)
 {
  if (seq_id_time_padding[i])
  {
   dt_seq_funcs[seq_id_time_conv[i]].delete(bnp->data->dt_seq_pb[i]);
   bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_time_conv[i]]
    .new(seq_id_time_conv[i], bnp->data->padding);
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
 bnp_time_t *bnp;
  
 Xc_TRACE(("change to pad with zero callback"));
 bnp = (bnp_time_t *)This->bnp_std_sel;
  
 bnp->data->padding = Xq_PADDING_ZERO;
 for(i = 0; i < 14; i++)
 {
  if (seq_id_time_padding[i])
  {
   dt_seq_funcs[seq_id_time_conv[i]].delete(bnp->data->dt_seq_pb[i]);
   bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_time_conv[i]]
    .new(seq_id_time_conv[i], bnp->data->padding);
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
 bnp_time_t *bnp;
  
 Xc_TRACE(("change to pad with space callback"));
 bnp = (bnp_time_t *)This->bnp_std_sel;
  
 bnp->data->padding = Xq_PADDING_SPACE;
 for(i = 0; i < 14; i++)
 {
  if (seq_id_time_padding[i])
  {
   dt_seq_funcs[seq_id_time_conv[i]].delete(bnp->data->dt_seq_pb[i]);
   bnp->data->dt_seq_pb[i] = dt_seq_funcs[seq_id_time_conv[i]]
    .new(seq_id_time_conv[i], bnp->data->padding);
  }
 }
 refresh_padding_field(This, bnp);
}

static void pb_callbacks(w, This)
Widget	w;
c_Box_nombres *This;
{
 bnp_time_t *bnp;
 int	i = 0;
  
 while(i < 14 && WIDGET(pb_seq_time[i]) != w)
  i++;
 if (i == 14) return;
  
 bnp = (bnp_time_t *)This->bnp_std_sel;
 bn_insert_dt_seq(This, bnp->data->dt_seq_pb[i]);
 XmProcessTraversal(WIDGET(This->tfPreview), XmTRAVERSE_CURRENT);
}
