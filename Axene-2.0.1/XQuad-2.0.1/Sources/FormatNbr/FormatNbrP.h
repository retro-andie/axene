/*
** FormatNbrP.h for XQuad in FormatNbr/
** loading callbacks for FormatNbr
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
** Started on  Thu Dec 14 15:12:00 1995 Emmanuel Paris
** Last update Sun Dec  1 15:05:02 1996 Emmanuel Paris
*/

#ifndef _FormatNbrP_h
#define _FormatNbrP_h

#define XcDF_FORMAT_KEYWORD "FORMAT"
#define XcDF_FORMATLIST_KEYWORD "FORMATLIST"

/* type keywords */
#define XcDF_TYPE_NORMAL_KEYWORD "NORMAL_TYPE"
#define XcDF_TYPE_UNIT_KEYWORD "UNIT_TYPE"
#define XcDF_TYPE_PERCENT_KEYWORD "PERCENT_TYPE"
#define XcDF_TYPE_FRACTION_KEYWORD "FRACTION_TYPE"
#define XcDF_TYPE_BOOLEAN_KEYWORD "BOOLEAN_TYPE"
#define XcDF_TYPE_DAY_KEYWORD "DAY_TYPE"
#define XcDF_TYPE_MONTH_KEYWORD "MONTH_TYPE"
#define XcDF_TYPE_TIME_KEYWORD "TIME_TYPE"
#define XcDF_TYPE_DATE_KEYWORD "DATE_TYPE"
#define XcDF_TYPE_DATE_TIME_KEYWORD "DATE_AND_TIME_TYPE"
#define XcDF_TYPE_EXPONANTIAL_KEYWORD "EXPONANTIAL_TYPE"

/* for normal value format */
#define XcDF_MIN_PREC_KEYWORD "MIN_PREC"
#define XcDF_MAX_PREC_KEYWORD "MAX_PREC"
#define XcDF_SPACE_KEYWORD "SPACE"
#define XcDF_COMMA_KEYWORD "COMMA"
#define XcDF_POS_KEYWORD "COLORPOS"
#define XcDF_NEG_KEYWORD "COLORNEG"
#define XcDF_CHARSEP_KEYWORD "CHARSEP"
#define XcDF_TRUNCATE_KEYWORD "TRUNCATE_ENABLE"
#define XcDF_C_MIN_PREC_KEYWORD "MINPREC"
#define XcDF_C_MAX_PREC_KEYWORD "MAXPREC"
#define XcDF_C_COMMA_KEYWORD "COMA"
#define XcDF_C_TRUNCATE_KEYWORD "AFFTRUNC"

/* for unit format */
#define XcDF_UNIT_NAME_KEYWORD "UNIT_NAME"
#define XcDF_C_UNIT_NAME_KEYWORD "UNIT"
#define XcDF_UNIT_AFTER_KEYWORD "UNIT_AFTER"
#define XcDF_C_UNIT_AFTER_KEYWORD "UNITAFTER"

/* for exponantial format */
#define XcDF_EXP_DIGIT_KEYWORD "EXP_DIGIT"
#define XcDF_EXP_MULTIPLE_KEYWORD "EXP_MULTIPLE"
#define XcDF_EXP_SIGN_KEYWORD "EXP_SIGN"
#define XcDF_EXP_STRING_KEYWORD "EXP_STRING"

/* for fraction format */
#define XcDF_FRAC_PREC_KEYWORD "FRAC_PREC"
#define XcDF_FRAC_DECIMAL_KEYWORD "FRAC_DECIMAL"

/* for boolean format */
#define XcDF_TRUE_COLOR_KEYWORD "TRUE_COLOR"
#define XcDF_FALSE_COLOR_KEYWORD "FALSE_COLOR"
#define XcDF_TRUE_NAME_KEYWORD "TRUE_NAME"
#define XcDF_FALSE_NAME_KEYWORD "FALSE_NAME"

/* for day value format */
#define XcDF_DAY_NAME_VERY_SHORT_KEYWORD "DAY_NAME_VERY_SHORT"
#define XcDF_DAY_NAME_SHORT_KEYWORD "DAY_NAME_SHORT"
#define XcDF_DAY_NAME_LONG_KEYWORD "DAY_NAME_LONG"

/* for month value format */
#define XcDF_MONTH_NAME_VERY_SHORT_KEYWORD "MONTH_NAME_VERY_SHORT"
#define XcDF_MONTH_NAME_SHORT_KEYWORD "MONTH_NAME_SHORT"
#define XcDF_MONTH_NAME_LONG_KEYWORD "MONTH_NAME_LONG"

/* for date value format */
#define XcDF_DATE_SEQ_KEYWORD "DATE_SEQUENCE"
#define XcDF_YEAR_KEYWORD "YEAR"
#define XcDF_YEAR_SHORT_KEYWORD "YEAR_SHORT"
#define XcDF_YEAR_JC_KEYWORD "YEAR_JC_REF"
#define XcDF_MONTH_KEYWORD "MONTH"
#define XcDF_WEEK_KEYWORD "WEEK"
#define XcDF_DAY_KEYWORD "DAY"
#define XcDF_DAY_IN_YEAR_KEYWORD "DAY_IN_YEAR"

/* for time value format */
#define XcDF_TIME_SEQ_KEYWORD "TIME_SEQUENCE"
#define XcDF_HOUR_KEYWORD "HOUR"
#define XcDF_HOUR12_KEYWORD "HOUR12"
#define XcDF_HOUR_AM_PM_KEYWORD "HOUR_AM_PM"
#define XcDF_MINUTE_KEYWORD "MINUTE"
#define XcDF_SECOND_KEYWORD "SECOND"
#define XcDF_TENTH_OF_SECOND_KEYWORD "TENTH_OF_SECOND"
#define XcDF_HUNDREDTH_OF_SECOND_KEYWORD "HUNDREDTH_OF_SECOND"
#define XcDF_THOUSANDTH_OF_SECOND_KEYWORD "THOUSANDTH_OF_SECOND"

/* for time, date and date_time purpose */
#define XcDF_DATE_TIME_SEQ_KEYWORD "DATE_AND_TIME_SEQUENCE"
#define XcDF_SEPARATOR_KEYWORD "STRING"
#define XcDF_PADDING_NONE_KEYWORD "PADDING_NONE"
#define XcDF_PADDING_SPACE_KEYWORD "PADDING_SPACE"
#define XcDF_PADDING_ZERO_KEYWORD "PADDING_ZERO"

/* other_formats */
#define XcDF_OTHER_FORMATS_KEYWORD "OTHER_FORMAT"

/* special formats */
#define XcDF_GENERIC_KEYWORD "GENERIC"
#define XcDF_STANDARD_KEYWORD "STANDARD"
#define XcDF_PERCENT_KEYWORD "PERCENT"
#define XcDF_CURRENCY_KEYWORD "CURRENCY"
#define XcDF_THOUSANDS_KEYWORD "THOUSANDS"
#define XcDF_SCIENTIFIC_KEYWORD "SCIENTIFIC"
#define XcDF_ENGINEER_KEYWORD "ENGINEER"

#ifndef XcDF_LOCK_KEYWORD
#define XcDF_LOCK_KEYWORD "LOCK"
#endif
#ifndef XcDF_HIDE_KEYWORD
#define XcDF_HIDE_KEYWORD "HIDE"
#endif

/*--- Data structure for loading ---*/
typedef struct {
  c_FormatNbr **format_base;
  c_Color **color_base;
  c_FormatNbr *format;
} format_read_t;

extern boolean readFormat ___PROTO((c_DocFile *doc, char *keyword,
				   long param, format_read_t *data));
extern boolean readFormatList ___PROTO((c_DocFile *doc, char *keyword,
				       long param, format_read_t *data));
extern boolean writeFormat ___PROTO((c_FormatNbr *this, c_DocFile *doc));
extern boolean writeFormatList ___PROTO((c_FormatNbr *this, c_DocFile *doc));

#endif /* _FormatNbrP_h */



