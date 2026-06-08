/*
** TextFrame.h for Xclamation in TextFrame/
** Declarations for the TextFrame class
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
** Started on  Thu Sep 22 17:37:10 1994 Stéphane Boisson
** Last update Thu Jun  5 15:29:29 1997 Emmanuel Paris
*/

#ifndef __TEXT_FRAME_H__
#define __TEXT_FRAME_H__

typedef struct sc_TextFrame c_TextFrame;

#include "xcalibur.h"
#include "CadreStd.h"
#include "Cadre.h"
#include "Text.h"
#include "Document.h"

#define XcDF_LINK_KEYWORD "LINK"

/*--- Forward declarations ---*/
typedef struct textframe_line_s textframe_line_t;


/*--- Define the methods for the TextFrame Class ---*/
typedef struct
{
  F_STD;
  F_CADRE_STD;
  
  /*___Links___*/
  boolean (*link) ___PROTO((c_TextFrame *this,
			    c_TextFrame *anchor, boolean before_flag));
  boolean (*unlink) ___PROTO((c_TextFrame *this));
  boolean (*attachText) ___PROTO((c_TextFrame *this, c_Text *text));
  
  /*___Text import___*/
  boolean (*importText) ___PROTO((c_TextFrame *this, BaseStd_t *base,
				  char *filename));
  boolean (*importASCII) ___PROTO((c_TextFrame *this, BaseStd_t *base,
				   char *filename,
				   c_TextStyle *style, c_TextRuler *ruler));
/*boolean (*importXMW) ___PROTO((c_TextFrame *this, BaseStd_t *base,*/
/*char *filename));*/
} sf_TextFrame;


/*--- Define the TextFrame Class ---*/
struct sc_TextFrame
{
  sf_TextFrame *f;

  /*___Links___*/
  c_TextFrame *previous;
  c_TextFrame *next;
  long id;

  /*___Data___*/
  c_Text *text;

  c_Cadre *frame;

  text_pos_t start_pos;
  boolean start_paragraph_flag;

  text_pos_t end_pos;
  boolean end_paragraph_flag;

  /*___Graphic aspect___*/
  angle_t angle;
  scale_t factor;
  boolean drawn_flag;

  /*___Text format data___*/
  unsigned int line_count;
  textframe_line_t *lines;

  /*___Text sequence___*/
  int	sequence;
};

extern sf_TextFrame fc_TextFrame;

#endif /* !__TEXT_FRAME_H__ */
