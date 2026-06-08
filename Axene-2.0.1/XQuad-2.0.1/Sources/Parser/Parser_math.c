/*
** Parser_math.c for XQuad in Parser/
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
** Last update Fri Dec 20 17:02:13 1996 Antoine Buat
*/

#include "Parser.h"

static char st_buf[300];

/* ----------------------------------------------------------------- ** 
** Alea entre borne (min; max)                                       ** 
** ----------------------------------------------------------------- */
int i_aleab(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	t_index;
  int	ini;
  int	end_type;

  Xc_TRACE(("internal aleab"));
  index = 0;
  while (string[index]==' ')
    index++;
  if (string[index] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  ini = *indexf;
  sprintf(strf+*indexf, "(random()%%(");
  Xc_TRACE(("ini: %s", strf+ini));
  *indexf += strlen(strf+*indexf);
  index++;
  t_index = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  if (end_type == TYPE_STRING)
    {
      This->cell_s->status = TYPE_NO_PARAM_NUM;
      return -1;
    }
  strncpy(st_buf, strf+t_index, *indexf-t_index);
  st_buf[*indexf-t_index]=0;
  *indexf = t_index;
  index += nindex;
  if (string[index-1]!=Xq_SEPAR)
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
  if (string[index-1]==')')
    {
      sprintf(strf+*indexf-1, "-%s)+%s)", st_buf, st_buf);
      *indexf += strlen(strf+*indexf);
    }
  else
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

/* ----------------------------------------------------------------- **
** atan2(num; base) return a atan2                                   **
** ----------------------------------------------------------------- */
int i_atan2(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;

  Xc_TRACE(("internal atan2"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;
  strf[(*indexf)++]='(';
  strf[(*indexf)++]='a';
  strf[(*indexf)++]='t';
  strf[(*indexf)++]='a';
  strf[(*indexf)++]='n';
  strf[(*indexf)++]='(';

  /* look for first parameter */
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  
  if (end_type == TYPE_STRING)
    {
      This->cell_s->status = TYPE_NO_PARAM_NUM;
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
  strf[(*indexf)++]='/';
  /* look for second parameter if exist*/
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  index += nindex-1;
  if (end_type == TYPE_STRING)
    {
      This->cell_s->status = TYPE_NO_PARAM_NUM;
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
  strf[(*indexf)++]=')';

  This->ret_type = TYPE_NUM;
  return index+1;
}

/* ----------------------------------------------------------------- **
** arrondi(num; decimal) return a rounded number                     **
** ----------------------------------------------------------------- */
int i_arrondi(This, string, strf, indexf)
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

  int	k;
  int	first_digit;
  int	first_decimal;
  int	nb_decimal;
  boolean comma;
  boolean carry;
  int	indexs;

  Xc_TRACE(("internal arrondi"));
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
  index += nindex-1;
  if (string[index]==';')
    index++;
  else
    {
      This->cell_s->status = TYPE_PARAM_LESS_ERROR;
      return -1;
    }


  /* look for second parameter if exist*/
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

  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }

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

  first_decimal = 0;
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
  k = 0;
  first_decimal--;
  while (first_decimal+prec > 0)
    {
      strf[(*indexf)++]=tmp_str[k];
      if (tmp_str[k]!='.')
	first_decimal--;
      k++;
    }
  if (k)
    {
      while (prec < 0)
	{
	  strf[(*indexf)++]='0';
	  prec++;
	}
    }
  else
    {
      strf[(*indexf)++]='0';
    }
    
  strf[*indexf]=0;
    
  free(tmp_str);

  This->ret_type = TYPE_NUM;
  return index+1;
}

/* ----------------------------------------------------------------- **
** base(num; basecible; precision) return a converted number         **
** ----------------------------------------------------------------- */
int i_base(This, string, strf, indexf)
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

  long	base;
  long	prec;

  int	ppoint;
  long	entier;
  double entierd;
  int	conv;
  double	decimal;

  Xc_TRACE(("internal base"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;

  /* look for first parameter num */
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
  ppoint = 0;
  while (tmp_str[ppoint] && (tmp_str[ppoint]!='.'))
    ppoint++;
  tmp_str[ppoint]=0;
  entier = (long)atof(tmp_str);
/*entier = (long)strtod(tmp_str, NULL);*/

  tmp_str[ppoint-1]='0';
  tmp_str[ppoint]='.';
  decimal = atof(tmp_str+ppoint);
/*decimal = strtod(tmp_str+ppoint, NULL);*/
  free(tmp_str);
  index += nindex-1;

  /* look for second parameter if exist*/
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
	  return -1;
	default:
	  This->cell_s->status = TYPE_CALC_ERROR;
	  Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
	  return -1;
	}
      *indexf = old_indexf;
      base = gtolong(calc_x);
      avma = avma_old;
      if (base > 36 || base < 2)
	{
	  This->cell_s->status = TYPE_VALUE_ERROR;
	  return -1;
	}
      index += nindex-1;
    }
  else
    {
      base = 16;
    }
      
  /* look for third parameter if exist*/
  if (string[index]==';')
    {
      index++;
      nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
      if (nindex < 2)
	{
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
	  return -1;
	default:
	  This->cell_s->status = TYPE_CALC_ERROR;
	  Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
	  return -1;
	}
      *indexf = old_indexf;
      prec = gtolong(calc_x);
      avma = avma_old;
      if (prec > 10 || prec < 0)
	{
	  This->cell_s->status = TYPE_VALUE_ERROR;
	  return -1;
	}
      index += nindex-1;
    }
  else
    {
      prec = 0;
    }
      
  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      free(tmp_str);
      return -1;
    }

  if(entier)
    {
      ppoint = 0;
      while (entier)
	{
	  conv = entier % base;
	  if (conv > 9)
	    st_buf[ppoint++] = conv+'A'-10;
	  else
	    st_buf[ppoint++] = conv+'0';
	  entier /= base;
	}
      while (ppoint)
	{
	  ppoint--;
	  strf[(*indexf)++]=st_buf[ppoint];
	}
    }
  else
    strf[(*indexf)++]='0';

  if (prec)
    {
      strf[(*indexf)++]='.';
      entierd = (pow((double)base, (double)prec)*decimal);
      ppoint = 0;
      while (entierd >= 1.0)
	{
	  /*conv = entierd % base;*/
	  conv = entierd - (floor(entierd / base) * base);
	  if (conv > 9)
	    st_buf[ppoint++] = conv+'A'-10;
	  else
	    st_buf[ppoint++] = conv+'0';
	  entierd = floor(entierd/base);
	}
      while (ppoint)
	{
	  ppoint--;
	  strf[(*indexf)++]=st_buf[ppoint];
	}
    }
  strf[*indexf]=0;

  This->ret_type = TYPE_STRING;
  return index+1;
}

/* ----------------------------------------------------------------- **
** log(num; base) return a log in base                               **
** ----------------------------------------------------------------- */
int i_log(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;
  GEN	calc_x;
  ulong	avma_old;
  int	old_indexf;

  Xc_TRACE(("internal log"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;
  strf[(*indexf)++]='(';
  strf[(*indexf)++]='(';
  strf[(*indexf)++]='l';
  strf[(*indexf)++]='n';
  strf[(*indexf)++]='(';

  /* look for first parameter */
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    {
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
      return -1;
    default:
      This->cell_s->status = TYPE_CALC_ERROR;
      Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
      return -1;
    }
  if (gcmpsg((long)0, calc_x)>=0)
    {
      avma = avma_old;
      This->cell_s->status = TYPE_D_OVERZERO_ERROR;
      return -1;
    }
  avma = avma_old;

  index += nindex-1;

  strf[(*indexf)++]=')';
  strf[(*indexf)++]='/';
  strf[(*indexf)++]='l';
  strf[(*indexf)++]='n';
  strf[(*indexf)++]='(';
  /* look for second parameter if exist*/
  if (string[index]==';')
    {
      index++;
      old_indexf = *indexf;
      nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
      if (nindex < 2)
	return -1;
      index += nindex-1;
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
      if (gcmpsg((long)0, calc_x)>=0)
	{
	  avma = avma_old;
	  This->cell_s->status = TYPE_D_OVERZERO_ERROR;
	  return -1;      
	}
      avma = avma_old;
    }
  else
    {
      strf[(*indexf)++]='1';
      strf[(*indexf)++]='0';
    }

  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  strf[(*indexf)++]=')';
  strf[(*indexf)++]=')';

  This->ret_type = TYPE_NUM;
  return index+1;
}

/* ----------------------------------------------------------------- **
** log10(num) return a log in base 10                                **
** ----------------------------------------------------------------- */
int i_log10(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;

  Xc_TRACE(("internal log10"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;
  strf[(*indexf)++]='(';
  strf[(*indexf)++]='(';
  strf[(*indexf)++]='l';
  strf[(*indexf)++]='n';
  strf[(*indexf)++]='(';

  /* look for first parameter */
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  
  if (end_type == TYPE_STRING)
    {
      This->cell_s->status = TYPE_NO_PARAM_NUM;
      return -1;
    }
  index += nindex-1;

  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }

  strf[(*indexf)++]=')';
  strf[(*indexf)++]='/';
  strf[(*indexf)++]='l';
  strf[(*indexf)++]='n';
  strf[(*indexf)++]='(';
  strf[(*indexf)++]='1';
  strf[(*indexf)++]='0';
  strf[(*indexf)++]=')';
  strf[(*indexf)++]=')';

  This->ret_type = TYPE_NUM;
  return index+1;
}

/* ----------------------------------------------------------------- **
** mod(num; diviseur) return a modulus of number                     **
** ----------------------------------------------------------------- */
int i_mod(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;

  Xc_TRACE(("internal base"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;
  strf[(*indexf)++]='(';
  strf[(*indexf)++]='(';

  /* look for first parameter */
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  
  if (end_type == TYPE_STRING)
    {
      This->cell_s->status = TYPE_NO_PARAM_NUM;
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

  strf[(*indexf)++]=')';
  strf[(*indexf)++]='%';
  /* look for second parameter if exist*/
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  index += nindex-1;
  if (end_type == TYPE_STRING)
    {
      This->cell_s->status = TYPE_NO_PARAM_NUM;
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

  This->ret_type = TYPE_NUM;
  return index+1;
}

/* ----------------------------------------------------------------- ** 
** somme (zone|const|cell; ...)                                      ** 
** ----------------------------------------------------------------- */
int i_somme(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	sortie;
  int	nindex;
  int	type;
  int	type2;
  GEN	gentmp = gzero;
  ulong	avma_old;

  Xc_TRACE(("internal somme"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  strf[(*indexf)++]='(';
  index = 1;
  while (string[index]==' ')
    index++;
  sortie = 0;
  avma_old = avma;
  while (!sortie)
    {
      nindex = F(This).srparse(This, string+index, strf, indexf,
			       &type, &type2);
      if (nindex < 1)
	{
	  avma = avma_old;
	  return -1;
	}
      index += nindex;

      if (type2 == TYPE_STRING)
	{
	  This->cell_s->status = TYPE_NO_PARAM_STRING;
	  avma = avma_old;
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
	  char	*tmp_str;
	  int	coma;
	  int	val;
	  int	anybody;

	  anybody = 0;
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

	  strf[(*indexf)++]='(';
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
			  avma = avma_old;
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
			  if (!anybody)
			    {
			      anybody = 1;
			      gentmp = gcopy(p_line->cell.value2);
			    }
			  else
			    {
			      gentmp = gadd(gentmp, p_line->cell.value2);
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
	  if (!anybody)
	    {
	      strcpy(strf+(*indexf), "0.0)");
	      *indexf += 4;
	    }
	  else
	    {
	      tmp_str = g2str(gentmp, BIG_PREC);
	      strcpy(strf+*indexf, tmp_str);
	      val = strlen(strf+*indexf);
	      i = 0;
	      coma = 0;
	      while (i < val)
		{
		  if (strf[*indexf+i]=='.')
		    {
		      coma = 1;
		      break;
		    }
		  i++;
		}
	      if (!coma)
		{
		  strf[*indexf+val++]='.';
		  strf[*indexf+val++]='0';
		  strf[*indexf+val]=0;
		}
	      free(tmp_str);
	      *indexf += val;
	      strf[(*indexf)++]=')';
	    }
	  Xc_TRACE(("String final: %s", strf));
	  continue;
	}

      Xc_TRACE(("String: %s", strf));
      if (string[index-1]==Xq_SEPAR)
	{
	  while (string[index]==' ')
	    index++;
	  strf[(*indexf)++]='+';
	  continue;
	}
      if (string[index-1]==')')
	{
	  /* en parenthesis was auto added by gettoken */
	  sortie = 1;
	}
    }
  This->ret_type = TYPE_NUM;
  avma = avma_old;
  return index;
}

/* ----------------------------------------------------------------- ** 
** produit (zone|const|cell; ...)                                      ** 
** ----------------------------------------------------------------- */
int i_prod(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	sortie;
  int	nindex;
  int	type;
  int	type2;
  ulong	avma_old;

  Xc_TRACE(("internal produit"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  strf[(*indexf)++]='(';
  index = 1;
  while (string[index]==' ')
    index++;
  sortie = 0;
  while (!sortie)
    {
      nindex = F(This).srparse(This, string+index, strf, indexf,
			       &type, &type2);
      if (nindex < 1)
	return -1;
      index += nindex;

      if (type2 == TYPE_STRING)
	{
	  This->cell_s->status = TYPE_NO_PARAM_STRING;
	  return -1;
	}

      avma_old = avma;
      if (type == TYPE_CELL_ZONE)
	{
	  int	zone_x_s;
	  int	zone_y_s;
	  int	zone_x_e;
	  int	zone_y_e;
	  int	i;
	  c_col	*p_col;
	  c_line *p_line;
	  char	*tmp_str;
	  int	coma;
	  int	val;
	  int	anybody;
	  GEN	gentmp = gzero;

	  anybody = 0;
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

	  strf[(*indexf)++]='(';
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
			  avma = avma_old;
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
			  if (!anybody)
			    {
			      anybody = 1;
			      gentmp = gcopy(p_line->cell.value2);
			    }
			  else
			    {
			      gentmp = gmul(gentmp, p_line->cell.value2);
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
	  if (!anybody)
	    {
	      (*indexf)--;
	      if (strf[*indexf-1]=='*')
		(*indexf)--;
	    }
	  else
	    {
	      tmp_str = g2str(gentmp, BIG_PREC);
	      strcpy(strf+*indexf, tmp_str);
	      val = strlen(strf+*indexf);
	      i = 0;
	      coma = 0;
	      while (i < val)
		{
		  if (strf[*indexf+i]=='.')
		    {
		      coma = 1;
		      break;
		    }
		  i++;
		}
	      if (!coma)
		{
		  strf[*indexf+val++]='.';
		  strf[*indexf+val++]='0';
		  strf[*indexf+val]=0;
		}
	      free(tmp_str);
	      *indexf += val;
	      strf[(*indexf)++]=')';
	    }
	  Xc_TRACE(("String final: %s", strf));	  
	  continue;
	}
      avma = avma_old;
      Xc_TRACE(("String: %s", strf));
      if (string[index-1]==Xq_SEPAR)
	{
	  while (string[index]==' ')
	    index++;
	  strf[(*indexf)++]='*';
	  continue;
	}
      if (string[index-1]==')')
	{
	  /* en parenthesis was auto added by gettoken */
	  if (strf[*indexf-2]=='(')
	    {
	      strf[*indexf-1]='0';
	      strf[*indexf]='.';
	      (*indexf)++;
	      strf[*indexf]='0';
	      (*indexf)++;
	      strf[*indexf]=')';
	      (*indexf)++;
	    }
	  sortie = 1;
	}
    }
  This->ret_type = TYPE_NUM;
  return index;
}

/* ----------------------------------------------------------------- **
** tronque(num; decimal) return a truncated number                   **
** ----------------------------------------------------------------- */
int i_tronque(This, string, strf, indexf)
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
  int	ppoint;
  int	k;

  Xc_TRACE(("internal arrondi"));
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
  tmp_str = g2str(calc_x, LOW_PREC);
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
    prec = 0;

  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }

  ppoint=0;
  while(tmp_str[ppoint] && (tmp_str[ppoint]!='.'))
    ppoint++;

  k=0;
  while (ppoint+prec)
    {
      strf[(*indexf)++]=tmp_str[k];
      if (tmp_str[k]!='.')
	ppoint--;
      k++;
    }
  if (k)
    {
      while (prec < 0)
	{
	  strf[(*indexf)++]='0';
	  prec++;
	}
    }
  else
    strf[(*indexf)++]='0';
    
    
  strf[*indexf]=0;
  free(tmp_str);

  This->ret_type = TYPE_NUM;
  return index+1;
}
