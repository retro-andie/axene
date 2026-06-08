/*
** units.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Global/
** Internal units
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
** Started on  Sun Jun 12 01:41:44 1994 Stéphane Boisson
** Last update Thu Aug 24 16:33:53 1995 Stéphane Boisson
*/

#ifndef __XC_UNIT_H__
#define __XC_UNIT_H__

#include <math.h>

/*--- Simple define type of unit ---*/
#define UNIT		0
#define POINT		1
#define INCH		2
#define MILLIMETER 	3
#define CENTIMETER 	4

#undef ROUNDFLOAT
#define ROUNDFLOAT(value) (value + .5)

/*--- Angles en 1/(2^14)eme de degre (precision: 10^-4) ---*/
#if 0
typedef int angle_t;
#define ANGLE_SCALEVALUE (1<<13)
#define ANGLE_ZERO ((angle_t)0)

#define SCALE_TO_RADIANS(angle) \
  ((real)(angle) / (real)ANGLE_SCALEVALUE * M_PI)

#define SCALE_FROM_RADIANS(angle) \
  ((angle_t) ROUNDFLOAT((real)(angle) * (real)ANGLE_SCALEVALUE / M_PI))

#define SCALE_TO_DEGREES(angle) \
  ((real)(angle) / (real)ANGLE_SCALEVALUE * 180.0)

#define SCALE_FROM_DEGREES(angle) \
  ((angle_t) ROUNDFLOAT((real)(angle) * (real)ANGLE_SCALEVALUE / 180.0))

#else

typedef real angle_t;
#define ANGLE_SCALEVALUE (1)
#define ANGLE_ZERO ((angle_t)0.0)

#define SCALE_TO_RADIANS(angle) ((double) (angle) * M_PI / 180.0)
#define SCALE_FROM_RADIANS(angle) ((angle_t) (angle) * 180.0 / M_PI)
#define SCALE_TO_DEGREES(angle) ((double)(angle))
#define SCALE_FROM_DEGREES(angle) ((angle_t)(angle))
#define SCALE_TO_XANGLE(angle) ( (int)( (double)(angle) * 64 ) )

typedef long vangle_t;  /* for varargs argument (real not supported on m88k) */
#define VANGLE_SCALEBITS 10
#define VANGLE_SCALEVALUE (1<<VANGLE_SCALEBITS)
#define SCALE_FROM_VDEGREES(angle) ((angle_t)((double)angle/VANGLE_SCALEVALUE))
#define SCALE_TO_VDEGREES(angle) ((vangle_t)((angle)*VANGLE_SCALEVALUE))

#endif

/*--- Coordonnees en 1024eme en points typographiques (1/72eme de pouce) ---*/
typedef long coord_t;
#define COORD_DEFAULT	793
#define COORD_SCALEBITS 10
#define COORD_SCALEVALUE (1<<COORD_SCALEBITS)
#define COORD_FONTSIZE_MASK  ((coord_t)~((1<<(COORD_SCALEBITS - 1)) - 1))
#define COORD_HALF_ONE ((coord_t)(1<<(COORD_SCALEBITS - 1)))
#define COORD_ONE ((coord_t)(1<<COORD_SCALEBITS))
#define COORD_ZERO ((coord_t)0)

#define SCALE_TO_PIXELS(coord) ((coord) >> COORD_SCALEBITS)
#define RSCALE_TO_PIXELS(coord) \
(((coord) + COORD_HALF_ONE) >> COORD_SCALEBITS)

#define SCALE_TO_POINTS(coord) \
((real)(coord) / (real)COORD_SCALEVALUE)

#define SCALE_FROM_POINTS(coord) \
((coord_t) ROUNDFLOAT((real)(coord) * (real)COORD_SCALEVALUE))

#define SCALE_TO_INCHES(coord) \
((real)(coord) / (real)(COORD_SCALEVALUE * 72))

#define SCALE_FROM_INCHES(coord) \
((coord_t) ROUNDFLOAT((real)(coord) * (real)(COORD_SCALEVALUE * 72)))

/* 1 pouce = 2.54 cm */
#define SCALE_TO_MILLIMETERS(coord) \
((real)(coord) * (25.4 / (real)(COORD_SCALEVALUE * 72)))

#define SCALE_FROM_MILLIMETERS(coord) \
((coord_t) ROUNDFLOAT((real)(coord)/ (25.4 / (real)(COORD_SCALEVALUE * 72))))

#define SCALE_TO_CENTIMETERS(coord) \
((real)(coord) * (2.54 / (real)(COORD_SCALEVALUE * 72)))

#define SCALE_FROM_CENTIMETERS(coord) \
((coord_t) ROUNDFLOAT((real)(coord)/ (2.54 / (real)(COORD_SCALEVALUE * 72))))


/*--- Rapport d'echelle (de 999% a 0.0156% (1/64)) ---*/
#if 0
typedef unsigned int scale_t;
#define SCALE_SCALEVALUE (1<<6)

#define SCALE_TO_COEF(scale) \
((real)(scale) / (real)(SCALE_SCALEVALUE * 100))

#define SCALE_FROM_COEF(scale) \
((scale_t) ROUNDFLOAT((real)(scale) * (real)(SCALE_SCALEVALUE * 100)))

#define SCALE_TO_PERCENT(scale) \
((real)(scale) / (real)SCALE_SCALEVALUE)

#define SCALE_FROM_PERCENT(scale) \
((scale_t) ROUNDFLOAT((real)(scale) * (real)SCALE_SCALEVALUE))

#else

typedef real scale_t;
#define SCALE_SCALEVALUE 1
#define SCALE_TO_COEF(scale) (scale)
#define SCALE_FROM_COEF(scale) ((scale_t) (scale))
#define SCALE_TO_PERCENT(scale) ((real)(scale) * 100.0)
#define SCALE_FROM_PERCENT(scale) ((scale_t) ((real)((scale) / 100.0)))
#define SCALE_ONE ((scale_t)SCALE_SCALEVALUE)
#define SCALE_ZERO ((scale_t)0)
#define SCALE_INVERT(scale) (SCALE_ONE / (scale))

typedef long vscale_t;  /* for varargs argument (real not supported on m88k) */
#define VSCALE_SCALEBITS 10
#define VSCALE_SCALEVALUE (1<<VSCALE_SCALEBITS)
#define SCALE_FROM_VSCALE(scale) ((scale_t)((double)scale/VANGLE_SCALEVALUE))
#define SCALE_TO_VSCALE(scale) ((vscale_t)((scale)*VANGLE_SCALEVALUE))

#endif

/*--- Caracteres ---*/
typedef unsigned char char_t;
#define Xc_FIRST_CHAR 32
#define Xc_CHAR_TOTAL ((1 << (8 * sizeof(char_t))) - Xc_FIRST_CHAR)


/*--- Nom d'un objet ---*/
#define XcMAX_NAME_LENGTH 32


#endif /* !__XC_UNIT_H__ */
