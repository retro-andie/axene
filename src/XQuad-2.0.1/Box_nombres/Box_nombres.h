/*
** Box_nombres.h for XQuad in Box_nombres/
** Object and method for Box_nombres Class
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
** Started on  Mon May 22 13:32:14 1995 Robin Castanier
** Last update Thu Aug 12 23:34:18 1999 Emmanuel Paris
*/

#ifndef _Box_nombres_h
#define _Box_nombres_h

typedef struct sc_Box_nombres c_Box_nombres;

#include "xcalibur.h"
#include "WInterface.h"
#include "ASLBox.h"
#include "Label.h"
#include "ItemMenu.h"
#include "ItemList.h"
#include "WTextField.h"
#include "Grid.h"
#include "RowColumn.h"
#include "ToggleButton.h"
#include "Frame.h"
#include "Date_Time_Common.h"
#include "BoxNombresPartStd.h"
#include "BNP_Bars.h"
#include "BaseStd.h"
#include "ModifList.h"
#include <X11/Intrinsic.h>

#define Xq_ALL	0

/*--- define the method for Class Box_nombres ---*/
typedef struct
{
  F_STD;
  void (*refresh_preview) ___PROTO((c_Box_nombres *This));
} sf_Box_nombres;

typedef struct {
  int		type;

  unsigned int	max_prec;
  unsigned int	min_prec;
  unsigned int	space_each;
  char		coma_sep;
  c_Color	*color1;
  c_Color	*color2;
  char		char_sep;
  boolean	truncate;
  
  char		unit[XcMAX_NAME_LENGTH];
  boolean	unitafter;
  
  unsigned int	exp_digit;
  unsigned int	exp_multiple;
  boolean	exp_sign;
  char		exp_string[XcMAX_NAME_LENGTH];
  
  unsigned int	frac_prec;
  boolean	frac_dec;
  
  char		true_name[XcMAX_NAME_LENGTH];
  char		false_name[XcMAX_NAME_LENGTH];
  
  name_size_id_t day_name_size;
  name_size_id_t month_name_size;
  
  dt_seq_t	*date_seq[XqMAX_DT_SEQ+1];
  dt_seq_t	*time_seq[XqMAX_DT_SEQ+1];
  dt_seq_t	*date_time_seq[XqMAX_DT_SEQ+1];
  
  c_FormatNbr	*other_format[15];
  c_FormatNbr	*format;
} format_modif_t;

enum {
  BN_FMT_NORMAL,
  BN_FMT_UNIT,
  BN_FMT_EXPONANTIAL,
  BN_FMT_PERCENT,
  BN_FMT_FRAC,
  BN_FMT_BOOLEAN,
  BN_FMT_DAY,
  BN_FMT_MONTH,
  BN_FMT_TIME,
  BN_FMT_DATE,
  BN_FMT_DATE_TIME,
  BN_FMT_ALL,
  NB_BN_FMT
};

typedef struct
{
  char		*name;
  int		num;
} bn_format_t;

/*--- define the Box_nombres Class ---*/
struct sc_Box_nombres
{
  sf_Box_nombres *f;
  Widget	w_This;
  c_ASLBox	*PBox;
  c_ItemMenu    *imBNFormatType;
  c_Grid	*Grid;
  c_Frame	*fOnglet;
  c_RowColumn	*rcOnglet;
  c_Frame	*fSetup;
  c_Frame	*fPreview;
  c_WTextField	*tfPreview;
  
  c_ModifList	*list_save;
  c_Color	*color_base;
  c_FormatNbr	*generic_format;
  c_FormatNbr	**format_base;
  c_FormatNbr	*v_format_base;
  c_FormatNbr	**d_format_base;
  int		d_nb_format;
  
  bn_format_t	*bn_format;
  int		selected_fmt;
  int		selected_type;
  int		selected_bnp_bar;
  int		selected_bnp_std;
  bnp_bar_t	**bnp_bar;
  bnp_std_t	*bnp_std_sel;
  c_FormatNbr	*edited_format;
  
  boolean	dim_calc;
  Dimension	width, height;
  
  format_modif_t modif;
  c_FormatNbr	**retptr;
  
  unsigned long	preview_color;
  c_FormatNbr	*preview_format;
  char		*preview_string;
  
  dt_seq_base_t	*dt_seq_base;
  boolean	modify_flag;
};

extern sf_Box_nombres fc_Box_nombres;

#endif





