/*
** BNP_Frac_Prec.h for XQuad in Box_nombres/
** Box Nombres Part that manage Fraction precision
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
** Started on  Wed Jan  3 22:16:04 1996 Emmanuel Paris
** Last update Thu Jan  4 13:41:44 1996 Emmanuel Paris
*/


#ifndef _BNP_Frac_Prec_h_
#define _BNP_Frac_Prec_h_

#include "BoxNombresPartStd.h"

typedef struct 
{
  c_Grid		*Grid;
  c_ToggleButton	*TBfracDec;
  c_Label		*LafracPrec;
  c_WTextField		*WTfracPrec;
} data_frac_prec_t;

typedef struct
{
  F_BNP_STD;
  data_frac_prec_t	*data;
} bnp_frac_prec_t;

extern bnp_std_t bnp_frac_prec;

#endif /* _BNP_Frac_Prec_h_ */
