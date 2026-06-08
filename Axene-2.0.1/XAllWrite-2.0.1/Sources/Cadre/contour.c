/*
** contour.c for Xclamation, XQuad and XAllWrite in Cadre/
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
** Last update Mon Sep  7 14:56:35 1998 Emmanuel Paris
*/

#include "../Global/xcalibur.h"
#include "Cadre.h"
#include "contour.h"

ct_pnt		*CalculIntersSegment();
l_ct_seg	*CalculCadreSegment();
void		FreeCadreSegment();
l_ct_seg	*CalculResultingSegment();
l_Point		*SubstractCadre();
l_Point		*AdditionCadre();
boolean		IsInCadre();

ct_pnt *CalculIntersSegment(segment1, segment2)
ct_seg	*segment1;
ct_seg	*segment2;
{
 ct_pnt		*xy;
 coord_t	x, y;
 coord_t	xa, ya, xb, yb;
 coord_t	xc, yc, xd, yd;
 double		dx1, dy1, dx2, dy2;
 double		det, vect1, vect2, ry;
 double		xtmp;
 double		limit;
  
 xa = segment1->point1->x;
 ya = segment1->point1->y;
 xb = segment1->point2->x;
 yb = segment1->point2->y;
 xc = segment2->point1->x;
 yc = segment2->point1->y;
 xd = segment2->point2->x;
 yd = segment2->point2->y;
 dx1 = xb-xa;
 dy1 = yb-ya;
 dx2 = xd-xc;
 dy2 = yd-yc;
 det = dx1*dy2-dy1*dx2;
 if (!det)
  return NULL;
 vect1=((double)xb*(double)ya-(double)xa*(double)yb);
 vect2=((double)xd*(double)yc-(double)xc*(double)yd);
 xtmp=(vect1*dx2-vect2*dx1)/det;
 limit=(double)((1L<<((sizeof(coord_t)<<3)-2))-1);
 if (dx1)
  ry=((vect1+xtmp*dy1)/dx1);
 else
  ry=((vect2+xtmp*dy2)/dx2);
 if (ry>limit || ry<-limit)
  return NULL;
 if (xtmp>limit || xtmp<-limit)
  return NULL;
 y=(coord_t)ry;  
 x=(coord_t)xtmp;
  
 if (ABS(dx1)>ABS(dy1))
 {
  if (SGN(x-xa)!=SGN(xb-x) || x==xa || x==xb)
   return NULL;
 }
 else
 {
  if (SGN(y-ya)!=SGN(yb-y) || y==ya || y==yb)
   return NULL;
 }
  
 if (ABS(dx2)>ABS(dy2))
 {
  if (SGN(x-xc)!=SGN(xd-x) || x==xc || x==xd)
   return NULL;
 }
 else
 {
  if (SGN(y-yc)!=SGN(yd-y) || y==yc || y==yd)
   return NULL;
 }
  
 Xc_TRACE(("xa:%d ya:%d xb:%d yb:%d xc:%d yc:%d xd:%d yd:%d xn:%d yn:%d",xa, ya, xb, yb, xc, yc, xd, yd, x,y));
  
 xy=(ct_pnt *)Xc_malloc("ct_pnt inters",sizeof(ct_pnt));
 xy->x = x;
 xy->y = y;
 xy->init = FALSE;
 return xy;
}

l_ct_seg *CalculCadreSegment(listpoint, witch_cadre, dx, dy, nb_seg)
l_Point	*listpoint;
boolean	witch_cadre;
coord_t	dx;
coord_t	dy;
int	*nb_seg;
{
 l_Point	*lpoint;
 l_Point	*plpoint;
 l_Point	*svglpoint;
 l_ct_seg	*lsegment = NULL;
 l_ct_seg	*svglsegment = NULL;
 ct_seg		*segment;
 boolean	prems;
 boolean	cont;
 ct_pnt		*svgpnt = NULL;
  
 prems = TRUE;
 cont = FALSE;
 svglpoint = plpoint = lpoint = listpoint;
 *nb_seg = 0;
 while(1)
 {
  if (!lpoint || !lpoint->cont)
  {
   if (svglpoint != plpoint)	/* elimine isolated point */
   {
	
    segment = (ct_seg *)Xc_malloc("ct_seg",sizeof(ct_seg));
    segment->point1 = lsegment->segment->point2;
    segment->point2 = svgpnt;
    cont = FALSE;
    segment->cadre1 = witch_cadre;
    if (prems)
    {
     svglsegment = lsegment =
      (l_ct_seg *)Xc_malloc("l_ct_seg", sizeof(l_ct_seg));
     lsegment->PrevSegment = NULL;
     prems = FALSE;
    }
    else
    {
     lsegment->NextSegment = (l_ct_seg *)Xc_malloc("l_ct_seg",
						   sizeof(l_ct_seg));
     lsegment->NextSegment->PrevSegment = lsegment;
     lsegment = lsegment->NextSegment;
    }
    lsegment->segment = segment;
    *nb_seg = *nb_seg+1;
   }
   if (!lpoint)
    break;
   svglpoint = lpoint;
  }
  else
  {
   if (!(!plpoint->cont && (!lpoint->NextPoint ||
			    !lpoint->NextPoint->cont)))
    /* elimine isolated line */
   {
    segment = (ct_seg *)Xc_malloc("ct_seg",sizeof(ct_seg));
    segment->point2 = (ct_pnt *)Xc_malloc("ct_pnt",sizeof(ct_pnt));
    segment->point2->x = lpoint->sX+dx;
    segment->point2->y = lpoint->sY+dy;
    segment->point2->init = TRUE;
    if (!cont)
    {
     segment->point1 = svgpnt = 
      (ct_pnt *)Xc_malloc("ct_pnt",sizeof(ct_pnt));
     segment->point1->x = plpoint->sX+dx;
     segment->point1->y = plpoint->sY+dy;
     segment->point1->init = TRUE;
     cont = TRUE;
    }
    else
    {
     segment->point1 = lsegment->segment->point2;
    }
    segment->cadre1 = witch_cadre;
    if (prems)
    {
     svglsegment = lsegment=(l_ct_seg *)Xc_malloc("l_ct_seg",
						  sizeof(l_ct_seg));
     lsegment->PrevSegment = NULL;
     prems = FALSE;
    }
    else
    {
     lsegment->NextSegment=(l_ct_seg *)Xc_malloc("l_ct_seg",
						 sizeof(l_ct_seg));
     lsegment->NextSegment->PrevSegment = lsegment;
     lsegment = lsegment->NextSegment;
    }
    lsegment->segment = segment;
    *nb_seg= *nb_seg+1;
   }
  }
  plpoint = lpoint;
  lpoint = lpoint->NextPoint;
 }
 if (!prems)
  lsegment->NextSegment = NULL;
 else
  return NULL;
 return svglsegment;
}

void FreeCadreSegment(lsegment)
l_ct_seg	*lsegment;
{
 l_ct_seg	*lseg;
 while(lsegment)
 {
  if (lsegment->segment->point1->init)
   Xc_free(lsegment->segment->point1);
  else
   lsegment->segment->point1->init = TRUE;
  Xc_free(lsegment->segment);
  lseg = lsegment->NextSegment;
  Xc_free(lsegment);
  lsegment = lseg;
 }
}

boolean	IsInCadre(lsegment, nb_seg, x,y)
l_ct_seg	*lsegment;
int	nb_seg;
coord_t	x;
coord_t	y;
{
 coord_t	*abscisse;
 coord_t	xa, ya, xb, yb;
 ct_pnt		*point1;
 ct_pnt		*point2;
 int		nb_abscisse;
 int		i;
  
 nb_abscisse = 0;
 abscisse=(coord_t *)Xc_malloc("abscisse",sizeof(coord_t)*nb_seg);
 for(i = 0;i<nb_seg;i++)
 {
  point1 = lsegment->segment->point1;
  point2 = lsegment->segment->point2;
  xa = point1->x;       
  ya = point1->y;
  xb = point2->x;
  yb = point2->y;
  if (y >= MIN(ya, yb) && y < MAX(ya, yb))
  { 
   abscisse[nb_abscisse++] = 
    (coord_t)(((double)(yb - y) * (xa - xb)) / (yb - ya)) + xb;
  }
  else
  {
   if (y == MAX(ya, yb))
   {
    if (ya == yb && x >= MIN(xa, xb) && x <= MAX(xa, xb))
    {
     Xc_free(abscisse);
     return TRUE;
    }
    else
    {
     if (ya>yb)
     {
      if (x==xa) 
      {
       Xc_free(abscisse);
       return TRUE;
      }
     }
     else
     {
      if (x==xb) 
      {
       Xc_free(abscisse);
       return TRUE;
      }
     }
    }
   }
  }
  lsegment = lsegment->NextSegment;
 }
 TriSimpleForCoord_t(nb_abscisse, abscisse);
 i = 0;
 while(i<nb_abscisse && x>abscisse[i])
  i++;
 Xc_free(abscisse);
 if (i==nb_abscisse || !(i&1))
  return FALSE;
 else
  return TRUE;
}

l_ct_seg *CalculResultingSegment(cadre1, cadre2)
c_Cadre	*cadre1;
c_Cadre	*cadre2;
{
 l_ct_seg	*lseg1;
 l_ct_seg	*lseg2;
 l_ct_seg	*lsegm1;
 l_ct_seg	*lsegm2;
 l_ct_seg	*lstseg1 = NULL;
 ct_inters	**inters1;
 ct_inters	**inters2;
 int		*nb_inters1;
 int		*nb_inters2;
 int		nb_seg1;
 int		nb_seg2;
 coord_t	dx1, dy1;
 coord_t	dx2, dy2;
 coord_t	distance;
 l_ct_seg	*lsegment;
 ct_seg		*segment;
 ct_pnt		*inters;
 boolean	is_in;
 int		i, j,k, l;

 dx1 = cadre1->sOrig_X-MIN(cadre1->sOrig_X, cadre2->sOrig_X);
 dy1 = cadre1->sOrig_Y-MIN(cadre1->sOrig_Y, cadre2->sOrig_Y);
 dx2 = cadre2->sOrig_X-MIN(cadre1->sOrig_X, cadre2->sOrig_X);
 dy2 = cadre2->sOrig_Y-MIN(cadre1->sOrig_Y, cadre2->sOrig_Y);
    
 lseg1 = CalculCadreSegment(cadre1->ListPoint, TRUE, dx1, dy1, &nb_seg1);
 lseg2 = CalculCadreSegment(cadre2->ListPoint, FALSE, dx2, dy2, &nb_seg2);
  
 inters1 = (ct_inters **)Xc_malloc("inters1*",sizeof(ct_inters *)*nb_seg1);
 *inters1 = (ct_inters *)Xc_malloc("inters1",
				   sizeof(ct_inters)*nb_seg1*nb_seg2);
 for(i = 1;i<nb_seg1;i++)
  inters1[i]=inters1[i-1]+nb_seg2;
 nb_inters1=(int *)Xc_malloc("nbinters1",sizeof(int)*nb_seg1);
 memset(nb_inters1, 0,sizeof(int)*nb_seg1);
 inters2 = (ct_inters **)Xc_malloc("inters2",sizeof(ct_inters *)*nb_seg2);
 *inters2 = (ct_inters *)Xc_malloc("inters2",
				   sizeof(ct_inters)*nb_seg2*nb_seg1);
 for(i = 1;i<nb_seg2;i++)
  inters2[i] = inters2[i-1]+nb_seg1;
 nb_inters2 = (int *)Xc_malloc("nbinters2",sizeof(int)*nb_seg2);
 memset(nb_inters2, 0, sizeof(int)*nb_seg2);
  
 lsegm1 = lseg1;
 i = 0;
 while(lsegm1)
 {
  lsegm2 = lseg2;
  j = 0;
  while(lsegm2)
  {
   inters = CalculIntersSegment(lsegm1->segment, lsegm2->segment);
   if (inters)
   {
    if (ABS(lsegm1->segment->point1->x-lsegm1->segment->point2->x)>
	ABS(lsegm1->segment->point1->y-lsegm1->segment->point2->y))
     distance = ABS(lsegm1->segment->point1->x-inters->x);
    else
     distance = ABS(lsegm1->segment->point1->y-inters->y);  
    k = 0;
    l = nb_inters1[i];
    while(k<l && inters1[i][k].distance<distance)
     k++;
    while(l>k)
    {
     memcpy(&inters1[i][l],&inters1[i][l-1],sizeof(ct_inters));
     l--;
    }
    inters1[i][k].inters = inters;
    inters1[i][k].distance = distance;
    nb_inters1[i]=nb_inters1[i]+1;
	
    if (ABS(lsegm2->segment->point1->x-lsegm2->segment->point2->x)>
	ABS(lsegm2->segment->point1->y-lsegm2->segment->point2->y))
     distance = ABS(lsegm2->segment->point1->x-inters->x);
    else
     distance = ABS(lsegm2->segment->point1->y-inters->y);  
    k = 0;
    l = nb_inters2[j];
    while(k<l && inters2[j][k].distance<distance)
     k++;
    while(l>k)
    {
     memcpy(&inters2[j][l],&inters2[j][l-1],sizeof(ct_inters));
     l--;
    }
    inters2[j][k].inters = inters;
    inters2[j][k].distance = distance;
    nb_inters2[j]=nb_inters2[j]+1;
   }
   lsegm2 = lsegm2->NextSegment;
   j++;
  }
  lsegm1 = lsegm1->NextSegment;
  i++;
 }

 k = 0;
 lsegm1 = lseg1;
 for(i = 0;i<nb_seg1;i++)
 {
  if (nb_inters1[i])
  {
   for(j = 0;j<nb_inters1[i];j++)
   {
    lsegment=(l_ct_seg *)Xc_malloc("lsegment",sizeof(l_ct_seg));
    lsegment->segment = segment = 
     (ct_seg *)Xc_malloc("segment", sizeof(ct_seg));
    segment->point1 = inters1[i][j].inters;
    segment->point2 = lsegm1->segment->point2;
    segment->cadre1 = TRUE;
    lsegm1->segment->point2 = inters1[i][j].inters;
    lsegment->PrevSegment = lsegm1;
    lsegment->NextSegment = lsegm1->NextSegment;
    if (lsegm1->NextSegment)
     lsegm1->NextSegment->PrevSegment = lsegment;
    lsegm1->NextSegment = lsegment;
    lsegm1 = lsegment;
    Xc_TRACE(("inters for polyg1: %x",inters1[i][j].inters));
   }
   k+=j;
  }
  lstseg1 = lsegm1;
  lsegm1 = lsegm1->NextSegment;
 }
 nb_seg1+=k;

 k = 0;
 lsegm2 = lseg2;
 for(i = 0;i<nb_seg2;i++)
 {
  if (nb_inters2[i])
  {
   for(j = 0;j<nb_inters2[i];j++)
   {
    lsegment=(l_ct_seg *)Xc_malloc("lsegment",sizeof(l_ct_seg));
    lsegment->segment = segment=(ct_seg *)Xc_malloc("segment",
						    sizeof(ct_seg));
    segment->point1 = inters2[i][j].inters;
    segment->point2 = lsegm2->segment->point2;
    segment->cadre1 = FALSE;
    lsegm2->segment->point2 = inters2[i][j].inters;
    lsegment->PrevSegment = lsegm2;
    lsegment->NextSegment = lsegm2->NextSegment;
    if (lsegm2->NextSegment)
     lsegm2->NextSegment->PrevSegment = lsegment;
    lsegm2->NextSegment = lsegment;
    lsegm2 = lsegment;
    Xc_TRACE(("inters for polyg2: %x",inters2[i][j].inters));
   }
   k+=j;
  }
  lsegm2 = lsegm2->NextSegment;
 }
 nb_seg2+=k;
  
 Xc_free(*inters1);
 Xc_free(inters1);
 Xc_free(nb_inters1);
 Xc_free(*inters2);
 Xc_free(inters2);
 Xc_free(nb_inters2);

 lsegm1 = lseg1;
 while(lsegm1)
 {
  is_in = IsInCadre(lseg2, nb_seg2,
		    lsegm1->segment->point1->x,
		    lsegm1->segment->point1->y);
  while(lsegm1 && !lsegm1->segment->point2->init)
  {
   lsegm1->segment->is_in = is_in;
   is_in ^= TRUE;
   lsegm1 = lsegm1->NextSegment;
  }
  if (lsegm1)
   lsegm1->segment->is_in = is_in;
  else
   break;
  lsegm1 = lsegm1->NextSegment;
 }

 lsegm2 = lseg2;
 while(lsegm2)
 {
  is_in = IsInCadre(lseg1, nb_seg1,
		    lsegm2->segment->point1->x,
		    lsegm2->segment->point1->y);
  while(lsegm2 && !lsegm2->segment->point2->init)
  {
   lsegm2->segment->is_in = is_in;
   is_in ^= TRUE;
   lsegm2 = lsegm2->NextSegment;
  }
  if (lsegm2)
   lsegm2->segment->is_in = is_in;
  else
   break;
  lsegm2 = lsegm2->NextSegment;
 }
  
 {
  char	cp1, cp2, ci1, ci2;
    
  Xc_TRACE(("nb seg1: %d  nb seg2: %d",nb_seg1, nb_seg2));
  lsegm1 = lseg1;
  cp1='0';
  cp2='1';
  ci1='a'-1;
  ci2='a';
  for(i = 0;i<nb_seg1;i++)
  {
   lsegm1->segment->point1->init?cp1++:ci1++;
   lsegm1->segment->point2->init?cp2++:ci2++;
   Xc_TRACE(("segment %c-%c  %c  %c",
	     (lsegm1->segment->point1->init?cp1:ci1),
	     (lsegm1->segment->point2->init?cp2:ci2),
	     '/',lsegm1->segment->is_in?'O':'N'));
   lsegm1 = lsegm1->NextSegment;
  }
    
  lsegm2 = lseg2;
  cp1='A'-1;
  cp2='A';
  ci1='a'-1;
  ci2='a';
  for(i = 0;i<nb_seg2;i++)
  {
   lsegm2->segment->point1->init?cp1++:ci1++;
   lsegm2->segment->point2->init?cp2++:ci2++;
   Xc_TRACE(("segment %c-%c  %c  %c",
	     (lsegm2->segment->point1->init?cp1:ci1),
	     (lsegm2->segment->point2->init?cp2:ci2),
	     lsegm2->segment->is_in?'O':'N','/'));
   lsegm2 = lsegm2->NextSegment;
  }
 }
  
 lstseg1->NextSegment = lseg2;
 lseg2->PrevSegment = lstseg1;

 return lseg1;
}

void AddPointToList(lpoint, point, cont, svlpoint)
l_Point	**lpoint;
ct_pnt	*point;
boolean	cont;
l_Point	**svlpoint;
{
 l_Point	*l_point;

 l_point=(l_Point *)Xc_malloc("lpoint",sizeof(l_Point));
 l_point->sX = point->x;
 l_point->sY = point->y;
 l_point->cont = cont;
 if (*lpoint)
 {
  (*lpoint)->NextPoint = l_point;
  *lpoint = l_point;
 }
 else
 {
  *lpoint = l_point;
  *svlpoint= *lpoint;
 }
 /*  Xc_TRACE(("adding point x: %d  y: %d  cont: %s",l_point->sX, l_point->sY,
     l_point->cont?"TRUE":"FALSE"));  */
}

l_Point *SubstractCadre(cadre1, cadre2)
c_Cadre	*cadre1;
c_Cadre	*cadre2;
{
 l_ct_seg	*lsegment;
 l_ct_seg	*lseg;
 l_ct_seg	*lseg2;
 l_ct_seg	*junk_lsegment;
 l_ct_seg	*junk_lseg;
 l_Point	*lpoint;
 l_Point	*svglpoint;
 ct_pnt		*point;
 ct_pnt		*svgpoint;
 boolean	found;

 lsegment = CalculResultingSegment(cadre1, cadre2);
 junk_lsegment = junk_lseg = NULL;
 svglpoint = lpoint = NULL;
 lseg = lsegment;
 while(lseg)
 {
  if (lseg->segment->cadre1 ^ lseg->segment->is_in)
  {
   AddPointToList(&lpoint, lseg->segment->point1, FALSE,&svglpoint);
   AddPointToList(&lpoint, lseg->segment->point2, TRUE,&svglpoint);
   if (junk_lseg)
   {
    junk_lseg->NextSegment = lseg;
    junk_lseg = lseg;
   }
   else
   {
    junk_lsegment = junk_lseg = lseg;
   }
   if (lseg->NextSegment)
    lseg->NextSegment->PrevSegment = lseg->PrevSegment;
   if (lseg->PrevSegment)
    lseg->PrevSegment->NextSegment = lseg->NextSegment;
   else
    lsegment = lseg->NextSegment;
   svgpoint = lseg->segment->point1;
   point = lseg->segment->point2;
   found = TRUE;
   do
   {
    lseg2 = lsegment;
    while(lseg2 && !((lseg2->segment->cadre1 ^ lseg2->segment->is_in)
		     && (lseg2->segment->point1==point ||
			 lseg2->segment->point2==point)))
     lseg2 = lseg2->NextSegment;
    if (lseg2)
    {
     if (lseg2->segment->point1==point)
      point = lseg2->segment->point2;
     else
      point = lseg2->segment->point1;
     if (point!=svgpoint)
      AddPointToList(&lpoint, point, TRUE,&svglpoint);
     junk_lseg->NextSegment = lseg2;
     junk_lseg = lseg2;
     if (lseg2->NextSegment)
      lseg2->NextSegment->PrevSegment = lseg2->PrevSegment;
     if (lseg2->PrevSegment)
      lseg2->PrevSegment->NextSegment = lseg2->NextSegment;
     else
      lsegment = lseg2->NextSegment;
    }
    else 
     found = FALSE;
   }
   while(found);
   lseg = lsegment;
  }
  else
   lseg = lseg->NextSegment;
 }  
 if (lpoint)
  lpoint->NextPoint = NULL;
 if (junk_lseg)
  junk_lseg->NextSegment = NULL;
 FreeCadreSegment(junk_lsegment);
 FreeCadreSegment(lsegment);
  
 lpoint = svglpoint;
 while(lpoint)
 {
  lpoint->X = lpoint->sX/cadre1->sScale;
  lpoint->Y = lpoint->sY/cadre1->sScale;
  lpoint = lpoint->NextPoint;
 }
 return svglpoint;
}

l_Point *AdditionCadre(cadre1, cadre2)
c_Cadre	*cadre1;
c_Cadre	*cadre2;
{
 l_ct_seg	*lsegment;
 l_ct_seg	*lseg;
 l_ct_seg	*lseg2;
 l_ct_seg	*junk_lsegment;
 l_ct_seg	*junk_lseg;
 l_Point	*lpoint;
 l_Point	*svglpoint;
 ct_pnt		*point;
 ct_pnt		*svgpoint;
 boolean	found;

 lsegment = CalculResultingSegment(cadre1, cadre2);
 junk_lsegment = junk_lseg = NULL;
 svglpoint = lpoint = NULL;
 lseg = lsegment;
 while(lseg)
 {
  if (!lseg->segment->is_in)
  {
   AddPointToList(&lpoint, lseg->segment->point1, FALSE,&svglpoint);
   AddPointToList(&lpoint, lseg->segment->point2, TRUE,&svglpoint);
   if (junk_lseg)
   {
    junk_lseg->NextSegment = lseg;
    junk_lseg = lseg;
   }
   else
   {
    junk_lsegment = junk_lseg = lseg;
   }
   if (lseg->NextSegment)
    lseg->NextSegment->PrevSegment = lseg->PrevSegment;
   if (lseg->PrevSegment)
    lseg->PrevSegment->NextSegment = lseg->NextSegment;
   else
    lsegment = lseg->NextSegment;
   svgpoint = lseg->segment->point1;
   point = lseg->segment->point2;
   found = TRUE;
   do
   {
    lseg2 = lsegment;
    while(lseg2 && !(!lseg2->segment->is_in
		     && (lseg2->segment->point1==point ||
			 lseg2->segment->point2==point)))
     lseg2 = lseg2->NextSegment;
    if (lseg2)
    {
     if (lseg2->segment->point1==point)
      point = lseg2->segment->point2;
     else
      point = lseg2->segment->point1;
     if (point!=svgpoint)
      AddPointToList(&lpoint, point, TRUE,&svglpoint);
     junk_lseg->NextSegment = lseg2;
     junk_lseg = lseg2;
     if (lseg2->NextSegment)
      lseg2->NextSegment->PrevSegment = lseg2->PrevSegment;
     if (lseg2->PrevSegment)
      lseg2->PrevSegment->NextSegment = lseg2->NextSegment;
     else
      lsegment = lseg2->NextSegment;
    }
    else 
     found = FALSE;
   }
   while(found);
   lseg = lsegment;
  }
  else
   lseg = lseg->NextSegment;
 }  
 if (lpoint)
  lpoint->NextPoint = NULL;
 if (junk_lseg)
  junk_lseg->NextSegment = NULL;
 FreeCadreSegment(junk_lsegment);
 FreeCadreSegment(lsegment);

 lpoint = svglpoint;
 while(lpoint)
 {
  lpoint->X = lpoint->sX/cadre1->sScale;
  lpoint->Y = lpoint->sY/cadre1->sScale;
  lpoint = lpoint->NextPoint;
 }
 return svglpoint;
}

boolean is_frame_a_rectangle(lpoint)
l_Point	*lpoint;
{
 boolean left = FALSE;
 coord_t x1,x2,y1,y2;
 int i;

 x1 = x2 = y1 = y2 = 0;
 for(i = 0; i < 4; i++)
 {
  if (!lpoint) return FALSE;
  switch(i)
  {
  case 0:
   x1 = lpoint->sX;
   y1 = lpoint->sY;
   break;
  case 1:
   if (lpoint->sX != x1)
   {
    left = TRUE;
    x2 = lpoint->sX;
   }
   else
   {
    y2 = lpoint->sY;
   }
   break;
  case 2:
   if (left)
   {
    if (lpoint->sX != x2) return FALSE;
    y2 = lpoint->sY;
   }
   else
   {
    if (lpoint->sY != y2) return FALSE;
    x2 = lpoint->sX;
   }
   break;
  case 3:
   if (left)
   { 
    if (lpoint->sX != x1) return FALSE;
    if (lpoint->sY != y2) return FALSE;
   }
   else
   {
    if (lpoint->sX != x2) return FALSE;
    if (lpoint->sY != y1) return FALSE;
   }
   break;
  }
  lpoint = lpoint->NextPoint;
  if (lpoint && !lpoint->cont) return FALSE;
 }

 if (lpoint) return FALSE;
 return TRUE;
}
