/*
** Date_Time.c for XQuad in FormatNbr/
** Date and Time display structures
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
** Started on  Wed Feb 22 16:59:27 1995 Emmanuel Paris
** Last update Tue Mar 19 11:48:21 1996 Emmanuel Paris
*/

#include "FormatNbr.h"
#include "TimeFuncs.h"
#include "Resource.h"

extern c_Resource *GlobResources;

void insert_dt_seq ___PROTO((dt_seq_t **base, dt_seq_t *item));
void replace_dt_seq ___PROTO((dt_seq_t **base, dt_seq_t *src,
				     dt_seq_t *dst));
void remove_dt_seq ___PROTO((dt_seq_t **base, dt_seq_t *item));
void copy_all_dt_seq ___PROTO((dt_seq_t **dup_base, dt_seq_t **base));
void destroy_all_dt_seq ___PROTO((dt_seq_t **base));
boolean compare_all_dt_seq ___PROTO((dt_seq_t **base1, dt_seq_t **base2));
boolean write_all_dt_seq ___PROTO((dt_seq_t **base, c_DocFile *doc));

static dt_seq_t *new_simple ___PROTO((int seq_num));
static dt_seq_t *new_padding ___PROTO((int seq_num,
				       padding_id_t padding));
static dt_seq_t *new_name ___PROTO((int seq_num,
				    name_size_id_t name_size));
static dt_seq_t *new_separator ___PROTO((int seq_num,
					 char *separator));
static dt_seq_t *copy_simple ___PROTO((dt_seq_t *dt_seq));
static dt_seq_t *copy_padding ___PROTO((dt_seq_t *dt_seq));
static dt_seq_t *copy_name ___PROTO((dt_seq_t *dt_seq));
static dt_seq_t *copy_separator ___PROTO((dt_seq_t *dt_seq));

static void delete_simple ___PROTO((dt_seq_t *dt_seq));
static void delete_padding ___PROTO((dt_seq_t *dt_seq));
static void delete_name ___PROTO((dt_seq_t *dt_seq));
static void delete_separator ___PROTO((dt_seq_t *dt_seq));

static boolean compare_simple ___PROTO((dt_seq_t *a, dt_seq_t *b));
static boolean compare_padding ___PROTO((dt_seq_t *a, dt_seq_t *b));
static boolean compare_name ___PROTO((dt_seq_t *a, dt_seq_t *b));
static boolean compare_separator ___PROTO((dt_seq_t *a, dt_seq_t *b));

static void concate_hour ___PROTO((dt_seq_t *dt_seq, char *str,
				   xc_time_t *xc_time));
static void concate_hour12 ___PROTO((dt_seq_t *dt_seq, char *str,
				     xc_time_t *xc_time));
static void concate_hour_am_pm ___PROTO((dt_seq_t *dt_seq, char *str,
					 xc_time_t *xc_time));
static void concate_minute ___PROTO((dt_seq_t *dt_seq, char *str,
				     xc_time_t *xc_time));
static void concate_second ___PROTO((dt_seq_t *dt_seq, char *str,
				     xc_time_t *xc_time));
static void concate_dixsecond ___PROTO((dt_seq_t *dt_seq, char *str,
					xc_time_t *xc_time));
static void concate_centsecond ___PROTO((dt_seq_t *dt_seq, char *str,
					 xc_time_t *xc_time));
static void concate_millisecond ___PROTO((dt_seq_t *dt_seq, char *str,
					  xc_time_t *xc_time));
static void concate_year ___PROTO((dt_seq_t *dt_seq, char *str,
				   xc_date_t *xc_date));
static void concate_year_short ___PROTO((dt_seq_t *dt_seq, char *str,
					 xc_date_t *xc_date));
static void concate_year_jc ___PROTO((dt_seq_t *dt_seq, char *str,
				      xc_date_t *xc_date));
static void concate_month ___PROTO((dt_seq_t *dt_seq, char *str,
				    xc_date_t *xc_date));
static void concate_month_name ___PROTO((dt_seq_t *dt_seq, char *str,
					 xc_date_t *xc_date));
static void concate_week ___PROTO((dt_seq_t *dt_seq, char *str,
				   xc_date_t *xc_date));
static void concate_day ___PROTO((dt_seq_t *dt_seq, char *str,
				  xc_date_t *xc_date));
static void concate_day_name ___PROTO((dt_seq_t *dt_seq, char *str,
				       xc_date_t *xc_date));
static void concate_day_in_year ___PROTO((dt_seq_t *dt_seq, char *str,
					  xc_date_t *xc_date));
static void concate_separator ___PROTO((dt_seq_t *dt_seq, char *str));

dt_seq_funcs_t dt_seq_funcs[Xq_NB_DT] = {
  { Xq_TIME_ID, Xq_DT_SEQ_PADDING,		              /* Xq_DT_HOUR */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_hour},
  { Xq_TIME_ID, Xq_DT_SEQ_PADDING,		            /* Xq_DT_HOUR12 */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_hour12},
  { Xq_TIME_ID, Xq_DT_SEQ_SIMPLE,		        /* Xq_DT_HOUR_AM_PM */ 
      new_simple, copy_simple, delete_simple,
      compare_simple, concate_hour_am_pm},
  { Xq_TIME_ID, Xq_DT_SEQ_PADDING,		            /* Xq_DT_MINUTE */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_minute},
  { Xq_TIME_ID, Xq_DT_SEQ_PADDING,			    /* Xq_DT_SECOND */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_second},
  { Xq_TIME_ID, Xq_DT_SEQ_SIMPLE,	           /* Xq_DT_TENTH_OF_SECOND */
      new_simple, copy_simple, delete_simple,
      compare_simple, concate_dixsecond},
  { Xq_TIME_ID, Xq_DT_SEQ_PADDING,	       /* Xq_DT_HUNDREDTH_OF_SECOND */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_centsecond},
  { Xq_TIME_ID, Xq_DT_SEQ_PADDING,	      /* Xq_DT_THOUSANDTH_OF_SECOND */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_millisecond},  
  { Xq_DATE_ID, Xq_DT_SEQ_PADDING,			      /* Xq_DT_YEAR */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_year},  
  { Xq_DATE_ID, Xq_DT_SEQ_PADDING,			/* Xq_DT_YEAR_SHORT */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_year_short},  
  { Xq_DATE_ID, Xq_DT_SEQ_PADDING,			   /* Xq_DT_YEAR_JC */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_year_jc},
  { Xq_DATE_ID, Xq_DT_SEQ_PADDING,			     /* Xq_DT_MONTH */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_month},
  { Xq_DATE_ID, Xq_DT_SEQ_NAME,				/* Xq_DT_MONTH_NAME */
      new_name, copy_name, delete_name,
      compare_name, concate_month_name},
  { Xq_DATE_ID, Xq_DT_SEQ_PADDING,			      /* Xq_DT_WEEK */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_week},
  { Xq_DATE_ID, Xq_DT_SEQ_PADDING,			       /* Xq_DT_DAY */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_day},
  { Xq_DATE_ID, Xq_DT_SEQ_NAME,			          /* Xq_DT_DAY_NAME */
      new_name, copy_name, delete_name,
      compare_name,  concate_day_name},
  { Xq_DATE_ID, Xq_DT_SEQ_PADDING,		       /* Xq_DT_DAY_IN_YEAR */
      new_padding, copy_padding, delete_padding,
      compare_padding, concate_day_in_year},
  { Xq_DATE_TIME_ID, Xq_DT_SEQ_SEPARATOR,		 /* Xq_ST_SEPARATOR */
      new_separator, copy_separator, delete_separator,
      compare_separator, concate_separator}
};

static char *padding_2f[3] = { "%d", "%2d", "%02d" };
static char *padding_3f[3] = { "%d", "%3d", "%03d" };

void insert_dt_seq(base, item)
dt_seq_t **base;
dt_seq_t *item;
{
  int		i;
  
  i = 0;
  while(i < XqMAX_DT_SEQ && *base)
  {
    i++;
    base++;
  }
  if (i == XqMAX_DT_SEQ)
  {
    Xc_WARNING(("This format doesn't accept more sequence"));
    return;
  }
  
  *base = item;
  *(base + 1) = NULL;
}

void replace_dt_seq(base, src, dst)
dt_seq_t **base;
dt_seq_t *src;
dt_seq_t *dst;
{
  while(*base && *base != src)
    base++;
  
  if (*base)
    *base = dst;
}

void remove_dt_seq(base, item)
dt_seq_t **base;
dt_seq_t *item;
{
  while(*base && *base != item)
    base++;
  
  if (*base)
  {
    while(*base)
    {
      *base = *(base + 1);
      base++;
    }
    *base = NULL;
  }
}
void copy_all_dt_seq(dup_base, base)
dt_seq_t **dup_base;
dt_seq_t **base;
{
  while(*base)
  {
    *dup_base = dt_seq_funcs[(*base)->dt_seq_id].copy(*base);
    dup_base++;
    base++;
  }
  *dup_base = NULL;
}

void destroy_all_dt_seq(base)
dt_seq_t **base;
{
  dt_seq_t **sbase;
  
  sbase = base;
  while(*base)
  {
    dt_seq_funcs[(*base)->dt_seq_id].delete(*base);
    base++;
  }
  memset(sbase, 0, sizeof(dt_seq_t *) * (XqMAX_DT_SEQ + 1));  
}

boolean compare_all_dt_seq(base1, base2)
dt_seq_t **base1;
dt_seq_t **base2;
{
  while(*base1 && *base2)
  {
    if ((dt_seq_funcs[(*base1)->dt_seq_id].type != 
	 dt_seq_funcs[(*base2)->dt_seq_id].type) || 
	!dt_seq_funcs[(*base1)->dt_seq_id].compare(*base1, *base2))
      return FALSE;
    base1++;
    base2++;
  }
  if (*base1 || *base2)
    return FALSE;
  return TRUE;
}

boolean write_all_dt_seq(base, doc)
dt_seq_t **base;
c_DocFile *doc;
{
  padding_id_t	padding;

  padding = Xq_PADDING_NONE;
  while(*base)
  {
    if (dt_seq_funcs[(*base)->dt_seq_id].type == Xq_DT_SEQ_PADDING)
    {
      if ((*base)->padding_dt.padding != padding)
      {
	padding = (*base)->padding_dt.padding;
	switch(padding)
	{
	case Xq_PADDING_NONE:
	  if (!F(doc).writeKeyword
	      (doc, XcDF_PADDING_NONE_KEYWORD, -1L))
	    return FALSE;
	  break;
	case Xq_PADDING_SPACE:
	  if (!F(doc).writeKeyword
	      (doc, XcDF_PADDING_SPACE_KEYWORD, -1L))
	    return FALSE;
	  break;
	case Xq_PADDING_ZERO:
	  if (!F(doc).writeKeyword
	      (doc, XcDF_PADDING_ZERO_KEYWORD, -1L))
	    return FALSE;
	  break;
	}
      }
    }
    switch((*base)->dt_seq_id)
    {
    case Xq_DT_HOUR:
      if (!F(doc).writeKeyword(doc, XcDF_HOUR_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_HOUR12:
      if (!F(doc).writeKeyword(doc, XcDF_HOUR12_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_HOUR_AM_PM:
      if (!F(doc).writeKeyword(doc, XcDF_HOUR_AM_PM_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_MINUTE:
      if (!F(doc).writeKeyword(doc, XcDF_MINUTE_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_SECOND:
      if (!F(doc).writeKeyword(doc, XcDF_SECOND_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_TENTH_OF_SECOND:
      if (!F(doc).writeKeyword(doc, XcDF_TENTH_OF_SECOND_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_HUNDREDTH_OF_SECOND:
      if (!F(doc).writeKeyword(doc, XcDF_HUNDREDTH_OF_SECOND_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_THOUSANDTH_OF_SECOND:
      if (!F(doc).writeKeyword
	  (doc, XcDF_THOUSANDTH_OF_SECOND_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_YEAR:
      if (!F(doc).writeKeyword(doc, XcDF_YEAR_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_YEAR_SHORT:
      if (!F(doc).writeKeyword(doc, XcDF_YEAR_SHORT_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_YEAR_JC:
      if (!F(doc).writeKeyword(doc, XcDF_YEAR_JC_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_MONTH:
      if (!F(doc).writeKeyword(doc, XcDF_MONTH_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_MONTH_NAME:
      switch((*base)->name_dt.name_size)
      {
      case Xq_NAME_VERY_SHORT:
	if (!F(doc).writeKeyword
	    (doc, XcDF_MONTH_NAME_VERY_SHORT_KEYWORD, -1L))
	  return FALSE;
	break;
      case Xq_NAME_SHORT:
	if (!F(doc).writeKeyword
	    (doc, XcDF_MONTH_NAME_SHORT_KEYWORD, -1L))
	  return FALSE;
	break;
      case Xq_NAME_LONG:
	if (!F(doc).writeKeyword
	    (doc, XcDF_MONTH_NAME_LONG_KEYWORD, -1L))
	  return FALSE;
	break;
      }
      break;
    case Xq_DT_WEEK:
      if (!F(doc).writeKeyword(doc, XcDF_WEEK_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_DAY:
      if (!F(doc).writeKeyword(doc, XcDF_DAY_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_DAY_NAME:
      switch((*base)->name_dt.name_size)
      {
      case Xq_NAME_VERY_SHORT:
	if (!F(doc).writeKeyword
	    (doc, XcDF_DAY_NAME_VERY_SHORT_KEYWORD, -1L))
	  return FALSE;
	break;
      case Xq_NAME_SHORT:
	if (!F(doc).writeKeyword
	    (doc, XcDF_DAY_NAME_SHORT_KEYWORD, -1L))
	  return FALSE;
	break;
      case Xq_NAME_LONG:
	if (!F(doc).writeKeyword
	    (doc, XcDF_DAY_NAME_LONG_KEYWORD, -1L))
	  return FALSE;
	break;
      }
      break;
    case Xq_DT_DAY_IN_YEAR:
      if (!F(doc).writeKeyword(doc, XcDF_DAY_IN_YEAR_KEYWORD, -1L))
	return FALSE;
      break;
    case Xq_DT_SEPARATOR:
      if (!F(doc).writeKeyword(doc, XcDF_SEPARATOR_KEYWORD, -1L))
	return FALSE;
      if (!F(doc).startSequence(doc)) return FALSE;
      if (!F(doc).write
	  (doc, (*base)->separator_dt.separator,
	   strlen((*base)->separator_dt.separator)))
	return FALSE;
      if (!F(doc).endSequence(doc)) return FALSE;
      break;
    }
    base++;
  }
  return TRUE;
}


static dt_seq_t *new_simple(seq_num)
int seq_num;
{
  dt_seq_t	*dt_seq;
  
  dt_seq = (dt_seq_t *)Xc_malloc("smp_dt_seq", sizeof(dt_seq_t));
  
  dt_seq->dt_seq_id = seq_num;
  return dt_seq;
}

static dt_seq_t *new_padding(seq_num, padding)
int seq_num;
padding_id_t padding;
{
  dt_seq_t	*dt_seq;
  
  dt_seq = (dt_seq_t *)Xc_malloc("smp_dt_seq", sizeof(dt_seq_t));
  
  dt_seq->dt_seq_id = seq_num;
  dt_seq->padding_dt.padding = padding;
  return dt_seq;
}

static dt_seq_t *new_name(seq_num, name_size)
int seq_num;
name_size_id_t name_size;
{
  dt_seq_t	*dt_seq;
  
  dt_seq = (dt_seq_t *)Xc_malloc("smp_dt_seq", sizeof(dt_seq_t));
  
  dt_seq->dt_seq_id = seq_num;
  dt_seq->name_dt.name_size = name_size;
  return dt_seq;
}

static dt_seq_t *new_separator(seq_num, separator)
int seq_num;
char *separator;
{
  dt_seq_t	*dt_seq;
  
  dt_seq = (dt_seq_t *)Xc_malloc("smp_dt_seq", sizeof(dt_seq_t));
  
  dt_seq->dt_seq_id = seq_num;
  dt_seq->separator_dt.separator = (char *)Xc_strdup("sep_dt", separator);
  return dt_seq;
}

static dt_seq_t *copy_simple(dt_seq)
dt_seq_t *dt_seq;
{
  dt_seq_t	*copy_seq;
  
  copy_seq = (dt_seq_t *)Xc_malloc("cpy_dt_seq", sizeof(dt_seq_t));
  memcpy(copy_seq, dt_seq, sizeof(simple_dt_seq_t));
  return copy_seq;
}

static dt_seq_t *copy_padding(dt_seq)
dt_seq_t *dt_seq;
{
  dt_seq_t	*copy_seq;
  
  copy_seq = (dt_seq_t *)Xc_malloc("cpy_dt_seq", sizeof(dt_seq_t));
  memcpy(copy_seq, dt_seq, sizeof(padding_dt_seq_t));
  return copy_seq;
}

static dt_seq_t *copy_name(dt_seq)
dt_seq_t *dt_seq;
{
  dt_seq_t	*copy_seq;
  
  copy_seq = (dt_seq_t *)Xc_malloc("cpy_dt_seq", sizeof(dt_seq_t));
  memcpy(copy_seq, dt_seq, sizeof(name_dt_seq_t));
  return copy_seq;
}

static dt_seq_t *copy_separator(dt_seq)
dt_seq_t *dt_seq;
{
  dt_seq_t	*copy_seq;
  
  copy_seq = (dt_seq_t *)Xc_malloc("cpy_dt_seq", sizeof(dt_seq_t));
  memcpy(copy_seq, dt_seq, sizeof(separator_dt_seq_t));
  copy_seq->separator_dt.separator = 
    (char *)Xc_strdup("sep_dt", dt_seq->separator_dt.separator);
  return copy_seq;
}

static void delete_simple(dt_seq)
dt_seq_t *dt_seq;
{
  Xc_free(dt_seq);
}

static void delete_padding(dt_seq)
dt_seq_t *dt_seq;
{
  Xc_free(dt_seq);
}

static void delete_name(dt_seq)
dt_seq_t *dt_seq;
{
  Xc_free(dt_seq);
}

static void delete_separator(dt_seq)
dt_seq_t *dt_seq;
{
  Xc_free(dt_seq->separator_dt.separator);
  Xc_free(dt_seq);
}

static boolean compare_simple(a, b)
dt_seq_t *a;
dt_seq_t *b;
{
  return (a->simple_dt.dt_seq_id == b->simple_dt.dt_seq_id);
}

static boolean compare_padding(a, b)
dt_seq_t *a;
dt_seq_t *b;
{
  return (a->padding_dt.dt_seq_id == b->padding_dt.dt_seq_id &&
	  a->padding_dt.padding == b->padding_dt.padding);
}

static boolean compare_name(a, b)
dt_seq_t *a;
dt_seq_t *b;
{
  return (a->name_dt.dt_seq_id == b->name_dt.dt_seq_id &&
	  a->name_dt.name_size == b->name_dt.name_size);
}

static boolean compare_separator(a, b)
dt_seq_t *a;
dt_seq_t *b;
{
  return (a->separator_dt.dt_seq_id == b->separator_dt.dt_seq_id &&
	  Xstrcmp(a->separator_dt.separator, b->separator_dt.separator));
}

static void concate_hour(dt_seq, str, xc_time)
dt_seq_t *dt_seq;
char *str;
xc_time_t *xc_time;
{
  char	buffer[5];
  
  sprintf(buffer, padding_2f[dt_seq->padding_dt.padding], xc_time->hour);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_hour12(dt_seq, str, xc_time)
dt_seq_t *dt_seq;
char *str;
xc_time_t *xc_time;
{
  char	buffer[5];
  
  sprintf(buffer, padding_2f[dt_seq->padding_dt.padding], xc_time->hour12);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_hour_am_pm(dt_seq, str, xc_time)
dt_seq_t *dt_seq;
char *str;
xc_time_t *xc_time;
{
  char	*buffer;
  
  if (xc_time->am_pm)
    buffer = F(GlobResources).getString(GlobResources, XcR_timeHourPM);
  else
    buffer = F(GlobResources).getString(GlobResources, XcR_timeHourAM);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_minute(dt_seq, str, xc_time)
dt_seq_t *dt_seq;
char *str;
xc_time_t *xc_time;
{
  char	buffer[5];
  
  sprintf(buffer, padding_2f[dt_seq->padding_dt.padding], xc_time->minute);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_second(dt_seq, str, xc_time)
dt_seq_t *dt_seq;
char *str;
xc_time_t *xc_time;
{
  char	buffer[5];
  
  sprintf(buffer, padding_2f[dt_seq->padding_dt.padding], xc_time->second);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_dixsecond(dt_seq, str, xc_time)
     dt_seq_t *dt_seq;
char *str;
xc_time_t *xc_time;
{
  char	buffer[3];
  
  sprintf(buffer, "%d", xc_time->millisecond / 100);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
  
}

static void concate_centsecond(dt_seq, str, xc_time)
dt_seq_t *dt_seq;
char *str;
xc_time_t *xc_time;
{
  char	buffer[5];
  
  sprintf(buffer, padding_2f[dt_seq->padding_dt.padding],
	  xc_time->millisecond / 10);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_millisecond(dt_seq, str, xc_time)
dt_seq_t *dt_seq;
char *str;
xc_time_t *xc_time;
{
  char	buffer[5];
  
  sprintf(buffer, padding_3f[dt_seq->padding_dt.padding],
	  xc_time->millisecond);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_year(dt_seq, str, xc_date)
dt_seq_t *dt_seq;
char *str;
xc_date_t *xc_date;
{
  char buffer[15];
  
  if (dt_seq->padding_dt.padding == Xq_PADDING_NONE || 
      xc_date->year < 0 || xc_date->year > 9999)
  {
    sprintf(buffer, "%d", xc_date->year);
  }
  else
  {
    if (dt_seq->padding_dt.padding == Xq_PADDING_SPACE)
      sprintf(buffer, "%4d", xc_date->year);
    else
      sprintf(buffer, "%04d", xc_date->year);
  }
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_year_short(dt_seq, str, xc_date)
dt_seq_t *dt_seq;
char *str;
xc_date_t *xc_date;
{
  char	buffer[15];
  
  if (xc_date->year >= 1920 && xc_date->year < 2020)
  {
    sprintf(buffer, "%02d", xc_date->year % 100);
  }
  else
  {
    sprintf(buffer, "%d", xc_date->year);
  }
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_year_jc(dt_seq, str, xc_date)
dt_seq_t *dt_seq;
char *str;
xc_date_t *xc_date;
{
  char *format;
  char buffer[30];
  int	year;
  
  year = ABS(xc_date->year);
  if (dt_seq->padding_dt.padding == Xq_PADDING_NONE || year > 9999)
  {
    format = "%d %.15s";
  }
  else
  {
    if (dt_seq->padding_dt.padding == Xq_PADDING_SPACE)
      format = "%4d %.15s";
    else
      format = "%04d %.15s";
  }
  sprintf(buffer, format, year, xc_date->after_jc ? 
	  F(GlobResources).getString(GlobResources, XcR_dateYearAfterJC):
	  F(GlobResources).getString(GlobResources, XcR_dateYearBeforeJC));
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_month(dt_seq, str, xc_date)
dt_seq_t *dt_seq;
char *str;
xc_date_t *xc_date;
{
  char	buffer[5];
  
  sprintf(buffer, padding_2f[dt_seq->padding_dt.padding], xc_date->month);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_month_name(dt_seq, str, xc_date)
dt_seq_t *dt_seq;
char *str;
xc_date_t *xc_date;
{
  int	value, i, j;
  char	*month_name;
  
  value = xc_date->month;
  value = (value - 1) % 12;
  if (value < 0) value = 12 + value;
  switch(dt_seq->name_dt.name_size)
  {
  case Xq_NAME_VERY_SHORT:
    month_name = F(GlobResources).getString(GlobResources,
					  XcR_dateMonthVeryShort);
    i = j = 0;
    while(month_name[i] && j < value)
    {
      if (month_name[i] == ' ')
	j++;
      i++;
    }
    if (j != value)
    {
      if (strlen(str) + 2 < Xq_FORMULA_LENGTH)
	strcat(str, "??");
    }
    else
    {
      j = i;
      while(month_name[j] && month_name[j] != ' ')
	j++;
      if (strlen(str) + j - i < Xq_FORMULA_LENGTH)
	strncat(str, month_name + i, j - i);
    }
    break;
  case Xq_NAME_SHORT:
    month_name = F(GlobResources).getString(GlobResources,
					    XcR_dateMonthJanShort + value);
    if (strlen(str) + strlen(month_name) < Xq_FORMULA_LENGTH)
      strcat(str, month_name);
    break;
  case Xq_NAME_LONG:
    month_name = F(GlobResources).getString(GlobResources,
					    XcR_dateMonthJan + value);
    if (strlen(str) + strlen(month_name) < Xq_FORMULA_LENGTH)
      strcat(str, month_name);
    break;
  }
}

static void concate_week(dt_seq, str, xc_date)
dt_seq_t *dt_seq;
char *str;
xc_date_t *xc_date;
{
  char	buffer[5];
  
  sprintf(buffer, padding_2f[dt_seq->padding_dt.padding], xc_date->week);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_day(dt_seq, str, xc_date)
dt_seq_t *dt_seq;
char *str;
xc_date_t *xc_date;
{
  char	buffer[5];
  
  sprintf(buffer, padding_2f[dt_seq->padding_dt.padding], xc_date->day);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_day_name(dt_seq, str, xc_date)
dt_seq_t *dt_seq;
char *str;
xc_date_t *xc_date;
{
  int	value, i, j;
  char	*day_name;
  
  value = xc_date->day_in_week;
  value = (value - 1) % 7;
  if (value < 0) value = 7 + value;
  switch(dt_seq->name_dt.name_size)
  {
  case Xq_NAME_VERY_SHORT:
    day_name = F(GlobResources).getString(GlobResources,
					  XcR_dateDayVeryShort);
    i = j = 0;
    while(day_name[i] && j < value)
    {
      if (day_name[i] == ' ')
	j++;
      i++;
    }
    if (j != value)
    {
      if (strlen(str) + 2 < Xq_FORMULA_LENGTH)
	strcat(str, "??");
    }
    else
    {
      j = i;
      while(day_name[j] && day_name[j] != ' ')
	j++;
      if (strlen(str) + j - i < Xq_FORMULA_LENGTH)
	strncat(str, day_name + i, j - i);
    }
    break;
  case Xq_NAME_SHORT:
    day_name = F(GlobResources).getString(GlobResources,
					    XcR_dateDayMonShort + value);
    if (strlen(str) + strlen(day_name) < Xq_FORMULA_LENGTH)
      strcat(str, day_name);
    break;
  case Xq_NAME_LONG:
    day_name = F(GlobResources).getString(GlobResources,
					    XcR_dateDayMon + value);
    if (strlen(str) + strlen(day_name) < Xq_FORMULA_LENGTH)
      strcat(str, day_name);
    break;
  }
}

static void concate_day_in_year(dt_seq, str, xc_date)
dt_seq_t *dt_seq;
char *str;
xc_date_t *xc_date;
{
  char	buffer[5];
  
  sprintf(buffer, padding_3f[dt_seq->padding_dt.padding],
	  xc_date->day_in_year);
  if (strlen(str) + strlen(buffer) < Xq_FORMULA_LENGTH)
    strcat(str, buffer);
}

static void concate_separator(dt_seq, str)
dt_seq_t *dt_seq;
char *str;
{
  if (strlen(str) + strlen(dt_seq->separator_dt.separator) < Xq_FORMULA_LENGTH)
    strcat(str, dt_seq->separator_dt.separator);
}

