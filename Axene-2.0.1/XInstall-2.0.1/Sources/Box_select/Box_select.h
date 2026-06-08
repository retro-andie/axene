/*
** Box_select.h for XInstall in Box_select/
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
** Started on  Mon Jul 17 14:00:16 1995 Robin Castanier
** Last update Thu Feb  5 18:34:58 1998 Emmanuel Paris
*/

#ifndef _Box_select_h
#define _Box_select_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Form.h"
#include "Frame.h"
#include "Icon.h"
#include "ManageWidget.h"
#include "Label.h"
#include "Grid.h"
#include "Resource.h"
#include "ScrolledWindow.h"

#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>

/*--- define the dep list ---*/
typedef struct _icon_dep
{
 int			number;
 struct _icon_dep	*next;
} icon_dep;

#define NONE_SELECT	0
#define NORMAL_SELECT	1
#define OPTION_SELECT	2
#define MANDAT_SELECT	3

/*--- define button list ---*/
typedef struct _icon_list
{
 int			number;
 int			key_number;
 char			*filename;
 boolean		selected;
 int			sel_type;
 unsigned long int	size;
 icon_dep		*mandatory;
 icon_dep		*optional;
 icon_dep		*mandated;
 icon_dep		*optioned;
 boolean		treated;
 int			language;
 /*--- temp buffer ---*/
 char			**pix_buffer;
 char			*tmp_label;
 /*--- interface ---*/
 c_Icon			*Icon;
 c_Icon			*IconValid;
 Widget			Label;
 struct _icon_list	*Next;
 struct _icon_list	*Prev;
} icon_list;

/*--- define the method for Class Box_select ---*/
typedef struct
{
 F_STD;
 int (*waitForUnmap)();
 void (*deletePackList) ___PROTO((icon_list **Ilist, boolean onlyone));
} sf_Box_select;

/*--- define the Box_select Class ---*/
typedef struct
{
 sf_Box_select	*f;
 Widget		w_This;

 unsigned long	used_size;
 unsigned long	free_space;
 int		select_count;
 int		tongue;
 c_CustomDialog	*Dialog;
 c_Grid		*Grid;
 c_Label	*Ltitre;
 c_Label	*LInfo1;
 Widget		LInfo2;
 c_Grid		*Grid2;

 icon_list	**Ilist;

 c_Grid		*GridIcon1;
 c_Grid		*GridIcon2;

 c_Frame	*FrameInfo1;
 c_Frame	*FrameInfo2;
 c_Frame	*FrameSelect;
 c_Frame	*FrameFreeSpace;
 c_Frame	*FrameUsedSpace;
 c_Label	*LUsedSpace;
 c_ScrolledWindow  *SWindow;
 Widget		wSSpace;
 c_Label	*LFreeSpace;
 boolean	bInstallAlreadyGrised;
 int		artificial_select;
 long		lforeground;
 
} c_Box_select;

extern sf_Box_select fc_Box_select;
extern c_Resource *GlobResources;

#endif
