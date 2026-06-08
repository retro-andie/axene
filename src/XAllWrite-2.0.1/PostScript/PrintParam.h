/*
** PrintParam.h for XAllWrite in PostScript/
** Printing parameters
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
** Started on  Fri Jun  2 15:18:09 1995 Stéphane Boisson
** Last update Fri Jun  2 15:31:12 1995 Stéphane Boisson
*/

#ifndef __XC_PRINT_PARAM_H__
#define __XC_PRINT_PARAM_H__

typedef struct print_param_s print_param_t;

#include "PostScript.h"

struct print_param_s
{
  printer_t	*printer;
  int		NbCopies;
  int		StartPage;
  int		EndPage;
  int		NbPage;
  boolean	Left;
  boolean	Right;
  boolean	AllPages;
  boolean	Draft;
  boolean	Color;
};

#endif /* !__XC_PRINT_PARAM_H__ */
