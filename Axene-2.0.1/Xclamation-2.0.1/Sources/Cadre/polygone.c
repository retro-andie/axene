/*
** polygone.c for Xclamation, XQuad and XAllWrite in Cadre/
** functions for polygone
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
** Last update Fri Jan  7 14:54:00 2000 Emmanuel Paris
*/

#include "Cadre.h"

extern c_ImageManager	*GlobImgMng;

boolean is_inters();
void DrawPolygone();
Pixmap CalculPolygoneMask();
void FreePolygone();
void CalculXminXmax();
Polygone *CreateNullPolygone();
Polygone *CreateRectPolygone();
Polygone *FormatPolygone();
Polygone *DuplicatePolygone();
boolean IsInPolygone();
void RelocatePolygoneSegbuf();
void UnionPolygone();
void SubtractPolygone();
Polygone *IntersectPolygone();
void CalculPolygone();
boolean IsAngleH(); /* return 0 if angle < or >,1 if v or ^  */

boolean IsRectIntersPolygone();
Polygone *IntersectRectPolygone();
Polygone *SubtractRectPolygone();
Pixmap CalculRectPolygoneMask();

/* *********************** */
/* * Polygone's managing * */
/* *********************** */

boolean is_inters(polyg1, polyg2)
Polygone *polyg1;
Polygone *polyg2;
{
 int x_or1, y_or1;
 int x_or2, y_or2;
 int min_y, max_y;
 Ligne_h *depart1;
 Ligne_h *depart2;
 Segment_h *segm1;
 Segment_h *segm2;
 int pnts1, pnts2;
 int i;
  
 if (!polyg1->nb_lig || !polyg2->nb_lig)
  return FALSE;
 x_or1 = polyg1->Orig_X;
 y_or1 = polyg1->Orig_Y;
 x_or2 = polyg2->Orig_X;
 y_or2 = polyg2->Orig_Y;
 if (polyg1->y_min + y_or1 < polyg2->y_min + y_or2 &&
     polyg1->y_max + y_or1 < polyg2->y_min + y_or2)
  return FALSE;
 if (polyg1->x_min + x_or1 < polyg2->x_min + x_or2 &&
     polyg1->x_max + x_or1 < polyg2->x_min + x_or2)
  return FALSE;
 if (polyg2->y_min + y_or2 < polyg1->y_min + y_or1 &&
     polyg2->y_max + y_or2 < polyg1->y_min + y_or1)
  return FALSE;
 if (polyg2->x_min + x_or2 < polyg1->x_min + x_or1 &&
     polyg2->x_max + x_or2 < polyg1->x_min + x_or1)
  return FALSE;
 min_y = MAX(polyg1->y_min + y_or1, polyg2->y_min + y_or2);
 max_y = MIN(polyg1->y_max + y_or1, polyg2->y_max + y_or2);
 depart1 = polyg1->ligne + min_y - (polyg1->y_min + y_or1);
 depart2 = polyg2->ligne + min_y - (polyg2->y_min + y_or2);
 for(i = min_y; i<=max_y; i++)
 {
  segm1 = depart1->segment;
  segm2 = depart2->segment;
  pnts1 = 0;
  pnts2 = 0;
  while(pnts1 < depart1->nb_seg && pnts2 < depart2->nb_seg)
  {
   if (segm1[pnts1].x_min + x_or1 < segm2[pnts2].x_min + x_or2)
   {
    if (segm1[pnts1].x_max + x_or1>=segm2[pnts2].x_min + x_or2)
     return TRUE;
    pnts1++;
   }
   else
   {
    if (segm2[pnts2].x_max + x_or2>=segm1[pnts1].x_min + x_or1)
     return TRUE;
    pnts2++;
   }
  }
  depart1++;
  depart2++;
 }
 return FALSE;
}

void DrawPolygone(polyg, xinfo)
Polygone	*polyg;
x_info		xinfo;
{
 int y_source;
 int y_delta;
 int i, j, px, py;
 Ligne_h *cline;
 Segment_h *cseg;
 int count_seg;
  
 if (!polyg)
  return;
 y_source = polyg->y_min;
 y_delta = polyg->nb_lig;
 cline = polyg->ligne;
 px = polyg->Orig_X;
 py = polyg->Orig_Y;
 count_seg = 0;
  
 for(i = 0; i < y_delta; i++)
 {
  cseg = cline->segment;
  for(j = 0; j < cline->nb_seg; j++)
  {
   if (count_seg!=MAX_SEGMENT)
   {
    segments[count_seg].x1 = cseg->x_min + px;
    segments[count_seg].y1 = y_source + py;
    segments[count_seg].x2 = cseg->x_max + px;
    segments[count_seg++].y2 = y_source + py;
   }
   else
   {
    XDrawSegments(xinfo.display, xinfo.window, xinfo.gc_inside,
		  segments, MAX_SEGMENT);
    count_seg = 0;
   }
   cseg++;
  }
  y_source++;
  cline++;
 }
 if (count_seg)
 {
  XDrawSegments(xinfo.display, xinfo.window, xinfo.gc_inside,
		segments, count_seg);
 } 
}

Pixmap CalculPolygoneMask(polyg, xinfo)
Polygone	*polyg;
x_info		xinfo;
{
 int	y_source;
 int	y_delta;
 int	i, j;
 int	count_seg;
 GC	gc;
 Pixmap	pixmap;
 Ligne_h	*cline;
 Segment_h	 *cseg;
  
 if (!polyg)
  return (Pixmap)NULL;
  
 pixmap = XCreatePixmap(xinfo.display, xinfo.window,
			polyg->x_max + 1,
			polyg->y_max + 1, 1);
 gc = XCreateGC(xinfo.display, pixmap, 0, NULL);
 XSetFunction(xinfo.display, gc, GXcopy);
 XSetForeground(xinfo.display, gc, 0);

 XFillRectangle(xinfo.display, pixmap, gc, 0, 0,
		polyg->x_max + 1,
		polyg->y_max + 1);

 XSetForeground(xinfo.display, gc, 1);
 XSetBackground(xinfo.display, gc, 0);
  
 cline = polyg->ligne;
 count_seg = 0;
 y_source = polyg->y_min;
 y_delta = polyg->nb_lig;
 for(i = 0; i < y_delta; i++)
 {
  cseg = cline->segment;
  for(j = 0; j < cline->nb_seg; j++)
  {
   if (count_seg != MAX_SEGMENT)
   {
    segments[count_seg].x1 = cseg->x_min;
    segments[count_seg].y1 = y_source;
    segments[count_seg].x2 = cseg->x_max;
    segments[count_seg++].y2 = y_source;
   }
   else
   {
    XDrawSegments(xinfo.display, pixmap, gc, segments, MAX_SEGMENT);
    count_seg = 0;
   }
   cseg++;
  }
  y_source++;
  cline++;
 }
 if (count_seg)
 {
  XDrawSegments(xinfo.display, pixmap, gc, segments, count_seg);
 } 
 if (GlobImgMng->draw_linesclip_bug)
 {
  XSetForeground(xinfo.display, gc, 0);
  XDrawPoint(xinfo.display, pixmap, gc, polyg->x_max, 0);
 }
 XFreeGC(xinfo.display, gc);
 return pixmap;
}

void FreePolygone(polyg, isforme)
Polygone *polyg;
boolean  isforme;
{
 if (!polyg)
  return;
  
 if (polyg->memory)
  Xc_free(polyg->segbuf_addr);
  
 if (polyg->nb_lig)
  Xc_free(polyg->ligne);

 polyg->nb_lig = 0;
 polyg->memory = 0;
 if (isforme==DEL_POLYGONE)
 {
  Xc_free(polyg);
 }
}

void CalculXminXmax(polyg)
Polygone *polyg;
{
 Ligne_h *ligne;
 int x_min, x_max;
 boolean prems;
 int i;
  
 ligne = polyg->ligne;
 prems = TRUE;
 x_min = x_max = 0;
 for(i = 0; i < polyg->nb_lig; i++)
 {
  if (ligne[i].nb_seg)
  {
   if (prems)
   {
    x_min = ligne[i].segment[0].x_min;
    x_max = ligne[i].segment[ligne[i].nb_seg - 1].x_max;
    prems = FALSE;
   }
   else
   {
    if (x_min>ligne[i].segment[0].x_min)
     x_min = ligne[i].segment[0].x_min;
    if (x_max < ligne[i].segment[ligne[i].nb_seg - 1].x_max)
     x_max = ligne[i].segment[ligne[i].nb_seg - 1].x_max;
   }
  }
 }
 polyg->x_min = x_min;
 polyg->x_max = x_max;
}

Polygone *CreateNullPolygone()
{
 Polygone *ret_polyg;
  
 ret_polyg = (Polygone *)Xc_malloc("null polyg",sizeof(Polygone));
 ret_polyg->ligne = NULL;
 ret_polyg->Orig_X = 0;
 ret_polyg->Orig_Y = 0;
 ret_polyg->y_min = 0;
 ret_polyg->y_max = 0;
 ret_polyg->x_min = 0;
 ret_polyg->x_max = 0;
 ret_polyg->nb_lig = 0;
 ret_polyg->segbuf_addr = 0;
 ret_polyg->memory = 0;
  
 return ret_polyg;
}

Polygone *CreateRectPolygone(x1, y1, x2, y2)
int x1;
int y1;
int x2;
int y2;
{
 Polygone *polyg;
 Segment_h *buffer;
 Ligne_h *ligne;
 int swp, i;
 int dx, dy;
  
 if (x1>x2)
 {
  swp = x1;
  x1 = x2;
  x2 = swp;
 }
 if (y1>y2)
 {
  swp = y1;
  y1 = y2;
  y2 = swp;
 }
 dx = x2 - x1;
 dy = y2 - y1;
 polyg = (Polygone *)Xc_malloc("rect polyg",sizeof(Polygone));
 polyg->Orig_X = x1;
 polyg->Orig_Y = y1;
 polyg->y_min = 0;
 polyg->y_max = dy;
 polyg->x_min = 0;
 polyg->x_max = dx;
 polyg->nb_lig = dy + 1;
 polyg->memory = sizeof(Segment_h)*(dy + 1);
 polyg->ligne = (Ligne_h *)Xc_malloc("rect lig",sizeof(Ligne_h)*(dy + 1));
 buffer = polyg->segbuf_addr = (Segment_h *)Xc_malloc("rect seg", 
						      polyg->memory);
 ligne = polyg->ligne;
 for(i = 0; i<=dy; i++)
 {
  ligne->nb_seg = 1;
  ligne->segment = buffer++;
  ligne->segment->x_min = 0;
  ligne->segment->x_max = dx;
  ligne++;
 }
 return polyg;
}

Polygone *FormatPolygone(polyg)
Polygone *polyg;
{
 Polygone *ret_polyg;
 Ligne_h *ligned,*lignef;
 int count_lig;
 int i, j;
  
 if (polyg->ligne[0].nb_seg!=0 && polyg->ligne[polyg->nb_lig - 1].nb_seg!=0)
 {
  return polyg;
 }  
 ret_polyg = (Polygone *)Xc_malloc("format polyg",sizeof(Polygone));
 memcpy(ret_polyg, polyg, sizeof(Polygone));
 count_lig = polyg->nb_lig;
 i = 0;
 ligned = polyg->ligne;
 while(ligned->nb_seg==0 && i < polyg->nb_lig)
 {
  ligned++;
  i++;
  count_lig--;
 }
 if (count_lig)
 {
  j = 0;
  lignef = polyg->ligne + polyg->nb_lig - 1;
  while(lignef->nb_seg==0 && j < polyg->nb_lig)
  {
   lignef--;
   j++;
   count_lig--;
  }
 }
 if (!count_lig)
 {
  ret_polyg->ligne = NULL;
  ret_polyg->y_min = 0;
  ret_polyg->y_max = 0;
  ret_polyg->x_min = 0;
  ret_polyg->x_max = 0;
  ret_polyg->nb_lig = 0;
  if (polyg->memory)
   Xc_free(polyg->segbuf_addr);
  polyg->segbuf_addr = NULL;
  polyg->memory = 0;
  if (polyg->ligne)
   Xc_free(polyg->ligne);
  Xc_free(polyg);
  return ret_polyg;
 }
 else
  ret_polyg->ligne = (Ligne_h *)Xc_malloc("format lig",sizeof(Ligne_h)*count_lig);
    
 ret_polyg->nb_lig = count_lig;
 ret_polyg->y_min = polyg->y_min + i;
 ret_polyg->y_max = polyg->y_max - j;
 memcpy(ret_polyg->ligne, ligned, sizeof(Ligne_h)*count_lig);
 Xc_free(polyg->ligne);
 Xc_free(polyg);
 return ret_polyg;
}

Polygone *DuplicatePolygone(polyg)
Polygone *polyg;
{
 Polygone *ret_polyg;
 Ligne_h  *ligne;
 Segment_h *buffer = NULL;
 int i;
  
 ret_polyg = (Polygone *)Xc_malloc("dup polyg",sizeof(Polygone));
 memcpy(ret_polyg, polyg, sizeof(Polygone));
 if (polyg->nb_lig)
 {
  ret_polyg->ligne =
   (Ligne_h *)Xc_malloc("dup ligne", sizeof(Ligne_h) * polyg->nb_lig);
  memcpy(ret_polyg->ligne, polyg->ligne, (polyg->nb_lig) * sizeof(Ligne_h));
  if (polyg->memory)
  {
   ret_polyg->segbuf_addr = buffer =
    (Segment_h *)Xc_malloc("dup seg", polyg->memory);
   memcpy(buffer, polyg->segbuf_addr, polyg->memory); 
  }
  ligne = ret_polyg->ligne;
  for(i = 0; i < polyg->nb_lig; i++)
  {
   ligne[i].segment = buffer;
   buffer += ligne[i].nb_seg;
  }
 }
 else
  ret_polyg->ligne = NULL;
 return ret_polyg;
}

boolean IsInPolygone(Forme, posX, posY)
Polygone *Forme;
int posX;
int posY;
{
 Ligne_h *ligne;
 int i;
  
 posY -= Forme->Orig_Y;
 posX -= Forme->Orig_X;

 if (posY>=Forme->y_min && posY<=Forme->y_max)
 {
  ligne= &Forme->ligne[posY - Forme->y_min];
  i = 0;
  while(i < ligne->nb_seg)
  {
   if (posX>=ligne->segment[i].x_min && posX<=ligne->segment[i].x_max)
    return TRUE;
   i++;
  }
 }
 return FALSE;
}

void RelocatePolygoneSegbuf(polyg, old_buf)
Polygone  *polyg;
Segment_h *old_buf;
{
 int		i;
 Ligne_h	*ligne_h;
 int		reloc;
  
 reloc = (char *)polyg->segbuf_addr - (char *)old_buf;
 ligne_h = polyg->ligne;
 for(i = 0; i < polyg->nb_lig; i++)
 {
  ligne_h->segment = (Segment_h *)((char *)ligne_h->segment + reloc);
  ligne_h++;
 }
}

void UnionPolygone(polygo1, polyg2)
Polygone **polygo1;
Polygone *polyg2;
{
 Polygone *ret_polyg;
 Polygone *polyg1;
 int x_or1, y_or1;
 int x_or2, y_or2;
 int min_y, max_y;
 int min_x, max_x;
 int miniy, maxiy;
 int minix = 0, maxix = 0;
 Ligne_h *depart1;
 Ligne_h *depart2;
 Ligne_h *arrivee;
 Ligne_h *cpysrc;
 Ligne_h *cpydst;
 Segment_h *segm1;
 Segment_h *segm2;
 Segment_h *segme;
 Segment_h *Segment_buf;
 int pnts1, pnts2;
 boolean change;
 boolean witch = FALSE;
 int xmin1, xmax1, xmin2, xmax2;
 int delta_x;
 int delta_y1;
 int delta_y2;
 boolean inters;
 int i, j; 
 int nb_seg;
  
 /*Xc_TRACE(("union polygone"));*/
 polyg1= *polygo1;
 if (!polyg2->nb_lig)
  return;
 if (!polyg1->nb_lig)
 {
  FreePolygone(polyg1, DEL_POLYGONE);
  polyg1 = DuplicatePolygone(polyg2);
  *polygo1 = polyg1;
  return;
 }
  
 x_or1 = polyg1->Orig_X;
 y_or1 = polyg1->Orig_Y;
 x_or2 = polyg2->Orig_X;
 y_or2 = polyg2->Orig_Y;
 miniy = MAX(polyg1->y_min + y_or1, polyg2->y_min + y_or2);
 maxiy = MIN(polyg1->y_max + y_or1, polyg2->y_max + y_or2);
 min_y = MIN(polyg1->y_min + y_or1, polyg2->y_min + y_or2);
 max_y = MAX(polyg1->y_max + y_or1, polyg2->y_max + y_or2);
 min_x = MIN(polyg1->x_min + x_or1, polyg2->x_min + x_or2);
 max_x = MAX(polyg1->x_max + x_or1, polyg2->x_max + x_or2);
 ret_polyg = (Polygone *)Xc_malloc("poly inters",sizeof(Polygone));
 ret_polyg->nb_lig = polyg1->nb_lig + polyg2->nb_lig - (maxiy - miniy + 1);
 ret_polyg->Orig_Y = min_y;
 ret_polyg->Orig_X = min_x;
 ret_polyg->y_min = 0;
 ret_polyg->y_max = ret_polyg->nb_lig - 1;
 ret_polyg->x_min = 0;
 ret_polyg->x_max = max_x - min_x;
 ret_polyg->ligne = (Ligne_h *)Xc_malloc("ligne inters",sizeof(Ligne_h)*ret_polyg->nb_lig);
 segme = Segment_buf = (Segment_h *)Xc_malloc("seg inters",
					      polyg1->memory + polyg2->memory);
 arrivee = ret_polyg->ligne;
 if (maxiy < miniy)
 {
  delta_y1 = maxiy - min_y + 1;
  delta_y2 = max_y - miniy + 1;
  inters = FALSE;
 }
 else
 {
  delta_y1 = miniy - min_y;
  delta_y2 = max_y - maxiy;
  inters = TRUE;
 }
 if (polyg1->y_min + y_or1 < polyg2->y_min + y_or2)
 {
  cpysrc = polyg1->ligne;
  delta_x = x_or1 - min_x;
 }
 else
 {
  cpysrc = polyg2->ligne;
  delta_x = x_or2 - min_x;
 }
 cpydst = arrivee;
 for(i = 0; i < delta_y1; i++)
 {
  cpydst->nb_seg = cpysrc->nb_seg;
  if (cpydst->nb_seg)
  {
   cpydst->segment = segme;
   if (delta_x)
    for(j = 0; j < cpysrc->nb_seg; j++)
    {
     segme->x_min = cpysrc->segment[j].x_min + delta_x;
     segme->x_max = cpysrc->segment[j].x_max + delta_x;
     segme++;
    }
   else
   {
    memcpy(segme, cpysrc->segment, sizeof(Segment_h)*cpysrc->nb_seg);
    segme+=cpysrc->nb_seg;
   }
  }
  else
   cpydst->segment = NULL;
  cpysrc++;
  cpydst++;
 }

 if (inters)
 {
  arrivee = ret_polyg->ligne + (miniy - min_y);
  depart1 = polyg1->ligne + miniy - y_or1 - polyg1->y_min;
  depart2 = polyg2->ligne + miniy - y_or2 - polyg2->y_min;
  for(i = miniy; i<=maxiy; i++)
  {
   segm1 = depart1->segment;
   segm2 = depart2->segment;
   arrivee->segment = segme;
   pnts1 = 0;
   pnts2 = 0;
   nb_seg = 0;
   change = TRUE;
      
   while(pnts1 < depart1->nb_seg || pnts2 < depart2->nb_seg)
   {
    if (pnts1==depart1->nb_seg)
    {
     if (change)
     {
      segme->x_min = segm2[pnts2].x_min + x_or2 - min_x;
      segme->x_max = segm2[pnts2].x_max + x_or2 - min_x;
      segme++;
      nb_seg++;
     }
     else
     {
      segme->x_min = minix - min_x;
      segme->x_max = maxix - min_x;
      segme++;
      nb_seg++;
      change = TRUE;
     }
     pnts2++;
     continue;
    }
    if (pnts2==depart2->nb_seg)
    {
     if (change)
     {
      segme->x_min = segm1[pnts1].x_min + x_or1 - min_x;
      segme->x_max = segm1[pnts1].x_max + x_or1 - min_x;
      segme++;
      nb_seg++;
     }
     else
     {
      segme->x_min = minix - min_x;
      segme->x_max = maxix - min_x;
      segme++;
      nb_seg++;
      change = TRUE;
     }
     pnts1++;
     continue;
    }
    xmin1 = segm1[pnts1].x_min + x_or1;
    xmax1 = segm1[pnts1].x_max + x_or1;
    xmin2 = segm2[pnts2].x_min + x_or2;
    xmax2 = segm2[pnts2].x_max + x_or2;      
    if (change)
    {
     if (xmin1 < xmin2)
     {
      if (xmax1 + 1 < xmin2)
      {
       segme->x_min = segm1[pnts1].x_min + x_or1 - min_x;
       segme->x_max = segm1[pnts1].x_max + x_or1 - min_x;
       segme++;
       nb_seg++;
       pnts1++;
      }
      else
      {
       minix = xmin1;
       change = FALSE;
       if (xmax1 < xmax2)
       {
	pnts1++;
	maxix = xmax2;
	witch = FALSE;
       }
       else
       {
	pnts2++;
	maxix = xmax1;
	witch = TRUE;
       }
      }
     }
     else
     {
      if (xmax2 + 1 < xmin1)
      {
       segme->x_min = segm2[pnts2].x_min + x_or2 - min_x;
       segme->x_max = segm2[pnts2].x_max + x_or2 - min_x;
       segme++;
       nb_seg++;
       pnts2++;
      }
      else
      {
       minix = xmin2;
       change = FALSE;
       if (xmax2 < xmax1)
       {
	pnts2++;
	maxix = xmax1;
	witch = TRUE;
       }
       else
       {
	pnts1++;
	maxix = xmax2;
	witch = FALSE;
       }
      }
     }
    }
    else
    {
     if (witch)
     {
      if (maxix + 1 < xmin2)
      {
       segme->x_min = minix - min_x;
       segme->x_max = maxix - min_x;
       segme++;
       nb_seg++;
       pnts1++;
       change = TRUE;
      }
      else
      {
       if (maxix < xmax2)
       {
	maxix = xmax2;
	pnts1++;
	witch = FALSE;
       }
       else
       {
	pnts2++;
       }
      }
     }
     else
     {
      if (maxix + 1 < xmin1)
      {
       segme->x_min = minix - min_x;
       segme->x_max = maxix - min_x;
       segme++;
       nb_seg++;
       pnts2++;
       change = TRUE;
      }
      else
      {
       if (maxix < xmax1)
       {
	maxix = xmax1;
	pnts2++;
	witch = TRUE;
       }
       else
       {
	pnts1++;
       }
      }
     }
    }
   }
   arrivee->nb_seg = nb_seg;
   if (!nb_seg)
    arrivee->segment = NULL;
   depart1++;
   depart2++;
   arrivee++;
  }
 }
 else
 {
  arrivee = ret_polyg->ligne + maxiy - min_y + 1;
  for(i = maxiy; i < miniy - 1; i++)
  {
   arrivee->segment = NULL;
   arrivee->nb_seg = 0;
   arrivee++;
  }
 }
  
 if (polyg1->y_max + y_or1>polyg2->y_max + y_or2)
 {
  cpysrc = polyg1->ligne + polyg1->nb_lig - 1;
  delta_x = x_or1 - min_x;
 }
 else
 {
  cpysrc = polyg2->ligne + polyg2->nb_lig - 1;
  delta_x = x_or2 - min_x;
 }
 cpydst = ret_polyg->ligne + ret_polyg->nb_lig - 1;
 for(i = 0; i < delta_y2; i++)
 {
  cpydst->nb_seg = cpysrc->nb_seg;
  if (cpysrc->nb_seg)
  {
   cpydst->segment = segme;
   if (delta_x)
    for(j = 0; j < cpysrc->nb_seg; j++)
    {
     segme->x_min = cpysrc->segment[j].x_min + delta_x;
     segme->x_max = cpysrc->segment[j].x_max + delta_x;
     segme++;
    }
   else
   {
    memcpy(segme, cpysrc->segment, sizeof(Segment_h)*cpysrc->nb_seg);
    segme+=cpysrc->nb_seg;
   }
  }
  else
   cpydst->segment = NULL;
  cpysrc--;
  cpydst--;
 }
 ret_polyg->memory = (segme - Segment_buf)*sizeof(Segment_h);
 ret_polyg->segbuf_addr=
  (Segment_h *)Xc_realloc("seg inters",Segment_buf, ret_polyg->memory);
 if (ret_polyg->segbuf_addr!=Segment_buf)
 {
  RelocatePolygoneSegbuf(ret_polyg, Segment_buf);
 }
 FreePolygone(polyg1, DEL_POLYGONE);
 *polygo1 = ret_polyg;
}

void SubtractPolygone(polygo1, polyg2)
Polygone **polygo1;
Polygone *polyg2;
{
 Polygone *ret_polyg;
 Polygone *polyg1;
 int x_or1, y_or1;
 int x_or2, y_or2;
 int min_y, max_y;
 int y_min, y_max;
 Ligne_h *depart1;
 Ligne_h *depart2;
 Ligne_h *arrivee;
 Segment_h *segm1;
 Segment_h *segm2;
 Segment_h *segme;
 Segment_h *Segment_buf;
 int pnts1, pnts2;
 boolean change;
 int xmin1 = 0, xmax1 = 0, xmin2, xmax2;
 int i; 
 int nb_seg;
  
 /*Xc_TRACE(("substract polygone"));*/
 polyg1= *polygo1;
 if (!is_inters(polyg1, polyg2))
  return ;
  
 x_or1 = polyg1->Orig_X;
 y_or1 = polyg1->Orig_Y;
 x_or2 = polyg2->Orig_X;
 y_or2 = polyg2->Orig_Y;
 min_y = MAX(polyg1->y_min + y_or1, polyg2->y_min + y_or2);
 max_y = MIN(polyg1->y_max + y_or1, polyg2->y_max + y_or2);
 y_min = min_y - y_or1 - polyg1->y_min;
 y_max = max_y - y_or1 - polyg1->y_min;
 depart1 = polyg1->ligne + min_y - y_or1 - polyg1->y_min;
 depart2 = polyg2->ligne + min_y - y_or2 - polyg2->y_min;
 ret_polyg = (Polygone *)Xc_malloc("poly subst",sizeof(Polygone));
 ret_polyg->nb_lig = polyg1->nb_lig;
 ret_polyg->y_min = polyg1->y_min;
 ret_polyg->y_max = polyg1->y_max;
 ret_polyg->Orig_Y = polyg1->Orig_Y;
 ret_polyg->Orig_X = polyg1->Orig_X;
 ret_polyg->ligne = (Ligne_h *)
  Xc_malloc("ligne subst",sizeof(Ligne_h)*ret_polyg->nb_lig);
 arrivee = ret_polyg->ligne;
 segme = Segment_buf=
  (Segment_h *)Xc_malloc("seg subst",polyg1->memory + polyg2->memory);
 for(i = 0; i < y_min; i++)
 {
  nb_seg = arrivee->nb_seg = polyg1->ligne[i].nb_seg;
  arrivee->segment = segme;
  memcpy(segme, polyg1->ligne[i].segment, sizeof(Segment_h)*nb_seg);
  segme+=nb_seg;
  arrivee++;
 }
 for(i = min_y; i<=max_y; i++)
 {
  segm1 = depart1->segment;
  segm2 = depart2->segment;
  arrivee->segment = segme;
  pnts1 = 0;
  pnts2 = 0;
  nb_seg = 0;
  change = TRUE;
  while(pnts1 < depart1->nb_seg)
  {
   if (change)
   {
    xmin1 = segm1[pnts1].x_min + x_or1;
    xmax1 = segm1[pnts1].x_max + x_or1;
   }
   change = TRUE;
   if (pnts2 < depart2->nb_seg)
   {
    xmin2 = segm2[pnts2].x_min + x_or2;
    xmax2 = segm2[pnts2].x_max + x_or2;
    if (xmin1 < xmin2)
    {
     if (xmax1 < xmin2)
     {
      segme->x_min = xmin1 - x_or1;
      segme->x_max = xmax1 - x_or1;
      segme++;
      nb_seg++;
      pnts1++;
     }
     else
     {
      segme->x_min = xmin1 - x_or1;
      segme->x_max = xmin2 - x_or1 - 1;
      segme++;
      nb_seg++;
      if (xmax1 <= xmax2)
      {
       pnts1++;
      }
      else
      {
       xmin1 = xmax2 + 1;
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
       xmin1 = xmax2 + 1;
       change = FALSE;
       pnts2++;
      }
      else
      {
       pnts1++;
      }
     }
    }
   }
   else
   {
    segme->x_min = xmin1 - x_or1;
    segme->x_max = xmax1 - x_or1;
    segme++;
    nb_seg++;
    pnts1++;
   }
  }
  arrivee->nb_seg = nb_seg;
  if (!nb_seg)
   arrivee->segment = NULL;
  arrivee++;
  depart1++;
  depart2++;
 }
 for(i = y_max + 1; i < polyg1->nb_lig; i++)
 {
  nb_seg = arrivee->nb_seg = polyg1->ligne[i].nb_seg;
  arrivee->segment = segme;
  memcpy(segme, polyg1->ligne[i].segment, sizeof(Segment_h)*nb_seg);
  segme+=nb_seg;
  arrivee++;
 }
 ret_polyg->memory = (segme - Segment_buf)*sizeof(Segment_h);
 ret_polyg->segbuf_addr=
  (Segment_h *)Xc_realloc("seg inters",Segment_buf, ret_polyg->memory);
 if (ret_polyg->segbuf_addr!=Segment_buf)
 {
  RelocatePolygoneSegbuf(ret_polyg, Segment_buf);
 }
  
 FreePolygone(polyg1, DEL_POLYGONE);
 polyg1 = ret_polyg;
 polyg1 = FormatPolygone(polyg1);
 CalculXminXmax(polyg1);
 *polygo1 = polyg1;
}

Polygone *IntersectPolygone(polyg1, polyg2)
Polygone *polyg1;
Polygone *polyg2;
{
 Polygone *ret_polyg;
 int x_or1, y_or1;
 int x_or2, y_or2;
 int min_y, max_y;
 int min_x, max_x;
 Ligne_h *depart1;
 Ligne_h *depart2;
 Ligne_h *arrivee;
 Segment_h *segm1;
 Segment_h *segm2;
 Segment_h *segme;
 Segment_h *Segment_buf;
 int pnts1, pnts2;
 boolean change;
 int xmin1 = 0, xmax1 = 0, xmin2, xmax2;
 int i; 
 int nb_seg;
  
 /*Xc_TRACE(("intersect polygone"));*/
 if (!is_inters(polyg1, polyg2))
  return CreateNullPolygone();
  
 x_or1 = polyg1->Orig_X;
 y_or1 = polyg1->Orig_Y;
 x_or2 = polyg2->Orig_X;
 y_or2 = polyg2->Orig_Y;
 min_y = MAX(polyg1->y_min + y_or1, polyg2->y_min + y_or2);
 max_y = MIN(polyg1->y_max + y_or1, polyg2->y_max + y_or2);
 min_x = MAX(polyg1->x_min + x_or1, polyg2->x_min + x_or2);
 max_x = MIN(polyg1->x_max + x_or1, polyg2->x_max + x_or2);
 depart1 = polyg1->ligne + min_y - y_or1 - polyg1->y_min;
 depart2 = polyg2->ligne + min_y - y_or2 - polyg2->y_min;
 ret_polyg = (Polygone *)Xc_malloc("poly inters",sizeof(Polygone));
 ret_polyg->nb_lig = max_y - min_y + 1;
 ret_polyg->y_min = 0;
 ret_polyg->y_max = max_y - min_y;
 ret_polyg->x_min = 0;
 ret_polyg->x_max = max_x - min_x;
 ret_polyg->Orig_Y = min_y;
 ret_polyg->Orig_X = min_x;
 ret_polyg->ligne=
  (Ligne_h *)Xc_malloc("ligne inters",sizeof(Ligne_h)*ret_polyg->nb_lig);
 segme = Segment_buf=
  (Segment_h *)Xc_malloc("seg inters",polyg1->memory + polyg2->memory);
 arrivee = ret_polyg->ligne;
 for(i = min_y; i<=max_y; i++)
 {
  segm1 = depart1->segment;
  segm2 = depart2->segment;
  arrivee->segment = segme;
  pnts1 = pnts2 = nb_seg = 0;
  change = TRUE;
  while(pnts1 < depart1->nb_seg && pnts2 < depart2->nb_seg)
  {
   if (change)
   {
    xmin1 = segm1[pnts1].x_min + x_or1;
    xmax1 = segm1[pnts1].x_max + x_or1;
   }
   change = TRUE;
   xmin2 = segm2[pnts2].x_min + x_or2;
   xmax2 = segm2[pnts2].x_max + x_or2;


   if (xmin1 < xmin2)
   {
    if (xmax1 < xmin2)
     pnts1++;
    else
    {
     if (xmax1 < xmax2)
     {
      segme->x_min = xmin2 - min_x;
      segme->x_max = xmax1 - min_x;
      segme++;
      nb_seg++;
      pnts1++;
     }
     else
     {
      segme->x_min = xmin2 - min_x;
      segme->x_max = xmax2 - min_x;
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
      segme->x_min = xmin1 - min_x;
      segme->x_max = xmax2 - min_x;
      segme++;
      nb_seg++;
      xmin1 = xmax2;
      change = FALSE;
      pnts2++;
     }
     else
     {
      segme->x_min = xmin1 - min_x;
      segme->x_max = xmax1 - min_x;    
      segme++;
      nb_seg++;
      pnts1++;
     }
    }
   }
  }
  arrivee->nb_seg = nb_seg;
  if (!nb_seg)
   arrivee->segment = NULL;
  arrivee++;
  depart1++;
  depart2++;
 }
 ret_polyg->memory = (segme - Segment_buf)*sizeof(Segment_h);
 ret_polyg->segbuf_addr=
  (Segment_h *)Xc_realloc("seg inters",Segment_buf, ret_polyg->memory);
 if (ret_polyg->segbuf_addr!=Segment_buf)
 {
  RelocatePolygoneSegbuf(ret_polyg, Segment_buf);
 }
 ret_polyg = FormatPolygone(ret_polyg);
 return ret_polyg;
}

void CalculPolygone(polyg, lpoint)
Polygone	*polyg;
l_Point	*lpoint;
{
 l_Point *prem;
 l_Point *points;
 l_Point *pointd = NULL;
 l_Point *spoint;
 int count_arr;
 int y_min;
 int y_max;
 int x_min;
 int x_max;
 int i, j, k, sk, l;
 int **coord;
 int *pnt;
 int *som;
 int cseg;
 Segment_h *buffer;
 Segment_h *buffertmp;
 Segment_h *buffertmp2;
 Segment_h *Segment_buf;
 Ligne_h *ligne_h;
 Ligne *ligne;
 Polygone *forme;
 boolean cont;
 Arrete *arrete;
 Sommet *sommet;
 Sommet *somtmp;
 int count_som;  
 int total_seg;
  
 prem = lpoint;
 count_arr = 0;
 cont = FALSE;
 while(prem)
 {
  cont = prem->cont;
  if (cont||prem!=lpoint)
   count_arr++;
  prem = prem->NextPoint;
 }
 if (cont) 
  count_arr++;
 if (!count_arr)
 {
  if (lpoint)
  {
   FreePolygone(polyg, NO_DEL_POLYGONE);
   forme = polyg;
   forme->nb_lig = 1;
   forme->y_min = 0;
   forme->y_max = 0;
   forme->x_min = 0;
   forme->x_max = 0;
   forme->ligne = (Ligne_h *)Xc_malloc("Ligne_h",sizeof(Ligne_h));
   forme->ligne->nb_seg = 1;
   forme->ligne->segment = (Segment_h *)Xc_malloc("Seg_h",
						  sizeof(Segment_h));
   forme->memory = sizeof(Segment_h);
   forme->segbuf_addr = forme->ligne->segment;
   forme->ligne->segment->x_min = 0;
   forme->ligne->segment->x_max = 0;
   Xc_TRACE(("cadre with only one point"));
  }
  else
   Xc_WARNING(("Cadre with no points !!"));
  return;
 }
#define NP NextPoint    
 if (count_arr==4 && !lpoint->NP->NP->NP->NP)
 {
  if (((lpoint->sX==lpoint->NP->sX) && 
       (lpoint->NP->NP->sX==lpoint->NP->NP->NP->sX) &&
       (lpoint->sY==lpoint->NP->NP->NP->sY) &&
       (lpoint->NP->sY==lpoint->NP->NP->sY)) ||
      ((lpoint->sX==lpoint->NP->NP->NP->sX) &&
       (lpoint->NP->sX==lpoint->NP->NP->sX) &&
       (lpoint->sY==lpoint->NP->sY) &&
       (lpoint->NP->NP->sY==lpoint->NP->NP->NP->sY)))
  {
   Xc_TRACE(("Create Cadre Rectangle"));
   forme = CreateRectPolygone(lpoint->X + polyg->Orig_X,
			      lpoint->Y + polyg->Orig_Y,
			      lpoint->NP->NP->X + polyg->Orig_X,
			      lpoint->NP->NP->Y + polyg->Orig_Y);
   FreePolygone(polyg, NO_DEL_POLYGONE);
   memcpy(polyg, forme, sizeof(Polygone));
   Xc_free(forme);
   return;
  }
 }
#undef NP    
 prem = lpoint;
 y_min = y_max = prem->Y;
 x_min = x_max = prem->X;
 while(prem)
 {
  if (prem->Y < y_min)
   y_min = prem->Y;
  if (prem->Y>y_max)
   y_max = prem->Y;
  if (prem->X < x_min)
   x_min = prem->X;
  if (prem->X>x_max)
   x_max = prem->X;
  prem = prem->NextPoint;
 }
 coord = (int **)Xc_malloc("Coord*",sizeof(int *)*(y_max - y_min + 1));
 *coord = (int *)Xc_malloc("Coord",sizeof(int *)*(y_max - y_min + 1)*
			   (count_arr<<2));
 if (!coord || !*coord)
  return;
 pnt = (int *)Xc_malloc("Pointer",sizeof(int)*(y_max - y_min + 1));
 *pnt = 0;
 for (i = 1; i<=(y_max - y_min); i++)
 {
  coord[i]=coord[i - 1]+(count_arr<<2);
  pnt[i]=0;
 }	
 arrete = (Arrete *)Xc_malloc("arretes",sizeof(Arrete)*count_arr);
 prem = lpoint;
 points = spoint = prem;
 i = 0;
 while(prem)
 {
  cont = prem->cont;
  if (cont||prem!=lpoint)
  {
   if (!cont)
   {
    arrete[i].som_src.sommet = pointd;
    arrete[i].som_dst.sommet = spoint;
    arrete[i].som_src.taken = NOT_TAKEN;
    arrete[i].som_dst.taken = NOT_TAKEN;
    if (spoint->NextPoint&&spoint->NextPoint->cont)
     arrete[i++].som_dst.angle_h = IsAngleH(spoint->Y, pointd->Y,
					    spoint->NextPoint->Y);
    else
     arrete[i++].som_dst.angle_h = ANGLE_V;
    spoint = points;
   }
   else
   {
    arrete[i].som_src.sommet = pointd;
    arrete[i].som_dst.sommet = points;
    arrete[i].som_src.taken = NOT_TAKEN;
    arrete[i].som_dst.taken = NOT_TAKEN;
    if (prem->NextPoint&&prem->NextPoint->cont)
     arrete[i++].som_dst.angle_h = IsAngleH(points->Y, pointd->Y,
					    prem->NextPoint->Y);
    else
     arrete[i++].som_dst.angle_h = IsAngleH(points->Y, pointd->Y, spoint->Y); 
   }
  }
    
  prem = prem->NextPoint;
  pointd = points;
  points = prem;
 }
 if (cont)
 {
  arrete[i].som_src.sommet = pointd;
  arrete[i].som_dst.sommet = spoint;
  arrete[i].som_src.taken = NOT_TAKEN;
  arrete[i].som_dst.taken = NOT_TAKEN;
  if (spoint->NextPoint&&spoint->NextPoint->cont)
   arrete[i++].som_dst.angle_h = IsAngleH(spoint->Y, pointd->Y,
					  spoint->NextPoint->Y);
  else
   arrete[i++].som_dst.angle_h = ANGLE_V;
 }
  
 /* for(i = 0; i < count_arr; i++)
    {
    Xc_TRACE(("segment %d: ssx:%d ssy: %d sdx: %d sdy: %d agl: %s",i, arrete[i].som_src.sommet->X, arrete[i].som_src.sommet->Y, arrete[i].som_dst.sommet->X, arrete[i].som_dst.sommet->Y, arrete[i].som_dst.angle_h?"Horizontal":"Vertical"));
    }*/
 total_seg = 0;
 for(i = 0; i < count_arr; i++)
 {
  ligne = CalculLigne(arrete[i].som_src.sommet->X,
		      arrete[i].som_src.sommet->Y,
		      arrete[i].som_dst.sommet->X,
		      arrete[i].som_dst.sommet->Y);
  if (ligne->nb_absc==1)
  {
   j = ligne->y_orig - y_min;
   coord[j][pnt[j]++]=ligne->abscisse[0].x_min;
   coord[j][pnt[j]++]=ligne->abscisse[0].x_max;
   coord[j][pnt[j]++]=ligne->abscisse[0].x_max;
   coord[j][pnt[j]++]=ligne->abscisse[0].x_max; 
   total_seg+=2;
  }
  else
  {
   for(j = ligne->y_orig + 1; j < ligne->y_dest; j++)
   {
    coord[j - y_min][pnt[j - y_min]++] = 
     ligne->abscisse[j - ligne->y_orig].x_min;
    coord[j - y_min][pnt[j - y_min]++] = 
     ligne->abscisse[j - ligne->y_orig].x_max;
    total_seg++;
   }
  }
  if (arrete[i].som_src.sommet->Y < arrete[i].som_dst.sommet->Y)
  {
   arrete[i].som_src.seg_som.x_min = ligne->abscisse[0].x_min;
   arrete[i].som_src.seg_som.x_max = ligne->abscisse[0].x_max;
   arrete[i].som_dst.seg_som.x_min=
    ligne->abscisse[ligne->y_dest - ligne->y_orig].x_min;
   arrete[i].som_dst.seg_som.x_max=
    ligne->abscisse[ligne->y_dest - ligne->y_orig].x_max;
  }
  else
  {
   arrete[i].som_src.seg_som.x_min=
    ligne->abscisse[ligne->y_dest - ligne->y_orig].x_min;
   arrete[i].som_src.seg_som.x_max=
    ligne->abscisse[ligne->y_dest - ligne->y_orig].x_max;
   arrete[i].som_dst.seg_som.x_min = ligne->abscisse[0].x_min;
   arrete[i].som_dst.seg_som.x_max = ligne->abscisse[0].x_max;
  }
  FreeLigne(ligne);
 }
  
 /*  for(i = 0; i < count_arr; i++)
     {
     Xc_TRACE(("segment %d: agl: %c %dx%d to %dx%d",i, arrete[i].som_dst.angle_h?'H':'V',arrete[i].som_src.sommet->X, arrete[i].som_src.sommet->Y, arrete[i].som_dst.sommet->X, arrete[i].som_dst.sommet->Y));
     } */
  
 sommet = (Sommet *)Xc_malloc("Sommet",sizeof(Sommet)*count_arr*2);	       
 k = 0;
 for(i = 0; i < count_arr; i++)
 {
  if(arrete[i].som_dst.taken==NOT_TAKEN)
  {
   memcpy(&sommet[k],&arrete[i].som_dst, sizeof(Sommet));
   arrete[i].som_dst.taken = TAKEN;
   /*    for(j = 0; j < count_arr; j++)
	 {
	 somtmp= &arrete[j].som_dst;
	 if (somtmp->taken==NOT_TAKEN &&
	 (somtmp->sommet->X==sommet[k].sommet->X) &&
	 (somtmp->sommet->Y==sommet[k].sommet->Y))
	 {
	 if (sommet[k].seg_som.x_min>somtmp->seg_som.x_min)
	 sommet[k].seg_som.x_min = somtmp->seg_som.x_min;
	 if (sommet[k].seg_som.x_max < somtmp->seg_som.x_max)
	 sommet[k].seg_som.x_max = somtmp->seg_som.x_max;
	 sommet[k].angle_h |= somtmp->angle_h;
	 somtmp->taken = TAKEN;
	 }
	 } */
   for(j = 0; j < count_arr; j++)
   {
    somtmp= &arrete[j].som_src;
    if (somtmp->taken==NOT_TAKEN &&
	(somtmp->sommet->X==sommet[k].sommet->X) &&
	(somtmp->sommet->Y==sommet[k].sommet->Y))
    {
     if (sommet[k].seg_som.x_min>somtmp->seg_som.x_min)
      sommet[k].seg_som.x_min = somtmp->seg_som.x_min;
     if (sommet[k].seg_som.x_max < somtmp->seg_som.x_max)
      sommet[k].seg_som.x_max = somtmp->seg_som.x_max;
     somtmp->taken = TAKEN;
    }
   }
   k++;
  }
 }
 count_som = k;
 /*  for(i = 0; i < count_som; i++)
     {
     Xc_TRACE(("Sommet %d: X=%d Y=%d Angle:%c   %d <-> %d ",i, sommet[i].sommet->X, sommet[i].sommet->Y, sommet[i].angle_h&1?'H':'V',sommet[i].seg_som.x_min, sommet[i].seg_som.x_max));
     } */ 
  
 som = (int *)Xc_malloc("Pointer",sizeof(int)*(y_max - y_min + 1));
 memset(som, 0, sizeof(int)*(y_max - y_min + 1));
 for(i = 0; i < count_som; i++)
 {
  if (sommet[i].angle_h==ANGLE_H)
  {
   coord[sommet[i].sommet->Y - y_min][pnt[sommet[i].sommet->Y - y_min]++]=
    sommet[i].seg_som.x_min;
   coord[sommet[i].sommet->Y - y_min][pnt[sommet[i].sommet->Y - y_min]++]=
    sommet[i].seg_som.x_max;
   total_seg++;
  }
  else
   som[sommet[i].sommet->Y - y_min]++;
 }
 for(i = 0; i<=(y_max - y_min); i++)
 {
  if (pnt[i])
  {
   TriSimple(pnt[i],coord[i]);
  }
 }
 FreePolygone(polyg, NO_DEL_POLYGONE);
 forme = polyg;
 forme->nb_lig = (y_max - y_min + 1);
 forme->y_min = y_min;
 forme->y_max = y_max;
 forme->x_min = x_min;
 forme->x_max = x_max;
 forme->ligne = (Ligne_h *)Xc_malloc("Ligne_h",
				     sizeof(Ligne_h)*(y_max - y_min + 1));
 Segment_buf = buffer = (Segment_h *)Xc_malloc("buffer",
					       sizeof(Segment_h) *
					       ((total_seg>>1) + count_som));
 for(i = 0; i<=(y_max - y_min); i++)
 {
  ligne_h = forme->ligne + i;
  ligne_h->nb_seg = pnt[i]>>2;
  ligne_h->segment = buffer;
  cseg = 0;
  if (ligne_h->nb_seg)
  {
   ligne_h->segment[cseg].x_min = coord[i][0];
   ligne_h->segment[cseg].x_max = coord[i][3];
   for(j = 4; j < ligne_h->nb_seg<<2; j+=4)
   {
    if (ligne_h->segment[cseg].x_max>=coord[i][j]-1)
    {
     ligne_h->segment[cseg].x_max = coord[i][j + 3];
    } 
    else
    {
     ligne_h->segment[++cseg].x_min = coord[i][j];
     ligne_h->segment[cseg].x_max = coord[i][j + 3];
    }
   }
   cseg++;
  }
  if (som[i])
  {
   j = l = 0;
   while(j < som[i])
   {
    while(sommet[l].angle_h!=ANGLE_V ||
	  sommet[l].sommet->Y - y_min!=i) 
     l++;
    x_min = sommet[l].seg_som.x_min;
    x_max = sommet[l].seg_som.x_max;
    ligne_h = forme->ligne + i;
    buffertmp = buffer;
    k = 0;
    while(k < cseg && buffertmp->x_max < x_min - 1)
    {
     buffertmp++;
     k++;
    }
    if (k==cseg)
    {
     buffertmp->x_min = x_min;
     buffertmp->x_max = x_max;
     buffertmp++;
     cseg++;
    }
    else
    {
     if (x_max < buffertmp->x_min - 1)
     {
      buffertmp2 = buffer + cseg - 1;
      while(buffertmp2>=buffertmp)
      {
       buffertmp2[1].x_min = buffertmp2->x_min;
       buffertmp2[1].x_max = buffertmp2->x_max;
       buffertmp2--;
      }
      buffertmp->x_min = x_min;
      buffertmp->x_max = x_max;
      cseg++;
     }
     else
     {
      buffertmp->x_min = MIN(buffertmp->x_min, x_min);
      buffertmp2 = buffertmp;
      sk = k;
      do
      {
       buffertmp2++;
       k++;
      }
      while(k < cseg && x_max + 1>=buffertmp2->x_min);
      buffertmp->x_max = MAX((buffertmp2 - 1)->x_max, x_max);
      sk = k - sk - 1;
      buffertmp++;
      while(k < cseg)
      {
       buffertmp->x_min = buffertmp2->x_min;
       buffertmp->x_max = buffertmp2->x_max;
       buffertmp2++;
       buffertmp++;
       k++;
      }
      cseg-=sk;
     }
    }
    j++;
   }
  }
  buffer+=cseg;
  ligne_h->nb_seg = cseg;
  if (!cseg)
   ligne_h->segment = NULL;
 }
  
 polyg->segbuf_addr=
  (Segment_h *)Xc_realloc("polyg_seg",Segment_buf,
			  (buffer - Segment_buf)*sizeof(Segment_h));
 polyg->memory = (buffer - Segment_buf)*sizeof(Segment_h);
 if (polyg->segbuf_addr!=Segment_buf)
 {
  RelocatePolygoneSegbuf(polyg, Segment_buf);
 }
 Xc_free(som);
 Xc_free(pnt);
 if (*coord)
  Xc_free(*coord);
 if (sommet)
  Xc_free(sommet);
 Xc_free(coord);
 Xc_free(arrete);
}

boolean IsAngleH(ys, yd1, yd2)
int ys;
int yd1;
int yd2;
{
 int dy1, dy2;
    
 dy1 = yd1 - ys;
 dy2 = yd2 - ys;
  
 if (SGN(dy1)==SGN(dy2))
  return ANGLE_V;
 return ANGLE_H;
}

boolean IsRectIntersPolygone(polyg, x, y, width, height)
Polygone	*polyg;
int	x;
int	y;
int	width;
int	height;
{
 int	x2;
 int	y2;
 int	nb_seg;
 int	y_min, y_max;
 Ligne_h	*ligne_min, *ligne_max;
  
 x -= polyg->Orig_X;
 y -= polyg->Orig_Y;
 x2 = x + width - 1;
 y2 = y + height - 1;
 y_min = polyg->y_min;
 y_max = polyg->y_max;
  
 if ( (x > polyg->x_max) || (y > y_max) ||
     (x2 < polyg->x_min) || (y2 < y_min) || !polyg->nb_lig)
  return FALSE;
  
 ligne_min = &polyg->ligne[MAX(y_min, y) - y_min];
 ligne_max = &polyg->ligne[MIN(y_max, y2) - y_min];
 while(ligne_min<=ligne_max)
 {
  nb_seg = 0;
  while(nb_seg < ligne_min->nb_seg)
  {
   if ( !(x > ligne_min->segment[nb_seg].x_max ||
	  x2 < ligne_min->segment[nb_seg].x_min) )
    return TRUE;
   nb_seg++;
  }
  ligne_min ++;
 }
 return FALSE;
}

Polygone *IntersectRectPolygone(polyg, x, y, width, height)
Polygone	*polyg;
int	x;
int	y;
int	width;
int	height;
{
 Polygone *ret_polyg;
 int x_or, y_or;
 int min_y, max_y;
 int min_x, max_x;
 Ligne_h *depart;
 Ligne_h *arrivee;
 Segment_h *segm;
 Segment_h *segme;
 Segment_h *Segment_buf;
 int pnts;
 int xmin, xmax, x2, y2; 
 int i; 
 int nb_seg;
  
 x2 = x + width - 1;
 y2 = y + height - 1;
 x_or = polyg->Orig_X;
 y_or = polyg->Orig_Y;
 min_y = MAX(polyg->y_min + y_or, y);
 max_y = MIN(polyg->y_max + y_or, y2);
 min_x = MAX(polyg->x_min + x_or, x);
 max_x = MIN(polyg->x_max + x_or, x2);
 depart = polyg->ligne + min_y - y_or - polyg->y_min;
 ret_polyg = (Polygone *)Xc_malloc("poly Rinters",sizeof(Polygone));
 ret_polyg->nb_lig = max_y - min_y + 1;
 ret_polyg->y_min = 0;
 ret_polyg->y_max = max_y - min_y;
 ret_polyg->x_min = 0;
 ret_polyg->x_max = max_x - min_x;
 ret_polyg->Orig_Y = min_y;
 ret_polyg->Orig_X = min_x;
 ret_polyg->ligne=
  (Ligne_h *)Xc_malloc("ligne Rinters",sizeof(Ligne_h)*ret_polyg->nb_lig);
 segme = Segment_buf=
  (Segment_h *)Xc_malloc("seg Rinters", polyg->memory);
 arrivee = ret_polyg->ligne;
 for(i = min_y; i<=max_y; i++)
 {
  segm = depart->segment;
  arrivee->segment = segme;
  pnts = nb_seg = 0;
  while(pnts < depart->nb_seg)
  {
   xmin = segm[pnts].x_min + x_or;
   xmax = segm[pnts].x_max + x_or;
      
   if (xmin > x2)
    break;
      
   if (xmax < x)
   {
    pnts++;
    continue;
   }
      
   if (x < xmin)
   {
    if (x2  < xmax)
    {
     segme->x_min = xmin - min_x;
     segme->x_max = x2 - min_x;
    }
    else
    {
     segme->x_min = xmin - min_x;
     segme->x_max = xmax - min_x;
    }
   }
   else
   {
    if (x2 < xmax)
    {
     segme->x_min = x - min_x;
     segme->x_max = x2 - min_x;
    }
    else
    {
     segme->x_min = x - min_x;
     segme->x_max = xmax - min_x;
    }
   }
   segme++;
   nb_seg++;
   pnts++;
  }
  arrivee->nb_seg = nb_seg;
  if (!nb_seg)
   arrivee->segment = NULL;
  arrivee++;
  depart++;
 }
 ret_polyg->memory = (segme - Segment_buf)*sizeof(Segment_h);
 ret_polyg->segbuf_addr=
  (Segment_h *)Xc_realloc("seg Rinters",Segment_buf, ret_polyg->memory);
 if (ret_polyg->segbuf_addr!=Segment_buf)
 {
  RelocatePolygoneSegbuf(ret_polyg, Segment_buf);
 }
 ret_polyg = FormatPolygone(ret_polyg);
 return ret_polyg;
}

Polygone *SubtractRectPolygone(polyg, x, y, width, height)
Polygone	*polyg;
int	x;
int	y;
int	width;
int	height;
{
 Polygone *ret_polyg;
 int x_or, y_or;
 int min_y, max_y;
 int min_x, max_x;
 Ligne_h *depart;
 Ligne_h *arrivee;
 Segment_h *segm;
 Segment_h *segme;
 Segment_h *Segment_buf;
 int pnts;
 int xmin, xmax, x2, y2, sx; 
 int i; 
 int nb_seg;
  
 x2 = x + width - 1;
 y2 = y + height - 1;
 x_or = polyg->Orig_X;
 y_or = polyg->Orig_Y;
 min_y = MAX(polyg->y_min + y_or, y);
 max_y = MIN(polyg->y_max + y_or, y2);
 min_x = MAX(polyg->x_min + x_or, x);
 max_x = MIN(polyg->x_max + x_or, x2);
  
 if (min_y > y2 || max_y < y || min_x > x2 || max_x < x)
  return CreateRectPolygone(x, y, x2, y2);
  
 depart = polyg->ligne + min_y - y_or - polyg->y_min;
 ret_polyg = (Polygone *)Xc_malloc("poly Rinters",sizeof(Polygone));
 ret_polyg->nb_lig = height;
 ret_polyg->y_min = 0;
 ret_polyg->y_max = height - 1;
 ret_polyg->x_min = 0;
 ret_polyg->x_max = width - 1;
 ret_polyg->Orig_Y = y;
 ret_polyg->Orig_X = x;
 ret_polyg->ligne =
  (Ligne_h *)Xc_malloc("ligne Rinters",sizeof(Ligne_h)*ret_polyg->nb_lig);
 segme = Segment_buf=
  (Segment_h *)Xc_malloc("seg Rinters", polyg->memory + 
			 sizeof(Segment_h) * height);
 arrivee = ret_polyg->ligne;
 for(i = y; i < min_y; i++)
 {
  arrivee->segment = segme;
  arrivee->nb_seg = 1;
  segme->x_min = 0;
  segme->x_max = width - 1;
  segme++;
  arrivee++;
 }
  
 for(; i <= max_y; i++)
 {
  segm = depart->segment;
  arrivee->segment = segme;
  pnts = nb_seg = 0; sx = x;
  while(pnts < depart->nb_seg)
  {
   xmin = segm[pnts].x_min + x_or;
   xmax = segm[pnts].x_max + x_or;
      
   if (xmin > x2)
    break;
      
   if (xmax < sx)
   {
    pnts++;
    continue;
   }
      
   if (sx < xmin)
   {
    if (x2  < xmax)
    {
     segme->x_min = sx - x;
     segme->x_max = xmin - x - 1;
     sx = x2 + 1;
     segme++;
     nb_seg++;
     break;
    }
    else
    {
     segme->x_min = sx - x;
     segme->x_max = xmin - x - 1;
     sx = xmax + 1;
     segme++;
     nb_seg++;
    }
   }
   else
   {
    if (x2 < xmax)
    {
     sx = x2 + 1;
     break;
    }
    else
    {
     sx = xmax + 1;
    }
   }
   pnts++;
  }
  if (sx <= x2)
  {
   segme->x_min = sx - x;
   segme->x_max = x2 - x;
   segme++;
   nb_seg++;      
  }
  arrivee->nb_seg = nb_seg;
  if (!nb_seg)
   arrivee->segment = NULL;
  arrivee++;
  depart++;
 }
  
  
 for(; i <= y2; i++)
 {
  arrivee->segment = segme;
  arrivee->nb_seg = 1;
  segme->x_min = 0;
  segme->x_max = width - 1;
  segme++;
  arrivee++;
 }
  
 ret_polyg->memory = (segme - Segment_buf)*sizeof(Segment_h);
 ret_polyg->segbuf_addr=
  (Segment_h *)Xc_realloc("seg Rinters",Segment_buf, ret_polyg->memory);
 if (ret_polyg->segbuf_addr!=Segment_buf)
 {
  RelocatePolygoneSegbuf(ret_polyg, Segment_buf);
 }
 ret_polyg = FormatPolygone(ret_polyg);
 return ret_polyg;
}

Pixmap CalculRectPolygoneMask(polyg, x, y, w, h, display, window)
Polygone	*polyg;
int		x;
int		y;
int		w;
int		h;
Display		*display;
Window		window;
{
 GC	gc;
 int	y_source;
 int	y_delta;
 int	 i, j, px, py;
 Ligne_h	 *cline;
 Segment_h	 *cseg;
 int		 count_seg;
 Pixmap	pixmap;
  
 if (!polyg)
  return (Pixmap)NULL;
 cline = polyg->ligne;
 count_seg = 0;
 px = polyg->Orig_X - x;
 py = polyg->Orig_Y - y;
 y_source = polyg->y_min + py;
 y_delta = polyg->nb_lig;
 pixmap = XCreatePixmap(display, window, w, h, 1);
 gc = XCreateGC(display, pixmap, 0, NULL);
 XSetFunction(display, gc, GXcopy);
 XSetForeground(display, gc, 0);
 XFillRectangle(display, pixmap, gc, 0, 0, w, h);
 XSetForeground(display, gc, 1);

 for(i = 0; i <y_delta; i++)
 {
  cseg = cline->segment;
  for(j = 0; j < cline->nb_seg; j++)
  {
   if (count_seg!=MAX_SEGMENT)
   {
    segments[count_seg].x1 = cseg->x_min + px;
    segments[count_seg].y1 = y_source;
    segments[count_seg].x2 = cseg->x_max + px;
    segments[count_seg++].y2 = y_source;
   }
   else
   {
    XDrawSegments(display, pixmap, gc, segments, MAX_SEGMENT);
    count_seg = 0;
   }
   cseg++;
  }
  y_source++;
  cline++;
 }
 if (count_seg)
 {
  XDrawSegments(display, pixmap, gc, segments, count_seg);
 } 
 XFreeGC(display, gc);
 return pixmap;
}

