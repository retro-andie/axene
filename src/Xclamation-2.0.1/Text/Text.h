/*
** Text.h for Xclamation in Text/
** Definition for the Text class
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
** Started on  Sun Jun 12 02:13:42 1994 Stéphane Boisson
** Last update Thu Jun  5 15:27:34 1997 Emmanuel Paris
*/

#ifndef __XC_TEXT_H__
#define __XC_TEXT_H__

typedef struct sc_Text c_Text;

#include "xcalibur.h"
#include "TextStyle.h"
#include "TextRuler.h"
#include "DocFile.h"
#include "Hook.h"
#include "BaseStd.h"

#define XcDF_TEXT_KEYWORD "TEXT"
#define XcDF_TEXTLIST_KEYWORD "TEXTLIST"

typedef long text_pos_t;

#define XcT_PAGE_CODE ((char_t) 0x0c)        /* \f */
#define XcT_LINE_CODE ((char_t) 0x0d)        /* \r */
#define XcT_TABULATOR_CODE ((char_t) 0x09)        /* \t */
#define XcT_PARAGRAPH_CODE ((char_t) 0x0a)   /* \n */
#define XcT_SPACE_CODE ((char_t) 0x20)

/*--- Internal structures ---*/
typedef struct
{
 c_Text *text;
 c_Text **text_base;
 c_TextStyle *style_base;
 c_TextRuler *ruler_base;
} text_read_t;


/*--- Reference structure ---*/
typedef enum
{
 XcT_TEXT_RULER = 0, XcT_TEXT_STYLE,
 XcT_INDEX, XcT_SECTION, XcT_HYPERTEXT, XcT_UNKNOWN
} text_ref_type_t;

typedef struct text_reference_s
{
 struct text_reference_s *next;

 unsigned int code;
 text_ref_type_t type;
 union 
{
 c_TextStyle *style;
 c_TextRuler *ruler;
 void *index;
 void *section;
 void *hyper;
 void *pointer;
} data;

 unsigned long count;
} text_reference_t;


/*--- Text sequence structure ---*/
typedef struct
{
 enum 
{
 XcT_NOTHING,
 XcT_END_OF_TEXT,
 XcT_END_OF_PAGE,
 XcT_END_OF_PARAGRAPH,
 XcT_END_OF_LINE
} special;

 boolean limit_reached;
 char_t *start;
 text_pos_t start_pos;
 text_pos_t len;
  
 text_reference_t *ref;
 unsigned int space_count;
} text_seq_t;


/*--- Define the methods for the Text Class ---*/
typedef struct 
{
 F_STD;

 error (*changeBase) ___PROTO((c_Text *this, BaseStd_t *new_base));

 /*___Loading & saving___*/
 boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			   long param, text_read_t *data));
 boolean (*write) ___PROTO((c_Text *this, c_DocFile *doc));
 boolean (*readList) ___PROTO((c_DocFile *doc, char *keyword,
			       long param, BaseStd_t *base));
 boolean (*writeList) ___PROTO((c_Text *this, c_DocFile *doc));

 /*___Text edition___*/
 void (*move) ___PROTO((c_Text *obj, text_pos_t pos, boolean save));
 void (*insertString) ___PROTO((c_Text *obj, char_t *string, text_pos_t len));
 void (*insertCode) ___PROTO((c_Text *this, char_t code));
 void (*insertReference) ___PROTO((c_Text *this, text_ref_type_t type,
				   void *data));
 void (*deleteString) ___PROTO((c_Text *obj, text_pos_t len));
 void (*undo) ___PROTO((c_Text *obj));

 /*___Text output___*/
 void (*getFirstSeq) ___PROTO((c_Text *obj, text_pos_t start,
			       c_TextStyle **first_style,
			       c_TextRuler **first_ruler));
 text_seq_t *(*getNextSeq) ___PROTO((c_Text *obj, text_pos_t));

 void (*warn) ___PROTO((c_Text *this));

 boolean (*export) ___PROTO((c_Text *this, BaseStd_t *base, char *filename));

 /*___Debug___*/
 void (*show) ___PROTO((c_Text *obj));
} sf_Text;


/*--- Define the Text Class ---*/
struct sc_Text
{
 c_Text *next;
 c_Text **base;

 sf_Text *f;

 char name[XcMAX_NAME_LENGTH];
 long id;

 /*___Buffer information___*/
 char_t *buffer_start;		/* start ptr of text buffer          */
 char_t *buffer_end;		/* end ptr of text buffer + 1        */
 text_pos_t pos;		/* position in text (not in gap)     */
 boolean modified_flag;		/* text changed since last save      */

 /*___Gap information___*/
 text_pos_t gap_pos;		/* start position of gap             */
 char_t *gap_start;		/* start ptr of gap                  */
 char_t *gap_end;		/* end ptr of gap + 1                */
 text_pos_t gap_len;		/* gap len (gap_end - gap_start - 1) */
 boolean gap_direction;		/* FALSE if save space first         */
 text_pos_t gap_save_len;	/* undo data len in gap              */
 text_pos_t gap_no_save;	/* char to not put in save buffer    */
 boolean save_modified_flag;	/* undo text changed since last save */
 text_pos_t save_pos;		/* undo pos                          */
  
 /*___References___*/
 text_reference_t ref_base[2];
 unsigned int first_style;
 unsigned int first_ruler;

 /*___Extracted sequence___*/
 text_pos_t seq_pos;
 text_seq_t seq;
 char_t *seq_buffer;

 /*___Hook___*/
 c_Hook *hook;

 /*___TextFrame sequence___*/
 int	sequence;
};

extern sf_Text fc_Text;

#endif /* !__XC_TEXT_H__ */
