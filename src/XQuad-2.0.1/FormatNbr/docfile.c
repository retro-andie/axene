/*
** docfile.c for XQuad in FormatNbr/
** loading callbacks for FormatNbr
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
** Started on  Fri Dec 16 15:12:00 1994 Antoine Buat
** Last update Sun Dec  1 15:04:55 1996 Emmanuel Paris
*/

#define NTRACE

#include "FormatNbr.h"
#include "Moteur.h"

extern c_Color	*GlobColorAuto;

boolean readFormat ___PROTO((c_DocFile *doc, char *keyword,
			   long param, format_read_t *data));
boolean readFormatList ___PROTO((c_DocFile *doc, char *keyword,
				 long param, format_read_t *data));
boolean writeFormat ___PROTO((c_FormatNbr *this, c_DocFile *doc));
boolean writeFormatList ___PROTO((c_FormatNbr *this, c_DocFile *doc));


static boolean cb_name ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_FormatNbr *format));
/* type keywords */
static boolean cb_type_normal ___PROTO((c_DocFile *doc, char *keyword,
					long param, c_FormatNbr *format));
static boolean cb_type_unit ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_FormatNbr *format));
static boolean cb_type_percent ___PROTO((c_DocFile *doc, char *keyword,
					 long param, c_FormatNbr *format));
static boolean cb_type_fraction ___PROTO((c_DocFile *doc, char *keyword,
					  long param, c_FormatNbr *format));
static boolean cb_type_boolean ___PROTO((c_DocFile *doc, char *keyword,
					 long param, c_FormatNbr *format));
static boolean cb_type_day ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_FormatNbr *format));
static boolean cb_type_month ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_FormatNbr *format));
static boolean cb_type_time ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_FormatNbr *format));
static boolean cb_type_date ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_FormatNbr *format));
static boolean cb_type_date_time ___PROTO((c_DocFile *doc, char *keyword,
					   long param, c_FormatNbr *format));
static boolean cb_type_exponantial ___PROTO((c_DocFile *doc, char *keyword,
					     long param, c_FormatNbr *format));
/* for normal value format */
static boolean cb_minprec ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_FormatNbr *format));
static boolean cb_maxprec ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_FormatNbr *format));
static boolean cb_space ___PROTO((c_DocFile *doc, char *keyword,
				long param, c_FormatNbr *format));
static boolean cb_unit_name ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_FormatNbr *format));
static boolean cb_comma ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_FormatNbr *format));
static boolean cb_color1 ___PROTO((c_DocFile *doc, char *keyword,
				   long param, format_read_t *data));
static boolean cb_color2 ___PROTO((c_DocFile *doc, char *keyword,
				   long param, format_read_t *data));
static boolean cb_uafter ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_FormatNbr *format));
static boolean cb_char ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_FormatNbr *format));
static boolean cb_truncate ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_FormatNbr *format));

/* for exponantial format */
static boolean cb_exp_digit ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_FormatNbr *format));
static boolean cb_exp_multiple ___PROTO((c_DocFile *doc, char *keyword,
					 long param, c_FormatNbr *format));
static boolean cb_exp_sign ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_FormatNbr *format));
static boolean cb_exp_string ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_FormatNbr *format));

/* for fraction format */
static boolean cb_frac_prec ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_FormatNbr *format));
static boolean cb_frac_dec ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_FormatNbr *format));

/* for boolean format */
static boolean cb_true_name ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_FormatNbr *format));
static boolean cb_false_name ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_FormatNbr *format));
/* for day value format */
static boolean cb_day_name_very_short ___PROTO((c_DocFile *doc, char *keyword,
						long param,
						c_FormatNbr *format));
static boolean cb_day_name_short ___PROTO((c_DocFile *doc, char *keyword,
					   long param, c_FormatNbr *format));
static boolean cb_day_name_long ___PROTO((c_DocFile *doc, char *keyword,
					  long param, c_FormatNbr *format));
/* for month value format */
static boolean cb_month_name_very_short ___PROTO((c_DocFile *doc,
						  char *keyword, long param,
						  c_FormatNbr *format));
static boolean cb_month_name_short ___PROTO((c_DocFile *doc, char *keyword,
					     long param, c_FormatNbr *format));
static boolean cb_month_name_long ___PROTO((c_DocFile *doc, char *keyword,
					    long param, c_FormatNbr *format));
/* for date, time and date_time format */
static boolean cb_date_sequence ___PROTO((c_DocFile *doc, char *keyword,
					  long param, c_FormatNbr *format));
static boolean cb_time_sequence ___PROTO((c_DocFile *doc, char *keyword,
					  long param, c_FormatNbr *format));
static boolean cb_date_time_sequence ___PROTO((c_DocFile *doc,
					       char *keyword, long param,
					       c_FormatNbr *format));
/* sequence of date and time */
static boolean cb_dt_year ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_FormatNbr *format));
static boolean cb_dt_year_short ___PROTO((c_DocFile *doc, char *keyword,
					  long param, c_FormatNbr *format));
static boolean cb_dt_year_jc ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_FormatNbr *format));
static boolean cb_dt_month ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_FormatNbr *format));
static boolean cb_dt_month_name_very_short ___PROTO((c_DocFile *doc,
						     char *keyword, long param,
						     c_FormatNbr *format));
static boolean cb_dt_month_name_short ___PROTO((c_DocFile *doc,
						char *keyword, long param,
						c_FormatNbr *format));
static boolean cb_dt_month_name_long ___PROTO((c_DocFile *doc,
					       char *keyword, long param,
					       c_FormatNbr *format));
static boolean cb_dt_week ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_FormatNbr *format));
static boolean cb_dt_day ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_FormatNbr *format));
static boolean cb_dt_day_name_very_short ___PROTO((c_DocFile *doc,
						   char *keyword,
						   long param,
						   c_FormatNbr *format));
static boolean cb_dt_day_name_short ___PROTO((c_DocFile *doc,
					      char *keyword, long param,
					      c_FormatNbr *format));
static boolean cb_dt_day_name_long ___PROTO((c_DocFile *doc, char *keyword,
					     long param, c_FormatNbr *format));
static boolean cb_dt_day_in_year ___PROTO((c_DocFile *doc, char *keyword,
					   long param, c_FormatNbr *format));
static boolean cb_dt_hour ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_FormatNbr *format));
static boolean cb_dt_hour12 ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_FormatNbr *format));
static boolean cb_dt_hour_am_pm ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_FormatNbr *format));
static boolean cb_dt_minute ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_FormatNbr *format));
static boolean cb_dt_second ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_FormatNbr *format));
static boolean cb_dt_10e_second ___PROTO((c_DocFile *doc, char *keyword,
					  long param, c_FormatNbr *format));
static boolean cb_dt_100e_second ___PROTO((c_DocFile *doc, char *keyword,
					   long param, c_FormatNbr *format));
static boolean cb_dt_1000e_second ___PROTO((c_DocFile *doc, char *keyword,
					    long param, c_FormatNbr *format));
static boolean cb_dt_separator ___PROTO((c_DocFile *doc, char *keyword,
					 long param, c_FormatNbr *format));
static boolean cb_dt_padding_none ___PROTO((c_DocFile *doc, char *keyword,
					    long param, c_FormatNbr *format));
static boolean cb_dt_padding_space ___PROTO((c_DocFile *doc, char *keyword,
					     long param, c_FormatNbr *format));
static boolean cb_dt_padding_zero ___PROTO((c_DocFile *doc, char *keyword,
					    long param, c_FormatNbr *format));
/* other format */
static boolean cb_other_format_list ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_FormatNbr *format));
static boolean cb_other_format ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_FormatNbr *format));
/* special formats */
static boolean cb_generic ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_FormatNbr *format));
static boolean cb_standard ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_FormatNbr *format));
static boolean cb_percent ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_FormatNbr *format));
static boolean cb_currency ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_FormatNbr *format));
static boolean cb_thousands ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_FormatNbr *format));
static boolean cb_scientific ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_FormatNbr *format));
static boolean cb_engineer ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_FormatNbr *format));

static boolean cb_hide ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_FormatNbr *format));
static boolean cb_lock ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_FormatNbr *format));

static int conv_seq_id[4] = { 
  0, XcF_ADD_DATE_SEQ, XcF_ADD_TIME_SEQ, XcF_ADD_DATE_TIME_SEQ };

/* ----------------------------------------------------------------- ** 
** readFormatList - Load some Formats                                ** 
** ----------------------------------------------------------------- */
boolean readFormatList(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
format_read_t *data;
{
  int	i, id;
  c_FormatNbr	*format, *format2;
  c_FormatNbr	*other_format[15];
  Xc_TRACE(("readFormatList"));

  if(!F(doc).addCallbacks(doc, XcDF_FORMAT_KEYWORD, readFormat, data, NULL))
    return FALSE;
  if (!F(doc).expectKeyword(doc, XcDF_FORMAT_KEYWORD, TRUE))
    return FALSE;

  format = *data->format_base;
  while(format)
    {
      memcpy(other_format, format->other_format, sizeof(c_FormatNbr *) * 15);
      memset(format->other_format, 0, sizeof(c_FormatNbr *)*15);
      i = 0;
      while(other_format[i])
	{
	  id = ((int)other_format[i]) & 0x00ffffff;
	  format2 = *data->format_base;
	  while(format2 && format2->id != id)
	    format2 = format2->next;
	  format->other_format[i] = format2;
	  i++;
	}
      format = format->next;
    }
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** readFormat - Set a Format from a saved form                       ** 
** ----------------------------------------------------------------- */
boolean readFormat(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
format_read_t *data;
{
  c_FormatNbr *format;

  Xc_TRACE(("readFormat"));

  if((format = NEW(c_FormatNbr)(data->format_base, NULL)) == NULL) 
    return FALSE;

  do {
    data->format = format;
    if(!F(doc).addCallbacks(doc,
			    XcDF_NAME_KEYWORD, cb_name, format,
			    
			    XcDF_TYPE_NORMAL_KEYWORD, cb_type_normal, format,
			    XcDF_TYPE_UNIT_KEYWORD, cb_type_unit, format,
			    XcDF_TYPE_PERCENT_KEYWORD, cb_type_percent, format,
			    XcDF_TYPE_FRACTION_KEYWORD,
				cb_type_fraction, format,
			    XcDF_TYPE_BOOLEAN_KEYWORD, cb_type_boolean, format,
			    XcDF_TYPE_DAY_KEYWORD, cb_type_day, format,
			    XcDF_TYPE_MONTH_KEYWORD, cb_type_month, format,
			    XcDF_TYPE_TIME_KEYWORD, cb_type_time, format,
			    XcDF_TYPE_DATE_KEYWORD, cb_type_date, format,
			    XcDF_TYPE_DATE_TIME_KEYWORD,
				cb_type_date_time, format,
			    XcDF_TYPE_EXPONANTIAL_KEYWORD,
				cb_type_exponantial, format,  
			    
			    XcDF_MIN_PREC_KEYWORD, cb_minprec, format,
			    XcDF_MAX_PREC_KEYWORD, cb_maxprec, format,
			    XcDF_SPACE_KEYWORD, cb_space, format,
			    XcDF_COMMA_KEYWORD, cb_comma, format,
			    XcDF_POS_KEYWORD, cb_color1, data,
			    XcDF_NEG_KEYWORD, cb_color2, data,
			    XcDF_CHARSEP_KEYWORD, cb_char, format,
			    XcDF_TRUNCATE_KEYWORD, cb_truncate, format,
			    XcDF_C_MIN_PREC_KEYWORD, cb_minprec, format,
			    XcDF_C_MAX_PREC_KEYWORD, cb_maxprec, format,
			    XcDF_C_COMMA_KEYWORD, cb_comma, format,
			    XcDF_C_TRUNCATE_KEYWORD, cb_truncate, format,
			    
			    XcDF_UNIT_NAME_KEYWORD, cb_unit_name, format,
			    XcDF_UNIT_AFTER_KEYWORD, cb_uafter, format,
			    XcDF_C_UNIT_NAME_KEYWORD, cb_unit_name, format,
			    XcDF_C_UNIT_AFTER_KEYWORD, cb_uafter, format,
			    
			    XcDF_EXP_DIGIT_KEYWORD, cb_exp_digit, format,
			    XcDF_EXP_MULTIPLE_KEYWORD, cb_exp_multiple, format,
			    XcDF_EXP_SIGN_KEYWORD, cb_exp_sign, format,
			    XcDF_EXP_STRING_KEYWORD, cb_exp_string, format,
			    
			    XcDF_FRAC_PREC_KEYWORD, cb_frac_prec, format,
			    XcDF_FRAC_DECIMAL_KEYWORD, cb_frac_dec, format,
			    
			    XcDF_TRUE_COLOR_KEYWORD, cb_color1, data,
			    XcDF_FALSE_COLOR_KEYWORD, cb_color2, data,
			    XcDF_TRUE_NAME_KEYWORD, cb_true_name, format,
			    XcDF_FALSE_NAME_KEYWORD, cb_false_name, format,
			    
			    XcDF_DAY_NAME_VERY_SHORT_KEYWORD, 
				cb_day_name_very_short, format,
			    XcDF_DAY_NAME_SHORT_KEYWORD,
				cb_day_name_short, format,
			    XcDF_DAY_NAME_LONG_KEYWORD,
				cb_day_name_long, format,
			    
			    XcDF_MONTH_NAME_VERY_SHORT_KEYWORD,
				cb_month_name_very_short, format,
			    XcDF_MONTH_NAME_SHORT_KEYWORD,
				cb_month_name_short, format,
			    XcDF_MONTH_NAME_LONG_KEYWORD,
				cb_month_name_long, format,
			    
			    XcDF_DATE_SEQ_KEYWORD, cb_date_sequence, format,
			    XcDF_TIME_SEQ_KEYWORD, cb_time_sequence, format,
			    XcDF_DATE_TIME_SEQ_KEYWORD,
			    	cb_date_time_sequence, format,

			    XcDF_OTHER_FORMATS_KEYWORD,
			    	cb_other_format_list, format,

			    XcDF_GENERIC_KEYWORD, cb_generic, format,
			    XcDF_STANDARD_KEYWORD, cb_standard, format,
			    XcDF_PERCENT_KEYWORD, cb_percent, format,
			    XcDF_CURRENCY_KEYWORD, cb_currency, format,
			    XcDF_THOUSANDS_KEYWORD, cb_thousands, format,
			    XcDF_SCIENTIFIC_KEYWORD, cb_scientific, format,
			    XcDF_ENGINEER_KEYWORD, cb_engineer, format,
			    
			    XcDF_HIDE_KEYWORD, cb_hide, format,
			    XcDF_LOCK_KEYWORD, cb_lock, format,
			    
			    NULL)) break;
    if(!F(doc).expectKeyword(doc, NULL, TRUE)) break;
    format->id = param;
    return TRUE;
  } while(0);

  DELETE(c_FormatNbr)(format);
  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_name - Name keyword loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_name(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *this;
{
  unsigned char *name;

  Xc_TRACE(("cb_name"));

  if(!F(doc).readString(doc, &name)) return FALSE;
  if(name != NULL)
    {
      F(this).set(this, XcF_NAME, name, XcF_END);
      Xc_free(name);
    }
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_type_normal - Type_Normal keyword loading callback             ** 
** ----------------------------------------------------------------- */
static boolean cb_type_normal(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_type_normal"));
  F(format).set(format, XcF_TYPE, Xq_VALUE, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_type_unit - Type_Unit keyword loading callback                 ** 
** ----------------------------------------------------------------- */
static boolean cb_type_unit(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_type_unit"));
  F(format).set(format, XcF_TYPE, Xq_UNIT, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_type_percent - Type_Percent keyword loading callback           **
** ----------------------------------------------------------------- */
static boolean cb_type_percent(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_type_percent"));
  F(format).set(format, XcF_TYPE, Xq_PERCENT, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_type_fraction - Type_Fraction keyword loading callback         **
** ----------------------------------------------------------------- */
static boolean cb_type_fraction(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_type_fraction"));
  F(format).set(format, XcF_TYPE, Xq_FRACTION, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_type_boolean - Type_Boolean keyword loading callback           **
** ----------------------------------------------------------------- */
static boolean cb_type_boolean(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_type_boolean"));
  F(format).set(format, XcF_TYPE, Xq_BOOLEAN, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_type_day - Type_Day keyword loading callback                   **
** ----------------------------------------------------------------- */
static boolean cb_type_day(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_type_day"));
  F(format).set(format, XcF_TYPE, Xq_DAY_IN_WEEK, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_type_month - Type_Month keyword loading callback               **
** ----------------------------------------------------------------- */
static boolean cb_type_month(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_type_month"));
  F(format).set(format, XcF_TYPE, Xq_MONTH, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_type_time - Type_Time keyword loading callback                 **
** ----------------------------------------------------------------- */
static boolean cb_type_time(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_type_time"));
  F(format).set(format, XcF_TYPE, Xq_TIME, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_type_date - Type_Date keyword loading callback                 ** 
** ----------------------------------------------------------------- */
static boolean cb_type_date(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_type_date"));
  F(format).set(format, XcF_TYPE, Xq_DATE, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_type_date_time - Type_Date_Time keyword loading callback       **
** ----------------------------------------------------------------- */
static boolean cb_type_date_time(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_type_date_time"));
  F(format).set(format, XcF_TYPE, Xq_DATE_TIME, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_type_exponantial - Type_Exponantial keyword loading callback   **
** ----------------------------------------------------------------- */
static boolean cb_type_exponantial(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_type_exponantial"));
  F(format).set(format, XcF_TYPE, Xq_EXPONANTIAL, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_minprec - precision loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_minprec(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_minprec(%ld)", param));

  param = MIN(MAX_MAX_PREC, MAX(0, param));
  F(format).set(format, XcF_MINPREC, (unsigned int)param, XcF_END);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_maxprec - precision loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_maxprec(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_maxprec(%ld)", param));

  param = MIN(MAX_MAX_PREC, MAX(0, param));
  F(format).set(format, XcF_MAXPREC, (unsigned int)param, XcF_END);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_space - space loading callback                                 ** 
** ----------------------------------------------------------------- */
static boolean cb_space(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_space(%ld)", param));

  param = MIN(MAX_SPACE, MAX(0, param));
  F(format).set(format, XcF_SPACE, (unsigned int)param, XcF_END);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_unit_name - unit_name loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_unit_name(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *this;
{
  unsigned char *str;

  Xc_TRACE(("cb_unit_name"));

  if(!F(doc).readString(doc, &str)) return FALSE;
  if(str != NULL)
  {
      F(this).set(this, XcF_UNIT_NAME, str, XcF_END);
      Xc_free(str);
    }
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_comma - comma loading callback                                 ** 
** ----------------------------------------------------------------- */
static boolean cb_comma(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_comma(%c)", (char)param));
  F(format).set(format, XcF_COMMA, (int)param, XcF_END);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_color1 - color1 loading callback                               ** 
** ----------------------------------------------------------------- */
static boolean cb_color1(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
format_read_t *data;
{
  c_Color *ptr;
  
  Xc_TRACE(("cb_color1(%ld)", param));
  
  if(param < 0) param = 0;
  for(ptr = *data->color_base; ptr != NULL; ptr = ptr->next)
    if(ptr->id == param)
    {
      F(data->format).set(data->format, XcF_COLORP, ptr, XcF_END);
      return TRUE;
    }
  
  Xc_ERROR(("Bad color #%ld", param));
  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_color2 - color2 loading callback                               ** 
** ----------------------------------------------------------------- */
static boolean cb_color2(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
format_read_t *data;
{
  c_Color *ptr;

  Xc_TRACE(("cb_color2(%ld)", param));

  if(param < 0) param = 0;
  for(ptr = *data->color_base; ptr != NULL; ptr = ptr->next)
    if(ptr->id == param)
      {
	F(data->format).set(data->format, XcF_COLORN, ptr, XcF_END);
	return TRUE;
      }

  Xc_ERROR(("Bad color #%ld", param));
  return FALSE;
}

/* ----------------------------------------------------------------- ** 
** cb_uafter - uafter loading callback                               ** 
** ----------------------------------------------------------------- */
static boolean cb_uafter(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_uafter"));

  F(format).set(format, XcF_UNITAFTER, (int)TRUE, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_char - char loading callback                                   ** 
** ----------------------------------------------------------------- */
static boolean cb_char(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_char(%c)", (char)param));
  F(format).set(format, XcF_CHARSEP, (int)param, XcF_END);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_truncate - truncate enable loading callback                    ** 
** ----------------------------------------------------------------- */
static boolean cb_truncate(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_truncate"));

  F(format).set(format, XcF_TRUNCATE, (int)TRUE, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_exp_digit - exponantial part digit loading callback            ** 
** ----------------------------------------------------------------- */
static boolean cb_exp_digit(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_exp_digit(%ld)", param));

  F(format).set(format, XcF_EXPDIGIT, (unsigned int)param, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_exp_multiple - exp multiple digit loading callback             ** 
** ----------------------------------------------------------------- */
static boolean cb_exp_multiple(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_exp_multiple(%ld)", param));

  F(format).set(format, XcF_EXPMULTIPLE, (unsigned int)param, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_exp_sign - exponant sign enable loading callback               ** 
** ----------------------------------------------------------------- */
static boolean cb_exp_sign(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_exp_sign"));

  F(format).set(format, XcF_EXPSIGN, (int)TRUE, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_exp_string - exponant sign loading callback                    ** 
** ----------------------------------------------------------------- */
static boolean cb_exp_string(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *this;
{
  unsigned char *str;

  Xc_TRACE(("cb_exp_string"));

  if(!F(doc).readString(doc, &str)) return FALSE;
  if(str != NULL)
  {
      F(this).set(this, XcF_EXPSTRING, str, XcF_END);
      Xc_free(str);
    }
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_frac_prec - frac_precision loading callback                    ** 
** ----------------------------------------------------------------- */
static boolean cb_frac_prec(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_frac_prec"));
  param = MAX(1, param);
  F(format).set(format, XcF_FRAC_PREC, (unsigned int)param, XcF_END);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_frac_dec - frac_decimal loading callback                       ** 
** ----------------------------------------------------------------- */
static boolean cb_frac_dec(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_frac_dec"));
  F(format).set(format, XcF_FRAC_DEC, (int)TRUE, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_true_name - true_name loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_true_name(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  unsigned char *name;
  Xc_TRACE(("cb_true_name"));
  
  if(!F(doc).readString(doc, &name)) return FALSE;
  if(name != NULL)
  {
    F(format).set(format, XcF_TRUE_NAME, name, XcF_END);
    Xc_free(name);
  }
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_false_name - false_name loading callback                       ** 
** ----------------------------------------------------------------- */
static boolean cb_false_name(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  unsigned char *name;
  Xc_TRACE(("cb_false_name"));
  
  if(!F(doc).readString(doc, &name)) return FALSE;
  if(name != NULL)
  {
    F(format).set(format, XcF_FALSE_NAME, name, XcF_END);
    Xc_free(name);
  }
  return TRUE;
}
  
/* ----------------------------------------------------------------- ** 
** cb_day_name_very_short - day_name_very_short loading callback     ** 
** ----------------------------------------------------------------- */
static boolean cb_day_name_very_short(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_day_name_very_short"));
  
  F(format).set(format, XcF_DAY_NAME_SIZE, Xq_NAME_VERY_SHORT, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_day_name_short - day_name_short loading callback               ** 
** ----------------------------------------------------------------- */
static boolean cb_day_name_short(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_day_name_short"));
  
  F(format).set(format, XcF_DAY_NAME_SIZE, Xq_NAME_SHORT, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_day_name_long - day_name_long loading callback                 ** 
** ----------------------------------------------------------------- */
static boolean cb_day_name_long(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_day_name_long"));
  
  F(format).set(format, XcF_DAY_NAME_SIZE, Xq_NAME_LONG, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_month_name_very_short - month_name_very_short loading callback ** 
** ----------------------------------------------------------------- */
static boolean cb_month_name_very_short(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_month_name_very_short"));
  
  F(format).set(format, XcF_MONTH_NAME_SIZE, Xq_NAME_VERY_SHORT, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_month_name_short - month_name_short loading callback           ** 
** ----------------------------------------------------------------- */
static boolean cb_month_name_short(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_month_name_short"));
  
  F(format).set(format, XcF_MONTH_NAME_SIZE, Xq_NAME_SHORT, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_month_name_long - month_name_long loading callback             ** 
** ----------------------------------------------------------------- */
static boolean cb_month_name_long(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_month_name_long"));
  
  F(format).set(format, XcF_MONTH_NAME_SIZE, Xq_NAME_LONG, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_date_sequence- date sequence loading callback                  ** 
** ----------------------------------------------------------------- */
static boolean cb_date_sequence(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_date_sequence"));
  
  format->padding = Xq_PADDING_NONE; 
  format->seq_id = Xq_DATE_ID;
  
  if(!F(doc).addCallbacks(doc, 
			  XcDF_YEAR_KEYWORD, cb_dt_year, format,
			  XcDF_YEAR_SHORT_KEYWORD, cb_dt_year_short, format,
			  XcDF_YEAR_JC_KEYWORD, cb_dt_year_jc, format,
			  XcDF_MONTH_KEYWORD, cb_dt_month, format,
			  XcDF_MONTH_NAME_VERY_SHORT_KEYWORD,
				cb_dt_month_name_very_short, format,
			  XcDF_MONTH_NAME_SHORT_KEYWORD,
				cb_dt_month_name_short, format,
			  XcDF_MONTH_NAME_LONG_KEYWORD,
				cb_dt_month_name_long, format,
			  XcDF_WEEK_KEYWORD, cb_dt_week, format,
			  XcDF_DAY_KEYWORD, cb_dt_day, format,
			  XcDF_DAY_NAME_VERY_SHORT_KEYWORD, 
				cb_dt_day_name_very_short, format,
			  XcDF_DAY_NAME_SHORT_KEYWORD,
				cb_dt_day_name_short, format,
			  XcDF_DAY_NAME_LONG_KEYWORD,
				cb_dt_day_name_long, format,
			  XcDF_DAY_IN_YEAR_KEYWORD, cb_dt_day_in_year, format,
			  XcDF_SEPARATOR_KEYWORD, cb_dt_separator, format,
			  XcDF_PADDING_NONE_KEYWORD,
			  	cb_dt_padding_none, format,
			  XcDF_PADDING_SPACE_KEYWORD,
			  	cb_dt_padding_space, format,
			  XcDF_PADDING_ZERO_KEYWORD,
			  	cb_dt_padding_zero, format,
			  NULL));
  
  return F(doc).expectKeyword(doc, NULL, TRUE);
}

/* ----------------------------------------------------------------- ** 
** cb_time_sequence- time sequence loading callback                  ** 
** ----------------------------------------------------------------- */
static boolean cb_time_sequence(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_time_sequence"));
  format->padding = Xq_PADDING_NONE; 
  format->seq_id = Xq_TIME_ID;
  
  if(!F(doc).addCallbacks(doc, 
			  XcDF_HOUR_KEYWORD, cb_dt_hour, format,
			  XcDF_HOUR12_KEYWORD, cb_dt_hour12, format,
			  XcDF_HOUR_AM_PM_KEYWORD, cb_dt_hour_am_pm, format,
			  XcDF_MINUTE_KEYWORD, cb_dt_minute, format,
			  XcDF_SECOND_KEYWORD, cb_dt_second, format,
			  XcDF_TENTH_OF_SECOND_KEYWORD,
			  	cb_dt_10e_second, format,
			  XcDF_HUNDREDTH_OF_SECOND_KEYWORD,
			  	cb_dt_100e_second, format,
			  XcDF_THOUSANDTH_OF_SECOND_KEYWORD,
			  	cb_dt_1000e_second,format,
			  XcDF_SEPARATOR_KEYWORD, cb_dt_separator, format,
			  XcDF_PADDING_NONE_KEYWORD,
			  	cb_dt_padding_none, format,
			  XcDF_PADDING_SPACE_KEYWORD,
				 cb_dt_padding_space, format,
			  XcDF_PADDING_ZERO_KEYWORD,
			  	cb_dt_padding_zero, format,
			  NULL));
  
  return F(doc).expectKeyword(doc, NULL, TRUE);
}

/* ----------------------------------------------------------------- ** 
** cb_date_time_sequence- date_time loading callback                 ** 
** ----------------------------------------------------------------- */
static boolean cb_date_time_sequence(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_date_time_sequence"));
  format->padding = Xq_PADDING_NONE; 
  format->seq_id = Xq_DATE_TIME_ID;
  
  if(!F(doc).addCallbacks(doc, 
			  XcDF_YEAR_KEYWORD, cb_dt_year, format,
			  XcDF_YEAR_SHORT_KEYWORD, cb_dt_year_short, format,
			  XcDF_YEAR_JC_KEYWORD, cb_dt_year_jc, format,
			  XcDF_MONTH_KEYWORD, cb_dt_month, format,
			  XcDF_MONTH_NAME_VERY_SHORT_KEYWORD,
				cb_dt_month_name_very_short, format,
			  XcDF_MONTH_NAME_SHORT_KEYWORD,
				cb_dt_month_name_short, format,
			  XcDF_MONTH_NAME_LONG_KEYWORD,
				cb_dt_month_name_long, format,
			  XcDF_WEEK_KEYWORD, cb_dt_week, format,
			  XcDF_DAY_KEYWORD, cb_dt_day, format,
			  XcDF_DAY_NAME_VERY_SHORT_KEYWORD, 
				cb_dt_day_name_very_short, format,
			  XcDF_DAY_NAME_SHORT_KEYWORD,
				cb_dt_day_name_short, format,
			  XcDF_DAY_NAME_LONG_KEYWORD,
				cb_dt_day_name_long, format,
			  XcDF_DAY_IN_YEAR_KEYWORD, cb_dt_day_in_year, format,
			  XcDF_HOUR_KEYWORD, cb_dt_hour, format,
			  XcDF_HOUR12_KEYWORD, cb_dt_hour12, format,
			  XcDF_HOUR_AM_PM_KEYWORD, cb_dt_hour_am_pm, format,
			  XcDF_MINUTE_KEYWORD, cb_dt_minute, format,
			  XcDF_SECOND_KEYWORD, cb_dt_second, format,
			  XcDF_TENTH_OF_SECOND_KEYWORD,
			  	cb_dt_10e_second, format,
			  XcDF_HUNDREDTH_OF_SECOND_KEYWORD,
			  	cb_dt_100e_second, format,
			  XcDF_THOUSANDTH_OF_SECOND_KEYWORD,
			  	cb_dt_1000e_second,format,
			  XcDF_SEPARATOR_KEYWORD, cb_dt_separator, format,
			  XcDF_PADDING_NONE_KEYWORD,
			  	cb_dt_padding_none, format,
			  XcDF_PADDING_SPACE_KEYWORD,
			  	cb_dt_padding_space, format,
			  XcDF_PADDING_ZERO_KEYWORD,
			  	cb_dt_padding_zero, format,
			  NULL));
  
  return F(doc).expectKeyword(doc, NULL, TRUE);
}

/* ----------------------------------------------------------------- ** 
** cb_dt_year- dt_year loading callback			             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_year(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_year"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_YEAR, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_year_short- dt_year_short loading callback	             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_year_short(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_year_short"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_YEAR_SHORT, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_year_jc- dt_year_jc loading callback     	             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_year_jc(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_year_jc"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_YEAR_JC, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_month- dt_month loading callback		             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_month(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_month"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_MONTH, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_month_name_very_short- dt_month_name_very_short load callb  ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_month_name_very_short(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_month_name_very_short"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_MONTH_NAME, Xq_NAME_VERY_SHORT, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_month_name_short- dt_month_name_short loading callback      ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_month_name_short(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_month_name_short"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_MONTH_NAME, Xq_NAME_SHORT, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_month_name_long- dt_month_name_long loading callback        ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_month_name_long(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_month_name_long"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_MONTH_NAME, Xq_NAME_LONG, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_week- dt_week loading callback			             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_week(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_week"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_WEEK, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_day- dt_day loading callback			             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_day(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_day"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_DAY, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_day_name_very_short- dt_day_name_very_short loading callb   ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_day_name_very_short(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_day_name_very_short"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_DAY_NAME, Xq_NAME_VERY_SHORT, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_day_name_short- dt_day_name_short loading callback          ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_day_name_short(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_day_name_short"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_DAY_NAME, Xq_NAME_SHORT, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_day_name_long- dt_day_name_long loading callback	     ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_day_name_long(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_day_name_long"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_DAY_NAME, Xq_NAME_LONG, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_day_in_year- dt_day_in_year loading callback	             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_day_in_year(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_day_in_year"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_DAY_IN_YEAR, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_hour- dt_hour loading callback                              ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_hour(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_hour"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_HOUR, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_hour12- dt_hour12 loading callback		             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_hour12(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_hour12"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_HOUR12, format->padding, XcF_END);
  return TRUE;
}
/* ----------------------------------------------------------------- ** 
** cb_dt_hour_am_pm- dt_hour_am_pm loading callback	             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_hour_am_pm(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_hour_am_pm"));
  F(format).set(format, conv_seq_id[format->seq_id], 
		Xq_DT_HOUR_AM_PM, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_minute- dt_minute loading callback		             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_minute(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_minute"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_MINUTE, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_second- dt_second loading callback		             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_second(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_second"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_SECOND, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_10e_second- dt_10e_second loading callback	             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_10e_second(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_10e_second"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_TENTH_OF_SECOND, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_100e_second- dt_100e_second loading callback	             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_100e_second(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_100e_second"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_HUNDREDTH_OF_SECOND, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_1000e_second- dt_1000e_second loading callback	             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_1000e_second(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_1000e_second"));
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_THOUSANDTH_OF_SECOND, format->padding, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_separator- dt_separator loading callback	             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_separator(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  unsigned char *separator;
  Xc_TRACE(("cb_dt_separator"));

  if (!F(doc).readString(doc, &separator)) return FALSE;
  if (separator == NULL) return TRUE;
  F(format).set(format, conv_seq_id[format->seq_id],
		Xq_DT_SEPARATOR, separator, XcF_END);
  Xc_free(separator);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_padding_none- dt_padding_none loading callback	             ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_padding_none(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_padding_none"));
  format->padding = Xq_PADDING_NONE; 
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_padding_space- dt_padding_space loading callback	     ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_padding_space(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_padding_space"));
  format->padding = Xq_PADDING_SPACE; 
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_dt_padding_zero- dt_padding_zero loading callback              ** 
** ----------------------------------------------------------------- */
static boolean cb_dt_padding_zero(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_dt_padding_zero"));
  format->padding = Xq_PADDING_ZERO; 
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_other_format_list- dt_other_format_list loading callback       ** 
** ----------------------------------------------------------------- */
static boolean cb_other_format_list(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_other_format_list"));
  
  if(!F(doc).addCallbacks(doc,
			  XcDF_FORMAT_KEYWORD, cb_other_format, format,
			  NULL));
  return F(doc).expectKeyword(doc, NULL, TRUE);
}

/* ----------------------------------------------------------------- ** 
** cb_other_format- dt_other_format loading callback                 ** 
** ----------------------------------------------------------------- */
static boolean cb_other_format(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  int	i;
  Xc_TRACE(("cb_other_format"));
  
  i = 0;
  while(format->other_format[i] && i < 15)
    i++;

  if (i < 15)
    format->other_format[i] = (void *)(0x66000000 + param);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_generic- standard loading callback                             ** 
** ----------------------------------------------------------------- */
static boolean cb_generic(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_generic"));
  F(format).set(format, XcF_LOCK, XcF_HIDE_ON, XcF_SPECIAL, XcFt_GENERIC,
		XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_standard - standard loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_standard(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_standard"));
  F(format).set(format, XcF_LOCK, XcF_SPECIAL, XcFt_STANDARD, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_percent - percent loading callback                             ** 
** ----------------------------------------------------------------- */
static boolean cb_percent(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_percent"));
  F(format).set(format, XcF_LOCK, XcF_SPECIAL, XcFt_PERCENT, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_currency - currency loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_currency(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_currency"));
  F(format).set(format, XcF_LOCK, XcF_SPECIAL, XcFt_CURRENCY, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_thousands - thousands loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_thousands(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{  
  Xc_TRACE(("cb_thousands"));
  F(format).set(format, XcF_LOCK, XcF_SPECIAL, XcFt_THOUSANDS, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_scientific - scientific loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_scientific(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{  
  Xc_TRACE(("cb_scientific"));
  F(format).set(format, XcF_LOCK, XcF_SPECIAL, XcFt_SCIENTIFIC, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_engineer - engineer loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_engineer(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{  
  Xc_TRACE(("cb_engineer"));
  F(format).set(format, XcF_LOCK, XcF_SPECIAL, XcFt_ENGINEER, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_download - Loading callback for all flags                      ** 
** ----------------------------------------------------------------- */
static boolean cb_hide(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_hide"));
  F(format).set(format, XcF_HIDE_ON, XcF_END);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_lock - lock loading callback                                   ** 
** ----------------------------------------------------------------- */
static boolean cb_lock(doc, keyword, param, format)
c_DocFile *doc;
char *keyword;
long param;
c_FormatNbr *format;
{
  Xc_TRACE(("cb_lock"));
  F(format).set(format, XcF_LOCK, XcF_END);
  return TRUE;
}

/*--------------------------------------------------------------------------*/
/*-------------------------------- WRITE -----------------------------------*/
/*--------------------------------------------------------------------------*/

/* ----------------------------------------------------------------- ** 
** writeFormatList - Write all the format in the list                ** 
** ----------------------------------------------------------------- */
boolean writeFormatList(this, doc)
c_FormatNbr *this;
c_DocFile *doc;
{
  c_FormatNbr *ptr;
  long id;

  Xc_HISTORY(("writeList"));

  if(!F(doc).writeKeyword(doc, XcDF_FORMATLIST_KEYWORD, -1L) ||
     !F(doc).startSequence(doc)) return FALSE;

  for(ptr = *this->base, id = 0; ptr != NULL; ptr = ptr->next, id++)
    {
      ptr->id = id;
      if(!F(ptr).write(ptr, doc)) return FALSE;
    }

  Xc_TRACE(("writeList done"));
  return F(doc).endSequence(doc);
}

/* ----------------------------------------------------------------- ** 
** writeFormat - Write the format description                        ** 
** ----------------------------------------------------------------- */
boolean writeFormat(this, doc)
c_FormatNbr *this;
c_DocFile *doc;
{
  int		i;
  Xc_HISTORY(("write(`%s')", this->name));

  if (!F(doc).writeKeyword(doc, XcDF_FORMAT_KEYWORD, this->id)) return FALSE;
  if (!F(doc).startSequence(doc)) return FALSE;
  
  if (this->special == XcFt_GENERIC)
  {
    if (!F(doc).writeKeyword(doc, XcDF_MIN_PREC_KEYWORD, this->min_prec))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_MAX_PREC_KEYWORD, this->max_prec))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_SPACE_KEYWORD, this->space_each))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_COMMA_KEYWORD, this->comma_sep))
      return FALSE;
    if (this->color1 != GlobColorAuto &&
	!F(doc).writeKeyword(doc, XcDF_POS_KEYWORD, this->color1->id))
      return FALSE;
    
    if (this->color2 != GlobColorAuto &&
	!F(doc).writeKeyword(doc, XcDF_NEG_KEYWORD, this->color2->id))
      return FALSE;
    
    
    if (!F(doc).writeKeyword(doc, XcDF_CHARSEP_KEYWORD, this->char_sep))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_TRUNCATE_KEYWORD, -1L)) return FALSE;
    
    if (!Xstrcmp(this->unit, ""))
    {
      if (!F(doc).writeKeyword(doc, XcDF_UNIT_NAME_KEYWORD, -1L))
	return FALSE;
      if (!F(doc).startSequence(doc)) return FALSE;
      if (!F(doc).write(doc, this->unit, strlen(this->unit))) return FALSE;
      if (!F(doc).endSequence(doc)) return FALSE;
    }
    if (this->unitafter &&
	!F(doc).writeKeyword(doc, XcDF_UNIT_AFTER_KEYWORD, -1L))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_TRUE_NAME_KEYWORD, -1L)) return FALSE;
    if (!F(doc).startSequence(doc)) return FALSE;
    if (!F(doc).write(doc, this->true_name, strlen(this->true_name)))
      return FALSE;
    if (!F(doc).endSequence(doc)) return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_FALSE_NAME_KEYWORD, -1L)) return FALSE;
    if (!F(doc).startSequence(doc)) return FALSE;
    if (!F(doc).write(doc, this->false_name, strlen(this->false_name)))
      return FALSE;
    if (!F(doc).endSequence(doc)) return FALSE;
  
      if (!F(doc).writeKeyword(doc, XcDF_TYPE_DAY_KEYWORD, -1L))
	return FALSE;
    
    switch(this->day_name_size)
    {
    case Xq_NAME_VERY_SHORT:
      if (!F(doc).writeKeyword(doc, XcDF_DAY_NAME_VERY_SHORT_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_NAME_SHORT:
      if (!F(doc).writeKeyword(doc, XcDF_DAY_NAME_SHORT_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_NAME_LONG:
      if (!F(doc).writeKeyword(doc, XcDF_DAY_NAME_LONG_KEYWORD, -1L))
	return FALSE;
      break;
    }
    
    switch(this->month_name_size)
    {
    case Xq_NAME_VERY_SHORT:
      if (!F(doc).writeKeyword(doc, XcDF_MONTH_NAME_VERY_SHORT_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_NAME_SHORT:
      if (!F(doc).writeKeyword(doc, XcDF_MONTH_NAME_SHORT_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_NAME_LONG:
      if (!F(doc).writeKeyword(doc, XcDF_MONTH_NAME_LONG_KEYWORD, -1L))
	return FALSE;
      break;
    }
    
    if (!F(doc).writeKeyword(doc, XcDF_TIME_SEQ_KEYWORD, -1L))
      return FALSE;
    if (!F(doc).startSequence(doc)) return FALSE;
    write_all_dt_seq(this->time_seq, doc);
    if (!F(doc).endSequence(doc)) return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_DATE_SEQ_KEYWORD, -1L))
      return FALSE;
    if (!F(doc).startSequence(doc)) return FALSE;
    write_all_dt_seq(this->date_seq, doc);
    if (!F(doc).endSequence(doc)) return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_DATE_TIME_SEQ_KEYWORD, -1L))
      return FALSE;
    if (!F(doc).startSequence(doc)) return FALSE;
    write_all_dt_seq(this->date_time_seq, doc);
    if (!F(doc).endSequence(doc)) return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_GENERIC_KEYWORD, -1L))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_LOCK_KEYWORD, -1L))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_HIDE_KEYWORD, -1L))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_NAME_KEYWORD, -1L)) return FALSE;
    if (!F(doc).startSequence(doc)) return FALSE;
    if (!F(doc).write(doc, this->name, strlen(this->name))) return FALSE;
    if (!F(doc).endSequence(doc) || !F(doc).endSequence(doc)) return FALSE;
    return TRUE;
  }
  
  switch(this->type)
  {
  case Xq_PERCENT:
  case Xq_VALUE:
  case Xq_UNIT:
  case Xq_EXPONANTIAL:
    
    switch(this->type)
    {
    case Xq_PERCENT:
      if (!F(doc).writeKeyword(doc, XcDF_TYPE_PERCENT_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_VALUE:
      if (!F(doc).writeKeyword(doc, XcDF_TYPE_NORMAL_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_UNIT:
      if (!F(doc).writeKeyword(doc, XcDF_TYPE_UNIT_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_EXPONANTIAL:
      if (!F(doc).writeKeyword(doc, XcDF_TYPE_EXPONANTIAL_KEYWORD, -1L))
	return FALSE;
      break;
    }
    
    if (!F(doc).writeKeyword(doc, XcDF_MIN_PREC_KEYWORD, this->min_prec))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_MAX_PREC_KEYWORD, this->max_prec))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_COMMA_KEYWORD, this->comma_sep))
      return FALSE;
    
    if (this->type != Xq_EXPONANTIAL)
    {
      if (!F(doc).writeKeyword(doc, XcDF_SPACE_KEYWORD, this->space_each))
	return FALSE;
      
    }
    
    if (this->color1 != GlobColorAuto &&
	!F(doc).writeKeyword(doc, XcDF_POS_KEYWORD, this->color1->id))
      return FALSE;
    
    if (this->color2 != GlobColorAuto &&
	!F(doc).writeKeyword(doc, XcDF_NEG_KEYWORD, this->color2->id))
      return FALSE;
    
    
    if (this->type != Xq_EXPONANTIAL)
    {
      if (!F(doc).writeKeyword(doc, XcDF_CHARSEP_KEYWORD, this->char_sep))
	return FALSE;
      
      if (this->truncate &&
	  !F(doc).writeKeyword(doc, XcDF_TRUNCATE_KEYWORD, -1L)) return FALSE;
    }
    else
    {
      if (!F(doc).writeKeyword(doc, XcDF_EXP_DIGIT_KEYWORD, this->exp_digit))
	return FALSE;
      
      if (!F(doc).writeKeyword(doc, XcDF_EXP_MULTIPLE_KEYWORD,
			       this->exp_multiple)) return FALSE;
      
      if (this->exp_sign &&
	  !F(doc).writeKeyword(doc, XcDF_EXP_SIGN_KEYWORD, -1L)) return FALSE;
      
      if (!Xstrcmp(this->exp_string, ""))
      {
	if (!F(doc).writeKeyword(doc, XcDF_EXP_STRING_KEYWORD, -1L))
	  return FALSE;
	if (!F(doc).startSequence(doc)) return FALSE;
	if (!F(doc).write(doc, this->exp_string, strlen(this->exp_string)))
	  return FALSE;
	if (!F(doc).endSequence(doc)) return FALSE;
      }
    }
    
    if (this->type == Xq_UNIT || this->type == Xq_EXPONANTIAL)
    {
      if (!Xstrcmp(this->unit, ""))
      {
	if (!F(doc).writeKeyword(doc, XcDF_UNIT_NAME_KEYWORD, -1L))
	  return FALSE;
	if (!F(doc).startSequence(doc)) return FALSE;
	if (!F(doc).write(doc, this->unit, strlen(this->unit))) return FALSE;
	if (!F(doc).endSequence(doc)) return FALSE;
	if (this->unitafter &&
	    !F(doc).writeKeyword(doc, XcDF_UNIT_AFTER_KEYWORD, -1L))
	  return FALSE;
      }
    }
    break;
    
  case Xq_FRACTION:
    if (!F(doc).writeKeyword(doc, XcDF_TYPE_FRACTION_KEYWORD, -1L))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_FRAC_PREC_KEYWORD, this->frac_prec))
      return FALSE;
    
    if (this->frac_dec &&
	!F(doc).writeKeyword(doc, XcDF_FRAC_DECIMAL_KEYWORD, -1L))
      return FALSE;
    break;
    
  case Xq_BOOLEAN:
    if (!F(doc).writeKeyword(doc, XcDF_TYPE_BOOLEAN_KEYWORD, -1L))
      return FALSE;
    
    if (this->color1 != GlobColorAuto &&
	!F(doc).writeKeyword(doc, XcDF_TRUE_COLOR_KEYWORD, this->color1->id))
      return FALSE;
    
    if (this->color2 != GlobColorAuto &&
	!F(doc).writeKeyword(doc, XcDF_FALSE_COLOR_KEYWORD, this->color2->id))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_TRUE_NAME_KEYWORD, -1L)) return FALSE;
    if (!F(doc).startSequence(doc)) return FALSE;
    if (!F(doc).write(doc, this->true_name, strlen(this->true_name)))
      return FALSE;
    if (!F(doc).endSequence(doc)) return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_FALSE_NAME_KEYWORD, -1L)) return FALSE;
    if (!F(doc).startSequence(doc)) return FALSE;
    if (!F(doc).write(doc, this->false_name, strlen(this->false_name)))
      return FALSE;
    if (!F(doc).endSequence(doc)) return FALSE;
    
    break;
    
  case Xq_DAY_IN_WEEK:
    if (!F(doc).writeKeyword(doc, XcDF_TYPE_DAY_KEYWORD, -1L))
      return FALSE;
    
    switch(this->day_name_size)
    {
    case Xq_NAME_VERY_SHORT:
      if (!F(doc).writeKeyword(doc, XcDF_DAY_NAME_VERY_SHORT_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_NAME_SHORT:
      if (!F(doc).writeKeyword(doc, XcDF_DAY_NAME_SHORT_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_NAME_LONG:
      if (!F(doc).writeKeyword(doc, XcDF_DAY_NAME_LONG_KEYWORD, -1L))
	return FALSE;
      break;
    }
    break;
    
  case Xq_MONTH:
    if (!F(doc).writeKeyword(doc, XcDF_TYPE_MONTH_KEYWORD, -1L))
      return FALSE;
    
    switch(this->month_name_size)
    {
    case Xq_NAME_VERY_SHORT:
      if (!F(doc).writeKeyword(doc, XcDF_MONTH_NAME_VERY_SHORT_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_NAME_SHORT:
      if (!F(doc).writeKeyword(doc, XcDF_MONTH_NAME_SHORT_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_NAME_LONG:
      if (!F(doc).writeKeyword(doc, XcDF_MONTH_NAME_LONG_KEYWORD, -1L))
	return FALSE;
      break;
    }
    break;
    
  case Xq_TIME:
    if (!F(doc).writeKeyword(doc, XcDF_TYPE_TIME_KEYWORD, -1L))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_TIME_SEQ_KEYWORD, -1L))
      return FALSE;
    if (!F(doc).startSequence(doc)) return FALSE;
    write_all_dt_seq(this->time_seq, doc);
    if (!F(doc).endSequence(doc)) return FALSE;
    break;
    
  case Xq_DATE:
    if (!F(doc).writeKeyword(doc, XcDF_TYPE_DATE_KEYWORD, -1L))
      return FALSE;

    if (!F(doc).writeKeyword(doc, XcDF_DATE_SEQ_KEYWORD, -1L))
      return FALSE;
    if (!F(doc).startSequence(doc)) return FALSE;
    write_all_dt_seq(this->date_seq, doc);
    if (!F(doc).endSequence(doc)) return FALSE;
    break;
    
  case Xq_DATE_TIME:
    if (!F(doc).writeKeyword(doc, XcDF_TYPE_DATE_TIME_KEYWORD, -1L))
      return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_DATE_TIME_SEQ_KEYWORD, -1L))
      return FALSE;
    if (!F(doc).startSequence(doc)) return FALSE;
    write_all_dt_seq(this->date_time_seq, doc);
    if (!F(doc).endSequence(doc)) return FALSE;
    break;
    
  }
  
  i = 0;
  while(i < 15)
    {
      if (this->other_format[i] && 
	  this->other_format[i]->special != XcFt_GENERIC)
	break;
      i++;
    }
  if (i < 15)
    {
      if (!F(doc).writeKeyword(doc, XcDF_OTHER_FORMATS_KEYWORD, -1L))
	return FALSE;
      if (!F(doc).startSequence(doc)) return FALSE; 
      i = 0;
      while(i < 15)
	{
	  if (this->other_format[i] && 
	      this->other_format[i]->special != XcFt_GENERIC)
	    {
	      if (!F(doc).writeKeyword(doc, XcDF_FORMAT_KEYWORD,
				       this->other_format[i]->id))
		return FALSE;
	    }
	  i++;
	}
      if (!F(doc).endSequence(doc)) return FALSE;
    }
    
  if (this->special == XcFt_STANDARD &&
      !F(doc).writeKeyword(doc, XcDF_STANDARD_KEYWORD, -1L))
    return FALSE;
  
  if (this->special == XcFt_PERCENT &&
      !F(doc).writeKeyword(doc, XcDF_PERCENT_KEYWORD, -1L))
    return FALSE;
  
  if (this->special == XcFt_CURRENCY &&
      !F(doc).writeKeyword(doc, XcDF_CURRENCY_KEYWORD, -1L))
    return FALSE;
  
  if (this->special == XcFt_THOUSANDS &&
      !F(doc).writeKeyword(doc, XcDF_THOUSANDS_KEYWORD, -1L))
    return FALSE;
  
  if (this->special == XcFt_SCIENTIFIC &&
      !F(doc).writeKeyword(doc, XcDF_SCIENTIFIC_KEYWORD, -1L))
    return FALSE;
  
  if (this->special == XcFt_ENGINEER &&
      !F(doc).writeKeyword(doc, XcDF_ENGINEER_KEYWORD, -1L))
    return FALSE;
  
  if ((this->lock_flag || this->special != XcFt_NORMAL) &&
      !F(doc).writeKeyword(doc, XcDF_LOCK_KEYWORD, -1L))
    return FALSE;
  
  if (this->hide_flag &&
      !F(doc).writeKeyword(doc, XcDF_HIDE_KEYWORD, -1L))
    return FALSE;
  
  if (!F(doc).writeKeyword(doc, XcDF_NAME_KEYWORD, -1L)) return FALSE;
  if (!F(doc).startSequence(doc)) return FALSE;
  if (!F(doc).write(doc, this->name, strlen(this->name))) return FALSE;
  if (!F(doc).endSequence(doc) || !F(doc).endSequence(doc)) return FALSE;
  return TRUE;
}

