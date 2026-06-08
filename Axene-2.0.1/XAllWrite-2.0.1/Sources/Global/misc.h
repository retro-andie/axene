/*
** misc.h for XAllWrite in Global/
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
** Started on  Mon Jul 17 17:34:52 1995 Emmanuel Paris
** Last update Sat Feb 21 18:15:33 1998 Emmanuel Paris
*/

#ifndef _misc_h_
#define _misc_h_

#include "global_defs.h"
#include <X11/Xlib.h>

#if (defined(__GNUC__) || defined(___iris4d) || defined(___sco386) || defined(___sco386sV)) && !defined(STDLIB_NOT_EXIST)
# include <string.h>
# include <stdlib.h>
#else
char *getenv ___PROTO((char *));
# if defined(STDC_HEADERS) || defined(HAVE_STRING_H)
#  include <string.h>
#  ifndef rindex
#   define rindex strrchr
#  endif
# else
#  include <strings.h>
# endif
#endif

/* global buffer for big XDrawPoints, XDrawSegments request */
extern XSegment *segments;
extern XPoint *points;

/* compare 2 string and return TRUE or FALSE */
extern boolean Xstrcmp();

/* compare 2 string with n char and return TRUE or FALSE */
extern boolean Xstrncmp();

/* search the char c in the string chaine at reverse */
extern int Xstrnchr();

/* search string 2 in string 1 and return position or NULL */
extern char *Xstrstr();

/* alternative function to setenv */
extern int Xsetenv();

/* alternative function to rint */
extern double Xrint();

/* trie un tableau 2 parametres: 		*/
/*   -nombre d'elements  (int)			*/
/*   -pointer du tableau (int *)		*/

extern void TriSimple();
extern void TriSimpleForCoord_t();

int strfind ___PROTO((char *Text, char *Pattern, int Textlen));
extern void Zusleep ___PROTO((long int u_during));
char *Xc_strconcat() /* ___PROTO((...)) */;
char *Xc_strndup ___PROTO((char *desc, char *string, long size));
char iso_code ___PROTO((int code1, int code2));

struct hostent *Xcf_gethostbyname();

#if defined(___alpha) || defined(___ncr_svr4) || defined(___sun4) || defined(___sco386)
# define Xc_gethostbyname Xcf_gethostbyname
#else
# define Xc_gethostbyname gethostbyname
#endif

#if 1			/* define getgecos macro */
#include <pwd.h>
#define Xc_getgecos getpwuid(getuid())->pw_gecos
#else
#define Xc_getgecos "unknown"
#endif

#endif
