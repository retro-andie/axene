/*
** TextRuler.c for XAllWrite in TextRuler/
** Methods for the TextRuler class
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
** Started on  Sun Jun 12 02:12:36 1994 Stéphane Boisson
** Last update Mon Sep  7 14:53:14 1998 Emmanuel Paris
*/

/*#define NHISTORY*/
/*#define NTRACE*/

#include <varargs.h>
#include "TextRuler.h"
#include "Document.h"

/*--- Prototypes ---*/
static void *contructor();
static void destructor();
static void *copy();
static c_TextRuler *get ___NPROTO((c_TextStyle *ruler, BaseStd_t base, ...));
static void set_params ___NPROTO((c_TextRuler *this, ...));
static boolean compare ___PROTO((c_TextRuler *a, c_TextRuler *b, 
				 boolean with_name));
static void merge ___PROTO((c_TextRuler *source, c_TextRuler **target,
			    boolean fusion));
static c_TextRuler *virtualCopy ___PROTO((c_TextRuler *this, BaseStd_t *base));
static void markUsed ___PROTO((c_TextRuler *This));
static void markUnused ___PROTO((c_TextRuler *This));

static coord_t initGetTab ___PROTO((c_TextRuler *This));
static boolean getNextTab ___PROTO((c_TextRuler *This, coord_t *pos, 
				    tab_just_t *type, char *filler));

static void vset ___PROTO((c_TextRuler *style, va_list ap));
static void hook_inherit ___PROTO((c_TextRuler *this, int reason,
				   c_TextRuler *old, c_TextRuler *new));

sf_TextRuler fc_TextRuler =
{
 contructor, destructor, copy,
 (c_TextRuler *(*) ___PROTO((c_TextRuler *ruler, BaseStd_t *base, ...)))get,
 (void (*) ___PROTO((c_TextRuler *this, ...))) set_params,
 compare, merge, virtualCopy,
 markUsed, markUnused,
 initGetTab, getNextTab,
 readRuler, writeRuler, writeRulerList, readRulerList
};

static ItemHandle *i_handle = NULL;
static long i_references = 0;


/* ----------------------------------------------------------------- ** 
** contructor - Constructor method                                   ** 
** ----------------------------------------------------------------- */
static void *contructor(base, name)
c_TextRuler **base;
char *name;
{
 c_TextRuler *p, *truler;

 Xc_HISTORY(("constructor(`%s')", name? name : ""));

 if(i_handle == NULL)
  i_handle = Xc_createItem("TextRuler base", 16, sizeof(c_TextRuler));
 if(i_handle == NULL) return NULL;

 if((p = Xc_new(i_handle)) == NULL) return NULL;
 memset(p, 0, sizeof(c_TextRuler));
 if((p->hook = NEW(c_Hook)(p, 8)) == NULL)
 {
  Xc_delete(i_handle, p);
  return NULL;
 }
 p->f = &fc_TextRuler;
 i_references++;

 /*--- Insert in linked list ---*/
 Xc_TRACE(("base %lx %lx", base, *base));
 if(*base == NULL)
 {
  p->next = p->previous = NULL;
  *base = p;
 }
 else
 {
  truler = *base;
  while(truler->next)
   truler = truler->next;
  
  p->next = NULL;
  p->previous = truler;
  truler->next = p;
 }
 p->base = base;  
 Xc_TRACE(("end link"));
  
 p->is_delayed = FALSE;
 
 if(name != NULL) 
 {
  strncpy(p->name, name, XcMAX_NAME_LENGTH - 1);
  p->hide_flag = FALSE;
 }
 else
 {
  p->name[0] = '\0';
  p->hide_flag = TRUE;  
 }
 /*--- Default values ---*/
 p->left_margin = COORD_ZERO;
 p->right_margin = COORD_ZERO;
 p->paragraph_indent = COORD_ZERO;

 p->tab_count = 0;
 {
  int i;
  for(i = 0; i < XcTR_MAX_TABULATOR; i++)
  {
   p->tab_just[i] = XcTAB_LEFT;
   p->tab_filler[i] = ' ';
   p->tab[i] = COORD_ZERO;
  }
 }
 p->def_tab_size = SCALE_FROM_CENTIMETERS(1.5);

 p->inter_mode = FALSE;
 p->rel_inter_line = COORD_ZERO;
 p->abs_inter_line = COORD_ZERO;
 p->inter_paragraph = SCALE_FROM_POINTS(5);
 p->inter_paragraph &= COORD_FONTSIZE_MASK;

 p->justification = XcTR_LEFT;
 p->word_distribution = SCALE_FROM_COEF(0.8);

 p->used_count = 0;
 Xc_TRACE(("end constructor"));
 return p;
}


/* ----------------------------------------------------------------- ** 
** destructor - Destructor method                                    ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_TextRuler *this;
{
 Xc_HISTORY(("destructor(`%s')", this->name));
 Xc_ASSERT(i_references > 0);

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

 /*--- reinsert to the end ---*/
 if (this->hook->count && !this->is_delayed)
 {
  c_TextRuler *list;
  list = this;
  while (list->next)
  {
   list = list->next;
  }
  list->next = this;
  this->is_delayed = TRUE;
  this->next = NULL;
  return;
 }
 
 /*--- Ruler inheritance hook ---*/
 if(this->inherit_from)
  XcH_UNREGISTER(this->inherit_from, hook_inherit, this);
 
 DELETE(c_Hook)(this->hook);

 Xc_delete(i_handle, this);
 if(--i_references == 0)
 {
  Xc_destroyItem(i_handle);
  i_handle = NULL;
 } 
}


/* ----------------------------------------------------------------- ** 
** copy - Copy method                                                ** 
** ----------------------------------------------------------------- */
static void *copy(this, base)
c_TextRuler *this;
BaseStd_t *base;
{
 c_TextRuler *p, *t;

 Xc_HISTORY(("copy(`%s')", this->name));

 if((p = Xc_new(i_handle)) == NULL) return NULL;
 memcpy(p, this, sizeof(c_TextRuler));
 if((p->hook = NEW(c_Hook)(p, 8)) == NULL)
 {
  Xc_delete(i_handle, p);
  return NULL;
 }
 i_references++;
	
 /*--- Insert copy in linked list ---*/
 if (base->text_ruler_base == NULL)
 {
  p->previous = p->next = NULL;
  base->text_ruler_base = p;
 }
 else
 {
  t = base->text_ruler_base;
  while(t->next)
   t = t->next;
  
  p->next = NULL;
  p->previous = t;
  t->next = p;
 }
 p->base = &(base->text_ruler_base);
 p->used_count = 0;

 if (p->inherit_from)
 {
  p->inherit_from = virtualCopy(this->inherit_from, base);
  XcH_REGISTER(p->inherit_from, hook_inherit, p);
 }
 
 return p;
}


/* ----------------------------------------------------------------- ** 
** virtualCopy - Copy object if it doesn't already exist             ** 
** ----------------------------------------------------------------- */
static c_TextRuler *virtualCopy(this, base)
c_TextRuler *this;
BaseStd_t *base;
{
 c_TextRuler *ptr;

 Xc_HISTORY(("virtualCopy(`%s')", this->name));
 
 if(this->base == &(base->text_ruler_base)) return this;
 
 for(ptr = base->text_ruler_base; ptr != NULL; ptr = ptr->next)
  if(F(this).compare(ptr, this, CMP_WITH_NAME))
  {
   Xc_TRACE(("found in new base"));
   break;
  }
 
 return((ptr == NULL)? COPY(c_TextRuler)(this, base) : ptr);
}

/* ----------------------------------------------------------------- ** 
** Get method: search if there is a similar ruler                    ** 
** or create a new one.                                              ** 
** ----------------------------------------------------------------- */
static c_TextRuler *get(ruler, base, va_alist)
c_TextRuler	*ruler;
BaseStd_t	*base;
va_dcl
{
 c_TextRuler *ptr, *truler;
 va_list ap;

 Xc_HISTORY(("get Ruler"));
  
 if (ruler)
 {
  truler = (c_TextRuler *)COPY(c_TextRuler)(ruler, base);
 }
 else
 {
  truler =  (c_TextRuler *)NEW(c_TextRuler)(&(base->text_ruler_base), NULL);
 }

 va_start(ap);
 vset(truler, ap);
 va_end(ap);

 for(ptr = base->text_ruler_base; ptr != NULL; ptr = ptr->next)
 {
  if((ptr != truler) && F(ptr).compare(ptr, truler, CMP_WITHOUT_NAME))
  {
   DELETE(c_TextRuler)(truler);
   return ptr; 
  }
 }
 return truler;
}

/* ----------------------------------------------------------------- ** 
** set_params - Set parameters for styles                            ** 
** ----------------------------------------------------------------- */
static void set_params(ruler, va_alist)
c_TextRuler *ruler;
va_dcl
{
 va_list ap;
  
 Xc_HISTORY(("set(`%s')", ruler->name));

 va_start(ap);
 vset(ruler, ap);
 va_end(ap);
}

/* ----------------------------------------------------------------- ** 
** set - Set method                                                  ** 
** ----------------------------------------------------------------- */
static void vset(this, ap)
c_TextRuler *this;
va_list ap;
{
 c_TextRuler *truler;
 boolean out_flag;
 char	*name;
 textruler_set_code_t code;
 int i, tabidx;
 coord_t pos;

 Xc_HISTORY(("set(`%s')", this->name));

 out_flag = FALSE;
 do 
 {
  code = va_arg(ap, textruler_set_code_t);
  switch(code)
  {
  case XcTR_END:
   out_flag = TRUE;
   break;

  case XcTR_NAME:
   Xc_TRACE(("name"));
   name = va_arg(ap, char *);
   if (name == NULL)
   {
    this->name[0] = '\0';
    this->hide_flag = TRUE;
   }
   else
   {
    strncpy(this->name, name, XcMAX_NAME_LENGTH - 1);
    this->hide_flag = FALSE;
   }
   continue;
	
  case XcTR_LEFT_MARGIN:
   Xc_TRACE(("left margin"));
   this->left_margin = va_arg(ap, coord_t);
   continue;

  case XcTR_RIGHT_MARGIN:
   Xc_TRACE(("right margin"));
   this->right_margin = va_arg(ap, coord_t);
   continue;

  case XcTR_INDENT:
   Xc_TRACE(("indent"));
   this->paragraph_indent = va_arg(ap, coord_t);
   continue;

  case XcTR_REL_INTERLINE:
   Xc_TRACE(("rel_interline"));
   this->rel_inter_line = va_arg(ap, coord_t);
   continue;

  case XcTR_ABS_INTERLINE:
   Xc_TRACE(("abs interline"));
   this->abs_inter_line = va_arg(ap, coord_t);
   continue;

  case XcTR_INTER_PARAGRAPH:
   Xc_TRACE(("inter paragraph"));
   this->inter_paragraph = va_arg(ap, coord_t);
   continue;

  case XcTR_JUSTIFICATION:
   Xc_TRACE(("justification"));
   this->justification = va_arg(ap, textruler_just_t);
   continue;

  case XcTR_WORD_DISTRIBUTION:
   this->word_distribution = (scale_t)va_arg(ap, double);
   Xc_TRACE(("distribution %g", SCALE_TO_COEF(this->word_distribution)));
   continue;

  case XcTR_INTERLINE_MODE:
   this->inter_mode = (boolean)va_arg(ap, int);
   continue;

  case XcTR_INTERLINE:
   if (this->inter_mode == TRUE)
    this->abs_inter_line = va_arg(ap, coord_t);
   else
    this->rel_inter_line = va_arg(ap, coord_t);
   continue;

  case XcTR_ADD_TAB:
   pos = va_arg(ap, coord_t);
   if (this->tab_count >= XcTR_MAX_TABULATOR)
   {
    tab_just_t dummy;
    char dummyf;

    dummy = va_arg(ap, tab_just_t);
    dummyf = va_arg(ap, char);
    break;
   }
   i = 0;
   while(i < (int)this->tab_count && this->tab[i] < pos)
    i++;
   if (i != (int)this->tab_count)
   {
    memmove(this->tab + i + 1, this->tab + i, 
	    sizeof(coord_t) * (this->tab_count - i));
    memmove(this->tab_just + i + 1, this->tab_just + i, 
	    sizeof(tab_just_t) * (this->tab_count - i));
    memmove(this->tab_filler + i + 1, this->tab_filler + i, 
	    sizeof(char) * (this->tab_count - i));
   }
   this->tab[i] = pos;
   this->tab_just[i] = va_arg(ap, tab_just_t);
   this->tab_filler[i] = va_arg(ap, char);
   this->tab_count++;
   break;

  case XcTR_MOVE_TAB:
   {
    tab_just_t ttype;
    char       tfiller;
    
    tabidx = va_arg(ap, int);
    pos = va_arg(ap, int);
    ttype = this->tab_just[tabidx];
    tfiller = this->tab_filler[tabidx];
    
    i = 0;
    while(i < (int)this->tab_count && this->tab[i] < pos)
     i++;
    i--;

    if (tabidx != i)
    {
     if (tabidx > i)
     {
      i++;
      memmove(this->tab + i + 1, this->tab + i,
	      sizeof(coord_t) * (tabidx - i));
      memmove(this->tab_just + i  + 1, this->tab_just + i,
	      sizeof(tab_just_t) * (tabidx - i));
      memmove(this->tab_filler + i  + 1, this->tab_filler + i,
	      sizeof(char) * (tabidx - i));
     }
     else
     {
      memmove(this->tab + tabidx, this->tab + tabidx + 1,
	      sizeof(coord_t) * (i - tabidx));
      memmove(this->tab_just + tabidx, this->tab_just + tabidx + 1,
	      sizeof(tab_just_t) * (i - tabidx));
      memmove(this->tab_filler + tabidx, this->tab_filler + tabidx + 1,
	      sizeof(char) * (i - tabidx));
     }
    }
    
    this->tab[i] = pos;
    this->tab_just[i] = ttype;
    this->tab_filler[i] = tfiller;
   }
   break;
  case XcTR_CHANGE_TAB_TYPE:
   tabidx = va_arg(ap, int);
   this->tab_just[tabidx] = va_arg(ap, tab_just_t);
   break;
  case XcTR_CHANGE_TAB_FILLER:
   tabidx = va_arg(ap, int);
   this->tab_filler[tabidx] = va_arg(ap, char);
   break;

  case XcTR_DEL_TAB:
   tabidx = va_arg(ap, int);
   if (tabidx < (int)this->tab_count - 1)
   {
    memmove(this->tab + tabidx, this->tab + tabidx + 1, 
	    sizeof(coord_t) * (this->tab_count - tabidx - 1));
    memmove(this->tab_just + tabidx, this->tab_just + tabidx + 1, 
	    sizeof(tab_just_t) * (this->tab_count - tabidx - 1));
    memmove(this->tab_filler + tabidx, this->tab_filler + tabidx + 1, 
	    sizeof(char) * (this->tab_count - tabidx - 1));
  }
   this->tab_count--;
   break;
   
  case XcTR_HIDE_ON:
   Xc_TRACE(("XcTR_HIDE_ON"));
   this->hide_flag = TRUE;
   break;
      
  case XcTR_HIDE_OFF:
   Xc_TRACE(("XcTR_HIDE_OFF"));
   this->hide_flag = FALSE;
   break;
	
  case XcTR_INHERIT:
   Xc_TRACE(("XcTR_INHERIT"));
   truler = (c_TextRuler *)va_arg(ap, c_TextRuler *);
   if (!this->inherit_from)
   {
    if (truler->inherit_from)
     this->inherit_from = truler->inherit_from;
    else
     this->inherit_from = truler;
    XcH_REGISTER(this->inherit_from, hook_inherit, this);
   }
   this->valid_field |= (unsigned int)va_arg(ap, unsigned int);
   break;

  default:
   Xc_BREAK(("Unknow textruler-set-code"));
  }
 } while(!out_flag);
 
 if (this->name[0] == '\0')
 {		   /* generate a formated name for unnamed ruler */
  if (this->inherit_from)
  {
   strncpy(this->name, this->inherit_from->name, XcMAX_NAME_LENGTH - 8);
   sprintf(this->name + MIN(strlen(this->inherit_from->name),
			       XcMAX_NAME_LENGTH - 8),
	   "_[%x]", this->valid_field);
  }
  else
   strcpy(this->name, "generate");
 }
 
 F(this->hook).callback(this->hook, XcH_CONTENT_CHANGED, this);
}


/* ----------------------------------------------------------------- ** 
** compare - Compare two TextRuler objects                           ** 
** ----------------------------------------------------------------- */
static boolean compare(a, b, with_name)
c_TextRuler *a;
c_TextRuler *b;
boolean	with_name;
{
 if (a == b) return TRUE;
 
 if ((a == NULL) || (b == NULL)) return FALSE;
 if (with_name &&
     !(a->hide_flag || b->hide_flag || Xstrcmp(a->name, b->name)))
  return FALSE; 
 
 if((a->left_margin == b->left_margin) &&
    (a->right_margin == b->right_margin) &&
    (a->paragraph_indent == b->paragraph_indent) &&
    (a->tab_count == b->tab_count) &&
    !memcmp(a->tab, b->tab, a->tab_count * sizeof(coord_t)) &&
    !memcmp(a->tab_just, b->tab_just, a->tab_count * sizeof(tab_just_t)) &&
    !memcmp(a->tab_filler, b->tab_filler, a->tab_count * sizeof(char)) &&
    (a->def_tab_size == b->def_tab_size) &&
    (a->inter_mode == b->inter_mode) &&
    (a->rel_inter_line == b->rel_inter_line) &&
    (a->abs_inter_line == b->abs_inter_line) &&
    (a->inter_paragraph == b->inter_paragraph) &&
    (a->justification == b->justification) &&
    (a->word_distribution != b->word_distribution))
  return TRUE;
    
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** merge - Merge two ruler list                                      ** 
** ----------------------------------------------------------------- */
static void merge(source, target, fusion_flag)
c_TextRuler *source;
c_TextRuler **target;
boolean fusion_flag;
{
 c_TextRuler *ptr, *base;

 Xc_HISTORY(("merge"));

 source = *source->base;
 base = *target;
 for(ptr = base; ptr != NULL; ptr = ptr->next) ptr->id = -1;
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
    DELETE(c_TextRuler)(ptr);
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
** markUsed : increment used counter                                 ** 
** ----------------------------------------------------------------- */
static void markUsed(This)
c_TextRuler *This;
{
 This->used_count++;
}

/* ----------------------------------------------------------------- ** 
** markUnused : decrement used counter                               ** 
** ----------------------------------------------------------------- */
static void markUnused(This)
c_TextRuler *This;
{
 This->used_count--;
 if (This->used_count <= 0)
 {
  if (This->hide_flag && !This->lock_flag)
  {
   DELETE(c_TextRuler)(This);
  }
  else
   This->used_count = 0;
 }
}

/* ----------------------------------------------------------------- ** 
** initGetTab : initialize Ruler for getNextTab                      ** 
** ----------------------------------------------------------------- */
static coord_t initGetTab(This)
c_TextRuler *This;
{
 This->tab_tmp_idx = 0;
 if (This->tab_count)  /* && This->tab[0] < COORD_ZERO) */
  return This->tab[0];
 else
 {
  coord_t tmp;
  
  tmp = ((int)floor((double)This->left_margin / This->def_tab_size) + 1) * 
   This->def_tab_size;
  This->tab_tmp_pos = tmp - COORD_ONE;
  return tmp;
 }
}

/* ----------------------------------------------------------------- ** 
** getNextTab : get the next tablulation.                            ** 
** ----------------------------------------------------------------- */
static boolean getNextTab(This, pos, type, filler)
c_TextRuler *This;
coord_t *pos;
tab_just_t *type;
char *filler;
{
 while(This->tab_tmp_idx < (int)This->tab_count &&
       This->tab[This->tab_tmp_idx] < *pos)
  This->tab_tmp_idx++;
 
 if (This->tab_tmp_idx < (int)This->tab_count)
 {
  This->tab_tmp_pos = *pos = This->tab[This->tab_tmp_idx];
  *type = This->tab_just[This->tab_tmp_idx];
  *filler = This->tab_filler[This->tab_tmp_idx++];
  return TRUE;
 }
 
 *pos = ((int)floor((double)*pos / This->def_tab_size) + 1) 
  * This->def_tab_size;
 This->tab_tmp_pos = *pos;
 *type = XcTAB_LEFT;
 *filler = ' ';
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** hook_inherit - TextRuler inherit hook                             ** 
** ----------------------------------------------------------------- */
static void hook_inherit(this, reason, old, new)
c_TextRuler	*this;
int		reason;
c_TextRuler	*old;
c_TextRuler	*new;
{
 Xc_TRACE(("hook_inherit"));

 if((reason == XcH_CONTENT_CHANGED) || (reason == XcH_REDRAW))
 {
  /*--- new left margin ? ---*/
  if (!(this->valid_field & XcTR_H_LEFT_MARGIN) &&
      this->left_margin != new->left_margin)
   F(this).set(this, XcTR_LEFT_MARGIN, new->left_margin, XcTS_END);

  /*--- new right margin ? ---*/
  if (!(this->valid_field & XcTR_H_RIGHT_MARGIN) &&
      this->right_margin != new->right_margin)
   F(this).set(this, XcTR_RIGHT_MARGIN, new->right_margin, XcTS_END);

  /*--- new paragraph indent ? ---*/
  if (!(this->valid_field & XcTR_H_INDENT) &&
      this->paragraph_indent != new->paragraph_indent)
   F(this).set(this, XcTR_INDENT, new->paragraph_indent, XcTS_END);

  /*--- new interline mode ? ---*/
  if (!(this->valid_field & XcTR_H_INTERLINE_MODE) &&
      this->inter_mode != new->inter_mode)
   F(this).set(this, XcTR_INTERLINE_MODE, new->inter_mode, XcTS_END);

  /*--- new relative interline ? ---*/
  if (!(this->valid_field & XcTR_H_REL_INTERLINE) &&
      this->rel_inter_line != new->rel_inter_line)
   F(this).set(this, XcTR_REL_INTERLINE, new->rel_inter_line, XcTS_END);
  
  /*--- new absolute interline ? ---*/
  if (!(this->valid_field & XcTR_H_ABS_INTERLINE) &&
      this->abs_inter_line != new->abs_inter_line)
   F(this).set(this, XcTR_ABS_INTERLINE, new->abs_inter_line, XcTS_END);

  /*--- new inter paragraph ? ---*/
  if (!(this->valid_field & XcTR_H_INTER_PARAGRAPH) &&
      this->inter_paragraph != new->inter_paragraph)
   F(this).set(this, XcTR_INTER_PARAGRAPH, new->inter_paragraph, XcTS_END);

  /*--- new justification ? ---*/
  if (!(this->valid_field & XcTR_H_JUSTIFICATION) &&
      this->justification != new->justification)
   F(this).set(this, XcTR_JUSTIFICATION, new->justification, XcTS_END);
  
  /*--- new word distribution ? ---*/
  if (!(this->valid_field & XcTR_H_WORD_DISTRIB) &&
      this->word_distribution != new->word_distribution)
   F(this).set(this, XcTR_WORD_DISTRIBUTION, new->word_distribution, XcTS_END);

  /*--- new tab list ? ---*/
  if (!(this->valid_field & XcTR_H_TAB))
  {
   if ((this->tab_count == new->tab_count) &&
       !memcmp(this->tab, new->tab, this->tab_count * sizeof(coord_t)) &&
       !memcmp(this->tab_just, new->tab_just, this->tab_count * 
	       sizeof(tab_just_t)) &&
       !memcmp(this->tab_filler, new->tab_filler, this->tab_count * 
	       sizeof(char)))
   {
    this->tab_count = new->tab_count;
    memcpy(this->tab, new->tab, this->tab_count * sizeof(coord_t));
    memcpy(this->tab_just, new->tab_just, this->tab_count * 
	   sizeof(tab_just_t));
    memcpy(this->tab_filler, new->tab_filler, this->tab_count * sizeof(char));
    F(this->hook).callback(this->hook, XcH_CONTENT_CHANGED, this);
   }
  }
 }
}
