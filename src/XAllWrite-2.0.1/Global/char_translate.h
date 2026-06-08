/*
** char_translate.h for Xclamation, XQuad and XAllWrite in Global/
** macro definition for removing accent, to_upper and to_lower function
** that work with iso-latin1 char encoding.
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
** Started on  Wed Jan 31 23:15:33 1996 Emmanuel Paris
** Last update Tue Mar 24 15:55:14 1998 Emmanuel Paris
*/

#ifndef _char_translate_h_
#define _char_translate_h_

#include "xcalibur.h"

extern unsigned char ct_suppress_accent[256];
extern unsigned char ct_to_upper[256];
extern unsigned char ct_to_lower[256];
extern unsigned char ct_type[256];
extern boolean ct_breakable[256];

#define Xc_UNKNOW	(unsigned char)0
#define Xc_LETTER	(unsigned char)1
#define Xc_DIGIT	(unsigned char)2
#define Xc_PUNCT	(unsigned char)3		
#define Xc_CURRENCY	(unsigned char)4
#define Xc_OPERATOR	(unsigned char)5
#define Xc_MISC		(unsigned char)6

#define Xc_SUPPRESS_ACCENT(a) \
((char)(ct_suppress_accent[(unsigned char)(a)]? \
	ct_suppress_accent[(unsigned char)(a)]:(a)))
#define Xc_TO_UPPER(a) \
((char)(ct_to_upper[(unsigned char)(a)]? \
	ct_to_upper[(unsigned char)(a)]:(a)))
#define Xc_TO_LOWER(a) \
((char)(ct_to_lower[(unsigned char)(a)]? \
	ct_to_lower[(unsigned char)(a)]:(a)))

#define Xc_IS_DIGIT(a) (ct_type[(int)((unsigned char)(a))] == Xc_DIGIT)
#define Xc_IS_LETTER(a) (ct_type[(int)((unsigned char)(a))] == Xc_LETTER)
#define Xc_IS_PUNCTUATION(a) (ct_type[(int)((unsigned char)(a))] == Xc_PUNCT)
#define Xc_IS_OPERATOR(a) (ct_type[(int)((unsigned char)(a))] == Xc_OPERATOR)

#define Xc_IS_BREAKABLE(a) (ct_breakable[(int)((unsigned char)(a))])
/* compare string s1 and string s2 without testing uppercase */
/* (for s1 and s2) and accent (for s2) */
extern boolean Xc_strcmp_parser ___PROTO((char *s1, char *s2));

#endif /* char_translate_h_ */



