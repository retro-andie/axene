/*
** TextRuler.h for XAllWrite in TextRuler/
** Definition of the TextRuler class
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
** Started on  Sun Jun 12 02:11:50 1994 Stéphane Boisson
** Last update Thu Aug 13 16:53:47 1998 Emmanuel Paris
*/

#ifndef __XC_TEXT_RULER_H__
#define __XC_TEXT_RULER_H__

typedef struct sc_TextRuler c_TextRuler;

#include "BaseStd.h"
#include "xcalibur.h"
#include "DocFile.h"
#include "TextRulerP.h"
#include "Hook.h"

#define XcTR_MAX_TABULATOR 50

/*--- define inheritance attributes ---*/
#define XcTR_H_LEFT_MARGIN	1
#define XcTR_H_RIGHT_MARGIN	2
#define XcTR_H_INDENT		4
#define XcTR_H_INTERLINE_MODE	8
#define XcTR_H_REL_INTERLINE	16
#define XcTR_H_ABS_INTERLINE	32
#define XcTR_H_INTER_PARAGRAPH	64
#define XcTR_H_JUSTIFICATION	128
#define XcTR_H_WORD_DISTRIB	256
#define XcTR_H_TAB		512

typedef enum
{
 XcTR_END, XcTR_NAME, XcTR_HIDE_ON, XcTR_HIDE_OFF, XcTR_INHERIT,
 XcTR_LEFT_MARGIN, XcTR_RIGHT_MARGIN, XcTR_INDENT,
 XcTR_INTERLINE_MODE, XcTR_REL_INTERLINE, XcTR_ABS_INTERLINE, 
 XcTR_INTERLINE, XcTR_INTER_PARAGRAPH,
 XcTR_JUSTIFICATION, XcTR_WORD_DISTRIBUTION,
 XcTR_ADD_TAB, XcTR_MOVE_TAB, XcTR_CHANGE_TAB_TYPE, XcTR_CHANGE_TAB_FILLER,
 XcTR_DEL_TAB
  
} textruler_set_code_t;

typedef enum
{
 XcTR_LEFT,
 XcTR_RIGHT,
 XcTR_CENTER,
 XcTR_JUSTIFIED
} textruler_just_t; 

typedef enum
{
 XcTAB_LEFT = 0,
 XcTAB_RIGHT,
 XcTAB_CENTER,
 XcTAB_CPERIOD
} tab_just_t; 


/*--- Define the methods for the TextRuler Class ---*/
typedef struct
{
 F_STD;

 c_TextRuler *(*get) ___PROTO((c_TextRuler *ruler, BaseStd_t *base, ...));
 void (*set) ___PROTO((c_TextRuler *this, ...));
 boolean (*compare) ___PROTO((c_TextRuler *a, c_TextRuler *b, 
			      boolean with_name));
 void (*merge) ___PROTO((c_TextRuler *source, c_TextRuler **target,
			 boolean fusion));
 c_TextRuler *(*virtualCopy) ___PROTO((c_TextRuler *ruler, BaseStd_t *base));

 /*___Uses function___*/
 void (*markUsed) ___PROTO((c_TextRuler *This));
 void (*markUnused) ___PROTO((c_TextRuler *This));

 /*___tabs function___*/
 coord_t (*initGetTab) ___PROTO((c_TextRuler *This));
 boolean (*getNextTab) ___PROTO((c_TextRuler *This, coord_t *pos, 
				 tab_just_t *type, char *filler));
 /*___Loading & saving___*/
 boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			   long param, c_TextRuler **base));
 boolean (*write) ___PROTO((c_TextRuler *class, c_DocFile *doc));
 boolean (*writeList) ___PROTO((c_TextRuler *class, c_DocFile *doc));
 boolean (*readList) ___PROTO((c_DocFile *doc, char *keyword,
			       long param, c_TextRuler **base));

} sf_TextRuler;


/*--- Define the TextRuler Class ---*/
struct sc_TextRuler
{
 c_TextRuler *previous;
 c_TextRuler *next;
 c_TextRuler **base;

 sf_TextRuler *f;

 char name[XcMAX_NAME_LENGTH];	/* TextRuler name */
 long id;

 /*___Horizontal spacing___*/
 coord_t left_margin;		/* Left margin on ruler (from 0)  */
 coord_t right_margin;		/* Right margin on ruler          */
 coord_t paragraph_indent;	/* Indent for paragraph 1st lines */
 unsigned int tab_count;	/* Number of tabulators           */
 tab_just_t tab_just[XcTR_MAX_TABULATOR]; /* Tabulators type	  */
 char tab_filler[XcTR_MAX_TABULATOR];	  /* Tabulator filler     */
 coord_t tab[XcTR_MAX_TABULATOR];	  /* Tabulators pos       */
 coord_t def_tab_size;		/* Implicite tablators size	  */

 /*___Vertical spacing___*/
 boolean inter_mode;		/* True:absolute / False:relative */
 coord_t rel_inter_line;	/* Relative distance between lines in par. */
 coord_t abs_inter_line;	/* Absolute distance between lines in par. */
 coord_t inter_paragraph;	/* Distance between paragraphs    */

 /*___Horizontal justification___*/
 textruler_just_t justification;/* Justification type             */
 scale_t word_distribution;	/* Space distribution ratio       */

 /*___Hook___*/
 c_Hook *hook;

 /*___Flags___*/
 boolean lock_flag;
 boolean hide_flag;

 coord_t	tab_tmp_pos;
 int		tab_tmp_idx;

 /*___Inherited From___*/
 c_TextRuler	*inherit_from;
 unsigned int	valid_field;	/* bit field of XcTR_H */

 /*___Delayed Destroy___*/
 boolean	is_delayed;
 int		used_count;
};

extern sf_TextRuler fc_TextRuler;

#endif /* !__XC_TEXT_RULER_H__ */



