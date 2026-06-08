
static XPoint *bezier_step ___PROTO((register XPoint *out,
				     register long xa, register long ya,
				     register long xb, register long yb,
				     register long xc, register long yc));


/* ----------------------------------------------------------------- ** 
** bezier_step - Recurse to linearize a bezier curve                 ** 
** ----------------------------------------------------------------- */
static XPoint *bezier_step(out, xA, yA, xB, yB, xC, yC)
register XPoint *out;
register long xA,yA;       /* control point 1 */
register long xB,yB;       /* control point 2 */
register long xC,yC;       /* End */
{
  register long dmax, tmp;

  Xc_TRACE(("bezier_step(%ld, %ld, %ld, %ld, %ld, %ld)",
	    xA, yA, xB, yB, xC, yC));

  /*--- Recursion end test ---*/
#if 1
  dmax = ABS(xA), tmp = ABS(yA);
  if(dmax < tmp) dmax = tmp;
  tmp = ABS(xC - xB);
  if(dmax < tmp) dmax = tmp;
  tmp = ABS(yC - yB);
  if(dmax < tmp) dmax = tmp;

  if(dmax <= BEZIER_LIMIT)
#else
  dmax = MAX(xA, MAX(xB, xC)) - MIN(xA, MAX(xB, xC));
  tmp = MAX(yA, MAX(yB, yC)) - MIN(yA, MAX(yB, yC));
  /*if((dmax < BEZIER_SCALE(1.5)) || (tmp < BEZIER_SCALE(1.5)))*/
#endif
    { 
      out->x = (xC + BEZIER_HALF) >> BEZIER_SCALEBITS;
      out->y = (yC + BEZIER_HALF) >> BEZIER_SCALEBITS;
      return(out + 1);
    }
  else
    {
      long x, y, dmax2, tmp2;

      tmp = xA; tmp2 = yA;
      xA += xB; yA += yB;
      xB += xC; yB += yC;

      dmax = tmp + xA; dmax2 = tmp2 + yA;
      xA += xB; yA += yB;

      x = (dmax + xA) >> 3; y = (dmax2 + yA) >> 3;

      out = bezier_step(out,
			tmp >> 1, tmp2 >> 1,
			dmax >> 2, dmax2 >> 2,
			x, y);

      return bezier_step(out,
			 (xA >> 2) - x, (yA >> 2) - y,
			 (xB >> 1) - x, (yB >> 1) - y,
			 xC - x, yC - y);
    }
} 
