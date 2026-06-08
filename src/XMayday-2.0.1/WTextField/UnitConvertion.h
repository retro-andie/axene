/*
** UnitConvertion.h for Xclamation, XQuad, XAllWrite and XMayday in WTextField/
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
** Last update Wed Nov  4 15:16:05 1998 Emmanuel Paris
*/

#ifndef _UnitConvertion_h_
#define _UnitConvertion_h_

typedef struct _unit_conv_t unit_conv_t;
typedef union _parse_unit_t parse_unit_t;

#include "xcalibur.h"

/* definition for unit bases, interface with WTextField */

#define XcUB_LENGTH_POINT	0
#define XcUB_LENGTH_MILLIMETER	1
#define XcUB_LENGTH_CENTIMETER	2
#define XcUB_RATIO_PERCENTAGE	3
#define XcUB_ANGLE_DEGREE	4

typedef struct 
{
  int		default_unit;
  unit_conv_t	*unit_conv;
} unit_base_t;

extern unit_base_t	unit_base[];

/* definition for unit type */

#define XcUC_INTEGER 'd'
#define XcUC_REAL    'g'

#define XcUC_ERROR	-2
#define XcUC_DEFAULT	-1

struct _unit_conv_t
{
  char	*unit_def;
  real (*str_to_val) ___PROTO((parse_unit_t *pulist));
  char	*(*val_to_str) ___NPROTO((char *unit_def, real value));
};

/* parse of a string value to usable struct */

enum {
  XcPU_INTEGER = 1,
  XcPU_REAL,
  XcPU_STRING
};

typedef struct {
  int		type;
  parse_unit_t	*next;
} pu_any_t;

typedef struct {
  int		type;
  parse_unit_t	*next;
  int		value;
} pu_integer_t;

typedef struct {
  int		type;
  parse_unit_t	*next;
  real		value;
} pu_real_t;

typedef struct {
  int		type;
  parse_unit_t	*next;
  char		*string;
} pu_string_t;

union _parse_unit_t
{
  int		type;
  pu_any_t	pu_any;
  pu_integer_t	pu_integer;
  pu_real_t	pu_real;
  pu_string_t	pu_string;
};

extern char *tf_convert_string_to_string ___PROTO((int unitb, char *str,
						   real	*defval,
						   real limitInf,
						   real limitSup));
extern real tf_convert_string_to_real ___PROTO((int unitb, char *str,
						real defval,
						real limitInf,
						real limitSup));
extern char *tf_convert_real_to_string ___PROTO((int unitb, real value,
						 real *defval,
						 real limitInf,
						 real limitSup));

#endif /* _UnitConvertion_h_ */












