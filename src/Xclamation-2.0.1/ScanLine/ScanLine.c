/*
** ScanLine.c for Xclamation in ScanLine/
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
** Started on  Tue Jul 18 12:19:05 1995 Emmanuel Paris
** Last update Tue Jun  9 18:36:34 1998 Emmanuel Paris
*/

#define NTRACE
#define NHISTORY

#include "ScanLine.h"

void *cons_ScanLine();
void dest_ScanLine();
void *copy_ScanLine();
int get_next_scanline();
boolean skip_scanline();
void Intersect_Segment();

void Calcul_sl_Arrete();
void Sort_sl_Arrete();
void Sort_sl_Sommet();

sf_ScanLine fc_ScanLine =
{
 cons_ScanLine,
 dest_ScanLine,
 copy_ScanLine,
 get_next_scanline,
 skip_scanline,
 Intersect_Segment,
};

void *cons_ScanLine(cadre, valeur, type_val)
c_Cadre	*cadre;
int	valeur;
boolean	type_val;
{
 c_ScanLine	*This;
 l_Point	*lpoint;
 l_Point	*svglpoint;
 l_Point	*prevlpoint;
 l_Point	*tmplpoint;
 sl_Sommet	tmpsommet;
 sl_Sommet	*slsommet;	
 sl_Arrete	*slarrete;	
 sl_Tranche	*sltranche;
 coord_t	x_min;
 coord_t	y_min;
 int		count_som;
 int		pnt_som;
 int		pnt_arr;
 int		pnt_tra;
 int		pnt_arr2;
 int		i;
 coord_t	y;
  
 /*  int 		b, c; */
  
 Xc_HISTORY(("constructor"));
 if ((This=(c_ScanLine *)Xc_malloc("ScanLine", sizeof(c_ScanLine))) == NULL)
  Xc_FATAL(("Can't build this object: memory allocation error"));
  
 This->f= &fc_ScanLine;
 lpoint = duplicate_list_point(cadre->ListPoint);
 if (cadre->sAngle)
  rotate_list_point(lpoint, cadre->ListPoint, cadre->sScale,
		    -(cadre->sAngle), cadre->saOrig_X, cadre->saOrig_Y);
 /*  trace_list_point(lpoint, cadre->X_info.display, cadre->X_info.window,
     cadre->X_info.gc_contour,
     cadre->Forme->Orig_X,
     cadre->Forme->Orig_Y,
     TRUE); */
 x_min = lpoint->sX;
 y_min = lpoint->sY;
 count_som = 0;
 tmplpoint = svglpoint = prevlpoint = lpoint;
 while(tmplpoint)
 {
  if (tmplpoint->cont)
  {
   if (tmplpoint->NextPoint && tmplpoint->NextPoint->cont)
   {
    count_som++;
   }
   else
   {
    if (prevlpoint != svglpoint)
     count_som++;
   }
  }
  else
  {
   if (prevlpoint != svglpoint && prevlpoint != svglpoint->NextPoint)
    count_som++;
   svglpoint = tmplpoint;
  }
  prevlpoint = tmplpoint;
  if (tmplpoint->sY < y_min)
   y_min = tmplpoint->sY;
  if (tmplpoint->sX < x_min)
   x_min = tmplpoint->sX;
  tmplpoint = tmplpoint->NextPoint;
 } 
 if (prevlpoint != svglpoint && prevlpoint != svglpoint->NextPoint)
  count_som++;
 tmplpoint = lpoint;
 while(tmplpoint)
 {
  tmplpoint->sX -= x_min;
  tmplpoint->sY -= y_min;
  tmplpoint = tmplpoint->NextPoint;
 }
  
 Xc_TRACE(("count sommet: %d", count_som));
  
 if (count_som < 2)
 {
  free_list_point(lpoint);
  This->nb_tranche = 0;
  return This;
 }
 slsommet = (sl_Sommet *)Xc_malloc("sommet", sizeof(sl_Sommet) * count_som);
 slarrete = (sl_Arrete *)Xc_malloc("arrete", sizeof(sl_Arrete) * count_som*2);
 sltranche = (sl_Tranche *)Xc_malloc("tranche",
				     sizeof(sl_Tranche) * (count_som));
 This->arrete_buf = slarrete;
  
 pnt_som = 0;
 tmplpoint = svglpoint = prevlpoint = lpoint;
 while(tmplpoint)
 {
  if (tmplpoint->cont)
  {
   if (tmplpoint->NextPoint && tmplpoint->NextPoint->cont)
   {
    tmpsommet.y = tmplpoint->sY;
    tmpsommet.point = tmplpoint;
    tmpsommet.point1 = prevlpoint;
    Calcul_sl_Arrete(slarrete, tmplpoint, prevlpoint);
    tmpsommet.arrete1 = slarrete++;
    tmpsommet.point2 = tmplpoint->NextPoint;
    Calcul_sl_Arrete(slarrete, tmplpoint, tmplpoint->NextPoint);
    tmpsommet.arrete2 = slarrete++;
    Sort_sl_Arrete(&tmpsommet);
    Sort_sl_Sommet(&tmpsommet, slsommet, pnt_som);
    pnt_som++;
   }
   else
   {
    if (prevlpoint != svglpoint)
    {
     tmpsommet.y = tmplpoint->sY;
     tmpsommet.point = tmplpoint;
     tmpsommet.point1 = prevlpoint;
     Calcul_sl_Arrete(slarrete, tmplpoint, prevlpoint);
     tmpsommet.arrete1 = slarrete++;
     tmpsommet.point2 = svglpoint;
     Calcul_sl_Arrete(slarrete, tmplpoint, svglpoint);
     tmpsommet.arrete2 = slarrete++;
     Sort_sl_Arrete(&tmpsommet);
     Sort_sl_Sommet(&tmpsommet, slsommet, pnt_som);
     pnt_som++;
    }
   }
  }
  else
  {
   if (prevlpoint != svglpoint && prevlpoint != svglpoint->NextPoint)
   {
    tmpsommet.y = svglpoint->sY;
    tmpsommet.point = svglpoint;
    tmpsommet.point1 = svglpoint->NextPoint;
    Calcul_sl_Arrete(slarrete, svglpoint, svglpoint->NextPoint);
    tmpsommet.arrete1 = slarrete++;
    tmpsommet.point2 = prevlpoint;
    Calcul_sl_Arrete(slarrete, svglpoint, prevlpoint);
    tmpsommet.arrete2 = slarrete++;
    Sort_sl_Arrete(&tmpsommet);
    Sort_sl_Sommet(&tmpsommet, slsommet, pnt_som);
    pnt_som++;
   }
   svglpoint = tmplpoint;
  }
  prevlpoint = tmplpoint;
  tmplpoint = tmplpoint->NextPoint;
 }
 if (prevlpoint != svglpoint && prevlpoint != svglpoint->NextPoint)
 {
  tmpsommet.y = svglpoint->sY;
  tmpsommet.point = svglpoint;
  tmpsommet.point1 = svglpoint->NextPoint;
  Calcul_sl_Arrete(slarrete, svglpoint, svglpoint->NextPoint);
  tmpsommet.arrete1 = slarrete++;
  tmpsommet.point2 = prevlpoint;
  Calcul_sl_Arrete(slarrete, svglpoint, prevlpoint);
  tmpsommet.arrete2 = slarrete++;
  Sort_sl_Arrete(&tmpsommet);
  Sort_sl_Sommet(&tmpsommet, slsommet, pnt_som);
  pnt_som++;
 }
  
 pnt_som = 0;
 pnt_tra = 0;
 while(pnt_som < count_som)
 {
  y = slsommet[pnt_som].y;
  sltranche[pnt_tra].arrete = (sl_Arrete **)
   Xc_malloc("sltr_arr", sizeof(sl_Arrete *) * (count_som));
  sltranche[pnt_tra].point = (l_Point **)
   Xc_malloc("sltr_pnt", sizeof(l_Point *) * (count_som));
  if (pnt_tra)
  {
   memcpy(sltranche[pnt_tra].point, sltranche[pnt_tra - 1].point,
	  sltranche[pnt_tra - 1].nb_arrete * sizeof(l_Point *));
   memcpy(sltranche[pnt_tra].arrete, sltranche[pnt_tra - 1].arrete,
	  sltranche[pnt_tra - 1].nb_arrete * sizeof(sl_Arrete *));
   pnt_arr = sltranche[pnt_tra - 1].nb_arrete;
  }
  else
   pnt_arr = 0;
  do
  {
   switch((int)slsommet[pnt_som].arrete1->x_value)
   {
   case SCANLINE_NOT_VALID:
    pnt_arr2 = 0;
    while(sltranche[pnt_tra].point[pnt_arr2] != slsommet[pnt_som].point)
    {
     pnt_arr2++;
    }
    memcpy(sltranche[pnt_tra].point + pnt_arr2,
	   sltranche[pnt_tra].point + pnt_arr2 + 1,
	   sizeof(l_Point *) * (pnt_arr - pnt_arr2 - 1));
    memcpy(sltranche[pnt_tra].arrete + pnt_arr2,
	   sltranche[pnt_tra].arrete + pnt_arr2 + 1,
	   sizeof(sl_Arrete *) * (pnt_arr - pnt_arr2 - 1));
    pnt_arr--;
   case SCANLINE_HORIZONTAL:
    break;
   default:
    sltranche[pnt_tra].point[pnt_arr] = slsommet[pnt_som].point1;
    sltranche[pnt_tra].arrete[pnt_arr++] = slsommet[pnt_som].arrete1;
    slsommet[pnt_som].arrete1->x_value = (real)slsommet[pnt_som].point->sX;
    break;
   }
   switch((int)slsommet[pnt_som].arrete2->x_value)
   {
   case SCANLINE_NOT_VALID:
    pnt_arr2 = 0;
    while(sltranche[pnt_tra].point[pnt_arr2] != slsommet[pnt_som].point)
    {
     pnt_arr2++;
    }
    memcpy(sltranche[pnt_tra].point + pnt_arr2,
	   sltranche[pnt_tra].point + pnt_arr2 + 1,
	   sizeof(l_Point *) * (pnt_arr - pnt_arr2 - 1));
    memcpy(sltranche[pnt_tra].arrete + pnt_arr2,
	   sltranche[pnt_tra].arrete + pnt_arr2 + 1,
	   sizeof(sl_Arrete *) * (pnt_arr - pnt_arr2 - 1));
    pnt_arr--;
   case SCANLINE_HORIZONTAL:
    break;
   default:
    sltranche[pnt_tra].point[pnt_arr] = slsommet[pnt_som].point2;
    sltranche[pnt_tra].arrete[pnt_arr++] = slsommet[pnt_som].arrete2;
    slsommet[pnt_som].arrete2->x_value = (real)slsommet[pnt_som].point->sX;
    break;
   }
   pnt_som++;
  } while(pnt_som < count_som && slsommet[pnt_som].y == y);
  sltranche[pnt_tra].nb_arrete = pnt_arr;
  if (pnt_som<count_som)
   sltranche[pnt_tra++].delta_y = slsommet[pnt_som].y - y;
  else
   sltranche[pnt_tra].delta_y = 0;
 }
 This->nb_tranche = pnt_tra;
 This->tranche = sltranche;
  
 if (type_val == SCANLINE_ORDONNEE)
 {
  pnt_tra = 0;
  while(sltranche[pnt_tra].delta_y < valeur)
  {
   for(i = 0; i < sltranche[pnt_tra].nb_arrete; i++)
   {
    sltranche[pnt_tra].arrete[i]->x_value += 
     sltranche[pnt_tra].arrete[i]->pente * sltranche[pnt_tra].delta_y;
   }
   valeur -= sltranche[pnt_tra++].delta_y;
   if (pnt_tra >= This->nb_tranche)
   {
    dest_ScanLine(This);
    return NULL;
   }
  }
  for(i = 0; i < sltranche[pnt_tra].nb_arrete; i++)
  {
   sltranche[pnt_tra].arrete[i]->x_value += 
    sltranche[pnt_tra].arrete[i]->pente * valeur;
  }
  This->cur_tranche = pnt_tra;
  This->delta_y = sltranche[pnt_tra].delta_y - valeur;
 }
 else
 {
    
 }
  
 /* TEST */
  
 Xc_TRACE(("Nombre de tranche: %d", This->cur_tranche));
 /*  c = cadre->sOrig_Y;
     XDrawLine(cadre->X_info.display,cadre->X_info.window,
     cadre->X_info.gc_contour,
     cadre->Forme->Orig_X,
     c/cadre->sScale,
     cadre->Forme->Orig_X+cadre->Forme->x_max,
     c/cadre->sScale);
     for(b = 0;b<This->nb_tranche;b++)
     {
     c+=sltranche[b].delta_y;
     XDrawLine(cadre->X_info.display,cadre->X_info.window,
     cadre->X_info.gc_contour,
     cadre->Forme->Orig_X,
     c/cadre->sScale,
     cadre->Forme->Orig_X+cadre->Forme->x_max,
     c/cadre->sScale);
     } */   
 /* END TEST */
  
 Xc_free(slsommet);
 free_list_point(lpoint);
  
 return This;
}

void dest_ScanLine(This)
c_ScanLine *This;
{
 int i;
  
 Xc_HISTORY(("destructor"));

 if (This->nb_tranche != 0)
 {
  for(i = 0; i < This->nb_tranche + 1; i++)
  {
   Xc_free(This->tranche[i].point);
   Xc_free(This->tranche[i].arrete);
  }
  Xc_free(This->tranche);
  Xc_free(This->arrete_buf);
 }
 Xc_free(This);
}

void *copy_ScanLine(This)
c_ScanLine *This;
{
 Xc_TRACE(("Function Copy ScanLine not usefull"));
 return NULL;;
}

int get_next_scanline(This, segment, steper)
c_ScanLine	*This;
sl_Segment	**segment;
coord_t		steper;
{
 sl_Tranche	*sltranche;
 int		i;
 int		sum_y;
  
 sl_Segment	*segmentA;
 int		nb_segmentA;
  
 sl_Segment	*segmentB;
 int		nb_segmentB;
  
 sl_Segment	*tmpseg;
 coord_t	*buftmpseg;
 coord_t	*bufseg;
  
 int		delta_y;
  
 Xc_HISTORY(("get_next_scanline(%g)",  SCALE_TO_POINTS(steper)));
 sum_y = steper;

 if (This->nb_tranche == 0)
 {
  *segment = NULL;
  return -1;
 }

 sltranche = This->tranche + This->cur_tranche;
 delta_y = MIN(This->delta_y, sum_y);
  
 nb_segmentA = sltranche->nb_arrete >> 1;
 if (nb_segmentA)
 {
  segmentA = (sl_Segment *)
   Xc_malloc("slgA", sizeof(sl_Segment) * nb_segmentA);
  bufseg=(coord_t *)segmentA;
  
  tmpseg = (sl_Segment *)Xc_malloc("slgA", sizeof(sl_Segment) * nb_segmentA);
  buftmpseg = (coord_t *)tmpseg;
 }
 else
 {
  segmentA = tmpseg = (sl_Segment *)NULL;
  bufseg = buftmpseg = (coord_t *)NULL;
 }
  
 for(i = 0; i < sltranche->nb_arrete; i++)
 {
  *bufseg = sltranche->arrete[i]->x_value;
  bufseg++;
  sltranche->arrete[i]->x_value += sltranche->arrete[i]->pente * delta_y;
  *buftmpseg = sltranche->arrete[i]->x_value;
  buftmpseg++;
 }
 TriSimpleForCoord_t(nb_segmentA << 1, (coord_t *)segmentA); 
 TriSimpleForCoord_t(nb_segmentA << 1, (coord_t *)tmpseg); 
 for(i = 0; i < nb_segmentA; i++)
 {
  if (tmpseg[i].x_min > segmentA[i].x_min)
   segmentA[i].x_min = tmpseg[i].x_min;
  if (tmpseg[i].x_max < segmentA[i].x_max)
   segmentA[i].x_max = tmpseg[i].x_max;
 }
 if (tmpseg)
  Xc_free(tmpseg);
  
 if (delta_y == sum_y)
 {
  This->delta_y -= sum_y;
  *segment = segmentA;
  return nb_segmentA;
 }
  
 sum_y -= delta_y;
 do
 {
  This->cur_tranche++;
  if (This->cur_tranche == This->nb_tranche)
  {
   if (segmentA)
    Xc_free(segmentA);
   *segment = (void *)NULL;
   return -1;
  }
  sltranche++;
    
  delta_y = MIN(sltranche->delta_y, sum_y);
    
  nb_segmentB = sltranche->nb_arrete >> 1;
  if (nb_segmentB)
  {
   segmentB = (sl_Segment *)
    Xc_malloc("slgB", sizeof(sl_Segment) * nb_segmentB);
   bufseg = (coord_t *)segmentB;
	
   tmpseg = (sl_Segment *)Xc_malloc("slgB", sizeof(sl_Segment) * nb_segmentB);
   buftmpseg = (coord_t *)tmpseg;
  }
  else 
  {
   segmentB = tmpseg = (sl_Segment *)NULL;
   buftmpseg = bufseg = (coord_t *)NULL;
  }
  for(i = 0; i < sltranche->nb_arrete; i++)
  {
   *bufseg = sltranche->arrete[i]->x_value;
   bufseg++;
   sltranche->arrete[i]->x_value += sltranche->arrete[i]->pente * delta_y;
   *buftmpseg = sltranche->arrete[i]->x_value;
   buftmpseg++;
  }
  TriSimpleForCoord_t(nb_segmentB << 1, (coord_t *)segmentB); 
  TriSimpleForCoord_t(nb_segmentB << 1, (coord_t *)tmpseg); 
  for(i = 0; i < nb_segmentB; i++)
  {
   if (tmpseg[i].x_min > segmentB[i].x_min)
    segmentB[i].x_min = tmpseg[i].x_min;
   if (tmpseg[i].x_max < segmentB[i].x_max)
    segmentB[i].x_max = tmpseg[i].x_max;
  }
  if (tmpseg)
   Xc_free(tmpseg);
  Intersect_Segment(&segmentA, &nb_segmentA, segmentB, nb_segmentB);
    
  sum_y -= delta_y;
 } while(sum_y);
  
 This->delta_y = sltranche->delta_y - delta_y;
 *segment = segmentA;
 return nb_segmentA;
}

boolean skip_scanline(This, valeur)
c_ScanLine	*This;
coord_t		valeur;
{		    
 int		i;
 int		pnt_tra;
 sl_Tranche	*sltranche;
  
 Xc_HISTORY(("skip_scanline(%g)", SCALE_TO_POINTS(valeur)));
 pnt_tra = This->cur_tranche;

 if (This->nb_tranche == 0)
  return FALSE;
 Xc_TRACE(("cur_tranche: %d", pnt_tra));
 sltranche = This->tranche;
 if (This->delta_y < valeur)
 {
  for(i = 0; i < sltranche[pnt_tra].nb_arrete; i++)
  {
   sltranche[pnt_tra].arrete[i]->x_value += 
    sltranche[pnt_tra].arrete[i]->pente * This->delta_y;
  }
  valeur -= This->delta_y;
  if (++pnt_tra >= This->nb_tranche)
  {
   return FALSE;
  }
  This->delta_y = sltranche[pnt_tra].delta_y;
 }
 while(sltranche[pnt_tra].delta_y<valeur)
 {
  for(i = 0; i < sltranche[pnt_tra].nb_arrete; i++)
  {
   sltranche[pnt_tra].arrete[i]->x_value += 
    sltranche[pnt_tra].arrete[i]->pente * sltranche[pnt_tra].delta_y;
  }
  valeur -= sltranche[pnt_tra++].delta_y;
  if (pnt_tra >= This->nb_tranche)
  {
   return FALSE;
  }
  This->delta_y = sltranche[pnt_tra].delta_y;
 }
 for(i = 0; i < sltranche[pnt_tra].nb_arrete; i++)
 {
  sltranche[pnt_tra].arrete[i]->x_value += 
   sltranche[pnt_tra].arrete[i]->pente * valeur;
 }
 This->cur_tranche = pnt_tra;
 This->delta_y -= valeur;
 Xc_TRACE(("end_tranche: %d", pnt_tra));
 return TRUE;
}

void Calcul_sl_Arrete(arrete, lpoint1, lpoint2)
sl_Arrete	*arrete;
l_Point	*lpoint1;
l_Point	*lpoint2;
{
 arrete->pente = 0.0;
 if (lpoint1->sY > lpoint2->sY)
 {
  arrete->x_value = SCANLINE_NOT_VALID;
 }
 else
 {
  if (lpoint1->sY == lpoint2->sY)
   arrete->x_value = SCANLINE_HORIZONTAL;    
  else
  {
   if (lpoint2->sX != lpoint1->sX)
    arrete->pente = (((real)(lpoint2->sX - lpoint1->sX) /
		    (real)(lpoint2->sY - lpoint1->sY)));
   arrete->x_value = SCANLINE_VALID;
  }
 }
}

void Sort_sl_Arrete(sommet)
sl_Sommet	*sommet;
{
 sl_Arrete	*swap_arrete;
 l_Point	*swap_lpoint;
  
 if (sommet->arrete1->pente > sommet->arrete2->pente)
 {
  swap_lpoint = sommet->point1;
  swap_arrete = sommet->arrete1;
  sommet->point1 = sommet->point2;
  sommet->arrete1 = sommet->arrete2;
  sommet->point2 = swap_lpoint;
  sommet->arrete2 = swap_arrete;
 }
}

void Sort_sl_Sommet(sommet, tsommet, nb_som)
sl_Sommet	*sommet;
sl_Sommet	*tsommet;
int	nb_som;
{
 int	i;
 sl_Sommet	*tmpsommet;
  
 i = 0;
 while(i < nb_som && tsommet[i].y < sommet->y)
 {
  i++;
 }
 if (i != nb_som)
 {
  tmpsommet = tsommet + nb_som;
  while(tmpsommet > tsommet + i)
  {
   memcpy(tmpsommet, tmpsommet - 1, sizeof(sl_Sommet));
   tmpsommet--;
  }
 }
 memcpy(tsommet + i, sommet, sizeof(sl_Sommet));
}

void Intersect_Segment(segmentA, nb_segmentA, segmentB, nb_segmentB)
sl_Segment	**segmentA;
int		*nb_segmentA;
sl_Segment	*segmentB;
int		nb_segmentB;
{
 sl_Segment	*ret_seg;
 sl_Segment	*segme;
 sl_Segment	*segm1;
 sl_Segment	*segm2;
 int		pnts1;
 int		pnts2;
 int		nb_seg;
 coord_t	xmin1 = COORD_ZERO;
 coord_t	xmax1 = COORD_ZERO;
 coord_t	xmin2;
 coord_t	xmax2;
 boolean	change;
  
 Xc_HISTORY(("intersect_segments"));
  
 segm1 = *segmentA;
 segm2 = segmentB;
 ret_seg = segme = (sl_Segment *)Xc_malloc("semint", sizeof(sl_Segment) *
					 (*nb_segmentA + nb_segmentB));
  
 pnts1 = pnts2 = nb_seg = 0;
 change = TRUE;
 while(pnts1 < *nb_segmentA && pnts2 < nb_segmentB)
 {
  if (change)
  {
   xmin1 = segm1[pnts1].x_min;
   xmax1 = segm1[pnts1].x_max;
  }
  change = TRUE;
  xmin2 = segm2[pnts2].x_min;
  xmax2 = segm2[pnts2].x_max;
  if (xmin1 < xmin2)
  {
   if (xmax1 < xmin2)
    pnts1++;
   else
   {
    if (xmax1 < xmax2)
    {
     segme->x_min = xmin2;
     segme->x_max = xmax1;
     segme++;
     nb_seg++;
     pnts1++;
    }
    else
    {
     segme->x_min = xmin2;
     segme->x_max = xmax2;
     segme++;
     nb_seg++;
     xmin1 = xmax2;
     change = FALSE;
     pnts2++;
    }
   }
  }
  else
  {
   if (xmax2 < xmin1)
    pnts2++;
   else
   {
    if (xmax2 < xmax1)
    {
     segme->x_min = xmin1;
     segme->x_max = xmax2;
     segme++;
     nb_seg++;
     xmin1 = xmax2;
     change = FALSE;
     pnts2++;
    }
    else
    {
     segme->x_min = xmin1;
     segme->x_max = xmax1;    
     segme++;
     nb_seg++;
     pnts1++;
    }
   }
  }
 }
 if (*segmentA)
 {
  Xc_free(*segmentA);
  *segmentA = NULL;
 }
 if (segmentB)
 {
  Xc_free(segmentB);
 }

 *nb_segmentA = nb_seg;
 if (!nb_seg)
 {
  Xc_free(ret_seg);
  ret_seg = 0;
 }
 else
  *segmentA = ret_seg;
}



