/*
** Tableau.h for XQuad in Tableau/
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
** Started on  Fri Sep 30 15:31:18 1994 Antoine Buat
** Last update Tue Apr 22 14:22:55 1997 Emmanuel Paris
*/


#ifndef _Tableau_h
#define _Tableau_h

typedef struct sc_Tableau c_Tableau;

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include "xcalibur.h"
#include "ScrollBarX.h"
#include "Moteur.h"
#include "Feuille.h"
#include "DocFile.h"
#include "DocumentStd.h"
#include "BaseStd.h"


#define XcDF_DOCUMENT_KEYWORD "DOCUMENT"

/*--- forward declarations ---*/
struct sfs_Tableau;

/*--- define the Tableau Class ---*/
struct sc_Tableau
{
 struct sfs_Tableau *f;   /* must be in first position      */

 Display	*display;  /* mustn't be move !!!!!!!!!      */
 Window	        window;	   /* this can identify the Tableau */
 boolean	modified;  /* idem */
 
 c_Feuille	*Feuille;
 void		*Pager;
 c_ScrollBarX	*ScrollBarV;
 c_ScrollBarX	*ScrollBarH;

 /*___configuration___*/
 boolean	visible;
 boolean	scrollbh_on;
 boolean	scrollbv_on;
 
 /* ___the moteur___ */
 c_Moteur	*moteur;
 
 /* ___Tableau ressources___*/
 BaseStd_t	BaseStd;

 /*___Document user info___*/
 char		name[XcMAX_NAME_LENGTH];
 char		creator[XcMAX_NAME_LENGTH];
 char		*filename;
 char		version[10];
 char		osname[31];
 char		started_on_date[31];
 char		last_update_date[31];

 int                   PosX;
 int                   PosY;
 int                   Width;
 int                   Height;
};

/*--- define the method for Class Tableau ---*/
typedef struct sfs_Tableau
{
  F_STD;
  F_DOCUMENT_STD;
  void (*Init_Tableau) ___PROTO((c_Tableau *This, Display *display,
				 Window window, boolean visible, 
				 char *name));
} sf_Tableau;

extern sf_Tableau fc_Tableau;

#define DTAB_SIZEX	360
#define DTAB_SIZEY	200
#define DTAB_POSX	5
#define DTAB_POSY	5
#define DTAB_COLOR	"gray40"

#endif




