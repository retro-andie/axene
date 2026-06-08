/*
** HelpLine_PAO.c for Xclamation in HelpLine/
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
** Started on  Mon Jul 17 17:45:41 1995 Emmanuel Paris
** Last update Mon Mar 16 20:26:38 1998 Emmanuel Paris
*/

#define _HELPLINE_PAO_PRIVATE_
#include "HelpLine_PAO.h"

int cnv_func_for_helpline(function)
int		function;
{
 f_interv_t	*func_interv = NULL;
 int		nb_interv = 0;
 int		i, message;

 func_interv = func_interv_pao;
 nb_interv = NB_INTERV_PAO;
 
 i=0;
 while(i < nb_interv && 
       (function<func_interv[i].func_beg || function>func_interv[i].func_end))
  i++;
 if (i == nb_interv)
  message = XcR_NONE;
 else
  message = func_interv[i].convert[function - func_interv[i].func_beg];

 return message;
}
