/*
** error_defs.h for AxeneOffice in Global/
** Constant definitions for internal errors
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
** Started on  Mon Jun 13 15:04:47 1994 Stéphane Boisson
** Last update Tue Oct  3 11:34:24 1995 Emmanuel Paris
*/

#ifndef _Error_defs_h_
#define _Error_defs_h_

typedef enum {
  XC_NO_ERROR,
  XC_NAME_NOT_EXIST,
  XC_NAME_ALREADY_EXIST,
  XC_MEMORY_ALLOCATION,
  XC_FILE_NOT_EXIST,
  XC_FILE_TRUNCATED,
  XC_FILE_FORMAT_ERROR,
  XC_FILE_UNKNOW_FORMAT,
  XC_NO_POINT_DELETED,
  XC_CADRE_MUST_BE_DELETED,
  XC_EMPTY_FRAME
} error;

#endif
