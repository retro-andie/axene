/*
** ObjectStd.h for Xclamation, XQuad and XAllWrite in Global/
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
** Started on  Mon Jul 17 17:41:02 1995 Emmanuel Paris
** Last update Fri Oct  9 17:06:46 1998 Emmanuel Paris
*/


#ifndef _ObjectStd_h
#define _ObjectStd_h

#include "Cursor.h"
#include "polygone.h"

#ifndef FUNCTION_FINISHED
#define FUNCTION_FINISHED	0
#define	FUNCTION_NOT_FINISHED	1
#define FUNCTION_ABORTED	2
#define FUNCTION_REINIT		3
#define FUNCTION_MODIFIED	4
#endif

/* short cut for standard function definition */
#define F_OBJECT_STD		\
void (*init)();			\
status_t (*eventlook)();	\
void (*abort)();		\
void (*redraw)()     

typedef struct _sf_object_function 
{
 F_OBJECT_STD;
} sf_object_function;

typedef struct _d_object_function
{  
#ifndef Xc_XALLWRITE
 int			old_function;/* previous function selected */
#endif
 c_Cursor		*cursor;
 void			*cadre;
 void			*object;
 void			*function_data;
 sf_object_function	*function;
 int			function_type;
} 
d_object_function;

#define _cursor	(cobject->cursor)
#define _cadre	((c_Cadre *)(cobject->cadre))
#define _image	((c_Image *)(cobject->object))
#define _vector	((c_VectorGraph *)(cobject->object))

#ifdef ___PSEUDO_COMPOSE_MACRO
#define Fo(a,b) \
(((c_Cadre *)b->cadre)->data_to_object)->a( \
					   ((c_Cadre *)b->cadre)->data_to_object->page, \
     ((c_Cadre *)b->cadre))
#else
#ifdef ___GNUC_COMPOSE_MACRO
#define Fo(a,b) \
(((c_Cadre *)##b##->cadre)->data_to_object)->##a##( \
						   ((c_Cadre *)##b##->cadre)->data_to_object->page, \
     ((c_Cadre *)##b##->cadre))
#else
#define Fo(a,b) \
(((c_Cadre *)			/**/b/**/->cadre)->data_to_object)->/**/a/**/( \
									      ((c_Cadre *)/**/b/**/->cadre)->data_to_object->page, \
     ((c_Cadre *)/**/b/**/->cadre))
#endif
#endif /* PSEUDO */

typedef struct _d_object_std
{
 void			*page;
 void			(*redraw_cadre)();
 void			(*redraw_rectangle)();
 Polygone		*(*get_clip_polygone)();
}
d_object_std;

#ifndef DATA_MACROS
#define DATA_MACROS
#ifdef ___PSEUDO_COMPOSE_MACRO
#define	DATA(a)	d_##a *data	 
#define DATA_ITSELF(a) (d_##a *)cobject->function_data
#define	SET_DATA(a) data=(d_##a *)Xc_malloc(#a ,sizeof(d_##a)); \
cobject->function_data=(void *)data
#define GET_DATA(a) data=(d_##a *)cobject->function_data
#define	d(a)   (data->a)
#else
#ifdef ___GNUC_COMPOSE_MACRO
#define	DATA(a)	d_##a## *data	 
#define DATA_ITSELF(a) (d_##a## *)cobject->function_data
#define	SET_DATA(a) data=(d_##a## *)Xc_malloc("##a##",sizeof(d_##a##)); \
cobject->function_data=(void *)data
#define GET_DATA(a) data=(d_##a## *)cobject->function_data
#define	d(a)   (data->##a##)
#else
#define	DATA(a)	d_/**/a/**/ *data	 
#define	DATA_ITSELF(a)	(d_/**/a/**/ *)cobject->function_data	 
#define	SET_DATA(a) data=(d_/**/a/**/ *)Xc_malloc("/**/a/**/",sizeof(d_/**/a/**/)); \
cobject->function_data=(void *)data
#define GET_DATA(a) data=(d_/**/a/**/ *)cobject->function_data
#define	d(a)   (data->/**/a/**/)
#endif
#endif /* PSEUDO */
#define FREE_DATA Xc_free(data); \
cobject->function_data=NULL
#endif

#endif /*_ObjectStd_h*/



