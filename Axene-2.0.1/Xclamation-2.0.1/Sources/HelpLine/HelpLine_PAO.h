/*
** HelpLine_PAO.h for Xclamation in HelpLine/
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
** Started on  Mon Jul 17 17:46:15 1995 Emmanuel Paris
** Last update Wed Aug  5 17:30:04 1998 Emmanuel Paris
*/

#ifndef _HelpLine_PAO_h
#define _HelpLine_PAO_h

#include "Resource.h"
#include "HelpLine.h"
#define DATA_MACROS
#include "PageStd.h"
#include "DocumentStd.h"

#ifdef _HELPLINE_PAO_PRIVATE_

static int  cnv_func_msg_cadre[] =
{
 XcR_NONE ,			/* SELECT_MOVE_RESIZE_CADRE */
 XcR_helpFFrameSquare ,		/* CREATE_CADRE_RECTANGLE */
 XcR_helpFFrameCircle ,		/* CREATE_CADRE_ELIPSE */
 XcR_helpFFramePolygon ,	/* CREATE_CADRE_POLYGONAL */
 XcR_NONE ,			/* DELETE_SELECTED_CADRE */
 XcR_NONE ,			/* SET_HIGHEST_PRIORITY */
 XcR_NONE ,			/* SET_HIGHER_PRIORITY */
 XcR_NONE ,			/* SET_LOWER_PRIORITY */
 XcR_NONE ,			/* SET_LOWEST_PRIORITY */
 XcR_NONE ,			/* SELECT_ALL_CADRE */
 XcR_helpFZoomIn ,		/* ZOOM_PLUS_PAGE */
 XcR_helpFZoomOut ,		/* ZOOM_MINUS_PAGE */
 XcR_helpFZoomZone ,		/* ZOOM_IN_ZONE */
 XcR_NONE ,			/* ZOOM_FIT_IN_WIDTH */
 XcR_NONE ,			/* ZOOM_FIT_IN_SCREEN */
 XcR_NONE ,			/* ZOOM_AT_REAL_SIZE */
 XcR_NONE ,			/* ZOOM_25_PAGE */
 XcR_NONE ,			/* ZOOM_50_PAGE */
 XcR_NONE ,			/* ZOOM_75_PAGE */
 XcR_NONE ,			/* ZOOM_100_PAGE */
 XcR_NONE ,			/* ZOOM_200_PAGE */
 XcR_NONE ,			/* ZOOM_400_PAGE */
 XcR_helpFFrameRotate ,		/* ROTATE_CADRE */
 XcR_helpFFrameScale ,		/* SCALE_CADRE */
 XcR_NONE ,			/* LOCK_CADRE */
 XcR_NONE ,			/* UNLOCK_CADRE */
 XcR_helpFFrameAddPoint ,	/* INSERT_POINT */
 XcR_helpFFrameSubPoint ,	/* DELETE_POINT */
 XcR_helpFFrameMovePoint ,	/* MOVE_POINT_MODE */
 XcR_helpFFrameSelect ,		/* RESIZE_CADRE_MODE */
 XcR_NONE ,			/* FUSION_CADRE */
 XcR_NONE ,			/* SUBSTRACT_CADRE */
 XcR_NONE ,			/* ADDITION_CADRE */
 XcR_NONE ,			/* OUTLINE_CADRE */
 XcR_NONE ,			/* ALIGN_CADRE */
 XcR_NONE ,			/* CREATE_COLUMN */
 XcR_NONE ,			/* CUT_OBJECT */
 XcR_NONE ,			/* COPY_OBJECT */
 XcR_NONE ,			/* PASTE_OBJECT */
 XcR_NONE ,			/* DELETE_OBJECT */
 XcR_NONE ,			/* DISPATCH_TO_OBJECT */
 XcR_NONE ,			/* DISPLAY_GRID */
 XcR_NONE ,			/* DISPLAY_TYPO_MARK */
 XcR_NONE ,			/* DISPLAY_ALIGN_MARK */
 XcR_NONE ,			/* DISPLAY_REGLE */
 XcR_NONE ,			/* DISPLAY_FRAME_BORDER */
 XcR_NONE ,			/* MAGNETIZE_ALL */
 XcR_NONE ,			/* REDRAW_ALL */
 XcR_NONE ,			/* ADD_LINK_AT_BEGINNING */
 XcR_NONE ,			/* ADD_LINK_AT_END */
 XcR_NONE ,			/* INSERT_LINK_BEFORE */
 XcR_NONE ,			/* INSERT_LINK_BEHIND */
};

static int  cnv_func_msg_image[] = 
{
 XcR_helpFPictureMove ,		/* MOVE_IMAGE */
 XcR_NONE ,			/* ROTATE_IMAGE */
 XcR_NONE ,			/* CENTER_IMAGE */
 XcR_NONE ,			/* INIT_IMAGE */
 XcR_NONE ,			/* RESIZE_IMAGE */
 XcR_NONE ,			/* SCALE_IMAGE */
 XcR_NONE ,			/* MAPPING1_IMAGE */
 XcR_NONE ,			/* MAPPING2_IMAGE */
 XcR_NONE ,			/* BEST_ASPECT_IMAGE */
 XcR_NONE ,			/* AUTO_RESIZE_IMAGE */
 XcR_NONE ,			/* MAX_ASPECT_IMAGE */
 XcR_NONE ,			/* FLIP_HORIZONTAL_IMAGE */
 XcR_NONE ,			/* FLIP_VERTICAL_IMAGE */
};

static int  cnv_func_msg_vector[] =
{
 XcR_helpFVectorMove ,		/* MOVE_VECTOR */
 XcR_NONE ,			/* ROTATE_VECTOR */
 XcR_NONE ,			/* CENTER_VECTOR */
 XcR_NONE ,			/* INIT_VECTOR */
 XcR_NONE ,			/* RESIZE_VECTOR */
 XcR_NONE ,			/* SCALE_VECTOR */
 XcR_NONE ,			/* BEST_ASPECT_VECTOR */
 XcR_NONE ,			/* AUTO_RESIZE_VECTOR */
 XcR_NONE ,			/* MAX_ASPECT_VECTOR */
 XcR_NONE ,			/* FLIP_HORIZONTAL_VECTOR */
 XcR_NONE ,			/* FLIP_VERTICAL_VECTOR */
};

#define NB_INTERV_PAO	3

typedef struct 
{
 int	func_beg;
 int	func_end;
 int	*convert;
} f_interv_t;

static f_interv_t func_interv_pao[NB_INTERV_PAO] =
{
 { SELECT_MOVE_RESIZE_CADRE, CADRE_FUNCTION_END - 1, cnv_func_msg_cadre},
 { IMAGE_FUNCTION_BEGIN + 1, IMAGE_FUNCTION_END - 1, cnv_func_msg_image},
 { VECTOR_FUNCTION_BEGIN + 1, VECTOR_FUNCTION_END - 1, cnv_func_msg_vector}
};

#endif

extern int cnv_func_for_helpline();

#endif






