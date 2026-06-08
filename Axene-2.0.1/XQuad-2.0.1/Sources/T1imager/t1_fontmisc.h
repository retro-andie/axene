/*
 * $XConsortium: fontmisc.h,v 1.5 91/05/12 16:15:48 rws Exp $
 *
 * Copyright 1991 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

#ifndef _FONTMISC_H_
#define _FONTMISC_H_

#include "MemoryManager.h"
#include "t1_port.h"

typedef unsigned char  *pointer;
#ifndef Bool
typedef int		Bool;
#endif

#ifndef X_PROTOCOL
typedef uint32_t	Atom;
typedef uint32_t	XID;
#endif

#ifndef LSBFirst
#define LSBFirst	0
#define MSBFirst	1
#endif

#ifndef None
#define None	0l
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

extern char	*NameForAtom ();
extern char	*t1imager_buffer;    

#define xalloc(n)   (t1imager_buffer = Xc_malloc("Type1-Imager",(size_t) n))
#define xfree(p)    Xc_free((pointer) p)
/*#define xrealloc(p,n)	realloc ((pointer)p,n)*/
#define lowbit(x) ((x) & (~(x) + 1))

extern void addmemory ___PROTO((long *addr, long size));
extern void delmemory();
extern Bool fontfcnA ___PROTO((char *env, int  *mode));
extern void QueryFontLib ___PROTO((char *env, char *infoName,
				   pointer infoValue, int  *rcodeP));

#endif /* _FONTMISC_H_ */






