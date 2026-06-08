/*
** FormatNbr_Excel.h for XQuad in FormatNbr/
** Object and methods defintion for class FormatNbr
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
** Started on  Fri Dec 16 15:22:18 1994 Antoine Buat
** Last update Mon Dec  2 14:40:36 1996 Emmanuel Paris
*/

#ifndef _FormatNbr_Excel_h
#define _FormatNbr_Excel_h

typedef struct
{
  char	*string;
  void	(*xlf_function) ___PROTO((c_FormatNbr *format, char *string));
} excel_format_t;

extern void xlf_general ___PROTO((c_FormatNbr *format, char *string));

extern void xlf_0decimal ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_2decimal ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_0decimalm ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_2decimalm ___PROTO((c_FormatNbr *format, char *string));

extern void xlf_0currencyw ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_0currencywc ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_2currencyw ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_2currencywc ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_0currencyd ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_0currencycd ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_2currencyd ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_2currencycd ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_0currencyf ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_0currencycf ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_2currencyf ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_2currencycf ___PROTO((c_FormatNbr *format, char *string));

extern void xlf_exponantial1 ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_exponantial2 ___PROTO((c_FormatNbr *format, char *string));

extern void xlf_dateddmmyy ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_dateddmmyyyy ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_dateddmmmyy ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_dateddmmm ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_datemmmyy ___PROTO((c_FormatNbr *format, char *string));

extern void xlf_timehmmAMPM ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_timehmmssAMPM ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_timehhmm ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_timehhmmss ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_timemmss ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_timemmss0 ___PROTO((c_FormatNbr *format, char *string));

extern void xlf_datetime1 ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_datetime2 ___PROTO((c_FormatNbr *format, char *string));

extern void xlf_1fraction ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_2fraction ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_3fraction ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_1fractiond ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_2fractiond ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_3fractiond ___PROTO((c_FormatNbr *format, char *string));

extern void xlf_0percent ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_1percent ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_2percent ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_3percent ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_4percent ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_5percent ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_6percent ___PROTO((c_FormatNbr *format, char *string));
extern void xlf_7percent ___PROTO((c_FormatNbr *format, char *string));

extern void xlf_unknow ___PROTO((c_FormatNbr *format, char *string));

#define NB_EXCEL_FORMAT 52

static excel_format_t excel_format[NB_EXCEL_FORMAT] = {
  { "General", xlf_general },
  { "0", xlf_0decimal },
  { "0.00", xlf_2decimal },
  { "#,##0", xlf_0decimalm },
  { "#,##0.00", xlf_2decimalm},
  { "#,##0\\ _F;\\-#,##0\\ _F", xlf_0currencyw },
  { "#,##0\\ _F;[Red]\\-#,##0\\ _F", xlf_0currencywc },
  { "#,##0.00\\ _F;\\-#,##0.00\\ _F", xlf_2currencyw },
  { "#,##0.00\\ _F;[Red]\\-#,##0.00\\ _F", xlf_2currencywc },
  { "#,##0\\ \"$\";\\-#,##0\\ \"$\"", xlf_0currencyd },
  { "#,##0\\ \"$\";[Red]\\-#,##0\\ \"$\"", xlf_0currencycd },
  { "#,##0.00\\ \"$\";\\-#,##0.00\\ \"$\"", xlf_2currencyd },
  { "#,##0.00\\ \"$\";[Red]\\-#,##0.00\\ \"$\"", xlf_2currencycd },
  { "#,##0\\ \"F\";\\-#,##0\\ \"F\"", xlf_0currencyf },
  { "#,##0\\ \"F\";[Red]\\-#,##0\\ \"F\"", xlf_0currencycf },
  { "#,##0.00\\ \"F\";\\-#,##0.00\\ \"F\"", xlf_2currencyf },
  { "#,##0.00\\ \"F\";[Red]\\-#,##0.00\\ \"F\"", xlf_2currencycf },
  { "0%", xlf_0percent },
  { "0.00%", xlf_2percent },
  { "0.00E+00", xlf_exponantial1 },
  { "##0.0E+0", xlf_exponantial2 },
  { "#\" \"?/?", xlf_1fractiond },
  { "#\" \"??/??", xlf_2fractiond },
  { "dd/mm/yy", xlf_dateddmmyy },
  { "dd/mm/yyyy", xlf_dateddmmyyyy },
  { "dd\\-mmm\\-yy", xlf_dateddmmmyy },
  { "dd\\-mmm", xlf_dateddmmm },
  { "mmm\\-yy", xlf_datemmmyy },
  { "h:mm\\ AM/PM", xlf_timehmmAMPM },
  { "h:mm:ss\\ AM/PM", xlf_timehmmssAMPM },
  { "hh:mm", xlf_timehhmm },
  { "hh:mm:ss", xlf_timehhmmss },
  { "dd/mm/yy\\ h:mm", xlf_datetime1 },
  { "dd/mm/yyyy\\ hh:mm", xlf_datetime2 },
  { "mm:ss", xlf_timemmss },
  { "mm:ss.0", xlf_timemmss0 },
  { "@", xlf_unknow },
  { "[h]:mm:ss", xlf_timehhmmss },
  { "_-* #,##0\\ \"$\"_-;\\-* #,##0\\ \"$\"_-;_-* \"-\"\\ \"$\"_-;_-@_-", xlf_unknow },
  { "_-* #,##0\\ _F_-;\\-* #,##0\\ _F_-;_-* \"-\"\\ _F_-;_-@_-", xlf_unknow },
  { "_-* #,##0.00\\ \"$\"_-;\\-* #,##0.00\\ \"$\"_-;_-* \"-\"??\\ \"$\"_-;_-@_-", xlf_unknow },
  { "_-* #,##0.00\\ _F_-;\\-* #,##0.00\\ _F_-;_-* \"-\"??\\ _F_-;_-@_-", xlf_unknow },
  { "0.0%", xlf_1percent },
  { "0.000%", xlf_3percent },
  { "0.0000%", xlf_4percent },
  { "0.00000%", xlf_5percent },
  { "0.000000%", xlf_6percent },
  { "0.0000000%", xlf_7percent },
  { "#\" \"???/???", xlf_3fractiond },
  { "?/?", xlf_1fraction },
  { "??/??", xlf_2fraction },
  { "???/???", xlf_3fraction }
};

#endif














