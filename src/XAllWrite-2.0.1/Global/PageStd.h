/*
** PageStd.h for XAllWrite in Global/
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Mon Jul 17 17:40:12 1995 Emmanuel Paris
** Last update Sat Jan 16 21:51:30 1999 One of the authors
*/


#ifndef _PageStd_h
#define _PageStd_h

#include "xcalibur.h"

enum 
{
 PAGE_FUNCTION_BEGIN = 200,
 SELECT_MOVE_EDIT_TEXT,
 SET_BOLD, SET_ITALIC, SET_UNDERLINE, SET_STRIKEOUT, 
 SET_SHADOW, SET_SUBSCRIPT, SET_SUPSCRIPT, SET_OUTLINE,
 SET_SMALL_CAPS, SET_BIG_CAPS,
 DEC_FONT_SIZE, INC_FONT_SIZE,
 CUT_TEXT, COPY_TEXT, PASTE_TEXT, DELETE_TEXT, SELECT_ALL_TEXT,
 SET_JLEFT, SET_JCENTER, SET_JRIGHT, SET_JUSTIFY,
 INC_INDENT, DEC_INDENT, INC_INTERLINE, DEC_INTERLINE,
 INC_INTERPARAG, DEC_INTERPARAG, INSERT_PAGE_BREAK,
 INSERT_EURO, INSERT_SPEC_CHAR,
 PAGE_FUNCTION_END
};

#ifndef FUNCTION_FINISHED
#define FUNCTION_FINISHED	0
#define	FUNCTION_NOT_FINISHED	1
#define FUNCTION_ABORTED	2
#define FUNCTION_REINIT		3
#define FUNCTION_MODIFIED	4
#endif

/* short cut for standard function definition */
#define F_PAGE_STD		\
void (*init)();			\
status_t (*eventlook)();	\
void (*abort)();		\
void (*redraw)()     
     
typedef struct _sf_page_function 
{
 F_PAGE_STD;
} sf_page_function;

#ifndef FUNCTION_STD
#define FUNCTION_STD

#define INIT(obj)	(obj)->function->init(obj);
#define EVENTLOOK(obj,event)	(obj)->function->eventlook(obj,event)
#define ABORT(obj)	(obj)->function->abort(obj)
#define REDRAW(obj)	(obj)->function->redraw(obj)

#define CURSOR (cpage->cursor)

#endif

#ifndef DATA_MACROS
#define DATA_MACROS
#ifdef ___PSEUDO_COMPOSE_MACRO
#define	DATA(a)	d_##a *data
#define DATA_ITSELF(a) (d_##a *)cpage->function_data
#define	SET_DATA(a) data=(d_##a *)Xc_malloc(#a,sizeof(d_##a)); \
		    cpage->function_data=(void *)data
#define GET_DATA(a) data=(d_##a *)cpage->function_data
#define GET_DATAF(a) c_FrMg *cfrmg; \
		     cfrmg = cpage->frmg; \
		     data=(d_##a *)cpage->function_data
#define	d(a)   (data->a)
#else
#ifdef ___GNUC_COMPOSE_MACRO
#define	DATA(a)	d_##a *data
#define DATA_ITSELF(a) (d_##a *)cpage->function_data
#define	SET_DATA(a) data=(d_##a *)Xc_malloc(#a,sizeof(d_##a)); \
		    cpage->function_data=(void *)data
#define GET_DATA(a) data=(d_##a *)cpage->function_data
#define GET_DATAF(a) c_FrMg *cfrmg; \
		     cfrmg = cpage->frmg; \
		     data=(d_##a *)cpage->function_data
#define	d(a)   (data->a)
#else
#define	DATA(a)	d_##a *data
#define	DATA_ITSELF(a)	(d_##a *)cpage->function_data
#define	SET_DATA(a) data=(d_##a *)Xc_malloc(#a,sizeof(d_##a)); \
		    cpage->function_data=(void *)data
#define GET_DATA(a) data=(d_##a *)cpage->function_data
#define GET_DATAF(a) c_FrMg *cfrmg; \
		     cfrmg = cpage->frmg; \
		     data=(d_##a *)cpage->function_data
#define	d(a)   (data->a)
#endif
#endif /* PSEUDO */
#define FREE_DATA Xc_free(data); \
                  cpage->function_data=NULL
#endif

#endif /*_PageStd_h*/
