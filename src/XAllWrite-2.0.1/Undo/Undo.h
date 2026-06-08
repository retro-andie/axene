/*
** Undo.h for XAllWrite in Undo/
** Undo object. Use context duplication by fork().
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
** Started on  Mon Sep  7 18:03:49 1998 Emmanuel Paris
** Last update Sat Jan 16 22:18:33 1999 Emmanuel Paris
*/

#ifndef _Undo_h_
#define	_Undo_h_

typedef struct sc_Undo c_Undo;

#include "xcalibur.h"
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include "WInterface.h"
#include "Resource.h"

enum
{
 XcU_MENU_ITEM,
 XcU_TOGGLE_ICON,
 XcU_COUPLE_ICON
};

typedef struct _undo_wid_t
{
 Widget		w_Undo;		/* undo menu item widget */
 Widget		w_Redo;		/* redo menu item widget */
 int		w_Type;		/* Widget type */
 char		*sundo;		/* undo formated string */
 char		*sredo;		/* redo formated string */
 struct _undo_wid_t *Next;
} undo_wid_t;

typedef struct _undo_pix_t
{
 boolean	state;		/* TRUE: register / FALSE: unregister */
 Pixmap		pixmap;
 struct _undo_pix_t *Next;
} undo_pix_t;

typedef struct _undo_gc_t
{
 GC		gc;		/* gc id */
 XGCValues	xgcv_init;	/* original values */
 XGCValues	xgcv_current;	/* final values */
 unsigned long	value_mask;	/* fields that have changed */
 struct _undo_gc_t *Next;
} undo_gc_t;

typedef	struct
{
 F_STD;
 void	 (*addWidgets) ___PROTO((c_Undo *This, Widget wUndo, Widget wRedo,
				 int wType));
 void	 (*addCallback) ___PROTO((c_Undo *This, 
				  void (*callback)(), void *cb_data));
 boolean (*saveState) ___PROTO((c_Undo *This, int fname));
 void	 (*registerPixmap) ___PROTO((c_Undo *This, Pixmap pixmap));
 boolean (*unregisterPixmap) ___PROTO((c_Undo *This, Pixmap pixmap));
} sf_Undo;

struct sc_Undo
{
 sf_Undo	*f;
  
 int		save_pid;

 undo_wid_t	*wList;
 void		(*callback)();	/* called to re-state interface */
 void		*cb_data;	/* callback data */	

 boolean	sensitive;	/* undo sensitive */
 boolean	onredo;		/* TRUE if on redo */	
 int		fname;		/* function name */

 /*--- Save X changes ---*/
 ItemHandle	*pHandle;
 ItemHandle	*gHandle;
 undo_pix_t	*pList;		/* pixmap (un)registered list */ 
 undo_gc_t	*gList;		/* gc cache list */ 
 
};

extern sf_Undo fc_Undo;

extern c_Undo *GlobUndo;
#define FLUSH_UNDO F(GlobUndo).saveState(GlobUndo, 0)
#define SET_UNDO(res) F(GlobUndo).saveState(GlobUndo, res)
#define GET_UNDO (GlobUndo->fname)

#define REGISTER_PIXMAP(pixm) F(GlobUndo).registerPixmap(GlobUndo, pixm)
#define UNREGISTER_PIXMAP(pixm) F(GlobUndo).unregisterPixmap(GlobUndo, pixm)

#endif






