/*
** FrameManagerStd.h for XQuad in Global/
** standard definition for FrameManager functions
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
** Started on  Sometime during     1995 One of the authors
** Last update Mon Jun 23 18:17:22 1997 One of the authors
*/


#ifndef _FrameManagerStd_h
#define _FrameManagerStd_h

#include "xcalibur.h"

enum {
  FRMG_FUNCTION_BEGIN = 300,
  SELECT_MOVE_RESIZE_CADRE,
  CREATE_CADRE_RECTANGLE,       
  DELETE_SELECTED_CADRE,
  SET_HIGHEST_PRIORITY,
  SET_HIGHER_PRIORITY,
  SET_LOWER_PRIORITY,
  SET_LOWEST_PRIORITY,
  SELECT_ALL_CADRE,
  LOCK_CADRE,
  UNLOCK_CADRE,
  DELETE_OBJECT,
  DISPLAY_FRAME_BORDER,
  MAGNETIZE_ALL,
  FRMG_FUNCTION_END
};

#ifndef FUNCTION_FINISHED
#define FUNCTION_FINISHED	0
#define	FUNCTION_NOT_FINISHED	1
#define FUNCTION_ABORTED	2
#define FUNCTION_REINIT		3
#define FUNCTION_MODIFIED	4
#endif

/* short cut for standard function definition */
#define F_FRMG_STD		\
void (*init)();			\
status_t (*eventlook)();	\
void (*abort)();		\
void (*redraw)()     
     
typedef struct _sf_frmg_function {
  F_FRMG_STD;
} sf_frmg_function;

#ifndef FUNCTION_STD
#define FUNCTION_STD

#define INIT(obj)	(obj)->function->init(obj);
#define EVENTLOOK(obj,event)	(obj)->function->eventlook(obj,event)
#define ABORT(obj)	(obj)->function->abort(obj)
#define REDRAW(obj)	(obj)->function->redraw(obj)

#define CURSOR (cfrmg->cursor)
#endif

#ifndef DATA_MACROS
#define DATA_MACROS
#ifdef ___PSEUDO_COMPOSE_MACRO
#define	DATA(a)	d_##a *data	 
#define DATA_ITSELF(a) (d_##a *)cfrmg->function_data
#define	SET_DATA(a) data=(d_##a *)Xc_malloc(#a ,sizeof(d_##a)); \
		    cfrmg->function_data=(void *)data
#define GET_DATA(a) data=(d_##a *)cfrmg->function_data
#define	d(a)   (data->a)
#else
#ifdef ___GNUC_COMPOSE_MACRO
#define	DATA(a)	d_##a## *data	 
#define DATA_ITSELF(a) (d_##a## *)cfrmg->function_data
#define	SET_DATA(a) data=(d_##a## *)Xc_malloc("##a##",sizeof(d_##a##)); \
		    cfrmg->function_data=(void *)data
#define GET_DATA(a) data=(d_##a## *)cfrmg->function_data
#define	d(a)   (data->##a##)
#else
#define	DATA(a)	d_/**/a/**/ *data	 
#define	DATA_ITSELF(a)	(d_/**/a/**/ *)cfrmg->function_data	 
#define	SET_DATA(a) data=(d_/**/a/**/ *)Xc_malloc("/**/a/**/",sizeof(d_/**/a/**/)); \
		    cfrmg->function_data=(void *)data
#define GET_DATA(a) data=(d_/**/a/**/ *)cfrmg->function_data
#define	d(a)   (data->/**/a/**/)
#endif
#endif /* PSEUDO */
#define FREE_DATA Xc_free(data); \
                  cfrmg->function_data=NULL
#endif

#endif /*_FrameManagerStd_h*/

