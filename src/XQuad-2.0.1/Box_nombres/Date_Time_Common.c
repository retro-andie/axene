/*
** Date_Time_Common.c for XQuad in Box_nombres/
** Object and method for Box_nombres Class
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
** Started on  Mon May 22 13:32:14 1995 Robin Castanier
** Last update Fri Oct  2 17:08:28 1998 Robin Castanier
*/

#include "Date_Time_Common.h"
#include "TimeFuncs.h"

static int strlen_seq();
static void calcul_strlen_dt_seq();
static void duplicate_dt_seq_tab();
static void add_dt_seq_to_base();
static void remove_from_dt_seq();
static void insert_to_dt_seq();

void bn_tf_modify(w, This, cbs)
Widget w;
c_Box_nombres *This;
XmTextVerifyCallbackStruct *cbs;
{
 c_FormatNbr	**format;
 char		field[Xq_FORMULA_LENGTH];
 int		seq_str[XqMAX_DT_SEQ+1];
 dt_seq_t	*seq_dst[XqMAX_DT_SEQ+1];
 dt_seq_t	*seq;
 xc_date_time_t	xc_date_time;
 XmTextPosition	left, right;
 double	value;  
 int		i, num;
  
 if (This->modify_flag)
  return;

 Xc_TRACE(("ci: %d ni: %d sp: %d ep: %d", cbs->currInsert,
	   cbs->newInsert, cbs->startPos, cbs->endPos)); 
  
 This->modify_flag = TRUE;
  
 if (!F(This->PBox->ItemList).getSelection
     (This->PBox->ItemList, (void ***)&format, &num) || num != 1)
  return;
  
 if (This->preview_string)
  value = atof(This->preview_string);
 else
  value = atof(This->bnp_bar[This->selected_bnp_bar]->preview_string);
  
 switch (This->selected_bnp_bar)
 {
 case BN_FMT_DATE:
  if (F(This->list_save).get(This->list_save,
			     *format, 20, seq_dst) == -1)
   duplicate_dt_seq_tab(This, (*format)->date_seq, seq_dst);
  convert_to_date(value, &(xc_date_time.date));
  break;
 case BN_FMT_TIME:
  if (F(This->list_save).get(This->list_save,
			     *format, 21, seq_dst) == -1)
   duplicate_dt_seq_tab(This, (*format)->time_seq, seq_dst);
  convert_to_time(value, &(xc_date_time.time));
  break;
 case BN_FMT_DATE_TIME:
  if (F(This->list_save).get(This->list_save,
			     *format, 22, seq_dst) == -1)
   duplicate_dt_seq_tab(This, (*format)->date_time_seq, seq_dst);
  convert_to_date_and_time(value, &xc_date_time);
  break;
 }
  
 calcul_strlen_dt_seq(seq_dst, seq_str, &xc_date_time);
  
 left =  cbs->startPos;
 right = cbs->endPos;
  
 if (left != right)
  remove_from_dt_seq(This, seq_dst, seq_str, &left, right);
  
 if (cbs->text->ptr != NULL && cbs->text->length)
 {
  strncpy(field, cbs->text->ptr, 
	  MIN(cbs->text->length, Xq_FORMULA_LENGTH - 1));
  field[MIN(cbs->text->length, Xq_FORMULA_LENGTH - 1)] = '\0';

  Xc_TRACE(("insert text: `%s' at pos: %d", field, cbs->startPos));
      
  for(i = 0; i < (int)strlen(field); i++)
   if ((unsigned char)field[i] < ' ') field[i] = ' ';
      
  seq = dt_seq_funcs[Xq_DT_SEPARATOR].new(Xq_DT_SEPARATOR, field);
  add_dt_seq_to_base(This, seq);
  insert_to_dt_seq(This, seq, seq_dst, seq_str, &left);
  left += strlen_seq(seq, &xc_date_time);
 }
  
 switch (This->selected_bnp_bar)
 {
 case BN_FMT_DATE:
  F(This->list_save).set(This->list_save, *format, 20, seq_dst);
  break;
 case BN_FMT_TIME:
  F(This->list_save).set(This->list_save, *format, 21, seq_dst);
  break;
 case BN_FMT_DATE_TIME:
  F(This->list_save).set(This->list_save, *format, 22, seq_dst);
  break;
 }
  
 F(This).refresh_preview(This);
 XmTextFieldSetInsertionPosition(WIDGET(This->tfPreview), left);
 Xc_TRACE(("replace cursor at: %d", left));
 cbs->doit = False;
 Xc_free(format);
 This->modify_flag = FALSE;
}

void bn_insert_dt_seq(This, seq)
c_Box_nombres *This;
dt_seq_t *seq;
{
 c_FormatNbr	**format;
 XmTextPosition	left, right;
 int		seq_str[XqMAX_DT_SEQ+1];
 dt_seq_t	*seq_dst[XqMAX_DT_SEQ+1];
 xc_date_time_t	xc_date_time;
 double	value;  
 int		num;
  
 if (!F(This->PBox->ItemList).getSelection
     (This->PBox->ItemList, (void ***)&format, &num) || num != 1)
  return;
  
 if (This->preview_string)
  value = atof(This->preview_string);
 else
  value = atof(This->bnp_bar[This->selected_bnp_bar]->preview_string);
  
 switch (This->selected_bnp_bar)
 {
 case BN_FMT_DATE:
  if (F(This->list_save).get(This->list_save,
			     *format, 20, seq_dst) == -1)
   duplicate_dt_seq_tab(This, (*format)->date_seq, seq_dst);
  convert_to_date(value, &(xc_date_time.date));
  break;
 case BN_FMT_TIME:
  if (F(This->list_save).get(This->list_save,
			     *format, 21, seq_dst) == -1)
   duplicate_dt_seq_tab(This, (*format)->time_seq, seq_dst);
  convert_to_time(value, &(xc_date_time.time));
  break;
 case BN_FMT_DATE_TIME:
  if (F(This->list_save).get(This->list_save,
			     *format, 22, seq_dst) == -1)
   duplicate_dt_seq_tab(This, (*format)->date_time_seq, seq_dst);
  convert_to_date_and_time(value, &xc_date_time);
  break;
 }
  
 calcul_strlen_dt_seq(seq_dst, seq_str, &xc_date_time);
  
 if (XmTextFieldGetSelectionPosition(WIDGET(This->tfPreview),
				     &left, &right) == True)
 {
  remove_from_dt_seq(This, seq_dst, seq_str, &left, right);
  XmTextFieldClearSelection(WIDGET(This->tfPreview), CurrentTime);
 }
 else
  left = XmTextFieldGetInsertionPosition(WIDGET(This->tfPreview));

 insert_to_dt_seq(This, seq, seq_dst, seq_str, &left);

 switch (This->selected_bnp_bar)
 {
 case BN_FMT_DATE:
  F(This->list_save).set(This->list_save, *format, 20, seq_dst);
  break;
 case BN_FMT_TIME:
  F(This->list_save).set(This->list_save, *format, 21, seq_dst);
  break;
 case BN_FMT_DATE_TIME:
  F(This->list_save).set(This->list_save, *format, 22, seq_dst);
  break;
 }
  
 F(This).refresh_preview(This);
 XmTextFieldSetInsertionPosition(WIDGET(This->tfPreview), 
				 left + strlen_seq(seq, &xc_date_time));
 Xc_free(format);
}

static int strlen_seq(dt_seq, date_time)
dt_seq_t *dt_seq;
xc_date_time_t *date_time;
{
 char	field[Xq_FORMULA_LENGTH];
  
 *field = '\0';
 switch(dt_seq_funcs[dt_seq->dt_seq_id].id)
 {
 case Xq_DATE_ID:
  dt_seq_funcs[dt_seq->dt_seq_id].concate(dt_seq, field,
					  &(date_time->date));
  break;
 case Xq_TIME_ID:
  dt_seq_funcs[dt_seq->dt_seq_id].concate(dt_seq, field, 
					  &(date_time->time));
  break;
 case Xq_DATE_TIME_ID:
  dt_seq_funcs[dt_seq->dt_seq_id].concate(dt_seq, field);
  break;
 }
 return strlen(field);
}

static void calcul_strlen_dt_seq(dt_seq, seq_str, date_time)
dt_seq_t *dt_seq[XqMAX_DT_SEQ+1];
int seq_str[XqMAX_DT_SEQ+1];
xc_date_time_t *date_time;
{
 int i = 0;
  
 while(*dt_seq)
 {
  seq_str[i++] = strlen_seq(*dt_seq, date_time);
  dt_seq++;
 }
 seq_str[i] = 0;
}

static void duplicate_dt_seq_tab(This, seq_src, seq_dst)
c_Box_nombres	*This;
dt_seq_t	*seq_src[XqMAX_DT_SEQ+1];
dt_seq_t	*seq_dst[XqMAX_DT_SEQ+1];
{
 int	i = 0;
 dt_seq_base_t *base;
  
 copy_all_dt_seq(seq_dst, seq_src);
 while(*seq_dst && i < XqMAX_DT_SEQ+1)
 {
  base = (dt_seq_base_t *)Xc_malloc("dtseqb", sizeof(dt_seq_base_t));
  base->item = *seq_dst;
  base->Next = This->dt_seq_base;
  This->dt_seq_base = base;
  seq_dst++; i++;
 }
 *seq_dst = NULL;
}

static void add_dt_seq_to_base(This, seq)
c_Box_nombres	*This;
dt_seq_t	*seq;
{
 dt_seq_base_t *base;
  
 base = (dt_seq_base_t *)Xc_malloc("dtseqb", sizeof(dt_seq_base_t));
 base->item = seq;
 base->Next = This->dt_seq_base;
 This->dt_seq_base = base;  
}

void remove_dt_seq_from_base(This, seq)
c_Box_nombres	*This;
dt_seq_t	*seq[XqMAX_DT_SEQ+1];
{
 dt_seq_base_t *base1, *base2;
  
 while(*seq)
 {
  base1 = base2 = This->dt_seq_base;
  while(base1 && base1->item != *seq)
  {
   base2 = base1;
   base1 = base1->Next;
  }
    
  if (base1)
  {
   if (base1 == base2)
   {
    This->dt_seq_base = base1->Next;
    dt_seq_funcs[base1->item->dt_seq_id].delete(base1->item);
    Xc_free(base1);
   }
   else
   {
    base2->Next = base1->Next;
    dt_seq_funcs[base1->item->dt_seq_id].delete(base1->item);
    Xc_free(base1);
   }      
  }
  seq++;
 }
}

void free_dt_seq_base(This)
c_Box_nombres	*This;
{
 dt_seq_base_t *base1, *base2;
  
 base1 = This->dt_seq_base;
  
 while(base1)
 {
  base2 = base1->Next;
  dt_seq_funcs[base1->item->dt_seq_id].delete(base1->item);
  Xc_free(base1);
  base1 = base2;
 }
}

static void remove_from_dt_seq(This, seq_src, seq_str, left, right)
c_Box_nombres	*This;
dt_seq_t	*seq_src[XqMAX_DT_SEQ+1];
int		seq_str[XqMAX_DT_SEQ+1];
int		*left;
int		right;
{
 dt_seq_t	*dt_seq[XqMAX_DT_SEQ+1];
 char	field[Xq_FORMULA_LENGTH];
 int	i, j, oj, k;
  
 memcpy(dt_seq, seq_src, sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1));
 i = j = 0;
 do 
 {
  if (!seq_str[i] || i == XqMAX_DT_SEQ+1) return;
  oj = j;
  j += seq_str[i++];
 } while(j <= *left);
  
 k = --i;
 do
 {
  if (dt_seq_funcs[(dt_seq[i])->dt_seq_id].id == Xq_DATE_TIME_ID)
  {
   if (j > right)
   {
    if (oj <= *left)
    {				/* tagada -> ta**da */
     strcpy(field, seq_src[k]->separator_dt.separator);
		
     memmove(field + *left - oj, field + right - oj, j - right + 1);
     seq_src[k] = dt_seq_funcs[(dt_seq[i])->dt_seq_id].new
      ((dt_seq[i])->dt_seq_id, field);
     add_dt_seq_to_base(This, seq_src[k]);
    }
    else
    {				/* tagada -> **ada */
     strcpy(field, seq_src[k]->separator_dt.separator);
     memmove(field, field + right - oj, j - right + 1);
     seq_src[k] = dt_seq_funcs[(dt_seq[i])->dt_seq_id].new
      ((dt_seq[i])->dt_seq_id, field);
     add_dt_seq_to_base(This, seq_src[k]);
    }
    return;
   }
   else
   {
    if (oj < *left)
    {				/* tagada -> tag*** */
     strcpy(field, seq_src[k]->separator_dt.separator);
     field[*left - oj] = '\0';
     seq_src[k] = dt_seq_funcs[(dt_seq[i])->dt_seq_id].new
      ((dt_seq[i])->dt_seq_id, field);
     add_dt_seq_to_base(This, seq_src[k++]);
    }
    else
    {				/* tagada -> ****** */
     memmove(seq_src + k, seq_src + k + 1, 
	     sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1-k));
    }
   }
  }
  else
  {
   if (*left > oj) *left = oj;
   memmove(seq_src + k, seq_src + k + 1, 
	   sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1-k));
  }
  oj = j;
  j += seq_str[++i];
 } while(j < right);
  
 if (oj >= right) return;
  
 if (dt_seq_funcs[(dt_seq[i])->dt_seq_id].id == Xq_DATE_TIME_ID)
 {
  /* tagada -> **ada */
  strcpy(field, seq_src[k]->separator_dt.separator);
  memmove(field, field + right - oj, j - right + 1);
  seq_src[k] = dt_seq_funcs[(dt_seq[i])->dt_seq_id].new
   ((dt_seq[i])->dt_seq_id, field);
  add_dt_seq_to_base(This, seq_src[k]);
 }
 else
 {
  if (*left > oj) *left = oj;
  memmove(seq_src + k, seq_src + k + 1, 
	  sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1-k));
 }
}

static void insert_to_dt_seq(This, seq, seq_src, seq_str, pos)
c_Box_nombres	*This;
dt_seq_t	*seq;
dt_seq_t	*seq_src[XqMAX_DT_SEQ+1];
int		seq_str[XqMAX_DT_SEQ+1];
int		*pos;
{
 dt_seq_t	*dt_seq[XqMAX_DT_SEQ+1];
 char	field[Xq_FORMULA_LENGTH*2];
 int i, j, oj;
  
 memcpy(dt_seq, seq_src, sizeof(dt_seq_t *) * (XqMAX_DT_SEQ+1));
  
 i = j = oj = 0;

 do 
 {
  if (i == XqMAX_DT_SEQ+1) return;
  oj = j;
  j += seq_str[i++];
 } while(j <= *pos && seq_str[i-1]);
 i--;

 if (oj == *pos)
 {				/* simple insertion between 2 seqs */
  if (dt_seq_funcs[seq->dt_seq_id].id == Xq_DATE_TIME_ID && 
      i > 0 && dt_seq_funcs[(dt_seq[i - 1])->dt_seq_id].id
      == Xq_DATE_TIME_ID)
  {
   strcpy(field, seq_src[i - 1]->separator_dt.separator);
   strcat(field, seq->separator_dt.separator);
   field[Xq_FORMULA_LENGTH - 1] = '\0';
   seq_src[i - 1] = dt_seq_funcs[(dt_seq[i - 1])->dt_seq_id].new
    ((dt_seq[i - 1])->dt_seq_id, field);
   add_dt_seq_to_base(This, seq_src[i - 1]);  
  }
  else
  {
   if(dt_seq_funcs[seq->dt_seq_id].id == Xq_DATE_TIME_ID && 
      dt_seq[i] && dt_seq_funcs[(dt_seq[i])->dt_seq_id].id
      == Xq_DATE_TIME_ID)
   {
    strcpy(field, seq->separator_dt.separator);
    strcat(field, seq_src[i]->separator_dt.separator);
    field[Xq_FORMULA_LENGTH - 1] = '\0';
    seq_src[i] = dt_seq_funcs[(dt_seq[i])->dt_seq_id].new
     ((dt_seq[i])->dt_seq_id, field);
    add_dt_seq_to_base(This, seq_src[i]);  
   }
   else
   {
    if (i < XqMAX_DT_SEQ)
    {
     if (seq_src[i] == NULL)
      seq_src[i+1] = NULL;
     else
      memmove(seq_src + i + 1, seq_src + i, 
	      sizeof(dt_seq_t *) * (XqMAX_DT_SEQ - i));
    }
    else
     return;
    seq_src[i] = dt_seq_funcs[seq->dt_seq_id].copy(seq);
    add_dt_seq_to_base(This, seq_src[i]);
    seq_src[XqMAX_DT_SEQ] = NULL;
   }
  }
  return;
 }
  
 if (dt_seq_funcs[(dt_seq[i])->dt_seq_id].id == Xq_DATE_TIME_ID)
 {
  if (dt_seq_funcs[seq->dt_seq_id].id == Xq_DATE_TIME_ID)
  {
   strncpy(field, dt_seq[i]->separator_dt.separator, *pos - oj);
   field[*pos - oj] = '\0';
   strcat(field, seq->separator_dt.separator);
   strcat(field, dt_seq[i]->separator_dt.separator + *pos - oj);
   field[Xq_FORMULA_LENGTH - 1] = '\0';
   seq_src[i] = dt_seq_funcs[(dt_seq[i])->dt_seq_id].new
    ((dt_seq[i])->dt_seq_id, field);
   add_dt_seq_to_base(This, seq_src[i]);
  }
  else
  {
   strncpy(field, dt_seq[i]->separator_dt.separator, *pos - oj);
   field[*pos - oj] = '\0';
   field[Xq_FORMULA_LENGTH - 1] = '\0';
   seq_src[i] = dt_seq_funcs[(dt_seq[i])->dt_seq_id].new
    ((dt_seq[i])->dt_seq_id, field);
   add_dt_seq_to_base(This, seq_src[i]);
	  
   i++;
   if (i < XqMAX_DT_SEQ)
   {
    if (seq_src[i] == NULL)
     seq_src[i+1] = NULL;
    else
     memmove(seq_src + i + 1, seq_src + i, 
	     sizeof(dt_seq_t *) * (XqMAX_DT_SEQ - i));
   }
   else
    return;
   seq_src[i] = dt_seq_funcs[seq->dt_seq_id].copy(seq);
   add_dt_seq_to_base(This, seq_src[i]);
   seq_src[XqMAX_DT_SEQ] = NULL;
	  
   i++;
   if (i < XqMAX_DT_SEQ)
   {
    if (seq_src[i] == NULL)
     seq_src[i+1] = NULL;
    else
     memmove(seq_src + i + 1, seq_src + i, 
	     sizeof(dt_seq_t *) * (XqMAX_DT_SEQ - i));
   }
   else
    return;
	  
   strcpy(field, dt_seq[i-2]->separator_dt.separator + *pos - oj);
   field[Xq_FORMULA_LENGTH - 1] = '\0';
   seq_src[i] = dt_seq_funcs[(dt_seq[i-2])->dt_seq_id].new
    ((dt_seq[i-2])->dt_seq_id, field);
   add_dt_seq_to_base(This, seq_src[i]);
   seq_src[XqMAX_DT_SEQ] = NULL;
  }
 }
 else
 {
  if (dt_seq_funcs[seq->dt_seq_id].id == Xq_DATE_TIME_ID)
  {
   memmove(seq_src + i, seq_src + i + 1, 
	   sizeof(dt_seq_t *) * (XqMAX_DT_SEQ + 1 - i));
   if (i > 0 && dt_seq_funcs[(dt_seq[i - 1])->dt_seq_id].id
       == Xq_DATE_TIME_ID)
   {
    strcpy(field, seq_src[i - 1]->separator_dt.separator);
    strcat(field, seq->separator_dt.separator);
    field[Xq_FORMULA_LENGTH - 1] = '\0';
    seq_src[i - 1] = dt_seq_funcs[(dt_seq[i - 1])->dt_seq_id].new
     ((dt_seq[i - 1])->dt_seq_id, field);
    add_dt_seq_to_base(This, seq_src[i - 1]);  
	      
    if (dt_seq_funcs[(dt_seq[i])->dt_seq_id].id
	== Xq_DATE_TIME_ID)
    {
     strcat(field, seq_src[i]->separator_dt.separator);
     seq_src[i - 1] = dt_seq_funcs[(dt_seq[i - 1])->dt_seq_id].new
      ((dt_seq[i - 1])->dt_seq_id, field);
     add_dt_seq_to_base(This, seq_src[i - 1]); 
     memmove(seq_src + i, seq_src + i + 1, 
	     sizeof(dt_seq_t *) * (XqMAX_DT_SEQ + 1 - i));
    }
   }
   else
   {
    if (dt_seq_funcs[(dt_seq[i])->dt_seq_id].id
	== Xq_DATE_TIME_ID)
    {
     strcpy(field, seq->separator_dt.separator);
     strcat(field, seq_src[i]->separator_dt.separator);
     field[Xq_FORMULA_LENGTH - 1] = '\0';
     seq_src[i] = dt_seq_funcs[(dt_seq[i])->dt_seq_id].new
      ((dt_seq[i])->dt_seq_id, field);
     add_dt_seq_to_base(This, seq_src[i]);  
    }
   }
   *pos = oj;
  }
  else
  {
   seq_src[i] = dt_seq_funcs[seq->dt_seq_id].copy(seq);
   add_dt_seq_to_base(This, seq_src[i]);
   *pos = oj;
  }
 }
}
