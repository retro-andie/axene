/*
** Cursor_HELP.h for XMayday in Cursor/
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
** Started on  Mon Jul 17 14:20:58 1995 Emmanuel Paris
** Last update Wed Apr  9 16:11:48 1997 Emmanuel Paris
*/

#ifndef _Cursor_HELP_
#define _Cursor_HELP_

#ifndef NB_CURSOR
#define NB_CURSOR 0
#endif

enum {
  C_STANDARD = NB_CURSOR,
  C_NORMAL,
  C_WAIT,
  C_EDIT_TEXT,
  C_ANCHOR,
  C_HELP_ASK,
  LAST_CURSOR_HELP
};

#undef NB_CURSOR
#define	NB_CURSOR LAST_CURSOR_HELP

#ifdef _PRIVATE_CURSOR_

#include "../Pixmaps/standard.zpm"
#include "../Pixmaps/edit_text.zpm"
#include "../Pixmaps/xmayday.arrow.zpm"
#include "../Pixmaps/xmayday.ask.zpm"

#endif

#define CURSOR_SET_HELP \
{ DEFINED_CURSOR_XPM,(void *)standard_xpm,				      \
    STANDARD_CURSOR, (void *)XC_top_left_arrow},	/* C_STANDARD */      \
{ DEFINED_CURSOR_XPM,(void *)standard_xpm,				      \
    STANDARD_CURSOR, (void *)XC_top_left_arrow},	/* C_STANDARD */      \
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_watch},	/* C_WAIT */	      \
{ DEFINED_CURSOR_XPM, (void *)edit_text_xpm,				      \
    STANDARD_CURSOR, (void *)XC_xterm },		/* C_EDIT_TEXT */     \
{ DEFINED_CURSOR_XPM, (void *)xmayday_arrow_xpm,				      \
    STANDARD_CURSOR, (void *)XC_hand2 },		/* C_ANCHOR */	      \
{ DEFINED_CURSOR_XPM, (void *)xmayday_ask_xpm,				      \
    STANDARD_CURSOR, (void *)XC_question_arrow },	/* C_HELP_ASK */
#endif



