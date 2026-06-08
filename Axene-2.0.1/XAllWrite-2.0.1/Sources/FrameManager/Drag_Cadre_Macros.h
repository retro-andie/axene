/*
 * Drag_Cadre_Macros.h    - definition for Drag_Cadre' functions
 *
 * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
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




