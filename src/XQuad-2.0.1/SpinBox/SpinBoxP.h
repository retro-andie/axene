/*
** SpinBoxP.h for Xclamation, XQuad and XAllWrite in SpinBox/
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
** Started on  Sun Oct 25 17:28:11 1998 Emmanuel Paris
** Last update Fri Oct 30 15:07:01 1998 Emmanuel Paris
*/

#ifndef SB_EXTERN_DEFINES

#include "../Pixmaps/plus.zpm"
#include "../Pixmaps/plus.arm.zpm"
#include "../Pixmaps/minus.zpm"
#include "../Pixmaps/minus.arm.zpm"

typedef struct 
{
 char **icon_plus;
 char **icon_plus_arm;	/* icon plus arm Pixmap */
 char **icon_minus;
 char **icon_minus_arm;	/* icon minus arm Pixmap */
} sb_record_t;

static sb_record_t sb_record[NB_XcSB_TYPE] = 
{
 { plus_xpm, plus_arm_xpm, minus_xpm, minus_arm_xpm },
};

#else		/* SB_EXTERN_DEFINES */

enum 
{
 XcSB_ARROWS_UP_DOWN = -2,
 XcSB_ARROWS_LEFT_RIGHT = -1,
 XcSB_PLUS_MINUS,
 NB_XcSB_TYPE
};

#endif
