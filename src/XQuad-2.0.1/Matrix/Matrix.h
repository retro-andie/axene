/*
** Matrix.h for XQuad in Matrix/
** Definition of the Matrix class
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
** Started on  Thu Dec  1 12:25:55 1994 Stéphane Boisson
** Last update Tue Mar 11 18:32:56 1997 Emmanuel Paris
*/

#ifndef __XC_MATRIX_H__
#define __XC_MATRIX_H__

typedef struct sc_Matrix c_Matrix;

#include "xcalibur.h"
#include "vectors.h"


/*--- Macros ---*/
#define XcM_X(matrix, x, y) (ROUNDFLOAT(((matrix)->M[0][0] * (x)) + ((matrix)->M[1][0] * (y))) + (matrix)->v.dx)

#define XcM_Y(matrix, x, y) (ROUNDFLOAT(((matrix)->M[0][1] * (x)) + ((matrix)->M[1][1] * (y))) + (matrix)->v.dy)

#define XcM_DX(matrix, x, y) ROUNDFLOAT(((matrix)->M[0][0] * (x)) + ((matrix)->M[1][0] * (y)))

#define XcM_DY(matrix, x, y) ROUNDFLOAT(((matrix)->M[0][1] * (x)) + ((matrix)->M[1][1] * (y)))

#define XcM_INIT_MATRIX(mat, cxx, cyx, cxy, cyy, tx, ty) \
((mat)->M[0][0] = (real)(cxx), (mat)->M[0][1] = (real)(cyx), \
 (mat)->M[1][0] = (real)(cxy), (mat)->M[1][1] = (real)(cyy), \
 (mat)->v.dx = (tx), (mat)->v.dy = (ty)) 

#define XcM_CMP_MATRIX(mat1, mat2) \
(((mat1)->M[0][0] == (mat2)->M[0][0]) && \
 ((mat1)->M[0][1] == (mat2)->M[0][1]) && \
 ((mat1)->M[1][0] == (mat2)->M[1][0]) && \
 ((mat1)->M[1][1] == (mat2)->M[1][1]) && \
 ((mat1)->v.dx == (mat2)->v.dx) && ((mat1)->v.dy == (mat2)->v.dy))

#define XcM_CMP_MATRIX2(mat1, mat2) \
(((mat1)->M[0][0] == (mat2)->M[0][0]) && \
 ((mat1)->M[0][1] == (mat2)->M[0][1]) && \
 ((mat1)->M[1][0] == (mat2)->M[1][0]) && \
 ((mat1)->M[1][1] == (mat2)->M[1][1]))

#define XcM_SWAP_MATRIX(mat1, mat2) \
{ \
   real swp; \
   swp=(mat1)->M[0][0]; (mat1)->M[0][0]=(mat2)->M[0][0]; (mat2)->M[0][0]=swp; \
   swp=(mat1)->M[0][1]; (mat1)->M[0][1]=(mat2)->M[0][1]; (mat2)->M[0][1]=swp; \
   swp=(mat1)->M[1][0]; (mat1)->M[1][0]=(mat2)->M[1][0]; (mat2)->M[1][0]=swp; \
   swp=(mat1)->M[1][1]; (mat1)->M[1][1]=(mat2)->M[1][1]; (mat2)->M[1][1]=swp; \
 }

#define XcM_COPY_MATRIX(dst, src) \
(((dst)->M[0][0] = (src)->M[0][0]), ((dst)->M[0][1] = (src)->M[0][1]), \
 ((dst)->M[1][0] = (src)->M[1][0]), ((dst)->M[1][1] = (src)->M[1][1]), \
 ((dst)->v.dx = (src)->v.dx), ((dst)->v.dy = (src)->v.dy))

#define XcM_DUMP_MATRIX(desc, mat) Xc_TRACE(("%s = [%g %g %g %g %g %g]", \
					     (desc),\
					     (double)((mat)->M[0][0]),\
					     (double)((mat)->M[0][1]),\
					     (double)((mat)->M[1][0]),\
					     (double)((mat)->M[1][1]),\
					     SCALE_TO_POINTS((mat)->v.dx),\
					     SCALE_TO_POINTS((mat)->v.dy)))


/*--- Strutures ---*/
typedef struct {
  real M[2][2];
  vector_t v;
} matrix_t;

/*	a b 0   M[0][0] M[0][1] 0	*/
/*	c d 0 = M[1][0] M[1][1] 0	*/
/*	e f 1    v.dx    v.dy   1	*/



/*--- Define the Matrix class methods ---*/
typedef struct {
  F_STD;

  matrix_t *(*get) ___PROTO((c_Matrix *this, int number));
  void (*pop) ___PROTO((c_Matrix *this));

  coord_t (*convertX) ___PROTO((c_Matrix *this, coord_t x, coord_t y));
  coord_t (*convertY) ___PROTO((c_Matrix *this, coord_t x, coord_t y));

  boolean (*translate) ___PROTO((c_Matrix *this, boolean push_flag,
				 coord_t dx, coord_t dy));
  boolean (*scale) ___PROTO((c_Matrix *this, boolean push_flag,
			     scale_t sx, scale_t sy));
  boolean (*rotate) ___PROTO((c_Matrix *this, boolean push_flag, angle_t a));
  boolean (*transform) ___PROTO((c_Matrix *this, boolean push_flag,
				 scale_t cxx, scale_t cyx,
				 scale_t cxy, scale_t cyy));
  boolean (*invert) ___PROTO((c_Matrix *this, boolean push_flag));
  boolean (*identity) ___PROTO((c_Matrix *this, boolean push_flag));

  coord_t (*convertDx) ___PROTO((c_Matrix *this, coord_t x, coord_t y));
  coord_t (*convertDy) ___PROTO((c_Matrix *this, coord_t x, coord_t y));

  boolean (*push) ___PROTO((c_Matrix *this));
  boolean (*concat) ___PROTO((c_Matrix *this, boolean push_flag,
			     matrix_t *matrix));
  void (*convertBBox) ___PROTO((c_Matrix *this, bbox_t *src, bbox_t *dst));
  boolean (*hardTranslate) ___PROTO((c_Matrix *this, boolean push_flag,
				     coord_t dx, coord_t dy));
  boolean (*invertConcat) ___PROTO((c_Matrix *this, boolean push_flag,
				    matrix_t *matrix));
  boolean (*vmirror) ___PROTO((c_Matrix *this, boolean push_flag));
  boolean (*hmirror) ___PROTO((c_Matrix *this, boolean push_flag));
  boolean (*init) ___PROTO((c_Matrix *this, boolean push_flag,
			    matrix_t *matrix));
} sf_Matrix;


/*--- Define the Matrix class ---*/
struct sc_Matrix {
  sf_Matrix *f;

  matrix_t *base;
  int pos;
  int max_pos;
};

extern sf_Matrix fc_Matrix;

#endif /* !__XC_MATRIX_H__ */
