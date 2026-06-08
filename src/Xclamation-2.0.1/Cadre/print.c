/*
** print.c for Xclamation in Cadre/
** Printing method for the Cadre class
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
** Started on  Mon Jul 24 16:44:17 1995 Stéphane Boisson
** Last update Mon Aug  3 17:52:09 1998 Emmanuel Paris
*/

#include "CadreP.h"

/* ----------------------------------------------------------------- ** 
** generate a ps file                                                ** 
** ----------------------------------------------------------------- */
boolean printCadre(this, post)
c_Cadre		*this;
c_PostScript	*post;
{
 l_Point *tmppoint;
 boolean stroke_flag, fill_flag, draft_flag;

 Xc_HISTORY(("ps_print"));

 draft_flag = post->print_param->Draft && 
  (this->object_type == CADRE_IMAGE_BITMAP);
 stroke_flag = ((this->border_color->transparency == 0) &&
		(this->thickness != 0))? TRUE : FALSE;
 fill_flag = (this->inside_color->transparency == 0)? TRUE : FALSE;
 if(!stroke_flag && !fill_flag && !this->object_type && !draft_flag)
  return TRUE;

 if(!F(post->matrix).translate(post->matrix, TRUE,
			       this->sOrig_X, this->sOrig_Y) ||
    !F(post).putLine(post, "q")) return FALSE;

 tmppoint = this->ListPoint;
 if(!F(post).putLine(post, "$p m", tmppoint->sX, tmppoint->sY)) return FALSE;
 tmppoint = tmppoint->NextPoint;

 /*--- Frame path ---*/
 for( ; tmppoint != NULL; tmppoint = tmppoint->NextPoint)
  if(!F(post).putLine(post, (tmppoint->cont == TRUE)? "$p l":"h $p m",
		      tmppoint->sX, tmppoint->sY)) return FALSE;
  
 if(stroke_flag | fill_flag)
 {
  if(!F(post).putLine(post, "q")) return FALSE;

  /*--- Fill ---*/
  if(fill_flag)
   if(!F(this->inside_color).ps_print(this->inside_color, post, FALSE)
      || (!stroke_flag && !draft_flag && !F(post).putLine(post, "f* Q")))
    return FALSE;
      
  /*--- Stroke ---*/
  if(stroke_flag && !draft_flag)
   if(!F(this->border_color).ps_print(this->border_color, post, TRUE) ||
      !F(post).putLine(post, fill_flag? "$c w b* Q":"$c w s Q",
		       MAX(COORD_ZERO, this->thickness))) return FALSE;
      
  if(draft_flag)
  {
   c_Color	*color;

   color = F(this->BaseStd->color_base).getColor
    (&(this->BaseStd->color_base), XcC_NAME, "Black",
     XcC_GRAY, XcC_SCALE_COLOR(0.0),
     XcC_TRANSPARENCY, 0,
     XcC_END);

   if((color == NULL) || !F(color).ps_print(color, post, TRUE) ||
      !F(post).putLine(post, fill_flag? "$c w b* Q":"$c w s Q",
		       MAX(0, this->thickness))) return FALSE;
  }
 }
  
 if(this->object_type)
 {
  /*--- Draw inside frame ---*/

  Xc_TRACE(("print inside"));

  if(!F(post).putLine(post, "W* n")) return FALSE;
  F(post->matrix).translate(post->matrix, FALSE,
			    this->saOrig_X, this->saOrig_Y);
  /*--- Rotate -angle because of 1 -1 scale ---*/
  F(post->matrix).rotate(post->matrix, FALSE, -this->sAngle);
  if(!PS_PRINT(this->object)(this->object, post)) return FALSE;
 }

 if(!F(post).putLine(post, "Q")) return FALSE;
 F(post->matrix).pop(post->matrix);
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** only print a frame shape path                                     ** 
** ----------------------------------------------------------------- */
boolean printCadrePath(this, post)
c_Cadre		*this;
c_PostScript	*post;
{
 l_Point *tmppoint;

 Xc_HISTORY(("ps_print_path"));

 if(!F(post->matrix).translate(post->matrix, TRUE,
			       this->sOrig_X, this->sOrig_Y))
  return FALSE;
 
 tmppoint = this->ListPoint;
 if(!F(post).putLine(post, "$p m", tmppoint->sX, tmppoint->sY)) return FALSE;
 tmppoint = tmppoint->NextPoint;

 /*--- Frame path ---*/
 for( ; tmppoint != NULL; tmppoint = tmppoint->NextPoint)
  if(!F(post).putLine(post, (tmppoint->cont == TRUE)? "$p l":"h $p m",
		      tmppoint->sX, tmppoint->sY))
   return FALSE;

 F(post->matrix).pop(post->matrix);
 return F(post).putLine(post, "h", NULL);
}
