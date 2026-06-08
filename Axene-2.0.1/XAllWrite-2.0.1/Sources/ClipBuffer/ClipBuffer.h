/*
** ClipBuffer.h for XAllWrite in ClipBuffer/
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
** Started on  Mon Jul 17 14:12:37 1995 Emmanuel Paris
** Last update Mon Apr 20 16:48:03 1998 Emmanuel Paris
*/


#ifndef _ClipBuffer_h_
#define	_ClipBuffer_h_

#include "xcalibur.h"
#include "BaseStd.h"

typedef	struct _c_ClipBuffer c_ClipBuffer;

#define XcCB_CURRENT_LEVEL(This) (This->level)
#define XcCB_INFINITE_LEVEL -1

typedef enum 
{
 XcCB_UNKNOW,
 XcCB_TEXT,
 XcCB_IMAGE,
 XcCB_VECTOR,
 XcCB_FRAME_EMPTY,
 XcCB_FRAME_TEXT,
 XcCB_FRAME_IMAGE,
 XcCB_FRAME_VECTOR
} clipb_e;

typedef struct _clipb_t
{
 void			*object;
 void			(*destructor)();
 clipb_e		type;
 int			level;
 struct _clipb_t	*next;
} clipb_t;	

/* define ClipBuffer object */

typedef	struct
{
  F_STD;
  void	(*new_level) ___PROTO((c_ClipBuffer *This));
  void	(*add_object) ___PROTO((c_ClipBuffer *This, void *object,
				clipb_e type, void (*destructor)()));
  void	(*del_object) ___PROTO((c_ClipBuffer *This, void *object));
  void	(*del_level) ___PROTO((c_ClipBuffer *This, int level));

  void  (*empty_list) ___PROTO((c_ClipBuffer *This));
  void  (*start_request_object) ___PROTO((c_ClipBuffer *This, int level));
  void *(*request_next_object) ___PROTO((c_ClipBuffer *This));
} sf_ClipBuffer;

struct _c_ClipBuffer
{
  sf_ClipBuffer		*f;
  BaseStd_t		BaseStd;

  clipb_t		*base;
  int			level;

  clipb_t		*req_item;
  int			req_level;
  
  int			store_level;
  int			low_level;
  int			nb_level;
};

extern sf_ClipBuffer fc_ClipBuffer;

#endif
