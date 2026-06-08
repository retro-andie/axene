/*
** Dispatch_To_Page.c for XAllWrite in PageArea/
** Dispatch function to a page function
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
** Started on  Mon Jun 16 15:59:26 1997 Emmanuel Paris
** Last update Mon Jun 16 17:38:18 1997 Emmanuel Paris
*/

#define _DTP_PRIVATE_
#include "Dispatch_To_Page.h"

void init_Dispatch_To_Page();
status_t eventlook_Dispatch_To_Page();
void abort_Dispatch_To_Page();
void redraw_Dispatch_To_Page();

void init_Dispatch_To_Page(This)
c_PageArea *This;
{
 This->function_data = (void *)1;	/* force ABORT to be called */
 INIT(This->ActivePage);
}

status_t eventlook_Dispatch_To_Page(This,event)
c_PageArea *This;
XEvent	event;
{ 
 status_t	retour_func;

 retour_func = EVENTLOOK(This->ActivePage, event);
 return retour_func;
}

void abort_Dispatch_To_Page(This)
c_PageArea *This;
{ 
 ABORT(This->ActivePage);
 This->function_data = NULL;
}

void redraw_Dispatch_To_Page(This)
c_PageArea *This;
{ 
 REDRAW(This->ActivePage);
}
