/*
** Parser_date.c for XQuad in Parser/
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
** Last update Thu Jan  6 23:37:28 2000 Emmanuel Paris
*/

#include "Parser.h"

/* ----------------------------------------------------------------- **
** day360(date_start; date_end) return base 360 number of day        **
** ----------------------------------------------------------------- */
int i_day360(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;
 double	date_start;
 xc_date_t	xc_date_s;
 double	date_end;
 xc_date_t	xc_date_e;
 int		old_indexf;
 GEN		calc_x;
 int		result;
 ulong		avma_old;

 Xc_TRACE(("internal day360"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  date_start = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  date_start = gtodouble(calc_x);
  avma = avma_old;
  index += nindex-1;
  if (string[index]==';')
   index++;
  else
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 if (string[index]==')')
 {
  index++;
  date_end = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }
      
  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  date_end = gtodouble(calc_x);
  avma = avma_old;
  index += nindex-1;
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 convert_to_date(date_start, &xc_date_s);
 convert_to_date(date_end, &xc_date_e);
 result = 360*(xc_date_e.year - xc_date_s.year) 
  + 30*(xc_date_e.month - xc_date_s.month)
   + (xc_date_e.day - xc_date_s.day);
 sprintf(strf+*indexf, "(%d)", result);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_NUM;
 return index;
}

/* ----------------------------------------------------------------- **
** today() return today date                                         **
** ----------------------------------------------------------------- */
int i_today(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 xc_date_t	xc_date_a;
 double	rdate_num;
 int		index;

 Xc_TRACE(("internal today"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 if (string[index] != ')')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 fill_xc_date(&xc_date_a);
 rdate_num = convert_to_xc_date(&xc_date_a, NULL);  
 sprintf(strf+*indexf, "(%f)", rdate_num);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_DATE;
 return index+1;
}

/* ----------------------------------------------------------------- **
** date(annee; mois; jours) return serial number                     **
** ----------------------------------------------------------------- */
int i_date(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 xc_date_t	xc_date;
 int		index;
 int		nindex;
 int		end_type;
 int		annee;
 int		mois;
 int		jours;
 double	_dbl;
 int		old_indexf;
 GEN		calc_x;
 int		result;
 ulong		avma_old;

 Xc_TRACE(("internal date"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  annee = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  annee = DB_TO_INT(_dbl);
  if (annee >= 0)
  {
   if (annee < 20)
    annee += 2000;
   else
    if (annee < 100)
     annee += 1900;
  }
  index += nindex-1;
  if (string[index]==';')
   index++;
  else
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 /* look for second parameter number */
 if (string[index]==';')
 {
  index++;
  mois = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  mois = DB_TO_INT(_dbl);
  index += nindex-1;
  if (string[index]==';')
   index++;
  else
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 /* look for third parameter number */
 if (string[index]==')')
 {
  index++;
  jours = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  jours = DB_TO_INT(_dbl);
  index += nindex-1;
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }

 mois--;
 xc_date.year = annee + mois / 12;
 if (xc_date.year >= 0 && annee < 0)
  xc_date.year ++;
 else
  if (xc_date.year <= 0 && annee > 0)
   xc_date.year --;
 xc_date.month = (mois % 12) + 1;
 xc_date.day = 1;
 result = convert_to_xc_date(&xc_date, NULL) + (double)(jours-1);
 sprintf(strf+*indexf, "(%d)", result);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_DATE;
 return index;
}

/* ----------------------------------------------------------------- **
** dateval(date) return serial number                                **
** ----------------------------------------------------------------- */
int i_dateval(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;
 double	_dbl;
 int		result;
 int		old_indexf;
 GEN		calc_x;
 ulong		avma_old;

 Xc_TRACE(("internal dateval"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==')')
 {
  index++;
  result = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  result = DB_TO_INT(_dbl);
  index += nindex-1;
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 sprintf(strf+*indexf, "(%d)", result);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_NUM;
 return index;
}

/* ----------------------------------------------------------------- **
** now() return today date and time                                  **
** ----------------------------------------------------------------- */
int i_now(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 xc_date_time_t	xc_date_time_a;
 double		rdate_num;
 int			index;

 Xc_TRACE(("internal now"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 if (string[index] != ')')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 fill_xc_date_and_time(&xc_date_time_a);
 rdate_num = convert_to_xc_date_and_time(&xc_date_time_a, NULL);  
 sprintf(strf+*indexf, "(%f)", rdate_num);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_DATE_TIME;
 return index+1;
}


/* ----------------------------------------------------------------- **
** time(heure; minute; seconde; miliseconde) return serial number    **
** ----------------------------------------------------------------- */
int i_time(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;
 int	       	heure;
 int		minute;
 int		seconde;
 int		milli_seconde;
 double	_dbl;
 int		old_indexf;
 GEN		calc_x;
 ulong		avma_old;

 Xc_TRACE(("internal time"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  heure = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  heure = DB_TO_INT(_dbl);
  index += nindex-1;
  if (string[index]==';')
   index++;
  else
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 /* look for second parameter number */
 if (string[index]==';')
 {
  index++;
  minute = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  minute = DB_TO_INT(_dbl);
  index += nindex-1;
  if (string[index]==';')
   index++;
  else
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 /* look for third parameter number */
 milli_seconde = 0;
 if (string[index]==')' || string[index]==';')
 {
  index++;
  seconde = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  seconde = DB_TO_INT(_dbl);
  index += nindex-1;

  if (string[index]==';')
  {
   index++;
   old_indexf = *indexf;
   nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
   if (nindex < 2)
    return -1;
   if (end_type == TYPE_STRING)
   {
    This->cell_s->status = TYPE_NO_PARAM_NUM;
    return -1;
   }

   GlobErrNum = 0;
   avma_old = avma;
   calc_x=lisexpr(strf+old_indexf);
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
   _dbl = gtodouble(calc_x);
   avma = avma_old;
   milli_seconde = DB_TO_INT(_dbl);
   index += nindex-1;
  }
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 _dbl = heure*60*60*1000 + minute*60*1000 + seconde*1000 + milli_seconde;
 _dbl /= Xc_TIME_FRACTION;
 sprintf(strf+*indexf, "(%f)", _dbl);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_TIME;
 return index;
}

/* ----------------------------------------------------------------- **
** timeval(time) return serial number                                **
** ----------------------------------------------------------------- */
int i_timeval(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;

 Xc_TRACE(("internal dateval"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 strf[*indexf]='(';
 (*indexf)++;
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==')')
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  return -1;
 }
 else
 {
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  index+=nindex-1;
  strf[*indexf]=0;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 This->ret_type = TYPE_NUM;
 return index;
}

/* ----------------------------------------------------------------- **
** year(date) return year                                            **
** ----------------------------------------------------------------- */
int i_year(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;
 double	_dbl;
 xc_date_t	xc_date;
 int		old_indexf;
 GEN		calc_x;
 ulong		avma_old;

 Xc_TRACE(("internal year"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==')')
 {
  index++;
  _dbl = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  index += nindex-1;
  while (string[index]==' ')
   index++;
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 convert_to_date(_dbl, &xc_date);
 sprintf(strf+*indexf, "(%d)", xc_date.year);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_NUM;
 return index;
}

int i_day(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;
 double	_dbl;
 xc_date_t	xc_date;
 int		old_indexf;
 GEN		calc_x;
 ulong		avma_old;

 Xc_TRACE(("internal day"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==')')
 {
  index++;
  _dbl = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  index += nindex-1;
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 convert_to_date(_dbl, &xc_date);  
 sprintf(strf+*indexf, "(%d)", xc_date.day);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_NUM;
 return index;
}

/* ----------------------------------------------------------------- **
** dayweek(date; number) return year                                  **
** ----------------------------------------------------------------- */
int i_dayweek(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;
 double	_dbl;
 long		_lng;
 xc_date_t	xc_date;
 int		old_indexf;
 GEN		calc_x;
 ulong		avma_old;

 Xc_TRACE(("internal dayweek"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 _lng = 1;
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==')')
 {
  index++;
  _dbl = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  index += nindex-1;

  if (string[index]==';')
  {
   index++;
   old_indexf = *indexf;
   nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
   if (nindex < 2)
    return -1;
   if (end_type == TYPE_STRING)
   {
    This->cell_s->status = TYPE_NO_PARAM_NUM;
    return -1;
   }

   GlobErrNum = 0;
   avma_old = avma;
   calc_x=lisexpr(strf+old_indexf);
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
   _lng = gtolong(calc_x);
   avma = avma_old;
   index += nindex-1;
  }
  if ((_lng < 1) || (_lng > 7))
  {
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   return -1;
  }
	
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 convert_to_date(_dbl, &xc_date);
 sprintf(strf+*indexf, "(%d)", (xc_date.day_in_week-(int)_lng+8)%7+1);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_NUM;
 return index;
}

int i_hour(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;
 double	_dbl;
 xc_time_t	xc_time;
 int		old_indexf;
 GEN		calc_x;
 ulong		avma_old;

 Xc_TRACE(("internal hour"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==')')
 {
  index++;
  _dbl = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  index += nindex-1;
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 convert_to_time(_dbl, &xc_time);
 sprintf(strf+*indexf, "(%d)", xc_time.hour);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_NUM;
 return index;
}

int i_minute(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;
 double	_dbl;
 xc_time_t	xc_time;
 int		old_indexf;
 GEN		calc_x;
 ulong		avma_old;

 Xc_TRACE(("internal minute"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==')')
 {
  index++;
  _dbl = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  index += nindex-1;
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 convert_to_time(_dbl, &xc_time);
 sprintf(strf+*indexf, "(%d)", xc_time.minute);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_NUM;
 return index;
}

int i_month(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;
 double	_dbl;
 xc_date_t	xc_date;
 int		old_indexf;
 GEN		calc_x;
 ulong		avma_old;

 Xc_TRACE(("internal month"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==')')
 {
  index++;
  _dbl = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  index += nindex-1;
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 convert_to_date(_dbl, &xc_date);  
 sprintf(strf+*indexf, "(%d)", xc_date.month);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_NUM;
 return index;
}

int i_second(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;
 double	_dbl;
 xc_time_t	xc_time;
 int		old_indexf;
 GEN		calc_x;
 ulong		avma_old;

 Xc_TRACE(("internal second"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==')')
 {
  index++;
  _dbl = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  index += nindex-1;
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 convert_to_time(_dbl, &xc_time);
 sprintf(strf+*indexf, "(%d)", xc_time.second);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_NUM;
 return index;
}

int i_milli(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int		index;
 int		nindex;
 int		end_type;
 double	_dbl;
 xc_time_t	xc_time;
 int		old_indexf;
 GEN		calc_x;
 ulong		avma_old;

 Xc_TRACE(("internal milli"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==')')
 {
  index++;
  _dbl = 0;
 }
 else
 {
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }

  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
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
  _dbl = gtodouble(calc_x);
  avma = avma_old;
  index += nindex-1;
  if (string[index]==')')
   index++;
  else
  {
   if (string[index]==Xq_SEPAR)
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   else
    This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
 }
 convert_to_time(_dbl, &xc_time);
 sprintf(strf+*indexf, "(%d)", xc_time.millisecond);
 while (strf[*indexf])
  (*indexf)++;
 This->ret_type = TYPE_NUM;
 return index;
}

