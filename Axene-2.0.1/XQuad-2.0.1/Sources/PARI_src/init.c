/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                                                                 */
/*                                                                 */
/*              PROGRAMME D'INITIALISATION DU SYSTEME              */
/*                                                                 */
/*                    ET TRAITEMENT DES ERREURS                    */
/*                                                                 */
/*                       copyright Babe Cool                       */
/*                                                                 */
/*                                                                 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define INIT_CONST

#include        "genpari.h"
#include <varargs.h>
#ifdef HPPA
#define SIGBUS 10
#endif

int STACKSIZE = 5000;  /* nombre de gn possibles */
int TBLSZ = 135;  /* taille de la table de hashcodes */
 int MAXBLOC = 5000;  /* nombre de blocs autorises dans le tas */
 int NUMPRTBELT = 20; /* taille table de premiers prives */

 double K = 9.632959862*(BYTES_IN_LONG/4);  /* 32*log(2)/log(10)  */
double K1 = 0.103810253/(BYTES_IN_LONG/4); /* log(10)/(32*log(2))*/
double K2 = 1.1239968;               /* 1/(1-(log(2)/(2*pi)))    */
double K4 = 17.079468445347/BITS_IN_LONG;  /* 2*e*pi/32          */
double LOG2 = 0.69314718055994531;     /* log(2)                   */
double L2SL10 = 0.301029995663981;   /* log(2)/log(10)           */
#ifndef  PI
double PI = 3.141592653589;          /* pi                       */
#endif
double rac5 = 2.23606797749;         /* racine de 5              */
double C1 = 0.9189385332;            /* log(2*pi)/2              */
double C2 = 22.18070978*(BYTES_IN_LONG/4);  /* 32*log(2)         */
double C3 = 0.0216950598/(BYTES_IN_LONG/4); /* log((1+sqrt(5))/2)/(32*log(2)) */
#ifdef LONG_IS_64BIT
double C31 = 9223372036854775808.0;  /* 2^63 */
#else
double C31 = 2147483648.0;           /* 2^31                     */
#endif

#ifdef LONG_IS_32BIT
long BIGINT = 32767;                 /* 2^15-1                   */
long EXP220 = 1048576;               /* 2^20                     */
long VERYBIGINT = 2147483647;        /* 2^31-1                   */
#endif

#ifdef LONG_IS_64BIT
long BIGINT = 2147483647;                 /* 2^31-1              */
long EXP220 = 1099511627776;              /* 2^40                */
long VERYBIGINT = 9223372036854775807;    /* 2^63-1              */
#endif

/*      Variables statiques communes :          */

unsigned long top,bot,avma;
long    avloc;
#ifdef LONG_IS_32BIT
long    prec=5;
#endif
#ifdef LONG_IS_64BIT
long    prec=4;
#endif
long    precdl=16, defaultpadicprecision=16;
long    tglobal,paribuffsize=30000,pariecho=0;
jmp_buf environnement;
jmp_buf G_erreur;
FILE    *outfile;
FILE    *errfile;
FILE    *logfile;
FILE    *infile;
long    nvar = 0;
GEN     gnil,gzero,gun,gdeux,ghalf,polvar,gi,RAVYZARC;
GEN     gpi=(GEN)0;
GEN     geuler=(GEN)0;
GEN     bernzone=(GEN)0;
entree  **varentries, **hashtable;
GEN     *blocliste, *polun, *polx, *g;
long    *ordvar,varchanged=0;
long    nextbloc = 0;
#ifdef LONG_IS_64BIT
long    glbfmt[]={'g',0,38};
#else
long    glbfmt[]={'g',0,28};
#endif
long    **rectgraph;
long    pari_randseed;
long    DEBUGLEVEL = 0;

byteptr diffptr;
GEN     primetab; /* nombres premiers prives */

#ifdef LONG_IS_64BIT
long    lontyp[30]={0,0x100000000,0x100000000,1,1,1,1,2,1,1,2,2,0,1,1,1,1,1,1,1};
long    lontyp2[30]={0,0x100000000,0x100000000,2,1,1,1,3,2,2,2,2,0,1,1,1,1,1,1,1};     
#else
long    lontyp[30]={0,0x10000,0x10000,1,1,1,1,2,1,1,2,2,0,1,1,1,1,1,1,1};
long    lontyp2[30]={0,0x10000,0x10000,2,1,1,1,3,2,2,2,2,0,1,1,1,1,1,1,1};     
#endif
void    (*printvariable) ___PROTO((long));

     
     /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
     /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
     /*                                                                 */
     /*                      INITIALISATION DU SYSTEME                  */
     /*                                                                 */
     /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
     /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

long dummy=0; /* Ne pas enlever !!! */

void
catchinterrupt(truc)
int truc;
{
  truc = truc; /* Only way to keep quiet all ANSI compilers... */
  if (infile != stdin) switchin(NULL);
  signal(SIGINT,catchinterrupt);
  err(interrupter);
}

#ifdef SIGBUS
void
catchbus(truc)
int truc;
{
  truc = truc; 
  signal(SIGBUS,catchbus);
  err(talker,"Bus error: bug in MathsLlib or in calling program");
}
#endif

void
catchsegv(truc)
int truc;
{
  truc = truc; 
  signal(SIGSEGV,catchsegv);
  err(talker,"Segmentation fault: bug in MathsLib or in calling program");
}

#ifdef __cplusplus
void init(long parisize, long maxprime, void (*printvar)(long)=printvargp)
#else
void
init(parisize, maxprime)
long parisize;
long maxprime;
#endif
{
  long v, n, *e;
  char *p;
  GEN p1;
  
#ifdef __cplusplus
  printvariable=printvar;
#else
  printvariable=printvargp;
#endif

  outfile = stdout;errfile = stderr;logfile = NULL;infile = stdin;
  if (setjmp(environnement))
  {
    fprintferr( "\n Error in the MathsLib system. Exit(1).\n");
    exit(1);
  }
/*
  signal(SIGINT,catchinterrupt);
#ifdef SIGBUS
  signal(SIGBUS,catchbus);
#endif
  signal(SIGSEGV,catchsegv);
*/
  pari_randseed=1;v=parisize&(BYTES_IN_LONG-1);
  if(v) parisize+=(BYTES_IN_LONG-v);
  if (!(diffptr=initprimes(maxprime))) err(memer);
#if __MWERKS__
  {
    OSErr resultCode; Handle newHand = MFTempNewHandle(parisize,&resultCode);
    if (!newHand) err(memer);
    HLock(newHand);
    bot=(long)*newHand;
  }
#else
  if (!(bot=(long)malloc(parisize))) err(memer);
#endif
  top=avma=bot+parisize;
  if (!(varentries=(entree **)malloc(sizeof(entree*)*MAXVAR))) err(memer);
  if (!(hashtable=(entree **)malloc(sizeof(entree*)*TBLSZ))) err(memer);
  if (!(blocliste=(GEN *)malloc(sizeof(GEN)*MAXBLOC))) err(memer);
  if (!(ordvar=(long *)malloc(sizeof(long)*MAXVAR))) err(memer);
  if (!(polun=(GEN *)malloc(sizeof(GEN)<<MAXSHIFTVAR))) err(memer);
  if (!(polx=(GEN *)malloc(sizeof(GEN)<<MAXSHIFTVAR))) err(memer);
  if (!(g=(GEN *)malloc(sizeof(GEN)*STACKSIZE))) err(memer);
  if (!(rectgraph=(long**)malloc(sizeof(long*)*16))) err(memer);
  for(n=0;n<16;n++) 
  {
    if(!(e=rectgraph[n]=(long*)malloc(sizeof(long)*10))) err(memer);
    e[0]=e[1]=e[2]=e[3]=0;e[4]=lgetr(3);e[5]=lgetr(3);e[6]=lgetr(3);e[7]=lgetr(3);
    e[8]=lgetr(3);e[9]=lgetr(3);
  }
  for(n = 0; n < TBLSZ; n++) hashtable[n] = NULL;
  for(v = 0; v < NUMFUNC; v++)
  {
    for(n = 0, p = fonctions[v].name; *p; p++) n = n << 1 ^ *p;
    if (n < 0) n = -n; n %= TBLSZ;
    fonctions[v].next = hashtable[n];
    hashtable[n] = fonctions + v;
  }
  gnil = cgeti(2);gnil[1]=2; setpere(gnil,MAXUBYTE);
  gzero = cgeti(2);gzero[1]=2; setpere(gzero, MAXUBYTE);
  gun = stoi(1); setpere(gun, MAXUBYTE);
  gdeux = stoi(2); setpere(gdeux, MAXUBYTE);
  ghalf = cgetg(3,4);ghalf[1]=un;ghalf[2]=deux; setpere(ghalf, MAXUBYTE);
  gi = cgetg(3,6); gi[1] = zero; gi[2] = un; setpere(gi, MAXUBYTE);
  p1=cgetg(4,10);p1[1]=evalsigne(1)+evalvarn(MAXVARN)+evallgef(4);
  p1[2]=zero;p1[3]=un;polx[MAXVARN]=p1;
  p1=cgetg(3,10);p1[1]=evalsigne(1)+evalvarn(MAXVARN)+evallgef(3);
  p1[2]=un;polun[MAXVARN]=p1;
  for(v=0; v < MAXVAR; v++) ordvar[v] = v;
  polvar = cgetg(MAXVAR + 1,17); setlg(polvar,1); setpere(polvar, MAXUBYTE);
  for(v=1;v<=MAXVAR;v++) polvar[v]=evaltyp(17)+evalpere(MAXUBYTE)+evallg(1);
  primetab = cgetg(NUMPRTBELT+2,17);
  for(v = 1; v <= NUMPRTBELT+1; v++) primetab[v]=un;

  for(v = 0; v < MAXBLOC; v++) blocliste[v] = (GEN)0;
  for(v = 0; v < STACKSIZE; v++) g[v] = gzero;
  lisseq("x");avloc=avma;
}

void
killall()
{
  long i,*e,n;
  char *p;
  
  for(n=0;n<16;n++) {e=rectgraph[n];if(e[0]) killrect(n);}
  for(i=1;i<=NUMPRTBELT;i++)
    if(!gcmp1((GEN)primetab[i])) {killbloc((GEN)primetab[i]);primetab[i]=un;}
  for(i=0;i<MAXBLOC;i++)
    if(blocliste[i]) {killbloc(blocliste[i]);blocliste[i]=(GEN)0;}
  gpi=geuler=bernzone=(GEN)0;
  for(i=0;i<STACKSIZE;i++) g[i]=gzero;
  for(i=0;i<MAXVAR;i++) ordvar[i]=i;
  setlg(polvar,1);
  for(i=1;i<=MAXVAR;i++) polvar[i]=evaltyp(17)+evalpere(MAXUBYTE)+evallg(1);  
  for(n=0;n<TBLSZ;n++) hashtable[n]=NULL;
  for(i=0;i<NUMFUNC;i++)
  {
    for(n=0,p=fonctions[i].name;*p;p++) n=n<<1^*p;
    if (n<0) n= -n;n%=TBLSZ;
    fonctions[i].next=hashtable[n];
    hashtable[n]=fonctions+i;
  }
  precdl=16;defaultpadicprecision=16;
  pariecho=nvar=varchanged=nextbloc=0;
  lisseq("x");
}  

void
freeall()
{
  int n;
  long i,*e;
  
  for(n=0;n<16;n++)
  {
    e=rectgraph[n];if(e[0]) killrect(n);
    free((void *)e);
  }
  free((void *)rectgraph);
  for(i=1;i<=NUMPRTBELT;i++)
    if(!gcmp1((GEN)primetab[i])) killbloc((GEN)primetab[i]);
  free((void *)g);free((void *)polx);
  free((void *)polun);free((void *)ordvar);
  for(i=0;i<MAXBLOC;i++) if(blocliste[i]) killbloc(blocliste[i]);
  free((void *)blocliste);
  free((void *)hashtable);free((void *)varentries);free((void *)bot);
  free((void *)diffptr);
}
  
GEN
geni()
     
{
  return gi;
}

long
marklist()
     
{
  long i;
  GEN x, *p = blocliste;
  for (i = 0; i < MAXBLOC; i++)
    if((x = blocliste[i]))
    {
      x[-2] = (long)p;
      *p++ = x;
    }
  for (nextbloc = i = p - blocliste; i < MAXBLOC; i++)
    blocliste[i] = 0;
  return nextbloc;
}

GEN
newbloc(n)
long n;
{
  long i, *x;
  for(i = nextbloc; i < MAXBLOC; i++) if (!blocliste[i]) break;
  if (i == MAXBLOC)
  {
    for (i = 0; i < nextbloc; i++) if (!blocliste[i]) break;
    if (i == nextbloc) err(newblocer1);
  }
  x = (long *)malloc((n << TWOPOTBYTES_IN_LONG) + 2*BYTES_IN_LONG);
  if (!x) err(memer);
  x += 2;
  x[-2] = (long)(blocliste + i);
  x[-1] = 0;
  blocliste[i] = x;
  nextbloc = i + 1;
  return x;
}

void
killbloc(x)
GEN x;
{
  if (!x || isonstack(x)) return;
  *(long *)x[-2] = 0;
  free((void *)(x-2));
}

void
newvalue(ep, val)
entree *ep;
GEN val;
{
  GEN y = gclone(val);
  y[-1] = (long) ep->value;
  ep->value = (void *)y;
}

void
changevalue(ep, val)
entree *ep;
GEN val;
{
  GEN y = gclone(val);
  GEN x = (GEN)ep->value;
  ep->value = (void *)y;
  if ((long)x - (long)ep == sizeof(entree)) 
  {
    y[-1] = (long)x;
    return;
  }
  y[-1] = x[-1];
  killbloc(x);
}

void
killvalue(ep)
entree *ep;
{
  GEN x = (GEN)ep->value;
  if ((long)x - (long)ep == sizeof(entree)) return;
  ep->value = (void *)x[-1];
  killbloc(x);
}


void
install(f, name, valence)
GEN (*f) (/* ??? */);
char *name;
int valence;
{
  int n;
  entree *ep;
  char *p;
  
  if ((valence < 0) || (valence > 3)) err(valencer1);
  for(n = 0, p = name; *p; p++) n = n << 1 ^ *p;
  if (n < 0) n = -n; n %= TBLSZ;
  for(ep = hashtable[n]; ep; ep = ep->next)
    if (!strcmp(name, ep->name)) err(nomer1);
  ep = (entree *)malloc(sizeof(entree) + strlen(name) + 1);
  ep->name = (char *)ep + sizeof(entree); strcpy(ep->name, name);
  ep->value = (void *)f;
  ep->valence = valence;
  ep->menu = 0;
  ep->next = hashtable[n];
  hashtable[n] = ep;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                                                                 */
/*              TRAITEMENT DES ERREURS                             */
/*                                                                 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
long	GlobErrNum;

#ifdef __GNUC__
__volatile__
#endif
void
err(numerr, va_alist)
long numerr;
va_dcl
{
  char c;
  va_list poer;
  char *ch;
  GEN noninv;

/* Added For XQuad */
  if (numerr == errpile)
    {
      /* try to continue */
      printf("Allocate more memory\n");
      allocatemoremem(0);
      return;
    }

  GlobErrNum = numerr;
  if (!numerr)
    {
      printf("Erreur = 0\n");
      numerr = -1;
    }
  longjmp(G_erreur, numerr);

  va_start(poer);
  if (numerr!=memer) {
    flusherr();
  } else {
    pariErr=NULL;		/* Try to avoid loop */
  }
/*fprintferr( "%s",errmessage[numerr]);*/
  switch (numerr)
  {
    case matcher1:
      ch=va_arg(poer, char*);c = *ch++;
      fprintferr( "'%c'\n  instead of: '%s'", c, ch); break;
    case impl: ch=va_arg(poer, char*);
      fprintferr( " %s is not yet implemented.",ch); break;
    case talker: ch=va_arg(poer, char*);
      fprintferr( "%s.",ch); break;
    case varer1:
    case unknowner1:
    case caracer1: ch=va_arg(poer, char*);fprintferr( "'%s'",ch);break;
    case invmoder: ch=va_arg(poer, char*);noninv=va_arg(poer, GEN);
      fprintferr(": ");bruterr(noninv,'g',-1);break;
    case errpile: 
      if (!pariErr || !pariErr->fputc) {
	fprintferr("\n");
      }
      allocatemoremem(0);break;
  }
  if (pariErr && pariErr->die) /* empty */;
  else flusherr();
  outfile=stdout;errfile=stderr;
  if (!pariErr || !pariErr->fputc) fprintferr("\n");
  va_end(poer);
  if (pariErr && pariErr->die) pariErr->die();
  longjmp(environnement, numerr);
}

void
recover(listloc)
long listloc;
{
  long i, m, n;
  GEN x;
  entree *ep, *ep2;

  for (n = 0; n < TBLSZ; n++)
    for (ep = hashtable[n]; ep;)
      if (ep->valence >= 100)
      {
        x = (GEN)ep->value;
        if ((long)x - (long)ep == sizeof(entree))
        {
          if (ep->valence == 200) ep = ep->next;
          else
            if (ep == hashtable[n])
            {
              hashtable[n] = ep->next;
              free(ep);
              ep = hashtable[n];
            }
            else
            {
              for(ep2 = hashtable[n]; ep2->next != ep; ep2 = ep2->next);
              ep2->next = ep->next;
              free(ep); ep = ep2->next;
            }
          continue;
        }
        m = (long *)x[-2] - (long *)blocliste;
        if ((m < listloc) || (m >= MAXBLOC)) ep=ep->next;
        else killvalue(ep);
      }
      else ep = ep->next;
  for (i = listloc; i < MAXBLOC; i++)
    if ((x = blocliste[i]) && (x != gpi) && (x != geuler))
      killbloc(x);
}

void
allocatemoremem(newsize)
unsigned long newsize;
{
  long av,declg,declg2,tl,parisize,v;
  GEN ll,pp,l1,l2,l3;
  unsigned long topold,avmaold,botold;

  avmaold=avloc;topold=top;botold=bot;
  if(newsize<3) 
    parisize=(topold-botold)<<1;
  else 
    {
      if(newsize<(topold-avmaold)) 
	err(talker,"required stack memory too small");
      else parisize=newsize+16-(((newsize-1)&15)+1);
    }
  if (!(bot=(long)malloc(parisize))) 
    err(nomer2);
  if(!newsize)
  {
    fprintferr("Warning: doubling the stack size; new stack = %ld\n",
	       parisize);
  }
  top=avma=bot+parisize;
  declg=(long)top-(long)topold;declg2=declg>>TWOPOTBYTES_IN_LONG;
  for(ll=(GEN)top,pp=(GEN)topold;pp>(GEN)avmaold;) *--ll= *--pp;
  av=(long)ll;
  while(ll<(GEN)top)
  {
    l2=ll+lontyp[tl=typ(ll)];
    if(tl==10) {l3=ll+lgef(ll);ll+=lg(ll);if(l3>ll) l3=l2;}
    else {ll+=lg(ll);l3=ll;} 
    for(;l2<l3;l2++) 
    {
      l1=(GEN)(*l2);
      if((l1<(GEN)topold)&&(l1>=(GEN)avmaold)) *l2+=declg;
    }
  }
  gnil+=declg2;gzero+=declg2;gun+=declg2;gdeux+=declg2;ghalf+=declg2;
  gi+=declg2;polx[MAXUBYTE]+=declg2;polun[MAXUBYTE]+=declg2;polvar+=declg2;
  for(v=0;v<=tglobal;v++)
    if((g[v]<(GEN)topold)&&(g[v]>=(GEN)avmaold)) g[v]+=declg2;
  free((void *)botold);avloc=avma=av;
}

GEN
allocatemem(newsize)
unsigned long newsize;
{
#if __MWERKS__
  newsize = newsize;
  err(talker, "Not implemented yet, sorry");
#else
  allocatemoremem(newsize);
  longjmp(environnement,errpile);
#endif
  return gnil; /*inutile mais ca fait plaisir a des compilos */
}

#if __MWERKS__
void *macrealloc(void *p, size_t oldsize, size_t newsize)
{
  char *q = malloc(newsize);
  char *qq = q, *pp = p;
  int l = newsize > oldsize ? oldsize : newsize;
  while (l--) *qq++ = *pp++;
  free(p);
  return q;
}
#endif
