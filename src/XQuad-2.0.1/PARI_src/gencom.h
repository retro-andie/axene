/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                                                                 */
/*                        Fichier Include PARI                     */
/*                                                                 */
/*                    commun a toutes les versions                 */
/*                                                                 */
/*                        copyright  Babecool                      */
/*                                                                 */
/*                                                                 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef _global_defs_h_
#ifdef __cplusplus
# define ___PROTO(x) x
#else
# ifdef ___m88k
#  define ___PROTO(x) ()
# else
#  ifdef __STDC__
#   define ___PROTO(x) x
#  else
#   define ___PROTO(x) ()
#  endif
# endif
#endif
#endif
/* don't know how to declare varargs */
#define ___NPROTO(x) ()

#if defined(___rs6000) && defined(ldiv)
#undef ldiv
#endif

#ifdef LONG_IS_32BIT
#define TWOPOTBYTES_IN_LONG  2
#define TWOPOTBITS_IN_LONG   5
#define BYTES_IN_LONG        4
#define BITS_IN_LONG        32
#define BITS_IN_RANDOM      32
#define MAXULONG    0xffffffffL
#define MAXHALFULONG    0xffffL
#define MAXUBYTE          0xffL
#define HIGHBIT     0x80000000L
#define HIGHMASK    0xffff0000L
#define LOWMASK         0xffffL
#define MAXVAR 150
#define MAXVARN 255
#define MAXSHIFTVAR 8
#define DEFAULTPREC 4
#define MEDDEFAULTPREC 6
#define BIGDEFAULTPREC 8
#endif

#ifdef LONG_IS_64BIT
#define TWOPOTBYTES_IN_LONG          3
#define TWOPOTBITS_IN_LONG           6
#define BYTES_IN_LONG                8
#define BITS_IN_LONG                64
/* if you prefer a 64-bit random number generator, simply
   change 32 to 64 in the line below */
#define BITS_IN_RANDOM              32
#define MAXULONG    0xffffffffffffffffL
#define MAXHALFULONG        0xffffffffL
#define MAXUBYTE                0xffffL
#define HIGHBIT     0x8000000000000000L
#define HIGHMASK    0xffffffff00000000L
#define LOWMASK             0xffffffffL
#define MAXVAR 8100
#define MAXVARN 8191
#define MAXSHIFTVAR 13
#define DEFAULTPREC 3
#define MEDDEFAULTPREC 4
#define BIGDEFAULTPREC 5
#endif

#ifdef LONG_IS_32BIT
#define SIGNBITS 0xff000000L
#define SIGNSHIFT 24
#define TYPBITS 0xff000000L
#define TYPSHIFT 24
#define PEREBITS 0xff0000L
#define PERESHIFT 16
#define LGBITS 0xffffL
#define LGEFBITS 0xffffL
#define EXPOBITS 0xffffffL
#define HIGHEXPOBIT 0x800000L
#define VALPBITS 0xffffL
#define HIGHVALPBIT 0x8000L
#define PRECPBITS 0xffff0000L
#define PRECPSHIFT 16
#define VARNBITS 0xff0000L
#define VARNSHIFT 16
#endif

#ifdef LONG_IS_64BIT
#define SIGNBITS  0xffff000000000000L
#define SIGNSHIFT 48
#define TYPBITS   0xffff000000000000L
#define TYPSHIFT  48
#define PEREBITS      0xffff00000000L
#define PERESHIFT 32
#define LGBITS            0xffffffffL
#define LGEFBITS          0xffffffffL
#define EXPOBITS      0xffffffffffffL
#define HIGHEXPOBIT   0x800000000000L
#define VALPBITS          0xffffffffL
#define HIGHVALPBIT       0x80000000L
#define PRECPBITS 0xffffffff00000000L
#define PRECPSHIFT 32
#define VARNBITS      0xffff00000000L
#define VARNSHIFT  32
#endif

#define BITS_IN_HALFULONG (BITS_IN_LONG/2)

#define HIGHWORD(a) ((a) >> BITS_IN_HALFULONG)
/* si le compilateur est bugge, il faut mettre
 ((a >> BITS_IN_HALFULONG) & LOWMASK) */
#define LOWWORD(a) ((a) & LOWMASK)

#ifndef GEN_DEFINED
#define GEN_DEFINED
typedef long    *GEN;
#endif

#ifdef ULONG_NOT_DEFINED
typedef unsigned long ulong;
#endif

typedef struct entree {
  char *name;
  long valence;
  void *value;
  long menu;
  struct entree *next;
} entree;

typedef unsigned char *byteptr;

/*      Variables statiques communes :
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  */

extern  ulong overflow,hiremainder;

extern  long    prec,precdl,defaultpadicprecision;
extern  GEN     bernzone,gpi,geuler;
extern  long    tglobal,paribuffsize,pariecho;
extern  long    *ordvar,varchanged;
extern  GEN     polvar;
extern  GEN     RAVYZARC;

extern  long    NUMFUNC;
extern  entree  fonctions[],**hashtable;
extern  long    lontyp[],lontyp2[];

extern  jmp_buf environnement;
extern  FILE    *outfile, *logfile, *infile, *errfile;

extern  ulong    avma,bot,top;
extern  GEN     gnil,gun,gdeux,ghalf,gi,gzero;

extern  GEN     *polun,*polx;
extern  byteptr diffptr;
extern  GEN     primetab;

extern  GEN     *g;
extern  entree  **varentries; /* noms des inconnues actives */
extern  GEN     *blocliste;   /* tableau des pointeur de blocs */
extern  long    nvar;         /* numero de la prochaine inconnue */
extern  long    glbfmt[];
extern  long    **rectgraph;
extern  long    pari_randseed;
extern  long    DEBUGLEVEL;

#ifndef INIT_CONST
#define INIT_CONST
extern  int STACKSIZE;  /* nombre de gn possibles */
extern  int TBLSZ;  /* taille de la table de hashcodes */
extern  int MAXBLOC;  /* nombre de blocs autorises dans le tas */
extern  int NUMPRTBELT; /* taille table de premiers prives */

extern  double K;             /* 32*log(2)/log(10)    */
extern  double K1;             /* log(10)/(32*log(2))  */
extern  double K2;               /* 1/(1-(log(2)/(2*pi)))*/
extern  double K4;            /* e*pi/16              */
extern  double LOG2;       /* log(2)               */
extern  double L2SL10;     /* log(2)/log(10)       */
#ifndef  PI
extern  double PI;          /* pi                   */
#endif
extern  double rac5;           /* racine de 5          */
extern  double C1;            /* log(2*pi)/2          */
extern  double C2;             /* 32*log(2)            */
extern  double C3;     /* log((1+sqrt(5))/2)/(32*log(2)) */
extern  double C31;           /* 2^31                 */
extern  long BIGINT;                  /* 2^15-1               */
extern  long EXP220;            /* 2^20                 */
extern  long VERYBIGINT;        /* 2^31-1               */
#endif

extern  char    *helpmessage[]; /* un message pour chaque fonction predefinie */
/*extern  char    *errmessage[]; */ /* un par numero d'erreur */
extern  char    *pariversion;
#ifdef HAVEEXP2
#ifdef __cplusplus
extern "C" {
  double exp2(double);
  double log2(double);
}
#endif
#else
#ifdef __cplusplus
inline double exp2(double x) {return exp(x*LOG2);}
inline double log2(double x) {return log(x)/LOG2;}
#else
#define exp2(x) exp((double)(x)*LOG2)
#define log2(x) log((double)(x)/LOG2)
#endif
#endif

#ifdef LONG_IS_32BIT
#undef labs
#define labs(x) abs(x)
#endif

#define min(a,b) ((a)>(b)?(b):(a))
#define max(a,b) ((a)>(b)?(a):(b))

#define separe(c)     ((c==';')||(c==':')||(c=='\n'))

#define output(x)     {bruteall(x,'g',-1,1);pariputc('\n');fflush(stdout);}
#define outmat(x)     {matbrute(x,'g',-1);pariputc('\n');fflush(stdout);}
#define outbeaut(x)   {sor(x,'g',-1,0);pariputc('\n');fflush(stdout);}

#define addis(x,s)  addsi(s,x)
#define addrs(x,s)  addsr(s,x)
#define mulis(x,s)  mulsi(s,x)
#define mulri(x,s)  mulir(s,x)
#define mulrs(x,s)  mulsr(s,x)

#define gval(x,v) ggval(x,polx[v])
#define gvar9(x) ((typ(x)==9)?gvar2(x):gvar(x))

#define lgeti   (long)cgeti
#define lgetr   (long)cgetr
#define lpile   (long)gerepile
#define lstoi   (long)stoi
#define lnegi   (long)negi
#define lnegr   (long)negr
#define lmpneg  (long)mpneg
#define labsi   (long)absi
#define labsr   (long)absr
#define lmpabs  (long)mpabs
#define lmptrunc (long)mptrunc
#define lmpent  (long)mpent
#define lshifts (long)shifts
#define lshifti (long)shifti
#define lshiftr (long)shiftr
#define lmpshift (long)mpshift
#define laddsi  (long)addsi
#define laddsr  (long)addsr
#define laddis  (long)addis
#define laddrs  (long)addrs
#define laddii  (long)addii
#define laddir  (long)addir
#define laddrr  (long)addrr
#define lmpadd  (long)mpadd
#define lsubsi  (long)subsi
#define lsubis  (long)subis
#define lsubsr  (long)subsr
#define lsubrs  (long)subrs
#define lsubii  (long)subii
#define lsubir  (long)subir
#define lsubri  (long)subri
#define lsubrr  (long)subrr
#define lmpsub  (long)mpsub
#define lmulss  (long)mulss
#define lmulsi  (long)mulsi
#define lmulsr  (long)mulsr
#define lmulis  (long)mulis
#define lmulrs  (long)mulrs
#define lmulii  (long)mulii
#define lmulir  (long)mulir
#define lmulri  (long)mulri
#define lmulrr  (long)mulrr
#define lmpmul  (long)mpmul
#define ldivsi  (long)divsi
#define ldivis  (long)divis
#define ldivsr  (long)divsr
#define ldivrs  (long)divrs
#define ldivii  (long)divii
#define ldivir  (long)divir
#define ldivri  (long)divri
#define ldivrr  (long)divrr
#define lmpdiv  (long)mpdiv
#define lmodii  (long)modii
#define lresii  (long)resii
#define ldvmdii (long)dvmdii
#define ldvmdsi (long)dvmdsi
#define ldvmdis (long)dvmdis
  
#define ltree   (long)gettree
#define lgen    (long)getgen
#define lcopy   (long)gcopy
#define lclone  (long)gclone
#define lgetg   (long)cgetg
#define lgetp   (long)cgetp
#define laddpex (long)gaddpex
#define lgreffe (long)greffe
#define lopsg2  (long)gopsg2
#define lopgs2  (long)gopgs2
#define lco8    (long)co8
#define lneg    (long)gneg
#define lmax    (long)gmax
#define lmin    (long)gmin
#define ladd    (long)gadd
#define lsub    (long)gsub
#define lmul    (long)gmul
#define ldiv    (long)gdiv
#define linv    (long)ginv
#define lmod    (long)gmod
#define ldivmod (long)gdivmod
#define lshift  (long)gshift
#define lmul2n  (long)gmul2n
#define lpuigs  (long)gpuigs
#define lpui    (long)gpui
#define lsubst  (long)gsubst
#define lderiv  (long)deriv
#define linteg  (long)integ
#define lrecip  (long)recip
#define lceil   (long)gceil
#define lfloor  (long)gfloor
#define lround  (long)ground
#define lcvtoi  (long)gcvtoi
#define lrndtoi (long)grndtoi
#define lfrac   (long)gfrac
#define ltrunc  (long)gtrunc
#define lmodulcp  (long)gmodulcp
#define lmodulo  (long)gmodulo
  
#define lconcat (long)concat
#define lnorm   (long)gnorm
#define lnorml2 (long)gnorml2
#define lconj   (long)gconj
#define lreal   (long)greal
#define limag   (long)gimag
#define lmppi   (long)mppi
#define lmpeuler (long)mpeuler
#define lmpsqrt (long)mpsqrt
#define lsqrt   (long)gsqrt
#define lmpexp1 (long)mpexp1
#define lmpexp  (long)mpexp
#define lexp    (long)gexp
#define lmplog  (long)mplog
#define llog    (long)glog
#define lmpsc1  (long)mpsc1
#define lmpcos  (long)mpcos
#define lcos    (long)gcos
#define lmpsin  (long)mpsin
#define lsin    (long)gsin
#define lmpaut  (long)mpaut
#define lmptan  (long)mptan
#define ltan    (long)gtan
#define lmpatan (long)mpatan
#define latan   (long)gatan
#define lmpasin (long)mpasin
#define lasin   (long)gasin
#define lmpacos (long)mpacos
#define lacos   (long)gacos
#define lmpch   (long)mpch
#define lch     (long)gch
#define lmpsh   (long)mpsh
#define lsh     (long)gsh
#define lmpth   (long)mpth
#define lth     (long)gth
#define lmpath  (long)mpath
#define lath    (long)gath
#define lmpash  (long)mpash
#define lash    (long)gash
#define lmpach  (long)mpach
#define lach    (long)gach
#define lmpgamma (long)mpgamma
#define lgamma  (long)ggamma
#define lmplngamma (long)mplngamma
#define llngamma  (long)glngamma
#define lgamd   (long)ggamd
#define lmppsi  (long)mppsi
#define lpsi    (long)gpsi
#define lmpgamd (long)mpgamd
#define larg    (long)garg
#define lsqr    (long)gsqr
  
#define ltrans  (long)gtrans
#define lscalmat (long)gscalmat
#define lscalsmat (long)gscalsmat
#define laddmat (long)gaddmat
#define laddsmat (long)gaddsmat
#define lgauss  (long)gauss
#define linvmat (long)invmat
#define linvmulmat (long)invmulmat
#define ldet    (long)det
#define ldet2   (long)det2
#define lcaract (long)caract
#define lcaradj (long)caradj
#define ladj    (long)adj
#define ltrace  (long)trace
#define lassmat (long)assmat
#define lscal   (long)gscal
  
#define linvmod (long)ginvmod
#define lred    (long)gred
#define ldeuc   (long)gdeuc
#define lres    (long)gres
#define ldivres (long)poldivres
#define lpoleval (long)poleval
#define lroots  (long)roots
#define lgcd    (long)ggcd
#define lpolgcd (long)polgcd
#define lcontent (long)content
#define lprimpart (long)primpart
#define lpsres  (long)psres
#define lsubres (long)subres
#define ldiscsr (long)discsr
#define lquadpoly (long)quadpoly
#define lquadgen (long)quadgen
  
#define llegendre (long)legendre
#define ltchebi (long)tchebi
#define lhilb   (long)hilb
#define lpasc   (long)pasc
#define lprec   (long)gprec
#define lbinome (long)binome
  
#define lracine (long)racine
#define lmppgcd (long)mppgcd
#define lmpfact (long)mpfact
#define lsfcont (long)sfcont
#define lbezout (long)bezout
#define lmpinvmod (long)mpinvmod
#define lpuissmodulo (long)puissmodulo
#define lfibo   (long)fibo
#define lchangevar (long)changevar
  
#define zero    (long)gzero
#define un      (long)gun
#define deux    (long)gdeux
#define lhalf   (long)ghalf
  
#define lpolx   (long)polx
#define lpolun   (long)polun

#define mpmodz(x,y,z)     (modiiz(x,y,z))
#define mpresz(x,y,z)     (resiiz(x,y,z))
#define mpmod(x,y)        (modii(x,y))
#define mpres(x,y)        (resii(x,y))

#define laddsg(s,y)         (lopsg2(gadd,s,y))
#define laddgs(y,s)         (lopsg2(gadd,s,y))
#define lsubsg(s,y)         (lopsg2(gsub,s,y))
#define lsubgs(y,s)         (lopgs2(gsub,y,s))
#define lmulsg(s,y)         ((long)gmulsg(s,y))
#define lmulgs(y,s)         ((long)gmulsg(s,y))
#define ldivsg(s,y)         (lopsg2(gdiv,s,y))
#define ldivgs(x,s)         ((long)gdivgs(x,s))
#define lmodsg(s,y)         (lopsg2(gmod,s,y))
#define lmodgs(y,s)         (lopgs2(gmod,y,s))
#define ldiventsg(s,y)      (lopsg2(gdivent,s,y))
#define ldiventgs(y,s)      (lopgs2(gdivent,y,s))
#define lminsg(s,y)         (lopsg2(gmin,s,y))
#define lmings(y,s)         (lopgs2(gmin,y,s))
#define lmaxsg(s,y)         (lopsg2(gmax,s,y))
#define lmaxgs(y,s)         (lopgs2(gmax,y,s))


#define mppiz(x)              (gop0z(mppi,x))
#define mpeulerz(x)           (gop0z(mpeuler,x))
#define mpsqrtz(x,y)        (gop1z(mpsqrt,x,y))
#define mpexpz(x,y)         (gop1z(mpexp,x,y))
#define mpexp1z(x,y)        (gop1z(mpexp1,x,y))
#define mplogz(x,y)         (gop1z(mplog,x,y))
#define mpcosz(x,y)         (gop1z(mpcos,x,y))
#define mpsinz(x,y)         (gop1z(mpsin,x,y))
#define autz(x,y)           (gop1z(mpaut,x,y))
#define mptanz(x,y)         (gop1z(mptan,x,y))
#define mpatanz(x,y)        (gop1z(mpatan,x,y))
#define mpasinz(x,y)        (gop1z(mpasin,x,y))
#define mpacosz(x,y)        (gop1z(mpacos,x,y))
#define mpchz(x,y)          (gop1z(mpch,x,y))
#define mpshz(x,y)          (gop1z(mpsh,x,y))
#define mpthz(x,y)          (gop1z(mpth,x,y))
#define mpathz(x,y)         (gop1z(mpath,x,y))
#define mpashz(x,y)         (gop1z(mpash,x,y))
#define mpachz(x,y)         (gop1z(mpach,x,y))
#define mpgammaz(x,y)       (gop1z(mpgamma,x,y))
#define mpargz(x,y,z)      (gop2z(mparg,x,y,z))
#define mpfactz(s,y)        (gops1z(mpfact,s,y))

#define gredz(x,y)          (gop1z(gred,x,y))
#define gnegz(x,y)          (gop1z(gneg,x,y))
#define gabsz(x,prec,y)    (gop2z(gabs,x,prec,y))
#define gmaxz(x,y,z)       (gop2z(gmax,x,y,z))
#define gminz(x,y,z)       (gop2z(gmin,x,y,z))
#define gaddz(x,y,z)       (gop2z(gadd,x,y,z))
#define gsubz(x,y,z)       (gop2z(gsub,x,y,z))
#define gmulz(x,y,z)       (gop2z(gmul,x,y,z))
#define gdivz(x,y,z)       (gop2z(gdiv,x,y,z))
#define gdeucz(x,y,z)      (gop2z(gdeuc,x,y,z))
#define gdiventz(x,y,z)    (gop2z(gdivent,x,y,z))
#define gmodz(x,y,z)       (gop2z(gmod,x,y,z))
#define gshiftz(x,s,z)      (gops2gsz(gshift,x,s,z))
#define gmul2nz(x,s,z)      (gops2gsz(gmul2n,x,s,z))
#define gaddsg(s,y)         (gopsg2(gadd,s,y))
#define gaddgs(y,s)         (gopsg2(gadd,s,y))
#define gsubsg(s,y)         (gopsg2(gsub,s,y))
#define gsubgs(y,s)         (gopgs2(gsub,y,s))
#define gcmpsg(s,y)         (-opgs2(gcmp,y,s))
#define gcmpgs(y,s)         (opgs2(gcmp,y,s))
#define gegalsg(s,y)        (opgs2(gegal,y,s))
#define gegalgs(y,s)        (opgs2(gegal,y,s))
#define gmulgs(y,s)         (gmulsg(s,y))
#define gdivsg(s,y)         (gopsg2(gdiv,s,y))
#define gdiventsg(s,y)      (gopsg2(gdivent,s,y))
#define gdiventgs(y,s)      (gopgs2(gdivent,y,s))
#define gmodsg(s,y)         (gopsg2(gmod,s,y))
#define gmodgs(y,s)         (gopgs2(gmod,y,s))
#define gminsg(s,y)         (gopsg2(gmin,s,y))
#define gmings(y,s)         (gopgs2(gmin,y,s))
#define gmaxsg(s,y)         (gopsg2(gmax,s,y))
#define gmaxgs(y,s)         (gopgs2(gmax,y,s))


#define gaddsgz(s,y,z)    (gopsg2z(gadd,s,y,z))
#define gaddgsz(y,s,z)    (gopsg2z(gadd,s,y,z))
#define gsubsgz(s,y,z)    (gopsg2z(gsub,s,y,z))
#define gsubgsz(y,s,z)    (gopgs2z(gsub,y,s,z))
#define gmulsgz(s,y,z)    (gops2sgz(gmulsg,s,y,z))
#define gmulgsz(y,s,z)    (gops2sgz(gmulsg,s,y,z))
#define gdivsgz(s,y,z)    (gopsg2z(gdiv,s,y,z))
#define gdivgsz(y,s,z)    (gops2gsz(gdivgs,y,s,z))
#define gdiventsgz(s,y,z) (gopsg2z(gdivent,s,y,z))
#define gdiventgsz(y,s,z) (gopgs2z(gdivent,y,s,z))
#define gmodsgz(s,y,z)    (gopsg2z(gmod,s,y,z))
#define gmodgsz(y,s,z)    (gopgs2z(gmod,y,s,z))
#define gminsgz(s,y,z)    (gopsg2z(gmin,s,y,z))
#define gmingsz(y,s,z)    (gopgs2z(gmin,y,s,z))
#define gmaxsgz(s,y,z)    (gopsg2z(gmax,s,y,z))
#define gmaxgsz(y,s,z)    (gopgs2z(gmax,y,s,z))

#define coeff(a,i,j)      (*((long*)(*(a+(j)))+(i)))
#define gcoeff(a,i,j)     (GEN)coeff(a,i,j)
#define bern(i)           (GEN)(bernzone + (i)*(*(bernzone + 2)) + 3)

#define copyifstack(x) (RAVYZARC=(GEN)(x),((RAVYZARC>=(GEN)bot)&&(RAVYZARC<(GEN)top))?lcopy(RAVYZARC):(long)RAVYZARC)

 /* The following definitions suppose well-behaved arrays and matrices */
 
#define aryeltsize(ar,ind) ((ind)<lg(ar)-1?  \
			    ((GEN)((ar)[(ind)+1]))-((GEN)((ar)[ind])): \
			    taille((GEN)((ar)[ind])))

#define mateltsize(mat,c,e) ((e)<lg(mat[c])-1?  \
(((GEN)(((GEN)((mat)[c]))[(e)+1])) - \
 ((GEN)(((GEN)((mat)[c]))[e]))): \
 ((c)<lg(mat)-1? \
  ((GEN)((mat)[(c)+1])) - \
  ((GEN)(((GEN)((mat)[c]))[e])): \
  taille((GEN)((GEN)((mat)[c]))[e])))

#define matcolsize(mat,col) ((col)<lg(mat)-1? \
((GEN)((mat)[(col)+1])) - \
((GEN)((mat)[col])): \
taille((GEN)((mat)[col])))
 
#ifdef __cplusplus
inline int isonstack(GEN x) {GEN RAVYZARC=x; return ((RAVYZARC>=(GEN)bot)&&(RAVYZARC<(GEN)top));}
inline int adecaler(GEN x, long tetpil, long anavma) {GEN RAVYZARC=x;return ((RAVYZARC>=(GEN)anavma)&&(RAVYZARC<(GEN)tetpil));}
#else
#define isonstack(x)   (RAVYZARC=(GEN)(x),((RAVYZARC>=(GEN)bot)&&(RAVYZARC<(GEN)top)))
#define adecaler(x,tetpil,anavma) (RAVYZARC=(GEN)(x),((RAVYZARC>=(GEN)anavma)&&(RAVYZARC<(GEN)tetpil)))
#endif

#define isscalar(x)   ((typ(x)<10)||((typ(x)==10)&&(lgef(x)<=3)))
#define isnonscalar(x)  ((typ(x)==10)&&(lgef(x)>3))
#define leadingterm(x)  ((typ(x)<10)?x:((GEN)(x[lgef(x)-1])))

#ifdef __cplusplus
inline int odd(long x) {return x&1;}
#else
#define odd(x)                ((x) & 1)
#endif
 
#define mpodd(x) (signe(x) && odd(mant(x,lgef(x) - 2)))

#define evalsigne(x) (((long)(x))<<SIGNSHIFT)
#define evaltyp(x) (((ulong)(x))<<TYPSHIFT)
#define evalpere(x) (((ulong)(x))<<PERESHIFT)
#define evallg(x) (x)
#define evallgef(x) (x)
#define evalvarn(x) (((ulong)(x))<<VARNSHIFT)
#define evalprecp(x) (((long)(x))<<PRECPSHIFT)
#define evalexpo(x) (HIGHEXPOBIT+(x))
#define evalvalp(x) (HIGHVALPBIT+(x))

/* alglin.c */
     
GEN     gtrans ___PROTO((GEN x)),gscalmat ___PROTO((GEN x, long n)),gscalsmat ___PROTO((long x, long n)),gaddmat ___PROTO((GEN x, GEN y)),gaddsmat ___PROTO((long s, GEN y)),inverseimage ___PROTO((GEN mat, GEN y));
GEN     ker ___PROTO((GEN x)),keri ___PROTO((GEN x)),kerreel ___PROTO((GEN x, long prec));
GEN     image ___PROTO((GEN x)),imagereel ___PROTO((GEN x, long prec)),imagecompl ___PROTO((GEN x)),image2 ___PROTO((GEN x)),suppl ___PROTO((GEN x)),eigen ___PROTO((GEN x, long prec)),hess ___PROTO((GEN x));
GEN     carhess ___PROTO((GEN x, long v));
GEN     gauss ___PROTO((GEN a, GEN b)),invmat ___PROTO((GEN a)),det ___PROTO((GEN a)),detreel ___PROTO((GEN a)),det2 ___PROTO((GEN a));
GEN     caract ___PROTO((GEN x, int v)),caradj ___PROTO((GEN x, long v, GEN *py)),adj ___PROTO((GEN x)),caradj0 ___PROTO((GEN x, long v)),trace ___PROTO((GEN x)),trace9 ___PROTO((GEN x,GEN p1));
GEN     assmat ___PROTO((GEN x)),gnorm ___PROTO((GEN x)),gnorml2 ___PROTO((GEN x)),gconj ___PROTO((GEN x)),concat ___PROTO((GEN x, GEN y)),idmat ___PROTO((long n)),conjvec ___PROTO((GEN x,long prec));
GEN     extract ___PROTO((GEN x, GEN l)),matextract ___PROTO((GEN x, GEN l1, GEN l2)),gtomat ___PROTO((GEN x)),invmulmat ___PROTO((GEN a, GEN b)),invmulmatreel ___PROTO((GEN a, GEN b)),invmatreel ___PROTO((GEN a));
GEN     sqred ___PROTO((GEN a)),sqred1 ___PROTO((GEN a)),sqred2 ___PROTO((GEN a, long flg)),sqred3 ___PROTO((GEN a)),signat ___PROTO((GEN a)),jacobi ___PROTO((GEN a, long prec)),matrixqz ___PROTO((GEN x, GEN pp)),matrixqz2 ___PROTO((GEN x)),matrixqz3 ___PROTO((GEN x));
GEN     indexrank ___PROTO((GEN x)),kerint ___PROTO((GEN x)),kerint1 ___PROTO((GEN x)),kerint2 ___PROTO((GEN x)),intersect ___PROTO((GEN x, GEN y)),deplin ___PROTO((GEN x)),detint ___PROTO((GEN x));
GEN     hnfspec ___PROTO((long** mat,GEN* ptdep,GEN* ptmatc,long* vperm,GEN* ptmatalpha,long co,long li,long k0,long* ptnlze,long* ptcol));
GEN     hnffinal ___PROTO((GEN matgen,GEN* ptpdep,GEN* ptmatc,long* vperm,GEN* ptmatalpha,long lnz,long co,long li,long col,long lig,long nlze,long* ptcol));
GEN     hnfadd ___PROTO((GEN mit,GEN* ptpdep,GEN* ptmatc,long* vperm,GEN* ptmatalpha,long co,long li,long col,long* ptnlze,GEN extramat,GEN extramatc));
long    rank ___PROTO((GEN x)),perf ___PROTO((GEN a));

/* anal.c */
     
GEN     lisexpr ___PROTO((char *t)),readexpr ___PROTO((char **c)),lisseq ___PROTO((char *t)),readseq ___PROTO((char **c));
void    switchin ___PROTO((char *name)), switchout ___PROTO((char *name)), fliplog ___PROTO((void));
     
/* arith.c */
     
GEN     racine ___PROTO((GEN a)),mppgcd ___PROTO((GEN a, GEN b)),mpfact ___PROTO((long n)),mpfactr ___PROTO((long n, long prec));
GEN     sfcont ___PROTO((GEN x, GEN x1, long k)),sfcont2 ___PROTO((GEN b, GEN x)),gcf ___PROTO((GEN x)),gcf2 ___PROTO((GEN b, GEN x)),pnqn ___PROTO((GEN x)),gboundcf ___PROTO((GEN x, long k));
GEN     bestappr ___PROTO((GEN x, GEN k)), addprimestotable ___PROTO((GEN primes));
GEN     bezout ___PROTO((GEN a, GEN b, GEN *u, GEN *v)),chinois ___PROTO((GEN x, GEN y)),mpinvmod ___PROTO((GEN a, GEN m)),puissmodulo ___PROTO((GEN a, GEN n, GEN m)),fibo ___PROTO((long n)),bigprem ___PROTO((GEN n)),prime ___PROTO((long n));
GEN     primes ___PROTO((long n)),phi ___PROTO((GEN n)),decomp ___PROTO((GEN n)),auxdecomp ___PROTO((GEN n, long all)),smallfact ___PROTO((GEN n)),boundfact ___PROTO((GEN n, long lim));
GEN     sumdiv ___PROTO((GEN n)),sumdivk ___PROTO((long k, GEN n)),numbdiv ___PROTO((GEN n)),binaire ___PROTO((GEN x)),order ___PROTO((GEN x)),gener ___PROTO((GEN m)),znstar ___PROTO((GEN x)),divisors ___PROTO((GEN n));
GEN     ellfacteur ___PROTO((GEN n1)),classno ___PROTO((GEN x)),classno2 ___PROTO((GEN x)),classno3 ___PROTO((GEN x)),fundunit ___PROTO((GEN x)),regula ___PROTO((GEN x, long prec));
GEN     compimag ___PROTO((GEN x, GEN y)),sqcomp ___PROTO((GEN x)),qfi ___PROTO((GEN x, GEN y, GEN z)),qfr ___PROTO((GEN x, GEN y, GEN z, GEN d)),compreal ___PROTO((GEN x, GEN y)),redreal ___PROTO((GEN x)),sqcompreal ___PROTO((GEN x));
GEN     rhoreal ___PROTO((GEN x)),rhorealnod ___PROTO((GEN x, GEN isqrtD)),redrealnod ___PROTO((GEN x, GEN isqrtD)),redimag ___PROTO((GEN x));
GEN     primeform ___PROTO((GEN x, GEN p, long prec));
GEN     nucomp ___PROTO((GEN x, GEN y, GEN l)),nudupl ___PROTO((GEN x, GEN l)),nupow ___PROTO((GEN x, GEN n));
GEN     comprealraw ___PROTO((GEN x, GEN y)),sqcomprealraw ___PROTO((GEN x)),powrealraw ___PROTO((GEN x, long n, long prec));

GEN     gkronecker ___PROTO((GEN x, GEN y)),gkrogs ___PROTO((GEN x, long y)),gcarreparfait ___PROTO((GEN x)),gcarrecomplet ___PROTO((GEN x, GEN *pt));
GEN     gisprime ___PROTO((GEN x)),gispsp ___PROTO((GEN x)),gissquarefree ___PROTO((GEN x)),gisfundamental ___PROTO((GEN x)),gbittest ___PROTO((GEN x, GEN n));
GEN     gpseudopremier ___PROTO((GEN n, GEN a)),gmillerrabin ___PROTO((GEN n, long k)),gmu ___PROTO((GEN n)),gomega ___PROTO((GEN n)),gbigomega ___PROTO((GEN n));
GEN     addprimestotable ___PROTO((GEN primes));


long    kronecker ___PROTO((GEN x, GEN y)),krosg ___PROTO((long s, GEN x)),krogs ___PROTO((GEN x, long y)),kross ___PROTO((long x, long y)),kro8 ___PROTO((GEN x, GEN y));
long    mu ___PROTO((GEN n)),omega ___PROTO((GEN n)),bigomega ___PROTO((GEN n)),hil ___PROTO((GEN x, GEN y, GEN p));
int     carreparfait ___PROTO((GEN x)),carrecomplet ___PROTO((GEN x, GEN *pt)),bittest ___PROTO((GEN x, long n));
int     isprime ___PROTO((GEN x)),ispsp ___PROTO((GEN x)),issquarefree ___PROTO((GEN x)),isfundamental ___PROTO((GEN x)),mpsqrtmod ___PROTO((GEN a, GEN p, GEN *pr));
int     millerrabin ___PROTO((GEN n, long k)),pseudopremier ___PROTO((GEN n, GEN a)),inversemodulo ___PROTO((GEN a, GEN b, GEN *res));
byteptr initprimes ___PROTO((long maxnum));
void    lucas ___PROTO((long n, GEN *ln, GEN *ln1));

/* base.c */

GEN     base ___PROTO((GEN x, GEN *y)),smallbase ___PROTO((GEN x, GEN *y)),discf ___PROTO((GEN x)),smalldiscf ___PROTO((GEN x)),discf2 ___PROTO((GEN x));
GEN     hnf ___PROTO((GEN x)),hnfhavas ___PROTO((GEN x)),hnfnew ___PROTO((GEN x)),hnfperm ___PROTO((GEN x));
GEN     cleanmod ___PROTO((GEN x,long lim,GEN detmat,GEN detmatsur2));
GEN     hnfmod ___PROTO((GEN x, GEN detmat)),hnfmodid ___PROTO((GEN x,GEN p)),smith ___PROTO((GEN x)),smith2 ___PROTO((GEN x));
GEN     factoredbase ___PROTO((GEN x, GEN p, GEN *y)),factoreddiscf ___PROTO((GEN x, GEN p)),allbase ___PROTO((GEN x, long code, GEN *y)),galois ___PROTO((GEN x, long prec)),initalg ___PROTO((GEN x, long prec)),initalgred ___PROTO((GEN x, long prec));
GEN     tschirnhaus ___PROTO((GEN x)),galoisconj ___PROTO((GEN x, long prec)),galoisconj1 ___PROTO((GEN x, long prec)),galoisconj2 ___PROTO((GEN x, long prec)),initalgred2 ___PROTO((GEN x, long prec));
GEN     primedec ___PROTO((GEN nf,GEN p)),idealmul ___PROTO((GEN nf,GEN ix,GEN iy)),idealmulred ___PROTO((GEN nf, GEN ix, GEN iy, long prec)), ideal_two_elt ___PROTO((GEN nf, GEN ix));
GEN     idealmulh ___PROTO((GEN nf, GEN ix, GEN iy)),element_mulh ___PROTO((GEN nf, long limi, long limj, GEN x, GEN y));
GEN     idealmulprime ___PROTO((GEN nf,GEN ix,GEN vp)),minideal ___PROTO((GEN nf,GEN ix,GEN vdir,long prec));
GEN     idealmulelt ___PROTO((GEN nf, GEN elt, GEN x)),idealmullll ___PROTO((GEN nf, GEN x, GEN y));
GEN     ideallllredall ___PROTO((GEN nf, GEN ix, GEN vdir, long prec, long precint));
GEN     ideallllred ___PROTO((GEN nf,GEN ix,GEN vdir,long prec));
GEN     ideallllredpart1 ___PROTO((GEN nf,GEN x,GEN vdir, long flprem, long prec));
GEN     ideallllredpart1spec ___PROTO((GEN nf, GEN x, GEN matt2, long flprem, long prec));
GEN     ideallllredpart2 ___PROTO((GEN nf,GEN arch,GEN z,long prec));
GEN     element_mul ___PROTO((GEN nf,GEN x,GEN y)),element_sqr ___PROTO((GEN nf,GEN x)),element_pow ___PROTO((GEN nf,GEN x,GEN k)), element_mulvec ___PROTO((GEN nf, GEN x, GEN v));
GEN     rootsof1 ___PROTO((GEN x)),idealinv ___PROTO((GEN nf, GEN ix)),oldidealinv ___PROTO((GEN nf, GEN ix));
GEN     idealpow ___PROTO((GEN nf, GEN ix, GEN n)),idealpowred ___PROTO((GEN nf, GEN ix, GEN n, long prec)),idealpows ___PROTO((GEN nf, GEN ideal, long iexp));
GEN     idealpowprime ___PROTO((GEN nf, GEN vp, GEN n,long prec)),idealfactor ___PROTO((GEN nf, GEN x));
GEN     idealhermite ___PROTO((GEN nf, GEN x)),idealhermite2 ___PROTO((GEN nf, GEN a, GEN b)),idealadd ___PROTO((GEN nf, GEN x, GEN y)), idealaddone ___PROTO((GEN nf, GEN x, GEN y)), idealaddmultone ___PROTO((GEN nf, GEN list)), idealdiv ___PROTO((GEN nf, GEN x, GEN y));
GEN     idealintersect ___PROTO((GEN nf, GEN x, GEN y)), principalideal ___PROTO((GEN nf, GEN a));
GEN     principalidele ___PROTO((GEN nf, GEN a)),idealdivexact ___PROTO((GEN nf, GEN x, GEN y)),idealnorm ___PROTO((GEN nf, GEN x));
GEN     idealappr ___PROTO((GEN nf, GEN x)),idealapprfact ___PROTO((GEN nf, GEN x)), idealapprall ___PROTO((GEN nf, GEN x, long fl)), idealchinese ___PROTO((GEN nf, GEN x, GEN y));
GEN     idealcoprime ___PROTO((GEN nf, GEN x, GEN y)),ideal_two_elt2 ___PROTO((GEN nf, GEN x, GEN a));
GEN     twototwo ___PROTO((GEN nf, GEN a, GEN b)),threetotwo ___PROTO((GEN nf, GEN a, GEN b, GEN c)),threetotwo1 ___PROTO((GEN nf, GEN a, GEN b, GEN c)),threetotwo2 ___PROTO((GEN nf, GEN a, GEN b, GEN c));
GEN     basistoalg ___PROTO((GEN nf, GEN x)),algtobasis ___PROTO((GEN nf, GEN x));
GEN     weakhermite ___PROTO((GEN nf, GEN x)),nfhermite ___PROTO((GEN nf, GEN x)),nfhermitemod ___PROTO((GEN nf, GEN x, GEN detmat)),nfsmith ___PROTO((GEN nf, GEN x));
GEN     nfdiveuc ___PROTO((GEN nf, GEN a, GEN b)), nfdivres ___PROTO((GEN nf, GEN a, GEN b)), nfmod ___PROTO((GEN nf, GEN a, GEN b)),element_div ___PROTO((GEN nf, GEN x, GEN y)),element_inv ___PROTO((GEN nf, GEN x));
GEN     nfdetint ___PROTO((GEN nf,GEN pseudo));
GEN     element_reduce ___PROTO((GEN nf, GEN x, GEN ideal));
GEN     checknf ___PROTO((GEN nf)), differente ___PROTO((GEN nf, GEN premiers));

long    idealval ___PROTO((GEN nf,GEN ix,GEN vp)), isideal ___PROTO((GEN nf,GEN x));
long    element_val ___PROTO((GEN nf, GEN x, GEN vp)), element_val2 ___PROTO((GEN nf, GEN x, GEN d, GEN vp));

long    rnfisfree ___PROTO((GEN bnf, GEN order));

GEN     allbase4 ___PROTO((GEN f, long code, GEN *y, GEN *ptw)),base2 ___PROTO((GEN x, GEN *y)),rnfround2all ___PROTO((GEN nf, GEN pol, long all)),rnfpseudobasis ___PROTO((GEN nf, GEN pol)),rnfdiscf ___PROTO((GEN nf, GEN pol)),rnfsimplifybasis ___PROTO((GEN bnf, GEN order)),rnfsteinitz ___PROTO((GEN nf, GEN order)),rnfbasis ___PROTO((GEN bnf, GEN order)),rnfhermitebasis ___PROTO((GEN bnf, GEN order)); 
GEN     bsrch ___PROTO((GEN p, GEN fa, long Ka, GEN eta, long Ma)),setup ___PROTO((GEN p,GEN f,GEN theta,GEN nut)),eleval ___PROTO((GEN f,GEN h,GEN a)),vstar ___PROTO((GEN p,GEN h)),factcp ___PROTO((GEN p,GEN f,GEN beta)),bestnu ___PROTO((GEN w)),gcdpm ___PROTO((GEN f1,GEN f2,GEN pm));
GEN     compositum ___PROTO((GEN pol1, GEN pol2));
GEN     initzeta ___PROTO((GEN pol, long prec)),gzetak ___PROTO((GEN nfz, GEN s, long prec)),glambdak ___PROTO((GEN nfz, GEN s, long prec)),gzetakall ___PROTO((GEN nfz, GEN s, long flag, long prec));

/* bibli1.c */
     
GEN     tayl ___PROTO((GEN x, long v, long precdl)),legendre ___PROTO((long n)),tchebi ___PROTO((long n)),hilb ___PROTO((long n)),pasc ___PROTO((long n)),laplace ___PROTO((GEN x));
GEN     gprec ___PROTO((GEN x, long l)),convol ___PROTO((GEN x, GEN y)),ggrando ___PROTO((GEN x, long n)),ggrandocp ___PROTO((GEN x, long n)),gconvsp ___PROTO((GEN x)),gconvpe ___PROTO((GEN x));
GEN     lll ___PROTO((GEN x, long prec)),lll1 ___PROTO((GEN x, long prec)),lllrat ___PROTO((GEN x)),lllgram ___PROTO((GEN x, long prec)),lllgram1 ___PROTO((GEN x, long prec)),lllgramint ___PROTO((GEN x)),lllint ___PROTO((GEN x)),lllintpartial ___PROTO((GEN mat)),lllintpartialall ___PROTO((GEN mat, long all));
GEN     lllgramkerim ___PROTO((GEN x)),lllkerim ___PROTO((GEN x)),lllgramall ___PROTO((GEN x, long all)),lllall0 ___PROTO((GEN x, long all));
GEN     lllgen ___PROTO((GEN x)),lllkerimgen ___PROTO((GEN x)),lllgramgen ___PROTO((GEN x)),lllgramkerimgen ___PROTO((GEN x)),lllgramallgen ___PROTO((GEN x, long all));
GEN     binome ___PROTO((GEN x, long k)),gscal ___PROTO((GEN x, GEN y)),cyclo ___PROTO((long n)),vecsort ___PROTO((GEN x, GEN k));
GEN     lindep ___PROTO((GEN x, long prec)),lindep2 ___PROTO((GEN x, long bit)),lindep2bis ___PROTO((GEN x, long bit, long prec));
GEN     algdep ___PROTO((GEN x, long n, long prec)),algdep2 ___PROTO((GEN x, long n, long bit)),changevar ___PROTO((GEN x, GEN y)),ordred ___PROTO((GEN x, long prec));
GEN     polrecip ___PROTO((GEN x)),reorder ___PROTO((GEN x)),sort ___PROTO((GEN x)),lexsort ___PROTO((GEN x)),indexsort ___PROTO((GEN x)),polsym ___PROTO((GEN x, long n));
GEN     minim ___PROTO((GEN a, long borne, long stockmax)),minimprim ___PROTO((GEN a, long borne, long stockmax));
GEN     polred ___PROTO((GEN x, long prec)),factoredpolred ___PROTO((GEN x, GEN p, long prec)),smallpolred ___PROTO((GEN x, long prec)),polred2 ___PROTO((GEN x, long prec)),factoredpolred2 ___PROTO((GEN x, GEN p, long prec)), polredabs ___PROTO((GEN x, long prec));
GEN     smallpolred2 ___PROTO((GEN x, long prec)),allpolred ___PROTO((GEN x, GEN *pta, long code, long prec)),polymodrecip ___PROTO((GEN x)),genrand ___PROTO((void)),permute ___PROTO((long n, GEN x)),permuteInv ___PROTO((GEN x));
long    mymyrand();
long    setprecr ___PROTO((long n)),setserieslength ___PROTO((long n)),ccontent ___PROTO((long* x,long n));
GEN     setrand ___PROTO((long seed)),getrand ___PROTO((void)),getstack ___PROTO((void)),gettime ___PROTO((void)),getheap ___PROTO((void));
     
/* bibli2.c */

GEN     somme ___PROTO((entree *ep, GEN x, GEN a, GEN b, char *ch)),produit ___PROTO((entree *ep, GEN x, GEN a, GEN b, char *ch)),suminf ___PROTO((entree *ep, GEN a, char *ch, long prec)),prodinf ___PROTO((entree *ep, GEN a, char *ch, long prec)),prodinf1 ___PROTO((entree *ep, GEN a, char *ch, long prec)),prodeuler ___PROTO((entree *ep, GEN a, GEN b, char *ch, long prec));
GEN     vecteur ___PROTO((entree *ep, GEN nmax, char *ch)),vvecteur ___PROTO((entree *ep, GEN nmax, char *ch)),matrice ___PROTO((entree *ep1, entree *ep2, GEN nlig, GEN ncol, char *ch)),divsomme ___PROTO((entree *ep, GEN num, char *ch));
GEN     qromb ___PROTO((entree *ep, GEN a, GEN b, char *ch, long prec)),qromo ___PROTO((entree *ep, GEN a, GEN b, char *ch, long prec)),qromi ___PROTO((entree *ep, GEN a, GEN b, char *ch, long prec)),rombint ___PROTO((entree *ep, GEN a, GEN b, char *ch, long prec));
GEN     polint ___PROTO((GEN xa, GEN ya, GEN x, GEN *dy)),plot ___PROTO((entree *ep, GEN a, GEN b, char *ch)),ploth ___PROTO((entree *ep, GEN a, GEN b, char *ch, long prec)),ploth2 ___PROTO((entree *ep, GEN a, GEN b, char *ch, long prec)),plothraw ___PROTO((GEN listx, GEN listy)),zbrent ___PROTO((entree *ep, GEN a, GEN b, char *ch, long prec));
GEN     sumalt ___PROTO((entree *ep, GEN a, char *ch, long prec)),sumalt1 ___PROTO((entree *ep, GEN a, char *ch, long prec)),sumalt2 ___PROTO((entree *ep, GEN a, char *ch, long prec)),sumalt3 ___PROTO((entree *ep, GEN a, char *ch, long prec)),sumpos ___PROTO((entree *ep, GEN a, char *ch, long prec)),sumposold ___PROTO((entree *ep, GEN a, char *ch, long prec));
GEN     forpari ___PROTO((entree *ep, GEN a, GEN b, char *ch)),forstep ___PROTO((entree *ep, GEN a, GEN b, GEN s, char *ch)),fordiv ___PROTO((entree *ep, GEN a, char *ch)),forprime ___PROTO((entree *ep, GEN a, GEN b, char *ch)),forvec ___PROTO((entree *ep, GEN x, char *ch));
GEN     initrect ___PROTO((long ne, long x, long y)),killrect ___PROTO((long ne)),rectcursor ___PROTO((long ne)),rectmove ___PROTO((long ne, GEN x, GEN y)),rectrmove ___PROTO((long ne, GEN x, GEN y)),rectpoint ___PROTO((long ne, GEN x, GEN y));
GEN     rectrpoint ___PROTO((long ne, GEN x, GEN y)),rectbox ___PROTO((long ne, GEN gx2, GEN gy2)),rectrbox ___PROTO((long ne, GEN gx2, GEN gy2)),rectline ___PROTO((long ne, GEN gx2, GEN gy2)),rectrline ___PROTO((long ne, GEN gx2, GEN gy2)),rectdraw ___PROTO((GEN list));
GEN     rectpoints ___PROTO((long ne, GEN listx, GEN listy)),rectlines ___PROTO((long ne, GEN listx, GEN listy)),rectstring ___PROTO((long ne, GEN x)),rectscale ___PROTO((long ne, GEN x1, GEN x2, GEN y1, GEN y2));
GEN     postdraw ___PROTO((GEN list)),postploth ___PROTO((entree *ep, GEN a, GEN b, char *ch)),postploth2 ___PROTO((entree *ep, GEN a, GEN b, char *ch)),postplothraw ___PROTO((GEN listx, GEN listy));
GEN     gtoset ___PROTO((GEN x)), setunion ___PROTO((GEN x, GEN y)), setintersect ___PROTO((GEN x, GEN y)), setminus ___PROTO((GEN x, GEN y));
GEN     dirmul ___PROTO((GEN x, GEN y)), dirdiv ___PROTO((GEN x, GEN y)), dirzetak ___PROTO((GEN nf, GEN b));
long    isvecset ___PROTO((GEN x)), setsearch ___PROTO((GEN x, GEN y));

/* buch1.c et buch2.c */

GEN     buchimag ___PROTO((GEN Dx, GEN gcbach, GEN gcbach2, GEN gCO));
GEN     buchreal ___PROTO((GEN Dx, GEN gsens, GEN gcbach, GEN gcbach2, GEN gRELSUP, long prec));
GEN     buchall ___PROTO((GEN P, GEN gcbach, GEN gcbach2, GEN gRELSUP, GEN gborne, long nbrelpid, long minsfb, long flun, long prec));
#define buchgen(P,gcbach,gcbach2,prec) buchall(P,gcbach,gcbach2,stoi(5),gzero,4,3,0,prec)
#define buchgenfu(P,gcbach,gcbach2,prec) buchall(P,gcbach,gcbach2,stoi(5),gzero,4,3,2,prec)
#define buchinit(P,gcbach,gcbach2,prec) buchall(P,gcbach,gcbach2,stoi(5),gzero,4,3,-1,prec)
#define buchinitfu(P,gcbach,gcbach2,prec) buchall(P,gcbach,gcbach2,stoi(5),gzero,4,3,-2,prec)
GEN     isprincipal ___PROTO((GEN bignf, GEN x)),isprincipalgen ___PROTO((GEN bignf, GEN x));
GEN     isunit ___PROTO((GEN bignf, GEN x)), signunit ___PROTO((GEN bignf)), buchnarrow ___PROTO((GEN bignf)), buchfu ___PROTO((GEN bignf));
int     compte ___PROTO((long **mat, long row, long longueur, long *firstnonzero));
int     compte2 ___PROTO((long **mat, long row, long longueur, long *firstnonzero));

/* elliptic.c */

GEN     ghell ___PROTO((GEN e, GEN a, long prec)),ghell2 ___PROTO((GEN e, GEN a, long prec)),ghell3 ___PROTO((GEN e, GEN a, long prec));
GEN     initell ___PROTO((GEN x, long prec)),initell2 ___PROTO((GEN x, long prec)),smallinitell ___PROTO((GEN x)),zell ___PROTO((GEN e, GEN z, long prec)),coordch ___PROTO((GEN e, GEN ch)),pointch ___PROTO((GEN x, GEN ch));
GEN     addell ___PROTO((GEN e, GEN z1, GEN z2)),subell ___PROTO((GEN e, GEN z1, GEN z2)),powell ___PROTO((GEN e, GEN z, GEN n));
GEN     mathell ___PROTO((GEN e, GEN x, long prec)),bilhell ___PROTO((GEN e, GEN z1, GEN z2, long prec));
GEN     ordell ___PROTO((GEN e, GEN x, long prec)),apell ___PROTO((GEN e, GEN pl)),apell1 ___PROTO((GEN e, GEN p)),apell2 ___PROTO((GEN e, GEN p));
GEN     anell ___PROTO((GEN e, long n)),akell ___PROTO((GEN e, GEN n));
GEN     localreduction ___PROTO((GEN e, GEN p1)), globalreduction ___PROTO((GEN e1));
GEN     lseriesell ___PROTO((GEN e, GEN s, GEN N, GEN A, long prec));
GEN     pointell ___PROTO((GEN e, GEN z, long prec)),taniyama ___PROTO((GEN e));
GEN     orderell ___PROTO((GEN e, GEN p)),torsell ___PROTO((GEN e));
int     oncurve ___PROTO((GEN e, GEN z));
void    eulsum ___PROTO((GEN *sum, GEN term, long jterm, GEN *tab, long *dsum, long prec));

/* es.c */

void    filtre ___PROTO((char *s)),  pariputc ___PROTO((char c)), pariputs ___PROTO((char *s)), ecrire ___PROTO((GEN x, char format, long dec, long chmp)), voir ___PROTO((GEN x, long nb)), sor ___PROTO((GEN g, char fo, long dd, long chmp));
void    brute ___PROTO((GEN g, char format, long dec)), matbrute ___PROTO((GEN g, char format, long dec)), texe ___PROTO((GEN g, char format, long dec)), etatpile ___PROTO((unsigned int n));
void    outerr ___PROTO((GEN x)), bruterr ___PROTO((GEN x,char format,long dec)),outbeauterr ___PROTO((GEN x));
void bruteall ___PROTO((GEN g, char format, long dec, long flbl));

char* gen2str ___PROTO((GEN x));
char* g2str ___PROTO((GEN x, long dec));
char* g2estr ___PROTO((GEN x, long dec));

void fprintferr ___PROTO((char* pat, ...));
void flusherr();

char *gitoascii ___PROTO((GEN g, char *buf));

void printvargp ___PROTO((long));
extern void  (*printvariable) ___PROTO((long));

long timer ___PROTO((void)),timer2 ___PROTO((void));

/* gen1.c */

GEN     gadd ___PROTO((GEN x, GEN y)),gsub ___PROTO((GEN x, GEN y)),gmul ___PROTO((GEN x, GEN y)),gdiv ___PROTO((GEN x, GEN y));

/* gen2.c gen3.c */

GEN     gcopy ___PROTO((GEN x)),forcecopy ___PROTO((GEN x)),gclone ___PROTO((GEN x)),cgetp ___PROTO((GEN x)),gaddpex ___PROTO((GEN x, GEN y));
GEN     greffe ___PROTO((GEN x, long l)),gopsg2 ___PROTO((GEN (*f) (), long s, GEN y)), gopgs2 ___PROTO((GEN (*f) (), GEN y, long s)), co8 ___PROTO((GEN x, long l)),cvtop ___PROTO((GEN x, GEN p, long l)),compo ___PROTO((GEN x, long n)),gsqr ___PROTO((GEN x));
GEN     gneg ___PROTO((GEN x)),gabs ___PROTO((GEN x, long prec));
GEN     gpui ___PROTO((GEN x, GEN n, long prec)), gpuigs ___PROTO((GEN x, long n));
GEN     gmax ___PROTO((GEN x, GEN y)),gmin ___PROTO((GEN x, GEN y)),ginv ___PROTO((GEN x)),denom ___PROTO((GEN x)),numer ___PROTO((GEN x)),lift ___PROTO((GEN x)),centerlift ___PROTO((GEN x)),vecmax ___PROTO((GEN x)),vecmin ___PROTO((GEN x));
GEN     gmulsg ___PROTO((long s, GEN y)),gdivgs ___PROTO((GEN x, long s)),gmodulo ___PROTO((GEN x, GEN y)),gmodulcp ___PROTO((GEN x, GEN y)),simplify ___PROTO((GEN x));
GEN     gmod ___PROTO((GEN x, GEN y)),gshift ___PROTO((GEN x, long n)),gmul2n ___PROTO((GEN x, long n));
GEN     gsubst ___PROTO((GEN x, long v, GEN y)),deriv ___PROTO((GEN x, long v)),integ ___PROTO((GEN x, long v)),recip ___PROTO((GEN x)),ground ___PROTO((GEN x)),gcvtoi ___PROTO((GEN x, long *e)),grndtoi ___PROTO((GEN x, long *e));
GEN     gceil ___PROTO((GEN x)),gfloor ___PROTO((GEN x)),gfrac ___PROTO((GEN x)),gtrunc ___PROTO((GEN x)),gdivent ___PROTO((GEN x, GEN y)),gdiventres ___PROTO((GEN x, GEN y));
GEN     gdivmod ___PROTO((GEN x, GEN y, GEN *pr)),geval ___PROTO((GEN x)),glt ___PROTO((GEN x, GEN y)),gle ___PROTO((GEN x, GEN y)),ggt ___PROTO((GEN x, GEN y)),gge ___PROTO((GEN x, GEN y)),geq ___PROTO((GEN x, GEN y)),gne ___PROTO((GEN x, GEN y));
GEN     gand ___PROTO((GEN x, GEN y)),gor ___PROTO((GEN x, GEN y)),glength ___PROTO((GEN x)),matsize ___PROTO((GEN x)),truecoeff ___PROTO((GEN x, long n)),gtype ___PROTO((GEN x)),gsettype ___PROTO((GEN x,long t));
GEN     gtopoly ___PROTO((GEN x, long v)),gtopolyrev ___PROTO((GEN x, long v)),gtoser ___PROTO((GEN x, long v)),gtovec ___PROTO((GEN x)),dbltor ___PROTO((double x));
GEN     karamul ___PROTO((GEN x, GEN y, long k)), mpkaramul ___PROTO((GEN x, GEN y, long k));
GEN     gdivround ___PROTO((GEN x, GEN y)), gpolvar ___PROTO((GEN y));


void    gop0z ___PROTO((GEN (*f)(), GEN x)),gop1z ___PROTO((GEN (*f)(), GEN x, GEN y)),gop2z ___PROTO((GEN (*f)(), GEN x, GEN y, GEN z)), gops2gsz ___PROTO((GEN (*f)(), GEN x, long s, GEN z)),gops2sgz ___PROTO((GEN (*f)(), long s, GEN y, GEN z)),gops2ssz ___PROTO((GEN (*f)(), long s, long y, GEN z));
void    gop3z ___PROTO((GEN (*f)(), GEN x, GEN y, GEN z, GEN t)),gops1z ___PROTO((GEN (*f)(), long s, GEN y)), gopsg2z ___PROTO((GEN (*f)(), long s, GEN y, GEN z)),gopgs2z ___PROTO((GEN (*f)(), GEN y, long s, GEN z)),gaffsg ___PROTO((long s, GEN x)),gaffect ___PROTO((GEN x, GEN y));
void    normalize ___PROTO((GEN *px)),normalizepol ___PROTO((GEN *px));

int     gcmp0 ___PROTO((GEN x)),gcmp1 ___PROTO((GEN x)),gcmp_1 ___PROTO((GEN x)),gcmp ___PROTO((GEN x, GEN y)),lexcmp ___PROTO((GEN x, GEN y)),gegal ___PROTO((GEN x, GEN y)),polegal ___PROTO((GEN x, GEN y)),vecegal ___PROTO((GEN x, GEN y)),gsigne ___PROTO((GEN x));
int     gvar ___PROTO((GEN x)),gvar2 ___PROTO((GEN x)),tdeg ___PROTO((GEN x)),precision ___PROTO((GEN x)),gprecision ___PROTO((GEN x)),ismonome ___PROTO((GEN x)),iscomplex ___PROTO((GEN x)),isexactzero ___PROTO((GEN g));
long    padicprec ___PROTO((GEN x, GEN p));
long    opgs2 ___PROTO((int (*f)(), GEN y, long s));
long    taille ___PROTO((GEN x)),taille2 ___PROTO((GEN x)),gexpo ___PROTO((GEN x)),gtolong ___PROTO((GEN x)),ggval ___PROTO((GEN x, GEN p)),rounderror ___PROTO((GEN x)),gsize ___PROTO((GEN x)),pvaluation ___PROTO((GEN x, GEN p, GEN *py));
double  rtodbl ___PROTO((GEN x)), gtodouble ___PROTO((GEN x));

/* init.c */

GEN     newbloc ___PROTO((long n)),geni ___PROTO((void));
GEN     allocatemem ___PROTO((ulong newsize));
long    marklist ___PROTO((void));
#ifdef __cplusplus
void    init(long parisize, long maxprime, void  ___PROTO((*printvar)) ___PROTO((long))=printvargp);
#else
void    init ___PROTO((long parisize, long maxprime));
#endif
void    freeall ___PROTO((void)), killall ___PROTO((void));

void    killbloc ___PROTO((GEN x)),newvalue ___PROTO((entree *ep, GEN val)),killvalue ___PROTO((entree *ep));
#ifdef __cplusplus
extern "C" void    err(long numerr, ...);
#else
extern
#ifdef __GNUC__
__volatile__
#endif
void    err ___PROTO((long numerr, ...));
#endif

void    recover ___PROTO((long listloc)),changevalue ___PROTO((entree *ep, GEN val)),allocatemoremem ___PROTO((ulong newsize));

/* polarit.c */
     
GEN     ginvmod ___PROTO((GEN x, GEN y)),gred ___PROTO((GEN x)),gdeuc ___PROTO((GEN x, GEN y)),gres ___PROTO((GEN x, GEN y)),poldivres ___PROTO((GEN x, GEN y, GEN *pr));
GEN     poleval ___PROTO((GEN x, GEN y)),roots ___PROTO((GEN x, long l)),roots2 ___PROTO((GEN pol,long PREC)),rootslong ___PROTO((GEN x, long l)),ggcd ___PROTO((GEN x, GEN y)),gbezout ___PROTO((GEN x, GEN y, GEN *u, GEN *v)),vecbezout ___PROTO((GEN x, GEN y)),glcm ___PROTO((GEN x, GEN y));
GEN     subresext ___PROTO((GEN x, GEN y, GEN *U, GEN *V)),vecbezoutres ___PROTO((GEN x, GEN y));
GEN     polgcd ___PROTO((GEN x, GEN y)),srgcd ___PROTO((GEN x, GEN y)),polgcdnun ___PROTO((GEN x, GEN y)),content ___PROTO((GEN x)),primpart ___PROTO((GEN x)),psres ___PROTO((GEN x, GEN y)),factmod9 ___PROTO((GEN f, GEN p, GEN a));
GEN     factmod ___PROTO((GEN f, GEN p)),factmod2 ___PROTO((GEN f, GEN p)),factmod_gen ___PROTO((GEN f, GEN p)),rootmod ___PROTO((GEN f, GEN p)),rootmod2 ___PROTO((GEN f, GEN p)),decpol ___PROTO((GEN x, long klim)),factor ___PROTO((GEN x)),gisirreducible ___PROTO((GEN x));
GEN     factpol ___PROTO((GEN x, long klim, long hint)),factpol2 ___PROTO((GEN x, long klim)),simplefactmod ___PROTO((GEN f, GEN p)), factberl ___PROTO((GEN x, GEN p));
GEN     subres ___PROTO((GEN x, GEN y)),discsr ___PROTO((GEN x)),quadpoly ___PROTO((GEN x)),quadgen ___PROTO((GEN x)),quaddisc ___PROTO((GEN x)),bezoutpol ___PROTO((GEN a, GEN b, GEN *u, GEN *v)),polinvmod ___PROTO((GEN x, GEN y));
GEN     resultant2 ___PROTO((GEN x, GEN y)),sylvestermatrix ___PROTO((GEN x,GEN y)),polfnf ___PROTO((GEN a, GEN t)),nfiso ___PROTO((GEN a, GEN b)),nfincl ___PROTO((GEN a, GEN b));
GEN     newtonpoly ___PROTO((GEN x, GEN p)),apprgen ___PROTO((GEN f, GEN a)),apprgen9 ___PROTO((GEN f, GEN a)),rootpadic ___PROTO((GEN f, GEN p, long r)),rootpadicfast ___PROTO((GEN f, GEN p, long r)),gcvtop ___PROTO((GEN x, GEN p, long r)),factorpadic2 ___PROTO((GEN x, GEN p, long r));
GEN     factorpadic4 ___PROTO((GEN x, GEN p, long r)),nilordpadic ___PROTO((GEN p,long r,GEN fx,long mf,GEN gx)),Decomppadic ___PROTO((GEN p,long r,GEN f,long mf,GEN theta,GEN chi,GEN nu)),squarefree ___PROTO((GEN f));
long    sturm ___PROTO((GEN x)),sturmpart ___PROTO((GEN x, GEN a, GEN b));
int     poldivis ___PROTO((GEN x, GEN y, GEN *z)),gdivise ___PROTO((GEN x, GEN y));
void    gredsp ___PROTO((GEN *px)),split ___PROTO((long m, GEN *t, long d, long p, GEN q)),split9 ___PROTO((GEN m, GEN *t, long d, long p, GEN q, GEN unfq, GEN qq, GEN a)),splitgen ___PROTO((GEN m, GEN *t,long d,GEN p, GEN q));
int     issimplefield ___PROTO((GEN x)),isinexactfield ___PROTO((GEN x));

/* trans.c */
     
GEN     greal ___PROTO((GEN x)),gimag ___PROTO((GEN x)),teich ___PROTO((GEN x)),agm ___PROTO((GEN x, GEN y, long prec)),palog ___PROTO((GEN x));

GEN     mpsqrt ___PROTO((GEN x)),gsqrt ___PROTO((GEN x, long prec));
GEN     gexp ___PROTO((GEN x, long prec));
GEN     mplog ___PROTO((GEN x)),glog ___PROTO((GEN x, long prec));
GEN     mpexp1 ___PROTO((GEN x)),mpexp ___PROTO((GEN x));
GEN     logagm ___PROTO((GEN q)),glogagm ___PROTO((GEN x, long prec));
GEN     mpsc1 ___PROTO((GEN x, long *ptmod8)),mpcos ___PROTO((GEN x)),gcos ___PROTO((GEN x, long prec)),mpsin ___PROTO((GEN x)),gsin ___PROTO((GEN x, long prec));
GEN     mpaut ___PROTO((GEN x)),mptan ___PROTO((GEN x)),gtan ___PROTO((GEN x, long prec)),mpatan ___PROTO((GEN x)),gatan ___PROTO((GEN x, long prec)),mpasin ___PROTO((GEN x)),gasin ___PROTO((GEN x, long prec));
GEN     mpacos ___PROTO((GEN x)),gacos ___PROTO((GEN x, long prec)),mparg ___PROTO((GEN x, GEN y)),mpch ___PROTO((GEN x)),gch ___PROTO((GEN x, long prec)),mpsh ___PROTO((GEN x)),gsh ___PROTO((GEN x, long prec));
GEN     mpth ___PROTO((GEN x)),gth ___PROTO((GEN x, long prec)),mpath ___PROTO((GEN x)),gath ___PROTO((GEN x, long prec)),mpash ___PROTO((GEN x)),gash ___PROTO((GEN x, long prec));
GEN     garg ___PROTO((GEN x, long prec)),sarg ___PROTO((GEN x, GEN y, long prec)),mppsi ___PROTO((GEN z)),gpsi ___PROTO((GEN x, long prec)),transc ___PROTO((GEN (*f)(), GEN x, long prec)),kbessel ___PROTO((GEN nu, GEN gx, long prec)),hyperu ___PROTO((GEN a, GEN b, GEN gx, long prec));
GEN     cxpsi ___PROTO((GEN z, long prec)),jbesselh ___PROTO((GEN n, GEN z, long prec)),gzeta ___PROTO((GEN x, long prec));
GEN     kbessel2 ___PROTO((GEN nu, GEN x, long prec)),eint1 ___PROTO((GEN x, long prec)),gerfc ___PROTO((GEN x, long prec)),eta ___PROTO((GEN x, long prec)),jell ___PROTO((GEN x, long prec)),wf2 ___PROTO((GEN x, long prec)),wf ___PROTO((GEN x, long prec));
GEN     incgam ___PROTO((GEN a, GEN x, long prec)),incgam1 ___PROTO((GEN a, GEN x, long prec)),incgam2 ___PROTO((GEN a, GEN x, long prec)),incgam3 ___PROTO((GEN a, GEN x, long prec)),incgam4 ___PROTO((GEN a, GEN x, GEN z, long prec)),bernreal ___PROTO((long n, long prec)),bernvec ___PROTO((long nomb));
GEN     mpach ___PROTO((GEN x)),gach ___PROTO((GEN x, long prec)),mpgamma ___PROTO((GEN x)),cxgamma ___PROTO((GEN x, long prec)),ggamma ___PROTO((GEN x, long prec)),mpgamd ___PROTO((long x, long prec)),ggamd ___PROTO((GEN x, long prec)),mppi ___PROTO((long prec));
GEN     mpeuler ___PROTO((long prec)),polylog ___PROTO((long m, GEN x, long prec)),dilog ___PROTO((GEN x, long prec)),polylogd ___PROTO((long m, GEN x, long prec)),polylogdold ___PROTO((long m, GEN x, long prec)),polylogp ___PROTO((long m, GEN x, long prec)),gpolylog ___PROTO((long m, GEN x, long prec));
GEN     theta ___PROTO((GEN q, GEN z, long prec)),thetanullk ___PROTO((GEN q, long k, long prec)),mplngamma ___PROTO((GEN x)),cxlngamma ___PROTO((GEN x, long prec)),glngamma ___PROTO((GEN x, long prec)),izeta ___PROTO((GEN x, long prec));

void    pi ___PROTO((long prec)),euler ___PROTO((long prec)),mpbern ___PROTO((long nomb, long prec)),gsincos ___PROTO((GEN x, GEN *s, GEN *c, long prec));
void    gsqrtz ___PROTO((GEN x, GEN y)),gexpz ___PROTO((GEN x, GEN y)),glogz ___PROTO((GEN x, GEN y)),gcosz ___PROTO((GEN x, GEN y)),gsinz ___PROTO((GEN x, GEN y)),mpsincos ___PROTO((GEN x, GEN *s, GEN *c)),gtanz ___PROTO((GEN x, GEN y));
void    gatanz ___PROTO((GEN x, GEN y)),gasinz ___PROTO((GEN x, GEN y)),gacosz ___PROTO((GEN x, GEN y)),gchz ___PROTO((GEN x, GEN y)),gshz ___PROTO((GEN x, GEN y)),gthz ___PROTO((GEN x, GEN y)),gashz ___PROTO((GEN x, GEN y)),gachz ___PROTO((GEN x, GEN y));
void    gathz ___PROTO((GEN x, GEN y)),ggammaz ___PROTO((GEN x, GEN y)),glngammaz ___PROTO((GEN x, GEN y)),mpgamdz ___PROTO((long s, GEN y)),ggamdz ___PROTO((GEN x, GEN y)),gpsiz ___PROTO((GEN x, GEN y)),gzetaz ___PROTO((GEN x, GEN y));
void    gpolylogz ___PROTO((long m, GEN x, GEN y));

/* version.c */

GEN     gerepilc ___PROTO((GEN l, GEN p, GEN q));
void    gerepilemany ___PROTO((long ltop, GEN*  gptr[], long nptr));
void    printversion ___PROTO((void)), printversionno ___PROTO((void));

/* #ifdef __cplusplus
extern "C" {
#endif
long    mulmodll ___PROTO((ulong a, ulong b, ulong c));
long    addll ___PROTO((ulong, ulong)), subll ___PROTO((ulong, ulong)), addllx ___PROTO((ulong, ulong)), subllx ___PROTO((ulong, ulong));
long    shiftl ___PROTO((ulong, ulong)), shiftlr ___PROTO((ulong, ulong));
long    mulll ___PROTO((ulong, ulong)),addmul ___PROTO((ulong, ulong)), divll ___PROTO((ulong, ulong));
int     bfffo ___PROTO((ulong));
#ifdef __cplusplus
}
#endif */


#ifdef __cplusplus
extern "C" {
int bfffo ___PROTO((ulong));
long divll ___PROTO((ulong, ulong));
long mulmodll ___PROTO((ulong a, ulong b, ulong c));
}
#else
long    addll ___PROTO((ulong, ulong)), subll ___PROTO((ulong, ulong)), addllx ___PROTO((ulong, ulong)), subllx ___PROTO((ulong, ulong));
long    shiftl ___PROTO((ulong, ulong)), shiftlr ___PROTO((ulong, ulong));
long    mulll ___PROTO((ulong, ulong)),addmul ___PROTO((ulong, ulong)), divll ___PROTO((ulong, ulong));
int     bfffo ___PROTO((ulong));
long    mulmodll ___PROTO((ulong a, ulong b, ulong c));
#endif

typedef struct PariOUT {
  void  (*fputc) ___PROTO((char));
  void (*fputs) ___PROTO((char*));
} PariOUT;

typedef struct PariERR {
  void (*fputc) ___PROTO((char));
  void (*fputs) ___PROTO((char*));
  void (*flush)();
  void (*die)();
} PariERR;

extern PariOUT *pariOut;
extern PariERR *pariErr;
extern long GlobErrNum;
