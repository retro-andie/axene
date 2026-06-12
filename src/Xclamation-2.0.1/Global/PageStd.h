/*
** PageStd.h for Xclamation in Global/
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
** Started on  Mon Jul 17 17:40:12 1995 Emmanuel Paris
** Last update Wed Oct 21 19:34:40 1998 Emmanuel Paris
*/


#ifndef _PageStd_h
#define _PageStd_h

#include "xcalibur.h"
#include "ImageStd.h"
#include "VectorStd.h"

enum
{
 SELECT_MOVE_RESIZE_CADRE = 100,
 CREATE_CADRE_RECTANGLE,       
 CREATE_CADRE_ELIPSE,
 CREATE_CADRE_POLYGONAL,
 DELETE_SELECTED_CADRE,
 SET_HIGHEST_PRIORITY,
 SET_HIGHER_PRIORITY,
 SET_LOWER_PRIORITY,
 SET_LOWEST_PRIORITY,
 SELECT_ALL_CADRE,
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
 ROTATE_CADRE,
 SCALE_CADRE,
 LOCK_CADRE,
 UNLOCK_CADRE,
 INSERT_POINT,
 DELETE_POINT,
 MOVE_POINT_MODE,
 RESIZE_CADRE_MODE,
 FUSION_CADRE,
 SUBSTRACT_CADRE,
 ADDITION_CADRE,
 OUTLINE_CADRE,
 ALIGN_CADRE,
 CREATE_COLUMN,
 CUT_OBJECT,
 COPY_OBJECT,
 PASTE_OBJECT,
 DELETE_OBJECT,
 DISPATCH_TO_OBJECT,
 DISPLAY_GRID,
 DISPLAY_TYPO_MARK,
 DISPLAY_ALIGN_MARK,
 DISPLAY_REGLE,
 DISPLAY_FRAME_BORDER,
 MAGNETIZE_ALL,
 REDRAW_ALL,
 ADD_LINK_AT_BEGINNING,
 ADD_LINK_AT_END,
 INSERT_LINK_BEFORE,
 INSERT_LINK_BEHIND,
 CADRE_FUNCTION_END
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

#define INIT(obj)	(obj)->function->init(obj);
#define EVENTLOOK(obj,event)	(obj)->function->eventlook(obj,event)
#define ABORT(obj)	(obj)->function->abort(obj)
#define REDRAW(obj)	(obj)->function->redraw(obj)

#define CURSOR (cpage->cursor)

#ifndef DATA_MACROS
#define DATA_MACROS
#ifdef ___PSEUDO_COMPOSE_MACRO
#define	DATA(a)	d_##a *data
#define DATA_ITSELF(a) (d_##a *)cpage->function_data
#define	SET_DATA(a) data=(d_##a *)Xc_malloc(#a,sizeof(d_##a)); \
		    cpage->function_data=(void *)data
#define GET_DATA(a) data=(d_##a *)cpage->function_data
#define	d(a)   (data->a)
#else
#ifdef ___GNUC_COMPOSE_MACRO
#define	DATA(a)	d_##a *data
#define DATA_ITSELF(a) (d_##a *)cpage->function_data
#define	SET_DATA(a) data=(d_##a *)Xc_malloc(#a,sizeof(d_##a)); \
		    cpage->function_data=(void *)data
#define GET_DATA(a) data=(d_##a *)cpage->function_data
#define	d(a)   (data->a)
#else
#define	DATA(a)	d_##a *data
#define	DATA_ITSELF(a)	(d_##a *)cpage->function_data
#define	SET_DATA(a) data=(d_##a *)Xc_malloc(#a,sizeof(d_##a)); \
		    cpage->function_data=(void *)data
#define GET_DATA(a) data=(d_##a *)cpage->function_data
#define	d(a)   (data->a)
#endif
#endif /* PSEUDO */
#define FREE_DATA Xc_free(data); \
cpage->function_data=NULL
#endif

#endif /*_PageStd_h*/
