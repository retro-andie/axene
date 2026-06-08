#include "genpari.h"

#ifdef __cplusplus
#ifdef LONG_IS_32BIT
char *pariversion = "               (portable C++ 32-bit version)\n";
#else
char *pariversion = "               (portable C++ 64-bit version)\n";
#endif
#else
#ifdef LONG_IS_32BIT  
char *pariversion = "                 (Portable 32-bit Version)\n";
#else
char *pariversion = "                 (Portable 64-bit Version)\n";
#endif
#endif

#define BITS_IN_HALFULONG (BITS_IN_LONG/2)

#define HIGHWORD(a) ((a) >> BITS_IN_HALFULONG)
/* si le compilateur est bugge, il faut mettre
 ((a >> BITS_IN_HALFULONG) & LOWMASK) */
#define LOWWORD(a) ((a) & LOWMASK)

ulong overflow,hiremainder;

#ifndef __cplusplus

long
addll(x, y)
ulong x;
ulong y;
{
  ulong z;

  z=x+y;overflow=(z<x)?1:0; return (long)z;
}

long
addllx(x, y)
ulong x;
ulong y;
{
  ulong z;

  z=x+y+overflow;overflow=(z<x)||((z==x)&&overflow)?1:0; return (long)z;
}

long
subll(x, y)
ulong x;
ulong y;
{
  ulong z;

  z=x-y;overflow=(z>x)?1:0; return (long)z;
}

long
subllx(x, y)
ulong x;
ulong y;
{
  ulong z;

  z=x-y-overflow;overflow=(z>x)||((z==x)&&overflow)?1:0; return (long)z;
}

long
shiftl(x, y)
ulong x;
ulong y;
{
  hiremainder=x>>(BITS_IN_LONG-y);return (x<<y);
}

long
shiftlr(x, y)
ulong x;
ulong y;
{
  hiremainder=x<<(BITS_IN_LONG-y);return (x>>y);
}

/* Ancienne version
   
long
mulll(x, y)
ulong x;
ulong y;
{
  ulong xlo,xhi,ylo,yhi;
  ulong z;

  xlo=x&MAXHALFULONG;xhi=x>>(BITS_IN_LONG/2);
  ylo=y&MAXHALFULONG;yhi=y>>(BITS_IN_LONG/2);
  z=addll(xlo*yhi,xhi*ylo);
  hiremainder=(overflow)?(xhi*yhi+(MAXHALFULONG+1)+(z>>(BITS_IN_LONG/2)))
    :(xhi*yhi+(z>>(BITS_IN_LONG/2)));
  z=addll(xlo*ylo,(z<<(BITS_IN_LONG/2)));hiremainder+=overflow;
  return z;
}

long
addmul(x, y)
ulong x;
ulong y;
{
  ulong xlo,xhi,ylo,yhi;
  ulong z,z2;

  xlo=x&MAXHALFULONG;xhi=x>>(BITS_IN_LONG/2);
  ylo=y&MAXHALFULONG;yhi=y>>(BITS_IN_LONG/2);
  z=addll(xlo*yhi,xhi*ylo);
  z2=(overflow)?(xhi*yhi+(MAXHALFULONG+1)+(z>>(BITS_IN_LONG/2)))
    :(xhi*yhi+(z>>(BITS_IN_LONG/2)));
  z=addll(xlo*ylo,(z<<(BITS_IN_LONG/2)));z2+=overflow;
  z=addll(z,hiremainder);hiremainder=z2+overflow;
  return z;
}

*/

/* Version Peter Montgomery */

long
mulll(x, y)
ulong x;
ulong y;
{
  ulong xlo,xhi,ylo,yhi;
  ulong xylo,xymid,xyhi;
  ulong xymidhi, xymidlo;
/*
        Assume (for presentation) that BITS_IN_LONG = 32.
        Then 0 <= xhi, xlo, yhi, ylo <= 2^16 - 1.  Hence

-2^31  + 2^16 <= (xhi - 2^15)*(ylo - 2^15) + (xlo - 2^15)*(yhi - 2^15) <= 2^31.

        If xhi*ylo + xlo*yhi = 2^32*overflow + xymid, then

-2^32 + 2^16 <= 2^32*overflow + xymid - 2^15*(xhi + ylo + xlo + yhi) <= 0.

2^16*overflow <= (xhi+xlo+yhi+ylo)/2 - xymid/2^16 <= 2^16*overflow + 2^16-1

        This inequality was derived using exact (rational) arithmetic;
        it remains valid when we truncate the two middle terms.
*/

  xlo = LOWWORD(x); xhi = HIGHWORD(x);
  ylo = LOWWORD(y); yhi = HIGHWORD(y);

  xylo = xlo*ylo; xyhi = xhi*yhi;
  xymid = (xhi + xlo)*(yhi + ylo) - (xyhi + xylo);

  xymidhi = HIGHWORD(xymid);
  xymidlo = xymid << BITS_IN_HALFULONG;

  xylo += xymidlo;
  hiremainder = xyhi + xymidhi + (xylo < xymidlo)
     + (((((xhi + xlo) + (yhi + ylo)) >> 1) - xymidhi) & HIGHMASK);

  return xylo;
}

long
addmul(x, y)
ulong x;
ulong y;
{
  ulong xlo,xhi,ylo,yhi;
  ulong xylo,xymid,xyhi;
  ulong xymidhi, xymidlo;

  xlo = LOWWORD(x); xhi = HIGHWORD(x);
  ylo = LOWWORD(y); yhi = HIGHWORD(y);

  xylo = xlo*ylo; xyhi = xhi*yhi;
  xymid = (xhi + xlo)*(yhi + ylo) - (xyhi + xylo);

  xylo += hiremainder; xyhi += (xylo < hiremainder);

  xymidhi = HIGHWORD(xymid);
  xymidlo = xymid << BITS_IN_HALFULONG;

  xylo += xymidlo;
  hiremainder = xyhi + xymidhi + (xylo < xymidlo)
     + (((((xhi + xlo) + (yhi + ylo)) >> 1) - xymidhi) & HIGHMASK);

  return xylo;
}

#endif

/* Ancienne version 

int
bfffo(x)
ulong x;
{
  int sc;
  static int tabshi[16]={4,3,2,2,1,1,1,1,0,0,0,0,0,0,0,0};

#ifdef LONG_IS_64BIT
  if(x&(0xffffffff00000000)) sc=0; else {sc=32;x<<=32;}
  if(!(x&(0xffff000000000000))) {sc+=16;x<<=16;}
  if(!(x&(0xff00000000000000))) {sc+=8;x<<=8;}
  if(x&(0xf000000000000000)) x>>=60; else {sc+=4;x>>=56;}
  sc+=tabshi[x];return sc;
#else
  if(x&(0xffff0000)) sc=0; else {sc=16;x<<=16;}
  if(!(x&(0xff000000))) {sc+=8;x<<=8;}
  if(x&(0xf0000000)) x>>=28; else {sc+=4;x>>=24;}
  sc+=tabshi[x];return sc;
#endif
}

*/
/* version Peter Montgomery */

int
bfffo(x)
ulong x;
{
  int sc;
  static int tabshi[16]={4,3,2,2,1,1,1,1,0,0,0,0,0,0,0,0};

  sc = BITS_IN_LONG - 4;
#ifdef LONG_IS_64BIT
  if(x & 0xffffffff00000000) {sc -= 32; x >>= 32;}
#endif
  if(x > 0xffffUL) {sc -= 16; x >>= 16;}
  if(x > 0x00ffUL) {sc -= 8; x >>= 8;}
  if(x > 0x000fUL) {sc -= 4; x >>= 4;}
  return sc + tabshi[x];
}

/* Ancienne version
  
long
divll(x, y)
ulong x;
ulong y;
{
#define GLUE(hi, lo) ((hi << (BITS_IN_LONG/2)) + lo)
#define SPLIT(a, b, c) b = HIGHWORD(a); c = LOWWORD(a)

  ulong v1, v2, u3, u4, q1, q2, aux, aux1, aux2;
  int k;
  
  for(k = 0; !(y & HIGHBIT); k++)
  {
    hiremainder <<= 1;
    if (x & HIGHBIT) hiremainder++;
    x <<= 1;
    y <<= 1;
  }
  
  SPLIT(y, v1, v2);
  SPLIT(x, u3, u4);
  
  q1 = hiremainder / v1; if (q1 & HIGHMASK) q1 = LOWMASK;
  hiremainder -= q1 * v1;
  aux = v2 * q1;
  again:
  SPLIT(aux, aux1, aux2);
  if (aux2 > u3) aux1++;
  if (aux1 > hiremainder) {q1--; hiremainder += v1; aux -= v2; goto again;}
  u3 -= aux2;
  hiremainder -= aux1;
  hiremainder <<= (BITS_IN_LONG/2); hiremainder += u3 & LOWMASK;
  
  q2 = hiremainder / v1; if (q2 & HIGHMASK) q2 = LOWMASK;
  hiremainder -= q2 * v1;
  aux = v2 * q2;
  again2:
  SPLIT(aux, aux1, aux2);
  if (aux2 > u4) aux1++;
  if (aux1 > hiremainder) {q2--; hiremainder += v1; aux -= v2; goto again2;}
  u4 -= aux2;
  hiremainder -= aux1;
  hiremainder <<= (BITS_IN_LONG/2); hiremainder += u4 & LOWMASK;
  hiremainder >>= k;
  return GLUE(q1, q2);
}

*/

/* Version Peter Montgomery */

long
divll(x, y)
ulong x;
ulong y;
{
#define GLUE(hi, lo) (((hi) << BITS_IN_HALFULONG) + (lo))
#define SPLIT(a, b, c) b = HIGHWORD(a); c = LOWWORD(a)

  ulong v1, v2, u3, u4, q1, q2, aux, aux1, aux2;
  int k;
  if (hiremainder >= y) err(talker, "Invalid arguments to divll");
  else if (hiremainder == 0)
  {    /* Only one division needed */
    hiremainder = x % y;
    return x/y;
  }
  else if (y <= LOWMASK)
  {    /* Use two half-word divisions */
    hiremainder = GLUE(hiremainder, HIGHWORD(x));
    q1 = hiremainder / y;
    hiremainder = GLUE(hiremainder % y, LOWWORD(x));
    q2 = hiremainder / y;
    hiremainder = hiremainder % y;
    return GLUE(q1, q2);
  }
  if (y & HIGHBIT) k = 0;
  else
  {
    k = bfffo(y);
    hiremainder = (hiremainder << k) + (x >> (BITS_IN_LONG - k));
    x <<= k;y <<= k;
  }
  SPLIT(y, v1, v2);
  SPLIT(x, u3, u4);
  q1 = hiremainder / v1; if (q1 > LOWMASK) q1 = LOWMASK;
  hiremainder -= q1 * v1;
  aux = v2 * q1;
  again:
  SPLIT(aux, aux1, aux2);
  if (aux2 > u3) aux1++;
  if (aux1 > hiremainder)
  {q1--; hiremainder += v1; aux -= v2; goto again;}

  hiremainder = GLUE(hiremainder - aux1, LOWWORD(u3 - aux2));
  q2 = hiremainder / v1; if (q2 > LOWMASK) q2 = LOWMASK;
  hiremainder -= q2 * v1;
  aux = v2 * q2;
  again2:
  SPLIT(aux, aux1, aux2);
  if (aux2 > u4) aux1++;
  if (aux1 > hiremainder)
  {q2--; hiremainder += v1; aux -= v2; goto again2;}
  hiremainder = GLUE(hiremainder - aux1, LOWWORD(u4 - aux2)) >> k;
  return GLUE(q1, q2);
}

#undef BITS_IN_HALFULONG

long
mulmodll(a, b, c)
ulong a;
ulong b;
ulong c;
{
  divll(mulll(a,b),c);return hiremainder;
}

#include <time.h>
#ifndef CLOCKS_PER_SEC
/* This value is probably incorrect on YOUR system */
#define CLOCKS_PER_SEC  1000000
#endif

long
timer()
     
{

/*  static clock_t oldclocks;
  clock_t totalclocks = clock();
  ulong delay = (totalclocks - oldclocks) * 1000. / CLOCKS_PER_SEC;
  oldclocks = totalclocks;
  return delay;
*/
}

long
timer2()
     
{
/*
  static clock_t oldclocks;
  clock_t totalclocks = clock();
  ulong delay = (totalclocks - oldclocks) * 1000. / CLOCKS_PER_SEC;
  oldclocks = totalclocks;
  return delay;
*/
}



