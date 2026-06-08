/*
** BNP_Time.h for XQuad in Box_nombres/
** Box Nombres Part that manage time to other format
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
** Last update Tue Apr 16 14:26:45 1996 Emmanuel Paris
*/


#ifndef _BNP_Time_h_
#define _BNP_Time_h_

#include "BoxNombresPartStd.h"
#include "Date_Time.h"

typedef struct 
{
  c_Grid		*Grid;
  c_RowColumn		*rcPadding;
  c_Frame		*fPadding;
  c_ToggleButton	*tbNoPad;
  c_ToggleButton	*tbPadWith0;
  c_ToggleButton	*tbPadWithSpace;
  c_Separator		*sSeparator1;
  c_RowColumn		*rcHour;
  c_Label		*lHour;
  c_Label		*lHour24;
  c_PushButton		*pbHour24;
  c_PushButton		*pbHour12;
  c_PushButton		*pbHourAM_PM;
  c_RowColumn		*rcMinute;
  c_Label		*lMinute;
  c_PushButton		*pbMinute;
  c_RowColumn		*rcSecond;
  c_Label		*lSecond;
  c_PushButton		*pbSecond;
  c_RowColumn		*rcTenthOfSecond;
  c_Label		*lTenthOfSecond;
  c_PushButton		*pbTenthOfSecond;
  c_RowColumn		*rcHundredthOfSecond;
  c_Label		*lHundredthOfSecond;
  c_PushButton		*pbHundredthOfSecond;
  c_RowColumn		*rcThousandthOfSecond;
  c_Label		*lThousandthOfSecond;
  c_PushButton		*pbThousandthOfSecond;
  c_Separator		*sSeparator2;
  c_Label		*lSep;
  c_RowColumn		*rcSeparators;
  c_PushButton		*pbSep1;
  c_PushButton		*pbSep2;
  c_PushButton		*pbSep3;
  c_PushButton		*pbSep4;
  c_PushButton		*pbSep5;
  c_PushButton		*pbSep6;
  dt_seq_t		*dt_seq_pb[14];
  padding_id_t		padding;
  Pixel			background;
} data_time_t;

typedef struct
{
  F_BNP_STD;
  data_time_t	*data;
} bnp_time_t;

extern bnp_std_t bnp_times;

#endif /* _BNP_Times_h_ */
