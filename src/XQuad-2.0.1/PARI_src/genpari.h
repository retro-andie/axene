#ifndef _genpari_h
#define _genpari_h

#ifdef ___mips
#define const 
#endif

#ifndef STDLIB_NOT_EXIST
#include        <stdlib.h>	/* malloc, free, atoi */
#endif

#include	"types.h"  /*define for ulong problem on mips*/
/*#include        <stdarg.h>*/
#include        <setjmp.h>
#include        <signal.h>
#include        <stdio.h>
#include        <string.h>
#include        <math.h>
#undef		isdigit
#include        <ctype.h>

#include "gencom.h"
#include "erreurs.h"
#include "genport.h"
#ifdef __cplusplus
#include "mpin.h"
#else
#include "mpansi.h"
#endif

#endif
