#include "fsdither.h"
#include "xcalibur.h"

/* MONO returns total intensity of r,g,b components */
#define MONO(rd,gn,bl) (((rd)*11 + (gn)*16 + (bl)*5) >> 5)  /*.33R+ .5G+ .17B*/

#define MAX_GHANDS	16

typedef unsigned char byte; 

byte           fsgamcr[256]; /* gamma correction curve (for FS dither) */

void GenerateFSGamma();
void InitSpline();
double EvalSpline();
void FloydDitherize1();
byte *FSDither();

/*********************/
void GenerateFSGamma()
{
 /* this function generates the Floyd-Steinberg gamma curve (fsgamcr)
    
    This function generates a 4 point spline curve to be used as a 
    non-linear grey 'colormap'.  Two of the points are nailed down at 0,0
    and 255,255, and can't be changed.  You specify the other two.  If
    you specify points on the line (0,0 - 255,255), you'll get the normal
    linear reponse curve.  If you specify points of 50,0 and 200,255, you'll
    get grey values of 0-50 to map to black (0), and grey values of 200-255
    to map to white (255) (roughly).  Values between 50 and 200 will cover
    the output range 0-255.  The reponse curve will be slightly 's' shaped. */

 int i,j;
 /*  static int x[4] = {0,24,232,255}; */
 static int x[4] = {0, 80, 160, 255};
 static int y[4] = {0, 0, 255, 255};
 double yf[4];

 InitSpline(x, y, 4, yf);
  
 for (i=0; i<256; i++)
 {
  j = (int) EvalSpline(x, y, yf, 4, (double) i);
  if (j < 0) j = 0;
  else if (j > 255) j = 255;
  fsgamcr[i] = j;
 }
}



/*********************/
void InitSpline(x,y,n,y2)
int *x, *y, n;
double *y2;
{
 /* given arrays of data points x[0..n-1] and y[0..n-1], computes the
    values of the second derivative at each of the data points
    y2[0..n-1] for use in the splint function */

 int i,k;
 double p,qn,sig,un,u[MAX_GHANDS];

 y2[0] = u[0] = 0.0;

 for (i=1; i<n-1; i++) 
 {
  sig = ((double) x[i]-x[i-1]) / ((double) x[i+1] - x[i-1]);
  p = sig * y2[i-1] + 2.0;
  y2[i] = (sig-1.0) / p;
  u[i] = (((double) y[i+1]-y[i]) / (x[i+1]-x[i])) - 
   (((double) y[i]-y[i-1]) / (x[i]-x[i-1]));
  u[i] = (6.0 * u[i]/(x[i+1]-x[i-1]) - sig*u[i-1]) / p;
 }
 qn = un = 0.0;

 y2[n-1] = (un-qn*u[n-2]) / (qn*y2[n-2]+1.0);
 for (k=n-2; k>=0; k--)
  y2[k] = y2[k]*y2[k+1]+u[k];
}



/*********************/
double EvalSpline(xa,ya,y2a,n,x)
double y2a[],x;
int n,xa[],ya[];
{
 int klo,khi,k;
 double h,b,a;

 klo = 0;
 khi = n-1;
 while (khi-klo > 1) 
 {
  k = (khi+klo) >> 1;
  if (xa[k] > x) khi = k;
  else klo = k;
 }
 h = xa[khi] - xa[klo];
 if (h==0.0) 
  Xc_FATAL(("bad xvalues in splint"));
 a = (xa[khi]-x)/h;
 b = (x-xa[klo])/h;
 return (a*ya[klo] + b*ya[khi] + ((a*a*a-a)*y2a[klo] +(b*b*b-b)*y2a[khi]) 
	 * (h*h) / 6.0);
}




/************************/
void FloydDitherize1(ximage,pic824,ptype, wide, high, rmap, gmap, bmap)
XImage *ximage;
byte   *pic824, *rmap, *gmap, *bmap;
int     ptype, wide, high;
{
 /* does floyd-steinberg ditherizing algorithm.  
 *
 * takes a wide*high input image, of type 'ptype' (PIC8, PIC24)
 *     (if PIC8, colormap is specified by rmap,gmap,bmap)
 *
 * output is a 1-bit per pixel XYBitmap, packed 8 pixels per byte
 *
 * Note: this algorithm is *only* used when running on a 1-bit display
 */

 register byte   pix8, bit;
 int            *thisline, *nextline;
 int            *thisptr, *nextptr, *tmpptr;
 int             i, j, err, bperpix, bperln, order;
 byte           *pp, *image, w1, b1, w8, b8, rgb[256];


 if (ptype == PIC8) 
 {				/* monoify colormap */
  for (i=0; i<256; i++)
   rgb[i] = MONO(rmap[i], gmap[i], bmap[i]);
 }
 
 image   = (byte *) ximage->data;
 bperln  = ximage->bytes_per_line;
 order   = ximage->bitmap_bit_order;
 bperpix = (ptype == PIC8) ? 1 : 3;


 thisline = (int *)Xc_malloc("thisline",wide * sizeof(int));
 nextline = (int *)Xc_malloc("nextline",wide * sizeof(int));
 if (!thisline || !nextline) 
  Xc_FATAL(("ran out of memory in FloydDitherize1()"));


 /* load up first line of picture */
 pp = pic824;
 if (ptype == PIC24) 
 {
  for (j=0, tmpptr = nextline; j<wide; j++, pp+=3)
   *tmpptr++ = fsgamcr[MONO(pp[0], pp[1], pp[2])];
 }
 else 
 {
  for (j=0, tmpptr = nextline; j<wide; j++, pp++)
   *tmpptr++ = fsgamcr[(unsigned int)rgb[(unsigned int)*pp]];
 }
 
      
 w1 = 0				/*white*/ &0x1;  b1= 1 /*black*/ &0x1;
 w8 = w1<<7;  b8 = b1<<7;	/* b/w bit in high bit */
  

 for (i=0; i<high; i++) 
 {
  /*    if ((i&63) == 0) WaitCursor(); */
  
  /* get next line of image */
  tmpptr = thisline;  thisline = nextline;  nextline = tmpptr;/* swap */
  if (i!=high-1) 
  {
   pp = pic824 + (i+1) * wide * bperpix;
   if (ptype == PIC24) 
   {
    for (j=0, tmpptr = nextline; j<wide; j++, pp+=3)
     *tmpptr++ = fsgamcr[MONO(pp[0], pp[1], pp[2])];
   }
   else 
   {
    for (j=0, tmpptr = nextline; j<wide; j++, pp++)
     *tmpptr++ = fsgamcr[(unsigned int)rgb[(unsigned int)*pp]];
   }
  }
  
  thisptr = thisline;  nextptr = nextline;
  
  pp  = image + i*bperln;
  
  
  if (order==LSBFirst) 
  {
   bit = pix8 = 0;
   for (j=0; j<wide; j++, thisptr++, nextptr++) 
   {
    if (*thisptr<128) 
    {
     err = *thisptr;
     pix8 |= b8; 
    }
    else 
    {
     err = *thisptr-255; 
     pix8 |= w8; 
    }
    
    if (bit==7) 
    { 
     *pp++ = pix8;  
     bit=pix8=0; 
    }
    else 
    { 
     pix8 >>= 1;
     bit++;
    }
    
    if (j<wide-1) 
     thisptr[1] += ((err*7)/16);
    
    if (i<high-1) 
    {
     nextptr[0] += ((err*5)/16);
     if (j>0)
      nextptr[-1] += ((err*3)/16);
     if (j<wide-1)
      nextptr[ 1] += (err/16);
    }
   }
   if (bit) 
    *pp++ = pix8 >> (7-bit);	/* write partial byte at end of line */
  }
  
  else 
  {				/* order==MSBFirst */
   bit = pix8 = 0;
   for (j=0; j<wide; j++, thisptr++, nextptr++) 
   {
    if (*thisptr<128) 
    {
     err = *thisptr;
     pix8 |= b1; 
    }
    else
    { 
     err = *thisptr-255; 
     pix8 |= w1;
    }
    
    if (bit==7) 
    { 
     *pp++ = pix8;  
     bit=pix8=0; 
    }
    else 
    { 
     pix8 <<= 1;
     bit++; 
    }

    if (j<wide-1) 
     thisptr[1] += ((err*7)/16);
    
    if (i<high-1) 
    {
     nextptr[0] += ((err*5)/16);
     if (j>0)  
      nextptr[-1] += ((err*3)/16);
     if (j<wide-1) 
      nextptr[ 1] += (err/16);
    }
   }
   if (bit)
    *pp++ = pix8<<(7-bit);	/* write partial byte at end of line */
  }
 }

 Xc_free(thisline);
 Xc_free(nextline);
}



/************************/
byte *FSDither(inpic, intype, w, h, rmap, gmap, bmap, 
	       bval, wval, bit_order)
byte *inpic, *rmap, *gmap, *bmap;
int   w,h, intype, bval, wval;
boolean bit_order;
{
 /* takes an input pic of size w*h, and type 'intype' (PIC8 or PIC24),
 *                (if PIC8, colormap specified by rmap,gmap,bmap)
 * and does the floyd-steinberg dithering algorithm on it.
 * generates (mallocs) a w*h 1-byte-per-pixel 'outpic', using 'bval'
 * and 'wval' as the 'black' and 'white' pixel values, respectively
 */

 int    i, j, k, err, w1, h1;
 byte  *pp, *outpic, rgb[256];
 int   *thisline, *nextline, *thisptr, *nextptr, *tmpptr;

 GenerateFSGamma();

 outpic = (byte *)Xc_malloc("outpic",((w/8)+1*(w%8!=0)) * h);
 if (!outpic) return outpic;
 memset(outpic,0,((w/8)+1*(w%8!=0))*h);
    

 if (intype == PIC8) 
 {				/* monoify colormap */
  for (i=0; i<256; i++)
   rgb[i] = MONO(rmap[i], gmap[i], bmap[i]);
 }


 thisline = (int *)Xc_malloc("thisline",w * sizeof(int));
 nextline = (int *)Xc_malloc("nextline",w * sizeof(int));
 if (!thisline || !nextline) 
  Xc_FATAL(("ran out of memory in FSDither()"));


 w1 = w-1;  h1 = h-1;

 /* load up first line of picture */
 pp = inpic;
 if (intype == PIC24) 
 {
  for (j=0, tmpptr=nextline; j<w; j++, pp+=3)
   *tmpptr++ = fsgamcr[MONO(pp[0], pp[1], pp[2])];
 }
 else 
 {
  for (j=0, tmpptr=nextline; j<w; j++, pp++)
   *tmpptr++ = fsgamcr[(unsigned int)rgb[(unsigned int)*pp]];
 }
 

 for (i = 0; i < h; i++)
 {
  /*    if ((i&31) == 0) WaitCursor(); */
  
  /* get next line of picture */
  tmpptr = thisline;  thisline = nextline;  nextline = tmpptr;/* swap */
  if (i!=h1) 
  {
   if (intype == PIC24) 
   {
    pp = inpic + (i+1) * w * 3;
    for (j=0, tmpptr=nextline; j<w; j++, pp+=3)
     *tmpptr++ = fsgamcr[MONO(pp[0], pp[1], pp[2])];
   }
   else 
   {
    pp = inpic + (i+1) * w;
    for (j=0, tmpptr = nextline; j<w; j++, pp++)
     *tmpptr++ = fsgamcr[(unsigned int)rgb[(unsigned int)*pp]];
   }
  }
  
  pp  = outpic + ((w/8)+1*(w%8!=0))*i;
  thisptr = thisline;  nextptr = nextline;
  
  for (j=0, k=0; j<w; j++, k++, thisptr++, nextptr++) 
  {
   if (*thisptr<128)
   {
    err = *thisptr;
    if (k&8)
    {
     k&=7;
     pp++;
    }
    if (bit_order)
     *pp|=(1<<k)*wval;
    else
     *pp|=(1<<(7-k))*wval;
   }
   else
   {
    err = *thisptr-255;
    if (k&8)
    {
     k&=7;
     pp++;
    }
    if (bit_order)
     *pp|=(1<<k)*bval;
    else
     *pp|=(1<<(7-k))*bval;
   }

   if (j<w1) thisptr[1] += ((err*7)/16);

   if (i<h1) 
   {
    nextptr[0] += ((err*5)/16);
    if (j>0)  nextptr[-1] += ((err*3)/16);
    if (j<w1) nextptr[ 1] += (err/16);
   }
  }
 }

 Xc_free(thisline);
 Xc_free(nextline);
 return outpic;
}









