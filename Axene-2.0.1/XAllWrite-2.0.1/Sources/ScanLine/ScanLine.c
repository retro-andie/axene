/*
** ScanLine.c for XAllWrite in ScanLine/
** ScanLine object: calcul line width for text placement.
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Wed Apr 23 15:51:28 1997 Emmanuel Paris
** Last update Tue Aug  4 18:23:46 1998 Emmanuel Paris
*/

#include "ScanLine.h"

static void *cons_ScanLine();
static void dest_ScanLine();
static void *copy_ScanLine();
static void init_scanline ___PROTO((c_ScanLine *This, coord_t y));
static coord_t start_scanline ___PROTO((c_ScanLine *This));
static coord_t newline_scanline ___PROTO((c_ScanLine *This, c_TextRuler *ruler,
					  boolean np, coord_t height));
static sl_Segment *get_next_scanline ___PROTO((c_ScanLine *This, 
					       c_TextRuler *ruler, boolean np,
					       coord_t height, coord_t *y,
					       int *nb_seg));
static boolean init_scan_state ___PROTO((scan_state_t *state, coord_t yinit));
static void clean_scan_state ___PROTO((c_ScanLine *This));
static void Calcul_sl_Arrete ___PROTO((sl_Arrete *arrete, l_Point *lpoint1,
				       l_Point *lpoint2));
static void Sort_sl_Arrete ___PROTO((sl_Sommet *sommet));
static void Sort_sl_Sommet ___PROTO((sl_Sommet *sommet, sl_Sommet *tsommet,
				     int nb_som));
static int get_next_scanline_state ___PROTO((scan_state_t *state,
					     sl_Segment	**segment,
					     coord_t height));
static boolean skip_scanline_state ___PROTO((scan_state_t *state, 
					     coord_t valeur));
static void Intersect_Segment ___PROTO((sl_Segment **segmentA,int *nb_segmentA,
					sl_Segment *segmentB,int nb_segmentB));
static void Union_Segment ___PROTO((sl_Segment **segmentA,int *nb_segmentA,
				    sl_Segment *segmentB,int nb_segmentB));
static void Complement_Segment ___PROTO((c_ScanLine *This, c_TextRuler *ruler,
					 boolean np, textflow_t tf_mode,
					 sl_Segment **segment, int *nb_seg));
#ifdef SC_DRAW
#include "Colormap.h"
extern c_Colormap *GlobColormap;
static void draw_segment ___PROTO((c_ScanLine *This, coord_t y, coord_t height,
				   sl_Segment *seg, int nb_seg));
#endif

sf_ScanLine fc_ScanLine =
{
 cons_ScanLine,
 dest_ScanLine,
 copy_ScanLine,
 init_scanline,
 start_scanline,
 newline_scanline,
 get_next_scanline,
};

static void *cons_ScanLine(page)
c_Page	*page;
{
 c_ScanLine *This;

 Xc_HISTORY(("Constructor ScanLine..."));

 if((This = (c_ScanLine *)Xc_malloc("ScanLine",
				    sizeof (c_ScanLine))) == NULL)
  return NULL;
 memset(This, 0, sizeof(c_ScanLine));
 This->f = &fc_ScanLine;

 This->page = page;

 This->record_segments = NULL;
 This->state_handle = Xc_createItem("scans", 100, sizeof(scan_state_t));
 This->state = NULL;

 Xc_TRACE(("Constructor ScanLine done"));
 return This;
}

static void dest_ScanLine(This)
c_ScanLine *This;
{ 
 Xc_HISTORY(("Destructor ScanLine..."));
 
 clean_scan_state(This);
 Xc_destroyItem(This->state_handle);

 if (This->record_segments) Xc_free(This->record_segments);
 Xc_free(This);
 Xc_TRACE(("ScanLine Destroyed"));
}

static void *copy_ScanLine(Orig)
c_ScanLine *Orig;
{
 Xc_TRACE(("Copy ScanLine not done."));
 return NULL;
}

static coord_t start_scanline(This)
c_ScanLine *This;
{
 init_scanline(This, This->page->top_margin);
 return This->y;
}

static void init_scanline(This, y)
c_ScanLine *This;
coord_t	   y;
{
 c_Cadre *cadre;
 l_Cadre *lcadre;
 scan_state_t *state, *ptr;

 This->y = y;
 lcadre = This->page->frmg->ListCadre;
 clean_scan_state(This);
 
 while(lcadre)
 {
  switch(lcadre->cadre->textflow_mode & XcTFM_HOLE_MASK)
  {
  case XcTFM_UNDER:
  case XcTFM_OVER:
   break;
  default:
   cadre = lcadre->cadre;
   if (cadre->sOrig_Y + cadre->sHeight >= y)
   {
    state = (scan_state_t *)Xc_new(This->state_handle);
    state->y_min = cadre->sOrig_Y;

    if (!This->state ||  state->y_min < This->state->y_min)
    {
     state->next = This->state;
     This->state = state;
    }
    else
    {
     ptr = This->state;
     while(ptr->next)
     {
      if (ptr->next->y_min > state->y_min)
       break;
      ptr = ptr->next;
     }
     state->next = ptr->next;
     ptr->next = state;
    }
    
    state->arrete_buf = NULL;
    state->nb_tranche = state->cur_tranche = 0;
    state->tranche = NULL;
    state->delta_y = COORD_ZERO;
    state->status = SSS_NOT_INITIATED;
    state->lcadre = lcadre;
   }
   break;
  }
  
  lcadre = lcadre->NextCadre;
 }
}

static coord_t newline_scanline(This, ruler, np, height)
c_ScanLine *This;
c_TextRuler *ruler;
boolean np;
coord_t	height;
{
 scan_state_t	*state;
 coord_t step, sheight;

 if (ruler->inter_mode == TRUE)	/* inter_line absolute */
  step = ruler->abs_inter_line;
 else
  step = height + ruler->rel_inter_line;
 
 if (np == TRUE)		/* new paragraph: add inter-paragraph */
  step += ruler->inter_paragraph;


 state = This->state;
 while(state)
 {
  if (state->status == SSS_INITIATED)
  {
   sheight = step;
   if (This->y < state->y_min)
    sheight -= (state->y_min - This->y);

   if (!skip_scanline_state(state, sheight))
    state->status = SSS_FINISHED;
  }
  state = state->next;
 }
 This->y += step;

 return This->y;
}

static sl_Segment *get_next_scanline(This, ruler, np, height, y, nb_seg)
c_ScanLine *This;
c_TextRuler *ruler;
boolean np;
coord_t	height;
coord_t *y;
int	*nb_seg;
{
 scan_state_t	*state;
 coord_t	x_min, x_max;
 sl_Segment	*seg, *tmpseg;
 int		nbseg, tmpnbseg;
 coord_t	sheight;
 boolean	frameline;	

 new_getline:
 
 if (This->record_segments) 
 {
  Xc_free(This->record_segments);
  This->record_segments = NULL;
 }
 
 nbseg = 0;
 seg = (sl_Segment *)-1;
 
 if (This->y + height > This->page->sHeight - This->page->bottom_margin)
 {
  *nb_seg = -1;
  return NULL;
 }
 
 state = This->state;
 
 while(state && state->status == SSS_FINISHED)
  state = state->next;

 if (state)
 {
  while(state && (*y + height) > state->y_min)
  {
   if (state->status == SSS_NOT_INITIATED)
   {
    if (!init_scan_state(state, MAX(COORD_ZERO, *y - state->y_min)))
    {
     state->status = SSS_FINISHED;
     state = state->next;
     continue;
    }
    state->status = SSS_INITIATED;
   }

   state = state->next;
  }
 }

 frameline = FALSE;

 state = This->state;
 while(state)
 {
  switch(state->status)
  {
  case SSS_NOT_INITIATED:
  case SSS_FINISHED:
   break;
  case SSS_INITIATED:
   frameline = TRUE;
   sheight = height;
   if (*y < state->y_min)
    sheight -= (state->y_min - *y);

   tmpnbseg = get_next_scanline_state(state, &tmpseg, sheight);
   Complement_Segment(This, ruler, np, state->lcadre->cadre->textflow_mode,
		      &tmpseg, &tmpnbseg);
   
   if (seg == (sl_Segment *)-1)
   {
    nbseg = tmpnbseg;
    seg = tmpseg;
   }
   else
   {
    if (seg)
     Intersect_Segment(&seg, &nbseg, tmpseg, tmpnbseg);
    else
    {
     if (tmpseg) Xc_free(tmpseg);
    }
   }
   break;
  }
  state = state->next;
 }

 if (frameline)
 {
#ifdef SC_DRAW
  draw_segment(This, *y, height, seg, nbseg);
#endif
  This->record_segments = seg;
  
  if (nbseg == 0)
  {
   *y = newline_scanline(This, ruler, np, height);
   goto new_getline;
  }
  
  *nb_seg = nbseg;
 }
 else
 {
  x_min = This->page->left_margin;
  x_max = This->page->sWidth - This->page->right_margin;
  
  seg = This->segments;
  
  seg[0].x_min = x_min + ruler->left_margin;
  seg[0].x_max = x_max - ruler->right_margin;
  if (np == TRUE)		/* new paragraph: add alinea */
   seg[0].x_min += ruler->paragraph_indent; 
  
  if (seg[0].x_min >= seg[0].x_max)
  {
   *nb_seg = 0;
   return NULL;
  }
  
  /*seg[1].x_max = seg[0].x_max;*/
  /*seg[1].x_min = seg[0].x_max -*/
  /*(x_max - x_min - SCALE_FROM_MILLIMETERS(10.0)) / 2;*/
  /*seg[0].x_max = seg[1].x_min - SCALE_FROM_MILLIMETERS(10.0);*/
  /**nb_seg = 2;*/

  *nb_seg = 1;
 }
 return seg;
}

/*----------------------- PRIVATE FUNCTIONS ------------------------------*/
static boolean init_scan_state(state, yinit)
scan_state_t	*state;
coord_t		yinit;
{
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
 
 lpoint = duplicate_list_point(state->lcadre->cadre->ListPoint);
 
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
  state->nb_tranche = 0;
  return FALSE;
 }
 
 slsommet = (sl_Sommet *)Xc_malloc("sommet", sizeof(sl_Sommet) * count_som);
 slarrete = (sl_Arrete *)Xc_malloc("arrete", sizeof(sl_Arrete) * count_som*2);
 sltranche = (sl_Tranche *)Xc_malloc("tranche",
				     sizeof(sl_Tranche) * (count_som));
 state->arrete_buf = slarrete;
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
  } 
  while(pnt_som < count_som && slsommet[pnt_som].y == y);
  sltranche[pnt_tra].nb_arrete = pnt_arr;
  if (pnt_som<count_som)
   sltranche[pnt_tra++].delta_y = slsommet[pnt_som].y - y;
  else
   sltranche[pnt_tra].delta_y = 0;
 }
 
 state->nb_tranche = pnt_tra;
 state->tranche = sltranche;
 
 pnt_tra = 0;
 while(sltranche[pnt_tra].delta_y < yinit)
 {
  for(i = 0; i < sltranche[pnt_tra].nb_arrete; i++)
  {
   sltranche[pnt_tra].arrete[i]->x_value += 
    sltranche[pnt_tra].arrete[i]->pente * sltranche[pnt_tra].delta_y;
  }
  yinit -= sltranche[pnt_tra++].delta_y;
  if (pnt_tra >= state->nb_tranche)
  {
   return FALSE;			
  }
 }
 for(i = 0; i < sltranche[pnt_tra].nb_arrete; i++)
 {
  sltranche[pnt_tra].arrete[i]->x_value += 
   sltranche[pnt_tra].arrete[i]->pente * yinit;
 }
 state->cur_tranche = pnt_tra;
 state->delta_y = sltranche[pnt_tra].delta_y - yinit;
 
 Xc_free(slsommet);
 free_list_point(lpoint);

 return TRUE;
}

static void clean_scan_state(This)
c_ScanLine *This;
{
 scan_state_t *ptr, *ptr2;
 int i;

 ptr = This->state;
 while(ptr)
 {
  if (ptr != SSS_NOT_INITIATED)
  {
   if (ptr->nb_tranche != 0)
   {
    for(i = 0; i < ptr->nb_tranche + 1; i++)
    {
     Xc_free(ptr->tranche[i].point);
     Xc_free(ptr->tranche[i].arrete);
    }
    Xc_free(ptr->tranche);
    Xc_free(ptr->arrete_buf);
   }
  }
  ptr2 = ptr->next;
  Xc_delete(This->state_handle, ptr);
  ptr = ptr2;
 }
 This->state = NULL;
}

static void Calcul_sl_Arrete(arrete, lpoint1, lpoint2)
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

static void Sort_sl_Arrete(sommet)
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

static void Sort_sl_Sommet(sommet, tsommet, nb_som)
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

static int get_next_scanline_state(state, segment, height)
scan_state_t	*state;
sl_Segment	**segment;
coord_t		height;
{
 sl_Tranche	*sltranche;
 int		i;
 int		sum_y;
 int		cur_tranche;
  
 sl_Segment	*segmentA;
 int		nb_segmentA;
  
 sl_Segment	*segmentB;
 int		nb_segmentB;
  
 sl_Segment	*tmpseg;
 coord_t	*buftmpseg;
 coord_t	*bufseg;
  
 int		delta_y;
 coord_t	ox = state->lcadre->cadre->sOrig_X;
  
 Xc_HISTORY(("get_next_scanline(%g)",  SCALE_TO_POINTS(height)));
 sum_y = height;

 if (state->nb_tranche == 0)
 {
  *segment = NULL;
  return -1;
 }
 
 cur_tranche = state->cur_tranche;
 sltranche = state->tranche + cur_tranche;
 delta_y = MIN(state->delta_y, sum_y);
  
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
  *bufseg = sltranche->arrete[i]->x_value + ox;
  bufseg++;
  *buftmpseg = sltranche->arrete[i]->x_value + 
   sltranche->arrete[i]->pente * delta_y + ox;
  buftmpseg++;
 }

 TriSimpleForCoord_t(nb_segmentA << 1, (coord_t *)segmentA); 
 TriSimpleForCoord_t(nb_segmentA << 1, (coord_t *)tmpseg); 

 for(i = 0; i < nb_segmentA; i++)
 {
  if (tmpseg[i].x_min < segmentA[i].x_min)
   segmentA[i].x_min = tmpseg[i].x_min;
  if (tmpseg[i].x_max > segmentA[i].x_max)
   segmentA[i].x_max = tmpseg[i].x_max;
 }
 if (tmpseg)
  Xc_free(tmpseg);
  
 if (delta_y == sum_y)
 {
  *segment = segmentA;
  return nb_segmentA;
 }
  
 sum_y -= delta_y;
 do
 {
  cur_tranche++;
  if (cur_tranche == state->nb_tranche)
  {
   *segment = segmentA;
   return nb_segmentA;
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
   *bufseg = sltranche->arrete[i]->x_value + ox;
   bufseg++;
   *buftmpseg = sltranche->arrete[i]->x_value + 
    sltranche->arrete[i]->pente * delta_y + ox;
   buftmpseg++;
  }

  TriSimpleForCoord_t(nb_segmentB << 1, (coord_t *)segmentB); 
  TriSimpleForCoord_t(nb_segmentB << 1, (coord_t *)tmpseg); 

  for(i = 0; i < nb_segmentB; i++)
  {
   if (tmpseg[i].x_min < segmentB[i].x_min)
    segmentB[i].x_min = tmpseg[i].x_min;
   if (tmpseg[i].x_max > segmentB[i].x_max)
    segmentB[i].x_max = tmpseg[i].x_max;
  }
  if (tmpseg)
   Xc_free(tmpseg);

  Union_Segment(&segmentA, &nb_segmentA, segmentB, nb_segmentB);
    
  sum_y -= delta_y;
 }
 while(sum_y);
  
 *segment = segmentA;
 return nb_segmentA;
}

static boolean skip_scanline_state(state, height)
scan_state_t	*state;
coord_t		height;
{		    
 int		i;
 int		pnt_tra;
 sl_Tranche	*sltranche;
  
 Xc_HISTORY(("skip_scanline(%g)", SCALE_TO_POINTS(height)));
 pnt_tra = state->cur_tranche;

 if (state->nb_tranche == 0)
  return FALSE;

 Xc_TRACE(("cur_tranche: %d", pnt_tra));
 sltranche = state->tranche;
 if (state->delta_y < height)
 {
  for(i = 0; i < sltranche[pnt_tra].nb_arrete; i++)
  {
   sltranche[pnt_tra].arrete[i]->x_value += 
    sltranche[pnt_tra].arrete[i]->pente * state->delta_y;
  }
  height -= state->delta_y;
  if (++pnt_tra >= state->nb_tranche)
  {
   return FALSE;
  }
  state->delta_y = sltranche[pnt_tra].delta_y;
 }
 while(sltranche[pnt_tra].delta_y < height)
 {
  for(i = 0; i < sltranche[pnt_tra].nb_arrete; i++)
  {
   sltranche[pnt_tra].arrete[i]->x_value += 
    sltranche[pnt_tra].arrete[i]->pente * sltranche[pnt_tra].delta_y;
  }
  height -= sltranche[pnt_tra++].delta_y;
  if (pnt_tra >= state->nb_tranche)
  {
   return FALSE;
  }
  state->delta_y = sltranche[pnt_tra].delta_y;
 }
 for(i = 0; i < sltranche[pnt_tra].nb_arrete; i++)
 {
  sltranche[pnt_tra].arrete[i]->x_value += 
   sltranche[pnt_tra].arrete[i]->pente * height;
 }
 state->cur_tranche = pnt_tra;
 state->delta_y -= height;
 Xc_TRACE(("end_tranche: %d", pnt_tra));
 return TRUE;
}

static void Intersect_Segment(segmentA, nb_segmentA, segmentB, nb_segmentB)
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
  *segmentA = NULL;
 }
 else
  *segmentA = ret_seg;
}

static void Union_Segment(segmentA, nb_segmentA, segmentB, nb_segmentB)
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
 coord_t	minix, maxix, xmin1, xmax1, xmin2, xmax2;
 boolean	change, witch;
 
 Xc_HISTORY(("intersect_segments"));
  
 segm1 = *segmentA;
 segm2 = segmentB;
 ret_seg = segme = (sl_Segment *)Xc_malloc("semint", sizeof(sl_Segment) *
					   (*nb_segmentA + nb_segmentB));
  
 pnts1 = pnts2 = nb_seg = 0;
 change = TRUE; witch = FALSE;
 minix = maxix = xmin1 = xmax1 = COORD_ZERO;

 while(pnts1 < *nb_segmentA || pnts2 < nb_segmentB)
 {
  if (pnts1 == *nb_segmentA)
  {
   if (change)
   {
    segme->x_min = segm2[pnts2].x_min;
    segme->x_max = segm2[pnts2].x_max;
    segme++;
    nb_seg++;
   }
   else
   {
    segme->x_min = minix;
    segme->x_max = maxix;
    segme++;
    nb_seg++;
    change = TRUE;
   }
   pnts2++;
   continue;
  }
  if (pnts2 == nb_segmentB)
  {
   if (change)
   {
    segme->x_min = segm1[pnts1].x_min;
    segme->x_max = segm1[pnts1].x_max;
    segme++;
    nb_seg++;
   }
   else
   {
    segme->x_min = minix;
    segme->x_max = maxix;
    segme++;
    nb_seg++;
    change = TRUE;
   }
   pnts1++;
   continue;
  }
  xmin1 = segm1[pnts1].x_min;
  xmax1 = segm1[pnts1].x_max;
  xmin2 = segm2[pnts2].x_min;
  xmax2 = segm2[pnts2].x_max;

  if (change)
  {
   if (xmin1 < xmin2)
   {
    if (xmax1 + 1 < xmin2)
    {
     segme->x_min = segm1[pnts1].x_min;
     segme->x_max = segm1[pnts1].x_max;
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
     segme->x_min = segm2[pnts2].x_min;
     segme->x_max = segm2[pnts2].x_max;
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
     segme->x_min = minix;
     segme->x_max = maxix;
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
     segme->x_min = minix;
     segme->x_max = maxix;
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
  *segmentA = NULL;
 }
 else
  *segmentA = ret_seg;

}

static void Complement_Segment(This, ruler, np, tf_mode, segment, nb_seg)
c_ScanLine	*This;
c_TextRuler	*ruler;
boolean		np;
textflow_t	tf_mode;
sl_Segment	**segment;
int		*nb_seg;
{
 sl_Segment	*ret_seg, *segme, *seg;
 int		i, nseg;
 coord_t	xmin, xmax, x1, x2;
 coord_t	lmargin, rmargin;
 boolean	hole;
 
 seg = *segment;
 ret_seg = (sl_Segment *)Xc_malloc("seg", sizeof(sl_Segment) * (*nb_seg + 1));
 segme = ret_seg;

 xmin = This->page->left_margin + ruler->left_margin + 
  ( np ? ruler->paragraph_indent : COORD_ZERO);
 xmax = This->page->sWidth - This->page->right_margin - ruler->right_margin;
 
 lmargin = rmargin = SCALE_FROM_MILLIMETERS(5.0);

 hole = (tf_mode & XcTFM_IN_HOLE) != 0;
 tf_mode &= XcTFM_HOLE_MASK;
 i = nseg = 0;

 if (*nb_seg == 0)
 {
  ret_seg->x_min = xmin;
  ret_seg->x_max = xmax;
  Xc_free(*segment);
  *segment = ret_seg;
  *nb_seg = 1;
  return;
 }
 
 x2 = seg->x_min - lmargin;
 if (x2 > xmin)
 {
  if (tf_mode != XcTFM_RIGHT && tf_mode != XcTFM_SKIP)
  {
   segme->x_min = xmin;
   segme->x_max = MIN(x2, xmax);
   segme++; nseg++;
  }
 }
 
 do
 {
  x1 = seg->x_max + rmargin;
  
  if (x1 >= xmax)
   break;
  
  i++;
  if (i >= *nb_seg)		/* last segment */
  {
   if (tf_mode != XcTFM_LEFT && tf_mode != XcTFM_SKIP)
   {
    segme->x_min = MAX(x1, xmin);
    segme->x_max = xmax;
    segme++; nseg++;
   }
   break;
  }

  seg++;
  if (hole)
  {
   x2 = seg->x_min - lmargin;
   if (x2 < xmin) continue;
   if (x2 > x1)
   {
    segme->x_min = MAX(x1, xmin);
    segme->x_max = MIN(x2, xmax);
    segme++; nseg++;
   }
  }
  
 } while(1);
 
 Xc_free(*segment);
 *segment = ret_seg;
 *nb_seg = nseg;
}

#ifdef SC_DRAW
static void draw_segment(This, y, height, seg, nb_seg)
c_ScanLine *This;
coord_t y;
coord_t height;
sl_Segment *seg;
int nb_seg;
{
 Display *display;
 Window window;
 GC gc;
 XRectangle *rect;
 int i;

 if (nb_seg == 0) return;

 rect = (XRectangle *)Xc_malloc("xrect", sizeof(XRectangle) * nb_seg);

 for (i = 0; i < nb_seg; i++)
 {
  rect[i].x = seg[i].x_min / This->page->sScale;
  rect[i].width = seg[i].x_max / This->page->sScale - rect[i].x;
  rect[i].y = y / This->page->sScale;
  rect[i].height = height / This->page->sScale;
 }

 display = This->page->X_info.display;
 window = This->page->X_info.window;
 gc = XCreateGC(display, window, 0, NULL);
 XSetForeground(display, gc, GlobColormap->Cred);
 
 XDrawRectangles(display, window, gc, rect, nb_seg);
 XFlush(display);
 printf("press enter to continue\n");
 getchar();
 
 XFreeGC(display, gc);
 Xc_free(rect);
}
#endif
