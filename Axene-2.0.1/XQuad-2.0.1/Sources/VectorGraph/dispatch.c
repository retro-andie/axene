/*
** dispatch.c for XQuad in VectorGraph/
** Dispatch methods for the VectorGraph class
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
** Started on  Tue Mar  7 18:20:42 1995 Stéphane Boisson
** Last update Wed Mar 15 14:48:42 1995 Stéphane Boisson
*/

#include "VectorGraphP.h"


/* ----------------------------------------------------------------- ** 
** vectorSetMode - Change mode                                       ** 
** ----------------------------------------------------------------- */
void vectorSetMode(this, mode, data)
c_VectorGraph *this;
vector_mode_t mode;
d_object_function *data;
{
  c_Cadre *frame;

  Xc_HISTORY(("setMode"));
  
  frame = (c_Cadre *)data->cadre;

  if(this->mode == mode) mode = XcVG_NORMAL;
  if(mode == XcVG_NORMAL)
    {
      this->width_factor = SCALE_ONE;
      this->height_factor = SCALE_ONE;
    }
  this->mode = mode;

  F(this).center(this, data);

  Xc_TRACE(("end setMode"));
}


/* ----------------------------------------------------------------- ** 
** vectorVerticalFlip - Vertical flip                                ** 
** ----------------------------------------------------------------- */
void vectorVerticalFlip(this, data)
c_VectorGraph *this;
d_object_function *data;
{
  c_Cadre *frame;

  Xc_HISTORY(("verticalFlip"));
  
  frame = (c_Cadre *)data->cadre;
  this->vflip_flag ^= TRUE;
  this->update_flag = TRUE;

  DISPLAY_INIT(this)(this, frame);
  Fo(redraw_cadre, data);

  Xc_TRACE(("end verticalFlip"));
}


/* ----------------------------------------------------------------- ** 
** vectorHorizontalFlip - Horizontal flip                            ** 
** ----------------------------------------------------------------- */
void vectorHorizontalFlip(this, data)
c_VectorGraph *this;
d_object_function *data;
{
  c_Cadre *frame;

  Xc_HISTORY(("horizontalFlip"));
  
  frame = (c_Cadre *)data->cadre;
  this->hflip_flag ^= TRUE;
  this->update_flag = TRUE;

  DISPLAY_INIT(this)(this, frame);
  Fo(redraw_cadre, data);

  Xc_TRACE(("endHorizontalFlip"));
}


/* ----------------------------------------------------------------- ** 
** vectorCenter - Center vector in frame                             ** 
** ----------------------------------------------------------------- */
void vectorCenter(this, data)
c_VectorGraph *this;
d_object_function *data;
{
  coord_t w, h, x, y;
  c_Cadre *frame;

  Xc_HISTORY(("center"));
  
  frame = (c_Cadre *)data->cadre;

  w = (coord_t)((this->bbox.urx - this->bbox.llx + 1) *
		SCALE_TO_COEF(this->width_factor));
  h = (coord_t)((this->bbox.ury - this->bbox.lly + 1) *
		SCALE_TO_COEF(this->height_factor));
  x = (coord_t)(-this->bbox.llx * SCALE_TO_COEF(this->width_factor));
  y = (coord_t)(-this->bbox.lly * SCALE_TO_COEF(this->width_factor));
  
  this->offset.dx = ((frame->saWidth - w) >> 1) + x;
  this->offset.dy = ((frame->saHeight - h) >> 1) + y;
  this->update_flag = TRUE;

  DISPLAY_INIT(this)(this, frame);
  Fo(redraw_cadre, data);

  Xc_TRACE(("endHorizontalFlip"));
}
