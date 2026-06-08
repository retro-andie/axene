/*
** Parser_stat.c for XQuad in Parser/
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
** Last update Sun Oct 27 19:05:56 1996 Emmanuel Paris
*/

#include "Parser.h"

/* ----------------------------------------------------------------- ** 
** max (zone|const|cell; ...)                                        ** 
** ----------------------------------------------------------------- */
int it_max(This, string, strf, indexf)
c_Parser	 *This;
char		*string;
char		*strf;
int		*indexf;
{
  int	index;
  int	sortie;
  int	nindex;
  int	type;
  GEN	max_g;
  char	*tmp_str;
  GEN	gtmp;

  Xc_TRACE(("internal max"));
  max_g = NULL;
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  strf[(*indexf)++]='(';
  index = 1;
  while (string[index]==' ')
    index++;
  sortie = 0;
  while (!sortie)
    {
      nindex = F(This).compute_param(This, string+index, strf, indexf, 
				     &type, &gtmp);
      if (nindex < 1)
	return -1;
      index += nindex;

      if (type == TYPE_STRING)
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

	  index++;
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
			  This->cell_s->status = TYPE_CIRC_ERROR;
			  return -1;
			}
		      F(This->motor).add_depend(This->motor,
						This->cell_s,
						This->y, This->x,
						p_line->number,
						p_col->number);
		    case IGN_DEP:
		      /* PENSER A LA PRECISION DE CALCUL */
		      if (p_line->cell.value2)
			{
			  if (max_g)
			    {
			      if (gcmp(p_line->cell.value2, max_g)>=0)
				{
				  max_g = p_line->cell.value2;
				}
			    }
			  else
			    max_g = p_line->cell.value2;
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
	}
      else
	{
	  if ((type != TYPE_PARSE_BEGIN) &&
	      (type != TYPE_PARSE_END) &&
	      (type != TYPE_SEPARATOR))
	    {
	      if (max_g)
		{
		  if (gcmp(gtmp, max_g)>=0)
		    {
		      max_g = gtmp;
		    }
		}
	      else
		max_g = gtmp;
	    }
	}
      if (string[index-1]==Xq_SEPAR)
	{
	  while (string[index]==' ')
	    index++;
	}
      if (string[index-1]==')')
	{
	  if(max_g)
	    {
	      tmp_str = g2str(max_g, BIG_PREC);
	      strcpy(strf+*indexf, tmp_str);
	      free(tmp_str);
	      *indexf += strlen(strf+*indexf);
	    }
	  else
	    {
	      strf[(*indexf)++]='0';
	      strf[(*indexf)++]='.';
	      strf[(*indexf)++]='0';
	    }
	  strf[(*indexf)++]=')';
	  strf[*indexf]=0;
	  sortie = 1;
	}
    }
  This->ret_type = TYPE_NUM;
  return index;
}

/* ----------------------------------------------------------------- ** 
** min (zone|const|cell; ...)                                        ** 
** ----------------------------------------------------------------- */
int it_min(This, string, strf, indexf)
c_Parser	 *This;
char		*string;
char		*strf;
int		*indexf;
{
  int	index;
  int	sortie;
  int	nindex;
  int	type;
  GEN	min_g;
  char	*tmp_str;
  GEN	gtmp;

  Xc_TRACE(("internal min"));
  min_g = NULL;
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  strf[(*indexf)++]='(';
  index = 1;
  while (string[index]==' ')
    index++;
  sortie = 0;
  while (!sortie)
    {
      nindex = F(This).compute_param(This, string+index, strf, indexf, 
				     &type, &gtmp);
      if (nindex < 1)
	return -1;
      index += nindex;

      if (type == TYPE_STRING)
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

	  index++;
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
			  This->cell_s->status = TYPE_CIRC_ERROR;
			  return -1;
			}
		      F(This->motor).add_depend(This->motor,
						This->cell_s,
						This->y, This->x,
						p_line->number,
						p_col->number);
		    case IGN_DEP:
		      /* PENSER A LA PRECISION DE CALCUL */
		      if (p_line->cell.value2)
			{
			  if (min_g)
			    {
			      if (gcmp(p_line->cell.value2, min_g)<=0)
				{
				  min_g = p_line->cell.value2;
				}
			    }
			  else
			    min_g = p_line->cell.value2;
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
	}
      else
	{
	  if ((type != TYPE_PARSE_BEGIN) &&
	      (type != TYPE_PARSE_END) &&
	      (type != TYPE_SEPARATOR))
	    {
	      if (min_g)
		{
		  if (gcmp(gtmp, min_g)<=0)
		    {
		      min_g = gtmp;
		    }
		}
	      else
		min_g = gtmp;
	    }
	}
      if (string[index-1]==Xq_SEPAR)
	{
	  while (string[index]==' ')
	    index++;
	}
      if (string[index-1]==')')
	{
	  if(min_g)
	    {
	      tmp_str = g2str(min_g, BIG_PREC);
	      strcpy(strf+*indexf, tmp_str);
	      free(tmp_str);
	      *indexf += strlen(strf+*indexf);
	    }
	  else
	    {
	      strf[(*indexf)++]='0';
	      strf[(*indexf)++]='.';
	      strf[(*indexf)++]='0';
	    }
	  strf[(*indexf)++]=')';
	  strf[*indexf]=0;
	  sortie = 1;
	}
    }
  This->ret_type = TYPE_NUM;
  return index;
}

/* ----------------------------------------------------------------- ** 
** moyenne (zone|const|cell; ...)                                    ** 
** ----------------------------------------------------------------- */
int i_moy(This, string, strf, indexf)
c_Parser *This;
char	*string;
char	*strf;
int	*indexf;
{
  int	index;
  int	sortie;
  int	nindex;
  int	nbrparam;
  int	type;
  int	type2;
  GEN	gentmp = gzero;
  int	oldindexf;

  Xc_TRACE(("internal moyenne"));
  nbrparam = 0;
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  strf[(*indexf)++]='(';
  strf[(*indexf)++]='(';
  index = 1;
  sortie = 0;
  while (!sortie)
    {
      oldindexf = *indexf;
      nindex = F(This).srparse(This, string+index, strf, indexf,
			       &type, &type2);
      if (nindex < 1)
	return -1;
      index += nindex;

      if (type2 == TYPE_STRING)
	{
/*This->cell_s->status = TYPE_NO_PARAM_STRING;*/
	  This->cell_s->status = TYPE_ERROR;
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
	  char	*tmp_str;
	  int	coma;
	  int	val;
	  int	anybody;

	  anybody = 0;
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

	  strf[(*indexf)++]='(';
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
		      if (p_line->cell.value2)
			{
			  nbrparam++;
			  if (!anybody)
			    {
			      anybody = 1;
			      gentmp = gcopy(p_line->cell.value2);
			    }
			  else
			    {
			      gentmp = gadd(gentmp, p_line->cell.value2);
			    }
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
	  if (!anybody)
	    {
	      /* supress parenthesis */
	      (*indexf)--;
	    }
	  else
	    {
	      tmp_str = g2str(gentmp, BIG_PREC);
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
	      *indexf += val;
	      strf[(*indexf)++]=')';
	    }
	}

      if (type == TYPE_SEPARATOR)
	{
	  if (*indexf-oldindexf > 1)
	    {
	      nbrparam++;
	    }
	  strf[(*indexf)++]='+';
	}

      if (type == TYPE_PARSE_END)
	{
	  if (*indexf-oldindexf > 1)
	    nbrparam++;
	  if (nbrparam < 1)
	    {
	      This->cell_s->status = TYPE_PARAM_LESS_ERROR;
	      return -1;
	    }
	  sprintf(strf+*indexf, "/%d.0)", nbrparam);
	  *indexf += strlen(strf+*indexf);
	  sortie = 1;
	}
    }
  This->ret_type = TYPE_NUM;
  return index;
}

/* ----------------------------------------------------------------- ** 
** compteur (zone|const|cell; ...)                                 ** 
** ----------------------------------------------------------------- */
int i_count(This, string, strf, indexf)
c_Parser	*This;
char		*string;
char		*strf;
int		*indexf;
{
  int	index;
  int	sortie;
  int	nindex;
  int	nbrparam;
  int	type;
  int	type2;
  int	save_index;
  int	oldindexf;

  Xc_TRACE(("internal count"));
  nbrparam = 0;
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  strf[(*indexf)++]='(';
  save_index = *indexf;
  index = 1;
  while (string[index]==' ')
    index++;
  sortie = 0;
  while (!sortie)
    {
      oldindexf = *indexf;
      nindex = F(This).srparse(This, string+index, strf, indexf,
			       &type, &type2);
      if (nindex < 1)
	return -1;
      index += nindex;

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
			  This->cell_s->status = TYPE_CIRC_ERROR;
			  return -1;
			}
		      F(This->motor).add_depend(This->motor,
						This->cell_s,
						This->y, This->x,
						p_line->number,
						p_col->number);
		    case IGN_DEP:
		      if (p_line->cell.type != Xq_VOID)
			{
			  nbrparam++;
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
	  continue;
	}

      if (type == TYPE_SEPARATOR)
	{
	  if (*indexf-oldindexf > 1)
	    nbrparam++;
	}

      if (type == TYPE_PARSE_END)
	{
	  if (*indexf-oldindexf > 1)
	    nbrparam++;
	  *indexf = save_index;
	  sprintf(strf+*indexf, "%d)", nbrparam);
	  *indexf += strlen(strf+*indexf);
	  sortie = 1;
	}
    }
  This->ret_type = TYPE_NUM;
  return index;
}

/* ----------------------------------------------------------------- ** 
** compteur nombre (zone|const|cell; ...)                            ** 
** ----------------------------------------------------------------- */
int i_countn(This, string, strf, indexf)
c_Parser	*This;
char		*string;
char		*strf;
int		*indexf;
{
  int	index;
  int	sortie;
  int	nindex;
  int	nbrparam;
  int	type;
  int	type2;
  int	save_index;
  int	oldindexf;

  Xc_TRACE(("internal count nombre"));
  nbrparam = 0;
  if (string[0] != '(')
    {
      This->cell_s->status = TYPE_MISSING_P_ERROR;
      return -1;
    }
  strf[(*indexf)++]='(';
  save_index = *indexf;
  index = 1;
  while (string[index]==' ')
    index++;
  sortie = 0;
  while (!sortie)
    {
      oldindexf = *indexf;
      nindex = F(This).srparse(This, string+index, strf, indexf,
			       &type, &type2);
      if (nindex < 1)
	return -1;
      index += nindex;

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
			  This->cell_s->status = TYPE_CIRC_ERROR;
			  return -1;
			}
		      F(This->motor).add_depend(This->motor,
						This->cell_s,
						This->y, This->x,
						p_line->number,
						p_col->number);
		    case IGN_DEP:
		      if ((p_line->cell.type == Xq_FORMULA) ||
			  (p_line->cell.type == Xq_VALUE))
			{
			  nbrparam++;
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
	}

      if (type == TYPE_SEPARATOR)
	{
	  if (*indexf-oldindexf > 1)
	    nbrparam++;
	}

      if (type == TYPE_PARSE_END)
	{
	  if (*indexf-oldindexf > 1)
	    nbrparam++;
       	  *indexf = save_index;
	  sprintf(strf+*indexf, "%d)", nbrparam);
	  *indexf += strlen(strf+*indexf);
	  sortie = 1;
	}
    }
  This->ret_type = TYPE_NUM;
  return index;
}
