/*
** Set_New_Priority.h for XAllWrite in FrameManager/
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
** Started on  Mon Jul 17 19:58:07 1995 Emmanuel Paris
** Last update Mon Jun 24 14:12:11 1996 Emmanuel Paris
*/

#ifndef _Set_New_Priority_h
#define _Set_New_Priority_h

extern void set_lower_priority();
extern void set_higher_priority();
extern void set_lowest_priority();
extern void set_highest_priority();

extern void set_priority_Cadre();            
extern int get_priority_Cadre();
extern void lower_priority();
extern void higher_priority();
extern void lowest_priority();
extern void highest_priority();
extern int get_highest_priority();
extern int get_lowest_priority();	

#endif
