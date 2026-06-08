/*
** Date_Time.h for XQuad in FormatNbr/
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
** Last update Fri Mar 22 14:10:05 1996 Emmanuel Paris
*/

#ifndef _DATE_TIME_h_
#define _DATE_TIME_h_

enum {
  Xq_DT_HOUR = 0,		/* PADDING */
  Xq_DT_HOUR12,			/* PADDING */
  Xq_DT_HOUR_AM_PM,		/* SIMPLE */
  Xq_DT_MINUTE,			/* PADDING */
  Xq_DT_SECOND,			/* PADDING */
  Xq_DT_TENTH_OF_SECOND,	/* SIMPLE */
  Xq_DT_HUNDREDTH_OF_SECOND,	/* PADDING */
  Xq_DT_THOUSANDTH_OF_SECOND,	/* PADDING */
  Xq_DT_YEAR,			/* PADDING */
  Xq_DT_YEAR_SHORT,		/* PADDING */
  Xq_DT_YEAR_JC,		/* SIMPLE */
  Xq_DT_MONTH,			/* PADDING */
  Xq_DT_MONTH_NAME,		/* NAME_SIZE */
  Xq_DT_WEEK,			/* PADDING */
  Xq_DT_DAY,			/* PADDING */
  Xq_DT_DAY_NAME,		/* NAME_SIZE */
  Xq_DT_DAY_IN_YEAR,		/* PADDING */
  Xq_DT_SEPARATOR,		/* SEPARATOR */
  Xq_NB_DT
};

enum {
  Xq_DT_SEQ_SIMPLE,
  Xq_DT_SEQ_PADDING,
  Xq_DT_SEQ_NAME,
  Xq_DT_SEQ_SEPARATOR
};

typedef enum {
  Xq_DATE_ID = 1,
  Xq_TIME_ID = 2,
  Xq_DATE_TIME_ID = 3
} date_time_id_t;

typedef enum {
  Xq_PADDING_NONE = 0,
  Xq_PADDING_SPACE,
  Xq_PADDING_ZERO
} padding_id_t;

typedef enum {
  Xq_NAME_VERY_SHORT,
  Xq_NAME_SHORT,
  Xq_NAME_LONG
} name_size_id_t;

typedef union _dt_seq_t dt_seq_t;

typedef struct {
  int	dt_seq_id;
} any_dt_seq_t;

typedef struct {
  int	dt_seq_id;
} simple_dt_seq_t;

typedef struct {
  int	dt_seq_id;
  padding_id_t	 padding;
} padding_dt_seq_t;

typedef struct {
  int	dt_seq_id;
  name_size_id_t name_size;
} name_dt_seq_t;
  
typedef struct {
  int	dt_seq_id;
  char	*separator;
} separator_dt_seq_t;

union _dt_seq_t 
{
  int	dt_seq_id;
  any_dt_seq_t		any_dt;
  simple_dt_seq_t	simple_dt;
  padding_dt_seq_t	padding_dt;
  name_dt_seq_t		name_dt;
  separator_dt_seq_t	separator_dt;
};

typedef struct {
  date_time_id_t	id; 
  int	type;
  dt_seq_t *(*new)();
  dt_seq_t *(*copy)();
  void	    (*delete)();
  boolean   (*compare)();
  void	    (*concate)();	
} dt_seq_funcs_t;

extern void insert_dt_seq ___PROTO((dt_seq_t **base, dt_seq_t *item));
extern void replace_dt_seq ___PROTO((dt_seq_t **base, dt_seq_t *src,
				     dt_seq_t *dst));
extern void remove_dt_seq ___PROTO((dt_seq_t **base, dt_seq_t *item));
extern void copy_all_dt_seq ___PROTO((dt_seq_t **dup_base, dt_seq_t **base));
extern void destroy_all_dt_seq ___PROTO((dt_seq_t **base));
extern boolean compare_all_dt_seq ___PROTO((dt_seq_t **base1,
					    dt_seq_t **base2));
extern boolean write_all_dt_seq ___NPROTO((dt_seq_t **base, c_DocFile *doc));

extern dt_seq_funcs_t dt_seq_funcs[Xq_NB_DT];

#endif /* _DATE_TIME_h_ */









