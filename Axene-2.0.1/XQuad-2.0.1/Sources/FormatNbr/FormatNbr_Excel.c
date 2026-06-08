/*
** FormatNbr_Excel.c for XQuad in FormatNbr/
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
** Last update Mon Dec  2 14:15:44 1996 Emmanuel Paris
*/

#include "FormatNbr.h"
#include "Moteur.h"

/* ---- STANDARD ----- */
void xlf_general(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->truncate = TRUE;
  Xc_TRACE(("General format"));
}

/* ----- DECIMAL ------ */
void xlf_0decimal(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->max_prec = format->min_prec = 0;
}

void xlf_2decimal(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->max_prec = format->min_prec = 2;
}

void xlf_0decimalm(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->space_each = 3;
  format->max_prec = format->min_prec = 0;
}

void xlf_2decimalm(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->space_each = 3;
  format->max_prec = format->min_prec = 2;
}

/* ------ CURRENCY -------- */
void xlf_0currencyw(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  strcpy(format->unit, "  ");
  format->unitafter = TRUE;
  format->space_each = 3;
  format->max_prec = format->min_prec = 0;
  format->type = Xq_UNIT;
}

void xlf_0currencywc(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  c_Color *color;
  
  strcpy(format->unit, "  ");
  format->unitafter = TRUE;
  format->space_each = 3;
  format->max_prec = format->min_prec = 0;
  if (format->BaseStd)
  {
    color = F(format->BaseStd->color_base).getColor
      (&(format->BaseStd->color_base), XcC_NAME, "Xl_Red", XcC_RGB,
       XcC_SCALE_COLOR(1.0), XcC_SCALE_COLOR(0.0), XcC_SCALE_COLOR(0.0),
       XcC_END);
    F(format).set(format, XcF_COLORN, color, XcF_END);
  }
  format->type = Xq_UNIT;
}

void xlf_2currencyw(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  strcpy(format->unit, "  ");
  format->unitafter = TRUE;
  format->space_each = 3;
  format->max_prec = format->min_prec = 2;
  format->type = Xq_UNIT;
}

void xlf_2currencywc(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  c_Color *color;
  
  strcpy(format->unit, "  ");
  format->unitafter = TRUE;
  format->space_each = 3;
  format->max_prec = format->min_prec = 2;
  if (format->BaseStd)
  {
    color = F(format->BaseStd->color_base).getColor
      (&(format->BaseStd->color_base), XcC_NAME, "Xl_Red", XcC_RGB,
       XcC_SCALE_COLOR(1.0), XcC_SCALE_COLOR(0.0), XcC_SCALE_COLOR(0.0),
       XcC_END);
    F(format).set(format, XcF_COLORN, color, XcF_END);
  }
  format->type = Xq_UNIT;
}

void xlf_0currencyd(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->unitafter = TRUE;
  strcpy(format->unit, " $");
  format->space_each = 3;
  format->max_prec = format->min_prec = 0;
  format->type = Xq_UNIT;
}

void xlf_0currencycd(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  c_Color *color;
  
  format->unitafter = TRUE;
  strcpy(format->unit, " $");
  format->space_each = 3;
  format->max_prec = format->min_prec = 0;
  if (format->BaseStd)
  {
    color = F(format->BaseStd->color_base).getColor
      (&(format->BaseStd->color_base), XcC_NAME, "Xl_Red", XcC_RGB,
       XcC_SCALE_COLOR(1.0), XcC_SCALE_COLOR(0.0), XcC_SCALE_COLOR(0.0),
       XcC_END);
    F(format).set(format, XcF_COLORN, color, XcF_END);
  }
  format->type = Xq_UNIT;
}

void xlf_2currencyd(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->unitafter = TRUE;
  strcpy(format->unit, " $");
  format->space_each = 3;
  format->max_prec = format->min_prec = 2;
  format->type = Xq_UNIT;
}

void xlf_2currencycd(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  c_Color *color;
  
  format->unitafter = TRUE;
  strcpy(format->unit, " $");
  format->space_each = 3;
  format->max_prec = format->min_prec = 2;
  if (format->BaseStd)
  {
    color = F(format->BaseStd->color_base).getColor
      (&(format->BaseStd->color_base), XcC_NAME, "Xl_Red", XcC_RGB,
       XcC_SCALE_COLOR(1.0), XcC_SCALE_COLOR(0.0), XcC_SCALE_COLOR(0.0),
       XcC_END);
    F(format).set(format, XcF_COLORN, color, XcF_END);
  }
  format->type = Xq_UNIT;
}

void xlf_0currencyf(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->unitafter = TRUE;
  strcpy(format->unit, " F");
  format->space_each = 3;
  format->max_prec = format->min_prec = 0;
  format->type = Xq_UNIT;
}

void xlf_0currencycf(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  c_Color *color;
  
  format->unitafter = TRUE;
  strcpy(format->unit, " F");
  format->space_each = 3;
  format->max_prec = format->min_prec = 0;
  if (format->BaseStd)
  {
    color = F(format->BaseStd->color_base).getColor
      (&(format->BaseStd->color_base), XcC_NAME, "Xl_Red", XcC_RGB,
       XcC_SCALE_COLOR(1.0), XcC_SCALE_COLOR(0.0), XcC_SCALE_COLOR(0.0),
       XcC_END);
    F(format).set(format, XcF_COLORN, color, XcF_END);
  }
  format->type = Xq_UNIT;
}

void xlf_2currencyf(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->unitafter = TRUE;
  strcpy(format->unit, " F");
  format->space_each = 3;
  format->max_prec = format->min_prec = 2;
  format->type = Xq_UNIT;
}

void xlf_2currencycf(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  c_Color *color;
  
  format->unitafter = TRUE;
  strcpy(format->unit, " F");
  format->space_each = 3;
  format->max_prec = format->min_prec = 2;
  if (format->BaseStd)
  {
    color = F(format->BaseStd->color_base).getColor
      (&(format->BaseStd->color_base), XcC_NAME, "Xl_Red", XcC_RGB,
       XcC_SCALE_COLOR(1.0), XcC_SCALE_COLOR(0.0), XcC_SCALE_COLOR(0.0),
       XcC_END);
    F(format).set(format, XcF_COLORN, color, XcF_END);
  }
  format->type = Xq_UNIT;
}

/* ------ EXPONANTIAL ---------*/
void xlf_exponantial1(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->min_prec = format->max_prec = 2;
  format->exp_digit = 2;
  format->exp_multiple = 1;
  format->exp_sign = TRUE;
  format->type = Xq_EXPONANTIAL;
}

void xlf_exponantial2(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->min_prec = format->max_prec = 1;
  format->exp_digit = 1;
  format->exp_multiple = 3;
  format->exp_sign = TRUE;
  format->type = Xq_EXPONANTIAL;
}

/* ------ DATE ---------*/
void xlf_dateddmmyy(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_DATE_SEQ, Xq_DT_DAY, Xq_PADDING_SPACE, 
		XcF_ADD_DATE_SEQ, Xq_DT_SEPARATOR, "/",
		XcF_ADD_DATE_SEQ, Xq_DT_MONTH, Xq_PADDING_ZERO, 
		XcF_ADD_DATE_SEQ, Xq_DT_SEPARATOR, "/",
		XcF_ADD_DATE_SEQ, Xq_DT_YEAR_SHORT, Xq_PADDING_ZERO, 
		XcF_END);
  format->type = Xq_DATE;
}

void xlf_dateddmmyyyy(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_DATE_SEQ, Xq_DT_DAY, Xq_PADDING_SPACE, 
		XcF_ADD_DATE_SEQ, Xq_DT_SEPARATOR, "/",
		XcF_ADD_DATE_SEQ, Xq_DT_MONTH, Xq_PADDING_ZERO, 
		XcF_ADD_DATE_SEQ, Xq_DT_SEPARATOR, "/",
		XcF_ADD_DATE_SEQ, Xq_DT_YEAR, Xq_PADDING_NONE, 
		XcF_END);
  format->type = Xq_DATE;
}

void xlf_dateddmmmyy(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_DATE_SEQ, Xq_DT_DAY, Xq_PADDING_SPACE, 
		XcF_ADD_DATE_SEQ, Xq_DT_SEPARATOR, "-",
		XcF_ADD_DATE_SEQ, Xq_DT_MONTH_NAME, Xq_NAME_SHORT, 
		XcF_ADD_DATE_SEQ, Xq_DT_SEPARATOR, "-",
		XcF_ADD_DATE_SEQ, Xq_DT_YEAR_SHORT, Xq_PADDING_ZERO, 
		XcF_END);
  format->type = Xq_DATE;
}

void xlf_dateddmmm(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_DATE_SEQ, Xq_DT_DAY, Xq_PADDING_SPACE, 
		XcF_ADD_DATE_SEQ, Xq_DT_SEPARATOR, "-",
		XcF_ADD_DATE_SEQ, Xq_DT_MONTH_NAME, Xq_NAME_SHORT, 
		XcF_END);
  format->type = Xq_DATE;
}

void xlf_datemmmyy(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_DATE_SEQ, Xq_DT_MONTH_NAME, Xq_NAME_SHORT, 
		XcF_ADD_DATE_SEQ, Xq_DT_SEPARATOR, "-",
		XcF_ADD_DATE_SEQ, Xq_DT_YEAR_SHORT, Xq_PADDING_ZERO, 
		XcF_END);
  format->type = Xq_DATE;
}

/* ------ TIME ------- */
void xlf_timehmmAMPM(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_TIME_SEQ, Xq_DT_HOUR12, Xq_PADDING_SPACE, 
		XcF_ADD_TIME_SEQ, Xq_DT_SEPARATOR, ":",
		XcF_ADD_TIME_SEQ, Xq_DT_MINUTE, Xq_PADDING_ZERO, 
		XcF_ADD_TIME_SEQ, Xq_DT_SEPARATOR, " ",
		XcF_ADD_TIME_SEQ, Xq_DT_HOUR_AM_PM,
		XcF_END);
  format->type = Xq_TIME;
}

void xlf_timehmmssAMPM(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_TIME_SEQ, Xq_DT_HOUR12, Xq_PADDING_SPACE, 
		XcF_ADD_TIME_SEQ, Xq_DT_SEPARATOR, ":",
		XcF_ADD_TIME_SEQ, Xq_DT_MINUTE, Xq_PADDING_ZERO, 
		XcF_ADD_TIME_SEQ, Xq_DT_SEPARATOR, ":",
		XcF_ADD_TIME_SEQ, Xq_DT_SECOND, Xq_PADDING_ZERO, 
		XcF_ADD_TIME_SEQ, Xq_DT_SEPARATOR, " ",
		XcF_ADD_TIME_SEQ, Xq_DT_HOUR_AM_PM,
		XcF_END);
  format->type = Xq_TIME;
}

void xlf_timehhmm(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_TIME_SEQ, Xq_DT_HOUR, Xq_PADDING_SPACE, 
		XcF_ADD_TIME_SEQ, Xq_DT_SEPARATOR, ":",
		XcF_ADD_TIME_SEQ, Xq_DT_MINUTE, Xq_PADDING_ZERO, 
		XcF_END);
  format->type = Xq_TIME;
}

void xlf_timehhmmss(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_TIME_SEQ, Xq_DT_HOUR, Xq_PADDING_SPACE, 
		XcF_ADD_TIME_SEQ, Xq_DT_SEPARATOR, ":",
		XcF_ADD_TIME_SEQ, Xq_DT_MINUTE, Xq_PADDING_ZERO, 
		XcF_ADD_TIME_SEQ, Xq_DT_SEPARATOR, ":",
		XcF_ADD_TIME_SEQ, Xq_DT_SECOND, Xq_PADDING_ZERO, 
		XcF_END);
  format->type = Xq_TIME;
}

void xlf_timemmss(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_TIME_SEQ, Xq_DT_MINUTE, Xq_PADDING_ZERO, 
		XcF_ADD_TIME_SEQ, Xq_DT_SEPARATOR, ":",
		XcF_ADD_TIME_SEQ, Xq_DT_SECOND, Xq_PADDING_ZERO, 
		XcF_END);
  format->type = Xq_TIME;
}

void xlf_timemmss0(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_TIME_SEQ, Xq_DT_MINUTE, Xq_PADDING_ZERO, 
		XcF_ADD_TIME_SEQ, Xq_DT_SEPARATOR, ":",
		XcF_ADD_TIME_SEQ, Xq_DT_SECOND, Xq_PADDING_ZERO, 
		XcF_ADD_TIME_SEQ, Xq_DT_SEPARATOR, ",",
		XcF_ADD_TIME_SEQ, Xq_DT_TENTH_OF_SECOND,
		XcF_END);
  format->type = Xq_TIME;
}

/* ------- DATE AND TIME ------- */
void xlf_datetime1(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_DAY, Xq_PADDING_SPACE,
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_SEPARATOR, "/",
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_MONTH, Xq_PADDING_ZERO,
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_SEPARATOR, "/",
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_YEAR_SHORT, Xq_PADDING_ZERO,
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_SEPARATOR, " ",
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_HOUR, Xq_PADDING_SPACE, 
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_SEPARATOR, ":",
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_MINUTE, Xq_PADDING_ZERO, 
		XcF_END);
  format->type = Xq_DATE_TIME;
}

void xlf_datetime2(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  F(format).set(format,
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_DAY, Xq_PADDING_SPACE,
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_SEPARATOR, "/",
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_MONTH, Xq_PADDING_ZERO,
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_SEPARATOR, "/",
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_YEAR, Xq_PADDING_NONE,
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_SEPARATOR, " ",
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_HOUR, Xq_PADDING_SPACE, 
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_SEPARATOR, ":",
		XcF_ADD_DATE_TIME_SEQ, Xq_DT_MINUTE, Xq_PADDING_ZERO, 
		XcF_END);
  format->type = Xq_DATE_TIME;
}

/* ------ FRACTION -------- */
void xlf_1fraction(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->frac_dec = FALSE;
  format->frac_prec = 1;
  format->type = Xq_FRACTION;
}

void xlf_2fraction(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->frac_dec = FALSE;
  format->frac_prec = 2;
  format->type = Xq_FRACTION;
}

void xlf_3fraction(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->frac_dec = FALSE;
  format->frac_prec = 3;
  format->type = Xq_FRACTION;
}

void xlf_1fractiond(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->frac_dec = TRUE;
  format->frac_prec = 1;
  format->type = Xq_FRACTION;
}

void xlf_2fractiond(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->frac_dec = TRUE;
  format->frac_prec = 2;
  format->type = Xq_FRACTION;
}

void xlf_3fractiond(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->frac_dec = TRUE;
  format->frac_prec = 3;
  format->type = Xq_FRACTION;
}

/* ------ PERCENT ------- */
void xlf_0percent(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->max_prec = format->min_prec = 0;
  format->type = Xq_PERCENT;
}

void xlf_1percent(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->max_prec = format->min_prec = 1;
  format->type = Xq_PERCENT;
}

void xlf_2percent(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->max_prec = format->min_prec = 2;
  format->type = Xq_PERCENT;
}

void xlf_3percent(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->max_prec = format->min_prec = 2;
  format->type = Xq_PERCENT;
}

void xlf_4percent(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->max_prec = format->min_prec = 2;
  format->type = Xq_PERCENT;
}

void xlf_5percent(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->max_prec = format->min_prec = 2;
  format->type = Xq_PERCENT;
}

void xlf_6percent(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->max_prec = format->min_prec = 2;
  format->type = Xq_PERCENT;
}

void xlf_7percent(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->max_prec = format->min_prec = 2;
  format->type = Xq_PERCENT;
}

/* ------- UNKNOW -------- */
void xlf_unknow(format, ptr)
c_FormatNbr *format;
char *ptr;
{
  format->truncate = TRUE;
  format->type = Xq_VALUE;
  Xc_TRACE(("xl format: %s unknow, set standard instead", ptr));
}






