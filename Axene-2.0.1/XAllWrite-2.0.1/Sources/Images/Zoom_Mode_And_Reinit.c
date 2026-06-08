/*
** Zoom_Mode_And_Reinit.c for Xclamation and XAllWrite in Images/
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
** Started on  Mon Jul 17 18:09:35 1995 Emmanuel Paris
** Last update Thu Jun 25 18:38:27 1998 Emmanuel Paris
*/

#include "Image.h"
#include "Cadre.h"
#include "CadreStd.h"
#include "ObjectStd.h"

#undef _cadre
#define _cadre ((c_Cadre *)(data->cadre))

void Zoom_Normal_Mode();
void Zoom_Auto_Mode_Mode();
void Zoom_Best_Aspect_Mode();
void Zoom_Max_Aspect_Mode();
void Reinit_Zoom_Image_Mode();
void Recenter_Image();
void Flip_Horizontal_Image();
void Flip_Vertical_Image();
void Mapping1_Mode();
void Mapping2_Mode();

void Zoom_Normal_Mode(This, data)
c_Image			*This;
d_object_function	*data;
{
 This->zoom_type = ZOOM_NORMAL_MODE;
 DISPLAY_INIT(This)(This, _cadre);
 Fo(redraw_cadre, data); 
}

void Zoom_Auto_Resize_Mode(This, data)
c_Image			*This;
d_object_function	*data;
{
 This->zoom_type = ZOOM_AUTO_RESIZE_MODE;
 DISPLAY_INIT(This)(This, _cadre);
 Fo(redraw_cadre, data); 
}

void Zoom_Best_Aspect_Mode(This, data)
c_Image			*This;
d_object_function	*data;
{
 This->zoom_type = ZOOM_BEST_ASPECT_MODE;
 DISPLAY_INIT(This)(This, _cadre);
 Fo(redraw_cadre, data); 
}

void Zoom_Max_Aspect_Mode(This, data)
c_Image			*This;
d_object_function	*data;
{
 This->zoom_type = ZOOM_MAX_ASPECT_MODE;
 DISPLAY_INIT(This)(This, _cadre);
 Fo(redraw_cadre, data); 
}

void Reinit_Zoom_Image(This, data)
c_Image			*This;
d_object_function	*data;
{
 This->zoom_type = NO_MODE;
 DISPLAY_INIT(This)(This, _cadre);
 Fo(redraw_cadre, data); 
}

void Recenter_Image(This, data)
c_Image		*This;
d_object_function	*data;
{
 coord_t	cwidth;
 coord_t	cheight;
  
 cwidth = _cadre->saWidth;
 cheight = _cadre->saHeight;
 This->x_orig = (cwidth - This->dwidth) / 2;
 This->y_orig = (cheight- This->dheight) / 2;
 DISPLAY_INIT(This)(This, _cadre);
 Fo(redraw_cadre, data); 
}

void Flip_Horizontal_Image(This, data)
c_Image			*This;
d_object_function	*data;
{
 This->flip_H ^= TRUE;
 This->Dx_zoom = 0;
 DISPLAY_INIT(This)(This, _cadre);
 Fo(redraw_cadre, data); 
}

void Flip_Vertical_Image(This, data)
c_Image			*This;
d_object_function	*data;
{
 This->flip_V ^= TRUE;
 This->Dx_zoom = 0;
 DISPLAY_INIT(This)(This, _cadre);
 Fo(redraw_cadre, data); 
}

void Mapping1_Mode(This, data)
c_Image			*This;
d_object_function	*data;
{
 if (This->zoom_type == MAPPING1_MODE)
  This->zoom_type = ZOOM_NORMAL_MODE;
 else
 {
  This->zoom_type = MAPPING1_MODE;
 }
 DISPLAY_INIT(This)(This, _cadre);
 Fo(redraw_cadre, data); 
}

void Mapping2_Mode(This, data)
c_Image			*This;
d_object_function	*data;
{
 if (This->zoom_type == MAPPING2_MODE)
  This->zoom_type = ZOOM_NORMAL_MODE;
 else
 {
  This->zoom_type = MAPPING2_MODE;
 }
 DISPLAY_INIT(This)(This, _cadre);
 Fo(redraw_cadre, data); 
}



