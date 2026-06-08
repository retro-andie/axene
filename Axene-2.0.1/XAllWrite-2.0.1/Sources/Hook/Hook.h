/*
** Hook.h for Xclamation, XQuad and XAllWrite in Hook/
** Definition of the Hook class
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
** Started on  Tue Dec 27 18:13:28 1994 Stéphane Boisson
** Last update Wed May 31 15:39:08 1995 Stéphane Boisson
*/

#ifndef __XC_HOOK_H__
#define __XC_HOOK_H__

typedef struct sc_Hook c_Hook;

#include "xcalibur.h"


/*--- Macros ---*/
#define XcH_VALUE_CHANGED 1
#define XcH_CONTENT_CHANGED 2
#define XcH_SAVE_PREP 3
#define XcH_REDRAW 4

#define XcH_REGISTER(object, cb_func, cb_data) \
  F((object)->hook).addCallback((object)->hook, \
				(XcHookCallbackProc)(cb_func), (cb_data))

#define XcH_REGISTER_ONCE(object, cb_func, cb_data) \
  F((object)->hook).addOneCallback((object)->hook, \
				   (XcHookCallbackProc)(cb_func), (cb_data))

#define XcH_UNREGISTER(object, cb_func, cb_data) \
  F((object)->hook).removeCallback((object)->hook, \
				   (XcHookCallbackProc)(cb_func), (cb_data))

#define XcH_REPLACE(old, new, cb_func, cb_data) \
  FUNCTION(c_Hook, changeHook)(((old)? (old)->hook : NULL), \
			       ((new)? (new)->hook : NULL), \
			       (XcHookCallbackProc)(cb_func), (cb_data))

#define XcH_USAGE(object) F((object)->hook).getUsage((object)->hook, NULL)

/*--- Internal types ---*/
typedef void (*XcHookCallbackProc) ___PROTO((void *cb_data,
					     int reason,
					     void *old,
					     void *new));
typedef struct 
{
  XcHookCallbackProc cb_func;
  void *cb_data;
  unsigned int count;
} hook_client_t;


/*--- Define the methods for the Hook Class ---*/
typedef struct
{
  F_STD;

  unsigned long (*getUsage) ___PROTO((c_Hook *this, void *cb_data));

  /*___Server methods___*/
  void (*callback) ___PROTO((c_Hook *this, int reason, void *new));
  boolean (*merge) ___PROTO((c_Hook *target, c_Hook *source));
  
  /*___Client methods___*/
  boolean (*addCallback) ___PROTO((c_Hook *this,
				   XcHookCallbackProc cb_func, void *cb_data));
  void (*removeCallback) ___PROTO((c_Hook *this,
				   XcHookCallbackProc cb_func, void *cb_data));
  boolean (*changeHook) ___PROTO((c_Hook *old, c_Hook *new,
				  XcHookCallbackProc cb_func, void *cb_data));
  
  unsigned long (*getClientData) ___PROTO((c_Hook *this, void ***pptr));

  boolean (*addOneCallback) ___PROTO((c_Hook *this,
				      XcHookCallbackProc cb_func,
				      void *cb_data));
} sf_Hook;


/*--- Define the Hook Class ---*/
struct sc_Hook
{
  sf_Hook *f;

  void *server_data;
  unsigned long increment;
  unsigned long count;
  unsigned long max_count;
  hook_client_t *clients;
};

extern sf_Hook fc_Hook;

#endif /* !__XC_HOOK_H__ */
