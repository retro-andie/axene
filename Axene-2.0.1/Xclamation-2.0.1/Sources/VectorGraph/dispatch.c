/*
** dispatch.c for Xclamation and XAllWrite in VectorGraph/
** Dispatch methods for the VectorGraph class
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
** Started on  Tue Mar  7 18:20:42 1995 Stéphane Boisson
** Last update Thu Jun 25 18:37:19 1998 Emmanuel Paris
*/

/*#define NTRACE*/
/*#define NHISTORY*/

#include "VectorGraphP.h"

/*--- Local variables ---*/
#ifndef NHISTORY
static char *debug_mode[] = 
{
 "XcVG_NORMAL", "XcVG_BEST_ASPECT", "XcVG_MAX_ASPECT", "XcVG_AUTOSIZE"
};
#endif

/* ----------------------------------------------------------------- ** 
** vectorSetMode - Change mode                                       ** 
** ----------------------------------------------------------------- */
void vectorSetMode(this, mode, data)
c_VectorGraph *this;
vector_mode_t mode;
d_object_function *data;
{
 c_Cadre *frame;

 Xc_HISTORY(("setMode(%s -> %s)", debug_mode[(int)this->mode],
	     debug_mode[(int)mode]));
  
 frame = (c_Cadre *)data->cadre;

 if(mode == XcVG_NORMAL)
 {
  Xc_TRACE(("Scale to 1:1"));
  this->width_factor = SCALE_ONE;
  this->height_factor = SCALE_ONE;
 }
 else if (this->mode == mode || mode == XcVG_NONE)
  mode = XcVG_NORMAL;
    
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
 c_Cadre *frame;
 coord_t w, h;

 Xc_HISTORY(("center"));
  
 frame = (c_Cadre *)data->cadre;

 w = (coord_t)((this->bbox.urx - this->bbox.llx + 1) *
	       SCALE_TO_COEF(this->width_factor));
 h = (coord_t)((this->bbox.ury - this->bbox.lly + 1) *
	       SCALE_TO_COEF(this->height_factor));

 this->offset.dx = (frame->saWidth - w) >> 1;
 this->offset.dy = (frame->saHeight - h) >> 1;
 this->update_flag = TRUE;

 Xc_TRACE(("offset(%g; %g)", SCALE_TO_POINTS(this->offset.dx),
	   SCALE_TO_POINTS(this->offset.dy)));

 DISPLAY_INIT(this)(this, frame);
 Fo(redraw_cadre, data);

 Xc_TRACE(("end center"));
}


/* ----------------------------------------------------------------- ** 
** vectorTextToggle - Toggle text drawing                            ** 
** ----------------------------------------------------------------- */
void vectorTextToggle(this, data)
c_VectorGraph *this;
d_object_function *data;
{
 Xc_HISTORY(("textToggle"));

 this->drawtext_flag ^= TRUE;
 Fo(redraw_cadre, data);
 Xc_TRACE(("end textToggle"));
}
