/*
** Parser.c for XQuad in Parser/
** Method for Class Parser
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
** Last update Sun Mar 15 14:31:20 1998 Emmanuel Paris
*/

#define _PARSER_PRIVATE_
#include "Parser.h"
#include "char_translate.h"

void *cons_Parser();
void dest_Parser();
void *copy_Parser();
static int Parser_parse();
static int Parser_rparse();
static int Parser_compute_param();
static int Parser_srparse();
static int Parser_get_token();
static void Parser_add_func();
static char *trans_language();

/* fonction math et trigo */
#include "Parser_math.h"

/* fonction statistique */
#include "Parser_stat.h"

/* fonction finance */
#include "Parser_finance.h"

/* fonction texte */
#include "Parser_text.h"

/* fonction tableur */
#include "Parser_tablor.h"

/* fonction logique */
#include "Parser_boolean.h"

/* fonction date et heure */
#include "Parser_date.h"

#include "Parse_token.h"

#define MAX_DEPTH 100		/* profondeur de recusrion max */
int 	type_d[MAX_DEPTH];	/* type par profondeur */
int	current_depth;		/* profondeur actuel dans le parse */

/* ----------------------------------------------------------------- ** 
** seting the method                                                 ** 
** ----------------------------------------------------------------- */
sf_Parser fc_Parser =
{
 cons_Parser,
 dest_Parser,
 copy_Parser,
 Parser_parse,
 Parser_rparse,
 Parser_get_token,
 Parser_add_func,
 Parser_srparse,
 Parser_compute_param,
 trans_language
};

/* ----------------------------------------------------------------- ** 
** constructor                                                       ** 
** ----------------------------------------------------------------- */
void *cons_Parser(motor)
c_Moteur	*motor;
{
 c_Parser	*ObjTmp;
 char		*tmp_string;
  
 if ((ObjTmp = (c_Parser *)Xc_malloc("Parser", 
				     sizeof(c_Parser))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Parser;
 ObjTmp->var = NULL;
 ObjTmp->func = NULL;
 ObjTmp->motor = motor;
  
 tmp_string = F(GlobResources).getString(GlobResources, 
					 XcR_dateEntryMode);
 if (tmp_string)
 {
  int	i;

  i=0;
  while(tmp_string[i])
  {
   if ((tmp_string[i]>'A') && (tmp_string[i]<'Z'))
    tmp_string[i]+='a'-'A';
   i++;
  }
  if (strcmp(tmp_string, "dayfirst") == 0)
  {
   ObjTmp->date_entry_mode = DAY_FIRST;
  }
  else
   ObjTmp->date_entry_mode = MONTH_FIRST;
 }
 else
  ObjTmp->date_entry_mode = MONTH_FIRST;

 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "abs", 
		    "abs", 1, ALL, XcR_funcAbs, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "acos", 
		    "acos", 1, ONEONE, XcR_funcAcos, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "acosh",
		    "acosh", 1, OVERONE, XcR_funcAcosh, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "adress", 
		    i_adresse, P_INTERN(5), ALL, XcR_funcAdress, 
		    Xq_FT_SPREADSHEET);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "alea",
		    "abs(cos(random()))", P_NONE, ALL, XcR_funcAlea, 
		    Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "aleab", 
		    i_aleab, P_INTERN(2), ALL, XcR_funcAleab, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "sln", 
		    i_amorlin, P_INTERN(3), ALL, XcR_funcSln, Xq_FT_BUSINESS);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "round", 
		    i_arrondi, P_INTERN(2), ALL, XcR_funcRound, 
		    Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "asin",
		    "asin", 1, ONEONE, XcR_funcAsin, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "asinh",
		    "asinh", 1, ALL, XcR_funcAsinh, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "atan",
		    "atan", 1, ALL, XcR_funcAtan, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "atan2", 
		    i_atan2, P_INTERN(2), ALL, XcR_funcAtan2, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "atanh",
		    "atanh", 1, ONEONEOPEN, XcR_funcAtanh, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "base", 
		    i_base, P_INTERN(2), ALL, XcR_funcBase, Xq_FT_NUMERIC);
 /*TODO: see if integer */
 /*
   F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "besselj",
   "jbesselh", 2, ALL, XcR_funcBesselj, Xq_FT_NUMERIC);
   F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "besselk",
   "kbessel", 2, ALL, XcR_funcBesselk, Xq_FT_NUMERIC);
   */
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "char",
		    i_car, P_INTERN(1), ALL, XcR_funcChar, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "ceil",
		    "ceil", 1, ALL, XcR_funcCeil, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "find",
		    i_cherche, P_INTERN(3), ALL, XcR_funcFind,
		    Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "cnum", 
		    i_cnum, P_INTERN(1), ALL, XcR_funcCnum, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "col", 
		    i_colonne, P_INTERN(1), ALL, XcR_funcCol, 
		    Xq_FT_SPREADSHEET);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "concat", 
		    i_concat, P_INTERN(P_ANY), ALL, XcR_funcConcat, 
		    Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "code", 
		    i_code, P_INTERN(1), ALL, XcR_funcCode, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "cos",
		    "cos", 1, ALL, XcR_funcCos, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "cosh",
		    "cosh", 1, ALL, XcR_funcCosh, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "count",
		    i_count, P_INTERN(P_ANY), ALL, XcR_funcCount, 
		    Xq_FT_STATISTICAL);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "countn",
		    i_countn, P_INTERN(P_ANY), ALL, XcR_funcCountn, 
		    Xq_FT_STATISTICAL);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "ctxt",
		    i_ctxt, P_INTERN(3), ALL, XcR_funcCtxt, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "degrees",
		    "180/pi*", 1, ALL, XcR_funcDegrees, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "right",
		    i_droite, P_INTERN(2), ALL, XcR_funcRight, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "e", 
		    "exp(1)", P_NONE, ALL, XcR_funcE, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "int", 
		    "floor", 1, ALL, XcR_funcInt, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "purge",
		    i_epurage, P_INTERN(1), ALL, XcR_funcPurge, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "and", 
		    i_et, P_INTERN(P_ANY), ALL, XcR_funcAnd, Xq_FT_LOGICAL);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "exact", 
		    i_exact, P_INTERN(2), ALL, XcR_funcExact, Xq_FT_TEXT);
 /* have come problem with exp because calculate too much digit */
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "exp",
		    "exp", 1, ALL, XcR_funcExp, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "fact",
		    "fact", 1, INTEGER, XcR_funcFact, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "false",
		    i_faux, P_INTERN(P_NONE), ALL, XcR_funcFalse, 
		    Xq_FT_LOGICAL);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "floor",
		    "floor", 1, ALL, XcR_funcFloor, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "frac",
		    "frac", 1, ALL, XcR_funcFrac, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "currency",
		    i_ctxt, P_INTERN(2), ALL, XcR_funcCurrency, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "left",
		    i_gauche, P_INTERN(2), ALL, XcR_funcLeft, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "indirect",
		    i_indirect, P_INTERN(2), ALL, XcR_funcIndirect,
		    Xq_FT_SPREADSHEET);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "inv", 
		    "1.0/", 1, ALLEZERO, XcR_funcInv, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "row",
		    i_ligne, P_INTERN(1), ALL, XcR_funcRow, 
		    Xq_FT_SPREADSHEET);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "ln", 
		    "ln", 1, OVERZERO, XcR_funcLn, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "log", 
		    i_log, P_INTERN(2), ALL, XcR_funcLog, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "log10",
		    i_log10, P_INTERN(1), ALL, XcR_funcLog10, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "upper", 
		    i_majuscule, P_INTERN(1), ALL, XcR_funcUpper, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "max", 
		    it_max, P_INTERN(P_ANY), ALL, XcR_funcMax, 
		    Xq_FT_STATISTICAL);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "min", 
		    it_min, P_INTERN(P_ANY), ALL, XcR_funcMin, 
		    Xq_FT_STATISTICAL);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "lower", 
		    i_minuscule, P_INTERN(1), ALL, XcR_funcLower, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "mod", 
		    i_mod, P_INTERN(2), ALL, XcR_funcMod, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "avg", 
		    i_moy, P_INTERN(P_ANY), ALL, XcR_funcAvg, 
		    Xq_FT_STATISTICAL);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "length", 
		    i_nbcar, P_INTERN(1), ALL, XcR_funcLength, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "proper", 
		    i_nompropre, P_INTERN(1), ALL, XcR_funcProper, 
		    Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "pi", 
		    "pi", P_NONE, ALL, XcR_funcPi, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "prod",
		    i_prod, P_INTERN(P_ANY), ALL, XcR_funcProd, 
		    Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "sqrt", 
		    "sqrt", 1, POSITIVE, XcR_funcSqrt, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "random",
		    "random", P_NONE, ALL, XcR_funcRandom, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "replace",
		    i_remplacer, P_INTERN(4), ALL, XcR_funcReplace, 
		    Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "repeat", 
		    i_rept, P_INTERN(2), ALL, XcR_funcRepeat, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "sign",
		    "sign", 1, ALL, XcR_funcSign, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "sin",
		    "sin", 1, ALL, XcR_funcSin, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "sinh",
		    "sinh", 1, ALL, XcR_funcSinh, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "summ",
		    i_somme, P_INTERN(P_ANY), ALL, XcR_funcSumm, 
		    Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "sqr",
		    "sqr", 1, ALL, XcR_funcSqr, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "mid",
		    i_stxt, P_INTERN(3), ALL, XcR_funcMid, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "substitute", 
		    i_substitue, P_INTERN(4), ALL, XcR_funcSubstitute, 
		    Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "trim", 
		    i_supprespace, P_INTERN(1), ALL, XcR_funcTrim, 
		    Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "radians", 
		    "pi/180*", 1, ALL, XcR_funcRadians, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "t", 
		    i_t, P_INTERN(1), ALL, XcR_funcT, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "tan", 
		    "tan", 1, ALLEPI2PI, XcR_funcTan, Xq_FT_NUMERIC);
 /* >30 = 1 ; <30 = -1 */
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "tanh",
		    "tanh", 1, ALL, XcR_funcTanh, Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "text", 
		    i_texte, P_INTERN(1), ALL, XcR_funcText, Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "trunc", 
		    i_tronque, P_INTERN(3), ALL, XcR_funcTrunc,
		    Xq_FT_NUMERIC);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "match", 
		    i_trouve, P_INTERN(3), ALL, XcR_funcMatch, 
		    Xq_FT_TEXT);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "true", 
		    i_vrai, P_INTERN(P_NONE), ALL, XcR_funcTrue,
		    Xq_FT_LOGICAL);

 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "day360", 
		    i_day360, P_INTERN(P_NONE), ALL, XcR_funcDay360,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "today", 
		    i_today, P_INTERN(P_NONE), ALL, XcR_funcToday,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "date", 
		    i_date, P_INTERN(P_NONE), ALL, XcR_funcDate,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "datevalue", 
		    i_dateval, P_INTERN(P_NONE), ALL, XcR_funcDateval,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "now", 
		    i_now, P_INTERN(P_NONE), ALL, XcR_funcNow,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "time", 
		    i_time, P_INTERN(P_NONE), ALL, XcR_funcTime,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "timevalue", 
		    i_timeval, P_INTERN(P_NONE), ALL, XcR_funcTimeval,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "year", 
		    i_year, P_INTERN(P_NONE), ALL, XcR_funcYear,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "day", 
		    i_day, P_INTERN(P_NONE), ALL, XcR_funcDay,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "weekday",
		    i_dayweek, P_INTERN(P_NONE), ALL, XcR_funcDayweek,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "time",
		    i_hour, P_INTERN(P_NONE), ALL, XcR_funcHour,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "minute",
		    i_minute, P_INTERN(P_NONE), ALL, XcR_funcMinute,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "month",
		    i_month, P_INTERN(P_NONE), ALL, XcR_funcMonth,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "second",
		    i_second, P_INTERN(P_NONE), ALL, XcR_funcSecond,
		    Xq_FT_DATE_TIME);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "millisecond",
		    i_milli, P_INTERN(P_NONE), ALL, XcR_funcMillisecond,
		    Xq_FT_DATE_TIME);

 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "db",
		    i_db, P_INTERN(P_NONE), ALL, XcR_funcDb,
		    Xq_FT_BUSINESS);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "ddb",
		    i_ddb, P_INTERN(P_NONE), ALL, XcR_funcDdb,
		    Xq_FT_BUSINESS);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "ipmt",
		    i_intper, P_INTERN(P_NONE), ALL, XcR_funcIpmt,
		    Xq_FT_BUSINESS);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "nper",
		    i_npm, P_INTERN(P_NONE), ALL, XcR_funcNper,
		    Xq_FT_BUSINESS);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "ppmt",
		    i_princper, P_INTERN(P_NONE), ALL, XcR_funcPpmt,
		    Xq_FT_BUSINESS);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "syd",
		    i_syd, P_INTERN(P_NONE), ALL, XcR_funcSyd,
		    Xq_FT_BUSINESS);
 /*
   F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "rate",
   i_taux, P_INTERN(P_NONE), ALL, XcR_funcRate,
   Xq_FT_BUSINESS);
   F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "irr",
   i_tri, P_INTERN(P_NONE), ALL, XcR_funcIrr,
   Xq_FT_BUSINESS);
   */

 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "mirr",
		    i_trim, P_INTERN(P_NONE), ALL, XcR_funcMirr,
		    Xq_FT_BUSINESS);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "pv",
		    i_va, P_INTERN(P_NONE), ALL, XcR_funcPv,
		    Xq_FT_BUSINESS);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "npv",
		    i_van, P_INTERN(P_NONE), ALL, XcR_funcNpv,
		    Xq_FT_BUSINESS);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "fv",
		    i_vc, P_INTERN(P_NONE), ALL, XcR_funcFv,
		    Xq_FT_BUSINESS);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "vdb",
		    i_vdb, P_INTERN(P_NONE), ALL, XcR_funcVdb,
		    Xq_FT_BUSINESS);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "pmt",
		    i_vpm, P_INTERN(P_NONE), ALL, XcR_funcPmt,
		    Xq_FT_BUSINESS);

 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "not", 
		    i_not, P_INTERN(P_ANY), ALL, XcR_funcNot, Xq_FT_LOGICAL);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "or", 
		    i_or, P_INTERN(P_ANY), ALL, XcR_funcOr, Xq_FT_LOGICAL);
 F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "if", 
		    i_if, P_INTERN(P_ANY), ALL, XcR_funcIf, Xq_FT_LOGICAL);
 /*  F(ObjTmp).add_func(ObjTmp, &(ObjTmp->func), "combin", 
     i_combin, P_INTERN(P_ANY), ALL, XcR_funcIf, Xq_FT_LOGICAL);
     */

 /*
   croissance
   droitereg
   ecartype
   ecartypep
   frequence
   logreg
   mediane      
   rang
   tendance
   var
   varp
   */

 /*
   choisir
   index
   zones
   */


 Xc_TRACE(("Object Parser build"));
 return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
void dest_Parser(This)
c_Parser *This;
{
 def_var	*tvar;
 def_var	*tvar2;
 def_func	*tfunc;
 def_func	*tfunc2;

 tvar = This->var;
 while (tvar)
 {
  tvar2 = tvar->Next;
  Xc_free(tvar);
  tvar = tvar2;
 }

 tfunc = This->func;
 while (tfunc)
 {
  tfunc2 = tfunc->Next;
  Xc_free(tfunc);
  tfunc = tfunc2;
 }

 Xc_free(This);
 Xc_TRACE(("Object Parser destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copy method                                                       ** 
** ----------------------------------------------------------------- */
void *copy_Parser(This)
c_Parser *This;
{
 Xc_TRACE(("Object Parser copy not implemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** main parse func                                                   ** 
** note: dep_type should be sub_dep add_dep or ign_dep		     **
** ----------------------------------------------------------------- */
static int Parser_parse(This, string, x, y, cell_s, dep_type, sel_f)
c_Parser	*This;
char		*string;
int		x;
int		y;
cell		*cell_s;
int		dep_type;
c_Feuille	*sel_f;
{
 char		str_f[MAX_STRF_LEN];
 int		indexf;
 int		end_type;

 indexf = 0;
 This->x = x;
 This->y = y;
 This->cell_s = cell_s;
 cell_s->ret_type = 0;
 This->d_type = dep_type;
 current_depth = 0;
 type_d[0] = TYPE_VOID;
 Xc_HISTORY(("parsing for cell %d-%d %p", x, y, cell_s));
 if (F(This).rparse(This, string, str_f, &indexf, &end_type) == -1)
 {
  return -1;
 }
 else
 {
  if (dep_type != SUB_DEP)
  {
   char *tmp_str;
   if (end_type == TYPE_STRING)
   {
    tmp_str = (char *)Xc_malloc("string result", strlen(str_f)+1);
    strcpy(tmp_str, str_f);
    if (cell_s->text)
     Xc_free(cell_s->text);
    cell_s->text = tmp_str;
    if (cell_s->value2)
    {
     Xc_free(cell_s->value2);
     cell_s->value2 = NULL;
    }
    Xc_TRACE(("text %s", tmp_str));
   }
   else
   {
    str_f[indexf] = 0;
    Xc_TRACE(("final string: %s", str_f));
    GlobErrNum = 0;
    if (cell_s->value2)
    {
     Xc_free(cell_s->value2);
     cell_s->value2 = NULL;
    }
    cell_s->value2=lisexpr2(str_f);
    switch (GlobErrNum)
    {
    case 0:
     break;
    case 260:
     cell_s->status = TYPE_DIVZ_ERROR;
     return -1;
    default:
     cell_s->status = TYPE_CALC_ERROR;
     Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
     return -1;
    }
    /*
      Xc_TRACE(("cell_s->value2: %p", cell_s->value2));
      tmp_str = g2str(cell_s->value2, BIG_PREC);
      Xc_TRACE(("number %s", tmp_str));
      */
   }
  }
 }
 return 0;
}

/* ----------------------------------------------------------------- ** 
** recursive parse func                                              ** 
** ----------------------------------------------------------------- */
static int Parser_rparse(This, string, strf, indexf, end_type)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
int	*end_type;
{
 int	sortie;
 int	index;
 int	result;
 int	first;
 int	type;
 int	last_type;
  
 Xc_HISTORY(("start rparse"));
 first = 1;
 index = 0;
 sortie = 0;
 result = 0;
 last_type = TYPE_VOID;
 type_d[current_depth] = TYPE_VOID;
 while (!sortie)
 {
  while(string[index]==' ')
   index++;
  if (string[index] == 0)
   type = TYPE_PARSE_EOF;
  else
   result = F(This).get_token(This, string+index,
			      &first, &type,
			      strf, indexf);
  if ((type > TYPE_ERROR) || (type == TYPE_CELL_ZONE))
  {
   This->cell_s->status = type;
   Xc_TRACE(("error in parse variable not found"));
   return -1;
  }
  if (type == TYPE_STRING)
  {
   if (type_d[current_depth] != TYPE_VOID)
   {
    This->cell_s->status = TYPE_NOT_COMPATIBLE;
    return -1;
   }
   type_d[current_depth] = TYPE_TEXT;
   Xc_TRACE(("Mega BUT 15-15-30: %d", type));
   last_type = type;
   index += result;
   *end_type = type;
   continue;
  }

  if (type == TYPE_PARSE_BEGIN)
  {
   index += result;
   result = F(This).rparse(This, string+index, strf, indexf, end_type);
   first = 0;
  }
  if (result == -1)
  {
   Xc_TRACE(("error in parse"));
   return -1;
  }
  if ((type == TYPE_PARSE_END) ||
      (type == TYPE_PARSE_EOF) ||
      (type == TYPE_SEPARATOR))
  {
   Xc_TRACE(("last %d", last_type));
   switch (last_type)
   {
   case TYPE_OPERATOR:
   case TYPE_U_OPERATOR:
    This->cell_s->status = TYPE_ERROR;
    Xc_TRACE(("end with operator"));
    return -1;
    break;
   }
   index += result;
   break;
  }
  if (type_d[current_depth] == TYPE_TEXT)
  {
   This->cell_s->status = TYPE_NOT_COMPATIBLE;
   return -1;
  }
  type_d[current_depth] = type;
  if (current_depth == 0)
  {
   if ((type != TYPE_U_OPERATOR) &&
       (type != TYPE_PARSE_BEGIN) &&
       (type != TYPE_PARSE_END) &&
       (type != TYPE_OPERATOR))
   {
    if (type > This->cell_s->ret_type)
    {
     if (type != TYPE_PERCENT)
      This->cell_s->ret_type = type;
    }
   }
  }
  *end_type = type;
  Xc_TRACE(("Mega BUT 15-15-30: %d", type));
  last_type = type;
  index += result;
 }
 strf[*indexf]=0;
 Xc_HISTORY(("end parse"));
 return index;
}


static int Parser_compute_param(This, string, strf, indexf, type, gnum)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
int	*type;
GEN	*gnum;
{
 int	index;
 int	result;
 int	first;
 int	end_type;
 int	type2;
 int	old_indexf;
  
 Xc_HISTORY(("start compute param"));
 first = 1;
 index = 0;
 old_indexf = *indexf;

 if (string[index] == 0)
 {
  *type = TYPE_PARSE_EOF;
  result = 0;
 }
 else
 {
  old_indexf = *indexf;
  result = F(This).get_token(This, string+index,
			     &first, type,
			     strf, indexf);
 }
  
 if (*type > TYPE_ERROR)
 {
  This->cell_s->status = *type;
  Xc_TRACE(("error in parse variable not found"));
  return -1;
 }
 if ((*type == TYPE_CELL_ZONE) ||
     (*type == TYPE_PARSE_END) ||
     (*type == TYPE_STRING) ||
     (*type == TYPE_SEPARATOR))
 {
  return index+result;
 }

 if (result == -1)
 {
  Xc_TRACE(("error in parse"));
  return -1;
 }

 *indexf = old_indexf;
 strf[(*indexf)++]='(';
 result = F(This).srparse(This, string+index, strf, indexf, 
			  &end_type, &type2);
 if (result == -1)
 {
  Xc_TRACE(("error in parse"));
  return -1;
 }
 if ((end_type == TYPE_PARSE_END) ||
     (end_type == TYPE_SEPARATOR))
  *type = TYPE_PARAM_P;
 else
  return -1;
 strf[(*indexf)++]=')';  
 strf[*indexf] = 0;

 GlobErrNum = 0;
 *gnum=lisexpr2(strf+old_indexf);
 switch (GlobErrNum)
 {
 case 0:
  break;
 case 260:
  This->cell_s->status = TYPE_DIVZ_ERROR;
  return -1;
 default:
  This->cell_s->status = TYPE_CALC_ERROR;
  Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
  return -1;
 }
 *indexf = old_indexf;
 Xc_HISTORY(("end compute_param"));
 return index+result;
}

/* ----------------------------------------------------------------- ** 
** recursive parse func for multi parameters func                    ** 
** ----------------------------------------------------------------- */
static int Parser_srparse(This, string, strf, indexf, end_type, type2)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
int	*end_type;
int	*type2;
{
 int	sortie;
 int	index;
 int	result;
 int	first;
 int	type;
 int	last_type;
  
 Xc_HISTORY(("start srparse"));
 first = 1;
 index = 0;
 sortie = 0;
 result = 0;
 last_type = TYPE_VOID;
 type_d[current_depth] = TYPE_VOID;
 *type2=0;
 while (!sortie)
 {
  while(string[index]==' ')
   index++;
  if (string[index] == 0)
   type = TYPE_PARSE_EOF;
  else
   result = F(This).get_token(This, string+index,
			      &first, &type,
			      strf, indexf);

  if ((type == TYPE_CELL_ZONE))
  {
   *end_type = type;
   return index+result;
  }
  if ((type > TYPE_ERROR))
  {
   This->cell_s->status = type;
   Xc_TRACE(("error in parse variable not found"));
   return -1;
  }
  if (type == TYPE_STRING)
  {
   if (type_d[current_depth] != TYPE_VOID)
   {
    This->cell_s->status = TYPE_NOT_COMPATIBLE;
    return -1;
   }
   type_d[current_depth] = TYPE_TEXT;
   Xc_TRACE(("Mega BUT 15-15-30: %d", type));
   last_type = type;
   index += result;
   *end_type = type;
   continue;
  }

  if (type == TYPE_PARSE_BEGIN)
  {
   index += result;
   result = F(This).rparse(This, string+index, strf, indexf, end_type);
   first = 0;
  }
  if (result == -1)
  {
   Xc_TRACE(("error in parse"));
   return -1;
  }
  if ((type == TYPE_PARSE_END) ||
      (type == TYPE_PARSE_EOF) ||
      (type == TYPE_SEPARATOR))
  {
   Xc_TRACE(("last %d", last_type));
   switch (last_type)
   {
   case TYPE_OPERATOR:
   case TYPE_U_OPERATOR:
    This->cell_s->status = TYPE_ERROR;
    Xc_TRACE(("end with operator"));
    return -1;
    break;
   }
   *type2 = last_type;
   *end_type = type;
   index += result;
   break;
  }
  if (type_d[current_depth] == TYPE_TEXT)
  {
   This->cell_s->status = TYPE_NOT_COMPATIBLE;
   return -1;
  }
  type_d[current_depth] = type;
  if (current_depth == 0)
  {
   if ((type != TYPE_U_OPERATOR) &&
       (type != TYPE_OPERATOR))
   {
    if (type > This->cell_s->ret_type)
     This->cell_s->ret_type = type;
   }
  }
  *end_type = type;
  Xc_TRACE(("Mega BUT 15-15-30: %d", type));
  last_type = type;
  index += result;
 }
 Xc_HISTORY(("end parse"));
 return index;
}

/* ----------------------------------------------------------------- ** 
** cut word by word                                                  ** 
** ----------------------------------------------------------------- */
/*
  the "first" parameter indicate wether the
  token is the first at this depth of the
  parse; if so, we should have to look at the unary
  operator
  */
static int Parser_get_token(This, string, first, type, strf, indexf)
c_Parser *This;
char	*string;
int	*first;
int	*type;
char	*strf;
int	*indexf;
{
 int		index;
 int		ret_tok;

 /* skip space */
 index = 0;
 while (string[index] == ' ')
  index++;

 if (string[index] == 0)
 {
  return index;
 }

 Xc_TRACE(("%s",string+index));

 if (string[index] == ';')
 {
  *type = TYPE_SEPARATOR;
  return index+1;
 }
 if (string[index] == ')')
 {
  strf[(*indexf)++] = ')';
  *type = TYPE_PARSE_END;
  return index+1;
 }
 if (string[index] == '(')
 {
  strf[(*indexf)++] = '(';
  *type = TYPE_PARSE_BEGIN;
  return index+1;
 }


 if (*first)
 {

  /* search for a string */
  if (string[index] == '\"')	/*"*/
  {
   int	initial;
   char	*test_;
	  
   test_ = NULL;
   index++;
   initial = *indexf;
   while (string[index] && string[index]!='\"')/*"*/
   {
    strf[initial++] = string[index++];
    if (string[index]=='\"' && string[index+1]=='\"')/*"*/
    {
     strf[initial++] = '\"';	/*"*/
     index+=2;
    }
   }
   if (string[index]!='\"')	/*"*/
   {
    *type = TYPE_UNTERM_STRING;
    return -1;
   }
   index++;
   while(string[index]==' ')
    index++;
   strf[initial] = 0;
	  
   if (initial - *indexf)
   {
    test_=(char *)Xc_malloc("test-str", initial - *indexf + 1);
    strcpy(test_, strf+*indexf);
    /* string is a date? */
    ret_tok = get_token_date(This, test_, type, strf, indexf);
    if (ret_tok != -1)
    {
     *first = 0;
     Xc_free(test_);
     return index;
    }
    /* string is a hour? */
    ret_tok = get_token_hour(This, test_, type, strf, indexf);
    if (ret_tok != -1)
    {
     *first = 0;
     Xc_free(test_);
     return index;
    }
    /* string is a day in week? */
    ret_tok = get_token_day(This, test_, type, strf, indexf);
    if (ret_tok != -1)
    {
     *first = 0;
     Xc_free(test_);
     return index;
    }
    /* string is a month ? */
    ret_tok = get_token_month(This, test_, type, strf, indexf);
    if (ret_tok != -1)
    {
     *first = 0;
     Xc_free(test_);
     return index;
    }
    /* string is a number? */
    ret_tok = get_token_number(This, string, type, strf, indexf);
    if (ret_tok != -1)
    {
     *first = 0;
     Xc_free(test_);
     return index;
    }
   }
   if (test_)
    Xc_free(test_);
   *indexf = initial;
   *type = TYPE_STRING;
   return index;
  }


  /* search for a cell or cell group */
  ret_tok = get_token_zone(This, string, type, strf, indexf);
  if (ret_tok != -1)
  {
   *first = 0;
   index += ret_tok;
   return index;
  }

  /* search for a variable */
      
  /* search for a boolean */
  ret_tok = get_token_boolean(This, string, type, strf, indexf);
  if (ret_tok != -1)
  {
   *first = 0;
   index += ret_tok;
   return index;
  }

  /* search for a function */
  ret_tok = get_token_function(This, string, type, strf, indexf);
  if (ret_tok != -1)
  {
   *first = 0;
   index += ret_tok;
   return index;
  }

  /* search for a number */
  ret_tok = get_token_number(This, string, type, strf, indexf);
  if (ret_tok != -1)
  {
   *first = 0;
   index += ret_tok;
   return index;
  }
	   
  /* search for an unary operator */
  ret_tok = get_token_unaryop(This, string, type, strf, indexf);
  if (ret_tok != -1)
  {
   *first = 1;
   index += ret_tok;
   return index;
  }
 }
 else
 {
  /* search for an binary operator */
  ret_tok = get_token_binaryop(This, string, type, strf, indexf);
  if (ret_tok != -1)
  {
   *first = 1;
   index += ret_tok;
   return index;
  }
 }
 *type = TYPE_ERROR;
 return -1;
}

static void Parser_add_func(This, func_list, name, outname, param_nbr,
			    domain, resource, type)
c_Parser 	*This;
def_func	**func_list;
void		*name;
char		*outname;
int		param_nbr;
int		domain;
int		resource;
int		type;
{
 def_func	**tmpfunc;

 tmpfunc = func_list;
 while (*tmpfunc)
 {
  tmpfunc = &(*tmpfunc)->Next;
 }
 *tmpfunc = (def_func *)Xc_malloc("func", sizeof(def_func));
 (*tmpfunc)->name = F(GlobResources).getString(GlobResources, 
					       resource);
 (*tmpfunc)->savename = name;
 Xc_TRACE(("in:%s save:%s", (*tmpfunc)->name, (*tmpfunc)->savename));

 /* is it an internal function */
 if (param_nbr <= P_INONE)
 {
  (*tmpfunc)->outname = NULL;
  (*tmpfunc)->func = (void *)outname;
 }
 else
 {
  (*tmpfunc)->outname = (char *)outname;
  (*tmpfunc)->func = NULL;
 }
 (*tmpfunc)->outsize = strlen(outname);
 (*tmpfunc)->nbr_param = param_nbr;
 (*tmpfunc)->domain = domain;
 (*tmpfunc)->type = type;
 (*tmpfunc)->Next = NULL;
}

/* ----------------------------------------------------------------- ** 
** translate language                                                ** 
** ----------------------------------------------------------------- */
static char *trans_language(This, string, save)
c_Parser	*This;
char		*string;
boolean		save;
{
 char		*str_trans;
 int		index;
 int		index_trans;
 def_func	*tmpfunc;
 int		e_index;
 int		size;
 char		*tok_str;
 int		len;

 index = 0;
 index_trans = 0;
 len = strlen(string)*5;
 str_trans = Xc_malloc("trans_lang", len);
 while (string[index])
 {
  while(string[index]==' ')
  {
   str_trans[index_trans++]=string[index++];
  }

  if (string[index] == '\"')	/*"*/
  {
   str_trans[index_trans++]=string[index++];
   while (string[index] && string[index]!='\"')/*"*/
   {
    str_trans[index_trans++]=string[index++];
    if (string[index]=='\"' &&	/*"*/
	string[index+1]=='\"')	/*"*/
    {
     str_trans[index_trans++]=string[index++];
     str_trans[index_trans++]=string[index++];
    }
   }
   if (string[index])
    str_trans[index_trans++]=string[index++];
   continue;
  }

  size = 0; tok_str = NULL;
  if (Xc_IS_LETTER(string[index])
      || (string[index] == '$')
      || (string[index] == '_'))
  {
   e_index = index;
   while (Xc_IS_LETTER(string[e_index])
	  || (string[e_index] == '$')
	  || (string[e_index] == '_')
	  || (string[e_index] == '.')
	  || (string[e_index] == ':')
	  || ((string[e_index] >= '0' && string[e_index] <= '9')))
    e_index++;
   size = e_index - index;
   tok_str = (char *)Xc_malloc("tok_str1", size+1);
   strncpy(tok_str, string+index, size);
   tok_str[size] = 0;
   Xc_TRACE(("Token: %s", tok_str));
  }

  if (size)
  {
   tmpfunc = This->func;
   if (save)
   {
    while (tmpfunc)
    {
     if (Xc_strcmp_parser(tok_str, tmpfunc->name))
     {
      strcpy(str_trans + index_trans, tmpfunc->savename);
      index_trans += strlen(tmpfunc->savename);
      index += size;
      break;
     }
     tmpfunc = tmpfunc->Next;
    }
   }
   else
   {
    while (tmpfunc)
    {
     if (Xc_strcmp_parser(tok_str, tmpfunc->savename))
     {
      strcpy(str_trans+index_trans, tmpfunc->name);
      index_trans += strlen(tmpfunc->name);
      index += size;
      break;
     }
     tmpfunc = tmpfunc->Next;
    }
   }
   if (!tmpfunc)
   {
    /* copie le token originel */
    strncpy(str_trans+index_trans, string+index, size);
    index_trans += size;
    index += size;
   }
  }
  else
   str_trans[index_trans++]=string[index++];
  if (tok_str) Xc_free(tok_str);
 }
 str_trans[index_trans++]=string[index++];
 return str_trans;
}



