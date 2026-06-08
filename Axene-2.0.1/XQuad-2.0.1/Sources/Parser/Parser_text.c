/*
** Parser_text.c for XQuad in Parser/
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
** Last update Fri Oct  2 17:01:56 1998 Robin Castanier
*/

#include "Parser.h"
#include "char_translate.h"

/* ----------------------------------------------------------------- **
** Car(num) return the car number                                    **
** ----------------------------------------------------------------- */
int i_car(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	calc_x;
 ulong	avma_old;
 long	num;

 Xc_TRACE(("internal car"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 old_indexf = *indexf;
 strf[(*indexf)++]='(';
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;
 if (end_type == TYPE_STRING)
 {
  This->cell_s->status = TYPE_NO_PARAM_NUM;
  return -1;
 }

 strf[*indexf]=0;

 Xc_TRACE(("str: %s", strf+old_indexf));
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
 num = gtolong(calc_x);
 avma = avma_old;
 Xc_TRACE(("NuMbEr: %d", num));
  
 index += nindex;
 if ((num < 0) || (num > 255))
 {
  This->cell_s->status = TYPE_D_0_TO_256_ERROR;
  return -1;
 }
 strf[(*indexf)++]=num;
 strf[(*indexf)]=0;

 if (string[index-1]!=')')
 {
  if (string[index-1]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 This->ret_type = TYPE_STRING;
 return index;
}

/* ----------------------------------------------------------------- **
** Cherche(texte cherche; texte; depart) return pos of first occur   **
** ----------------------------------------------------------------- */
int i_cherche(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	calc_x;
 ulong	avma_old;
 long	num;
 char	*search;
 char	*s_string;
 int	cnt_num;

 Xc_TRACE(("internal cherche"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 strf[(*indexf)++]='(';

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 index+=nindex-1;
 if (string[index]==';')
  index++;
 else
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  return -1;
 }
 search = (char *)Xc_malloc("str_cherche", strlen(strf+old_indexf)+1);
 strcpy(search, strf+old_indexf);

 /* look for second parameter string */
 *indexf = old_indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;
  
 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  Xc_free(search);
  return -1;
 }

 index+=nindex-1;
 s_string = (char *)Xc_malloc("str_to find", strlen(strf+old_indexf)+1);
 strcpy(s_string, strf+old_indexf);

 /* look for third parameter number */
 *indexf = old_indexf;
 if (string[index]==';')
 {
  index++;
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
   Xc_free(search);
   Xc_free(s_string);
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   Xc_free(search);
   Xc_free(s_string);
   return -1;
  }
  *indexf = old_indexf;
  num = gtolong(calc_x);
  avma = avma_old;
  index += nindex-1;
 }
 else
 {
  if (string[index]!=')')
  {
   This->cell_s->status = TYPE_ERROR;
   return -1;
  }
  num = 0;
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  Xc_free(search);
  Xc_free(s_string);
  return -1;
 }

 Xc_TRACE(("NuMbEr: %d", num));
  
 if ((num < 0) || (num > (int)strlen(s_string)))
 {
  This->cell_s->status = TYPE_D_OUTOFSTR_ERROR;
  Xc_free(search);
  Xc_free(s_string);
  return -1;
 }

 /* maj the final string */
 cnt_num = 0;
 while (s_string[cnt_num])
 {
  s_string[cnt_num] = Xc_TO_UPPER(s_string[cnt_num]);
  cnt_num++;
 }

 /* maj the search string */
 cnt_num = 0;
 while (search[cnt_num])
 {
  search[cnt_num] = Xc_TO_UPPER(search[cnt_num]);
  cnt_num++;
 }

 while (s_string[num])
 {
  if (s_string[num] == search[0])
  {
   int sav_num;
   int src_num;

   sav_num = num+1;
   src_num = 1;
   while (search[src_num] && (s_string[sav_num] == search[src_num]))
   {
    src_num++;
    sav_num++;
   }
   if (!search[src_num])
   {
    break;
   }
   if (!s_string[sav_num])
   {
    This->cell_s->status = TYPE_NO_ANSWER;
    Xc_free(search);
    Xc_free(s_string);
    return -1;
   }
  }
  num++;
 }
 if (!s_string[num])
 {
  This->cell_s->status = TYPE_NO_ANSWER;
  Xc_free(search);
  Xc_free(s_string);
  return -1;
 }
 sprintf(strf+*indexf, "%d.0)", (int)(num+1));
 while (strf[*indexf])
  (*indexf)++;

 This->ret_type = TYPE_NUM;
 Xc_free(search);
 Xc_free(s_string);
 return index+1;
}

/* ----------------------------------------------------------------- **
** Cnum(text or num) return in number format                         **
** ----------------------------------------------------------------- */
int i_cnum(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;

 Xc_TRACE(("internal cnum"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;
 index += nindex-1;

 if (end_type == TYPE_STRING)
 {
  int i;
  (*indexf)--;
  strf[*indexf] = 0;
  i = old_indexf;
  while (strf[i] && (strf[i] >= '0') &&
	 (strf[i] <= '9'))
   i++;
  if (strf[i]=='.' || strf[i]==',')
   i++;
  while (strf[i] && (strf[i] >= '0') &&
	 (strf[i] <= '9'))
   i++;

  if (strf[i])
  {
   This->cell_s->status = TYPE_NO_ANSWER;
   return -1;
  }
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }

 strf[(*indexf)]=0;

 This->ret_type = TYPE_NUM;
 return index+1;
}

/* ----------------------------------------------------------------- **
** code(texte) return number of ascii pos                            **
** ----------------------------------------------------------------- */
int i_code(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 int	num;

 Xc_TRACE(("internal code"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 strf[(*indexf)++]='(';

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 index+=nindex-1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
  
 *indexf = old_indexf;
 num = strf[*indexf];
 sprintf(strf+*indexf, "%d.0)", num);
 while (strf[*indexf])
  (*indexf)++;

 This->ret_type = TYPE_NUM;
 return index+1;
}

/* ----------------------------------------------------------------- **
** concat(text1; text2; ...) return concatenated text                **
** ----------------------------------------------------------------- */
int i_concat(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 boolean	sortie;
 int		type;
 int		nindex;
 int		index;

 Xc_TRACE(("internal concat"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
  
 sortie = FALSE;
 while (!sortie)
 {
  nindex = F(This).rparse(This, string+index, strf, indexf,
			  &type);
  if (nindex < 1)
  {
   return -1;
  }
  index += nindex;
  if (type != TYPE_STRING)
  {
   This->cell_s->status = TYPE_MUST_BE_STRING;
   return -1;
  }
  if (string[index-1]==Xq_SEPAR)
  {
   while (string[index]==' ')
    index++;
   continue;
  }
  if (string[index-1]==')')
  {
   strf[*indexf-1]=0;
   sortie = TRUE;
  }
 }

 This->ret_type = TYPE_STRING;
 return index;
}

/* ----------------------------------------------------------------- **
** ctxt(num; decimal; flag_sep) return a formated number             **
** ----------------------------------------------------------------- */
int i_ctxt(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	calc_x;
 ulong	avma_old;
 char	*tmp_str;
 long	prec;
 long	flag_sep;

 int	k;
 int	first_digit;
 int	first_decimal;
 int	nb_decimal;
 boolean comma;
 boolean carry;
 int	mind;
 int	indexs;
 int	ret;
 int	p_point;
 int	space_nbr;
 int	last;

 int	space_each;
 char	char_sep;
 char	coma_sep;


 Xc_TRACE(("internal ctxt"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 space_each = 3;
 coma_sep = ',';
 char_sep = ' ';
 /* look for first parameter */
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
 tmp_str = g2str(calc_x, BIG_PREC);
 avma = avma_old;
 index += nindex-1;

 /* look for second parameter if exist*/
 if (string[index]==';')
 {
  index++;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
  {
   free(tmp_str);
   return -1;
  }
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
   free(tmp_str);
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   free(tmp_str);
   return -1;
  }
  *indexf = old_indexf;
  prec = gtolong(calc_x);
  avma = avma_old;
  if (prec > 127)
  {
   This->cell_s->status = TYPE_CALC_ERROR;
   free(tmp_str);
   return -1;
  }
  index += nindex-1;
 }
 else
 {
  if (string[index]!=')')
  {
   This->cell_s->status = TYPE_ERROR;
   free(tmp_str);
   return -1;
  }
  prec = 2;
 }

 /* look for third parameter if exist*/
 if (string[index]==';')
 {
  index++;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
  {
   free(tmp_str);
   return -1;
  }
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
   free(tmp_str);
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   free(tmp_str);
   return -1;
  }
  *indexf = old_indexf;
  flag_sep = gtolong(calc_x);
  avma = avma_old;
  index += nindex-1;
 }
 else
 {
  if (string[index]!=')')
  {
   This->cell_s->status = TYPE_ERROR;
   free(tmp_str);
   return -1;
  }
  flag_sep = 0;
 }

 if (flag_sep)
  space_each = 0;

 indexs = 0;
 if (tmp_str[indexs]=='-')
 {
  k = first_digit = indexs + 1;
 }
 else
 {
  k = first_digit = indexs;
 }

 while(tmp_str[k] && tmp_str[k]!='.')
  k++;

 if (tmp_str[k])
 {
  first_decimal = ++k;
  while(tmp_str[k])
   k++;
  nb_decimal = k - first_decimal;
  if (nb_decimal > prec)
  {
   k = first_decimal + prec;
   if (tmp_str[k] >= '5')
   {
    k--;
    comma = FALSE;
    carry = TRUE;
    do
    {
     if (tmp_str[k] == '.')
     {
      k--;
      comma = TRUE;
      continue;
     }
     if (tmp_str[k] != '9')
     {
      if (tmp_str[k]>='0' && tmp_str[k]<='8')
      {
       tmp_str[k] += 1;
       carry = FALSE;
      }
      else
      {
       k++;
       carry = TRUE;
       break;
      }
     }
     else
     {
      tmp_str[k] = '0';
      carry = TRUE;
      if (k == first_digit) 
       break;
      k--;
     }
    } while(carry);
    if (carry)
    {
     memmove(tmp_str + k + 1, tmp_str + k,
	     strlen(tmp_str)+1-k);
     tmp_str[k] = '1';
    }
   }
  }
 }
  
 if (space_each)
 {
  mind = 0;
  p_point = 0;
  if (tmp_str[p_point]=='-')
  {
   strf[(*indexf)++]=tmp_str[indexs++];
   mind = 1;
  }
  while ((tmp_str[p_point+mind]!='.') && (tmp_str[p_point+mind]!=0))
   p_point++;
  space_nbr = p_point / space_each;
  last = p_point % space_each;
  if (!last)
  {
   space_nbr--;
   last = space_each;
  }
  while (space_nbr)
  {
   while (last)
   {
    strf[(*indexf)++]=tmp_str[indexs++];
    last--;
   }
   strf[(*indexf)++]=char_sep;
   space_nbr--;
   last = space_each;
  }
  while (last)
  {
   strf[(*indexf)++]=tmp_str[indexs++];
   last--;
  }
 }
 else
 {
  while ((tmp_str[indexs]!='.') && (tmp_str[indexs]!=0))
   strf[(*indexf)++]=tmp_str[indexs++];
 }

 if (tmp_str[indexs] == '.')
 {
  indexs++;
  if (prec != 0)
  {
   ret = 1;
   strf[(*indexf)++] = coma_sep;
  }
  else
   ret = -1;
 }
 else
  ret = -1;
 
 last = prec;
 while (last)
 {
  if (tmp_str[indexs])
   strf[(*indexf)++] = tmp_str[indexs++];
  else
   break;
  last--;
 }

 while (last>0)
 {
  last--;
  if (ret == -1)
  {
   strf[(*indexf)++] = coma_sep;
   ret = 1;
  }
  strf[(*indexf)++] = '0';
 }
 strf[*indexf] = 0;

 free(tmp_str);
 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index++;
 This->ret_type = TYPE_STRING;
 return index;
}

/* ----------------------------------------------------------------- **
** Droite(texte; nbr car) return right part of text                  **
** ----------------------------------------------------------------- */
int i_droite(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	calc_x;
 ulong	avma_old;
 long	num;
 char	*search;
 int	len;

 Xc_TRACE(("internal droite"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 index+=nindex-1;

 len = strlen(strf+old_indexf);
 search = (char *)Xc_malloc("str_cherche", len+1);
 strcpy(search, strf+old_indexf);

 /* look for second parameter number if exist */
 *indexf = old_indexf;
 if (string[index]==';')
 {
  index++;
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
   Xc_free(search);
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   Xc_free(search);
   return -1;
  }
  *indexf = old_indexf;
  num = gtolong(calc_x);
  avma = avma_old;
  index += nindex-1;
 }
 else
 {
  if (string[index]!=')')
  {
   This->cell_s->status = TYPE_ERROR;
   return -1;
  }
  num = 1;
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  Xc_free(search);
  return -1;
 }

 Xc_TRACE(("NuMbEr: %d", num));
  
 if (num <= 0)
  num = 1;
 if (num > len)
  num = len;
 strcpy(strf+*indexf, search+len-num);
 (*indexf)+=num;
 strf[*indexf]=0;
 
 This->ret_type = TYPE_STRING;
 Xc_free(search);
 return index+1;
}

/* ----------------------------------------------------------------- **
** epurage(texte) return epured car < 32 string                      **
** ----------------------------------------------------------------- */
int i_epurage(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;

 Xc_TRACE(("internal epurage"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 index+=nindex-1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
  
 (*indexf)--;
 strf[*indexf]=0;
 *indexf = old_indexf;
 while (strf[*indexf])
 {
  int	i;
  unsigned char c;

  while ( (c = (unsigned char)strf[*indexf]) != '\0')
  {
   if (c < 32 || (c > 126 && c < 160))
    break;
   (*indexf)++;
  }

  i = 0;
  while ( (c = (unsigned char)strf[*indexf + i]) != '\0')
  {
   if ( (c >= 32 && c <= 126) || c >= 160)
    break;
   i++;
  }
  if (i)
  {
   strcpy(strf+*indexf, strf+*indexf+i);
   *indexf+=i;
  }
 }
  
 This->ret_type = TYPE_STRING;
 return index+1;
}

/* ----------------------------------------------------------------- **
** exact(texte1; texte2) return 1 if same strings else 0             **
** ----------------------------------------------------------------- */
int i_exact(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 char	*search;
 char	*s_string;

 Xc_TRACE(("internal exact"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 strf[(*indexf)++]='(';

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 index+=nindex-1;
 if (string[index]==';')
  index++;
 else
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  return -1;
 }

 strf[*indexf]=0;
 search = (char *)Xc_malloc("str_cherche", strlen(strf+old_indexf)+1);
 strcpy(search, strf+old_indexf);

 /* look for second parameter string */
 *indexf = old_indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;
  
 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  Xc_free(search);
  return -1;
 }

 index+=nindex-1;
 (*indexf)--;
 strf[*indexf] = 0;
 s_string = (char *)Xc_malloc("str_to find", strlen(strf+old_indexf)+1);
 strcpy(s_string, strf+old_indexf);
  
 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  Xc_free(search);
  Xc_free(s_string);
  return -1;
 }
 *indexf = old_indexf;

 if (strcmp(search, s_string) == 0)
  strf[(*indexf)++]='1';
 else
  strf[(*indexf)++]='0';
 strf[(*indexf)++]=')';
 strf[*indexf]=0;

 This->ret_type = TYPE_BOOLEAN;
 Xc_free(search);
 Xc_free(s_string);
 return index+1;
}

/* ----------------------------------------------------------------- **
** franc(num; decimal) return a string number in monetaire format    **
** ----------------------------------------------------------------- */
int i_franc(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	calc_x;
 ulong	avma_old;
 char	*tmp_str;

 Xc_TRACE(("internal franc"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter */
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
 tmp_str = g2str(calc_x, BIG_PREC);
 avma = avma_old;
 index += nindex;
 return index;
}

/* ----------------------------------------------------------------- **
** gauche(texte; nbr car) return left part of text                   **
** ----------------------------------------------------------------- */
int i_gauche(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	calc_x;
 ulong	avma_old;
 long	num;
 char	*search;
 int	len;

 Xc_TRACE(("internal gauche"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 index+=nindex-1;

 len = strlen(strf+old_indexf);
 search = (char *)Xc_malloc("str_cherche", len+1);
 strcpy(search, strf+old_indexf);

 /* look for second parameter number */
 *indexf = old_indexf;
 if (string[index]==';')
 {
  index++;
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
   Xc_free(search);
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   Xc_free(search);
   return -1;
  }
  *indexf = old_indexf;
  num = gtolong(calc_x);
  avma = avma_old;
  index += nindex-1;
 }
 else
 {
  if (string[index]!=')')
  {
   This->cell_s->status = TYPE_ERROR;
   return -1;
  }
  num = 1;
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  Xc_free(search);
  return -1;
 }

 Xc_TRACE(("NuMbEr: %d", num));
  
 if (num <= 0)
  num = 1;
 if (num > len)
  num = len;
 strncpy(strf+*indexf, search, num);
 (*indexf)+=num;
 strf[*indexf]=0;
 This->ret_type = TYPE_STRING;
 Xc_free(search);
 return index+1;
}

/* ----------------------------------------------------------------- **
** majuscule(texte) return the text in majuscule                     **
** ----------------------------------------------------------------- */
int i_majuscule(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;

 Xc_TRACE(("internal majuscule"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 index+=nindex-1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 (*indexf)--;
 strf[*indexf]=0;

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
  
 *indexf = old_indexf;
 *indexf = old_indexf;
 while (strf[*indexf])
 {
  strf[*indexf] = Xc_TO_UPPER(strf[*indexf]);
  (*indexf)++;
 }
 This->ret_type = TYPE_STRING;
 return index+1;
}

/* ----------------------------------------------------------------- **
** minuscule(texte) return the text in minuscule                     **
** ----------------------------------------------------------------- */
int i_minuscule(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;

 Xc_TRACE(("internal minuscule"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 index+=nindex-1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 (*indexf)--;
 strf[*indexf]=0;

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
  
 *indexf = old_indexf;
 while (strf[*indexf])
 {
  strf[*indexf] = Xc_TO_LOWER(strf[*indexf]);
  (*indexf)++;
 }
 This->ret_type = TYPE_STRING;
 return index+1;
}

/* ----------------------------------------------------------------- **
** nbcar(texte) return number of car in string                       **
** ----------------------------------------------------------------- */
int i_nbcar(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 int	num;

 Xc_TRACE(("internal nbcar"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 strf[(*indexf)++]='(';

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 index+=nindex-1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
  
 *indexf = old_indexf;
 num = strlen(strf+*indexf);
 sprintf(strf+*indexf, "%d.0)", num);
 while (strf[*indexf])
  (*indexf)++;

 This->ret_type = TYPE_NUM;
 return index+1;
}

/* ----------------------------------------------------------------- **
** nompropre(texte) return the text with maj at each word            **
** ----------------------------------------------------------------- */
int i_nompropre(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;

 Xc_TRACE(("internal nompropre"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 index+=nindex-1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 (*indexf)--;
 strf[*indexf]=0;

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
  
 *indexf = old_indexf;
 strf[*indexf] = Xc_TO_UPPER(strf[*indexf]);
 (*indexf)++;
 while (strf[*indexf])
 {
  strf[*indexf] = Xc_TO_LOWER(strf[*indexf]);
  if (strf[*indexf]==' ')
  {
   (*indexf)++;
   while (strf[*indexf]==' ')
    (*indexf)++;
   if (!strf[*indexf])
    break;
   strf[*indexf] = Xc_TO_UPPER(strf[*indexf]);
  }
  (*indexf)++;
 }
 This->ret_type = TYPE_STRING;
 return index+1;
}

/* ----------------------------------------------------------------- **
** remplacer(texte_aremp; depart; nbr_car; str_remp) replace letters **
** ----------------------------------------------------------------- */
int i_remplacer(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	calc_x;
 ulong	avma_old;
 long	start;
 long	len;
 char	*initial;
 int	ilen;
 char	*newstr;

 Xc_TRACE(("internal remplacer"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 index+=nindex-1;
 if (string[index]==';')
  index++;
 else
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  return -1;
 }

 initial = (char *)Xc_malloc("str_init", strlen(strf+old_indexf)+1);
 strcpy(initial, strf+old_indexf);

 /* look for second parameter number */
 *indexf = old_indexf;
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
  Xc_free(initial);
  return -1;
 default:
  This->cell_s->status = TYPE_CALC_ERROR;
  Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
  Xc_free(initial);
  return -1;
 }
 *indexf = old_indexf;
 start = gtolong(calc_x);
 avma = avma_old;
 if (start < 1)
 {
  This->cell_s->status = TYPE_DOMAIN_ERROR;
  Xc_free(initial);
  return -1;
 }
 index += nindex-1;
 if (string[index]==';')
  index++;
 else
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  return -1;
 }

 /* look for third parameter number */
 *indexf = old_indexf;
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
  Xc_free(initial);
  return -1;
 default:
  This->cell_s->status = TYPE_CALC_ERROR;
  Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
  Xc_free(initial);
  return -1;
 }
 *indexf = old_indexf;
 len = gtolong(calc_x);
 avma = avma_old;
 if (len < 0)
 {
  This->cell_s->status = TYPE_DOMAIN_ERROR;
  Xc_free(initial);
  return -1;
 }
 index += nindex-1;
 if (string[index]==';')
  index++;
 else
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  return -1;
 }

 /* look for fourth parameter string */
 *indexf = old_indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;
  
 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  Xc_free(initial);
  return -1;
 }

 (*indexf)--;
 strf[*indexf]=0;
 index+=nindex-1;
 newstr = (char *)Xc_malloc("newstr", strlen(strf+old_indexf)+1);
 strcpy(newstr, strf+old_indexf);

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  Xc_free(initial);
  Xc_free(newstr);
  return -1;
 }
 *indexf = old_indexf;

 start--;
 ilen = strlen(initial);
 if (start>ilen)
  start = ilen;
 if (start)
 {
  strncpy(strf+*indexf, initial, start);
  *indexf+=start;
 }
 strcpy(strf+*indexf, newstr);
 *indexf+=strlen(newstr);
 if (start+len < ilen)
 {
  strcpy(strf+*indexf, initial+start+len);
  *indexf+=ilen-(start+len);
 }
  
 This->ret_type = TYPE_STRING;
 Xc_free(initial);
 Xc_free(newstr);
 return index+1;
}

/* ----------------------------------------------------------------- **
** rept(texte; nbr) return nbr * texte string                        **
** ----------------------------------------------------------------- */
int i_rept(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	calc_x;
 ulong	avma_old;
 long	num;
 char	*search;
 int	len;

 Xc_TRACE(("internal rept"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 index+=nindex-1;
 strf[*indexf]=0;

 len = strlen(strf+old_indexf);
 search = (char *)Xc_malloc("str_cherche", len+1);
 strcpy(search, strf+old_indexf);

 /* look for second parameter number */
 *indexf = old_indexf;
 if (string[index]==';')
 {
  index++;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
   return -1;
  if (end_type == TYPE_STRING)
  {
   This->cell_s->status = TYPE_NO_PARAM_NUM;
   return -1;
  }
  (*indexf)--;
  strf[*indexf]=0;
      
  GlobErrNum = 0;
  avma_old = avma;
  calc_x=lisexpr(strf+old_indexf);
  switch (GlobErrNum)
  {
  case 0:
   break;
  case 260:
   This->cell_s->status = TYPE_DIVZ_ERROR;
   Xc_free(search);
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   Xc_free(search);
   return -1;
  }
  *indexf = old_indexf;
  num = gtolong(calc_x);
  avma = avma_old;
  index += nindex-1;
 }
 else
 {
  if (string[index]!=')')
  {
   This->cell_s->status = TYPE_ERROR;
   return -1;
  }
  num = 1;
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  Xc_free(search);
  return -1;
 }

 Xc_TRACE(("NuMbEr: %d", num));
  
 if (len * num > MAX_STRF_LEN-1000)
 {
  num = (MAX_STRF_LEN-1000)/len;
 }
 while (num)
 {
  strcpy(strf+*indexf, search);
  (*indexf)+=len;
  num--;
 }
 strf[*indexf]=0;
 This->ret_type = TYPE_STRING;
 Xc_free(search);
 return index+1;
}

/* ----------------------------------------------------------------- **
** stxt(texte; depart; nbr) return a part of the string              **
** ----------------------------------------------------------------- */
int i_stxt(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	calc_x;
 ulong	avma_old;
 char	*search;
 long	start;
 long	len;

 Xc_TRACE(("internal stxt"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }
 index+=nindex-1;
 if (string[index]==';')
  index++;
 else
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  return -1;
 }

 search = (char *)Xc_malloc("str_cherche", strlen(strf+old_indexf)+1);
 strcpy(search, strf+old_indexf);

 /* look for second parameter number */
 *indexf = old_indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
 {
  Xc_free(search);
  return -1;
 }
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
  Xc_free(search);
  return -1;
 default:
  This->cell_s->status = TYPE_CALC_ERROR;
  Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
  Xc_free(search);
  return -1;
 }
 *indexf = old_indexf;
 start = gtolong(calc_x);
 avma = avma_old;
 index += nindex-1;
 if (string[index]==';')
  index++;
 else
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  Xc_free(search);
  return -1;
 }
 if (start<1)
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  Xc_free(search);
  return -1;
 }
    
 /* look for third parameter number */
 *indexf = old_indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
 {
  Xc_free(search);
  return -1;
 }
 if (end_type == TYPE_STRING)
 {
  This->cell_s->status = TYPE_NO_PARAM_NUM;
  return -1;
 }

 (*indexf)--;
 strf[*indexf]=0;
  
 GlobErrNum = 0;
 avma_old = avma;
 calc_x=lisexpr(strf+old_indexf);
 switch (GlobErrNum)
 {
 case 0:
  break;
 case 260:
  This->cell_s->status = TYPE_DIVZ_ERROR;
  Xc_free(search);
  return -1;
 default:
  This->cell_s->status = TYPE_CALC_ERROR;
  Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
  Xc_free(search);
  return -1;
 }
 *indexf = old_indexf;
 len = gtolong(calc_x);
 avma = avma_old;
 if (len < 0)
 {
  This->cell_s->status = TYPE_CALC_ERROR;
  Xc_free(search);
  return -1;
 }
 index += nindex-1;

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  Xc_free(search);
  return -1;
 }

 start--;
 if (start < (int)strlen(search) && len)
 {
  strncpy(strf + *indexf, search + start, len);
  *indexf += len;
  strf[*indexf] = 0;
 }
 else
 {
  strf[*indexf]=0;
 }

 This->ret_type = TYPE_STRING;
 Xc_free(search);
 return index+1;
}

/* ----------------------------------------------------------------- **
** substitue(texte; old_text; new_text; pos) change letters          **
** ----------------------------------------------------------------- */
int i_substitue(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	calc_x;
 ulong	avma_old;
 long	num;
 char	*search;
 char	*oldstr;
 char	*newstr;
 boolean	all_pos;
 int	ptr_str;

 Xc_TRACE(("internal subsistitue"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 index+=nindex-1;
 if (string[index]==';')
  index++;
 else
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  return -1;
 }

 strf[*indexf]=0;
 search = (char *)Xc_malloc("str_cherche", strlen(strf+old_indexf)+1);
 strcpy(search, strf+old_indexf);

 /* look for second parameter string */
 *indexf = old_indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
 {
  Xc_free(search);
  return -1;
 }
 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  Xc_free(search);
  return -1;
 }

 index+=nindex-1;
 if (string[index]==';')
  index++;
 else
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  Xc_free(search);
  return -1;
 }

 strf[*indexf]=0;
 oldstr = (char *)Xc_malloc("str_to find", strlen(strf+old_indexf)+1);
 strcpy(oldstr, strf+old_indexf);

 /* look for third parameter string */
 *indexf = old_indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
 {
  Xc_free(search);
  Xc_free(oldstr);
  return -1;
 }
  
 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  Xc_free(search);
  Xc_free(oldstr);
  return -1;
 }

 index+=nindex-1;
 if (string[index]==';')
  strf[*indexf]=0;
 else
 {
  (*indexf)--;
  strf[*indexf]=0;
 }

 newstr = (char *)Xc_malloc("str_to find", strlen(strf+old_indexf)+1);
 strcpy(newstr, strf+old_indexf);

 /* look for fourth parameter number */
 *indexf = old_indexf;
 if (string[index]==';')
 {
  index++;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
  {
   Xc_free(search);
   Xc_free(oldstr);
   Xc_free(newstr);
   return -1;
  } 
  if (end_type == TYPE_STRING)
  {
   Xc_free(search);
   Xc_free(oldstr);
   Xc_free(newstr);
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
   Xc_free(search);
   Xc_free(oldstr);
   Xc_free(newstr);
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   Xc_free(search);
   Xc_free(oldstr);
   Xc_free(newstr);
   return -1;
  }
  *indexf = old_indexf;
  num = gtolong(calc_x);
  avma = avma_old;
  index += nindex-1;
  all_pos = FALSE;
 }
 else
 {
  if (string[index]!=')')
  {
   This->cell_s->status = TYPE_ERROR;
   Xc_free(search);
   Xc_free(oldstr);
   Xc_free(newstr);
   return -1;
  }
  num = 1;
  all_pos = TRUE;
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  Xc_free(search);
  Xc_free(oldstr);
  Xc_free(newstr);
  return -1;
 }

 Xc_TRACE(("NuMbEr: %d", num));
  
 if (num < 1)
 {
  This->cell_s->status = TYPE_D_OUTOFSTR_ERROR;
  Xc_free(search);
  Xc_free(oldstr);
  Xc_free(newstr);
  return -1;
 }

 ptr_str = 0;
 while (search[ptr_str])
 {
  if (search[ptr_str] == oldstr[0])
  {
   int sav_num;
   int src_num;

   sav_num = ptr_str+1;
   src_num = 1;
   while (oldstr[src_num] && (search[sav_num] == oldstr[src_num]))
   {
    src_num++;
    sav_num++;
   }
   if (!oldstr[src_num])
   {
    /* ok pattern oldstr found in search */
    num--;
    if (all_pos || !num)
    {
     strcpy(strf+*indexf, newstr);
     while (strf[*indexf])
      (*indexf)++;
     ptr_str+=strlen(oldstr);
    }
    if (!all_pos && !num)
    {
     /* copie all */
     strcpy(strf+*indexf, search+ptr_str);
     while (strf[*indexf])
      (*indexf)++;
     break;
    }
    continue;
   }
  }
  strf[(*indexf)++]= search[ptr_str++];
 }

 This->ret_type = TYPE_STRING;
 Xc_free(search);
 Xc_free(oldstr);
 Xc_free(newstr);
 return index+1;
}

/* ----------------------------------------------------------------- **
** supprespace(texte) return the text without any superfluous space  **
** ----------------------------------------------------------------- */
int i_supprespace(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;

 Xc_TRACE(("internal supprespace"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 index+=nindex-1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
  
 (*indexf)--;
 strf[*indexf]=0;
 *indexf = old_indexf;

 if (strf[*indexf]==' ')
 {
  old_indexf = *indexf;
  (*indexf)++;
  while (strf[*indexf]==' ')
   (*indexf)++;
  strcpy(strf+old_indexf, strf+*indexf);
  *indexf = old_indexf;
 }

 while (strf[*indexf])
 {
  if (strf[*indexf]==' ')
  {
   (*indexf)++;
   old_indexf = *indexf;
   while (strf[*indexf]==' ')
    (*indexf)++;
   if (*indexf != old_indexf)
   {
    strcpy(strf+old_indexf, strf+*indexf);
    *indexf = old_indexf;
   }
  }
  (*indexf)++;
 }
 This->ret_type = TYPE_STRING;
 return index+1;
}

/* ----------------------------------------------------------------- **
** t(value) return text if text                                      **
** ----------------------------------------------------------------- */
int i_t(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;

 Xc_TRACE(("internal t"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;

 /* look for first parameter string */
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;
  
 index+=nindex-1;
  
 if (end_type != TYPE_STRING)
 {
  strf[0]=0;
 }
 else
 {
  (*indexf)--;
  strf[*indexf]=0;
 }

 while (string[index]==' ')
  index++;

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }

 This->ret_type = TYPE_STRING;
 return index+1;
}

/*TODO*/
/* ----------------------------------------------------------------- **
** texte(value; format) return text formated as specified            **
** ----------------------------------------------------------------- */
int i_texte(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;

 Xc_TRACE(("internal text"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;

 strf[(*indexf)++]='(';
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;
 index += nindex-1;

 if (end_type == TYPE_STRING)
 {
  int i;
  (*indexf)--;
  strf[*indexf] = 0;
  i = old_indexf;
  while (strf[i] && (strf[i] >= '0') &&
	 (strf[i] <= '9'))
   i++;
  if (strf[i]=='.' || strf[i]==',')
   i++;
  while (strf[i] && (strf[i] >= '0') &&
	 (strf[i] <= '9'))
   i++;

  if (strf[i])
  {
   This->cell_s->status = TYPE_NO_ANSWER;
   return -1;
  }
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }

 strf[(*indexf)]=0;

 This->ret_type = TYPE_NUM;
 return index+1;
}

/* ----------------------------------------------------------------- **
** trouve(texte cherche; texte; depart) return pos of first occur    **
** ----------------------------------------------------------------- */
int i_trouve(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
 int	index;
 int	nindex;
 int	end_type;
 int	old_indexf;
 GEN	calc_x;
 ulong	avma_old;
 long	num;
 char	*search;
 char	*s_string;

 Xc_TRACE(("internal trouve"));
 if (string[0] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index = 1;
 while (string[index]==' ')
  index++;
 strf[(*indexf)++]='(';

 /* look for first parameter string */
 old_indexf = *indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;

 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  return -1;
 }

 index+=nindex-1;
 if (string[index]==';')
  index++;
 else
 {
  This->cell_s->status = TYPE_PARAM_LESS_ERROR;
  return -1;
 }

 search = (char *)Xc_malloc("str_cherche", strlen(strf+old_indexf)+1);
 strcpy(search, strf+old_indexf);

 /* look for second parameter string */
 *indexf = old_indexf;
 nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
 if (nindex < 2)
  return -1;
  
 if (end_type != TYPE_STRING)
 {
  This->cell_s->status = TYPE_MUST_BE_STRING;
  Xc_free(search);
  return -1;
 }

 index+=nindex-1;
 s_string = (char *)Xc_malloc("str_to find", strlen(strf+old_indexf)+1);
 strcpy(s_string, strf+old_indexf);

 /* look for third parameter number */
 *indexf = old_indexf;
 if (string[index]==';')
 {
  index++;
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
   Xc_free(search);
   Xc_free(s_string);
   return -1;
  default:
   This->cell_s->status = TYPE_CALC_ERROR;
   Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
   Xc_free(search);
   Xc_free(s_string);
   return -1;
  }
  *indexf = old_indexf;
  num = gtolong(calc_x);
  avma = avma_old;
  index += nindex-1;
 }
 else
 {
  if (string[index]!=')')
  {
   This->cell_s->status = TYPE_ERROR;
   return -1;
  }
  num = 0;
 }

 if (string[index]!=')')
 {
  if (string[index]==Xq_SEPAR)
   This->cell_s->status = TYPE_PARAM_MORE_ERROR;
  else
   This->cell_s->status = TYPE_MISSING_P_ERROR;
  Xc_free(search);
  Xc_free(s_string);
  return -1;
 }

 Xc_TRACE(("NuMbEr: %d", num));
  
 if ((num < 0) || (num > (int)strlen(s_string)))
 {
  This->cell_s->status = TYPE_D_OUTOFSTR_ERROR;
  Xc_free(search);
  Xc_free(s_string);
  return -1;
 }

 while (s_string[num])
 {
  if (s_string[num] == search[0])
  {
   int sav_num;
   int src_num;

   sav_num = num+1;
   src_num = 1;
   while (search[src_num] && (s_string[sav_num] == search[src_num]))
   {
    src_num++;
    sav_num++;
   }
   if (!search[src_num])
   {
    break;
   }
   if (!s_string[sav_num])
   {
    This->cell_s->status = TYPE_NO_ANSWER;
    Xc_free(search);
    Xc_free(s_string);
    return -1;
   }
  }
  num++;
 }
 if (!s_string[num])
 {
  This->cell_s->status = TYPE_NO_ANSWER;
  Xc_free(search);
  Xc_free(s_string);
  return -1;
 }
 sprintf(strf+*indexf, "%d.0)", (int)(num+1));
 while (strf[*indexf])
  (*indexf)++;

 This->ret_type = TYPE_NUM;
 Xc_free(search);
 Xc_free(s_string);
 return index+1;
}
