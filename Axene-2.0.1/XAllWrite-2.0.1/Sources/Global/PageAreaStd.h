/*
** PageAreaStd.h for XAllWrite in Global/
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
** Started on  Mon Jul 17 17:40:12 1995 Emmanuel Paris
** Last update Mon May  5 16:49:36 1997 Emmanuel Paris
*/


#ifndef _PageAreaStd_h
#define _PageAreaStd_h

#include "xcalibur.h"

enum 
{
 PAGEAREA_FUNCTION_BEGIN = 100,
 DISPATCH_TO_PAGE,
 ZOOM_PLUS_PAGE,
 ZOOM_MINUS_PAGE,
 ZOOM_IN_ZONE,
 ZOOM_FIT_IN_WIDTH,
 ZOOM_FIT_IN_SCREEN,
 ZOOM_AT_REAL_SIZE,
 ZOOM_25_PAGE,
 ZOOM_50_PAGE,
 ZOOM_75_PAGE,
 ZOOM_100_PAGE,
 ZOOM_200_PAGE,
 ZOOM_400_PAGE,
 REDRAW_ALL,
 DISPLAY_SPEC_CHARS,
 PAGEAREA_FUNCTION_END
};

#ifndef FUNCTION_FINISHED
#define FUNCTION_FINISHED	0
#define	FUNCTION_NOT_FINISHED	1
#define FUNCTION_ABORTED	2
#define FUNCTION_REINIT		3
#define FUNCTION_MODIFIED	4
#endif

/* short cut for standard function definition */
#define F_PAGEAREA_STD		\
void (*init)();			\
status_t (*eventlook)();	\
void (*abort)();		\
void (*redraw)()     
     
typedef struct _sf_pagearea_function 
{
 F_PAGEAREA_STD;
} sf_pagearea_function;

#ifndef FUNCTION_STD
#define FUNCTION_STD

#define INIT(obj)	(obj)->function->init(obj);
#define EVENTLOOK(obj,event)	(obj)->function->eventlook(obj,event)
#define ABORT(obj)	(obj)->function->abort(obj)
#define REDRAW(obj)	(obj)->function->redraw(obj)

#define CURSOR (This->cursor)

#endif

#ifndef DATA_MACROS
#define DATA_MACROS
#ifdef ___GNUC_COMPOSE_MACRO
#define	DATA(a)	d_##a## *data	 
#define DATA_ITSELF(a) (d_##a## *)This->function_data
#define	SET_DATA(a) data=(d_##a## *)Xc_malloc("##a##",sizeof(d_##a##)); \
		    This->function_data=(void *)data
#define GET_DATA(a) data=(d_##a## *)This->function_data
#define	d(a)   (data->##a##)
#else
#define	DATA(a)	d_/**/a/**/ *data	 
#define	DATA_ITSELF(a)	(d_/**/a/**/ *)This->function_data	 
#define	SET_DATA(a) data=(d_/**/a/**/ *)Xc_malloc("/**/a/**/",sizeof(d_/**/a/**/)); \
		    This->function_data=(void *)data
#define GET_DATA(a) data=(d_/**/a/**/ *)This->function_data
#define	d(a)   (data->/**/a/**/)
#endif
#define FREE_DATA Xc_free(data); \
                  This->function_data=NULL
#endif

#endif /*_PageStd_h*/
