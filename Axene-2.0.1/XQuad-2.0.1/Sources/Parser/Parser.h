/*
** Parser.h for XQuad in Parser/
** Definition for class Parser
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
** Started on  Thu Mar  2 16:32:40 1995 Antoine Buat
** Last update Wed Nov 27 15:11:12 1996 Antoine Buat
*/

#ifndef _Parser_h
#define _Parser_h

/*
#define TYPE_VALUE_ERROR	55
should be used for each parameter out of range 

**à definir comme erreur du moteur:
TYPE_NO_ANSWER -- fonction: cherche, cnum
TYPE_VALUE_ERROR -- fonction: adresse, base 
TYPE_D_0_TO_256 -- fonction: car 
*/

typedef struct _c_Parser c_Parser;

#include "xcalibur.h"
#include "genpari.h"
#include "Moteur.h"
#include "TimeFuncs.h"

#define Xq_SEPAR ';'
#define Xq_PARI_SEPAR ','
#define MAX_STRF_LEN 5000

#define TYPE_NONE	0
#define TYPE_VOID	0
#define TYPE_TEXT	1
#define TYPE_STRING	1
#define TYPE_NUM	2
#define	TYPE_VAR_NUM	3
#define	TYPE_FUNC	4
#define	TYPE_OPERATOR	5
#define	TYPE_U_OPERATOR	6
#define TYPE_PARSE_BEGIN 7
#define TYPE_PARSE_END	8
#define TYPE_SEPARATOR	9
#define TYPE_CELL	10
#define TYPE_CELL_ZONE	11
#define TYPE_PARSE_EOF	12
#define TYPE_PARAM_P	13
#define TYPE_PERCENT	14
#define TYPE_UNIT	15
#define TYPE_FRACTION	16
#define TYPE_BOOLEAN	17
#define TYPE_DAY_IN_WEEK 18
#define TYPE_MONTH	19
#define TYPE_TIME	20
#define TYPE_DATE	21
#define TYPE_DATE_TIME	22
#define TYPE_EXPONANTIAL 23

#define	TYPE_ERROR	30
#define	TYPE_CIRC_ERROR	31
#define	TYPE_VAL_ERROR	32
#define	TYPE_NAME_ERROR	33
#define	TYPE_CHAR_ERROR	34
#define	TYPE_CALC_ERROR	35
#define	TYPE_DIVZ_ERROR	36
#define	TYPE_PARAM_LESS_ERROR	37
#define	TYPE_PARAM_MORE_ERROR	38
#define	TYPE_CELLRANGE_ERROR	39
#define	TYPE_DOMAIN_ERROR	40
#define	TYPE_D_INT_ERROR	41
#define	TYPE_D_ONEONE_ERROR	42
#define	TYPE_D_OVERONE_ERROR	43
#define	TYPE_D_ONEONEOPEN_ERROR	44
#define	TYPE_D_ALLEZERO_ERROR	45
#define	TYPE_D_OVERZERO_ERROR	46
#define	TYPE_D_POSITIVE_ERROR	47
#define TYPE_MISSING_P_ERROR	48
#define TYPE_UNTERM_STRING	49
#define TYPE_NO_PARAM_STRING	50
#define TYPE_D_0_TO_256_ERROR	51
#define TYPE_MUST_BE_STRING	52
#define TYPE_D_OUTOFSTR_ERROR	53
#define TYPE_NO_ANSWER		54
#define TYPE_VALUE_ERROR	55
#define TYPE_NO_PARAM_NUM	56
#define TYPE_NOT_COMPATIBLE	57
#define TYPE_NEED_PARAM_ZONE	58

#define ADD_DEP		1
#define SUB_DEP		2
#define IGN_DEP		3

#define P_NONE		(-1)
#define P_ANY		(-2)
#define P_INONE		(-3)
#define P_IANY		(-4)
#define P_INTERN(a)	(-5-a)

#define ALL		0
#define ONEONE		1
#define OVERONE		2
#define ONEONEOPEN	3
#define INTEGER		4
#define ALLEZERO	5
#define OVERZERO	6
#define POSITIVE	7
#define ALLANDINT	8
#define ALLEPI2PI	9

#define Xq_FT_NUMERIC		1
#define Xq_FT_STATISTICAL	2
#define Xq_FT_BUSINESS		4
#define Xq_FT_DATE_TIME 	8
#define Xq_FT_SPREADSHEET	16
#define Xq_FT_LOGICAL		32
#define Xq_FT_TEXT		64

#define DB_TO_INT(a)	(int)((a)>0.0?(a)+0.5:(a)-0.5)

#define MONTH_FIRST	0
#define DAY_FIRST	1

/*
   existante type are:
   -string
   -date
   -cellvalue_t
   others could easily be added
   only cellvalue_t should be treated by pari 
*/

typedef struct _op_list
{
  int		size;
  char		*name;
  int		outsize;
  char		*outname;
  boolean	converttofunc;
} op_list;


typedef struct _def_var
{
  char			*name;
  char			*string_exp;
  struct _def_var	*Next;
} def_var;

typedef struct _def_func
{
  char			*name;
  char			*savename;
  char			*outname;
  int			outsize;
  int			(*func)();
  int			nbr_param;
  int			domain;
  int			type;		/* Xq_FT_TEXT etc... */
  struct _def_func	*Next;
} def_func;

/* ----------------------------------------------------------------- ** 
** define methods                                                    ** 
** ----------------------------------------------------------------- */
typedef struct
{
  F_STD;
  int	(*parse)();
  int	(*rparse)();
  int	(*get_token)();
  void	(*add_func)();
  int	(*srparse)();
  int	(*compute_param)();
  char	*(*trans_language)();
} sf_Parser;

struct _c_Parser
{
  sf_Parser	*f;
  def_var	*var;
  def_func	*func;
  c_Moteur	*motor;
  int		d_type;
  int		x;
  int		y;
  cell		*cell_s;
  int		ret_type;
  int		date_entry_mode;
};

extern sf_Parser fc_Parser;

#endif
