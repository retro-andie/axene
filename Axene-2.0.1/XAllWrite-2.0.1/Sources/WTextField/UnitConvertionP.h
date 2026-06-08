/*
** UnitConvertionP.h for Xclamation, XQuad, XAllWrite and XMayday in 
** 	WTextField/
** Header for Unit Convertion in WTextField
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
** Started on  Sun Aug 18 15:08:04 1996 Emmanuel Paris
** Last update Tue Oct 20 19:18:19 1998 Emmanuel Paris
*/

#ifndef _UnitConvertionP_h_
#define _UnitConvertionP_h_

#include "UnitConvertion.h"
#define XcUC_UNIT_DEF
#include "UnitLength.h"
#include "UnitRatio.h"
#include "UnitAngle.h"

unit_base_t	unit_base[] = 
{
  { XcUC_LENGTH_POINT,	    unit_length },
  { XcUC_LENGTH_MILLIMETER, unit_length },
  { XcUC_LENGTH_CENTIMETER, unit_length },  
  { XcUC_RATIO_PERCENTAGE , unit_ratio },
  { XcUC_ANGLE_DEGREE , unit_angle }  
};

#endif /* _UnitConvertionP_h_ */

















