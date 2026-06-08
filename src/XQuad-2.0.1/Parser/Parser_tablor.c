/*
** Parser_tablor.c for XQuad in Parser/
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
** Last update Thu Nov 28 12:07:22 1996 Antoine Buat
*/

#include "Parser.h"

/* ----------------------------------------------------------------- **
** adresse(line; colone; abs_rel; a1orlc; feuille) return adresse    **
** ----------------------------------------------------------------- */
int i_adresse(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;
  int	old_indexf;
  GEN	calc_x;
  ulong	avma_old;
  int	num;
  long	line = 0;
  long	column;
  long	ref;
  long	isa1;

  Xc_TRACE(("internal adresse"));
  index = 0;
  while (string[index]==' ')
    index++;
  if (string[index] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index++;

  /* look for first parameter number */
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  if (end_type == TYPE_STRING)
    {
      This->cell_s->status = TYPE_NO_PARAM_NUM;
      return -1;
    }
  
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
  line = gtolong(calc_x);
  avma = avma_old;
  index += nindex-1;
  if ((line <= 0) || (line > ROW_MAX+1))
    {
      This->cell_s->status = TYPE_VALUE_ERROR;
      return -1;
    }

  if (string[index]==';')
    index++;
  else
    {
      This->cell_s->status = TYPE_PARAM_LESS_ERROR;
      return -1;
    }

  /* look for second parameter number */
  *indexf = old_indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  if (end_type == TYPE_STRING)
    {
      This->cell_s->status = TYPE_NO_PARAM_NUM;
      return -1;
    }
  
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
  column = gtolong(calc_x)-1;
  avma = avma_old;
  index += nindex-1;
  if ((column < 0) || (column > COLUMN_MAX))
    {
      This->cell_s->status = TYPE_VALUE_ERROR;
      return -1;
    }

  /* look for third parameter number if exist */
  if (string[index]==';')
    {
      index++;
      *indexf = old_indexf;
      nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
      if (nindex < 2)
	return -1;
      if (end_type == TYPE_STRING)
	{
	  This->cell_s->status = TYPE_NO_PARAM_NUM;
	  return -1;
	}
      
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
      ref = gtolong(calc_x);
      avma = avma_old;
      if ((ref < 1) || (ref > 8))
	{
	  This->cell_s->status = TYPE_VALUE_ERROR;
	  return -1;
	}
      index += nindex-1;
    }
  else
    ref = 1;

  /* look for fourth parameter number if exist */
  if (string[index]==';')
    {
      index++;
      *indexf = old_indexf;
      nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
      if (nindex < 2)
	return -1;
      if (end_type == TYPE_STRING)
	{
	  This->cell_s->status = TYPE_NO_PARAM_NUM;
	  return -1;
	}
      
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
      isa1 = gtolong(calc_x);
      avma = avma_old;
      index += nindex-1;
    }

  /* look for fifth parameter string if exist */
  if (string[index]==';')
    {
      index++;
      *indexf = old_indexf;
      nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
      if (nindex < 2)
	return -1;
      
      if (end_type != TYPE_STRING)
	{
	  This->cell_s->status = TYPE_NO_PARAM_STRING;
	  return -1;
	}
      index += nindex-1;
    }

  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }

  *indexf = old_indexf;

  switch (ref)
    {
    case 1:
    case 5:
      strf[(*indexf)++]='$';
      coltostr(column, &num, strf+*indexf);
      (*indexf)+=num;
      strf[(*indexf)++]='$';
      sprintf(strf+*indexf,"%d", (int)line);
      break;
    case 2:
    case 6:
      coltostr(column, &num, strf+*indexf);
      (*indexf)+=num;
      strf[(*indexf)++]='$';
      sprintf(strf+*indexf,"%d", (int)line);
      break;
    case 3:
    case 7:
      strf[(*indexf)++]='$';
      coltostr(column, &num, strf+*indexf);
      (*indexf)+=num;
      sprintf(strf+*indexf,"%d", (int)line);
      break;
    case 4:
    case 8:
      coltostr(column, &num, strf+*indexf);
      (*indexf)+=num;
      sprintf(strf+*indexf,"%d", (int)line);
    }
  while(strf[*indexf])
    (*indexf)++;
  
  This->ret_type = TYPE_STRING;
  return index+1;
}

/* ----------------------------------------------------------------- **
** colonne(ref) return num of the column                             **
** ----------------------------------------------------------------- */
int i_colonne(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  long	column;

  Xc_TRACE(("internal colonne"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;
  strf[(*indexf)++]='(';

  /* look for first parameter CELL if exist */
  /* this should be a valid zone or cell
      index++;
      old_indexf = *indexf;
      nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
      if (nindex < 2)
	return -1;
      
      *indexf = old_indexf;
      index += nindex-1;
    }
  else
    {
      column = This->x+1;
    }
*/
  column = This->x+1;

  sprintf(strf+*indexf,"%d)", (int)column);
  while (strf[*indexf])
    (*indexf)++;
  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }

  This->ret_type = TYPE_NUM;
  return index+1;
}

/* ----------------------------------------------------------------- **
** ligne(ref) return num of the line                                 **
** ----------------------------------------------------------------- */
int i_ligne(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;
  int	old_indexf;
  long	line = 0;

  Xc_TRACE(("internal ligne"));
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  index = 1;
  while (string[index]==' ')
    index++;
  strf[(*indexf)++]='(';

  /* look for first parameter number if exist */
  if (string[index]==';')
    {
      index++;
      old_indexf = *indexf;
      nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
      if (nindex < 2)
	return -1;
      
      *indexf = old_indexf;
      index += nindex-1;
      line = 0;
    }
  else
    {
      line = This->y+1;
    }

  sprintf(strf+*indexf, "%d)", (int)line);
  while (strf[*indexf])
    (*indexf)++;
  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }

  This->ret_type = TYPE_NUM;
  return index+1;
}

/* ----------------------------------------------------------------- **
** indirect(ref_str; isa1) return content of ref cellule             **
** ----------------------------------------------------------------- */
int i_indirect(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	nindex;
  int	end_type;
  int	old_indexf;
  long	isa1;
  char	*tok_str;
  int	cell_x;
  int	cell_y;
  int	other_i;
  int	init_o_i;
  cell	*cell_s;
  int	val;
  char	*tmp_str;

  Xc_TRACE(("internal indirect"));
  index = 0;
  while (string[index]==' ')
    index++;
  if (string[index] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  strf[*indexf]='(';
  (*indexf)++;
  index++;
  while (string[index]==' ')
    index++;

  /* look for first parameter string */
  old_indexf = *indexf;
  nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
  if (nindex < 2)
    return -1;
  if (end_type == !TYPE_STRING)
    {
      This->cell_s->status = TYPE_MUST_BE_STRING;
      return -1;
    }

  index+=nindex-1;
  tok_str = (char *)Xc_malloc("string result", strlen(strf+old_indexf)+1);
  strcpy(tok_str, strf+old_indexf);
  
  /* look for second parameter number if exist */
  *indexf = old_indexf;
  if (string[index]==';')
    {
      index++;
      nindex = F(This).rparse(This, string+index, strf, indexf, &end_type);
      if (nindex < 2)
	{
	  Xc_free(tok_str);
	  return -1;
	}
      if (end_type == TYPE_STRING)
	{
	  This->cell_s->status = TYPE_NO_PARAM_NUM;
	  Xc_free(tok_str);
	  return -1;
	}
      
/* ignored right now...
      GlobErrNum = 0;
      avma_old = avma;
      calc_x=lisexpr(strf+old_indexf);
      switch (GlobErrNum)
	{
	case 0:
	  break;
	case 260:
	  This->cell_s->status = TYPE_DIVZ_ERROR;
	  Xc_free(tok_str);
	  return -1;
	default:
	  This->cell_s->status = TYPE_CALC_ERROR;
	  Xc_TRACE(("erreur in calculation system %d", GlobErrNum));
	  Xc_free(tok_str);
	  return -1;
	}
      isa1 = gtolong(calc_x);
      avma = avma_old;
*/
      *indexf = old_indexf;
      isa1 = 1;
      index += nindex-1;
    }
  else
    {
      isa1 = 1;
    }

  /* teste si c'est une cellule */
  if (((tok_str[0] >= 'a') && (tok_str[0] <= 'z')) ||
      ((tok_str[0] >= 'A') && (tok_str[0] <= 'Z')) ||
      (tok_str[0] == '$'))
    {
      /* search for cell */
      other_i = 0;
      cell_x = 0;
      /* ignore $ this act only with move */
      if (tok_str[other_i]=='$')
	other_i++;
      init_o_i = other_i;
      while ((((tok_str[other_i] >= 'A') && (tok_str[other_i] <= 'Z')) || 
	      ((tok_str[other_i] >= 'a') && (tok_str[other_i] <= 'z'))) &&
	     other_i<init_o_i+2)
	{
	  if ((tok_str[other_i] >= 'a') && (tok_str[other_i] <= 'z'))
	    {
	      cell_x *= 26;
	      cell_x += tok_str[other_i] - 'a' + 1;
	    }
	  else
	    {
	      cell_x *= 26;
	      cell_x += tok_str[other_i] - 'A' + 1;
	    }
	  other_i++;
	}
      if (other_i == init_o_i)
	{
	  This->cell_s->status = TYPE_VALUE_ERROR;
	  Xc_free(tok_str);
	  return -1;
	}
      cell_x--;

      cell_y = 0;
      if (tok_str[other_i]=='$')
	other_i++;
      init_o_i = other_i;
      while((tok_str[other_i] >= '0') && (tok_str[other_i] <= '9'))
	{
	  cell_y *= 10;
	  cell_y += tok_str[other_i] - '0';
	  other_i++;
	}
      if (other_i == init_o_i)
	{
	  This->cell_s->status = TYPE_VALUE_ERROR;
	  Xc_free(tok_str);
	  return -1;
	}
      cell_y--;
      while(tok_str[other_i]==' ')
	other_i++;
      if (tok_str[other_i] == ')')
	other_i++;
      if (tok_str[other_i] != 0)
	{
	  This->cell_s->status = TYPE_VALUE_ERROR;
	  Xc_free(tok_str);
	  return -1;
	}
      Xc_free(tok_str);
      cell_s = F(This->motor).get_cell(This->motor,
				       cell_x,
				       cell_y);
      
      switch (This->d_type)
	{
	case ADD_DEP:
	  /* check circular dependancy */
	  if (F(This->motor).check_c_depend(This->motor, 
					    This->cell_s,
					    cell_x,
					    cell_y))
	    {
	      Xc_TRACE(("Error circular dep"));
	      This->cell_s->status = TYPE_CIRC_ERROR;
	      return -1;
	    }
	  F(This->motor).add_depend(This->motor,
				    This->cell_s,
				    This->y, This->x,
				    cell_y, cell_x);
	case IGN_DEP:
	  if ((cell_s->type == Xq_TEXT) || 
	      ((cell_s->type == Xq_FORMULA) 
	       && cell_s->text 
	       && !(cell_s->value2)))
	    {
	      int	i;
	      i=0;
	      if (cell_s->text[i]=='\'')
		i++;
	      while (cell_s->text[i])
		strf[(*indexf)++] = cell_s->text[i++];
	      strf[(*indexf)] = 0;
	      This->ret_type = TYPE_STRING;
	      break;
	    }

	  if (cell_s->value2)
	    {
	      int	coma;
	      int	i;
	      
	      tmp_str = g2str(cell_s->value2, BIG_PREC);
	      /* try to cut last space */
	      
	      strcpy(strf+*indexf, tmp_str);
	      val = strlen(strf+*indexf);
	      i = 0;
	      coma = 0;
	      while (i < val)
		{
		  if (strf[*indexf+i]=='.')
		    {
		      coma = 1;
		      break;
		    }
		  i++;
		}
	      if (!coma)
		{
		  strf[*indexf+val++]='.';
		  strf[*indexf+val++]='0';
		  strf[*indexf+val]=0;
		}
	      free(tmp_str);
	      This->ret_type = cell_s->ret_type;
	    }
	  else 
	    {	 /* Xq_VOID case -> set zero value */
	      strf[*indexf] = '0';
	      strf[*indexf+1] = 0;
	      val = 1;
	      This->ret_type = TYPE_NUM;
	    }
	  *indexf += val;
	  break;
	case SUB_DEP:
	  F(This->motor).sub_depend(This->motor,
				    This->cell_s,
				    cell_y, cell_x);
	  This->ret_type = TYPE_CELL;
	  break;
	}
    }

  if (string[index]!=')')
    {
      if (string[index]==Xq_SEPAR)
	This->cell_s->status = TYPE_PARAM_MORE_ERROR;
      else
	This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }

  This->ret_type = TYPE_CELL;
  return index;
}
