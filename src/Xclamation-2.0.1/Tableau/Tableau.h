/*
** Tableau.h for Xclamation in Tableau/
** Declaration of the Tableau class
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
** Started on  Fri Sep 30 15:31:18 1994 Emmanuel Paris
** Last update Mon Dec 12 15:26:43 1994 Emmanuel Paris
*/


#ifndef _Tableau_h
#define _Tableau_h

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include "xcalibur.h"
#include "ScrollBarX.h"
#include "Pager.h"
#include "TextStyle.h"
#include "TextRuler.h"
#include "Color.h"
#include "PostScript.h"
#include "DocumentStd.h"

/*--- define for load & save ---*/
#define XcDF_TABLEAU_KEYWORD "TABLEAU"

/*--- forward declarations ---*/
struct sfs_Tableau;

/*--- define the Tableau Class ---*/
typedef struct
{
 struct sfs_Tableau *f;   /* must be in first position      */

 Display	*display;  /* mustn't be move !!!!!!!!!      */
 Window	        window;	   /* this can identify the Tableau */
 boolean	modified;  /* idem */
 
 c_Pager	*Pager;
 c_ScrollBarX	*ScrollBarV;
 c_ScrollBarX	*ScrollBarH;

 /*___configuration___*/
 boolean	visible;
 boolean	scrollbh_on;
 boolean	scrollbv_on;

 /*___Tableau ressources___*/
 c_Color	*color_base;
 c_TextStyle	*text_style_base;
 c_TextRuler	*text_ruler_base;

} c_Tableau;

/*--- define the method for Class Tableau ---*/
typedef struct sfs_Tableau
{
  F_STD;
  F_DOCUMENT_STD;
  void (*Init_Tableau) ___PROTO((c_Tableau *This, Display *display,
				  Window window, boolean visible));
} sf_Tableau;

extern sf_Tableau fc_Tableau;

#define DTAB_SIZEX	360
#define DTAB_SIZEY	200
#define DTAB_POSX	5
#define DTAB_POSY	5
#define DTAB_COLOR	"gray40"

#endif




