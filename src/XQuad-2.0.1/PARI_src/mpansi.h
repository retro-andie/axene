#include "mpdefs.h"

GEN cgeti ___PROTO((long x)),cgetr ___PROTO((long x)),stoi ___PROTO((long x));
GEN cgetg ___PROTO((long x, long y)),negi ___PROTO((GEN x)),negr ___PROTO((GEN x)),absi ___PROTO((GEN x)),absr ___PROTO((GEN x));
GEN shiftr ___PROTO((GEN x, long n)),mpadd ___PROTO((GEN x, GEN y)),subii ___PROTO((GEN x, GEN y));
GEN subrr ___PROTO((GEN x, GEN y)),subir ___PROTO((GEN x, GEN y)),subri ___PROTO((GEN x, GEN y));
GEN mpsub ___PROTO((GEN x, GEN y)),subsi ___PROTO((long x, GEN y)),subsr ___PROTO((long x, GEN y));
GEN subss ___PROTO((long x, long y)),mpmul ___PROTO((GEN x, GEN y));
GEN divss ___PROTO((long x, long y)),mpdiv ___PROTO((GEN x, GEN y)),dvmdss ___PROTO((long x, long y, GEN *z));
GEN ressi ___PROTO((long x, GEN y)),modis ___PROTO((GEN x, long y)),resis ___PROTO((GEN x, long y));
double  gtodouble ___PROTO((GEN x));
long itos ___PROTO((GEN x)),divisii ___PROTO((GEN x, long y, GEN z)),vali ___PROTO((GEN x));
int expi ___PROTO((GEN x)),cmpir ___PROTO((GEN x, GEN y)),mpcmp ___PROTO((GEN x, GEN y));
int cmpsr ___PROTO((long x, GEN y)),mpdivis ___PROTO((GEN x, GEN y, GEN z)),divise ___PROTO((GEN x, GEN y));
void affii ___PROTO((GEN x, GEN y)),mpaff ___PROTO((GEN x, GEN y)),affsi ___PROTO((long s, GEN x));
void affsr ___PROTO((long s, GEN x)),addssz ___PROTO((long x, long y, GEN z));
void mulssz ___PROTO((long x, long y, GEN z)),mulsii ___PROTO((long x, GEN y, GEN z));
void addsii ___PROTO((long x, GEN y, GEN z)),dvmdssz ___PROTO((long x, long y, GEN z, GEN t));
void dvmdsiz ___PROTO((long x, GEN y, GEN z, GEN t)),dvmdisz ___PROTO((GEN x, long y, GEN z, GEN t));
void dvmdiiz ___PROTO((GEN x, GEN y, GEN z, GEN t)),divisz ___PROTO((GEN x, long y, GEN z));
void divsiz ___PROTO((long x, GEN y, GEN z)),divssz ___PROTO((long x, long y, GEN z));
void divrrz ___PROTO((GEN x, GEN y, GEN z)),resiiz ___PROTO((GEN x, GEN y, GEN z));

/* mp.c ou mp.s */

GEN     gerepile ___PROTO((long l, long p, GEN q)), icopy ___PROTO((GEN x)), rcopy ___PROTO((GEN x));
GEN     mptrunc ___PROTO((GEN x)),mpent ___PROTO((GEN x)),shifts ___PROTO((long x, long y)),shifti ___PROTO((GEN x, long n));
GEN     addsi ___PROTO((long x, GEN y)),addsr ___PROTO((long x, GEN y)),addii ___PROTO((GEN x, GEN y)),addir ___PROTO((GEN x, GEN y)),addrr ___PROTO((GEN x, GEN y)), addss ___PROTO((long x, long y));
GEN     mulss ___PROTO((long x, long y)),mulsi ___PROTO((long x, GEN y)),mulsr ___PROTO((long x, GEN y)),mulii ___PROTO((GEN x, GEN y)),mulir ___PROTO((GEN x, GEN y)),mulrr ___PROTO((GEN x, GEN y));
GEN     divsi ___PROTO((long x, GEN y)),divis ___PROTO((GEN y, long x)),divsr ___PROTO((long x, GEN y)),divrs ___PROTO((GEN x, long y)),divir ___PROTO((GEN x, GEN y));
GEN     divri ___PROTO((GEN x, GEN y)),divrr ___PROTO((GEN x, GEN y)),convi ___PROTO((GEN x)),confrac ___PROTO((GEN x));
GEN     modss ___PROTO((long x, long y)),resss ___PROTO((long x, long y)),modsi ___PROTO((long x, GEN y)),modii ___PROTO((GEN x, GEN y));
GEN     dvmdii ___PROTO((GEN x, GEN y, GEN *z)),dvmdsi ___PROTO((long x, GEN y, GEN *z)),dvmdis ___PROTO((GEN x, long y, GEN *z));
long    vals ___PROTO((long x));
int     cmpss ___PROTO((long x, long y)),cmpsi ___PROTO((long x, GEN y)),cmpii ___PROTO((GEN x, GEN y)),cmprr ___PROTO((GEN x, GEN y));
void    affir ___PROTO((GEN x, GEN y)),affrr ___PROTO((GEN x, GEN y)),diviiz ___PROTO((GEN x, GEN y, GEN z));
void    cgiv ___PROTO((GEN x)),mpdivz ___PROTO((GEN x, GEN y, GEN z)),modiiz ___PROTO((GEN x, GEN y, GEN z));
void    modiiz ___PROTO((GEN x, GEN y, GEN z));
GEN g_shifts ___PROTO((long x, long y));



