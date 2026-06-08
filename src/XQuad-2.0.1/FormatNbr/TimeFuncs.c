/*
** TimeFuncs.c for XQuad in FormatNbr/
** Object and methods defintion for class FormatNbr
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
** Started on  Thr Dec  7 18:58:18 1995 Emmanuel Paris
** Last update Sun Dec  1 18:47:09 1996 Emmanuel Paris
*/

#define _TimeFuncs_Private_
#include "TimeFuncs.h"
#include <time.h>

xc_date_time_t *fill_xc_date_and_time();
xc_date_t *fill_xc_date();
xc_time_t *fill_xc_time();
xc_date_time_t *convert_to_date_and_time();
xc_date_t *convert_to_date();
xc_time_t *convert_to_time();
double convert_to_xc_date_and_time();
double convert_to_xc_date();
double convert_to_xc_time();
     
static int calcul_year();

xc_date_time_t *fill_xc_date_and_time(ret_value)
xc_date_time_t *ret_value;
{
  if (ret_value == NULL)
    ret_value = (xc_date_time_t *)Xc_malloc("date&time",
					    sizeof(xc_date_time_t));
  fill_xc_date(&ret_value->date);
  fill_xc_time(&ret_value->time);

  return ret_value;
}

xc_date_t *fill_xc_date(ret_value)
xc_date_t *ret_value;
{
  time_t	current_time;
  struct tm	*time_detail;

  if (ret_value == NULL)
    ret_value = (xc_date_t *)Xc_malloc("date&time", sizeof(xc_date_t));

  current_time = time(NULL);
  time_detail = localtime(&current_time);

  ret_value->year = 1900 + time_detail->tm_year;
  ret_value->after_jc = TRUE;
  ret_value->month = time_detail->tm_mon + 1;
  /* ret_value->week = ? */
  ret_value->day = time_detail->tm_mday;
  if (time_detail->tm_wday == 0)
    ret_value->day_in_week = 7;
  else
    ret_value->day_in_week = time_detail->tm_wday;
  ret_value->day_in_year = time_detail->tm_yday + 1;

  return ret_value;
}

xc_time_t *fill_xc_time(ret_value)
xc_time_t *ret_value;
{
  time_t	current_time;
  struct tm	*time_detail;

  if (ret_value == NULL)
    ret_value = (xc_time_t *)Xc_malloc("date&time", sizeof(xc_time_t));
  
  current_time = time(NULL);
  time_detail = localtime(&current_time);
  
  ret_value->hour = time_detail->tm_hour;
  ret_value->hour12 = time_detail->tm_hour / 2;
  if (ret_value->hour12 == 0)
    ret_value->hour12 = 12;
  ret_value->am_pm = (time_detail->tm_hour >= 12);
  ret_value->minute = time_detail->tm_min;
  ret_value->second = time_detail->tm_sec;
  ret_value->millisecond = 0;
  
  return ret_value;
}

xc_date_time_t *convert_to_date_and_time(value, ret_value)
double value;
xc_date_time_t *ret_value;
{
  double int_value, dec_value;
  
  if (ret_value == NULL)
    ret_value = (xc_date_time_t *)Xc_malloc("date&time",
					    sizeof(xc_date_time_t));
  
  int_value = floor(value);
  convert_to_date(int_value, &ret_value->date);
  
  dec_value = value - int_value;
  convert_to_time(dec_value, &ret_value->time);
  
  return ret_value;
}

xc_date_t *convert_to_date(value, ret_value)
double value;
xc_date_t *ret_value;
{
  long val, sval, wval;
  unsigned short *month;
  int		i, j, y;
  
  if (ret_value == NULL)
    ret_value = (xc_date_t *)Xc_malloc("date&time", sizeof(xc_date_t));
  
  if (value >= 0.0)
  {
    if (value > 2147483647.0)
      val = sval = 2147483647L;
    else
      val = sval = (long)rint(floor(value));
  }
  else
  {
    if (value < -2147483647.0)
      val = sval = -2147483647L;
    else
      val = sval = (long)rint(floor(value));
  }    
  
  if (val >= 0)
  {
    ret_value->year = calcul_year(&val) + Xc_YEAR_ZERO;
    wval = sval - val;
    
    ret_value->after_jc = TRUE;    
    
    if (ret_value->year % 4 == 0)
    {
      if (ret_value->year % 100 == 0 && ret_value->year % 400 != 0)
	month = non_bissextile_year;
      else
	month = bissextile_year;
    }
    else
      month = non_bissextile_year;
  }
  else
  {
    if (val < -90323) /* The Gregorian Reformation ! 2 -> 14 sept 1752 */
      if (val > -2147483630) /* to avoid fpu execption */
	val -= 11;
    
    if (val <= -109208) /* 31th december 1700 */
    {
      if (val <= -730487) /* 31th december -2 */
	val--;
      
      val = abs(val + 109208);
      
      y = (val / 1461) << 2;
      val = val % 1461;
      
      if (val >= 1096) { val -= 1096; y += 3; }
      else
      {
	if (val >= 731) { val -= 731; y += 2; }
	else
	{
	  if (val >= 366) { val -= 366; y += 1; }
	}
      }
      
      ret_value->year = 1700 - y;
      wval = sval + val;
      val = 364 - val;
      
      if (ret_value->year <= 0)
      {
	ret_value->year -= 1;
	ret_value->after_jc = FALSE;
      }
      else
	ret_value->after_jc = TRUE;
      
      if (ret_value->year % 4 == 0)
      {
	month = bissextile_year;
	val ++;
      }
      else
	month = non_bissextile_year;
    }
    else
    {
      val = ABS(val) + 365;
      
      ret_value->year = Xc_YEAR_ZERO - calcul_year(&val);
      wval = sval + val;
      val = 364 - val;
            
      if (ret_value->year % 4 == 0)
      {
	if (ret_value->year % 100 == 0 && ret_value->year % 400 != 0)
	  month = non_bissextile_year;
	else
	{
	  month = bissextile_year;
	  val ++;
	}
      }
      else
	month = non_bissextile_year;
      ret_value->after_jc = TRUE;
    }
  }
  
  ret_value->day_in_year = val + 1;
  i = 0; j = val; 
  for(i = 0; i < 12; i++)
  {
    if (j < month[i])
      break;
    j -= month[i];
  }
  ret_value->month = i + 1;
  ret_value->day = j + 1;
  
  i = sval % 7;
  if (i < 0) i = 7 + i;
  i = (i + Xc_FIRST_DAY) % 7;
  ret_value->day_in_week = i + 1;
  
  i = wval % 7;
  if (i < 0) i = 7 + i;
  i = (i + Xc_FIRST_DAY) % 7;
  j = (val + i ) / 7 + 1;
  if (j > 52) j -= 52;
  ret_value->week = j;
  
  return ret_value;
}

static int calcul_year(mval)
long	*mval;
{
  int	y;
  long	val;
  
  val = *mval;
  y = 0;
  if (val > 36889)
  {
    while(val >= 9350208) { val -= 9350208; y += 25600; }
    while(val >= 584388) { val -= 584388; y += 1600;  }
    while(val >= 146097) { val -= 146097; y += 400; }
    
    if (val > 36889)
    {
      if (val < 73414)
      { val -= 36524;  y += 100; }
      else
      {
	if (val < 109938)  
	{ val -= 73048; y += 200; }
	else
	{ val -= 109572; y += 300; }
      }
    }
  }
  y += (val / 1461) << 2;
  val = val % 1461;
  
  if (val >= 1096) { val -= 1096; y += 3; }
  else
  {
    if (val >= 731) { val -= 731; y += 2; }
    else
    {
      if (val >= 366) { val -= 366; y += 1; }
    }
  }
  *mval = val;
  return y;
}

xc_time_t *convert_to_time(value, ret_value)
double value;
xc_time_t *ret_value;
{
  unsigned long	val;
  
  if (ret_value == NULL)
    ret_value = (xc_time_t *)Xc_malloc("date&time", sizeof(xc_time_t));
  
  value = floor((value - floor(value)) * Xc_TIME_FRACTION);
  if (value > 4294967295.0)
    val = 424967295L;
  else
    val = (unsigned long)floor(value);
  ret_value->millisecond = val % 1000;
  val /= 1000;
  ret_value->second = val % 60;
  val /= 60;
  ret_value->minute = val % 60;
  val /= 60;
  val %= 24; /* for the case of wrong value, never > 23 */
  ret_value->hour = val;
  if (val >= 12)
  {
    ret_value->hour12 = val - 12;
    ret_value->am_pm = TRUE;
  }
  else
  {
    ret_value->hour12 = val;
    ret_value->am_pm = FALSE;
  }
  if (ret_value->hour12 == 0)
    ret_value->hour12 = 12;
  return ret_value;
}

double convert_to_xc_date_and_time(xc_value, valid)
xc_date_time_t	*xc_value;
boolean		*valid;
{
  boolean	is_valid;
  double	date_val, time_val, result;
  
  date_val = convert_to_xc_date(&(xc_value->date), &is_valid);

  if (valid)  *valid = is_valid;  
  if (is_valid == FALSE)
    return 0.0;
  
  time_val = convert_to_xc_time(&(xc_value->time), &is_valid);
  
  if (valid) *valid = is_valid;
  if (is_valid == FALSE)
    return 0.0;
  
  result = date_val + time_val;
  return result;
}

double convert_to_xc_date(xc_value, valid)
xc_date_t	*xc_value;
boolean		*valid;
{
  long result;
  int nb400y, nb100y, nb4y, nb, nb_bis, i, j;
  unsigned short *month;
  
  if (xc_value->year == 0 ||
      xc_value->year >= 5881610 || xc_value->year <= -5877491 || 
      xc_value->month <= 0 || xc_value->month > 12 ||
      xc_value->day <= 0 || xc_value->day > 31)
  {
    if (valid) *valid = FALSE;
    return 0.0;
  }
  if (valid) *valid = TRUE;
  
  if (xc_value->year >= Xc_YEAR_ZERO)
  {
    if (xc_value->year == Xc_YEAR_ZERO)
    {
      result = 0;
      month = bissextile_year;
    }
    else
    {
      nb = xc_value->year - Xc_YEAR_ZERO - 1;
      nb400y = nb / 400 + 1;
      nb100y = nb / 100;
      nb4y = nb / 4;
      nb_bis = nb4y - nb100y + nb400y;
      result = nb_bis * 366 + (nb + 1 - nb_bis) * 365;
      
      if (xc_value->year % 4 == 0)
      {
	if (xc_value->year % 100 == 0 && xc_value->year % 400 != 0)
	  month = non_bissextile_year;
	else
	  month = bissextile_year;
      }
      else
	month = non_bissextile_year;
    }
  }
  else
  {
    if (xc_value->year > 1700)
    {
      nb = Xc_YEAR_ZERO - xc_value->year;
      nb100y = nb / 100;
      nb4y = nb / 4;
      nb_bis = nb4y - nb100y;
      result = -(nb_bis * 366 + (nb - nb_bis) * 365);
      if (xc_value->year % 4 == 0 && xc_value->year % 100 != 0)
	month = bissextile_year;
      else
	month = non_bissextile_year;
    }
    else
    {
      nb = 1700 - xc_value->year;
      nb4y = nb / 4;
      result = -(nb4y * 366 + (nb - nb4y) * 365 + 109573);
      if (xc_value->year < 0)
	result += 366;
      if (xc_value->year % 4 == 0)
	month = bissextile_year;
      else
	month = non_bissextile_year;
    }
  }
  
  j = 0;
  for(i = 0; i < xc_value->month - 1; i++)
    j += month[i];
  
  if (xc_value->day > month[i])
  {
    if (valid) *valid = FALSE;
    return 0.0;
  }
  
  result += j + xc_value->day - 1;
  if (result < -90323)
  {
    if (result > -90335)
    {
      if (valid) *valid = FALSE;
      return 0.0;
    }
    result += 11;
  }
  
  return result;
}

double convert_to_xc_time(xc_value, valid)
xc_time_t	*xc_value;
boolean		*valid;
{
  unsigned long result;
  double	value;
  
  if (xc_value->hour > 23 || xc_value->minute > 59 ||
      xc_value->second > 59 || xc_value->millisecond > 999)
  {
    if (valid) *valid = FALSE;
    return 0.0;
  }
  if (valid) *valid = TRUE;
  result = xc_value->hour;
  result = result * 60 + xc_value->minute;
  result = result * 60 + xc_value->second;
  result = result * 1000 + xc_value->millisecond;
  
  if (result >= Xc_TIME_FRACTION2)
    result %= Xc_TIME_FRACTION2;
  
  value = result / Xc_TIME_FRACTION;
  return value;
}









