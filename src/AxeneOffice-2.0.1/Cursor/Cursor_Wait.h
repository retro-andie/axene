/*
** Cursor_Wait.h for AxeneOffice in Cursor/
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
** Started on  Mon Jul 17 14:20:58 1995 Emmanuel Paris
** Last update Wed Apr  9 16:12:40 1997 Emmanuel Paris
*/

#ifndef _Cursor_Wait_
#define _Cursor_Wait_

#ifndef NB_CURSOR
#define NB_CURSOR 0
#endif

enum {
  C_WAIT1 = NB_CURSOR,
  C_WAIT2,
  C_WAIT3,
  C_WAIT4,
  C_WAIT5,
  C_WAIT6,
  C_WAIT7,
  C_WAIT8,
  C_WAIT9,
  C_WAIT10,
  LAST_CURSOR_WAIT
};

#undef NB_CURSOR
#define	NB_CURSOR LAST_CURSOR_WAIT

#ifdef _PRIVATE_CURSOR_

#include "../Pixmaps/globe01.zpm"
#include "../Pixmaps/globe02.zpm"
#include "../Pixmaps/globe03.zpm"
#include "../Pixmaps/globe04.zpm"
#include "../Pixmaps/globe05.zpm"
#include "../Pixmaps/globe06.zpm"
#include "../Pixmaps/globe07.zpm"
#include "../Pixmaps/globe08.zpm"
#include "../Pixmaps/globe09.zpm"
#include "../Pixmaps/globe10.zpm"

#endif

#define CURSOR_SET_WAIT \
{ DEFINED_CURSOR_XPM, (void *)globe01_xpm,				      \
    STANDARD_CURSOR, (void *)XC_watch},		/* C_WAIT1 */		      \
{ DEFINED_CURSOR_XPM, (void *)globe02_xpm,				      \
    STANDARD_CURSOR, (void *)XC_watch},		/* C_WAIT2 */		      \
{ DEFINED_CURSOR_XPM, (void *)globe03_xpm,				      \
    STANDARD_CURSOR, (void *)XC_watch},		/* C_WAIT3 */		      \
{ DEFINED_CURSOR_XPM, (void *)globe04_xpm,				      \
    STANDARD_CURSOR, (void *)XC_watch},		/* C_WAIT4 */		      \
{ DEFINED_CURSOR_XPM, (void *)globe05_xpm,				      \
    STANDARD_CURSOR, (void *)XC_watch},		/* C_WAIT5 */		      \
{ DEFINED_CURSOR_XPM, (void *)globe06_xpm,				      \
    STANDARD_CURSOR, (void *)XC_coffee_mug},	/* C_WAIT6 */		      \
{ DEFINED_CURSOR_XPM, (void *)globe07_xpm,				      \
    STANDARD_CURSOR, (void *)XC_coffee_mug},	/* C_WAIT7 */		      \
{ DEFINED_CURSOR_XPM, (void *)globe08_xpm,				      \
    STANDARD_CURSOR, (void *)XC_coffee_mug},	/* C_WAIT8 */		      \
{ DEFINED_CURSOR_XPM, (void *)globe09_xpm,				      \
    STANDARD_CURSOR, (void *)XC_coffee_mug},	/* C_WAIT9 */		      \
{ DEFINED_CURSOR_XPM, (void *)globe10_xpm,				      \
    STANDARD_CURSOR, (void *)XC_coffee_mug},	/* C_WAIT10 */		      

#endif






