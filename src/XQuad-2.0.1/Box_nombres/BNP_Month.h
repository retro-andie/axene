/*
** BNP_Month.h for XQuad in Box_nombres/
** Box Nombres Part that manage Month name size
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
** Last update Tue Jan  9 15:20:57 1996 Emmanuel Paris
*/


#ifndef _BNP_Month_h_
#define _BNP_Month_h_

#include "BoxNombresPartStd.h"

typedef struct 
{
  c_Grid		*Grid;
  c_RadioBox		*RBmonthNameSize;
} data_month_t;

typedef struct
{
  F_BNP_STD;
  data_month_t	*data;
} bnp_month_t;

extern bnp_std_t bnp_month_name;

#endif /* _BNP_Month_h_ */

