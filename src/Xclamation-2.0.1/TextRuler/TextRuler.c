/*
** TextRuler.c for Xclamation in TextRuler/
** Methods for the TextRuler class
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
** Started on  Sun Jun 12 02:12:36 1994 St�phane Boisson
** Last update Wed Apr 16 17:08:36 1997 Emmanuel Paris
*/

#define NHISTORY
#define NTRACE

#include <stdarg.h>
#include "TextRuler.h"
#include "Document.h"

/*--- Prototypes ---*/
static void *contructor();
static void destructor();
static void *copy();
static void set ___PROTO((c_TextRuler *this, ...));
static boolean compare ___PROTO((c_TextRuler *a, c_TextRuler *b));
static void merge ___PROTO((c_TextRuler *source, c_TextRuler **target,
			    boolean fusion));
static c_TextRuler *virtualCopy ___PROTO((c_TextRuler *this, BaseStd_t *base));

static boolean readRuler ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_TextRuler **base));
static boolean readRulerList ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_TextRuler **base));
static boolean writeRuler ___PROTO((c_TextRuler *this, c_DocFile *doc));
static boolean writeRulerList ___PROTO((c_TextRuler *this, c_DocFile *doc));
static boolean cb_left ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_TextRuler *ruler));
static boolean cb_right ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_TextRuler *ruler));
static boolean cb_center ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_TextRuler *ruler));
static boolean cb_justified ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_TextRuler *ruler));
static boolean cb_name ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_TextRuler *this));


sf_TextRuler fc_TextRuler =
{
  contructor, destructor, copy,
  (void (*) ___PROTO((c_TextRuler *this, ...))) set,
  compare, merge, virtualCopy,
  readRuler, writeRuler, writeRulerList, readRulerList
};

static ItemHandle *i_handle = NULL;
static long i_references = 0;


/* ----------------------------------------------------------------- ** 
** contructor - Constructor method                               ** 
** ----------------------------------------------------------------- */
static void *contructor(base, name)
c_TextRuler **base;
char *name;
{
  c_TextRuler *p;

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
    }
  else
    {
      p->next = *base;
      p->previous = NULL;
      (*base)->previous = p;
    }
  *base = p;
  p->base = base;  
  Xc_TRACE(("end link"));
  
  if(name != NULL) strncpy(p->name, name, XcMAX_NAME_LENGTH - 1);

  /*--- Default values ---*/
  p->left_margin = 0;
  p->right_margin = 0;
  p->paragraph_indent = 0;


  p->inter_mode = FALSE;
  p->inter_line = 0;
  p->inter_paragraph = SCALE_FROM_POINTS(5);
  p->inter_paragraph &= COORD_FONTSIZE_MASK;

  p->justification = XcTR_LEFT;
  p->word_distribution = SCALE_FROM_COEF(0.8);
  p->min_ratio = SCALE_FROM_COEF(0.75);
  p->max_ratio = SCALE_FROM_COEF(1);

  Xc_TRACE(("end constructor"));
  return p;
}


/* ----------------------------------------------------------------- ** 
** destructor - Destructor method                                ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_TextRuler *this;
{
  Xc_HISTORY(("destructor(`%s')", this->name));
  Xc_ASSERT(i_references > 0);

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

  Xc_delete(i_handle, this);
  if(--i_references == 0)
    {
      Xc_destroyItem(i_handle);
      i_handle = NULL;
    } 
}


/* ----------------------------------------------------------------- ** 
** copy - Copy method                                      ** 
** ----------------------------------------------------------------- */
static void *copy(this, base)
c_TextRuler *this;
c_TextRuler **base;
{
  c_TextRuler *p;

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
  if(base == NULL) base = this->base;
  if(*base != NULL) (*base)->previous = p;
  p->next = *base;
  p->previous = NULL;
  *base = p;
  p->base = base;
       
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
    if(F(this).compare(ptr, this))
      {
	Xc_TRACE(("found in new base"));
	break;
      }

  return((ptr == NULL)?
	 COPY(c_TextRuler)(this, &(base->text_ruler_base)) : ptr);
}


/* ----------------------------------------------------------------- ** 
** set - Set method                                                  ** 
** ----------------------------------------------------------------- */
static void set(c_TextRuler *this, ...)
{
  boolean out_flag;
  va_list ap;

  Xc_HISTORY(("set(`%s')", this->name));

  out_flag = FALSE;
  va_start(ap, this);
  do {
    switch(va_arg(ap, textruler_set_code_t))
      {
      case XcTR_END:
	out_flag = TRUE;
	break;

      case XcTR_NAME:
	Xc_TRACE(("name"));
        strncpy(this->name, va_arg(ap, char *), XcMAX_NAME_LENGTH - 1);
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
	this->inter_mode = FALSE;
	this->inter_line = va_arg(ap, coord_t);
	continue;

      case XcTR_ABS_INTERLINE:
	Xc_TRACE(("abs interline"));
	this->inter_mode = TRUE;
	this->inter_line = va_arg(ap, coord_t);
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
	/*this->word_distribution = va_arg(ap, scale_t);*/
	this->word_distribution = (scale_t)va_arg(ap, double);
	Xc_TRACE(("distribution %g", SCALE_TO_COEF(this->word_distribution)));
	continue;

      case XcTR_MIN_RATIO:
	Xc_TRACE(("min ratio"));
	/*this->min_ratio = va_arg(ap, scale_t);*/
	this->min_ratio = (scale_t) va_arg(ap, double);
	continue;

      case XcTR_MAX_RATIO:
	Xc_TRACE(("max ratio"));
	/*this->max_ratio = va_arg(ap, scale_t);*/
	this->max_ratio = (scale_t)va_arg(ap, double);
	continue;

      case XcTR_INTERLINE_MODE:
	this->inter_mode = (boolean)va_arg(ap, int);
	continue;

      case XcTR_INTERLINE:
/*Xc_ASSERT(((this->inter_mode == TRUE) &&*/
/*(this->inter_line > COORD_ZERO)) ||*/
/*(this->inter_line >= COORD_ZERO));*/ /* hehehe !! */
	this->inter_line = va_arg(ap, coord_t);
	continue;

      default:
        Xc_BREAK(("Unknow textruler-set-code"));
      }
  } while(!out_flag);
  va_end(ap);
  F(this->hook).callback(this->hook, XcH_CONTENT_CHANGED, this);
  F(this->hook).callback(this->hook, XcH_REDRAW, this);
}


/* ----------------------------------------------------------------- ** 
** compare - Compare two TextRuler objects                           ** 
** ----------------------------------------------------------------- */
static boolean compare(a, b)
c_TextRuler *a;
c_TextRuler *b;
{
  Xc_TRACE(("compare(`%s', `%s')", a->name, b->name));

  if(!strcmp(a->name, b->name) &&
     (a->left_margin == b->left_margin) &&
     (a->right_margin == b->right_margin) &&
     (a->paragraph_indent == b->paragraph_indent) &&
     (a->tab_count == b->tab_count) &&
     !memcmp(a->tab, b->tab, a->tab_count * sizeof(coord_t)) &&
     (a->inter_mode == b->inter_mode) &&
     (a->inter_line == b->inter_line) &&
     (a->inter_paragraph == b->inter_paragraph) &&
     (a->justification == b->justification) &&
     (a->min_ratio == b->min_ratio))
    {
     if((a->justification == XcTR_JUSTIFIED) &&
	((a->word_distribution != b->word_distribution) ||
	 (a->max_ratio != b->max_ratio))) return FALSE;
     return TRUE;
   }
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
	    if(F(source).compare(source, ptr)) break;
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
** readRulerList - TextRuler loading                                 ** 
** ----------------------------------------------------------------- */
static boolean readRulerList(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler **base;
{
  Xc_TRACE(("readRulerList"));

  if(!F(doc).addCallbacks(doc, XcDF_RULER_KEYWORD, readRuler, base, NULL))
    return FALSE;
  return F(doc).expectKeyword(doc, XcDF_RULER_KEYWORD, TRUE);
}


/* ----------------------------------------------------------------- ** 
** readRuler - TextRuler loading                                     ** 
** ----------------------------------------------------------------- */
static boolean readRuler(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler **base;
{
  c_TextRuler *ruler;
  boolean flag;
  real r;

  Xc_TRACE(("readRuler"));

  if((ruler = NEW(c_TextRuler)(base, NULL)) == NULL) return FALSE;
  ruler->id = param;
  do {
    if(!F(doc).addCallbacks(doc,
			    XcDF_LEFT_KEYWORD, cb_left, ruler,
			    XcDF_RIGHT_KEYWORD, cb_right, ruler,
			    XcDF_CENTER_KEYWORD, cb_center, ruler,
			    XcDF_JUSTIFIED_KEYWORD, cb_justified, ruler,
			    XcDF_NAME_KEYWORD, cb_name, ruler,
			    NULL)) break;
    if(!F(doc).readReal(doc, &r)) break;
    F(ruler).set(ruler, XcTR_LEFT_MARGIN, SCALE_FROM_POINTS(r), XcTR_END); 
    if(!F(doc).readReal(doc, &r)) break;
    F(ruler).set(ruler, XcTR_RIGHT_MARGIN, SCALE_FROM_POINTS(r), XcTR_END); 
    if(!F(doc).readReal(doc, &r)) break;
    F(ruler).set(ruler, XcTR_INDENT, SCALE_FROM_POINTS(r), XcTR_END); 

    if(!F(doc).readBoolean(doc, &flag)) break;
    if(!F(doc).readReal(doc, &r)) break;
    F(ruler).set(ruler, flag? XcTR_ABS_INTERLINE : XcTR_REL_INTERLINE,
		 SCALE_FROM_POINTS(r), XcTR_END); 

    if(!F(doc).readReal(doc, &r)) break;
    F(ruler).set(ruler, XcTR_INTER_PARAGRAPH, SCALE_FROM_POINTS(r), XcTR_END);

    if(!F(doc).readReal(doc, &r)) break;
    Xc_TRACE(("real distribution = %g", SCALE_TO_COEF(r)));

    F(ruler).set(ruler, XcTR_WORD_DISTRIBUTION, SCALE_FROM_COEF(r), XcTR_END); 
    if(!F(doc).readReal(doc, &r)) break;
    F(ruler).set(ruler, XcTR_MIN_RATIO, SCALE_FROM_COEF(r), XcTR_END); 
    if(!F(doc).readReal(doc, &r)) break;
    F(ruler).set(ruler, XcTR_MAX_RATIO, SCALE_FROM_COEF(r), XcTR_END);

    if(!F(doc).expectKeyword(doc, NULL, TRUE)) break;
    return TRUE;
  } while(0);
  DELETE(c_TextRuler)(ruler);
  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_name - Name keyword loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_name(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *this;
{
  unsigned char *name;

  Xc_TRACE(("cb_name"));

  if(!F(doc).readString(doc, &name)) return FALSE;
  if(name != NULL)
    {
      F(this).set(this, XcTR_NAME, name, XcTR_END);
      Xc_free(name);
    }
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_left - Justification loading callback                          ** 
** ----------------------------------------------------------------- */
static boolean cb_left(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
  Xc_TRACE(("cb_left"));

  F(ruler).set(ruler, XcTR_JUSTIFICATION, XcTR_LEFT, XcTR_END);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_right - Right justification loading callback                   ** 
** ----------------------------------------------------------------- */
static boolean cb_right(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
  Xc_TRACE(("cb_right"));

  F(ruler).set(ruler, XcTR_JUSTIFICATION, XcTR_RIGHT, XcTR_END);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_center - Center justification loading callback                 ** 
** ----------------------------------------------------------------- */
static boolean cb_center(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
  Xc_TRACE(("cb_center"));

  F(ruler).set(ruler, XcTR_JUSTIFICATION, XcTR_CENTER, XcTR_END);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_justified - Justification loading callback                     ** 
** ----------------------------------------------------------------- */
static boolean cb_justified(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
  Xc_TRACE(("cb_justified"));

  F(ruler).set(ruler, XcTR_JUSTIFICATION, XcTR_JUSTIFIED, XcTR_END);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** writeRuler - TextRuler saving                                     ** 
** ----------------------------------------------------------------- */
static boolean writeRuler(this, doc)
c_TextRuler *this;
c_DocFile *doc;
{
  Xc_HISTORY(("write(`%s')", this->name));

  if(!F(doc).writeKeyword(doc, XcDF_RULER_KEYWORD, this->id) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->left_margin)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->right_margin)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->paragraph_indent)) ||
     !F(doc).writeBoolean(doc, this->inter_mode) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->inter_line)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->inter_paragraph)))
    return FALSE;
  
  if(((this->justification == XcTR_LEFT) &&
      !F(doc).writeKeyword(doc, XcDF_LEFT_KEYWORD, -1L)) ||
     ((this->justification == XcTR_RIGHT) &&
      !F(doc).writeKeyword(doc, XcDF_RIGHT_KEYWORD, -1L)) ||
     ((this->justification == XcTR_CENTER) &&
      !F(doc).writeKeyword(doc, XcDF_CENTER_KEYWORD, -1L)) ||
     ((this->justification == XcTR_JUSTIFIED) &&
      !F(doc).writeKeyword(doc, XcDF_JUSTIFIED_KEYWORD, -1L))) return FALSE;
  
  if(!F(doc).writeReal(doc, SCALE_TO_COEF(this->word_distribution)) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->min_ratio)) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->max_ratio))) return FALSE;

  return (F(doc).writeKeyword(doc, XcDF_NAME_KEYWORD, -1L) &&
	  F(doc).startSequence(doc) &&
	  F(doc).write(doc, this->name, strlen(this->name)) &&
	  F(doc).endSequence(doc) &&
	  F(doc).endSequence(doc));
}


/* ----------------------------------------------------------------- ** 
** writeRulerList - Write a TextRuler list                           ** 
** ----------------------------------------------------------------- */
static boolean writeRulerList(this, doc)
c_TextRuler *this;
c_DocFile *doc;
{
  c_TextRuler *ptr;
  long id;

  Xc_HISTORY(("writeList"));

  if(!F(doc).writeKeyword(doc, XcDF_RULERLIST_KEYWORD, -1L) ||
     !F(doc).startSequence(doc)) return FALSE;
  for(ptr = *this->base, id = 0; ptr != NULL; ptr = ptr->next, id++)
    {
      ptr->id = id;
      if(!F(ptr).write(ptr, doc)) return FALSE;
    }
  return F(doc).endSequence(doc);
}
