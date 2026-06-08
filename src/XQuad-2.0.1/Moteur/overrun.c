/*
** overrun.c for XQuad in Moteur/
** Overrun methods for Moteur class
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Thu Sep 28 13:58:56 1995 Stéphane Boisson
** Last update Tue Dec 10 15:30:31 1996 Emmanuel Paris
*/

#define NTRACE

#include "MoteurP.h"
#include "Feuille.h"

/*--- Local functions ---*/
static boolean adjustOverrunRegion ___PROTO((c_Moteur *this, int x, int y,
					     region_t *region));


/* ----------------------------------------------------------------- ** 
** adjustOverrunRegion - Adjust overrun region                       ** 
**                       Return TRUE if clipped                      ** 
** ----------------------------------------------------------------- */
static boolean adjustOverrunRegion(this, x, y, region)
c_Moteur *this;
int x;
int y;
region_t *region;
{
  boolean rc;
  cell *cp;
  int i;

  Xc_TRACE(("adjustOverrunRegion(%d; %d)", x, y));

  rc = FALSE;
  if(region->x_min == region->x_max)
    {
      /*--- Clip upper part ---*/
      for(i = y - 1; i >= region->y_min; --i)
	{
	  cp = F(this).get_cell(this, x, i);
	  if(cp->type != Xq_VOID)
	    {
	      Xc_TRACE(("clip up"));
	      
	      region->y_min = i + 1;
	      rc = TRUE;
	      break;
	    }
	}

      /*--- Clip bottom part ---*/
      for(i = y + 1; i <= region->y_max; ++i)
	{
	  cp = F(this).get_cell(this, x, i);
	  if(cp->type != Xq_VOID)
	    {
	      Xc_TRACE(("clip bottom"));

	      region->y_max = i - 1;
	      rc = TRUE;
	      break;
	    }
	}
    }
  else
    {
      Xc_ASSERT(region->y_min == region->y_max);

      /*--- Clip left part ---*/
      for(i = x - 1; i >= region->x_min; --i)
	{
	  cp = F(this).get_cell(this, i, y);
	  if(cp->type != Xq_VOID)
	    {
	      Xc_TRACE(("clip left"));
	      
	      region->x_min = i + 1;
	      rc = TRUE;
	      break;
	    }
	}

      /*--- Clip right part ---*/
      for(i = x + 1; i <= region->x_max; ++i)
	{
	  cp = F(this).get_cell(this, i, y);
	  if(cp->type != Xq_VOID)
	    {
	      Xc_TRACE(("clip right"));

	      region->x_max = i - 1;
	      rc = TRUE;
	      break;
	    }
	}
    }

  return rc;
}


/* ----------------------------------------------------------------- ** 
** moteurGetOverrunRegion - Get overrun region of cell               ** 
** ----------------------------------------------------------------- */
void moteurGetOverrunRegion(this, cellp, x, y, regionp)
c_Moteur *this;
cell *cellp;
int x;
int y;
region_t *regionp;
{
  bbox_t bbox;

  Xc_HISTORY(("getOverrunRegion(%d; %d)", x, y));

  regionp->x_min = regionp->x_max = x;
  regionp->y_min = regionp->y_max = y;
  F(this).getBoundingBox(this, regionp, &bbox);

  if(cellp->overrun_direction == OVERRUN_HORIZONTALY)
    {
      bbox.llx -= cellp->overrun_neg;
      bbox.urx += cellp->overrun_pos;
    }
  else
    {
      bbox.lly -= cellp->overrun_neg;
      bbox.ury += cellp->overrun_pos;
    }
  
  F(this).getCellByXY(this, bbox.llx, bbox.lly, &regionp->x_min,
		      &regionp->y_min, NULL);
  if(bbox.llx < 0) regionp->x_min = -1;
  if(bbox.lly < 0) regionp->y_min = -1;
  F(this).getCellByXY(this, bbox.urx, bbox.ury, &regionp->x_max,
		      &regionp->y_max, NULL);
  Xc_DUMP_REGION("overrun region", regionp);
}


/* ----------------------------------------------------------------- ** 
** moteurGetOverrunCells - Get overruning cells on region            ** 
** ----------------------------------------------------------------- */
moteur_overrun_t *moteurGetOverrunCells(this, region)
c_Moteur *this;
region_t *region;
{
  moteur_overrun_t *base, **ptr;
  region_t oregion;
  c_col *column;

  Xc_HISTORY(("getOverrunCells"));
  Xc_DUMP_REGION("region", region);

  base = NULL, ptr = &base;

  for(column = this->tableCL; column != NULL; column = column->Next)
    {
      c_line *line;

      for(line = column->line; line != NULL; line = line->Next)
	{
	  register int code;
	  boolean clip_flag;
	  cell *cp;

	  /*--- Bit 0: is in horizontal span ---*/
	  code = ((column->number >= region->x_min) &&
		  (column->number <= region->x_max))? 1 : 0;

	  /*--- Bit 1: is in vertical span ---*/
	  code += ((line->number >= region->y_min) &&
		   (line->number <= region->y_max))? 2 : 0;
	  Xc_TRACE(("(%d; %d) code=%d", column->number, line->number, code));
	  
	  /*--- Might be intersecting ---*/
	  if((line->cell.type != Xq_TEXT) || (code == 0)) continue;
	  
	  cp = &line->cell;
	  F(this).computeOverrun(this, cp, column->number, line->number,
				 SCALE_ONE);
	  if(!cp->overrun_pos && !cp->overrun_neg) continue;
	      
	  F(this).getOverrunRegion(this, cp, column->number, line->number,
				   &oregion);
	  
	  /*--- Horizontal intersection ---*/
	  if(oregion.x_max > region->x_max) oregion.x_max = region->x_max;
	  if(oregion.x_min < region->x_min) oregion.x_min = region->x_min;
	  if(oregion.x_min > oregion.x_max) continue;

	  /*--- Vertical intersection ---*/
	  if(oregion.y_max > region->y_max) oregion.y_max = region->y_max;
	  if(oregion.y_min < region->y_min) oregion.y_min = region->y_min;
	  if(oregion.y_min > oregion.y_max) continue;

	  /*--- Overrun region intersect ---*/
	  Xc_DUMP_REGION("i-region", &oregion);
	  clip_flag = adjustOverrunRegion(this, column->number, line->number,
					  &oregion);
	  Xc_DUMP_REGION("c-region", &oregion);
	  /*if(clip_flag &&*/
	  /*(oregion.x_max == oregion.x_min) &&*/
	  /*(oregion.y_max == oregion.y_min) &&*/
	  /*(oregion.x_min == column->number) &&*/
	  /*(oregion.y_min == line->number)) continue;*/

	  /*--- Alloc new one ---*/
	  *ptr = Xc_malloc("Overrun region", sizeof(moteur_overrun_t));
	  if(*ptr == NULL) continue;

	  /*--- Fill structure fields ---*/
	  (*ptr)->row = line->number;
	  (*ptr)->column = column->number;
	  (*ptr)->ptr = cp;
	  (*ptr)->code = code | (clip_flag? 4 : 0);
	  memcpy(&(*ptr)->region, &oregion, sizeof(region_t));
	  Xc_TRACE(("new(%d; %d) code=%d", (*ptr)->column, (*ptr)->row,
		    (*ptr)->code));

	  ptr = &(*ptr)->next;
	  *ptr = NULL;
	}
    }

  return base;
}


/* ----------------------------------------------------------------- ** 
** moteurFreeOverrunCells - Release memory                           ** 
** ----------------------------------------------------------------- */
void moteurFreeOverrunCells(this, ptr)
c_Moteur *this;
moteur_overrun_t *ptr;
{
  moteur_overrun_t *tmp;

  Xc_HISTORY(("freeOverrunCells"));

  while(ptr != NULL)
    {
      tmp = ptr;
      ptr = ptr->next;
      Xc_free(tmp);
    }

  Xc_TRACE(("done"));
}

/* ----------------------------------------------------------------- ** 
** moteurComputeOverrun - Compute cell overrun values                ** 
** ----------------------------------------------------------------- */
void moteurComputeOverrun(This, cell_s, x, y, factor)
c_Moteur	*This;
cell 		*cell_s;
int		x;
int		y;
scale_t		factor;
{
  CellStyle_t	*cstyle;
  c_CellStyle	*cbase;
  coord_t	cell_size;
  
  /*Xc_HISTORY(("computeOverrun"));*/

  if(!cell_s) cell_s = F(This).get_cell(This, x, y);
  cstyle = F(This).get_cell_style(This, x, y);
  cbase = This->BaseStd->cell_style_base;
  cell_s->overrun_cstyle = cstyle;
  cell_s->overrun_direction = 
    F(cbase).CalculTextOverrun(cbase, 
			       cell_s->text + (cell_s->text[0] == '\''),
			       cstyle, 
			       &cell_s->overrun_pos, 
			       &cell_s->overrun_neg, factor);
  if (cell_s->overrun_direction == OVERRUN_HORIZONTALY)
    cell_size = F(This).get_size(This, Xc_COLUMN, x) * factor;
  else
    cell_size = F(This).get_size(This, Xc_ROW, y) * factor;
  cell_s->overrun_size = cell_size;
  if (cell_s->overrun_pos && cell_s->overrun_neg)  /* if align center */
    cell_size = cell_size >> 1;
  cell_s->overrun_pos = MAX(COORD_ZERO, cell_s->overrun_pos - cell_size);
  cell_s->overrun_neg = MAX(COORD_ZERO, cell_s->overrun_neg - cell_size);
  Xc_TRACE(("Calcul Overrun for cell %c%d: text: %s direction: %s over_pos: %d over_neg: %d", x+'A', y+1, cell_s->text, cell_s->overrun_direction?"horizontal":"vertical", cell_s->overrun_pos, cell_s->overrun_neg));
}

#define OVERRUN_BLOCK_QUIT { \
			       *pcell_s = NULL; \
			       return TRUE; \
			   }


/* ----------------------------------------------------------------- ** 
** moteurTestOverrun - Test if cell is overrunning                   ** 
** ----------------------------------------------------------------- */
boolean moteurTestOverrun(This, pcell_s, x, y, direct, sign, factor)
c_Moteur	*This;
cell 		**pcell_s;
int		x;
int		y;
boolean		direct;
boolean		sign;
scale_t		factor;
{
  cell	*cell_s;
  CellStyle_t	*cstyle;
  coord_t	cell_size;
  
  /*Xc_HISTORY(("testOverrun"));*/

  if (!(*pcell_s))
    *pcell_s = cell_s = F(This).get_cell(This, x, y);    
  else
    cell_s = *pcell_s;
  
  switch(cell_s->type)
    {
    case Xq_VOID:
      return FALSE;
    case Xq_TEXT:
      cstyle = F(This).get_cell_style(This, x, y);
      if (cell_s->overrun_direction == OVERRUN_HORIZONTALY)
	cell_size = F(This).get_size(This, Xc_COLUMN, x) * factor;
      else
	cell_size = F(This).get_size(This, Xc_ROW, y) * factor;
      
      if (cstyle != cell_s->overrun_cstyle || 
	  factor != cell_s->overrun_cstyle->factor ||
	  cell_size != cell_s->overrun_size)
	F(This).computeOverrun(This, cell_s, x, y, factor);
      
      if (cell_s->overrun_direction == direct)
	{
	  if (sign)
	    {
	      if (cell_s->overrun_pos)
		return TRUE;
	      else
		OVERRUN_BLOCK_QUIT;
	    }
	  else
	    {
	      if (cell_s->overrun_neg)
		return TRUE;
	      else
		OVERRUN_BLOCK_QUIT;
	    }
	}
      else
	OVERRUN_BLOCK_QUIT;
      break;
      
    default:
      OVERRUN_BLOCK_QUIT;
    }
}     

/* ----------------------------------------------------------------- ** 
** moteurTestCellOverrun - Test if cell is overrunning               ** 
** ----------------------------------------------------------------- */
boolean moteurTestCellOverrun(This, cellule, cstyle,x,y, direct, sign, factor)
c_Moteur	*This;
cell 		*cellule;
CellStyle_t	*cstyle;
int		x;
int		y;
boolean		direct;
boolean		sign;
scale_t		factor;
{
  coord_t	cell_size;
  
  /*Xc_HISTORY(("testCellOverrun"));*/
  
  switch(cellule->type)
    {
    case Xq_VOID:
      return FALSE;
    case Xq_TEXT:
      if (cellule->overrun_direction == OVERRUN_HORIZONTALY)
	cell_size = F(This).get_size(This, Xc_COLUMN, x) * factor;
      else
	cell_size = F(This).get_size(This, Xc_ROW, y) * factor;
      
      if (cstyle != cellule->overrun_cstyle || 
	  factor != cellule->overrun_cstyle->factor ||
	  cell_size != cellule->overrun_size)
	F(This).computeOverrun(This, cellule, x, y, factor);
      
      if (cellule->overrun_direction == direct)
	{
	  if (sign)
	    {
	      if (cellule->overrun_pos)
		return TRUE;
	      return FALSE;
	    }
	  else
	    {
	      if (cellule->overrun_neg)
		return TRUE;
	      return FALSE;
	    }
	}
      return FALSE;
      break;
      
    default:
      return FALSE;
    }
}     

extern cell voidcell;

/* -------------------------------------------------------------------- ** 
** moteurFillCellInfo - Optimization for fill Feuille's cell_info array ** 
** -------------------------------------------------------------------- */
void moteurFillCellInfo(This, cell_info, region, xo, yo)
c_Moteur *This;
cell_info_t **cell_info;
region_t  *region;
{
  int i, io, id, j, jo, jd, count;
  int mx, my, Mx, My, tmp;
  
  c_col			*tmpccol;
  c_line		*tmpcline;
  cell_style_list	*list_tmp;
  CellStyle_t		*def_cstyle;
  
  mx = region->x_min; Mx = region->x_max;
  my = region->y_min; My = region->y_max;
  xo = mx - xo; yo = my - yo;
  
  def_cstyle = This->BaseStd->cell_style_base->default_cstyle;
  for(i = xo, io = mx; io < Mx; i++, io++)
    for(j = yo, jo = my; jo < My; j++, jo++)
    {
      cell_info[i][j].cellule = &voidcell;
      cell_info[i][j].cstyle = def_cstyle;
    }
  
  tmpccol = This->tableCL;
  io = mx;
  while(io < Mx)
  {
    while (tmpccol && (tmpccol->number < io))
      tmpccol = tmpccol->Next;
    
    if (tmpccol && tmpccol->number < Mx)
    {
      io = tmpccol->number;
      tmpcline = tmpccol->line;
      
      jo = my;
      while(jo < My)
      {
	while(tmpcline && (tmpcline->number < jo))
	  tmpcline = tmpcline->Next;
	
	if (tmpcline && tmpcline->number < My)
	{
	  jo = tmpcline->number;
	  cell_info[io - mx + xo][jo - my + yo].cellule = &tmpcline->cell;
	  jo++;
	}
	else
	  jo = My;
      }
      io++;
    }
    else
      io = Mx;
  }
  
  count = (Mx - mx) * (My - my);
  Mx--; My--;
  list_tmp = This->style_list;
  while(list_tmp)
  {
    if (list_tmp->region.x_max >= mx && list_tmp->region.x_min <= Mx &&
	list_tmp->region.y_max >= my && list_tmp->region.y_min <= My)
    {
      io = MAX(list_tmp->region.x_min, mx);
      id = MIN(list_tmp->region.x_max, Mx);
      jo = MAX(list_tmp->region.y_min, my);
      jd = MIN(list_tmp->region.y_max, My);
      
      for(i = io - mx; io <= id; i++, io++)
	for(j = jo - my, tmp = jo; tmp <= jd; j++, tmp++, count--)
	  cell_info[i + xo][j + yo].cstyle= list_tmp->cstyle;
      
      if (count <= 0)
	break;
    }
    list_tmp = list_tmp->Next;
  }
  
  /*  
     for(i = 0, io = mx; io < Mx; i++, io++)
     {
     for(j = 0, jo = my; jo < My; j++, jo++)
     {
     cell_info[i][j].cellule = F(This).get_cell(This, io, jo); 
     cell_info[i][j].cstyle= F(This).get_cell_style(This, io, jo);
     }
     }
     */
}

