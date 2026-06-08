/*
** SignalHandler.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in SignalHandler/
** Definition of the class SignalHandler
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
** Started on  Thu Oct 13 17:10:06 1994 Emmanuel Paris
** Last update Tue Jan 20 16:43:41 1998 Robin Castanier
*/

#ifndef _SignalHandler_h
#define _SignalHandler_h

#include "xcalibur.h"
#include <signal.h>
#include <X11/Xlib.h>

/* ----------------------------------------------------------------- ** 
** define the method for SignalHandler Class                         ** 
** ----------------------------------------------------------------- */
typedef struct
{
 F_STD;
 void	(*Add_Callback)();
 void	(*Remove_Callback)();
 void	(*Modify_Callback)();
} sf_SignalHandler;

typedef struct Signal_Callback_s
{
 struct Signal_Callback_s	*Next;
 void				(*Callback)();
 void				*Callback_Data;
 int				Reason;
} Signal_Callback_t;
/* ----------------------------------------------------------------- ** 
** define the SignalHandler Class                                    ** 
** ----------------------------------------------------------------- */
typedef struct
{
 sf_SignalHandler	*f;
 Signal_Callback_t	*Callback_List;
} c_SignalHandler;

extern sf_SignalHandler fc_SignalHandler;

#endif




