/*
** FormatNbr.c for XQuad in FormatNbr/
** Object and methods for FormatNbr
**
** Copyright (C) 1994-2000 Axene.
** Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
** Started on  Fri Dec 16 15:12:00 1994 Antoine Buat
** Last update Fri Oct  2 17:03:50 1998 Robin Castanier
*/

#define NHISTORY
#define NTRACE

#include "FormatNbr.h"
#include "FormatNbr_Excel.h"
#include "Moteur.h"
#include "Resource.h"
#include "TimeFuncs.h"
#include <stdarg.h>

extern c_Resource *GlobResources;
extern c_Color	*GlobColorAuto;

static void *cons_FormatNbr ___PROTO((c_FormatNbr **format_base, char *name));
static void dest_FormatNbr();
static void *copy_FormatNbr();
static void set_format ___PROTO((c_FormatNbr *f, ...));
static boolean compare ___PROTO((c_FormatNbr *a, c_FormatNbr *b,
				 boolean with_name));
static void merge ___PROTO((c_FormatNbr *source, c_FormatNbr **target,
			    boolean fusion));
static c_FormatNbr *virtualCopy();
static c_FormatNbr *get_format(c_FormatNbr *, format_type_t, BaseStd_t *, ...);
static void FormatNbr_convert_string ___PROTO((c_FormatNbr *this, char *str,
					       int type, conv_info_t *info));

static void hook_color ___PROTO((c_FormatNbr *this, int reason,
                                 c_Color *old, c_Color *new));

/*--- Loading callbacks ---*/
static void vset ___PROTO((c_FormatNbr *f, va_list ap));
static void parseFormatString ___PROTO((c_FormatNbr *This, char *ptr));

sf_FormatNbr fc_FormatNbr =
{
 cons_FormatNbr,
 dest_FormatNbr,
 copy_FormatNbr,
 FormatNbr_convert_string,
 set_format,
 compare,
 merge,
 virtualCopy,
 get_format,
 readFormat, writeFormat,
 readFormatList, writeFormatList
};

static char	_Form_str[300];

/* ----------------------------------------------------------------- ** 
** constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *cons_FormatNbr(format_base, name)
c_FormatNbr	**format_base;
char		*name;
{
 int	i;
 c_FormatNbr	*ObjTmp;

 if((ObjTmp = Xc_malloc("FormatNbr", sizeof(c_FormatNbr))) == NULL)
  return NULL;

 memset(ObjTmp, 0, sizeof(c_FormatNbr));
 ObjTmp->f = &fc_FormatNbr;
  
 if((ObjTmp->hook = NEW(c_Hook)(ObjTmp, 16)) == NULL)
  return NULL;

 if(*format_base == NULL)
 {
  ObjTmp->next = ObjTmp->previous = NULL;
 }
 else
 {
  ObjTmp->next = *format_base;
  ObjTmp->previous = NULL;
  (*format_base)->previous = ObjTmp;
 }
 *format_base = ObjTmp;
 ObjTmp->base = format_base;
  
 ObjTmp->type = Xq_VALUE;
  
 ObjTmp->max_prec = MAX_MAX_PREC;
 ObjTmp->min_prec = 0;
 ObjTmp->space_each = 0;

 ObjTmp->unitafter = FALSE;
 strcpy(ObjTmp->unit, "");
  
 ObjTmp->comma_sep = (int)',';
 ObjTmp->color1 = GlobColorAuto;
 ObjTmp->color2 = GlobColorAuto;
 ObjTmp->char_sep = (int)' ';
 if(name)
  strncpy(ObjTmp->name, name, XcMAX_NAME_LENGTH - 1);
 else
  strcpy(ObjTmp->name, "");
    
 ObjTmp->truncate = FALSE;
  
 ObjTmp->exp_digit = 2;
 ObjTmp->exp_multiple = 1;
 ObjTmp->exp_sign = FALSE;
 strcpy(ObjTmp->exp_string, "E");
  
 ObjTmp->frac_prec = 3;
 ObjTmp->frac_dec = FALSE;
  
 strncpy(ObjTmp->true_name,
	 F(GlobResources).getString(GlobResources, XcR_true),
	 XcMAX_NAME_LENGTH - 1);
 strncpy(ObjTmp->false_name,
	 F(GlobResources).getString(GlobResources, XcR_false),
	 XcMAX_NAME_LENGTH - 1);
  
 ObjTmp->day_name_size = Xq_NAME_LONG;
 ObjTmp->month_name_size = Xq_NAME_LONG;
  
 ObjTmp->date_seq[0] = ObjTmp->time_seq[0] = ObjTmp->date_time_seq[0] = NULL;
 ObjTmp->padding = Xq_PADDING_NONE;
  
 for(i = 0; i < Xq_NB_TYPE; i++)
 {
  ObjTmp->other_format[i] = NULL;
 }
  
 ObjTmp->special = XcFt_NORMAL;
 ObjTmp->lock_flag = FALSE;
 ObjTmp->hide_flag = FALSE;

 ObjTmp->BaseStd = NULL;
  
 Xc_HISTORY(("Object FormatNbr build"));
 return ObjTmp;
}


/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
static void dest_FormatNbr(this)
c_FormatNbr	*this;
{
 int	i;
 c_FormatNbr	*format;
 Xc_HISTORY(("destructor(`%s')", this->name));
  
 DELETE(c_Hook)(this->hook);

 /*--- Remove from the linked list ---*/
 if(this->next != NULL)
  this->next->previous = this->previous;
 if(this->previous != NULL)
  this->previous->next = this->next;
 if(*(this->base) == this)
 {
  Xc_ASSERT(this->previous == NULL);
  *(this->base) = this->next;
 }
  
 format = *this->base;
 while(format)
 {
  for(i = 0; i < Xq_NB_TYPE; i++)
  {
   if (format->other_format[i] == this)
    format->other_format[i] = NULL;
  }
  format = format->next;
 }
 if(this->color2 != GlobColorAuto)
  XcH_UNREGISTER(this->color2, hook_color, this);
 if(this->color1 != GlobColorAuto)
  XcH_UNREGISTER(this->color1, hook_color, this);
  
 destroy_all_dt_seq(this->date_seq);
 destroy_all_dt_seq(this->time_seq);
 destroy_all_dt_seq(this->date_time_seq);
  
 Xc_free(this);

 Xc_HISTORY(("destroyed"));
}


/* ----------------------------------------------------------------- ** 
** copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy_FormatNbr(this, base)
c_FormatNbr *this;
BaseStd_t *base;
{
 c_Color *color;
 c_FormatNbr *p;
  
 Xc_HISTORY(("copy(`%s')", this->name));
 Xc_ASSERT(base != NULL);

 if((p = Xc_malloc("FormatNbr", sizeof(c_FormatNbr))) == NULL)
  return NULL;
 memcpy(p, this, sizeof(c_FormatNbr));
 if((p->hook = NEW(c_Hook)(p, 16)) == NULL)
 {
  Xc_free(p);
  return NULL;
 }

 /*--- Insert copy in linked list ---*/
 if(base->format_base != NULL)
  base->format_base->previous = p;
 p->next = base->format_base;
 p->previous = NULL;
 p->base = &base->format_base;
 base->format_base = p;
 p->BaseStd = base;
  
 /*--- Copy colors ---*/
 p->color2 = p->color1 = GlobColorAuto;
 if(this->color2 != GlobColorAuto)
 {
  if(((color = F(this->color2).virtualCopy(this->color2, base)) == NULL) ||
     !XcH_REGISTER(color, hook_color, p))
  {
   DELETE(c_FormatNbr)(p);
   return NULL;
  }
  else
   p->color2 = color;
 }

 if(this->color1 != GlobColorAuto)
 {
  if(((color = F(this->color1).virtualCopy(this->color1, base)) == NULL) ||
     !XcH_REGISTER(color, hook_color, p))
  {
   DELETE(c_FormatNbr)(p);
   return NULL;
  }
  else
   p->color1 = color;
 }
  
 copy_all_dt_seq(p->date_seq, this->date_seq);
 copy_all_dt_seq(p->time_seq, this->time_seq);
 copy_all_dt_seq(p->date_time_seq, this->date_time_seq);
  
 Xc_TRACE(("end copy"));
 return p;
}

/* ----------------------------------------------------------------- ** 
** virtualCopy - Copy if object doesn't exist already in base        ** 
** ----------------------------------------------------------------- */
static c_FormatNbr *virtualCopy(this, base)
c_FormatNbr *this;
BaseStd_t *base;
{
  int	i;
  c_FormatNbr *format, *ptr;

  Xc_HISTORY(("virtualCopy(`%s')", format->name));
  
  if(this->base == &(base->format_base))
   return this;
  for(format = base->format_base; format != NULL; format = format->next)
   if(F(this).compare(format, this, CMP_WITH_NAME))
   {
    Xc_TRACE(("found in new base"));
    break;
   }
  
  for(i = 0; i < Xq_NB_TYPE; i++)
  {
   if (format->other_format[i] != NULL)
   {
    ptr = base->format_base;
    while(ptr && 
	  !F(ptr).compare(ptr, format->other_format[i], CMP_WITHOUT_NAME))
     ptr = ptr->next;
    format->other_format[i] = ptr;
   }
  }
  
  return((format == NULL)? COPY(c_FormatNbr)(this, base) : format);
 }


/* ----------------------------------------------------------------- ** 
** set_format - Set format parameters                                ** 
** ----------------------------------------------------------------- */
static void set_format(c_FormatNbr *f, ...)
{
 va_list ap;
  
 Xc_HISTORY(("set(`%s')", f->name));

 va_start(ap, f);
 vset(f, ap);
 va_end(ap);
 F(f->hook).callback(f->hook, XcH_CONTENT_CHANGED, f);
}


/* ----------------------------------------------------------------- ** 
** compare - Compare two FormatNbr objects                           ** 
** ----------------------------------------------------------------- */
static boolean compare(a, b, with_name)
c_FormatNbr *a;
c_FormatNbr *b;
boolean	with_name;
{
 int	i;
 Xc_TRACE(("compare(`%s', `%s')", a->name, b->name));

 if(a == b) return TRUE;
 if (with_name && 
     !(a->hide_flag || b->hide_flag || Xstrcmp(a->name, b->name)))
  return FALSE;
  
 if (a->type != b->type)
  return FALSE;
  
 switch(a->type)
 {
 case Xq_VALUE:
 case Xq_PERCENT:
  if ((a->max_prec != b->max_prec) ||
      (a->min_prec != b->min_prec) ||
      (a->space_each != b->space_each) ||
      (a->comma_sep != b->comma_sep) ||
      !F(a->color1).compare(a->color1, b->color1, CMP_WITH_NAME) ||
      !F(a->color2).compare(a->color2, b->color2, CMP_WITH_NAME) ||
      (a->char_sep != b->char_sep) ||
      (a->truncate != b->truncate))
   return FALSE;
 case Xq_UNIT:
  if ((a->max_prec != b->max_prec) ||
      (a->min_prec != b->min_prec) ||
      (a->space_each != b->space_each) ||
      (a->comma_sep != b->comma_sep) ||
      !F(a->color1).compare(a->color1, b->color1, CMP_WITH_NAME) ||
      !F(a->color2).compare(a->color2, b->color2, CMP_WITH_NAME) ||
      (a->char_sep != b->char_sep) ||
      (a->truncate != b->truncate) ||
      !Xstrcmp(a->unit, b->unit) ||
      (a->unitafter != b->unitafter))
   return FALSE;
  break;
 case Xq_EXPONANTIAL:
  if ((a->max_prec != b->max_prec) ||
      (a->min_prec != b->min_prec) ||
      (a->comma_sep != b->comma_sep) ||
      !F(a->color1).compare(a->color1, b->color1, CMP_WITH_NAME) ||
      !F(a->color2).compare(a->color2, b->color2, CMP_WITH_NAME) ||
      !Xstrcmp(a->unit, b->unit) ||
      (a->unitafter != b->unitafter) ||
      (a->exp_digit != b->exp_digit) ||
      (a->exp_multiple != b->exp_multiple) ||
      (a->exp_sign != b->exp_sign) || 
      !Xstrcmp(a->exp_string, b->exp_string))
   return FALSE;
  break;
 case Xq_FRACTION:
  if (!F(a->color1).compare(a->color1, b->color1, CMP_WITH_NAME) ||
      !F(a->color2).compare(a->color2, b->color2, CMP_WITH_NAME) ||
      !Xstrcmp(a->unit, b->unit) ||
      (a->unitafter != b->unitafter) ||
      (a->frac_prec != b->frac_prec) ||
      (a->frac_dec != b->frac_dec))
   return FALSE;
  break;
 case Xq_BOOLEAN:
  if (!F(a->color1).compare(a->color1, b->color1, CMP_WITH_NAME) ||
      !F(a->color2).compare(a->color2, b->color2, CMP_WITH_NAME) ||
      !Xstrcmp(a->true_name, b->true_name) ||
      !Xstrcmp(a->false_name, b->false_name))
   return FALSE;
  break;
 case Xq_DAY_IN_WEEK:
  if (a->day_name_size != b->day_name_size)
   return FALSE;
  break;
 case Xq_MONTH:
  if (a->month_name_size != b->month_name_size)
   return FALSE;
  break;
 case Xq_DATE:
  if (!compare_all_dt_seq(a->date_seq, b->date_seq))
   return FALSE;
  break;
 case Xq_TIME:
  if (!compare_all_dt_seq(a->time_seq, b->time_seq))
   return FALSE;
  break;
 case Xq_DATE_TIME:
  if (!compare_all_dt_seq(a->date_time_seq, b->date_time_seq))
   return FALSE;
  break;
 }
  
 if (a->special != b->special)
  return FALSE;
  
 for(i = 0; i < Xq_NB_TYPE; i++)
 {
  if (a->other_format[i] == NULL ||
      a->other_format[i]->special == XcFt_GENERIC)
  {
   if (b->other_format[i] != NULL && 
       b->other_format[i]->special != XcFt_GENERIC)
    return FALSE;
  }
  else
  {
   if (a->other_format[i] != b->other_format[i])
    return FALSE;
  }
 }
  
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** merge - Merge two color list                                      ** 
** ----------------------------------------------------------------- */
static void merge(source, target, fusion_flag)
c_FormatNbr *source;
c_FormatNbr **target;
boolean fusion_flag;
{
 c_FormatNbr *ptr, *base;

 Xc_HISTORY(("merge"));

 source = *source->base;
 base = *target;
 Xc_ASSERT(target != source->base);

 for(ptr = base; ptr != NULL; ptr = ptr->next) 
  ptr->id = -1;
 while(source != NULL)
 {
  if(fusion_flag != FALSE)
  {
   for(ptr = base; ptr != NULL; ptr = ptr->next)
    if(F(source).compare(source, ptr, CMP_WITH_NAME)) break;
   if(ptr != NULL)
   {
    ptr->id = source->id;
    ptr = source;
    source = source->next;
    DELETE(c_FormatNbr)(ptr);
    continue;
   }
  }

  ptr = source;
  source = source->next;

  /*--- Remove from source base ---*/
  Xc_ASSERT(ptr->previous == NULL);
  if(ptr->next != NULL) ptr->next->previous = NULL;
  *ptr->base = ptr->next;

  /*--- Add to target base ---*/
  ptr->next = base;
  ptr->base = target;
  if(base != NULL) base->previous = ptr;
  *target = base = ptr;
 }
}

/* ----------------------------------------------------------------- ** 
** get_format - get special format, or creat new one                 ** 
** ----------------------------------------------------------------- */
static c_FormatNbr *get_format(c_FormatNbr *format, format_type_t type, BaseStd_t *base, ...)
{
 c_FormatNbr *ptr, *format2;
 va_list ap;
  
 Xc_HISTORY(("getFormat"));
  
 if (type == XcFt_NORMAL)
 {
  if (format)
  {
   format2 = (c_FormatNbr *)COPY(c_FormatNbr)(format, base);
   format2->special = XcFt_NORMAL;
  }
  else
  {
   format2 = (c_FormatNbr *)NEW(c_FormatNbr)(&(base->format_base), "");
   format2->BaseStd = base;
  }
      
  va_start(ap, base);
  vset(format2, ap);
  va_end(ap);
      
  for(ptr = base->format_base; ptr != NULL; ptr = ptr->next)
  {
   if((ptr != format2) && F(ptr).compare(ptr, format2, CMP_WITHOUT_NAME)) 
   {
    DELETE(c_FormatNbr)(format2);
    return ptr; 
   }
  }
  return format2;
 }
 else
 {
  ptr = base->format_base;
  while(ptr && ptr->special != type)
   ptr = ptr->next;
  return ptr;
 }
}

/* ----------------------------------------------------------------- ** 
** convert string                                                    ** 
** ----------------------------------------------------------------- */
static void FormatNbr_convert_string(This, str, type, info_ret)
c_FormatNbr	*This;
char 		*str;
int		type;
conv_info_t	*info_ret;
{
 conv_info_t	info;
 int		expp = 0;	/* exponantial part */
  
 memset(&info, 0, sizeof(conv_info_t));
  
 Xc_TRACE(("convert string: %s to format type: %d", str, type));
 if (type != This->type)
 {
  if (type == Xq_VALUE)
   type = This->type;
  else
  {
   if (This->other_format[type] != NULL)
    This  = This->other_format[type];
   else
   {
    c_FormatNbr	*format;
	      
    format = *This->base;
    while(format && format->special != XcFt_GENERIC)
     format = format->next;
    if (!format)
    {
     format = This;
     Xc_WARNING(("Doesn't found GENERIC format"));
    }
    else
    {
     This->other_format[type] = format;
     This = format;
    }
   }
  }
 }

 info.color = NULL;
 switch(type)
 {
 case Xq_PERCENT:
  {
   int	indexs;
      
   indexs = 0;
   while(str[indexs] != '.' && str[indexs] != '\0')
    indexs++;
   if (str[indexs] == '\0' || str[indexs + 1] == '\0')
   {
    str[indexs] = '0';
    str[indexs+1] = '0';
    str[indexs+2] = '\0';
   }
   else
   {
    if (str[indexs + 2] == '\0')
    {
     str[indexs+2] = '0';
     str[indexs+3] = '\0';	  
    }
    str[indexs] = str[indexs + 1];
    str[indexs + 1] = str[indexs + 2];
    if (str[indexs + 3] != '\0')
     str[indexs + 2] = '.';
    else
     str[indexs + 2] = '\0';
   }
   if (str[0] == '0' && indexs == 1)
   {
    if (str[1] == '0')
     memcpy(str, str + 2, Xq_FORMULA_LENGTH - 2);
    else
     memcpy(str, str + 1, Xq_FORMULA_LENGTH - 1);
   }
   else
    if (str[0] == '-' && str[1] == '0' && indexs == 2)
    {
     if (str[2] == '0')
      memcpy(str + 1, str + 3, Xq_FORMULA_LENGTH - 3);
     else
      memcpy(str + 1, str + 2, Xq_FORMULA_LENGTH - 2);
    }
  }
 case Xq_EXPONANTIAL:
  if (type != Xq_PERCENT)
  {
   char tmp_str[Xq_FORMULA_LENGTH];
   int indexs, intp=0, decp=0, sign = 0, exps = 0, i;
   int comma_pos = -1, exp_pos = -1, final_exp;
      
   indexs = 0;
   if (str[indexs] == '-')
   { 
    indexs++; 
    sign = 1;
   }
      
   while(str[indexs])
   {
    if (str[indexs] == '.')
    {
     comma_pos = indexs;
     intp = indexs - sign;
    }
    if (str[indexs] == 'E')
     exp_pos = indexs;
    indexs ++;
   }
      
   if (comma_pos == -1)
   {
    decp = 0;
    if (exp_pos == -1)
     intp = indexs - sign;
   }
   else
   {
    decp = (exp_pos != -1 ? exp_pos : indexs) - comma_pos - 1;
   }
      
   if (exp_pos != -1)
   {
    if (str[exp_pos+1] == '-')
     exps = 1;
    strcpy(tmp_str, str + exp_pos + exps + 1);
    expp = atoi(tmp_str);
   }
      
   if (exps) expp = -expp;
      
   Xc_TRACE(("before: type exponantial: %s", str));
   Xc_TRACE(("exps: %d mantise: %c%d.%d", expp, sign?'-':'+', intp, decp));
      
      
   if (intp > 1)
   {
    sprintf(tmp_str, "%c.", str[sign]);
    strncat(tmp_str, str + sign + 1, intp - 1);
    if (decp) strncat(tmp_str, str + comma_pos + 1, decp);
    expp += intp - 1;
    decp += intp - 1;
    intp = 1;
   }
   else
   {
    if (str[sign] == '0' && decp)
    {
     indexs = comma_pos + 1;
     while(str[indexs] == '0')
      indexs++;
     sprintf(tmp_str, "%c.", str[indexs]);
     strncat(tmp_str, str + indexs + 1, decp - (indexs - comma_pos));
     expp -= indexs - comma_pos;
     decp -= indexs - comma_pos; 
    }
    else
    {
     if (exp_pos != -1)
     {
      strncpy(tmp_str, str + sign, exp_pos);
      tmp_str[exp_pos - sign] = '\0'; 
      /* curious, strncpy should normaly add a null char */
     }
     else
      strcpy(tmp_str, str + sign);
    }
   }
   if (decp)
   {
    indexs = strlen(tmp_str);
    while(tmp_str[indexs - 1] == '0')
    {
     indexs--;
     decp--;
    }
    if (tmp_str[indexs] == '.') indexs--;
    tmp_str[indexs] = '\0';
   }
      
   Xc_TRACE(("after: type exponantial: %s", tmp_str));
   Xc_TRACE(("exps: %d mantisa: %c%d.%d", expp, sign?'-':'+', intp, decp));
      
   if (This->exp_multiple != 1)
   {	
    if (expp < 0)
     final_exp = -(((ABS(expp) / This->exp_multiple) + 1)
		   * This->exp_multiple);	  
    else
     final_exp = (expp / This->exp_multiple) * This->exp_multiple;

	
    if (final_exp != expp)
    {
     final_exp = expp - final_exp;
     decp -= final_exp;
     if (decp <= 0)
     {	    
      indexs = strlen(tmp_str);
      if (indexs > 1)
       memmove(tmp_str + 1, tmp_str + 2, --indexs);
      for(i = 0; i < -decp; i++)
       tmp_str[indexs++] = '0';
      tmp_str[indexs] = '\0';
      decp = 0;
     }
     else
     {
      memmove(tmp_str + 1, tmp_str + 2, final_exp);
      tmp_str[final_exp + 1] = '.';
     }
     expp -= final_exp;
     intp += final_exp;
    }
   }
      
   strcpy(str + sign, tmp_str);
   Xc_TRACE(("finaly: type exponantial: %s", str));
   Xc_TRACE(("exps: %d mantisa: %c%d.%d", expp, sign?'-':'+', intp, decp));
  }
 case Xq_UNIT:
 case Xq_VALUE:
  {
   int	p_point, space_nbr, last;
   int	indexd, indexs,	mind;
   int	k, first_digit, first_decimal, nb_decimal;
   boolean	carry, comma;
      
   indexd = indexs = 0;
     
   info.ruler = XqR_H_ALIGN_RIGHT;
   if (str[indexs]=='-')
   {
    k = first_digit = indexs + 1;
    if(This->color2 != GlobColorAuto)
     info.color = This->color2;
   }
   else
   {
    k = first_digit = indexs;
    if (This->color1 != GlobColorAuto)
     info.color = This->color1;
   }
      
   while(str[k] && str[k]!='.')
    k++;
      
   if (str[k])
   {
    first_decimal = ++k;
    while(str[k])
     k++;
    nb_decimal = k - first_decimal;
    if (nb_decimal > (int)This->max_prec)
    {
     k = first_decimal + This->max_prec;
     if (str[k] >= '5')
     {
      k--;
      comma = FALSE; carry = TRUE;
      do
      {
       if (str[k] == '.')
       {
	k--;
	comma = TRUE;
	continue;
       }
       if (str[k] != '9')
       {
	if (str[k]>='0' && str[k]<='8')
	{
	 str[k] += 1;
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
	str[k] = '0';
	carry = TRUE;
	if (k == first_digit) 
	 break;
	k--;
       }
      }
      while(carry);
      if (carry)
      {
       memmove(str + k + 1, str + k, 
	       strlen(str)+1-k);
       str[k] = '1';
      }
     }
    }
   }
      
   if ((type == Xq_UNIT || type == Xq_EXPONANTIAL) 
       && !This->unitafter && This->unit[0] != '\0')
   {
    last = 0;
    while (This->unit[last] != '\0')
    {
     _Form_str[indexd++] = This->unit[last];
     last++;
    }
   }
      
   if (This->space_each && This->char_sep != '\0')
   {
    mind = 0;
    p_point = 0;
    if (str[p_point]=='-')
    {
     _Form_str[indexd++]=str[indexs++];
     mind = 1;
    }
    while ((str[p_point+mind]!='.') && (str[p_point+mind]!=0))
     p_point++;
    space_nbr = p_point / This->space_each;
    last = p_point % This->space_each;
    if (!last && space_nbr > 0)
    {
     space_nbr--;
     last = This->space_each;
    }
    while (space_nbr)
    {
     while (last)
     {
      _Form_str[indexd++]=str[indexs++];
      last--;
     }
     _Form_str[indexd++]=This->char_sep;
     space_nbr--;
     last = This->space_each;
    }
    while (last)
    {
     _Form_str[indexd++]=str[indexs++];
     last--;
    }
   }
   else
   {
    while ((str[indexs]!='.') && (str[indexs]!=0))
     _Form_str[indexd++]=str[indexs++];
   }
      
   info.trunc_begin = indexd;
      
   if (str[indexs] == '.')
   {
    indexs++;
    if (This->max_prec != 0)
    {
     info.comma_pos = indexd;
     _Form_str[indexd++] = This->comma_sep;
    }
    else
     info.comma_pos = -1;
   }
   else
    info.comma_pos = -1;
      
   last = This->max_prec;
   while (last)
   {
    if (str[indexs])
     _Form_str[indexd++] = str[indexs++];
    else
     break;
    last--;
   }
   last = This->min_prec - (This->max_prec - last);
   while (last>0)
   {
    last--;
    if (info.comma_pos == -1)
    {
     info.comma_pos = indexd;
     _Form_str[indexd++] = This->comma_sep;
    }
    _Form_str[indexd++] = '0';
   }
      
   if (This->truncate)
   {
    if (This->min_prec)
     info.trunc_begin += This->min_prec+1;
	
    info.trunc_end = indexd;
	
    if (info.trunc_begin >= info.trunc_end)
    {
     info.truncable = FALSE;
     info.trunc_begin = -1;
    }
    else
     if (type == Xq_VALUE)
      info.truncable = TRUE;
   }
   else
   {
    info.trunc_begin = info.trunc_end = -1;
    info.trunc_end = FALSE;
   }
      
   if (type == Xq_EXPONANTIAL && !(This->exp_digit == 0 && expp == 0))
   {
    char fmt_str[10], exp_str[40];
	
    _Form_str[indexd] = '\0';
    strcat(_Form_str, This->exp_string);
    if (expp >= 0 && This->exp_sign)
     strcat(_Form_str, "+");
    else if (expp < 0)
     strcat(_Form_str, "-");
	
    sprintf(fmt_str, "%%0%dd", This->exp_digit);
    sprintf(exp_str, fmt_str, ABS(expp));
    strcat(_Form_str, exp_str);
    indexd = strlen(_Form_str);
   }
   if ((type == Xq_UNIT || type == Xq_EXPONANTIAL)
       && This->unitafter && This->unit[0] != '\0')
   {
    last = 0;
    while (This->unit[last] != '\0')
    {
     _Form_str[indexd++] = This->unit[last];
     last++;
    }
   }
   if (type == Xq_PERCENT)
   {
    _Form_str[indexd++] = '%';
   }
   _Form_str[indexd] = 0;
   strcpy(str, _Form_str);
  }
  break;
 case Xq_FRACTION:
  {
   double	value, val, dec = 0.0, u, a, x, y, h, i, c;
   char	ptr[Xq_FORMULA_LENGTH];
      
   value = atof(str);
   info.ruler = XqR_H_ALIGN_RIGHT;
   if (value < 0.0)
   {
    if (This->color2 != GlobColorAuto)
     info.color = This->color2;
   }
   else
   {   
    if (This->color1 != GlobColorAuto)
     info.color = This->color1;
   }
      
   if ((value < 0.0 && value == ceil(value)) || 
       (value >= 0.0 && value == floor(value)))
   {
    if (This->frac_dec)
    {
     dec = value;
     h = 0.0; i = 1.0;
    }
    else
    {
     h = value; i = 1.0;
    }
   }
   else
   {
    if (This->frac_dec)
    {
     if (value < 0.0)
     {
      dec = ceil(value);
      val = dec - value;
     }
     else
     {
      dec = floor(value);
      val = value - dec;
     }
    }
    else
    {
     val = value;
    }
	
    u = val;
    a = floor(u);
    x = a;
    y = h = 1.0;
    i = 0.0;
    while( (x/y != val) && (u != a) && (log10(y) <= This->frac_prec))
    {
     u = 1 / (u - a);
     a = floor(u);
     c = x * a + h;
     h = x;
     x = c;
     c = y * a + i;
     i = y;
     y = c;
    }

    if (x/y == val)
    {
     h = x; i = y;
    }
   }
      
   if (This->frac_dec)
    sprintf(ptr, "%.0f %.0f/%.0f", dec, h, i);
   else
    sprintf(ptr, "%.0f/%.0f", h, i);
   if (This->unit[0] != '\0' && This->special != XcFt_GENERIC)
   {
    if (This->unitafter)
     sprintf(str, "%s%s", ptr, This->unit);
    else
     sprintf(str, "%s%s", This->unit, ptr);
   }
   else
    strcpy(str, ptr);
  }
  break;
 case Xq_BOOLEAN:
  info.ruler = XqR_H_ALIGN_CENTER;
  info.color = NULL;
  if (str[0] == '0' && str[1] == '\0')
  {
   if(This->color2 != GlobColorAuto)
    info.color = This->color2;
   strcpy(str, This->false_name);
  }
  else
  {   
   if(This->color1 != GlobColorAuto)
    info.color = This->color1;
   strcpy(str, This->true_name);
  }
  break;
 case Xq_DAY_IN_WEEK:
  {
   int	value, i, j;
   char	*day_name;
      
   value = atoi(str);
   info.ruler = XqR_H_ALIGN_RIGHT;
   value = (value - 1) % 7;
   if (value < 0) value = 7 + value;
   switch(This->day_name_size)
   {
   case Xq_NAME_VERY_SHORT:
    day_name = F(GlobResources).getString(GlobResources,
					  XcR_dateDayVeryShort);
    i = j = 0;
    while(day_name[i] && j < value)
    {
     if (day_name[i] == ' ')
      j++;
     i++;
    }
    if (j != value)
     strcpy(str, "??");
    else
    {
     j = i;
     while(day_name[j] && day_name[j] != ' ')
      j++;
     memcpy(str, day_name + i, j - i);
     str[j-i] = '\0';
    }
    break;
   case Xq_NAME_SHORT:
    day_name = F(GlobResources).getString(GlobResources,
					  XcR_dateDayMonShort + value);
    strncpy(str, day_name, Xq_FORMULA_LENGTH);
    break;
   case Xq_NAME_LONG:
    day_name = F(GlobResources).getString(GlobResources,
					  XcR_dateDayMon + value);
    strncpy(str, day_name, Xq_FORMULA_LENGTH);
    break;
   }
  }
  break;
 case Xq_MONTH:
  {
   int	value, i, j;
   char	*month_name;
      
   value = atoi(str);
   info.ruler = XqR_H_ALIGN_RIGHT;
   value = (value - 1) % 12;
   if (value < 0) value = 12 + value;
   switch(This->month_name_size)
   {
   case Xq_NAME_VERY_SHORT:
    month_name = F(GlobResources).getString(GlobResources,
					    XcR_dateMonthVeryShort);
    i = j = 0;
    while(month_name[i] && j < value)
    {
     if (month_name[i] == ' ')
      j++;
     i++;
    }
    if (j != value)
     strcpy(str, "??");
    else
    {
     j = i;
     while(month_name[j] && month_name[j] != ' ')
      j++;
     memcpy(str, month_name + i, j - i);
     str[j-i] = '\0';
    }
    break;
   case Xq_NAME_SHORT:
    month_name = F(GlobResources).getString(GlobResources,
					    XcR_dateMonthJanShort + value);
    strncpy(str, month_name, Xq_FORMULA_LENGTH);
    break;
   case Xq_NAME_LONG:
    month_name = F(GlobResources).getString(GlobResources,
					    XcR_dateMonthJan + value);
    strncpy(str, month_name, Xq_FORMULA_LENGTH);
    break;
   }
  }
  break;
 case Xq_DATE:
  {
   double	value;
   xc_date_t	xc_date;
   dt_seq_t	**dt_seq;
      
   value = atof(str);
   info.ruler = XqR_H_ALIGN_RIGHT;
   convert_to_date(value, &xc_date);
   dt_seq = This->date_seq;
   str[0] = '\0';
   while(*dt_seq)
   {
    if (dt_seq_funcs[(*dt_seq)->dt_seq_id].id == Xq_DATE_ID)
     dt_seq_funcs[(*dt_seq)->dt_seq_id].concate(*dt_seq, str, &xc_date);
    else
     dt_seq_funcs[(*dt_seq)->dt_seq_id].concate(*dt_seq, str);
    dt_seq++;
   }
  }
  break;
 case Xq_TIME:
  {
   double	value;
   xc_time_t	xc_time;
   dt_seq_t	**dt_seq;
      
   value = atof(str);
   info.ruler = XqR_H_ALIGN_RIGHT;
   convert_to_time(value, &xc_time);
   dt_seq = This->time_seq;
   str[0] = '\0';
   while(*dt_seq)
   {
    if (dt_seq_funcs[(*dt_seq)->dt_seq_id].id == Xq_TIME_ID)
     dt_seq_funcs[(*dt_seq)->dt_seq_id].concate(*dt_seq, str, &xc_time);
    else
     dt_seq_funcs[(*dt_seq)->dt_seq_id].concate(*dt_seq, str);
    dt_seq++;
   }
  }
  break;
 case Xq_DATE_TIME:
  {
   double	value;
   xc_date_time_t	xc_date_time;
   dt_seq_t	**dt_seq;
      
   value = atof(str);
   info.ruler = XqR_H_ALIGN_RIGHT;
   convert_to_date_and_time(value, &xc_date_time);
   dt_seq = This->date_time_seq;
   str[0] = '\0';
   while(*dt_seq)
   {
    switch(dt_seq_funcs[(*dt_seq)->dt_seq_id].id)
    {
    case Xq_DATE_ID:
     dt_seq_funcs[(*dt_seq)->dt_seq_id].concate(*dt_seq, str,
						&(xc_date_time.date));
     break;
    case Xq_TIME_ID:
     dt_seq_funcs[(*dt_seq)->dt_seq_id].concate(*dt_seq, str, 
						&(xc_date_time.time));
     break;
    case Xq_DATE_TIME_ID:
     dt_seq_funcs[(*dt_seq)->dt_seq_id].concate(*dt_seq, str);
     break;
    }
    dt_seq++;
   }
  }
  break;
 case Xq_TEXT:
 default:
  break;
 }
  
 if (info_ret)
  memcpy(info_ret, &info, sizeof(conv_info_t));
}

/* ----------------------------------------------------------------- ** 
** vset - Set format parameters                                      ** 
** ----------------------------------------------------------------- */
static void vset(f, ap)
c_FormatNbr *f;
va_list ap;
{
 boolean out_flag;
 c_Color *color;
 dt_seq_t *dt_seq, **pdt_seq;
 char *ptr;
 int	num;
  
 Xc_TRACE(("vset"));

 out_flag = FALSE;
 do {
     switch(va_arg(ap, format_set_code_t))
     {
     case XcF_END:
      Xc_TRACE(("XcF_END"));
      out_flag = TRUE;
      break;
	
     case XcF_TYPE:
      f->other_format[f->type] = NULL;
      f->type = va_arg(ap, int);
      Xc_TRACE(("XcF_TYPE: %d", f->type));
      break;
	
     case XcF_NAME:
      strncpy(f->name, va_arg(ap, char *), XcMAX_NAME_LENGTH - 1);
      Xc_TRACE(("XcF_NAME `%s'", f->name));
      break;
	
     case XcF_MINPREC:
      f->min_prec = va_arg(ap, unsigned int);
      Xc_TRACE(("XcF_MINPREC `%d'", f->min_prec));
      break;

     case XcF_MAXPREC:
      f->max_prec = va_arg(ap, unsigned int);
      Xc_TRACE(("XcF_MAXPREC `%d'", f->max_prec));
      break;

     case XcF_SPACE:
      f->space_each = va_arg(ap, unsigned int);
      Xc_TRACE(("XcF_SPACE `%d'", f->space_each));
      break;

     case XcF_UNIT_NAME:
      ptr = va_arg(ap, char *);
      if (!ptr)
       f->unit[0] = '\0';
      else
       strncpy(f->unit, ptr, XcMAX_NAME_LENGTH - 1);
      Xc_TRACE(("XcF_UNIT_NAME `%s'", f->unit));
      break;

     case XcF_COMMA:
      f->comma_sep = (char)va_arg(ap, int);
      Xc_TRACE(("XcF_COMMA `%c'", f->comma_sep));
      break;

     case XcF_COLORP:
     case XcF_TRUE_COLOR:
      color = va_arg(ap, c_Color *);	
      Xc_TRACE(("XcF_COLORP `%s'", color->name));
      if (color == GlobColorAuto) color = NULL;
      if(f->color1 == GlobColorAuto) f->color1 = NULL;
      if(XcH_REPLACE(f->color1, color, hook_color, f)) f->color1 = color;
      if(f->color1 == NULL) f->color1 = GlobColorAuto;
      break;

     case XcF_COLORN:
     case XcF_FALSE_COLOR:
      color = va_arg(ap, c_Color *);	
      Xc_TRACE(("XcF_COLORN `%s'", color->name));
      if (color == GlobColorAuto) color = NULL;
      if(f->color2 == GlobColorAuto) f->color2 = NULL;
      if(XcH_REPLACE(f->color2, color, hook_color, f)) f->color2 = color;
      if(f->color2 == NULL) f->color2 = GlobColorAuto;
      break;

     case XcF_UNITAFTER:
      f->unitafter = (boolean)va_arg(ap, int);
      Xc_TRACE(("XcF_UNITAFTER `%d'", f->unitafter));
      break;

     case XcF_CHARSEP:
      f->char_sep = (char)va_arg(ap, int);
      Xc_TRACE(("XcF_CHARSEP `%c'", f->char_sep));
      break;

     case XcF_TRUNCATE:
      f->truncate = (boolean)va_arg(ap, int);
      Xc_TRACE(("XcF_TRUNCATE `%d'", f->truncate));
      break;
	
     case XcF_EXPDIGIT:
      f->exp_digit = va_arg(ap, unsigned int);
      Xc_TRACE(("XcF_EXPDIGIT `%d'", f->exp_digit));
      break;
	
     case XcF_EXPMULTIPLE:
      f->exp_multiple = va_arg(ap, unsigned int);
      Xc_TRACE(("XcF_EXPMULTIPLE `%d'", f->exp_multiple));
      break;
	
     case XcF_EXPSTRING:
      ptr = va_arg(ap, char *);
      if (!ptr)
      {
       f->exp_string[0] = 'E';
       f->exp_string[1] = '\0';
      }
      else
       strncpy(f->exp_string, ptr, XcMAX_NAME_LENGTH - 1);
      Xc_TRACE(("XcF_EXPSTRING `%s'", f->exp_string));
      break;
	
     case XcF_EXPSIGN:
      f->exp_sign = (boolean)va_arg(ap, int);
      Xc_TRACE(("XcF_EXPSIGN `%d'", f->exp_sign));
      break;
	
     case XcF_FRAC_PREC:
      f->frac_prec = (unsigned int)va_arg(ap, unsigned int);
      Xc_TRACE(("XcF_FRAC_PREC `%d'", f->frac_prec));
      break;
	
     case XcF_FRAC_DEC:
      f->frac_dec = (boolean)va_arg(ap, int);
      Xc_TRACE(("XcF_FRAC_DEC `%s'", f->frac_dec ? "TRUE" : "FALSE"));
      break;
	
     case XcF_TRUE_NAME:
      strncpy(f->true_name, va_arg(ap, char *), XcMAX_NAME_LENGTH - 1);
      Xc_TRACE(("XcF_TRUE_NAME"));
      break;
	
     case XcF_FALSE_NAME:
      strncpy(f->false_name, va_arg(ap, char *), XcMAX_NAME_LENGTH - 1);
      Xc_TRACE(("XcF_FALSE_NAME"));
      break;
	
     case XcF_DAY_NAME_SIZE:
      f->day_name_size = va_arg(ap, name_size_id_t);
      Xc_TRACE(("XcF_DAY_NAME_SIZE: %d", f->day_name_size));
      break;
	
     case XcF_MONTH_NAME_SIZE:
      f->month_name_size = va_arg(ap, name_size_id_t);
      Xc_TRACE(("XcF_MONTH_NAME_SIZE: %d", f->month_name_size));
      break;
	
     case XcF_ADD_DATE_SEQ:
      num = va_arg(ap, int);
      Xc_TRACE(("XcF_ADD_DATE_SEQ type: %d", num));
      if ((dt_seq_funcs[num].id & Xq_DATE_ID) == 0)
      {
       Xc_WARNING(("Adding wrong sequence to date sequence"));
       break;
      }
      switch(dt_seq_funcs[num].type)
      {
      case Xq_DT_SEQ_PADDING:
       dt_seq = dt_seq_funcs[num].new(num, va_arg(ap, padding_id_t));
       break;
      case Xq_DT_SEQ_NAME:
       dt_seq = dt_seq_funcs[num].new(num, va_arg(ap, name_size_id_t));
       break;
      case Xq_DT_SEQ_SEPARATOR:
       dt_seq = dt_seq_funcs[num].new(num, va_arg(ap, char *));
       break;
      case Xq_DT_SEQ_SIMPLE:
      default:
       dt_seq = dt_seq_funcs[num].new(num);
       break;
      }
      insert_dt_seq(f->date_seq, dt_seq);
      break;
      
     case XcF_ADD_TIME_SEQ:
      num = va_arg(ap, int);
      Xc_TRACE(("XcF_ADD_TIME_SEQ type: %d", num));
      if ((dt_seq_funcs[num].id & Xq_TIME_ID) == 0)
      {
       Xc_WARNING(("Adding wrong sequence to time sequence"));
       break;
      }
      switch(dt_seq_funcs[num].type)
      {
      case Xq_DT_SEQ_PADDING:
       dt_seq = dt_seq_funcs[num].new(num, va_arg(ap, padding_id_t));
       break;
      case Xq_DT_SEQ_NAME:
       dt_seq = dt_seq_funcs[num].new(num, va_arg(ap, name_size_id_t));
       break;
      case Xq_DT_SEQ_SEPARATOR:
       dt_seq = dt_seq_funcs[num].new(num, va_arg(ap, char *));
       break;
      case Xq_DT_SEQ_SIMPLE:
      default:
       dt_seq = dt_seq_funcs[num].new(num);
       break;
      }
      insert_dt_seq(f->time_seq, dt_seq);
      break;
      
     case XcF_ADD_DATE_TIME_SEQ:
      num = va_arg(ap, int);
      Xc_TRACE(("XcF_ADD_DATE_TIME_SEQ type: %d", num));
      switch(dt_seq_funcs[num].type)
      {
      case Xq_DT_SEQ_PADDING:
       dt_seq = dt_seq_funcs[num].new(num, va_arg(ap, padding_id_t));
       break;
      case Xq_DT_SEQ_NAME:
       dt_seq = dt_seq_funcs[num].new(num, va_arg(ap, name_size_id_t));
       break;
      case Xq_DT_SEQ_SEPARATOR:
       dt_seq = dt_seq_funcs[num].new(num, va_arg(ap, char *));
       break;
      case Xq_DT_SEQ_SIMPLE:
      default:
       dt_seq = dt_seq_funcs[num].new(num);
       break;
      }
      insert_dt_seq(f->date_time_seq, dt_seq);
      break;
      
     case XcF_SET_DATE_SEQS:
      Xc_TRACE(("XcF_SET_DATE_SEQS"));
      pdt_seq = (dt_seq_t **)va_arg(ap, dt_seq_t **);
      destroy_all_dt_seq(f->date_seq);
      copy_all_dt_seq(f->date_seq, pdt_seq);
      break;
	
     case XcF_SET_TIME_SEQS:
      Xc_TRACE(("XcF_SET_TIME_SEQS"));
      pdt_seq = (dt_seq_t **)va_arg(ap, dt_seq_t **);
      destroy_all_dt_seq(f->time_seq);
      copy_all_dt_seq(f->time_seq, pdt_seq);
      break;
	
     case XcF_SET_DATE_TIME_SEQS:
      Xc_TRACE(("XcF_SET_DATE_TIME_SEQS"));
      pdt_seq = (dt_seq_t **)va_arg(ap, dt_seq_t **);
      destroy_all_dt_seq(f->date_time_seq);
      copy_all_dt_seq(f->date_time_seq, pdt_seq);
      break;

     case XcF_OTHER_TYPE:
      Xc_TRACE(("XcF_OTHER_TYPE: %d", num));
      num = va_arg(ap, int);
      f->other_format[num] = (c_FormatNbr *)va_arg(ap, void *);
      break;
      
     case XcF_ALL_OTHER_TYPES:
      Xc_TRACE(("XcF_ALL_OTHER_TYPES"));
      memcpy(f->other_format, (c_FormatNbr **)va_arg(ap, void *),
	     sizeof(c_FormatNbr *) * Xq_NB_TYPE);
      break;
	
     case XcF_SPECIAL:
      f->special = (boolean)va_arg(ap, format_type_t);
      break;
	
     case XcF_HIDE_ON:
      Xc_TRACE(("XcF_HIDE_ON"));
      f->hide_flag = TRUE;
      break;
	
     case XcF_HIDE_OFF:
      Xc_TRACE(("XcF_HIDE_OFF"));
      f->hide_flag = FALSE;
      break;
	  
     case XcF_LOCK:
      f->lock_flag = TRUE;
      break;
	
     case XcF_PARSE_STRING:
      ptr = (char *)va_arg(ap, char *);
      parseFormatString(f, ptr);
      break;
	
     default:
      Xc_BREAK(("Unknow color-set-code."));
      break;
     }
    } while(!out_flag);
}

/* ----------------------------------------------------------------- ** 
** readFormatList - Load some Formats                                ** 
** ----------------------------------------------------------------- */
static void parseFormatString(This, ptr)
c_FormatNbr *This;
char *ptr;
{
 int	i = 0;
  
 while(i < NB_EXCEL_FORMAT)
 {
  if (Xstrcmp(ptr, excel_format[i].string))
  {
   excel_format[i].xlf_function(This, ptr);
   break;
  }
  i++;
 }
}

/* ----------------------------------------------------------------- **
** hook_color - Color hook                                           **
** ----------------------------------------------------------------- */
static void hook_color(this, reason, old, new)
c_FormatNbr *this;
int reason;
c_Color *old;
c_Color *new;
{
 Xc_TRACE(("hook_color"));

 if(reason == XcH_CONTENT_CHANGED)
 {
  F(this->hook).callback(this->hook, XcH_CONTENT_CHANGED, this);
  return;
 }
}
