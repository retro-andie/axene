/*
** TimeFuncs.h for XQuad in FormatNbr/
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
** Last update Thr Dec  7 18:58:18 1995 Emmanuel Paris
*/

#ifndef _TimeFuncs_h_
#define _TimeFuncs_h_

#include "xcalibur.h"

#define Xc_YEAR_ZERO	2000
#define Xc_FIRST_DAY	5	/* day in week the 1st january Xc_YEAR_ZERO */
#define Xc_TIME_FRACTION 86400000.0
#define Xc_TIME_FRACTION2 86400000

/* @ notice struct field that are used in convertion_to_xc_.... */
typedef struct {
  int			year;		/* ]-m,-1] U [1,+m[ */	        /* @ */
  boolean		after_jc;	/* FALSE before JC; TRUE after JC */
  unsigned short	month;		/* [1-12] */		        /* @ */
  unsigned short	week;		/* [1-52] */
  unsigned short	day;		/* [1-31] */		        /* @ */
  unsigned short	day_in_week;	/* [1-7] (1 -> monday) */
  unsigned short	day_in_year;	/* [1-366] */
} xc_date_t;

typedef struct {
  unsigned short	hour;		/* [0-23] */		        /* @ */
  unsigned short	hour12;		/* [1-12] */
  boolean		am_pm;		/* FALSE am; TRUE pm */
  unsigned short	minute;		/* [0-59] */		        /* @ */
  unsigned short	second;		/* [0-59] */		        /* @ */
  unsigned short	millisecond;	/* [0-999] */		        /* @ */
} xc_time_t;

typedef struct {
  xc_date_t	date;
  xc_time_t	time;
} xc_date_time_t;

extern xc_date_time_t *fill_xc_date_and_time ___PROTO((xc_date_time_t
						       *ret_value));

extern xc_date_t *fill_xc_date ___PROTO((xc_date_t *ret_value));

extern xc_time_t *fill_xc_time ___PROTO((xc_time_t *ret_value));

extern xc_date_time_t *convert_to_date_and_time ___PROTO((double value,
							  xc_date_time_t
							  *ret_value));
extern xc_date_t *convert_to_date ___PROTO((double value,
					    xc_date_t *ret_value));

extern xc_time_t *convert_to_time ___PROTO((double value,
					    xc_time_t *ret_value));

extern double convert_to_xc_date_and_time ___PROTO((xc_date_time_t *xc_value,
						    boolean *valid));

extern double convert_to_xc_date ___PROTO((xc_date_t *xc_value,
					   boolean *valid));

extern double convert_to_xc_time ___PROTO((xc_time_t *xc_value,
					   boolean *valid));


#ifdef _TimeFuncs_Private_

static unsigned short non_bissextile_year[12] = 
{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static unsigned short bissextile_year[12] = 
{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#endif

#endif /* _TimeFuncs_h_ */









