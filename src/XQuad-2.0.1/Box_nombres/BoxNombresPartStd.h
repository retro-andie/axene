/*
** BoxNombresPartStd.h for XQuad in Box_nombres/
** Standard definition for Part of Box nombres
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
** Last update Mon Mar  4 15:22:54 1996 Emmanuel Paris
*/

#ifndef _BoxNombresPartStd_h_
#define _BoxNombresPartStd_h_

typedef struct sbnp_bar_t bnp_bar_t;
typedef struct sbnp_std_t bnp_std_t;

#include "Box_nombres.h"
#include "RowColumn.h"
#include "Grid.h"
#include "ColorList.h"
#include "ToggleButton.h"
#include "PushButton.h"
#include "RadioBox.h"
#include "Separator.h"

#define F_BNP_STD \
boolean	created; \
boolean managed; \
void (*CreateBoxPart) ___NPROTO((c_Box_nombres *This, bnp_std_t *bnp)); \
void (*ManageBoxPart) ___NPROTO((c_Box_nombres *This, bnp_std_t *bnp)); \
void (*RefreshBoxPart) ___NPROTO((c_Box_nopbres *This, bnp_std_t *bnp)); \
void (*DestroyBoxPart) ___NPROTO((c_Box_nombres *This, bnp_std_t *bnp))
     
struct sbnp_std_t
{
  F_BNP_STD;
  void	*data;
};

#define BNP_CREATE(bnp) \
((bnp_std_t *)(bnp))->CreateBoxPart

#define BNP_MANAGE(bnp) \
((bnp_std_t *)(bnp))->ManageBoxPart

#define BNP_REFRESH(bnp) \
((bnp_std_t *)(bnp))->RefreshBoxPart

#define BNP_DESTROY(bnp) \
((bnp_std_t *)(bnp))->DestroyBoxPart

struct sbnp_bar_t
{
  int		nb_bnp;
  int		bnp_sel;
  bnp_std_t	**bnp;
  char		**bnp_name;
  c_ToggleButton **bnp_tbutton;
  c_Grid	*bnp_grid;
  char		*preview_string;
};

#endif /* _BoxNombresPartStd_h_ */


















