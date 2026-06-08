/*
** ClipBuffer.h for Xclamation in ClipBuffer/
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
** Started on  Mon Jul 17 14:12:37 1995 Emmanuel Paris
** Last update Mon Jul 17 14:12:37 1995 Robin Castanier
*/


#ifndef _ClipBuffer_h_
#define	_ClipBuffer_h_

#include "xcalibur.h"
#include "LinkManager.h"
#include "BaseStd.h"
/* define list of Link Manager Object */

typedef struct _l_XcLM_Object
{
  XcLM_Object		*object;
  struct _l_XcLM_Object	*NextLMobject;
} l_XcLM_Object;	

/* define ClipBuffer object */

typedef	struct
{
  F_STD;
  void	(*add_object_in_list)();
  void	(*del_object_in_list)();
  void  (*empty_list)();
  void  (*start_request_object)();
  XcLM_Object *(*request_next_object)();
} sf_ClipBuffer;

typedef	struct
{
  sf_ClipBuffer		*f;
  l_XcLM_Object		*LMobject;
  l_XcLM_Object		*Request_LMobject;
  BaseStd_t		BaseStd;
} c_ClipBuffer;

extern sf_ClipBuffer fc_ClipBuffer;

#endif
