/*
** geometry.c for Xclamation, XQuad and XAllWrite in Cadre/
** routines for miscellaneous geometry functions
**
** Copyright (C) 1994-2000 Axene.
** Authors: Stéphane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
** Email: xcalibur@axene.org
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Started on  Mon Jul 17 14:08:22 1995 Emmanuel Paris
** Last update Tue Apr 28 18:13:03 1998 Emmanuel Paris
*/

#include "Cadre.h"

l_Point	*projection_on_line();
int	distance_line_point();

/* projection sur (BD) de M parallalelement à (AB) */
l_Point	*projection_on_line(A, B, pointD, M)
l_Point *A;
l_Point *B;
l_Point *pointD;
l_Point *M;
{
 int	xd, yd;
 int	xm, ym;
 int	dx1, dy1, dx2, dy2;
 float	r1, r2;
 float retx;
 l_Point	*ret_pnt;
  
 xd = pointD->X;
 yd = pointD->Y;
 xm = M->X;
 ym = M->Y;
 dx1 = B->X - xd;
 dy1 = B->Y - yd;
 dx2 = B->X - A->X;
 dy2 = B->Y - A->Y;
  
 ret_pnt = (l_Point *)Xc_malloc("project", sizeof(l_Point));
 if ((dx1 == 0 && (dx2 == 0 || dy1 == 0)) || 
     (dx2 == 0 && (dx1 == 0 || dy2 == 0)))
 {
  ret_pnt->X = B->X;
  ret_pnt->Y = B->Y;
  return ret_pnt;
 }
 if (dx1 == 0)
 {
  ret_pnt->X = xd;
  ret_pnt->Y = (dx2 * ym + dy2 * (xd - xm)) / dx2;
  return ret_pnt;
 }
 if (dx2 == 0)
 {
  ret_pnt->X = xm;
  ret_pnt->Y = (dx1 * yd + dy1 * (xm - xd)) / dx1;
  return ret_pnt;
 }
 r1 = (float)dy1 / dx1;
 r2 = (float)dy2 / dx2;
 if (r1 == r2)
 {
  ret_pnt->X = B->X;
  ret_pnt->Y = B->Y;
  return ret_pnt;
 }
 retx = (r1 * xd - r2 * xm - yd + ym) / (r1 - r2);
 ret_pnt->Y = (int)(r1 * retx - r1 * xd + yd);
 ret_pnt->X = retx;
 return ret_pnt;
}

/*retourne le carre' de la  distance entre le point M et la droite (AB)*/
int distance_line_point(A, B, M)
l_Point	*A;
l_Point	*B;
l_Point	*M;
{
 int	xab;
 int	yab;
 int	xam;
 int	yam;
 float	detabam;
 int	normeab;
 int	distabm;
 l_Point	pointD;
 l_Point	*H;
  
 xab = B->X - A->X;
 yab = B->Y - A->Y;
 xam = M->X - A->X;
 yam = M->Y - A->Y;
  
 normeab = xab * xab + yab * yab;
 if (normeab == 0)
  return -1;
 detabam = (float)(xab * yam - yab * xam);
 detabam *= detabam;
 distabm = (int)(detabam / normeab);
  
 if (distabm)
 {
  pointD.X = A->X - yab;
  pointD.Y = A->Y + xab;
  H = projection_on_line(&pointD, A, B, M);
 }
 else
 {
  H = (l_Point *)Xc_malloc("project", sizeof(l_Point));
  memcpy(H, M, sizeof(l_Point));
 }
  
 if (ABS(xab)>ABS(yab))
 {
  if (SGN(H->X - A->X) == SGN(B->X - H->X))
  {
   Xc_free(H);
   return distabm;
  }
 }
 else
 {
  if (SGN(H->Y - A->Y) == SGN(B->Y - H->Y))
  {
   Xc_free(H);
   return distabm;
  }
 }
 Xc_free(H);
 return -1;
}


