/*
** XErrorHandler.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in XErrorHandler/
** Definition of the class XErrorHandler
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
** Started on  Thu Oct 13 17:10:06 1994 Marc Blet
** Last update Tue Jan 20 16:37:12 1998 Robin Castanier
*/

#ifndef _XErrorHandler_h
#define _XErrorHandler_h

#include "xcalibur.h"
#include <X11/Xlib.h>

#define XcXH_IO_ERROR_CODE -1

/* ----------------------------------------------------------------- ** 
** define the method for XErrorHandler Class                         ** 
** ----------------------------------------------------------------- */
typedef struct
{
 F_STD;
 void	(*Add_Callback)();
 void	(*Remove_Callback)();
} sf_XErrorHandler;

typedef struct XError_Callback_s
{
 int				(*Callback)();
 void				*Callback_Data;
 int				Reason;
 struct XError_Callback_s	*Next;
 struct XError_Callback_s	*Prev;
} XError_Callback_t;
/* ----------------------------------------------------------------- ** 
** define the XErrorHandler Class                                    ** 
** ----------------------------------------------------------------- */
typedef struct
{
 sf_XErrorHandler	*f;
 XError_Callback_t	*Callback_List;
 
} c_XErrorHandler;

extern sf_XErrorHandler fc_XErrorHandler;

#endif




