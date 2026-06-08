/*
** Date_Time_Common.h for XQuad in Box_nombres/
** Object and method for Box_nombres Class
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
** Started on  Mon May 22 13:32:14 1995 Robin Castanier
** Last update Tue Mar 19 17:02:29 1996 Emmanuel Paris
*/

#ifndef _Date_Time_Common_h
#define _Date_Time_Common_h

typedef struct _dt_seq_base_t dt_seq_base_t;

#include "Box_nombres.h"
#include "Date_Time.h"

struct _dt_seq_base_t
{
  dt_seq_t	*item;
  struct _dt_seq_base_t *Next;
};

extern void bn_tf_modify ___PROTO((Widget w, c_Box_nombres *This, 
				   XmTextVerifyCallbackStruct *cbs));
extern void bn_insert_dt_seq ___PROTO((c_Box_nombres *This, dt_seq_t *seq));
extern void free_dt_seq_base ___PROTO((c_Box_nombres *this));


#endif /*_Date_Time_Common_h*/








