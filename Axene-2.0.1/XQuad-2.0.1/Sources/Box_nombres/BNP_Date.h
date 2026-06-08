/*
** BNP_Date.h for XQuad in Box_nombres/
** Box Nombres Part that manage date to other format
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
** Last update Tue Apr 16 12:17:23 1996 Emmanuel Paris
*/


#ifndef _BNP_Date_h_
#define _BNP_Date_h_

#include "BoxNombresPartStd.h"
#include "Date_Time.h"

typedef struct 
{
  c_Grid		*Grid;
  c_Frame		*fPadding;
  c_RowColumn		*rcPadding;
/*c_Label		*lPadding;*/
  c_ToggleButton	*tbNoPad;
  c_ToggleButton	*tbPadWith0;
  c_ToggleButton	*tbPadWithSpace;
  c_Frame		*fNameSize;
  c_RowColumn		*rcNameSize;
/*c_Label		*lNameSize;*/
  c_ToggleButton	*tbNSVeryShort;
  c_ToggleButton	*tbNSShort;
  c_ToggleButton	*tbNSLong;
  c_Separator		*sSeparator1;
  c_Separator		*sSeparator2;
  c_RowColumn		*rcYear;
  c_Label		*lYear;
  c_PushButton		*pbYear;
  c_PushButton		*pbYearShort;
  c_PushButton		*pbYearJC;
  c_RowColumn		*rcMonth;
  c_Label		*lMonth;
  c_PushButton		*pbMonth;
  c_PushButton		*pbMonthName;
  c_RowColumn		*rcWeek;
  c_Label		*lWeek;
  c_PushButton		*pbWeek;
  c_RowColumn		*rcDay;
  c_Label		*lDay;
  c_PushButton		*pbDay;
  c_PushButton		*pbDayName;
  c_RowColumn		*rcDayInYear;
  c_Label		*lDayInYear;
  c_PushButton		*pbDayInYear;
  c_Label		*lSep;
  c_RowColumn		*rcSeparators;
  c_PushButton		*pbSep1;
  c_PushButton		*pbSep2;
  c_PushButton		*pbSep3;
  c_PushButton		*pbSep4;
  c_PushButton		*pbSep5;
  c_PushButton		*pbSep6;
  dt_seq_t		*dt_seq_pb[15];
  padding_id_t		padding;
  name_size_id_t	name_size;
  Pixel			background;
} data_date_t;

typedef struct
{
  F_BNP_STD;
  data_date_t	*data;
} bnp_date_t;

extern bnp_std_t bnp_dates;

#endif /* _BNP_Date_h_ */

