/*
** Unmap_Map_ScrollBar.c for Xclamation in Page/
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
** Started on  Mon Jul 17 19:50:52 1995 Emmanuel Paris
** Last update Thu Jun 20 21:30:17 1996 Emmanuel Paris
*/

#include "Page.h"

void Map_ScrollBarH(This)
c_Page	*This;
{
  Xc_TRACE(("Map ScrollBar Horizontal"));
  XMapRaised(This->X_info.display, This->ScrollBarH->window);
  F(This->RegleV).Resize(This->RegleV, 0, REGLE_SIZE + 1,
			 This->RegleV->Size - SCROLLB_SIZE);  
  if (This->ScrollBarV_actif)
  {
    if (This->ScrollBarV->PosY)
    {
      F(This->ScrollBarV).Resize(This->ScrollBarV, 
				 This->RegleH->Size 
				 + REGLE_SIZE * This->Regle_actif + 1, 
				 REGLE_SIZE,
				 This->RegleV->Size + 1,
				 This->RegleV->Size 
				 + SCROLLB_SIZE + PAGER_SIZE + 1, 1,
				 This->Orig_X, This->Orig_Y, TRUE);
    }
    else
    {
      F(This->ScrollBarV).Resize(This->ScrollBarV, 
				 This->RegleH->Size 
				 + REGLE_SIZE * This->Regle_actif + 1, 0,
				 This->RegleV->Size + 1,
				 This->RegleV->Size 
				 + REGLE_SIZE * This->Regle_actif 
				 + SCROLLB_SIZE + PAGER_SIZE + 1, 1,
				 This->Orig_X, This->Orig_Y, TRUE);      
    }
  }
  F(This->Pager).Resize(This->Pager, 0, This->RegleV->Size + SCROLLB_SIZE
			+ REGLE_SIZE * This->Regle_actif + 1,
			This->Pager->Size - 
			SCROLLB_SIZE * This->ScrollBarV_actif);
}

void Unmap_ScrollBarH(This)
     c_Page	*This;
{
    Xc_TRACE(("Unmap ScrollBar Horizontal"));
    XUnmapWindow(This->X_info.display, This->ScrollBarH->window);
    F(This->RegleV).Resize(This->RegleV, 0, REGLE_SIZE + 1,
			   This->RegleV->Size + SCROLLB_SIZE);
    if (This->ScrollBarV_actif)
    {
      if (This->ScrollBarV->PosY)
      {
	F(This->ScrollBarV).Resize(This->ScrollBarV, 
				   This->RegleH->Size 
				   + REGLE_SIZE * This->Regle_actif + 1, 
				   REGLE_SIZE, This->RegleV->Size + 1,
				   This->RegleV->Size + 1, 0,
				   This->Orig_X, This->Orig_Y ,TRUE);
      }
      else
      {
	F(This->ScrollBarV).Resize(This->ScrollBarV, 
				   This->RegleH->Size 
				   + REGLE_SIZE * This->Regle_actif + 1, 0,
				   This->RegleV->Size + 1,
				   This->RegleV->Size 
				   + REGLE_SIZE * This->Regle_actif + 1, 0,
				   This->Orig_X, This->Orig_Y ,TRUE);
      }
    }				 
    F(This->Pager).Resize(This->Pager, 0, This->RegleV->Size
			  + REGLE_SIZE * This->Regle_actif + 1,
			  This->Pager->Size + 
			  SCROLLB_SIZE * This->ScrollBarV_actif);
}

void Map_ScrollBarV(This)
     c_Page	*This;
{
  Xc_TRACE(("Map ScrollBar Vertical"));
  XMapWindow(This->X_info.display, This->ScrollBarV->window);
  if (This->ScrollBarH_actif)
  {
    XMapRaised(This->X_info.display, This->ScrollBarH->window);
  }
  F(This->RegleH).Resize(This->RegleH, REGLE_SIZE + 1, 0,
			 This->RegleH->Size - SCROLLB_SIZE); 
  F(This->Pager).Resize(This->Pager, 0, This->RegleV->Size 
			+ REGLE_SIZE * This->Regle_actif
			+ SCROLLB_SIZE * This->ScrollBarH_actif + 1,
			This->Pager->Size 
			- SCROLLB_SIZE * This->ScrollBarH_actif );
}

void Unmap_ScrollBarV(This)
     c_Page	*This;
{
  Xc_TRACE(("Unmap ScrollBar Vertical"));
  XUnmapWindow(This->X_info.display, This->ScrollBarV->window);
  F(This->RegleH).Resize(This->RegleH, REGLE_SIZE + 1, 0,
			 This->RegleH->Size + SCROLLB_SIZE);
  F(This->Pager).Resize(This->Pager, 0, This->RegleV->Size
			+ REGLE_SIZE * This->Regle_actif
			+ SCROLLB_SIZE * This->ScrollBarH_actif + 1,
			This->Pager->Size
			+ SCROLLB_SIZE * This->ScrollBarH_actif );
}











