/*
** Drag_Cadre_Macros.h for Xclamation in Page/
** definition for Drag_Cadre' functions
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
** Started on  Sat Jan 14 23:06:43 1995 Stéphane Boisson
** Last update Fri Mar 22 11:19:55 1996 Emmanuel Paris
*/

#ifndef _Drag_Cadre_Macros_h
#define _Drag_Cadre_Macros_h

#if 0

#ifdef __GNUC__
#define DROP_FLAGS(a)	(##a##).xclient.data.l[0]
#define DROP_VALUE(a)	(##a##).xclient.data.l[1]
#define DROP_ID(a)	(##a##).xclient.data.l[2]
#define DROP_DATA1(a)	(##a##).xclient.data.l[3]
#define DROP_DATA2(a)   (##a##).xclient.data.l[4]
#ifndef LONG_IS_64BIT 
#define DROP_DATA(a)	(##a##).xclient.data.l[4]
#define SET_DROP_DATA(a,b)      DROP_DATA1(##a##)=0; \
				DROP_DATA2(##a##)=(int32_t)(##b##);
#else
#define DROP_DATA(a)	((((##a##).xclient.data.l[3])<<32)+\
			(##a##).xclient.data.l[4])
#define SET_DROP_DATA(a,b)   \
		DROP_DATA1(##a##)=(int32_t)((long)(##b##)>>32); \
		DROP_DATA2(##a##)=(int32_t)((long)(##b##)&0xffffffff);
#endif

#else
#define DROP_FLAGS(a)	(/**/a/**/).xclient.data.l[0]
#define DROP_VALUE(a)	(/**/a/**/).xclient.data.l[1]
#define DROP_ID(a)	(/**/a/**/).xclient.data.l[2]
#define DROP_DATA1(a)	(/**/a/**/).xclient.data.l[3]
#define DROP_DATA2(a)   (/**/a/**/).xclient.data.l[4]
#ifndef LONG_IS_64BIT
#define DROP_DATA(a)	((/**/a/**/).xclient.data.l[4])
#define SET_DROP_DATA(a,b)	DROP_DATA1(/**/a/**/)=0; \
				DROP_DATA2(/**/a/**/)=(int32_t)(/**/b/**/);
#else
#define DROP_DATA(a)	((((/**/a/**/).xclient.data.l[3])<<32)+\
			(/**/a/**/).xclient.data.l[4])
#define SET_DROP_DATA(a,b)   \
		DROP_DATA1(/**/a/**/)=(int32_t)((long)(/**/b/**/)>>32); \
		DROP_DATA2(/**/a/**/)=(int32_t)((long)(/**/b/**/)&0xffffffff);
#endif			     

#endif /* __GNUC__ */

#else /* 0 */

#define DROP_FLAGS(a)	(a).xclient.data.l[0]
#define DROP_VALUE(a)	(a).xclient.data.l[1]
#define DROP_ID(a)	(a).xclient.data.l[2]
#define DROP_DATA1(a)	(a).xclient.data.l[3]
#define DROP_DATA2(a)   (a).xclient.data.l[4]
#ifndef LONG_IS_64BIT
#define DROP_DATA(a)	(a).xclient.data.l[4]
#define SET_DROP_DATA(a,b)      DROP_DATA1(a)=0; DROP_DATA2(a)=(int32_t)(b);
#else
#define DROP_DATA(a)	((((a).xclient.data.l[3])<<32)+ (a).xclient.data.l[4])
#define SET_DROP_DATA(a,b)   \
                DROP_DATA1(a)=(int32_t)((long)(b)>>32); \
		DROP_DATA2(a)=(int32_t)((long)(b)&0xffffffff);
#endif

#endif /* 0 */

#endif /* !_Drag_Cadre_Macros_h */




