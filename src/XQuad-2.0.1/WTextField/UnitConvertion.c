/*
** UnitConvertion.c for Xclamation, XQuad, XAllWrite and XMayday in WTextField/
** Object and method for Unit Convertion in WTextField
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
** Started on  Sun Aug 18 15:08:04 1996 Emmanuel Paris
** Last update Wed Nov  4 15:15:27 1998 Emmanuel Paris
*/

#include "UnitConvertionP.h"

static parse_unit_t *parse_str_to_unit ___PROTO((char *str));
static void free_parse_unit_list ___PROTO((parse_unit_t *pulist));
static int determine_unit ___PROTO((unit_conv_t *unit_base,
				    parse_unit_t *pulist));

/* ------ parse a string and create a parse_unit_t linked list -------- */
static parse_unit_t *parse_str_to_unit(str)
char *str;
{
 int	i, j, k;
 int	old_type, type;
 char	*token;
 parse_unit_t	putop, *pulist;
 boolean	loop = TRUE;
  
 i = 0;
 while(str[i] == ' ')
  i++;
  
 if (i == (int)strlen(str))
  return NULL;
  
 if (i)
  memmove(str, str + i, strlen(str) - i + 1);
 i = strlen(str) - 1;
 while(str[i] == ' ')
  i--;
 str[i+1] = '\0';
  
 token = Xc_malloc("pu token", strlen(str) + 2);
 pulist = &putop;
  
 i = j = 0; old_type = XcPU_STRING; type = 0;
 while(loop)
 {
  switch(str[i])
  {
  case '\0':
   loop = FALSE;
   type = old_type;
   break;
  case '+':
  case '-':
   if ((str[i+1] >= '0' && str[i+1] <= '9') || 
       ((str[i+1] == '.' || str[i + 1] == ',') &&
	(str[i+2] >= '0' && str[i+2] <= '9')))
    type = XcPU_INTEGER;
   else
    if (old_type != XcPU_STRING)
     type = XcPU_STRING;
   break;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
   if (old_type == XcPU_STRING)
    type = XcPU_INTEGER;
   break;
  case '.':
  case ',':
   if (old_type != XcPU_REAL && str[i+1] >= '0' && str[i+1] <= '9')
    type = XcPU_REAL;
   else
    if (old_type != XcPU_STRING)
     type = XcPU_STRING;
   break;
  default:
   if (old_type != XcPU_STRING)
    type = XcPU_STRING;
   break;
  }
    
  if (type)
  {
   switch(old_type)
   {
   case XcPU_INTEGER:
    if (type == XcPU_REAL)
     break;

    token[j] = '\0';
	
    pulist = pulist->pu_any.next = Xc_malloc("pu*", sizeof(parse_unit_t));
    pulist->type = XcPU_INTEGER;
    pulist->pu_integer.next = NULL;
    pulist->pu_integer.value = atoi(token);
    j = 0;
    break;
   case XcPU_REAL:
    token[j] = '\0';
    for(k = 0; k < j; k++)
     if (token[k] == ',') token[k] = '.';

    if (token[0] == '.')
    {
     memmove(token + 1, token, j+1);
     token[0] = '0';
    }
    if ((token[0] == '+' || token[0] == '-') && (token[1] == '.'))
    {
     memmove(token + 2, token + 1, j - 1);
     token[1] = '0';	  
    }
	
    pulist = pulist->pu_any.next = Xc_malloc("pu*", sizeof(parse_unit_t));
    pulist->type = XcPU_REAL;
    pulist->pu_real.next = NULL;
    pulist->pu_real.value = (real)atof(token);
    j = 0;
    break;
   case XcPU_STRING:
    if (!j) break;
	
    k = 0;
    while(token[k] == ' ')
     k++;
	
    if (k == j)
    {
     j = 0;
     break;
    }
    if (k)
     memmove(token, token + k, j - k + 1);
	
    k = j - k - 1;
    while(token[k] == ' ')
     k--;
    token[k + 1] = '\0';
	
    pulist = pulist->pu_any.next = Xc_malloc("pu*", sizeof(parse_unit_t));
    pulist->type = XcPU_STRING;
    pulist->pu_string.next = NULL;
    pulist->pu_string.string = Xc_strdup("pus*", token);
	
    j = 0;
    break;
   }
   old_type = type;
   type = 0;
  }
    
  token[j++] = str[i++];
 }
 Xc_free(token);
  
 return putop.pu_any.next;
}

/* ------ free a parse_unit_t linked list -------- */
static void free_parse_unit_list(pulist)
parse_unit_t	*pulist;
{
 parse_unit_t	*pulist2;
  
 while(pulist)
 {
  pulist2 = pulist->pu_any.next;
  if (pulist->type == XcPU_STRING)
   Xc_free(pulist->pu_string.string);
  Xc_free(pulist);
  pulist = pulist2;
 }
}

/* -- determine the corresponding unit from a parse_unit_t linked list -- */
static int determine_unit(unit_base, pulist)
unit_conv_t *unit_base;
parse_unit_t *pulist;
{
 int	i, j, k, l;
 int	ret_uc;
 parse_unit_t *pul;
 boolean suit = FALSE;
 char	*str, *unit;
  
  
 if (!pulist)
  return XcUC_ERROR;
  
 if (!pulist->pu_any.next && pulist->type != XcPU_STRING)
  return XcUC_DEFAULT;
  
 i = 0; ret_uc = XcUC_ERROR;
 while(unit_base[i].unit_def)
 {
  j = 0; 
  str = unit_base[i].unit_def;
  unit = Xc_malloc("unit", strlen(str) + 1);
  pul = pulist;
  while(pul)
  {
   suit = FALSE;
   switch(pul->type)
   {
   case XcPU_INTEGER:
    if (str[j] == '%' &&  
	(str[j+1] == XcUC_INTEGER || str[j+1] == XcUC_REAL))
     suit = TRUE;
    j += 2;
    break;
   case XcPU_REAL:
    if (str[j] == '%' && str[j+1] == XcUC_REAL)
     suit = TRUE;
    j += 2;
    break;
   case XcPU_STRING:
    k = 0;
    while(str[j])
    {
     if (str[j] == '%')
     {
      if (str[j+1] == '%')
      {
       unit[k++] = '%';
       j += 2;
      }
      else
       break;
     }
     else
      unit[k++] = str[j++];
    }
    unit[k] = '\0';
	
    l = 0;
    while(unit[l] == ' ')
     l++;
	
    if (k == l)
    {
     strcpy(unit, " ");
    }
    else
    {
     if (l)
      memmove(unit, unit + l, k - l + 1);
     l = k - l - 1;
     while(unit[l] == ' ')
      l--;
     unit[l + 1] = '\0';
    }
	
    if (Xstrcmp(unit, pul->pu_string.string))
     suit = TRUE;
    break;
   }
   if (!suit)
    break;
   pul = pul->pu_any.next;
  }
  Xc_free(unit);
    
  if (suit && str[j] == '\0')
  {
   ret_uc = i;
   break;
  }
  i++;
 }
 return ret_uc;
}

char	*tf_convert_string_to_string(unitb, str, defval, limitInf, limitSup)
int	unitb;
char	*str;
real	*defval;
real	limitInf;
real	limitSup;
{
 real	val = 0.0;
 int	unit;
 char	*ret_str;
 parse_unit_t	*pulist;
  
 pulist = parse_str_to_unit(str);
  
 unit = determine_unit(unit_base[unitb].unit_conv, pulist);
  
 if (unit == XcUC_DEFAULT)
  unit = unit_base[unitb].default_unit;
  
 if (unit == XcUC_ERROR)
 {
  if (defval)
   val = *defval;
  unit = unit_base[unitb].default_unit;
 }
 else
 {
  val = unit_base[unitb].unit_conv[unit].str_to_val(pulist);
 }
 if (limitInf < limitSup)
 {
  if (val < limitInf)
   val = limitInf;
    
  if (val > limitSup)
   val = limitSup;
 }
  
 if (defval)
  *defval = val;
  
 ret_str = unit_base[unitb].unit_conv[unit].val_to_str
  (unit_base[unitb].unit_conv[unit].unit_def, val);
  
 free_parse_unit_list(pulist);
  
 return ret_str;
}

real tf_convert_string_to_real(unitb, str, defval, limitInf, limitSup)
int	unitb;
char	*str;
real	defval;
real	limitInf;
real	limitSup;
{
 real	val;
 int	unit;
 parse_unit_t	*pulist;
  
 pulist = parse_str_to_unit(str);
  
 unit = determine_unit(unit_base[unitb].unit_conv, pulist);
  
 if (unit == XcUC_DEFAULT)
  unit = unit_base[unitb].default_unit;
  
 if (unit == XcUC_ERROR)
 {
  val = defval;
  unit = unit_base[unitb].default_unit;
 }
 else
 {
  val = unit_base[unitb].unit_conv[unit].str_to_val(pulist);
 }
    
 if (limitInf < limitSup)
 {
  if (val < limitInf)
   val = limitInf;
    
  if (val > limitSup)
   val = limitSup;
 }
  
 free_parse_unit_list(pulist);
 return val;
}

char	*tf_convert_real_to_string(unitb, val, defval, limitInf, limitSup)
int	unitb;
real	val;
real	*defval;
real	limitInf;
real	limitSup;
{
 int	unit;
 char	*ret_str;
    
 if (limitInf < limitSup)
 {
  if (val < limitInf)
   val = limitInf;
   
  if (val > limitSup)
   val = limitSup;
 }
  
 if (defval)
  *defval = val;
  
 unit = unit_base[unitb].default_unit;
 ret_str = unit_base[unitb].unit_conv[unit].val_to_str
  (unit_base[unitb].unit_conv[unit].unit_def, val);
  
 return ret_str;
}

#ifdef _UNIT_CONVERTION_MAIN_
/* main function to test the unit's parser */

main()
{
 char		input[200];
 parse_unit_t	*pu, *pulist;
 int		unit, type;
 real		val;
  
 printf("XcUB_LENGTH_POINT        0\n");
 printf("XcUB_LENGTH_MILLIMETER   1\n");
 printf("XcUB_LENGTH_CENTIMETER   2\n");
 printf("XcUB_RATIO_PERCENTAGE    3\n");
 printf("Enter unit type: ");
  
 gets(input);
 type = atoi(input);
  
 do
 {
  printf("enter a string: ");
  gets(input);
    
  pulist = pu = parse_str_to_unit(input);
    
  while(pu)
  {
   switch(pu->type)
   {
   case XcPU_INTEGER:
    printf("INTEGER: %d\n", pu->pu_integer.value);
    break;
   case XcPU_REAL:
    printf("REAL   : %g\n", pu->pu_real.value);
    break;
   case XcPU_STRING:
    printf("STRING : %s\n", pu->pu_string.string);
    break;
   }
   pu = pu->pu_any.next;
  }
    
  unit = determine_unit(unit_base[type].unit_conv, pulist);
    
  switch(unit)
  {
  case XcUC_ERROR:
   printf("Error, unit not reconize\n");
   break;
  case XcUC_DEFAULT: 
   printf("Taking default unit\n");
   break;
  default:
   switch(type)
   {
   case XcUB_LENGTH_POINT:
   case XcUB_LENGTH_MILLIMETER:
   case XcUB_LENGTH_CENTIMETER:
    switch(unit)
    {
    case XcUC_LENGTH_POINT:
     printf("Taking unit POINT: %s\n", unit_length[unit].unit_def);
     break;
    case XcUC_LENGTH_MILLIMETER:
     printf("Taking unit MILIMETER: %s\n", unit_length[unit].unit_def);
     break;
    case XcUC_LENGTH_CENTIMETER:
     printf("Taking unit CENTIMETER: %s\n", unit_length[unit].unit_def);
     break;
    case XcUC_LENGTH_DECIMETER:
     printf("Taking unit DECIMETER: %s\n", unit_length[unit].unit_def);
     break;
    case XcUC_LENGTH_METER:
     printf("Taking unit METER: %s\n", unit_length[unit].unit_def);
     break;
    case XcUC_LENGTH_INCH:
    case XcUC_LENGTH_INCH2:
     printf("Taking unit INCH: %s\n", unit_length[unit].unit_def);
     break;
    case XcUC_LENGTH_PICA:
    case XcUC_LENGTH_PICA2:
     printf("Taking unit PICA: %s\n", unit_length[unit].unit_def);
     break;
    case XcUC_LENGTH_CICERO:
    case XcUC_LENGTH_CICERO2:
     printf("Taking unit CICERO: %s\n", unit_length[unit].unit_def);
     break;
    }
    break;
   case XcUB_RATIO_PERCENTAGE:
    switch(unit)
    {
    case XcUC_RATIO_PERCENTAGE:
     printf("Taking unit PERCENTAGE: %s\n", unit_ratio[unit].unit_def);
     break;
    case XcUC_RATIO_FRACTION:
     printf("Taking unit FRACTION: %s\n", unit_ratio[unit].unit_def);
     break;
    }
    break;
   }
  }
    
  if (unit == XcUC_ERROR)
  {
   printf("No Convertion needed, return 0\n\n");
   free_parse_unit_list(pu);
   continue;
  }
    
  if (unit == XcUC_DEFAULT)
  {
   printf("Default unit: %d\n", unit_base[type].default_unit);
   unit = unit_base[type].default_unit;
  }
  printf("Convertion input->real: %g\n", 
	 val = unit_base[type].unit_conv[unit].str_to_val(pulist));
  printf("Convertion real->output: `%s'\n", 
	 unit_base[type].unit_conv[unit].val_to_str
	 (unit_base[type].unit_conv[unit].unit_def, val));
    
  printf("\n");
    
  free_parse_unit_list(pu);
 } while(1);
}

#endif

















