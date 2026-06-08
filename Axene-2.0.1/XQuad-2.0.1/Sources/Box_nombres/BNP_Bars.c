/*
** BNP_Bars.c for XQuad in Box_nombres/
** Box Nombres Part that defines bars contents
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
** Last update Sat Feb  7 19:03:30 1998 Emmanuel Paris
*/

#include "BoxNombresPartStd.h"
#include "BNP_Color.h"
#include "BNP_Unit.h"
#include "BNP_Std_Prec.h"
#include "BNP_Exp_Prec.h"
#include "BNP_Frac_Prec.h"
#include "BNP_Boolean.h"
#include "BNP_Day.h"
#include "BNP_Month.h"
#include "BNP_Date.h"
#include "BNP_Time.h"
#include "BNP_Bars.h"

#ifdef ___PSEUDO_COMPOSE_MACRO
#define BNP_BAR_MACRO(type) \
static bnp_bar_t bar_##type = \
{ \
    NB_BNP_##type, 0, bnp_##type, name_##type, \
    tbutton_##type, NULL, preview_##type \
}
#else
#ifdef ___GNUC_COMPOSE_MACRO
#define BNP_BAR_MACRO(type) \
static bnp_bar_t bar_##type## = \
{ \
    NB_BNP_##type##, 0, bnp_##type##, name_##type##, \
    tbutton_##type##, NULL, preview_##type## \
}
#else
#define BNP_BAR_MACRO(type) \
static bnp_bar_t bar_/**/type/**/ = \
{ \
    NB_BNP_/**/type/**/, 0, bnp_/**/type/**/, name_/**/type/**/, \
    tbutton_/**/type/**/, NULL, preview_/**/type/**/ \
}
#endif
#endif

#define NB_BNP_normal 2
static bnp_std_t *bnp_normal[] = 
{
  &bnp_std_prec, &bnp_color,
};
static char	*name_normal[NB_BNP_normal] = 
{
  "tbStdPrec", "tbColor"
};
static c_ToggleButton *tbutton_normal[NB_BNP_normal];
static char	preview_normal[] = "-1582146.1549223";

#define NB_BNP_unit 3
static bnp_std_t *bnp_unit[] = 
{
  &bnp_unit_name, &bnp_std_prec, &bnp_color
};
static char	*name_unit[NB_BNP_unit] = 
{
  "tbUnit", "tbStdPrec", "tbColor"
};
static c_ToggleButton *tbutton_unit[NB_BNP_unit];
static char	preview_unit[] = "15643.179842";

#define NB_BNP_exponantial 3
static bnp_std_t *bnp_exponantial[] = 
{
  &bnp_exp_prec, &bnp_unit_name, &bnp_color
};
static char	*name_exponantial[NB_BNP_exponantial] = 
{
  "tbExpPrec", "tbUnit", "tbColor"
};
static c_ToggleButton *tbutton_exponantial[NB_BNP_exponantial];
static char	preview_exponantial[] = "-15643.179842E+22";

#define NB_BNP_percent 2
static bnp_std_t *bnp_percent[] = 
{
  &bnp_std_prec, &bnp_color
};
static char	*name_percent[NB_BNP_percent] = 
{
  "tbStdPrec", "tbColor"
};
static c_ToggleButton *tbutton_percent[NB_BNP_percent];
static char	preview_percent[] = "-.206";

#define NB_BNP_fraction 3
static bnp_std_t *bnp_fraction[] = 
{
  &bnp_frac_prec, &bnp_unit_name, &bnp_color
};
static char	*name_fraction[NB_BNP_fraction] = 
{
  "tbFracPrec", "tbUnit", "tbColor"
};
static c_ToggleButton *tbutton_fraction[NB_BNP_fraction];
static char	preview_fraction[] = "3.14159265358";

#define NB_BNP_boolean 2
static bnp_std_t *bnp_boolean[] = 
{
  &bnp_boolean_name, &bnp_color
};
static char	*name_boolean[NB_BNP_boolean] = 
{
  "tbBoolean", "tbColor2"
};
static c_ToggleButton *tbutton_boolean[NB_BNP_boolean];
static char	preview_boolean[] = "0";

#define NB_BNP_day_in_week 1
static bnp_std_t *bnp_day_in_week[] = 
{
  &bnp_day_name
};
static char	*name_day_in_week[NB_BNP_day_in_week] = 
{
  "tbDay"
};
static c_ToggleButton *tbutton_day_in_week[NB_BNP_day_in_week];
static char	preview_day_in_week[Xq_FORMULA_LENGTH];

#define NB_BNP_month 1
static bnp_std_t *bnp_month[] = 
{
  &bnp_month_name
};
static char	*name_month[NB_BNP_month] = 
{
  "tbMonth"
};
static c_ToggleButton *tbutton_month[NB_BNP_month];
static char	preview_month[Xq_FORMULA_LENGTH];

#define NB_BNP_time 1
static bnp_std_t *bnp_time[] = 
{
  &bnp_times
};
static char	*name_time[NB_BNP_time] = 
{
  "tbTime"
};
static c_ToggleButton *tbutton_time[NB_BNP_time];
static char	preview_time[Xq_FORMULA_LENGTH];

#define NB_BNP_date 1
static bnp_std_t *bnp_date[] = 
{
  &bnp_dates
};
static char	*name_date[NB_BNP_date] = 
{
  "tbDate"
};
static c_ToggleButton *tbutton_date[NB_BNP_date];
static char	preview_date[Xq_FORMULA_LENGTH];

#define NB_BNP_date_time 2
static bnp_std_t *bnp_date_time[] = 
{
  &bnp_dates, &bnp_times
};
static char	*name_date_time[NB_BNP_date_time] = 
{
  "tbDate", "tbTime"
};
static c_ToggleButton *tbutton_date_time[NB_BNP_date_time];
static char	preview_date_time[Xq_FORMULA_LENGTH];

BNP_BAR_MACRO(normal);
BNP_BAR_MACRO(unit);
BNP_BAR_MACRO(exponantial);
BNP_BAR_MACRO(percent);
BNP_BAR_MACRO(fraction);
BNP_BAR_MACRO(boolean);
BNP_BAR_MACRO(day_in_week);
BNP_BAR_MACRO(month);
BNP_BAR_MACRO(time);
BNP_BAR_MACRO(date);
BNP_BAR_MACRO(date_time);

bnp_bar_t *bnp_bar[NB_BNP_BAR] = 
{
  &bar_normal, &bar_unit, &bar_exponantial, &bar_percent, &bar_fraction,
  &bar_boolean, &bar_day_in_week, &bar_month, &bar_time, &bar_date,
  &bar_date_time
};




