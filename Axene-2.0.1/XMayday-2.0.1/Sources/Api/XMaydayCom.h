/*
** XMaydayCom.h for XMayday in Api/
** XMaydayCom.h
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Sometime during     1996 One of the authors
** Last update Sun Jul 13 15:59:22 1997 One of the authors
*/


#ifndef _XMaydayCom_h_
#define _XMaydayCom_h_

#include <X11/Xlib.h>
#include "XMayday.h"

extern void xmayday_remote_init_atoms();
extern void xmayday_remote_reset();
extern Window xmayday_remote_find_window();
extern boolean xmayday_remote_obtain_lock();
extern boolean xmayday_remote_free_lock();
extern boolean xmayday_remote_command();

#endif /* _XMaydayCom_h_ */
