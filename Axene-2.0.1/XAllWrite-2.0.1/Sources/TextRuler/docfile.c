/*
** docfile.c for XAllWrite in TextRuler/
** Text ruler read and write functions.
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Mon Sep  1 17:04:20 1997 Emmanuel Paris
** Last update Fri Oct  2 16:51:02 1998 Robin Castanier
*/

#include "TextRuler.h"

boolean readRuler ___PROTO((c_DocFile *doc, char *keyword,
			    long param, c_TextRuler **base));
boolean readRulerList ___PROTO((c_DocFile *doc, char *keyword,
				long param, c_TextRuler **base));
boolean writeRuler ___PROTO((c_TextRuler *this, c_DocFile *doc));
boolean writeRulerList ___PROTO((c_TextRuler *this, c_DocFile *doc));

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
static boolean cb_tablist ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_TextRuler *ruler));
static boolean cb_tab ___PROTO((c_DocFile *doc, char *keyword,
				long param, c_TextRuler *ruler));
static boolean cb_tab_left ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_TextRuler *ruler));
static boolean cb_tab_right ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_TextRuler *ruler));
static boolean cb_tab_center ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_TextRuler *ruler));
static boolean cb_tab_cperiod ___PROTO((c_DocFile *doc, char *keyword,
					long param, c_TextRuler *ruler));
static boolean cb_tab_filler ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_TextRuler *ruler));
static boolean cb_tabf_point ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_TextRuler *ruler));
static boolean cb_tabf_dot ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_TextRuler *ruler));
static boolean cb_tabf_hyphen ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_TextRuler *ruler));
static boolean cb_tabf_user ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_TextRuler *ruler));
static boolean cb_hide ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_TextRuler *ruler));
static boolean cb_inherit ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_TextRuler *this));
static boolean cb_ruler ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_TextRuler *ruler));

/* ----------------------------------------------------------------- ** 
** writeRuler - TextRuler saving                                     ** 
** ----------------------------------------------------------------- */
boolean writeRuler(this, doc)
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
    !F(doc).writeReal(doc, SCALE_TO_POINTS(this->rel_inter_line)) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(this->abs_inter_line)) ||
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
  
 if(!F(doc).writeReal(doc, SCALE_TO_COEF(this->word_distribution)))
  return FALSE;

 if (!F(doc).writeKeyword(doc, XcDF_NAME_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).write(doc, this->name, strlen(this->name)) ||
     !F(doc).endSequence(doc)) return FALSE;

 if (this->tab_count != 0 || this->def_tab_size != SCALE_FROM_CENTIMETERS(1.5))
 {
  int i;

  if (!F(doc).writeKeyword(doc, XcDF_TABLIST_KEYWORD, -1L) ||
      !F(doc).startSequence(doc)) return FALSE;
  
  if (!F(doc).writeReal(doc, SCALE_TO_POINTS(this->def_tab_size)))
   return FALSE;
  
  for(i = 0; i < (int)this->tab_count; i++)
  {
   if (!F(doc).writeKeyword(doc, XcDF_TAB_KEYWORD, -1L) ||
       !F(doc).startSequence(doc) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(this->tab[i])))
    return FALSE;
   
   switch(this->tab_just[i])
   {
   case XcTAB_LEFT:
   default:
    if (!F(doc).writeKeyword(doc, XcDF_LEFT_KEYWORD, -1L)) return FALSE;
    break;
   case XcTAB_RIGHT:
    if (!F(doc).writeKeyword(doc, XcDF_RIGHT_KEYWORD, -1L)) return FALSE;
   break;
   case XcTAB_CENTER:
    if (!F(doc).writeKeyword(doc, XcDF_CENTER_KEYWORD, -1L)) return FALSE;
    break;
   case XcTAB_CPERIOD:
    if (!F(doc).writeKeyword(doc, XcDF_CPERIOD_KEYWORD, -1L)) return FALSE;
    break;
   }
   
   switch((unsigned int)this->tab_filler[i])
   {
   case (int)'.':
    if (!F(doc).writeKeyword(doc, XcDF_TAB_FILLER_KEYWORD, -1L) ||
	!F(doc).startSequence(doc) ||
	!F(doc).writeKeyword(doc, XcDF_TABF_POINT_KEYWORD, -1L) ||
	!F(doc).endSequence(doc)) return FALSE;
    break;
   case (int)183:	/* dot */
    if (!F(doc).writeKeyword(doc, XcDF_TAB_FILLER_KEYWORD, -1L) ||
	!F(doc).startSequence(doc) ||
	!F(doc).writeKeyword(doc, XcDF_TABF_DOT_KEYWORD, -1L) ||
	!F(doc).endSequence(doc)) return FALSE;
    break;
   case (int)173:	/* hyphent */
    if (!F(doc).writeKeyword(doc, XcDF_TAB_FILLER_KEYWORD, -1L) ||
	!F(doc).startSequence(doc) ||
	!F(doc).writeKeyword(doc, XcDF_TABF_HYPHEN_KEYWORD, -1L) ||
	!F(doc).endSequence(doc)) return FALSE;
    break;
   default:
    if ((unsigned int)this->tab_filler[i] > 32)
    {
     if (!F(doc).writeKeyword(doc, XcDF_TAB_FILLER_KEYWORD, -1L) ||
	 !F(doc).startSequence(doc) ||
	 !F(doc).writeKeyword(doc, XcDF_TABF_USER_KEYWORD, -1L) ||
	 !F(doc).startSequence(doc) ||
	 !F(doc).write(doc, &(this->tab_filler[i]), 1) ||
	 !F(doc).endSequence(doc) ||
	 !F(doc).endSequence(doc)) return FALSE;
    }
    break;
   }
   if (!F(doc).endSequence(doc)) return FALSE;
  }
  
  if (!F(doc).endSequence(doc)) return FALSE;
 }
 /*--- inheritance ---*/
 if (this->inherit_from)
 {
  if((!F(doc).writeKeyword(doc, XcDF_INHERIT_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !F(doc).writeKeyword(doc, XcDF_RULER_KEYWORD, this->inherit_from->id) ||
      !F(doc).writeInteger(doc, this->valid_field) ||
      !F(doc).endSequence(doc))) return FALSE;
 }
 
 if(this->hide_flag != FALSE)
  if(!F(doc).writeKeyword(doc, XcDF_HIDE_KEYWORD, -1L)) return FALSE;
  
 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** writeRulerList - Write a TextRuler list                           ** 
** ----------------------------------------------------------------- */
boolean writeRulerList(this, doc)
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
  if (ptr->inherit_from != NULL) continue;
  ptr->id = id;
  if(!F(ptr).write(ptr, doc)) return FALSE;
 }
 for(ptr = *this->base; ptr != NULL; ptr = ptr->next, id++)
 {
  if (ptr->inherit_from == NULL) continue;
  ptr->id = id;
  if(!F(ptr).write(ptr, doc)) return FALSE;
 }
 return F(doc).endSequence(doc);
}

/* ----------------------------------------------------------------- ** 
** readRulerList - TextRuler loading                                 ** 
** ----------------------------------------------------------------- */
boolean readRulerList(doc, keyword, param, base)
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
boolean readRuler(doc, keyword, param, base)
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
 do 
 {
  if(!F(doc).addCallbacks(doc,
			  XcDF_LEFT_KEYWORD, cb_left, ruler,
			  XcDF_RIGHT_KEYWORD, cb_right, ruler,
			  XcDF_CENTER_KEYWORD, cb_center, ruler,
			  XcDF_JUSTIFIED_KEYWORD, cb_justified, ruler,
			  XcDF_NAME_KEYWORD, cb_name, ruler,
			  XcDF_RULER_KEYWORD, cb_ruler, ruler,
			  XcDF_TABLIST_KEYWORD, cb_tablist, ruler,
			  XcDF_HIDE_KEYWORD, cb_hide, ruler,
			  XcDF_INHERIT_KEYWORD, cb_inherit, ruler,
			  NULL)) break;
  if(!F(doc).readReal(doc, &r)) break;
  F(ruler).set(ruler, XcTR_LEFT_MARGIN, SCALE_FROM_POINTS(r), XcTR_END); 
  if(!F(doc).readReal(doc, &r)) break;
  F(ruler).set(ruler, XcTR_RIGHT_MARGIN, SCALE_FROM_POINTS(r), XcTR_END); 
  if(!F(doc).readReal(doc, &r)) break;
  F(ruler).set(ruler, XcTR_INDENT, SCALE_FROM_POINTS(r), XcTR_END); 
  
  if(!F(doc).readBoolean(doc, &flag)) break;
  F(ruler).set(ruler, XcTR_INTERLINE_MODE, flag, XcTR_END);
  
  if(!F(doc).readReal(doc, &r)) break;
  F(ruler).set(ruler, XcTR_REL_INTERLINE, SCALE_FROM_POINTS(r), XcTR_END); 
  if(!F(doc).readReal(doc, &r)) break;
  F(ruler).set(ruler, XcTR_ABS_INTERLINE, SCALE_FROM_POINTS(r), XcTR_END); 
  if(!F(doc).readReal(doc, &r)) break;
  F(ruler).set(ruler, XcTR_INTER_PARAGRAPH, SCALE_FROM_POINTS(r), XcTR_END);
  
  if(!F(doc).readReal(doc, &r)) break;
  Xc_TRACE(("real distribution = %g", SCALE_TO_COEF(r)));
  
  F(ruler).set(ruler, XcTR_WORD_DISTRIBUTION, SCALE_FROM_COEF(r), XcTR_END); 
  
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
** cb_hide - Hide loading callback                                   ** 
** ----------------------------------------------------------------- */
static boolean cb_hide(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 Xc_TRACE(("cb_hide"));
 F(ruler).set(ruler, XcTR_HIDE_ON, XcTR_END);
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_inherit - Inheritance Loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_inherit(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 long	is_valid;
 Xc_TRACE(("cb_hinerit"));
 
 if(!F(doc).readInteger(doc, &is_valid)) return FALSE;
 
 if(doc->data_type == XcDF_RULER_TYPE)
 {
  F(ruler).set(ruler, XcTR_INHERIT, (c_TextRuler *)doc->data_ptr,
	       (unsigned int)is_valid, XcTR_END);
  doc->data_type = XcDF_NONE;
  doc->data_ptr = NULL;
 }
 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_ruler - Ruler loading callback                                 ** 
** ----------------------------------------------------------------- */
static boolean cb_ruler(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 c_TextRuler *ptr;
 
 Xc_TRACE(("cb_ruler(%ld)", param));
  
 if(param < 0) param = 0;
 for(ptr = *(ruler->base); ptr != NULL; ptr = ptr->next)
 {
  if(ptr->id == param)
  {
   doc->data_type = XcDF_RULER_TYPE;
   doc->data_ptr = ptr;
   return TRUE;
  }
 }

 Xc_ERROR(("Bad ruler #%ld", param));
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** cb_tablist - Tab List loading callback                            ** 
** ----------------------------------------------------------------- */
static boolean cb_tablist(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 real r;
 
 if(!F(doc).readReal(doc, &r)) return FALSE;
 ruler->def_tab_size = SCALE_FROM_POINTS(r);

 if(!F(doc).addCallbacks(doc, XcDF_TAB_KEYWORD, cb_tab, ruler, NULL))
  return FALSE;

 if(!F(doc).expectKeyword(doc, NULL, TRUE)) return FALSE;

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_tab - Tab loading callback                                     ** 
** ----------------------------------------------------------------- */
static boolean cb_tab(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 real r;
 
 if(!F(doc).readReal(doc, &r)) return FALSE;
 
 F(ruler).set(ruler, XcTR_ADD_TAB, SCALE_FROM_POINTS(r), XcTAB_LEFT, ' ',
	      XcTR_END);
 
 if(!F(doc).addCallbacks(doc,
			 XcDF_LEFT_KEYWORD, cb_tab_left, ruler,
			 XcDF_RIGHT_KEYWORD, cb_tab_right, ruler,
			 XcDF_CENTER_KEYWORD, cb_tab_center, ruler,
			 XcDF_CPERIOD_KEYWORD, cb_tab_cperiod, ruler,
			 XcDF_TAB_FILLER_KEYWORD, cb_tab_filler, ruler,
			 NULL)) return FALSE;
 
 if(!F(doc).expectKeyword(doc, NULL, TRUE)) return FALSE;

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_tab_left - Tab Left loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_tab_left(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 int index = ruler->tab_count - 1;
 F(ruler).set(ruler, XcTR_CHANGE_TAB_TYPE, index, XcTAB_LEFT, XcTR_END); 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_tab_right - Tab Right loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_tab_right(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 int index = ruler->tab_count - 1;
 F(ruler).set(ruler, XcTR_CHANGE_TAB_TYPE, index, XcTAB_RIGHT, XcTR_END); 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_tab_center - Tab Center loading callback                       ** 
** ----------------------------------------------------------------- */
static boolean cb_tab_center(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 int index = ruler->tab_count - 1;
 F(ruler).set(ruler, XcTR_CHANGE_TAB_TYPE, index, XcTAB_CENTER, XcTR_END); 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_tab_cperiod - Tab Cperiod loading callback                     ** 
** ----------------------------------------------------------------- */
static boolean cb_tab_cperiod(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 int index = ruler->tab_count - 1;
 F(ruler).set(ruler, XcTR_CHANGE_TAB_TYPE, index, XcTAB_CPERIOD, XcTR_END); 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_tab_filler - Tab filler loading callback                       ** 
** ----------------------------------------------------------------- */
static boolean cb_tab_filler(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 if(!F(doc).addCallbacks(doc,
			 XcDF_TABF_POINT_KEYWORD, cb_tabf_point, ruler,
			 XcDF_TABF_DOT_KEYWORD, cb_tabf_dot, ruler,
			 XcDF_TABF_HYPHEN_KEYWORD, cb_tabf_hyphen, ruler,
			 XcDF_TABF_USER_KEYWORD, cb_tabf_user, ruler,
			 NULL)) return FALSE;
 
 if(!F(doc).expectKeyword(doc, NULL, TRUE)) return FALSE;
 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_tabf_point - Tab Filler point loading callback                 ** 
** ----------------------------------------------------------------- */
static boolean cb_tabf_point(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 int index = ruler->tab_count - 1;
 F(ruler).set(ruler, XcTR_CHANGE_TAB_FILLER, index, '.', XcTR_END); 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_tabf_dot - Tab Filler dot loading callback                     ** 
** ----------------------------------------------------------------- */
static boolean cb_tabf_dot(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 int index = ruler->tab_count - 1;
 F(ruler).set(ruler, XcTR_CHANGE_TAB_FILLER, index, (char)183, XcTR_END); 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_tabf_hyphen - Tab Filler hyphen loading callback               ** 
** ----------------------------------------------------------------- */
static boolean cb_tabf_hyphen(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 int index = ruler->tab_count - 1;
 F(ruler).set(ruler, XcTR_CHANGE_TAB_FILLER, index, (char)173, XcTR_END); 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_tabf_user - Tab Filler user loading callback                   ** 
** ----------------------------------------------------------------- */
static boolean cb_tabf_user(doc, keyword, param, ruler)
c_DocFile *doc;
char *keyword;
long param;
c_TextRuler *ruler;
{
 int index = ruler->tab_count - 1;
 unsigned char *filler;
 
 if (!F(doc).readString(doc, &filler)) return FALSE;
 
 if (filler != NULL && strlen(filler) == 1)
  F(ruler).set(ruler, XcTR_CHANGE_TAB_FILLER, index, (char)*filler, XcTR_END); 
 Xc_free(filler);
 return TRUE;
}

