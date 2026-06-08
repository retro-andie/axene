/*
** ObjStd.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Global/
** Standard definition for all classes
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
** Started on  Wed Sep 14 19:12:44 1994 Stéphane Boisson
** Last update Tue Oct  3 13:06:40 1995 Stéphane Boisson
*/

#ifndef _ObjStd_h
#define _ObjStd_h
/*
 * This Macro are defined without the end ';'
 * So it can be used like the C standard function
 */ 

/* short cut for standard function definition */
#define F_STD		\
void	*(*cons)();	\
void	(*dest)();	\
void	*(*copy)()

#ifdef ___PSEUDO_COMPOSE_MACRO
#define FUNCTION(a,m) (*f##a.m)

/* short cut for constructor function call */
#define NEW(a) (*f##a.cons)

/* short cut for destructor function call */
#define DELETE(a) (*f##a.dest)

/* short cut for copy function call */
#define COPY(a) (*f##a.copy)

#else

#ifdef ___GNUC_COMPOSE_MACRO
#define FUNCTION(a,m) (*f##a##.##m##)

/* short cut for constructor function call */
#define NEW(a) (*f##a##.cons)

/* short cut for destructor function call */
#define DELETE(a) (*f##a##.dest)

/* short cut for copy function call */
#define COPY(a) (*f##a##.copy)

#else

#define FUNCTION(a,m) (*f/**/a/**/./**/m/**/)

/* short cut for constructor function call */
#define NEW(a) (*f/**/a/**/.cons)

/* short cut for destructor function call */
#define DELETE(a) (*f/**/a/**/.dest)

/* short cut for copy function call */
#define COPY(a) (*f/**/a/**/.copy)

#endif
#endif /* PSEUDO */

/* short cut for function adressing */
#define F(a)	(*((a)->f))

/* short cut for data adressing */
#define D(a)	(*(a))

#endif

