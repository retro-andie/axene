/*
** Text.h for XAllWrite in Text/
** Text Object. Manage Document's text blocks.
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
** Started on  Wed Mar 26 14:05:52 1997 Emmanuel Paris
** Last update Thu Oct 15 17:54:31 1998 Emmanuel Paris
*/

#ifndef _Text_h_
#define _Text_h_

typedef struct sc_Text c_Text;
typedef union _xa_item_t xa_item_t;

#include "xcalibur.h"
#include "DocFile.h"
#include "BaseStd.h"

#define Xa_BODY		0
#define Xa_CLIPBOARD	1
#define Xa_HEADER	2
#define Xa_FOOTER	3
#define Xa_FOOTNOTES	4

/*--- xa item structures and union ---*/
enum		/* xa_item type */
{
 XA_TEXT, XA_LINEFEED, XA_BEGIN, XA_END, XA_BULLET, XA_TAB,
 XA_PAGEBREAK, XA_ANCHOR, XA_HRULE
};

enum
{
 XA_LEFT, XA_RIGHT, XA_UNKNOW
};

enum
{
 XA_CHAR, XA_WORD, XA_LINE, XA_PARAGRAPH, XA_PAGE, XA_ALL, XA_GLOBAL
};

typedef struct _any_item_t
{
 int	type;
 xa_item_t		*prev;
 xa_item_t		*next;
 c_TextStyle		*style;
} any_item_t;

typedef struct _text_item_t
{
 int	type;
 xa_item_t		*prev;
 xa_item_t		*next;
 c_TextStyle		*style;

 char	*text;
 int	text_len;
} text_item_t;

enum		 /* bullet type */ 
{
 BULLET_UNORDORED, BULLET_ORDORED
};

typedef struct _bullet_item_t
{
 int	type;
 xa_item_t		*prev;
 xa_item_t		*next;
 c_TextStyle		*style;

 int	bullet_type;
 int	bullet_level;
} bullet_item_t;

typedef struct _linefeed_item_t
{
 int	type;
 xa_item_t		*prev;
 xa_item_t		*next;
 c_TextStyle		*style;

 c_TextRuler		*ruler;
} linefeed_item_t;

typedef struct _tab_item_t
{
 int	type;
 xa_item_t		*prev;
 xa_item_t		*next;
 c_TextStyle		*style;

 char			filler;
} tab_item_t;

union _xa_item_t
{
 int			type;
 any_item_t		any;
 text_item_t		text;
 linefeed_item_t	linefeed;
 bullet_item_t		bullet;
 tab_item_t		tab;
 any_item_t		pagebreak;
 any_item_t		anchor;
 any_item_t		hrule;
};

typedef struct
{
 F_STD;
 void (*insert) ___PROTO((c_Text *This, int item_type, void *data));
 void (*delete) ___PROTO((c_Text *This));
 void (*goTo) ___PROTO((c_Text *This, int type, int direction, int count,
			xa_item_t *item, int pos, boolean select));
 xa_item_t *(*cut_line) ___PROTO((c_Text *This, xa_item_t *item,  int pos));
 void (*pack) ___PROTO((c_Text *This, xa_item_t **item));
 void (*orTextStyle) ___PROTO((c_Text *This, c_TextStyle *(*cb_func)(),
			     void *page, void *cb_data));
 void (*orTextRuler) ___PROTO((c_Text *This, c_TextRuler *(*cb_func)(),
			       void *page, void *cb_data));
 void (*clip_copy) ___PROTO((c_Text *This, c_Text *clipboard));
 void (*clip_paste) ___PROTO((c_Text *This, c_Text *clipboard));
 void (*select_all) ___PROTO((c_Text *This));
 boolean (*checkDiffTextItem) ___PROTO((c_Text *This, xa_item_t *item1,
					xa_item_t *item2, int *pos));
 void (*purge_changed_items) ___PROTO((c_Text *This));
 boolean (*need_undo) ___PROTO((c_Text *This, int nb_char));
 /*___Loading & saving___*/
 boolean (*read) ___NPROTO((c_DocFile *doc, char *keyword,
			    long param, text_read_t *data));
 boolean (*write) ___PROTO((c_Text *this, c_DocFile *doc));
 boolean (*readList) ___PROTO((c_DocFile *doc, char *keyword,
			       long param, BaseStd_t *base));
 boolean (*writeList) ___PROTO((c_Text *this, c_DocFile *doc));
} sf_Text;

/* define the Text Class */
struct sc_Text
{
 sf_Text        *f;
 
 c_Text		*next;
 c_Text		**base;
 
 char		name[XcMAX_NAME_LENGTH];
 int		type;
 long		id;

 BaseStd_t	*BaseStd;
 xa_item_t	*item_base;
 ItemHandle	*i_handle;

 xa_item_t	*select_start;
 int		select_start_pos;
 xa_item_t	*select_end;
 int		select_end_pos;
 boolean	start_end;	/* true if cursor is on start pos */

 c_TextStyle	*style;
 c_TextRuler	*ruler;

 xa_item_t	*changed_start;
 xa_item_t	*changed_end;
 xa_item_t	*changed_stack;
};

extern sf_Text fc_Text;

#endif /* _Text_h_ */








