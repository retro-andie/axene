/*
** Parser_finance.c for XQuad in Parser/
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
** Last update Fri Oct  2 17:25:41 1998 Robin Castanier
*/

#include "Parser.h"
extern jmp_buf G_erreur;

/* ----------------------------------------------------------------- ** 
** Amortissement lineaire(cout; valeur_residuel; duree)              ** 
** ----------------------------------------------------------------- */
int i_amorlin(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	old_indexf;
 int	index;
 int	nindex;
 int	t_index;
 int	ini;
 int	end_type;
 GEN	duree;

 Xc_TRACE(("internal amorlin"));
 index = 0;
 while (string[index]==' ')
  index++;  
 if (string[index] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 ini = *indexf;
 strf[(*indexf)++]='(';
 strf[(*indexf)++]='(';
 index++;
 t_index = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;
 index += nindex;
 if (end_type == TYPE_STRING)
 {
  This->cell_s->status = TYPE_NO_PARAM_NUM;
  return -1;
 }

 if (string[index-1]==Xq_SEPAR)
 {
  strf[(*indexf)++]='-';
  while (string[index]==' ')
   index++;
 }
 else 
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  return -1;
 }
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;
 index += nindex;
 if (end_type == TYPE_STRING)
 {
  This->cell_s->status = TYPE_NO_PARAM_NUM;
  return -1;
 }
 if (string[index-1]==Xq_SEPAR)
 {
  strf[(*indexf)++]=')';
  strf[(*indexf)++]='/';
  while (string[index]==' ')
   index++;
 }
 else 
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  return -1;
 }
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;
 index += nindex;
 if (end_type == TYPE_STRING)
 {
  This->cell_s->status = TYPE_NO_PARAM_NUM;
  return -1;
 }
 GlobErrNum = 0;
 duree=lisexpr(strf+old_indexf);
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
 if (gcmp(duree, gzero) <= 0)
 {
  This->cell_s->status = TYPE_VALUE_ERROR;
  return -1;
 }

 if (string[index-1]!=')')
 {
  if (string[index-1]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 This->ret_type = TYPE_NUM;
 return index;
}

/* -------------------------------------------------------------------- ** 
** Amortissement double(cout; valeur_residuel; duree; periode; mois)    ** 
** -------------------------------------------------------------------- */
int i_db(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	cout;
 GEN	val_res;
 GEN	duree;
 int	periode;
 int	periode_save;
 GEN	mois;
 GEN	gtmp;
 GEN	gtmp2;
 GEN	gtaux;
 GEN	gresult;
 double _dbl;
 char	*tmp_str;

 Xc_TRACE(("internal db"));

 if (setjmp(G_erreur))
 {
  switch (GlobErrNum)
  {
  case 260:
   This->cell_s->status = TYPE_DIVZ_ERROR;
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   return -1;
  }
 }

 mois = gzero;
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 strf[(*indexf)++]='(';
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  cout = gzero;
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
  cout=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for second parameter number */
 if (string[index]==';')
 {
  index++;
  val_res=gzero;
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
  val_res=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for third parameter number */
 if (string[index]==';')
 {
  index++;
  duree=gzero;
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
  duree=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (gcmp(duree, gzero) <= 0)
  {
   This->cell_s->status = TYPE_VALUE_ERROR;
   return -1;
  }

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for fourth parameter number */
 if (string[index]==')' || string[index]==';')
 {
  index++;
  periode = 0;
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
  gtmp=lisexpr(strf+old_indexf);
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
  if (gcmp(gtmp, gzero) <= 0)
  {
   This->cell_s->status = TYPE_VALUE_ERROR;
   return -1;
  }
  _dbl = gtodouble(gtmp);
  periode = DB_TO_INT(_dbl);
  index += nindex-1;

  while (string[index]==' ')
   index++;
  if (string[index]==';')
  {
   index++;
   if (string[index]==')')
   {
    mois = gzero;
   }
   else
   {
    old_indexf = *indexf;
    nindex = F(This).rparse(This, string+index, strf,
			    indexf, &end_type);
    if (nindex < 2)
     return -1;
    if (end_type == TYPE_STRING)
    {
     This->cell_s->status = TYPE_NO_PARAM_NUM;
     return -1;
    }
	      
    GlobErrNum = 0;
    mois = lisexpr(strf+old_indexf);
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
    index += nindex-1;
   }
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
  
 periode_save = periode;
 gtmp = gdiv(gun, duree);
 gtmp2 = gdiv(val_res, cout);
 gtaux = gpui(gtmp2, gtmp, LOW_PREC);
 gtaux = gsub(gun, gtaux);

 _dbl = gtodouble(duree);
 if (!gcmp0(mois))
 {
  if (DB_TO_INT(_dbl)+1 < periode)
  {
   This->cell_s->status = TYPE_VALUE_ERROR;
   return -1;
  }
  gtmp = gmul(cout, gtaux);
  gtmp = gmul(gtmp, mois);
  gtmp2 = stoi((long)12);
  gtmp2 = gdiv(gtmp, gtmp2);
  gtmp = gtmp2;
  periode --;
 }
 else
 {
  if (DB_TO_INT(_dbl) < periode)
  {
   This->cell_s->status = TYPE_VALUE_ERROR;
   return -1;
  }
  gtmp2 = gzero;
 }

 if (periode_save == DB_TO_INT(_dbl)+1)
 {
  periode--;
 }

 while (periode)
 {
  gresult = gsub(cout, gtmp2);
  gtmp = gmul(gresult, gtaux);
  gtmp2 = gadd(gtmp2, gtmp);
  periode --;
 }

 periode = DB_TO_INT(_dbl);
 if (periode_save == periode+1)
 {
  if (!gcmp0(mois))
  {
   gresult = gsub(cout, gtmp2);
   gtmp = gmul(gresult, gtaux);
   gtmp2 = stoi((long)12);
   gresult = gsub(gtmp2, mois);
   gtmp = gmul(gresult, gtmp);
   gtmp = gdiv(gtmp, gtmp2);
  }
 }

 tmp_str = g2str(gtmp, BIG_PREC);
 sprintf(strf+*indexf, "%s)", tmp_str);
 (*indexf) += strlen(tmp_str)+1;
 free(tmp_str);  
 This->ret_type = TYPE_NUM;
 return index;
}

/* -------------------------------------------------------------------- ** 
** Amortissement double(cout; valeur_residuel; duree; periode; [facteur]) ** 
** -------------------------------------------------------------------- */
int i_ddb(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	cout;
 GEN	val_res;
 GEN	duree;
 int	periode;
 GEN	facteur;
 GEN	gtmp;
 GEN	gtmp2;
 GEN	gresult;
 double _dbl;
 char	*tmp_str;

 Xc_TRACE(("internal ddb"));

 if (setjmp(G_erreur))
 {
  switch (GlobErrNum)
  {
  case 260:
   This->cell_s->status = TYPE_DIVZ_ERROR;
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   return -1;
  }
 }

 facteur = gdeux;
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 strf[(*indexf)++]='(';
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  cout = gzero;
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
  cout=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for second parameter number */
 if (string[index]==';')
 {
  index++;
  val_res=gzero;
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
  val_res=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for third parameter number */
 if (string[index]==';')
 {
  index++;
  duree=gzero;
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
  duree=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (gcmp(duree, gzero) <= 0)
  {
   This->cell_s->status = TYPE_VALUE_ERROR;
   return -1;
  }

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for fourth parameter number */
 if (string[index]==')' || string[index]==';')
 {
  index++;
  periode = 0;
  gtmp = gzero;
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
  gtmp = lisexpr(strf+old_indexf);
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
  if (gcmp(gtmp, gzero) <= 0)
  {
   This->cell_s->status = TYPE_VALUE_ERROR;
   return -1;
  }
  _dbl = gtodouble(gtmp);
  periode = DB_TO_INT(_dbl);
  index += nindex-1;

  while (string[index]==' ')
   index++;
  if (string[index]==';')
  {
   index++;
   if (string[index]==')')
   {
    facteur = gzero;
   }
   else
   {
    old_indexf = *indexf;
    nindex = F(This).rparse(This, string+index, strf,
			    indexf, &end_type);
    if (nindex < 2)
     return -1;
    if (end_type == TYPE_STRING)
    {
     This->cell_s->status = TYPE_NO_PARAM_NUM;
     return -1;
    }
	      
    GlobErrNum = 0;
    facteur = lisexpr(strf+old_indexf);
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
    index += nindex-1;
   }
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
 
 gtmp2 = gzero;
 gresult = cout;

 _dbl = gtodouble(duree);
 if (DB_TO_INT(_dbl) < periode)
 {
  This->cell_s->status = TYPE_VALUE_ERROR;
  return -1;
 }

 while (periode)
 {
  gtmp = gmul(gresult, facteur);
  gtmp = gdiv(gtmp, duree);
  gtmp2 = gadd(gtmp2, gtmp);
  gresult = gsub(cout, gtmp2);
  if (gcmp(gresult, val_res) <= 0)
  {
   periode --;
   if (periode)
   {
    gtmp = gzero;
   }
   else
   {
    gresult= gsub(gtmp2, gtmp);
    gtmp2 = gsub(cout, val_res);
    gtmp = gsub(gtmp2, gresult);
   }
   break;
  }
  periode --;
 }

 tmp_str = g2str(gtmp, BIG_PREC);
 sprintf(strf+*indexf, "%s)", tmp_str);
 (*indexf) += strlen(tmp_str)+1;
 free(tmp_str);  
 This->ret_type = TYPE_NUM;
 return index;
}

/* -------------------------------------------------------------------- ** 
** Remboursement d'interet(taux; per; npm; va; [vc; type]) p.190        ** 
** -------------------------------------------------------------------- */
int i_intper(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;
  int	old_indexf;
  GEN	taux;
  GEN	per;
  GEN	npm;
  GEN	va;
  GEN	vc;
  GEN	type;
  GEN	One_r;
  GEN	gtmp;
  GEN	gtmp2;
  GEN	gtmp3;
  char	*tmp_str;
  boolean	parse_ended;
  int	precis;

  Xc_TRACE(("internal intper"));
  if (setjmp(G_erreur))
  {
   switch (GlobErrNum)
   {
   case 260:
    This->cell_s->status = TYPE_DIVZ_ERROR;
    return -1;
   default:
    This->cell_s->status = TYPE_CALC_ERROR;
    Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
    return -1;
   }
  }

  parse_ended = FALSE;
  vc = gzero;
  type = gzero;
  if (string[0] != '(')
  {
   This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
  strf[(*indexf)++]='(';
  index = 1;
  while (string[index]==' ')
   index++;
  /* look for first parameter number */
  if (string[index]==';')
  {
   index++;
   taux = gzero;
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
   taux=lisexpr(strf+old_indexf);
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
   index += nindex-1;

   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
   else 
   {
    This->cell_s->status = TYPE_PARAM_LESS_ERROR;
    return -1;
   }
  }
  while (string[index]==' ')
   index++;
  /* look for second parameter number */
  if (string[index]==';')
  {
   index++;
   per = gzero;
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
   per = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (gcmp(per, gzero) <= 0)
   {
    This->cell_s->status = TYPE_VALUE_ERROR;
    return -1;
   }

   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
   else 
   {
    This->cell_s->status = TYPE_PARAM_LESS_ERROR;
    return -1;
   }
  }
  while (string[index]==' ')
   index++;
  /* look for third parameter number */
  if (string[index]==';')
  {
   index++;
   npm = gzero;
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
   npm = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (gcmp(npm, gzero) <= 0)
   {
    This->cell_s->status = TYPE_VALUE_ERROR;
    return -1;
   }

   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
   else 
   {
    This->cell_s->status = TYPE_PARAM_LESS_ERROR;
    return -1;
   }
  }
  while (string[index]==' ')
   index++;
  
  /* look for fourth parameter number */
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';' || string[index]==')')
  {
   index++;
   va = gzero;
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
   va = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
  }
  while (string[index]==' ')
   index++;
  
  /* look for fifth parameter number optiponal*/
  if (!parse_ended)
  {
   if (string[index]==')')
    parse_ended = TRUE;
   if (string[index]==';' || string[index]==')')
   {
    index++;
    vc = gzero;
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
    vc = lisexpr(strf+old_indexf);
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
    index += nindex-1;
    if (string[index]==')')
    {
     parse_ended = TRUE;
     index++;
    }
    if (string[index]==Xq_SEPAR)
    {
     index++;
    }
   }
   while (string[index]==' ')
    index++;
  }
  
  /* look for sixth parameter number optionnal */
  if (!parse_ended)
  {
   if (string[index]==')')
    parse_ended = TRUE;
   if (string[index]==';')
   {
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
    return -1;
   }
   if (string[index]==')')
   {
    index++;
    type = gzero;
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
    type = lisexpr(strf+old_indexf);
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
    index += nindex-1;
    if (string[index]==')')
    {
     parse_ended = TRUE;
     index++;
    }
    if (string[index]==Xq_SEPAR)
    {
     This->cell_s->status = TYPE_PARAM_MORE_ERROR;
     return -1;
    }
   }
  }
  
  if (gcmp(npm, per) < 0)
  {
   This->cell_s->status = TYPE_VALUE_ERROR;
   return -1;
  }
  
  if (!gcmp0(taux))
  {
   /* 1 + r */
   One_r = gadd(gun, taux);
   /* -(T - (t-1))= t - 1 -T */
   gtmp = gsub(per, gun);
   gtmp = gsub(gtmp, npm);
   /* (1 + r)^-(T-(t-1)) */
   precis = precision(gtmp);
   gtmp = gpui(One_r, gtmp, precis);
   /* 1 - (1 + r)^-(T-t-1) */
   gtmp = gsub(gun, gtmp);
      
   /* (1 + r)^-T */
   gtmp2 = gneg(npm);
   gtmp2 = gpui(One_r, gtmp2, precis);
   /* (1 + r)^-T - 1 */
   gtmp2 = gsub(gtmp2, gun);
      
   gtmp = gdiv(gtmp, gtmp2);
      
   if (!gcmp0(vc))
   {
    Xc_TRACE(("hello I"));
    gtmp2 = gsub(per, gun);
    gtmp2 = gpui(One_r, gtmp2, precis);
    gtmp2 = gsub(gun, gtmp2);

    gtmp3 = gpui(One_r, npm, precis);
    gtmp3 = gsub(gtmp3, gun);

    gtmp2 = gdiv(gtmp2, gtmp3);
    gtmp2 = gmul(vc, gtmp2);

    gtmp = gsub(gtmp, gtmp2);
   }

   gtmp = gmul(va, gtmp);
   gtmp = gmul(taux, gtmp);
   if (!gcmp0(type))
   {
    Xc_TRACE(("hello II"));
    gtmp = gdiv(gtmp, One_r);
   }
  }
  else
  {
   gtmp = gzero;
  }
  
  tmp_str = g2str(gtmp, BIG_PREC);
  sprintf(strf+*indexf, "%s)", tmp_str);
  (*indexf) += strlen(tmp_str)+1;
  free(tmp_str);  
  This->ret_type = TYPE_NUM;
  return index;
 }

/* -------------------------------------------------------------------- ** 
** Nombre de periode(taux; vpm; va; [vc; type]) p.157                   ** 
** -------------------------------------------------------------------- */
int i_npm(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{

 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	taux;
 GEN	vpm;
 GEN	va;
 GEN	vc;
 GEN	type;
 GEN	gtmp;
 GEN	gtmp2;
 GEN	gtmp3;
 char	*tmp_str;
 boolean	parse_ended;

 Xc_TRACE(("internal npm"));
 if (setjmp(G_erreur))
 {
  switch (GlobErrNum)
  {
  case 260:
   This->cell_s->status = TYPE_DIVZ_ERROR;
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   return -1;
  }
 }

 parse_ended = FALSE;
 vc = gzero;
 type = gzero;
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 strf[(*indexf)++]='(';
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  taux = gzero;
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
  taux=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for second parameter number */
 if (string[index]==';')
 {
  index++;
  vpm = gzero;
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
  vpm = lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for third parameter number */
 if (string[index]==')')
  parse_ended = TRUE;
 if (string[index]==';' || string[index]==')')
 {
  index++;
  va = gzero;
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
  va = lisexpr(strf+old_indexf);
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
  index += nindex-1;
  if (string[index]==')')
  {
   parse_ended = TRUE;
   index++;
  }
  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for fourth parameter number optional */
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';' || string[index]==')')
  {
   index++;
   vc = gzero;
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
   vc = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
  }
  while (string[index]==' ')
   index++;
 }

 /* look for fifth parameter number optional */
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';')
  {
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   return -1;
  }
  if (string[index]==')')
  {
   index++;
   type = gzero;
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
   type = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   if (string[index]==Xq_SEPAR)
   {
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
    return -1;
   }
  }
 }

 if (!gcmp0(type))
  gtmp = gadd(gun, taux);
 else
  gtmp = gun;
 gtmp = gmul(gtmp, vpm);
 gtmp = gdiv(gtmp, taux);

 gtmp2 = gsub(gtmp, vc);
 gtmp3 = gadd(gtmp, va);
 gtmp = gdiv(gtmp2, gtmp3);
 gtmp = glog(gtmp, LOW_PREC);

 gtmp2 = gadd(gun, taux);
 gtmp2 = glog(gtmp2, LOW_PREC);

 gtmp = gdiv(gtmp, gtmp2);

 tmp_str = g2str(gtmp, BIG_PREC);
 sprintf(strf+*indexf, "%s)", tmp_str);
 (*indexf) += strlen(tmp_str)+1;
 free(tmp_str);  
 This->ret_type = TYPE_NUM;
 return index;
}

/* -------------------------------------------------------------------- ** 
** Remboursement de principal(taux; per; npm; va; [vc; type]) p.187     ** 
** -------------------------------------------------------------------- */
int i_princper(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	taux;
 GEN	per;
 GEN	npm;
 GEN	va;
 GEN	vc;
 GEN	type;
 GEN	One_r;
 GEN	gtmp;
 GEN	gtmp2;
 GEN	gtmp3;
 char	*tmp_str;
 boolean	parse_ended;

 Xc_TRACE(("internal princper"));
 if (setjmp(G_erreur))
 {
  switch (GlobErrNum)
  {
  case 260:
   This->cell_s->status = TYPE_DIVZ_ERROR;
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   return -1;
  }
 }

 parse_ended = FALSE;
 vc = gzero;
 type = gzero;
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 strf[(*indexf)++]='(';
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  taux = gzero;
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
  taux=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for second parameter number */
 if (string[index]==';')
 {
  index++;
  per = gzero;
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
  per = lisexpr(strf+old_indexf);
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
  index += nindex-1;
  if (gcmp(per, gzero) <= 0)
  {
   This->cell_s->status = TYPE_VALUE_ERROR;
   return -1;
  }

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for third parameter number */
 if (string[index]==';')
 {
  index++;
  npm = gzero;
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
  npm = lisexpr(strf+old_indexf);
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
  index += nindex-1;
  if (gcmp(npm, gzero) <= 0)
  {
   This->cell_s->status = TYPE_VALUE_ERROR;
   return -1;
  }

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for fourth parameter number */
 if (string[index]==')')
  parse_ended = TRUE;
 if (string[index]==';' || string[index]==')')
 {
  index++;
  va = gzero;
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
  va = lisexpr(strf+old_indexf);
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
  index += nindex-1;
  if (string[index]==')')
  {
   parse_ended = TRUE;
   index++;
  }
  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for fifth parameter number optiponal*/
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';' || string[index]==')')
  {
   index++;
   vc = gzero;
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
   vc = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
  }
  while (string[index]==' ')
   index++;
 }

 /* look for sixth parameter number optionnal */
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';')
  {
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   return -1;
  }
  if (string[index]==')')
  {
   index++;
   type = gzero;
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
   type = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   if (string[index]==Xq_SEPAR)
   {
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
    return -1;
   }
  }
 }

 if (gcmp0(taux))
 {
  if (!gcmp0(vc))
  {
   gtmp2 = gsub(va, vc);
  }
  else
   gtmp2 = va;
  gtmp = gdiv(gtmp2, npm);
 }
 else
 {
  /* 1 + r */
  One_r = gadd(gun, taux);
  /* -(T - (t-1))= t - 1 -T */
  gtmp = gsub(per, gun);
  gtmp = gsub(gtmp, npm);
  /* (1 + r)^-(T-(t-1)) */
  gtmp = gpui(One_r, gtmp, LOW_PREC);

  /* (1 + r)^-T */
  gtmp2 = gneg(npm);
  gtmp2 = gpui(One_r, gtmp2, LOW_PREC);
  /* (1 + r)^-T - 1 */
  gtmp2 = gsub(gtmp2, gun);

  gtmp = gdiv(gtmp, gtmp2);
  gtmp = gmul(va, gtmp);

  if (!gcmp0(vc))
  {
   gtmp2 = gsub(per, gun);
   gtmp2 = gpui(One_r, gtmp2, LOW_PREC);
	  
   gtmp3 = gpui(One_r, npm, LOW_PREC);
   gtmp3 = gsub(gtmp3, gun);
	  
   gtmp2 = gdiv(gtmp2, gtmp3);
   gtmp2 = gmul(vc, gtmp2);

   gtmp = gsub(gtmp, gtmp2);
  }

  gtmp = gmul(taux, gtmp);
  if (!gcmp0(type))
  {
   gtmp = gdiv(gtmp, One_r);
  }
 }
 tmp_str = g2str(gtmp, BIG_PREC);
 sprintf(strf+*indexf, "%s)", tmp_str);
 (*indexf) += strlen(tmp_str)+1;
 free(tmp_str);  
 This->ret_type = TYPE_NUM;
 return index;
}

/* -------------------------------------------------------------------- ** 
** Amnortissement degressif(cout; valeur; duree; periode) p.95          ** 
** -------------------------------------------------------------------- */
int i_syd(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	cout;
 GEN	valeur;
 GEN	duree;
 GEN	periode;
 GEN	gtmp;
 GEN	gtmp2;
 GEN	gtmp3;
 char	*tmp_str;

 Xc_TRACE(("internal syd"));
 if (setjmp(G_erreur))
 {
  switch (GlobErrNum)
  {
  case 260:
   This->cell_s->status = TYPE_DIVZ_ERROR;
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   return -1;
  }
 }

 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 strf[(*indexf)++]='(';
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  cout = gzero;
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
  cout=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for second parameter number */
 if (string[index]==';')
 {
  index++;
  valeur = gzero;
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
  valeur = lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for third parameter number */
 if (string[index]==';')
 {
  index++;
  duree = gzero;
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
  duree = lisexpr(strf+old_indexf);
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
  index += nindex-1;
  if (gcmp(duree, gzero) <= 0)
  {
   This->cell_s->status = TYPE_VALUE_ERROR;
   return -1;
  }

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for fourth parameter number */
 if (string[index]==';' || string[index]==')')
 {
  index++;
  periode = gzero;
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
  periode = lisexpr(strf+old_indexf);
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
  index += nindex-1;
  if (gcmp(periode, gzero) <= 0)
  {
   This->cell_s->status = TYPE_VALUE_ERROR;
   return -1;
  }

  if (string[index]==')')
  {
   index++;
  }
  if (string[index]==Xq_SEPAR)
  {
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   return -1;
  }
 }

 gtmp = gadd(duree, gun);
 gtmp2 = gsub(gtmp, periode);

 gtmp3 = gmul(gtmp, duree);
 gtmp3 = gdiv(gtmp3, gdeux);

 gtmp = gdiv(gtmp2, gtmp3);
 gtmp2 = gsub(cout, valeur);
 gtmp = gmul(gtmp, gtmp2);

 tmp_str = g2str(gtmp, BIG_PREC);
 sprintf(strf+*indexf, "%s)", tmp_str);
 (*indexf) += strlen(tmp_str)+1;
 free(tmp_str);  
 This->ret_type = TYPE_NUM;
 return index;
 This->cell_s->status = TYPE_NAME_ERROR;
 return -1;
}

/* -------------------------------------------------------------------- ** 
** Taux d'interet(npm; vpm; va; [vc; type; estimation]) p.207           ** 
** -------------------------------------------------------------------- */
int i_taux(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  This->cell_s->status = TYPE_NAME_ERROR;
  return -1;
}

/* -------------------------------------------------------------------- ** 
** Taux de rentabilite interne(valeur; [estimation]) p.200              ** 
** -------------------------------------------------------------------- */
int i_tri(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  This->cell_s->status = TYPE_NAME_ERROR;
  return -1;
}

/* -------------------------------------------------------------------- ** 
** Taux de rentabilite interne modifie(valeur; sur; risque) p.203       ** 
** -------------------------------------------------------------------- */
int i_trim(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;
  int	old_indexf;
  int	type;
  int	type2;
  GEN	taux_emp;
  GEN	taux_pla;
  GEN	gtmp;
  GEN	gresult_pos;
  GEN	gresult_neg;
  GEN	nvalue;
  char	*tmp_str;
  int	zone_x_s;
  int	zone_y_s;
  int	zone_x_e;
  int	zone_y_e;
  c_col	*p_col;
  c_line *p_line;

  Xc_TRACE(("internal trim"));
  if (setjmp(G_erreur))
  {
   switch (GlobErrNum)
   {
   case 260:
    This->cell_s->status = TYPE_DIVZ_ERROR;
    return -1;
   default:
    This->cell_s->status = TYPE_CALC_ERROR;
    Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
    return -1;
   }
  }
  
  if (string[0] != '(')
  {
   This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
  strf[(*indexf)++]='(';
  index = 1;
  while (string[index]==' ')
   index++;
  /* look for first parameters zone */
  old_indexf = *indexf;
  nindex = F(This).srparse(This, string+index, strf, indexf,
			   &type, &type2);
  if (nindex < 1)
  {
   return -1;
  }
  index += nindex;
  
  if (type == TYPE_CELL_ZONE)
  {
   int	i;

   zone_x_s = 0;
   i = 0;
   while (strf[*indexf+i]!='-')
   {
    zone_x_s *= 10;
    zone_x_s += strf[*indexf+i]-'0';
    i++;
   }
   i++;
   zone_y_s = 0;
   while (strf[*indexf+i]!='-')
   {
    zone_y_s *= 10;
    zone_y_s += strf[*indexf+i]-'0';
    i++;
   }
   i++;
   zone_x_e = 0;
   while (strf[*indexf+i]!='-')
   {
    zone_x_e *= 10;
    zone_x_e += strf[*indexf+i]-'0';
    i++;
   }
   i++;
   zone_y_e = 0;
   while (strf[*indexf+i]!='-')
   {
    zone_y_e *= 10;
    zone_y_e += strf[*indexf+i]-'0';
    i++;
   }

   Xc_TRACE(("Mystifier les esprits: %d-%d:%d-%d",
	     zone_x_s, zone_y_s,
	     zone_x_e, zone_y_e));
      
   index++;
  }
  else
  {
   This->cell_s->status = TYPE_NEED_PARAM_ZONE;
   return -1;
  }
  if (string[index-1]!=Xq_SEPAR)
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
  
  while (string[index]==' ')
   index++;
  /* look for second parameter number */
  if (string[index]==';')
  {
   index++;
   taux_emp = gzero;
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
   taux_emp=lisexpr(strf+old_indexf);
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
   index += nindex-1;

   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
   else 
   {
    This->cell_s->status = TYPE_PARAM_LESS_ERROR;
    return -1;
   }
  }
  while (string[index]==' ')
   index++;
  /* look for third parameter number */
  if (string[index]==';')
  {
   index++;
   taux_pla = gzero;
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
   taux_pla=lisexpr(strf+old_indexf);
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
   index += nindex-1;

   if (string[index]!=')')
   {
    if (string[index]==Xq_SEPAR)
     This->cell_s->status = TYPE_PARAM_MORE_ERROR;
    else
     This->cell_s->status = TYPE_MISSING_P_ERROR;
    return -1;
   }
  }
  
  nvalue = gzero;
  gresult_pos = gzero;
  gresult_neg = gzero;
  
  p_col = This->motor->tableCL;
  while(p_col && (p_col->number < zone_x_s))
  {
   p_col = p_col->Next;
  }
  while (p_col && (p_col->number <= zone_x_e))
  {
   p_line = p_col->line;
   while(p_line && (p_line->number < zone_y_s))
   {
    p_line = p_line->Next;
   }
   while (p_line && (p_line->number <= zone_y_e))
   {
    switch (This->d_type)
    {
    case ADD_DEP:
     if (F(This->motor).check_c_depend(This->motor, 
				       This->cell_s,
				       p_col->number,
				       p_line->number))
     {
      Xc_TRACE(("Error circular dep"));
      type = TYPE_CIRC_ERROR;
      return -1;
     }
     F(This->motor).add_depend(This->motor,
			       This->cell_s,
			       This->y, This->x,
			       p_line->number,
			       p_col->number);
    case IGN_DEP:
     /* PENSER A LA PRECISION DE CALCUL */
     if ((p_line->cell.type!=Xq_VOID) && 
	 (p_line->cell.value2))
     {
      nvalue = gadd(gun, nvalue);
      if (gcmp(p_line->cell.value2, gzero)>0)
      {
       gtmp = gadd(gun, taux_pla);
       gtmp = gpui(gtmp, nvalue, LOW_PREC);
       gtmp = gdiv(p_line->cell.value2, gtmp);
       gresult_pos = gadd(gresult_pos, gtmp);
      }
      else
      {
       gtmp = gadd(gun, taux_emp);
       gtmp = gpui(gtmp, nvalue, LOW_PREC);
       gtmp = gdiv(p_line->cell.value2, gtmp);
       gresult_neg = gadd(gresult_neg, gtmp);
      }
     }
     break;
    case SUB_DEP:
     F(This->motor).sub_depend(This->motor,
			       This->cell_s,
			       p_line->number,
			       p_col->number);
     break;
    }
    p_line = p_line->Next;
   }
   p_col = p_col->Next;
  }
  
  if (This->d_type != SUB_DEP)
  {
   if (gcmp0(gresult_pos) || gcmp0(gresult_neg))
   {
    This->cell_s->status = TYPE_VALUE_ERROR;
    return -1;
   }
   gtmp = gadd(gun, taux_pla);
   gtmp = gpui(gtmp, nvalue, LOW_PREC);
   gresult_pos = gmul(gtmp, gresult_pos);
   gtmp = gadd(gun, taux_emp);
   gresult_neg = gmul(gtmp, gresult_neg);
   gresult_pos = gneg(gresult_pos);
   gresult_pos = gdiv(gresult_pos, gresult_neg);
   gtmp = gsub(nvalue, gun);
   gtmp = gdiv(gun, gtmp);
   gtmp = gpui(gresult_pos, gtmp, LOW_PREC);
   gtmp = gsub(gtmp, gun);

   tmp_str = g2str(gtmp, BIG_PREC);
   sprintf(strf+*indexf, "%s)", tmp_str);
   (*indexf) += strlen(tmp_str)+1;
   free(tmp_str);
  }
  This->ret_type = TYPE_NUM;
  return index;
 }

/* -------------------------------------------------------------------- ** 
** Valeur actualise(taux; npm; vpm; [vc; type]) p.215                   ** 
** -------------------------------------------------------------------- */
int i_va(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	taux;
 GEN	npm;
 GEN	vpm;
 GEN	vc;
 GEN	type;
 GEN	One_r;
 GEN	gtmp;
 GEN	gtmp2;
 char	*tmp_str;
 boolean	parse_ended;

 Xc_TRACE(("internal va"));
 if (setjmp(G_erreur))
 {
  switch (GlobErrNum)
  {
  case 260:
   This->cell_s->status = TYPE_DIVZ_ERROR;
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   return -1;
  }
 }

 parse_ended = FALSE;
 vc = gzero;
 type = gzero;
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 strf[(*indexf)++]='(';
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  taux = gzero;
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
  taux=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for second parameter number */
 if (string[index]==';')
 {
  index++;
  npm = gzero;
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
  npm = lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for third parameter number */
 if (string[index]==')')
  parse_ended = TRUE;
 if (string[index]==';' || string[index]==')')
 {
  index++;
  vpm = gzero;
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
  vpm = lisexpr(strf+old_indexf);
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
  index += nindex-1;
  if (string[index]==')')
  {
   parse_ended = TRUE;
   index++;
  }
  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for fourth parameter number optiponal*/
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';' || string[index]==')')
  {
   index++;
   vc = gzero;
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
   vc = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
  }
  while (string[index]==' ')
   index++;
 }

 /* look for fifth parameter number optionnal */
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';')
  {
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   return -1;
  }
  if (string[index]==')')
  {
   index++;
   type = gzero;
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
   type = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   if (string[index]==Xq_SEPAR)
   {
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
    return -1;
   }
  }
 }

 if (gcmp0(taux))
 {
  gtmp = gmul(npm, vpm);
  gtmp2 = gadd(gtmp, vc);
  gtmp = gneg(gtmp2);
 }
 else
 {
  /* 1 + r */
  One_r = gadd(gun, taux);

  gtmp = gneg(npm);
  gtmp = gpui(One_r, gtmp, LOW_PREC);
  gtmp = gsub(gun, gtmp);
  gtmp = gdiv(gtmp, taux);
  gtmp2 = gneg(vpm);
  gtmp = gmul(gtmp, gtmp2);
  if (!gcmp0(type))
  {
   gtmp = gmul(gtmp, One_r);
  }

  if (!gcmp0(vc))
  {
   gtmp2 = gneg(npm);
   gtmp2 = gpui(One_r, gtmp2, LOW_PREC);
   gtmp2 = gmul(vc, gtmp2);

   gtmp = gsub(gtmp, gtmp2);
  }
 }

 tmp_str = g2str(gtmp, BIG_PREC);
 sprintf(strf+*indexf, "%s)", tmp_str);
 (*indexf) += strlen(tmp_str)+1;
 free(tmp_str);  
 This->ret_type = TYPE_NUM;
 return index;
}

/* -------------------------------------------------------------------- ** 
** Valeur actualise d'une serie(taux; valeur1; valeur2; ...) p.219      ** 
** -------------------------------------------------------------------- */
int i_van(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;
  int	old_indexf;
  int	type;
  int	type2;
  GEN	taux;
  GEN	gtmp;
  GEN	gtmp2;
  GEN	gresult;
  GEN	nvalue;
  char	*tmp_str;
  boolean	sortie;

  Xc_TRACE(("internal van"));
  if (setjmp(G_erreur))
  {
   switch (GlobErrNum)
   {
   case 260:
    This->cell_s->status = TYPE_DIVZ_ERROR;
    return -1;
   default:
    This->cell_s->status = TYPE_CALC_ERROR;
    Xc_TRACE(("erreur in calculation system %d",
	      GlobErrNum));
    return -1;
   }
  }

  if (string[0] != '(')
  {
   This->cell_s->status = TYPE_MISSING_P_ERROR;
   return -1;
  }
  strf[(*indexf)++]='(';
  index = 1;
  while (string[index]==' ')
   index++;
  
  /* look for first parameter number */
  if (string[index]==';')
  {
   index++;
   taux = gzero;
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
   taux=lisexpr(strf+old_indexf);
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
   index += nindex-1;

   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
   else 
   {
    This->cell_s->status = TYPE_PARAM_LESS_ERROR;
    return -1;
   }
  }
  while (string[index]==' ')
   index++;
  /* look for other parameters */
  nvalue = gzero;
  gresult = gzero;
  sortie = FALSE;
  while (!sortie)
  {
   old_indexf = *indexf;
   nindex = F(This).srparse(This, string+index, strf, indexf,
			    &type, &type2);
   if (nindex < 1)
   {
    return -1;
   }
   index += nindex;

   if (type2 == TYPE_STRING)
   {
    This->cell_s->status = TYPE_NO_PARAM_STRING;
    return -1;
   }

   if (type == TYPE_CELL_ZONE)
   {
    int	zone_x_s;
    int	zone_y_s;
    int	zone_x_e;
    int	zone_y_e;
    int	i;
    c_col	*p_col;
    c_line *p_line;

    zone_x_s = 0;
    i = 0;
    while (strf[*indexf+i]!='-')
    {
     zone_x_s *= 10;
     zone_x_s += strf[*indexf+i]-'0';
     i++;
    }
    i++;
    zone_y_s = 0;
    while (strf[*indexf+i]!='-')
    {
     zone_y_s *= 10;
     zone_y_s += strf[*indexf+i]-'0';
     i++;
    }
    i++;
    zone_x_e = 0;
    while (strf[*indexf+i]!='-')
    {
     zone_x_e *= 10;
     zone_x_e += strf[*indexf+i]-'0';
     i++;
    }
    i++;
    zone_y_e = 0;
    while (strf[*indexf+i]!='-')
    {
     zone_y_e *= 10;
     zone_y_e += strf[*indexf+i]-'0';
     i++;
    }

    Xc_TRACE(("Mystifier les esprits: %d-%d:%d-%d",
	      zone_x_s, zone_y_s,
	      zone_x_e, zone_y_e));

    p_col = This->motor->tableCL;
    while(p_col && (p_col->number < zone_x_s))
    {
     p_col = p_col->Next;
    }
    while (p_col && (p_col->number <= zone_x_e))
    {
     p_line = p_col->line;
     while(p_line && (p_line->number < zone_y_s))
     {
      p_line = p_line->Next;
     }
     while (p_line && (p_line->number <= zone_y_e))
     {
      switch (This->d_type)
      {
      case ADD_DEP:
       if (F(This->motor).check_c_depend(This->motor, 
					 This->cell_s,
					 p_col->number,
					 p_line->number))
       {
	Xc_TRACE(("Error circular dep"));
	type = TYPE_CIRC_ERROR;
	return -1;
       }
       F(This->motor).add_depend(This->motor,
				 This->cell_s,
				 This->y, This->x,
				 p_line->number,
				 p_col->number);
      case IGN_DEP:
       /* PENSER A LA PRECISION DE CALCUL */
       if ((p_line->cell.type!=Xq_VOID) && 
	   (p_line->cell.value2))
       {
	nvalue = gadd(gun, nvalue);
	gtmp = gadd(gun, taux);
	gtmp = gpui(gtmp, nvalue, LOW_PREC);
	gtmp = gdiv(p_line->cell.value2, gtmp);
	gresult = gadd(gresult, gtmp);
       }
       break;
      case SUB_DEP:
       F(This->motor).sub_depend(This->motor,
				 This->cell_s,
				 p_line->number,
				 p_col->number);
       break;
      }
      p_line = p_line->Next;
     }
     p_col = p_col->Next;
    }
    index++;
   }
   else
   {
    GlobErrNum = 0;
    strf[*indexf]=0;
    Xc_TRACE((strf+old_indexf));
    gtmp2=lisexpr(strf+old_indexf);
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

    nvalue = gadd(gun, nvalue);
    gtmp = gadd(gun, taux);
    gtmp = gpui(gtmp, nvalue, LOW_PREC);
    gtmp = gdiv(gtmp2, gtmp);
    gresult = gadd(gresult, gtmp);
   }
   
   *indexf = old_indexf;
   if (string[index-1]==Xq_SEPAR)
   {
    while (string[index]==' ')
     index++;
    continue;
   }
   if (string[index-1]==')')
   {
    sortie = TRUE;
   }
  }
  
  tmp_str = g2str(gresult, BIG_PREC);
  sprintf(strf+*indexf, "%s)", tmp_str);
  (*indexf) += strlen(tmp_str)+1;
  free(tmp_str);  
  This->ret_type = TYPE_NUM;
  return index;
 }

/* -------------------------------------------------------------------- ** 
** Valeur a terme annuite constante(taux; npm; vpm[va; type]) p.222     ** 
** -------------------------------------------------------------------- */
int i_vc(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	taux;
 GEN	npm;
 GEN	vpm;
 GEN	va;
 GEN	type;
 GEN	One_r;
 GEN	gtmp;
 GEN	gtmp2;
 char	*tmp_str;
 boolean	parse_ended;

 Xc_TRACE(("internal vc"));
 if (setjmp(G_erreur))
 {
  switch (GlobErrNum)
  {
  case 260:
   This->cell_s->status = TYPE_DIVZ_ERROR;
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   return -1;
  }
 }

 parse_ended = FALSE;
 va = gzero;
 type = gzero;
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 strf[(*indexf)++]='(';
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  taux = gzero;
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
  taux=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for second parameter number */
 if (string[index]==';')
 {
  index++;
  npm = gzero;
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
  npm = lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for third parameter number */
 if (string[index]==')')
  parse_ended = TRUE;
 if (string[index]==';' || string[index]==')')
 {
  index++;
  vpm = gzero;
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
  vpm = lisexpr(strf+old_indexf);
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
  index += nindex-1;
  if (string[index]==')')
  {
   parse_ended = TRUE;
   index++;
  }
  else
   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
 }
 while (string[index]==' ')
  index++;

 /* look for fourth parameter number optiponal*/
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';' || string[index]==')')
  {
   index++;
   va = gzero;
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
   va = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   else
    if (string[index]==Xq_SEPAR)
    {
     index++;
    }
  }
  while (string[index]==' ')
   index++;
 }

 /* look for fifth parameter number optionnal */
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';')
  {
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   return -1;
  }
  if (string[index]==')')
  {
   index++;
   type = gzero;
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
   type = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   else
    if (string[index]==Xq_SEPAR)
    {
     This->cell_s->status = TYPE_PARAM_MORE_ERROR;
     return -1;
    }
  }
 }

 /* 1 + r */
 One_r = gadd(gun, taux);

 gtmp = gpui(One_r, npm, LOW_PREC);
 gtmp = gsub(gtmp, gun);
 gtmp = gdiv(gtmp, taux);
 gtmp2 = gneg(vpm);
 gtmp = gmul(gtmp, gtmp2);
 if (!gcmp0(type))
 {
  gtmp = gmul(gtmp, One_r);
 }

 if (!gcmp0(va))
 {
  gtmp2 = gpui(One_r, npm, LOW_PREC);
  gtmp2 = gmul(va, gtmp2);

  gtmp = gsub(gtmp, gtmp2);
 }

 tmp_str = g2str(gtmp, BIG_PREC);
 sprintf(strf+*indexf, "%s)", tmp_str);
 (*indexf) += strlen(tmp_str)+1;
 free(tmp_str);  
 This->ret_type = TYPE_NUM;
 return index;
}

/* -------------------------------------------------------------------- ** 
** Amortissement periode(cout; vres; duree; p_deb; p_fin; [fact; vlog]) ** 
** -------------------------------------------------------------------- */
int i_vdb(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	cout;
 GEN	vres;
 GEN	duree;
 GEN	p_deb;
 GEN	p_fin;
 GEN	fact;
 GEN	vlog;
 GEN	gtmp;
 GEN	gresult;
 GEN	gresult2;
 GEN	periode;
 char	*tmp_str;
 boolean	parse_ended;

 Xc_TRACE(("internal vdb"));
 if (setjmp(G_erreur))
 {
  switch (GlobErrNum)
  {
  case 260:
   This->cell_s->status = TYPE_DIVZ_ERROR;
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   return -1;
  }
 }

 parse_ended = FALSE;
 fact = gdeux;
 vlog = gzero;
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 strf[(*indexf)++]='(';
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  cout = gzero;
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
  cout=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for second parameter number */
 if (string[index]==';')
 {
  index++;
  vres = gzero;
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
  vres = lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for third parameter number */
 if (string[index]==';')
 {
  index++;
  duree = gzero;
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
  duree = lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for fourth parameter number */
 if (string[index]==';')
 {
  index++;
  p_deb = gzero;
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
  p_deb = lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for fifth parameter number */
 if (string[index]==')')
  parse_ended = TRUE;
 if (string[index]==';' || string[index]==')')
 {
  index++;
  p_fin = gzero;
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
  p_fin = lisexpr(strf+old_indexf);
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
  index += nindex-1;
  if (string[index]==')')
  {
   parse_ended = TRUE;
   index++;
  }
  else
   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
 }
 while (string[index]==' ')
  index++;

 /* look for sixth parameter number optiponal*/
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';' || string[index]==')')
  {
   index++;
   fact = gzero;
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
   fact = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   else
    if (string[index]==Xq_SEPAR)
    {
     index++;
    }
  }
  while (string[index]==' ')
   index++;
 }

 /* look for seventh parameter number optionnal */
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';')
  {
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   return -1;
  }
  if (string[index]==')')
  {
   index++;
   vlog = gzero;
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
   vlog = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   else
    if (string[index]==Xq_SEPAR)
    {
     This->cell_s->status = TYPE_PARAM_MORE_ERROR;
     return -1;
    }
  }
 }

 gresult = gzero;
 periode = gzero;
 while (gcmp(p_deb, periode) > 0)
 {
  gtmp = gsub(cout, gresult);
  gtmp = gmul(gtmp, fact);
  gtmp = gdiv(gtmp, duree);
  gresult = gadd(gresult, gtmp);
  periode = gadd(periode, gun);
 }

 gresult2 = gzero;
 while (gcmp(p_fin, periode) > 0)
 {
  gtmp = gsub(cout, gresult);
  gtmp = gmul(gtmp, fact);
  gtmp = gdiv(gtmp, duree);
  gresult2 = gadd(gresult2, gtmp);
  gresult = gadd(gresult, gtmp);
  periode = gadd(periode, gun);
 }

 tmp_str = g2str(gresult2, BIG_PREC);
 sprintf(strf+*indexf, "%s)", tmp_str);
 (*indexf) += strlen(tmp_str)+1;
 free(tmp_str);  
 This->ret_type = TYPE_NUM;
 return index;
}

/* -------------------------------------------------------------------- ** 
** Remboursement(taux; npm; va; [vc; type]) p.183                       ** 
** -------------------------------------------------------------------- */
int i_vpm(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	taux;
 GEN	npm;
 GEN	va;
 GEN	vc;
 GEN	type;
 GEN	One_r;
 GEN	gtmp;
 GEN	gtmp2;
 char	*tmp_str;
 boolean	parse_ended;

 Xc_TRACE(("internal vpm"));
 if (setjmp(G_erreur))
 {
  switch (GlobErrNum)
  {
  case 260:
   This->cell_s->status = TYPE_DIVZ_ERROR;
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   return -1;
  }
 }

 parse_ended = FALSE;
 vc = gzero;
 type = gzero;
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 strf[(*indexf)++]='(';
 index = 1;
 while (string[index]==' ')
  index++;
 /* look for first parameter number */
 if (string[index]==';')
 {
  index++;
  taux = gzero;
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
  taux=lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;
 /* look for second parameter number */
 if (string[index]==';')
 {
  index++;
  npm = gzero;
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
  npm = lisexpr(strf+old_indexf);
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
  index += nindex-1;

  if (string[index]==Xq_SEPAR)
  {
   index++;
  }
  else 
  {
   This->cell_s->status = TYPE_PARAM_LESS_ERROR;
   return -1;
  }
 }
 while (string[index]==' ')
  index++;

 /* look for third parameter number */
 if (string[index]==')')
  parse_ended = TRUE;
 if (string[index]==';' || string[index]==')')
 {
  index++;
  va = gzero;
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
  va = lisexpr(strf+old_indexf);
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
  index += nindex-1;
  if (string[index]==')')
  {
   parse_ended = TRUE;
   index++;
  }
  else
  {
   if (string[index]==Xq_SEPAR)
   {
    index++;
   }
  }
 }
 while (string[index]==' ')
  index++;

 /* look for fourth parameter number optiponal*/
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';' || string[index]==')')
  {
   index++;
   vc = gzero;
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
   vc = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   else
   {
    if (string[index]==Xq_SEPAR)
    {
     index++;
    }
   }
  }
  while (string[index]==' ')
   index++;
 }

 /* look for fifth parameter number optionnal */
 if (!parse_ended)
 {
  if (string[index]==')')
   parse_ended = TRUE;
  if (string[index]==';')
  {
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
   return -1;
  }
  if (string[index]==')')
  {
   index++;
   type = gzero;
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
   type = lisexpr(strf+old_indexf);
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
   index += nindex-1;
   if (string[index]==')')
   {
    parse_ended = TRUE;
    index++;
   }
   else
   {
    if (string[index]==Xq_SEPAR)
    {
     This->cell_s->status = TYPE_PARAM_MORE_ERROR;
     return -1;
    }
   }
  }
 }

 if (gcmp0(npm))
 {
  This->cell_s->status = TYPE_DIVZ_ERROR;
  return -1;
 }

 /* 1 + r */
 if (!gcmp0(taux))
 {
  One_r = gadd(gun, taux);
      
  gtmp = gneg(npm);
  gtmp = gpui(One_r, gtmp, LOW_PREC);
  gtmp = gsub(gtmp, gun);
  gtmp = gdiv(va, gtmp);

  if (!gcmp0(vc))
  {
   gtmp2 = gpui(One_r, npm, LOW_PREC);
   gtmp2 = gsub(gtmp2, gun);
   gtmp2 = gdiv(vc, gtmp2);
	  
   gtmp = gsub(gtmp, gtmp2);
  }

  gtmp = gmul(taux, gtmp);
  if (!gcmp0(type))
  {
   gtmp = gdiv(gtmp, One_r);
  }
 }
 else
 {
  gtmp = gdiv(va, npm);
 }

 tmp_str = g2str(gtmp, BIG_PREC);
 sprintf(strf+*indexf, "%s)", tmp_str);
 (*indexf) += strlen(tmp_str)+1;
 free(tmp_str);  
 This->ret_type = TYPE_NUM;
 return index;
}
