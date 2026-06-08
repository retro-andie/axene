/*
** FormatNbr.h for XQuad in FormatNbr/
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
** Last update Sun Dec  1 15:04:19 1996 Emmanuel Paris
*/

#ifndef _FormatNbr_h
#define _FormatNbr_h

typedef struct _c_FormatNbr c_FormatNbr;

#include "xcalibur.h"
#include "Color.h"
#include "Hook.h"
#include "DocFile.h"
#include "FormatNbrP.h"
#include "ruler.h"
#include "Date_Time.h"

#define MAX_MAX_PREC 20
#define MAX_SPACE 20

typedef enum {
  XcFt_NORMAL,   XcFt_GENERIC,
  XcFt_STANDARD, XcFt_PERCENT, XcFt_CURRENCY, XcFt_THOUSANDS,
  XcFt_SCIENTIFIC, XcFt_ENGINEER
} format_type_t;

typedef enum {
  XcF_TYPE, XcF_NAME,
  /* value fields */
  XcF_MAXPREC, XcF_MINPREC, XcF_SPACE, XcF_UNIT_NAME, XcF_COMMA,
  XcF_COLORP, XcF_COLORN, XcF_UNITAFTER, XcF_CHARSEP, XcF_TRUNCATE,
  /* exponant fields */
  XcF_EXPDIGIT, XcF_EXPMULTIPLE, XcF_EXPSTRING, XcF_EXPSIGN,
  /* fraction fields */
  XcF_FRAC_PREC, XcF_FRAC_DEC,
  /* boolean fields */
  XcF_TRUE_COLOR, XcF_FALSE_COLOR, XcF_TRUE_NAME, XcF_FALSE_NAME,
  /* day in week & month fields */
  XcF_DAY_NAME_SIZE, XcF_MONTH_NAME_SIZE,
  /* date, time & date_time fields */
  XcF_ADD_DATE_SEQ, XcF_ADD_TIME_SEQ, XcF_ADD_DATE_TIME_SEQ, 
  /* generic format */
  XcF_OTHER_TYPE, XcF_SPECIAL, XcF_HIDE_ON, XcF_HIDE_OFF, XcF_LOCK,
  XcF_END, XcF_PARSE_STRING,
  XcF_SET_DATE_SEQS, XcF_SET_TIME_SEQS, XcF_SET_DATE_TIME_SEQS,
  XcF_ALL_OTHER_TYPES
} format_set_code_t;

/*--- Data structure for convert_string return ---*/
typedef struct {
  c_Color	*color;
  ruler_t	ruler;
  boolean	truncable;
  int		comma_pos;
  int		trunc_begin;
  int		trunc_end;
} conv_info_t;

/* define methods for Class FormatNbr */
typedef struct
{
  F_STD;
  /*___Utils function___*/
  void (*convert_string) ___PROTO((c_FormatNbr *this, char *str, 
				   int type, conv_info_t *info));
  /*___Class control___*/
  void (*set) ___NPROTO((c_FormatNbr *class, ...));
  boolean (*compare) ___PROTO((c_FormatNbr *a, c_FormatNbr *b,
			       boolean with_name));
  void (*merge) ___PROTO((c_FormatNbr *source, c_FormatNbr **target,
			  boolean fusion));
  c_FormatNbr *(*virtualCopy) ___PROTO((c_FormatNbr *format, BaseStd_t *base));
  c_FormatNbr *(*get) ___NPROTO((c_FormatNbr *class, ...));
  
  /*___Loading & saving___*/
  boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			    long param, format_read_t *data));
  boolean (*write) ___PROTO((c_FormatNbr *class, c_DocFile *doc));
  boolean (*readList) ___PROTO((c_DocFile *doc, char *keyword,
				long param, format_read_t *data));
  boolean (*writeList) ___PROTO((c_FormatNbr *class, c_DocFile *doc));
} sf_FormatNbr;

/* define the FormatNbr Class */
struct _c_FormatNbr
{
  c_FormatNbr	*previous;
  c_FormatNbr	*next;
  c_FormatNbr	**base;

  sf_FormatNbr	*f;

  char 		name[XcMAX_NAME_LENGTH]; /* format name                 */
  long 		id;		/* format ID                   		*/
  
  int		type;
  
  /*	field for type Xq_VALUE		*/
  unsigned int	max_prec;	/* aff precsion after ','		*/
  unsigned int	min_prec;	/* aff precsion after ','		*/
  unsigned int	space_each;	/* space each n caracter before ','	*/
  char 		comma_sep;	/* special comma eg. '.' or ',' 	*/
  c_Color	*color1;	/* color for positif number or TRUE	*/
  c_Color	*color2;	/* color for negatif number of FALSE	*/
  char 		char_sep;	/* special space eg. ' ' or ',' 	*/
  boolean	truncate;	/* unit is after or befor number	*/
  

  /*	field for type Xq_UNIT		*/
  char		unit[XcMAX_NAME_LENGTH]; /* need a unit			*/
  boolean	unitafter;	/* unit is after or befor number	*/
  
  /*	field for type Xq_EXPONANTIAL	*/
  unsigned int	exp_digit;
  unsigned int	exp_multiple;
  boolean	exp_sign;
  char		exp_string[XcMAX_NAME_LENGTH];
  
  /*	field for type Xq_FRACTION	*/
  unsigned int	frac_prec;	/* fraction precision: ??/?? */
  boolean	frac_dec;	/* TRUE if fraction only on decimal part */
  
  /*	field for type Xq_BOOLEAN	*/
  char		true_name[XcMAX_NAME_LENGTH];
  char		false_name[XcMAX_NAME_LENGTH];
  
  /*	field for type Xq_DAY_IN_WEEK	*/
  name_size_id_t day_name_size;
  
  /*	field for type Xq_MONTH		*/
  name_size_id_t month_name_size;
  
  /*	field for date, time and date_time	*/
  dt_seq_t	*date_seq[XqMAX_DT_SEQ+1];
  dt_seq_t	*time_seq[XqMAX_DT_SEQ+1];
  dt_seq_t	*date_time_seq[XqMAX_DT_SEQ+1];
  
  padding_id_t	padding;	/* for docfile only */
  date_time_id_t seq_id;	/* for docfile only */
  
  c_FormatNbr	*other_format[15];
  
  format_type_t	special;	/* special format, associate with icon */
  /*___Hook___*/
  c_Hook *hook;

  /*___Flags___*/
  boolean lock_flag;
  boolean hide_flag;
  
  BaseStd_t	*BaseStd;
};

extern sf_FormatNbr fc_FormatNbr;

#endif







