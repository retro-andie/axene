/*
** ImageStd.h for Xclamation and XAllWrite in Global/
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
** Started on  Mon Jul 17 17:42:39 1995 Emmanuel Paris
** Last update Thu Jun 25 18:24:53 1998 Emmanuel Paris
*/


#ifndef _ImageStd_h
#define _ImageStd_h

enum
{
 IMAGE_FUNCTION_BEGIN = 400,
 MOVE_IMAGE,
 ROTATE_IMAGE,
 CENTER_IMAGE,
 INIT_IMAGE,
 RESIZE_IMAGE,
 SCALE_IMAGE,
 NORMAL_ASPECT_IMAGE,
 BEST_ASPECT_IMAGE,
 AUTO_RESIZE_IMAGE,
 MAX_ASPECT_IMAGE,
 FLIP_HORIZONTAL_IMAGE,
 FLIP_VERTICAL_IMAGE,
 MAPPING1_IMAGE,
 MAPPING2_IMAGE,
 IMAGE_FUNCTION_END
};

#endif /*_ImageStd_h*/
