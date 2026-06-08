/*
** Parser_boolean.c for XQuad in Parser/
** Method for Class Parser
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
** Started on  Thu Mar  2 16:32:40 1995 Antoine Buat
** Last update Fri Nov 29 19:19:13 1996 Antoine Buat
*/

#include "Parser.h"

/* ----------------------------------------------------------------- **
** faux() return false                                               **
** ----------------------------------------------------------------- */
int i_faux(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;

  Xc_TRACE(("internal faux"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;

  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  else
    index++;
  strcpy(strf+*indexf, "(0)");
  while(strf[*indexf])
    (*indexf)++;
  This->ret_type = TYPE_BOOLEAN;
  return index;
}

/* ----------------------------------------------------------------- **
** vrai() return true                                                **
** ----------------------------------------------------------------- */
int i_vrai(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;

  Xc_TRACE(("internal vrai"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;

  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  else
    index++;
  
  strcpy(strf+*indexf, "(1)");
  while(strf[*indexf])
    (*indexf)++;
  This->ret_type = TYPE_BOOLEAN;
  return index;
}

/* ----------------------------------------------------------------- **
** et(zone;const;...) return et logical                              **
** ----------------------------------------------------------------- */
int i_et(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	sortie;
  int	nindex;
  int	type;
  int	type2;
  int	curval;
  GEN	calc_x;
  int	old_indexf;
  int	lock;
  ulong	avma_old;

  Xc_TRACE(("internal et"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;

  lock=1;
  curval=1;
  sortie=0;
  old_indexf = *indexf;
  while (!sortie)
    {
      *indexf = old_indexf;
      nindex = F(This).srparse(This, string+index, strf, indexf,
			       &type, &type2);
      if (nindex < 1)
	return -1;
      index += nindex-1;

      if (string[index]==')')
	{
	  (*indexf)--;
	  strf[*indexf]=0;
	  sortie = 1;
	}

      if (type2 == TYPE_STRING)
	{
	  This->cell_s->status = TYPE_NO_PARAM_STRING;
	  return -1;
	}

      if (type == TYPE_CELL_ZONE)
	{
	  int	zone_x_s;
	  int	zone_y_s;
	  int	zone_x_e;
	  int	zone_y_e;
	  int	i;
	  c_col	*p_col;
	  c_line *p_line;

	  zone_x_s = 0;
	  i = 0;
	  while (strf[*indexf+i]!='-')
	    {
	      zone_x_s *= 10;
	      zone_x_s += strf[*indexf+i]-'0';
	      i++;
	    }
	  i++;
	  zone_y_s = 0;
	  while (strf[*indexf+i]!='-')
	    {
	      zone_y_s *= 10;
	      zone_y_s += strf[*indexf+i]-'0';
	      i++;
	    }
	  i++;
	  zone_x_e = 0;
	  while (strf[*indexf+i]!='-')
	    {
	      zone_x_e *= 10;
	      zone_x_e += strf[*indexf+i]-'0';
	      i++;
	    }
	  i++;
	  zone_y_e = 0;
	  while (strf[*indexf+i]!='-')
	    {
	      zone_y_e *= 10;
	      zone_y_e += strf[*indexf+i]-'0';
	      i++;
	    }

	  Xc_TRACE(("Mystifier les esprits: %d-%d:%d-%d",
		    zone_x_s, zone_y_s,
		    zone_x_e, zone_y_e));

	  p_col = This->motor->tableCL;
	  while(p_col && (p_col->number < zone_x_s))
	    {
	      p_col = p_col->Next;
	    }
	  while (p_col && (p_col->number <= zone_x_e))
	    {
	      p_line = p_col->line;
	      while(p_line && (p_line->number < zone_y_s))
		{
		  p_line = p_line->Next;
		}
	      while (p_line && (p_line->number <= zone_y_e))
		{
		  switch (This->d_type)
		    {
		    case ADD_DEP:
		      if (F(This->motor).check_c_depend(This->motor, 
							This->cell_s,
							p_col->number,
							p_line->number))
			{
			  Xc_TRACE(("Error circular dep"));
			  type = TYPE_CIRC_ERROR;
			  return -1;
			}
		      F(This->motor).add_depend(This->motor,
						This->cell_s,
						This->y, This->x,
						p_line->number,
						p_col->number);
		    case IGN_DEP:
		      /* PENSER A LA PRECISION DE CALCUL */
		      if ((p_line->cell.type!=Xq_VOID) && 
			  (p_line->cell.value2))
			{
			  lock = 0;
			  if (gcmp0(p_line->cell.value2))
			    curval &= 0;
			  else
			    curval &= 1;
			}
		      break;
		    case SUB_DEP:
		      F(This->motor).sub_depend(This->motor,
						This->cell_s,
						p_line->number,
						p_col->number);
		      break;
		    }
		  p_line = p_line->Next;
		}
	      p_col = p_col->Next;
	    }
	  index++;
	  if (string[index]==Xq_SEPAR)
	    {
	      index++;
	    }
	  if (string[index]==')')
	    {
	      sortie = 1;
	    }
	  continue;
	}

      if (string[index]==Xq_SEPAR)
	{
	  index++;
	}

      if (*indexf == old_indexf)
	{
	  curval = 0;
	}
      else
	{
	  GlobErrNum = 0;
	  avma_old = avma;
	  calc_x=lisexpr(strf+old_indexf);
	  switch (GlobErrNum)
	    {
	    case 0:
	      break;
	    case 260:
	      This->cell_s->status = TYPE_DIVZ_ERROR;
	      return -1;
	    default:
	      This->cell_s->status = TYPE_CALC_ERROR;
	      Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
	      return -1;
	    }
	  lock=0;
	  if (gcmp0(calc_x))
	    curval = 0;
	  else
	    curval &= 1;
	  avma = avma_old;
	}
    }
  if (lock)
    {
      /* #valeur! */
      This->cell_s->status = TYPE_CALC_ERROR;
      return -1;
    }
  sprintf(strf+old_indexf, "(%d.0)", curval);
  *indexf = old_indexf;
  while (strf[*indexf])
    (*indexf)++;
  This->ret_type = TYPE_BOOLEAN;
  return index+1;
}

/* ----------------------------------------------------------------- **
** not(zone|const|...) return not logical                            **
** ----------------------------------------------------------------- */
int i_not(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;
  GEN	calc_x;
  int	old_indexf;
  int	curval;
  ulong	avma_old;

  Xc_TRACE(("internal not"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;

  /* look for first parameter number */
  if (string[index]==')')
    {
      This->cell_s->status = TYPE_PARAM_LESS_ERROR;
      return -1;
    }

  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  if (end_type == TYPE_STRING)
    {
      This->cell_s->status = TYPE_NO_PARAM_NUM;
      return -1;
    }

  avma_old = avma;
  GlobErrNum = 0;
  calc_x=lisexpr(strf+old_indexf);
  switch (GlobErrNum)
    {
    case 0:
      break;
    case 260:
      This->cell_s->status = TYPE_DIVZ_ERROR;
      return -1;
	default:
      This->cell_s->status = TYPE_CALC_ERROR;
      Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
      return -1;
    }
  *indexf = old_indexf;
  if (gcmp0(calc_x))
    curval = 1;
  else
    curval = 0;
  avma = avma_old;
  index += nindex-1;

  if (string[index]==';')
    {
      This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      return -1;
    }
  sprintf(strf+old_indexf, "(%d.0)", curval);
  *indexf = old_indexf;
  while (strf[*indexf])
    (*indexf)++;
  This->ret_type = TYPE_BOOLEAN;
  return index+1;
}

/* ----------------------------------------------------------------- **
** or(zone;const;...) return or logical                            **
** ----------------------------------------------------------------- */
int i_or(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	sortie;
  int	nindex;
  int	type;
  int	type2;
  int	curval;
  GEN	calc_x;
  int	old_indexf;
  int	lock;
  ulong	avma_old;

  Xc_TRACE(("internal ou"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;

  lock=1;
  curval=0;
  sortie=0;
  old_indexf = *indexf;
  while (!sortie)
    {
      *indexf = old_indexf;
      nindex = F(This).srparse(This, string+index, strf, indexf,
			       &type, &type2);
      if (nindex < 1)
	return -1;
      index += nindex-1;

      if (string[index]==')')
	{
	  (*indexf)--;
	  strf[*indexf]=0;
	  sortie = 1;
	}

      if (type2 == TYPE_STRING)
	{
	  This->cell_s->status = TYPE_NO_PARAM_STRING;
	  return -1;
	}

      if (type == TYPE_CELL_ZONE)
	{
	  int	zone_x_s;
	  int	zone_y_s;
	  int	zone_x_e;
	  int	zone_y_e;
	  int	i;
	  c_col	*p_col;
	  c_line *p_line;

	  zone_x_s = 0;
	  i = 0;
	  while (strf[*indexf+i]!='-')
	    {
	      zone_x_s *= 10;
	      zone_x_s += strf[*indexf+i]-'0';
	      i++;
	    }
	  i++;
	  zone_y_s = 0;
	  while (strf[*indexf+i]!='-')
	    {
	      zone_y_s *= 10;
	      zone_y_s += strf[*indexf+i]-'0';
	      i++;
	    }
	  i++;
	  zone_x_e = 0;
	  while (strf[*indexf+i]!='-')
	    {
	      zone_x_e *= 10;
	      zone_x_e += strf[*indexf+i]-'0';
	      i++;
	    }
	  i++;
	  zone_y_e = 0;
	  while (strf[*indexf+i]!='-')
	    {
	      zone_y_e *= 10;
	      zone_y_e += strf[*indexf+i]-'0';
	      i++;
	    }

	  Xc_TRACE(("Mystifier les esprits: %d-%d:%d-%d",
		    zone_x_s, zone_y_s,
		    zone_x_e, zone_y_e));

	  p_col = This->motor->tableCL;
	  while(p_col && (p_col->number < zone_x_s))
	    {
	      p_col = p_col->Next;
	    }
	  while (p_col && (p_col->number <= zone_x_e))
	    {
	      p_line = p_col->line;
	      while(p_line && (p_line->number < zone_y_s))
		{
		  p_line = p_line->Next;
		}
	      while (p_line && (p_line->number <= zone_y_e))
		{
		  switch (This->d_type)
		    {
		    case ADD_DEP:
		      if (F(This->motor).check_c_depend(This->motor, 
							This->cell_s,
							p_col->number,
							p_line->number))
			{
			  Xc_TRACE(("Error circular dep"));
			  type = TYPE_CIRC_ERROR;
			  return -1;
			}
		      F(This->motor).add_depend(This->motor,
						This->cell_s,
						This->y, This->x,
						p_line->number,
						p_col->number);
		    case IGN_DEP:
		      /* PENSER A LA PRECISION DE CALCUL */
		      if ((p_line->cell.type!=Xq_VOID) && 
			  (p_line->cell.value2))
			{
			  lock = 0;
			  if (gcmp0(p_line->cell.value2))
			    curval |= 0;
			  else
			    curval |= 1;
			}
		      break;
		    case SUB_DEP:
		      F(This->motor).sub_depend(This->motor,
						This->cell_s,
						p_line->number,
						p_col->number);
		      break;
		    }
		  p_line = p_line->Next;
		}
	      p_col = p_col->Next;
	    }
	  index++;
	  if (string[index]==Xq_SEPAR)
	    {
	      index++;
	    }
	  if (string[index]==')')
	    {
	      sortie = 1;
	    }
	  continue;
	}

      if (string[index]==Xq_SEPAR)
	{
	  index++;
	}

      if (*indexf == old_indexf)
	{
	  curval |= 0;
	}
      else
	{
	  GlobErrNum = 0;
	  avma_old = avma;
	  calc_x=lisexpr(strf+old_indexf);
	  switch (GlobErrNum)
	    {
	    case 0:
	      break;
	    case 260:
	      This->cell_s->status = TYPE_DIVZ_ERROR;
	      return -1;
	    default:
	      This->cell_s->status = TYPE_CALC_ERROR;
	      Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
	      return -1;
	    }
	  lock=0;
	  if (gcmp0(calc_x))
	    curval |= 0;
	  else
	    curval |= 1;
	  avma = avma_old;
	}
    }
  if (lock)
    {
      /* #valeur! */
      This->cell_s->status = TYPE_CALC_ERROR;
      return -1;
    }
  sprintf(strf+old_indexf, "(%d.0)", curval);
  *indexf = old_indexf;
  while (strf[*indexf])
    (*indexf)++;
  This->ret_type = TYPE_BOOLEAN;
  return index+1;
}

/* ----------------------------------------------------------------- **
** if(condition; result_true; [result_false])                        **
** ----------------------------------------------------------------- */
int i_if(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;
  GEN	calc_x;
  int	old_indexf;
  int	curval;
  ulong	avma_old;

  Xc_TRACE(("internal if"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;

  /* look for first parameter number */
  if (string[index]==')')
    {
      This->cell_s->status = TYPE_PARAM_LESS_ERROR;
      return -1;
    }

  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  if (end_type == TYPE_STRING)
    {
      This->cell_s->status = TYPE_NO_PARAM_NUM;
      return -1;
    }

  avma_old = avma;
  GlobErrNum = 0;
  calc_x=lisexpr(strf+old_indexf);
  switch (GlobErrNum)
    {
    case 0:
      break;
    case 260:
      This->cell_s->status = TYPE_DIVZ_ERROR;
      return -1;
	default:
      This->cell_s->status = TYPE_CALC_ERROR;
      Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
      return -1;
    }
  *indexf = old_indexf;
  if (gcmp0(calc_x))
    curval = 0;
  else
    curval = 1;
  avma = avma_old;
  index += nindex-1;
  if (string[index]==';')
    {
      index++;
    }
  else
    {
      This->cell_s->status = TYPE_PARAM_LESS_ERROR;
      return -1;
    }

  /* look for second parameter */
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  if (curval)
    {
      if (end_type == TYPE_STRING)
	This->ret_type = TYPE_STRING;
      else
	This->ret_type = TYPE_NUM;
    }
  else
    *indexf = old_indexf;
  index += nindex-1;
  if (string[index]==';')
    {
      index++;
    }
  else
    {
      This->cell_s->status = TYPE_PARAM_LESS_ERROR;
      return -1;
    }

  /* look for second parameter */
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  if (!curval)
    {
      if (strf[*indexf-1] == ')')
	strf[*indexf-1] = 0;
      if (end_type == TYPE_STRING)
	This->ret_type = TYPE_STRING;
      else
	This->ret_type = TYPE_NUM;
    }
  else
    *indexf = old_indexf;
  index += nindex-1;

  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }

  return index+1;
}
