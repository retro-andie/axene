/*
** TextRuler.h for Xclamation in TextRuler/
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
** Last update Mon Aug 14 16:58:36 1995 Stéphane Boisson
*/

#ifndef __XC_TEXT_RULER_H__
#define __XC_TEXT_RULER_H__

typedef struct sc_TextRuler c_TextRuler;

#include "BaseStd.h"
#include "xcalibur.h"
#include "DocFile.h"
#include "Hook.h"

#define XcDF_RULER_KEYWORD "RULER"
#define XcDF_RULERLIST_KEYWORD "RULERLIST"
#define XcDF_LEFT_KEYWORD "LEFT"
#define XcDF_RIGHT_KEYWORD "RIGHT"
#define XcDF_CENTER_KEYWORD "CENTER"
#define XcDF_JUSTIFIED_KEYWORD "JUSTIFIED"

#define XcTR_MAX_TABULATOR 64

typedef enum {
  XcTR_END, XcTR_NAME, XcTR_LEFT_MARGIN, XcTR_RIGHT_MARGIN, XcTR_INDENT,
  XcTR_REL_INTERLINE, XcTR_ABS_INTERLINE, XcTR_INTER_PARAGRAPH,
  XcTR_JUSTIFICATION, XcTR_WORD_DISTRIBUTION, XcTR_MIN_RATIO, XcTR_MAX_RATIO,
  XcTR_INTERLINE_MODE, XcTR_INTERLINE
} textruler_set_code_t;

typedef enum {
  XcTR_LEFT,
  XcTR_RIGHT,
  XcTR_CENTER,
  XcTR_JUSTIFIED
} textruler_just_t; 


/*--- Define the methods for the TextRuler Class ---*/
typedef struct
{
  F_STD;

  void (*set) ___PROTO((c_TextRuler *this, ...));
  boolean (*compare) ___PROTO((c_TextRuler *a, c_TextRuler *b));
  void (*merge) ___PROTO((c_TextRuler *source, c_TextRuler **target,
			  boolean fusion));
  c_TextRuler *(*virtualCopy) ___PROTO((c_TextRuler *ruler, BaseStd_t *base));

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

  char name[XcMAX_NAME_LENGTH];		/* TextRuler name */
  long id;

  /*___Horizontal spacing___*/
  coord_t left_margin;			/* Left margin on ruler (from 0)  */
  coord_t right_margin;			/* Right margin on ruler          */
  coord_t paragraph_indent;		/* Indent for paragraph 1st lines */
  unsigned int tab_count;		/* Number of tabulators           */
  coord_t tab[XcTR_MAX_TABULATOR];	/* Tabulators                     */

  /*___Vertical spacing___*/
  boolean inter_mode;			/* True:absolute / False:relative */
  coord_t inter_line;			/* Distance between lines in par. */
  coord_t inter_paragraph;		/* Distance between paragraphs    */

  /*___Horizontal justification___*/
  textruler_just_t justification;	/* Justification type             */
  scale_t word_distribution;		/* Space distribution ratio       */
  scale_t min_ratio;			/* minimum length for linefeed    */
  scale_t max_ratio;			/* maximum length for linefeed    */

  /*___Hook___*/
  c_Hook *hook;

  /*___Flags___*/
  boolean lock_flag;
};

extern sf_TextRuler fc_TextRuler;

#endif /* !__XC_TEXT_RULER_H__ */
