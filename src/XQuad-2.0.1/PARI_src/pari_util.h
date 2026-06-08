/* 
** Moteur.h for tablor in /PARI_src
** Object and methods defintion for class Moteur
** 
** Copyright (C) 1994, XCalibur-PAO. All Rights reserved.
** Login <xcalibur@axene.org>
** 
** Started on  Wed May 17 14:19:18 1995 Gudul
** Last update Fri May 16 17:36:46 1997 Gudul
** 
*/

#ifndef PARI_UI_H
#define PARI_UI_H

#include "xcalibur.h"

#ifndef GEN_DEFINED
#define GEN_DEFINED
typedef long    *GEN;
#endif /* GEN_DEFINED */

extern  unsigned long    avma,bot,top;

GEN str2g ___PROTO((char *t));
char *g2str ___PROTO((GEN x, long dec));
char *g2estr ___PROTO((GEN x, long dec));
GEN gcopy ___PROTO((GEN x));
double gtodouble ___PROTO((GEN x));
double gtodouble2 ___PROTO((GEN x));

long taille ___PROTO((GEN x)); /* gen2.c */
GEN brutcopy ___PROTO((GEN x, GEN y)); /* gen2.c */
GEN lisexpr2 ___PROTO((char *t)); /* anal.c */
void str2g2 ___PROTO((char *t, GEN *value)); /* anal.c */
void gduplicate ___PROTO((GEN x, GEN *value)); /* anal.c */

GEN dbltor ___PROTO((double x));
GEN gadd ___PROTO((GEN x, GEN y));
GEN gsub ___PROTO((GEN x, GEN y));
GEN gmul ___PROTO((GEN x, GEN y));
GEN gdiv ___PROTO((GEN x, GEN y));

/*#define BIG_PREC	-1*/
#define BIG_PREC	22
#define LOW_PREC	20

#endif /* PARI_UI_H */

