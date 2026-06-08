/*
** print.c for XAllWrite in Page/
** printing function for Page object
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Tue Mar 18 17:16:13 1997 Emmanuel Paris
** Last update Tue Jul  7 17:53:47 1998 Emmanuel Paris
*/

#include "Page.h"

static boolean ps_printPage_callback ___PROTO((c_Page *this, 
					       c_PostScript *post));

boolean ps_printPage(this, post)
c_Page          *this;
c_PostScript    *post;
{
 boolean over_clip;

 Xc_HISTORY(("ps_print(%d)", this->page_number));
 
 if(!F(post).beginPage(post, this->page_number)) return FALSE;
 
 /*--- Landscape printing ---*/
 if(this->page_orient == Xc_LANDSCAPE)
 {
  if(!F(post->matrix).rotate(post->matrix, TRUE,
			     SCALE_FROM_DEGREES(90.0)))
     return FALSE;
 }
 else
  if(!F(post->matrix).translate(post->matrix, TRUE, COORD_ZERO,
				this->sHeight))
   return FALSE;
 
 F(post->matrix).scale(post->matrix, FALSE, SCALE_ONE, -SCALE_ONE);

 if (this->text_line_start && this->text_line_end)
  if (!F(this->Calibration).ps_print_text_lines(this->Calibration, post,
						this->text_line_start,
						this->text_line_end))
   return FALSE;

 F(post).putLine(post, "q $m cm Q", F(post->matrix).get(post->matrix, 0));

 /*--- Print frames if needed ---*/
 if (!F(this->frmg).ps_print(this->frmg, post, &over_clip))
  return FALSE;

 /*--- if there are some XcTFM_OVER frame, reprint text in clipped path ---*/
 if (over_clip)
 {
  if (!F(this->frmg).ps_print_over
      (this->frmg, post,  (void (*)())ps_printPage_callback, (void *)this))
   return FALSE;
 }
 
 F(post->matrix).pop(post->matrix);
 
 return F(post).endPage(post);
}

static boolean ps_printPage_callback(this, post)
c_Page          *this;
c_PostScript    *post;
{
 if ((this->text_line_start && this->text_line_end) && 
     !F(this->Calibration).ps_print_text_lines(this->Calibration, post,
					       this->text_line_start,
					       this->text_line_end))
  return FALSE;

 return TRUE;
}
