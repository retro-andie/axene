/*
** Parse_token.c for XQuad in Parser/
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
** Last update Fri Oct  2 17:02:18 1998 Robin Castanier
*/
#include "Parser.h"
#include "Parse_tokenP.h"
#include "Resource.h"
#include "char_translate.h"

extern c_Resource *GlobResources;

/*--- Test BINARYOP ---*/
#define NUMBER_OP	11
static op_list	op[NUMBER_OP] =
{
 {1, "%", 1, "%", FALSE},
 {1, "&", 1, "&", FALSE},
 {1, "*", 1, "*", FALSE},
 {1, "+", 1, "+", FALSE},
 {1, "-", 1, "-", FALSE},
 {1, "/", 1, "/", FALSE},
 {1, "<", 1, "<", FALSE},
 {1, "=", 2, "==", FALSE},
 {1, ">", 1, ">", FALSE},
 {1, "^", 1, "^", FALSE},
 {1, "|", 1, "|", FALSE}
};
/*  {2, ">>", 1, */
/*  {1, "@", 3, "mod", TRUE} */
/*
  constraint to optimize parsing:
  -operators must be classified in ascii order 
  -place always Axxx before A (ex: '>>' before '>')
  */

int get_token_binaryop(This, string, type, strf, indexf)
c_Parser	*This;
char		*string;
int		*type;
char		*strf;
int		*indexf;
{
 int	op_num;

 op_num = 0;
 while (op_num < NUMBER_OP)
 {
  if (string[0] == op[op_num].name[0])
  {
   /* compare if really found else try other*/
   if (strncmp(string,
	       op[op_num].name,
	       op[op_num].size) == 0)
   {
    strncpy(strf+*indexf, op[op_num].outname,
	    op[op_num].outsize);
    *indexf += op[op_num].outsize;
    *type = TYPE_OPERATOR;
    return op[op_num].size;
   }
  }
  if (string[0] < op[op_num].name[0])
  {
   /* binary operand not found return an error */
   *type = TYPE_CHAR_ERROR;
   return -1;
  }
  op_num ++;
 }
 return -1;
}

/*--- Test UNARYOP ---*/
#define NUMBER_UNARY_OP	2
static op_list	unary_op[NUMBER_UNARY_OP] =
{
 {1, "+", 1, "+", FALSE},
 {1, "-", 1, "-", FALSE}
};

int get_token_unaryop(This, string, type, strf, indexf)
c_Parser	*This;
char		*string;
int		*type;
char		*strf;
int		*indexf;
{
 int	op_num;

 op_num = 0;
 while (op_num < NUMBER_UNARY_OP)
 {
  if (string[0] == unary_op[op_num].name[0])
  {
   /* compare if really found else try other*/
   if (strncmp(string,
	       unary_op[op_num].name,
	       unary_op[op_num].size) == 0)
   {
    strncpy(strf+*indexf, unary_op[op_num].outname,
	    unary_op[op_num].outsize);
    *indexf += unary_op[op_num].outsize;
    *type = TYPE_U_OPERATOR;
    return unary_op[op_num].size;
   }
  }
  if (string[0] < unary_op[op_num].name[0])
  {
   /* unary operand not found return an error */
   *type = TYPE_CHAR_ERROR;
   return -1;
  }
  op_num ++;
 }
 return -1;
}

/*--- Test FUNCTION ---*/
static int compute_func(This, string, strf, indexf, tmpfunc)
c_Parser	 *This;
char		*string;
char		*strf;
int		*indexf;
def_func	*tmpfunc;
{
 int	index;
 int	sortie;
 int	nindex;
 int	param_nbr;
 int	save_indexf;
 int	end_type;

 Xc_TRACE(("generic function"));
 index = 0;
 while (string[index]==' ')
  index++;
 if (tmpfunc->nbr_param == P_NONE)
 {
  if (string[index] == '(')
  {
   index++;
   while (string[index]==' ')
    index++;
   if (string[index] == ')')
    return index+1;
   else
   {
    if (string[index])
     This->cell_s->status = TYPE_PARAM_MORE_ERROR;
    else
     This->cell_s->status = TYPE_MISSING_P_ERROR;
    return -1;
   }
  }
  return 0;
 }
 param_nbr = 0;
 if (string[index] != '(')
 {
  This->cell_s->status = TYPE_MISSING_P_ERROR;
  return -1;
 }
 index++;
 strf[(*indexf)++]='(';
 sortie = 0;
 while (!sortie)
 {
  save_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  param_nbr++;
  Xc_TRACE(("nindex %d", nindex));
  if (nindex < 2)
   return -1;

  if(tmpfunc->domain != ALL)
  {
   char	save_car;
   char	*tmp_str;
   GEN	gtmp;
   int	i;
   int	j;

   save_car = strf[*indexf-1];
   strf[*indexf-1] = 0;
   GlobErrNum = 0;
   gtmp=lisexpr2(strf+save_indexf);
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
   switch(tmpfunc->domain)
   {
   case ALLANDINT:
    if (param_nbr == 1)
     break;
   case INTEGER:
    tmp_str = g2str(gtmp, BIG_PREC);
    i=0;
    while ((*(tmp_str+i) != '.') &&
	   (*(tmp_str+i) != 0))
     i++;
    if (*(tmp_str+i) != 0)
    {
     j=i+1;
     while ((*(tmp_str+j) == '0') &&
	    (*(tmp_str+j) != 0))
      j++;
     if (*(tmp_str+j) != 0)
     {
      This->cell_s->status = TYPE_D_INT_ERROR;
      Xc_free(gtmp);
      return -1;
     }
    }
    strncpy(strf+save_indexf, tmp_str, i);
    free(tmp_str);
    *indexf = save_indexf+i+1;
    break;
   case ONEONE:
    if (gcmpsg((long)-1, gtmp)>0)
    {
     This->cell_s->status = TYPE_D_ONEONE_ERROR;
     Xc_free(gtmp);
     return -1;
    }
    if (gcmpsg((long)1, gtmp)<0)
    {
     This->cell_s->status = TYPE_D_ONEONE_ERROR;
     Xc_free(gtmp);
     return -1;
    }
    break;
   case OVERONE:
    if (gcmpsg((long)1, gtmp)>0)
    {
     This->cell_s->status = TYPE_D_OVERONE_ERROR;
     Xc_free(gtmp);
     return -1;
    }
    break;
   case ONEONEOPEN:
    if (gcmpsg((long)-1, gtmp)>=0)
    {
     This->cell_s->status = TYPE_D_ONEONEOPEN_ERROR;
     Xc_free(gtmp);
     return -1;
    }
    if (gcmpsg((long)1, gtmp)<=0)
    {
     This->cell_s->status = TYPE_D_ONEONEOPEN_ERROR;
     Xc_free(gtmp);
     return -1;
    }
    break;
   case ALLEZERO:
    if (gcmp0(gtmp))
    {
     This->cell_s->status = TYPE_D_ALLEZERO_ERROR;
     Xc_free(gtmp);
     return -1;
    }
    break;
   case OVERZERO:
    if (gcmpsg((long)0, gtmp)>=0)
    {
     This->cell_s->status = TYPE_D_OVERZERO_ERROR;
     Xc_free(gtmp);
     return -1;
    }
    break;
   case POSITIVE:
    if (gcmpsg((long)0, gtmp)>0)
    {
     This->cell_s->status = TYPE_D_POSITIVE_ERROR;
     Xc_free(gtmp);
     return -1;
    }
    break;
   }

   strf[*indexf-1] = save_car;
   Xc_free(gtmp);
  }

  index += nindex;
  if (string[index-1]==Xq_SEPAR)
  {
   if ((param_nbr+1 > tmpfunc->nbr_param) &&
       (tmpfunc->nbr_param != P_ANY))
   {
    This->cell_s->status = TYPE_PARAM_MORE_ERROR;
    return -1;
   }
   while (string[index]==' ')
    index++;
   strf[(*indexf)++]=Xq_PARI_SEPAR;
  }
  if (string[index-1]==')')
  {
   if ((param_nbr == tmpfunc->nbr_param) 
       || (tmpfunc->nbr_param == P_ANY))
   {
    sortie = 1;
   }
   else
   {
    This->cell_s->status = TYPE_PARAM_LESS_ERROR;
    return -1;
   }
  }
 }
 return index;
}

int get_token_function(This, string, type, strf, indexf)
c_Parser	*This;
char		*string;
int		*type;
char		*strf;
int		*indexf;
{
 char		*tok_str;
 int		e_index;
 int		size;
 def_func	*tmpfunc;
 int		var_test = 0;

 e_index = 0;
 while (Xc_IS_LETTER(string[e_index])
	|| (string[e_index] == '$')
	|| (string[e_index] == '_')
	|| (string[e_index] == '.')
	|| (string[e_index] == ':')
	|| ((string[e_index] >= '0' && string[e_index] <= '9')))
  e_index++;
 size = e_index;
 tok_str = (char *)Xc_malloc("tok_str1", size+1);
 strncpy(tok_str, string, size);
 tok_str[size] = '\0';
 Xc_TRACE(("Token: %s", tok_str));

 tmpfunc = This->func;
 while (tmpfunc)
 {
  if (Xc_strcmp_parser(tok_str, tmpfunc->name))
  {
   if (tmpfunc->outname)
   {
    *(strf+*indexf)='(';
    (*indexf)++;
    strncpy(strf+*indexf, tmpfunc->outname,
	    tmpfunc->outsize);
    *indexf += tmpfunc->outsize;
    var_test = compute_func(This, string+size,
			    strf, indexf, tmpfunc);
    *(strf+*indexf) = ')';
    (*indexf)++;
    if (var_test == -1)
    {
     Xc_free(tok_str);
     *type = This->cell_s->status;
     return -1;
    }
    *type = TYPE_FUNC;
   }
   if (tmpfunc->func)
   {
    while (string[size]==' ')
     size++;
    current_depth ++;
    var_test = tmpfunc->func(This, string+size,
			     strf, indexf);
    current_depth --;
    if (var_test == -1)
    {
     Xc_free(tok_str);
     *type = This->cell_s->status;
     return -1;
    }
    *type = This->ret_type;
   }
   Xc_free(tok_str);
   Xc_TRACE(("function found"));
   return size+var_test;
  }
  tmpfunc = tmpfunc->Next;
 }
 Xc_free(tok_str);
 *type = TYPE_NAME_ERROR;
 Xc_TRACE(("no variable or function found"));
 return -1;
}

/*--- Test VARIABLE ---*/
int get_token_variable(This, string, type)
char		*string;
int		*type;
c_Parser	*This;
{
 /*
   def_var	*tmpvar;
   
   tmpvar = This->var;
   while (tmpvar)
   {
   if (strcmp(tmpvar->name, tok_str) == 0)
   {
   Xc_free(tok_str);
   *type = TYPE_VAR_NUM;
   Xc_TRACE(("variable found"));
   return index+size;
   }
   tmpvar = tmpvar->Next;
   }
   */
 return -1;
}

/*--- Test NUMBER ---*/
int get_token_number(This, string, type, strf, indexf)
c_Parser	*This;
char		*string;
int		*type;
char		*strf;
int		*indexf;
{
 int	index;
 int	prec;
 int	prec_pos;
 int	exp;
 int	exp_real;
 int	exp_sign;
 char	strtmp[10];

 index = 0;
 prec = 0;
 prec_pos = 0;
 exp = 0;
 exp_real = 0;
 exp_sign = 1;
 if (string[index] >= '0' && string[index] <= '9')
 {
  while ((string[index] >= '0') && (string[index] <= '9') &&
	 (prec < LOW_PREC))
  {
   prec++;
   strf[(*indexf)++] = string[index];
   index++;
  }
  while ((string[index] >= '0') && (string[index] <= '9'))
  {
   exp++;
   index++;
  }
  if (string[index] == '.' || string[index] == ',')
  {
   strf[(*indexf)++] = '.';
   if (prec >= LOW_PREC)
    strf[(*indexf)++] = '0';
	    
   index++;
   while ((string[index] >= '0') && (string[index] <= '9') &&
	  (prec < LOW_PREC))
   {
    prec++;
    strf[(*indexf)++] = string[index];
    index++;
   }
   while ((string[index] >= '0') && (string[index] <= '9'))
   {
    index++;
   }
  }
  else
  {
   strf[(*indexf)++] = '.';
   strf[(*indexf)++] = '0';
  }

  /* test for exponentiel */
  *type = TYPE_NUM;
  while (string[index] == ' ')
   index++;
  if (string[index] == 'e' || string[index] == 'E')
  {
   *type = TYPE_EXPONANTIAL;
   index++;
   while (string[index] == ' ')
    index++;
   if (string[index] == '+')
    index++;
   else if (string[index] == '-')
   {
    exp_sign = -1;
    index++;
   }
   while (string[index] == ' ')
    index++;
   while (string[index] >= '0' && string[index] <= '9')
   {
    exp_real = (exp_real*10)+string[index]-'0';
    index++;
   }
   exp_real *= exp_sign;
   exp_real += exp;
   strf[*indexf] = 0;
   sprintf(strtmp,"E%d", exp_real);
   strcat(strf, strtmp);
   (*indexf) += strlen(strtmp);
  }
  else if (exp)
  {
   *type = TYPE_EXPONANTIAL;
   strf[*indexf] = 0;
   sprintf(strtmp,"E%d", exp);
   strcat(strf, strtmp);
   (*indexf) += strlen(strtmp);
  }
  return index;
 }
 return -1;
}


int get_num(string, j)
char	*string;
int	*j;
{
 int	i;
 int	num;

 num = 0;
 i = *j;
 if (isdigit(string[i]))
 {
  num = 0;
  while(isdigit(string[i]))
  {
   num *= 10;
   num += (string[i] - '0');
   i++;
  }
  *j = i;
  return num;
 }
 return 0;
}

int get_month(string, p_i, zerofirst)
char	*string;
int	*p_i;
boolean *zerofirst;
{
 int	month;
 char	*month_name;
 char	buffer[Xq_FORMULA_LENGTH];	
 char	c1, c2;
 int	i, j, k;

 month = 0;
 i = *p_i;
 if (Xc_IS_LETTER(string[i]))
 {
  j = 0;
  while(Xc_IS_LETTER(string[i]))
  {
   buffer[j] = string[i];
   j++; i++;
  }
  buffer[j] = '\0';
	  
  if (j < 3) return -1;
	  
  if (j == 3)
  {
   month = 0;
   while(month < 12)
   {
    month_name = 
     F(GlobResources).getString(GlobResources,
				XcR_dateMonthJanShort + month);
    k = -1;
    do
    {
     k++;
     c1 = Xc_TO_LOWER(buffer[k]);
     c2 = Xc_TO_LOWER(month_name[k]);
    }
    while((k < 3) && 
	  (c1 == c2 || c1 == Xc_SUPPRESS_ACCENT(c2)));
		  
    if (k == 3)
     break;
	
    month ++;
   }
   if (month >= 12) return -1;
   month ++;
  }
  else
  {
   /* last look for month long */
   month = 0;
   while(month < 12)
   {
    month_name = 
     F(GlobResources).getString(GlobResources,
				XcR_dateMonthJan + month);
		  
    k = -1;
    do
    {
     k++;
     c1 = Xc_TO_LOWER(buffer[k]);
     c2 = Xc_TO_LOWER(month_name[k]);
    }
    while((k < j) && 
	  (c1 == c2 || c1 == Xc_SUPPRESS_ACCENT(c2)));
	      
    if (k == j)
     break;
    month ++;
   }
   if (month >= 12) return -1;
   month ++;
  }
  *p_i = i;
  return month;
 }

 if (isdigit(string[i]) || string[i] == '-')
 {
  int sign;
      
  *zerofirst = FALSE;
  if (string[i] == '-')
  {
   sign = -1;
   i ++;
  }
  else
   sign = 1;

  if (string[i] == '0') 
   *zerofirst = TRUE;
  if (isdigit(string[i]))
  {
   month = 0;
   while(isdigit(string[i]))
   {
    month *= 10;
    month += (string[i] - '0');
    i++;
   }
   month *= sign;

   *p_i = i;
   return month;
  }
  return -1;
 }
 return 0;
}

/* date is conditionned by the external ressource dateEntryMode */
/*--- Test DATE ---*/
int get_token_date(This, string, type, strf, indexf)
c_Parser	*This;
char		*string;
int		*type;
char 		*strf;
int		*indexf;
{
 int		day, month, year;
 xc_date_t	xc_date_a;
 boolean	valid;
 double	date_num;
 boolean	zerofirst;
 int		i;

 day = month = year = 0;

 i = 0;
 while(string[i] == ' ')
  i++;

 if (string[i] == '\0') return -1;

 This->date_entry_mode = DAY_FIRST;
 if (This->date_entry_mode == DAY_FIRST)
 {
  /* if DIGIT => day or month*/
  day = get_num(string, &i);
  if (day)
  {
   if (string[i] == '\0' || day == 0 || day > 31) return -1;
   while(string[i] == ' ')
    i++;
   if (string[i] == '\0') return -1;
   if (string[i] == '-' || string[i] == '/')
    i++;
   else
    return -1;
   while(string[i] == ' ')
    i++;
   if (string[i] == '\0') return -1;
  }

  /* if LETTER => month */
  month = get_month(string, &i, &zerofirst);
  if (month <= 0 || month > 12)
  {
   if (day)
   {
    year = month;
    month = day;
    day = 1;
	      
    if (month <= 0 || month > 12) return -1;	
    if (!zerofirst || year == 0)
    {
     if (year >= 0 && year < 20)/* ok month and year */
      year += 2000;
     else
     {
      if (year >= 20 && year <= 99)
       year += 1900;
     }
    }
    Xc_TRACE(("found %d/%d/%d", day, month, year));

    fill_xc_date(&xc_date_a);
    xc_date_a.day = 1;
    xc_date_a.month = month;
    xc_date_a.year = year;
    date_num = convert_to_xc_date(&xc_date_a, &valid);
    if (valid)
    {
     sprintf(strf+*indexf, "%f", date_num);
     while (strf[*indexf])
      (*indexf)++;
     *type = TYPE_DATE;
     return i;
    }
    return -1;
   }
   else
    return -1;
  }
 }

 while(string[i] == ' ')
  i++;

 if (string[i] == '\0' && day && month)/* OK day and month name */
 {
  Xc_TRACE(("found %d/%d", day, month));
  fill_xc_date(&xc_date_a);
  xc_date_a.day = day;
  xc_date_a.month = month;
  date_num = convert_to_xc_date(&xc_date_a, &valid);
  if (valid)
  {
   sprintf(strf+*indexf, "%f", date_num);
   while (strf[*indexf])
    (*indexf)++;
   *type = TYPE_DATE;
   return i;
  }
  return -1;
 }

 if (!day)
  day = 1;
  
 if (string[i] == '-' || string[i] == '/')
  i++;
 else
  return -1;
 while(string[i] == ' ')
  i++;
 if (string[i] == '\0') 
  return -1;
  
 if (isdigit(string[i]) || string[i] == '-')
 {
  int sign;
  boolean zerofirst = FALSE;
    
  if (string[i] == '-')
  {
   sign = -1;
   i ++;
  }
  else
   sign = 1;
    
  if (!isdigit(string[i])) return -1;
  if (string[i] == '0') zerofirst = TRUE;
  year = 0;
  while(isdigit(string[i]))
  {
   year *= 10;
   year += (string[i] - '0');
   i++;
  }
  year *= sign;
    
  while(string[i] == ' ')
   i++;
    
  if (string[i] == '\0')
  {
   if (!zerofirst || year == 0)
   {
    if (year >= 0 && year < 20) /* ok month and year */
     year += 2000;
    else
    {
     if (year >= 20 && year <= 99)
      year += 1900;
    }
   }
   Xc_TRACE(("found %d/%d/%d", day, month, year));
   fill_xc_date(&xc_date_a);
   xc_date_a.day = day;
   xc_date_a.month = month;
   xc_date_a.year = year;
   date_num = convert_to_xc_date(&xc_date_a, &valid);
   if (valid)
   {
    sprintf(strf+*indexf, "%f", date_num);
    while (strf[*indexf])
     (*indexf)++;
    *type = TYPE_DATE;
    return i;
   }      
  }
 }
 return -1;
}

/*--- Test HOUR ---*/
int get_token_hour(This, string, type, strf, indexf)
c_Parser	*This;
char		*string;
int		*type;
char 		*strf;
int		*indexf;
{
 char		buffer[5];
 int		bufi;
 int		i;
 int		hour;
 int		minu;
 int		sec;
 int		mil;
 xc_time_t	xc_time_a;
 double	time_num;
 boolean	valid;

 /* first look for hour */
 i = 0;
 bufi = 0;
 minu = 0;
 sec = 0;
 mil = 0;
 while (string[i] && (string[i]>='0') && (string[i]<='9') && (bufi < 3))
 {
  buffer[bufi]=string[i];
  bufi++;
  i++;
 }
 if (!bufi || (bufi > 2))
  return -1;
 buffer[bufi]=0;
 hour = atoi(buffer);
 if ((hour > 23) || (hour < 0))
  return -1;
  
 if (string[i] == ':')
  i++;
 else
  return -1;
 
 /* then look for minute */
 if (string[i])
 {
  bufi = 0;
  while (string[i] && (string[i]>='0') && (string[i]<='9') && (bufi < 3))
  {
   buffer[bufi]=string[i];
   bufi++;
   i++;
  }
  if (!bufi || (bufi > 2))
   return -1;
  buffer[bufi]=0;
  minu = atoi(buffer);
  if ((minu > 59) || (minu < 0))
   return -1;
 }

 /* then look for seconde */
 if (string[i])
 {
  if (string[i] == ':')
   i++;
  else
   return -1;
  bufi = 0;
  while (string[i] && (string[i]>='0') && (string[i]<='9') && (bufi < 3))
  {
   buffer[bufi]=string[i];
   bufi++;
   i++;
  }
  if (!bufi || (bufi > 2))
   return -1;
  buffer[bufi]=0;
  sec = atoi(buffer);
  if ((sec > 59) || (sec < 0))
   return -1;
 }

 /* then look for mili seconde */
 if (string[i])
 {
  if (string[i] == '.')
   i++;
  else
   return -1;
  bufi = 0;
  while (string[i] && (string[i]>='0') && (string[i]<='9') && (bufi < 4))
  {
   buffer[bufi]=string[i];
   bufi++;
   i++;
  }
  if (!bufi || (bufi > 3))
   return -1;
  buffer[bufi]=0;
  mil = atoi(buffer);
  switch (bufi)
  {
  case 1:
   mil *= 100;
   break;
  case 2:
   mil *= 10;	  
  }
 }

 if (string[i])
 {
  return -1;
 }
  
 Xc_TRACE(("found %d:%d:%d.%d", hour, minu, sec, mil));
 xc_time_a.hour = hour;
 xc_time_a.minute = minu;
 xc_time_a.second = sec;
 xc_time_a.millisecond = mil;
 time_num = convert_to_xc_time(&xc_time_a, &valid);
 if (valid)
 {
  sprintf(strf+*indexf, "%.20f", time_num);
  while (strf[*indexf])
   (*indexf)++;
  *type = TYPE_TIME;
  return i;
 }
 else
  return -1;
}

/*--- Test DAYINWEEK ---*/
int get_token_day(This, string, type, strf, indexf)
c_Parser	*This;
char		*string;
int		*type;
char 		*strf;
int		*indexf;
{
 char		*day_name;
 char		buffer[Xq_FORMULA_LENGTH];
 char		c1, c2;
 int		nbs, i;
 int		length;
 int		day;

 nbs = length = 0;
 while(string[nbs] == ' ')
  nbs++;
  
 i = nbs;
 while(Xc_IS_LETTER(string[i]))
 {
  buffer[length] = string[i];
  length++; i++;
 }
 if (length < 3)
  return -1;
  
 buffer[length] = '\0';
      
 /* fist look for day short */
 if (length == 3)
 {
  day = 0;
  while(day < 7)
  {
   day_name = F(GlobResources).getString(GlobResources,
					 XcR_dateDayMonShort + day);
      
   i = -1;
   do
   {
    i++;
    c1 = Xc_TO_LOWER(buffer[i]);
    c2 = Xc_TO_LOWER(day_name[i]);
   }
   while( (i < 3) && (c1 == c2 || c1 == Xc_SUPPRESS_ACCENT(c2)));
      
   if (i == 3)
    break;
      
   day ++;
  }
  if (day < 7)
  {
   sprintf(strf+*indexf, "%d.0", day + 1);
   while (strf[*indexf])
    (*indexf)++;
   *type = TYPE_DAY_IN_WEEK;
   return i + nbs;
  }
  return -1;
 }
  
 /* last look for day long */
 day = 0;
 while(day < 7)
 {
  day_name = F(GlobResources).getString(GlobResources,
					XcR_dateDayMon + day);
    
  i = -1;
  do
  {
   i++;
   c1 = Xc_TO_LOWER(buffer[i]);
   c2 = Xc_TO_LOWER(day_name[i]);
  }
  while( (i < length) && (c1 == c2 || c1 == Xc_SUPPRESS_ACCENT(c2)));
    
  if (i == length)
   break;
    
  day ++;
 }
 if (day < 7)
 {
  sprintf(strf+*indexf, "%d.0", day + 1);
  while (strf[*indexf])
   (*indexf)++;
  *type = TYPE_DAY_IN_WEEK;
  return i + nbs;
 }
 return -1;
}

/*--- Test MONTH ---*/

int get_token_month(This, string, type, strf, indexf)
c_Parser	*This;
char		*string;
int		*type;
char 		*strf;
int		*indexf;
{
 char		*month_name;
 char		buffer[Xq_FORMULA_LENGTH];
 char		c1, c2;
 int		nbs, i;
 int		length;
 int		month;
  
 nbs = length = 0;
 while(string[nbs] == ' ')
  nbs++;
  
 i = nbs;
 while(Xc_IS_LETTER(string[i]))
 {
  buffer[length] = string[i];
  length++; i++;
 }
 if (length < 3)
  return -1;
  
 buffer[length] = '\0';
  
 /* first look for month short */
 if (length == 3)
 {
  month = 0;
  while(month < 12)
  {
   month_name = F(GlobResources).getString(GlobResources,
					   XcR_dateMonthJanShort + month);
      
   i = -1;
   do
   {
    i++;
    c1 = Xc_TO_LOWER(buffer[i]);
    c2 = Xc_TO_LOWER(month_name[i]);
   }
   while( (i < 3) && (c1 == c2 || c1 == Xc_SUPPRESS_ACCENT(c2)));
      
   if (i == 3)
    break;
      
   month ++;
  }
  if (month < 12)
  {
   sprintf(strf+*indexf, "%d.0", month + 1);
   while (strf[*indexf])
    (*indexf)++;
   *type = TYPE_MONTH;
   return i + nbs;
  }
  return -1;
 }
  
 /* last look for month long */
 month = 0;
 while(month < 12)
 {
  month_name = F(GlobResources).getString(GlobResources,
					  XcR_dateMonthJan + month);
    
  i = -1;
  do
  {
   i++;
   c1 = Xc_TO_LOWER(buffer[i]);
   c2 = Xc_TO_LOWER(month_name[i]);
  }
  while( (i < length) && (c1 == c2 || c1 == Xc_SUPPRESS_ACCENT(c2)));
    
  if (i == length)
   break;
    
  month ++;
 }
 if (month < 12)
 {
  sprintf(strf+*indexf, "%d.0", month + 1);
  while (strf[*indexf])
   (*indexf)++;
  *type = TYPE_MONTH;
  return i + nbs;
 }
 return -1;
}


/*--- Test BOOLEAN ---*/
int get_token_boolean(This, string, type, strf, indexf)
c_Parser	*This;
char		*string;
int		*type;
char 		*strf;
int		*indexf;
{
 char	buffer[Xq_FORMULA_LENGTH];
 char	c1, c2;
 int	nbs, i, length;
 char	*name;
  
 nbs = length = 0;
 while(string[nbs] == ' ')
  nbs++;
  
 i = nbs;
 while(Xc_IS_LETTER(string[i]))
 {
  buffer[length] = string[i];
  length++; i++;
 }
 if (!length)
  return -1;
  
 buffer[length] = '\0';
  
 while (string[i] == ' ')
  i++;
 if (string[i] == '(')
  return -1;
  
 /* first test for true */
 name = F(GlobResources).getString(GlobResources, XcR_true);
 if (length == (int)strlen(name))
 {
  i = -1;
  do
  {
   i++;
   c1 = Xc_TO_LOWER(buffer[i]);
   c2 = Xc_TO_LOWER(name[i]);
  }
  while( (i < length) && (c1 == c2 || c1 == Xc_SUPPRESS_ACCENT(c2)));
    
  if (i == length)
  {
   *type = TYPE_BOOLEAN;
   strf[*indexf]='1';
   (*indexf)++;
   return length + nbs;
  }
 }
  
 /* last test for false */
 name = F(GlobResources).getString(GlobResources, XcR_false);
 if (length == (int)strlen(name))
 {
  i = -1;
  do
  {
   i++;
   c1 = Xc_TO_LOWER(buffer[i]);
   c2 = Xc_TO_LOWER(name[i]);
  }
  while( (i < length) && (c1 == c2 || c1 == Xc_SUPPRESS_ACCENT(c2)));
      
  if (i == length)
  {
   *type = TYPE_BOOLEAN;
   strf[*indexf]='0';
   (*indexf)++;
   return length + nbs;
  }
 }
 return -1;
}


/*--- Test CELL or CELL ZONE ---*/
static int valid_cell(cell_x, cell_y)
int	cell_x;
int	cell_y;
{
 if (cell_x < 0)
 {
  Xc_TRACE(("out of range %d", cell_x));
  return -1;
 }
 if (cell_x > COLUMN_MAX)
 {
  Xc_TRACE(("out of range %d", cell_x));
  return -1;
 }
 if (cell_y < 0)
 {
  Xc_TRACE(("out of range %d", cell_y));
  return -1;
 }
 if (cell_y > ROW_MAX)
 {
  Xc_TRACE(("out of range %d", cell_y));
  return -1;
 }
 return 1;
}

int get_token_zone(This, string, type, strf, indexf)
c_Parser	*This;
char		*string;
int		*type;
char 		*strf;
int		*indexf;
{
 int	e_index;
 int	size;
 char	*tok_str;
 int	cell_x;
 int	cell_y;
 int	cell_x_end;
 int	cell_y_end;
 int	cell_x_tmp;
 int	cell_y_tmp;
 int	other_i;
 int	init_o_i;
 int	var_test;		/* this test show monogamik */
 cell	*cell_s;
 char	*tmp_str;
 int	val;

 if (((string[0] >= '0') && (string[0] <= '9')) ||
     ((string[0] >= 'a') && (string[0] <= 'z')) ||
     ((string[0] >= 'A') && (string[0] <= 'Z')) ||
     (string[0] == '$'))
 {
  e_index = 0;
  while (((string[e_index] >= 'a') && (string[e_index] <= 'z'))
	 || ((string[e_index] >= 'A') && (string[e_index] <= 'Z'))
	 || (string[e_index] == '$')
	 || (string[e_index] == ':')
	 || ((string[e_index] >= '0' && string[e_index] <= '9')))
   e_index++;
  size = e_index;
  tok_str = (char *)Xc_malloc("tok_str1", size+1);
  strncpy(tok_str, string, size);
  tok_str[size] = 0;
  Xc_TRACE(("Token: %s", tok_str));
	  
  /* search for cell or cell group*/
  other_i = 0;
  cell_x = 0;
  /* ignore $ this act only with move */
  if (tok_str[other_i]=='$')
   other_i++;
  init_o_i = other_i;
  while ((((tok_str[other_i] >= 'A') && (tok_str[other_i] <= 'Z')) || 
	  ((tok_str[other_i] >= 'a') && (tok_str[other_i] <= 'z'))) &&
	 other_i<init_o_i+2)
  {
   if ((tok_str[other_i] >= 'a') && (tok_str[other_i] <= 'z'))
   {
    cell_x *= 26;
    cell_x += tok_str[other_i] - 'a' + 1;
   }
   else
   {
    cell_x *= 26;
    cell_x += tok_str[other_i] - 'A' + 1;
   }
   other_i++;
  }
	
  if (other_i == init_o_i)
  {
   Xc_free(tok_str);
   /* test for line:line */
   other_i = 0;
   if ((string[other_i] >= '0') &&
       (string[other_i] <= '9'))
   {
    cell_y = 0;
    while((string[other_i] >= '0') &&
	  (string[other_i] <= '9'))
    {
     cell_y *= 10;
     cell_y += string[other_i] - '0';
     other_i++;
    }
    cell_y--;
    if (string[other_i]==':')
    {
     other_i++;
     if ((string[other_i] >= '0') &&
	 (string[other_i] <= '9'))
     {
      cell_y_end = 0;
      while((string[other_i] >= '0') &&
	    (string[other_i] <= '9'))
      {
       cell_y_end *= 10;
       cell_y_end += string[other_i] - '0';
       other_i++;
      }
      cell_y_end--;
      cell_x = 0;
      cell_x_end = COLUMN_MAX;

      if (cell_y > cell_y_end)
       SWAP(int, cell_y, cell_y_end);
      *type = TYPE_CELL_ZONE;
      sprintf(strf+*indexf, "%d-%d-%d-%d-",
	      cell_x, cell_y,
	      cell_x_end, cell_y_end);

      switch (This->d_type)
      {
      case ADD_DEP:
       /*TODO  check circular dependancy */
       F(This->motor).add_depend_zone
	(This->motor,
	 This->cell_s,
	 This->y, This->x,
	 cell_x, cell_y,
	 cell_x_end, cell_y_end);
      case IGN_DEP:
       break;
      case SUB_DEP:
       F(This->motor).sub_depend_zone(This->motor,
				      This->cell_s);
       break;
      }
      return other_i;
     }
    }
   }
   return -1;
  }

  cell_x--;
  var_test = 0;
  cell_y = 0;
  if (tok_str[other_i]=='$')
   other_i++;
  while((tok_str[other_i] >= '0') && (tok_str[other_i] <= '9'))
  {
   var_test++;
   cell_y *= 10;
   cell_y += tok_str[other_i] - '0';
   other_i++;
  }
	  
  if (var_test)
  {
   cell_y--;
   if (valid_cell(cell_x, cell_y) == -1)
   {
    *type = TYPE_CELLRANGE_ERROR;
    Xc_free(tok_str);
    return -1;
   }
   cell_x_end = cell_x;
   cell_y_end = cell_y;
  }
  else
  {
   /* test for a monogamik zone colonne */
   cell_y = 0;
   cell_y_end = ROW_MAX;
   cell_x_tmp = 0;
   if (tok_str[other_i]==':')
   {
    other_i++;
    init_o_i = other_i;
		  
    while ((((tok_str[other_i] >= 'A') && 
	     (tok_str[other_i] <= 'Z')) || 
	    ((tok_str[other_i] >= 'a') && 
	     (tok_str[other_i] <= 'z'))) &&
	   other_i<init_o_i+2)
    {
     if ((tok_str[other_i] >= 'a') 
	 && (tok_str[other_i] <= 'z'))
     {
      cell_x_tmp *= 26;
      cell_x_tmp += tok_str[other_i] - 'a' + 1;
     }
     else
     {
      cell_x_tmp *= 26;
      cell_x_tmp += tok_str[other_i] - 'A' + 1;
     }
     other_i++;
    }
		
    /* any letters? */
    if (other_i == init_o_i)
    {
     *type = TYPE_ERROR;
     Xc_free(tok_str);
     return -1;
    }
    /* too much letter */
    if (tok_str[other_i] != 0)
    {
     *type = TYPE_ERROR;
     Xc_free(tok_str);
     return -1;
    }
    cell_x_end = cell_x_tmp-1;

    Xc_TRACE(("zone ok COL:COL"));
    if (cell_x > cell_x_end)
     SWAP(int, cell_x, cell_x_end);
    *type = TYPE_CELL_ZONE;
    sprintf(strf+*indexf, "%d-%d-%d-%d-",
	    cell_x, cell_y,
	    cell_x_end, cell_y_end);		  
    switch (This->d_type)
    {
    case ADD_DEP:
     /*TODO  check circular dependancy */
     F(This->motor).add_depend_zone
      (This->motor,
       This->cell_s,
       This->y, This->x,
       cell_x, cell_y,
       cell_x_end, cell_y_end);
    case IGN_DEP:
     break;
    case SUB_DEP:
     F(This->motor).sub_depend_zone(This->motor,
				    This->cell_s);
     break;
    }
    Xc_free(tok_str);
    return size;
   }
   else
   {
    *type = TYPE_ERROR;
    Xc_free(tok_str);
    return -1;
   }
   /* end of monogamik test zone colonne */
  }

  /* test if it is a zone */
  if ((tok_str[other_i]==':'))
  {
   other_i++;
   cell_x_tmp = 0;
   if (tok_str[other_i]=='$')
    other_i++;
   init_o_i = other_i;
   while ((((tok_str[other_i] >= 'A') && 
	    (tok_str[other_i] <= 'Z')) || 
	   ((tok_str[other_i] >= 'a') && 
	    (tok_str[other_i] <= 'z'))) &&
	  other_i<init_o_i+2)
   {	      
    if ((tok_str[other_i] >= 'a') 
	&& (tok_str[other_i] <= 'z'))
    {
     cell_x_tmp *= 26;
     cell_x_tmp += tok_str[other_i] - 'a' + 1;
    }
    else
    {
     cell_x_tmp *= 26;
     cell_x_tmp += tok_str[other_i] - 'A' + 1;
    }
    other_i++;
   }
	      
   if (other_i == init_o_i)
   {
    *type = TYPE_ERROR;
    Xc_free(tok_str);
    return -1;
   }
   var_test = 0;
   cell_y_tmp = 0;
   if (tok_str[other_i]=='$')
    other_i++;
   while((tok_str[other_i] >= '0') && (tok_str[other_i] <= '9'))
   {
    var_test++;
    cell_y_tmp *= 10;
    cell_y_tmp += tok_str[other_i] - '0';
    other_i++;
   }
   if (!var_test)
   {
    *type = TYPE_ERROR;
    /* bad zone */
    Xc_free(tok_str);
    return -1;
   }
   /* too much letter */
   if (tok_str[other_i] != 0)
   {
    *type = TYPE_ERROR;
    /* bad zone */
    Xc_free(tok_str);
    return -1;
   }
   cell_x_end = cell_x_tmp-1;
   cell_y_end = cell_y_tmp-1;

   if (cell_x > cell_x_end)
    SWAP(int, cell_x, cell_x_end);
   if (cell_y > cell_y_end)
    SWAP(int, cell_y, cell_y_end);

   Xc_TRACE(("zone ok"));
   *type = TYPE_CELL_ZONE;
   sprintf(strf+*indexf, "%d-%d-%d-%d-",
	   cell_x, cell_y,
	   cell_x_end, cell_y_end);
   switch (This->d_type)
   {
   case ADD_DEP:
    /*TODO  check circular dependancy */
    F(This->motor).add_depend_zone(This->motor,
				   This->cell_s,
				   This->y, This->x,
				   cell_x, cell_y,
				   cell_x_end, cell_y_end);
   case IGN_DEP:
    break;
   case SUB_DEP:
    F(This->motor).sub_depend_zone(This->motor,
				   This->cell_s);
    break;
   }
   Xc_free(tok_str);
   return size;
  }

  /* cell is not a zone */
  if (!tok_str[other_i])
  {
   Xc_free(tok_str);
   cell_s = F(This->motor).get_cell(This->motor,
				    cell_x,
				    cell_y);

   /* NEW_DEBUG 23_05_96
      if ((cell_s->type != Xq_VALUE) &&
      (cell_s->type != Xq_FORMULA) &&
      (cell_s->type != Xq_TEXT) &&
      (cell_s->type != Xq_VOID))
      {
      *type = TYPE_VAL_ERROR;
      return -1;
      }
      */
		  
   switch (This->d_type)
   {
   case ADD_DEP:
    /* check circular dependancy */
    if (F(This->motor).check_c_depend(This->motor, 
				      This->cell_s,
				      cell_x,
				      cell_y))
    {
     Xc_TRACE(("Error circular dep"));
     *type = TYPE_CIRC_ERROR;
     return -1;
    }
    F(This->motor).add_depend(This->motor,
			      This->cell_s,
			      This->y, This->x,
			      cell_y, cell_x);
   case IGN_DEP:
    if ((cell_s->type == Xq_TEXT) || 
	((cell_s->type == Xq_FORMULA) 
	 && cell_s->text 
	 && !(cell_s->value2)))
    {
     int	i;
     i=0;
     if (cell_s->text[i]=='\'')
      i++;
     while (cell_s->text[i])
      strf[(*indexf)++] = cell_s->text[i++];
     strf[(*indexf)] = 0;
     *type = TYPE_STRING;
     return size;
    }

    if (cell_s->value2)
    {
     int	coma;
     int	i;
		  
     if (cell_s->ret_type == TYPE_EXPONANTIAL)
      tmp_str = g2estr(cell_s->value2, BIG_PREC);
     else
      tmp_str = g2str(cell_s->value2, BIG_PREC);
     /* try to cut last space */

     strcpy(strf+*indexf, tmp_str);
     val = strlen(strf+*indexf);
     i = 0;
     coma = 0;
     while (i < val)
     {
      if ((strf[*indexf+i]=='.') || 
	  (strf[*indexf+i]=='E') || (strf[*indexf+i]=='e'))
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
     *type = cell_s->ret_type;
    }
    else 
    {				/* Xq_VOID case -> set zero value */
     strf[*indexf] = '0';
     strf[*indexf+1] = 0;
     val = 1;
     *type = TYPE_NUM;
    }
    *indexf += val;
    /*	      *type = TYPE_CELL; */
    break;
   case SUB_DEP:
    F(This->motor).sub_depend(This->motor,
			      This->cell_s,
			      cell_y, cell_x);
    if ((cell_s->type == Xq_TEXT) || 
	((cell_s->type == Xq_FORMULA) 
	 && cell_s->text 
	 && !(cell_s->value2)))
    {
     *type = TYPE_STRING;
    }
    else
     *type = TYPE_NUM;
    break;
   }
   return size;
  }
  Xc_free(tok_str);
 }
 *type = TYPE_ERROR;
 return -1;
}


