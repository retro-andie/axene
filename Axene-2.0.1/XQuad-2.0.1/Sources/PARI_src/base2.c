/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                                                                 */
/*                       BASE D'ENTIERS                            */
/*                                                                 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

# include "genpari.h"

#define TRUE 1
#define FALSE 0

GEN pradical ___PROTO((GEN nf,GEN p)),pol_min ___PROTO((GEN alpha,GEN nf,GEN p,GEN algebre,GEN algebre1));
GEN eval_pol ___PROTO((GEN nf,GEN pol,GEN alpha,GEN p,GEN algebre,GEN algebre1));
GEN lens ___PROTO((GEN nf,GEN p,GEN a)),two_elt ___PROTO((GEN nf,GEN p,GEN ideal));
GEN element_mulid ___PROTO((GEN nf, GEN x, long i));
GEN element_muli ___PROTO((GEN nf, GEN x, GEN y));
GEN nfker ___PROTO((GEN nf, GEN R, GEN prhall));
GEN nfsuppl ___PROTO((GEN nf, GEN x, long n, GEN prhall));
GEN nfidealdet1 ___PROTO((GEN nf, GEN a, GEN b));
long idealvalint ___PROTO((GEN nf, GEN x, GEN vp));

GEN maxord ___PROTO((GEN p,GEN f,long mf));
GEN nilord ___PROTO((GEN p,GEN fx,long mf,GEN gx));
GEN Decomp ___PROTO((GEN p,GEN f,long mf,GEN theta,GEN chi,GEN nu));
GEN respm ___PROTO((GEN f1,GEN f2,GEN pm));
GEN hnfpm ___PROTO((GEN a,GEN pm));
GEN nbasis ___PROTO((GEN ibas,GEN pd));
GEN pbasis ___PROTO((GEN p,GEN f,long mf,GEN alpha));
GEN eltppm ___PROTO((GEN f,GEN pd,GEN theta,GEN k));
GEN testb ___PROTO((GEN p,GEN fa,long Da,GEN theta,long Dt));
GEN testc ___PROTO((GEN p, GEN fa, long c, GEN alph2, long Ma, GEN thet2, long Mt));
GEN testd ___PROTO((GEN p,GEN fa,long c,long Da,GEN alph2,long Ma,GEN theta));
GEN csrch ___PROTO((GEN p,GEN fa,GEN gamma));
GEN dedek ___PROTO((GEN f,GEN p,GEN g));
long cbezout ___PROTO((long a,long b,long *u,long *v));
long clcm ___PROTO((long a,long b));

#define coef1(a,i,j)      (*((long*)(*(a+(j)+1))+(i)+1))
#define gcoef1(a,i,j)     (GEN)coef1(a,i,j)

GEN rquot ___PROTO((GEN x, GEN y)),ordmax ___PROTO((GEN f, GEN p, GEN e, GEN *ptdelta)),rtran ___PROTO((GEN v, GEN w, GEN q)),mtran ___PROTO((GEN v, GEN w, GEN q, GEN m)),matinv ___PROTO((GEN x, GEN d));

void rowred ___PROTO((GEN a, long rlim, GEN rmod));

/*******************************************************************
                             ROUND 2

  Entree:     x polynome unitaire a coefficients dans Z de deg n
	    definissant un corps de nombres K=Q(theta);
              code 0, 1 ou (long)p selon que l'on veut base, smallbase
            ou factoredbase.
	      y pointeur sur un GEN destine a recevoir
	    le discriminant du corps K.
  Sortie:    retourne 1) un vecteur (horizontal) a n composantes, 
            de polynomes a coeff dans Q (de deg 0,1...n-1)
	    constituant une base de l'anneau des entiers de K.
	        2) le discriminant de K (dans *y).
	    Rem: le denominateur commun des coef. est dans da.
*******************************************************************/

GEN
allbase(x, code, y)
GEN x;
long code;
GEN *y;
{
  GEN p,a,at,bt,b,da,db,q;
  long av=avma,tetpil,n,h,j,je,k,r,s,t,pro,v;

  if(typ(x)!=10) err(allbaser1);
  n=lgef(x)-3;if(n<=0) err(allbaser1);
  v=varn(x);*y=discsr(x);
#ifdef DEBUG_MODE
  if(DEBUGLEVEL) timer2();
#endif
  switch(code)
  {
    case 0: p=auxdecomp(absi(*y),1);h=lg((GEN)p[1])-1;break; /* base */
    case 1: p=auxdecomp(absi(*y),0);h=lg((GEN)p[1])-1;break; /* smallbase */
    default: p=(GEN)code;
      if((typ(p)!=19)||(lg(p)!=3)) err(factoreder1); /* factoredbase */
      h=lg((GEN)p[1])-1;
      q=gun;for(je=1;je<=h;je++) q=gmul(q,gpui(gcoeff(p,je,1),gcoeff(p,je,2),0));
      if(gcmp(absi(q),absi(*y))) err(factoreder2);
  }
#ifdef DEBUG_MODE
  if(DEBUGLEVEL>0) {fprintferr("temps factpol: ");fprintferr("%ld\n",timer2());flusherr();}
#endif
  a=idmat(n);da=gun;
  for(je=1;je<=h;je++)
  {
    if(gcmpgs(gcoeff(p,je,2),1)>0)
    {
      b=ordmax(x,gcoeff(p,je,1),gcoeff(p,je,2),&db);
      a=gmul(db,a);b=gmul(da,b);
      da=mulii(db,da);db=da;
      at=gtrans(a);bt=gtrans(b);
      for(r=n-1;r>=0;r--)
	for(s=r;s>=0;s--)
	  while(signe(gcoef1(bt,s,r)))
	  {
	    q=rquot(gcoef1(at,s,s),gcoef1(bt,s,r));
	    at[s+1]=(long)rtran((GEN)at[s+1],(GEN)bt[r+1],q);
	    for(t=s-1;t>=0;t--)
	    {
	      q=rquot(gcoef1(at,t,s),gcoef1(at,t,t));
	      at[s+1]=(long)rtran((GEN)at[s+1],(GEN)at[t+1],q);
	    }
	    pro=at[s+1];at[s+1]=bt[r+1];bt[r+1]=pro;
	  }
      for(j=n-1;j>=0;j--)
      {
	for(k=0;k<j;k++)
	{
	  while(signe(gcoef1(at,j,k)))
	  {
	    q=rquot(gcoef1(at,j,j),gcoef1(at,j,k));
	    at[j+1]=(long)rtran((GEN)at[j+1],(GEN)at[k+1],q);
	    pro=at[j+1];at[j+1]=at[k+1];at[k+1]=pro;
	  }
	}
	if(signe(gcoef1(at,j,j))<0)
	  for(k=0;k<=j;k++) coef1(at,k,j)=lnegi(gcoef1(at,k,j));
	for(k=j+1;k<n;k++)
	{
	  q=rquot(gcoef1(at,j,k),gcoef1(at,j,j));
	  at[k+1]=(long)rtran((GEN)at[k+1],(GEN)at[j+1],q);
	}
      }
      for(j=1;j<n;j++)
	if(!cmpii(gcoef1(at,j,j),gcoef1(at,j-1,j-1)))
	{
	  coef1(at,0,j)=zero;
	  for(k=1;k<=j;k++)
	    coef1(at,k,j)=coef1(at,k-1,j-1);
	}
      a=gtrans(at);
    }
  }
  for(j=1;j<=n;j++)
  {
    *y=divii(mulii(gcoeff(a,j,j),*y),da);
    *y=divii(mulii(gcoeff(a,j,j),*y),da);
  }
  tetpil=avma;*y=gcopy(*y);at=cgetg(n+1,17);
  for(j=1;j<=n;j++)
  {
    q=cgetg(j+2,10);q[1]=evalsigne(1)+evallgef(2+j)+evalvarn(v);at[j]=(long)q;
    for(k=2;k<=j+1;k++) q[k]=ldiv(gcoeff(a,j,k-1),da);
  }
  pro=lpile(av,tetpil,0)>>TWOPOTBYTES_IN_LONG;at+=pro;(*y)+=pro;
  return at;
}

GEN
base(x, y)
GEN x;
GEN *y;
{
  return allbase4(x,0,y,(GEN *)0);
}

GEN
base2(x, y)
GEN x;
GEN *y;
{
  return allbase(x,0,y);
}

GEN
smallbase(x, y)
GEN x;
GEN *y;
{
  return allbase4(x,1,y,(GEN *)0);
}

GEN
factoredbase(x, p, y)
GEN x;
GEN p;
GEN *y;
{
  return allbase4(x,(long)p,y,(GEN *)0);
}

GEN
discf(x)
GEN x;
{
  GEN y;
  long av,tetpil;

  av=avma;allbase4(x,0,&y,(GEN *)0);tetpil=avma;
  return gerepile(av,tetpil,gcopy(y));
}

GEN
discf2(x)
GEN x;
{
  GEN y;
  long av,tetpil;

  av=avma;allbase(x,0,&y);tetpil=avma;
  return gerepile(av,tetpil,gcopy(y));
}

GEN
smalldiscf(x)
GEN x;
{
  GEN y;
  long av,tetpil;

  av=avma;allbase4(x,1,&y,(GEN *)0);tetpil=avma;
  return gerepile(av,tetpil,gcopy(y));
}

GEN
factoreddiscf(x, p)
GEN x;
GEN p;
{
  GEN y;
  long av,tetpil;

  av=avma;allbase4(x,(long)p,&y,(GEN *)0);tetpil=avma;
  return gerepile(av,tetpil,gcopy(y));
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                                                                 */
/*   Quotient et Reste normalises   ( -1/2 < r = x-q*y <= 1/2 )    */
/*                                                                 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

GEN
rquot(x, y)
GEN x;
GEN y;
{
  GEN u,v,w,p;
  long av,av1;

  av=avma;
  u=absi(y);v=shifti(x,1);w=shifti(y,1);
  if ( cmpii(u,v)>0) p=subii(v,u);
  else p=addsi(-1,addii(u,v));
  av1=avma;
  return gerepile(av,av1,divii(p,w));
}
 
GEN
rrmdr(x, y)
GEN x;
GEN y;
{
  GEN p;
  long av,av1;

  av=avma;
  p=mulii(rquot(x,y),y);
  av1=avma;
  return gerepile(av,av1,subii(x,p));
}

GEN
rinv(x, y)
GEN x;
GEN y;
{
  GEN a,c,q,r,t;
  long av,av1;

  av=avma;
  a=gun;c=gzero;
  while( signe(y))
  {
    q=rquot(x,y);
    r=subii(a,mulii(q,c));a=c;c=r;
    t=subii(x,mulii(q,y));x=y;y=t;
  }
  av1=avma;
  if (signe(x)<0) a=negi(a);
  if (signe(c)) { av1=avma; a=rrmdr(a,c); }
  return gerepile(av,av1,a);
}

GEN
rgcd(x, y)
GEN x;
GEN y;
{
  GEN z;
  long av,av1;

  av=avma;
  while(signe(y))
  {
    z=rrmdr(x,y);x=y;y=z;
  }
  av1=avma;
  return gerepile(av,av1,absi(x));
}

GEN
rlcm(x, y)
GEN x;
GEN y;
{
  GEN d,z;
  long av,av1;

  av=avma;
  z=mulii(x,y);d=rgcd(x,y);
  av1=avma;
  return gerepile(av,av1,divii(z,d));
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                                                                 */
/*           Matrice compagnon du polynome unitaire x              */
/*                                                                 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

GEN
companion(x)
GEN x;
{
  long    i,j,l;
  GEN     y;
  
  l=lgef(x)-2;y=cgetg(l,19);
  for(i=1;i<l;i++) y[i]=lgetg(l,18);
  for(i=0;i<l-2;i++)
    for(j=0;j<l-1;j++) coef1(y,i,j)=((i+1)==j) ? un : zero;
  for(j=0;j<l-1;j++) coef1(y,l-2,j)=lneg((GEN)x[j+2]);
  return y;
}



GEN
ordmax(f, p, e, ptdelta)
GEN f;
GEN p;
GEN e;
GEN *ptdelta;
{
  GEN m,hh,pp,dd,ppdd,index,q,r,s,b,c,t,jp,v,delta;
  GEN cf[100],w[100],a;
  long h,i,j,k,sp,epsilon,n=lgef(f)-3,av=avma,tetpil,av3,dec;

  a=cgetg(n*n+1,19);
  for(j=1;j<=n*n;j++)
  {
    a[j]=lgetg(n+1,18);
    for(k=1;k<=n;k++) coeff(a,k,j)=zero;
  }
  v=cgetg(n+1,18);
  cf[0]=idmat(n);
  cf[1]=companion(f);
  for(j=2;j<n;j++) cf[j]=gmul(cf[1],cf[j-1]);
  delta=gun; epsilon=itos(e);
  m=idmat(n);

  do
  {
    pp=mulii(p,p);
    dd=mulii(delta,delta);
    ppdd=mulii(dd,pp);
    b=matinv(m,delta);
    for(i=0;i<n;i++)
    {
      t=gscalsmat(0,n); /* t <--- matrice nulle d'ordre n */
      for(h=0;h<n;h++)
	for(j=0;j<n;j++)
	  for(k=0;k<n;k++)
	    coef1(t,j,k)=(long)rrmdr(addii(gcoef1(t,j,k),mulii(gcoef1(m,i,h),gcoef1(cf[h],j,k))),ppdd);
      c=gmul(t,b);
      w[i]=gmul(m,c);
      for(j=0;j<n;j++)
	for(k=0;k<n;k++)
	  coef1(w[i],j,k)=(long)rrmdr(divii(gcoef1(w[i],j,k),dd),pp);
    }
    if(cmpis(p,n)>0)
    {
      for(i=0;i<n;i++)
	for(j=0;j<n;j++)
	{
	  coeff(t,i+1,j+1)=zero;
	  for(k=0;k<n;k++)
	    for(h=0;h<n;h++)
	    {
	      r=modii(gcoef1(w[i],k,h),p);
	      s=modii(gcoef1(w[j],h,k),p);
	      coef1(t,i,j)=lmodii(addii(gcoef1(t,i,j),mulii(r,s)),p);
	    }
	}
    }
    else
    {
      for(j=0;j<n;j++)
      {
	for(i=0;i<n;i++)
	  coef1(b,i,j)=(i==0)? un : zero;
/* ici la boucle en k calcule la puissance p mod p de w[j] */
	sp=itos(p);
	for(k=0;k<sp;k++)
	{
	  for(i=0;i<n;i++)
	  {
	    v[i+1]=zero;
	    for(h=0;h<n;h++)
	      v[i+1]=lmodii(addii((GEN)v[i+1],mulii(gcoef1(b,h,j),gcoef1(w[j],h,i))),p);
	  }
	  for(i=0;i<n;i++) coef1(b,i,j)=v[i+1];
	}
      }
      q=p;t=b;
      while(cmpis(q,n)<0)
      {
	q=mulii(q,p);
	t=gmul(b,t);
      }
    }
    for(i=0;i<n;i++)
      for(j=0;j<n;j++)
      {
	coef1(a,j,i)=(i==j)? (long)p : zero;
	coef1(a,j,n+i)=lmodii(gcoef1(t,i,j),p);
      }
    rowred(a,2*n-1,pp);
    for(i=0;i<n;i++)
      for(j=0;j<n;j++)
	coef1(b,j,i)=coef1(a,j,i);
    jp=matinv(b,p);
    for(k=0;k<n;k++)
    {
      t=gmul(jp,w[k]);
      t=gmul(t,b);
      for(i=0;i<n;i++)
	for(j=0;j<n;j++)
	  coef1(t,i,j)=ldivii(gcoef1(t,i,j),p);
      h=0;
      for(i=0;i<n;i++)
	for(j=0;j<n;j++)
	{
	  coef1(a,k,h)=coef1(t,i,j);
	  h++;
	}
    }
    rowred(a,n*n-1,pp);
    index=gun;
    for(i=0;i<n;i++)
      index=mulii(index,gcoef1(a,i,i));
    if (cmpsi(1,index))
    {
      delta=mulii(index,delta);
      for(i=0;i<n;i++)
	for(j=0;j<n;j++)
	  coef1(c,i,j)=coef1(a,i,j);
      b=matinv(c,index);
      m=gmul(b,m);
      hh=delta;
      for(i=0;i<n;i++)
	for(j=0;j<n;j++)
	  hh=rgcd(gcoef1(m,i,j),hh);
      if(cmpis(hh,1)>1)
      {
	delta=divii(delta,hh);
	for(i=0;i<n;i++)
	  for(j=0;j<n;j++)
	    coef1(m,i,j)=ldivii(gcoef1(m,i,j),hh);
      }
      q=index;
      while(!signe(modii(q,p)))
      {
	q=divii(q,p);
	epsilon=epsilon-2;
      }
    }
  }
  while(!gcmp1(index) && (epsilon>=2));
  tetpil=avma;delta=gcopy(delta);m=gcopy(m);
  av3=avma;dec=lpile(av,tetpil,0)>>TWOPOTBYTES_IN_LONG;
  *ptdelta=adecaler(delta,tetpil,av3)?delta+dec:delta;
  return m+dec;
}

void
rowred(a, rlim, rmod)
GEN a;
long rlim;
GEN rmod;
{
  long j,k,n,pro;
  GEN q;

  n=lg((GEN)a[1])-1;
  for(j=0;j<n;j++)
  {
    for(k=j+1;k<=rlim;k++)
      while (signe(gcoef1(a,j,k)))
      {
	q=rquot(gcoef1(a,j,j),gcoef1(a,j,k));
	a[j+1]=(long)mtran((GEN)a[j+1],(GEN)a[k+1],q,rmod);
	pro=a[j+1];a[j+1]=a[k+1];a[k+1]=pro;
      }
    if (signe(gcoef1(a,j,j))<0)
      for(k=j;k<n;k++) coef1(a,k,j)=lnegi(gcoef1(a,k,j));
    for(k=0;k<j;k++)
    {
      q=rquot(gcoef1(a,j,k),gcoef1(a,j,j));
      a[k+1]=(long)mtran((GEN)a[k+1],(GEN)a[j+1],q,rmod);
    }
  }
}

GEN
rtran(v, w, q)
GEN v;
GEN w;
GEN q;
{
  long av,tetpil;
  GEN p1;

  if (signe(q))
  {
    av=avma;p1=gmul(q,w);tetpil=avma;
    return gerepile(av,tetpil,gsub(v,p1));
  }
  else return v;
}

GEN
mtran(v, w, q, m)
GEN v;
GEN w;
GEN q;
GEN m;
{
  long k;
  
  if (signe(q))
  {
    for(k=0;k<lg(v)-1;k++)
    {
      v[k+1]=(long)rrmdr(subii((GEN)v[k+1],modii(mulii(q,(GEN)w[k+1]),m)),m);
    }
  }
  return v;
}


GEN matinv(x, d)
GEN x;
GEN d;
             
/*=======================================================================
    Calcule d/x  ou  d est entier et x matrice triangulaire inferieure
  entiere dont les coeff diagonaux divisent
  d ( resultat entier).
========================================================================*/
{
  long n,i,j,k,av,av1;
  GEN y,h;

  av=avma;
  y=idmat(n=lg(x)-1);
  for(i=1;i<=n;i++)
    coeff(y,i,i)=ldivii(d,gcoeff(x,i,i));
  for(i=2;i<=n;i++)
    for(j=i-1;j;j--)
    {
      for(h=gzero,k=j+1;k<=i;k++)
	h=gadd(h,mulii(gcoeff(y,i,k),gcoeff(x,k,j)));
      coeff(y,i,j)=ldivii(negi(h),gcoeff(x,j,j));
    }
  av1=avma;
  return gerepile(av,av1,gcopy(y));
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                                                                 */
/*                    BASE D'ENTIERS (ROUND 4)                     */
/*                                                                 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int
fnz(x, j)
GEN x;long j;
{
  long i;
  i=1;while(!signe((GEN)x[i])) i++;
  return i==j;
}

GEN
allbase4(f, code, y, ptw)
GEN f;long code;
GEN *y;
GEN *ptw;
{
/* retourne la base, dans y le discf et dans ptw la factorisation (peut
 etre partielle) de discf */
   
  GEN w,w1,w2,a,da,b,db,bas,q,bdiag,ab,centre;
  long v,n,mf,h,lfa;
  long l,i,j,k,av=avma,tetpil,pro,first;
#ifdef DEBUG_MODE
  long templevel;
#endif
  
  if(typ(f)!=10) err(allbaser1);
  n=lgef(f)-3;if(n<=0) err(allbaser1);
  v=varn(f);
  *y=discsr(f);
#ifdef DEBUG_MODE
  if(DEBUGLEVEL) {timer2();templevel=DEBUGLEVEL;DEBUGLEVEL=5;}
#endif
  switch(code)
  {
    case 0: w=auxdecomp(absi(*y),1);h=lg((GEN)w[1])-1;break; /* base */
    case 1: w=auxdecomp(absi(*y),0);h=lg((GEN)w[1])-1;break; /* smallbase */
    default: w=(GEN)code;
      if((typ(w)!=19)||(lg(w)!=3)) err(factoreder1); /* factoredbase */
      h=lg((GEN)w[1])-1;
      q=gun;for(i=1;i<=h;i++) q=gmul(q,gpui((GEN)coeff(w,i,1),(GEN)coeff(w,i,2),0));
      if(gcmp(absi(q),absi((GEN)*y))) err(factoreder2);
  }
#ifdef DEBUG_MODE
  if(DEBUGLEVEL) 
  {
    DEBUGLEVEL=templevel;
    fprintferr("temps factorisation disc: ");fprintferr("%ld\n",timer2());flusherr();
  }
#endif
  a=idmat(n);da=gun;first=TRUE;
  for(i=1;i<=h;i++)
  {
    mf=itos((GEN)coeff(w,i,2));
    if(mf>1)
    { 
#ifdef DEBUG_MODE
      if(DEBUGLEVEL)
      {
	fprintferr("On traite le cas p^k = ");
	bruterr((GEN)coeff(w,i,1),'g',-1);
	fprintferr("^%ld\n",mf);
      }
#endif
      b=(GEN)maxord((GEN)coeff(w,i,1),f,mf);
      bdiag=cgetg(n+1,17);for(j=1;j<=n;j++) bdiag[j]=coeff(b,j,j);
      db=denom(bdiag); 
      if (!(gcmp1(db)))    /* la matrice est identite   */
      {
	da=gmul(da,db);
	if (first!=TRUE)
	{
	  b=gmul(da,b);a=gmul(db,a);
	  for(j=1;(j<=n)&&(fnz((GEN)a[j],j)&&fnz((GEN)b[j],j));j++);
	  k=j-1;ab=cgetg(2*n-k+1,19);
	  for(j=1;j<=k;j++)
	  {
	    ab[j]=a[j];
	    coeff(ab,j,j)=(long)mppgcd(gcoeff(a,j,j),gcoeff(b,j,j));
	  }
	  for(;j<=n;j++) ab[j]=a[j];
	  for(;j<=2*n-k;j++) ab[j]=b[j+k-n];
	  a=hnf(ab);
	}
	else {a=gmul(b,db);first=FALSE;}
      }
#ifdef DEBUG_MODE
      if(DEBUGLEVEL>=3)
      {
	fprintferr("Le resultat pour ce nombre p est : \n ");
	outerr(b);fprintferr("\n");
      }
#endif
    }
  } 
  for(j=1;j<=n;j++)
  {
    *y=divii(mulii((GEN)coeff(a,j,j),*y),da);
    *y=divii(mulii((GEN)coeff(a,j,j),*y),da);
  }
  if(ptw)
  {
    w1=(GEN)w[1];w2=(GEN)w[2];lfa=0;
    for(j=1;j<=h;j++)
    {
      k=ggval(*y,(GEN)w1[j]);
      w2[j]=lstoi(k);if(k) lfa++;
    }
  }
  tetpil=avma;
  *y=gcopy(*y);bas=cgetg(n+1,17);
  for(j=n-1;j>0;j--)
    if(cmpis((GEN)coeff(a,j,j),2)==1)
    {
      centre=shifti((GEN)coeff(a,j,j),-1);
      for(k=j+1;k<=n;k++)
	if(cmpii((GEN)coeff(a,j,k),centre)==1)
	  for(l=1;l<=j;l++)
	    coeff(a,l,k)=lsubii((GEN)coeff(a,l,k),(GEN)coeff(a,l,j));
    }

  for(k=1;k<=n;k++)
  {
    q=cgetg(k+2,10);q[1]=evalsigne(1)+evallgef(2+k)+evalvarn(v);bas[k]=(long)q;
    for(j=2;j<=k+1;j++) q[j]=ldiv((GEN)coeff(a,j-1,k),da);
  }
  if(ptw)
  {
    w=cgetg(3,19);w[1]=lgetg(lfa+1,18);w[2]=lgetg(lfa+1,18);
    for(l=0,j=1;j<=h;j++)
      if(signe((GEN)w2[j]))
      {l++;coeff(w,l,1)=lcopy((GEN)w1[j]);coeff(w,l,2)=lcopy((GEN)w2[j]);}
  }
  pro=lpile(av,tetpil,0)>>TWOPOTBYTES_IN_LONG;
  bas+=pro;(*y)+=pro;if(ptw) *ptw=w+pro;
  return bas;
}

    /*     p-maximal order of Af        */
    /*     p^m does not divide Df       */

GEN
maxord(p, f, mf)
GEN p;GEN f;long mf;
{
  GEN w,g,h,res,fmp;
  long j,r,v=varn(f),n=lgef(f)-3,av=avma,tetpil,flw;

  if((flw=(cmpsi(n,p)<0)))
  {
    fmp=gmul(gmodulcp(gun,p),f);
    g=gdeuc(fmp,polgcd(fmp,deriv(fmp,v)));
  }
  else 
  {
    w=factmod(f,p);r=lg((GEN)w[1])-1;g=gun;
    for(j=1;j<=r;j++) g=gmul((GEN)coeff(w,j,1),g);
  }
  res=dedek(f,p,g);
  if(itos((GEN)res[1])) {avma=av;return idmat(n);}
  else
    if (2*itos((GEN)res[3])+1>=mf)
    {
      tetpil=avma;
      return gerepile(av,tetpil,gdiv((GEN)res[2],p));
    }
    else
    {
      if(flw) {w=factmod(f,p);r=lg((GEN)w[1])-1;}
      h=bestnu(w);
      if (r==1)
      { 
	res=nilord(p,f,mf,h);
	tetpil=avma;return gerepile(av,tetpil,gcopy(res));
      }
      else
      { 
	tetpil=avma;return gerepile(av,tetpil,Decomp(p,f,mf,polx[v],f,h));
      }
    }
}

GEN
dedek(f, p, g)
GEN f;GEN p;GEN g;
{
  long av=avma,tetpil,index,dk,n=lgef(f)-3;
  long dh,c,i,v=varn(f);
  GEN k,h,unmodp,U,res,b,p1;
  
  res=cgetg(4,17);
#ifdef DEBUG_MODE
  if(DEBUGLEVEL>=3)
  {
    fprintferr(" On est dans Dedekind ");
    if(DEBUGLEVEL>=4)
    {
      fprintferr(" avec les parametres \n" );
      fprintferr(" p=");bruterr(p,'g',-1);
      fprintferr(",  f=");bruterr(f,'g',-1);
    }
    fprintferr("\n");
  } 
#endif
  unmodp=gmodulcp(gun,p);
  g=gmul(g,unmodp);
 
  h=gdivent(gmul(f,unmodp),g);
  k=gdiv(gsub(lift(f),gmul(lift(g),lift(h))),p);
  k=ggcd(gmul(k,unmodp),ggcd(g,h));
  dk=lgef(k)-3;
#ifdef DEBUG_MODE
  if(DEBUGLEVEL>=4)
    fprintferr(" Le pgcd est de degre %ld \n",dk );
#endif
  res[1]= (dk==0)?un:zero;
  if (dk!=0)
  {
    U=gdiv(gmul(f,unmodp),k);
    h=lift(U);
    b=cgetg(2*n+1,19);
    for(c=1;c<=n;c++)
    {
      p1=cgetg(n+1,18);b[c]=(long)p1;
      for(i=1;i<=n;i++) p1[i]=(c==i)?(long)p:zero;
    }
    for(c=n+1;c<=2*n;c++)
    {
      dh=lgef(h)-3;
      p1=cgetg(n+1,18);b[c]=(long)p1;
      for(i=1;i<=dh+1;i++) p1[i]=(long)lift(gmul((GEN)h[1+i],unmodp));
      for(i=dh+2;i<=n;i++) p1[i]=zero;
      if (c!=2*n) {h=gmod(gmul(h,polx[v]),f);} 
    }
#ifdef DEBUG_MODE
    if(DEBUGLEVEL>=4)
    {
      fprintferr(" On construit un nouvel ordre  \n" );
      if(DEBUGLEVEL>=5) outerr(b);
      fprintferr(" On fait sa HNF \n");
    }
#endif
    b=hnfmodid(b,p);
#ifdef DEBUG_MODE
    if(DEBUGLEVEL>=4)
    {
      fprintferr(" Sa HNF est finie \n");
      if(DEBUGLEVEL>=5) outerr(b);
    }
#endif

    res[2]=(long)b;
      
    index=0;
    for(i=1;i<=n;i++)
      index=index+ggval((GEN)coeff(b,i,i),p);
    res[3]=lstoi(n-index);
  }
  else
  {
    res[2]=zero;
    res[3]=un;
  }
  tetpil=avma;
  return gerepile(av,tetpil,gcopy(res));
}

GEN
Decomp(p, f, mf, theta, chi, nu)
GEN p;GEN f;long mf;GEN theta;GEN chi;GEN nu;
{
  long n1,n2,j,i,v2,v1,v=varn(f),av=avma,tetpil;
  GEN unmodpdr,unmodp,unmodpdrp,unmodpkpdr,unmodpmr;
  GEN pk,ph,pmr,pdr;
  GEN b1,b2,b3,a2,a1,e,f1,f2;
  GEN ib1,ib2,ibas,h;

#ifdef DEBUG_MODE
  if(DEBUGLEVEL>=3)
  {
    fprintferr(" On entre dans Decomp ");
    if(DEBUGLEVEL>=4)
    {
      fprintferr(" avec les parametres suivants \n ");
      fprintferr(" p=");bruterr(p,'g',-1);
      fprintferr(",  f=");bruterr(f,'g',-1);
      fprintferr(",  exposant=%ld ",mf);
    }
    fprintferr("\n");
  }
#endif
  
  unmodp=gmodulcp(gun,p);

  pdr=(GEN)respm(f,deriv(f,v),gpuigs(p,mf));
  pmr=mulii(pdr,mulii(pdr,p));

  unmodpmr=gmodulcp(gun,pmr);
  unmodpdr=gmodulcp(gun,pdr);
  unmodpdrp=gmodulcp(gun,mulii(pdr,p));

  b1=gmul(chi,unmodp);  a2=gzero;
  b2=unmodp;            a1=gun;
  b3=gmul(nu,unmodp);
  
  while ( lgef(b3)>3 )
  {
    b1=gdivent(b1,b3);
    b2=gmul(b2,b3);
    b3=lift(gbezout(b2,b1,&a1,&a2));
  }
                           
  e=(GEN)eleval(f,lift(gmul(a1,b2)),theta);
  e=gdiv(lift(gmul(gmul(pdr,e),unmodpdrp)),pdr); 

  pk=p;
  ph=mulii(pdr,pmr); 

      /*    E(t)- e(t) belongs to p^k Op, which is contained in p^(k-df)*Zp[xi]  */

  while (cmpii(pk,ph)==-1)
  {
    e=gmod(gmul(e,gmul(e,gsubsg(3,gmulsg(2,e)))),f);
    pk=gmul(pk,pk);
    unmodpkpdr=gmodulcp(gun,mulii(pk,pdr ));
    e=gdiv(lift(gmul(gmul(pdr,e),unmodpkpdr)),pdr);
  }  
  
  f1=(GEN)gcdpm(f,gmul(pdr,gsubsg(1,e)),mulii(pmr,pdr));f1=lift(gmul(gmod(f1,f),unmodpmr));
  f2=gdivent(f,f1);f2=lift(gmul(gmod(f2,f),unmodpmr));

  n1=lgef(f1)-3;v1=ggval(discsr(f1),p); b1=(GEN)maxord(p,f1,v1);

  ib1=cgetg(n1+1,17);
  for(i=1;i<=n1;i++)
  {
    h=gzero;
    for(j=1;j<=i;j++)
      h=gadd(h,gmul((GEN)coeff(b1,j,i),gpuigs(polx[v],j-1)));
    ib1[i]=(long)h;
  }  
  
  n2=lgef(f2)-3; v2=ggval(discsr(f2),p); b2=(GEN)maxord(p,f2,v2);

  ib2=cgetg(n2+1,17);
  for(i=1;i<=n2;i++)
  {
    h=gzero;
    for(j=1;j<=i;j++)
      h=gadd(h,gmul((GEN)coeff(b2,j,i),gpuigs(polx[v],j-1)));
    ib2[i]=(long)h;
  }
  
  ibas=cgetg(n1+n2+1,17);


  for(j=1;j<=n1;j++)
    ibas[j]=(long)lift(gmul(gmod(gmul(gmul(pdr,(GEN)ib1[j]),e),f),unmodpdr));
  for(j=n1+1;j<=n1+n2;j++)
    ibas[j]=(long)lift(gmul(gmod(gmul(gsubsg(1,e),gmul(pdr,(GEN)ib2[j-n1])),f),unmodpdr));
  tetpil=avma;
  return gerepile(av,tetpil,nbasis(ibas,pdr));
 
}

GEN
nilord(p, fx, mf, gx)
GEN p;GEN fx;long mf;GEN gx;
{
  long La,Ma,first=TRUE,v=varn(fx),av=avma,tetpil; 
  GEN alpha,chi,nu,eta,w,phi;
  GEN res,pm,Dchi,unmodp,unmodpm;
  

#ifdef DEBUG_MODE
  if(DEBUGLEVEL>=3)
  {
    fprintferr(" On entre dans Nilord ");
    if(DEBUGLEVEL>=4)
    {
      fprintferr(" avec les parametres suivants \n ");
      fprintferr(" p=");bruterr(p,'g',-1);
      fprintferr(",  fx=");bruterr(fx,'g',-1);
      fprintferr(",  exposant=%ld,  gx= ",mf);bruterr(gx,'g',-1);
    }
    fprintferr("\n");
  } 
#endif
  
  pm=gpuigs(p,mf+1);

  alpha=polx[v];     chi=fx;     nu=gx;       Dchi=gpuigs(p,mf);

  unmodpm=gmodulcp(gun,pm);
  unmodp=gmodulcp(gun,p);
 
  res=cgetg(4,17);

  while (TRUE)
  {
    if (gcmp0(Dchi))
      alpha=gadd(alpha,gmul(p,polx[v]));
    else
    {
      if (first!=TRUE)
      {
	res=dedek(chi,p,nu);
      }
      else { res[1]=zero;first=FALSE;}
      if (itos((GEN)res[1])==1) 
      { 
	tetpil=avma;
	return gerepile(av,tetpil,pbasis(p,fx,mf,alpha));
      }
      else
      { 
	if (gcmp(vstar(p,chi),gzero)==1)
	{
	  alpha=gadd(alpha,gun);
	  chi=gsubst(chi,v,gsub(polx[v],gun));
	  nu=lift(gmul(gsubst(nu,v,gsub(polx[v],gun)),unmodp));
	}
	w=(GEN)setup(p,chi,polx[v],nu);
	eta=(GEN)w[2];
	La=itos((GEN)w[3]);
	Ma=itos((GEN)w[4]);
	if (La>1)
	  alpha=gadd(alpha,eleval(fx,eta,alpha));
	else
	{
	  w=(GEN)bsrch(p,chi,ggval(Dchi,p),eta,Ma);
	  phi=(GEN)eleval(fx,(GEN)w[2],alpha);
	  if (gcmp1((GEN)w[1]))
	  {
	    tetpil=avma;
	    return gerepile(av,tetpil,Decomp(p,fx,mf,phi,(GEN)w[3],(GEN)w[4]));
	  }
	  else alpha=phi;
	}
      }
    }
    w=(GEN)factcp(p,fx,alpha);
    chi=(GEN)w[1];nu=(GEN)w[2];
    if(cmpis((GEN)w[4],1)==1)
    {
      tetpil=avma;
      return gerepile(av,tetpil,Decomp(p,fx,mf,alpha,chi,nu));
    }
    Dchi=lift(gmul(discsr(lift(gmul(chi,unmodpm))),unmodpm));
    if (gcmp0(Dchi))
      Dchi=discsr(chi);
     
  }
}


/***********************************************************************/
/****             returns                                           ****/
/****       [1,theta,chi,nu]  if theta non-primary                  ****/
/****       [2,phi, * , * ]   if D_phi > D_alpha or M_phi > M_alpha ****/
/***********************************************************************/

GEN
bsrch(p, fa, ka, eta, Ma)
GEN p;GEN fa;long ka;GEN eta;long Ma;
{
  long n=lgef(fa)-3,Da=lgef(eta)-3;
  long c,r,field,j,MaVb,deg,av=avma,tetpil;
  GEN pc,pcc,unmodpcc,Vb;
  GEN beta,b,gamma,delta,pik,w;
  
  pc=respm(fa,deriv(fa,varn(fa)),gpuigs(p,ka));
  c=ggval(pc,p);
  pcc=gmul(pc,pc);
  unmodpcc=gmodulcp(gun,pcc);
  
  r=1+(long)ceil(c/(double)(Da)+gtodouble(gdivsg(c*n-2,mulsi(Da,subis(p,1)))));
  
  b=cgetg(5,17);
  
  beta=gdiv(lift(gpuigs(gmodulcp(eta,fa),Ma)),p);
  
  while(TRUE)
  { 
    beta=gdiv(lift(gmul(gmul(pc,beta),unmodpcc)),pc);
    w=testd(p,fa,c,Da,eta,Ma,beta);
    if(cmpis((GEN)w[1],3)==-1) 
    { tetpil=avma;
    return gerepile(av,tetpil,gcopy((GEN)w));
    } 
      
    Vb=vstar(p,(GEN)w[3]);
      
    MaVb=itos(gmulsg(Ma,Vb));
      
    pik=lift(gpuigs(gmodulcp(eta,fa),MaVb));
      
    gamma=gmod(gmul(beta,(GEN)(vecbezout(pik,fa))[1]),fa);
    gamma=gdiv(lift(gmul(gmul(pc,gamma),unmodpcc)),pc);
    w=(GEN)testd(p,fa,c,Da,eta,Ma,gamma);
    if (cmpis((GEN)w[1],3)==-1) 
    {
      tetpil=avma;
      return gerepile(av,tetpil,gcopy((GEN)w));
    } 

    delta=eltppm(fa,pc,gamma,gpuigs(p,r*Da));
    delta=gdiv(lift(gmul(gmul(pc,delta),unmodpcc)),pc);
    w=(GEN)testd(p,fa,c,Da,eta,Ma,delta);
    if (cmpis((GEN)w[1],3)==-1)
    {
      tetpil=avma;
      return gerepile(av,tetpil,gcopy((GEN)w));
    } 
      
    field=TRUE;
    deg=lgef(delta)-3;
    for(j=0;j<=deg;j++)
      if (!(gcmp0((GEN)delta[j+2])))
	if (ggval((GEN)delta[j+2],p) < 0)  field=FALSE;
    if (field) 
      beta=gsub(beta,gmod(gmul(pik,delta),fa));
    else
    { 
      tetpil=avma;
      return gerepile(av,tetpil,csrch(p,fa,gamma));
    } 
  }
}

/***********************************************************************/
/****    returns                                                    ****/
/****    [1,phi,chi,nu]      if theta non-primary                   ****/
/****    [2,phi,chi,nu]      if D_phi > D_aplha or M_phi > M_alpha  ****/
/****    [3,phi,chi,nu]      otherwise                              ****/
/***********************************************************************/

GEN
testd(p, fa, c, Da, alph2, Ma, theta)
GEN p;GEN fa;long c;long Da;GEN alph2;long Ma;GEN theta;
{
  long Mt,Dt,av=avma,tetpil;
  GEN chit,nut,thet2,b,w;
  
  b=cgetg(5,17);
  
  
  w=factcp(p,fa,theta);
  chit=(GEN)w[1];
  nut=(GEN)w[2];
  Dt=itos((GEN)w[3]);

  if (cmpis((GEN)w[4],1)==1)
  {
    b[1]=un;
    b[2]=(long)theta;
    b[3]=(long)chit;
    b[4]=(long)nut;
    tetpil=avma;
    return gerepile(av,tetpil,gcopy((GEN)b));
  } 

  if (Da< clcm(Da,Dt)) 
  { 
    tetpil=avma;
    return gerepile(av,tetpil,testb(p,fa,Da,theta,Dt));
  }
  
  w=setup(p,fa,theta,nut);
  thet2=(GEN)w[2];
  Mt=itos((GEN)w[4]);
  
  if (Ma < clcm(Ma,Mt))
  {
    tetpil=avma;
    return gerepile(av,tetpil,testc(p,fa,c,alph2,Ma,thet2,Mt));
  }
  else
  {   
    b[1]=(long)stoi(3);
    b[2]=(long)theta;
    b[3]=(long)chit;
    b[4]=(long)nut; 
    tetpil=avma;
    return gerepile(av,tetpil,gcopy((GEN)b));
  }
}


/***********************************************************************/
/*****    Returns [1,phi,chi,nu] if phi non-primary                *****/
/*****            [2,phi,chi,nu] if D_phi = lcm (D_alpha, D_theta) *****/
/***********************************************************************/

GEN
testc(p, fa, c, alph2, Ma, thet2, Mt)
GEN p;
GEN fa;
long c;
GEN alph2;
long Ma;
GEN thet2;
long Mt;

{
  GEN b,pc,ppc,c1,c2,c3,psi,unmodppc,phi,w;
  long g,r,s,t,v=varn(fa),av=avma,tetpil;

  b=cgetg(5,17);
  pc=gpuigs(p,c);
  ppc=mulii(pc,p);
  unmodppc=gmodulcp(gun,ppc);

  g=cbezout(Ma,Mt,&r,&s);
  t=0;
  while (r<0)
  {
    r=r+Mt;
    t++;
  }
  while (s<0)
  {
    s=s+Ma;
    t++;
  }
  c1=lift(gpuigs(gmodulcp(alph2,fa),s));
  c2=lift(gpuigs(gmodulcp(thet2,fa),r));
  c3=gdiv(gmod(gmul(c1,c2),fa),gpuigs(p,t));
  psi=gdiv(lift(gmul(gmul(pc,c3),unmodppc)),pc);
  phi=gadd(polx[v],psi);

  w=factcp(p,fa,phi);
  if(cmpis((GEN)w[4],1)==1)
  {
    b[1]=un;
    b[2]=(long)phi;
    b[3]=w[1];
    b[4]=w[2];
    tetpil=avma;
    return gerepile(av,tetpil,gcopy((GEN)b));
  }
  else
  {   
    b[1]=deux;
    b[2]=(long)phi;
    b[3]=w[1];
    b[4]=w[2]; 
    tetpil=avma;
    return gerepile(av,tetpil,gcopy((GEN)b));
  }
}


/************************************************************************/
/*****   Returns [1,phi,chi,nu] if phi non-primary                  *****/
/*****           [2,phi,chi,nu] if D_phi = lcm (D_alpha, D_theta)   *****/
/************************************************************************/


GEN
testb(p, fa, Da, theta, Dt)
GEN p;GEN fa;long Da;GEN theta;long Dt;
{
  long Dat,t,j,vf=varn(fa),av=avma,tetpil;
  GEN b,w,r,v;
  GEN phi,h;
  
  
  Dat=clcm(Da,Dt);
  b=cgetg(5,17);
  t=0;
  
  while (TRUE)
  {
    t++;
    v=stoi(t);
    h=gzero;
    j=0;
    while (!(gcmp0(v)))
    {
      r=gmod(v,p);
      v=gdivent(v,p);
      h=gadd(h,gmul(r,gpuigs(polx[vf],j)));
      j++;
    }
    phi=gadd(theta,gmod(h,fa));
    w=factcp(p,fa,phi);
    if (cmpis((GEN)w[4],1)==1)
    {
      b[1]=un;
      b[2]=(long)phi;
      b[3]=w[1];
      b[4]=w[2];
      tetpil=avma;
      return gerepile(av,tetpil,gcopy((GEN)b));
    }
    if (cmpis((GEN)w[3],Dat)==0)
    {
      b[1]=deux;
      b[2]=(long)phi;
      b[3]=w[1];
      b[4]=w[2];
      tetpil=avma;
      return gerepile(av,tetpil,gcopy((GEN)b));
    }
  }
}

/***********************************************************************/
/*****     Factorize characteristic polynomial of beta mod p       *****/
/***********************************************************************/

GEN
factcp(p, f, beta)
GEN p;GEN f;GEN beta;
{
  GEN chi,nu,b;
  long v,av=avma,tetpil;
  
  v=varn(f);
  chi=lift(caradj0(gmodulcp(beta,f),v));
  nu=lift(factmod(chi,p));
  
  b=cgetg(5,17);
  
  b[1]=(long)chi;
  b[2]=coeff(nu,1,1);
  b[3]=lstoi(lgef((GEN)b[2])-3);
  b[4]=lstoi(lg((GEN)nu[1])-1);
  tetpil=avma;
  return gerepile(av,tetpil,gcopy((GEN)b));
}

/***********************************************************************/
/*****************          minimum extension valuation     ************/
/***********************************************************************/

GEN
vstar(p, h)
GEN p;GEN h;
{
  long m,first,j,av=avma,tetpil;
  GEN w,v;
  
  m=lgef(h)-3;
  first=TRUE;
  v=gzero;
  for(j=1;j<=m;j++)
    if (!(gcmp0((GEN)h[m-j+2])))
    {
      w=gdiv(stoi(ggval((GEN)h[m-j+2],p)),stoi(j));
      if (first) 
	v=w;
      else 
	if(gcmp(w,v)==-1) v=w;
      first=FALSE;
    }
  tetpil=avma;
  return gerepile(av,tetpil,gcopy(v));
}

/************************************************************************/
/*** Returns [theta_1,theta_2,L_theta,M_theta] with theta non-primary ***/
/***            [1]      [2]     [3]     [4]                         ****/
/************************************************************************/

GEN
setup(p, f, theta, nut)
GEN p;GEN f;GEN theta;GEN nut;
{
  GEN b,t1,t2,v1;
  long Lt,Mt,r,s,c,v,av=avma,tetpil;
  
  v=varn(f);
  b=cgetg(5,17);
  
  t1=eleval(f,nut,theta);
  v1=vstar(p,lift(caradj0(gmodulcp(t1,f),v)));
  
  if (typ(v1)==1) 
  {     
    Lt=itos(v1);
    Mt=1;
  }
  else
  {
    Lt=itos((GEN)v1[1]);
    Mt=itos((GEN)v1[2]);
  }
  
  c=cbezout(Lt,-Mt,&r,&s);
  
  while(r<=0)
  {
    r=r+Mt;
    s=s+Lt;
  }
  t2=gdiv(lift(gpuigs(gmodulcp(t1,f),r)),gpuigs(p,s));
  
  b[1]=(long)t1;
  b[2]=(long)t2;
  b[3]=lstoi(Lt);
  b[4]=lstoi(Mt);
  
  tetpil=avma;
  return gerepile(av,tetpil,gcopy((GEN)b));
}

/***********************************************************************/
/**************          evaluate g(a)                ******************/
/***********************************************************************/

GEN
eleval(f, h, a)
GEN f;GEN h;GEN a;
{
  long n,k,v=varn(f),av=avma,tetpil;
  GEN g,y;
  
  g=gmul(h,polun[v]);
  n=lgef(g)-3;
  y=gzero;
  for(k=n;k>=0;k--)
    y=gmod(gadd(gmul(y,a),(GEN)g[k+2]),f);
  tetpil=avma;
  return gerepile(av,tetpil,gcopy(y));
}

/************************************************************************/
/************ Returns [theta,chi,nu ] with theta non-primary ************/
/************************************************************************/

GEN
csrch(p, fa, gamma)
GEN p;GEN fa;GEN gamma;
{
  GEN b,h,theta,w,v,r;
  long t,j,vf=varn(fa),av=avma,tetpil;
  
  b=cgetg(5,17);
  
  t=0;
  while (TRUE)
  {
    t++;
    v=stoi(t);
    h=gzero;
    j=0;
    while (!(gcmp0(v)))
    {
      r=gmod(v,p);
      v=gdivent(v,p);
      h=gadd(h,gmul(r,gpuigs(polx[vf],j)));
      j++;
    }
    theta=gadd(gamma,gmod(h,fa));
    w=factcp(p,fa,theta);
    if (cmpis((GEN)w[4],1)==1)
    {
      b[1]=un;
      b[2]=(long)theta;
      b[3]=w[1];
      b[4]=w[2];
      tetpil=avma;
      return gerepile(av,tetpil,gcopy((GEN)b));
    }
  }
}

/***********************************************************************/
/*****************          Modular power of an elment        **********/
/***********************************************************************/

GEN
eltppm(f, pd, theta, k)
GEN f;GEN pd;GEN theta;GEN k;
{
  GEN pdd,phi,psi,unmodpdd,q;
  long r,av=avma,tetpil;
  
  pdd=gmul(pd,pd);
  unmodpdd=gmodulcp(gun,pdd);
  phi=pd;
  psi=gmul(pd,theta);
  q=k;
  
  while (cmpis(q,0)!=0)
  { 
    r=q[lgef(q)-1]&1;
    if (r !=0)
    { 
      phi=gmod(gdiv(gmul(phi,psi),pd),f);
      phi=lift(gmul(phi,unmodpdd));
    }
    q=gshift(q,-1);
    if (cmpis(q,0) != 0)
    {
      psi=gmod(gdiv(gmul(psi,psi),pd),f);
      psi=lift(gmul(psi,unmodpdd));
    }
  }
  tetpil=avma;
  return gerepile(av,tetpil,gdiv(phi,pd));
}

/***********************************************************************/
/************** Hermite normal form (n x n ) mod p^m      **************/
/***********************************************************************/
 
GEN
hnfpm(a, pm)
GEN a;GEN pm;
{
  GEN b,cb,unmodpm,c00,c11,c12,c22,c21,c13,c23,q;
  long m,n,j,k,c,i,l,av=avma,tetpil;
  
  unmodpm=gmodulcp(gun,pm);
  m=lg(a)-1;                  /* col  */
  n=lg((GEN)a[1])-1;          /* ligne */
  
  b=cgetg(m+n+1,19);
  
  for(k=1;k<=m;k++)
  {
    b[k]=lgetg(n+1,18);
    for(j=1;j<=n;j++)
    {
      coeff(b,j,k)=(long)lift(gmul((GEN)coeff(a,j,k),unmodpm));
    }
  }
  for(k=m+1;k<=m+n;k++)
  {
    b[k]=lgetg(n+1,18);
    for(j=1;j<=n;j++)
    {
      coeff(b,j,k)=((j+m)==(k))? (long)pm:zero;
    }
  }
  k=m+n;
  for(i=n;i>=1;i--)
  {
    for(j=k-1;j>=1;j--)
      if (!(gcmp0((GEN)coeff(b,i,j))))
      {
	c00=gbezout((GEN)coeff(b,i,k),(GEN)coeff(b,i,j),&c11,&c12);
	c21=gdiv(gneg((GEN)coeff(b,i,j)),c00);
	c22=gdiv((GEN)coeff(b,i,k),c00);
	    
	for(l=1;l<=n;l++)
	{ 
	  c13=gadd(gmul(c11,(GEN)coeff(b,l,k)),gmul(c12,(GEN)coeff(b,l,j)));
	  c23=gadd(gmul(c21,(GEN)coeff(b,l,k)),gmul(c22,(GEN)coeff(b,l,j)));
	  if (j<=m)
	  { 
	    coeff(b,l,k)=(long)lift(gmul(c13,unmodpm));
	    coeff(b,l,j)=(long)lift(gmul(c23,unmodpm));
	  }
	  else
	  {
	    coeff(b,l,k)=(long)c13;
	    coeff(b,l,j)=(long)c23;
	  }
	}
      }
    if (cmpis((GEN)coeff(b,i,k),0)==-1)
      for(l=1;l<=n;l++) coeff(b,l,k)=lneg((GEN)coeff(b,l,k));
    for(c=k+1;c<=m+n;c++)
    {
      q=gdivent((GEN)coeff(b,i,c),(GEN)coeff(b,i,k));
      if (!(gcmp0(q)))
	for(l=1;l<=n;l++)
	  coeff(b,l,c)=lsub((GEN)coeff(b,l,c),gmul(q,(GEN)coeff(b,l,k)));
    }
    k--;
  }
  cb=cgetg(n+1,19);
  for(k=m+1;k<=m+n;k++) cb[k-m]=b[k];
  tetpil=avma;
  return gerepile(av,tetpil,gcopy(cb));
}

/**********************************************************************/
/******       polynomial gcd mod p^m (assumes f1 monic)        ********/
/**********************************************************************/

GEN
gcdpm(f1, f2, pm)
GEN f1;GEN f2;GEN pm;
{
  long n,c,deg,k,j,v=varn(f1),av=avma,tetpil;
  GEN a,h,unmodpm,b;
  
  
  n=lgef(f1)-3;
  unmodpm=gmodulcp(gun,pm);
  a=cgetg(n+1,19);
  
  h=lift(gmul(gmod(f2,f1),unmodpm));
  for(k=1;k<=n;k++)
    a[k]=lgetg(n+1,18);
  for(j=1;j<=n;j++)
  { deg=lgef(h)-3;
  for(k=1;k<=deg+1;k++)
    coeff(a,k,j)=h[k+1];
  for(k=deg+2;k<=n;k++)
    coeff(a,k,j)=zero;
      
  if (j<n)
    h=lift(gmul(gmod(gmul(polx[v],h),f1),unmodpm));
  }

  a=hnfpm(a,pm);

  c=0;
  
  for(j=n;j>=1;j--)
    if (!(gcmp0(lift(gmul((GEN)coeff(a,j,j),unmodpm)))))
      c=j;
  b=gmul(gzero,polun[v]);
  for(k=1;k<=c;k++)
    b=gadd(b,gmul(gdiv((GEN)coeff(a,k,c),(GEN)coeff(a,c,c)),gpuigs(polx[v],k-1)));
  tetpil=avma;
  return gerepile(av,tetpil,gcopy((GEN)b));
}

/***********************************************************************/
/*****      reduced resultant mod p^m (assumes f1 monic)        ********/
/***********************************************************************/

GEN
respm(f1, f2, pm)
GEN f1;GEN f2;GEN pm;
{
  long av=avma,tetpil;
  GEN a1,a2,pc,g,unmodpm;
 
  unmodpm=gmodulcp(gun,pm);
  g=gbezout(f1,f2,&a1,&a2);

  a1=lift(gmul(gmul(pm,a1),unmodpm));
  a2=lift(gmul(gmul(pm,a2),unmodpm));

  pc=ggcd(pm,content(a1));
  pc=ggcd(pc,content(a2));

  tetpil=avma;

  return gerepile(av,tetpil,gdiv(pm,pc));
}

/***********************************************************************/
/***********  Normalized integral basis                   **************/
/***********************************************************************/

GEN
nbasis(ibas, pd)
GEN ibas;GEN pd;
{
  long n,j,k,m,av=avma,tetpil;
  GEN a,unmodpd;
  
  unmodpd=gmodulcp(gun,pd);
  n=lg(ibas)-1;
  
  a=cgetg(n+1,19);
  m=lgef((GEN)ibas[1])-2;
  for(k=1;k<=n;k++)
  {
    m=lgef((GEN)ibas[k])-2;
    a[k]=lgetg(n+1,18);
    for(j=1;j<=m;j++)
      coeff(a,j,k)=(long)((GEN)ibas[k])[j+1];
    for(j=m+1;j<=n;j++)
      coeff(a,j,k)=zero;
  }
  
  a=hnfpm(a,pd);
  tetpil=avma;
  return gerepile(av,tetpil,gdiv(a,pd));
}

/************************************************************************/
/**************                Power basis                 **************/
/************************************************************************/

GEN
pbasis(p, f, mf, alpha)
GEN p;GEN f;long mf;GEN alpha;
{
  long n,j,k,df,dh,v=varn(f),av=avma,tetpil;
  GEN a,h,pd;
  
  df=mf/2;
  pd=gpuigs(p,df);
  
  n=lgef(f)-3;
  a=cgetg(n+1,19);
  h=gmul(pd,polun[v]);
  
  for(k=1;k<=n;k++)
  { 
    dh=lgef(h)-3;
    a[k]=lgetg(n+1,18);
    for(j=1;j<=dh+1;j++)
      coeff(a,j,k)=h[j+1];
    for(j=dh+2;j<=n;j++)
      coeff(a,j,k)=zero;
      
    h=gmod(gmul(alpha,h),f);
  }
  
  a=(GEN)hnfpm(a,pd);
  tetpil=avma;
  return gerepile(av,tetpil,gdiv(a,pd));
}

/***********************************************************************/
/**************        Pick best divisor of chi           **************/
/***********************************************************************/

GEN
bestnu(w)
GEN w;
{
  long r,j,av=avma,tetpil;
  GEN g,h;
  
  r=lg((GEN)w[1])-1;
  g=polun[0];
  
  for(j=1;j<=r;j++)
  {
    h=(GEN)coeff(w,j,1);
    if (lgef(h)>lgef(g)) g=h;
  }
  tetpil=avma;
  return gerepile(av,tetpil,lift(g));
}

/**********************************************************************/
/******                        bezout etendu                    *******/
/******                  Return d=pgcd(a,b)  and  &u,&v         *******/
/**********************************************************************/

long
cbezout(a, b, u, v)
long a;long b;long *u;long *v;
{
  long d,v3,v1,q,r,t;
  
  (*u)=1;
  d=a;
  v1=0;
  v3=b;
  
  while (1)
  {
    if (v3==0) 
    { 
      (*v)=(d-a*(*u))/b;
      return d;
    }
    q=d/v3;
    r=d%v3;
    t=(*u)-v1*q;
    (*u)=v1;
    d=labs(v3);
    v1=t;
    v3=r;
  }
}

long
clcm(a, b)
long a;long b;
{
  long d,r,v1,q;
  
  d=a;
  r=b;

  while (1)
  {
    if (r==0) 
    { 
      return (a*b)/d;
    }
    v1=r;
    q=d/r;
    r=d%r;
    d=labs(v1);
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                                                                 */
/*                 ALGORITHME DE BUCHMANN-LENSTRA                  */
/*                                                                 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



GEN
pradical(nf, p)
GEN nf;
GEN p;
              
/* Calcule une F_p base du p-radical de Z_K,i.e une
 matrice N fois r (r=dimension sur F_p du p-radical) */

{
  long av=avma,tetpil,j,k,N=lgef((GEN)nf[1])-3;
  GEN p1,unmodp,zmodp,x,m;

  p1=p;while(cmpis(p1,N)<0) p1=mulii(p1,p);
  m=cgetg(N+1,19);
  unmodp=gmodulcp(gun,p);zmodp=gmul(gzero,unmodp);
  for(k=1;k<=N;k++)
  {
    x=cgetg(N+1,18);
    for(j=1;j<=N;j++) x[j]=(long)zmodp;
    x[k]=(long)unmodp;
    m[k]=(long)element_pow(nf,x,p1);
  }
  tetpil=avma;return gerepile(av,tetpil,gmul(unmodp,ker(m)));
}
      
GEN
pol_min(alpha, nf, p, algebre, algebre1)
GEN alpha;
GEN nf;
GEN p;
GEN algebre;
GEN algebre1;
                                     
/* Calcule le polynome minimal de alpha dans algebre (polynome a
 coefficients dans F_p) */

{
  long av=avma,tetpil,i,j,N=lgef((GEN)nf[1])-3,k=N-lg(algebre1)+1;
  GEN puiss,puiss2,noyau,unmodp,zmodp,vecteur;

  unmodp=gmodulcp(gun,p);zmodp=gmul(unmodp,gzero);
  vecteur=gmul(unmodp,(GEN)(idmat(N)[k+1]));
  puiss=cgetg(N+2,19);puiss[1]=(long)vecteur;
  for(i=1;i<=N;i++)
    puiss[i+1]=(long)(inverseimage(algebre,element_pow(nf,alpha,stoi(i))));
  puiss2=cgetg(N+2,19);
  for(i=1;i<=N+1;i++)
  {
    puiss2[i]=lgetg(N-k+1,18);
    for(j=1;j<=N-k;j++)
      coeff(puiss2,j,i)=coeff(puiss,k+j,i);
  }
  noyau=gmul(ker(puiss2),unmodp);
  tetpil=avma;return gerepile(av,tetpil,gtopolyrev((GEN)noyau[1],0));
}

GEN
eval_pol(nf, pol, alpha, p, algebre, algebre1)
GEN nf;
GEN pol;
GEN alpha;
GEN p;
GEN algebre;
GEN algebre1;
                                         
/* Evalue le polynome pol en alpha,element de nf */

{
  long av=avma,tetpil,i;
  long N=lgef((GEN)nf[1])-3,k=N-lg(algebre1)+1,lx=lgef(pol)-3;
  GEN res,valeur,unmodp,zmodp;

  res=cgetg(N+1,18);
  unmodp=gmodulcp(gun,p);zmodp=gmul(unmodp,gzero);
  for(i=2;i<=N;i++) res[i]=(long)zmodp;
  res[1]=lmul(unmodp,(GEN)pol[lx+2]);
  for(i=lx+1;i>=2;i--)
  {
    res=element_mul(nf,alpha,res);res[1]=ladd((GEN)res[1],(GEN)pol[i]);
  }
  res=inverseimage(algebre,res);
  valeur=cgetg(N-k+1,18);
  for(i=1;i<=N-k;i++) valeur[i]=res[k+i];
  tetpil=avma;return gerepile(av,tetpil,gmul(algebre1,valeur));
}

GEN
kerlens2(x, pgen)
GEN x;
GEN pgen;
{
  long i,j,k,t,nbc,nbl,av,av1;
  GEN a,c,l,d,y,q;

  av=avma;
  a=gmul(x,gmodulcp(gun,pgen));
  nbl=nbc=lg(x)-1;
  c=cgetg(nbl+1,17);l=cgetg(nbc+1,17);
  d=cgetg(nbc+1,17);
  for(i=1;i<=nbl;i++) c[i]=0;
  k=1;t=1;
  while((t<=nbl)&&(k<=nbc))
  {
    for(j=1;j<k;j++)
      for(i=1;i<=nbl;i++)
	if(i!=l[j])
	  coeff(a,i,k)=lsub(gmul((GEN)d[j],gcoeff(a,i,k)),gmul(gcoeff(a,l[j],k),gcoeff(a,i,j)));
    t=1;while((t<=nbl)&&((c[t])||gcmp0(gcoeff(a,t,k)))) t++;
    if (t<=nbl) {d[k]=coeff(a,t,k);c[t]=k;l[k++]=t;}
  }
  if(k>nbc) err(kerlenser);
  y=cgetg(nbc+1,18);
  y[1]=(k>1)?(long)coeff(a,l[1],k):un;
  for(q=gun,j=2;j<k;j++)
  {
    q=gmul(q,(GEN)d[j-1]);
    y[j]=lmul(gcoeff(a,l[j],k),q);
  }
  if(k>1) y[k]=lneg(gmul(q,(GEN)d[k-1]));
  for(j=k+1;j<=nbc;j++) y[j]=zero;
  av1=avma;
  return gerepile(av,av1,gcopy(lift(y))); 
}

GEN
kerlens(x, pgen)
GEN x;
GEN pgen;
{
  long i,j,k,t,nbc,nbl,p,q,*c,*l,*d,**a;
  GEN y;

  if(cmpis(pgen,(MAXHALFULONG>>1))>0)
    return kerlens2(x,pgen);
/* ici p<=(MAXHALFULONG>>1) ==> simple precision (long de C) */
  p=itos(pgen);
  nbl=nbc=lg(x)-1;
  a=(long**)newbloc(nbc+1);
  for(j=1;j<=nbc;j++)
  {
    c=a[j]=newbloc(nbl+1);
    for(i=1;i<=nbl;i++) c[i]=itos(modis(gcoeff(x,i,j),p));
  }
  c=newbloc(nbl+1);
  l=newbloc(nbc+1);
  d=newbloc(nbc+1);
  for(i=1;i<=nbl;i++) c[i]=0;
  k=1;t=1;
  while((t<=nbl)&&(k<=nbc))
  {
    for(j=1;j<k;j++)
      for(i=1;i<=nbl;i++)
	if(i!=l[j]) a[k][i]=(d[j]*a[k][i]-a[j][i]*a[k][l[j]]) % p;
    t=1;while((t<=nbl)&&((c[t])||(!a[k][t]))) t++;
    if (t<=nbl) {d[k]=a[k][t];c[t]=k;l[k++]=t;}
  }
  if(k>nbc) err(kerlenser);
  y=cgetg(nbc+1,18);
  t=(k>1) ? a[k][l[1]]:1;
  y[1]=(t>0)? lstoi(t):lstoi(t+p);
  for(q=1,j=2;j<k;j++)
  {
    q=(q*d[j-1])%p;
    t=(a[k][l[j]]*q)%p;
    y[j]=(t>0)? lstoi(t):lstoi(t+p);
  }
  if(k>1)
  {
    t=(q*d[k-1])%p;
    y[k]=(t>0)? lstoi(p-t):lstoi(-t);
  }
  for(j=k+1;j<=nbc;j++) y[j]=zero;
  killbloc(c);killbloc(l);killbloc(d);
  for(j=1;j<=nbc;j++) killbloc(a[j]);killbloc((GEN)a);
  return y;
}
  
GEN
lens(nf, p, a)
GEN nf;
GEN p;
GEN a;
                
/* Calcule la constante de lenstra de l'ideal p.Z_K+a.Z_K ou a est un
vecteur sur la base d'entiers */

{
  long av=avma,tetpil,N=lgef((GEN)nf[1])-3,j;
  GEN mat;

  mat=cgetg(N+1,19);for(j=1;j<=N;j++) mat[j]=(long)element_mulid(nf,a,j);
  tetpil=avma;return gerepile(av,tetpil,kerlens(mat,p));
}

GEN
two_elt(nf, p, ideal)
GEN nf;
GEN p;
GEN ideal;
                    
/* Recoit un ideal (mod p) et calcule une representation a deux
 elements (ideal non egal a Z_K) */

{
  long av=avma,av1,tetpil,N=lgef((GEN)nf[1])-3,m,r,i,j,fl;
  GEN beta,alpha,lambda,norme,pf;

  m=lg(ideal)-1;
  if(!m)
  {alpha=cgetg(N+1,18);for(i=1;i<=N;i++) alpha[i]=zero;return alpha;}
  beta=gmodulcp(gmul((GEN)nf[7],lift(ideal)),(GEN)nf[1]);
  pf=gpuigs(p,N-m);fl=r=1;
  for(i=1;(i<=m)&&fl;i++)
  {
    alpha=(GEN)beta[i];norme=gnorm(alpha);
    if(signe(modii(divii(norme,pf),p))) fl=0;
    else
    {
      alpha=gadd(alpha,p);norme=gnorm(alpha);
      if(signe(modii(divii(norme,pf),p))) fl=0;
    }
  }
  if(fl)
  {
    lambda=cgeti(m+1);
    av1=avma;
    for(i=1;i<=m;i++) lambda[i]=r;
    do
    {
      avma=av1;
      alpha=gmodulcp(gzero,(GEN)nf[1]);
      for(i=1;i<=m;i++) alpha=gadd(alpha,gmulsg(lambda[i],(GEN)beta[i]));
      norme=gnorm(alpha);
      if(signe(modii(divii(norme,pf),p))) fl=0;
      else
      {
	alpha=gadd(alpha,p);norme=gnorm(alpha);
	if(signe(modii(divii(norme,pf),p))) fl=0;
      }
      if(fl)
      {
	for(j=m;(lambda[j]+r)==0;j--);
	lambda[j]--;
	for(i=j+1;i<=m;i++) lambda[i]=r;
	for(j=1;(j<m)&&(!lambda[j]);j++);
	if(!lambda[j])
	{
	  r++;for(i=1;i<=m;i++) lambda[i]=r;
	  if(cmpis(p,(r<<1))<0) err(talker,"bug in two_elt");
	}
      }
    }
    while(fl);
  }
  alpha=lift(alpha);beta=cgetg(N+1,18);
  for(i=1;i<=N;i++) beta[i]=(long)truecoeff(alpha,i-1);
  alpha=gmul((GEN)nf[8],beta);
  alpha=gmul(gmodulcp(gun,p),alpha);
  alpha=centerlift(alpha);
  if(!signe(modii(divii(subres(gmul((GEN)nf[7],alpha),(GEN)nf[1]),pf),p)))
    alpha[1]=(long)gadd((GEN)alpha[1],p);
  tetpil=avma;return gerepile(av,tetpil,gcopy(alpha));
}


GEN
primedec(nf, p)
GEN nf;
GEN p;
              
/* Recoit un corps de nombres nf et un premier p,ressort une liste 
 des ideaux premiers au dessus de p dans le format vu plus haut, dans
l'ordre croissant des degres residuels */

{
  long av=avma,tetpil,i,j,k,v,kbar,l3,np,c,i1,i2,indice,N,lp;
  GEN f,ff,list,list2,ip,elementh,hensemble;
  GEN algebre,algebre1,b,b2,mat1,mat2;
  GEN alpha,beta,beta1,p1,p2,p3,unmodp,zmodp,vecteur,pol,f1,polg;
  GEN T,pidmat,ppuin;

#ifdef DEBUG_MODE
  if(DEBUGLEVEL>=3) timer2();
#endif
  nf=checknf(nf);  
  T=(GEN)nf[1];N=lgef(T)-3;
  if(signe(modii((GEN)nf[4],p)))
  {
    f=centerlift(ff=factmod(T,p));np=lg((GEN)f[1]);
#ifdef DEBUG_MODE
    if(DEBUGLEVEL>=6) {fprintferr("temps factmod: ");fprintferr("%ld\n",timer2());flusherr();}
#endif
    list=cgetg(np,17);
    for(i=1;i<np;i++)
    {
      p1=(GEN)(list[i]=lgetg(6,17));
      p1[1]=(long)p;p3=gcoeff(f,i,1);l3=lgef(p3)-1;
      p2=cgetg(N+1,18);
      if(l3==(N+2)) 
      {
	p1[2]=(long)p2;p1[3]=un;p1[4]=lstoi(N);
	p3=cgetg(N+1,18);p1[5]=(long)p3;p3[1]=un;
	p2[1]=(long)p;for(j=2;j<=N;j++) p3[j]=p2[j]=zero;
      }
      else
      {
	v=ggval(subres(p3,T),p)/(l3-2);if(v>1) p3[2]=ladd((GEN)p3[2],p);
	for(j=1;j<l3;j++) p2[j]=p3[j+1];
	for(j=l3;j<=N;j++) p2[j]=zero;
	p1[2]=lmul((GEN)nf[8],p2);
	p1[3]=(long)coeff(f,i,2);
	p1[4]=lstoi(l3-2);
	p3=gdiv(T,gcoeff(ff,i,1));l3=lgef(p3)-1;
	for(j=1;j<l3;j++) p2[j]=p3[j+1];
	for(j=l3;j<=N;j++) p2[j]=zero;
	p1[5]=(long)centerlift(gmul((GEN)nf[8],p2));
      }
    }
#ifdef DEBUG_MODE
    if(DEBUGLEVEL>=6) {fprintferr("temps lens etc...: ");fprintferr("%ld\n",timer2());flusherr();}      
#endif
    p1=stoi(4);tetpil=avma;return gerepile(av,tetpil,vecsort(list,p1));
  }
  else 
  {
    unmodp=gmodulcp(gun,p);zmodp=gmodulcp(gzero,p);
    list=cgetg(N+1,17);for(i=1;i<=N;i++) list[i]=lgetg(6,17);
    f=centerlift(ff=factmod(T,p));indice=0;
#ifdef DEBUG_MODE
    if(DEBUGLEVEL>=6) {fprintferr("temps factmod: ");fprintferr("%ld\n",timer2());flusherr();}
#endif
    f1=(GEN)ff[1];np=lg(f1);polg=(GEN)f1[1];
    for(i=2;i<np;i++) polg=gmul(polg,(GEN)f1[i]);
    polg=gmul(unmodp,gdiv(gsub(gmul(lift(polg),lift(gdiv(T,polg))),T),p));
    for(i=1;i<np;i++)
    {
      if((gcmp1(gcoeff(f,i,2)))||(!gdivise(polg,(GEN)f1[i])))
      {
	indice++;p1=(GEN)list[indice];
	p1[1]=(long)p;p3=gcoeff(f,i,1);l3=lgef(p3)-1;
	p2=cgetg(N+1,18);
	if(l3==(N+2)) 
	{
	  p1[2]=(long)p2;p1[3]=un;p1[4]=lstoi(N);
	  p3=cgetg(N+1,18);p1[5]=(long)p3;p3[1]=un;
	  p2[1]=(long)p;for(j=2;j<=N;j++) p3[j]=p2[j]=zero;
	  p3=gmul(unmodp,polun[varn(T)]);
	}
	else
	{
	  v=ggval(subres(p3,T),p)/(l3-2);
	  if(v>1) p3[2]=ladd((GEN)p3[2],p);
	  for(j=1;j<l3;j++) p2[j]=p3[j+1];
	  for(j=l3;j<=N;j++) p2[j]=zero;
	  p1[2]=lmul((GEN)nf[8],p2);
	  p1[3]=(long)coeff(f,i,2);
	  p1[4]=lstoi(l3-2);
	  p3=gdiv(T,gcoeff(ff,i,1));l3=lgef(p3)-1;
	  for(j=1;j<l3;j++) p2[j]=p3[j+1];
	  for(j=l3;j<=N;j++) p2[j]=zero;
	  p1[5]=(long)centerlift(gmul((GEN)nf[8],p2));
	}
	beta=(indice==1)?p3:gdiv(beta,gcoeff(ff,i,1));
      }
    }
#ifdef DEBUG_MODE
    if(DEBUGLEVEL>=3) {fprintferr("temps %ld facteurs non ramifies : ",indice);fprintferr("%ld\n",timer2());flusherr();}      
#endif
    ip=pradical(nf,p);
#ifdef DEBUG_MODE
    if(DEBUGLEVEL>=3) {fprintferr("temps pradical: ");fprintferr("%ld\n",timer2());flusherr();}
#endif
    if(indice)
    {
      if(typ(beta)!=10) err(talker,"bugbeta in primedec");
      p2=cgetg(N+1,18);l3=lgef(beta)-1;
      for(j=1;j<l3;j++) p2[j]=beta[j+1];
      for(j=l3;j<=N;j++) p2[j]=zero;
      beta=gmul((GEN)nf[8],p2);beta1=lift(beta);
      lp=lg(ip)-1;p1=cgetg(lp+lp+N+1,19);
      for(i=1;i<=N;i++) p1[i]=(long)element_mulid(nf,beta,i);
      for(;i<=N+lp;i++) 
      {
	p2=lift((GEN)ip[i-N]);p1[i]=ldiv(element_mul(nf,p2,beta1),p);
	p1[i+lp]=(long)p2;
      }
      ip=image(gmul(unmodp,p1));
    }
    pidmat=cgetg(N+1,18);vecteur=cgetg(N+1,18);
    for(i1=2;i1<=N;i1++) {pidmat[i1]=zero;vecteur[i1]=(long)zmodp;}
    pidmat[1]=(long)p;vecteur[1]=(long)unmodp;
    ppuin=gpuigs(p,N);c=0;
    hensemble=cgetg(N+1,17); 
    if(lg(ip)<N+1) {c=1;hensemble[1]=(long)ip;}
    while(c)
    {  
      elementh=(GEN)(hensemble[c]);k=lg(elementh)-1;kbar=N-k;
      algebre=gmul(unmodp,suppl(concat(elementh,vecteur)));
      algebre1=cgetg(kbar+1,19);
      for(i1=1;i1<=kbar;i1++) algebre1[i1]=algebre[i1+k];
      b=cgetg(kbar+1,19);
      for(i1=1;i1<=kbar;i1++)
	b[i1]=lsub(element_pow(nf,(GEN)algebre1[i1],p),(GEN)algebre1[i1]);
      b2=inverseimage(algebre,b);
      mat1=cgetg(kbar+1,19);
      for(i1=1;i1<=kbar;i1++) mat1[i1]=lgetg(kbar+1,18);
      for(i1=1;i1<=kbar;i1++)
	for(i2=1;i2<=kbar;i2++)
	  coeff(mat1,i2,i1)=coeff(b2,k+i2,i1);
      mat2=cgetg(k+N+1,19);
      for(i1=1;i1<=k;i1++) mat2[i1]=elementh[i1];
      mat1=gmul(unmodp,ker(mat1));
      if(lg(mat1)>2)
      {
	alpha=gmul(algebre1,(GEN)mat1[2]);
	pol=pol_min(alpha,nf,p,algebre,algebre1);
	setvarn(pol,0);
	p1=(GEN)factmod(pol,p)[1];
	for(i=1;i<lg(p1);i++)
	{
	  beta1=eval_pol(nf,(GEN)p1[i],alpha,p,algebre,algebre1);
	  for(i1=1;i1<=N;i1++)
	    mat2[k+i1]=(long)element_mulid(nf,beta1,i1);
	  hensemble[c]=(long)(image(mat2));c++;
	}
	c--;
#ifdef DEBUG_MODE
	if(DEBUGLEVEL>=3) {fprintferr("temps hensemble[%ld]: ",c);fprintferr("%ld\n",timer2());flusherr();}
#endif
      }
      else
      {
	indice++;p1=(GEN)list[indice];
	p1[1]=(long)p;p1[4]=lstoi(kbar);
	p1[2]=(long)two_elt(nf,p,elementh);
	p1[5]=(long)lens(nf,p,(GEN)p1[2]);
	p1[3]=lstoi(element_val2(nf,pidmat,ppuin,p1));
	c--;
#ifdef DEBUG_MODE
	if(DEBUGLEVEL>=3) {fprintferr("temps hensemble[%ld]: ",c);fprintferr("%ld\n",timer2());flusherr();}
#endif
      }
    }
    list2=cgetg(indice+1,17);
    for(i=1;i<=indice;i++) list2[i]=list[i];
    p1=stoi(4);tetpil=avma;
    return gerepile(av,tetpil,vecsort(list2,p1));
  }
}

long
idealval(nf, ix, vp)
GEN nf;
GEN ix;
GEN vp;
                  
/* recoit un ideal ix et un ideal premier vp dans le format
donne par primedec et calcule la valuation de ix en vp */

{
  long N,v,vd,w,av=avma,i,j,bo;
  GEN mat,x,d,bp,p,p1,r,denx;

  nf=checknf(nf);  
  if((typ(vp)!=17)||(lg(vp)!=6)) err(idealer3);
  if((typ(ix)<=10)||(typ(ix)==18)) return element_val(nf,ix,vp);
  N=lgef((GEN)nf[1])-3;p=(GEN)vp[1];
  if((typ(ix)==17)&&(lg(ix)==3)) x=(GEN)ix[1]; else x=ix;
  if(typ(x)!=19) err(idealer2);
  denx=denom(x);
  if(!gcmp1(denx)) x=gmul(denx,x);
  if(lg((GEN)x[1])!=(N+1)) err(idealer4);
  if(lg(x)!=(N+1)) x=idealmul(nf,x,idmat(N));
  for(d=gun,i=1;i<=N;i++) d=mulii(d,(GEN)coeff(x,i,i));
  v=ggval(d,p);vd=ggval(denx,p);
  if(!v) return -vd*itos((GEN)vp[3]);
  bo=0;w=0;bp=(GEN)vp[5];
  do
  {
    if(w) {for(i=1;i<=N;i++) mat[i]=(long)element_muli(nf,(GEN)mat[i],bp);}
    else 
    {
      mat=cgetg(N+1,19);
      for(i=1;i<=N;i++) mat[i]=(long)element_mulh(nf,i,N,(GEN)x[i],bp);
    }
    if(divise(gcoeff(mat,N,N),p))
    {
      for(j=1;j<=N;j++)
	for(i=1;i<=N;i++)
	{
	  p1=dvmdii(gcoeff(mat,i,j),p,&r);
	  if(signe(r)) goto labeliv; else coeff(mat,i,j)=(long)p1;
	}
      w++;
    }
    else bo=1;
  }
  while((bo==0)&&(w<v));
  labeliv:
  avma=av;return w-vd*itos((GEN)vp[3]);
}

/*******************************************************************
                           ROUND 2 relatif

  Entree:   nf = corps de base K dans le format initalg.
            x polynome unitaire a coefficients dans Z_K de deg n
	    definissant une extension relative L=K(theta);
	    La variable de x doit etre de numero strictement
	    inferieur a celle de nf[1].
  Sortie:   retourne une pseudo-base [A,I] de Z_L, ou A est une matrice
            nxn a coefficients dans nf[1] sous forme HNF et I un vecteur
	    d'ideaux a n composantes
            
*******************************************************************/
#if 0

GEN rnfordmax(GEN nf, GEN pol, GEN pr, GEN unnf, GEN zeronf, GEN id, GEN psid, GEN powbasis);
GEN rnfjoinmodules(GEN nf, GEN x, GEN y);

void
checkbnf(bnf)
GEN bnf;
{
  if(typ(bnf)!=17) err(idealer1);
  if(lg(bnf)!=9)
  {
    if((lg(bnf)==10)&&(typ((GEN)bnf[1])==10))
      err(talker,"please apply buchinit first in rnf function");
    else err(idealer1);
  }
}

#endif

GEN
checknf(nf)
GEN nf;
{
  if(typ(nf)!=17) err(idealer1);
  if(lg(nf)==10) return nf;
  if(lg(nf)==9) return checknf((GEN)nf[7]);
  err(idealer1);return gnil;
}

#if 0
GEN
rnfround2all(nf, pol, all)
GEN nf;
GEN pol;
long all;
{
  long av=avma,tetpil,i,j,n,N,nbidp,ipr,vpol,cpt;
  GEN p1,p2,p3,p4,polnf,fact,list,ep,unnf,zeronf,id,A,I,W,pseudo,y,discpol,psid,powbasis,d,D;

  nf=checknf(nf);
  polnf=(GEN)nf[1];vpol=varn(pol);
  if((typ(pol)!=10)||(vpol>=varn(polnf)))
    err(talker,"incorrect polynomial in relativeround2");
  N=lgef(polnf)-3;n=lgef(pol)-3;discpol=discsr(pol);
  fact=idealfactor(nf,discpol);list=(GEN)fact[1];ep=(GEN)fact[2];
  nbidp=lg(list)-1;
#ifdef DEBUG_MODE
  if(DEBUGLEVEL>1)
  {
    fprintferr(" Ideaux a considerer :\n");
    for(i=1;i<=nbidp;i++)
    {
      j=itos((GEN)ep[i]);
      if(j>1) {bruterr((GEN)list[i],'g',-1);fprintferr("^%ld\n",j);}
    }
    flusherr();
  }
#endif
  unnf=cgetg(N+1,18);for(i=2;i<=N;i++) unnf[i]=zero;unnf[1]=un;
  zeronf=cgetg(N+1,18);for(i=1;i<=N;i++) zeronf[i]=zero;
  id=idmat(N);A=cgetg(n+1,19);
  for(j=1;j<=n;j++)
  {
    p1=cgetg(n+1,18);A[j]=(long)p1;
    for(i=1;i<=n;i++) p1[i]=(i==j)?(long)unnf:(long)zeronf;
  }
  I=cgetg(n+1,17);for(i=1;i<=n;i++) I[i]=(long)id;
  pseudo=cgetg(3,17);pseudo[1]=(long)A;pseudo[2]=(long)I;
  psid=gcopy(pseudo);
  powbasis=cgetg(n+1,17);powbasis[1]=(long)polun[vpol];
  for(i=2;i<=n;i++) powbasis[i]=lmul((GEN)powbasis[i-1],polx[vpol]);
  cpt=0;
  for(ipr=1;ipr<=nbidp;ipr++)
  {
    if(!gcmp1((GEN)ep[ipr]))
    {
      y=rnfordmax(nf,pol,(GEN)list[ipr],unnf,zeronf,id,psid,powbasis);
      if(cpt) pseudo=rnfjoinmodules(nf,pseudo,y);
      else {cpt++;pseudo=y;}
    }
  }
  W=gmodulcp(gmul(powbasis,basistoalg(nf,(GEN)pseudo[1])),pol);
  I=(GEN)pseudo[2];
  p2=cgetg(n+1,19);for(j=1;j<=n;j++) p2[j]=lgetg(n+1,18);
  for(j=1;j<=n;j++) for(i=j;i<=n;i++)
  {
    coeff(p2,i,j)=(long)trace(gmul((GEN)W[i],(GEN)W[j]));
    if(i!=j) coeff(p2,j,i)=coeff(p2,i,j);
  }
  d=algtobasis(nf,det(p2));
  for(i=1;(i<=n)&&vecegal((GEN)I[i],id);i++);
  if(i>n) D=id;
  else
  {
    D=(GEN)I[i];
    for(i++;i<=n;i++)
      if(!vecegal((GEN)I[i],id)) D=idealmul(nf,D,(GEN)I[i]);
    D=idealmul(nf,D,D);
  }
  p2=gtomat(d);
  p3=auxdecomp(content(d),0);
  p4=gun;
  for(i=1;i<lg((GEN)p3[1]);i++)
    p4=gmul(p4,gpuigs(gcoeff(p3,i,1),(itos(gcoeff(p3,i,2)))>>1));
  p4=gsqr(p4);
  tetpil=avma;
  if(all)
  {
    p1=cgetg(5,17);p1[1]=lcopy((GEN)pseudo[1]);
    p1[2]=lcopy(I);p1[3]=(long)idealmul(nf,D,p2);p1[4]=(long)gdiv(d,p4);
  }
  else
  {
    p1=cgetg(3,17);p1[1]=(long)idealmul(nf,D,p2);p1[2]=(long)gdiv(d,p4);
  }
  tetpil=avma;return gerepile(av,tetpil,p1);
}

GEN
rnfpseudobasis(nf, pol)
GEN nf;
GEN pol;
{
  return rnfround2all(nf,pol,1);
}

GEN
rnfdiscf(nf, pol)
GEN nf;
GEN pol;
{
  return rnfround2all(nf,pol,0);
}
#endif

GEN
nfreducemodpr(nf, x, prhall)
GEN nf;
GEN x;
GEN prhall;
{
 /* a usage interne, pas de gestion de pile */
  
  long N=lg(x)-1,i,flx,v;
  GEN p,prh,den;

  flx=1;for(i=1;(i<=N)&&flx;i++) flx=(typ((GEN)x[i])!=3);
  if(!flx) x=lift(x);prh=(GEN)prhall[1];
  p=gcoeff(prh,1,1);
  if(gcmp1(p)) err(talker,"bug in reducemodpr");
  den=denom(x);
  if(!gcmp1(den))
  {
    v=ggval(den,p);
    if(v) x=element_mul(nf,x,element_pow(nf,(GEN)prhall[2],stoi(v)));
  }
  x=gmod(x,p);
  for(i=N;i>=1;i--)
    if(gcmp1(gcoeff(prh,i,i))) x=gsub(x,gmul((GEN)x[i],(GEN)prh[i]));
  return gmul(gmodulcp(gun,p),x);
}

#if 0
GEN
rnfelement_mulmod(nf, multab, zeronf, unnf, x, y, prhall)
GEN nf;
GEN multab;
GEN zeronf;
GEN unnf;
GEN x;
GEN y;
GEN prhall;
{
  /* a usage interne, pas de gestion de pile : x et y sont des vecteurs dont
     les coefficients sont les composantes sur nf[7] ; avec reduction mod pr sauf
     si prhall=gzero */

  long i,j,k,n;
  GEN p1,p2,z,s;

  n=lg(x)-1;x=lift(x);y=lift(y);z=cgetg(n+1,18);
  for(k=1;k<=n;k++)
  {
    s=zeronf;
    for(i=1;i<=n;i++)
    {
      for(j=1;j<=n;j++)
      {
	p2=gcoeff(multab,k,(i-1)*n+j);
	if(!gcmp0(p2))
	{
	  p1=element_mul(nf,(GEN)x[i],(GEN)y[j]);
	  if(vecegal(p2,unnf)) s=gadd(s,p1);
	  else s=gadd(s,element_mul(nf,p1,p2));
	}
      }
    }
    if(!gcmp0(prhall)) z[k]=(long)nfreducemodpr(nf,s,prhall);
    else z[k]=(long)s;
  }
  return z;
}

GEN
rnfelement_sqrmod(nf, multab, zeronf, unnf, x, prhall)
GEN nf;
GEN multab;
GEN zeronf;
GEN unnf;
GEN x;
GEN prhall;
  /* a usage interne, pas de gestion de pile : x est un vecteur dont
     les coefficients sont les composantes sur nf[7] */
              
{
  long i,j,k,n;
  GEN p1,p2,z,s;

  n=lg(x)-1;x=lift(x);
  z=cgetg(n+1,18);
  for(k=1;k<=n;k++)
  {
    s=zeronf;
    for(i=1;i<=n;i++)
    {
      if(!gcmp0(p2=gcoeff(multab,k,(i-1)*n+i)))
      {
	p1=element_sqr(nf,(GEN)x[i]);
	if(vecegal(p2,unnf)) s=gadd(s,p1);
	else s=gadd(s,element_mul(nf,p1,p2));
      }
    }
    for(i=1;i<=n;i++) for(j=i+1;j<=n;j++)
    {
      if(!gcmp0(p2=gcoeff(multab,k,(i-1)*n+j)))
      {
	p1=gmul2n(element_mul(nf,(GEN)x[i],(GEN)x[j]),1);
	if(vecegal(p2,unnf)) s=gadd(s,p1);
	else s=gadd(s,element_mul(nf,p1,p2));
      }
    }
    if(!gcmp0(prhall)) z[k]=(long)nfreducemodpr(nf,s,prhall);
    else z[k]=(long)s;
  }
  return z;
}

GEN
rnfelement_powmod(nf, multab, zeronf, unnf, x, k, prhall)
GEN nf;
GEN multab;
GEN zeronf;
GEN unnf;
GEN x;
GEN k;
GEN prhall;
                
/* Calcule x^k mod pr dans l'extension . */
 

{
  long i,f,n,av=avma,tetpil;
  GEN k1,y,z;

  n=lg(x)-1;k1=k;z=x;f=1;y=cgetg(n+1,18);
  for(i=2;i<=n;i++) y[i]=(long)zeronf;y[1]=(long)unnf;
  while(f)
  {
    if(mpodd(k1)) y=rnfelement_mulmod(nf,multab,zeronf,unnf,z,y,prhall);
    k1=shifti(k1,-1);f=signe(k1);
    if(f) z=rnfelement_sqrmod(nf,multab,zeronf,unnf,z,prhall);
  }
  tetpil=avma;return gerepile(av,tetpil,gcopy(y));
}

GEN
rnfordmax(nf, pol, pr, unnf, zeronf, id, psid, powbasis)
GEN nf;
GEN pol;
GEN pr;
GEN unnf;
GEN zeronf;
GEN id;
GEN psid;
GEN powbasis;
{
  long av=avma,tetpil,av1,av2,lim,dec,i,j,k,n,N,notfinished=1,v1,v2,vpol,m,fl;
  GEN polnf,p,q,q1,prh,prhall,A,Aa,Aaa,A1,den,I,R,p1,p2,p3,multab,Aainv,alphalist;
  GEN pip,baseIp,baseOp,alpha,matprod,alphainv,matC,matG,matV,vecpro,matH;
  GEN neworder,matId,H,Hid,alphalistinv,epr,betae;
  
  long cmpt;

  polnf=(GEN)nf[1];N=lgef(polnf)-3;n=lgef(pol)-3;vpol=varn(pol);
  p=(GEN)pr[1];q=gpui(p,(GEN)pr[4],0);pip=(GEN)pr[2];
  q1=q;while(cmpis(q1,n)<0) q1=mulii(q1,q);
  prh=idealmulprime(nf,id,pr);
  epr=(GEN)pr[3];betae=gdiv(element_pow(nf,(GEN)pr[5],epr),gpui(p,addis(epr,-1),0));
  p1=cgetg(2,19);p1[1]=(long)betae;
  p1=idealadd(nf,gmul(p,id),idealmul(nf,p1,id));
  prhall=cgetg(3,17);prhall[1]=(long)prh;
  prhall[2]=idealaddone(nf,pr,p1)[2];
/* ceci contient un alpha congru a 1 mod pr et a 0 mod q^{e_q} pour tous
   les autres ideaux premiers q au dessus de p */
  A=(GEN)psid[1];I=(GEN)psid[2];matId=(GEN)psid[1];

  cmpt=0;
#ifdef DEBUG_MODE
  if(DEBUGLEVEL>1)
  {fprintferr("\n Ideal traite : ");outerr(pr);flusherr();}
#endif

  lim=(bot+avma)>>1;av1=avma;
  while(notfinished)
  {
    if(cmpt&&(avma<lim))
    {
      tetpil=avma;A=gcopy(A);I=gcopy(I);av2=avma;
      dec=lpile(av1,tetpil,0)>>TWOPOTBYTES_IN_LONG;
      if(adecaler(A,tetpil,av2)) A+=dec;
      if(adecaler(I,tetpil,av2)) I+=dec;
    }
    cmpt++;
#ifdef DEBUG_MODE
    if(DEBUGLEVEL>1)
    {fprintferr("\n\n     %ld eme passe \n",cmpt);flusherr();}
#endif
    
    alphalist=cgetg(n+1,17);alphalistinv=cgetg(n+1,17);
    for(i=1;i<=n;i++)
    {
      if(vecegal((GEN)I[i],id)) alphalist[i]=alphalistinv[i]=(long)unnf;
      else
      {
	den=denom((GEN)I[i]);p1=gcmp1(den)? (GEN)I[i]:gmul(den,(GEN)I[i]);
	p1=gdiv(ideal_two_elt(nf,p1),den);
	v1=(gcmp0((GEN)p1[1]))?EXP220:element_val(nf,p2=gmul((GEN)p1[1],unnf),pr);
	v2=(gcmp0((GEN)p1[2]))?EXP220:element_val(nf,(GEN)p1[2],pr);
	if(v1>v2) p2=(GEN)p1[2];
	alphalist[i]=(long)p2;alphalistinv[i]=(long)element_inv(nf,p2);
      }
    }
    A1=cgetg(n+1,19);
    for(j=1;j<=n;j++)
    {
      p1=cgetg(n+1,18);A1[j]=(long)p1;
      for(i=1;i<=n;i++) p1[i]=(long)element_mul(nf,gcoeff(A,i,j),(GEN)alphalist[j]);
    }
    Aa=basistoalg(nf,A1);Aainv=ginv(Aa);
    Aaa=gmodulcp(gmul(powbasis,Aa),pol);
    multab=cgetg(n*n+1,19);for(j=1;j<=n*n;j++) multab[j]=lgetg(n+1,18);
    for(i=1;i<=n;i++) for(j=i;j<=n;j++)
    {
      p1=gmul((GEN)Aaa[i],(GEN)Aaa[j]);p2=cgetg(n+1,18);
      if((typ(p1)==9)&&(varn((GEN)p1[1])==vpol)) p1=(GEN)p1[2];
      if((gcmp0(p1))||(typ(p1)<=9)||((typ(p1)==10)&&(varn(p1)>vpol)))
      {p2[1]=(long)p1;for(k=2;k<=n;k++) p2[k]=(long)gmodulcp(gzero,polnf);}
      else for(k=1;k<=n;k++) p2[k]=(long)truecoeff(p1,k-1);
      p3=algtobasis(nf,gmul(Aainv,p2));
      for(k=1;k<=n;k++)
      {coeff(multab,k,(i-1)*n+j)=(long)p3[k];coeff(multab,k,(j-1)*n+i)=(long)p3[k];}
    }
    R=cgetg(n+1,19);
    for(j=1;j<=n;j++)
      R[j]=(long)rnfelement_powmod(nf,multab,zeronf,unnf,(GEN)matId[j],q1,prhall);
    baseIp=nfker(nf,R,prhall);
    baseOp=nfsuppl(nf,baseIp,n,prhall);
    alpha=cgetg(n+1,19);
    for(j=1;j<lg(baseIp);j++) alpha[j]=(long)lift((GEN)baseOp[j]);
    for(;j<=n;j++)
    {
      p1=cgetg(n+1,18);alpha[j]=(long)p1;
      for(i=1;i<=n;i++) p1[i]=(long)element_mul(nf,pip,lift(gcoeff(baseOp,i,j)));
    }
    matprod=cgetg(n+1,19);
    for(j=1;j<=n;j++)
    {
      p1=cgetg(n+1,18);matprod[j]=(long)p1;
      for(i=1;i<=n;i++)
	p1[i]=(long)rnfelement_mulmod(nf,multab,zeronf,unnf,(GEN)matId[j],(GEN)alpha[i],gzero);
    }
    p1=basistoalg(nf,alpha);alphainv=ginv(p1);
    matC=cgetg(n+1,19);
    for(j=1;j<=n;j++)
    {
      p1=cgetg(n*n+1,18);matC[j]=(long)p1;
      for(i=1;i<=n;i++)
      {
	p2=gmul(alphainv,basistoalg(nf,gcoeff(matprod,i,j)));
	for(k=1;k<=n;k++)
	  p1[(i-1)*n+k]=(long)nfreducemodpr(nf,algtobasis(nf,(GEN)p2[k]),prhall);
      }
    }
    matG=nfker(nf,matC,prhall);m=lg(matG)-1;
    matV=cgetg(n+m+1,19);
    for(j=1;j<=m;j++) matV[j]=(long)lift((GEN)matG[j]);
    for(j=1;j<=n;j++)
    {
      p1=cgetg(n+1,18);matV[j+m]=(long)p1;
      for(k=1;k<=n;k++) matV[j+m]=(long)matId[j];
    }
    vecpro=cgetg(3,17);vecpro[1]=(long)matV;
    p1=cgetg(n+m+1,17);vecpro[2]=(long)p1;
    for(i=1;i<=m;i++) p1[i]=(long)idealinv(nf,prh);
    for(i=m+1;i<=n+m;i++)
      p1[i]=(long)idealmul(nf,(GEN)I[i-m],(GEN)alphalistinv[i-m]);
    matH=nfhermite(nf,vecpro);
    p1=algtobasis(nf,gmul(basistoalg(nf,A1),basistoalg(nf,(GEN)matH[1])));
    p2=(GEN)matH[2];
    H=cgetg(n+1,19);
    for(j=1;j<=n;j++)
    {
      p3=cgetg(n+1,18);H[j]=(long)p3;
      for(i=1;i<=n;i++) p3[i]=(long)element_mul(nf,gcoeff(p1,i,j),(GEN)alphalistinv[j]);
    }
    Hid=cgetg(n+1,17);
    for(j=1;j<=n;j++) Hid[j]=(long)idealmul(nf,(GEN)p2[j],(GEN)alphalist[j]);
#ifdef DEBUG_MODE
    if(DEBUGLEVEL>1)
    {
      fprintferr(" Nouvel ordre :\n");outerr((GEN)H);
      outerr((GEN)Hid);flusherr();
    }
#endif
    fl=i=1;while(fl&&(i<=n)){if(!vecegal((GEN)I[i],(GEN)Hid[i])) fl=0;i++;}
    if(fl) notfinished=0;
    A=H;I=Hid;
  }
  neworder=cgetg(3,17);neworder[1]=(long)A;neworder[2]=(long)I;
  tetpil=avma;return gerepile(av,tetpil,gcopy(neworder));
}
      
GEN
rnfjoinmodules(nf, x, y)
GEN nf;
GEN x;
GEN y;
{
/* given MODULES x and y by their pseudo-bases in HNF, gives a
   pseudo-basis of the module generated by x and y. A usage interne,
   pas de verifications, mais gestion de pile. */

  long av=avma,tetpil,j,lx,ly;
  GEN p1,p2,z,Hx,Hy,Ix,Iy;

  Hx=(GEN)x[1];Ix=(GEN)x[2];Hy=(GEN)y[1];Iy=(GEN)y[2];
  lx=lg(Hx);ly=lg(Hy);
  z=cgetg(3,17);
  p1=cgetg(lx+ly-1,19);z[1]=(long)p1;
  p2=cgetg(lx+ly-1,17);z[2]=(long)p2;
  for(j=1;j<lx;j++) {p1[j]=Hx[j];p2[j]=Ix[j];}
  for(;j<lx+ly-1;j++) {p1[j]=Hy[j-lx+1];p2[j]=Iy[j-lx+1];}
  tetpil=avma;return gerepile(av,tetpil,nfhermite(nf,z));
}

GEN
rnfsimplifybasis(bnf, order)
GEN bnf;
GEN order;
/* given bnf as output by buchinit and a pseudo-basis of an order
   in HNF [A,I] (or [A,I,D,d] it does not matter), tries to simplify the
   HNF as much as possible. The resulting matrix will be upper triangular
   but the diagonal coefficients will not be equal to 1. The ideals
   are guaranteed to be integral and primitive. */
{
  long av=avma,tetpil,j,N,n;
  GEN p1,id,Az,Iz,nf,A,I;

  checkbnf(bnf);
  if((typ(order)!=17)||(lg(order)<3))
    err(talker,"not a pseudo-basis in nfsimplifybasis");
  A=(GEN)order[1];I=(GEN)order[2];n=lg(A)-1;nf=(GEN)bnf[7];
  N=lgef((GEN)nf[1])-3;id=idmat(N);Iz=cgetg(n+1,17);Az=cgetg(n+1,19);
  for(j=1;j<=n;j++)
  {
    if(vecegal((GEN)I[j],id)) {Iz[j]=(long)id;Az[j]=A[j];}
    else
    {
      p1=content((GEN)I[j]);
      if(!gcmp1(p1))
      {
	Iz[j]=(long)gdiv((GEN)I[j],p1);Az[j]=lmul((GEN)A[j],p1);
      }
      else Az[j]=A[j];
      if(!vecegal((GEN)Iz[j],id))
      {
	p1=isprincipalgen(bnf,(GEN)Iz[j]);
	if(gcmp0((GEN)p1[1]))
	{
	  p1=(GEN)p1[2];Iz[j]=(long)id;
	  Az[j]=(long)element_mulvec(nf,p1,(GEN)Az[j]);
	}
      }
    }
  }
  tetpil=avma;p1=cgetg(lg(order),17);p1[1]=lcopy(Az);p1[2]=lcopy(Iz);
  for(j=3;j<lg(order);j++) p1[j]=lcopy((GEN)order[j]);
  return gerepile(av,tetpil,p1);
}

GEN
rnfsteinitz(nf, order)
GEN nf;
GEN order;
/* given a pseudo-basis of an order in HNF [A,I] (or [A,I,D,d] it does
   not matter), gives an nxn matrix (not in HNF) of a pseudo-basis and
   an ideal vector [id,id,...,id,I] such that
   order=nf[7]^(n-1)xI. Since it uses the approximation theorem,
   can be long. */
{
  long av=avma,tetpil,N,j,n;
  GEN id,A,I,p1,p2,a,b;

  nf=checknf(nf);
  N=lgef((GEN)nf[1])-3;id=idmat(N);
  if(typ(order)==10) order=rnfpseudobasis(nf,order);
  if((typ(order)!=17)||(lg(order)<3))
    err(talker,"not a pseudo-matrix in rnfsteinitz");
  A=gcopy((GEN)order[1]);I=gcopy((GEN)order[2]);n=lg(A)-1;
  for(j=1;j<=n-1;j++)
  {
    a=(GEN)I[j];
    if(!vecegal(a,id))
    {
      b=(GEN)I[j+1];
      if(vecegal(b,id))
      {
	p1=(GEN)A[j];A[j]=A[j+1];A[j+1]=lneg(p1);
	I[j]=(long)b;I[j+1]=(long)a;
      }
      else
      {
	p2=nfidealdet1(nf,a,b);
	p1=gadd(element_mulvec(nf,(GEN)p2[1],(GEN)A[j]),element_mulvec(nf,(GEN)p2[2],(GEN)A[j+1]));
	A[j+1]=(long)gadd(element_mulvec(nf,(GEN)p2[3],(GEN)A[j]),element_mulvec(nf,(GEN)p2[4],(GEN)A[j+1]));
	A[j]=(long)p1;
	I[j]=(long)id;I[j+1]=(long)idealmul(nf,a,b);
	p1=content((GEN)I[j+1]);
	if(!gcmp1(p1))
	{I[j+1]=(long)gdiv((GEN)I[j+1],p1);A[j+1]=lmul(p1,(GEN)A[j+1]);}
      }
    }
  }
  tetpil=avma;p1=cgetg(lg(order),17);
  p1[1]=lcopy(A);p1[2]=lcopy(I);
  for(j=3;j<lg(order);j++) p1[j]=lcopy((GEN)order[j]);
  return gerepile(av,tetpil,p1);
}

GEN
rnfbasis(bnf, order)
GEN bnf;
GEN order;
/* Given bnf as output by buchinit and either an order as output by
   rnfpseudobasis or a polynomial, and outputs a basis if it is free,
   an n+1-generating set if it is not */
{
  long av=avma,tetpil,j,N,n;
  GEN nf,A,I,classe,p1,p2,id;

  checkbnf(bnf);
  nf=(GEN)bnf[7];N=lgef((GEN)nf[1])-3;id=idmat(N);
  if(typ(order)==10) order=rnfpseudobasis(nf,order);
  if((typ(order)!=17)||(lg(order)<3))
    err(talker,"not a pseudo-matrix in rnfbasis");
  A=(GEN)order[1];I=(GEN)order[2];n=lg(A)-1;
  for(j=1;(j<=(n-1))&&vecegal((GEN)I[j],id);j++);
  if(j<n) order=rnfsteinitz(nf,order);
  A=(GEN)order[1];I=(GEN)order[2];classe=(GEN)I[n];
  p1=isprincipalgen(bnf,classe);
  if(gcmp0((GEN)p1[1]))
  {
    tetpil=avma;p2=cgetg(n+1,19);
    for(j=1;j<=n-1;j++) p2[j]=lcopy((GEN)A[j]);
    p2[n]=(long)element_mulvec(nf,(GEN)p1[2],(GEN)A[n]);
  }
  else
  {
    p1=ideal_two_elt(nf,classe);
    tetpil=avma;p2=cgetg(n+2,19);
    for(j=1;j<=n-1;j++) p2[j]=lcopy((GEN)A[j]);
    p2[n]=lmul((GEN)p1[1],(GEN)A[n]);
    p2[n+1]=(long)element_mulvec(nf,(GEN)p1[2],(GEN)A[n]);
  }
  return gerepile(av,tetpil,p2);
}

GEN
rnfhermitebasis(bnf, order)
GEN bnf;
GEN order;
/* Given bnf as output by buchinit and either an order as output by
   rnfpseudobasis or a polynomial, and outputs a basis (not pseudo)
   in Hermite Normal Form if it exists, zero if not */
{
  long av=avma,tetpil,j,N,n;
  GEN nf,A,I,p1,id;

  checkbnf(bnf);
  nf=(GEN)bnf[7];N=lgef((GEN)nf[1])-3;id=idmat(N);
  if(typ(order)==10)
  {
    order=rnfpseudobasis(nf,order);
    A=(GEN)order[1];
  }
  else
  {
    if((typ(order)!=17)||(lg(order)<3))
      err(talker,"not a pseudo-matrix in rnfbasis");
    A=gcopy((GEN)order[1]);
  }
  I=(GEN)order[2];n=lg(A)-1;
  for(j=1;j<=n;j++)
  {
    if(!vecegal((GEN)I[j],id))
    {
      p1=isprincipalgen(bnf,(GEN)I[j]);
      if(gcmp0((GEN)p1[1]))
	A[j]=(long)element_mulvec(nf,(GEN)p1[2],(GEN)A[j]);
      else {avma=av;return gzero;}
    }
  }
  tetpil=avma;return gerepile(av,tetpil,gcopy(A));
}

long
rnfisfree(bnf, order)
GEN bnf;
GEN order;
{
  long av=avma,n,N,j;
  GEN nf,p1,id,I;
  
  checkbnf(bnf);
  if(gcmp1((GEN)((GEN)((GEN)bnf[8])[1])[1])) return 1;
  nf=(GEN)bnf[7];N=lgef((GEN)nf[1])-3;id=idmat(N);
  if(typ(order)==10) order=rnfpseudobasis(nf,order);
  if((typ(order)!=17)||(lg(order)<3))
    err(talker,"not a pseudo-matrix in rnfisfree");
  I=(GEN)order[2];n=lg(I)-1;
  for(j=1;(j<=n)&&vecegal((GEN)I[j],id);j++);
  if(j>n) {avma=av;return 1;}
  p1=(GEN)I[j];
  for(j++;j<=n;j++) if(!vecegal((GEN)I[j],id)) p1=idealmul(nf,p1,(GEN)I[j]);
  j=gcmp0(isprincipal(bnf,p1));avma=av;
  return j;
}

#endif

