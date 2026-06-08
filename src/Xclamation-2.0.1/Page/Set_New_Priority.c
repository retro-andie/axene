/*
** Set_New_Priority.c for Xclamation in Page/
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
** Started on  Mon Jul 17 19:52:25 1995 Emmanuel Paris
** Last update Sun Oct 27 17:44:13 1996 Emmanuel Paris
*/

#include "Page.h"

void set_priority_Cadre();            
int get_priority_Cadre();
void lower_priority();
void higher_priority();
void lowest_priority();
void highest_priority();
int get_highest_priority();
int get_lowest_priority();

void set_lower_priority(cpage)
     c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Cadre	*svglcadre;
  
  lcadre=(l_Cadre *)sort_asc_list_cadre(cpage->SelectedCadre);
  svglcadre=lcadre;
  while(lcadre)
  {
    lcadre->priority=F(cpage).get_priority_cadre(cpage,lcadre);
    F(cpage).lower_priority(cpage,lcadre);
    lcadre=lcadre->NextCadre;
  }
  free_list_cadre(svglcadre,NO_DEL_CADRE);
}

void set_higher_priority(cpage)
     c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Cadre	*svglcadre;
  
  lcadre=(l_Cadre *)sort_desc_list_cadre(cpage->SelectedCadre);
  svglcadre=lcadre;
  while(lcadre)
  {
    lcadre->priority=F(cpage).get_priority_cadre(cpage,lcadre);
    F(cpage).higher_priority(cpage,lcadre);
    lcadre=lcadre->NextCadre;
  }
  Xc_TRACE(("set higher function end"));
  free_list_cadre(svglcadre,NO_DEL_CADRE);
}

void set_lowest_priority(cpage)
     c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Cadre	*svglcadre;
  
  lcadre=(l_Cadre *)sort_desc_list_cadre(cpage->SelectedCadre);
  svglcadre=lcadre;
  while(lcadre)
  {
    lcadre->priority=F(cpage).get_priority_cadre(cpage,lcadre);
    F(cpage).lowest_priority(cpage,lcadre);
    lcadre=lcadre->NextCadre;
  }
  free_list_cadre(svglcadre,NO_DEL_CADRE);
}

void set_highest_priority(cpage)
     c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Cadre	*svglcadre;
  
  lcadre=(l_Cadre *)sort_asc_list_cadre(cpage->SelectedCadre);
  svglcadre=lcadre;
  while(lcadre)
  {
    lcadre->priority=F(cpage).get_priority_cadre(cpage,lcadre);
    F(cpage).highest_priority(cpage,lcadre);
    lcadre=lcadre->NextCadre;
  }
  free_list_cadre(svglcadre,NO_DEL_CADRE);
}

/* --------------------------------------------------------------------- */
/* -----------------  functions on frame priority  --------------------- */
/* --------------------------------------------------------------------- */

void set_priority_Cadre(This,lcadre,priority)
c_Page *This;
l_Cadre	*lcadre;
int priority;
{
  l_Cadre	*tmplcadre;
  
  tmplcadre=This->ListCadre;
  while(tmplcadre->cadre!=lcadre->cadre && tmplcadre)
  {
    tmplcadre=tmplcadre->NextCadre;
  }
  if (tmplcadre)
    tmplcadre->priority=priority;
  tmplcadre=This->SelectedCadre;
  while(tmplcadre && tmplcadre->cadre!=lcadre->cadre)
  {
    tmplcadre=tmplcadre->NextCadre;
  }
  if (tmplcadre)
    tmplcadre->priority=priority;
}

int get_priority_Cadre(This,lcadre)
c_Page *This;
l_Cadre	*lcadre;
{
  l_Cadre	*tmplcadre;
  
  tmplcadre=This->ListCadre;
  while(tmplcadre->cadre!=lcadre->cadre && tmplcadre)
  {
    tmplcadre=tmplcadre->NextCadre;
  }
  if (tmplcadre)
    return tmplcadre->priority;
  return 0;
}

void lower_priority(This,lcadre)
c_Page *This;
l_Cadre *lcadre;
{
  l_Cadre	*lowlist;
  l_Cadre	*highlist;
  l_Cadre	*tmplist;
  l_Cadre	*tmplist2;
  l_Cadre	*transp;
  l_Cadre	*lowcadre;
  l_Cadre	*lowercadre;
  l_Cadre	*tmpcadre;
  Polygone	*forme;
  Polygone	*ftransp;
  Polygone	*oldforme = NULL;
  int		priority;
  int		priortmp;
  int		priorswp;
  
  list_cadre_low_high_priority(This, lcadre, &lowlist, &highlist,INTERS_CADRE);
  lowcadre = sort_desc_list_cadre(lowlist);
  if (!lowcadre)
  {
    free_list_cadre(lowcadre, NO_DEL_CADRE);
    free_list_cadre(lowlist, NO_DEL_CADRE);
    free_list_cadre(highlist, NO_DEL_CADRE);
    return;
  }
  
  F(This).undisplay_selected_lcadre(This);
  forme = IntersectPolygone(lcadre->cadre->Forme, lowcadre->cadre->Forme);
  tmplist = highlist;
  transp = NULL;
  while(tmplist)
  {
    if (tmplist->cadre->inside_color->transparency)
      add_cadre_in_list(&transp, tmplist);
    else
      SubtractPolygone(&forme, tmplist->cadre->Forme);
    tmplist=tmplist->NextCadre;
  }
  if (transp)
    oldforme = DuplicatePolygone(lcadre->cadre->Forme);
  F(lowcadre->cadre).drawcadre(lowcadre->cadre, forme);
  F(lowcadre->cadre).drawcontour(lowcadre->cadre, forme,
				 This->Frame_Border_actif);
  FreePolygone(forme, DEL_POLYGONE);
  free_list_cadre(lowlist, NO_DEL_CADRE);
  free_list_cadre(highlist, NO_DEL_CADRE);
  
  tmplist = transp;
  while(tmplist)
  {
    list_cadre_low_high_priority(This, tmplist, &lowlist,
				 &highlist, INTERS_CADRE);
    ftransp = IntersectPolygone(tmplist->cadre->Forme, oldforme);
    tmplist2 = highlist;
    while(tmplist2)
    {
      if (!tmplist2->cadre->inside_color->transparency)
	SubtractPolygone(&ftransp, tmplist2->cadre->Forme);
      tmplist2 = tmplist2->NextCadre;
    }
    F(tmplist->cadre).drawcadre(tmplist->cadre, ftransp);
    F(tmplist->cadre).drawcontour(tmplist->cadre, ftransp,
				  This->Frame_Border_actif);
    
    free_list_cadre(lowlist, NO_DEL_CADRE);
    free_list_cadre(highlist, NO_DEL_CADRE);
    FreePolygone(ftransp, DEL_POLYGONE);
    tmplist = tmplist->NextCadre;
  }
  if (transp)
  {
    FreePolygone(oldforme, DEL_POLYGONE);
    free_list_cadre(transp, NO_DEL_CADRE);
  }
  
  F(This).display_selected_lcadre(This);

  list_cadre_low_high_priority(This, lcadre, &lowlist, &highlist, ALL_CADRE);
  lowercadre = sort_desc_list_cadre(lowlist);
  priority = lowcadre->priority;
  tmpcadre = lowercadre;
  priortmp = lcadre->priority;
  while(!Xstrcmp(tmpcadre->name, lowcadre->name))
  {
    priorswp = tmpcadre->priority;
    F(This).set_priority_cadre(This, tmpcadre, priortmp);
    priortmp = priorswp;
    tmpcadre = tmpcadre->NextCadre;
  }
  F(This).set_priority_cadre(This, lowcadre, priortmp);
  F(This).set_priority_cadre(This, lcadre, priority);
  free_list_cadre(lowcadre,NO_DEL_CADRE);
  free_list_cadre(lowercadre,NO_DEL_CADRE);
  free_list_cadre(lowlist, NO_DEL_CADRE);
  free_list_cadre(highlist, NO_DEL_CADRE);
}

void higher_priority(This,lcadre)
c_Page *This;
l_Cadre *lcadre;
{
  l_Cadre	*lowlist;
  l_Cadre	*highlist;
  l_Cadre	*tmplist;
  l_Cadre	*tmplist2;
  l_Cadre	*highcadre;
  l_Cadre	*highercadre;
  l_Cadre	*tmpcadre;
  l_Cadre	*transp;
  Polygone	*forme;
  Polygone	*ftransp;
  Polygone	*oldforme = NULL;
  int		priority;
  int		priortmp;
  int		priorswp;
  
  list_cadre_low_high_priority(This, lcadre, &lowlist, &highlist,INTERS_CADRE);
  highcadre = sort_asc_list_cadre(highlist);
  if (!highcadre)
  {
    free_list_cadre(highcadre,NO_DEL_CADRE);
    free_list_cadre(lowlist,NO_DEL_CADRE);
    free_list_cadre(highlist,NO_DEL_CADRE);
    return;
  }
  
  F(This).undisplay_selected_lcadre(This);
  forme = IntersectPolygone(lcadre->cadre->Forme, highcadre->cadre->Forme);
  transp = NULL;
  tmplist = highcadre->NextCadre;
  while(tmplist)
  {
    if (tmplist->cadre->inside_color->transparency)
      add_cadre_in_list(&transp, tmplist);
    else
      SubtractPolygone(&forme, tmplist->cadre->Forme);
    tmplist=tmplist->NextCadre;
  }
  if (transp)
    oldforme = DuplicatePolygone(lcadre->cadre->Forme);
  
  F(lcadre->cadre).drawcadre(lcadre->cadre, forme);
  F(lcadre->cadre).drawcontour(lcadre->cadre, forme,
			       This->Frame_Border_actif);
  FreePolygone(forme, DEL_POLYGONE);
  
  free_list_cadre(lowlist, NO_DEL_CADRE);
  free_list_cadre(highlist, NO_DEL_CADRE);
  
  list_cadre_low_high_priority(This, lcadre, &lowlist, &highlist, ALL_CADRE);
  highercadre = sort_asc_list_cadre(highlist);
  priority = highcadre->priority;
  tmpcadre = highercadre;
  priortmp = lcadre->priority;
  while(tmpcadre->cadre != highcadre->cadre)
  {
    priorswp = tmpcadre->priority;
    F(This).set_priority_cadre(This, tmpcadre, priortmp);
    priortmp = priorswp;
    tmpcadre = tmpcadre->NextCadre;
  }
  F(This).set_priority_cadre(This, highcadre, priortmp);
  F(This).set_priority_cadre(This, lcadre, priority);
  
  free_list_cadre(highercadre, NO_DEL_CADRE);
  free_list_cadre(highcadre, NO_DEL_CADRE);
  free_list_cadre(lowlist, NO_DEL_CADRE);
  free_list_cadre(highlist, NO_DEL_CADRE);
  
  tmplist = transp;
  while(tmplist)
  {
    list_cadre_low_high_priority(This, tmplist, &lowlist,
				 &highlist, INTERS_CADRE);
    ftransp = IntersectPolygone(tmplist->cadre->Forme, oldforme);
    tmplist2 = highlist;
    while(tmplist2)
    {
      if (!tmplist2->cadre->inside_color->transparency)
	SubtractPolygone(&ftransp, tmplist2->cadre->Forme);
      tmplist2 = tmplist2->NextCadre;
    }
    F(tmplist->cadre).drawcadre(tmplist->cadre, ftransp);
    F(tmplist->cadre).drawcontour(tmplist->cadre, ftransp,
				  This->Frame_Border_actif);
    
    free_list_cadre(lowlist, NO_DEL_CADRE);
    free_list_cadre(highlist, NO_DEL_CADRE);
    FreePolygone(ftransp, DEL_POLYGONE);
    tmplist = tmplist->NextCadre;
  }
  if (transp)
  {
    FreePolygone(oldforme, DEL_POLYGONE);
    free_list_cadre(transp, NO_DEL_CADRE);
  }
  
  F(This).display_selected_lcadre(This);
}

void lowest_priority(This, lcadre)
c_Page *This;
l_Cadre *lcadre;
{
  l_Cadre	*lowlist;
  l_Cadre	*highlist;
  l_Cadre	*tmplist;
  l_Cadre	*tmplist2;
  l_Cadre	*pivolist;
  l_Cadre	*transp;
  Polygone	*forme;
  Polygone	*total;
  Polygone	*ftransp;
  Polygone	*oldforme = NULL;
  int		min_prior;

  min_prior = get_lowest_priority(This);
  if (lcadre->priority == min_prior+1)
    return;
  
  F(This).undisplay_selected_lcadre(This);
  list_cadre_low_high_priority(This, lcadre, &lowlist, &highlist,INTERS_CADRE);
  tmplist = highlist;
  transp = NULL;
  total = DuplicatePolygone(lcadre->cadre->Forme);
  while(tmplist)
  {
    if (tmplist->cadre->inside_color->transparency)
      add_cadre_in_list(&transp, tmplist);
    else
      SubtractPolygone(&total, tmplist->cadre->Forme);
    tmplist = tmplist->NextCadre;
  }
  if (transp)
    oldforme = DuplicatePolygone(lcadre->cadre->Forme);
  
  tmplist = sort_asc_list_cadre(lowlist);
  free_list_cadre(lowlist, NO_DEL_CADRE);
  lowlist = tmplist;
  
  while(tmplist)
  {
    forme = IntersectPolygone(total, tmplist->cadre->Forme);
    pivolist = tmplist->NextCadre;
    while(pivolist)
    {
      if (!pivolist->cadre->inside_color->transparency)
	SubtractPolygone(&forme, pivolist->cadre->Forme);
      pivolist = pivolist->NextCadre;
    }
    F(tmplist->cadre).drawcadre(tmplist->cadre, forme);
    F(tmplist->cadre).drawcontour(tmplist->cadre, forme,
				  This->Frame_Border_actif);
    FreePolygone(forme, DEL_POLYGONE);
    tmplist = tmplist->NextCadre;
  }
  FreePolygone(total, DEL_POLYGONE);
  free_list_cadre(lowlist, NO_DEL_CADRE);
  free_list_cadre(highlist, NO_DEL_CADRE);
  F(This).set_priority_cadre(This, lcadre, min_prior);
  
  tmplist = transp;
  while(tmplist)
  {
    list_cadre_low_high_priority(This, tmplist, &lowlist,
				 &highlist, INTERS_CADRE);
    ftransp = IntersectPolygone(tmplist->cadre->Forme, oldforme);
    tmplist2 = highlist;
    while(tmplist2)
    {
      if (!tmplist2->cadre->inside_color->transparency)
	SubtractPolygone(&ftransp, tmplist2->cadre->Forme);
      tmplist2 = tmplist2->NextCadre;
    }
    F(tmplist->cadre).drawcadre(tmplist->cadre, ftransp);
    F(tmplist->cadre).drawcontour(tmplist->cadre, ftransp,
				  This->Frame_Border_actif);
    
    free_list_cadre(lowlist, NO_DEL_CADRE);
    free_list_cadre(highlist, NO_DEL_CADRE);
    FreePolygone(ftransp, DEL_POLYGONE);
    tmplist = tmplist->NextCadre;
  }
  if (transp)
  {
    FreePolygone(oldforme, DEL_POLYGONE);
    free_list_cadre(transp, NO_DEL_CADRE);
  }
  
  F(This).display_selected_lcadre(This);
}

void highest_priority(This,lcadre)
c_Page *This;
l_Cadre *lcadre;
{
  l_Cadre *lowlist;
  l_Cadre *highlist;
  l_Cadre *tmplist;
  int max_prior;
  Polygone *forme;
  Polygone *total;

  max_prior=get_highest_priority(This);
  if (lcadre->priority!=max_prior-1)
  { 
    F(This).undisplay_selected_lcadre(This);
    list_cadre_low_high_priority(This,lcadre,&lowlist,&highlist,INTERS_CADRE);
    total = CreateNullPolygone();
    tmplist = highlist;
    while(tmplist)
    {
      forme=IntersectPolygone(lcadre->cadre->Forme,tmplist->cadre->Forme);
      UnionPolygone(&total,forme);
      FreePolygone(forme,DEL_POLYGONE);
      tmplist=tmplist->NextCadre;
    }
    F(lcadre->cadre).drawcadre(lcadre->cadre,total);
    F(lcadre->cadre).drawcontour(lcadre->cadre,total,
				    This->Frame_Border_actif);
    FreePolygone(total,DEL_POLYGONE);
    free_list_cadre(lowlist,NO_DEL_CADRE);
    free_list_cadre(highlist,NO_DEL_CADRE);
    F(This).set_priority_cadre(This,lcadre,max_prior);
    
    F(This).display_selected_lcadre(This);
  }
}

int get_highest_priority(This)
c_Page *This;
{
  l_Cadre *list_cadre;
  int max_prior;
  
  /*Xc_TRACE(("get highest priority "));*/
  list_cadre=This->ListCadre;
  if (!list_cadre)
    return 0;
  max_prior=list_cadre->priority;
  while(list_cadre)
    {
      if (list_cadre->priority>max_prior)
	max_prior=list_cadre->priority;
      list_cadre=list_cadre->NextCadre;
    }
  return max_prior+1;
}

int get_lowest_priority(This)
c_Page *This;
{
  l_Cadre *list_cadre;
  int min_prior;
  
/*  Xc_TRACE(("get lowest priority "));*/
  list_cadre=This->ListCadre;
  if (!list_cadre)
    return 0;
  min_prior=list_cadre->priority;
  while(list_cadre)
    {
      if (list_cadre->priority<min_prior)
	min_prior=list_cadre->priority;
      list_cadre=list_cadre->NextCadre;
    }
  return min_prior-1;
}
